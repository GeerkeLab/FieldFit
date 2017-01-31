#include "configuration.h"

#include <iostream>

#include "../io/block.h"
#include "../io/blockParser.h"

#include "../util/util.h"

Configuration::Configuration( const BlockParser &bp )
{                              
    mStatus = ReadSites( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadFitSites( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadPermSites( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadValueConstraints( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadSymConstraints( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadSumConstraints( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadRespRestraints( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadUnionFit( bp );
    if ( Error::FAILED( mStatus ) )
    {
        return;
    }
    
    mStatus = Error::STATUS::OK;
}

size_t Configuration::NumCollections() const
{
    return mConfigCollections.size();
}

Error::STATUS Configuration::GetStatus() const
{
    return mStatus;
}

U32 Configuration::FitSites( const U32 coll ) const
{
    return static_cast< U32 >( mConfigCollections[coll].fitSites.size() );
}

U32 Configuration::ValueConstr( const U32 coll ) const
{
	return static_cast< U32 >( mConfigCollections[coll].valueConstraints.size() );
}

U32 Configuration::SymConstr( const U32 coll ) const
{
	return static_cast< U32 >( mConfigCollections[coll].symConstraints.size() );
}

U32 Configuration::SumConstr( const U32 coll ) const
{
	return static_cast< U32 >( mConfigCollections[coll].sumConstraints.size() );
}

U32 Configuration::RespRestr( const U32 coll ) const
{
	return static_cast< U32 >( mConfigCollections[coll].respConstraints.size() );
}

U32 Configuration::UnionContr() const
{
    return static_cast< U32 >( mUnionConstraint.size() );
}

U32 Configuration::UnknownID() const
{	
	return 0xFFFFFF;
}

U32 Configuration::GetIndexFromID( const U32 coll, const U32 id ) const
{
	std::map< U32, U32 >::const_iterator it = mConfigCollections[coll].indices.find( id );
	
	if ( it == mConfigCollections[coll].indices.end() )
	{
		return 	UnknownID();
	}
	
	return it->second;
}

bool Configuration::IdIsPresent( const U32 coll, const U32 id )
{
	std::map< U32, U32 >::iterator it = mConfigCollections[coll].indices.find( id );
	
	if ( it == mConfigCollections[coll].indices.end() )
	{
		return false;
	}
	
	return true;
}

const Configuration::FitSite * Configuration::GetSite( const U32 coll, const U32 index ) const
{
    if ( index < FitSites(coll) )
    {
        return &mConfigCollections[coll].fitSites[ index ];
    }

    return NULL;
}

const Configuration::ValueConstraint * Configuration::GetValueConstraint( const U32 coll, const U32 index ) const
{
    if ( index < mConfigCollections[coll].valueConstraints.size() )
    {
        return &mConfigCollections[coll].valueConstraints[ index ];
    }

    return NULL;
}

const Configuration::SymConstraint * Configuration::GetSymConstraint( const U32 coll, const U32 index ) const
{
    if ( index < mConfigCollections[coll].symConstraints.size() )
    {
        return &mConfigCollections[coll].symConstraints[ index ];
    }

    return NULL;
}

const Configuration::SumConstraint * Configuration::GetSumConstraint( const U32 coll, const U32 index ) const
{
    if ( index < mConfigCollections[coll].sumConstraints.size() )
    {
        return &mConfigCollections[coll].sumConstraints[ index ];
    }

    return NULL;
}

const Configuration::RespRestraint * Configuration::GetRespRestraint( const U32 coll, const U32 index ) const
{
    if ( index < mConfigCollections[coll].respConstraints.size() )
    {
        return &mConfigCollections[coll].respConstraints[ index ];
    }

    return NULL;
}

const Configuration::UnionContraint  * Configuration::GetUnionConstraint(   const U32 index ) const
{
    if ( index < mUnionConstraint.size() )
    {
        return &mUnionConstraint[ index ];
    }
    
    return NULL;
}

Configuration::FitSite * Configuration::GetSiteMod( const U32 coll, const U32 index )
{
    return &mConfigCollections[coll].fitSites[ index ];
}


F32 Configuration::FitSite::GetValue( const valueType type ) const
{
    return values[ static_cast<U32>( type ) ];
}

void Configuration::FitSite::SetValue( const valueType type, const F32 val )
{
    values[ static_cast<U32>( type ) ] = val;
}

F32 Configuration::FitSite::GetPermValue( const valueType type ) const
{
    return permValues[ static_cast<U32>( type ) ];
}

void Configuration::FitSite::SetPermValue( const valueType type, const F32 val )
{
    permValues[ static_cast<U32>( type ) ] = val;
}

void Configuration::FitSite::Raport( std::ostream &stream )
{
	std::string flagString;
    for ( U32 j=0; j < 9; j++ )
    {
         if ( fitFlags & ( 1 << j ) )
             flagString += "1";
         else
            flagString += "0";
    }
    
    std::string permString;
    for ( U32 j=0; j < 9; j++ )
    {
         if ( permFlags & ( 1 << j ) )
             permString += "1";
         else
            permString += "0";
    }
	
    stream << "\t " << ID << " \t " << name << " \t " << atomicCharge << " \t " << position.GetX() 
           << "\t " << position.GetY() << " \t " << position.GetZ() << std::endl;  
    
    stream << " \t " << flagString << " \t ";
    for ( U32 i=0; i < 9; ++i )
    	stream << values[i] << " \t ";
    stream << std::endl;   
    
    stream << " \t " << permString << " \t ";
    for ( U32 i=0; i < 9; ++i )
    	stream << permValues[i] << " \t ";
    stream << std::endl;
    
    stream << std::endl;
}

void Configuration::Raport( std::ostream &stream )
{
    for ( ConfigCollection collection : mConfigCollections)
    {
        stream << "[RUNINPUT]" << std::endl;
        stream << "#\t ID \t Atomic Charge \t Name \t coordX \t coordY \t coordZ" << std::endl;
        stream << "#\t 1) Fit values; " << std::endl;
        stream << "#\t 2) Permanent values; " << std::endl;
        stream << "#\t 3) Value constraints; " << std::endl;
        stream << "#\t Flags,  Charge, DipoleX,  DipoleY,  DipoleZ,   Qd20,  Qd21c,  Qd21s,  Qd22c,  Qd22s " << std::endl;
        
        for ( FitSite site : collection.fitSites)
        {
            site.Raport( stream );
        }
        
        stream << "[END]" << std::endl;
    }
}

Error::STATUS Configuration::ReadSites( const BlockParser &bp )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("[SITES]");
    
    if ( !blockArray )
    {
        Error::Warn( std::cout, "block [SITES] was not present!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    mConfigCollections.resize(blockArray->size());
    
    size_t coll = 0;
    for ( const Block &block : *blockArray )
    {
        ConfigCollection &confColl = mConfigCollections[coll];
        
        if ( block.Size() < 1 )
        {
            Error::Warn( std::cout, "block [SITES] was too small ( at least 1 argument expected ) !" );
            return Error::STATUS::FAILED_IO;
        }

        U32 sites = block.GetToken( 0 )->GetValue< U32 >();

        if ( sites < 1 )
        {
            Error::Warn( std::cout, "At least 1 site was expected!" );
            return Error::STATUS::FAILED_IO;
        }
        
        if ( block.Size() != ( 1 + ( sites * 6 ) ) )
        {
            Error::Warn( std::cout, "block [SITES] did not have the right amount of arguments based on the size indicator !" );
            return Error::STATUS::FAILED_IO;
        }

        U32 index = 1;

        for ( U32 i=0; i < sites; ++i )
        {
            U32 id = block.GetToken( index )->GetValue< U32 >();
    	
            std::string name = block.GetToken( index+1 )->GetToken();
        
            F32 acharge = block.GetToken( index+2 )->GetValue< F32 >();

            F32 x = block.GetToken( index+3 )->GetValue< F32 >();
            F32 y = block.GetToken( index+4 )->GetValue< F32 >();
            F32 z = block.GetToken( index+5 )->GetValue< F32 >();
        
            if ( confColl.indices.find( id ) != confColl.indices.end() )
            {
                Error::Warn( std::cout, "[SITES] contains a duplicate site ID!" );
                return 	Error::STATUS::FAILED_IO;
            }
        
            confColl.fitSites.push_back( FitSite( id, name, acharge, x, y, z ) );
            confColl.indices.insert( std::pair< U32, U32 >( id, confColl.fitSites.size() - 1 ) );
        	
            index += 6;
        }
        
        coll++;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadFitSites( const BlockParser &bp )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("[FITSITES]");
    
    if ( !blockArray )
    {
        Error::Warn( std::cout, "block [FITSITES] was not present!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    if ( blockArray->size() != mConfigCollections.size() )
    {
        Error::Warn( std::cout, "number of [FITSITES] blocks does not match number of [SITES] blocks!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    size_t coll = 0;
    for ( const Block &block : *blockArray )
    {
        ConfigCollection &confColl = mConfigCollections[coll];
        
        
        if ( block.Size() < 1 )
        {
            Error::Warn( std::cout, "block [FITSITES] was too small ( at least 1 argument expected ) !" );
            return Error::STATUS::FAILED_IO;
        }

        U32 fitSites = block.GetToken( 0 )->GetValue< U32 >();

        if ( fitSites < 1 )
        {
            Error::Warn( std::cout, "At least 1 fit site was expected!" );
            return Error::STATUS::FAILED_IO;
        }

        if ( block.Size() != ( 1 + ( fitSites * 2 ) ) )
        {
            Error::Warn( std::cout, "block [FITSITES] did not have the right amount of arguments based on the size indicator !" );
            return Error::STATUS::FAILED_IO;
        }

        U32 index = 1;
    
        for ( U32 i=0; i < fitSites; ++i )
        {
    	
            U32 id = block.GetToken( index )->GetValue< U32 >();
            std::string fitflags = block.GetToken( index+1 )->GetToken();

            U32 convFlags = Util::ToFlags( fitflags );
        
            Error::STATUS checkStatus;
            if ( Error::FAILED( checkStatus = FlagSanityCheck( convFlags) ) )
            {
                return checkStatus;
            }
            
            //find the id
            std::map< U32, U32 >::iterator it = confColl.indices.find( id );
        
            if ( it == confColl.indices.end() )
            {
                Error::Warn( std::cout, "block [fitsites] contains an index ( "+Util::ToString( id )+
                                        " ) that was not present in the sites definition!" );
                return Error::STATUS::FAILED_IO;
            }
        
            confColl.fitSites[ it->second ].fitFlags = convFlags;
        
            index += 2;
        }
        
        coll++;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadPermSites( const BlockParser &bp )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("[PERMSITES]");
    
    if ( !blockArray )
    {
        Error::Warn( std::cout, "block [PERMSITES] was not present!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    if ( blockArray->size() != mConfigCollections.size() )
    {
        Error::Warn( std::cout, "number of [PERMSITES] blocks does not match number of [SITES] blocks!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    size_t coll = 0;
    for ( const Block &block : *blockArray )
    {
        ConfigCollection &confColl = mConfigCollections[coll];

        if ( block.Size() < 1 )
        {
            Error::Warn( std::cout, "block [PERMSITES] was too small ( at least 1 argument expected ) !" );
            return Error::STATUS::FAILED_IO;
        }
        
        U32 permSites = block.GetToken( 0 )->GetValue< U32 >();

        U32 index = 1;
    
        for ( U32 i=0; i < permSites; ++i )
        {
            //test if there is enough space for at least the id and flags
            if ( block.Size() < ( index + 2 ) )
            {
                Error::Warn( std::cout, "block [PERMSITES] did not have the right amount of arguments based on the size indicator !"  );
                return Error::STATUS::FAILED_IO;
            }
    	
            U32 id = block.GetToken( index )->GetValue< U32 >();
            std::string fitflags = block.GetToken( index+1 )->GetToken();

            U32 convFlags = Util::ToFlags( fitflags );
        
            Error::STATUS checkStatus;
            if ( Error::FAILED( checkStatus = FlagSanityCheck( convFlags) ) )
            {
                return checkStatus;
            }
            
            //find the id
            std::map< U32, U32 >::iterator it = confColl.indices.find( id );
        
            if ( it == confColl.indices.end() )
            {
                Error::Warn( std::cout, "block [PERMSITES] contains an index ( "+Util::ToString( id )+
                                        " ) that was not present in the sites definition!" );
                return Error::STATUS::FAILED_IO;
            }
        
            confColl.fitSites[ it->second ].permFlags = convFlags;
        
            //shift by 2
            index += 2;
        
            for ( U32 j=0; j <  9; ++j )
            {
                //test if this type if active
                if ( convFlags & ( 1 << j ) )
                {
                    if ( block.Size() <= ( index ) )
                    {
                        Error::Warn( std::cout, "block [PERMSITES] contains an index ( "+Util::ToString( id )+
                                                " ) that does not contain enough perm values!" );
                        return Error::STATUS::FAILED_IO;
                    }
        		
                    F32 val = block.GetToken( index )->GetValue< F32 >();
				
                    confColl.fitSites[ it->second ].SetPermValue( static_cast< valueType >( j ), val );
				
                    index += 1;
                }
            }
        }
    
        if ( index != block.Size() )
        {
            Error::Warn( std::cout, "block [PERMSITES] has more arguments than expected, based on the size indicator!"  );
            return Error::STATUS::FAILED_IO;
        }
        
        coll++;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadValueConstraints( const BlockParser &bp )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("[VALUECONSTR]");
    
    if ( !blockArray )
    {
        Error::Warn( std::cout, "block [VALUECONSTR] was not present!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    if ( blockArray->size() != mConfigCollections.size() )
    {
        Error::Warn( std::cout, "number of [VALUECONSTR] blocks does not match number of [SITES] blocks!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    size_t coll = 0;
    for ( const Block &block : *blockArray )
    {
        ConfigCollection &confColl = mConfigCollections[coll];
    
        if ( block.Size() < 1 )
        {
            Error::Warn( std::cout, "block [VALUECONSTR] was too small ( at least 1 argument expected ) !" );
            return Error::STATUS::FAILED_IO;
        }

        U32 constraints = block.GetToken( 0 )->GetValue< U32 >();

        U32 index = 1;
	
        for ( U32 i=0; i < constraints; ++i )
        {
            if ( block.Size() < ( index + 2 ) )
            {
                Error::Warn( std::cout, "block [VALUECONSTR] did not have the right amount of arguments based on the size indicator !"  );
                return Error::STATUS::FAILED_IO;
            }
    	
            U32 id = block.GetToken( index )->GetValue< U32 >();
    	
            std::string fitflags = block.GetToken( index+1 )->GetToken();
            U32 convFlags = Util::ToFlags( fitflags );
        
            Error::STATUS checkStatus;
            if ( Error::FAILED( checkStatus = FlagSanityCheck( convFlags) ) )
            {
                return checkStatus;
            }
            
            //shift by 2
            index += 2;
        
            for ( U32 j=0; j <  9; ++j )
            {
                //test if this type if active
                if ( convFlags & ( 1 << j ) )
                {
                    if ( block.Size() <= ( index ) )
                    {
                        Error::Warn( std::cout, "block [VALUECONSTR] contains an index ( "+Util::ToString( id )+
                                                " ) that does not contain enough constr values!" );
                        return Error::STATUS::FAILED_IO;
                    }
        		
                    F32 val = block.GetToken( index )->GetValue< F32 >();
                    confColl.valueConstraints.push_back( ValueConstraint( static_cast< valueType >( j ), id, val ) );

                    index += 1;
                }
            }
        }
    
        if ( index != block.Size() )
        {
            Error::Warn( std::cout, "block [VALUECONSTR] has more arguments than expected, based on the size indicator!"  );
            return Error::STATUS::FAILED_IO;
        }
        
        coll++;
    }
    
    return Error::STATUS::OK;
}


Error::STATUS Configuration::ReadSymConstraints( const BlockParser &bp )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("[SYMCONSTR]");
    
    if ( !blockArray )
    {
        Error::Warn( std::cout, "block [SYMCONSTR] was not present!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    if ( blockArray->size() != mConfigCollections.size() )
    {
        Error::Warn( std::cout, "number of [SYMCONSTR] blocks does not match number of [SITES] blocks!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    size_t coll = 0;
    for ( const Block &block : *blockArray )
    {
        ConfigCollection &confColl = mConfigCollections[coll];

        if ( block.Size() < 1 )
        {
            Error::Warn( std::cout, "block [SYMCONSTR] was too small ( at least 1 argument expected ) !" );
            return Error::STATUS::FAILED_IO;
        }

        U32 constraints = block.GetToken( 0 )->GetValue< U32 >();

        U32 index = 1;
	
        for ( U32 i=0; i < constraints; ++i )
        {
            //read the amount of indices
            if ( index >= block.Size() )
            {
                Error::Warn( std::cout, "block [SYMCONSTR] was too small 1!" );
                return Error::STATUS::FAILED_IO;
            }
    	
            U32 indices = block.GetToken( index )->GetValue< U32 >();
    	
            if ( indices < 2 )
            {
                Error::Warn( std::cout, "at least two indices expected in a sym constraint!" );
                return Error::STATUS::FAILED_IO;
            }
    	
            if ( ( index+2+indices) > block.Size() )
            {
                Error::Warn( std::cout, "block [SYMCONSTR] was too small 2!" );
                return Error::STATUS::FAILED_IO;
            }
    	
            //first find all indices and types involved
            std::vector< U32 > ind;
            for ( U32 j=0 ; j < indices; ++j )
            {
                ind.push_back( block.GetToken( index+1+j )->GetValue< U32 >() );
            }
    	
            std::string fitflags = block.GetToken( index+1+indices )->GetToken();
            U32 convFlags = Util::ToFlags( fitflags );
    	
            Error::STATUS checkStatus;
            if ( Error::FAILED( checkStatus = FlagSanityCheck( convFlags) ) )
            {
                return checkStatus;
            }
            
            //convert to an complete intermediate list
            std::vector< std::pair< U32, valueType > > constraints;
            for ( U32 j=0 ; j < indices; ++j )
            {
                for ( U32 n=0; n < 9; ++n )
                {
                    if ( convFlags & ( 1 << n ) )
                    {
                        constraints.push_back( std::make_pair( ind[j], static_cast< valueType >( n ) ) );
                    }
    				
                }
            }
        
            //add the constraints pairwise
            for ( U32 j=1 ; j < constraints.size(); ++j )
            {
                std::pair< U32, valueType > id1 = constraints[j-1];
                std::pair< U32, valueType > id2 = constraints[j];
    		
                confColl.symConstraints.push_back( SymConstraint( id1.second, id2.second, id1.first, id2.first ) );
            }
    	
            index += ( 2 + indices );
        }
        
        coll++;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadSumConstraints( const BlockParser &bp )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("[SUMCONSTR]");
    
    if ( !blockArray )
    {
        Error::Warn( std::cout, "block [SUMCONSTR] was not present!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    if ( blockArray->size() != mConfigCollections.size() )
    {
        Error::Warn( std::cout, "number of [SUMCONSTR] blocks does not match number of [SITES] blocks!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    size_t coll = 0;
    for ( const Block &block : *blockArray )
    {
        ConfigCollection &confColl = mConfigCollections[coll];

        if ( block.Size() < 1 )
        {
            Error::Warn( std::cout, "block [SUMCONSTR] was too small ( at least 1 argument expected ) !" );
            return Error::STATUS::FAILED_IO;
        }

        U32 constraints = block.GetToken( 0 )->GetValue< U32 >();

        U32 index = 1;
	
        for ( U32 i=0; i < constraints; ++i )
        {
            //read the amount of indices
            if ( index >= block.Size() )
            {
                Error::Warn( std::cout, "block [SUMCONSTR] was too small!" );
                return Error::STATUS::FAILED_IO;
            }
    	
            U32 indices = block.GetToken( index )->GetValue< U32 >();
    	
            if ( indices < 2 )
            {
                Error::Warn( std::cout, "at least two indices expected in a sym constraint!" );
                return Error::STATUS::FAILED_IO;
            }
    	
            if ( ( index+2+(indices*2)) > block.Size() )
            {
                Error::Warn( std::cout, "block [SUMCONSTR] was too small!" );
                return Error::STATUS::FAILED_IO;
            }
    	
            index+=1;
    	
            std::vector< std::pair< U32, valueType > > ind;
            for ( U32 j=0 ; j < indices; ++j )
            {
                U32 id = block.GetToken( index )->GetValue< U32 >();
    		
                std::string fitflags = block.GetToken( index+1 )->GetToken();
                U32 convFlags = Util::ToFlags( fitflags );
			
                Error::STATUS checkStatus;
                if ( Error::FAILED( checkStatus = FlagSanityCheck( convFlags) ) )
                {
                    return checkStatus;
                }
                
                for ( U32 n=0; n < 9; ++n )
                {
                    if ( convFlags & ( 1 << n ) )
                    {
                        ind.push_back( std::pair< U32, valueType >( id, static_cast< valueType >( n ) ) );
                    }
                }
    		
                index += 2;
            }
    	
            F32 value = block.GetToken( index )->GetValue< F32 >();
    	

            if ( ind.size() > 0 )
            {
                confColl.sumConstraints.push_back( SumConstraint( ind, value ) );
            }
            
    	
            index += 1;
        }
    
        if ( block.Size() != index )
        {
            Error::Warn( std::cout, "block [SUMCONSTR] contained more statements than expected based on the size indicator!" );
            return Error::STATUS::FAILED_IO;
        }
        
        coll++;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadRespRestraints( const BlockParser &bp )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("[RESP]");
    
    if ( !blockArray )
    {
        Error::Warn( std::cout, "block [RESP] was not present!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    if ( blockArray->size() != mConfigCollections.size() )
    {
        Error::Warn( std::cout, "number of [RESP] blocks does not match number of [SITES] blocks!" );
        
        return Error::STATUS::FAILED_IO;
    }
    
    size_t coll = 0;
    for ( const Block &block : *blockArray )
    {
        ConfigCollection &confColl = mConfigCollections[coll];

        if ( block.Size() < 1 )
        {
            Error::Warn( std::cout, "block [RESP] was too small ( at least 1 argument expected ) !" );
            return Error::STATUS::FAILED_IO;
        }

        U32 constraints = block.GetToken( 0 )->GetValue< U32 >();

        U32 index = 1;
	
        for ( U32 i=0; i < constraints; ++i )
        {
            if ( block.Size() < ( index + 2 ) )
            {
                Error::Warn( std::cout, "block [RESP] did not have the right amount of arguments based on the size indicator !"  );
                return Error::STATUS::FAILED_IO;
            }
    	
            U32 id = block.GetToken( index )->GetValue< U32 >();
    	
            std::string fitflags = block.GetToken( index+1 )->GetToken();
            U32 convFlags = Util::ToFlags( fitflags );
        
            Error::STATUS checkStatus;
            if ( Error::FAILED( checkStatus = FlagSanityCheck( convFlags) ) )
            {
                return checkStatus;
            }
            
            //find the amount of types
            U32 typesCount = 0;
            for ( U32 j=0; j < 9; ++j )
            {
                if ( convFlags & ( 1 << j ) )
                {
                    typesCount++;
                }
        		
            }
            
            //shift by 2
            index += 2;
        
            //test for size
            if ( block.Size() < ( index + ( 2 * typesCount ) ) )
            {
                Error::Warn( std::cout, "block [RESP] contains an index ( "+Util::ToString( id )+
                                        " ) that does not contain enough reference values and force constants!" );
                return Error::STATUS::FAILED_IO;
            }
        
            for ( U32 j=0; j < typesCount; ++j )
            {
                F32 val = block.GetToken( index+j )->GetValue< F32 >();
                F32 intensity = block.GetToken( index+j+typesCount )->GetValue< F32 >();
        	
                if ( convFlags & ( 1 << j ) )
                {
                    confColl.respConstraints.push_back( RespRestraint( static_cast< valueType >( j ), id, val, intensity ) );
                }
            }

            index += ( 2 * typesCount );
        }
    
        if ( index != block.Size() )
        {
            Error::Warn( std::cout, "block [RESP] has more arguments than expected, based on the size indicator!"  );
            return Error::STATUS::FAILED_IO;
        }
        
        coll++;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::FlagSanityCheck( U32 convFlags ) const
{
    // Perform a sanity check
    // We do not allow mixing of q, dpol and qpol
    if ( (convFlags & 0x01) && ( convFlags & 0x1FE ) )
    {
        Error::Warn( std::cout, "Invalid input flag detected, we do not allow mixing of charge constraints/restraints with dipole/qpol!"  );
        return Error::STATUS::FAILED_IO;
    }
    else if ( (convFlags & 0xE) && ( convFlags & 0x1F0 ) )
    {
        Error::Warn( std::cout, "Invalid input flag detected, we do not allow mixing of dipole constraints/restraints with qpol!"  );
        return Error::STATUS::FAILED_IO;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadUnionFit( const BlockParser &bp )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("[UNION]");
    
    if ( !blockArray )
    {
        return Error::STATUS::OPTIONAL_BLOCK;
    }
    
    for ( const Block &block : *blockArray )
    {
        if ( block.Size() < 1 )
        {
            Error::Warn( std::cout, "block [UNION] was too small ( at least 1 argument expected ) !" );
            return Error::STATUS::FAILED_IO;
        }
        
        const U32 numUnions = block.GetToken( 0 )->GetValue< U32 >();
        
        U32 index = 1;
        for ( U32 i=0; i < numUnions; ++i )
        {
            if ( block.Size() < ( index + 6 ) )
            {
                Error::Warn( std::cout, "block [UNION] did not have the right amount of arguments based on the size indicator !"  );
                return Error::STATUS::FAILED_IO;
            }
            
            const std::string fitflags = block.GetToken( index )->GetToken();
            
            std::vector< valueType > flags;
            const U32 convFlags = Util::ToFlags( fitflags );
            
            Error::STATUS checkStatus;
            if ( Error::FAILED( checkStatus = FlagSanityCheck( convFlags) ) )
            {
                return checkStatus;
            }
            
            if ( convFlags & 0x1F0 )
            {
                Error::Warn( std::cout, "block [UNION] contains constraints for qpols, while these are currently not supported !"  );
                return Error::STATUS::FAILED_IO;
            }
            
            for ( U32 j=0; j < 9; ++j )
            {
                if ( convFlags & ( 1 << j ) )
                {
                    flags.push_back( static_cast< valueType >( j ) );
                }
            }
            
            const U32 collection_i = block.GetToken( index+1 )->GetValue< U32 >();
            const U32 ID_i = block.GetToken( index+2 )->GetValue< U32 >();
            
            const U32 collection_start_j = block.GetToken( index+3 )->GetValue< U32 >();
            const U32 collection_end_j = block.GetToken( index+4 )->GetValue< U32 >();
            const U32 ID_j = block.GetToken( index+5 )->GetValue< U32 >();
            
            if ( collection_start_j >= mConfigCollections.size() ||
                 collection_end_j >= mConfigCollections.size() ||
                 collection_i >= mConfigCollections.size() )
            {
                Error::Warn( std::cout, "block [UNION] contains an out of range collection!"  );
                return Error::STATUS::FAILED_IO;
            }
            
            mUnionConstraint.emplace_back( UnionContraint(collection_i, ID_i, collection_start_j, collection_end_j, ID_j, flags) );
            
            //shift by 6
            index += 6;
        }
        
        if ( index != block.Size() )
        {
            Error::Warn( std::cout, "block [RESP] has more arguments than expected, based on the size indicator!"  );
            return Error::STATUS::FAILED_IO;
        }
    }
    
    return Error::STATUS::OK;
}



