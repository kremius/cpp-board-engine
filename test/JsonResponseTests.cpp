#include "JsonResponse.h"

#include <proxygen/httpserver/Mocks.h>

#include "HandlersTestUtils.h"

using ::testing::_;
using ::testing::Property;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::Sequence;
using proxygen::MockRequestHandler;
using proxygen::MockResponseHandler;
using proxygen::HTTPMessage;
using proxygen::HTTPHeaderCode;

TEST(JsonResponse, BuildJsonResponse) {
    MockRequestHandler request;
    MockResponseHandler response(&request);

    Sequence sequence;

    const auto headers_matcher = AllOf(
        Property(&HTTPMessage::getStatusCode, 42),
        Property(&HTTPMessage::getStatusMessage, "test message"),
        IsJsonContentType());
    EXPECT_CALL(response,
                sendHeaders(headers_matcher))
        .Times(1)
        .InSequence(sequence);

    folly::fbstring body;
    EXPECT_CALL(response, sendBody(_))
        .InSequence(sequence)
        .WillOnce(DoAll(
            Invoke([&] (std::shared_ptr<folly::IOBuf> buffer) {
                body = buffer->moveToFbString();
            }),
            Return()));

    EXPECT_CALL(response, sendEOM())
        .Times(1)
        .InSequence(sequence);

    request.setResponseHandler(&response);

    board::buildJsonResponse(
        &response, 42, "test message", folly::dynamic::object("test", "json"));
    EXPECT_EQ(body, R"({"test":"json"})");
}

TEST(JsonResponse, BuildNotFoundResponse) {
    MockRequestHandler request;
    MockResponseHandler response(&request);

    Sequence sequence;

    const auto headers_matcher = AllOf(
        Property(&HTTPMessage::getStatusCode, 404),
        Property(&HTTPMessage::getStatusMessage, "Not Found"),
        IsJsonContentType());
    EXPECT_CALL(response,
                sendHeaders(headers_matcher))
        .Times(1)
        .InSequence(sequence);

    folly::fbstring body;
    EXPECT_CALL(response, sendBody(_))
        .InSequence(sequence)
        .WillOnce(DoAll(
            Invoke([&] (std::shared_ptr<folly::IOBuf> buffer) {
                body = buffer->moveToFbString();
            }),
            Return()));

    EXPECT_CALL(response, sendEOM())
        .Times(1)
        .InSequence(sequence);

    request.setResponseHandler(&response);

    board::buildNotFoundResponse(&response);
    EXPECT_EQ(body, "{}");
}

TEST(JsonResponse, buildInternalErrorResponse) {
    MockRequestHandler request;
    MockResponseHandler response(&request);

    Sequence sequence;

    const auto headers_matcher = AllOf(
        Property(&HTTPMessage::getStatusCode, 500),
        Property(&HTTPMessage::getStatusMessage, "Internal Server Error"),
        IsJsonContentType());
    EXPECT_CALL(response,
                sendHeaders(headers_matcher))
        .Times(1)
        .InSequence(sequence);

    folly::fbstring body;
    EXPECT_CALL(response, sendBody(_))
        .InSequence(sequence)
        .WillOnce(DoAll(
            Invoke([&] (std::shared_ptr<folly::IOBuf> buffer) {
                body = buffer->moveToFbString();
            }),
            Return()));

    EXPECT_CALL(response, sendEOM())
        .Times(1)
        .InSequence(sequence);

    request.setResponseHandler(&response);

    board::buildInternalErrorResponse(&response);
    EXPECT_EQ(body, "{}");
}

TEST(JsonResponse, buildOkResponse) {
    MockRequestHandler request;
    MockResponseHandler response(&request);

    Sequence sequence;

    const auto headers_matcher = AllOf(
        Property(&HTTPMessage::getStatusCode, 200),
        Property(&HTTPMessage::getStatusMessage, "OK"),
        IsJsonContentType());
    EXPECT_CALL(response,
                sendHeaders(headers_matcher))
        .Times(1)
        .InSequence(sequence);

    folly::fbstring body;
    EXPECT_CALL(response, sendBody(_))
        .InSequence(sequence)
        .WillOnce(DoAll(
            Invoke([&] (std::shared_ptr<folly::IOBuf> buffer) {
                body = buffer->moveToFbString();
            }),
            Return()));

    EXPECT_CALL(response, sendEOM())
        .Times(1)
        .InSequence(sequence);

    request.setResponseHandler(&response);

    board::buildOkResponse(&response, folly::dynamic::object("test", 24));
    EXPECT_EQ(body, R"({"test":24})");
}
