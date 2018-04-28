#include <proxygen/httpserver/HTTPServer.h>

#include <glog/logging.h>

#include "BoardThreadHandler.h"
#include "Router.h"

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

    auto data_holder = std::make_shared<board::DataHolder>();

    auto router = std::make_unique<RouterFactory>();
    router->addRoutes(RoutesChain()
        .addThen<BoardThreadHandlerFactory>("/b/", {HTTPMethod::GET}, data_holder, "/b/")
        .addThen<BoardThreadHandlerFactory>("/vg/", {HTTPMethod::GET}, data_holder, "/vg/")
        .build());

    options.handlerFactories = RequestHandlerChain()
        .addThen(std::move(router))
        .build();
    options.h2cEnabled = true;

    HTTPServer server(std::move(options));
    server.bind({{folly::SocketAddress("127.0.0.1", 3333, true), HTTPServer::Protocol::HTTP}});

    LOG(INFO) << "Server will be started";
    server.start();

    return 0;
}
