#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>
#include <functional>
#include <memory>
#include "c-enzyme.h"

template<typename T> using Vec = std::vector<T>;
template<typename T> using Mat = std::vector<std::vector<T>>;

std::unique_ptr<Vec<double>> linear_interpolation_vec(const Vec<double>& s, const Vec<double>& f, double x) {
    const size_t n = s.size();
    std::unique_ptr<Vec<double>> interp = std::make_unique<Vec<double>>(n, 0);
    for (int i = 0; i < n; ++i) {
        (*interp)[std::lerp(s[i], f[i], x)];
    }
    return interp;
};

int main() {
    
    Vec<double> v_start{};
    Vec<double> v_end{};

    for (int i = 1; i < 100; ++i) {
        v_start.push_back(static_cast<double>(i));
        v_end.push_back(static_cast<double>(i * i));
    }

    double p = 0.5;
    auto res = __enzyme_autodiff(&linear_interpolation_vec, enzyme_const, &v_start, enzyme_const, &v_end, enzyme_out, p);
    //auto res = linear_interpolation_vec(v_start, v_end, p);
    /*
    auto v = *res;
    for (size_t idx = 0; idx < v_start.size(); ++idx) {
        std::cout << "y1 = " << v_start[idx] << " y2 = " << v_end[idx] << " interp value = " << v[idx] << "\n";
    }
    
    */
    return 0;
}