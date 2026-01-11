/*$T WORuniverse.h GC 1.138 07/08/05 18:37:27 */

/* Initialisations du module World */
#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __WORUNIVERSE_H__
#define __WORUNIVERSE_H__

#include "BASe/BAStypes.h"

#include "BIGfiles/BIGkey.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

WOR_tdst_World	*WOR_pst_Universe_AddWorldList(WOR_tdst_World *, BIG_KEY, char);
WOR_tdst_World	*WOR_pst_Universe_MergeWorldList(WOR_tdst_World *, BIG_KEY, char);
WOR_tdst_World	*WOR_pst_Universe_AddWorld(BIG_KEY, char, BOOL);
WOR_tdst_World	*WOR_pst_Universe_MergeWorld(WOR_tdst_World *, BIG_KEY, char, BOOL);
void			WOR_Universe_DeleteWorld(WOR_tdst_World *, char);
WOR_tdst_World	*WOR_pst_Universe_GetActiveWorld(int);
LONG			WOR_l_Universe_GetWorldIndex(WOR_tdst_World *_pst_World);
void			WOR_Universe_Update_GroPointerChange(void *, void *);
void			WOR_Universe_Update_RLI(struct WOR_tdst_Update_RLI_ *_pst_Data);

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#define WOR_Cte_MaxFixWorldNb	8

/*$2------------------------------------------------------------------------------------------------------------------*/

extern ULONG	WOR_gul_FixWorldKeys[WOR_Cte_MaxFixWorldNb];
extern ULONG	WOR_gul_FixWorldLoaded[WOR_Cte_MaxFixWorldNb];
extern ULONG	WOR_gul_FixWorldToLoad[WOR_Cte_MaxFixWorldNb];
extern ULONG	WOR_gul_FixWorldToUnload[WOR_Cte_MaxFixWorldNb];

/*$2------------------------------------------------------------------------------------------------------------------*/

void	WOR_RegisterFixWorldKey(int _l_Index, ULONG _ul_Key);
void	WOR_RequestFixWorld(ULONG _ul_Key);
void	WOR_ResolveFixWorldRequest(void);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WORUNIVERSE_H__ */
