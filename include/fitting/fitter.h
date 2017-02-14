#pragma once
#ifndef __FITTER_H__
#define __FITTER_H__

#include "common/types.h"

#include "configuration/fitType.h"

#include <string>
#include <vector>
#include <armadillo>

namespace FieldFit
{
    class Site;
    class System;
    class Console;
    class Constraints;
    class Configuration;
    class PrototypeConstraint;
    
    class Fitter
    {
    public:
    
        struct LocalSystem
        {
            LocalSystem();
            
            const System *sourceSystem;
            
            size_t first_row;
            size_t first_col;
            size_t last_row;
            size_t last_col;
            
            U32 collectionIndex;
        };
        
        struct InternalConstraint
        {
            InternalConstraint();
            
        	F64 reference;
            F64 fconst;
        	std::vector< U32 > columns;
        	std::vector< F64 > coefficients;
        };
    
        void Fit( Console &console, const Configuration &config, const Constraints &constr, bool verbose );
    
        void SelectCollection( U32 col );
    
    private:
        
        F64 ConstraintCoefficient( const Site *site, FitType fitType, U32 collectionIndex );
        void PerSiteConstraintList( Console &console, const LocalSystem &localSys, const PrototypeConstraint &proto, 
                                    std::vector< InternalConstraint > &perSiteList );
        void HandleSymConstraint( Console &console, const PrototypeConstraint &proto );
        void HandleSumConstraint( Console &console, const PrototypeConstraint &proto );
        
        void AddConfiguration( Console &console, const Configuration &config );
        void AddConstraints( Console &console, const Constraints &constr );
        
        void WriteSolution( Console &console );
        
        arma::mat x_prime_x;
        arma::mat x_prime_y;
        arma::vec mSolution;
        
        std::vector< U32 > mTargetCollections;
        std::vector< LocalSystem > mLocalSystems;
        std::vector< InternalConstraint > mInternalConstraints;
        std::vector< InternalConstraint > mInternalRestraints;
    };
};

#endif