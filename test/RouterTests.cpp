#include "Router.h"

#include "DefaultPageNotFoundHandler.h"

#include <proxygen/httpserver/Mocks.h>

class MockRequestHandlerFactory : public proxygen::RequestHandlerFactory {
public:
    GMOCK_METHOD1_(, noexcept, , onServerStart, void(folly::EventBase*));
    GMOCK_METHOD0_(, noexcept, , onServerStop, void());
    GMOCK_METHOD2_(, noexcept, , onRequest,
                   proxygen::RequestHandler*(proxygen::RequestHandler*, proxygen::HTTPMessage*));
};

namespace {

template<int i>
class DummyHandler : public proxygen::RequestHandler {
public:
    explicit DummyHandler() {
        // Nothing
    }

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> /*headers*/)
        noexcept override {
        // Nothing
    }

    void onBody(std::unique_ptr<folly::IOBuf> /*body*/) noexcept override {
        // Nothing
    }

    void onEOM() noexcept override {
        // Nothing
    }

    void onUpgrade(proxygen::UpgradeProtocol /*proto*/) noexcept override {
        // Nothing
    }

    void requestComplete() noexcept override {
        delete this;
    }

    void onError(proxygen::ProxygenError /*err*/) noexcept override {
        delete this;
    }
};

}

TEST(RoutesChain, BasicUsage) {
    using namespace proxygen;

    auto factory1 = std::make_unique<MockRequestHandlerFactory>();
    auto factory2 = std::make_unique<MockRequestHandlerFactory>();

    auto factory1_ptr = factory1.get();
    auto factory2_ptr = factory2.get();

    const auto methods1 = std::vector<HTTPMethod>{HTTPMethod::GET};
    const auto methods2 = std::vector<HTTPMethod>{HTTPMethod::GET, HTTPMethod::POST};

    std::vector<Route> routes = RoutesChain()
        .addThen("/url1", {HTTPMethod::GET}, std::move(factory1))
        .addThen("/different_url", {HTTPMethod::GET, HTTPMethod::POST}, std::move(factory2))
        .build();

    ASSERT_EQ(routes.size(), 2);

    EXPECT_EQ(routes[0].path, "/url1");
    EXPECT_EQ(routes[0].methods, methods1);
    EXPECT_EQ(routes[0].factory.get(), factory1_ptr);
    EXPECT_EQ(routes[1].path, "/different_url");
    EXPECT_EQ(routes[1].methods, methods2);
    EXPECT_EQ(routes[1].factory.get(), factory2_ptr);
}

using ::testing::_;
using ::testing::Return;

TEST(RouterFactory, StopAndStart) {
    using namespace proxygen;

    auto factory1 = std::make_unique<MockRequestHandlerFactory>();
    auto factory2 = std::make_unique<MockRequestHandlerFactory>();

    folly::EventBase base;

    using ::testing::Sequence;
    Sequence sequence1;
    Sequence sequence2;

    EXPECT_CALL(*factory1, onServerStart(&base))
        .Times(1)
        .InSequence(sequence1);
    EXPECT_CALL(*factory1, onServerStop())
        .Times(1)
        .InSequence(sequence1);
    EXPECT_CALL(*factory2, onServerStart(&base))
        .Times(1)
        .InSequence(sequence2);
    EXPECT_CALL(*factory2, onServerStop())
        .Times(1)
        .InSequence(sequence2);

    RouterFactory router;
    router.addRoutes(RoutesChain()
        .addThen("1", {HTTPMethod::GET}, std::move(factory1))
        .addThen("2", {HTTPMethod::POST}, std::move(factory2))
        .build());

    router.onServerStart(&base);
    router.onServerStop();
}

