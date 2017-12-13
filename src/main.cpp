#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

using namespace proxygen;

class DummyHandler : public RequestHandler {
public:
    explicit DummyHandler() {
        // Nothing
    }

    void onRequest(std::unique_ptr<HTTPMessage> /*headers*/)
        noexcept override {
        ResponseBuilder(downstream_)
            .status(200, "OK")
            .body("<h1>Hello, world!</h1>")
            .sendWithEOM();
    }

    void onBody(std::unique_ptr<folly::IOBuf> /*body*/) noexcept override {
        // Nothing
    }

    void onEOM() noexcept override {
        // Nothing
    }

    void onUpgrade(UpgradeProtocol /*proto*/) noexcept override {
        // Nothing
    }

    void requestComplete() noexcept override {
        delete this;
    }

    void onError(ProxygenError /*err*/) noexcept override {
        delete this;
    }
};


class DummyHandlerFactory : public RequestHandlerFactory {
public:
    void onServerStart(folly::EventBase* /*evb*/) noexcept override {
        // Nothing
    }

    void onServerStop() noexcept override {
        // Nothing
    }

    RequestHandler* onRequest(RequestHandler*, HTTPMessage*) noexcept override {
        return new DummyHandler;
    }
};

int main(int argc, char* argv[]) {
    using proxygen::HTTPServerOptions;
    using proxygen::HTTPServer;

    HTTPServerOptions options;
    options.threads = 4;
    options.idleTimeout = std::chrono::milliseconds(60000);
    options.shutdownOn = {SIGINT, SIGTERM};
    options.enableContentCompression = false;
    options.handlerFactories = RequestHandlerChain()
        .addThen<DummyHandlerFactory>()
        .build();
    options.h2cEnabled = true;

    HTTPServer server(std::move(options));
    server.bind({{folly::SocketAddress("127.0.0.1", 3333, true), HTTPServer::Protocol::HTTP}});

    server.start();

    return 0;
}
