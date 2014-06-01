#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <string>
#include <vector>

#include "../common/types.h"
#include "../error/error.h"
#include "../math/Vec3.h"

class Configuration
{
public:

    enum class valueType
    {
        charge  = 0,
        dipoleX = 1,
        dipoleY = 2,
        dipoleZ = 3,
        qd20    = 4,
        qd21c   = 5,
        qd21s   = 6,
        qd22c   = 7,
        qd22s   = 8
    };

    struct FitSite
    {
        FitSite( const std::string &n, const F32 charge, const F32 x, const F32 y, const F32 z, const U32 flags ) :
            name( n ), atomicCharge( charge ), position( x, y, z ), fitFlags( flags ) 
        {
        }

        const std::string name;
        const F32 atomicCharge;
        const Vec3 position;
        const U32 fitFlags;
    };

    Configuration( const std::string &file );

    const Error::STATUS GetStatus() const;

    U32 Size() const;

    const FitSite *GetSite( const U32 index ) const;

    F32 GetValue( const valueType type );
    void SetValue( const valueType type, const F32 val );

private:

    F32 values[9];

    Error::STATUS mStatus;

    std::vector< FitSite > mFitSites;
};

#endif