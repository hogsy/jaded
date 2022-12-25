/*$T OBJslowaccess.h GC!1.71 02/25/00 14:46:44 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/* All accesses to the structures */
#ifndef __OBJ_SLOWACCESS_H__
#define __OBJ_SLOWACCESS_H__

#include "ENGine/Sources/OBJects/OBJstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void    *OBJ_p_GetGro(OBJ_tdst_GameObject *);
void    *OBJ_p_GetGrm(OBJ_tdst_GameObject *);
BOOL    OBJ_b_IsChild(OBJ_tdst_GameObject *);
void    *OBJ_p_GetCurrentGeo( OBJ_tdst_GameObject *);

void	OBJ_VertexColor_ChangePointer( OBJ_tdst_GameObject *, ULONG *, int);
ULONG	*OBJ_VertexColor_Realloc( OBJ_tdst_GameObject *, ULONG );
void	OBJ_VertexColor_Free( OBJ_tdst_GameObject * );

#ifdef ACTIVE_EDITORS
BOOL OBJ_b_AllGao_GetBV( ULONG, MATH_tdst_Vector *, MATH_tdst_Vector *);

// Get the location of the RLI. return value :
// 0x00 -> No RLI
// 0x01 -> RLI in GAO
// 0x02 -> RLI in GRO
// 0x04 -> RLI in, at least, one LOD
u8 OBJ_i_RLIlocation (OBJ_tdst_GameObject *);
#define u8_RLIinGAO 0x01
#define u8_RLIinGRO 0x02
#define u8_RLIinLOD 0x04

#endif //ACTIVE_EDITORS

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __OBJ_SLOWACCESS_H */
