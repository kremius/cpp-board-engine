#pragma once

#include <folly/dynamic.h>
#include <folly/futures/Future.h>

class DataHolder
{
public:
    // TODO: should dynamic be used or struct should be returned instead?
    using FutureDynamic = folly::Future<folly::dynamic>;

    FutureDynamic FetchThreadPosts(uint64_t thread_id);
private:
};
