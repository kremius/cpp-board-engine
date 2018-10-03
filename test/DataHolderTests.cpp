#include "DataHolder.h"

#include <gtest/gtest.h>

#include <boost/range/irange.hpp>

TEST(DataHolder, Empty)
{
    board::DataHolder holder;

    {
        auto future = holder.FetchThreadPosts(0);
        EXPECT_THROW(std::move(future).get(), std::runtime_error);
    }

    {
        auto future = holder.FetchThreadPosts(42);
        EXPECT_THROW(std::move(future).get(), std::runtime_error);
    }
}

TEST(DataHolder, ThreadAndPosts)
{
    board::DataHolder holder;

    {
        auto future = holder.CreateThread({0, 0, "hello world", "image.png"});
        const uint64_t thread_id = std::move(future).get();
        ASSERT_EQ(thread_id, 1);
    }
    {
        const board::DataHolder::PostsType posts = holder.FetchThreadPosts(1).get();
        ASSERT_EQ(posts.size(), 1);
        EXPECT_EQ(posts[0].post_id, 1);
        EXPECT_EQ(posts[0].thread_id, 1);
        EXPECT_EQ(posts[0].text, "hello world");
        EXPECT_EQ(posts[0].image, "image.png");
    }

    // Thread not found
    EXPECT_THROW(holder.FetchThreadPosts(2).get(), std::runtime_error);
    EXPECT_THROW(holder.AddPostToThread({0, 2, "", ""}).get(), std::runtime_error);

    {
        auto future = holder.CreateThread({0, 0, "another thread", ""});
        const uint64_t thread_id = std::move(future).get();
        ASSERT_EQ(thread_id, 2);
    }
    {
        const board::DataHolder::PostsType posts = holder.FetchThreadPosts(2).get();
        ASSERT_EQ(posts.size(), 1);
        EXPECT_EQ(posts[0].post_id, 2);
        EXPECT_EQ(posts[0].thread_id, 2);
        EXPECT_EQ(posts[0].text, "another thread");
        EXPECT_EQ(posts[0].image, "");
    }

    // Thread not found
    EXPECT_THROW(holder.FetchThreadPosts(3).get(), std::runtime_error);
    EXPECT_THROW(holder.AddPostToThread({0, 3, "", ""}).get(), std::runtime_error);

    {
        auto future = holder.AddPostToThread({0, 1, "post1", ""});
        const uint64_t post_id = std::move(future).get();
        ASSERT_EQ(post_id, 3);
    }
    {
        const board::DataHolder::PostsType posts = holder.FetchThreadPosts(1).get();
        ASSERT_EQ(posts.size(), 2);
        EXPECT_EQ(posts[0].post_id, 1);
        EXPECT_EQ(posts[0].thread_id, 1);
        EXPECT_EQ(posts[0].text, "hello world");
        EXPECT_EQ(posts[0].image, "image.png");
        EXPECT_EQ(posts[1].post_id, 3);
        EXPECT_EQ(posts[1].thread_id, 1);
        EXPECT_EQ(posts[1].text, "post1");
        EXPECT_EQ(posts[1].image, "");
    }

    for (const int i : boost::irange(0, 1000)) {
        auto future = holder.AddPostToThread({0, 1, "post", "image"});
        const uint64_t post_id = std::move(future).get();
        ASSERT_EQ(post_id, i + 4);
    }
    {
        auto future = holder.AddPostToThread({0, 2, "post2", "image2"});
        const uint64_t post_id = std::move(future).get();
        ASSERT_EQ(post_id, 1004);
    }
    {
        const board::DataHolder::PostsType posts = holder.FetchThreadPosts(1).get();
        ASSERT_EQ(posts.size(), 1002);
        EXPECT_EQ(posts[0].post_id, 1);
        EXPECT_EQ(posts[0].thread_id, 1);
        EXPECT_EQ(posts[0].text, "hello world");
        EXPECT_EQ(posts[0].image, "image.png");
        EXPECT_EQ(posts[1].post_id, 3);
        EXPECT_EQ(posts[1].thread_id, 1);
        EXPECT_EQ(posts[1].text, "post1");
        EXPECT_EQ(posts[1].image, "");
        for (const int i : boost::irange(3, 1002)) {
            EXPECT_EQ(posts[i].post_id, i + 2);
            EXPECT_EQ(posts[i].thread_id, 1);
            EXPECT_EQ(posts[i].text, "post");
            EXPECT_EQ(posts[i].image, "image");
        }
    }
    {
        const board::DataHolder::PostsType posts = holder.FetchThreadPosts(2).get();
        ASSERT_EQ(posts.size(), 2);
        EXPECT_EQ(posts[0].post_id, 2);
        EXPECT_EQ(posts[0].thread_id, 2);
        EXPECT_EQ(posts[0].text, "another thread");
        EXPECT_EQ(posts[0].image, "");
        EXPECT_EQ(posts[1].post_id, 1004);
        EXPECT_EQ(posts[1].thread_id, 2);
        EXPECT_EQ(posts[1].text, "post2");
        EXPECT_EQ(posts[1].image, "image2");
    }
}
