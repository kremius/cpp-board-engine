#include "CreateThreadHandler.h"

#include "HandlersTestUtils.h"

using CreateThreadHandlerTest = BaseHandlerTest<board::CreateThreadHandler>;

TEST_F(CreateThreadHandlerTest, Basics) {
    mockResponse(200);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/");
    handler_.onRequest(std::move(headers));
    handler_.onBody(folly::IOBuf::copyBuffer(R"({"text":"test text 42"})"));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, R"({"id":2})");
}

TEST_F(CreateThreadHandlerTest, NoRequestBody) {
    mockResponse(500);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/");
    handler_.onRequest(std::move(headers));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, R"({})");
}

TEST_F(CreateThreadHandlerTest, BadJson) {
    mockResponse(400);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/");
    handler_.onRequest(std::move(headers));
    handler_.onBody(folly::IOBuf::copyBuffer(R"({"text":"that's invalid json actually)"));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, R"({})");
}

TEST_F(CreateThreadHandlerTest, WrongJsonKey) {
    mockResponse(400);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/");
    handler_.onRequest(std::move(headers));
    handler_.onBody(folly::IOBuf::copyBuffer(R"({"no-text":"valid, but wrong key"})"));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, R"({})");
}
