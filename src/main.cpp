#include "system/field.h"
#include "system/configuration.h"

#include "system/multiMatrix.h"

#include "fitting/fitter.h"

#include <iostream>

int main()
{	
    Field f( "../testInput/basicDipole/1007_esp.in" );
    Configuration conf( "../testInput/basicDipole/1007.fsite" );

    Fitter::FitSites( conf, f );

    system("pause");

	return 0;
}