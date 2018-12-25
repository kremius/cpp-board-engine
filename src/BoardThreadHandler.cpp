#include "BoardThreadHandler.h"

#include <boost/algorithm/string.hpp>

#include <folly/fibers/FiberManagerMap.h>
#include <folly/json.h>

#include <proxygen/httpserver/ResponseBuilder.h>

#include <glog/logging.h>

using proxygen::ResponseBuilder;
using proxygen::HTTPHeaderCode;

namespace board {

void ThreadHandler::handleRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
    const auto& url = headers->getURL();
    const auto maybe_thread_number = utils::extractThreadNumber(url, getPrefix());
    if (!maybe_thread_number) {
        buildNotFoundResponse();
        return;
    }

    const uint64_t number = maybe_thread_number.value();
    auto maybe_posts = data_holder_->fetchThreadPosts(number).getTry();
    if (maybe_posts.hasException()) {
        buildNotFoundResponse();
        return;
    }

    folly::dynamic array = folly::dynamic::array();
    for (const auto& post : maybe_posts.value())
    {
        array.push_back(
            folly::dynamic::object("id", post.post_id)("text", post.text));
    }
    buildOkResponse(folly::dynamic::object("posts", array));
}

} // namespace board
