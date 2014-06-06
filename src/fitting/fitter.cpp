#include "fitter.h"
#include "../common/types.h"
#include "../util/util.h"

#include <stdio.h>
#include <map>

#include "lapacke.h"

Fitter::ChargeSite::ChargeSite()
{

}

Fitter::ChargeSite::ChargeSite( const U32 index, const Vec3 & p, const Configuration::valueType t ) : confIndex( index ), pos( p ), type( t )
{

}

Fitter::DipoleSite::DipoleSite()
{

}

Fitter::DipoleSite::DipoleSite( const U32 index, const Vec3 & p, const Configuration::valueType t ) : confIndex( index ), pos( p ), type( t )
{

}

Fitter::QuadrupoleSite::QuadrupoleSite()
{

}

Fitter::QuadrupoleSite::QuadrupoleSite( const U32 index, const Vec3 & p, const Configuration::valueType t ) : confIndex( index ), pos( p ), type( t )
{

}

F64 Fitter::DelComp( const Configuration::valueType type, const Vec3 &i, const Vec3 &j )
{
    F64 delX = i.GetX() - j.GetX();
    F64 delY = i.GetY() - j.GetY();
    F64 delZ = i.GetZ() - j.GetZ();

    //
	//	Computes the numerator of Cij based on its type
	//
    switch( type )
    {
    case Configuration::valueType::charge:
    	
    	return 1.0;
    	
    case Configuration::valueType::dipoleX:
    	
        return delX;

    case Configuration::valueType::dipoleY:
    	
        return delY;

    case Configuration::valueType::dipoleZ:
    	
        return delZ;

    case Configuration::valueType::qd20:

        return delZ*delZ - 0.5 * (delX*delX + delY*delY );

    case Configuration::valueType::qd21c:

        return 1.73205081 * delX * delZ;

    case Configuration::valueType::qd21s:

        return 1.73205081*delY*delZ;

    case Configuration::valueType::qd22c:

        return (0.5 * 1.73205081 )*(delX*delX-delY*delY);

    case Configuration::valueType::qd22s:

        return 1.73205081*delX*delY;
    }
    
    return 0.0;
}

