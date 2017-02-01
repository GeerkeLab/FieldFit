#pragma once 
#ifndef __INSYSTEM_H__
#define __INSYSTEM_H__

#include "common/types.h"
#include "io/units.h"

#include <map>

namespace FieldFit
{
    class Block;
    class System;
    class BlockParser;
    class Configuration;
    
    
    Units ReadUnits( const BlockParser & );
    
    void ReadGrids( const BlockParser &, const Units &units, Configuration &config );
    void ReadFields( const BlockParser &, const Units &units, Configuration &config );
    void ReadEfields( const BlockParser &, const Units &units, Configuration &config );
    void ReadSystems( const BlockParser &, const Units &units, Configuration &config );
    void ReadPermChargeSets( const BlockParser &, const Units &units, Configuration &config );
    void ReadPermDipoleSets( const BlockParser &, const Units &units, Configuration &config );

    System* ReadSystem( const Block &, const Units &units );

    void ReadGrid( const Block &, const Units &units, Configuration &config );
    void ReadField( const Block &, const Units &units, Configuration &config );
    void ReadEfield( const Block &, const Units &units, Configuration &config );
    void ReadPermChargeSet( const Block &, const Units &units, Configuration &config );
    void ReadPermDipoleSet( const Block &, const Units &units, Configuration &config );
}

#endif