/*$T OBJgrp.h GC! 1.100 06/12/01 12:42:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __OBJGRP_H__
#define __OBJGRP_H__

#include "BASe/BAStypes.h"
#include "TABles/TABles.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/* Definition of a group */
#define OBJ_C_GroupFlag_Engine	1

typedef struct	OBJ_tdst_Group_
{
	TAB_tdst_PFtable	*pst_AllObjects;	/* All objects of the group */
	ULONG				ul_Flags;			/* Flags of the group */
	ULONG				ul_NbObjectsUsingMe;
#ifdef ACTIVE_EDITORS
	char				*sz_Name;
#endif
} OBJ_tdst_Group;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Protos
 -----------------------------------------------------------------------------------------------------------------------
 */

extern ULONG						OBJ_ul_World_LoadGolCallback(ULONG);
extern void							OBJ_FreeGroup(OBJ_tdst_Group *);
extern void							OBJ_FreeGroupGrl(TAB_tdst_PFtable *);
extern void							OBJ_AddInGroup(OBJ_tdst_Group *, void *);
extern void							OBJ_DelInGroup(OBJ_tdst_Group *, void *);
extern void							OBJ_OrderGroupByHie(OBJ_tdst_Group *);
extern struct OBJ_tdst_GameObject_	*OBJ_pst_GroupGetByRank(OBJ_tdst_Group *, int);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Access functions
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
    Aim:    Set flags
 =======================================================================================================================
 */
_inline_ void OBJ_GroupFlagsSet(OBJ_tdst_Group *_pst_Grp, ULONG _ul_Flags)
{
	_pst_Grp->ul_Flags |= _ul_Flags;
}

/*
 =======================================================================================================================
    Aim:    Get flags
 =======================================================================================================================
 */
_inline_ ULONG OBJ_GroupFlagsGet(OBJ_tdst_Group *_pst_Grp)
{
	return(_pst_Grp->ul_Flags);
}

/*
 =======================================================================================================================
    Aim:    Test Flags
 =======================================================================================================================
 */
_inline_ ULONG OBJ_b_GroupFlagsTest(OBJ_tdst_Group *_pst_Grp, ULONG _ul_Flags)
{
	return(_pst_Grp->ul_Flags & _ul_Flags);
}

/*
 =======================================================================================================================
    Aim:    Clear Flags
 =======================================================================================================================
 */
_inline_ void OBJ_GroupFlagsClear(OBJ_tdst_Group *_pst_Grp, ULONG _ul_Flags)
{
	(_pst_Grp->ul_Flags) &= (~_ul_Flags);
}

#ifdef ACTIVE_EDITORS

/* Extern OBJ_tdst_Group *OBJ_CreateNewGroup(WOR_tdst_World *, char *); */
extern void OBJ_SaveGolGroup(OBJ_tdst_Group *);

#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
