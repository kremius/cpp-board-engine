#pragma once

#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

using Route = std::pair<std::string, std::unique_ptr<proxygen::RequestHandlerFactory>>;

class RoutesChain {
public:
    std::vector<Route> build() {
        return std::move(chain_);
    }

    template <typename T, typename... Args>
    RoutesChain& addThen(const std::string& path, Args&&... args) {
        chain_.push_back({path, std::make_unique<T>(std::forward<Args>(args)...)});
        return *this;
    }

    RoutesChain& addThen(
        const std::string& path,
        std::unique_ptr<proxygen::RequestHandlerFactory> factory) {
        chain_.push_back({path, std::move(factory)});
        return *this;
    }
private:
    std::vector<Route> chain_;
};

class RouterFactory : public proxygen::RequestHandlerFactory {
public:
    void onServerStart(folly::EventBase* evb) noexcept override;
    void onServerStop() noexcept override;

    proxygen::RequestHandler* onRequest(
        proxygen::RequestHandler* handler,
        proxygen::HTTPMessage* message) noexcept override;

    void addRoutes(std::vector<Route> routes) noexcept;
private:
    using Route = std::pair<std::string, std::unique_ptr<RequestHandlerFactory>>;
    std::vector<Route> routes_;
};
