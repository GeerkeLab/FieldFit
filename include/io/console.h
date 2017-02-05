#pragma once
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <string>
#include <vector>
#include <iostream>

namespace FieldFit
{
    struct Message
    {
        Message( const std::string &i_ns,
                 const std::string &i_source,
                 const std::string &i_message );
    
        std::string ns;
        std::string source;
        std::string message;
    };
    
    class Console
    {
    public:
        
        void Warn( const Message &msg );
        void Error( const Message &msg );   
    
        void WritePlain( std::ostream &stream );
        
    private:
        	
        std::vector< Message > mWarnings;
        std::vector< Message > mErrors;
    };
}

#endif