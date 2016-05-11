#include "fitter.h"
#include "../common/types.h"
#include "../util/util.h"

#include <stdio.h>
#include <map>

#include "lapacke.h"

#include "../system/constants.h"

Fitter::ChargeSite::ChargeSite()
{

}

Fitter::ChargeSite::ChargeSite( const U32 collection,const U32 index, const U32 stride, const Vec3 & p, const Configuration::valueType t ) :
collIndex( collection), confIndex( index ), stride( stride ), pos( p ), type( t )
{

}

Fitter::DipoleSite::DipoleSite()
{

}

Fitter::DipoleSite::DipoleSite( const U32 collection,const U32 index, const U32 stride, const Vec3 & p, const Configuration::valueType t ) :
collIndex( collection),confIndex( index ), stride( stride ), pos( p ), type( t )
{

}

Fitter::QuadrupoleSite::QuadrupoleSite()
{

}

Fitter::QuadrupoleSite::QuadrupoleSite( const U32 collection,const U32 index, const U32 stride, const Vec3 & p, const Configuration::valueType t ) :
collIndex( collection), confIndex( index ), stride( stride ), pos( p ), type( t )
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

Fitter::SiteHash Fitter::GenerateSiteHash( const U32 collection, const U32 ID, Configuration::valueType type )
{
    const U64 part1 = (U64)ID;
    const U64 part2 = ( (U64)collection ) << 32;
                           
    std::pair< U64, Configuration::valueType > hash = std::make_pair( part1 | part2, type );
    
    return hash;
}

Error::STATUS Fitter::TabulateSites( const Configuration &conf, std::vector< ChargeSite > &charges, std::vector< DipoleSite > &dipoles, std::vector< QuadrupoleSite > &quadrupoles, std::map< SiteHash, U32 >& columnTranslation )
{    
    U32 column = 0;
   
    // Iterate collections!
    for ( U32 coll=0; coll < conf.NumCollections(); ++coll )
    {
    
        //
        //	Iterate over all sites and types and collect them in formatted charges, dipole and quadrupole arrays
        //  We also connect ID,Type - Column information so that we can easily add constraints and restraints
        //
	
        for ( U32 i=0; i < conf.FitSites(coll); ++i )
        {
            const Configuration::FitSite *site = conf.GetSite( coll, i );
            const U32 fitFlags = site->fitFlags;
            const Vec3 & pos   = site->position;
            const U32 id = site->ID;
        
            if ( fitFlags & (0x01) )
            {
                charges.push_back( ChargeSite( coll, i, column, pos, Configuration::valueType::charge ) );
            
                const SiteHash hash = GenerateSiteHash( coll, id, Configuration::valueType::charge );
                columnTranslation.insert( std::make_pair( hash , column ) );
                column++;
            }
            
            if ( fitFlags & (0x02) )
            {
                dipoles.push_back( DipoleSite( coll, i, column, pos, Configuration::valueType::dipoleX ) );
            
                const SiteHash hash = GenerateSiteHash( coll, id, Configuration::valueType::dipoleX );
                columnTranslation.insert( std::make_pair( hash , column ) );
                column++;
            }
        
            if ( fitFlags & (0x04) )
            {
                dipoles.push_back( DipoleSite( coll, i, column, pos, Configuration::valueType::dipoleY ) );
            
                const SiteHash hash = GenerateSiteHash( coll, id, Configuration::valueType::dipoleY );
                columnTranslation.insert( std::make_pair( hash , column ) );
                column++;
            }
        
            if ( fitFlags & (0x08) )
            {
                dipoles.push_back( DipoleSite( coll, i, column, pos, Configuration::valueType::dipoleZ ) );
            
                const SiteHash hash = GenerateSiteHash( coll, id, Configuration::valueType::dipoleZ );
                columnTranslation.insert( std::make_pair( hash , column ) );
                column++;
            }
        
            if ( fitFlags & (0x10) )
            {
                quadrupoles.push_back( QuadrupoleSite( coll, i, column, pos, Configuration::valueType::qd20 ) );
            
                const SiteHash hash = GenerateSiteHash( coll, id, Configuration::valueType::qd20 );
                columnTranslation.insert( std::make_pair( hash , column ) );
                column++;
            }
        
            if ( fitFlags & (0x20) )
            {
                quadrupoles.push_back( QuadrupoleSite( coll, i, column, pos, Configuration::valueType::qd21c ) );
            
                const SiteHash hash = GenerateSiteHash( coll, id, Configuration::valueType::qd21c );
                columnTranslation.insert( std::make_pair( hash , column ) );
                column++;
            }
        
            if ( fitFlags & (0x40) )
            {
                quadrupoles.push_back( QuadrupoleSite( coll, i, column, pos, Configuration::valueType::qd21s ) );
            
                const SiteHash hash = GenerateSiteHash( coll, id, Configuration::valueType::qd21s );
                columnTranslation.insert( std::make_pair( hash , column ) );
                column++;
            }
        
            if ( fitFlags & (0x80) )
            {
                quadrupoles.push_back( QuadrupoleSite( coll, i, column, pos, Configuration::valueType::qd22c ) );
                
                const SiteHash hash = GenerateSiteHash( coll, id, Configuration::valueType::qd22c );
                columnTranslation.insert( std::make_pair( hash , column ) );
                column++;
            }
        
            if ( fitFlags & (0x100) )
            {
                quadrupoles.push_back( QuadrupoleSite( coll, i, column, pos, Configuration::valueType::qd22s ) );
            
                const SiteHash hash = GenerateSiteHash( coll, id, Configuration::valueType::qd22s );
                columnTranslation.insert( std::make_pair( hash , column ) );
                column++;
            }
        }
    }
        
    return Error::STATUS::OK;
}

