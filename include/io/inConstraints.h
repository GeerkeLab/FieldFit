#pragma once
#ifndef __INCONSTRAINTS_H__
#define __INCONSTRAINTS_H__

#include "common/types.h"
#include "io/units.h"

namespace FieldFit
{
    class Block;
    class BlockParser;
    class Constraints;   
    
    void ReadSumConstraintSet( const BlockParser &bp, const Units &units, Constraints &constr );
    void ReadSymConstraintSet( const BlockParser &bp, const Units &units, Constraints &constr );
         
    void ReadSumConstraints( const Block &block, const Units &units, Constraints &constr );
    void ReadSymConstraints( const Block &block, const Units &units, Constraints &constr );
}
 
#endif