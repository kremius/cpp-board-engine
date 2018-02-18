#pragma once

#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include <experimental/optional>

template<class T>
using Optional = std::experimental::optional<T>;

class BoardThreadHandler : public proxygen::RequestHandler {
public:
    static const std::string URL_PREFIX;
    static Optional<uint64_t> extractThreadNumber(const std::string& url);

    explicit BoardThreadHandler() {
        // Nothing
    }

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
        noexcept override;

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
    void handleRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept;
};

class BoardThreadHandlerFactory : public proxygen::RequestHandlerFactory {
public:
    void onServerStart(folly::EventBase* /*evb*/) noexcept override {
        // Nothing
    }

    void onServerStop() noexcept override {
        // Nothing
    }

    proxygen::RequestHandler* onRequest(
        proxygen::RequestHandler*,
        proxygen::HTTPMessage*) noexcept override;
};
