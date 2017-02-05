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

void FieldFit::ReadGrids( const BlockParser &bp, const Units &units, Configuration &config )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("GRID");
    
    if ( !blockArray )
    {
        throw ArgException( "FieldFit", "ReadGrids", "block GRID was not present!" );
    }
    
    for ( const Block &block : *blockArray )
    { 
        ReadGrid(block,units,config);
    }
}

void FieldFit::ReadFields( const BlockParser &bp, const Units &units, Configuration &config )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("FIELD");
    
    if ( !blockArray )
    {
        throw ArgException( "FieldFit", "ReadFields", "block FIELD was not present!" );
    }
    
    for ( const Block &block : *blockArray )
    { 
        ReadField(block,units,config);
    }
}

void FieldFit::ReadEfields( const BlockParser &bp, const Units &units, Configuration &config )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("EFIELD");
    
    if ( blockArray )
    {
        for ( const Block &block : *blockArray )
        { 
            ReadEfield(block, units, config);            
        }
    }
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
        
        config.InsertSystem(newSys);
    }      
}

void FieldFit::ReadPermChargeSets( const BlockParser &, const Units &units, Configuration &config )
{
    
}

void FieldFit::ReadPermDipoleSets( const BlockParser &, const Units &units, Configuration &config )
{
    
}

void FieldFit::ReadGrid( const Block &block, const Units &units, Configuration &config )
{   
    if ( block.Size() < 2 )
    {
        throw ArgException( "FieldFit", "ReadGrid", "block [GRID] was too small ( at least 1 argument1 expected ) !" );
    }
    
    const std::string systemName = block.GetToken( 0 )->GetToken();
    const U32 numCoords = block.GetToken( 1 )->GetValue< U32 >();
    
    System *sys = config.FindSystem( systemName );
    
    if ( !sys )
    {
        throw ArgException( "FieldFit", "ReadGrid", "System with name "+systemName+" not found!" );
    }
    
    arma::vec x = arma::zeros( numCoords );
    arma::vec y = arma::zeros( numCoords );
    arma::vec z = arma::zeros( numCoords );
    
    //test for the correct size
    if ( block.Size() != ( 2  + ( numCoords * 3 ) ) )
    {
        throw ArgException( "FieldFit", "ReadGrid", "block [GRID] did not have the right amount of arguments based on the size indicators !" );
    }
    
    const F64 coordConv = units.GetCoordConv();
    
    U32 index = 2;
    
    for ( U32 i=0; i < numCoords; ++i,index+=3 )
    {
        x[i] = block.GetToken( index+0 )->GetValue< F64 >() * coordConv;
        y[i] = block.GetToken( index+1 )->GetValue< F64 >() * coordConv;
        z[i] = block.GetToken( index+2 )->GetValue< F64 >() * coordConv;
    }
    
    Grid *newGrid = new Grid( x, y, z );
    sys->InsertGrid(newGrid);
}

void FieldFit::ReadField( const Block &block, const Units &units, Configuration &config )
{   
    if ( block.Size() < 3 )
    {
        throw ArgException( "FieldFit", "ReadField", "block [FIELD] was too small ( at least 3 arguments expected )!" );
    }
    
    const std::string &systemName = block.GetToken( 0 )->GetToken();
    const U32 numSets   = block.GetToken( 1 )->GetValue< U32 >();
    const U32 numPoints = block.GetToken( 2 )->GetValue< U32 >();
    
    System *sys = config.FindSystem( systemName );
    
    if ( !sys )
    {
        throw ArgException( "FieldFit", "ReadGrid", "System with name "+systemName+" not found!" );
    }
    
    const Grid *grid = sys->GetGrid();
    
    if ( !grid )
    {
        throw ArgException( "FieldFit", "ReadGrid", "Grid for system with name "+systemName+" not found!" );
    }
    
    if ( numPoints != grid->GetX().n_elem ||
         numPoints != grid->GetY().n_elem || 
         numPoints != grid->GetZ().n_elem  )
    {
        throw ArgException( "FieldFit", "ReadField", "configuration "+systemName+" was assigned a field matrix that does not match its grid" );
    }
    
    arma::mat potentials = arma::zeros( numPoints, numSets );

    //test for the correct size
    if ( block.Size() != ( 3 + ( numPoints * numSets ) ) )
    {
        throw ArgException( "FieldFit", "ReadField", "block [FIELD] did not have the right amount of arguments based on the size indicators !" );    
    }
    
    const F64 potConv = units.GetPotConv();
    
    U32 index = 3;
    
    for ( U32 s=0; s < numSets; ++s )
    {
        for ( U32 i=0; i < numPoints; ++i,index++ )
        {
            potentials.col(s)[i] = block.GetToken( index )->GetValue< F64 >() * potConv;
        }  
    }
    
    Field *newField = new Field(potentials);
    sys->InsertField( newField );
}