Error::STATUS Fitter::TabulateSites( const Configuration &conf, std::vector< ChargeSite > &charges, std::vector< DipoleSite > &dipoles, std::vector< QuadrupoleSite > &quadrupoles, std::map< std::pair< U32, Configuration::valueType >, U32 >& mColumnTranslation )
{    
    U32 column = 0;
   
    //
	//	Iterate over all sites and types and collect them in formatted charges, dipole and quadrupole arrays
	//  We also connect ID,Type - Column information so that we can easily add constraints and restraints
	//
	
    for ( U32 i=0; i < conf.Size(); ++i )
    {
        const Configuration::FitSite *site = conf.GetSite( i );
        const U32 fitFlags = site->fitFlags;
        const Vec3 & pos   = site->position;
        const U32 id = site->ID;
        
        if ( fitFlags & (0x01) )
        {
            charges.push_back( ChargeSite( i, pos, Configuration::valueType::charge ) );
            
            std::pair< U32, Configuration::valueType > hash = std::make_pair( id, Configuration::valueType::charge );
            mColumnTranslation.insert( std::make_pair( hash , column ) );
            column++;
        }
            
        if ( fitFlags & (0x02) )
        {
            dipoles.push_back( DipoleSite( i, pos, Configuration::valueType::dipoleX ) );
            
            std::pair< U32, Configuration::valueType > hash = std::make_pair( id, Configuration::valueType::dipoleX );
            mColumnTranslation.insert( std::make_pair( hash , column ) );
            column++;
        }
        
        if ( fitFlags & (0x04) )
        {
            dipoles.push_back( DipoleSite( i, pos, Configuration::valueType::dipoleY ) );
            
            std::pair< U32, Configuration::valueType > hash = std::make_pair( id, Configuration::valueType::dipoleY );
            mColumnTranslation.insert( std::make_pair( hash , column ) );
            column++;
        }
        
        if ( fitFlags & (0x08) )
        {
            dipoles.push_back( DipoleSite( i, pos, Configuration::valueType::dipoleZ ) );
            
            std::pair< U32, Configuration::valueType > hash = std::make_pair( id, Configuration::valueType::dipoleZ );
            mColumnTranslation.insert( std::make_pair( hash , column ) );
            column++;
        }
        
        if ( fitFlags & (0x10) )
        {
            quadrupoles.push_back( QuadrupoleSite(i, pos, Configuration::valueType::qd20 ) );
            
            std::pair< U32, Configuration::valueType > hash = std::make_pair( id, Configuration::valueType::qd20 );
            mColumnTranslation.insert( std::make_pair( hash , column ) );
            column++;
        }
        
        if ( fitFlags & (0x20) )
        {
            quadrupoles.push_back( QuadrupoleSite(i, pos, Configuration::valueType::qd21c ) );
            
            std::pair< U32, Configuration::valueType > hash = std::make_pair( id, Configuration::valueType::qd21c );
            mColumnTranslation.insert( std::make_pair( hash , column ) );
            column++;
        }
        
        if ( fitFlags & (0x40) )
        {
            quadrupoles.push_back( QuadrupoleSite(i, pos, Configuration::valueType::qd21s ) );
            
            std::pair< U32, Configuration::valueType > hash = std::make_pair( id, Configuration::valueType::qd21s );
            mColumnTranslation.insert( std::make_pair( hash , column ) );
            column++;
        }
        
        if ( fitFlags & (0x80) )
        {
            quadrupoles.push_back( QuadrupoleSite(i, pos, Configuration::valueType::qd22c ) );
            
            std::pair< U32, Configuration::valueType > hash = std::make_pair( id, Configuration::valueType::qd22c );
            mColumnTranslation.insert( std::make_pair( hash , column ) );
            column++;
        }
        
        if ( fitFlags & (0x100) )
        {
            quadrupoles.push_back( QuadrupoleSite(i, pos, Configuration::valueType::qd22s ) );
            
            std::pair< U32, Configuration::valueType > hash = std::make_pair( id, Configuration::valueType::qd22s );
            mColumnTranslation.insert( std::make_pair( hash , column ) );
            column++;
        }
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Fitter::TabulateConstraints( const Configuration &conf, std::vector< Constraint > & constraints, std::map< std::pair< U32, Configuration::valueType >, U32 >& mColumnTranslation )
{
	//
	//	We now convert different constraints to one general format
	//  For dipoles and qpols convert input units to compute units using constants 2.541748 and 1.3450351
	//
	
	for ( U32 i=0; i < conf.ValueConstr(); ++i )
    {
    	const Configuration::ValueConstraint *constr = conf.GetValueConstraint( i );
    	const U32 id = constr->ID;
    	const Configuration::valueType type = constr->type;
        F32 val = constr->value;
    	
    	//if dipole or quadrupole convert!
    	if ( type == Configuration::valueType::dipoleX || 
    		 type == Configuration::valueType::dipoleY ||
    		 type == Configuration::valueType::dipoleZ
    	   )
    	{
    		val /= 2.541748;
    	}
    	else if ( type == Configuration::valueType::qd20  || 
    		      type == Configuration::valueType::qd21c ||
    		   	  type == Configuration::valueType::qd21s ||
    		 	  type == Configuration::valueType::qd22c ||
    		  	  type == Configuration::valueType::qd22s
    	        )
    	{
    		val /= 1.3450351;
    	}
    	
    	//find the correct column;
    	std::map< std::pair< U32, Configuration::valueType >, U32 >::iterator it = mColumnTranslation.find( std::make_pair( id, type ) );
    	
    	if ( it == mColumnTranslation.end() )
    	{
    		Error::Warn( std::cout, "The combination of id " + Util::ToString( id ) + " and type " + Util::ToString( static_cast<U32>( type ) ) 
    			                  + "  is not a fitting combination, so cannot be constrained !" );
    		
    		return Error::STATUS::FAILED_CONSTR_R;
    	}
    	
    	std::vector< U32 > targetColumns;
    	std::vector< F32 > targetCoefficients;
    	targetColumns.push_back( it->second );
    	targetCoefficients.push_back( 1.0 );
    	
    	constraints.push_back( Constraint( targetColumns, targetCoefficients, val ) );
    }
    
    for ( U32 i=0; i < conf.SymConstr(); ++i )
    {
    	const Configuration::SymConstraint *constr = conf.GetSymConstraint( i );
    	const U32 id1 = constr->ID1;
    	const U32 id2 = constr->ID2;
    	const Configuration::valueType type = constr->type;

        std::map< std::pair< U32, Configuration::valueType >, U32 >::iterator it1 = mColumnTranslation.find( std::make_pair( id1, type ) );
        std::map< std::pair< U32, Configuration::valueType >, U32 >::iterator it2 = mColumnTranslation.find( std::make_pair( id2, type ) );

        if ( it1 == mColumnTranslation.end() )
        {
            Error::Warn( std::cout, "The combination of id " + Util::ToString( id1 ) + " and type " + Util::ToString( static_cast<U32>( type ) ) 
                + "  is not a fitting combination, so cannot be constrained !" );

            return Error::STATUS::FAILED_CONSTR_R;
        }

        if ( it2 == mColumnTranslation.end() )
        {
            Error::Warn( std::cout, "The combination of id " + Util::ToString( id2 ) + " and type " + Util::ToString( static_cast<U32>( type ) ) 
                + "  is not a fitting combination, so cannot be constrained !" );

            return Error::STATUS::FAILED_CONSTR_R;
        }

        std::vector< U32 > targetColumns;
        std::vector< F32 > targetCoefficients;
        targetColumns.push_back( it1->second );
        targetColumns.push_back( it2->second );
        targetCoefficients.push_back( 1.0 );
        targetCoefficients.push_back( -1.0 );

        constraints.push_back( Constraint( targetColumns, targetCoefficients, 0.0 ) );
    }

    for ( U32 i=0; i < conf.SumConstr(); ++i )
    {
        const Configuration::SumConstraint *constr = conf.GetSumConstraint( i );
       
        std::vector< U32 > targetColumns;
        std::vector< F32 > targetCoefficients;
        
        for ( U32 j=0; j < constr->locations.size(); j++ )
        {
            const std::pair< U32, Configuration::valueType > &pair = constr->locations[j];

            const U32 id = pair.first;
            const Configuration::valueType type = pair.second;

            //if dipole or quadrupole convert!
            F32 coefficientScaling = 1.0;
            
			if ( type == Configuration::valueType::dipoleX || 
				 type == Configuration::valueType::dipoleY ||
				 type == Configuration::valueType::dipoleZ
			   )
			{
				coefficientScaling = 2.541748;
			}
			else if ( type == Configuration::valueType::qd20  || 
					  type == Configuration::valueType::qd21c ||
					  type == Configuration::valueType::qd21s ||
					  type == Configuration::valueType::qd22c ||
					  type == Configuration::valueType::qd22s
					)
			{
				coefficientScaling = 1.3450351;
			}
            
            std::map< std::pair< U32, Configuration::valueType >, U32 >::iterator it = mColumnTranslation.find( std::make_pair( id, type ) );

            if ( it == mColumnTranslation.end() )
            {
                Error::Warn( std::cout, "The combination of id " + Util::ToString( id ) + " and type " + Util::ToString( static_cast<U32>( type ) ) 
                    + "  is not a fitting combination, so cannot be constrained !" );

                return Error::STATUS::FAILED_CONSTR_R;
            }
            
            targetColumns.push_back( it->second );
            targetCoefficients.push_back( coefficientScaling * 1.0 );
        }

        F32 reference = constr->value;

        constraints.push_back( Constraint( targetColumns, targetCoefficients, reference ) );
    }
	
	return Error::STATUS::OK;
}

Error::STATUS Fitter::GenerateMatrices( const std::vector< ChargeSite > &charges,  const std::vector< DipoleSite > &dipoles, const std::vector< QuadrupoleSite > &quadrupoles, const Field &field, multiMatrix &a, multiMatrix &b )
{
	//
	//	Generate a A( N x N ) matrix and B( N ) matrix set that fully describes the influences of all sites ( with all selected types ) on all grid points
	//  Most inputs have 130.000 gridpoints so obviously we cannot make a 130k x 130k matrix, so instead we merge all potential sites/
	//
	//	For n gridpoints;
	//
	//	Lets assume we are dealing with a charge i and a gridpoint j; The potential on j will then be ( q / dist( i, j ) );
	//  This equation can be split into an constant coefficient C = 1.0 / dist( i, j )  and a charge qi.
	//  The pot on j is then  pt = C * qi;
	//
	//  We can now use this given to express the sum of potential on all gridpoints by the charge qi.
	//
	//	totalPotQi =  ( SUM( j to n ){ 1.0 / dist( i, j ) } )  * qi;
	//  the constant coefficent that we can extract is C = SUM( j to n ){ 1.0 / dist( i, j ) }        
	//
	//  The same can be done for dipoles and quadrupoles resulting in a linear system of  q1 ... qk, d1 ... dl, q1 ... qm
	//  where k = total unique charge site, l = total unique dipole direction sites and m = total unique qpol direction sites
	//
	//	this system has coefficients C1 ... Ck, C1 ... Cl, C1 ... Cm 
	//  now this results in only one row for the matrix while we need N rows with rank( N )  ( unique )
	//  so instead we take for the coefficients for row I and column J
	//  C'_IJ = C_I * C_J;
	//
	//	Then we also have to adjust the value in B(i) to 
	//  B'(I) = SUM( j to n ){ potential( j ) / C_I }  
	//
	//	This trick allows use to describe the system in a N X N matrix, where N = number of fit variables
	//  while maintaining rank( N );
	//
	
    //
    // Charges
    //
    U32 row = 0;

    for( U32 i=0; i < charges.size(); ++i )
    {
        //
        //  Charge-Charge block
        //
        U32 column = 0;

        for( U32 j=0; j < charges.size(); ++j )
        {
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( n );

                F64 disti_2 = fpos.Distance2( charges[i].pos );
                F64 distj_2 = fpos.Distance2( charges[j].pos );

                sum += 1.0/Mathf::Sqrt( disti_2 * distj_2 );
            }
            a( row, column ) = 2.0 * sum;

            column++;
        }

        //
        //  Charge-Dipole block
        //
        for( U32 j=0; j < dipoles.size(); ++j )
        {
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( n );

                F64 disti_2 = fpos.Distance2( charges[i].pos );
                F64 distj_2 = fpos.Distance2( dipoles[j].pos );

                F64 delcomp = DelComp( dipoles[j].type, fpos, dipoles[j].pos );

                sum += delcomp / ( distj_2 * Mathf::Sqrt( disti_2 * distj_2 ) );

            }

            a( row, column )=2.0*sum;

            column++;
        }

        //
        //  Charge-Quadrupole block
        //

        for( U32 j=0; j < quadrupoles.size(); ++j )
        {
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( n );

                F64 disti_2 = fpos.Distance2( charges[i].pos );
                F64 distj_2 = fpos.Distance2( quadrupoles[j].pos );

                F64 delcomp = DelComp( quadrupoles[j].type, fpos, quadrupoles[j].pos );

                sum += delcomp / ( distj_2*distj_2*Mathf::Sqrt(disti_2*distj_2) );
            }
            a(row,column)=2.0*sum;

            column++;
        }

        //
        //  X_vector
        //
        F64 sum = 0.0;
        for ( U32 n=0; n < field.Size(); ++n )
        {
            Vec3 fpos = field.GetFieldPosition( n );

            F64 disti_2 = fpos.Distance2( charges[i].pos );

            sum += ( field.GetFieldDiff(n) )/Mathf::Sqrt( disti_2 );
        }

        b(row)=2.0*sum;

        row++;
    }

    //
    // Dipoles
    //
    for( U32 i=0; i < dipoles.size(); ++i )
    {
        //
        //  Dipole-Charge block
        //
        U32 column = 0;

        for( U32 j=0; j < charges.size(); ++j )
        {
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( n );

                F64 disti_2 = fpos.Distance2( dipoles[i].pos );
                F64 distj_2 = fpos.Distance2( charges[j].pos );

                F64 delcomp = DelComp( dipoles[i].type, fpos, dipoles[j].pos );

                sum += delcomp / ( disti_2 * Mathf::Sqrt( disti_2 * distj_2 ) );
            }
            a( row, column ) = 2.0 * sum;

            column++;
        }

        //
        //  Dipole-Dipole block
        //
        for( U32 j=0; j < dipoles.size(); ++j )
        {
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( n );

                F64 disti_2 = fpos.Distance2( dipoles[i].pos );
                F64 distj_2 = fpos.Distance2( dipoles[j].pos );

                F64 delcompi = DelComp( dipoles[i].type, fpos, dipoles[i].pos );
                F64 delcompj = DelComp( dipoles[j].type, fpos, dipoles[j].pos );

                sum += delcompi * delcompj / ( disti_2 * distj_2 * Mathf::Sqrt( disti_2 * distj_2 ) );

            }

            a( row, column )=2.0*sum;

            column++;
        }

        //
        //  Dipole-Quadrupole block
        //
        for( U32 j=0; j < quadrupoles.size(); ++j )
        {
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( n );

                F64 disti_2 = fpos.Distance2( dipoles[i].pos );
                F64 distj_2 = fpos.Distance2( quadrupoles[j].pos );

                F64 delcompi = DelComp( dipoles[i].type, fpos, dipoles[i].pos );
                F64 delcompj = DelComp( quadrupoles[j].type, fpos, quadrupoles[j].pos );

                sum += delcompi * delcompj / ( disti_2 * distj_2 * distj_2 * Mathf::Sqrt(disti_2*distj_2) );
            }
            a(row,column)=2.0*sum;

            column++;
        }

        //
        //  X_vector
        //
        F64 sum = 0.0;
        for ( U32 n=0; n < field.Size(); ++n )
        {
            Vec3 fpos = field.GetFieldPosition( n );

            F64 disti_2 = fpos.Distance2( dipoles[i].pos );
            F64 delcompi = DelComp( dipoles[i].type, fpos, dipoles[i].pos );

            sum += ( field.GetFieldDiff(n) * delcompi ) / ( disti_2 * Mathf::Sqrt( disti_2 ) );
        }

        b(row)=2.0*sum;

        row++;
    }

    //
    // Quadrupoles
    //
    for( U32 i=0; i < quadrupoles.size(); ++i )
    {
        U32 column = 0;

        //
        //  Quadrupole-Charge block
        //
        for( U32 j=0; j < charges.size(); ++j )
        {
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( n );

                F64 disti_2 = fpos.Distance2( quadrupoles[i].pos );
                F64 distj_2 = fpos.Distance2( charges[j].pos );

                F64 delcomp = DelComp( quadrupoles[i].type, fpos, quadrupoles[i].pos );

                sum += delcomp / ( disti_2 * disti_2 * Mathf::Sqrt( disti_2 * distj_2 ) );
            }

            a( row, column ) = 2.0 * sum;

            column++;
        }

        //
        //  Quadrupole-Dipole block
        //
        for( U32 j=0; j < dipoles.size(); ++j )
        {
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( n );

                F64 disti_2 = fpos.Distance2( quadrupoles[i].pos );
                F64 distj_2 = fpos.Distance2( dipoles[j].pos );

                F64 delcompi = DelComp( quadrupoles[i].type, fpos, quadrupoles[i].pos );
                F64 delcompj = DelComp( dipoles[j].type, fpos, dipoles[j].pos );

                sum += delcompi * delcompj / ( disti_2 * disti_2 * distj_2 * Mathf::Sqrt( disti_2 * distj_2 ) );

            }

            a( row, column )=2.0*sum;

            column++;
        }

        //
        //  Quadrupole-Quadrupole block
        //
        for( U32 j=0; j < quadrupoles.size(); ++j )
        {
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( n );

                F64 disti_2 = fpos.Distance2( quadrupoles[i].pos );
                F64 distj_2 = fpos.Distance2( quadrupoles[j].pos );

                F64 delcompi = DelComp( quadrupoles[i].type, fpos, quadrupoles[i].pos );
                F64 delcompj = DelComp( quadrupoles[j].type, fpos, quadrupoles[j].pos );

                sum += delcompi * delcompj / ( disti_2 * disti_2 * distj_2 * distj_2 * Mathf::Sqrt(disti_2*distj_2) );
            }
            a(row,column)=2.0*sum;

            column++;
        }

        //
        //  X_vector
        //
        F64 sum = 0.0;
        for ( U32 n=0; n < field.Size(); ++n )
        {
            Vec3 fpos = field.GetFieldPosition( n );

            F64 disti_2 = fpos.Distance2( quadrupoles[i].pos );
            F64 delcomp = DelComp( quadrupoles[i].type, fpos, quadrupoles[i].pos );

            sum += ( field.GetFieldDiff(n) * delcomp ) / ( disti_2 * disti_2 * Mathf::Sqrt( disti_2 ) );
        }

        b(row)=2.0*sum;

        row++;
    }


    return Error::STATUS::OK;
}

