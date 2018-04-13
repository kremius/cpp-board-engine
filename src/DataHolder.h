#pragma once

#include <folly/dynamic.h>
#include <folly/futures/Future.h>
#include <folly/FBVector.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

namespace tags
{

struct thread_post_asc {};
struct post_asc {};

}

using boost::multi_index::indexed_by;
using boost::multi_index::ordered_unique;
using boost::multi_index::hashed_unique;
using boost::multi_index::tag;
using boost::multi_index::member;
using boost::multi_index::composite_key;

class DataHolder
{
public:
    struct Record
    {
        uint64_t post_id;
        uint64_t thread_id;
        folly::fbstring text;
        folly::fbstring image;
    };
    using FuturePosts = folly::Future<folly::fbvector<Record>>;

    FuturePosts FetchThreadPosts(uint64_t thread_id);
private:
    using DataContainer
        = boost::multi_index_container<Record,
            indexed_by<
                ordered_unique<
                    tag<tags::thread_post_asc>,
                    composite_key<
                        Record,
                        member<Record, uint64_t, &Record::thread_id>,
                        member<Record, uint64_t, &Record::post_id>>>,
                hashed_unique<
                    tag<tags::post_asc>,
                    member<Record, uint64_t, &Record::post_id>>>>;

    DataContainer container_;
};
