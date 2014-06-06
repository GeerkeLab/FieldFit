#ifndef __FIELD_H__
#define __FIELD_H__

#include "../common/types.h"
#include "../error/error.h"
#include "../math/vec3.h"
#include "../io/blockParser.h"

#include <string>
#include <vector>

class Configuration;

class Field
{
public:

    Field( const BlockParser &bp );

    F64 GetGridPointPotential( const Vec3 &gridPoint, const Configuration &conf ) const;
    
    Error::STATUS SetPermField( const Configuration &conf );
    
    Error::STATUS GetStatus() const;

    U32 Size() const;
    const Vec3 & GetFieldPosition( const U32 index ) const;
    
    F64 GetFieldPotential( const U32 index ) const;
    F64 GetFieldDiff( const U32 index ) const;
    F64 GetPermPotential( const U32 index ) const;
    
    F64 GetFieldStats( const Configuration &conf ) const;
    
private:

    Error::STATUS mStatus;

    //std::vector< Vec3 > mGenerationSites;

    std::vector< Vec3 > mFieldPositions;
    std::vector< F64 >  mFieldPotential;
    
    std::vector< F64 >  mPermPotential;
};


#endif