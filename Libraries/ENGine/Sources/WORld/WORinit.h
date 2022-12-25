/*$T WORinit.h GC! 1.081 06/13/00 10:01:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Initialisations du module World */
#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __WORINIT_H__
#define __WORINIT_H__

#include "BASe/BAStypes.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Initialisation and destruction of the universe
 -----------------------------------------------------------------------------------------------------------------------
 */

void			WOR_Universe_Init(void);
void			WOR_Universe_Destroy(void);
void			WOR_Universe_Open(BIG_KEY);
void			WOR_Universe_Close(LONG);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Views
 -----------------------------------------------------------------------------------------------------------------------
 */

int				WOR_View_Create(WOR_tdst_World *);
void			WOR_View_Init(WOR_tdst_View *, MATH_tdst_Matrix *, OBJ_tdst_GameObject *);
void			WOR_View_Close(WOR_tdst_View *);
void			WOR_View_SetFocale(WOR_tdst_View *, float);
void			WOR_View_SetViewport(WOR_tdst_View *, float, float, float, float);

#ifdef ACTIVE_EDITORS
BOOL			WOR_View_SplitView(WOR_tdst_World *);
void			WOR_View_UnsplitView(WOR_tdst_World *);
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Initialisation & destruction of a world
 -----------------------------------------------------------------------------------------------------------------------
 */

WOR_tdst_World	*WOR_pst_World_Create(void);
BOOL			WOR_World_Close(WOR_tdst_World *);
BOOL			WOR_World_Destroy(WOR_tdst_World *);
void			WOR_World_Init(WOR_tdst_World *, ULONG);
void            WOR_World_Attached(WOR_tdst_World *);
void			WOR_World_KillRasters(WOR_tdst_World *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WORINIT_H__ */
