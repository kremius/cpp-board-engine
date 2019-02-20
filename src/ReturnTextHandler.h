#pragma once

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include "Utils.h"

class ReturnTextHandler : public proxygen::RequestHandler {
public:
    explicit ReturnTextHandler(
        int status_code,
        const std::string &code_name,
        utils::string_view text)
        : status_code_(status_code),
          code_name_(code_name),
          text_(text) {
        // Nothing
    }

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
        noexcept override {
        using proxygen::ResponseBuilder;
        using proxygen::HTTPMessage;
        ResponseBuilder(downstream_)
            .status(status_code_, code_name_)
            .body(folly::IOBuf::copyBuffer(text_.data(), text_.size()))
            .sendWithEOM();
    }

    void onBody(std::unique_ptr<folly::IOBuf> /*body*/) noexcept override {
        // Nothing
    }

    void onEOM() noexcept override {
        // Nothing
    }

    void onUpgrade(proxygen::UpgradeProtocol /*proto*/) noexcept override {
        // Nothing
    }

    void requestComplete() noexcept override {
        delete this;
    }

    void onError(proxygen::ProxygenError /*err*/) noexcept override {
        delete this;
    }
private:
    int status_code_;
    std::string code_name_;
    utils::string_view text_;
};
