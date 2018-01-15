#include "DefaultPageNotFoundHandler.h"

#include <proxygen/httpserver/Mocks.h>

TEST(DefaultPageNotFoundHandler, Basics)
{
    // TODO: is it right?
    DefaultPageNotFoundHandler handler;
    proxygen::MockResponseHandler response_mock(&handler);
    handler.setResponseHandler(&response_mock);

    using ::testing::Sequence;
    Sequence sequence;

    using ::testing::_;
    // TODO: check actual value passed
    EXPECT_CALL(response_mock, sendHeaders(_))
        .Times(1)
        .InSequence(sequence);

    handler.onRequest(std::make_unique<proxygen::HTTPMessage>());
}

