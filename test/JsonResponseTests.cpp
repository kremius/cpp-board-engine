#include "JsonResponse.h"

#include <proxygen/httpserver/Mocks.h>

using namespace proxygen;

TEST(JsonResponse, BuildJsonResponse)
{
    MockRequestHandler request;
    MockResponseHandler response(&request);

    EXPECT_CALL(response, sendEOM())
        .Times(1);

    request.setResponseHandler(&response);

    board::buildJsonResponse(
        &response, 42, "test message", folly::dynamic::object("test", "json"));
}

