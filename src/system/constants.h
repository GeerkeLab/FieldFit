//
//  constants.h
//  fieldfit
//
//  Created by koen visscher on 5/11/16.
//
//
#pragma once
#ifndef constants_h
#define constants_h

#include "../common/types.h"

namespace Constants
{
    static const F64 four_pi_eps_rcp = 138.9354;    // kJ * nm * mol * e^-2
    
    static const F64 eBohr_D = 2.541748;            // electron bohr to Debye
    static const F64 D_eBohr = 1.0 / eBohr_D;
    
    static const F64 eBohr2_DA = 1.3450351;         // electron bohr^2 to DebyeAngstrom ( Buckingham )
    static const F64 DA_eBohr2 = 1.0 / eBohr2_DA;
    
    static const F64 nm_bohr = 18.897259886;        // nano meter to bohr
    static const F64 bohr_nm = 1.0 / nm_bohr;
    
    static const F64 nm2_bohr2 = nm_bohr * nm_bohr;        // nano meter^2 to bohr^2
    static const F64 bohr2_nm2 = 1.0 / nm2_bohr2;
    
    static const F64 nm3_bohr3 = nm2_bohr2 * nm_bohr;        // nano meter^2 to bohr^2
    static const F64 bohr3_nm3 = 1.0 / nm3_bohr3;
    
    static const F64 D_enm = D_eBohr * bohr_nm;     // Debye to electron nano meter
    static const F64 enm_D = 1.0 / D_enm;
    
}

#endif /* constants_h */
