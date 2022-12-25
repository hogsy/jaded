/*$T EOTstruct.h GC! 1.077 03/13/00 16:14:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * Definition of a set of Engine Objects Tables £
 * There is one of this structure for each world.
 */
#include "TABles/TABles.h"

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __EOTSTRUCT_H__
#define __EOTSTRUCT_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
typedef struct	EOT_tdst_SetOfEOT_
{
	TAB_tdst_PFtable	st_Visu;		/* Objects that have a visual */
	TAB_tdst_PFtable	st_Anims;		/* Objects that have an animation */
	TAB_tdst_PFtable	st_Dyna;		/* Objects that move, without priority considerations */
	TAB_tdst_PFtable	st_AI;			/* Objects that have an AI */
	TAB_tdst_PFtable	st_ColMap;		/* Objects that have a ColMap */
	TAB_tdst_PFtable	st_ZDM;			/* Objects that have a ColSet */
	TAB_tdst_PFtable	st_ZDE;			/* Objects with events Zones */
	TAB_tdst_PFtable	st_Events;		/* Objects with event tracks */
	TAB_tdst_PFtable	st_Hierarchy;	/* Objects with a father */
	TAB_tdst_PFtable	st_ODE;	/* Objects with ODE */
} EOT_tdst_SetOfEOT;
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EOTSTRUCT_H__ */ 
 