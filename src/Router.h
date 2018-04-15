#pragma once

#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

struct Route {
    std::string path;
    std::vector<proxygen::HTTPMethod> methods;
    std::unique_ptr<proxygen::RequestHandlerFactory> factory;
};

class RoutesChain {
public:
    std::vector<Route> build() {
        return std::move(chain_);
    }

    template <typename T, typename... Args>
    RoutesChain& addThen(
        const std::string& path, const std::vector<proxygen::HTTPMethod>& methods, Args&&... args) {
        chain_.push_back({path, methods, std::make_unique<T>(std::forward<Args>(args)...)});
        return *this;
    }

    RoutesChain& addThen(
        const std::string& path,
        const std::vector<proxygen::HTTPMethod>& methods,
        std::unique_ptr<proxygen::RequestHandlerFactory> factory) {
        chain_.push_back({path, methods, std::move(factory)});
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
    std::vector<Route> routes_;
};
