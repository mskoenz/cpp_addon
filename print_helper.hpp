// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    26.02.2015 08:38:17 CET
// File:    print_helper.hpp

#ifndef ADDON_PRINT_HELPER_HEADER
#define ADDON_PRINT_HELPER_HEADER

#include <iostream>
#include <vector>
#include <sstream>

template<typename T>
std::string to_str(T const & t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template<typename T>
std::string to_str(std::vector<T> const & t) {
    std::stringstream ss;
    ss << "[";
    
    for(uint i = 0; i < t.size(); ++i) {
        ss << t[i];
        if(i < t.size()-1) {
            ss << ", ";
        }
    }
    ss << "]";
    return ss.str();
}

#endif //ADDON_PRINT_HELPER_HEADER
