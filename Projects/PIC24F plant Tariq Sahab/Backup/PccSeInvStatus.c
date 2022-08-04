/********************************************************************\
\*******************************************************************/

#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

/********** *** ** * Process Electronic Cards Data * ** ** **********/
  int	ii ;

  ChanBMP[0] = ChanBMP[1] = 0 ;

  for(ii=0; ii<6; ii++) {
    if(TSTBITchar(&EINVDT,ii)) {
      SETBITchar(ChanBMP,ii) ;
    }
  }
  if(ChanBMP[0] || ChanBMP[1]) {
  }
  return(0) ;
}
/********** ** * End Of Process Electronic Cards Data * ** **********/

/*************** *** ** * End Of File * ** ** ***************/