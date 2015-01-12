// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    17.12.2014 11:36:01 CET
// File:    random.hpp

#ifndef ADDON_RANDOM_HEADER
#define ADDON_RANDOM_HEADER

#include <random>
#include <chrono>
#include <type_traits>

namespace addon {
    struct default_rng {
        typedef std::default_random_engine type;
    };
    
    struct mersenne {
        typedef std::mt19937 type;
    };
    
    struct linear_congruential {
        typedef std::minstd_rand0 type;
    };
    
    class global_seed_class {
    public:
        global_seed_class():  base_seed_(std::chrono::system_clock::now().time_since_epoch().count())
                            , distr_(0, 1000)
                            , rng_(base_seed_) {
        }
        uint64_t operator()() {
            ctr_ += distr_(rng_);
            std::cout << ctr_ << std::endl;
            return base_seed_ + ctr_;
        }
        uint64_t const & seed() const {
            return base_seed_;
        }
        void seed(uint64_t const & seed) {
            base_seed_ = seed;
            rng_.seed(base_seed_);
        }
    private:
        uint64_t base_seed_;
        uint64_t ctr_ = 0;
        std::uniform_int_distribution<int> distr_;
        std::default_random_engine rng_;
    } global_seed;
    
    template<typename T, typename RNG = default_rng> 
    class random_class {
    public:
        random_class(T const & lower = T(0), T const & upper = T(1)): distr_(lower, upper), rng_(global_seed()) {
        }
        T operator()() {
            return distr_(rng_);
        }
        uint64_t const & seed() const {
            return rng_.seed();
        }
        void seed(uint64_t const & seed) {
            rng_.seed(seed);
        }
    private:
        template<typename U, bool b>
        struct distr_chooser {
            using type = std::uniform_int_distribution<U>;
        };
        template<typename U> 
        struct distr_chooser<U, false> {
            using type = std::uniform_real_distribution<U>;
        };
        
        typename distr_chooser<T, std::is_integral<T> ::value>::type distr_;
        typename RNG::type rng_;
    };
}// end namespace addon

#endif // ADDON_RANDOM_HEADER
