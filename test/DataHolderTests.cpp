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
