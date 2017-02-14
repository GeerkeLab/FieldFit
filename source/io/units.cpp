#include "io/units.h"
#include "io/block.h"

using namespace FieldFit;

#include <map>

Units::Units() :
    mCoordConv( 0.0 ), mChargeConv( 0.0 ),
    mDipoleConv( 0.0 ), mQpolConv( 0.0 ),
    mPotConv( 0.0 ), mEfieldConv( 0.0 ),
    mAlphaConv( 0.0 )
{
    
}

F64 Units::GetAlphaConv() const
{
    return mAlphaConv;
}

F64 Units::GetCoordConv() const
{
    return mCoordConv;
}

F64 Units::GetChargeConv() const
{
    return mChargeConv;
}

F64 Units::GetDipoleConv() const
{
    return mDipoleConv;   
}

F64 Units::GetQpolConv() const
{
    return mQpolConv;
}

F64 Units::GetPotConv() const
{
    return mPotConv;
}

F64 Units::GetEfieldConv() const
{
    return mEfieldConv;
}

//

F64 &Units::GetAlphaConv() 
{
    return mAlphaConv;
}

F64 &Units::GetCoordConv() 
{
    return mCoordConv;
}

F64 &Units::GetChargeConv() 
{
    return mChargeConv;
}

F64 &Units::GetDipoleConv() 
{
    return mDipoleConv;   
}

F64 &Units::GetQpolConv() 
{
    return mQpolConv;
}

F64 &Units::GetPotConv() 
{
    return mPotConv;
}

F64 &Units::GetEfieldConv() 
{
    return mEfieldConv;
}

F64 Units::ToInternalUnits( FitType type, F64 val ) const
{
    F64 result = val;
    
    switch( type )
    {
    case FitType::charge:
        	
        result *= mChargeConv;
        break;
    case FitType::dipoleX:
        	
        result *= mDipoleConv;
        break;
    case FitType::dipoleY:
        	
        result *= mDipoleConv;
        break;
    case FitType::dipoleZ:
        	
        result *= mDipoleConv;
        break;
    case FitType::qd20:
    
        result *= mQpolConv;
        break;
    case FitType::qd21c:
    
        result *= mQpolConv;
        break;
    case FitType::qd21s:
    
        result *= mQpolConv;
        break;
    case FitType::qd22c:
    
        result *= mQpolConv;
        break;
    case FitType::qd22s:
    
        result *= mQpolConv;
        break;
    };
    
    return result;
}

F64 Units::FromInternalUnits( FitType type, F64 val ) const
{
    F64 result = val;
    
    switch( type )
    {
    case FitType::charge:
        	
        result /= mChargeConv;
        break;
    case FitType::dipoleX:
        	
        result /= mDipoleConv;
        break;
    case FitType::dipoleY:
        	
        result /= mDipoleConv;
        break;
    case FitType::dipoleZ:
        	
        result /= mDipoleConv;
        break;
    case FitType::qd20:
    
        result /= mQpolConv;
        break;
    case FitType::qd21c:
    
        result /= mQpolConv;
        break;
    case FitType::qd21s:
    
        result /= mQpolConv;
        break;
    case FitType::qd22c:
    
        result /= mQpolConv;
        break;
    case FitType::qd22s:
    
        result /= mQpolConv;
        break;
    };
    
    return result;
}