void FieldFit::ReadEfield( const Block &block, const Units &units, Configuration &config )
{
    if ( block.Size() < 3 )
    {
        throw ArgException( "FieldFit", "ReadEfield", "block [EFIELD] was too small ( at least 2 argument expected ) !" );    
    }
    
    const std::string &systemName = block.GetToken( 0 )->GetToken();
    const U32 sites       = block.GetToken( 1 )->GetValue< U32 >();
    const U32 setsPerSite = block.GetToken( 2 )->GetValue< U32 >();
    
    System *sys = config.FindSystem( systemName );
    if ( !sys )
    {
        throw ArgException( "FieldFit", "ReadEfield", "System with name "+systemName+" not found!" );
    }
       
    if ( block.Size() != ( 3 + ( sites + sites * setsPerSite * 3 ) ) )
    {
        throw ArgException( "FieldFit", "ReadEfield", "block [EFIELD] did not have the right amount of arguments based on the size indicator!" );
    }
    
    const Field *field = sys->GetField();
    
    if ( !field )
    {
        throw ArgException( "FieldFit", "ReadEfield", "Field for system with name "+systemName+" not found!" );
    }
    
    if ( setsPerSite != field->GetPotentials().n_cols )
    {
        throw ArgException( "FieldFit", "ReadEfield", "Number of efield inputs per sites does not match the number of potential sets" );
    }

    const F64 efieldConv = units.GetEfieldConv();

    U32 index = 3;
    for ( U32 i=0; i < sites; ++i )
    {
        const std::string name = block.GetToken( index+0 )->GetToken();
        
        index++;
        
        Site *site = sys->FindSite( name );
        
        // skip
        if ( !site )
        {
            index+=3*setsPerSite;
            continue;
        }
        
        arma::vec ex;
        arma::vec ey;
        arma::vec ez;
        ex.resize( setsPerSite );
        ey.resize( setsPerSite );
        ez.resize( setsPerSite );
        
        // read ef_xyz
        for ( U32 s=0; s < setsPerSite; ++s )
        {
            F64 efx = block.GetToken( index+0 )->GetValue< F64 >() * efieldConv;
            F64 efy = block.GetToken( index+1 )->GetValue< F64 >() * efieldConv;
            F64 efz = block.GetToken( index+2 )->GetValue< F64 >() * efieldConv;
            
            ex[s] = efx;
            ey[s] = efy;
            ez[s] = efz;
            
            index+=3;
        }
        
        site->AddEfield( ex, ey, ez );
    }
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
        
        U32 fitTypes = StringTypeToFitFlags( fitFlags );
        
        Site *site = new Site( fitTypes, atomName, coulFlag, coord_x, coord_y, coord_z );
        newSys->InsertSite(site);
        
        index += 6;
    }
    
    return newSys;
}

void FieldFit::ReadPermChargeSet( const Block &block, const Units &units, Configuration &config )
{
    /*
    if ( block.Size() < 2 )
    {
        Error::Warn( std::cout, "block [PERMCHARGES] was too small ( at least 2 argument expected ) !" );
        return Error::Status::FAILED_IO;
    }

    mSystemName = block.GetToken( 0 )->GetToken();
    U32 numSites   = block.GetToken( 1 )->GetValue< U32 >();
      
    if ( block.Size() != ( 2 + ( numSites * 4 ) ) )
    {
        Error::Warn( std::cout, "block [PERMCHARGES] did not have the right amount of arguments based on the size indicator!" );
        return Error::Status::FAILED_IO;
    }

    const F64 coordConv  = units.GetCoordConv();
    const F64 chargeConv = units.GetChargeConv();
    
    U32 index = 2;

    for ( U32 i=0; i < numSites; ++i )
    {
        PermSite permSite;
        
        permSite.x   	 = block.GetToken( index+0 )->GetValue< F64 >() * coordConv;
        permSite.y       = block.GetToken( index+1 )->GetValue< F64 >() * coordConv;
        permSite.z       = block.GetToken( index+2 )->GetValue< F64 >() * coordConv;
        permSite.pval    = block.GetToken( index+3 )->GetValue< F64 >() * chargeConv;
        permSite.permType = ValueType::charge;
        mPermSites.push_back( permSite );

        index += 4;
    }
    */
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