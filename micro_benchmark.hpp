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
    addon::performance_class clock2; // use another clock in order not to create conflicts with addon::clock
    class mb_timer_class {
    public:
        mb_timer_class() {
            
        }
        void start(std::string const & key) {
            double cyc = addon::clock2.cycles(false);
            addon::clock2.start();
            addon::clock2.stop();
            cyc -= addon::clock2.cycles(false);
            
            for(auto const & k : stack_) {
                std::get<0>(tel_.at(k)) += cyc;
            }
            tel_[key];
            if(std::get<2>(tel_.at(key)) == "") {
                if(stack_.size() == 0) {
                    std::get<2>(tel_.at(key)) = "super_secret_root_name";
                }
                else
                    std::get<2>(tel_.at(key)) = stack_.back();
            }
                
            stack_.push_back(key);
        }
        void stop() {
            double cyc = addon::clock2.cycles(false);
            addon::clock2.start();
            addon::clock2.stop();
            cyc -= addon::clock2.cycles(false);
            
            std::string key = stack_.back();
            for(auto const & k : stack_)
                std::get<0>(tel_.at(k)) += cyc;
            
            std::get<1>(tel_.at(key)) << std::get<0>(tel_.at(key));
            std::get<0>(tel_.at(key)) = 0;
            stack_.pop_back();
        }
        void print() {
            std::set<std::string> parents;
            
            std::cout << GREENB_ << "Microbenchmarks:" << NONE_ << std::endl;
            for(auto const & p : tel_) {
                std::cout << YELLOW_ << std::setw(12) << p.first << NONE_ << " used " << YELLOWB_ << std::get<1>(p.second) << NONE_ << " cycles" << std::endl;
                parents.insert(std::get<2>(p.second));
            }
            
            for(auto const & p : parents) {
                double time = 0;
                if(p == "super_secret_root_name") {
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
                        std::cout << YELLOW_ << std::setw(12) << t.first << NONE_ << " uses " << YELLOWB_ <<int((std::get<1>(tel_.at(t.first)).mean() * std::get<1>(tel_.at(t.first)).count() / time) * 100) << "%" << NONE_ << std::endl;
                }
            }
            
        }
        void clear() {
            tel_.clear();
            stack_.clear();
        }
        std::map<std::string, std::pair<int, double>> get(std::string const & p) const {
            std::map<std::string, std::pair<int, double>> res;
            
            double time = 0;
            if(p == "super_secret_root_name") {
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
        std::map<std::string, std::tuple<double, accumulator<int64_t>, std::string>> tel_;
        std::vector<std::string> stack_;
    } mb_timer;
}//end namespace addon

#ifdef ENABLE_MICRO
    #define START_MICRO(name)           \
    addon::clock2.stop();               \
    addon::mb_timer.start(name);        \
    addon::clock2.start();              //

    #define STOP_MICRO()                \
    addon::clock2.stop();               \
    addon::mb_timer.stop();             \
    addon::clock2.start();              //

    #define NEXT_MICRO(name)            \
    STOP_MICRO()                        \
    addon::clock2.stop();               \
    addon::mb_timer.start(name);        \
    addon::clock2.start();              //
#else
    #define START_MICRO(name) ;
    #define STOP_MICRO() ;
    #define NEXT_MICRO(name) ;
#endif

#define P_MICRO()                   \
addon::mb_timer.print();            //

#define CLEAR_MICRO()               \
addon::mb_timer.clear();            //

#define RES_MICRO(parent) addon::mb_timer.get(parent)//

#endif //ADDON_MICRO_BENCHMARK_HEADER