Error::STATUS Fitter::AddConstraints( std::vector< Constraint > &constraints, multiMatrix &a, multiMatrix &b )
{
	U32 strideCol = a.Columns();
	U32 strideRow = a.Rows();
	
	//add the additional rows and columns to accomidate the extra constraints;
	
	//
    //   Init the extra rows and colums to zero
    //
	for ( U32 i=0; i < a.Rows(); ++i )
	{
		for ( U32 j=0; j < constraints.size(); ++j )
		{
			a( i, strideCol+j ) = 0.0;
		}
	}
	
	for ( U32 i=0; i < constraints.size(); ++i )
	{
		for ( U32 j=0; j < a.Columns(); ++j )
		{
			a( strideRow+i, j ) = 0.0;
		}
		
		b( strideRow+i ) = 0.0;
	}
	
	//add the actual constraints
	for ( U32 i=0; i < constraints.size(); ++i )
	{
		const Constraint &constr = constraints[i];
		
		for ( U32 j=0; j < constr.matrixColumns.size(); ++j )
		{
			U32 col= constr.matrixColumns[j];
			F32 cf = constr.matrixCoefficients[j];
			
			a( strideRow+i, col ) = cf;
			a( col, strideRow+i ) = cf;
		}
		
		b( strideRow+i ) = constr.reference;
	}
	
	return Error::STATUS::OK;
}

