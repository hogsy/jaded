/*$T AI.c GC! 1.076 03/08/00 18:19:54 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "AIengine.h"
#include "AIdebug.h"
#include "BASe/MEMory/MEM.h"
#include "INOut/INOsaving.h"


extern void AI2C_InitFix(void);
extern SCR_tt_SFDyn	AI_gst_SFDyn[SFDYN_MAX];


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_InitModule(void)
{
	AI_ResolveEnums();
	L_memset(AI_gpst_MainActors, 0, sizeof(AI_gpst_MainActors));
	L_memset(AI_gst_SFDyn, 0, sizeof(AI_gst_SFDyn));
	L_memset(INO_gst_SavManager.ac_CurrSlotBuffer+INO_Cte_SavUniverseMaxSize, 0, sizeof(AI_gst_SFDyn));

#ifdef ACTIVE_EDITORS
	AI_gpc_StackIn = (char*)MEM_p_Alloc(10000);
	AI_InitBreakList();
#endif

#ifdef GAMECUBE_USE_AI2C_DLL
    AI2C_InitFix();
#endif    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_CloseModule(void)
{
#ifdef ACTIVE_EDITORS
	if(AI_gpc_StackIn) MEM_Free(AI_gpc_StackIn);
#endif
}

