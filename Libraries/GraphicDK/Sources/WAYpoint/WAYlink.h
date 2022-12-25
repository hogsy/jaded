/*$T WAYlink.h GC! 1.098 10/12/00 16:06:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS

#pragma once
#ifndef __WAYLINK_H__
#define __WAYLINK_H__

#include "GRObject/GROstruct.h"
#include "ENGine/Sources/WAYs/WAYstruct.h"
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	WAY_tdst_GraphicLink_
{
	OBJ_tdst_GameObject *pst_Origin;
	WAY_tdst_Link		*pst_Link;
} WAY_tdst_GraphicLink;

typedef struct	WAY_tdst_GraphicLinks_
{
	LONG					l_Max;
	LONG					l_Next;
	WAY_tdst_GraphicLink	*dst_GL;
} WAY_tdst_GraphicLinks;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void					WAY_Links_Init(WAY_tdst_GraphicLinks *);
void					WAY_Links_Close(WAY_tdst_GraphicLinks *);
void					WAY_Links_Clear(WAY_tdst_GraphicLinks *);
WAY_tdst_GraphicLink	*WAY_Links_GetNextLink(WAY_tdst_GraphicLinks *);
void					WAY_Link_Render( OBJ_tdst_GameObject *, WAY_tdst_Link *, ULONG, char, char );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __WAYLINK_H__ */ 

#endif /* ACTIVE_EDITORS */
 