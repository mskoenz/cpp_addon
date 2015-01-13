// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    24.03.2014 22:12:10 CET
// File:    performance.hpp

#ifndef ADDON_PERFORMACE_HEADER
#define ADDON_PERFORMACE_HEADER

#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>

#include "timer.hpp"

namespace addon {
    class perf {
    public:
        static std::map<std::string, double> const & data() {
            return perf_measure;
        }
        
        static void results() {
            for(auto p : data()) {
                //~ std::cout << GREENB << std::setw(7) << std::lround(p.second) << NONE << " --> "
                std::cout << GREENB_ << std::setw(7) << p.second << NONE_ << " --> "
                             GREEN_ << p.first << NONE_ << std::endl;
            }
        }
        
        static void speedup() {
            double max = 0;
            using p_type = std::pair<std::string, double>;
            std::vector<p_type> temp;
            
            for(auto p : data()) {
                temp.push_back(p);
                if(p.second > max)
                    max = p.second;
            }
            
            std::sort (temp.begin(), temp.end(), [](p_type const & p1, p_type const & p2) {return p1.second < p2.second;});
            
            for(auto & p : temp) {
                std::cout << REDB_ << std::setw(7) << std::setprecision (2) << std::fixed << max / p.second << NONE_ << " --> " << RED_ << p.first << NONE_ << std::endl;
            }
        }
        
        static void clear() {
            perf_measure.clear();
        }
        
        static timer_class timer;
        static double num_runs;
        static std::map<std::string, double> perf_measure;
    };
    
    timer_class perf::timer;
    double perf::num_runs;
    std::map<std::string, double> perf::perf_measure;
}//end namespace addon

#define CALIBRATE(expr)             \
addon::perf::num_runs = 0.5;        \
addon::perf::timer.clear();         \
while(addon::perf::timer.cycles(false) < addon::perf::timer.min_cycles()) {  \
    addon::perf::num_runs *= 2;     \
    addon::perf::timer.start();     \
    for(uint64_t i = 0; i < addon::perf::num_runs; ++i) {  \
        expr;                       \
    }                               \
    addon::perf::timer.stop();      \
}                                   //


#define MEASURE(expr, name)         \
CALIBRATE(expr)                     \
addon::perf::perf_measure[name] = addon::perf::timer.cycles(false) / addon::perf::num_runs;  \
                                    //
                                    
#define MEASURE_DIV(expr, name, div)\
CALIBRATE(expr)                     \
addon::perf::perf_measure[name] = addon::perf::timer.cycles(false) / (addon::perf::num_runs * div);  \
                                    //

#endif //ADDON_PERFORMACE_HEADER
