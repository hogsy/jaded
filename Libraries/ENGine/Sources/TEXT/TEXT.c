/*$T TEXT.c GC!1.71 02/14/00 17:19:13 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "TEXTstruct.h"
#include "TEXTload.h"
#include "BASe/MEMory/MEM.h"
#include "INOut/INO.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#undef __FILE__
#define __FILE__ "TXT.c"

/* Global var for all texts */
TEXT_tdst_AllText   TEXT_gst_Global;

#ifdef ACTIVE_EDITORS
extern BOOL			LOA_gb_SpeedMode ;
extern BOOL        EDI_gb_ComputeMap;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXT_InitModule(void)
{
    L_memset(&TEXT_gst_Global, 0, sizeof(TEXT_gst_Global));

#ifdef ACTIVE_EDITORS
    if(LOA_gb_SpeedMode || EDI_gb_ComputeMap)
    {
    	TEXT_gst_Global.i_CurrentLanguage = INO_e_French;	/* Default language */
    	TEXT_gst_Global.i_CurrentSndLanguage = INO_e_French;	/* Default language */
    }
    else
    {
        TEXT_gst_Global.i_CurrentLanguage = INO_e_English;	
        TEXT_gst_Global.i_CurrentSndLanguage = INO_e_English;	
    }
#else
	TEXT_gst_Global.i_CurrentLanguage = INO_e_French;	/* Default language */
	TEXT_gst_Global.i_CurrentSndLanguage = INO_e_French;	/* Default language */
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXT_CloseModule(void)
{
	TEXT_FreeAll();
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
