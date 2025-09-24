#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>
/*
The following code is modified from RIVET to support rational numbers. 
It is used to handle floating point precision and draw lines and projection on lines.
It is not very nessary for this project, but we keep here for comparison.  
*/
namespace rivet {

    // Rivet exact Type using rational numbers from boost::multiprecision
    using exact = boost::multiprecision::cpp_rational;
    
    // Output stream operator for exact type
    inline std::ostream& operator<<(std::ostream& os, const exact& r) {
        os << r.str();  // Convert to string and output
        return os;
    }
    
    // Forward declare ExactValue
    struct ExactValue;
    
    // Output stream operator for ExactValue
    inline std::ostream& operator<<(std::ostream& os, const ExactValue& ev);
    
     // approximate a double to an exact type
     exact approx(double x)
     {
         if (x == 0.0) {
             return exact(0);
         }
         
         int d = 7; //desired number of significant digits
         int log = (int)floor(log10(x)) + 1;
     
         if (log >= d)
             return exact((int)floor(x));
     
         long denom = pow(10, d - log);
         return exact((long)floor(x * denom), denom);
     }
    
    // exact value struct
    struct ExactValue {
        double double_value;
        exact exact_value;
    
        mutable std::vector<unsigned> indexes; //indexes of points corresponding to this value (e.g. points whose birth time is this value)
    
        static double epsilon;

         ExactValue(exact e)
             : exact_value(e)
         {
             if (denominator(e) == 0) {
                 double_value = 0.0;
             } else {
                 double_value = numerator(e).convert_to<double>() / denominator(e).convert_to<double>(); //can aos use static_cast in C++11
             }
         }
    
        bool operator<=(const ExactValue& other) const
        {
            //if the two double values are nearly equal, then compare exact values
            if (almost_equal(double_value, other.double_value))
                return exact_value <= other.exact_value;
    
            //otherwise, compare double values
            return double_value <= other.double_value;
        }
    
        static bool almost_equal(const double a, const double b)
        {
            double diff = std::abs(a - b);
            if (diff <= epsilon)
                return true;
    
            if (diff <= (std::abs(a) + std::abs(b)) * epsilon)
                return true;
            return false;
        }
    };
    
    // set the epsilon for the exact type
    double ExactValue::epsilon = std::pow(2, -30);
    
    //comparator for ExactValue pointers
    struct ExactValueComparator {
        bool operator()(const ExactValue& lhs, const ExactValue& rhs) const
        {
            //if the two double values are nearly equal, then compare exact values
            if (ExactValue::almost_equal(lhs.double_value, rhs.double_value))
                return lhs.exact_value < rhs.exact_value;
    
            //otherwise, compare double values
            return lhs.double_value < rhs.double_value;
        }
    };
    
    //ExactSet will help sort grades
    typedef std::set<ExactValue, ExactValueComparator> ExactSet;
    
    // Define the output operator after ExactValue is fully defined
    inline std::ostream& operator<<(std::ostream& os, const ExactValue& ev) {
        os << ev.exact_value;  // Use the exact value's output operator
        return os;
    }
    
    } // namespace rivet