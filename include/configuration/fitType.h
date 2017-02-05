#pragma once
#ifndef __FIT_TYPE_H__
#define __FIT_TYPE_H__

#include "common/types.h"

#include <string>

namespace FieldFit
{
    enum FitType
    {
        charge  = 0,
        dipoleX = 1,
        dipoleY = 2,
        dipoleZ = 3,
        qd20    = 4,
        qd21c   = 5,
        qd21s   = 6,
        qd22c   = 7,
        qd22s   = 8,
        size    = 9
    };
    
    enum SpecialFlag
    {
        alpha = 10
    };
    
    U32 StringTypeToFitFlags( const std::string &flags );
    
    bool IsSet( U32 flags, FitType type );
    bool IsMultiSet( U32 flags, U32 testValue );
    bool IsSpecialSet( U32 flags, SpecialFlag sf );
}

#endif