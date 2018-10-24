#pragma once

#include <experimental/optional>

namespace utils {

template<class T>
using Optional = std::experimental::optional<T>;

} // namespace utils
