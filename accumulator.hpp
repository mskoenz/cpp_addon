// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    08.01.2015 10:31:37 CET
// File:    accumulator.hpp

#ifndef ADDON_ACCUM_HEADER
#define ADDON_ACCUM_HEADER

#include <cmath>
#include <iomanip>
#include <iostream>

namespace addon {
    template<typename T>
    class accumulator {
            using mean_type = decltype((T() + T()) / double());
            using count_type = uint64_t;
        public:
            accumulator(): n_(0), sum_(0), sum2_(0) {};
            inline void operator<<(T const & val) {
                ++n_;
                sum_ += val;
                sum2_ += val * val;
            }
            inline mean_type mean() const {
                return mean_type(sum_) / n_;
            }
            inline mean_type dev() const {
                mean_type sc0 = n_ - 1;
                return sqrt(sum2_ / sc0 - sum_ * sum_ / sc0 / n_);
            }
            inline mean_type std() const {
                using std::sqrt;
                return dev() / sqrt(n_);
            }
            inline count_type const & count() const {
                return n_;
            }
            inline T const & sum() const {
                return sum_;
            }
            inline void clear() {
                n_ = 0;
                sum_ = 0;
                sum2_ = 0;
            }
        private:
            count_type n_;
            T sum_;
            T sum2_;
    };

    template<typename T>
    std::ostream & operator<<(std::ostream & os, accumulator<T> const & d) {
        os << d.mean() << " +/- " << std::setprecision(3) << (100 * d.std() / d.mean()) << "% (n = " << d.count() << ")";
        return os;
    }
}// end namespace addon

#endif // ADDON_ACCUM_HEADER
