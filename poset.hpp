#include <iostream>
#include <utility> // for std::pair

class Coordinate {
public:
    // Constructors
    Coordinate() : x(0.0), y(0.0) {}
    Coordinate(double x, double y) : x(x), y(y) {}

    // Accessor methods
    double getX() const { return x; }
    double getY() const { return y; }

    // Comparison operators
    bool operator<(const Coordinate& other) const {
        // Compare first by x, then by y if x's are equal
        return x <= other.x && y <= other.y;
    }

    bool operator>(const Coordinate& other) const {
        return other < *this;
    }

    bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Coordinate& other) const {
        return !(*this == other);
    }

    bool operator<=(const Coordinate& other) const {
        return *this < other || *this == other;
    }

    // Output operator
    friend std::ostream& operator<<(std::ostream& os, const Coordinate& coord) {
        os << "(" << coord.x << ", " << coord.y << ")";
        return os;
    }

private:
    double x, y;
};

// int main() {
//     Coordinate c1(1.0, 2.0);
//     Coordinate c2(1.0, 3.0);
//     Coordinate c3(2.0, 1.0);

//     std::cout << "Coordinate c1: " << c1 << std::endl;
//     std::cout << "Coordinate c2: " << c2 << std::endl;
//     std::cout << "Coordinate c3: " << c3 << std::endl;

//     std::cout << "c1 < c2: " << (c1 < c2) << std::endl;
//     std::cout << "c2 < c3: " << (c2 < c3) << std::endl;
//     std::cout << "c1 == c2: " << (c1 == c2) << std::endl;

//     std::cout << "Let's check harder things" << std::endl;
//     std::cout << "c1 < c3: " << (c1 < c3) << std::endl;
//     std::cout << "c1 > c3: " << (c1 > c3) << std::endl;
//     return 0;
// }
