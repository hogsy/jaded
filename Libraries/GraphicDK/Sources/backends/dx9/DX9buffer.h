// Dx9buffer.h

#ifndef __DX9BUFFER_H__
#define __DX9BUFFER_H__

#include "Dx9struct.h"
#include "WORld/WORstruct.h"
#include "GEOmetric/GEOobject.h"

#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
    Function
 ****************************************************************************************************/

void	Dx9_PrepareVB( WOR_tdst_World *pWorld );

void	Dx9_ReleaseVBForObject( GEO_tdst_Object *pObject );


#ifdef __cplusplus
}
#endif

#endif /* __DX9BUFFER_H__ */
