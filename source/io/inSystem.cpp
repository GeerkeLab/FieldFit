#include "io/block.h"
#include "io/inSystem.h"
#include "io/blockParser.h"

#include "common/exception.h"

#include "configuration/system.h"
#include "configuration/configuration.h"

namespace FieldFit
{
    void FillUnitsMap( Units &units, std::map< std::string, F64* > &map,
                       std::map< std::pair< std::string, std::string >, F64 > &unitsMap );  
}

FieldFit::Units FieldFit::ReadUnits( const BlockParser &bp )
{
    const Block *block = bp.GetBlock("UNITS");
    
    if ( !block )
    {
        throw ArgException( "FieldFit", "ReadUnits", "block UNITS was not present!" );
    }
    
    if ( block->Size() % 2 != 0 )
    {
        throw ArgException( "FieldFit", "ReadUnits", "block [UNITS] did not have the right amount of arguments" );
    }
    
    Units unitsObj;
    std::map< std::string, F64* > mapping;
    std::map< std::pair< std::string, std::string >, F64 > units;
    
    FillUnitsMap( unitsObj, mapping, units );
    
    for ( U32 i=0; i < block->Size(); i += 2 )
    {
        std::string key  = block->GetToken( i+0 )->GetToken();
        std::string unit = block->GetToken( i+1 )->GetToken();
        
        auto itm = mapping.find( key );
        
        if ( itm == mapping.end() )
        {
            throw ArgException( "FieldFit", "ReadUnits", "in block [UNITS] unknown type "+key );
        }
        
        auto itu = units.find( std::make_pair( key, unit ) );
        
        if ( itu == units.end() )
        {
            throw ArgException( "FieldFit", "ReadUnits", "in block [UNITS] unknown unit "+key+" for type "+key );
        }
        
        F64 convFactor = itu->second;
        
        *( itm->second ) = convFactor;
    }
    
    for ( auto it=mapping.begin(),itend=mapping.end(); it != itend; ++it )
    {
        if ( *it->second == 0.0 )
        {
            throw ArgException( "FieldFit", "ReadUnits", "type "+it->first+" not present in block [UNITS] " );
        }
    }
    
    return unitsObj;
}



void FieldFit::ReadGrids( const BlockParser &, const Units &units, Configuration &config )
{
    
}

void FieldFit::ReadFields( const BlockParser &, const Units &units, Configuration &config )
{
    
}

void FieldFit::ReadEfields( const BlockParser &, const Units &units, Configuration &config )
{
    
}

void FieldFit::ReadSystems( const BlockParser &bp, const Units &units, Configuration &config )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("SYSTEM");
    
    if ( !blockArray )
    {
        throw ArgException( "FieldFit", "ReadSystems", "block [SYSTEM] was not present!" );
    }
    
    for ( const Block &block : *blockArray )
    {
        System *newSys = ReadSystem( block, units );
        
        if ( !newSys )
        {
            throw ArgException( "FieldFit", "ReadSystems", "Unable to parse a system blocks" );
        }
        
        const System *it = config.FindSystem( newSys->GetName() );
        
        if ( it )
        {
            throw ArgException( "FieldFit", "ReadSystems", "Duplicate system name " + newSys->GetName() );
        }
        
        //configurations.insert( std::make_pair( newConfig.GetSystemName(), newConfig ) );
        
    }      
}

void FieldFit::ReadPermChargeSets( const BlockParser &, const Units &units, Configuration &config )
{
    
}

void FieldFit::ReadPermDipoleSets( const BlockParser &, const Units &units, Configuration &config )
{
    
}

void FieldFit::ReadGrid( const Block &, const Units &units, Configuration &config )
{
    
}

void FieldFit::ReadField( const Block &, const Units &units, Configuration &config )
{
    
}

void FieldFit::ReadEfield( const Block &, const Units &units, Configuration &config )
{
    
}

