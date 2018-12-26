#pragma once

#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include "DataHolder.h"
#include "JsonResponse.h"
#include "Utils.h"

namespace board {

class BaseHandler : public proxygen::RequestHandler {
public:
    explicit BaseHandler(folly::fbstring prefix)
        : prefix_(std::move(prefix)),
          is_deletion_scheduled_(false),
          is_finished_(false)
    {
        // Nothing
    }

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
        noexcept final;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept final {
        if (!body_) {
            body_ = std::move(body);
            return;
        }
        body_->prependChain(std::move(body));
    }

    using MaybeBody = utils::optional<std::unique_ptr<folly::IOBuf>>;
    MaybeBody getBody() {
        full_body_baton_.wait();
        if (isDeletionScheduled()) {
            return {};
        }
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
        scheduleDeletion();
    }

    void onError(proxygen::ProxygenError /*err*/) noexcept final {
        // TODO: log error
        scheduleDeletion();
    }
protected:
    virtual void handleRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept = 0;

    bool isDeletionScheduled() const {
        return is_deletion_scheduled_;
    }
    bool isFinished() const {
        return is_finished_;
    }
    void tryDelete() {
        if (!is_finished_ || !is_deletion_scheduled_) {
            return;
        }
        delete this;
    }

    utils::string_view getPrefix() const {
        return utils::string_view(prefix_.data(), prefix_.size());
    }
    void buildJsonResponse(
        uint16_t code,
        const std::string& message,
        const folly::dynamic& json = folly::dynamic::object()) {
        board::buildJsonResponse(downstream_, code, message, json);
    }
    void buildNotFoundResponse() {
        board::buildNotFoundResponse(downstream_);
    }
    void buildInternalErrorResponse() {
        board::buildInternalErrorResponse(downstream_);
    }
    void buildOkResponse(const folly::dynamic& json) {
        board::buildOkResponse(downstream_, json);
    }
private:
    void scheduleDeletion() {
        full_body_baton_.post();
        is_deletion_scheduled_ = true;
        tryDelete();
    }

    folly::fbstring prefix_;

    std::unique_ptr<folly::IOBuf> body_;
    folly::fibers::Baton full_body_baton_;

    bool is_deletion_scheduled_;
    bool is_finished_;
};

template<class HandlerType>
class BaseHandlerFactory final : public proxygen::RequestHandlerFactory {
public:
    BaseHandlerFactory(std::shared_ptr<board::DataHolder> holder, folly::fbstring prefix)
        : prefix_(std::move(prefix)),
          data_holder_(holder) {
        // Nothing
    }
    void onServerStart(folly::EventBase* /*evb*/) noexcept override {
        // Nothing
    }

    void onServerStop() noexcept override {
        // Nothing
    }

    proxygen::RequestHandler* onRequest(
        proxygen::RequestHandler*,
        proxygen::HTTPMessage*) noexcept override {
        return new HandlerType(data_holder_, prefix_);
    }
private:
    folly::fbstring prefix_;
    std::shared_ptr<board::DataHolder> data_holder_;
};

} // namespace board
