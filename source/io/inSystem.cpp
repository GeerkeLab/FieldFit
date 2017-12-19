#include "io/block.h"
#include "io/inSystem.h"
#include "io/blockParser.h"

#include "common/exception.h"

#include "configuration/system.h"
#include "configuration/configuration.h"

#include <set>

namespace FieldFit
{
    void FillUnitsMap( Units &units, std::map< std::string, F64* > &map,
                       std::map< std::pair< std::string, std::string >, F64 > &unitsMap );  
}

FieldFit::Units* FieldFit::ReadUnits( BlockParser &bp )
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
    
    Units *unitsObj = new Units;
    std::map< std::string, F64* > mapping;
    std::map< std::pair< std::string, std::string >, F64 > units;
    
    FillUnitsMap( *unitsObj, mapping, units );
    
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

    bp.DeleteBlock("UNITS");
    
    return unitsObj;
}

void FieldFit::ReadGrids( BlockParser &bp, const Units &units, Configuration &config )
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

    bp.DeleteBlock("GRID");
}

void FieldFit::ReadFields( BlockParser &bp, const Units &units, Configuration &config, const std::vector< U32 > &collectionSelection )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("FIELD");
    
    if ( !blockArray )
    {
        throw ArgException( "FieldFit", "ReadFields", "block FIELD was not present!" );
    }

    for ( const Block &block : *blockArray )
    { 
        ReadField(block,units,config, collectionSelection);
    }

    bp.DeleteBlock("FIELD");
}

void FieldFit::ReadEfields( BlockParser &bp, const Units &units, Configuration &config )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("EFIELD");
    
    if ( blockArray )
    {
        for ( const Block &block : *blockArray )
        {   
            ReadEfield(block, units, config);            
        }
    }

    bp.DeleteBlock("EFIELD");
}

void FieldFit::ReadSystems( BlockParser &bp, const Units &units, Configuration &config )
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

    bp.DeleteBlock("SYSTEM"); 
}

void FieldFit::ReadPermChargeSets( BlockParser &bp, const Units &units, Configuration &config )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("PERMCHARGES");
    
    if ( blockArray )
    {
        for ( const Block &block : *blockArray )
        { 
            ReadPermChargeSet(block,units,config);
        }
    }

    bp.DeleteBlock("PERMCHARGES");
}

void FieldFit::ReadPermDipoleSets( BlockParser &bp, const Units &units, Configuration &config )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("PERMDIPOLES");
    
    if ( blockArray )
    {
        for ( const Block &block : *blockArray )
        { 
            ReadPermDipoleSet(block,units,config);
        }
    }

    bp.DeleteBlock("PERMDIPOLES");
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

