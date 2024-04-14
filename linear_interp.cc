#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>
#include <functional>
#include <memory>
#include <iomanip>
#include "c-enzyme.h"

template<typename T> using Vec = std::vector<T>;
template<typename T> using Mat = std::vector<std::vector<T>>;

double linear_interpolation(const double s, const double f, double& x) {
    return std::lerp(s, f, x);;
};

int main() {
    
    Vec<double> v_start{};
    Vec<double> v_end{};

    for (int i = 1; i < 10; ++i) {
        v_start.push_back(static_cast<double>(i));
        v_end.push_back(static_cast<double>(i * i));
    }

    
    
    for (int idx = 0; idx < v_start.size(); ++idx) {
        
        std::cout << "y1 = " << v_start[idx] << " y2 = " << v_end[idx] << std::endl;
        for (int i = 1; i < 10; i++) {
            double p = 0.1 * i;
            double dp = 0.0;
            __enzyme_autodiff(
                &linear_interpolation
                , enzyme_const, v_start[idx]
                , enzyme_const, v_end[idx]
                , enzyme_dup, &p, &dp
            );

            std::cout << " f'(" << p << ")" << " = " << dp << std::setw(5);
        }
        std::cout << std::endl;
       
    }
    return 0;
}