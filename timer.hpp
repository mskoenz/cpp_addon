// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    12.01.2015 08:46:40 CET
// File:    timer.hpp

#ifndef ADDON_TIMER_HEADER
#define ADDON_TIMER_HEADER

#include <chrono>
#include <iostream>

#include "debug.hpp"
#include "color.hpp"
#include "clock.hpp"

namespace addon {
    namespace detail {
        struct tsc_counter {
            double cycle() const {
                return (uint64_t(hi) << 32) + lo;
            };
            void clear() {
                hi = 0;
                lo = 0;
            }
            uint32_t hi;
            uint32_t lo;
        };

        double operator-(tsc_counter const & end, tsc_counter const & begin) {
            return end.cycle() - begin.cycle();
        }
        
    }// end namespace detail
    
    class timer_class {
    public:
        timer_class(): min_cycles_(1e9) {
            clear();
        }
        //--------------------------- methods ------------------------------------------------------
        void start() {
            start_time_ = clock::usec();
            __asm __volatile__ ("cpuid" : : "a" (0) : "bx", "cx", "dx" ); // CPUID
            __asm__ __volatile__ ("rdtsc" : "=a" ((start_cyc_).lo), "=d"((start_cyc_).hi)); // RDTSC
        }
        void stop() {
            __asm__ __volatile__ ("rdtsc" : "=a" ((end_cyc_).lo), "=d"((end_cyc_).hi)); // RDTSC
            __asm__ __volatile__ ("cpuid" : : "a" (0) : "bx", "cx", "dx" ); // CPUID
            end_time_ = clock::usec();
        }
        void clear() {
            start_cyc_.clear();
            end_cyc_.clear();
            start_time_ = clock::usec();
            end_time_ = clock::usec();
        }
        //------------------------- const methods --------------------------------------------------
        template<typename S>
        void print(S & os) const {
            os << GREEN_ << "cycles measured with rdtsc:      " << GREENB_ << cycles() << " cycles" << NONE_ << std::endl;
            os << GREEN_ << "time measured with hi res clock: " << GREENB_ << sec() << " seconds" << NONE_ << std::endl;
        }
        
        double cycles(bool const & check_min = true) const {
            auto cyc = end_cyc_ - start_cyc_;
            if(check_min and (cyc < min_cycles_))
                WARNING("measure more cycles (>1e9)");
            return end_cyc_ - start_cyc_;
        }
        double sec() const {
            return (end_time_ - start_time_) / 1000000.;
        }
        double msec() const {
            return (end_time_ - start_time_) / 1000.;
        }
        double usec() const {
            return (end_time_ - start_time_);
        }
        double elapsed_sec() const {
            return (clock::usec() - start_time_) / 1000000.;
        }
        double elapsed_msec() const {
            return (clock::usec() - start_time_) / 1000.;
        }
        double elapsed_usec() const {
            return (clock::usec() - start_time_);
        }
        double const & min_cycles() const {
            return min_cycles_;
        }
    private:
        detail::tsc_counter start_cyc_, end_cyc_;
        double start_time_, end_time_;
        double const min_cycles_;
    };
    
    //------------------------- stream operator ----------------------------------------------------
    std::ostream & operator<<(std::ostream & os, timer_class const & arg) {
        arg.print(os);
        return os;
    }
}// end namespace addon

#endif // ADDON_TIMER_HEADER
