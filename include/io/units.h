#pragma once
#ifndef __UNITS_H__
#define __UNITS_H__

#include "common/types.h"

namespace FieldFit
{
    namespace Constants
    {
        static const F64 four_pi_eps_rcp = 138.9354;    // kJ * nm * mol * e^-2
        
        static const F64 eBohr_D = 2.541748;            // electron bohr to Debye
        static const F64 D_eBohr = 1.0 / eBohr_D;
        
        static const F64 eBohr2_DA = 1.3450351;         // electron bohr^2 to DebyeAngstrom ( Buckingham )
        static const F64 DA_eBohr2 = 1.0 / eBohr2_DA;
        
        static const F64 A_bohr  = 1.8897259886;
        static const F64 bohr_A  = 1 / A_bohr;
        
        static const F64 nm_bohr = 18.897259886;        // nano meter to bohr
        static const F64 bohr_nm = 1.0 / nm_bohr;
        
        static const F64 A2_bohr2 = A_bohr * A_bohr;        // nano meter^2 to bohr^2
        static const F64 bohr2_A2 = 1.0 / A2_bohr2;
        
        static const F64 nm2_bohr2 = nm_bohr * nm_bohr;        // nano meter^2 to bohr^2
        static const F64 bohr2_nm2 = 1.0 / nm2_bohr2;
        
        static const F64 A3_bohr3 = A2_bohr2 * A_bohr;        // nano meter^2 to bohr^2
        static const F64 bohr3_A3 = 1.0 / A3_bohr3;
        
        static const F64 nm3_bohr3 = nm2_bohr2 * nm_bohr;        // nano meter^2 to bohr^2
        static const F64 bohr3_nm3 = 1.0 / nm3_bohr3;
        
        static const F64 D_enm = D_eBohr * bohr_nm;     // Debye to electron nano meter
        static const F64 enm_D = 1.0 / D_enm;
        
    }
    
    class Units
    {
    public:
        
        Units();
        
        F64 GetCoordConv() const;
        F64 GetChargeConv() const;
        F64 GetDipoleConv() const;
        F64 GetQpolConv() const;
        F64 GetPotConv() const;
        F64 GetEfieldConv() const;
        F64 GetAlphaConv() const;
        
        F64 &GetCoordConv();
        F64 &GetChargeConv();
        F64 &GetDipoleConv();
        F64 &GetQpolConv();
        F64 &GetPotConv();
        F64 &GetEfieldConv();
        F64 &GetAlphaConv();
        
    private:
    
        F64 mCoordConv;
        F64 mChargeConv;
        F64 mDipoleConv;
        F64 mQpolConv; 
        F64 mPotConv;  
        F64 mEfieldConv; 
        F64 mAlphaConv;
    };
}

#endif