#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>
#include <functional>
#include "spline.h"
#include "c-enzyme.hh"

using Time = double;
using Price = double;
using mDouble = double;
using ForwardCurve = std::map<Time, Price>;

template<typename T> using Vec = std::vector<T>;

template<typename T> using Mat = std::vector<std::vector<T>>;

Vec<Time> vec{}; 

Mat<Time> mat{};


class VolSurface {
  //using IM = InterpolationMethod;
  const Vec<Time> times_;
  const Vec<Price> strikes_;
  const Mat<mDouble> vols_;
  public:
  VolSurface(
      const Vec<Time>& t,
      const Vec<Price>& s,
      const Mat<mDouble>& v
      //const ForwardCurve fwd_,
      //IM x_interp_m_,
      //IM y_interp_m
      ): times_(t), strikes_(s), vols_(v) {
  }
  private:
  std::pair<size_t, size_t> get_time_slice_idx(Time& t) {
      auto idx = std::upper_bound(times_.begin(), times_.end(), t) - times_.begin();
      return {idx - 1, idx};        
  }

  Vec<mDouble> get_vol_slice_vector(Time& time) {
      auto [t1, t2] = get_time_slice_idx(time);
      auto vol_v_1 = vols_[t1];
      auto vol_v_2 = vols_[t2];
      //std::cout << "Times: " << times_[t1] << " " << times_[t2] <<  "\n";
      auto t = static_cast<mDouble>(time - times_[t1]) / (times_[t2] - times_[t1]);
      //std::cout << "Param: t: " << t << "\n";
      Vec<mDouble> vol_slice(vol_v_1.size());
      for (size_t idx = 0; idx < vol_v_1.size(); ++idx) {
          vol_slice[idx] = std::lerp(vol_v_1[idx], vol_v_2[idx], t);
      }
      return vol_slice;
  }
  public:
  mDouble getVar(Time& time, Price& strike) {
      auto vol_slice_at_time = get_vol_slice_vector(time);
      tk::spline s(strikes_, vol_slice_at_time, tk::spline::cspline);
      return s(strike);
  }

  private:
  //TODO: write check bounds
  void check_bounds(Time& time, Price& strike);
  
};


double foo(VolSurface& surf, double& x, double& y) {return surf.getVar(x,y);}


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

  tk::spline::spline_type type = tk::spline::cspline;

  tk::spline s(strikes,vols[0],type);

  VolSurface surface(times, strikes, vols);

  Time T =  (1718928000000000000 + 1726790400000000000) / 2;
  Price K  = (66.0 + 76.0) / 2;

  Price dK = 0.0;

  double dfdy = __enzyme_autodiff<double>((&foo), enzyme_const, &surface, 
                                                    enzyme_const, &T,
                                                    enzyme_dup, &K, &dK);


    std::cout << "T = " << T << " K = " << K <<  "n";

    std::cout << " Interpolated surface: " <<  surface.getVar(T, K) << "\n";

    std::cout << "Print Derivatives: dfdy:" << dfdy  <<  " dK: " <<  dK << "\n";
     
    return 0;
}