TEST(RouterFactory, Route) {
    using namespace proxygen;

    auto factory1 = std::make_unique<MockRequestHandlerFactory>();
    auto factory2 = std::make_unique<MockRequestHandlerFactory>();
    auto factory3 = std::make_unique<MockRequestHandlerFactory>();

    MockRequestHandler handler1;
    MockRequestHandler handler2;
    MockRequestHandler handler3;

    EXPECT_CALL(*factory1, onRequest(_, _))
        .WillRepeatedly(Return(&handler1));
    EXPECT_CALL(*factory2, onRequest(_, _))
        .WillRepeatedly(Return(&handler2));
    EXPECT_CALL(*factory3, onRequest(_, _))
        .WillRepeatedly(Return(&handler3));

    RouterFactory router;
    router.addRoutes(RoutesChain()
        .addThen("/api/v3/books", {HTTPMethod::POST}, std::move(factory1))
        .addThen("/static/images/", {HTTPMethod::GET}, std::move(factory2))
        .build());

    router.addRoutes(RoutesChain()
        .addThen("/test/test/test/test", {HTTPMethod::POST, HTTPMethod::GET}, std::move(factory3))
        .build());

    {
        HTTPMessage message;
        message.setMethod(HTTPMethod::POST);
        message.setURL("/api/v3/books/something/else");

        auto handler = router.onRequest(nullptr, &message);
        EXPECT_EQ(handler, &handler1);
    }

    {
        HTTPMessage message;
        message.setMethod(HTTPMethod::GET);
        message.setURL("/static/images/kitty.png");

        auto handler = router.onRequest(nullptr, &message);
        EXPECT_EQ(handler, &handler2);
    }

    {
        HTTPMessage message;
        message.setMethod(HTTPMethod::POST);
        message.setURL("/test/test/test/test/ffffffffffffff");

        auto handler = router.onRequest(nullptr, &message);
        EXPECT_EQ(handler, &handler3);
    }

    {
        HTTPMessage message;
        message.setMethod(HTTPMethod::GET);
        message.setURL("/test/test/test/test/ffffffffffffff");

        auto handler = router.onRequest(nullptr, &message);
        EXPECT_EQ(handler, &handler3);
    }

    {
        HTTPMessage message;
        message.setMethod(HTTPMethod::GET);
        message.setURL("/thispagedoesnotexistandneverexisted");

        auto handler = router.onRequest(nullptr, &message);
        EXPECT_TRUE(dynamic_cast<DefaultPageNotFoundHandler*>(handler) != nullptr);
        // Free memory
        handler->onEOM();
    }

    {
        HTTPMessage message;
        message.setMethod(HTTPMethod::POST);
        message.setURL("/static/images/kitty.png");

        auto handler = router.onRequest(nullptr, &message);
        EXPECT_TRUE(dynamic_cast<DefaultPageNotFoundHandler*>(handler) != nullptr);
        // Free memory
        handler->onEOM();
    }

    // TODO: custon method test
}

TEST(RouterFactory, Prefixes) {
    using namespace proxygen;

    MockRequestHandler handler1;
    MockRequestHandler handler2;

    auto test_url = [](auto& router, auto url, auto expected_handler) {
        HTTPMessage message;
        message.setMethod(HTTPMethod::GET);
        message.setURL(url);

        auto handler = router.onRequest(nullptr, &message);
        EXPECT_EQ(handler, expected_handler);
    };

    {
        auto factory1 = std::make_unique<MockRequestHandlerFactory>();
        auto factory2 = std::make_unique<MockRequestHandlerFactory>();

        EXPECT_CALL(*factory1, onRequest(_, _))
            .WillRepeatedly(Return(&handler1));
        EXPECT_CALL(*factory2, onRequest(_, _))
            .WillRepeatedly(Return(&handler2));

        RouterFactory router;
        router.addRoutes(RoutesChain()
            .addThen("/prefix/", {HTTPMethod::GET}, std::move(factory2))
            .addThen("/prefix", {HTTPMethod::GET}, std::move(factory1))
            .build());

        test_url(router, "/prefix", &handler1);
        test_url(router, "/prefix/", &handler2);
        test_url(router, "/prefix/stuffstuffstuff", &handler2);
    }
    // In reverse order
    {
        auto factory1 = std::make_unique<MockRequestHandlerFactory>();
        auto factory2 = std::make_unique<MockRequestHandlerFactory>();

        EXPECT_CALL(*factory1, onRequest(_, _))
            .WillRepeatedly(Return(&handler1));
        EXPECT_CALL(*factory2, onRequest(_, _))
            .WillRepeatedly(Return(&handler2));

        RouterFactory router;
        router.addRoutes(RoutesChain()
            .addThen("/prefix", {HTTPMethod::GET}, std::move(factory1))
            .addThen("/prefix/", {HTTPMethod::GET}, std::move(factory2))
            .build());

        test_url(router, "/prefix", &handler1);
        test_url(router, "/prefix/", &handler1);
        test_url(router, "/prefix/stuffstuffstuff", &handler1);
    }
}