void FieldFit::ReadField( const Block &block, const Units &units, Configuration &config, const std::vector< U32 > &collectionSelection )
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
        throw ArgException( "FieldFit", "ReadField", "System with name "+systemName+" not found!" );
    }
    
    const Grid *grid = sys->GetGrid();
    
    if ( !grid )
    {
        throw ArgException( "FieldFit", "ReadField", "Grid for system with name "+systemName+" not found!" );
    }
    
    if ( numPoints != grid->GetX().n_elem ||
         numPoints != grid->GetY().n_elem || 
         numPoints != grid->GetZ().n_elem  )
    {
        throw ArgException( "FieldFit", "ReadField", "configuration "+systemName+" was assigned a field matrix that does not match its grid" );
    }

    std::set< U32 > collectionSet( collectionSelection.begin(), collectionSelection.end() );

    // if no selections were made 
    if ( collectionSelection.size() == 0 )
    {   
        // if no selection make a full set
        for ( U32 i=0; i < numSets; ++i )
        {
            collectionSet.insert(i);
        }
    }

    for ( auto it = collectionSet.begin(), itend = collectionSet.end(); it != itend; ++it )
    {
        const U32 sel = *it;

        if (sel >= numSets)
        {
             throw ArgException( "FieldFit", "ReadField", "Out of bounds collection selection!" );
        }
    }
    
    // we might do a subselection so do that that into account
    const U32 numEffectiveSets = collectionSet.size();
    
    arma::mat potentials = arma::zeros( numPoints, numEffectiveSets );

    //test for the correct size
    if ( block.Size() != ( 3 + ( numPoints * numSets ) ) )
    {
        throw ArgException( "FieldFit", "ReadField", "block [FIELD] did not have the right amount of arguments based on the size indicators !" );    
    }
    
    const F64 potConv = units.GetPotConv();
    
    U32 index = 3;
    U32 column = 0;

    for ( U32 s=0; s < numSets; ++s )
    {
        // test if we actually want to read here or just increment index
        if ( collectionSet.find(s) != collectionSet.end() )
        {
            //std::cout << "SELECTING FIELD SET " << s << std::endl;
            for ( U32 i=0; i < numPoints; ++i,index++ )
            {
                potentials.col(column)[i] = block.GetToken( index )->GetValue< F64 >() * potConv;
            }

            column++;
        }
        else
        {
            index+=numPoints;
        }
    }

    if ( column != numEffectiveSets )
    {
        throw ArgException( "FieldFit", "ReadField", "Number of promised subselections does not match number of read in columns!" );
    }
    
    Field *newField = new Field(potentials, collectionSet, numSets);
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
    
    if ( setsPerSite != field->PreSelectNumSets() )
    {
        throw ArgException( "FieldFit", "ReadEfield", "Number of efield inputs per sites does not match the number of potential sets" );
    }

    const std::set< U32 > &collectionSet = field->GetCollectionSet();
    const size_t effectiveSetsPerSite = collectionSet.size();
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
        ex.resize( effectiveSetsPerSite );
        ey.resize( effectiveSetsPerSite );
        ez.resize( effectiveSetsPerSite );
        
        // read ef_xyz
        U32 row = 0;
        for ( U32 s=0; s < setsPerSite; ++s )
        {
            // do we want to subselect this set?
            if ( collectionSet.find(s) != collectionSet.end() )
            {
                //std::cout << "EFIELD_SELECT " << s << std::endl;

                F64 efx = block.GetToken( index+0 )->GetValue< F64 >() * efieldConv;
                F64 efy = block.GetToken( index+1 )->GetValue< F64 >() * efieldConv;
                F64 efz = block.GetToken( index+2 )->GetValue< F64 >() * efieldConv;
                
                ex[row] = efx;
                ey[row] = efy;
                ez[row] = efz;

                row++;
            }
            
            index+=3;
        }

        if ( row != effectiveSetsPerSite )
        {
            throw ArgException( "FieldFit", "ReadEfield", "Number of promised subselections does not match number of read in rows!" );
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
    if ( block.Size() < 2 )
    {
        throw ArgException( "FieldFit", "ReadPermChargeSet", "block [PERMCHARGES] was too small ( at least 2 argument expected ) !" );    
    }
    
    const std::string &systemName = block.GetToken( 0 )->GetToken();
    const U32 permSites = block.GetToken( 1 )->GetValue< U32 >();
    
    if ( block.Size() != ( 2 + ( permSites * 4 ) ) )
    {
        throw ArgException( "FieldFit", "ReadPermChargeSet", "block [PERMCHARGES] did not have the right amount of arguments based on the size indicator!" );    
    }
    
    System *sys = config.FindSystem( systemName );
    if ( !sys )
    {
        throw ArgException( "FieldFit", "ReadPermChargeSet", "System with name "+systemName+" not found!" );
    }
       
    const F64 coordConv  = units.GetCoordConv();
    const F64 chargeConv = units.GetChargeConv();
    
    U32 index = 2;
    for ( U32 i=0; i < permSites; ++i )
    {
        const F64 x = block.GetToken( index+0 )->GetValue< F64 >() * coordConv;
        const F64 y = block.GetToken( index+1 )->GetValue< F64 >() * coordConv;
        const F64 z = block.GetToken( index+2 )->GetValue< F64 >() * coordConv;
        const F64 val = block.GetToken( index+3 )->GetValue< F64 >() * chargeConv;
        
        PermSite *psite = new PermSite( x, y, z, val, FitType::charge );        
        sys->InsertPermSite(psite);

        index += 4;
    }
}

void FieldFit::ReadPermDipoleSet( const Block &block, const Units &units, Configuration &config )
{
    if ( block.Size() < 2 )
    {
        throw ArgException( "FieldFit", "ReadPermDipoleSet", "block [PERMDIPOLES] was too small ( at least 2 argument expected ) !" );    
    }
    
    const std::string &systemName = block.GetToken( 0 )->GetToken();
    const U32 permSites = block.GetToken( 1 )->GetValue< U32 >();
    
    if ( block.Size() != ( 2 + ( permSites * 6 ) ) )
    {
        throw ArgException( "FieldFit", "ReadPermDipoleSet", "block [PERMDIPOLES] did not have the right amount of arguments based on the size indicator!" );    
    }
    
    System *sys = config.FindSystem( systemName );
    if ( !sys )
    {
        throw ArgException( "FieldFit", "ReadPermDipoleSet", "System with name "+systemName+" not found!" );
    }
    
    const F64 coordConv  = units.GetCoordConv();
    const F64 dipoleConv = units.GetDipoleConv();
    
    U32 index = 2;

    for ( U32 i=0; i < permSites; ++i )
    {
        const F64 x = block.GetToken( index+0 )->GetValue< F64 >() * coordConv;
        const F64 y = block.GetToken( index+1 )->GetValue< F64 >() * coordConv;
        const F64 z = block.GetToken( index+2 )->GetValue< F64 >() * coordConv;
        const F64 valX = block.GetToken( index+3 )->GetValue< F64 >() * dipoleConv;
        const F64 valY = block.GetToken( index+4 )->GetValue< F64 >() * dipoleConv;
        const F64 valZ = block.GetToken( index+5 )->GetValue< F64 >() * dipoleConv;
        
        PermSite *psiteX = new PermSite( x, y, z, valX, FitType::dipoleX );  
        PermSite *psiteY = new PermSite( x, y, z, valY, FitType::dipoleY );
        PermSite *psiteZ = new PermSite( x, y, z, valZ, FitType::dipoleZ );
              
        sys->InsertPermSite(psiteX);
        sys->InsertPermSite(psiteY);
        sys->InsertPermSite(psiteZ);

        index += 6;
    }
}

void FieldFit::FillUnitsMap( Units &units, std::map< std::string, F64* > &nameToUnit,
                    	     std::map< std::pair< std::string, std::string >, F64 > &unitsMap )
{
    nameToUnit.insert( std::make_pair( "coord"    , &units.GetCoordConv()  ) );
    nameToUnit.insert( std::make_pair( "charge"   , &units.GetChargeConv() ) );
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