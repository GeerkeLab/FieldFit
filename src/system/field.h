#ifndef __FIELD_H__
#define __FIELD_H__

#include "../common/types.h"
#include "../error/error.h"
#include "../math/Vec3.h"

#include <string>
#include <vector>

class Field
{
public:

    Field( const std::string &file );

    const Error::STATUS GetStatus() const;

    U32 Size() const;
    const Vec3 & GetFieldPosition( const U32 index ) const;
    const F64 GetFieldPotential( const U32 index ) const;

private:

    Error::STATUS mStatus;

    std::vector< Vec3 > mGenerationSites;

    std::vector< Vec3 > mFieldPositions;
    std::vector< F64 >  mFieldPotential;
};


#endif