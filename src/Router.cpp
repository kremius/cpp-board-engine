#include "Router.h"

#include <boost/range/algorithm.hpp>
#include <boost/algorithm/string.hpp>

void RouterFactory::onServerStart(folly::EventBase* /*evb*/) noexcept {
    // TODO: start all factories
}

void RouterFactory::onServerStop() noexcept {
    // TODO: stop all factories
}

proxygen::RequestHandler* RouterFactory::onRequest(
    proxygen::RequestHandler* handler,
    proxygen::HTTPMessage* message) noexcept {
    auto it = boost::find_if(routes_, [message](const Route& route) {
        return boost::starts_with(message->getURL(), route.first);
    });

    if (it == routes_.end()) {
        // TODO: 404 handler
        return nullptr;
    }

    return it->second->onRequest(handler, message);
}

void RouterFactory::addRoutes(std::vector<RouterFactory::Route> routes) noexcept {
    if (routes_.empty()) {
        routes_ = std::move(routes);
    } else {
        // TODO: range move?
        std::move(routes.begin(), routes.end(), std::back_inserter(routes_));
    }
}
