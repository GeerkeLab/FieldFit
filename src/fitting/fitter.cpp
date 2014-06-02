#include "fitter.h"
#include "../system/report.h"
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

    switch( type )
    {
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

Error::STATUS Fitter::GenerateMatrices( std::vector< ChargeSite > &charges, std::vector< DipoleSite > &dipoles, std::vector< QuadrupoleSite > &quadrupoles, const Field &field, multiMatrix &a, multiMatrix &b )
{
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

            sum += ( field.GetFieldPotential(n) )/Mathf::Sqrt( disti_2 );
        }

        b(row, 0)=2.0*sum;

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

            sum += ( field.GetFieldPotential(n) * delcompi ) / ( disti_2 * Mathf::Sqrt( disti_2 ) );
        }

        b(row, 0)=2.0*sum;

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

            sum += ( field.GetFieldPotential(n) * delcomp ) / ( disti_2 * disti_2 * Mathf::Sqrt( disti_2 ) );
        }

        b(row, 0)=2.0*sum;

        row++;
    }


    return Error::STATUS::OK;
}

Error::STATUS Fitter::AddConstraints( std::vector< Constraint > &constraints, multiMatrix &a, multiMatrix &b )
{
	U32 strideCol = a.Columns();
	U32 strideRow = a.Rows();
	
	//add the additional rows and columns to accomidate the extra constraints;
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
	for ( U32 i=0; constraints.size(); ++i )
	{
		const Constraint &constr = constraints[i];
		
		for ( U32 j=0; j < constr.matrixColumns.size(); ++j )
		{
			U32 colRow = constr.matrixColumns[j];
			F32 cf = constr.matrixCoefficients[j];
			
			a( strideRow+i, colRow ) = cf;
		}
		
		b( strideRow+i ) = constr.reference;
	}
	
	return Error::STATUS::OK;
}

Error::STATUS Fitter::FitSites( Configuration &conf, const Field &field )
{
    //
    //  Tabulate
    //  

    const U32 cfSize = conf.Size();

    std::vector< ChargeSite > charges;
    std::vector< DipoleSite > dipoles;
    std::vector< QuadrupoleSite > quadrupoles;

    //map that we will use for the constraint to easily turn an ID and type into the correct column;
    //std::pair< U32, Configuration::valueType > is turned into a hash
    std::map< std::pair< U32, Configuration::valueType >, U32 > mColumnTranslation;
    
    U32 column = 0;
    
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
    
    //
    //	Translate the constraints to an easy format
    //
    std::vector< Constraint > constraints;
    
    for ( U32 i=0; i < conf.ValueConstr(); ++i )
    {
    	const Configuration::ValueConstraint *constr = conf.GetValueConstraint( i );
    	const U32 id = constr->ID;
    	const Configuration::valueType type = constr->type;
    	const F32 val = constr->value;
    	
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
    	
    	
    	
    }
    

    //
    //  Construct AX = b
    //
    multiMatrix a( 200, 200 ), b( 200 );

    GenerateMatrices( charges, dipoles, quadrupoles, field, a, b );
    
    std::cout << "TTTT " << conf.ValueConstr() << std::endl;
    
    AddConstraints( constraints, a, b );
    
    multiMatrix x(200);
    for ( U32 i=0; i < 200; i++ )
    {
        x(i) = 0.0f;
    } 

    S32 *ipiv = new S32[ 200 ];

    std::cout << "A:  rows; " << a.Rows() << "   Cols; " << a.Columns() << std::endl;
    std::cout << "B:  rows; " << b.Rows() << "   Cols; " << b.Columns() << std::endl;
	
    F64 *abuffer = a.GetBuffer();
    F64 *bBuffer = b.GetBuffer();
	
    for ( U32 i=0; i < a.Rows(); ++i )
    {
        for ( U32 j=0; j < a.Columns(); ++j )
        {
           printf( "%12.5f", abuffer[ i*a.Columns() + j ] );
        }
        
         printf( "\n" );
    }
     printf( "\n" );

     U32 N = charges.size() + dipoles.size() + quadrupoles.size();

    S32 result = LAPACKE_dgesv( LAPACK_ROW_MAJOR, N, 1, abuffer, N, ipiv, bBuffer, 1 );    

    std::cout << result << std::endl;
    
    b.BufferSwap();
    
    x = b;

    //Fit
    F32 f=2.541748;
    F32 f1=1.3450351;

    U32 stride = 0;
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
    
    Report rp( conf, field );
    rp.WriteToStream( std::cout );

    return Error::STATUS::OK;
}
