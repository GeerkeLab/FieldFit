#pragma once
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "common/types.h"

#include "configuration/fitType.h"

#include <string>
#include <vector>
#include <armadillo>
#include <unordered_map>

namespace FieldFit
{
    class Site
    {
    public:
    
        Site( const U32 types,
              const std::string &name,
              const std::string &coultype,
              const F64 coordX,
              const F64 coordY,
              const F64 coordZ );
        
        void AddEfield( const arma::vec &ex,
                        const arma::vec &ey,
                        const arma::vec &ez );
        
        void AddAlias( const std::string &alias );
    
        const std::string &GetName() const;
        const std::vector< std::string > &GetCoulTypes() const;
        
        F64 GetCoordX() const;
        F64 GetCoordY() const;
        F64 GetCoordZ() const;
        
        const arma::vec &GetEfieldX() const;
        const arma::vec &GetEfieldY() const;
        const arma::vec &GetEfieldZ() const;
        
        bool TestFitType( FitType type ) const;
        bool TestSpecialType( SpecialFlag type ) const;
        
        U32 GetFlags() const;
        
        size_t NumColumns() const;
        
    private:
        
        F64 mCoordX;
        F64 mCoordY;
        F64 mCoordZ;
        
        arma::vec mEfield_x;
        arma::vec mEfield_y;
        arma::vec mEfield_z;
        
        std::string mName;
        std::vector< std::string > mCoulTypes;
        
        U32 mTypes;
    };
    
    class PermSite
    {
    public:
        
        PermSite( F64 x, F64 y, F64 z, F64 val, FitType type );
    
        F64 GetCoordX() const;
        F64 GetCoordY() const;
        F64 GetCoordZ() const;
        F64 GetValue() const;
        FitType GetType() const;
        
    private:
        F64 mCoordX;
        F64 mCoordY;
        F64 mCoordZ;
        F64 mValue;
        FitType mType;
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
        
        size_t Size();
        
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
        
        //void OnUpdate();
        void OnUpdate2();
        
        Site * FindSite( const std::string &name );
        
        void InsertSite( Site *site );
        void InsertGrid( Grid *grid );
        void InsertField( Field *field );
        void InsertPermSite( PermSite *site );
        
        const std::string &GetName() const; 
        const std::vector< Site* > &GetSites() const;
        
        Grid *GetGrid() const;
        Field *GetField() const;
        
        const F64 ComputeChi2( arma::vec result, size_t collIndex ) const;
        const arma::mat &GetLocalXPrimeX() const;
        const arma::mat &PotentialMatrix() const;
        
        size_t NumColumns() const;
        
    private:
    
        size_t NumberOfColumns() const;
        arma::vec GeneratePermField() const;
        
        // Data
        Grid *mGrid;
        Field *mFields;
        std::string mName;
        std::vector< Site* > mSites; 
        std::vector< PermSite* > mPermSites; 
        std::unordered_map< std::string, Site* > mNameToSite;
        
        arma::vec mPermField;
        arma::mat mCoefficients;
        
        // OnUpdate generated
        arma::mat mX_prime_x;
        arma::mat mX_prime_y;
    };
}

#endif