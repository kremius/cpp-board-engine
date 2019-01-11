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
    GetThreadHandlerTest()
        : holder_(std::make_shared<board::DataHolder>()),
          handler_(holder_, "/test/"),
          response_mock_(&handler_) {
        handler_.setResponseHandler(&response_mock_);
        holder_->createThread({0, 0, "text1", "image1"}).getTry();
    };
    std::shared_ptr<board::DataHolder> holder_;
    board::GetThreadHandler handler_;
    proxygen::MockResponseHandler response_mock_;
};

TEST_F(GetThreadHandlerTest, Basics) {
    Sequence sequence;

    const auto headers_matcher = AllOf(
        Property(&HTTPMessage::getStatusCode, 200),
        IsJsonContentType());
    EXPECT_CALL(response_mock_,
                sendHeaders(headers_matcher))
        .Times(1)
        .InSequence(sequence);
    folly::fbstring body;
    EXPECT_CALL(response_mock_, sendBody(_))
        .InSequence(sequence)
        .WillOnce(DoAll(
            Invoke([&] (std::shared_ptr<folly::IOBuf> buffer) {
                body = buffer->moveToFbString();
            }),
            Return()));
    EXPECT_CALL(response_mock_, sendEOM())
        .Times(1)
        .InSequence(sequence);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/1");
    handler_.onRequest(std::move(headers));
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body, R"({"posts":[{"text":"text1","id":1}]})");
}

TEST_F(GetThreadHandlerTest, InvalidThreadId) {
    Sequence sequence;

    const auto headers_matcher = AllOf(
        Property(&HTTPMessage::getStatusCode, 404),
        IsJsonContentType());
    EXPECT_CALL(response_mock_,
                sendHeaders(headers_matcher))
        .Times(1)
        .InSequence(sequence);
    folly::fbstring body;
    EXPECT_CALL(response_mock_, sendBody(_))
        .InSequence(sequence)
        .WillOnce(DoAll(
            Invoke([&] (std::shared_ptr<folly::IOBuf> buffer) {
                body = buffer->moveToFbString();
            }),
            Return()));
    EXPECT_CALL(response_mock_, sendEOM())
        .Times(1)
        .InSequence(sequence);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/notanumber");
    handler_.onRequest(std::move(headers));
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body, "{}");
}

TEST_F(GetThreadHandlerTest, ThreadDoesNotExist) {
    Sequence sequence;

    const auto headers_matcher = AllOf(
        Property(&HTTPMessage::getStatusCode, 404),
        IsJsonContentType());
    EXPECT_CALL(response_mock_,
                sendHeaders(headers_matcher))
        .Times(1)
        .InSequence(sequence);
    folly::fbstring body;
    EXPECT_CALL(response_mock_, sendBody(_))
        .InSequence(sequence)
        .WillOnce(DoAll(
            Invoke([&] (std::shared_ptr<folly::IOBuf> buffer) {
                body = buffer->moveToFbString();
            }),
            Return()));
    EXPECT_CALL(response_mock_, sendEOM())
        .Times(1)
        .InSequence(sequence);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/2");
    handler_.onRequest(std::move(headers));
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body, "{}");
}
