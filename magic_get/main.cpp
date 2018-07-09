/*
c++: 17
dependencies:
    - pvt.cppan.demo.magic_get-master
*/

#include <iostream>
#include <string>

#define BOOST_PFR_USE_CPP17 1
#include "boost/pfr/precise.hpp"

struct some_person {
    std::string name;
    unsigned birth_year;
};

int main() {
    some_person val{"Edgar Allan Poe", 1809};

    std::cout << boost::pfr::get<0>(val)                // No macro!
        << " was born in " << boost::pfr::get<1>(val);  // Works with any aggregate initializables!
}
