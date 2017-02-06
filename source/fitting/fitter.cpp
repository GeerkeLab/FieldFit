#include "fitting/fitter.h"

#include "common/exception.h"

#include "configuration/system.h"
#include "configuration/constraints.h"
#include "configuration/configuration.h"

#include <iostream>
#include <map>
#include <math.h>

FieldFit::Fitter::LocalSystem::LocalSystem() :
    sourceSystem( nullptr )
{
    
}

FieldFit::Fitter::InternalConstraint::InternalConstraint() :
    reference( 0.0 ), fconst( 0.0 )
{
    
}

void FieldFit::Fitter::Fit( Console &console, const Configuration &config, const Constraints &constraints, bool debug )
{
    AddConfiguration( console, config );
    AddConstraints( console, constraints );
    
    //
    // Add contraints to the system
    //
    
    if ( mInternalConstraints.size() > x_prime_x.n_cols )
    {
        throw ArgException( "FieldFit", "Fitter::Fit", "The configuration is overconstrained" );
    }
    
    size_t row =  x_prime_x.n_rows;
    x_prime_x.resize( x_prime_x.n_rows + mInternalConstraints.size(), 
                      x_prime_x.n_cols + mInternalConstraints.size() );
    x_prime_y.resize( x_prime_y.n_rows + mInternalConstraints.size() );
    
    for ( const InternalConstraint &constr : mInternalConstraints )
    {
        for ( U32 c=0; c < constr.columns.size(); ++c )
        {
            x_prime_x.row( row )[ constr.columns[c] ] = constr.coefficients[c];
            x_prime_x.col( row )[ constr.columns[c] ] = constr.coefficients[c];
        }
        
        x_prime_y[row] = constr.reference;
        
        row++;
    }
    
    //
    // Generate OLS
    //
    
    if ( debug )
    {
        std::cout << "[A]" << std::endl;
        std::cout << x_prime_x;
        std::cout << "[END]" << std::endl;
    
        std::cout << "[B]" << std::endl;
        std::cout << x_prime_y;
        std::cout << "[END]" << std::endl;
    }
    
    mSolution = solve( x_prime_x, x_prime_y );
    
    WriteSolution(console);
}

void FieldFit::Fitter::WriteSolution(Console &console)
{
    SystemResult systemResult("", 0);
    
    // Transfer to local sytem
    size_t row = 0;
    for ( const LocalSystem &localSys : mLocalSystems )
    { 
        const System *sys = localSys.sourceSystem;
        
        if ( systemResult.name != sys->GetName() )
        {
            if ( systemResult.fitResults.size() > 0 )
            {
                console.AddSystemResult( systemResult );
            }
            
            systemResult = SystemResult( sys->GetName(), sys->GetSites().size() );
        }
        
        const size_t systemCols = sys->NumColumns();
        arma::vec lvec = mSolution.rows( row, row + systemCols - 1 );
        
        F64 chi2 = sys->ComputeChi2( lvec, localSys.collectionIndex );
        systemResult.chi2.push_back( chi2 );
        systemResult.rmsd.push_back( std::sqrt( chi2 / sys->GetGrid()->Size() ) );
        
        size_t col = 0;
        size_t siteIndex = 0;
        for ( const Site *site : sys->GetSites() )
        {
            FitResult &fitResult = systemResult.fitResults[siteIndex];
            fitResult.name = site->GetName();
            
            for ( S32 t=0; t < FitType::size; ++t )
            {
                FitType fitType = (FitType) t;
           
                if ( site->TestFitType( fitType ) )
                {
                    fitResult.values[fitType].push_back( lvec[col] );
                
                    col++;
                }
            }
            
            siteIndex++;
        }
        
        row += systemCols;
    }
    
    // Flush
    if ( systemResult.fitResults.size() > 0 )
    {
        console.AddSystemResult( systemResult );
    }
}

