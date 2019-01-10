#include "GetThreadHandler.h"

#include <proxygen/httpserver/Mocks.h>

using ::testing::_;
using ::testing::Property;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::Sequence;
using proxygen::MockResponseHandler;
using proxygen::HTTPMessage;
using proxygen::HTTPHeaderCode;

namespace {

MATCHER(IsJsonContentType, "") {
    const auto& headers = arg.getHeaders();
    return headers.getSingleOrEmpty(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE) == "application/json";
}

} // namespace

class GetThreadHandlerTest : public ::testing::Test {
protected:
    GetThreadHandlerTest() {
        holder_ = std::make_shared<board::DataHolder>();
        holder_->createThread({0, 0, "text1", "image1"}).getTry();
    };
    std::shared_ptr<board::DataHolder> holder_;
};

TEST_F(GetThreadHandlerTest, Basics) {
    board::GetThreadHandler handler(holder_, "/test/");
    proxygen::MockResponseHandler response_mock(&handler);
    handler.setResponseHandler(&response_mock);

    Sequence sequence;

    const auto headers_matcher = AllOf(
        Property(&HTTPMessage::getStatusCode, 200),
        IsJsonContentType());
    EXPECT_CALL(response_mock,
                sendHeaders(headers_matcher))
        .Times(1)
        .InSequence(sequence);
    folly::fbstring body;
    EXPECT_CALL(response_mock, sendBody(_))
        .InSequence(sequence)
        .WillOnce(DoAll(
            Invoke([&] (std::shared_ptr<folly::IOBuf> buffer) {
                body = buffer->moveToFbString();
            }),
            Return()));
    EXPECT_CALL(response_mock, sendEOM())
        .Times(1)
        .InSequence(sequence);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/1");
    handler.onRequest(std::move(headers));
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body, R"({"posts":[{"text":"text1","id":1}]})");
}
