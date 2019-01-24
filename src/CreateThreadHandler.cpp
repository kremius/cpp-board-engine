#include "CreateThreadHandler.h"

namespace {

namespace key {

const utils::string_view TEXT("text");
const utils::string_view ID("id");

} // namespace key

} // namespace

void board::CreateThreadHandler::handleRequest(
    std::unique_ptr<proxygen::HTTPMessage> /*headers*/) noexcept
{
    // TODO: filter out requests without proper json content header
    // with separate handler
    auto maybe_body = getBody();
    if (!maybe_body) {
        buildInternalErrorResponse();
        return;
    }

    const char* data = reinterpret_cast<const char*>(maybe_body.value()->data());
    auto maybe_json_body = utils::parseJson(
        utils::string_view(data, maybe_body.value()->length()));
    if (!maybe_json_body) {
        buildJsonResponse(400, "Bad Request");
        return;
    }
    folly::dynamic json_body = maybe_json_body.value();
    auto text_it = json_body.find(key::TEXT);
    if (   (text_it == json_body.items().end())
        || !text_it->second.isString()) {
        buildJsonResponse(400, "Bad Request");
        return;
    }

    auto result = data_holder_->createThread(
        {0, 0, text_it->second.asString(), {}}).getTry();

    if (result.hasException()) {
        buildInternalErrorResponse();
        return;
    }

    buildOkResponse(folly::dynamic::object(
        folly::StringPiece(key::ID), result.value()));
}
