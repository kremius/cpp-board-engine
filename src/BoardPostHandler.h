#pragma once

#include "BaseHandler.h"

namespace board {

class PostHandler : public BaseHandler {
public:
    explicit PostHandler(std::shared_ptr<board::DataHolder> holder, folly::fbstring prefix)
        : BaseHandler(prefix),
          data_holder_(holder) {
        // Nothing
    }
private:
    void handleRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;
    std::shared_ptr<board::DataHolder> data_holder_;
};

using PostHandlerFactory = BaseHandlerFactory<PostHandler>;

} // namespace board

