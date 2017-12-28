#include "Router.h"

//#include <gmock/gmock.h>

#include <proxygen/httpserver/Mocks.h>

class MockRequestHandlerFactory : public proxygen::RequestHandlerFactory
{
public:
    GMOCK_METHOD1_(, noexcept, , onServerStart, void(folly::EventBase*));
    GMOCK_METHOD0_(, noexcept, , onServerStop, void());
    GMOCK_METHOD2_(, noexcept, , onRequest,
                   proxygen::RequestHandler*(proxygen::RequestHandler*, proxygen::HTTPMessage*));
};

namespace
{

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

TEST(RoutesChain, BasicUsage)
{
    using namespace proxygen;

    auto factory1 = std::make_unique<MockRequestHandlerFactory>();
    auto factory2 = std::make_unique<MockRequestHandlerFactory>();

    auto factory1_ptr = factory1.get();
    auto factory2_ptr = factory2.get();

    std::vector<Route> routes = RoutesChain()
        .addThen("/url1", std::move(factory1))
        .addThen("/different_url", std::move(factory2))
        .build();

    ASSERT_EQ(routes.size(), 2);

    EXPECT_EQ(routes[0].first, "/url1");
    EXPECT_EQ(routes[0].second.get(), factory1_ptr);
    EXPECT_EQ(routes[1].first, "/different_url");
    EXPECT_EQ(routes[1].second.get(), factory2_ptr);
}

using ::testing::_;
using ::testing::Return;

TEST(RouterFactory, Route)
{
    using namespace proxygen;

    auto factory1 = std::make_unique<MockRequestHandlerFactory>();
    auto factory2 = std::make_unique<MockRequestHandlerFactory>();

    MockRequestHandler handler1;
    MockRequestHandler handler2;

    EXPECT_CALL(*factory1, onRequest(_, _))
        .WillRepeatedly(Return(&handler1));
    EXPECT_CALL(*factory2, onRequest(_, _))
        .WillRepeatedly(Return(&handler2));

    RouterFactory router;
    router.addRoutes(RoutesChain()
        .addThen("/api/v3/books", std::move(factory1))
        .addThen("/static/images/", std::move(factory2))
        .build());

    {
        HTTPMessage message;
        message.setURL("/api/v3/books/something/else");

        auto handler = router.onRequest(nullptr, &message);
        EXPECT_EQ(handler, &handler1);
    }

    {
        HTTPMessage message;
        message.setURL("/static/images/kitty.png");

        auto handler = router.onRequest(nullptr, &message);
        EXPECT_EQ(handler, &handler2);
    }
}
