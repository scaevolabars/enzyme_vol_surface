#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>
#include <functional>
#include "c-enzyme.h"
#include "spline.h"

using Time = double;
using Price = double;
using mDouble = double;
using ForwardCurve = std::map<Time, Price>;

template<typename T> using Vec = std::vector<T>;

template<typename T> using Mat = std::vector<std::vector<T>>;

Vec<Time> vec{}; 

Mat<Time> mat{};


class VolSurface {
  public:
  mDouble getVar(double& time, double& strike) {
    return strike * strike;
  }

  private:  
};

double wrap(VolSurface& surf, double& x, double& y) {return surf.getVar(x,y);}

mDouble dVdK(VolSurface& s, double T, double K){
  Price dK = 0.0;
  __enzyme_autodiff((&wrap), enzyme_const, &s, 
                            enzyme_const, &T,
                            enzyme_dup, &K, &dK);

  return dK;
}

int main() {

  Vec<Price> strikes{56.0,66.0,76.0};
  Vec<Time>  times{
    1710460800000000000,
    1718928000000000000,
    1726790400000000000
  }; 
  Mat<mDouble> vols{
    {1.299047685,1.179255756,1.066483208},
    {0.6627478701,0.620289717,0.5800563088},
    {0.5352105909,0.508991143,0.4840308026}
  };

  VolSurface surface{};
  Time T =  (1718928000000000000 + 1726790400000000000) / 2;
  Price K  = (66.0 + 76.0) / 2;
  std::cout << "T = " << T << " K = " << K <<  "n";
  std::cout << " Interpolated surface: " <<  surface.getVar(T, K) << "\n";
  std::cout << "Print Derivatives: dfdy:" << dVdK(surface, T, K) << "\n";
     
    return 0;
}