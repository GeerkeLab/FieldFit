#ifndef __FITTER_H__
#define __FITTER_H__

#include "../system/configuration.h"
#include "../system/field.h"
#include "../system/multiMatrix.h"
#include "../math/mathf.h"


/*
**
**	Namespace that contains all general fitting functions
**
*/
namespace Fitter
{
	/*
	**	Types of solver kernels
	*/
	enum class FitKernel
	{
		DGESV  = 1,
		DGELS  = 2,
		DGGLSE = 3
	};
	
	/*
	**	Final pre-matrix construct for a charge site
	*/
    struct ChargeSite
    {
        ChargeSite();

        ChargeSite( const U32 index, const Vec3 & p, const Configuration::valueType t );

        U32 confIndex;
        Vec3 pos;
        Configuration::valueType type;
    };


    /*
	**	Final pre-matrix construct for a dipole site
	*/
    struct DipoleSite
    {
        DipoleSite();

        DipoleSite( const U32 index, const Vec3 & p, const Configuration::valueType t );

        U32 confIndex;
        Vec3 pos;
        Configuration::valueType type;
    };

    /*
	**	Final pre-matrix construct for a qpol site
	*/
    struct QuadrupoleSite
    {
        QuadrupoleSite();

        QuadrupoleSite( const U32 index, const Vec3 & p, const Configuration::valueType t );

        U32 confIndex;
        Vec3 pos;
        Configuration::valueType type;
    };
    
    /*
	**	Final pre-matrix construct for a constraint site
	*/
    struct Constraint
    {
    	Constraint();
    	
    	Constraint( const std::vector< U32 > &cols, const std::vector< F32 > &coefficients, const F32 ref ):
    	reference( ref ), matrixColumns( cols ), matrixCoefficients( coefficients )
    	{}
    	
    	F32 reference;
    	std::vector< U32 > matrixColumns;
    	std::vector< F32 > matrixCoefficients;
    };

    /*
	**	Helper function to determine the coefficient's numerator
	*/
    F64 DelComp( const Configuration::valueType, const Vec3 &i, const Vec3 &j );

    /*
	**	Gather the configuration sites in a final pre-matrix format
	*/
    Error::STATUS TabulateSites( const Configuration &conf, std::vector< ChargeSite > &charges, std::vector< DipoleSite > &dipoles, std::vector< QuadrupoleSite > &quadrupoles, std::map< std::pair< U32, Configuration::valueType >, U32 >& mColumnTranslation );   
    
    /*
	**	Gather the configuration constraints in a final pre-matrix format
	*/
    Error::STATUS TabulateConstraints( const Configuration &conf, std::vector< Constraint > & constraints, std::map< std::pair< U32, Configuration::valueType >, U32 >& mColumnTranslation );
   
    /*
	**	Generate the matrices from the pretabulated charges, dipoles and qpols
	*/
    Error::STATUS GenerateMatrices( const std::vector< ChargeSite > &charges, const std::vector< DipoleSite > &dipoles, const std::vector< QuadrupoleSite > &quadrupoles, const Field &field, multiMatrix &a, multiMatrix &b  );
   
    /*
	**	Add the preformatted constraints to the matrices
	*/
    Error::STATUS AddConstraints( std::vector< Constraint > &constraints, multiMatrix &a, multiMatrix &b, const bool addMirror  );
    
    /*
	**	In case of Resp append an identity matrix and add the correct force constants to the diagonal
	*/
    Error::STATUS AddRestraints( const Configuration &conf, multiMatrix &b, multiMatrix &c, std::map< std::pair< U32, Configuration::valueType >, U32 >& mColumnTranslation);
    
    /*
	**	Start function for the fitting functionality
	*/
    Error::STATUS FitSites( Configuration &conf, const Field &field, const bool verbose  );
};

#endif