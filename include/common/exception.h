#pragma once
#ifndef __FIELD_FIT_EXCEPTION_H__
#define __FIELD_FIT_EXCEPTION_H__

#include <exception>
#include <string>

namespace FieldFit
{  
    class ArgException : public std::exception
    {
    public:
        
        ArgException( const std::string &ns, 
                      const std::string &location,  
                      const std::string &description);
                      
        virtual ~ArgException() throw();
        
        const char* what() const throw();
    
    private:
        
        std::string mNamespace;
        std::string mLocation;
        std::string mDescription;
    };
}

#endif