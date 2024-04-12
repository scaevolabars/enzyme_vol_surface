# Volality Surface


Input: Regular Vol. Surface, Trivial FWD Yield

- [x]
 Step 1. For each moneyness level,  interpolate piecewise linearly the total variance.

- [x]
Step 2. For each given TTM (including both quoted from the surface, and the ones for Euler Scheme steps) build a cubic spline interpolation over strikes, with linear extrapolation. 

- [ ]
Step 3. Simulate the process via the Euler Scheme using  the Dupire formula with both the second order derivative of premium wrt. strike and the first order derivative of premium wrt TTM taken finite difference balanced (computed on the fly, including the needed extra linear tot. var. moneyness fixed interpolation).

- [ ]
Step 4. Run the the whole algorithm through EnzymeAD and Matlogica pricing a vanilla 

Output: PV and Vegas wrt regular surface points.