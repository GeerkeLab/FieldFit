#pragma once
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "common/types.h"

#include <string>
#include <vector>
#include <armadillo>
#include <unordered_map>

namespace FieldFit
{
    enum FitType
    {
        charge  = 0,
        dipoleX = 1,
        dipoleY = 2,
        dipoleZ = 3,
        qd20    = 4,
        qd21c   = 5,
        qd21s   = 6,
        qd22c   = 7,
        qd22s   = 8,
        size    = 9
    };
    
    class Site
    {
    public:
    
        Site( const std::string &name,
              const std::string &coultype,
              const F64 coordX,
              const F64 coordY,
              const F64 coordZ );
        
        void AddFitType( FitType type );	
        void AddEfield( const arma::vec &ex,
                        const arma::vec &ey,
                        const arma::vec &ez );
    
        const std::string &GetName() const;
        const std::string &GetCoulType() const;
        
        F64 GetCoordX() const;
        F64 GetCoordY() const;
        F64 GetCoordZ() const;
        
        bool TestFitType( FitType type ) const;
        
    private:
        
        F64 mCoordX;
        F64 mCoordY;
        F64 mCoordZ;
        
        arma::vec mEfield_x;
        arma::vec mEfield_y;
        arma::vec mEfield_z;
        
        std::string mName;
        std::string mCoulType;
        
        bool mTypes[FitType::size];
    };
    
    class Grid
    {
    public:
    
        Grid( const arma::vec &x,
              const arma::vec &y,
              const arma::vec &z );
        
        const arma::vec &GetX() const;
        const arma::vec &GetY() const;
        const arma::vec &GetZ() const;
        
    private:
    
        arma::vec mGridX;
        arma::vec mGridY;
        arma::vec mGridZ;
    };
    
    class Field
    {
    public:
    
        Field( const arma::mat &mat );
        
        const arma::mat &GetPotentials() const;
    
    private:
    
        arma::mat mPotentials;
    };
    
    class System
    {
    public:
        
        System( const std::string &name );
        ~System();
        
        void OnUpdate();
        
        Site * FindSite( const std::string &name );
        
        void InsertSite( Site *site );
        void InsertGrid( Grid *grid );
        void InsertField( Field *field );
        
        const std::string &GetName() const; 
    
        Grid *GetGrid() const;
        Field *GetField() const;
        
    private:
    
        size_t NumberOfColumns() const;
    
        // Data
        Grid *mGrid;
        Field *mFields;
        std::string mName;
        std::vector< Site* > mSites; 
        std::unordered_map< std::string, Site* > mNameToSite;
        
        // OnUpdate generated
        arma::mat mCoefficientMatrix;
        arma::mat mPotentialMatrix;
    };
}

#endif