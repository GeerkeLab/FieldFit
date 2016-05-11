#ifndef __FIELD_H__
#define __FIELD_H__

#include "../common/types.h"
#include "../error/error.h"
#include "../math/vec3.h"
#include "../io/blockParser.h"

#include <string>
#include <vector>

class Configuration;


struct FieldCollection
{
    std::vector< Vec3 > fieldPositions;
    std::vector< F64 >  fieldPotential;
    
    std::vector< F64 >  permPotential;
};

struct EFieldCollection
{
    std::vector< Vec3 > efieldSite;
};


class Field
{
public:

    Field( const BlockParser &bp );

    bool ContainsElectricField() const;
    
    F64 GetGridPointPotential( const U32 coll, const Vec3 &gridPoint, const Configuration &conf ) const;
    F64 GetAlpha( const U32 coll, const U32 fitSite, const Configuration &conf ) const;
    const Vec3 & GetEfield( const U32 coll, const U32 fitSite ) const;
    
    Error::STATUS SetPermField( const Configuration &conf );
    
    Error::STATUS GetStatus() const;

    size_t Size( const U32 collection ) const;
    const Vec3 & GetFieldPosition( const U32 collection, const U32 index ) const;
    F64 GetFieldDiff( const U32 collection, const U32 index ) const;
    
    F64 GetFieldStats( const Configuration &conf ) const;
    
    size_t NumCollections() const;
    
private:
    
    Error::STATUS ReadFieldSites( const BlockParser &bp );
    Error::STATUS ReadElectricFieldSites( const BlockParser &bp );
    
    F64 GetFieldPotential( const U32 collection, const U32 index ) const;
    F64 GetPermPotential( const U32 collection, const U32 index ) const;
    
    Error::STATUS mStatus;

    std::vector< FieldCollection > mCollections;
    std::vector< EFieldCollection > mEFieldCollections;
};


#endif