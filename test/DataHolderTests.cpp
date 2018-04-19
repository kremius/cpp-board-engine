#include "DataHolder.h"

#include <gtest/gtest.h>

TEST(DataHolder, Empty)
{
    board::DataHolder holder;

    {
        auto future = holder.FetchThreadPosts(0);
        EXPECT_THROW(future.get(), std::runtime_error);
    }

    {
        auto future = holder.FetchThreadPosts(42);
        EXPECT_THROW(future.get(), std::runtime_error);
    }
}

TEST(DataHolder, ThreadAndPosts)
{
    board::DataHolder holder;

    {
        auto future = holder.CreateThread({0, 0, "hello world", "image.png"});
        const uint64_t thread_id = future.get();
        ASSERT_EQ(thread_id, 1);
    }

    {
        auto future = holder.FetchThreadPosts(1);
        const board::DataHolder::PostsType posts = future.get();
        ASSERT_EQ(posts.size(), 1);
        EXPECT_EQ(posts[0].post_id, 1);
        EXPECT_EQ(posts[0].thread_id, 1);
        EXPECT_EQ(posts[0].text, "hello world");
        EXPECT_EQ(posts[0].image, "image.png");
    }

    // TODO: more
}
