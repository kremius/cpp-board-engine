#pragma once

#include "BaseHandler.h"
#include "DataHolder.h"

namespace board {

class BoardThreadHandler : public BaseHandler {
public:
    utils::Optional<uint64_t> extractThreadNumber(const std::string& url);

    explicit BoardThreadHandler(std::shared_ptr<board::DataHolder> holder, folly::fbstring prefix)
        : BaseHandler(prefix),
          data_holder_(holder) {
        // Nothing
    }
private:
    void handleRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;
    std::shared_ptr<board::DataHolder> data_holder_;
};

class BoardThreadHandlerFactory : public proxygen::RequestHandlerFactory {
public:
    BoardThreadHandlerFactory(std::shared_ptr<board::DataHolder> holder, folly::fbstring prefix)
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
        proxygen::HTTPMessage*) noexcept override;
private:
    folly::fbstring prefix_;
    std::shared_ptr<board::DataHolder> data_holder_;
};

} // namespace board
