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

// void FieldFit::Fitter::SelectCollection( U32 col )
// {
//     mTargetCollections.push_back( col );
// }

void FieldFit::Fitter::Fit( Console &console, const Configuration &config, const Constraints &constraints, bool debug, bool compat )
{
    //std::cout << "SETUP" << std::endl;

    AddConfiguration( console, config );
    AddConstraints( console, constraints, compat );
    
    //
    // Add restraints
    //
    
    for ( const InternalConstraint &constr : mInternalRestraints )
    {
        const F64 sqrt_fc = std::sqrt( constr.fconst );
        
        for ( size_t i = 0; i < constr.columns.size(); ++i )
        {
            const U32 col_i = constr.columns[i];
            
            for ( size_t j = 0; j < constr.columns.size(); ++j )
            {
                const U32 col_j = constr.columns[j];
                
                x_prime_x.row( col_i )[ col_j ] += constr.coefficients[i] * sqrt_fc *
                                                   constr.coefficients[j] * sqrt_fc;
                x_prime_y[ col_i ] += constr.reference / sqrt_fc;
            }
        }
    }
    
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
    x_prime_y.resize( x_prime_y.n_rows + mInternalConstraints.size(), 1 );
    
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

    //std::cout << "OLS" << std::endl;
    
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
    
    mSolution = FIELDFIT_SOLVE( x_prime_x, x_prime_y );
    
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
            fitResult.coulTypes = site->GetCoulTypes();

            F64 dipoleX = 0.0;
            F64 dipoleY = 0.0;
            F64 dipoleZ = 0.0;
            
            for ( S32 t=0; t < FitType::size; ++t )
            {
                FitType fitType = (FitType) t;
           
                if ( site->TestFitType( fitType ) )
                {
                    fitResult.values[fitType].push_back( lvec[col] );
                
                    // Store information for the alpha calculation
                    if ( fitType == FitType::dipoleX )
                    {
                        dipoleX = lvec[col];
                    }
                    else if ( fitType == FitType::dipoleY )
                    {
                        dipoleY = lvec[col];
                    }
                    else if ( fitType == FitType::dipoleZ )
                    {
                        dipoleZ = lvec[col];
                    }
                    
                    col++;
                }
            }
            
            if ( site->TestSpecialType( SpecialFlag::alpha ) )
            {
                const size_t efieldIndex = localSys.collectionIndex;
                const F64 efieldX = site->GetEfieldX()[ efieldIndex ];
                const F64 efieldY = site->GetEfieldY()[ efieldIndex ];
                const F64 efieldZ = site->GetEfieldZ()[ efieldIndex ];
                
                size_t alphaContrib = 0;
        	    F64 alphaSum = 0.0;
                
                if ( dipoleX != 0.0 && efieldX != 0.0 )
                {
                    F64 alpha_xx = dipoleX / efieldX;
                    fitResult.efX.push_back(efieldX);
                    fitResult.alphaX.push_back(alpha_xx);

                    alphaContrib++;
                    alphaSum += alpha_xx;
                }
                
                if ( dipoleY != 0.0 && efieldY != 0.0 )
                {
                    F64 alpha_yy = dipoleY / efieldY;
                    fitResult.efY.push_back(efieldY);
                    fitResult.alphaY.push_back(alpha_yy);
                    
                    alphaContrib++;
                    alphaSum += alpha_yy;
                }
                
                if ( dipoleZ != 0.0 && efieldZ != 0.0 )
                {
                    F64 alpha_zz = dipoleZ / efieldZ;
                    fitResult.efZ.push_back(efieldZ);
                    fitResult.alphaZ.push_back(alpha_zz);

                    alphaContrib++;
                    alphaSum += alpha_zz;
                }
                
                const F64 alpha = alphaSum / F64(alphaContrib);
                fitResult.alpha.push_back(alpha);
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
        const Field *field = sys->GetField();

        if (!field)
        {
            throw ArgException( "FieldFit", "Fitter::AddConfiguration", "Fitting system "+sys->GetName()+" requires a field" );
        }

        const arma::mat &localXPrimeX = sys->GetLocalXPrimeX();
        const arma::mat &localXPrimeY  = sys->PotentialMatrix();
          
        // // if no selections were made 
        // if ( mTargetCollections.size() == 0 )
        // {
        //     mTargetCollections.resize( localXPrimeY.n_cols );
        //     std::iota(mTargetCollections.begin(), mTargetCollections.end(), 0);
        // }

        for ( U32 i=0; i < field->NumColumns(); ++i )
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
            
            x_prime_y.resize( x_prime_y.n_rows + localXPrimeY.n_rows, 1 );
            
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

void FieldFit::Fitter::AddConstraints( Console &console, const Constraints &constraints, bool compat )
{
    for ( const PrototypeConstraint &proto : constraints.GetConstraints() )
    {
        const ConstrType constrType = proto.GetType();
        
        if ( constrType == ConstrType::SymConstr )
        {
            HandleSymConstraint( console, proto, compat );
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
        bool include = false;
        
        for ( const std::string &ctype : site->GetCoulTypes() )
        {
            if ( coulTypes.find(ctype) != coulTypes.end() )
            {
                include=true;
                break;
            }
        }

        // Test if we want to include this site
        if ( include )
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
                            newConstraint.columns.push_back( startCol + collOffset );
                             
                            F64 coef = ConstraintCoefficient( site, fitType, localSys.collectionIndex );
                            newConstraint.coefficients.push_back( coef );  
                        }
                            
                        collOffset++;
                    }
                }
            }
            else
            {
                throw ArgException( "FieldFit", "Fitter::HandleSumConstraint", "For atom "+site->GetName()+" in system "+system->GetName()+" not all contraints can be processed due to missing fit types" );
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
        
        if ( perSiteList.size() == 0 )
        {
            console.Warn( Message( "FieldFit", "Fitter::HandleSumConstraint", "Couldnt fulfill a constraint" ));
            continue;
        }
        
        // Merge the per site lists;
        InternalConstraint newConstraint;
        newConstraint.reference = proto.GetTarget();
        newConstraint.fconst = proto.GetForceConstant();
        
        for ( const InternalConstraint &ic : perSiteList )
        {
            newConstraint.columns.insert(newConstraint.columns.end(), ic.columns.begin(), ic.columns.end());
            newConstraint.coefficients.insert(newConstraint.coefficients.end(), ic.coefficients.begin(), ic.coefficients.end());
        }
        
        if ( newConstraint.fconst != 0.0 )
        {
            mInternalRestraints.push_back( newConstraint );
        }
        else
        {
            mInternalConstraints.push_back( newConstraint );
        }
    }
}

