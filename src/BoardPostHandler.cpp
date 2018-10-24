#include "BoardPostHandler.h"

#include <boost/algorithm/string.hpp>

#include <folly/fibers/FiberManagerMap.h>
#include <folly/json.h>

#include <proxygen/httpserver/ResponseBuilder.h>

#include <glog/logging.h>

using proxygen::ResponseBuilder;
using proxygen::HTTPHeaderCode;

void BoardPostHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
    folly::EventBase* base = folly::EventBaseManager::get()->getEventBase();

    folly::fibers::getFiberManager(*base).add(
    [this, headers = std::move(headers)]() mutable {
        handleRequest(std::move(headers));
    });
}

Optional<uint64_t> BoardPostHandler::extractThreadNumber(const std::string& url) {
    if (!boost::starts_with(url, prefix_)) {
        return {};
    }
    const size_t start = prefix_.size();
    const size_t end = url.size();

    if (end <= start) {
        return {};
    }

    folly::fbstring after_perfix(&url[start], end - start);
    auto maybe_number = folly::tryTo<uint64_t>(after_perfix);
    if (!maybe_number) {
        return {};
    }
    const uint64_t number = maybe_number.value();
    return number;
}

void BoardPostHandler::handleRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
    const auto& url = headers->getURL();
    const auto maybe_thread_number = extractThreadNumber(url);
    if (!maybe_thread_number) {
        const folly::dynamic value = folly::dynamic::object("thread", "Not Found");
        ResponseBuilder(downstream_)
            .status(404, "Not Found")
            .header(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE, "application/json")
            .body(folly::toJson(value))
            .sendWithEOM();
        return;
    }
    const uint64_t number = maybe_thread_number.value();

    // TODO
    ResponseBuilder(downstream_)
        .status(200, "OK")
        .header(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE, "application/json")
        .body(folly::toJson(folly::dynamic::object("number", number)))
        .sendWithEOM();
}

proxygen::RequestHandler* BoardPostHandlerFactory::onRequest(
    proxygen::RequestHandler*,
    proxygen::HTTPMessage*) noexcept {
    return new BoardPostHandler(data_holder_, prefix_);
}
