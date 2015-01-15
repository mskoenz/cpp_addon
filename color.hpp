// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.12.2014 11:08:14 CET
// File:    color.hpp

#ifndef ADDON_COLOR_HEADER
#define ADDON_COLOR_HEADER

#include <iostream>

#ifndef ADDON_NO_COLOR

#define BLACK_ "\033[0;30m"
#define BLACKB_ "\033[1;30m"
#define RED_ "\033[0;31m"
#define REDB_ "\033[1;31m"
#define GREEN_ "\033[0;32m"
#define GREENB_ "\033[1;32m"
#define YELLOW_ "\033[0;33m"
#define YELLOWB_ "\033[1;33m"
#define BLUE_ "\033[0;34m"
#define BLUEB_ "\033[1;34m"
#define MAGENTA_ "\033[0;35m"
#define MAGENTAB_ "\033[1;35m"
#define CYAN_ "\033[0;36m"
#define CYANB_ "\033[1;36m"
#define WHITE_ "\033[0;37m"
#define WHITEB_ "\033[1;37m"

#define BLACKBG_ "\033[0;40m"
#define REDBG_ "\033[0;41m"
#define GREENBG_ "\033[0;42m"
#define YELLOWBG_ "\033[0;43m"
#define BLUEBG_ "\033[0;44m"
#define MAGENTABG_ "\033[0;45m"
#define CYANBG_ "\033[0;46m"
#define WHITEBG_ "\033[0;47m"

#define NONE_ "\033[0m" 

//------------------------- special bash prompt ----------------------------------------------------
#define CLRSCR_ "\033[2J\033[100A"
#define RENTER_ "\033[1A\x1B[2K"
#define CLEAR_ "\x1B[2K"
#define UP_ "\033[1A"

#else // ADDON_NO_COLOR

#define BLACK_ ""
#define BLACKB_ ""
#define RED_ ""
#define REDB_ ""
#define GREEN_ ""
#define GREENB_ ""
#define YELLOW_ ""
#define YELLOWB_ ""
#define BLUE_ ""
#define BLUEB_ ""
#define MAGENTA_ ""
#define MAGENTAB_ ""
#define CYAN_ ""
#define CYANB_ ""
#define WHITE_ ""
#define WHITEB_ ""

#define BLACKBG_ ""
#define REDBG_ ""
#define GREENBG_ ""
#define YELLOWBG_ ""
#define BLUEBG_ ""
#define MAGENTABG_ ""
#define CYANBG_ ""
#define WHITEBG_ ""

#define NONE_ ""

//------------------------- special bash prompt ----------------------------------------------------
#define CLRSCR_ ""
#define RENTER_ ""

#endif // ADDON_NO_COLOR

#define GEN_COLOR_FCT(X) template<typename T> void X(T const & t, std::string const & end = "\n") { std::cout << X##_ << t << NONE_ << end;}

GEN_COLOR_FCT(BLACK)
GEN_COLOR_FCT(BLACKB)
GEN_COLOR_FCT(RED)
GEN_COLOR_FCT(REDB)
GEN_COLOR_FCT(GREEN)
GEN_COLOR_FCT(GREENB)
GEN_COLOR_FCT(YELLOW)
GEN_COLOR_FCT(YELLOWB)
GEN_COLOR_FCT(BLUE)
GEN_COLOR_FCT(BLUEB)
GEN_COLOR_FCT(MAGENTA)
GEN_COLOR_FCT(MAGENTAB)
GEN_COLOR_FCT(CYAN)
GEN_COLOR_FCT(CYANB)
GEN_COLOR_FCT(WHITE)
GEN_COLOR_FCT(WHITEB)

GEN_COLOR_FCT(BLACKBG)
GEN_COLOR_FCT(REDBG)
GEN_COLOR_FCT(GREENBG)
GEN_COLOR_FCT(YELLOWBG)
GEN_COLOR_FCT(BLUEBG)
GEN_COLOR_FCT(MAGENTABG)
GEN_COLOR_FCT(CYANBG)
GEN_COLOR_FCT(WHITEBG)

#endif // ADDON_COLOR_HEADER
