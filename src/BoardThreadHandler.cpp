#include "BoardThreadHandler.h"

void BoardThreadHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept
{
    // TODO
    /*headers_ = std::move(headers);
    folly::EventBase* base = folly::EventBaseManager::get()->getEventBase();
    folly::fibers::getFiberManager(*base).add([this]() {
        handleRequest();
    });*/
}

proxygen::RequestHandler* BoardThreadHandlerFactory::onRequest(
    proxygen::RequestHandler*,
    proxygen::HTTPMessage*) noexcept {
    return new BoardThreadHandler;
}
