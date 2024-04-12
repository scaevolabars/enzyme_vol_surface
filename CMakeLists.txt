cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CC_COMPILER "clang")
project(vol_surface)

add_subdirectory( Enzyme/enzyme )

add_executable(prototype prototype.cc)

target_link_libraries(prototype PUBLIC ClangEnzymeFlags)
target_compile_features(prototype PRIVATE cxx_std_20 )



