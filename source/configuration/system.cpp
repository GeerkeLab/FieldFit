#include "configuration/system.h"

#include "common/exception.h"

#include "fitting/delcomp.h"

#include <iostream>

FieldFit::Site::Site( const U32 types,
                      const std::string &name,
                      const std::string &coultype,
                      const F64 coordX,
                      const F64 coordY,
                      const F64 coordZ ) :
     mName( name ), mCoulType( coultype ),
     mCoordX( coordX ), mCoordY( coordY ), mCoordZ( coordZ ),
     mTypes( types )
{
}

bool FieldFit::Site::TestFitType( FitType type ) const
{
    return IsSet( mTypes, type );
}

bool FieldFit::Site::TestSpecialType( SpecialFlag type ) const
{
    return IsSpecialSet( mTypes, type );
}

void FieldFit::Site::AddEfield( const arma::vec &ex,
                                const arma::vec &ey,
                                const arma::vec &ez )
{
    if ( mEfield_x.n_elem != 0 ||
         mEfield_y.n_elem != 0 ||
         mEfield_z.n_elem != 0  )
    {
        throw ArgException( "FieldFit", "Site::AddEfield", "electric field for an atom "+mName+" was already set" );
    }
    
    mEfield_x = ex;
    mEfield_y = ey;
    mEfield_z = ez;
}

const std::string &FieldFit::Site::GetName() const
{
    return mName;
}

const std::string &FieldFit::Site::GetCoulType() const
{
    return mCoulType;
}
        
F64 FieldFit::Site::GetCoordX() const
{
    return mCoordX;
}

F64 FieldFit::Site::GetCoordY() const
{
    return mCoordY;
}

F64 FieldFit::Site::GetCoordZ() const
{
    return mCoordZ;
}

const arma::vec &FieldFit::Site::GetEfieldX() const
{
    return mEfield_x;
}

const arma::vec &FieldFit::Site::GetEfieldY() const
{
    return mEfield_y;
}

const arma::vec &FieldFit::Site::GetEfieldZ() const
{
    return mEfield_z;
}

U32 FieldFit::Site::GetFlags() const
{
    return mTypes;
}

size_t FieldFit::Site::NumColumns() const
{
   size_t collOffset = 0;
    
   for ( S32 t=0; t < FitType::size; ++t )
   {
        FitType fitType = (FitType) t;
        
        if ( TestFitType( fitType )  )
        {
            collOffset++;
        }
    } 
    
    return collOffset;
}

FieldFit::Grid::Grid( const arma::vec &x,
        	       	  const arma::vec &y,
                      const arma::vec &z ) :
      mGridX( x ), mGridY( y ), mGridZ( z )
{
    
}
const arma::vec &FieldFit::Grid::GetX() const
{
    return mGridX;
}

const arma::vec &FieldFit::Grid::GetY() const
{
    return mGridY;
}

const arma::vec &FieldFit::Grid::GetZ() const
{
    return mGridZ;
}

size_t FieldFit::Grid::Size()
{
    return mGridX.n_rows;
}

FieldFit::Field::Field( const arma::mat &mat ) :
    mPotentials( mat )
{
    
}

const arma::mat &FieldFit::Field::GetPotentials() const
{
    return mPotentials;
}

FieldFit::System::System( const std::string &name ) :
    mGrid(nullptr), mFields(nullptr), mName( name )
{
    
}

FieldFit::System::~System()
{
    mNameToSite.clear();
    
    for( Site *site : mSites )
    {
        delete site;
    }
    
    if ( mGrid )
    {
        delete mGrid;
    }
    
    if ( mFields )
    {
        delete mFields;
    }
}

size_t FieldFit::System::NumberOfColumns() const
{
    size_t n_col = 0;
    
    for( const Site *site : mSites )
    {
        for ( S32 t=0; t < FitType::size; ++t )
        {
            if ( site->TestFitType((FitType)t) )
            {
                n_col++;
            }
        }
    }
    
    return n_col;
}

void FieldFit::System::OnUpdate()
{
    if (!mFields)
    {
        throw ArgException( "FieldFit", "System::OnUpdate", "Fitting system "+mName+" requires a field" );
    }
    
    if ( !mGrid)
    {
        throw ArgException( "FieldFit", "System::OnUpdate", "Fitting system "+mName+" requires a grid" );
    }
    
    const size_t n_col = NumberOfColumns();
    const size_t n_sets = mFields->GetPotentials().n_cols;
    
    // find out the size we need to rescale
    mX_prime_x = arma::zeros( n_col, n_col );
    mX_prime_y = arma::zeros( n_col, n_sets );
    
    //
    // Todo, generate effective potentials, by subtracting the permanent field
    //
    
    U32 row = 0;
    for ( const Site* site_i : mSites )
    {
        //test if we have to include this type
        for ( S32 t_i=0; t_i < FitType::size; ++t_i )
        {
            FitType fitType_i = (FitType) t_i;
            
            if ( site_i->TestFitType((FitType)t_i) )
            {
                arma::vec delcomp_i = DelComp( site_i->GetCoordX(), site_i->GetCoordY(), site_i->GetCoordZ(),
                           	                   mGrid->GetX(), mGrid->GetY(),  mGrid->GetZ(), fitType_i );
                
                U32 col = 0;
                for ( const Site* site_j : mSites )
                {
                    for ( S32 t_j=0; t_j < FitType::size; ++t_j )
                    {
                        FitType fitType_j = (FitType) t_j;
                    	
                        if ( site_j->TestFitType((FitType)t_j) )
                        {
                            arma::vec delcomp_j = DelComp( site_j->GetCoordX(), site_j->GetCoordY(), site_j->GetCoordZ(),
                           	                    	       mGrid->GetX(), mGrid->GetY(),  mGrid->GetZ(), fitType_j );
                                                              
                            mX_prime_x.col( col )[ row ] = arma::sum( delcomp_i % delcomp_j );
                    	   
                            col++;
                        }
                    }
                }
                
                for ( size_t s=0; s < n_sets; ++s )
                {
                    mX_prime_y.row( row )[s] = arma::sum( delcomp_i % mFields->GetPotentials().col( s ) );
                }
                
                row++;
            }
        }
    }
}

