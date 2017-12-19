#include "configuration/fitType.h"

#include "common/exception.h"

U32 FieldFit::StringTypeToFitFlags( const std::string &fitFlags )
{
    U32 flags = 0;
    
    if ( fitFlags == "charge" )
    {   
        flags |= 1 << FitType::charge;
    }
    else if ( fitFlags == "dipole_x" )
    {
        flags |= 1 << FitType::dipoleX;
    }
    else if ( fitFlags == "dipole_xy" )
    {
        flags |= 1 << FitType::dipoleX;
        flags |= 1 << FitType::dipoleY;
    }
    else if ( fitFlags == "dipole_xz" )
    {
        flags |= 1 << FitType::dipoleX;
        flags |= 1 << FitType::dipoleZ;
    }
    else if ( fitFlags == "dipole_y" )
    {
        flags |= 1 << FitType::dipoleY;
    }
    else if ( fitFlags == "dipole_yz" )
    {
        flags |= 1 << FitType::dipoleY;
        flags |= 1 << FitType::dipoleZ;
    }
    else if ( fitFlags == "dipole_z" )
    {
        flags |= 1 << FitType::dipoleZ;
    }
    else if ( fitFlags == "dipole" )
    {
        flags |= 1 << FitType::dipoleX;
        flags |= 1 << FitType::dipoleY;
        flags |= 1 << FitType::dipoleZ;
    }
    else if (  fitFlags == "alpha" )
    {
        flags |= 1 << FitType::dipoleX;
        flags |= 1 << FitType::dipoleY;
        flags |= 1 << FitType::dipoleZ;
        flags |= 1 << SpecialFlag::alpha;
    }
    else if ( fitFlags == "qpol" )
    {
        flags |= 1 << FitType::qd20;
        flags |= 1 << FitType::qd21c;
        flags |= 1 << FitType::qd21s;
        flags |= 1 << FitType::qd22c;
        flags |= 1 << FitType::qd22s;
    }
    else if ( fitFlags == "charge|dipole" )
    {
        flags |= 1 << FitType::charge;
        flags |= 1 << FitType::dipoleX;
        flags |= 1 << FitType::dipoleY;
        flags |= 1 << FitType::dipoleZ;
    }
    else if ( fitFlags == "charge|dipole|qpol" )
    {
        flags |= 1 << FitType::charge;
        flags |= 1 << FitType::dipoleX;
        flags |= 1 << FitType::dipoleY;
        flags |= 1 << FitType::dipoleZ;
        flags |= 1 << FitType::qd20;
        flags |= 1 << FitType::qd21c;
        flags |= 1 << FitType::qd21s;
        flags |= 1 << FitType::qd22c;
        flags |= 1 << FitType::qd22s;
    }
    else
    {
        throw ArgException( "FieldFit", "ReadSystem", "Unknown fitflags "+fitFlags );
    }
    
    return flags;
}

std::string FieldFit::EnumToString( FitType type )
{
    switch( type )
    {
    case FitType::charge:
        	
        return "charge";
        	
    case FitType::dipoleX:
        	
        return "dipoleX";
    
    case FitType::dipoleY:
        	
        return "dipoleY";
    
    case FitType::dipoleZ:
        	
        return "dipoleZ";
    
    case FitType::qd20:
    
        return "qd20";
    
    case FitType::qd21c:
    
        return "qd21c";
    
    case FitType::qd21s:
    
        return "qd21s";
    
    case FitType::qd22c:
    
        return "qd22c";
    
    case FitType::qd22s:
    
        return "qd22s";
    };
        
    return "Undefined";
}

bool FieldFit::IsSet( U32 flags, FitType type )
{
    const U32 flagValue = ( 1 << type );
    return ( (flags) & (flagValue) ) == (flagValue);
}

bool FieldFit::IsMultiSet( U32 flags, U32 testValue )
{
    return ( (flags) & (testValue) ) == (testValue);
}

bool FieldFit::IsSpecialSet( U32 flags, SpecialFlag sf )
{
    const U32 flagValue = ( 1 << sf );
    return ( (flags) & (flagValue) ) == (flagValue);
}