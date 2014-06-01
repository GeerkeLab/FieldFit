#include "fitter.h"

#include "../system/report.h"

#include <fstream>

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

Error::STATUS Fitter::FitSites( Configuration &conf, const Field &field )
{
    //
    //  Tabulate
    //  

    const U32 cfSize = conf.Size();

    std::vector< ChargeSite > charges;
    std::vector< DipoleSite > dipoles;
    std::vector< QuadrupoleSite > quadrupoles;

    for ( U32 i=0; i < conf.Size(); ++i )
    {
        const Configuration::FitSite *site = conf.GetSite( i );
        const U32 fitFlags = site->fitFlags;
        const Vec3 & pos   = site->position;

        if ( fitFlags & (0x01) )
            charges.push_back( ChargeSite( i, pos, Configuration::valueType::charge ) );

        if ( fitFlags & (0x02) )
            dipoles.push_back( DipoleSite( i, pos, Configuration::valueType::dipoleX ) );

        if ( fitFlags & (0x04) )
            dipoles.push_back( DipoleSite( i, pos, Configuration::valueType::dipoleY ) );

        if ( fitFlags & (0x08) )
            dipoles.push_back( DipoleSite( i, pos, Configuration::valueType::dipoleZ ) );

        if ( fitFlags & (0x10) )
            quadrupoles.push_back( QuadrupoleSite(i, pos, Configuration::valueType::qd20 ) );

        if ( fitFlags & (0x20) )
            quadrupoles.push_back( QuadrupoleSite(i, pos, Configuration::valueType::qd21c ) );

        if ( fitFlags & (0x40) )
            quadrupoles.push_back( QuadrupoleSite(i, pos, Configuration::valueType::qd21s ) );

        if ( fitFlags & (0x80) )
            quadrupoles.push_back( QuadrupoleSite(i, pos, Configuration::valueType::qd22c ) );

        if ( fitFlags & (0x100) )
            quadrupoles.push_back( QuadrupoleSite(i, pos, Configuration::valueType::qd22s ) );
    }

    //
    //  Construct AX = b
    //
    multiMatrix a( 200, 200 ), b( 200, 1 );

    GenerateMatrices( charges, dipoles, quadrupoles, field, a, b );

    multiMatrix x(200);

    //
    //  DEBUG!
    //
    x( b.Rows()+1 ) = 0.0;

    //Fit
    U32 stride=0;
    for ( U32 i=0; i < charges.size(); ++i )
    {
        conf.GetSiteMod( stride )->SetValue( charges[i].type, x(stride) );

        stride++;
    }
    
    Report rp( conf, field );
    rp.WriteToStream( std::cout );

    return Error::STATUS::OK;
}
