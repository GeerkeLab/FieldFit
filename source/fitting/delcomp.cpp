#include "fitting/delcomp.h"

#include "configuration/fitType.h"

namespace FieldFit
{
    arma::vec DelComp( const F64 posX, const F64 posY, const F64 posZ,
                       const arma::vec &gridX, const arma::vec &gridY,  const arma::vec &gridZ,
                       const FitType type )
    {
        arma::vec delX = gridX - posX;
        arma::vec delY = gridY - posY;
        arma::vec delZ = gridZ - posZ;
        
        const arma::vec r2 = delX % delX + delY % delY + delZ % delZ;
        const arma::vec r = arma::sqrt( r2 );
        
        switch( type )
        {
        case FitType::charge:
        	
        	return arma::ones( delX.n_elem ) / r;
        	
        case FitType::dipoleX:
        	
            return delX / ( r2 % r );
    
        case FitType::dipoleY:
        	
            return delY / ( r2 % r );
    
        case FitType::dipoleZ:
        	
            return delZ / ( r2 % r );
    
        case FitType::qd20:
    
            return ( delZ % delZ - (delX % delX + delY % delY ) * 0.5 ) / ( r2 % r2 % r );
    
        case FitType::qd21c:
    
            return ( delX % delZ * 1.73205081 ) / ( r2 % r2 % r );
    
        case FitType::qd21s:
    
            return ( delY % delZ * 1.73205081 ) / ( r2 % r2 % r );
    
        case FitType::qd22c:
    
            return ( (delX % delX - delY % delY) * (0.5 * 1.73205081 ) ) / ( r2 % r2 % r );
    
        case FitType::qd22s:
    
            return ( delX % delY * 1.73205081 ) / ( r2 % r2 % r );
        }
        
        return arma::zeros( delX.n_elem );
    }
}