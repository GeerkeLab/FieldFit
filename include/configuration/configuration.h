#pragma once
#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "configuration/system.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace FieldFit
{
    class Configuration
    {
    public:
        
        Configuration();
        ~Configuration();
        
        System *FindSystem( const std::string &name );
    
    protected:
    
    
    
    private:
        
        std::vector<System*> mSystems;
        std::unordered_map< std::string, System*> mNameToSystem; 
    };
}

#endif