// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    06.01.2015 16:10:44 CET
// File:    helper.hpp

#ifndef ADDON_HELPER_HEADER
#define ADDON_HELPER_HEADER

#include "color.hpp"

#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <sstream>

void ERROR(std::string const & text) {
    std::stringstream ss;
    ss << REDB_ << "error: " << RED_ << text << NONE_;
    throw std::runtime_error(ss.str());
}

void WARNING(std::string const & text) {
    std::cout << YELLOWB_ << "warning: " << YELLOW_ << text << NONE_ << std::endl;
}


void ASSERT(bool const & cond, std::string const & text = "") {
    #ifndef NDEBUG
        if(!cond) {
            std::cout << REDB_ << "assert failed: " << RED_ << text << NONE_ << std::endl;
            assert(cond);
        }
    #endif // NDEBUG
}

#endif // ADDON_HELPER_HEADER
