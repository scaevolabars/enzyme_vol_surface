#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
#include <memory>
#include "c-enzyme.h"

class Calculation {
    public:
    // This version doesn't work
    std::vector<double> squares(double x, int n) {
        std::vector<double> sq{};
        for (int i = 1; i <= n; ++i) {
            sq.push_back(std::pow(x,i));
        }
        return sq;
    }

    std::unique_ptr<std::vector<double>> squares_unique_ptr(double x, int n) {
        std::unique_ptr<std::vector<double>> sq = std::make_unique<std::vector<double>>();
        for (int i = 1; i <= n; ++i) {
            sq->push_back(std::pow(x,i));
        }
        return sq;
    }

    double operator()(double x) {
        double sm = 0;
        //auto v = squares(x, 3); // Not working version
        auto v = squares_unique_ptr(x, 3);
        for (auto& x: *v) {
            sm += x;
        }
        return sm;
    }
};

double foo(Calculation& calc, double x) {return calc(x);}

int main() {
    Calculation calc{};
    double x =  2.0;
    std::cout << "x = " << x << "f(x) = " << calc(x) << " f'(x)' = " << __enzyme_autodiff<double>(&foo, enzyme_const, &calc, enzyme_out, x);
    return 0;
}