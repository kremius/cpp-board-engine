#include <proxygen/httpserver/HTTPServer.h>

#include <glog/logging.h>

#include "DummyHandler.h"

using namespace proxygen;

int main(int /*argc*/, char* argv[]) {
    google::InitGoogleLogging(argv[0]);

    using proxygen::HTTPServerOptions;
    using proxygen::HTTPServer;

    HTTPServerOptions options;
    options.threads = 8;
    options.idleTimeout = std::chrono::milliseconds(60000);
    options.shutdownOn = {SIGINT, SIGTERM};
    options.enableContentCompression = false;
    options.handlerFactories = RequestHandlerChain()
        .addThen<DummyHandlerFactory>()
        .build();
    options.h2cEnabled = true;

    HTTPServer server(std::move(options));
    server.bind({{folly::SocketAddress("127.0.0.1", 3333, true), HTTPServer::Protocol::HTTP}});

    LOG(INFO) << "Server will be started";
    server.start();

    return 0;
}
