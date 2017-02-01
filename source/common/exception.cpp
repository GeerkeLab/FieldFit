#include "common/exception.h"

using namespace FieldFit;

ArgException::ArgException( const std::string &ns, 
                            const std::string &location,  
                            const std::string &description ) :
    	                    std::exception(), 
                            mNamespace(ns), 
                            mLocation(location), 
                            mDescription(description)
{
    
}  	

ArgException::~ArgException() throw() 
{
    
}
        
const char* ArgException::what() const throw() 
{
    std::string val = "[ArgException] " + mNamespace + "::" + mLocation + " -> " + mDescription;
    return val.c_str();
}