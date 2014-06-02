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
    
    struct Constraint
    {
    	Constraint();
    	
    	Constraint( const std::vector< U32 > &cols, const std::vector< F32 > &coefficients, const F32 ref ):
    	matrixColumns( cols ), matrixCoefficients( coefficients ), reference( ref )
    	{}
    	
    	F32 reference;
    	std::vector< U32 > matrixColumns;
    	std::vector< F32 > matrixCoefficients;
    };

    F64 DelComp( const Configuration::valueType, const Vec3 &i, const Vec3 &j );

    Error::STATUS GenerateMatrices( std::vector< ChargeSite > &charges, std::vector< DipoleSite > &dipoles, std::vector< QuadrupoleSite > &quadrupoles, const Field &field, multiMatrix &a, multiMatrix &b  );
    Error::STATUS AddConstraints( std::vector< Constraint > &constraints, multiMatrix &a, multiMatrix &b  );
    
    Error::STATUS FitSites( Configuration &conf, const Field &field  );
};

#endif