Error::STATUS Fitter::TabulateConstraints( const Configuration &conf, const Field &field, std::vector< Constraint > & constraints, std::map< SiteHash, U32 >& columnTranslation )
{
    // Iterate collections!
    for ( U32 coll=0; coll < conf.NumCollections(); ++coll )
    {
    
        //
        //	We now convert different constraints to one general format
        //  For dipoles and qpols convert input units to compute units using constants eBohr_D and eBohr2_DA
        //
	
        for ( U32 i=0; i < conf.ValueConstr(coll); ++i )
        {
            const Configuration::ValueConstraint *constr = conf.GetValueConstraint( coll, i );
            const U32 id = constr->ID;
            const Configuration::valueType type = constr->type;
            F32 val = constr->value;
    	
            //if dipole or quadrupole convert!
            if ( type == Configuration::valueType::dipoleX ||
                 type == Configuration::valueType::dipoleY ||
                 type == Configuration::valueType::dipoleZ
                )
            {
                val /= Constants::eBohr_D;
            }
            else if ( type == Configuration::valueType::qd20  ||
                      type == Configuration::valueType::qd21c ||
                      type == Configuration::valueType::qd21s ||
                      type == Configuration::valueType::qd22c ||
    		  	      type == Configuration::valueType::qd22s
                     )
            {
                val /= Constants::eBohr2_DA;
            }
    	
            //find the correct column;
            const SiteHash hash = GenerateSiteHash( coll, id, type );
            std::map< SiteHash, U32 >::iterator it = columnTranslation.find( hash );
    	
            if ( it == columnTranslation.end() )
            {
                Error::Warn( std::cout, "The combination of id " + Util::ToString( id ) + " and type " + Util::ToString( static_cast<U32>( type ) )
                                      + "is not a fitting combination, so cannot be constrained !" );
    		
                return Error::STATUS::FAILED_CONSTR_R;
            }
    	
            std::vector< U32 > targetColumns;
            std::vector< F32 > targetCoefficients;
            targetColumns.push_back( it->second );
            targetCoefficients.push_back( 1.0 );
    	
            constraints.push_back( Constraint( targetColumns, targetCoefficients, val ) );
        }
    
        for ( U32 i=0; i < conf.SymConstr(coll); ++i )
        {
            const Configuration::SymConstraint *constr = conf.GetSymConstraint( coll, i );
            const U32 id1 = constr->ID1;
            const U32 id2 = constr->ID2;
            const Configuration::valueType type1 = constr->type1;
            const Configuration::valueType type2 = constr->type2;
    	
            const SiteHash hash1 = GenerateSiteHash( coll, id1, type1 );
            const SiteHash hash2 = GenerateSiteHash( coll, id2, type2 );
            std::map< SiteHash, U32 >::iterator it1 = columnTranslation.find( hash1 );
            std::map< SiteHash, U32 >::iterator it2 = columnTranslation.find( hash2 );
            
            if ( it1 == columnTranslation.end() )
            {
                Error::Warn( std::cout, "The combination of id " + Util::ToString( id1 ) + " and type " + Util::ToString( static_cast<U32>( type1 ) )
                                       + "  is not a fitting combination, so cannot be constrained !" );

                return Error::STATUS::FAILED_CONSTR_R;
            }

            if ( it2 == columnTranslation.end() )
            {
                Error::Warn( std::cout, "The combination of id " + Util::ToString( id2 ) + " and type " + Util::ToString( static_cast<U32>( type2 ) )
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

        for ( U32 i=0; i < conf.SumConstr(coll); ++i )
        {
            const Configuration::SumConstraint *constr = conf.GetSumConstraint( coll, i );
       
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
                    coefficientScaling = Constants::eBohr_D;
                }
                else if ( type == Configuration::valueType::qd20  ||
                          type == Configuration::valueType::qd21c ||
                          type == Configuration::valueType::qd21s ||
                          type == Configuration::valueType::qd22c ||
                          type == Configuration::valueType::qd22s
                         )
                {
                    coefficientScaling = Constants::eBohr2_DA;
                }
            
                const SiteHash hash = GenerateSiteHash( coll, id, type );
                std::map< SiteHash, U32 >::iterator it = columnTranslation.find( hash );

                if ( it == columnTranslation.end() )
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
    }
    
    //
    // Add union constraint
    //
    
    for ( U32 i=0; i < conf.UnionContr(); ++i )
    {
        const Configuration::UnionContraint *constr = conf.GetUnionConstraint(i);
        
        std::vector< U32 > targetColumns;
        std::vector< F32 > targetCoefficients;
        
        const U32 coll_i = constr->collection_i;
        const U32 id_i = constr->ID_i;
        const U32 siteIndex_i = conf.GetIndexFromID( coll_i, id_i );
    
        const U32 id_j = constr->ID_j;
        
        if ( siteIndex_i == conf.UnknownID() )
        {
            Error::Warn( std::cout, "ID " + Util::ToString( id_i ) + " for [UNION] constraint is invalid!" );
            
            return Error::STATUS::FAILED_CONSTR_R;
        }
        
        
        // Iterate j coll range
        for ( U32 coll_j=constr->collectionStart_j; coll_j <= constr->collectionEnd_j; ++coll_j )
        {
            targetColumns.clear();
            targetCoefficients.clear();
            
            const U32 siteIndex_j = conf.GetIndexFromID( coll_j, id_j );
            
            if ( siteIndex_j == conf.UnknownID() )
            {
                Error::Warn( std::cout, "ID " + Util::ToString( id_j ) + " for [UNION] constraint is invalid!" );
                
                return Error::STATUS::FAILED_CONSTR_R;
            }
            
            // Iterate all types
            for ( Configuration::valueType type : constr->types )
            {
                //if dipole or quadrupole convert!
                F32 coefficientScaling_i = 1.0;
                F32 coefficientScaling_j = 1.0;
                
                if ( type == Configuration::valueType::dipoleX )
                {
                    if ( !field.ContainsElectricField() )
                    {
                        Error::Warn( std::cout, "Tried to apply union constraints to dipoles, but electric fields are unknown at the fit sites!" );
                        
                        return Error::STATUS::FAILED_CONSTR_R;
                    }
                    
                    const Vec3 &field_i = field.GetEfield( coll_i, siteIndex_i );
                    const Vec3 &field_j = field.GetEfield( coll_j, siteIndex_j );
                    
                    coefficientScaling_i = 1.0 / field_i[0];
                    coefficientScaling_j = 1.0 / field_j[0];
                }
                else if ( type == Configuration::valueType::dipoleY )
                {
                    if ( !field.ContainsElectricField() )
                    {
                        Error::Warn( std::cout, "Tried to apply union constraints to dipoles, but electric fields are unknown at the fit sites!" );
                        
                        return Error::STATUS::FAILED_CONSTR_R;
                    }
                    
                    const Vec3 &field_i = field.GetEfield( coll_i, siteIndex_i );
                    const Vec3 &field_j = field.GetEfield( coll_j, siteIndex_j );
                    
                    coefficientScaling_i = 1.0 / field_i[1];
                    coefficientScaling_j = 1.0 / field_j[1];
                }
                else if ( type == Configuration::valueType::dipoleZ )
                {
                    if ( !field.ContainsElectricField() )
                    {
                        Error::Warn( std::cout, "Tried to apply union constraints to dipoles, but electric fields are unknown at the fit sites!" );
                        
                        return Error::STATUS::FAILED_CONSTR_R;
                    }
                    
                    const Vec3 &field_i = field.GetEfield( coll_i, siteIndex_i );
                    const Vec3 &field_j = field.GetEfield( coll_j, siteIndex_j );
                    
                    coefficientScaling_i = 1.0 / field_i[2];
                    coefficientScaling_j = 1.0 / field_j[2];
                }
                else if ( type == Configuration::valueType::qd20  ||
                         type == Configuration::valueType::qd21c ||
                         type == Configuration::valueType::qd21s ||
                         type == Configuration::valueType::qd22c ||
                         type == Configuration::valueType::qd22s
                         )
                {
                    Error::Warn( std::cout, "Qpols not supported for union constraints!" );
                    
                    return Error::STATUS::FAILED_CONSTR_R;
                }
                
                const SiteHash hash_i = GenerateSiteHash( coll_i, id_i, type );
                const SiteHash hash_j = GenerateSiteHash( coll_j, id_j, type );
                
                std::map< SiteHash, U32 >::iterator it_i = columnTranslation.find( hash_i );
                std::map< SiteHash, U32 >::iterator it_j = columnTranslation.find( hash_j );
                
                if ( it_i == columnTranslation.end() )
                {
                    Error::Warn( std::cout, "The combination of id " + Util::ToString( id_i ) + " and type " + Util::ToString( static_cast<U32>( type ) )
                                + "  is not a fitting combination, so cannot be constrained !" );
                    
                    return Error::STATUS::FAILED_CONSTR_R;
                }
                
                if ( it_j == columnTranslation.end() )
                {
                    Error::Warn( std::cout, "The combination of id " + Util::ToString( id_j ) + " and type " + Util::ToString( static_cast<U32>( type ) )
                                + "  is not a fitting combination, so cannot be constrained !" );
                    
                    return Error::STATUS::FAILED_CONSTR_R;
                }
                
                targetColumns.push_back( it_i->second );
                targetColumns.push_back( it_j->second );
                targetCoefficients.push_back( coefficientScaling_i *  1.0 );
                targetCoefficients.push_back( coefficientScaling_j * -1.0 );
            }
            
            constraints.push_back( Constraint( targetColumns, targetCoefficients, 0.0 ) );
        }
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

    for( const ChargeSite &charge_i : charges )
    {
        const U32 row = charge_i.stride;
        
        //
        //  Charge-Charge block
        //
        for( const ChargeSite &charge_j : charges )
        {
            const U32 column = charge_j.stride;
            
            // Ignore inter collection contacts
            if ( charge_i.collIndex != charge_j.collIndex )
            {
                continue;
            }
            
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(charge_j.collIndex); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( charge_j.collIndex, n );
                
                F64 disti_2 = fpos.Distance2( charge_i.pos );
                F64 distj_2 = fpos.Distance2( charge_j.pos );
                
                sum += 1.0/Mathf::Sqrt( disti_2 * distj_2 );
            }
            a( row, column ) += 2.0 * sum;
        }
        
        //
        //  Charge-Dipole block
        //
        for( const DipoleSite &dipole_j : dipoles )
        {
            const U32 column = dipole_j.stride;
            
            // Ignore inter collection contacts
            if ( charge_i.collIndex != dipole_j.collIndex )
            {
                continue;
            }
            
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(dipole_j.collIndex); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( dipole_j.collIndex, n );
                
                F64 disti_2 = fpos.Distance2( charge_i.pos );
                F64 distj_2 = fpos.Distance2( dipole_j.pos );
                
                F64 delcomp = DelComp( dipole_j.type, fpos, dipole_j.pos );
                
                sum += delcomp / ( distj_2 * Mathf::Sqrt( disti_2 * distj_2 ) );
                
            }
            
            a( row, column ) += 2.0*sum;
        }
        
        //
        //  Charge-Quadrupole block
        //
        
        for( const QuadrupoleSite &qpol_j : quadrupoles )
        {
            const U32 column = qpol_j.stride;
            
            // Ignore inter collection contacts
            if ( charge_i.collIndex != qpol_j.collIndex )
            {
                continue;
            }
            
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(qpol_j.collIndex); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( qpol_j.collIndex, n );
                
                F64 disti_2 = fpos.Distance2( charge_i.pos );
                F64 distj_2 = fpos.Distance2( qpol_j.pos );
                
                F64 delcomp = DelComp( qpol_j.type, fpos, qpol_j.pos );
                
                sum += delcomp / ( distj_2*distj_2*Mathf::Sqrt(disti_2*distj_2) );
            }
            a(row,column) += 2.0*sum;
        }
        
        //
        //  X_vector
        //
        F64 sum = 0.0;
        for ( U32 n=0; n < field.Size(charge_i.collIndex); ++n )
        {
            Vec3 fpos = field.GetFieldPosition( charge_i.collIndex, n );
            
            F64 disti_2 = fpos.Distance2( charge_i.pos );
            
            sum += ( field.GetFieldDiff(charge_i.collIndex, n) )/Mathf::Sqrt( disti_2 );
        }
        
        b(row) += 2.0*sum;
    }
    
    //
    // Dipoles
    //
    for( const DipoleSite &dipole_i : dipoles )
    {
        const U32 row = dipole_i.stride;
        
        //
        //  Dipole-Charge block
        //
        U32 column = 0;
        
        for( const ChargeSite &charge_j : charges )
        {
            const U32 column = charge_j.stride;
            
            // Ignore inter collection contacts
            if ( dipole_i.collIndex != charge_j.collIndex )
            {
                continue;
            }
            
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(charge_j.collIndex); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( charge_j.collIndex, n );
                
                F64 disti_2 = fpos.Distance2( dipole_i.pos );
                F64 distj_2 = fpos.Distance2( charge_j.pos );
                
                // TO CHECK, was first dipole_j.pos
                F64 delcomp = DelComp( dipole_i.type, fpos, dipole_i.pos );
                
                sum += delcomp / ( disti_2 * Mathf::Sqrt( disti_2 * distj_2 ) );
            }
            
            a( row, column )  += 2.0 * sum;
        }
        
        //
        //  Dipole-Dipole block
        //
        for( const DipoleSite &dipole_j : dipoles )
        {
            const U32 column = dipole_j.stride;
            
            // Ignore inter collection contacts
            if ( dipole_i.collIndex != dipole_j.collIndex )
            {
                continue;
            }
            
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(dipole_j.collIndex); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( dipole_j.collIndex, n );
                
                F64 disti_2 = fpos.Distance2( dipole_i.pos );
                F64 distj_2 = fpos.Distance2( dipole_j.pos );
                
                F64 delcompi = DelComp( dipole_i.type, fpos, dipole_i.pos );
                F64 delcompj = DelComp( dipole_j.type, fpos, dipole_j.pos );
                
                sum += delcompi * delcompj / ( disti_2 * distj_2 * Mathf::Sqrt( disti_2 * distj_2 ) );
                
            }
            
            a( row, column ) += 2.0*sum;
        }
        
        //
        //  Dipole-Quadrupole block
        //
        for ( const QuadrupoleSite &qpol_j : quadrupoles )
        {
            const U32 column = qpol_j.stride;

            // Ignore inter collection contacts
            if ( dipole_i.collIndex != qpol_j.collIndex )
            {
                continue;
            }
            
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(qpol_j.collIndex); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( qpol_j.collIndex, n );
                
                F64 disti_2 = fpos.Distance2( dipole_i.pos );
                F64 distj_2 = fpos.Distance2( qpol_j.pos );
                
                F64 delcompi = DelComp( dipole_i.type, fpos, dipole_i.pos );
                F64 delcompj = DelComp( qpol_j.type, fpos, qpol_j.pos );
                
                sum += delcompi * delcompj / ( disti_2 * distj_2 * distj_2 * Mathf::Sqrt(disti_2*distj_2) );
            }
            a(row,column) += 2.0*sum;
        }
        
        //
        //  X_vector
        //
        F64 sum = 0.0;
        for ( U32 n=0; n < field.Size(dipole_i.collIndex); ++n )
        {
            Vec3 fpos = field.GetFieldPosition( dipole_i.collIndex, n );
            
            F64 disti_2 = fpos.Distance2( dipole_i.pos );
            F64 delcompi = DelComp( dipole_i.type, fpos, dipole_i.pos );
            
            sum += ( field.GetFieldDiff(dipole_i.collIndex, n) * delcompi ) / ( disti_2 * Mathf::Sqrt( disti_2 ) );
        }
        
        b(row) += 2.0*sum;
    }
    
    //
    // Quadrupoles
    //
    for( const QuadrupoleSite &qpol_i : quadrupoles )
    {
        const U32 row = qpol_i.stride;
        
        //
        //  Quadrupole-Charge block
        //
        for( const ChargeSite &charge_j : charges )
        {
            const U32 column = charge_j.stride;
            
            // Ignore inter collection contacts
            if ( qpol_i.collIndex != charge_j.collIndex )
            {
                continue;
            }
            
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(charge_j.collIndex); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( charge_j.collIndex, n );
                
                F64 disti_2 = fpos.Distance2( qpol_i.pos );
                F64 distj_2 = fpos.Distance2( charge_j.pos );
                
                F64 delcomp = DelComp( qpol_i.type, fpos, qpol_i.pos );
                
                sum += delcomp / ( disti_2 * disti_2 * Mathf::Sqrt( disti_2 * distj_2 ) );
            }
            
            a( row, column ) += 2.0 * sum;
        }
        
        //
        //  Quadrupole-Dipole block
        //
        for( const DipoleSite &dipole_j : dipoles )
        {
            const U32 column = dipole_j.stride;
            
            // Ignore inter collection contacts
            if ( qpol_i.collIndex != dipole_j.collIndex )
            {
                continue;
            }
            
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(dipole_j.collIndex); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( dipole_j.collIndex, n );
                
                F64 disti_2 = fpos.Distance2( qpol_i.pos );
                F64 distj_2 = fpos.Distance2( dipole_j.pos );
                
                F64 delcompi = DelComp( qpol_i.type, fpos, qpol_i.pos );
                F64 delcompj = DelComp( dipole_j.type, fpos, dipole_j.pos );
                
                sum += delcompi * delcompj / ( disti_2 * disti_2 * distj_2 * Mathf::Sqrt( disti_2 * distj_2 ) );
                
            }
            
            a( row, column ) += 2.0*sum;
        }
        
        //
        //  Quadrupole-Quadrupole block
        //
        for( const QuadrupoleSite &qpol_j : quadrupoles )
        {
            const U32 column = qpol_j.stride;
            
            // Ignore inter collection contacts
            if ( qpol_i.collIndex != qpol_j.collIndex )
            {
                continue;
            }
            
            F64 sum = 0.0f;
            for ( U32 n=0; n < field.Size(qpol_j.collIndex); ++n )
            {
                Vec3 fpos = field.GetFieldPosition( qpol_j.collIndex, n );
                
                F64 disti_2 = fpos.Distance2( qpol_i.pos );
                F64 distj_2 = fpos.Distance2( qpol_j.pos );
                
                F64 delcompi = DelComp( qpol_i.type, fpos, qpol_i.pos );
                F64 delcompj = DelComp( qpol_j.type, fpos, qpol_j.pos );
                
                sum += delcompi * delcompj / ( disti_2 * disti_2 * distj_2 * distj_2 * Mathf::Sqrt(disti_2*distj_2) );
            }
            
            a(row,column) += 2.0*sum;
        }
        
        //
        //  X_vector
        //
        F64 sum = 0.0;
        for ( U32 n=0; n < field.Size(qpol_i.collIndex); ++n )
        {
            Vec3 fpos = field.GetFieldPosition( qpol_i.collIndex, n );
            
            F64 disti_2 = fpos.Distance2( qpol_i.pos );
            F64 delcomp = DelComp( qpol_i.type, fpos, qpol_i.pos );
            
            sum += ( field.GetFieldDiff(qpol_i.collIndex, n) * delcomp ) / ( disti_2 * disti_2 * Mathf::Sqrt( disti_2 ) );
        }
        
        b(row) += 2.0*sum;
    }
    
    //
    // Now make sure we imply a constaints between the different field fits
    //
    
    /*
    const U32 stride = charges.size() + dipoles.size() + quadrupoles.size();
    
    // Always take the first field as reference so start with 1
    for ( U32 i=1; i < field.NumCollections(); ++i )
    {
        for ( U32 j = 0; j < stride; ++j )
        {
            const size_t offsetColumn = i * stride + j;
            
            Constraint newContraint;
            newContraint.reference = 0.0;
            
            newContraint.matrixColumns.push_back( j );
            newContraint.matrixColumns.push_back( offsetColumn );
            
            newContraint.matrixCoefficients.push_back(  1.0 );
            newContraint.matrixCoefficients.push_back( -1.0 );
            
            constraints.push_back(newContraint);
        }
    }
    */
    
    return Error::STATUS::OK;
}

