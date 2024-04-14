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

## How to build
It's crucial that your machine should have x86 architercture

Fistsly, one should get latest version of LLVM with following flags

```bash
#/path/to/llvm_project
mkdir build && cd build
cmake -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=Release -DLLVM_INSTALL_UTILS=ON -DLLVM_INSTALL_TOOLCHAIN_ONLY=ON -G Ninja ../llvm
```

Make sure that you complile with with the compiler that you have already built

```bash
#/path/to/your_project
mkdir build && cd build
cmake -G Ninja .. -DLLVM_DIR=/home/ubuntu/.enzyme/llvm-project/build/lib/cmake/llvm -DClang_DIR=/home/ubuntu/.enzyme/llvm-project/build/lib/cmake/clangs
ninja
```




