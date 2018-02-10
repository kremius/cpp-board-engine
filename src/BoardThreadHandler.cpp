#include "BoardThreadHandler.h"

#include <folly/fibers/FiberManagerMap.h>

#include <proxygen/httpserver/ResponseBuilder.h>

using proxygen::ResponseBuilder;

void BoardThreadHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept
{
    folly::EventBase* base = folly::EventBaseManager::get()->getEventBase();
    folly::fibers::getFiberManager(*base).add(
    [this, headers = std::move(headers)]() mutable {
        handleRequest(std::move(headers));
    });
}

void BoardThreadHandler::handleRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept
{
    ResponseBuilder(downstream_)
        .status(200, "OK")
        .body("<h1>Board thread handler</h1><br>")
        .body(headers->getURL())
        .sendWithEOM();
}

proxygen::RequestHandler* BoardThreadHandlerFactory::onRequest(
    proxygen::RequestHandler*,
    proxygen::HTTPMessage*) noexcept {
    return new BoardThreadHandler;
}
