#include "JsonResponse.h"

#include <proxygen/httpserver/Mocks.h>

using namespace proxygen;

namespace {

MATCHER(IsJsonContentType, "")
{
    const auto& headers = arg.getHeaders();
    return headers.getSingleOrEmpty(proxygen::HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE) == "application/json";
}

} // namespace

TEST(JsonResponse, BuildJsonResponse)
{
    MockRequestHandler request;
    MockResponseHandler response(&request);

    using ::testing::Sequence;
    Sequence sequence;

    proxygen::HTTPMessage message;


    using ::testing::_;
    using ::testing::Property;
    using proxygen::HTTPMessage;
    const auto headers_matcher = AllOf(
        Property(&HTTPMessage::getStatusCode, 42),
        Property(&HTTPMessage::getStatusMessage, "test message"),
        IsJsonContentType());
    EXPECT_CALL(response,
                sendHeaders(headers_matcher))
        .Times(1)
        .InSequence(sequence);

    EXPECT_CALL(response, sendEOM())
        .Times(1)
        .InSequence(sequence);

    request.setResponseHandler(&response);

    board::buildJsonResponse(
        &response, 42, "test message", folly::dynamic::object("test", "json"));
}

