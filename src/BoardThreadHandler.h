#pragma once

#include "BaseHandler.h"
#include "DataHolder.h"

namespace board {

class GetThreadHandler : public BaseHandler {
public:
    explicit GetThreadHandler(std::shared_ptr<board::DataHolder> holder, folly::fbstring prefix)
        : BaseHandler(prefix),
          data_holder_(holder) {
        // Nothing
    }
private:
    void handleRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;
    std::shared_ptr<board::DataHolder> data_holder_;
};

using ThreadHandlerFactory = BaseHandlerFactory<GetThreadHandler>;

} // namespace board
