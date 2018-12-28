#pragma once

#include "BaseHandler.h"

namespace board {

class CreateThreadHandler : public BaseHandler {
public:
    explicit CreateThreadHandler(std::shared_ptr<board::DataHolder> holder, folly::fbstring prefix)
        : BaseHandler(prefix),
          data_holder_(holder) {
        // Nothing
    }
private:
    void handleRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;
    std::shared_ptr<board::DataHolder> data_holder_;
};

using CreateThreadHandlerFactory = BaseHandlerFactory<CreateThreadHandler>;

} // namespace board

