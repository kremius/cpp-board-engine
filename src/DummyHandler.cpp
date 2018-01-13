#include "DummyHandler.h"

#include "DefaultPageNotFoundHandler.h"

#include <folly/fibers/FiberManagerMap.h>

using namespace proxygen;

void DummyHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {
    headers_ = std::move(headers);
    folly::EventBase* base = folly::EventBaseManager::get()->getEventBase();
    folly::fibers::getFiberManager(*base).add([this]() {
        handleRequest();
    });
}

void DummyHandler::handleRequest() noexcept {
    ResponseBuilder(downstream_)
        .status(200, "OK")
        .body("<h1>Hello, world!</h1>")
        .body(headers_->getURL())
        .sendWithEOM();
}

RequestHandler* DummyHandlerFactory::onRequest(
    RequestHandler*, HTTPMessage*) noexcept {
    return new DefaultPageNotFoundHandler;
}
