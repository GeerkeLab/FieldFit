#pragma once
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "common/types.h"

#include <vector>

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
        	
    
    protected:
    
    
    private:
    
        bool mTypes[FitType::size];
        F64  mValues[FitType::size];
    };
    
    class System
    {
    public:
    
        void OnInit();
        void OnUpdate();
        void OnRelease();
    
    protected:
    
    
    
    private:
    
      
        std::vector< Site > mSites; 
    };
}

#endif