#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "../common/types.h"
#include "../error/error.h"
#include "../math/vec3.h"

class BlockParser;

class Configuration
{
public:

    enum valueType
    {
        charge  = 0,
        dipoleX = 1,
        dipoleY = 2,
        dipoleZ = 3,
        qd20    = 4,
        qd21c   = 5,
        qd21s   = 6,
        qd22c   = 7,
        qd22s   = 8
    };
    
    struct ValueConstraint
    {
    	ValueConstraint( const valueType t, const U32 id, const F32 v ) :
    	type( t ), ID( id ), value( v )
    	{}
    	
    	const valueType type;
    	const U32 		ID;
    	const F32 	    value;
    };
    
    struct SymConstraint
    {
    	SymConstraint( const valueType t1, const valueType t2, const U32 id1, const U32 id2 ) :
    	type1( t1 ), type2( t2 ), ID1( id1 ), ID2( id2 )
    	{}
    	
    	const valueType type1;   
    	const valueType type2;
    	const U32 ID1;
    	const U32 ID2;
    };
    
    struct SumConstraint
    {
    	SumConstraint( const std::vector< std::pair< U32, valueType > >  &locs, const F32 v ) :
    	locations( locs ), value( v )
    	{}
    	
    	const std::vector< std::pair< U32, valueType > > locations;
    	const F32 	    value;
    };
    
    struct RespRestraint
    {
    	RespRestraint(  const valueType t, const U32 id, const F32 v, const F32 i ) :
    	type( t ), ID( id ), refValue( v ), intensity( i )
    	{}
    	
    	const valueType type;
    	const U32 		ID;
    	const F32 	    refValue;
    	const F32  		intensity;
    };
    
    struct UnionContraint
    {
        UnionContraint( U32 coll_i, U32 id_i, U32 coll_s_j, U32 coll_e_j, U32 id_j, const std::vector< valueType > &types ) :
        collection_i( coll_i ), ID_i( id_i ), collectionStart_j( coll_s_j ), collectionEnd_j( coll_e_j ), ID_j( id_j ), types( types )
        {}
        const U32 collection_i;
        const U32 ID_i;
        const U32 collectionStart_j;
        const U32 collectionEnd_j;
        const U32 ID_j;
        const std::vector< valueType > types;
    };
    
    struct FitSite
    {	
        FitSite( const U32 id, const std::string &n, const F32 charge, const F32 x, const F32 y, const F32 z ) :
            ID( id ), name( n ), position( x, y, z ), atomicCharge( charge ), fitFlags( 0 ), permFlags( 0 )
        {
            for ( U32 i=0; i < 9; ++i )
            {
                values[i] = 0.0;
                permValues[i] = 0.0;
            }
        }

        F32  GetValue( const valueType type ) const;
        void SetValue( const valueType type, const F32 val );

        F32  GetPermValue( const valueType type ) const;
        void SetPermValue( const valueType type, const F32 val );
        
        void Raport( std::ostream &stream );
        
        const U32 ID;
        const std::string name;
        const Vec3 position;
        const F32 atomicCharge;
        
        U32 fitFlags;
        U32 permFlags;
        
        F32 values[9];     
        F32 permValues[9];
    };
    
    struct ConfigCollection
    {
        std::map< U32, U32 > indices;
        std::vector< FitSite > fitSites;
        
        std::vector< ValueConstraint > valueConstraints;
        std::vector< SymConstraint > symConstraints;
        std::vector< SumConstraint > sumConstraints;
        std::vector< RespRestraint > respConstraints;
    };
    

    Configuration( const BlockParser & );

    Error::STATUS GetStatus() const;

    size_t NumCollections() const;
    
    U32 FitSites( const U32 coll ) const;
    
    U32 ValueConstr( const U32 coll ) const;
    U32 SymConstr( const U32 coll ) const;
    U32 SumConstr( const U32 coll ) const;
    U32 RespRestr( const U32 coll ) const;
    U32 UnionContr() const;
    
    U32 GetIndexFromID( const U32 coll, const U32 id ) const;
    bool IdIsPresent( const U32 coll, const U32 id );
    U32 UnknownID() const;
    
    const FitSite *GetSite( const U32 coll, const U32 index ) const;
    FitSite *GetSiteMod( const U32 coll, const U32 index );

    const ValueConstraint * GetValueConstraint( const U32 coll, const U32 index ) const;
    const SymConstraint   * GetSymConstraint(   const U32 coll, const U32 index ) const;
    const SumConstraint   * GetSumConstraint(   const U32 coll, const U32 index ) const;
    const RespRestraint   * GetRespRestraint(   const U32 coll, const U32 index ) const;
    const UnionContraint  * GetUnionConstraint(   const U32 index ) const;
    
    void Raport( std::ostream &stream );

private:
	
    Error::STATUS FlagSanityCheck( U32 convFlags ) const;
	Error::STATUS ReadSites( const BlockParser & );
	Error::STATUS ReadFitSites( const BlockParser & );
	Error::STATUS ReadPermSites( const BlockParser & );
	Error::STATUS ReadValueConstraints( const BlockParser & );
	Error::STATUS ReadSymConstraints( const BlockParser & );
	Error::STATUS ReadSumConstraints( const BlockParser & );
	Error::STATUS ReadRespRestraints( const BlockParser & );
	Error::STATUS ReadUnionFit( const BlockParser & );
    
    Error::STATUS mStatus;

    std::vector< ConfigCollection > mConfigCollections;
    std::vector< UnionContraint > mUnionConstraint;
};

#endif