#include "common/util.h"
#include "common/exception.h"

#include "io/block.h"
#include "io/inSystem.h"
#include "io/blockParser.h"

#include "configuration/configuration.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>

using namespace FieldFit;

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
    
    bool json = false;
    bool verbose = false;
    
	try 
    {  
        const std::string blockDesc = "Block Description: ";
	    TCLAP::CmdLine cmd( blockDesc, ' ', "0.9" );
	        
	    TCLAP::SwitchArg verboseSwitch("v","verbose","Print verbose output", cmd, false);
        TCLAP::SwitchArg jsonSwitch("j","json","Format output as json", cmd, false);
       
        TCLAP::MultiArg<std::string> multiFileArg("f", "files", "File containing field-fit file names", false,"string" );
        TCLAP::UnlabeledMultiArg<std::string> multi( "fieldFiles", "Generic input for field-files containing blocks", false,"string" );
       
        cmd.add( multiFileArg );
        //make sure this is last
        cmd.add(  multi );
	    cmd.parse( argc, argv );
       
        multiFiles = multiFileArg.getValue();
        fieldFiles = multi.getValue();
       
        json = jsonSwitch.getValue();
        verbose = verboseSwitch.getValue();
	} 
    catch (TCLAP::ArgException &e)  // catch any exceptions
	{ 
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
    }
    
    try 
    {
        // In a first step we grep all the lines from the multifiles
        for ( const std::string &f : multiFiles )
        {
            ReadFilesFromList(f,fieldFiles);
        }
        
        Configuration config;
        
        // Initiate reading of the field files
        BlockParser bp(fieldFiles);
        
        const Units units = ReadUnits( bp );
        
        ReadSystems( bp, units, config );
        ReadGrids( bp, units, config );
        ReadFields( bp, units, config );
        ReadEfields( bp, units, config );
        
        //clean up after reading
        bp.Clear();
    }
    catch (FieldFit::ArgException &e)  // catch any exceptions
	{ 
        std::cerr << "error: " << e.what() << std::endl; 
    }
}