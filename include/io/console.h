#pragma once
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "io/units.h"
#include "common/types.h"
#include "configuration/fitType.h"

#include <string>
#include <vector>
#include <iostream>

namespace FieldFit
{
    struct FitResult
    {
        template <typename Writer>
        void SerializeArray( Writer& writer, const std::vector<F64> &vec, const std::string &name, const F64 conv_factor, bool verbose ) const; 
        
        template <typename Writer>
        void Serialize( Writer& writer, const Units &units, bool verbose ) const;
        
        std::string name;  
        std::vector< std::string > coulTypes;
        std::vector< F64 > values[FitType::size];
        std::vector< F64 > efX;
        std::vector< F64 > efY;
        std::vector< F64 > efZ;
        std::vector< F64 > alphaX;
        std::vector< F64 > alphaY;
        std::vector< F64 > alphaZ;
        std::vector< F64 > alpha;
    };
    
    struct SystemResult
    {
        SystemResult( const std::string &name, size_t numSites );
        ~SystemResult();

        template <typename Writer>
        void Serialize( Writer& writer, const Units &units, bool verbose ) const; 
        
        std::string name;  
        std::vector< FitResult > fitResults;
        std::vector< F64 > chi2;
        std::vector< F64 > rmsd;
    };
    
    struct Message
    {
        Message( const std::string &i_ns,
                 const std::string &i_source,
                 const std::string &i_message );
    
        std::string Source() const;
        std::string Compose() const;
    
        std::string ns;
        std::string source;
        std::string message;
    };
    
    class Console
    {
    public:
    
        void Warn( const Message &msg );
        void Error( const Message &msg );   
    
        void AddSystemResult( const SystemResult &sr );
        
        void Write( std::ostream &stream, const Units *units, bool plain, bool verbose );
        
        
    private:

        void WritePlain( std::ostream &stream, const Units *units, bool verbose );
        void WriteJson( std::ostream &stream, const Units *units, bool verbose );
        	
        std::vector< Message > mWarnings;
        std::vector< Message > mErrors;
        std::vector< SystemResult > mSystemResults;
    };
    

    template <typename Writer>
    void FitResult::SerializeArray( Writer& writer, const std::vector<F64> &vec, const std::string &name, const F64 conv_factor, bool verbose ) const
    {
        if ( vec.size() > 0 || verbose )
        { 
            writer.Key(name.c_str());
            writer.StartArray();
            for ( F64 val : vec )
            {
                writer.Double( val / conv_factor );
            }
            writer.EndArray();
        }
    }

    template <typename Writer>
    void FitResult::Serialize( Writer& writer, const Units &units, bool verbose ) const 
    {
       writer.Key(name.c_str());
       writer.StartObject();

       writer.Key("fit_keys");
       writer.StartArray();
       for ( const std::string &fit_key : coulTypes )
       {
           writer.String(fit_key.c_str());
       }
       writer.EndArray();

       writer.Key("fit_result");
       writer.StartObject();            
       for ( S32 t=0; t < FitType::size; ++t )
       {
           FitType fitType = (FitType) t;
            	
           if ( values[t].size() > 0 || verbose )
           {
               std::string fitName = EnumToString( fitType );
                    
               writer.Key(fitName.c_str());
               writer.StartArray();
               for ( F64 val : values[t] )
               {
                   writer.Double( units.FromInternalUnits( fitType, val ) );
               }
               writer.EndArray();
           }
        }
            
        SerializeArray( writer, alpha,  "alpha",  units.GetAlphaConv(), verbose );
        SerializeArray( writer, alphaX, "alphaX", units.GetAlphaConv(), verbose );
        SerializeArray( writer, alphaY, "alphaY", units.GetAlphaConv(), verbose );
        SerializeArray( writer, alphaZ, "alphaZ", units.GetAlphaConv(), verbose );

        SerializeArray( writer, efX, "efX", units.GetEfieldConv(), verbose );
        SerializeArray( writer, efY, "efY", units.GetEfieldConv(), verbose );
        SerializeArray( writer, efZ, "efZ", units.GetEfieldConv(), verbose );
        
        writer.EndObject();      
        writer.EndObject();
    }    
    
    template <typename Writer>
    void SystemResult::Serialize( Writer& writer, const Units &units, bool verbose ) const 
    {
        writer.Key(name.c_str());
        writer.StartObject();
        {
             writer.Key("sites");
             writer.StartObject();
             for ( const FitResult &result : fitResults )
             {
                 result.Serialize( writer, units, verbose );
             }
             writer.EndObject();
                 
             writer.Key("chi2");
             writer.StartArray();
             for ( F64 val : chi2)
             {
                writer.Double( val );
             }
             writer.EndArray();
                 
             writer.Key("rmsd");
             writer.StartArray();
             for ( F64 val : rmsd)
             {
                 writer.Double( val );
             }
             writer.EndArray();
        }
        writer.EndObject();
    }
}

#endif
