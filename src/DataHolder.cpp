#include "DataHolder.h"

DataHolder::FuturePosts DataHolder::FetchThreadPosts(uint64_t thread_id)
{
    auto&& view = container_.get<tags::thread_post_asc>();
    auto pair = view.range(
        [thread_id](auto tuple) { return thread_id <= tuple.value.thread_id; },
        [thread_id](auto tuple) { return tuple.value.thread_id <= thread_id; });

    folly::fbvector<Record> retval;
    for (auto it = pair.first; it != pair.second; ++it)
    {
        retval.push_back(*it);
    }

    // TODO
    return folly::makeFuture(std::move(retval));
}
