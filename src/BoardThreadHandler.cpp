#include "BoardThreadHandler.h"

#include <regex>

#include <folly/fibers/FiberManagerMap.h>
#include <folly/json.h>

#include <proxygen/httpserver/ResponseBuilder.h>

#include <glog/logging.h>

using proxygen::ResponseBuilder;
using proxygen::HTTPHeaderCode;

void BoardThreadHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept
{
    folly::EventBase* base = folly::EventBaseManager::get()->getEventBase();

    folly::fibers::FiberManager::Options options;
    // TODO: stack overflow on long numbers
    options.stackSize = 16 * 1024;

    folly::fibers::getFiberManager(*base, options).add(
    [this, headers = std::move(headers)]() mutable {
        handleRequest(std::move(headers));
    });
}

void BoardThreadHandler::handleRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept
{
    const auto& url = headers->getURL();

    static thread_local std::regex regex(R"(^/thread/([0-9]{1,20})/?$)");
    std::smatch match;

    if (!std::regex_search(url.begin(), url.end(), match, regex))
    {
        const folly::dynamic value = folly::dynamic::object("thread", "Not Found");
        ResponseBuilder(downstream_)
            .status(404, "Not Found")
            .header(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE, "application/json")
            .body(folly::toJson(value))
            .sendWithEOM();
        return;
    }

    auto number = folly::tryTo<int64_t>(match.str(1));
    if (!number)
    {
        const folly::dynamic value = folly::dynamic::object("thread", "Not Found");
        ResponseBuilder(downstream_)
            .status(404, "Not Found")
            .header(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE, "application/json")
            .body(folly::toJson(value))
            .sendWithEOM();
        return;
    }

    const folly::dynamic value = folly::dynamic::object("thread", number.value());
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
