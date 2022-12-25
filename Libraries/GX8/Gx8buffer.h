// Gx8buffer.h

#ifndef __GX8BUFFER_H__
#define __GX8BUFFER_H__

#include "WORld/WORstruct.h"
#include "GEOmetric/GEOobject.h"

#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
    Function
 ****************************************************************************************************/

void	Gx8_PrepareVB( WOR_tdst_World *pWorld );

void	Gx8_ReleaseVBForObject( GEO_tdst_Object *pObject );


#ifdef __cplusplus
}
#endif

#endif /* __GX8BUFFER_H__ */
