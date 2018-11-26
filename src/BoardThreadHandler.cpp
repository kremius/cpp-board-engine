#include "BoardThreadHandler.h"

#include <boost/algorithm/string.hpp>

#include <folly/fibers/FiberManagerMap.h>
#include <folly/json.h>

#include <proxygen/httpserver/ResponseBuilder.h>

#include <glog/logging.h>

using proxygen::ResponseBuilder;
using proxygen::HTTPHeaderCode;

namespace board {

utils::Optional<uint64_t> ThreadHandler::extractThreadNumber(const std::string& url) {
    if (!boost::starts_with(url, getPrefix())) {
        return {};
    }
    const size_t start = getPrefix().size();
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

void ThreadHandler::handleRequest(
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

    auto maybe_posts = data_holder_->fetchThreadPosts(number).getTry();

    if (maybe_posts.hasException()) {
        // TODO: It would be nice to reduce copy-paste here
        const folly::dynamic value = folly::dynamic::object("thread", "Not Found");
        ResponseBuilder(downstream_)
            .status(404, "Not Found")
            .header(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE, "application/json")
            .body(folly::toJson(value))
            .sendWithEOM();
        return;
    }

    folly::dynamic array = folly::dynamic::array();
    auto&& posts = maybe_posts.value();
    for (const auto& post : posts)
    {
        array.push_back(
            // TODO: no conversion from fbstring (?)
            folly::dynamic::object("id", post.post_id)("text", post.text.c_str()));
    }

    ResponseBuilder(downstream_)
        .status(200, "OK")
        .header(HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE, "application/json")
        .body(folly::toJson(folly::dynamic::object("posts", array)))
        .sendWithEOM();
}

} // namespace board
