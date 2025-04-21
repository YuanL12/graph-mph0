#pragma once
#include <functional>

// General template for filtration type(Poset) hash functor
template <typename T>
struct FTHash {
    std::size_t operator()(const T& t) const {
        return std::hash<T>{}(t);
    }
};