Error::STATUS Fitter::AddConstraints( std::vector< Constraint > &constraints, multiMatrix &a, multiMatrix &b, const bool addMirror )
{
	U32 strideCol = a.Columns();
	U32 strideRow = a.Rows();
	
	//add the additional rows and columns to accomidate the extra constraints;
	
	//
    //   Init the extra rows and colums to zero
    //
    
    if ( addMirror )
    {
		for ( U32 i=0; i < a.Rows(); ++i )
		{
			for ( U32 j=0; j < constraints.size(); ++j )
			{
				a( i, strideCol+j ) = 0.0;
			}
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
			
			if ( addMirror )
				a( col, strideRow+i ) = cf;
		}
		
		b( strideRow+i ) = constr.reference;
	}
	
	return Error::STATUS::OK;
}

Error::STATUS Fitter::AddRestraints( const Configuration &conf, multiMatrix &q, multiMatrix &c, 
	                                 std::map< SiteHash, U32 >& columnTranslation )
{
    U32 rowStride = q.Rows();
    
    //
    //   Init the extra rows to zero
    //
    for ( U32 i=0; i < ( columnTranslation.size() ); ++i )
    {
        for ( U32 j=0; j < ( columnTranslation.size() ); ++j )
        {
            q( rowStride+i, j ) = 0.0;
        }
        
        c( rowStride+i ) = 0.0;
    }
    
    // Iterate collections!
    for ( U32 coll=0; coll < conf.NumCollections(); ++coll )
    {
        //
        //   Add the resp force constants to the diagonal of the extra rows
        //
        for ( U32 i=0; i < conf.RespRestr(coll); ++i )
        {
            const Configuration::RespRestraint *restr = conf.GetRespRestraint( coll, i );
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
                reference /= Constants::eBohr_D;
            }
            else if ( type == Configuration::valueType::qd20  ||
                      type == Configuration::valueType::qd21c ||
                      type == Configuration::valueType::qd21s ||
                      type == Configuration::valueType::qd22c ||
    		  	      type == Configuration::valueType::qd22s
                     )
            {
                reference /= Constants::eBohr2_DA;
            }
		
            //find the correct column;
            const SiteHash hash = GenerateSiteHash( coll, id, type );
            std::map< SiteHash, U32 >::iterator it = columnTranslation.find( hash );
    	
            if ( it == columnTranslation.end() )
            {
                Error::Warn( std::cout, "The combination of id " + Util::ToString( id ) + " and type " + Util::ToString( static_cast<U32>( type ) )
                                      + "  is not a fitting combination, so cannot be restrained !" );
    		
                return Error::STATUS::FAILED_CONSTR_R;
            }
		
            U32 index = it->second;
    	
            q( rowStride+i, index ) =  1.0f * intensity;
            c( rowStride+i ) = reference;
        }
    }
    
	return Error::STATUS::OK;
}

