#pragma once
#ifndef __CONSTRAINTS_h__
#define __CONSTRAINTS_h__

#include "common/types.h"

#include <string>
#include <vector>
#include <unordered_set>

namespace FieldFit
{
    enum ConstrType
    {
        SymConstr = 1,
        SumConstr = 2
    };
    
    class PrototypeConstraint
    {
    public:
       
        PrototypeConstraint( ConstrType type );
    
        void AddCoulType( const std::string &ctype );
        void SetForceConstant( F64 fc );
        void SetTarget( F64 target );
        void SetFlags( U32 flags );
        
        const std::unordered_set< std::string > &GetCoulTypes() const;
        F64 GetForceConstant() const;
        ConstrType GetType() const;
        F64 GetTarget() const;
        F64 GetFlags() const;
        
    private:
        
        F64 mFc;
        F64 mTarget;
        U32 mTargetFlags;
        ConstrType mType;
        std::unordered_set< std::string > mColReferences;
    };
    
    class Constraints
    {
    public:
    
        void InsertContraint( const PrototypeConstraint & );
        
        const std::vector< PrototypeConstraint > & GetConstraints() const;
    
    private:
    
        std::vector< PrototypeConstraint > mConstraints;           	    
    };
}

#endif