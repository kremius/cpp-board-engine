#include "Utils.h"

#include <gtest/gtest.h>

TEST(Utils, ExtractThreadNumber)
{
    using result_type = utils::optional<uint64_t>;

    EXPECT_EQ(utils::extractThreadNumber("prefix100", "wrong_prefix"), result_type{});
    EXPECT_EQ(utils::extractThreadNumber("prefix100", "prefix"), result_type{100});
    EXPECT_EQ(utils::extractThreadNumber("prefix-1", "prefix"), result_type{});
    EXPECT_EQ(utils::extractThreadNumber("prefix-1", "prefix-"), result_type{1});
    EXPECT_EQ(utils::extractThreadNumber("prefixnotanumber", "prefix"), result_type{});
    EXPECT_EQ(utils::extractThreadNumber("prefix10", "veryveryverylongprefix"), result_type{});
    EXPECT_EQ(utils::extractThreadNumber("prefix", "prefix"), result_type{});
    EXPECT_EQ(utils::extractThreadNumber("prefix", "prefixlong"), result_type{});
    EXPECT_EQ(utils::extractThreadNumber("prefix0", "prefix"), result_type{0});
    EXPECT_EQ(
        utils::extractThreadNumber("prefix18446744073709551615", "prefix"),
        result_type{18446744073709551615UL});
    EXPECT_EQ(
        utils::extractThreadNumber("prefix18446744073709551616", "prefix"),
        result_type{});
}
