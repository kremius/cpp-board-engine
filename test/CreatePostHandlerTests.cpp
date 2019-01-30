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
