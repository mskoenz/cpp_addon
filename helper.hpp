// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    06.01.2015 16:10:44 CET
// File:    helper.hpp

#ifndef ADDON_HELPER_HEADER
#define ADDON_HELPER_HEADER

#include "color.hpp"
#include "debug.hpp"

#include <iomanip>
#include <sstream>
#include <assert.h>
#include <iostream>
#include <stdexcept>

#include <unistd.h>

namespace addon {
    std::string sec_to_time(uint64_t const & sec) {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << (sec)/3600
                         << ":" << std::setw(2) << (sec/60)%60
                         << ":" << std::setw(2) << (sec)%60;
        return ss.str();
    }
    
    std::string cwd() {
        char * path = getcwd(NULL, 0);
        std::string cwd = path;
        return cwd;
    }
    
    std::string abspath(char* argv[]) {
        std::string dir(argv[0]);
        auto pos = dir.rfind("/");
        dir.erase(pos, dir.size() - pos); // erase progname in ./folder/progname
        
        std::string cwd_ = cwd();
        
        if(dir[0] == '/') // full specified path
            return dir;
        else
            dir.erase(0, 1); // erase . in ./folder
            return cwd_ + dir;
    }
    
    std::string progress_bar(double const & p) {
        std::stringstream ss;
        int const size = 50;
        int bars = p * size;
        
        if(p < .33)
            ss << RED_;
        else if(p < .66)
            ss << YELLOW_;
        else if(p >= 1)
            ss << CYAN_;
        else
            ss << GREEN_;
        
        ss << "<" << std::setfill('|') << std::setw(bars) << "" << std::setfill(' ') 
                  << std::setw(size - bars) << "" << ">" << NONE_ 
                  << std::setw(3) << int(p * 100) << "% " << std::endl;
        
        return ss.str();
    }
    
}//end namespace addon

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