Error::STATUS Fitter::FitSites( Configuration &conf, const Field &field, const bool verbose  )
{
    //
    //  Tabulate
    //  
    std::vector< ChargeSite > charges;
    std::vector< DipoleSite > dipoles;
    std::vector< QuadrupoleSite > quadrupoles;

    //map that we will use for the constraint to easily turn an ID and type into the correct column;
    //std::pair< U32, Configuration::valueType > is turned into a hash
    std::map< SiteHash, U32 > columnTranslation;
    
	
    TabulateSites( conf, charges, dipoles, quadrupoles, columnTranslation );
    
    //
    //	Translate the user constraints to an easy format
    //
   
    std::vector< Constraint > constraints;
    
    TabulateConstraints( conf, field, constraints,  columnTranslation );
     
    multiMatrix a( 200, 200 ), b( 200 );
    multiMatrix x(200);
    
    //
    //  Construct AX = b
    //
    GenerateMatrices( charges, dipoles, quadrupoles, field, a, b );

    //init x as b;
    x = b;
    
    //
    //  Determine what kernel from lapack to use, based on the availability of constraints and restraints
    //
    
    U32 numRestraints  = 0;
    
    for ( U32 coll=0; coll < conf.NumCollections(); ++coll )
    {
        numRestraints += conf.RespRestr(coll);
    }
    
    FitKernel kernel;
    
    if ( constraints.size() > 0 && numRestraints > 0 )
    {
    	//Error::Warn( std::cout, "The system currently does not support both constraints and restraints at the same time!" );
    	//return Error::STATUS::FAILED_UNSUP_PARAM_COMBO;
    	
    	kernel = FitKernel::DGGLSE;
    }
    else if ( constraints.size() > 0 )
    {
    	kernel = FitKernel::DGESV;
    }
    else if ( numRestraints > 0 )
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
    	AddConstraints( constraints, a, b, true );  
    	
    	if ( verbose )
    	{
    		a.Report("MATRIX_A");
    		b.Report("MATRIX_B");
    	}
    	
    	U32 LWORK = 4 * 3 * a.Columns();  
    	S32 *ipiv = new S32[ LWORK  ];
    	
    	F64 *abuffer = a.GetBuffer();
		F64 *bBuffer = b.GetBuffer();
    	
    	result = LAPACKE_dgesv( LAPACK_ROW_MAJOR, a.Columns(), 1, abuffer, a.Columns(), ipiv, bBuffer, 1 );    
    	
    	delete [] ipiv;
    	
    	x = b;
    }
    else if ( kernel == FitKernel::DGGLSE ) 
    {
    	Console::Report( std::cout, "[SOLVE_KERNEL]\n" );
    	Console::Report( std::cout, "\tLAPACKE_DGGLSE\n" );
    	Console::Report( std::cout, "[END]" );
    	
    	//add the restraints to matrices and b
    	AddRestraints( conf, a, b, columnTranslation );
    	
    	U32 M = a.Rows();
    	U32 N = a.Columns();
    	U32 P = constraints.size();
    	
    	multiMatrix c( P , N ), d( P );
    	
    	AddConstraints( constraints, c, d, false );
    	
    	if ( verbose )
    	{
    		a.Report("MATRIX_A");
    		b.Report("MATRIX_B");
    		c.Report("MATRIX_C");
    		d.Report("MATRIX_D");
    	}
    
		F64 *aBuffer = a.GetBuffer();
		F64 *bBuffer = b.GetBuffer();
		F64 *cbuffer = c.GetBuffer();
		F64 *dBuffer = d.GetBuffer();
		F64 *xBuffer = x.GetBuffer();
		
		result = LAPACKE_dgglse( LAPACK_ROW_MAJOR, M, N, P, aBuffer, N, cbuffer, 
			                     N, bBuffer, dBuffer, xBuffer );
    	
    }
    //switch to the LSE compute kernel
    else
    {
    	Console::Report( std::cout, "[SOLVE_KERNEL]\n" );
    	Console::Report( std::cout, "\tLAPACKE_DGELS\n" );
    	Console::Report( std::cout, "[END]" );
    	
    	//add the restraints to matrices and b
    	AddRestraints( conf, a, b, columnTranslation );
    	
    	if ( verbose )
    	{
    		a.Report("MATRIX_A");
    		b.Report("MATRIX_B");
    	}
    	
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

    if ( verbose )
    {
    	x.Report("MATRIX_X");
    }


    //Set the fitted results in the configuration
    for ( const ChargeSite &charge : charges )
    {
        conf.GetSiteMod( charge.collIndex, charge.confIndex )->SetValue( charge.type, x(charge.stride) );
    }

    for ( const DipoleSite &dipole : dipoles )
    {
        conf.GetSiteMod( dipole.collIndex, dipole.confIndex )->SetValue( dipole.type, Constants::eBohr_D * x(dipole.stride) );
    }
 
    for ( const QuadrupoleSite &qpol : quadrupoles )
    {
        conf.GetSiteMod( qpol.collIndex, qpol.confIndex )->SetValue( qpol.type, Constants::eBohr2_DA * x(qpol.stride) );

    }

    
    return Error::STATUS::OK;
}
