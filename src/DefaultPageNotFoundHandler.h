#pragma once

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

class DefaultPageNotFoundHandler : public proxygen::RequestHandler {
public:
    explicit DefaultPageNotFoundHandler() {
        // Nothing
    }

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
        noexcept override {
        using proxygen::ResponseBuilder;
        using proxygen::HTTPMessage;
        ResponseBuilder(downstream_)
            .status(404, "Not Found")
            .body("<h1>The requested URL ")
            .body(headers->getURL())
            .body(" was not found on this server.</h1>")
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
};
