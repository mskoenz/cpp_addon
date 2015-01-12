// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    24.03.2014 22:12:10 CET
// File:    performance.hpp

#ifndef __PERFORMACE_HEADER
#define __PERFORMACE_HEADER

#include <chrono>
#include <iostream>

#include "debug.hpp"
#include "color.hpp"

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
        
    }//end namespace detail
    
    
    class performance_class {
        using chrono_clock = std::chrono::high_resolution_clock;
    public:
        performance_class(): min_cycles_(1e9) {
            clear();
        }
        //------------------- methods -------------------
        void start() {
            start_time_ = chrono_clock::now();
            __asm __volatile__ ("cpuid" : : "a" (0) : "bx", "cx", "dx" ); //CPUID
            __asm__ __volatile__ ("rdtsc" : "=a" ((start_cyc_).lo), "=d"((start_cyc_).hi)); //RDTSC
        }
        void stop() {
            __asm__ __volatile__ ("rdtsc" : "=a" ((end_cyc_).lo), "=d"((end_cyc_).hi)); //RDTSC
            __asm__ __volatile__ ("cpuid" : : "a" (0) : "bx", "cx", "dx" ); //CPUID
            end_time_ = chrono_clock::now();
        }
        void clear() {
            start_cyc_.clear();
            end_cyc_.clear();
            start_time_ = chrono_clock::now();
            end_time_ = chrono_clock::now();
        }
        //------------------- const methods -------------------
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
            using namespace std::chrono;
            duration<double, std::ratio<1, 1>> res = duration_cast<duration<double>>(end_time_ - start_time_);
            return res.count();
        }
        double msec() const {
            using namespace std::chrono;
            duration<double, std::ratio<1, 1000>> res = duration_cast<duration<double>>(end_time_ - start_time_);
            return res.count();
        }
        double usec() const {
            using namespace std::chrono;
            duration<double, std::ratio<1, 1000000>> res = duration_cast<duration<double>>(end_time_ - start_time_);
            return res.count();
        }
        double const & min_cycles() const {
            return min_cycles_;
        }
    private:
        detail::tsc_counter start_cyc_, end_cyc_;
        chrono_clock::time_point start_time_, end_time_;
        double const min_cycles_;
    } clock; //<- one instance named clock
    //-------------- stream operator (only works for std::cout...) --------------
    std::ostream & operator<<(std::ostream & os, performance_class const & arg) {
        arg.print(os);
        return os;
    }
}//end namespace addon

#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>

namespace addon {
    namespace detail {
        double num_runs;
        std::map<std::string, double> perf_measure;
        
        void print_results() {
            for(auto & p : perf_measure) {
                //~ std::cout << GREENB << std::setw(7) << std::lround(p.second) << NONE << " --> "
                std::cout << GREENB_ << std::setw(7) << p.second << NONE_ << " --> "
                             GREEN_ << p.first << NONE_ << std::endl;
            }
        }
        void print_speedup() {
            double max = 0;
            using p_type = std::pair<std::string, double>;
            std::vector<p_type> temp;
            
            for(auto & p : perf_measure) {
                temp.push_back(p);
                if(p.second > max)
                    max = p.second;
            }
            
            std::sort (temp.begin(), temp.end(), [](p_type const & p1, p_type const & p2) {return p1.second < p2.second;});
            
            for(auto & p : temp) {
                std::cout << REDB_ << std::setw(7) << std::setprecision (2) << std::fixed << max / p.second << NONE_ << " --> " << RED_ << p.first << NONE_ << std::endl;
            }
        }
    }//end namespace detail
    
}//end namespace addon

#define M_RESULTS addon::detail::perf_measure
#define P_RESULTS() addon::detail::print_results();
#define P_SPEEDUP() addon::detail::print_speedup();
#define CLEAR_MEASURE() addon::detail::perf_measure.clear();

#define CALIBRATE(expr)             \
addon::detail::num_runs = 0.5;      \
addon::clock.clear();               \
while(addon::clock.cycles(false) < addon::clock.min_cycles()) {  \
    addon::detail::num_runs *= 2;   \
    addon::clock.start();           \
    for(uint64_t i = 0; i < addon::detail::num_runs; ++i) {  \
        expr;                       \
    }                               \
    addon::clock.stop();            \
}                                   //


#define MEASURE(expr, name)         \
CALIBRATE(expr)                     \
addon::detail::perf_measure[name] = addon::clock.cycles(false) / addon::detail::num_runs;  \
                                    //

#define MEASURE_DIV(expr, name, div)    \
CALIBRATE(expr)                     \
addon::detail::perf_measure[name] = addon::clock.cycles(false) / (addon::detail::num_runs * div);  \
                                    //

#endif //__PERFORMACE_HEADER
