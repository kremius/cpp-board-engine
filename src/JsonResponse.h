#pragma once

#include <folly/dynamic.h>
#include <folly/json.h>

#include <proxygen/httpserver/ResponseBuilder.h>
#include <proxygen/httpserver/ResponseHandler.h>

namespace board {

inline void buildJsonResponse(
    proxygen::ResponseHandler* handler, uint16_t code, const std::string& message, const folly::dynamic& json) {
    proxygen::ResponseBuilder(handler)
        .status(code, message)
        .header(proxygen::HTTPHeaderCode::HTTP_HEADER_CONTENT_TYPE, "application/json")
        .body(folly::toJson(json))
        .sendWithEOM();
}

inline void buildNotFoundResponse(proxygen::ResponseHandler* handler) {
    buildJsonResponse(handler, 404, "Not Found", folly::dynamic::object());
}

inline void buildInternalErrorResponse(proxygen::ResponseHandler* handler) {
    buildJsonResponse(handler, 500, "Internal Server error", folly::dynamic::object());
}

inline void buildOkResponse(proxygen::ResponseHandler* handler, const folly::dynamic& json) {
    buildJsonResponse(handler, 200, "OK", json);
}

} // namespace board

