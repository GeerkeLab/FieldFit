#include <string>
#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>

int main(int argc, char** argv)
{
	try 
    {  
	   TCLAP::CmdLine cmd("Command description message", ' ', "0.9");
	   TCLAP::ValueArg<std::string> nameArg("n","name","Name to print",true,"homer","string");

	   cmd.add( nameArg );

	   TCLAP::SwitchArg reverseSwitch("r","reverse","Print name backwards", cmd, false);

	   cmd.parse( argc, argv );
       
       
       
	} 
    catch (TCLAP::ArgException &e)  // catch any exceptions
	{ 
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
    }
}