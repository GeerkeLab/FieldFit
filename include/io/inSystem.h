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
    
    Units* ReadUnits( BlockParser & );
    
    void ReadGrids( BlockParser &, const Units &units, Configuration &config );
    void ReadFields( BlockParser &, const Units &units, Configuration &config, const std::vector< U32 > &collectionSelection );
    void ReadEfields( BlockParser &, const Units &units, Configuration &config );
    void ReadSystems( BlockParser &, const Units &units, Configuration &config );
    void ReadPermChargeSets( BlockParser &, const Units &units, Configuration &config );
    void ReadPermDipoleSets( BlockParser &, const Units &units, Configuration &config );

    System* ReadSystem( const Block &, const Units &units );

    void ReadGrid( const Block &, const Units &units, Configuration &config );
    void ReadField( const Block &, const Units &units, Configuration &config, const std::vector< U32 > &collectionSelection );
    void ReadEfield( const Block &, const Units &units, Configuration &config );
    void ReadPermChargeSet( const Block &, const Units &units, Configuration &config );
    void ReadPermDipoleSet( const Block &, const Units &units, Configuration &config );
}

#endif