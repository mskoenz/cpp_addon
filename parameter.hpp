// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.12.2014 14:28:28 CET
// File:    parameter.hpp

#ifndef ADDON_PARAMETER_HEADER
#define ADDON_PARAMETER_HEADER

#include "helper.hpp"

#include <boost/any.hpp>

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
        proxy_struct(boost::any & val): val(val) {
        }
        
        #define CAST_OP(X) operator X() {return boost::any_cast<X>(val);}
        CAST_OP(int)
        CAST_OP(double)
        CAST_OP(std::string)
        #undef CAST_OP
        
        void operator=(const char t[]) { // handle spaecial case since bla is not a string
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
        
        boost::any & val;
    };
    
    //============================ addition ========================================================
    #define OP_SUPPORT(NAME, OP)                                        \
    template<typename T, typename X>                                    \
    typename std::enable_if< std::is_convertible<T, X>::value, X>::type \
                                    NAME##_helper(proxy_struct a, T b) {\
        return X(a) OP X(b);                                            \
    }                                                                   \
    template<typename T, typename X>                                    \
    typename std::enable_if<!std::is_convertible<T, X>::value, X>::type \
                                    NAME##_helper(proxy_struct a, T b) {\
        std::stringstream ss;                                           \
        ss << "type " << typeid(T).name() << "(" << b                   \
           << ") is not convertible to " << typeid(X).name();           \
        ERROR(ss.str());                                                \
        return X();                                                     \
    }                                                                   \
    template<typename T>                                \
    boost::any operator OP (proxy_struct a, T b) {      \
        if(a.val.type() == typeid(std::string))         \
            return NAME##_helper<T, std::string>(a, b); \
        else if(a.val.type() == typeid(int))            \
            return NAME##_helper<T, int>(a, b);         \
        else if(a.val.type() == typeid(double))         \
            return NAME##_helper<T, double>(a, b);      \
        else                                            \
            ERROR("not supported type in proxy_struct");\
                                                        \
        return 0;                                       \
    }                                                   \
    template<typename T>                                \
    boost::any operator OP(T b, proxy_struct a) {       \
        return a OP b;                                  \
    }                                                   // 
    
    OP_SUPPORT(add, + )
    OP_SUPPORT(mult, * )
    
    #undef OP_SUPPORT
    
    std::ostream & operator<<(std::ostream & os, proxy_struct const & arg) {
        #define CAST_BACK(X) if(arg.val.type() == typeid(X)) {      \
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
        using map_type = std::map<std::string, boost::any>;
    public:
        parameter_class() {
            map_["warn_"] = 1;
        }
        void add(std::string const & name, boost::any const & val) {
            map_[name] = val;
        }
        proxy_struct operator[](std::string const & name) {
            return proxy_struct(map_[name]);
        }
        void warn(std::string const & text) {
            if(boost::any_cast<int>(map_["warn_"]))
                WARNING(text);
        }
        void read(int argc, char * argv[]) {
            //======================= get program dir ==============================================
            std::string dir(argv[0]);
            auto pos = dir.rfind("/");
            dir.erase(pos + 1, dir.size() - pos - 1); // erase progname in ./folder/progname
            dir.erase(0, 1); // erase . in ./folder
            
            char * path = getcwd(NULL, 0);
            std::string cwd = path;
            (*this)["prog_dir"] = cwd + dir;
            
            //===================== format incoming argv ===========================================
            std::stringstream ss;
            //--------------- in case there is a bash_in.txt, read it ------------------------------
            std::ifstream ifs;
            ifs.open(cwd + dir + "bash_in.txt");
            
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
            for(int i = 1; i < argc; ++i) {
                ss << argv[i] << " ";
            }
            //----------------------- kill whitespace ----------------------------------------------
            std::string text = ss.str();
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
            
            for(uint i = 1; i < v.size(); ++i) {
                
                auto & w = v[i];
                // checking if = sign
                if(w.find("=") != std::string::npos) {
                    std::string key = w.substr(0, w.find("="));
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
                        std::string val = v[i + 1];
                        //-------------- just checking for false input -----------------------------
                        if(has_param(key)) {
                            std::stringstream ss;
                            ss << "parameter " << key << " already set to " << (*this)[key] << " -> overwrite to " << val;
                            warn(ss.str());
                        }
                        //----------------- setting the parameter ----------------------------------
                        add_helper(key, val);
                        pas = true;
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
            for(auto v: map_) { // has to be copy...
                os << v.first << " = " << proxy_struct(v.second) << std::endl;
            }
            os << "Flags: ";
            for(auto const & f: flag_) {
                os << "-" << f << " ";
            }
            os << std::endl << "Args: ";
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
                add(key, ival);
            else if(dpos == val.size())
                add(key, dval);
            else
                add(key, val);
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
