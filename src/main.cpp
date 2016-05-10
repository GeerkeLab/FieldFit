#include "system/field.h"
#include "system/configuration.h"

#include "fitting/fitter.h"
#include "error/error.h"
#include "io/blockParser.h"
#include "io/block.h"

#include "testing/testing.h"

#include "system/report.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>

class CommandLine
{
public:
	CommandLine( )
	{
		options.insert( std::make_pair( "-v", false ) );
		options.insert( std::make_pair( "--showinput", false ) );
	}
	
	void ShowHelp()
	{
		std::cout << "   ________________________________________________________________________________                        " << std::endl
		          << "**                                                                                                         " << std::endl
		          << "**                                                                                                         " << std::endl
				  << "**                FIELDFITTER   V1.1.0                                                                     " << std::endl
				  << "**                                                                                                         " << std::endl
				  << "**                Author; K.M.Visscher                                                                     " << std::endl
		          << "**                                                                                                         " << std::endl
		          << "**_________________________________________________________________________________                        " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "* <<< Command line options >>>                                                                             " << std::endl
		          << "*    -v                              =>   Show verbose output                                              " << std::endl
		          << "*    --showinput                     =>   Show input fields                                                " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*  --------------------------------------------------------------------------------                        " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "* <<< Input blocks syntax >>>                                                                              " << std::endl
		          << "*    - {}                            =>   Denotes a variable                                               " << std::endl
		          << "*    - {I}                           =>   Denotes an independent variable (Upper-case)                     " << std::endl
		          << "*    - {i}                           =>   Denotes an iterator    variable (lower-case)                     " << std::endl
		          << "*    - {CHARGE_i}                    =>   Denotes a linked variable ( at i )                               " << std::endl
		          << "*    - FOR {localVar} IN {SET}       =>   Describes repeating fields                                       " << std::endl
		          << "*    - SIZE( {FLAGS} )               =>   Number of unique types in flags                                  " << std::endl
		          << "*    - #                             =>   Comment line                                                     " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*  -------------------------------------------------------------------------------                         " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "* <<< Type flags syntax >>>                                                                                " << std::endl
		          << "*    - 100000000                     =>   Charge                                                           " << std::endl
		          << "*    - 010000000                     =>   Dipole-x                                                         " << std::endl
		          << "*    - 001000000                     =>   Dipole-y                                                         " << std::endl
		          << "*    - 000100000                     =>   Dipole-z                                                         " << std::endl
		          << "*    - 000010000                     =>   Qp-qd20                                                          " << std::endl
		          << "*    - 000001000                     =>   Qp-qd21c                                                         " << std::endl
		          << "*    - 000000100                     =>   Qp-qd21s                                                         " << std::endl
		          << "*    - 000000010                     =>   Qp-qd22c                                                         " << std::endl
		          << "*    - 000000001                     =>   Qp-qd22s                                                         " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    - To fit multiple types, apply additon to the flags, ie, 011100000 ( dipole in x,y and z )            " << std::endl
		          << "*    - Note! trailing zeros are redundant  0111 == 011100000                                               " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*  -------------------------------------------------------------------------------                         " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "* <<< Input blocks >>>                                                                                     " << std::endl
		          << "*    [FIELD]                                                                                               " << std::endl
		          << "*        # QM field sites                                                                                  " << std::endl
		          << "*        {N}                                                                                               " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*        FOR {i} IN {N}                                                                                    " << std::endl
		          << "*            {POTENTIAL_i} {XCOORD_i} {YCOORD_i} {ZCOORD_i}                                                " << std::endl
		          << "*    [END]                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    -----                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    [SITE]                                                                                                " << std::endl
		          << "*        # QM atom sites                                                                                   " << std::endl
		          << "*        {A}                                                                                               " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*        FOR {i} IN {A}                                                                                    " << std::endl
		          << "*            #the id links the sites to fitsites permsites and constraints ( must be unique )              " << std::endl
		          << "*            {ID_i} {NAME_i} {ATOMIC-CHARGE_i} {XCOORD_i} {YCOORD_i} {ZCOORD_i}                            " << std::endl                    
		          << "*    [END]                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    -----                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    [FITSITES]                                                                                            " << std::endl
		          << "*        # Number of fit sites                                                                             " << std::endl
		          << "*        {FSITES}                                                                                          " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*        FOR {i} IN {FSITES}                                                                               " << std::endl
		          << "*            {ID_i} {TYPE-FLAGS_i}                                                                         " << std::endl   
		          << "*    [END]                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    -----                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    [PERMSITES]                                                                                           " << std::endl
		          << "*        # Number of permanent sites                                                                       " << std::endl
		          << "*        {PSITES}                                                                                          " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*        FOR {i} IN {PSITES}                                                                               " << std::endl
		          << "*            {ID_i} {TYPE-FLAGS_i}                                                                         " << std::endl  
		          << "*                                                                                                          " << std::endl
		          << "*            FOR {j} IN SIZE( {TYPE-FLAGS_i} )                                                             " << std::endl
		          << "*                # permanent value for a type j                                                            " << std::endl
		          << "*                {CONSTRAINT-REFERENCE_i_j}                                                                " << std::endl 
		          << "*    [END]                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    -----                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    [VALUECONSTR]                                                                                         " << std::endl
		          << "*        # Number of value constraints                                                                     " << std::endl
		          << "*        {VCONSTR}                                                                                         " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*        FOR {i} IN {VCONSTR}                                                                              " << std::endl
		          << "*            {ID_i} {TYPE-FLAGS_i}                                                                         " << std::endl 
		          << "*                                                                                                          " << std::endl
		          << "*            FOR {j} IN SIZE( {TYPE-FLAGS_i} )                                                             " << std::endl
		          << "*                # value to constraint to                                                                  " << std::endl
		          << "*                {CONSTRAINT-REFERENCE_i_j}                                                                " << std::endl 
		          << "*    [END]                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    -----                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    [SYMCONSTR]                                                                                           " << std::endl
		          << "*        # Number of symmetry constraints                                                                  " << std::endl
		          << "*        {SCONSTR}                                                                                         " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*        FOR {i} IN {SCONSTR}                                                                              " << std::endl
		          << "*            #number of ids for symmetry                                                                   " << std::endl
		          << "*            {NUM-IDS_i}                                                                                   " << std::endl 
		          << "*                                                                                                          " << std::endl
		          << "*            FOR {j} IN {NUM-IDS_i}                                                                        " << std::endl
		          << "*                # an id to use for the sym constr                                                         " << std::endl
		          << "*                {ID_i_j}                                                                                  " << std::endl 
		          << "*                                                                                                          " << std::endl
		          << "*           {TYPE-FLAGS_i}                                                                                 " << std::endl
		          << "*    [END]                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    -----                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    [SUMCONSTR]                                                                                           " << std::endl
		          << "*        # Number of symmetry constraints                                                                  " << std::endl
		          << "*        {SCONSTR}                                                                                         " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*        FOR {i} IN {SCONSTR}                                                                              " << std::endl
		          << "*            #number of ids for symmetry                                                                   " << std::endl
		          << "*            {NUM-IDS_i}                                                                                   " << std::endl 
		          << "*                                                                                                          " << std::endl
		          << "*            FOR {j} IN {NUM-IDS_i}                                                                        " << std::endl
		          << "*                # an id and flags to use for the sym constr                                               " << std::endl
		          << "*                {ID_i_j} {TYPE-FLAGS_i_j}                                                                 " << std::endl 
		          << "*                                                                                                          " << std::endl
		          << "*            {REFERENCE_i}                                                                                 " << std::endl
		          << "*    [END]                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    -----                                                                                                 " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*    [RESP]                                                                                                " << std::endl
		          << "*        # Number of symmetry constraints                                                                  " << std::endl
		          << "*        {R-RESTRAINTS}                                                                                    " << std::endl
		          << "*                                                                                                          " << std::endl
		          << "*        FOR {i} IN {R-RESTRAINTS}                                                                         " << std::endl
		          << "*            # an id and flags to use for the Resp restraint                                               " << std::endl                                             
		          << "*            {ID_i} {TYPE-FLAGS_i}                                                                         " << std::endl 
		          << "*                                                                                                          " << std::endl
		          << "*            FOR {j} IN SIZE( {TYPE-FLAGS_i} )                                                             " << std::endl
		          << "*                 #target value for resp                                                                   " << std::endl
		          << "*                 {TARGET-VALUE_i_j}                                                                       " << std::endl 
		          << "*                                                                                                          " << std::endl
		          << "*            FOR {j} IN SIZE( {TYPE-FLAGS_i} )                                                             " << std::endl
		          << "*                 #force constant for resp                                                                 " << std::endl
		          << "*                 {FORCE-CONST_i_j}                                                                        " << std::endl 
		          << "*                                                                                                          " << std::endl
		          << "*    [END]                                                                                                 " << std::endl;

	}
	
