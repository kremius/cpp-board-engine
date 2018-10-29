#pragma once

#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include "DataHolder.h"
#include "Utils.h"

namespace board {

class BaseHandler : public proxygen::RequestHandler {
public:
    explicit BaseHandler(std::shared_ptr<board::DataHolder> holder, folly::fbstring prefix)
        : prefix_(std::move(prefix)),
          data_holder_(std::move(holder)) {
        // Nothing
    }

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
        noexcept final;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept final {
        if (!body_) {
            body_ = std::move(body);
        }
        body_->prependChain(std::move(body));
    }

    std::unique_ptr<folly::IOBuf> getBody() {
        full_body_baton_.wait();
        body_->coalesce();
        return std::move(body_);
    }

    void onEOM() noexcept final {
        full_body_baton_.post();
    }

    void onUpgrade(proxygen::UpgradeProtocol /*proto*/) noexcept override {
        // Nothing
    }

    void requestComplete() noexcept final {
        delete this;
    }

    void onError(proxygen::ProxygenError /*err*/) noexcept final {
        // TODO: schedule for deletion, because
        // it can happen when handler waits for future, etc
        delete this;
    }
private:
    virtual void handleRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept = 0;

    folly::fbstring prefix_;
    std::shared_ptr<board::DataHolder> data_holder_;

    std::unique_ptr<folly::IOBuf> body_;
    folly::fibers::Baton full_body_baton_;
};

} // namespace board