void FieldFit::Fitter::HandleSymConstraint( Console &console, const PrototypeConstraint &proto, bool compat )
{
    std::vector< InternalConstraint > perSiteList;
    
    // Go over all of the local systems
    // Extend the per site list over all
    for ( const LocalSystem &localSys : mLocalSystems )
    { 
        PerSiteConstraintList( console, localSys, proto,  perSiteList );  
    }
    
    if ( perSiteList.size() == 0 )
    {
        console.Warn( Message( "FieldFit", "Fitter::HandleSymConstraint", "Couldnt fulfill a constraint" ));
        return;
    }
    
    //std::cout << "perSiteList: " << perSiteList.size() << std::endl;

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
            
            //std::cout << "ic_i.columns: " << ic_i.columns.size() << std::endl;

            // Now for the coeficients we need to invert them first
            for ( F64 coef : ic_j.coefficients )
            {
                ic_i.coefficients.push_back( -coef );
            }
            
            if ( ic_i.fconst != 0.0 )
            {
                mInternalRestraints.push_back( ic_i );
            }
            else
            {
                mInternalConstraints.push_back( ic_i );
            }
        }
    }

    // also sym the inner components
    // can be made a flag later on!
    if (!compat)
    {
        for ( size_t i=0,iend=perSiteList.size(); i < iend; ++i )
        {
            const InternalConstraint &ic_i = perSiteList[i];

            // if more columns than 1 we want to sym all inner columns
            // if the flag is on
            if (ic_i.columns.size() > 1)
            {
                for (size_t ij=0,ijend=ic_i.columns.size()-1; ij < ijend; ++ij )
                {
                    size_t jj = ij + 1;

                    InternalConstraint new_c;

                    new_c.columns.push_back(ic_i.columns[ij]);
                    new_c.columns.push_back(ic_i.columns[jj]);

                    new_c.coefficients.push_back( ic_i.coefficients[ij]);
                    new_c.coefficients.push_back(-ic_i.coefficients[jj]);

                    new_c.fconst = ic_i.fconst;
                    new_c.reference = 0.0;

                    if ( new_c.fconst != 0.0 )
                    {
                        mInternalRestraints.push_back( new_c );
                    }
                    else
                    {
                        mInternalConstraints.push_back( new_c );
                    }
                }
            }
        }
    }
}