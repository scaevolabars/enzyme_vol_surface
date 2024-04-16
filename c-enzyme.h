#pragma once

// Enzyme voodoo
extern int enzyme_allocated
         , enzyme_const
         , enzyme_dup
         , enzyme_dupnoneed
         , enzyme_out
         , enzyme_tape;

template < typename return_type, typename ... T >
return_type __enzyme_autodiff(void*, T ... );

template < typename return_type, typename ... T >
return_type __enzyme_fwddiff(void*, T ... );

#define APPROX_EQ(LHS, RHS, TOL) {\
    if (std::abs((LHS) - (RHS)) > (TOL)) {\
        throw std::logic_error{\
            #LHS " (which is " + std::to_string(LHS) + ")"\
            " != " #RHS " (which is " + std::to_string(RHS) + ")"\
            " within " + std::to_string(TOL) + " tolerance"\
        };\
    }\
}