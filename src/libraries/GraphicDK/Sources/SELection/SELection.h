/*$T SELection.h GC! 1.081 06/17/02 11:54:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "BASe/BAStypes.h"

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __SELECTION_H__
#define __SELECTION_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define SEL_C_SIF_Object		0x00000001
#define SEL_C_SIF_Link			0x00000002
#define SEL_C_SIF_ZDx			0x00000004
#define SEL_C_SIF_Cob			0x00000008
#define SEL_C_SIF_HieLink		0x00000010

#define SEL_C_SIF_ContentIsAKey 0x40000000
#define SEL_C_SIF_Delete		0x80000000

#define SEL_C_SIF_All			0xFFFFFFFF

#define SEL_C_SF_Locked			1
#define SEL_C_SF_Editable		2

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	SEL_tdst_SelectedItem_
{
	void							*p_Content;
	LONG							ul_User;
	LONG							l_Flag;
	struct SEL_tdst_SelectedItem_	*pst_Next;
} SEL_tdst_SelectedItem;

typedef struct	SEL_tdst_Selection_
{
	LONG					l_Flag;
	SEL_tdst_SelectedItem	*pst_FirstItem;
	SEL_tdst_SelectedItem	*pst_LastItem;
} SEL_tdst_Selection;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void					SEL_Init(SEL_tdst_Selection *);
void					SEL_Close(SEL_tdst_Selection *);

BOOL					SEL_b_IsEmpty(SEL_tdst_Selection *);
LONG					SEL_l_CountItem(SEL_tdst_Selection *, LONG);

SEL_tdst_SelectedItem	*SEL_pst_AddItem(SEL_tdst_Selection *, void *, LONG);
void					SEL_DelItem(SEL_tdst_Selection *, void *);
void					SEL_DelItems(SEL_tdst_Selection *, void *, LONG);
SEL_tdst_SelectedItem	*SEL_RetrieveItem(SEL_tdst_Selection *, void *);
SEL_tdst_SelectedItem	*SEL_RetrieveItemWithType(SEL_tdst_Selection *, void *, LONG);
void					SEL_ReplaceItemContent(SEL_tdst_Selection *, void *, void *);

void					SEL_EnumItem
						(
							SEL_tdst_Selection *,
							LONG,
							BOOL (*) (SEL_tdst_SelectedItem *, ULONG, ULONG),
							ULONG,
							ULONG
						);

void					SEL_EnumWorldGao
						(
							struct WOR_tdst_World_ *,
							BOOL (*) (SEL_tdst_SelectedItem *, ULONG, ULONG),
							ULONG,
							ULONG
						);

void					SEL_Duplicate(SEL_tdst_Selection *, SEL_tdst_Selection *);

void					*SEL_pst_GetFirstItem(SEL_tdst_Selection *, LONG);
SEL_tdst_SelectedItem	*SEL_pst_GetFirst(SEL_tdst_Selection *, LONG);
SEL_tdst_SelectedItem	*SEL_pst_GetNext(SEL_tdst_Selection *, SEL_tdst_SelectedItem *, LONG);
void					SEL_Circle(SEL_tdst_Selection *, int dir);

#endif ACTIVE_EDITORS

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SELECTION_H__ */
