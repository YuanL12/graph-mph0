#include <iostream>
#include <utility> // for std::pair

class R2 {
public:
    // Constructors
    R2() : x(0.0), y(0.0) {}
    R2(double x, double y) : x(x), y(y) {}

    // Accessor methods
    double getX() const { return x; }
    double getY() const { return y; }

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

private:
    double x, y;
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
