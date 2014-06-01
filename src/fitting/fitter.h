#ifndef __FITTER_H__
#define __FITTER_H__

#include "../system/configuration.h"
#include "../system/field.h"

#include "../system/multiMatrix.h"

#include "../math/mathf.h"

namespace Fitter
{
    struct ChargeSite
    {
        ChargeSite();

        ChargeSite( const U32 index, const Vec3 & p, const Configuration::valueType t );

        U32 confIndex;
        Vec3 pos;
        Configuration::valueType type;
    };


    struct DipoleSite
    {
        DipoleSite();

        DipoleSite( const U32 index, const Vec3 & p, const Configuration::valueType t );

        U32 confIndex;
        Vec3 pos;
        Configuration::valueType type;
    };

    struct QuadrupoleSite
    {
        QuadrupoleSite();

        QuadrupoleSite( const U32 index, const Vec3 & p, const Configuration::valueType t );

        U32 confIndex;
        Vec3 pos;
        Configuration::valueType type;
    };

    F64 DelComp( const Configuration::valueType, const Vec3 &i, const Vec3 &j );

    Error::STATUS GenerateMatrices( std::vector< ChargeSite > &charges, std::vector< DipoleSite > &dipoles, std::vector< QuadrupoleSite > &quadrupoles, const Field &field, multiMatrix< F64 > &a, multiMatrix< F64 > &b  );

    Error::STATUS FitSites( Configuration &conf, const Field &field  );
};

#endif