	Error::STATUS Parse( int argc, char** argv )
	{
		for ( S32 i=0; i < argc; ++i )
		{
			std::string argument = std::string( argv[i] );
			
			if ( argument.compare( "-h" )    == 0 ||
				 argument.compare( "-help" ) == 0  )
			{
				ShowHelp();
				
				return Error::STATUS::FAILED_COMMAND_PARSE;
			}
			else if ( argument[0] == '-' )
			{
				std::map< std::string, bool >::iterator it = options.find( argument );
				
				if ( it == options.end() )
				{
					Error::Warn( std::cout, std::string("Option ")+argument+" is not a valid command line option!" );
					
					return Error::STATUS::FAILED_COMMAND_PARSE;
				}
					
				it->second = true;
			}
			else
			{
				files.push_back( argument );
			}
		}
		
		return Error::STATUS::OK;
	}
	
	bool GetOption( const std::string &option )
	{
		std::map< std::string, bool >::iterator it = options.find( option );
		
		if ( it != options.end() )
		{
			return	it->second;		
		}
		
		return false;
	}
	
	const std::vector< std::string > &GetFiles()
	{
		return files;
	} 
	
private:
	
	std::map< std::string, bool > options;
	std::vector< std::string > files;
};


int main( int argc, char** argv)
{	
	CommandLine cline;
	
	if ( Error::FAILED( cline.Parse( argc, argv ) ) )
	{
        return 1;	
	}
	
	BlockParser bp( cline.GetFiles() );

    if ( Error::FAILED( bp.GetStatus() ) )
    {
        Error::Warn( std::cout, "Unable to open all presented files!" );
        return 1;
    }  
    
    Field field( bp );
    
    if ( Error::FAILED( field.GetStatus() ) )
    {
    	Error::Warn( std::cout, "Unable to generate a field input, fatal error!" );

    	return 1;
    }
    
    Configuration conf( bp );

    if ( Error::FAILED( conf.GetStatus() ) )
    {
    	Error::Warn( std::cout, "Unable to generate a configuration input, fatal error!" );
    	
        return 1;
    }
    
    if ( cline.GetOption("--showinput") )
    {
    	conf.Raport( std::cout );
    }
    
    if ( field.NumCollections() != conf.NumCollections() )
    {
        Error::Warn( std::cout, "Number of collections (Data frames) in the configuration section is not the same as the number of field collections!" );
        
        return 1;
    }
    
    //init the permanent field
    if ( Error::FAILED( field.SetPermField( conf ) ) )
    {
    	Error::Warn( std::cout, "Unable to set up the permanent field contributions, fatal error!" );
    	
        return 1;	
    }
    
    if ( Error::FAILED( Fitter::FitSites( conf, field, cline.GetOption("-v") ) ) )
    {
    	Error::Warn( std::cout, "Unable to fit the selected sites, fatal error!" );

    	return 1;
    }
    
    Report rp( conf, field, cline.GetOption("-v") );
    rp.WriteToStream( std::cout );
    
    //test the final values to fortran values if a test block was given
    if ( Error::FAILED(  Testing::CompareToF( conf, bp ) ) )
    {
    	Error::Warn( std::cout, "System failed the unit test, see output!" );

    	return 1;		
    }

	return 0;
}