// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    08.01.2015 10:39:00 CET
// File:    micro_benchmark.hpp

#ifndef ADDON_MICRO_BENCHMARK_HEADER
#define ADDON_MICRO_BENCHMARK_HEADER

#include "performance.hpp"
#include "accumulator.hpp"

#include <map>
#include <vector>
#include <set>
#include <tuple>
#include <iomanip>
#include <utility>

namespace addon {
    addon::timer_class timer2;
    class mb {
    public:
        static void start(std::string const & key) {
            double cyc = addon::timer2.cycles(false);
            addon::timer2.start();
            addon::timer2.stop();
            cyc -= addon::timer2.cycles(false);
            
            for(auto const & k : stack_) {
                std::get<0>(tel_.at(k)) += cyc;
            }
            tel_[key];
            if(std::get<2>(tel_.at(key)) == "") {
                if(stack_.size() == 0) {
                    std::get<2>(tel_.at(key)) = "root";
                }
                else
                    std::get<2>(tel_.at(key)) = stack_.back();
            }
                
            stack_.push_back(key);
        }
        static void stop() {
            double cyc = addon::timer2.cycles(false);
            addon::timer2.start();
            addon::timer2.stop();
            cyc -= addon::timer2.cycles(false);
            
            std::string key = stack_.back();
            for(auto const & k : stack_)
                std::get<0>(tel_.at(k)) += cyc;
            
            std::get<1>(tel_.at(key)) << std::get<0>(tel_.at(key));
            std::get<0>(tel_.at(key)) = 0;
            stack_.pop_back();
        }
        static void results() {
            std::set<std::string> parents;
            
            std::cout << GREENB_ << "Microbenchmarks:" << NONE_ << std::endl;
            for(auto const & p : tel_) {
                std::cout << YELLOW_ << std::setw(12) << p.first << NONE_ << " used " << YELLOWB_ << std::get<1>(p.second) << NONE_ << " cycles" << std::endl;
                parents.insert(std::get<2>(p.second));
            }
            
            for(auto const & p : parents) {
                double time = 0;
                if(p == "root") {
                    std::cout << GREENB_ << "Performance for all:" << NONE_ << std::endl;
                    for(auto const & t : tel_) {
                    if(std::get<2>(t.second) == p)
                        time += std::get<1>(t.second).mean() * std::get<1>(t.second).count();
                    }
                }
                else {
                    std::cout << GREENB_ << "Performance for " << p << ":" << NONE_ << std::endl;
                    time = std::get<1>(tel_.at(p)).mean() * std::get<1>(tel_.at(p)).count();
                }
                     
                for(auto const & t : tel_) {
                    if(std::get<2>(t.second) == p)
                        std::cout << YELLOW_ << std::setw(12) << t.first << NONE_ << " uses " << YELLOWB_ << int((std::get<1>(tel_.at(t.first)).mean() * std::get<1>(tel_.at(t.first)).count() / time) * 100) << "%" << NONE_ << std::endl;
                }
            }
            
        }
        static void clear() {
            tel_.clear();
            stack_.clear();
        }
        static std::map<std::string, std::pair<double, double >> get(std::string const & p) {
            std::map<std::string, std::pair<double, double >> res;
            
            double time = 0;
            if(p == "root") {
                for(auto const & t : tel_) {
                if(std::get<2>(t.second) == p)
                    time += std::get<1>(t.second).mean() * std::get<1>(t.second).count();
                }
            }
            else {
                time = std::get<1>(tel_.at(p)).mean() * std::get<1>(tel_.at(p)).count();
            }
                 
            for(auto const & t : tel_) {
                if(std::get<2>(t.second) == p)
                    res[t.first] = std::make_pair((std::get<1>(tel_.at(t.first)).mean() * std::get<1>(tel_.at(t.first)).count() / time) * 100, std::get<1>(tel_.at(t.first)).mean());
            }
            return res;
        }
    private:
        static std::map<std::string, std::tuple<double, accumulator<int64_t>, std::string >> tel_;
        static std::vector<std::string> stack_;
    };
    std::map<std::string, std::tuple<double, accumulator<int64_t>, std::string >> mb::tel_;
    std::vector<std::string> mb::stack_;
}// end namespace addon

#ifdef ENABLE_MICRO
    #define START_MICRO(name)           \
    addon::timer2.stop();               \
    addon::mb::start(name);             \
    addon::timer2.start();              // 

    #define STOP_MICRO()                \
    addon::timer2.stop();               \
    addon::mb::stop();                  \
    addon::timer2.start();              // 

    #define NEXT_MICRO(name)            \
    STOP_MICRO()                        \
    addon::timer2.stop();               \
    addon::mb::start(name);             \
    addon::timer2.start();              // 
#else
    #define START_MICRO(name) ;
    #define STOP_MICRO() ;
    #define NEXT_MICRO(name) ;
#endif

#endif // ADDON_MICRO_BENCHMARK_HEADER
