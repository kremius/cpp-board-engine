#pragma once

#include "BaseHandler.h"

namespace board {

class CreatePostHandler : public BaseHandler {
public:
    explicit CreatePostHandler(std::shared_ptr<board::DataHolder> holder, folly::fbstring prefix)
        : BaseHandler(prefix),
          data_holder_(holder) {
        // Nothing
    }
private:
    void handleRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;
    std::shared_ptr<board::DataHolder> data_holder_;
};

using CreatePostHandlerFactory = BaseHandlerFactory<CreatePostHandler>;

} // namespace board

