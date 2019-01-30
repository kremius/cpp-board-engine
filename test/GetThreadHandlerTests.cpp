#include "GetThreadHandler.h"

#include "HandlersTestUtils.h"

using GetThreadHandlerTest = BaseHandlerTest<board::GetThreadHandler>;

TEST_F(GetThreadHandlerTest, Basics) {
    mockResponse(200);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/1");
    handler_.onRequest(std::move(headers));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, R"({"posts":[{"text":"text1","id":1}]})");
}

TEST_F(GetThreadHandlerTest, InvalidThreadId) {
    mockResponse(404);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/notanumber");
    handler_.onRequest(std::move(headers));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, "{}");
}

TEST_F(GetThreadHandlerTest, ThreadDoesNotExist) {
    mockResponse(404);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/2");
    handler_.onRequest(std::move(headers));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body_, "{}");
}
