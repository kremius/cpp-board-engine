#pragma once

#include "BaseHandler.h"
#include "DataHolder.h"

namespace board {

class ThreadHandler : public BaseHandler {
public:
    utils::Optional<uint64_t> extractThreadNumber(const std::string& url);

    explicit ThreadHandler(std::shared_ptr<board::DataHolder> holder, folly::fbstring prefix)
        : BaseHandler(prefix),
          data_holder_(holder) {
        // Nothing
    }
private:
    void handleRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;
    std::shared_ptr<board::DataHolder> data_holder_;
};

using ThreadHandlerFactory = BaseHandlerFactory<ThreadHandler>;

} // namespace board
