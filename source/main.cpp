#include "common/util.h"
#include "common/exception.h"

#include "io/block.h"
#include "io/console.h"
#include "io/inSystem.h"
#include "io/blockParser.h"
#include "io/inConstraints.h"

#include "fitting/fitter.h"

#include "configuration/constraints.h"
#include "configuration/configuration.h"

#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>

using namespace FieldFit;
using namespace std::chrono;

void ReadFilesFromList( const std::string &file, std::vector< std::string > &appList )
{
    std::ifstream stream;
    stream.open( file.c_str(), std::ios::in );
        
    if ( !stream.is_open() )
    {
        throw ArgException( "::", "ReadFilesFromList", "Unable to open file "+file );
    }
        
    std::string line = "";
    while ( getline ( stream, line ) )
    {
        line = Util::Trim( line );
        
        if ( line.size() > 0 )
        {
            //test for comment
            if ( line[0] == '#' )
            {
                continue;
            }
            
            appList.push_back( line );
        }
    }
}

int main(int argc, char** argv)
{
    std::vector< std::string > multiFiles;
    std::vector< std::string > fieldFiles;
    
    std::vector< U32 > collectionSelection;
    
    bool json = false;
    bool plain = false;
    bool debug = false;
    bool verbose = false;
    
    Console console;
    auto t0 = high_resolution_clock::now();
    
	try 
    {  
        const std::string blockDesc = "Block Description: ";
	    TCLAP::CmdLine cmd( blockDesc, ' ', "0.9.1" );
	        
	    TCLAP::SwitchArg verboseSwitch("v","verbose","Print verbose output", cmd, false);
        //TCLAP::SwitchArg plainSwitch("p","plain","Format output as plain", cmd, false);
        TCLAP::SwitchArg debugSwitch("d","debug","Debug print internal matrices", cmd, false); 
        TCLAP::MultiArg<std::string> multiFileArg("f", "files", "File containing field-fit file names", false,"string" );
        TCLAP::UnlabeledMultiArg<std::string> multi( "fieldFiles", "Generic input for field-files containing blocks", false,"string" );
       
        TCLAP::MultiArg<U32> multiSelect("s", "select", "Select a column in the field files (counts for all!)", false,"U32" );

        cmd.add( multiFileArg );
        cmd.add( multiSelect );
        
        //make sure this is last
        cmd.add(  multi );
	    cmd.parse( argc, argv );
       
        multiFiles = multiFileArg.getValue();
        fieldFiles = multi.getValue();
       
        collectionSelection = multiSelect.getValue();
        std::sort( collectionSelection.begin(), collectionSelection.end() );

        //plain = plainSwitch.getValue();
        verbose = verboseSwitch.getValue();
        debug = debugSwitch.getValue();
	} 
    catch (TCLAP::ArgException &e)  // catch any exceptions
	{ 
        console.Error( Message( "::", "TCLAP::main", e.error() ) );
    }
    
    bool valid_state = true;

    const Units *units = nullptr;
    Configuration config;
    Constraints   constr;

    try 
    {
        // In a first step we grep all the lines from the multifiles
        for ( const std::string &f : multiFiles )
        {
            ReadFilesFromList(f,fieldFiles);
        }
        
        // Initiate reading of the field files
        BlockParser bp(fieldFiles);
        units = ReadUnits( bp );

        ReadSystems( bp, *units, config );
        ReadGrids( bp, *units, config );
        ReadFields( bp, *units, config, collectionSelection );
        ReadEfields( bp, *units, config );
        ReadPermChargeSets( bp, *units, config );
        ReadPermDipoleSets( bp, *units, config );

        // parse constraints
        ReadSumConstraintSet( bp, *units, constr );
        ReadSymConstraintSet( bp, *units, constr );
        
        //clean up after reading
        bp.Clear();
        
        
    }
    catch (FieldFit::ArgException &e)  // catch any exceptions
	{ 
        console.Error( e.GenMessage() );
        valid_state = false;
    }

    auto t1 = high_resolution_clock::now();

    try 
    {
        if (valid_state)
        {
            // start data generation
            for ( FieldFit::System *sys : config.GetSystems() )
            {
                if (sys)
                {
                    sys->OnUpdate2();
                }   
            }

            Fitter fitter; 
            fitter.Fit( console, config, constr, debug );
        }
    }
    catch (FieldFit::ArgException &e)  // catch any exceptions
	{ 
        console.Error( e.GenMessage() );
    }

    auto t2 = high_resolution_clock::now();
    console.Warn( Message( "", "main", "Runtime (seconds): " + Util::ToString( (size_t)duration_cast<seconds>( t2 - t0).count() ) ) );
    console.Warn( Message( "", "main", "Parsing (seconds): " + Util::ToString( (size_t)duration_cast<seconds>( t1 - t0).count() ) ) );
    console.Warn( Message( "", "main", "Solving (seconds): " + Util::ToString( (size_t)duration_cast<seconds>( t2 - t1).count() ) ) );

    console.Write(std::cout, units, plain, verbose);

    // if allocated release
    if ( units )
    {
        delete units;
    }
}