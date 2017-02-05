#include "io/block.h"
#include "io/inSystem.h"
#include "io/blockParser.h"
#include "io/inConstraints.h"

#include "common/exception.h"

#include "configuration/fitType.h"
#include "configuration/constraints.h"

void FieldFit::ReadSumConstraintSet( const BlockParser &bp, const Units &units, Constraints &constr )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("SUMCONSTR");
    
    if ( blockArray )
    {
        for ( const Block &block : *blockArray )
        { 
            ReadSumConstraints(block, units, constr);            
        }
    }
}

void FieldFit::ReadSymConstraintSet( const BlockParser &bp, const Units &units, Constraints &constr )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("SYMCONSTR");
    
    if ( blockArray )
    {
        for ( const Block &block : *blockArray )
        { 
            ReadSymConstraints(block, units, constr);            
        }
    }
}
         
void FieldFit::ReadSumConstraints( const Block &block, const Units &units, Constraints &constr )
{
    if ( block.Size() < 1 )
    {
        throw ArgException( "FieldFit", "ReadSumConstraints", "block [SUMCONSTR] was too small ( at least 1 argument expected ) !" );
    }

    const U32 numConstr = block.GetToken( 0 )->GetValue< U32 >();
    
    U32 index = 1;
    for ( U32 i=0; i < numConstr; ++i )
    {
        if ( index >= block.Size() )
        {
            throw ArgException( "FieldFit", "ReadSumConstraints", "block [SUMCONSTR] did not have the right amount of arguments based on the size indicator!" );
        }
        
        const U32 numItems = block.GetToken( index+0 )->GetValue< U32 >();
        
        if ( ( index + numItems + 1 ) >= block.Size() )
    	{
            throw ArgException( "FieldFit", "ReadSumConstraints", "block [SUMCONSTR] did not have the right amount of arguments based on the size indicator!" );
        }
        
        // advance
        index += 1;
        
        PrototypeConstraint constraint( ConstrType::SumConstr );
        
        for ( U32 j=0; j < numItems; ++j )
        {
            std::string name = block.GetToken( index+0 )->GetToken();
            constraint.AddCoulType( name );  
            index += 1;
        }
        
        const std::string fitFlags = block.GetToken( index+0 )->GetToken();
        constraint.SetTarget( block.GetToken( index+1 )->GetValue< F64 >() );
        constraint.SetForceConstant( block.GetToken( index+2 )->GetValue< F64 >() );
        constraint.SetFlags( StringTypeToFitFlags( fitFlags ) );
        
        // Perform a type conversion
        if ( IsSet( constraint.GetFlags(), FitType::charge ) )
        {
            constraint.SetTarget( constraint.GetTarget() * units.GetChargeConv() );
            constraint.SetForceConstant( constraint.GetForceConstant() * units.GetChargeConv() );   	
        }
    	else if ( IsSet( constraint.GetFlags(), FitType::dipoleX ) ||
                  IsSet( constraint.GetFlags(), FitType::dipoleY ) ||
                  IsSet( constraint.GetFlags(), FitType::dipoleZ ) )
        {
            // in case of dipole test if alpha was requested
            if ( IsSpecialSet( constraint.GetFlags(), SpecialFlag::alpha ) )
            {
                // we multiply by 3 here to compensate for the fact that efield constraints do not divide by 3 ( for average ).
                constraint.SetTarget( constraint.GetTarget() * units.GetAlphaConv() * 3 );
                constraint.SetForceConstant( constraint.GetForceConstant() * units.GetAlphaConv() );
            }
            else
            {
                constraint.SetTarget( constraint.GetTarget() * units.GetDipoleConv() );
                constraint.SetForceConstant( constraint.GetForceConstant() * units.GetDipoleConv() );
            }        
        }
        else if ( IsSet( constraint.GetFlags(), FitType::qd20  ) ||
                  IsSet( constraint.GetFlags(), FitType::qd21c ) ||
                  IsSet( constraint.GetFlags(), FitType::qd21s ) ||
                  IsSet( constraint.GetFlags(), FitType::qd22c ) ||
                  IsSet( constraint.GetFlags(), FitType::qd22s ) 
                )
        {
            constraint.SetTarget( constraint.GetTarget() * units.GetQpolConv() );
            constraint.SetForceConstant( constraint.GetForceConstant() * units.GetQpolConv() );
        }
        
        constr.InsertContraint( constraint );
        
        index += 3;
    }
    
    if ( index != block.Size() )
    {
        throw ArgException( "FieldFit", "ReadSumConstraints", "block [SUMCONSTR] did not have the right amount of arguments based on the size indicator!" );
    }
}

void FieldFit::ReadSymConstraints( const Block &block, const Units &units, Constraints &constr )
{
    if ( block.Size() < 1 )
    {
        throw ArgException( "FieldFit", "ReadSymConstraints", "block [SYMCONSTR] was too small ( at least 1 argument expected ) !" );
    }

    U32 constraints = block.GetToken( 0 )->GetValue< U32 >();
        
    if ( block.Size() != ( 1 + ( constraints * 3 ) ) )
    {
        throw ArgException( "FieldFit", "ReadSymConstraints", "block [SYMCONSTR] did not have the right amount of arguments based on the size indicator!" );
    }

    U32 index = 1;

    for ( U32 i=0; i < constraints; ++i )
    {
        PrototypeConstraint constraint( ConstrType::SymConstr );
        
        std::string name     = block.GetToken( index+0 )->GetToken();
        std::string fitFlags = block.GetToken( index+1 )->GetToken();
        
        constraint.AddCoulType( name ); 
        constraint.SetForceConstant( block.GetToken( index+2 )->GetValue< F64 >() );
        constraint.SetFlags( StringTypeToFitFlags( fitFlags ) ); 
        
        // Perform a type conversion
        if ( IsSet( constraint.GetFlags(), FitType::charge ) )
        {
            constraint.SetForceConstant( constraint.GetForceConstant() * units.GetChargeConv() );
        }
    	else if ( IsSet( constraint.GetFlags(), FitType::dipoleX ) ||
                  IsSet( constraint.GetFlags(), FitType::dipoleY ) ||
                  IsSet( constraint.GetFlags(), FitType::dipoleZ ) )
        {
            // in case of dipole test if alpha was requested
            
            if ( IsSpecialSet( constraint.GetFlags(), SpecialFlag::alpha )  )
            {
                 constraint.SetForceConstant( constraint.GetForceConstant() * units.GetAlphaConv() );
            }
            else
            {
                constraint.SetForceConstant( constraint.GetForceConstant() * units.GetDipoleConv() );
            }        
        }
        else if ( IsSet( constraint.GetFlags(), FitType::qd20  ) ||
                  IsSet( constraint.GetFlags(), FitType::qd21c ) ||
                  IsSet( constraint.GetFlags(), FitType::qd21s ) ||
                  IsSet( constraint.GetFlags(), FitType::qd22c ) ||
                  IsSet( constraint.GetFlags(), FitType::qd22s )
                )
        {
            constraint.SetForceConstant( constraint.GetForceConstant() * units.GetQpolConv() );
        }
        
        constr.InsertContraint( constraint );
        
        index += 3;
    }
    
    if ( index != block.Size() )
    {
        throw ArgException( "FieldFit", "ReadSumConstraints", "block [SYMCONSTR] did not have the right amount of arguments based on the size indicator!" );
    }
}