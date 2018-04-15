#include "DataHolder.h"

#include <boost/range/iterator_range.hpp>

using namespace board;

DataHolder::DataHolder()
    : executor_(1) {
    // Nothing
}

folly::Future<DataHolder::PostsType> DataHolder::FetchThreadPosts(uint64_t thread_id) {
    folly::Promise<PostsType> promise;
    folly::Future<PostsType> future = promise.getFuture();

    executor_.add([this, thread_id, promise = std::move(promise)]() mutable {
        auto&& view = container_.get<tags::thread_post_asc>();

        auto pair = view.range(
            [thread_id](auto tuple) { return thread_id <= tuple.value.thread_id; },
            [thread_id](auto tuple) { return tuple.value.thread_id <= thread_id; });

        folly::fbvector<Record> retval;
        for (const auto& Record : boost::make_iterator_range(pair.first, pair.second)) {
            retval.push_back(Record);
        }
        promise.setValue(std::move(retval));
    });

    return future;
}
