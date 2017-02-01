#include "configuration/configuration.h"

FieldFit::Configuration::Configuration()
{
    
}

FieldFit::Configuration::~Configuration()
{
    mNameToSystem.clear();
    
    for ( System *sys : mSystems )
    {
        delete sys;
    }
}

FieldFit::System *FieldFit::Configuration::FindSystem( const std::string &name )
{
    auto itf = mNameToSystem.find(name);
    
    if ( itf == mNameToSystem.end() )
    {
        return nullptr;
    }
    
    return itf->second;
}
