#pragma once
#include <type_traits>
#include <cmath>

/*
Fuzzy comparison 
usage: 

clean way:
std::set<double, Compare<double>> // double with fuzzy comparison
std::set<int, Compare<int>> // int with exact comparison

You can also specify whether to use fuzzy comparison explicitly:
std::set<double, Compare<double, true>> // double with fuzzy comparison
std::set<double, Compare<double, false>> // double with exact comparison
std::set<int, Compare<int, false>> // int with exact comparison
*/
template<typename T>
bool almost_equal(const T a, const T b, const T epsilon = std::pow(2, -30))
{
    T diff = std::abs(a - b);
    if (diff <= epsilon)
        return true;

    if (diff <= (std::abs(a) + std::abs(b)) * epsilon)
        return true;
    return false;
}

// Exact comparison
template<typename T>
struct ExactCompare {
    bool operator()(const T a, const T b) const {
        return a < b;
    }
};

// Specialization for floating-point types
template<typename T>
struct FuzzyCompare {
    bool operator()(const T a, const T b) const {
        if (almost_equal<T>(a, b))
            return false;  // if they're almost equal, they're equal
        return a < b;     // otherwise use normal ordering
    }
};

// Primary Template
template<typename T, bool UseFuzzy = std::is_floating_point<T>::value>
struct Compare : ExactCompare<T> {};

// Partial Specialization 
template<typename T>
struct Compare<T, true> : FuzzyCompare<T> {};