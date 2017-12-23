#include "DummyHandler.h"

using namespace proxygen;

void DummyHandler::onRequest(std::unique_ptr<HTTPMessage>) noexcept {
    ResponseBuilder(downstream_)
        .status(200, "OK")
        .body("<h1>Hello, world!</h1>")
        .sendWithEOM();
}

RequestHandler* DummyHandlerFactory::onRequest(
    RequestHandler*, HTTPMessage*) noexcept {
    return new DummyHandler;
}
