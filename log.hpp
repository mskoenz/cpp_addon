// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    07.01.2015 12:02:00 CET
// File:    log.hpp

#ifndef ADDON_LOG_HEADER
#define ADDON_LOG_HEADER

#include "color.hpp"
#include "clock.hpp"
#include "parameter.hpp" //for proxy struct

#include <map>
#include <fstream>
#include <iomanip>

namespace addon {
    class log_class {
        using map_type = std::map<std::string, proxy_struct>;
    public:
        log_class(std::string const & name = parameter["wd"] + "/log.txt"): name_(name) {
        }
        proxy_struct & operator[](std::string const & name) {
            if(find(order_.begin(), order_.end(), name) == order_.end()) {
                order_.push_back(name);
            }
            return map_[name];
        }
        void print() {
            for(auto & a: order_) {
                std::cout << map_[a] << " ";
            }
            std::cout << std::endl;
        }
        void write() {
            verify_header();
            //------------------------ write data --------------------------------------------------
            std::ofstream ofs(name_.c_str(), std::ios_base::app);
            for(auto & a: order_) {
                ofs << map_[a] << " ";
            }
            ofs << std::endl;
            //----------------------- write timestamp ----------------------------------------------
            ofs << "#" << std::setfill('-') << std::setw(99) << clock::date_time() 
                << std::setfill(' ') << std::endl;
            ofs.close();
        }
        template<typename T>
        void set_param(T const & t) {
            std::stringstream ss;
            ss << "#param ";
            for(auto const & a: t) {
                ss << a.first << "=" << "\"" << a.second << "\" ";
            }
            ss << "\n";
            param_line_ = ss.str();
        }
    private:
        void write_first(std::string const & line_should_be) const {
            std::ofstream ofs(name_.c_str(), std::ios_base::trunc | std::ios_base::out);
            ofs << line_should_be << std::endl;
            ofs << param_line_;
            
            ofs.close();
        }
        void verify_header() {
            std::stringstream ss;
            for(auto & a: order_) {
                ss << a << " ";
            }
            std::string line_should_be = ss.str();
            
            if(first_open_) {
                first_open_ = false;
                if(parameter.has_flag("log_kill")) {
                    write_first(line_should_be);
                    return;
                }
            }
            
            std::ifstream ifs(name_.c_str(), std::ios_base::in);
            if(ifs.is_open()) { // file exists
                
                std::string line_read = "";
                while((line_read == "" or line_read[0] == '#') and !ifs.eof()) {
                    std::getline(ifs, line_read);
                }
                ifs.close();
                
                if(line_read == line_should_be) { // all is ok, descriptor matches
                    return;
                }
                
                if(line_read != "") { // the file exists, but isn't empty
                    DEBUG_VAR(line_read)
                    DEBUG_VAR(line_should_be)
                    ERROR("Descriptor line in write_data is different. Please be consistent.");
                    return;
                }
            }
            
            write_first(line_should_be);
        }
        std::string name_;
        map_type map_;
        std::vector<std::string> order_;
        std::string param_line_ = "";
        bool first_open_ = true;
    };
}// end namespace addon

#endif // ADDON_LOG_HEADER
