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
