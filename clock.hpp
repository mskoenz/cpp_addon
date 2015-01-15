// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    13.01.2015 08:21:14 CET
// File:    clock.hpp

#ifndef ADDON_CLOCK_HEADER
#define ADDON_CLOCK_HEADER

#include "color.hpp"

#include <ctime>
#include <chrono>
#include <thread>
#include <iostream>

namespace addon {
    namespace detail {
        using chrono_clock = std::chrono::high_resolution_clock;
        
        class detail_clock_class {
        public:
            static double sec() {
                using namespace std::chrono;
                duration<double, std::ratio<1, 1 >> res = duration_cast<duration<double >> (chrono_clock::now() - start_time_);
                return res.count();
            }
            static double msec() {
                using namespace std::chrono;
                duration<double, std::ratio<1, 1000 >> res = duration_cast<duration<double >> (chrono_clock::now() - start_time_);
                return res.count();
            }
            static double usec() {
                using namespace std::chrono;
                duration<double, std::ratio<1, 1000000 >> res = duration_cast<duration<double >> (chrono_clock::now() - start_time_);
                return res.count();
            }
            static std::string time(bool const & abstime = true) {
                return format("%H:%M:%S", abstime);
            }
            static std::string date() {
                return format("%d.%m.%y");
            }
            static std::string date_time(std::string const & seperator = "---") {
                return format("%d.%m.%y" + seperator + "%H:%M");
            }
            static std::string format(std::string const & format, bool const & abstime = true) {
                chrono_clock::time_point t = chrono_clock::now();
                
                if(!abstime)
                    t = chrono_clock::time_point(t - start_time_);
                    
                std::time_t tt = chrono_clock::to_time_t(t);
                
                struct tm * timeinfo;
                
                if(abstime) {
                    timeinfo = localtime( & tt);
                } else {
                    timeinfo = gmtime( & tt);
                }
                
                char buffer[100];
                strftime(buffer, 100, format.c_str(), timeinfo);
                
                return buffer;
                
                // better version / not jet supported (g++-4.9)
                //~ std::stringstream ss;
                //~ ss << std::put_time(timeinfo, format);
                //~ return ss.str();
            }
            static void sleep(double const & sec) {
                std::this_thread::sleep_for(std::chrono::milliseconds(uint(1000 * sec)));
            }
            static void print() {
                std::cout << GREEN_ << "Date/Time: " << GREENB_ << date_time("---") << NONE_ << std::endl;
                std::cout << GREEN_ << "Runtime:   " << GREENB_ << time(false) << NONE_ << std::endl;
            }
        private:
            static chrono_clock::time_point start_time_;
        };
        chrono_clock::time_point detail_clock_class::start_time_ = chrono_clock::now();
    }// end namespace detail
    
    using clock = detail::detail_clock_class;
    
}// end namespace addon

#endif // ADDON_CLOCK_HEADER