FieldFit::System* FieldFit::ReadSystem( const Block &block, const Units &units  )
{
    System *newSys = nullptr;
    
    if ( block.Size() < 2 )
    {
        throw ArgException( "FieldFit", "ReadSystem", "block [SYSTEM] was too small ( at least 2 argument expected ) !" );
    }

    const std::string systemName = block.GetToken( 0 )->GetToken();
    const U32 numSites = block.GetToken( 1 )->GetValue< U32 >();

    if ( numSites < 1 )
    {
        throw ArgException( "FieldFit", "ReadSystem", "in block [SYSTEM] at least 1 fit site was expected!" );
    }
        
    if ( block.Size() != ( 2 + ( numSites * 6 ) ) )
    {
        throw ArgException( "FieldFit", "ReadSystem", "block [SYSTEM] did not have the right amount of arguments based on the size indicator!" );
    }

    newSys = new System( systemName );

    const F64 coordConv = units.GetCoordConv();

    U32 index = 2;

    for ( U32 i=0; i < numSites; ++i )
    {
        const std::string atomName = block.GetToken( index+0 )->GetToken();
        const std::string coulFlag = block.GetToken( index+1 )->GetToken();
        const std::string fitFlags = block.GetToken( index+2 )->GetToken();
        
        const F64 coord_x = block.GetToken( index+3 )->GetValue< F64 >() * coordConv;
        const F64 coord_y = block.GetToken( index+4 )->GetValue< F64 >() * coordConv;
        const F64 coord_z = block.GetToken( index+5 )->GetValue< F64 >() * coordConv;
        
        Site site( atomName, coulFlag, coord_x, coord_y, coord_z );
        
        if ( fitFlags == "charge" )
        {   
            site.AddFitType( FitType::charge );
        }
        else if ( fitFlags == "dipole_x" )
        {
            site.AddFitType( FitType::dipoleX );
        }
        else if ( fitFlags == "dipole_xy" )
        {
            site.AddFitType( FitType::dipoleX );
            site.AddFitType( FitType::dipoleY );
        }
        else if ( fitFlags == "dipole_xz" )
        {
            site.AddFitType( FitType::dipoleX );
            site.AddFitType( FitType::dipoleZ );
        }
        else if ( fitFlags == "dipole_y" )
        {
            site.AddFitType( FitType::dipoleY );
        }
        else if ( fitFlags == "dipole_yz" )
        {
            site.AddFitType( FitType::dipoleY );
            site.AddFitType( FitType::dipoleZ );
        }
        else if ( fitFlags == "dipole_z" )
        {
            site.AddFitType( FitType::dipoleZ );
        }
        else if ( fitFlags == "dipole_xyz" || fitFlags == "alpha" )
        {
            site.AddFitType( FitType::dipoleX );
            site.AddFitType( FitType::dipoleY );
            site.AddFitType( FitType::dipoleZ );
        }
        else if ( fitFlags == "qpol" )
        {
            site.AddFitType( FitType::qd20 );
            site.AddFitType( FitType::qd21c );
            site.AddFitType( FitType::qd21s );
            site.AddFitType( FitType::qd22c );
            site.AddFitType( FitType::qd22s );
        }
        else
        {
            throw ArgException( "FieldFit", "ReadSystem", "Unknown fitflags "+fitFlags );
        }
        
        index += 6;
    }
    
    return newSys;
}

void FieldFit::ReadPermChargeSet( const Block &, const Units &units, Configuration &config )
{
    
}

void FieldFit::ReadPermDipoleSet( const Block &, const Units &units, Configuration &config )
{
    
}

void FieldFit::FillUnitsMap( Units &units, std::map< std::string, F64* > &nameToUnit,
                    	     std::map< std::pair< std::string, std::string >, F64 > &unitsMap )
{
    nameToUnit.insert( std::make_pair( "coord"    , &units.GetCoordConv()  ) );
    nameToUnit.insert( std::make_pair( "charge"   , &units.GetDipoleConv() ) );
    nameToUnit.insert( std::make_pair( "dipole"   , &units.GetDipoleConv() ) );
    nameToUnit.insert( std::make_pair( "qpol"     , &units.GetQpolConv() ) );
    nameToUnit.insert( std::make_pair( "potential", &units.GetPotConv() ) );
    nameToUnit.insert( std::make_pair( "efield"   , &units.GetEfieldConv() ) );
    nameToUnit.insert( std::make_pair( "alpha"    , &units.GetAlphaConv() ) );
    
    unitsMap.insert( std::make_pair( std::make_pair( "coord", "bohr" ), 1.0 ) );
    unitsMap.insert( std::make_pair( std::make_pair( "coord", "angstrom" ), Constants::A_bohr ) );
    unitsMap.insert( std::make_pair( std::make_pair( "coord", "nm" ), Constants::nm_bohr ) );
    
    unitsMap.insert( std::make_pair( std::make_pair( "charge", "e" ), 1.0 ) );
    
    unitsMap.insert( std::make_pair( std::make_pair( "dipole", "debye" ), Constants::D_eBohr ) );
    unitsMap.insert( std::make_pair( std::make_pair( "dipole", "e_bohr" ), 1.0 ) );
    
    unitsMap.insert( std::make_pair( std::make_pair( "qpol", "buckingham" ), Constants::DA_eBohr2 ) );
    unitsMap.insert( std::make_pair( std::make_pair( "qpol", "e_bohr^2" ), 1.0 ) );
    
    unitsMap.insert( std::make_pair( std::make_pair( "potential", "e/bohr" ), 1.0 ) );
    
    unitsMap.insert( std::make_pair( std::make_pair( "efield", "kJ/mol_nm_e" ), ( 1.0 / Constants::four_pi_eps_rcp ) / ( Constants::nm_bohr * Constants::nm_bohr ) ) );
    unitsMap.insert( std::make_pair( std::make_pair( "efield", "e/bohr^2" ), 1.0 ) );
    
    unitsMap.insert( std::make_pair( std::make_pair( "alpha", "nm^3" ), Constants::nm3_bohr3 ) );
    unitsMap.insert( std::make_pair( std::make_pair( "alpha", "A^3" ), Constants::A3_bohr3 ) );
    unitsMap.insert( std::make_pair( std::make_pair( "alpha", "bohr^3" ), 1.0 ) );
}