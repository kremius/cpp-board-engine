#include "Router.h"

#include "DefaultPageNotFoundHandler.h"

#include <boost/range/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include <array>

void RouterFactory::onServerStart(folly::EventBase* evb) noexcept {
    for (auto& route : routes_) {
        route.factory->onServerStart(evb);
    }
}

void RouterFactory::onServerStop() noexcept {
    for (auto& route : routes_) {
        route.factory->onServerStop();
    }
}

proxygen::RequestHandler* RouterFactory::onRequest(
    proxygen::RequestHandler* handler,
    proxygen::HTTPMessage* message) noexcept {

    const auto maybe_method = message->getMethod();
    if (!maybe_method) {
        // TODO: bad method handler
        return nullptr;
    }
    const auto method = std::array<proxygen::HTTPMethod, 1>{{maybe_method.get()}};

    auto it = boost::find_if(routes_, [message, method](const Route& route) {
        return    boost::contains(route.methods, method)
               && boost::starts_with(message->getURL(), route.path);
    });

    if (it == routes_.end()) {
        // TODO: customizable 404 handler
        return new DefaultPageNotFoundHandler;
    }

    return it->factory->onRequest(handler, message);
}

void RouterFactory::addRoutes(std::vector<Route> routes) noexcept {
    if (routes_.empty()) {
        routes_ = std::move(routes);
    } else {
        // TODO: range move?
        std::move(routes.begin(), routes.end(), std::back_inserter(routes_));
    }
}
