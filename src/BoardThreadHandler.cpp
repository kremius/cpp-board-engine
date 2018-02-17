#include "BoardThreadHandler.h"

#include <folly/fibers/FiberManagerMap.h>
#include <folly/json.h>

#include <proxygen/httpserver/ResponseBuilder.h>

using proxygen::ResponseBuilder;
using proxygen::HTTPHeaderCode;

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
    folly::dynamic value = folly::dynamic::object;
    value["thread"] = headers->getURL();

    ResponseBuilder(downstream_)
        .status(200, "OK")
        .header(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE, "application/json")
        .body(folly::toJson(value))
        .sendWithEOM();
}

proxygen::RequestHandler* BoardThreadHandlerFactory::onRequest(
    proxygen::RequestHandler*,
    proxygen::HTTPMessage*) noexcept {
    return new BoardThreadHandler;
}
