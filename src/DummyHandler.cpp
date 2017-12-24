#include "DummyHandler.h"

#include <folly/fibers/FiberManagerMap.h>

using namespace proxygen;

void DummyHandler::onRequest(std::unique_ptr<HTTPMessage>) noexcept {
    folly::EventBase* base = folly::EventBaseManager::get()->getEventBase();
    folly::fibers::getFiberManager(*base).add([this]() {
        ResponseBuilder(downstream_)
            .status(200, "OK")
            .body("<h1>Hello, world!</h1>")
            .sendWithEOM();
    });
}

RequestHandler* DummyHandlerFactory::onRequest(
    RequestHandler*, HTTPMessage*) noexcept {
    return new DummyHandler;
}
