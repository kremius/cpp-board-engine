#include "DummyHandler.h"

#include <folly/fibers/FiberManagerMap.h>

using namespace proxygen;

void DummyHandler::onRequest(std::unique_ptr<HTTPMessage>) noexcept {
    ResponseBuilder(downstream_)
        .status(200, "OK")
        .body("<h1>Hello, world!</h1>")
        .sendWithEOM();

    folly::EventBase* base = folly::EventBaseManager::get()->getEventBase();
    const auto empty_options = folly::fibers::FiberManager::Options();
    folly::fibers::getFiberManager(*base, empty_options).add([]() {
        while (true) {
            // Do nothing
        };
    });
}

RequestHandler* DummyHandlerFactory::onRequest(
    RequestHandler*, HTTPMessage*) noexcept {
    return new DummyHandler;
}
