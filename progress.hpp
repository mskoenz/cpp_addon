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
            fname_ = std::string(parameter["prog_dir"] + "/status.txt"); //strange that I have to use cast here bc static...
            load_ = in;
            timer_.start();
            mod_ = 1;
            first_idx_ = -1;
            last_print_ = timer_.elapsed_sec();
        }
        static void write_state_file() {
            std::stringstream ss;
            ss << "p " << p() << std::endl
               << "eta " << eta_ << std::endl
               << "eta_2 " << sec_to_time(eta_) << std::endl;
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
                    
                    if(p() < 1) {
                        print();
                        std::cout << RENTER_;
                    }
                    else {
                        eta_ = timer_.sec();
                        print();
                    }
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
            update(load_); //ifs in this fct
        }
        static inline double p() {
            return (idx_-first_idx_) / (load_ - first_idx_);
        }
        static void print() {
            if(to_file)
                write_state_file();
            if(to_term)
                std::cout << "Job done in " << sec_to_time(eta_) << "  " << progress_bar(p());
        }
        static bool to_file;
        static bool to_term;
    private:
        static timer_class timer_;
        static uint64_t load_;
        
        //------------------- modulator -------------------
        static uint64_t mod_;
        static double last_print_;
        static double first_idx_;
        static double idx_;
        
        //------------------- timer & file -------------------
        static double eta_;
        static std::string fname_;
        static bool trigger_;
    };
    
    bool progress::to_file = false;
    bool progress::to_term = true;
    
    timer_class progress::timer_;
    uint64_t progress::load_;
    uint64_t progress::mod_;
    double progress::last_print_;
    double progress::first_idx_;
    double progress::idx_;
    double progress::eta_;
    std::string progress::fname_;
    bool progress::trigger_ = false;
}//end namespace addon
#endif // ADDON_PROGRESS_HEADER
