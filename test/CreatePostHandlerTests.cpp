#include "CreatePostHandler.h"

#include "HandlersTestUtils.h"

using CreatePostHandlerTest = BaseHandlerTest<board::CreatePostHandler>;

TEST_F(CreatePostHandlerTest, Basics) {
    mockResponse(200);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/1");
    handler_.onRequest(std::move(headers));
    handler_.onBody(folly::IOBuf::copyBuffer(R"({"text":"word"})"));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, R"({"id":2})");
}

TEST_F(CreatePostHandlerTest, InvalidThreadNumber) {
    mockResponse(404);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/notfound");
    handler_.onRequest(std::move(headers));
    handler_.onBody(folly::IOBuf::copyBuffer(R"({"text":"word"})"));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, R"({})");
}

TEST_F(CreatePostHandlerTest, NoBody) {
    mockResponse(500);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/1");
    handler_.onRequest(std::move(headers));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, R"({})");
}

TEST_F(CreatePostHandlerTest, WrongJsonKey) {
    mockResponse(400);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/1");
    handler_.onRequest(std::move(headers));
    handler_.onBody(folly::IOBuf::copyBuffer(R"({"no-text":"word"})"));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, R"({})");
}

TEST_F(CreatePostHandlerTest, ThreadNotFound) {
    mockResponse(404);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/42");
    handler_.onRequest(std::move(headers));
    handler_.onBody(folly::IOBuf::copyBuffer(R"({"text":"word"})"));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, R"({})");
}
