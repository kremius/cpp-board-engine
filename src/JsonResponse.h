#pragma once

#include <folly/dynamic.h>

#include <proxygen/httpserver/ResponseHandler.h>

namespace board {

void buildJsonResponse(uint16_t code, const std::string& message, const folly::dynamic& json) {
    // TODO
}

} // namespace board

