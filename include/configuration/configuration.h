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
        
        void InsertSystem( System *sys );
        
        System *FindSystem( const std::string &name );
        
        std::vector<System*> & GetSystems();
        const std::vector<System*> & GetSystems() const;
        
    private:
        
        std::vector<System*> mSystems;
        std::unordered_map< std::string, System*> mNameToSystem; 
    };
}

#endif