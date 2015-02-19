// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.12.2014 14:28:28 CET
// File:    parameter.hpp

#ifndef ADDON_PARAMETER_HEADER
#define ADDON_PARAMETER_HEADER

#include "helper.hpp"
#include "boost_any.hpp"

#include <map>
#include <regex>
#include <fstream>
#include <sstream>
#include <type_traits>

namespace addon {
    //  +------------------------------------------------------------------------------------------+
    //  |                        proxy struct                                                      |
    //  +------------------------------------------------------------------------------------------+
    struct proxy_struct {
        proxy_struct() {
        }
        template<typename T>
        proxy_struct(T t): val(t) {
        }
        #define CAST_OP(X) operator X() {return boost::any_cast<X>(val);}
        CAST_OP(int)
        CAST_OP(double)
        CAST_OP(std::string)
        #undef CAST_OP
        
        std::type_info const & type() const {
            return val.type();
        }
        void operator=(const char t[]) { // handle spaecial case since "bla" is not a string
            val = std::string(t);
        }
        template<typename T>
        typename std::enable_if<!std::is_same<T, proxy_struct>::value>::type operator=(T const & t){
            val = t;
        }
        template<typename T>
        typename std::enable_if< std::is_same<T, proxy_struct>::value>::type operator=(T const & t){
            val = t.val;
        }
        
        boost::any val;
    };
    
