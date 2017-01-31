#ifndef __REPORT_H__
#define __REPORT_H__

#define USE_REPORTING

#include <sstream>

#include "../common/types.h"

class Configuration;
class Field;

class Report
{
public:

    Report( const Configuration &conf, const Field &field, const bool verbose );

    void WriteToStream( std::ostream &stream );

private:

    std::stringstream mSs;
};

#endif