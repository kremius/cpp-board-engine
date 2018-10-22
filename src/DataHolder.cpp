#include "DataHolder.h"

#include <boost/range/iterator_range.hpp>

using namespace board;

DataHolder::DataHolder()
    : post_id_(1),
      executor_(1) {
    // Nothing
}

folly::SemiFuture<DataHolder::PostsType> DataHolder::FetchThreadPosts(uint64_t thread_id) {
    folly::Promise<PostsType> promise;
    folly::SemiFuture<PostsType> future = promise.getSemiFuture();

    executor_.add([this, thread_id, promise = std::move(promise)]() mutable {
        auto&& view = container_.get<tags::thread_post_asc>();

        const auto pair = view.range(
            [thread_id](const auto& post) { return thread_id <= post.value.thread_id; },
            [thread_id](const auto& post) { return post.value.thread_id <= thread_id; });

        folly::fbvector<Post> retval;
        for (const auto& Record : boost::make_iterator_range(pair.first, pair.second)) {
            retval.push_back(Record);
        }
        if (retval.empty()) {
            // TODO: proper exception types
            promise.setException(std::runtime_error("Thread is not found"));
            return;
        }
        promise.setValue(std::move(retval));
    });

    return future;
}

folly::SemiFuture<uint64_t> DataHolder::AddPostToThread(DataHolder::Post post) {
    folly::Promise<uint64_t> promise;
    folly::Future<uint64_t> future = promise.getFuture();

    executor_.add([this, post = std::move(post), promise = std::move(promise)]() mutable {
        auto&& view = container_.get<tags::post_hashed>();
        auto thread_it = view.find(post.thread_id);
        if (thread_it == view.end()) {
            promise.setException(std::runtime_error("Thread is not found"));
            return;
        }
        post.post_id = post_id_;
        const uint64_t retval = post_id_;
        ++post_id_;
        container_.insert(std::move(post));
        promise.setValue(retval);
    });

    return future;
}

folly::SemiFuture<uint64_t> DataHolder::CreateThread(DataHolder::Post post)
{
    folly::Promise<uint64_t> promise;
    folly::Future<uint64_t> future = promise.getFuture();

    executor_.add([this, post = std::move(post), promise = std::move(promise)]() mutable {
        post.post_id = post_id_;
        post.thread_id = post_id_;
        const uint64_t retval = post_id_;
        ++post_id_;
        container_.insert(std::move(post));
        promise.setValue(retval);
    });

    return future;
}
