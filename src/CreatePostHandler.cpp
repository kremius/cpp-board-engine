#include "CreatePostHandler.h"

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

void CreatePostHandler::handleRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
    const auto& url = headers->getURL();
    const auto maybe_thread_number = utils::extractThreadNumber(url, getPrefix());
    if (!maybe_thread_number) {
        buildNotFoundResponse();
        return;
    }
    const uint64_t number = maybe_thread_number.value();

    auto maybe_body = getBody();
    if (!maybe_body) {
        buildInternalErrorResponse();
        return;
    }

    // TODO: extra copy here, can be removed?
    const char* data = reinterpret_cast<const char*>(maybe_body.value()->data());
    folly::dynamic json_body = folly::parseJson(utils::string_view(data, maybe_body.value()->length()));
    auto text_it = json_body.find(key::TEXT);
    if (   (text_it == json_body.items().end())
        || !text_it->second.isString()) {
        buildJsonResponse(400, "Bad Request");
        return;
    }

    auto result = data_holder_->addPostToThread({0, number, text_it->second.asString(), {}}).getTry();

    if (result.hasException()) {
        buildNotFoundResponse();
        return;
    }

    buildOkResponse(folly::dynamic::object("id", result.value()));
}

} // namespace board