Error::STATUS Fitter::AddRestraints( const Configuration &conf, std::vector< Constraint > &constraints, multiMatrix &q, multiMatrix &c, 
	                                 std::map< std::pair< U32, Configuration::valueType >, U32 >& mColumnTranslation )
{
	U32 rowStride = q.Rows();
	
	
	//
    //   Init the extra rows to zero
    //
	for ( U32 i=0; i < ( mColumnTranslation.size() + constraints.size() ); ++i )
	{
		for ( U32 j=0; j < ( mColumnTranslation.size() + constraints.size() ); ++j )
		{
			q( rowStride+i, j ) = 0.0;
		}
		
		c( rowStride+i ) = 0.0;
	}
	
	//
    //   Add the resp force constants to the diagonal of the extra rows
    //
	for ( U32 i=0; i < conf.RespRestr(); ++i )
	{
		const Configuration::RespRestraint *restr = conf.GetRespRestraint( i );
		const Configuration::valueType type = restr->type;
		const U32 id = restr->ID;
		const F32 intensity = restr->intensity;
		F32 reference = restr->refValue;
		
		//if dipole or quadrupole convert!
    	if ( type == Configuration::valueType::dipoleX || 
    		 type == Configuration::valueType::dipoleY ||
    		 type == Configuration::valueType::dipoleZ
    	   )
    	{
    		reference /= 2.541748;
    	}
    	else if ( type == Configuration::valueType::qd20  || 
    		      type == Configuration::valueType::qd21c ||
    		   	  type == Configuration::valueType::qd21s ||
    		 	  type == Configuration::valueType::qd22c ||
    		  	  type == Configuration::valueType::qd22s
    	        )
    	{
    		reference /= 1.3450351;
    	}
		
		//find the correct column;
    	std::map< std::pair< U32, Configuration::valueType >, U32 >::iterator it = mColumnTranslation.find( std::make_pair( id, type ) );
    	
    	if ( it == mColumnTranslation.end() )
    	{
    		Error::Warn( std::cout, "The combination of id " + Util::ToString( id ) + " and type " + Util::ToString( static_cast<U32>( type ) ) 
    			                  + "  is not a fitting combination, so cannot be restrained !" );
    		
    		return Error::STATUS::FAILED_CONSTR_R;
    	}
		
    	U32 index = it->second;
    	
		q( rowStride+i, index ) =  1.0f * intensity;
		c( rowStride+i ) = reference;
	}
	
	return Error::STATUS::OK;
}

