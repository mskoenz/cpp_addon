// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.12.2014 11:19:07 CET
// File:    debug.hpp

#ifndef ADDON_DEBUG_HEADER
#define ADDON_DEBUG_HEADER

#include "color.hpp"

#ifndef ADDON_NO_DEBUG
    #define DEBUG_VAR(X) std::cout << RED_ << "  DEBUG_VAR: " << REDB_ << #X << " = " << X << NONE_ << std::endl;
    #define DEBUG_MSG(X) std::cout << RED_ << "  DEBUG_MSG: " << REDB_                << X << NONE_ << std::endl;
#else
    #define DEBUG_VAR(X) ;
    #define DEBUG_MSG(X) ;
#endif // ADDON_NO_DEBUG
#endif // ADDON_DEBUG_HEADER
