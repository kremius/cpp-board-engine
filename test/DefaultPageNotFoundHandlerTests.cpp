#include "DefaultPageNotFoundHandler.h"

#include <proxygen/httpserver/Mocks.h>

TEST(DefaultPageNotFoundHandler, Basics) {
    DefaultPageNotFoundHandler handler;
    proxygen::MockResponseHandler response_mock(&handler);
    handler.setResponseHandler(&response_mock);

    using ::testing::Sequence;
    Sequence sequence;

    using ::testing::_;
    using ::testing::Property;
    using proxygen::HTTPMessage;
    EXPECT_CALL(response_mock,
                sendHeaders(Property(&HTTPMessage::getStatusCode, 404)))
        .Times(1)
        .InSequence(sequence);
    EXPECT_CALL(response_mock, sendEOM())
        .Times(1)
        .InSequence(sequence);

    handler.onRequest(std::make_unique<proxygen::HTTPMessage>());
}
