// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    07.01.2015 12:02:00 CET
// File:    log.hpp

#ifndef ADDON_LOG_HEADER
#define ADDON_LOG_HEADER

#include "color.hpp"
#include "parameter.hpp"
#include "clock.hpp"

#include <map>
#include <fstream>
#include <iomanip>

namespace addon {
    class log_class {
        using map_type = std::map<std::string, boost::any>;
    public:
        log_class(std::string const & name): name_(name) {
        }
        void add(std::string const & name, boost::any const & val) {
            map_[name] = val;
        }
        proxy_struct operator[](std::string const & name) {
            return proxy_struct(map_[name]);
        }
        void print() {
            for(auto & a: map_) {
                std::cout << a.second << " ";
            }
            std::cout << std::endl;
        }
        void write() {
            verify_header();
            //------------------------ write data --------------------------------------------------
            std::ofstream ofs(name_.c_str(), std::ios_base::app);
            for(auto & a: map_) {
                ofs << a.second << " ";
            }
            ofs << std::endl;
            //----------------------- write timestamp ----------------------------------------------
            ofs << "#" << std::setfill('-') << std::setw(99) << clock::date_time() 
                << std::setfill(' ') << std::endl;
            ofs.close();
        }
    private:
        void verify_header() {
            std::ifstream ifs(name_.c_str(), std::ios_base::in);
            
            std::stringstream ss;
            for(auto & a: map_) {
                ss << a.first << " ";
            }
            std::string line_should_be = ss.str();
            
            if(ifs.is_open()) { // file exists
                
                std::string line_read = "";
                while(line_read == "" and !ifs.eof())
                    std::getline(ifs, line_read);
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
            
            std::ofstream ofs(name_.c_str(), std::ios_base::trunc | std::ios_base::out);
            ofs << line_should_be << std::endl;
            ofs.close();
        }
        std::string name_;
        map_type map_;
    };
}// end namespace addon

#endif // ADDON_LOG_HEADER