    //============================ addition ========================================================
    #define OP_SUPPORT(NAME, OP)                        \
    template<typename T, bool, bool, bool>              \
    struct NAME##_helper {                              \
        static proxy_struct exec(proxy_struct a, T b) { \
            return a;                                   \
        }                                               \
    };                                                  \
    template<typename T>                                \
    struct NAME##_helper<T, true, false, false> {       \
        static proxy_struct exec(proxy_struct a, T b) { \
            return std::string(a) + std::string(b);     \
        }                                               \
    };                                                  \
    template<typename T>                                \
    struct NAME##_helper<T, false, true, false> {       \
        static proxy_struct exec(proxy_struct a, T b) { \
            if(a.type() == typeid(int))                 \
                return proxy_struct(int(a) OP double(b));\
            if(a.type() == typeid(double))              \
                return proxy_struct(double(a) OP double(b));\
        }                                               \
    };                                                  \
    template<typename T>                                \
    struct NAME##_helper<T, false, false, true> {       \
        static proxy_struct exec(proxy_struct a, T b) { \
            if(a.type() == typeid(int))                 \
                return proxy_struct(int(a) OP int(b));  \
            if(a.type() == typeid(double))              \
                return proxy_struct(double(a) OP double(b));\
        }                                               \
    };                                                  \
                                                        \
    template<typename T>                                \
    proxy_struct operator OP (proxy_struct a, T b) {    \
        proxy_struct res;                               \
        res = NAME##_helper<T, std::is_convertible<T, std::string>::value   \
                             , std::is_floating_point<T>::value             \
                             , std::is_integral<T>::value                   \
                           >::exec(a, b);               \
                                                        \
        return res;                                     \
    }                                                   \
    template<typename T>                                \
    proxy_struct operator OP(T b, proxy_struct a) {     \
        return a OP b;                                  \
    }                                                   //
    
    OP_SUPPORT(add, + )
    OP_SUPPORT(mult, * )
    
    #undef OP_SUPPORT
    
    std::ostream & operator<<(std::ostream & os, proxy_struct const & arg) {
        #define CAST_BACK(X) if(arg.type() == typeid(X)) {      \
                                 os << boost::any_cast<X>(arg.val); \
                             } else                                 // 
        
        CAST_BACK(int)
        CAST_BACK(double)
        CAST_BACK(std::string)
            os << YELLOW_ << "no cast found" << NONE_;
        
        #undef CAST_BACK
        
        return os;
    }
    
    //  +------------------------------------------------------------------------------------------+
    //  |                       parameter class                                                    |
    //  +------------------------------------------------------------------------------------------+
    class parameter_class {
        using map_type = std::map<std::string, proxy_struct>;
    public:
        parameter_class() {
            map_["warn_"] = 1;
        }
        proxy_struct & operator[](std::string const & name) {
            return map_[name];
        }
        void warn(std::string const & text) {
            if(int(map_["warn_"]))
                WARNING(text);
        }
        void read(uint argc, char * argv[]) {
            //======================= get workin dir / program dir =================================
            std::string wd = abspath(argv);
            
            //------------------- search argv for "wd" -------------------
            std::string text = "";
            //~ bool found = false;
            
            for(uint i = 1; i < argc; ++i) {
                std::string s = argv[i];
                if(s.find("wd") != std::string::npos) {
                    if(s.find("-wd") != std::string::npos)
                        text = argv[i+1];
                    else {
                        text = s;
                        if(i < argc - 1) // in case it is "wd= bla" or "wd =bla"
                            text += " " + std::string(argv[i + 1]);
                        if(i < argc - 2) // since it could be "wd = bla"
                            text += " " + std::string(argv[i + 2]);
                        
                        text = std::regex_replace(text, std::regex(" ?= ?"), "="); // kill whitespace
                        if(text.find(" ") != std::string::npos) {
                            int p = text.find(" ");
                            text = text.erase(p, text.size());
                        }
                        text = text.erase(0, 3); //remove "wd="
                    }
                    //~ found = true;
                    if(text[0] == '/')
                        wd = text;
                    else
                        wd += "/" + text;
                }
            }
            
            (*this)["wd"] = wd;
            
            //===================== format incoming argv ===========================================
            std::stringstream ss;
            //--------------- in case there is a bash_in.txt, read it ------------------------------
            std::ifstream ifs;
            ifs.open(wd + "/bash_in.txt");
            
            if(ifs.is_open()) {
                GREEN("bash_in.txt file found");
                std::string temp;
                while(ifs) {
                    ifs >> temp;
                    if(ifs)
                        ss << temp << " ";
                }
                ifs.close();
            }
            //---------------- add normal argv last (higher prio) ----------------------------------
            for(uint i = 1; i < argc; ++i) {
                ss << argv[i] << " ";
            }
            //----------------------- kill whitespace ----------------------------------------------
            text = ss.str();
            text = std::regex_replace(text, std::regex(" ?= ?"), "="); // kill whitespace
            
            //-------------------------- split -----------------------------------------------------
            std::stringstream iss(text);
            std::vector<std::string> v{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
            
            
            //----------------------- error checking -----------------------------------------------
            for(auto & w: v) {
                if(w[0] == '-' and w.find("=") != std::string::npos)
                    ERROR("flags cannot be assigned with a equal sigh");
                if(w.find("=") != std::string::npos and w.find("=") != w.rfind("="))
                    ERROR("too many = signs, check syntax");
                if(w.back() == '=')
                    ERROR("no assignment after equal sign");
                if(w[0] == '-' and w.size() == 1)
                    ERROR("syntax not correct, check -");
            }
            
            bool pas = false;
            
            for(uint i = 0; i < v.size(); ++i) {
                
                auto & w = v[i];
                // checking if = sign
                if(w.find("=") != std::string::npos) {
                    std::string key = w.substr(0, w.find("="));
                    if(key == "wd")
                        continue;
                    std::string val = w.substr(w.find("=") + 1, w.size());
                    if(has_param(key)) {
                        std::stringstream ss;
                        ss << "parameter " << key << " already set to " << (*this)[key] << " -> overwrite to " << val;
                        warn(ss.str());
                    }
                    
                    add_helper(key, val);
                    continue;
                }
                if(w[0] == '-' and w.size() > 1) {
                    std::string key = w.substr(1, w.size());
                    if(i + 1 < v.size() and v[i + 1][0] != '-' and v[i + 1].find("=") == std::string::npos) { // param
                        pas = true;
                        if(key == "wd")
                            continue;
                        std::string val = v[i + 1];
                        //-------------- just checking for false input -----------------------------
                        if(has_param(key)) {
                            std::stringstream ss;
                            ss << "parameter " << key << " already set to " << (*this)[key] << " -> overwrite to " << val;
                            warn(ss.str());
                        }
                        //----------------- setting the parameter ----------------------------------
                        add_helper(key, val);
                    }
                    else { // flag
                        //-------------- just checking for false input -----------------------------
                        if(has_flag(key)) {
                            std::stringstream ss;
                            ss << "flag " << key << " was already set";
                            warn(ss.str());
                        }
                        else
                            //----------------- setting the flag -----------------------------------
                            flag_.push_back(key);
                    }
                }
                else {
                    if(pas)
                        pas = false;
                    else { // arg
                        //-------------- just checking for false input -----------------------------
                        if(has_arg(w)) {
                            std::stringstream ss;
                            ss << "arg " << w << " was already set";
                            warn(ss.str());
                        }
                        else
                            //------------------ adding the arg ------------------------------------
                            arg_.push_back(w);
                    }
                }
            }
        }
        map_type const & map() const {
            return map_;
        }
        //=========================== query ========================================================
        bool has_param(std::string const & key) const {
            if(map_.find(key) != map_.end())
                return true;
            return false;
        }
        bool has_flag(std::string const & flag) const {
            if(find(flag_.begin(), flag_.end(), flag) != flag_.end())
                return true;
            return false;
        }
        bool has_arg(std::string const & arg) const {
            if(find(arg_.begin(), arg_.end(), arg) != arg_.end())
                return true;
            return false;
        }
        bool has_key(std::string const & key) const {
            return has_param(key) or has_flag(key) or has_arg(key);
        }
        //========================== printing ======================================================
        template<typename S>
        void print(S & os) const {
            os << GREENB_ << "Parameter Class contains:" << NONE_ << std::endl;
            //------------------- print int -------------------
            os << GREEN_ << "integer:" << NONE_ << std::endl;
            for(auto v: map_) // has to be copy...
                if(v.second.type() == typeid(int))
                    os << "    " << v.first << " = " << v.second << std::endl;
            //------------------- print double -------------------
            os << GREEN_ << "double:" << NONE_ << std::endl;
            for(auto v: map_)
                if(v.second.type() == typeid(double))
                    os << "    " << v.first << " = " << v.second << std::endl;
            //------------------- print string -------------------
            os << GREEN_ << "string:" << NONE_ << std::endl;
            for(auto v: map_)
                if(v.second.type() == typeid(std::string))
                    os << "    " << v.first << " = " << v.second << std::endl;
            os << GREEN_ << "Flags: " << NONE_;
            for(auto const & f: flag_) {
                os << "-" << f << " ";
            }
            os << std::endl;
            os << GREEN_ << "Args: " << NONE_;
            for(auto const & a: arg_) {
                os << a << " ";
            }
            os << std::endl;
        }
    private:
        void add_helper(std::string const & key, std::string const & val) {
            size_t ipos = 0;
            int ival = 0;
            try {
                ival = std::stoi(val, & ipos);
            } catch(...) {
                ipos = 0;
            }
            
            size_t dpos = 0;
            double dval = 0;
            try {
                dval = std::stod(val, & dpos);
            } catch(...) {
                dpos = 0;
            }
            
            if(ipos == val.size())
                map_[key] = ival;
            else if(dpos == val.size())
                map_[key] = dval;
            else
                map_[key] = val;
        }
        map_type map_;
        std::vector<std::string> arg_;
        std::vector<std::string> flag_;
    } parameter;

    std::ostream & operator<<(std::ostream & os, parameter_class const & arg) {
        arg.print(os);
        return os;
    }
}// end namespace addon

#endif // ADDON_PARAMETER_HEADER
