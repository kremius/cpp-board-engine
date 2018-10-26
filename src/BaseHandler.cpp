#include "BaseHandler.h"

#include <folly/fibers/FiberManagerMap.h>

namespace board {

void BaseHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
    folly::EventBase* base = folly::EventBaseManager::get()->getEventBase();

    folly::fibers::getFiberManager(*base).add(
    [this, headers = std::move(headers)]() mutable {
        handleRequest(std::move(headers));
    });
}

} // namespace board
