#pragma once

#include <folly/dynamic.h>
#include <folly/futures/Future.h>
#include <folly/FBVector.h>
#include <folly/executors/CPUThreadPoolExecutor.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

namespace board {

namespace tags {

struct thread_post_asc {};
struct post_hashed {};

}

using boost::multi_index::indexed_by;
using boost::multi_index::ordered_unique;
using boost::multi_index::hashed_unique;
using boost::multi_index::tag;
using boost::multi_index::member;
using boost::multi_index::composite_key;

class DataHolder {
    // TODO: current implementation is slow, can be made much better
public:
    struct Post {
        Post(const Post& other) = default;
        Post(Post&& other) = default;

        uint64_t post_id;
        uint64_t thread_id;
        folly::fbstring text;
        folly::fbstring image;
    };
    using PostsType = folly::fbvector<Post>;

    DataHolder();

    folly::Future<uint64_t> CreateThread(Post post);
    folly::Future<uint64_t> AddPostToThread(Post post);
    folly::Future<PostsType> FetchThreadPosts(uint64_t thread_id);
private:
    using DataContainer
        = boost::multi_index_container<Post,
            indexed_by<
                ordered_unique<
                    tag<tags::thread_post_asc>,
                    composite_key<
                        Post,
                        member<Post, uint64_t, &Post::thread_id>,
                        member<Post, uint64_t, &Post::post_id>>>,
                hashed_unique<
                    tag<tags::post_hashed>,
                    member<Post, uint64_t, &Post::post_id>>>>;

    DataContainer container_;
    uint64_t post_id_;

    folly::CPUThreadPoolExecutor executor_;
};

} // namespace board
