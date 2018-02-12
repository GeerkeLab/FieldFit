#pragma once
#ifndef __FITTING_MATH_H__
#define __FITTING_MATH_H__

#include <armadillo>

#ifndef FIELDFIT_USE_SPARSE

typedef arma::mat fit_matrix;
#define FIELDFIT_SOLVE solve

#else

typedef arma::sp_mat fit_matrix;
#define FIELDFIT_SOLVE spsolve

#endif

#endif