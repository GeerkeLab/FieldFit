#ifndef __REPORT_H__
#define __REPORT_H__

#include <sstream>

#include "configuration.h"
#include "field.h"

#include "../common/types.h"

class Report
{
public:

    Report( const Configuration &conf, const Field &field )
    {
        //report the result
        mSs << "[FITS]" << std::endl;
        mSs << "\t\t" << conf.Size() << std::endl << std::endl;

        for ( U32 i=0; i < conf.Size(); ++i )
        {
            const Configuration::FitSite *site = conf.GetSite( i );
            
            std::string flagString;
            for ( U32 j=0; j < 9; j++ )
            {
                if ( site->fitFlags & ( 1 << j ) )

                    flagString += "1";
                else

                    flagString += "0";
            }

            mSs << "\tSITE   " << site->name << "\t" << site->position.GetX() << "\t" << site->position.GetY() << "\t" << site->position.GetZ() << std::endl;
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
            mSs << std::endl;
        }

        mSs << "[END]" << std::endl;
    }

    void WriteToStream( std::ostream &stream )
    {
        stream << mSs.str();
    }

private:

    std::stringstream mSs;
};

#endif