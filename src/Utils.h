#pragma once

#include <experimental/optional>
#include <experimental/string_view>

#include <boost/algorithm/string.hpp>

#include <folly/Conv.h>
#include <folly/dynamic.h>
#include <folly/json.h>

namespace utils {

template<class T>
using optional = std::experimental::optional<T>;

using string_view = std::experimental::string_view;

inline utils::optional<uint64_t> extractThreadNumber(string_view url, string_view prefix) {
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

// Optional-style wrapper
inline utils::optional<folly::dynamic> parseJson(folly::StringPiece data) {
    try {
        return folly::parseJson(data);
    } catch (const std::exception&) {
        return {};
    }
}

} // namespace utils
