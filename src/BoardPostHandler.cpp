#include "BoardPostHandler.h"

#include <boost/algorithm/string.hpp>

#include <folly/dynamic.h>
#include <folly/fibers/FiberManagerMap.h>
#include <folly/json.h>

#include <proxygen/httpserver/ResponseBuilder.h>

#include <glog/logging.h>

using proxygen::ResponseBuilder;
using proxygen::HTTPHeaderCode;

namespace
{

namespace key
{

const utils::string_view TEXT("text");

} // namespace key

} // namespace

namespace board {

void PostHandler::handleRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
    const auto& url = headers->getURL();
    const auto maybe_thread_number = utils::extractThreadNumber(url, getPrefix());
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

    auto maybe_body = getBody();
    if (!maybe_body) {
        // TODO: should we send anything at all?
        ResponseBuilder(downstream_)
            .status(500, "Internal server error")
            .sendWithEOM();
        return;
    }

    // TODO: extra copy here, can be removed?
    const char* data = reinterpret_cast<const char*>(maybe_body.value()->data());
    folly::dynamic json_body = folly::parseJson(utils::string_view(data, maybe_body.value()->length()));
    auto text_it = json_body.find(key::TEXT);
    if (   (text_it == json_body.items().end())
        || !text_it->second.isString()) {
        // TODO: proper return code about bad request
        ResponseBuilder(downstream_)
            .status(500, "Internal server error")
            .sendWithEOM();
        return;
    }

    auto result = data_holder_->addPostToThread({0, number, text_it->second.asString(), {}}).getTry();

    if (result.hasException()) {
        // TODO: It would be nice to reduce copy-paste here
        const folly::dynamic value = folly::dynamic::object("thread", "Not Found");
        ResponseBuilder(downstream_)
            .status(404, "Not Found")
            .header(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE, "application/json")
            .body(folly::toJson(value))
            .sendWithEOM();
        return;
    }

    ResponseBuilder(downstream_)
        .status(200, "OK")
        .header(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE, "application/json")
        .body(folly::toJson(folly::dynamic::object("id", result.value())))
        .sendWithEOM();
}

} // namespace board

