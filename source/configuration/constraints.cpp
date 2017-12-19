#include "configuration/constraints.h"

FieldFit::PrototypeConstraint::PrototypeConstraint( ConstrType type ) :
    mFc( 0.0 ), mTarget( 0.0 ), mTargetFlags( 0 ), mType( type ) 
{
    
}

void FieldFit::PrototypeConstraint::AddCoulType( const std::string &ctype )
{
    mColReferences.insert( ctype );
}

void FieldFit::PrototypeConstraint::SetForceConstant( F64 fc )
{
    mFc = fc;
}

void FieldFit::PrototypeConstraint::SetTarget( F64 target )
{
    mTarget = target;
}

void FieldFit::PrototypeConstraint::SetFlags( U32 flags )
{
    mTargetFlags = flags;
}
        
const std::unordered_set< std::string > &FieldFit::PrototypeConstraint::GetCoulTypes() const
{
    return mColReferences;
}

F64 FieldFit::PrototypeConstraint::GetForceConstant() const
{
    return mFc;
}

F64 FieldFit::PrototypeConstraint::GetTarget() const
{
    return mTarget;
}

FieldFit::ConstrType FieldFit::PrototypeConstraint::GetType() const
{
    return mType;
}

F64 FieldFit::PrototypeConstraint::GetFlags() const
{
    return mTargetFlags;
}

void FieldFit::Constraints::InsertContraint( const PrototypeConstraint &pconstr )
{
    mConstraints.push_back(pconstr);
}

const std::vector< FieldFit::PrototypeConstraint > & FieldFit::Constraints::GetConstraints() const
{
    return mConstraints;
}