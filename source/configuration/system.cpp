#include "configuration/system.h"

FieldFit::Site::Site( const std::string &name,
                      const std::string &coultype,
                      const F64 coordX,
                      const F64 coordY,
                      const F64 coordZ ) :
     mName( name ), mCoulType( coultype ),
     mCoordX( coordX ), mCoordY( coordY ), mCoordZ( coordZ )
{
    
}

FieldFit::Site::~System()
{
    mNameToSite.clear();
    mCoulTypeToSite.clear();
    
    for( Site *site : mSites )
    {
        delete site;
    }
}

void FieldFit::Site::AddFitType( FitType type )
{
    mTypes[type] = true;
}

FieldFit::System::System( const std::string &name ) :
    mName( name )
{
    
}

void FieldFit::System::InsertSite( Site *site )
{
    if ( !site )
    {
        throw ArgException( "FieldFit", "ReadSystem", "Unknown fitflags "+fitFlags );
    }
    
    /// TODODODO
}

const std::string &FieldFit::System::GetName() const
{
    return mName;
}