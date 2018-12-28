#include "CreateThreadHandler.h"

void board::CreateThreadHandler::handleRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept
{
    // TODO
    (void)headers;
    buildNotFoundResponse();
}
