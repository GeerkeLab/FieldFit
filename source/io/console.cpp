#include "io/console.h"

FieldFit::Message::Message(  const std::string &i_ns,
                             const std::string &i_source,
                             const std::string &i_message ) :
                             ns( i_ns ), source( i_source ), message( i_message )
{
    
}

void FieldFit::Console::Warn( const Message &msg )
{
    mWarnings.push_back(msg);
}

void FieldFit::Console::Error( const Message &msg )
{
    mErrors.push_back(msg);
}  

void FieldFit::Console::WritePlain( std::ostream &stream )
{
    for ( const Message &msg : mWarnings )
    {
        stream << "warning: " << msg.ns << "::" << msg.source << " -> " << msg.message << std::endl;
    }
    
    for ( const Message &msg : mErrors )
    {
        stream << "[error]: " << msg.ns << "::" << msg.source << " -> " << msg.message << std::endl;
    }
}