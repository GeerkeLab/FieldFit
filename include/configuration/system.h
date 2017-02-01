#pragma once
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "common/types.h"

#include <string>
#include <vector>
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
    
    private:
        
        F64 mCoordX;
        F64 mCoordY;
        F64 mCoordZ;
        
        std::string mName;
        std::string mCoulType;
        
        bool mTypes[FitType::size];
        F64  mValues[FitType::size];
    };
    
    class System
    {
    public:
        
        System( const std::string &name );
        ~System();
        
        void OnInit();
        void OnUpdate();
        void OnRelease();
        
        void InsertSite( Site *site );
    
        const std::string &GetName() const; 
    
    private:
    
        std::string mName;
        std::vector< Site* > mSites; 
        std::unordered_map< std::string, Site* > mNameToSite;
        std::unordered_map< std::string, Site* > mCoulTypeToSite;
    };
}

#endif