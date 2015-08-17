// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    13.01.2015 08:13:28 CET
// File:    progress.hpp

#ifndef ADDON_PROGRESS_HEADER
#define ADDON_PROGRESS_HEADER

#include "color.hpp"
#include "helper.hpp"
#include "parameter.hpp"

#include <iomanip>
#include <iostream>

namespace addon {
    class progress {
    public:
        static void set_load(uint64_t const & in) {
            fname_ = std::string(parameter["wd"] + "/status.txt"); //strange that I have to use cast here bc static...
            load_ = in;
            mod_ = 1;
            idx_ = 0;
            first_idx_ = -1;
            
            timer_.start();
            done_ = false;
            last_print_ = timer_.elapsed_sec();
            launch_time_ = clock::time_since_epoch();
        }
        static void write_state_file() {
            std::stringstream ss;
            ss << "p " << p() << std::endl
               << "eta " << eta_ << std::endl
               << "time " << timer_.elapsed_sec() << std::endl
               << "launch " << launch_time_ << std::endl;
            std::ofstream ofs;
            ofs.open(fname_, std::ios_base::out);
            ofs << ss.str();
            ofs.close();
        }
        static void update(uint64_t const & i, double const & low = 1, double const & high = 3) {
            if((i & (mod_ - 1)) == 0) {
                if(timer_.elapsed_sec() - last_print_ > low) {
                    double ela = timer_.elapsed_sec();
                    if(ela - last_print_ > high and mod_ > 1)
                        mod_ >>= 1;
                    
                    last_print_ = ela;
                    idx_ = i;
                    eta_ = (1 - p()) / p() * ela;
                    
                    print();
                    
                    trigger_ = true;
                }
                else {
                    if(first_idx_ == -1)
                        first_idx_ = i;
                    
                    if(i != first_idx_)
                        mod_ <<= 1;
                }
            }
        }
        static void update() {
            ++idx_;
            update(idx_);
        }
        template<typename F>
        static void trigger(F fct) {
            if(trigger_) {
                trigger_ = false;
                fct();
            }
        }
        static void done() {
            mod_ = 1; //pass first if
            last_print_ -= 3600; //heuristics remove an hour, should be sure to pass second if
            timer_.stop();
            done_ = true;
            
            update(load_); //ifs in this fct
        }
        static inline double p() {
            return (idx_-first_idx_) / (load_ - first_idx_);
        }
        static void print() {
            if(to_file)
                write_state_file();
            if(to_term) {
                if(done_) {
                    std::cout << "Job done in " << sec_to_time(timer_.elapsed_sec()) << "  " << progress_bar(p());
                } else {
                    std::cout << "Job done in " << sec_to_time(eta_) << "  " << progress_bar(p());
                    std::cout << RENTER_;
                }
            }
        }
        static inline double elapsed_sec() {
            return timer_.elapsed_sec();
        }
        static bool to_file;
        static bool to_term;
    private:
        static timer_class timer_;
        static uint64_t load_;
        
        //------------------- modulator -------------------
        static uint64_t mod_;
        static double last_print_;
        static uint64_t launch_time_;
        static double first_idx_;
        static double idx_;
        
        //------------------- timer & file -------------------
        static double eta_;
        static std::string fname_;
        static bool trigger_;
        static bool done_;
    };
    
    bool progress::to_file = false;
    bool progress::to_term = true;
    
    timer_class progress::timer_;
    uint64_t progress::load_;
    uint64_t progress::mod_;
    double progress::last_print_;
    uint64_t progress::launch_time_;
    double progress::first_idx_;
    double progress::idx_;
    double progress::eta_;
    std::string progress::fname_;
    bool progress::trigger_ = false;
    bool progress::done_ = false;
}//end namespace addon
#endif // ADDON_PROGRESS_HEADER
