/*
Test the rivet rational number implementation.
*/

#include "RIVET.hpp"
#include <iostream>

int main() {
    std::cout << "Rivet rational number implementation test" << std::endl;

    int n = 10;
    auto a = rivet::ExactValue(0.0);
    std::cout << a << std::endl;
    return 0;
}