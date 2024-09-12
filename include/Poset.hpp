#pragma once
#include <iostream>
#include <utility> // for std::pair


// General template for filtration type(Poset) hash functor
template <typename T>
struct FTHash {
    std::size_t operator()(const T& t) const {
        return std::hash<T>{}(t);
    }
};


class R2 {
public:
    double x, y;
    // static const double CoordinateMax;
    using CoordinateTP = double; // todo: check double float with python
    // Constructors
    R2() : x(0.0), y(0.0) {}
    R2(double x, double y) : x(x), y(y) {}

    // Accessor methods
    double getX() const { return x; }
    double getY() const { return y; }

    // Overload the index operator
    int operator[](int index) const {
        switch (index) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Index out of range for R2");
        }
    }

    // Comparison operators
    bool operator<(const R2& other) const {
        // Compare first by x, then by y if x's are equal
        return x <= other.x && y <= other.y && !(x == other.x && y == other.y);
    }

    bool operator>(const R2& other) const {
        return other < *this;
    }

    bool operator==(const R2& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const R2& other) const {
        return !(*this == other);
    }

    bool operator<=(const R2& other) const {
        return *this < other || *this == other;
    }

    // Output operator
    friend std::ostream& operator<<(std::ostream& os, const R2& coord) {
        os << "(" << coord.x << ", " << coord.y << ")";
        return os;
    }
    
};


// // Definition and initialization of the static constant
// const double R2::CoordinateMax = std::numeric_limits<double>::max();
// const double R2::CoordinateMax = std::numeric_limits<double>::infinity();


// Specialization of MyHash for R2
template <>
struct FTHash<R2> {
    std::size_t operator()(const R2& r2) const {
        std::size_t hx = std::hash<double>{}(r2.getX());
        std::size_t hy = std::hash<double>{}(r2.getY());
        return hx ^ (hy << 1); // Combine the two hash values
    }
};


struct LexicographicalCompareR2 {
    bool operator()(const R2& lhs, const R2& rhs) const {
        if (lhs.getX() < rhs.getX()) return true;
        if (lhs.getX() == rhs.getX() && lhs.getY() < rhs.getY()) return true;
        return false;
    }
};


// int main() {
//     R2 c1(1.0, 2.0);
//     R2 c2(1.0, 3.0);
//     R2 c3(2.0, 1.0);

//     std::cout << "R2 c1: " << c1 << std::endl;
//     std::cout << "R2 c2: " << c2 << std::endl;
//     std::cout << "R2 c3: " << c3 << std::endl;

//     std::cout << "c1 < c2: " << (c1 < c2) << std::endl;
//     std::cout << "c2 < c3: " << (c2 < c3) << std::endl;
//     std::cout << "c1 == c2: " << (c1 == c2) << std::endl;

//     std::cout << "Let's check harder things" << std::endl;
//     std::cout << "c1 < c3: " << (c1 < c3) << std::endl;
//     std::cout << "c1 > c3: " << (c1 > c3) << std::endl;
//     return 0;
// }
