#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>
#include <functional>
#include <memory>
#include <random>
#include <cstdio>
#include "c-enzyme.h"




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
  VolSurface(Vec<Time>& t, Vec<Price>& k, Mat<mDouble>& vols, Time T, size_t N_path, size_t N_step):
      m_times(t)
    , m_strikes(k)
    , m_vols(vols)
    , m_volslice_interp(m_strikes.size())
    , m_T(T)
    , h(m_strikes.size() - 1)
    , alpha(m_strikes.size())
    , l(m_strikes.size() - 1)
    , mu(m_strikes.size() - 1)
    , z(m_strikes.size())
    , m_d(m_strikes.size() - 1)
    , m_c(m_strikes.size())
    , m_b(m_strikes.size() - 1)
    , m_a(m_strikes.size())
    , m_random_paths()
    , m_pv_vec(N_path)
    {
      for (int i = 0; i < N_path; ++i) {
        std::vector<double> randomVector = generateRandomVector(N_step, 0.0, 1.0);
        m_random_paths.push_back(randomVector);
    }
  }

  Vec<double> generateRandomVector(int dim, double mean, double stddev) {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::normal_distribution<double> distribution(mean, stddev);

      Vec<double> randomVector(dim);
      for (int i = 0; i < dim; ++i) {
          randomVector[i] = distribution(gen);
      }

      return randomVector;
  }

  size_t find_closest_t(double t) const {
    Vec<double>::const_iterator it;
    it=std::upper_bound(m_times.begin(),m_times.end(), t);       // *it > x
    size_t idx = std::max(int(it-m_times.begin()) - 1, 0);   // m_x[idx] <= x
    return idx;
  }

  size_t find_closest_k(double& k) const {
    Vec<double>::const_iterator it;
    it=std::upper_bound(m_strikes.begin(),m_strikes.end(), k);       // *it > x
    size_t idx = std::max(int(it-m_strikes.begin()) - 1, 0);   // m_x[idx] <= x
    return idx;
  }

  mDouble linear_interpolation(const double s, const double f, double& x) {
    return std::lerp(s, f, x);;
  }

  mDouble cubic_interpolation(double x_interp) const {
        int n = m_strikes.size();
        int i = 0;
        // Find the interval [x[i], x[i+1]] where x_interp lies
        while (i < n - 1 && x_interp > m_strikes[i + 1]) {
            ++i;
        }
        // Evaluate cubic spline at x_interp
        double xi = x_interp - m_strikes[i];
        return m_a[i] + m_b[i] * xi + m_c[i] * xi * xi + m_d[i] * xi * xi * xi;
  }

  mDouble getVar(double time, double strike) {

    size_t left_idx_t = find_closest_t(time);
    size_t right_idx_t = left_idx_t + 1;
    mDouble t = (time - m_times[left_idx_t])/(m_times[right_idx_t] - m_times[left_idx_t]);

    const auto& v_slice_left = m_vols[left_idx_t];
    const auto& v_slice_right = m_vols[right_idx_t];
    const size_t M = m_vols[0].size();

    for (size_t i = 0; i < M; ++i) {
        mDouble lv = v_slice_left[i];
        mDouble rv = v_slice_right[i];
        m_volslice_interp[i] = linear_interpolation(lv, rv, t);
    }
    const auto& y = m_volslice_interp;
    const auto& x = m_strikes;

    // Сubic interpolation, coefficients calcilation
    for (int i = 0; i < M - 1; ++i) {h[i] = x[i + 1] - x[i];}
    for (int i = 1; i < M - 1; ++i) {
      alpha[i] = (3.0 / h[i]) * (y[i + 1] - y[i]) - (3.0 / h[i - 1]) * (y[i] - y[i - 1]);
    }

    l[0] = 1;
    mu[0] = 0;
    z[0] = 0;

    for (int i = 1; i < M - 1; ++i) {
        l[i] = 2 * (x[i + 1] - x[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }

    l[M - 1] = 1;
    z[M - 1] = 0;
    m_c.resize(M);
    m_b.resize(M - 1);
    m_d.resize(M - 1);

    for (int j = M - 2; j >= 0; --j) {
        m_c[j] = z[j] - mu[j] * m_c[j + 1];
        m_b[j] = (y[j + 1] - y[j]) / h[j] - h[j] * (m_c[j + 1] + 2 * m_c[j]) / 3;
        m_d[j] = (m_c[j + 1] - m_c[j]) / (3 * h[j]);
    }
    m_a = y;
    return cubic_interpolation(strike);
  }

  mDouble PV(mDouble time, mDouble strike) {
      auto sigma = getVar(time, strike);
      auto S0 = strike;
      
      const auto N_paths = m_random_paths.size();
      const auto N_steps = m_random_paths[0].size();
      mDouble dT = (m_T - time)/255;
      dT /= N_steps;
      for (size_t i = 0; i < N_paths; ++i) {
          m_pv_vec[i] = S0;
          for (size_t j = 0; j < N_steps; ++j) {
            m_pv_vec[i] *= std::exp(sigma * std::sqrt(dT) * m_random_paths[i][j]);
          }
      }
      mDouble PV = 0;
      for (auto& term_value : m_pv_vec) {
          PV += std::max(term_value - strike, 0.0);
      }
      return PV / N_paths;
  }

  private:
  Vec<Time> m_times;
  Vec<Price> m_strikes;
  Mat<mDouble> m_vols;
  Mat<mDouble> m_random_paths;
  mDouble m_T;
  Vec<Price> m_volslice_interp;
  Vec<double> h;
  Vec<double> alpha;
  Vec<double> l;
  Vec<double> mu;
  Vec<double> z;
  Vec<double> m_d;
  Vec<double> m_c;
  Vec<double> m_b;
  Vec<double> m_a;
  Vec<double> m_pv_vec;
};

double wrapVar(VolSurface& surf, double x, double y) {return surf.getVar(x,y);}
double wrapPV(VolSurface& surf, double x, double y) {return surf.PV(x,y);}

mDouble dVdK(VolSurface& s, double T, double K){
  Price dK = 0.0;
  double deriv = __enzyme_fwddiff<mDouble>((void*)(&wrapVar), enzyme_const, &s, 
                             enzyme_const, T,
                             enzyme_dupnoneed, K, dK);

  return deriv;
}

mDouble dPVdK(VolSurface& s, double T, double K){
  Price dK = 0.0;
  double deriv = __enzyme_fwddiff<mDouble>((void*)(&wrapPV), enzyme_const, &s, 
                             enzyme_const, T,
                             enzyme_dupnoneed, K, dK);

  return deriv;
}

double f(double x) { return x * x; }



int main() {

  Vec<Price> strikes{56.0,66.0,76.0};
  Vec<Time>  times{
    171,
    172,
    173
  }; 
  Mat<mDouble> vols{
    {1.299047685,1.179255756,1.066483208},
    {0.6627478701,0.620289717,0.5800563088},
    {0.5352105909,0.508991143,0.4840308026}
  };

  VolSurface surface{times, strikes, vols, 173 + 20, 1000, 1000};
  Time T =  (172 + 173) / 2;
  Price K  = (66.0 + 76.0) / 2;
  

  std::cout << "T = " << T << " K = " << K <<  "\n";
  std::cout << "PV: " <<  surface.PV(T, K) << "\n";
  std::cout << "dPVdK: " << dPVdK(surface, T, K) << "\n";

  double x = 5.0;
  double dx = 1.0;  
  double df_dx = __enzyme_fwddiff<mDouble>((void*)f, enzyme_dup, x, dx); 
  std::cout << "f(x) = " << f(x) << " f'(x) = " << df_dx;

     
    return 0;
}