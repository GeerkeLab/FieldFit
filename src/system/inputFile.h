#ifndef __INPUTFILE_H__
#define __INPUTFILE_H__

#include "../common/types.h"
#include "../error/error.h"
#include "../math/vec3.h"

#include <string>
#include <vector>

class InputFile
{
public:

    InputFile ( const std::string &file );

    const Error::STATUS GetStatus() const;

private:

    Error::STATUS mStatus;

    std::string mConfigurationFile;
    std::string mFieldFile;
};


#endif