void FieldFit::Fitter::AddConfiguration( Console &console, const Configuration &config )
{
    size_t rowOrigin = 0;
    size_t colOrigin = 0;
    
    for ( const System *sys : config.GetSystems() )
    {
        const arma::mat &localXPrimeX = sys->GetLocalXPrimeX();
        const arma::mat &localXPrimeY  = sys->PotentialMatrix();
        
        //
        // Insert collection selector here!
        //
        // PLACEHOLDER
        mTargetCollections.resize( localXPrimeY.n_cols );
        std::iota(mTargetCollections.begin(), mTargetCollections.end(), 0);
        // END PLACEHOLDER
        
        for ( U32 i : mTargetCollections )
        {
            // this is for later if the selection is user input driven!
            if ( i >= localXPrimeY.n_cols )
            {
                throw ArgException( "FieldFit", "Fitter::AddConfiguration", "Collection selection out of range for system"+sys->GetName() );
            }
            
            LocalSystem localSys;
            localSys.sourceSystem = sys;
            localSys.collectionIndex = i;
            
            //
            // Insert the coefficient matrices
            //
            
            x_prime_x.resize( x_prime_x.n_rows + localXPrimeX.n_rows,
                              x_prime_x.n_cols + localXPrimeX.n_cols );
            
            x_prime_y.resize( x_prime_y.n_rows + localXPrimeY.n_rows );
            
            localSys.first_row = rowOrigin;
            localSys.first_col = colOrigin;
            localSys.last_row  = rowOrigin + localXPrimeX.n_rows - 1;
            localSys.last_col  = colOrigin + localXPrimeX.n_cols - 1;
             
            x_prime_x.submat( localSys.first_row, localSys.first_col, 
                              localSys.last_row, localSys.last_col ) = localXPrimeX;
            x_prime_y.rows( localSys.first_row, localSys.last_row ) = localXPrimeY.col( i );
            
            mLocalSystems.push_back(localSys);
            
            rowOrigin += localXPrimeX.n_rows;
            colOrigin += localXPrimeX.n_cols;
        }
    }
}

void FieldFit::Fitter::AddConstraints( Console &console, const Constraints &constraints )
{
    for ( const PrototypeConstraint &proto : constraints.GetConstraints() )
    {
        const ConstrType constrType = proto.GetType();
        
        if ( constrType == ConstrType::SymConstr )
        {
            HandleSymConstraint( console, proto );
        }
        else if ( constrType == ConstrType::SumConstr )
        {
            HandleSumConstraint( console, proto );
        }
    }
}

F64 FieldFit::Fitter::ConstraintCoefficient( const Site *site, FitType fitType, U32 collectionIndex )
{
    F64 lcoef = 1.0;
                            
    // Normally the coefficients are just 1.0
    // However in the case of alpha constraining 
    // we have to check if the electric field was turned on
    if ( site->TestSpecialType( SpecialFlag::alpha ) ) 
    {
        // test if the electric field is present
        if ( site->GetEfieldX().n_elem == 0 ||
             site->GetEfieldY().n_elem == 0 || 
             site->GetEfieldZ().n_elem == 0 )
        {
            throw ArgException( "FieldFit", "Fitter::ConstraintCoefficient", "Atom "+site->GetName()+" does not contain electric field elements" );
        }
                                
        if ( fitType == FitType::dipoleX )
        {
            lcoef = 1.0 / site->GetEfieldX()[ collectionIndex ];
        }
        else if ( fitType == FitType::dipoleY )
        {
            lcoef = 1.0 / site->GetEfieldY()[ collectionIndex ];
        }
        else if ( fitType == FitType::dipoleZ )
        {
            lcoef = 1.0 / site->GetEfieldZ()[ collectionIndex ];
        }
    }
    
    return lcoef;
}

