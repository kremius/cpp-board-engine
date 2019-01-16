#pragma once

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
    std::shared_ptr<board::DataHolder> holder_;
    THandler handler_;
    proxygen::MockResponseHandler response_mock_;
};
