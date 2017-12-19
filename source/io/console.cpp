#include "io/console.h"
#include "rapidjson/prettywriter.h"
#include <cstdio>
#include <string>
#include <vector>

using namespace rapidjson;

FieldFit::SystemResult::SystemResult( const std::string &name, size_t numSites ) :
    name( name )
{
    fitResults.resize( numSites );
}


FieldFit::SystemResult::~SystemResult()
{
    
}


FieldFit::Message::Message(  const std::string &i_ns,
                             const std::string &i_source,
                             const std::string &i_message ) :
                             ns( i_ns ), source( i_source ), message( i_message )
{
    
}

std::string FieldFit::Message::Source() const
{
    return ns + "::" + source;   
}

std::string FieldFit::Message::Compose() const
{
    return Source() + " -> " + message;
}

void FieldFit::Console::Warn( const Message &msg )
{
    mWarnings.push_back(msg);
}

void FieldFit::Console::Error( const Message &msg )
{
    mErrors.push_back(msg);
}  

void FieldFit::Console::AddSystemResult( const SystemResult &sr )
{
    mSystemResults.push_back( sr );
}

void FieldFit::Console::Write( std::ostream &stream, const Units *units, bool plain, bool verbose )
{
    if ( plain )
    {
        WritePlain(std::cout, units, verbose);
    }
    else 
    {
        WriteJson(std::cout, units, verbose);
    }
}

void FieldFit::Console::WritePlain( std::ostream &stream, const Units *units, bool verbose )
{
    for ( const Message &msg : mWarnings )
    {
        stream << "[runtime]: " << msg.Compose()  << std::endl;
    }
    
    for ( const Message &msg : mErrors )
    {
        stream << "[error]: " << msg.Compose() << std::endl;
    }
}

void FieldFit::Console::WriteJson( std::ostream &stream, const Units *units, bool verbose )
{
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    
    writer.StartObject();
    {
        // only write data if we have units available
        if ( units )
        {
            writer.Key("fits");
            writer.StartObject();
            for ( const SystemResult &system : mSystemResults )
            {
                system.Serialize( writer, *units, verbose );
            }
            writer.EndObject();
        }
        
        writer.Key("runtime");
        writer.StartArray();
        for ( const Message &msg : mWarnings )
        {
            writer.StartObject();
            std::string msgSource = msg.Source();
            writer.Key("namespace");
            writer.String(msg.ns.c_str());
            writer.Key("source");
            writer.String(msg.source.c_str());
            writer.Key("message");
            writer.String(msg.message.c_str());
            writer.EndObject();
        }
        writer.EndArray();
        
        writer.Key("error");
        writer.StartArray();
        for ( const Message &msg : mErrors )
        {
            writer.StartObject();
            std::string msgSource = msg.Source();
            writer.Key("namespace");
            writer.String(msg.ns.c_str());
            writer.Key("source");
            writer.String(msg.source.c_str());
            writer.Key("message");
            writer.String(msg.message.c_str());
            writer.EndObject();
        }
        writer.EndArray();
    }
    writer.EndObject();
    
    stream << sb.GetString() << std::endl;
}