void FieldFit::Fitter::PerSiteConstraintList( Console &console, const LocalSystem &localSys, const PrototypeConstraint &proto, 
                                              std::vector< InternalConstraint > &perSiteList )
{  
    const std::unordered_set< std::string > &coulTypes = proto.GetCoulTypes();
    
    // fetch the source system
    const size_t startCol = localSys.first_col;
    const System *system = localSys.sourceSystem;
        
    size_t collOffset = 0;
    for ( const Site *site : system->GetSites() )
    {
        // Test if we want to include this site
        if ( coulTypes.find(site->GetCoulType()) != coulTypes.end() )
        {
            InternalConstraint newConstraint;
            newConstraint.reference = proto.GetTarget();
            newConstraint.fconst = proto.GetForceConstant();
            
            // test if all the flags are present
            if ( IsMultiSet( site->GetFlags(), proto.GetFlags() ) )
            {
                for ( S32 t=0; t < FitType::size; ++t )
                {
                    FitType fitType = (FitType) t;
                    
                    // if present and activated in the constraint
                    if ( site->TestFitType( fitType )  )
                    {
                        if( IsSet( proto.GetFlags(), fitType ) )
                        {
                            // Test if we want to include this site
                            if ( coulTypes.find(site->GetCoulType()) != coulTypes.end() )
                            {
                                newConstraint.columns.push_back( startCol + collOffset );
                             
                                F64 coef = ConstraintCoefficient( site, fitType, localSys.collectionIndex );
                                newConstraint.coefficients.push_back( coef );
                            }    
                        }
                            
                        collOffset++;
                    }
                }
            }
            else
            {
                throw ArgException( "FieldFit", "Fitter::HandleSumConstraint", "For atom "+site->GetName()+" not all contraints can be processed due to missing fit types" );
            }
            
            if ( newConstraint.columns.size() > 0 )
            {
                 perSiteList.push_back( newConstraint );
            }
        }
        else
        {
            collOffset += site->NumColumns();
        }
    }
}

void FieldFit::Fitter::HandleSumConstraint( Console &console, const PrototypeConstraint &proto )
{    
    std::vector< InternalConstraint > perSiteList;
    
    // Go over all of the local systems
    for ( const LocalSystem &localSys : mLocalSystems )
    { 
        perSiteList.clear();
        PerSiteConstraintList( console, localSys, proto,  perSiteList );
        
        // Merge the per site lists;
        InternalConstraint newConstraint;
        newConstraint.reference = proto.GetTarget();
        newConstraint.fconst = proto.GetForceConstant();
        
        for ( const InternalConstraint &ic : perSiteList )
        {
            newConstraint.columns.insert(newConstraint.columns.end(), ic.columns.begin(), ic.columns.end());
            newConstraint.coefficients.insert(newConstraint.coefficients.end(), ic.coefficients.begin(), ic.coefficients.end());
        }
        
        mInternalConstraints.push_back( newConstraint );
    }
}

void FieldFit::Fitter::HandleSymConstraint( Console &console, const PrototypeConstraint &proto )
{
    std::vector< InternalConstraint > perSiteList;
    
    // Go over all of the local systems
    // Extend the per site list over all
    for ( const LocalSystem &localSys : mLocalSystems )
    { 
        PerSiteConstraintList( console, localSys, proto,  perSiteList );  
    }
    
    //for ( size_t i=0,iend=perSiteList.size(); i < iend; ++i )
    for ( size_t i=0,iend=perSiteList.size()-1; i < iend; ++i )
    {
        size_t j = i + 1;
        
        //for ( size_t j=i+1,jend=perSiteList.size(); j < jend; ++j )
        {
            // copy 
            InternalConstraint ic_i = perSiteList[i];
            
            // reference
            const InternalConstraint &ic_j = perSiteList[j];
            
            ic_i.columns.insert(ic_i.columns.end(), ic_j.columns.begin(), ic_j.columns.end());
            
            // Now for the coeficients we need to invert them first
            for ( F64 coef : ic_j.coefficients )
            {
                ic_i.coefficients.push_back( -coef );
            }
            
            mInternalConstraints.push_back( ic_i );
        }
    }
}