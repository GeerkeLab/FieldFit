#include "report.h"

#include "configuration.h"
#include "field.h"

#include "../util/util.h"

Report::Report( const Configuration &conf, const Field &field, const bool verbose )
{
    // Find total number of fits
    U32 numSites = 0;
    for ( U32 coll=0; coll < conf.NumCollections(); ++coll )
    {
        for ( U32 i=0; i < conf.FitSites(coll); ++i )
        {
            const Configuration::FitSite *site = conf.GetSite( coll, i );
            
            // If this was a fitsite
            if (site->fitFlags != 0 )
            {
                numSites++;
            }
        }
    }
    
    //report the result
    mSs << "[FITS]" << std::endl;
    
    mSs << "\t" << numSites << std::endl  << std::endl;
    
    //if( !verbose )
    {
        mSs << "# Frame   ID  Name    Charge   DipoleX   DipoleY   DipoleZ       Q20      Q21C      Q21S      Q22C      Q22S     ALPHA" << std::endl;
    }
    
    // Now process them
    for ( U32 coll=0; coll < conf.NumCollections(); ++coll )
    {
        
        for ( U32 i=0; i < conf.FitSites(coll); ++i )
        {
            const Configuration::FitSite *site = conf.GetSite( coll, i );
            
            // If this was a fitsite
            if (site->fitFlags != 0 )
            {
            
                std::string flagString = Util::FromFlags( site->fitFlags );
                
                /*
                if ( verbose )
                {
            
                    mSs << "\tSITE   " << site->name << "\t" << site->position.GetX() << "\t" << site->position.GetY() << "\t" << site->position.GetZ() << std::endl;
                    mSs << "\tFRAME  " << coll << std::endl;
                    mSs << "\tID     " << site->ID << std::endl;
                    mSs << "\tFLAGS  " << flagString << std::endl;
                    mSs << "\tCHARGE " << site->GetValue( Configuration::valueType::charge ) << std::endl;
                    mSs << "\tDX     " << site->GetValue( Configuration::valueType::dipoleX ) << std::endl;
                    mSs << "\tDY     " << site->GetValue( Configuration::valueType::dipoleY ) << std::endl;
                    mSs << "\tDZ     " << site->GetValue( Configuration::valueType::dipoleZ ) << std::endl;
                    mSs << "\tQ20    " << site->GetValue( Configuration::valueType::qd20 ) << std::endl;
                    mSs << "\tQ21C   " << site->GetValue( Configuration::valueType::qd21c ) << std::endl;
                    mSs << "\tQ21S   " << site->GetValue( Configuration::valueType::qd21s ) << std::endl;
                    mSs << "\tQ22C   " << site->GetValue( Configuration::valueType::qd22c ) << std::endl;
                    mSs << "\tQ22S   " << site->GetValue( Configuration::valueType::qd22s ) << std::endl;
                    mSs << "\tAlpha  " << field.GetAlpha( coll, i, conf) << std::endl;
                    mSs << std::endl;
                }
                else
                 */
                {
                 
                    mSs << Util::ToStringFormatted( coll , 5, 6 )
                        << Util::ToString( site->ID, 6 )
                        << Util::AddWhiteSpace( site->name, 6 )
                        << Util::ToStringFormatted( site->GetValue( Configuration::valueType::charge ) , 5, 10 )
                        << Util::ToStringFormatted( site->GetValue( Configuration::valueType::dipoleX ) , 5, 10 )
                        << Util::ToStringFormatted( site->GetValue( Configuration::valueType::dipoleY ) , 5, 10 )
                        << Util::ToStringFormatted( site->GetValue( Configuration::valueType::dipoleZ ), 5, 10 )
                        << Util::ToStringFormatted( site->GetValue( Configuration::valueType::qd20 ) , 5, 10 )
                        << Util::ToStringFormatted( site->GetValue( Configuration::valueType::qd21c ) , 5, 10 )
                        << Util::ToStringFormatted( site->GetValue( Configuration::valueType::qd21s ) , 5, 10 )
                        << Util::ToStringFormatted( site->GetValue( Configuration::valueType::qd22c ), 5, 10 )
                        << Util::ToStringFormatted( site->GetValue( Configuration::valueType::qd22s ) , 5, 10 )
                        << Util::ToStringFormatted( field.GetAlpha( coll, i, conf ) , 5, 10 )
                        << std::endl;
                }
                
            }
        }
    }
    
    mSs << "[END]" << std::endl;
    
    const F64 chisqr = field.GetFieldStats( conf );
        
    //also calculate chiSqr
    mSs << "[CHISQR]" << std::endl;
    mSs << "\t" << chisqr << std::endl;
    mSs << "[END]" << std::endl;
    
}

void Report::WriteToStream( std::ostream &stream )
{
        stream << mSs.str();
}