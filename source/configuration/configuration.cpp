#include "configuration/configuration.h"

#include "common/exception.h"

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

void FieldFit::Configuration::InsertSystem( System *sys )
{
    if ( !sys )
    {
        throw ArgException( "FieldFit", "Configuration::InsertSystem", "Tried to insert and NULL system" );
    }
    
    auto nts_it = mNameToSystem.find( sys->GetName() );
    
    if ( nts_it != mNameToSystem.end() )
    {
        throw ArgException( "FieldFit", "Configuration::InsertSystem", "System with name "+sys->GetName()+" already exists" );
    }
   
    mSystems.push_back( sys );
    mNameToSystem.insert( std::make_pair( sys->GetName(), sys ) );
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

std::vector<FieldFit::System*> & FieldFit::Configuration::GetSystems()
{
    return mSystems;
}

const std::vector<FieldFit::System*> &  FieldFit::Configuration::GetSystems() const
{
    return mSystems;
}