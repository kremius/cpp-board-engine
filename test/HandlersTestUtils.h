#pragma once

#include <folly/io/async/EventBaseManager.h>

#include <proxygen/httpserver/Mocks.h>

#include "DataHolder.h"

MATCHER(IsJsonContentType, "") {
    using proxygen::HTTPHeaderCode;

    const auto& headers = arg.getHeaders();
    return headers.getSingleOrEmpty(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE) == "application/json";
}

template<class THandler>
class BaseHandlerTest : public ::testing::Test {
protected:
    BaseHandlerTest()
        : holder_(std::make_shared<board::DataHolder>()),
          handler_(holder_, "/test/"),
          response_mock_(&handler_) {
        handler_.setResponseHandler(&response_mock_);
        holder_->createThread({0, 0, "text1", "image1"}).getTry();
    };

    void mockResponse(int status_code) {
        using ::testing::_;
        using ::testing::Invoke;
        using ::testing::Property;
        using ::testing::Return;
        using proxygen::HTTPMessage;

        const auto headers_matcher = AllOf(
            Property(&HTTPMessage::getStatusCode, status_code),
            IsJsonContentType());
        EXPECT_CALL(response_mock_,
                    sendHeaders(headers_matcher))
            .Times(1)
            .InSequence(sequence_);
        EXPECT_CALL(response_mock_, sendBody(_))
            .InSequence(sequence_)
            .WillOnce(DoAll(
                Invoke([this](std::shared_ptr<folly::IOBuf> buffer) {
                    body_ = buffer->moveToFbString();
                }),
                Return()));
        EXPECT_CALL(response_mock_, sendEOM())
            .Times(1)
            .InSequence(sequence_);
    }

    std::shared_ptr<board::DataHolder> holder_;
    THandler handler_;
    proxygen::MockResponseHandler response_mock_;
    ::testing::Sequence sequence_;

    folly::fbstring body_;
};
