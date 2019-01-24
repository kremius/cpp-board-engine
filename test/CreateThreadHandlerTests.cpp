#include "CreateThreadHandler.h"

#include <proxygen/httpserver/Mocks.h>

#include "HandlersTestUtils.h"

using ::testing::_;
using ::testing::Property;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::Sequence;
using proxygen::MockResponseHandler;
using proxygen::HTTPMessage;
using proxygen::HTTPHeaderCode;

using CreateThreadHandlerTest = BaseHandlerTest<board::CreateThreadHandler>;

TEST_F(CreateThreadHandlerTest, Basics) {
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
            Invoke([&](std::shared_ptr<folly::IOBuf> buffer) {
                body = buffer->moveToFbString();
            }),
            Return()));
    EXPECT_CALL(response_mock_, sendEOM())
        .Times(1)
        .InSequence(sequence);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/");
    handler_.onRequest(std::move(headers));
    handler_.onBody(folly::IOBuf::copyBuffer(R"({"text":"test text 42"})"));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body, R"({"id":2})");
}

TEST_F(CreateThreadHandlerTest, NoRequestBody) {
    Sequence sequence;

    const auto headers_matcher = AllOf(
        Property(&HTTPMessage::getStatusCode, 500),
        IsJsonContentType());
    EXPECT_CALL(response_mock_,
                sendHeaders(headers_matcher))
        .Times(1)
        .InSequence(sequence);
    folly::fbstring body;
    EXPECT_CALL(response_mock_, sendBody(_))
        .InSequence(sequence)
        .WillOnce(DoAll(
            Invoke([&](std::shared_ptr<folly::IOBuf> buffer) {
                body = buffer->moveToFbString();
            }),
            Return()));
    EXPECT_CALL(response_mock_, sendEOM())
        .Times(1)
        .InSequence(sequence);

    auto headers = std::make_unique<proxygen::HTTPMessage>();
    headers->setURL("/test/");
    handler_.onRequest(std::move(headers));
    handler_.onEOM();
    folly::EventBaseManager::get()->getEventBase()->loop();
    EXPECT_EQ(body, R"({})");
}

//TEST_F(CreateThreadHandlerTest, BadJson) {
//    Sequence sequence;

//    const auto headers_matcher = AllOf(
//        Property(&HTTPMessage::getStatusCode, 400),
//        IsJsonContentType());
//    EXPECT_CALL(response_mock_,
//                sendHeaders(headers_matcher))
//        .Times(1)
//        .InSequence(sequence);
//    folly::fbstring body;
//    EXPECT_CALL(response_mock_, sendBody(_))
//        .InSequence(sequence)
//        .WillOnce(DoAll(
//            Invoke([&](std::shared_ptr<folly::IOBuf> buffer) {
//                body = buffer->moveToFbString();
//            }),
//            Return()));
//    EXPECT_CALL(response_mock_, sendEOM())
//        .Times(1)
//        .InSequence(sequence);

//    auto headers = std::make_unique<proxygen::HTTPMessage>();
//    headers->setURL("/test/");
//    handler_.onRequest(std::move(headers));
//    handler_.onBody(folly::IOBuf::copyBuffer(R"({"text":"that's invalid json actually)"));
//    handler_.onEOM();
//    folly::EventBaseManager::get()->getEventBase()->loop();
//    EXPECT_EQ(body, R"({1})");
//}
