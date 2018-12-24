#include "JsonResponse.h"

#include <proxygen/httpserver/Mocks.h>

using ::testing::_;
using ::testing::Property;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::Sequence;
using proxygen::MockRequestHandler;
using proxygen::MockResponseHandler;
using proxygen::HTTPMessage;
using proxygen::HTTPHeaderCode;

namespace {

MATCHER(IsJsonContentType, "")
{
    const auto& headers = arg.getHeaders();
    return headers.getSingleOrEmpty(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE) == "application/json";
}

} // namespace

TEST(JsonResponse, BuildJsonResponse)
{
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

TEST(JsonResponse, BuildNotFoundResponse)
{
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
