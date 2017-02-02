#include "configuration/system.h"

#include "common/exception.h"

#include <iostream>

FieldFit::Site::Site( const std::string &name,
                      const std::string &coultype,
                      const F64 coordX,
                      const F64 coordY,
                      const F64 coordZ ) :
     mName( name ), mCoulType( coultype ),
     mCoordX( coordX ), mCoordY( coordY ), mCoordZ( coordZ )
{
    for ( S32 i=0; i < FitType::size; ++i )
    {
        mTypes[i] = false;
    }
}
void FieldFit::Site::AddFitType( FitType type )
{
    mTypes[type] = true;
}

bool FieldFit::Site::TestFitType( FitType type ) const
{
    return mTypes[type];
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
}

size_t FieldFit::System::NumberOfColumns() const
{
    size_t n_col = 0;
    
    for( const Site *site : mSites )
    {
        for ( S32 t=0; t < FitType::size; ++t )
        {
            if ( site->TestFitType(t) )
            {
                n_col++;
            }
        }
    }
    
    return n_col;
}

void FieldFit::System::OnUpdate()
{
    if (!mFields || !mGrid)
    {
        throw ArgException( "FieldFit", "System::OnUpdate", "Fitting system "+mName+" requires both a grid and field" );
    }
    
    const size_t n_col = NumberOfColumns();
    const size_t n_sets = mFields->GetPotentials().n_rows;
    
    // find out the size we need to rescale
    coefmat = arma::zeros( n_col, n_col );
    potential = arma::zeros( n_col, n_sets );
    
    U32 row = 0;
    for ( const Site* site : mSites )
    {
        arma::vec delX_i = mGrid->GetX() - site->GetCoordX();
        arma::vec delY_i = mGrid->GetY() - site->GetCoordY();
        arma::vec delZ_i = mGrid->GetZ() - site->GetCoordZ();
    }
    
    /*
    for ( auto it = SitesBegin(), itend = SitesEnd(); it != itend; ++it, ++row )
    {
        arma::vec delX_i = mGrid.x - it->second.x;
        arma::vec delY_i = mGrid.y - it->second.y;
        arma::vec delZ_i = mGrid.z - it->second.z;
           
        arma::vec delcomp_i = DelComp( it->second.fitType, delX_i, delY_i, delZ_i );
        
        U32 col=0;
        for ( auto jt = SitesBegin(), jtend = SitesEnd(); jt != jtend; ++jt, ++col )
        {
            
            arma::vec delX_j = mGrid.x - jt->second.x;
            arma::vec delY_j = mGrid.y - jt->second.y;
            arma::vec delZ_j = mGrid.z - jt->second.z;
        
            coefmat.col( col )[ row ] = arma::sum( delcomp_i % DelComp( jt->second.fitType, delX_j, delY_j, delZ_j ) );
        } 
       
        for ( size_t s=0; s < NumPotentialSets(); ++s )
        {
            potential.row( row )[s] = arma::sum( delcomp_i % mGrid.potentials.col( s ) );
        }
    }
    */
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
    
    if ( field->GetPotentials().n_cols != 0 ||
         field->GetPotentials().n_rows != 0 )
    {
        throw ArgException( "FieldFit", "System::InsertGrid", "configuration "+mName+" was already assigned a potential matrix" );
    }
    
    mFields = field;
}

const std::string &FieldFit::System::GetName() const
{
    return mName;
}

FieldFit::Grid *FieldFit::System::GetGrid() const
{
    return mGrid;
}

FieldFit::Field *FieldFit::System::GetField() const
{
    return mFields;
}