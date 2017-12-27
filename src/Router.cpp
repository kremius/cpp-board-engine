#include "Router.h"

void RouterFactory::onServerStart(folly::EventBase* /*evb*/) noexcept {
    // Do nothing
}

void RouterFactory::onServerStop() noexcept {
    // Do nothing
}

proxygen::RequestHandler* RouterFactory::onRequest(
    proxygen::RequestHandler* /*handler*/,
    proxygen::HTTPMessage *message) noexcept {
    // TODO:
    return nullptr;
}

void RouterFactory::addRoutes(std::vector<RouterFactory::Route> routes) noexcept {
    // TODO:
}
