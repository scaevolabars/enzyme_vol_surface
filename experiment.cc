#include <iostream>

class Calculation {
    public:
    double operator()(double x) {
        return x * x;
    }
};

 extern int enzyme_allocated
         , enzyme_const
         , enzyme_dup
         , enzyme_duponneed
         , enzyme_out
         , enzyme_tape;

template <typename Retval, typename... Args>
Retval __enzyme_autodiff(Retval (*)(Args...), auto...);

double foo(Calculation& calc, double x) {return calc(x);}

int main() {
    Calculation calc{};
    double x =  2.0;
    std::cout << "x = " << x << "x^2 = " << calc(x) << " (x^2)' = " << __enzyme_autodiff<double>(&foo, enzyme_const, &calc, enzyme_out, x);
    return 0;
}