#pragma once
#ifndef __DELCOMP_H__
#define __DELCOMP_H__

#include "common/types.h"

#include "configuration/fitType.h"

#include <armadillo>

namespace FieldFit
{
    arma::vec DelComp( const F64 posX, const F64 posY, const F64 posZ,
                       const arma::vec &gridX, const arma::vec &gridY,  const arma::vec &gridZ,
                       const FitType type );
}

#endif