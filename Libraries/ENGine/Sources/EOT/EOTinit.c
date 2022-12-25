/*$T EOTinit.c GC! 1.081 04/12/00 10:11:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Initialisations */
#include "Precomp.h"
#include "TABles/TABles.h"
#include "ENGine/Sources/EOT/EOT.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
    Aim:    Init all the tables of a Set of Engine Objects Tables
 =======================================================================================================================
 */
void EOT_SetOfEOT_Init(EOT_tdst_SetOfEOT *pst_SetOfEOT)
{
	TAB_PFtable_Init(&(pst_SetOfEOT->st_Visu), OETVisuSize, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&(pst_SetOfEOT->st_Anims), OETAnimsSize, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&(pst_SetOfEOT->st_Dyna), OETDynaSize, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&(pst_SetOfEOT->st_AI), OETAISize, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&(pst_SetOfEOT->st_ColMap), OETColMapSize, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&(pst_SetOfEOT->st_ZDM), OETZDMSize, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&(pst_SetOfEOT->st_ZDE), OETZDESize, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&(pst_SetOfEOT->st_Events), OETEventsSize, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&(pst_SetOfEOT->st_Hierarchy), OETHierarchySize, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&(pst_SetOfEOT->st_ODE), OETODESize, TAB_Cf_IgnoreHoles);
}

/*
 =======================================================================================================================
    Aim:    Close all the tables of a Set of Engine Objects Tables
 =======================================================================================================================
 */
void EOT_SetOfEOT_Close(EOT_tdst_SetOfEOT *pst_SetOfEOT)
{
	TAB_PFtable_Close(&(pst_SetOfEOT->st_Visu));
	TAB_PFtable_Close(&(pst_SetOfEOT->st_Anims));
	TAB_PFtable_Close(&(pst_SetOfEOT->st_Dyna));
	TAB_PFtable_Close(&(pst_SetOfEOT->st_AI));
	TAB_PFtable_Close(&(pst_SetOfEOT->st_ColMap));
	TAB_PFtable_Close(&(pst_SetOfEOT->st_ZDM));
	TAB_PFtable_Close(&(pst_SetOfEOT->st_ZDE));
	TAB_PFtable_Close(&(pst_SetOfEOT->st_Events));
	TAB_PFtable_Close(&(pst_SetOfEOT->st_Hierarchy));
	TAB_PFtable_Close(&(pst_SetOfEOT->st_ODE));
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