void FieldFit::System::OnUpdate2()
{
    if (!mFields)
    {
        throw ArgException( "FieldFit", "System::OnUpdate", "Fitting system "+mName+" requires a field" );
    }
    
    if ( !mGrid)
    {
        throw ArgException( "FieldFit", "System::OnUpdate", "Fitting system "+mName+" requires a grid" );
    }
    
    const size_t n_col = NumberOfColumns();
    const size_t n_sets = mFields->GetPotentials().n_cols;
    const size_t n_points = mFields->GetPotentials().n_rows;
    
    // find out the size we need to rescale
    mX_prime_x = arma::zeros( n_col, n_col );
    mX_prime_y = arma::zeros( n_col, n_sets );
    mCoefficients = arma::zeros( n_points, n_col );
    
    //
    // Todo, generate effective potentials, by subtracting the permanent field
    //
    
    U32 col = 0;
    for ( const Site* site_i : mSites )
    {
        //test if we have to include this type
        for ( S32 t_i=0; t_i < FitType::size; ++t_i )
        {
            FitType fitType_i = (FitType) t_i;
            
            if ( site_i->TestFitType((FitType)t_i) )
            {
                mCoefficients.col( col ) = DelComp( site_i->GetCoordX(), site_i->GetCoordY(), site_i->GetCoordZ(),
                           	                   mGrid->GetX(), mGrid->GetY(),  mGrid->GetZ(), fitType_i );
                                                  
                col++;
            }
        }
    }
    
    arma::mat x_prime = arma::trans( mCoefficients );
    
    mX_prime_x = x_prime * mCoefficients;
    mX_prime_y = x_prime * mFields->GetPotentials();
}

FieldFit::Site * FieldFit::System::FindSite( const std::string &name )
{
    auto nts_it = mNameToSite.find( name );
    
    if ( nts_it == mNameToSite.end() )
    {
        return nullptr;
    }
    
    return nts_it->second;
}

void FieldFit::System::InsertSite( Site *site )
{
    if ( !site )
    {
        throw ArgException( "FieldFit", "System::InsertSite", "Tried to insert and NULL site" );
    }
    
    auto nts_it = mNameToSite.find( site->GetName() );
    
    if ( nts_it != mNameToSite.end() )
    {
        throw ArgException( "FieldFit", "System::InsertSite", "Site with name "+site->GetName()+" already exists for system "+mName );
    }
   
    mSites.push_back( site );
    mNameToSite.insert( std::make_pair( site->GetName(), site ) );
}

void FieldFit::System::InsertGrid( Grid *grid )
{
    if ( !grid )
    {
        throw ArgException( "FieldFit", "System::InsertGrid", "Tried to insert and NULL grid" );
    }
    
    if (mGrid)
    {
        throw ArgException( "FieldFit", "System::InsertGrid", "A grid already exists for system "+mName );
    }
    
    mGrid = grid;
}

void FieldFit::System::InsertField( Field *field )
{
    if ( !field )
    {
        throw ArgException( "FieldFit", "System::InsertField", "Tried to insert and NULL field" );
    }
    
    if (mFields)
    {
        throw ArgException( "FieldFit", "System::InsertField", "A field already exists for system "+mName );
    }
    
    mFields = field;
}

const std::string &FieldFit::System::GetName() const
{
    return mName;
}

const std::vector< FieldFit::Site* > &FieldFit::System::GetSites() const
{
    return mSites;
}

FieldFit::Grid *FieldFit::System::GetGrid() const
{
    return mGrid;
}

FieldFit::Field *FieldFit::System::GetField() const
{
    return mFields;
}

const arma::mat &FieldFit::System::GetLocalXPrimeX() const
{
    return mX_prime_x;
}

const arma::mat &FieldFit::System::PotentialMatrix() const
{
    return mX_prime_y;
}

const F64 FieldFit::System::ComputeChi2( arma::vec result, size_t collIndex ) const
{
    if ( !mFields || collIndex >= mFields->GetPotentials().n_cols )
    {
        throw ArgException( "FieldFit", "System::ComputeChi2", "Tried to access a field column that does not exist " );
    }
    
    arma::vec diffE = mFields->GetPotentials()[collIndex] - (mCoefficients * result);
    arma::vec chi2 = arma::trans(diffE) * diffE;
    
    return chi2[0];
}

size_t FieldFit::System::NumColumns() const
{
    size_t cols = 0;
    for( const Site *site : mSites )
    {
        cols += site->NumColumns();
    }
    
    return cols;
}

