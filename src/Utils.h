#pragma once

#include <experimental/optional>
#include <experimental/string_view>

#include <boost/algorithm/string.hpp>

#include <folly/Conv.h>

namespace utils {

template<class T>
using Optional = std::experimental::optional<T>;

using string_view = std::experimental::string_view;

inline utils::Optional<uint64_t> extractThreadNumber(string_view url, string_view prefix) {
    if (!boost::starts_with(url, prefix)) {
        return {};
    }
    const size_t start = prefix.size();
    const size_t end = url.size();

    if (end <= start) {
        return {};
    }

    string_view after_perfix(&url[start], end - start);
    auto maybe_number = folly::tryTo<uint64_t>(after_perfix);
    if (!maybe_number) {
        return {};
    }
    const uint64_t number = maybe_number.value();
    return number;
}

} // namespace utils
