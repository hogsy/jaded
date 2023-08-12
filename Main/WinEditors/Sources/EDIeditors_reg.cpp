/*$T EDIeditors_reg.cpp GC! 1.078 03/14/00 15:06:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * Aim: This is the file where all editors must be registered. £
 * You must also modified the EDIeditors_info.h file.
 */
#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"
#include "Res/Res.h"
#include "SouND/sources/SNDwave.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INCLUDE YOUR EDITOR FRAME HERE.
 -----------------------------------------------------------------------------------------------------------------------
 */

#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDItors/Sources/RASters/RASframe.h"
#include "EDItors/Sources/TEXtures/TEXframe.h"
#include "EDItors/Sources/PROperties/PROframe.h"
#include "EDItors/Sources/LOGfile/LOGframe.h"
#include "EDItors/Sources/AIscript/AIframe.h"
#include "EDItors/Sources/MENu/MENframe.h"
#include "EDItors/Sources/MATerial/MATframe.h"
#include "EDItors/Sources/TEXTe/TEXTframe.h"
#include "EDItors/Sources/SOuNd/SONframe.h"
#include "EDItors/Sources/ACTions/EACTframe.h"
#include "EDItors/Sources/EVEnts/EVEframe.h"
#include "EDItors/Sources/PreFaB/PFBframe.h"
#include "EDItors/Sources/PENdingCheckins/PENCframe.h"
#ifdef JADEFUSION
#include "EDItors/Sources/SELection/SELectionframe.h"
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::RegisterEditors(void)
{
	EDI_M_RegisterEditor(EDI_IDEDIT_BROWSER, EBRO_cl_Frame, "Browser", EBRO_IDD_DIALOGBAR, 0/*EBRO_IDR_TOOLBAR*/);
	EDI_M_RegisterEditor(EDI_IDEDIT_OUTPUT, EOUT_cl_Frame, "3D View", 0, EOUT_IDR_TOOLBAR);
	EDI_M_RegisterEditor(EDI_IDEDIT_RASTERS, ERAS_cl_Frame, "Rasters", ERAS_IDD_DIALOGBAR, 0);
	EDI_M_RegisterEditor(EDI_IDEDIT_TEXTURE, ETEX_cl_Frame, "Textures", PANES_IDD_DIALOGBAR, 0);
	EDI_M_RegisterEditor(EDI_IDEDIT_PROPERTIES, EPRO_cl_Frame, "Properties", 0, EPRO_IDR_TOOLBAR);
	EDI_M_RegisterEditor(EDI_IDEDIT_LOGFILE, ELOG_cl_Frame, "Log", 0, ELOG_IDR_TOOLBAR);
	EDI_M_RegisterEditor(EDI_IDEDIT_AI, EAI_cl_Frame, "AI script", PANES_IDD_DIALOGBAR, 0);
	EDI_M_RegisterEditor(EDI_IDEDIT_MENU, EMEN_cl_Frame, "Menus", 0, 0);
	EDI_M_RegisterEditor(EDI_IDEDIT_PREFAB, EPFB_cl_Frame, "Prefab", 0, 0);
	// Perforce related shite ~hogsy
	//EDI_M_RegisterEditor(EDI_IDEDIT_PENC, EPEN_cl_Frame, "Pending Checkins", 0, EPEN_IDR_TOOLBAR);
	//EDI_M_RegisterEditor(EDI_IDEDIT_PERF, EPER_cl_Frame, "Perforce", EPER_IDR_DIALOGBAR, 0);
	//EDI_M_RegisterEditor(EDI_IDEDIT_PERFCHECK, EPERCheck_cl_Frame, "Check Consistency", EPERCHECK_IDD_DIALOGBAR, 0);
#ifdef JADEFUSION
	EDI_M_RegisterEditor(EDI_IDEDIT_SELECTION, ESELection_cl_Frame, "Selection", 0, 0);
#endif
	/* Hole */
	mast_ListOfEditors[mi_TotalEditors].i_Type = -1;
	mast_ListOfEditors[mi_TotalEditors].po_Instance = NULL;
    mi_TotalEditors++;

	EDI_M_RegisterEditor(EDI_IDEDIT_MAT, EMAT_cl_Frame, "Material", 0, 0);
	//EDI_M_RegisterEditor(EDI_IDEDIT_TEXT, ETEXT_cl_Frame, "Text", PANES_IDD_DIALOGBAR, 0);
	EDI_M_RegisterEditor(EDI_IDEDIT_TEXT, ETEXT_cl_Frame, "Text", 0, 0);

	/* Hole */
	mast_ListOfEditors[mi_TotalEditors].i_Type = -1;
	mast_ListOfEditors[mi_TotalEditors].po_Instance = NULL;
    mi_TotalEditors++;

	EDI_M_RegisterEditor(EDI_IDEDIT_SOUND, ESON_cl_Frame, "Sound", PANES_IDD_SOUNDBAR, ESON_IDR_TOOLBAR);
	EDI_M_RegisterEditor(EDI_IDEDIT_ACTIONS, EACT_cl_Frame, "Actions", 0, 0);
	EDI_M_RegisterEditor(EDI_IDEDIT_EVENTS, EEVE_cl_Frame, "Events", 0, 0);
}

#endif /* ACTIVE_EDITORS */