Error::STATUS Fitter::FitSites( Configuration &conf, const Field &field )
{
    //
    //  Tabulate
    //  
    std::vector< ChargeSite > charges;
    std::vector< DipoleSite > dipoles;
    std::vector< QuadrupoleSite > quadrupoles;

    //map that we will use for the constraint to easily turn an ID and type into the correct column;
    //std::pair< U32, Configuration::valueType > is turned into a hash
    std::map< std::pair< U32, Configuration::valueType >, U32 > mColumnTranslation;
	
	TabulateSites( conf, charges, dipoles, quadrupoles, mColumnTranslation );
    
    //
    //	Translate the constraints to an easy format
    //
    std::vector< Constraint > constraints;
    
    TabulateConstraints( conf, constraints,  mColumnTranslation );
     
    multiMatrix a( 200, 200 ), b( 200 );
    multiMatrix x(200);
    
    //init x
    for ( U32 i=0; i < 200; i++ )
    {
        x(i) = 0.0f;
    } 
    
    //
    //  Construct AX = b
    //
    GenerateMatrices( charges, dipoles, quadrupoles, field, a, b );   

    
    //
    //  Determine what kernel from lapack to use, based on the availability of constraints and restraints
    //
    FitKernel kernel;
    
    if ( constraints.size() > 0 && conf.RespRestr() > 0 )
    {
    	Error::Warn( std::cout, "The system currently does not support both constraints and restraints at the same time!" );
    	
    	return Error::STATUS::FAILED_UNSUP_PARAM_COMBO;
    }
    else if ( constraints.size() > 0 )
    {
    	kernel = FitKernel::DGESV;
    }
    else if ( conf.RespRestr() > 0 )
    {
    	kernel = FitKernel::DGELS;
    }
    //switch to default
    else
    {
    	kernel = FitKernel::DGESV;
    }
    
    S32 result;
    
    //switch to the normal compute kernel
    if ( kernel == FitKernel::DGESV )
    {
    	Console::Report( std::cout, "[SOLVE_KERNEL]\n" );
    	Console::Report( std::cout, "\tLAPACKE_DGESV\n" );
    	Console::Report( std::cout, "[END]" );
    	
    	//add the constraints to matrices and b
    	AddConstraints( constraints, a, b );  
    	
    	a.Report("MATRIX_A");
		b.Report("MATRIX_B");
    	
    	U32 LWORK = 4 * 3 * a.Columns();  
    	S32 *ipiv = new S32[ LWORK  ];
    	
    	F64 *abuffer = a.GetBuffer();
		F64 *bBuffer = b.GetBuffer();
    	
    	result = LAPACKE_dgesv( LAPACK_ROW_MAJOR, a.Columns(), 1, abuffer, a.Columns(), ipiv, bBuffer, 1 );    
    	
    	delete [] ipiv;
    	
    	x = b;
    }
    //switch to the LSE compute kernel
    else
    {
    	Console::Report( std::cout, "[SOLVE_KERNEL]\n" );
    	Console::Report( std::cout, "\tLAPACKE_DGELS\n" );
    	Console::Report( std::cout, "[END]" );
    	
    	multiMatrix q( 200, 200 ), c( 200 );
    	
    	//add the restraints to matrices and b
    	AddRestraints( conf, constraints, a, b, mColumnTranslation );
    	
    	a.Report("MATRIX_A");
		b.Report("MATRIX_B");
    	
		F64 *abuffer = a.GetBuffer();
		F64 *bBuffer = b.GetBuffer();
		
    	result = LAPACKE_dgels( LAPACK_ROW_MAJOR, 'N',  a.Rows(), a.Columns(), 1, abuffer, a.Columns(), bBuffer, 1 );

    	x = b;
    }

    //
    //	Test the output from lapack; If result < 0; then 
    //                                   
    // 									illegal input, 
    //
    // 							      else if result > 0; then 
    // 							        
    // 							        singular matrix 
    // 							       
    // 							      else OK!
    //
    
    if ( result != 0 )
    {
    	if ( result > 0 )
    	{
    		Error::Warn( std::cout, "cannot fit because the generated matrix is signular!" );
    		
    		return Error::STATUS::FAILED_FIT;	
    	}
    	
    	if ( result < 0 )
    	{
    		Error::Warn( std::cout, "cannot fit because the generated matrix contains an illigal input value!" );
    		
    		return Error::STATUS::FAILED_FIT;	
    	}
    }
    
    // Copy to the result buffer
    // For lapack we needed c-style arrays so now we copy it back to my easy matrix format
    x.BufferSwap();

    x.Report("MATRIX_X");
    
    //convertion constants for the correct output format
    F32 f=2.541748;
    F32 f1=1.3450351;

    U32 stride = 0;
    //Set the fitted results in the configuration
    for ( U32 i=0; i < charges.size(); ++i )
    {
        conf.GetSiteMod( charges[i].confIndex )->SetValue( charges[i].type, x(stride) );
    
        stride++;
    }

    for ( U32 i=0; i < dipoles.size(); ++i )
    {
        conf.GetSiteMod( dipoles[i].confIndex )->SetValue( dipoles[i].type, f * x(stride) );
        
        stride++;
    }
 
    for ( U32 i=0; i < quadrupoles.size(); ++i )
    {
        conf.GetSiteMod( quadrupoles[i].confIndex )->SetValue( quadrupoles[i].type, f1 * x(stride) );

        stride++;   
    }
    
    return Error::STATUS::OK;
}
