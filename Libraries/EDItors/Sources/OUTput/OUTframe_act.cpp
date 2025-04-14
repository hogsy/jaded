/*$T OUTframe_act.cpp GC 1.139 04/15/04 10:19:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "OUTframe.h"
#include "OUTframe_act.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDIstrings.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAcheckworld_dlg.h"
#include "DIAlogs/DIAsecto_dlg.h"
#include "Res/Res.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "OUTstrings.h"
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORcheck.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/GRId/GRI_save.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_display.h"
#include "ENGine/Sources/COLlision/COLsave.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/ENGvars.h"
#include "OUTmsg.h"
#include "BIGfiles/BIGkey.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKstruct.h"
#include "EDIapp.h"
#include "SOFT/SOFThelper.h"
#include "SELection/SELection.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "GEOmetric/GEOdebugobject.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/PreFab/PFBframe.h"
#include "DIAlogs/DIAgamematerial_dlg.h"
#include "DIAlogs/DIAgamematerial_inside.h"
#include "DIAlogs/DIAselection_dlg.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAgaoinfo_dlg.h"
#include "DIAlogs/DIAwpshowstatus_dlg.h"
#include "DIAlogs/DIAsecto_dlg.h"
#ifdef JADEFUSION
#include "DIAlogs/DIAreplace_dlg.h"
#include "DIAlogs/DIAcubemapgen_dlg.h"
#include "DIAlogs/DIAScreenshotSeqBuilder_dlg.h"
#endif
#include "ANImation/ANIinit.h"
#include "SOuNd/SONutil.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"


#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORaccess.h"

#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOstaticLOD.h"

#ifdef Active_CloneListe
#include "ENGine/Sources/OBJects/OBJinit.h"
#endif

#ifdef JADEFUSION
#include "ENGine/Sources/Modifier/MDFmodifier_SoftBody.h"
#include "ENGine/Sources/Modifier/MDFmodifier_Wind.h"
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeGDInterface.h"
#include "XenonGraphics/XeScreenshotSeqBuilder.h"

#if defined(ACTIVE_EDITORS)
#include "GraphicDK/Sources/GEOmetric/GEOstaticLOD.h"
#endif
#endif

extern BOOL			EBRO_gb_CanRefresh;
extern BOOL			LINK_gb_EditRefresh;
extern int			LINK_gi_SpeedDraw;


#ifdef JADEFUSION
extern BOOL		OGL_gb_DispStrip;
extern BOOL		OGL_gb_DispLOD;
extern BOOL		ENG_gb_ActiveSectorization;
extern ULONG	GDI_GetInterface(GDI_tdst_DisplayData *);
extern void		GDI_ChangeInterface(GDI_tdst_DisplayData *, ULONG ulNew);
extern BOOL		ENG_gb_DistCompute;
extern BOOL		GRO_gb_ResetGhost;
extern BOOL		OBJ_gb_DebugPhotoMode;
extern BOOL		OBJ_gb_DebugPhotoMode;
extern UCHAR	COL_s_GhostOptimisation;
extern ULONG	g_ul_BIG_SNAPSHOT_COUNTER;
extern BOOL		GRI_gb_Modified;
extern int		GRI_UpdateCompress(struct GRID_tdst_World_ *);
extern void		COL_SaveGameMaterial(COL_tdst_GameMatList *, BIG_KEY);
extern int		WOR_i_GetAllChildsOf(WOR_tdst_World	*, OBJ_tdst_GameObject *, OBJ_tdst_GameObject **, BOOL);
extern BOOL		COL_gb_SoundColorDisplay;
extern BOOL		OBJ_gb_SaveDuplicateRLI;
#else
extern BOOL		OGL_gb_DispStrip;
extern "C" BOOL		OGL_gb_DispLOD;
extern "C" BOOL		ENG_gb_ActiveSectorization;
extern "C" ULONG	GDI_GetInterface(GDI_tdst_DisplayData *);
extern "C" void		GDI_ChangeInterface(GDI_tdst_DisplayData *, ULONG ulNew);
extern "C" BOOL		ENG_gb_DistCompute;
extern "C" BOOL		GRO_gb_ResetGhost;
extern "C" BOOL		OBJ_gb_DebugPhotoMode;
extern "C" BOOL		OBJ_gb_DebugPhotoMode;
extern "C" UCHAR	COL_s_GhostOptimisation;
extern "C" ULONG	g_ul_BIG_SNAPSHOT_COUNTER;
extern "C" BOOL		GRI_gb_Modified;
extern "C" int		GRI_UpdateCompress(struct GRID_tdst_World_ *);
extern "C" void		COL_SaveGameMaterial(COL_tdst_GameMatList *, BIG_KEY);
extern "C" int		WOR_i_GetAllChildsOf(WOR_tdst_World	*, OBJ_tdst_GameObject *, OBJ_tdst_GameObject **, BOOL);
extern "C" BOOL		COL_gb_SoundColorDisplay;
extern "C" BOOL		OBJ_gb_SaveDuplicateRLI;
//extern "C" BOOL     BIG_bSaveBfSize;
#endif //JADEFUSION

EDIA_cl_SectoDialog *gspo_SectoDialog = NULL;
OBJ_tdst_GameObject *OUT_gpst_GORef=NULL;


/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void EOUT_DisplayCollisionOption(char *asz_Tmp, ULONG _ul_Flags)
{
	sprintf(asz_Tmp, "Display Options: ");
	if(_ul_Flags & GDI_Cul_DF_ShowZDM) strcat(asz_Tmp, "ZDM ");
	if(_ul_Flags & GDI_Cul_DF_ShowZDE) strcat(asz_Tmp, "ZDE ");
	if(_ul_Flags & GDI_Cul_DF_ShowCOB) strcat(asz_Tmp, "REC");
	LINK_PrintStatusMsg(asz_Tmp);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::UpdateShowAiVectors()
{
	WOR_tdst_World		*pst_World;
	TAB_tdst_PFelem		*pst_CurrentElemW;
	TAB_tdst_PFelem		*pst_EndElemW;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;

    pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
    pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
    for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
    {
        pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
        if(!TAB_b_IsAHole(pst_World))
        {
            pst_AIEOT = &(pst_World->st_AllWorldObjects);
            pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
            pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
            for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
            {
                pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
                if(TAB_b_IsAHole(pst_GAO)) continue;

                BOOL bShowAiVectors;

                if (DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowAIDebugVectors)
                {
                    if (DDD()->b_DisplayDebugSelected)
                    {
                        if ((DW() == NULL) || (SEL_b_IsEmpty(DW()->pst_Selection)))
                            bShowAiVectors = FALSE;
                        else
                        {
                            bShowAiVectors = FALSE;

                            SEL_tdst_SelectedItem	*pst_Item = SEL_pst_GetFirst(DW()->pst_Selection, SEL_C_SIF_Object);

                            while (pst_Item )
                            {
                        		OBJ_tdst_GameObject *pst_GO = (OBJ_tdst_GameObject *) pst_Item->p_Content;
                                if (pst_GO == pst_GAO)
                                {
                                    bShowAiVectors = TRUE;
                                    break;
                                }
                        		pst_Item = SEL_pst_GetNext(DW()->pst_Selection, pst_Item, SEL_C_SIF_Object);
                            }
                        }
                    }
                    else
                        bShowAiVectors = TRUE;
                }
                else
                    bShowAiVectors = FALSE;

                pst_GAO->b_DontDisplayDebug = !bShowAiVectors;
            }
        }
    }
}

void EOUT_cl_Frame::SaveWorld(ULONG _ul_Action, bool onlySelected)
{
    //BIG_bSaveBfSize = FALSE;
    if(DP()->mb_GridEdit)
    {
        if(DW()->pst_Grid1) GRI_SaveGrid(DW()->pst_Grid1);
        if(DW()->pst_Grid) GRI_SaveGrid(DW()->pst_Grid);
        LINK_PrintStatusMsg("Grid is saved");
        M_MF()->FlashJade();
    }
    else
    {
        if(GRI_gb_Modified)
        {
            if
                (
                MessageBox
                (
                "Grid(s) has been changed. Do you want to update them ?",
                "Please confirm",
                MB_YESNO
                ) == IDYES
                )
            {
                if(DW()->pst_Grid) GRI_UpdateCompress(DW()->pst_Grid);
                if(DW()->pst_Grid1) GRI_UpdateCompress(DW()->pst_Grid1);
            }

            GRI_gb_Modified = FALSE;
        }

        if(DW()->pst_Grid) GRI_SaveGrid(DW()->pst_Grid);
        if(DW()->pst_Grid1) GRI_SaveGrid(DW()->pst_Grid1);

        if
            (
            DP()->mpo_GameMaterialDialog
            &&	DP()->mpo_GameMaterialDialog->mpo_View
            &&	DP()->mpo_GameMaterialDialog->mpo_View->mpst_GameMatList
            )
        {
            COL_SaveGameMaterial(DP()->mpo_GameMaterialDialog->mpo_View->mpst_GameMatList, BIG_C_InvalidIndex);
        }

        mb_LockUpdate = TRUE;
        M_MF()->SendMessageToEditors(EOUT_MESSAGE_SAVEWORLD, (ULONG) DW(), 0);
        EBRO_gb_CanRefresh = FALSE;
        DP()->SaveWorld(onlySelected);
        M_MF()->FatHasChanged();
        EBRO_gb_CanRefresh = TRUE;
        M_MF()->DataHasChanged();
        M_MF()->FlashJade();
    }
    if (_ul_Action == EOUT_ACTION_SAVEWORLDDUPLICATERLI )
        OBJ_gb_SaveDuplicateRLI = 0;
}

void EOUT_cl_Frame::OnAction(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	cl_ChooseFile(EOUT_STR_Csz_TitleExportMad, 0, FALSE);
	CString				o_Temp;
	char				sz_FileName[_MAX_PATH], *psz_EndPath;
	WOR_tdst_World		*pst_World;
	BIG_KEY				ul_Key;
	BIG_INDEX			ul_Index;
	OBJ_tdst_GameObject *pst_GO, *pst_Father;
	OBJ_tdst_GameObject *ago[100];
	COL_tdst_Cob		*pst_Cob;
	int					numago;
	EDIA_cl_NameDialog	o_Dlg("Enter Grid Size");
	EDIA_cl_NameDialog	o_Dlg1("Enter Value");
	EDIA_cl_NameDialog	o_Dlg2("Enter Focale");
	char				asz_Tmp[100];
	float				f_Value;
	int					cx, cy;
	CRect				o_Rect;
	ULONG				i, j;
	GDI_tdst_Resolution ast_Res[1] = { { 640, 480, 32, 60 } };
	TAB_tdst_PFelem		*pst_CurrentElemW;
	TAB_tdst_PFelem		*pst_EndElemW;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	char				*pz;

#ifdef JADEFUSION
#if defined(ACTIVE_EDITORS)
	std::string			strName;
	std::vector<BIG_INDEX>	 vGROIndex;
	std::vector<std::string> vFiles;
#endif
#endif

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_ul_Action)
	{

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    World Menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_NEWWORLD:
		DP()->NewWorld();
		M_MF()->FatHasChanged();
		M_MF()->DataHasChanged();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EOUT_ACTION_SAVEWORLDDUPLICATERLI:
		if ( MessageBox( "Are you sure (this will duplicate RLI file that are not in world directory ?)", "Please confirm", MB_YESNO ) != IDYES )
			break;
		OBJ_gb_SaveDuplicateRLI = 1;
		
	case EOUT_ACTION_SAVEWORLD:
        SaveWorld(_ul_Action, false);
		break;
		
	// showin added save only selected option
	case EOUT_ACTION_SAVEWORLDONLYSELECTED:
		SaveWorld(_ul_Action, true);
		break;

    case EOUT_ACTION_AUTOSAVEWORLD:
        break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_DISPSTRIP:
		OGL_gb_DispStrip = OGL_gb_DispStrip ? FALSE : TRUE;
		mst_Ini.DispStrips = mst_Ini.DispStrips ? FALSE : TRUE;
		LINK_Refresh();
		break;

	case EOUT_ACTION_DISPLOD:
		OGL_gb_DispLOD = OGL_gb_DispLOD ? FALSE : TRUE;
		mst_Ini.DispLOD = mst_Ini.DispLOD ? FALSE : TRUE;
		LINK_Refresh();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_EXPORTONLYSELECTED:
		mst_Ini.uc_ExportOnlySelection = !mst_Ini.uc_ExportOnlySelection;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_EXPORTTEXTURE:
		mst_Ini.uc_ExportTexture = !mst_Ini.uc_ExportTexture;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_EXPORTDIR:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_FileDialog	o_Dlg("Choose export dir", 2, FALSE);
			char				*pc_Cur;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(o_Dlg.DoModal() == IDOK)
			{
				/*
				 * Get path name £
				 * h_Item = mpo_TreeCtrl->GetSelectedItem(); ERR_X_Assert(h_Item != NULL);
				 * BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(h_Item), asz_Path);
				 */
				L_strcpy(DP()->msz_ExportDir, (char *) (LPCSTR) o_Dlg.masz_FullPath);

				pc_Cur = DP()->msz_ExportDir;
				while(*pc_Cur)
				{
					if(*pc_Cur == '\\') *pc_Cur = '/';
					pc_Cur++;
				}

				if((pc_Cur > DP()->msz_ExportDir) && (pc_Cur[-1] == '/')) pc_Cur[-1] = 0;
			}
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_EXPORTWORLDTOMAD:
		{
			if(cl_ChooseFile.DoModal() == IDOK)
			{
				L_strcpy(sz_FileName, cl_ChooseFile.masz_FullPath);
				psz_EndPath = sz_FileName + L_strlen(sz_FileName);
				if(*(psz_EndPath - 1) != '/') *psz_EndPath++ = '/';

				cl_ChooseFile.GetItem(cl_ChooseFile.mo_File, 1, o_Temp);
				strcpy(psz_EndPath, (char *) (LPCSTR) o_Temp);

				psz_EndPath = strrchr(sz_FileName, '.');
				if(!psz_EndPath || (L_stricmp(psz_EndPath, ".mad")))
				{
					psz_EndPath = sz_FileName + L_strlen(sz_FileName);
					strcpy(psz_EndPath, ".mad");
				}

				DP()->ExportWorldToMad(sz_FileName, FALSE);
			}
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_EXPORTWORLDTOMADSKIN:
		{
			if(cl_ChooseFile.DoModal() == IDOK)
			{
				L_strcpy(sz_FileName, cl_ChooseFile.masz_FullPath);
				psz_EndPath = sz_FileName + L_strlen(sz_FileName);
				if(*(psz_EndPath - 1) != '/') *psz_EndPath++ = '/';

				cl_ChooseFile.GetItem(cl_ChooseFile.mo_File, 1, o_Temp);
				strcpy(psz_EndPath, (char *) (LPCSTR) o_Temp);

				psz_EndPath = strrchr(sz_FileName, '.');
				if(!psz_EndPath || (L_stricmp(psz_EndPath, ".mad")))
				{
					psz_EndPath = sz_FileName + L_strlen(sz_FileName);
					strcpy(psz_EndPath, ".mad");
				}

				DP()->ExportWorldToMad(sz_FileName, TRUE);
			}
		}
		break;


	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_EXPORTWORLDTOASSOCIATEDMAD:
		DP()->ExportWorldToMad(DP()->msz_AssociatedMadFile, FALSE);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_EXPORTQUICK:
		i = mst_Ini.uc_ExportOnlySelection;
		mst_Ini.uc_ExportOnlySelection = 1;
		snprintf( DP()->msz_AssociatedMadFile, sizeof( DP()->msz_AssociatedMadFile ), "%s/QuickExport.mad", DP()->msz_ExportDir );
		DP()->ExportWorldToMad(DP()->msz_AssociatedMadFile, true);
		mst_Ini.uc_ExportOnlySelection = (UCHAR) i;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_CLOSEWORLD:
		CloseWorld();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_WORLD_CHECK:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_CheckWorld	*po_Dlg;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			po_Dlg = new EDIA_cl_CheckWorld(DW());
#ifdef JADEFUSION
			po_Dlg->mpo_View = DP();
#endif
			po_Dlg->DoModeless();
			po_Dlg->CenterWindow();
			po_Dlg->ShowWindow(SW_SHOW);
#ifdef JADEFUSION
			DP()->mpo_CheckWorldDialog = po_Dlg;
#endif
			break;
		}
#ifdef JADEFUSION
    case EOUT_ACTION_LOG_LIGHT_USAGE:
        {
            DDD()->bLogLightUsage = true;
            break;
        }

    case EOUT_ACTION_PACKBASICWORLDS:
        PackBasicWorlds();
        break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_IMPORT_GRO_MTL_ASSOCIATION:
		{
			if(!DW())
				break;

			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_FileDialog	o_File("Choose the file to import", 3, 0, 0, NULL);//, "*.htm");
			CString				o_file;

			EDIA_cl_NameDialog  o_NameDlg("Separate prefixes with ' , '");

			CRect			o_Rect;
			char*			sz_PrefixeList;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			//we want prefixe filtering
			if(o_NameDlg.DoModal() == IDOK)
			{
				sz_PrefixeList = (char*)(LPCSTR) o_NameDlg.mo_Name;
			}
			else
			{
				sz_PrefixeList = NULL;
			}

			if(o_File.DoModal() == IDOK)
			{
				o_file = CString(o_File.masz_FullPath) + CString("/") + o_File.mo_File.MakeLower();
				o_file.Replace(".bin/", ".bin");

				mpo_MyView->GetWindowRect( &o_Rect );

				DP()->ImportGRO_MTLassociation((char*)(LPCSTR) o_file, &o_Rect, sz_PrefixeList);
			}

			break;
		}

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_EXPORT_GRO_MTL_ASSOCIATION:
		{
			if(!DW())
				break;

			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_FileDialog	o_File("Enter the file name", 1);
			EDIA_cl_NameDialog  o_NameDlg("Separate prefixes with ' , '");
			CString				strFileName;
			char				asz_FullPath[BIG_C_MaxLenPath];
			char				*psz_Temp;
			char*				sz_PrefixeList;
			char				sz_test[] = "test";
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			//we want prefixe filtering
			if(o_NameDlg.DoModal() == IDOK)
			{
				sz_PrefixeList = (char*)(LPCSTR) o_NameDlg.mo_Name;
			}
			else
			{
				sz_PrefixeList = NULL;
			}

			if(o_File.DoModal() == IDOK)
			{
				/* Get file name */
				o_File.GetItem(o_File.mo_File, 1, strFileName);
				psz_Temp = (char *) (LPCSTR) strFileName;

				/* Compute full name on disk of new file */
				L_strcpy(asz_FullPath, o_File.masz_FullPath);
				if(asz_FullPath[L_strlen(asz_FullPath) - 1] != '\\') L_strcat(asz_FullPath, "\\");
				L_strcat(asz_FullPath, psz_Temp);

				/* Check if file already exists */
				if(!L_access(asz_FullPath, 0))
					if(MessageBox(EDI_STR_Csz_ConfirmOverwrite, EDI_STR_Csz_TitleConfirm, MB_YESNO | MB_ICONQUESTION) == IDNO)
						return;

				DP()->ExportGRO_MTLassociation(asz_FullPath, sz_PrefixeList);
				
			}
			break;
		}
#endif

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_ENGINEDISPLAY:
		mst_Ini.b_EngineDisplay = mst_Ini.b_EngineDisplay ? FALSE : TRUE;
		OnSetMode();
		if(DW())
		{
			mul_CurrentWorld = LOA_ul_SearchKeyWithAddress((ULONG) DW());
			ERR_X_Assert(mul_CurrentWorld != BIG_C_InvalidIndex);
			mul_CurrentWorld = BIG_ul_SearchKeyToFat(mul_CurrentWorld);
			ERR_X_Assert(mul_CurrentWorld != BIG_C_InvalidIndex);
			M_MF()->SendMessageToLinks(this, EOUT_MESSAGE_CREATEWORLD, mul_CurrentWorld, 0);
		}
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Display menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_DISPLAYCAMERAS:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayCamera)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayCamera;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayCamera;
		break;

	case EOUT_ACTION_DISPLAYSOUND:
        DDD()->ul_DisplayFlags ^= GDI_Cul_DF_DisplaySnd;
        break;

	case EOUT_ACTION_DISPLAYSOUNDOPT:
		ESON_OnDisplayFilter(NULL);
		break;

	case EOUT_ACTION_DISPLAYCURVE:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DrawCurve)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DrawCurve;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_DrawCurve;
		break;

	case EOUT_ACTION_DISPLAYSKELOBBOX:
		if(DW() == NULL) break;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) break;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO) break;
		if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) break;

		if
		(
			pst_GO->pst_Base
		&&	pst_GO->pst_Base->pst_GameObjectAnim
		&&	pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
		)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			OBJ_tdst_Group		*pst_Skeleton;
			TAB_tdst_PFelem		*pst_CurrentBone, *pst_EndBone;
			OBJ_tdst_GameObject *pst_BoneGO;
			int					i;
			BOOL				b_First, b_Display;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			b_First = TRUE;

			pst_Skeleton = pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;

			pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
			pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
			for(i = 0; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++, i++)
			{
				pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
				if(TAB_b_IsAHole(pst_BoneGO)) continue;

				if(b_First)
				{
					b_Display = !(pst_BoneGO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV);
					b_First = FALSE;
				}

				if(b_Display && (pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_OBBox))
					pst_BoneGO->ul_EditorFlags |= OBJ_C_EditFlags_ShowBV + OBJ_C_EditFlags_ShowAltBV;
				else
				{
					pst_BoneGO->ul_EditorFlags &= ~OBJ_C_EditFlags_ShowBV;
					pst_BoneGO->ul_EditorFlags &= ~OBJ_C_EditFlags_ShowAltBV;
				}

			}
		}
		break;

	case EOUT_ACTION_DISPLAYSKELETON:
		if(DW() == NULL) return;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO) return;
		pst_Father = ANI_pst_GetReference(pst_GO);
		if(!(pst_Father->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) return;

		if(pst_Father->ul_EditorFlags & OBJ_C_EditFlags_DrawSkeleton)
		{
			pst_Father->ul_EditorFlags &= ~OBJ_C_EditFlags_DrawSkeleton;
			if(DP()->mb_AnimOn) DP()->Selection_Anim();
			DP()->Selection_GAO_HideGizmo();
		}
		else
		{
			pst_Father->ul_EditorFlags |= OBJ_C_EditFlags_DrawSkeleton;
			DP()->Selection_GAO_ViewGizmo();
			if(!(DP()->mb_AnimOn)) DP()->Selection_Anim();
		}
		break;

	case EOUT_ACTION_DISPLAYSKIN:
		if(DW() == NULL) return;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO) return;
		pst_Father = ANI_pst_GetReference(pst_GO);
		if(!(pst_Father->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) return;

		if(pst_Father->ul_EditorFlags & OBJ_C_EditFlags_HideSkin)
			pst_Father->ul_EditorFlags &= ~OBJ_C_EditFlags_HideSkin;
		else
			pst_Father->ul_EditorFlags |= OBJ_C_EditFlags_HideSkin;
		break;

	case EOUT_ACTION_DISPLAYLIGHTS:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayLight)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayLight;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayLight;
		break;

	case EOUT_ACTION_DISPLAYWP:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypoint)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayWaypoint;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayWaypoint;
		break;

	case EOUT_ACTION_DISPLAYWPINNETWORK:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypointInNetwork)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayWaypointInNetwork;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayWaypointInNetwork;
		break;

	case EOUT_ACTION_DISPLAYWPASBV:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_WaypointBoundingVolume)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_WaypointBoundingVolume;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_WaypointBoundingVolume;
		break;

	case EOUT_ACTION_DISPLAYINVISIBLE:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayInvisible;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayInvisible;
		break;

	case EOUT_ACTION_DISPLAYUNDERCOL:
	case EOUT_ACTION_DISPLAYINTFGAO:
		{
			/*~~~~~*/
			int test;
			/*~~~~~*/

			pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
			pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
			for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
			{
				pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
				if(!TAB_b_IsAHole(pst_World))
				{
					pst_AIEOT = &(pst_World->st_AllWorldObjects);
					pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
					pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
					for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
					{
						pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
						if(TAB_b_IsAHole(pst_GAO)) continue;
						if(!pst_GAO->pst_Base) continue;
						if(!pst_GAO->pst_Base->pst_Visu) continue;
						if(!pst_GAO->sz_Name) continue;
						if(_ul_Action == EOUT_ACTION_DISPLAYINTFGAO)
							test = (_strnicmp(pst_GAO->sz_Name, "INTF", 4) == 0);
						else
						{
							pz = L_strrchr(pst_GAO->sz_Name, '_');
							if(!pz) continue;
							test = L_strcmpi(pz, "_COL.gao") == 0;
						}

						if(test)
						{
							if(pst_GAO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Draw)
								pst_GAO->pst_Base->pst_Visu->ul_DrawMask &= ~GDI_Cul_DM_Draw;
							else
								pst_GAO->pst_Base->pst_Visu->ul_DrawMask |= GDI_Cul_DM_Draw;
						}
					}
				}
			}
		}

		LINK_Refresh();
		break;

	case EOUT_ACTION_DISPLAYNOFOG:
		if(DDD()->ul_DrawMask & GDI_Cul_DM_Fogged)
			DDD()->ul_DrawMask &= ~GDI_Cul_DM_Fogged;
		else
			DDD()->ul_DrawMask |= GDI_Cul_DM_Fogged;
		break;
	
	case EOUT_ACTION_DISPLAYNOAUTOCLONE:
		if(DDD()->ul_DrawMask & GDI_Cul_DM_NoAutoClone)
		{
			DDD()->ul_DrawMask &= ~GDI_Cul_DM_NoAutoClone;
#ifdef Active_CloneListe
			OBJ_GameObject_UpdateCloneListe();
#endif
		}
		else
			DDD()->ul_DrawMask |= GDI_Cul_DM_NoAutoClone;
			break;

#ifdef JADEFUSION
    case EOUT_ACTION_FORCEMAXDYNLIGHTS:
        if(DDD()->ul_DisplayFlags & GDI_Cul_DF_ForceMaxDynLights)
            DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ForceMaxDynLights;
        else
            DDD()->ul_DisplayFlags |= GDI_Cul_DF_ForceMaxDynLights;
        break;

    case EOUT_ACTION_ENABLEELEMENTCULLING:
        DDD()->uc_EnableElementCulling ^= 1;
        break;
#endif

	case EOUT_ACTION_DISPLAYVISIBLE:
		if(DDD()->ul_DrawMask & GDI_Cul_DM_Draw)
			DDD()->ul_DrawMask &= ~GDI_Cul_DM_Draw;
		else
			DDD()->ul_DrawMask |= GDI_Cul_DM_Draw;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_FORCEDT:
		switch(DDD()->uc_ForceNumFrames)
		{
		case 0:
			DDD()->uc_ForceNumFrames = 1;
			break;

		case 1:
			DDD()->uc_ForceNumFrames = 2;
			break;

		case 2:
			DDD()->uc_ForceNumFrames = 3;
			break;


		case 3:
			DDD()->uc_ForceNumFrames = 0;
			break;
		}

		EDI_go_TheApp.OnIdle(0);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_SHOWAIVECTORS:
        DDD()->ul_DisplayFlags |= GDI_Cul_DF_ShowAIDebugVectors;
        DDD()->b_DisplayDebugSelected = FALSE;
        UpdateShowAiVectors();
        break;

    case EOUT_ACTION_SHOWNOAIVECTORS:
        DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowAIDebugVectors;
        UpdateShowAiVectors();
        break;

    case EOUT_ACTION_SHOWSELECTEDAIVECTORS:
        DDD()->ul_DisplayFlags |= GDI_Cul_DF_ShowAIDebugVectors;
        DDD()->b_DisplayDebugSelected = TRUE;
        UpdateShowAiVectors();
        break;

    case EOUT_ACTION_PHOTOMODE:
		if(OBJ_gb_DebugPhotoMode)
			OBJ_gb_DebugPhotoMode = FALSE;
		else
			OBJ_gb_DebugPhotoMode = TRUE;
		break;

	case EOUT_ACTION_SECTORISATION:
		if(ENG_gb_ActiveSectorization)
			ENG_gb_ActiveSectorization = FALSE;
		else
			ENG_gb_ActiveSectorization = TRUE;
		LINK_Refresh();
		break;

	case EOUT_ACTION_ZONE_GMATSOUND:
		COL_gb_SoundColorDisplay = COL_gb_SoundColorDisplay ? FALSE : TRUE;
		if(DP()->mpo_GameMaterialDialog)
		{
			DP()->mpo_GameMaterialDialog->mpo_View->SetDlgItemText(IDC_CHECK_SOUND, COL_gb_SoundColorDisplay ? "Sound Color" : "ID Color");
		}
		break;

	case EOUT_ACTION_ZONE_SHOWOPTIM:
		if(COL_s_GhostOptimisation)
			COL_s_GhostOptimisation = 0;
		else
			COL_s_GhostOptimisation = 1;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_SECTO:
		if(!gspo_SectoDialog)
		{
			gspo_SectoDialog = new EDIA_cl_SectoDialog(DW());
			gspo_SectoDialog->DoModeless();
		}
		break;

	case EOUT_ACTION_DISPLAYPOSTIT:
		if(DDD()->uc_DrawPostIt)
			DDD()->uc_DrawPostIt = 0;
		else
			DDD()->uc_DrawPostIt = 1;
		break;

	case EOUT_ACTION_DISPLAYZDM:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowZDM)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowZDM;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_ShowZDM;
		EOUT_DisplayCollisionOption(asz_Tmp, DDD()->ul_DisplayFlags);
		break;

	case EOUT_ACTION_DISPLAYZDE:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowZDE)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowZDE;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_ShowZDE;
		EOUT_DisplayCollisionOption(asz_Tmp, DDD()->ul_DisplayFlags);
		break;

	case EOUT_ACTION_DISPLAYCOB:
 		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowCOB)
		{
			if(DDD()->uc_ColMapDisplayMode & 0x80)
			{
				DDD()->uc_ColMapDisplayMode &= ~0x80;
				DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowCOB;
			}
			else
			{
				DDD()->uc_ColMapDisplayMode |= 0x80;
			}
		}
		else
		{
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_ShowCOB;
		}

		EOUT_DisplayCollisionOption(asz_Tmp, DDD()->ul_DisplayFlags);
		break;


	case EOUT_ACTION_SHOWSLOPE:
		if(DDD()->uc_ColMapShowSlope == 1)
			DDD()->uc_ColMapShowSlope = 0;
		else
			DDD()->uc_ColMapShowSlope = 1;
		break;

	case EOUT_ACTION_SHOWSLOPE45:
		if(DDD()->uc_ColMapShowSlope == 2)
			DDD()->uc_ColMapShowSlope = 0;
		else
			DDD()->uc_ColMapShowSlope = 2;
		break;


	case EOUT_ACTION_DISPLAYCOBMODE:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowCOB)
		{
			switch(DDD()->uc_ColMapDisplayMode & 0x7F)
			{
			case 0:
				COL_gb_SoundColorDisplay = FALSE;
				DDD()->uc_ColMapDisplayMode &= ~0x7F;
				DDD()->uc_ColMapDisplayMode |= 1;
				LINK_PrintStatusMsg("Camera Faces");
				break;

			case 1:
				COL_gb_SoundColorDisplay = FALSE;
				DDD()->uc_ColMapDisplayMode &= ~0x7F;
				DDD()->uc_ColMapDisplayMode |= 2;
				LINK_PrintStatusMsg("No Camera Faces");
				break;

			case 2:
				COL_gb_SoundColorDisplay = FALSE;
				DDD()->uc_ColMapDisplayMode &= ~0x7F;
				DDD()->uc_ColMapDisplayMode |= 3;
				LINK_PrintStatusMsg("No FlagX and No Camera Faces");
				break;

			case 3:
				COL_gb_SoundColorDisplay = FALSE;
				DDD()->uc_ColMapDisplayMode &= ~0x7F;
				DDD()->uc_ColMapDisplayMode |= 4;
				LINK_PrintStatusMsg("All ColMap Faces - NO ODE ");
				break;

			case 4:
				COL_gb_SoundColorDisplay = FALSE;
				DDD()->uc_ColMapDisplayMode &= ~0x7F;
				LINK_PrintStatusMsg("All ColMap Faces - ODE");
				break;



			default:
				DDD()->uc_ColMapDisplayMode &= ~0x7F;
				LINK_PrintStatusMsg("All ColMap Faces");
				break;
			}
		}
		break;

	case EOUT_ACTION_DISPLAYGRID:
		if(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show)
		{
			DDD()->pst_GridOptions->ul_Flags &= ~GRID_Cul_DO_Show;
			if(DP()->Grid_b_InEdit() && !(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1))
				DP()->Grid_EditSwap();
		}
		else
			DDD()->pst_GridOptions->ul_Flags |= GRID_Cul_DO_Show;
		break;

	case EOUT_ACTION_DISPLAYGRID1:
		if(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1)
		{
			DDD()->pst_GridOptions->ul_Flags &= ~GRID_Cul_DO_Show1;
			if(DP()->Grid_b_InEdit() && !(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show)) DP()->Grid_EditSwap();
		}
		else
			DDD()->pst_GridOptions->ul_Flags |= GRID_Cul_DO_Show1;
		break;

	case EOUT_ACTION_DISPLAYFORCEDCOLOR:
		if(DDD()->ul_DrawMask & GDI_Cul_DM_DontForceColor)
			DDD()->ul_DrawMask &= ~GDI_Cul_DM_DontForceColor;
		else
			DDD()->ul_DrawMask |= GDI_Cul_DM_DontForceColor;
		SPG2_Modifier_FlushAll();
		break;

#ifdef JADEFUSION
    case EOUT_ACTION_DISPLAY_FORCE_VERTEX_COLOR:
        DDD()->b_ShowVertexColor = !DDD()->b_ShowVertexColor;
        break;

    case EOUT_ACTION_DISPLAY_DISABLE_BASE_MAP:
        DDD()->b_DisableBaseMap = !DDD()->b_DisableBaseMap;
        break;

    case EOUT_ACTION_DISPLAY_DISABLE_MOSS:
        DDD()->b_DisableMoss = !DDD()->b_DisableMoss;
        break;

    case EOUT_ACTION_DISPLAY_DISABLE_LIGHTMAPS:
        DDD()->b_DisableLightmap = !DDD()->b_DisableLightmap;
        break;


    case EOUT_ACTION_DISPLAY_UNLOAD_ALL_SHADERS:
#if defined(_XENON_RENDER)
        extern void Xe_ResetAllShaders();
        Xe_ResetAllShaders();
#endif
        break;
#endif //JADEFUSION

	case EOUT_ACTION_DISPLAYPROPORTIONNAL:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_Proportionnal)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_Proportionnal;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_Proportionnal;
		break;

	case EOUT_ACTION_DISPLAYWIREFRAMED:
		{
			DDD()->ul_WiredMode &= 3;
			DDD()->ul_WiredMode++;
			DDD()->ul_WiredMode %= 3;
			DDD()->ul_WiredMode |= DDD()->ul_WiredMode << 8;
		}
		break;
#ifndef JADEFUSION //POPOWARNING
	case EOUT_ACTION_DISPLAYFACETMODE:
        {
			if(DDD()->ul_DrawMask & GDI_Cul_DM_NoFacetMode) {
                DDD()->ul_DrawMask &= ~GDI_Cul_DM_NoFacetMode;
				if(!(DDD()->ul_DrawMask & GDI_Cul_DM_NoShowRLIPlaceMode))
					DDD()->ul_DrawMask |= GDI_Cul_DM_NoShowRLIPlaceMode;
			} else {
                DDD()->ul_DrawMask |= GDI_Cul_DM_NoFacetMode;
			}
            break;
        }
	case EOUT_ACTION_DISPLAYRLIPLACEMODE:
        {
			if(DDD()->ul_DrawMask & GDI_Cul_DM_NoShowRLIPlaceMode) {
                DDD()->ul_DrawMask &= ~GDI_Cul_DM_NoShowRLIPlaceMode;
				if (!(DDD()->ul_DrawMask & GDI_Cul_DM_NoFacetMode))
					DDD()->ul_DrawMask |= GDI_Cul_DM_NoFacetMode;
			} else {
                DDD()->ul_DrawMask |= GDI_Cul_DM_NoShowRLIPlaceMode;
			}
            break;
        }
#endif
    case EOUT_ACTION_DISPLAYHISTOGRAMM:
		{
			if (DDD()->ShowHistogramm)
				DDD()->ShowHistogramm = 0;
			else
				DDD()->ShowHistogramm = TRUE;
		}
		break;

	case EOUT_ACTION_SHADESELECTED:
		DDD()->ul_WiredMode = 0;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_ShadeSelected)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShadeSelected;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_ShadeSelected;
		break;

	case EOUT_ACTION_DISPLAYHIERARCHY:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayHierarchy)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayHierarchy;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayHierarchy;
		break;

	case EOUT_ACTION_DISPLAYOK3:
		if(DDD()->ul_DisplayFlags & GDI_cul_DF_DisplayOK3)
			DDD()->ul_DisplayFlags &= ~GDI_cul_DF_DisplayOK3;
		else
			DDD()->ul_DisplayFlags |= GDI_cul_DF_DisplayOK3;
		break;

	case EOUT_ACTION_SELECTBONE:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_SelectBone)
			DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_SelectBone;
		else
			DDD()->ul_DisplayFlags |= GDI_Cul_DF_SelectBone;
		break;

	case EOUT_ACTION_SELECTPREFAB:
		if(mst_Ini.ul_SelFlags & EOUT_SelFlags_Prefab)
			mst_Ini.ul_SelFlags &= ~EOUT_SelFlags_Prefab;
		else
			mst_Ini.ul_SelFlags |= EOUT_SelFlags_Prefab;
		break;

	case EOUT_ACTION_DISPLAYHIDEALL:
		if(DW())
		{
			for(i = 0; i < WOR_C_MaxSecto; i++)
			{
				DW()->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor1;
				DW()->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor2;
				DW()->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor3;
				for(j = 0; j < WOR_C_MaxSectoPortals; j++)
					DW()->ast_AllSectos[i].ast_Portals[j].uw_Flags &= ~WOR_CF_PortalRender;
			}
		}

		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplaySnd;
		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayCamera;
		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayLight;
		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayWaypoint;
		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayWaypointInNetwork;
		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayInvisible;
		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowZDM;
		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowZDE;
		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowCOB;
		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayHierarchy;
		DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowAIDebugVectors;
		DDD()->ul_DisplayFlags &= ~GDI_cul_DF_DisplayOK3;
		DDD()->pst_GridOptions->ul_Flags &= ~GRID_Cul_DO_Show;
		DDD()->pst_GridOptions->ul_Flags &= ~GRID_Cul_DO_Show1;
		DDD()->ul_DrawMask |= GDI_Cul_DM_Draw;
		OBJ_gb_DebugPhotoMode = FALSE;
		if(DW())
		{
			DP()->Selection_b_Treat(NULL, 0, 0, 0);
			OBJ_HideAllBVs(&DW()->st_AllWorldObjects);
		}

		if(DDD()->uc_ForceNumFrames)
		{
			DDD()->uc_ForceNumFrames = 0;
		}
		DDD()->uc_EditBounding = 0;
		if(LINK_gi_SpeedDraw != 0) LINK_gi_SpeedDraw = 0;
		mst_Ini.DispStrips = OGL_gb_DispStrip = FALSE;
		mst_Ini.DispLOD = OGL_gb_DispLOD = FALSE;
		LINK_Refresh();
		DP()->mi_EditedCurve = -1;
		if(DP()->mpst_EditedPortal)
		{
			DP()->mpst_EditedPortal->uw_Flags &= ~WOR_CF_PortalPicked;
			DP()->mpst_EditedPortal = NULL;
		}

		DP()->mi_FreezeHelper = -1;
		COL_s_GhostOptimisation = 0;
		RefreshMenu();
		break;

	case EOUT_ACTION_COPYGAOREF:
		OUT_gpst_GORef = NULL;
		if(DW() == NULL) return;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO) return;
		OUT_gpst_GORef = pst_GO;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_DISPLAYCAPTURENAME:
		cl_ChooseFile.mo_Title = "Choose capture file";
		if(cl_ChooseFile.DoModal() == IDOK)
		{
			L_strcpy(sz_FileName, cl_ChooseFile.masz_FullPath);
			psz_EndPath = sz_FileName + L_strlen(sz_FileName);
			if(*(psz_EndPath - 1) != '\\') *psz_EndPath++ = '\\';

			cl_ChooseFile.GetItem(cl_ChooseFile.mo_File, 1, o_Temp);
			strcpy(psz_EndPath, (char *) (LPCSTR) o_Temp);

			strcpy(DDD()->sz_SnapshotName, sz_FileName);
		}
		break;

	case EOUT_ACTION_DISPLAYCAPTUREONE:
		DDD()->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_Type;
		DDD()->uc_SnapshotFlag |= GDI_Cc_CaptureType_One;
		break;

	case EOUT_ACTION_DISPLAYCAPTUREREALTIME:
		DDD()->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_Type;
		DDD()->uc_SnapshotFlag |= GDI_Cc_CaptureType_Multiple;
		break;

	case EOUT_ACTION_DISPLAYCAPTUREINFINITE:
		DDD()->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_Type;
		DDD()->uc_SnapshotFlag |= GDI_Cc_CaptureType_Infinit;
		break;

	case EOUT_ACTION_DISPLAYCAPTUREBIG:
		DDD()->uc_SnapshotFlag ^= GDI_Cc_CaptureFlag_BIG;
		break;

	case EOUT_ACTION_DISPLAYCAPTURESTARTSTOP:
		if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Running)
		{
			DDD()->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_Running;
			if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureType_Multiple)
			{
//				DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ForceOneTrameEngine;
				DDD()->uc_ForceNumFrames = 0;
			}
		}
		else
		{
			if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureFlag_BIG) g_ul_BIG_SNAPSHOT_COUNTER = 0x800F;
			DDD()->uc_SnapshotFlag |= GDI_Cc_CaptureFlag_Running;
#ifdef JADEFUSION
			if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureFlag_ForSeqBuilder)
			{
				g_oXeScreenshotSeqBuilder.Add(&DDD()->st_Camera.st_Matrix);
				//update list
				DP()->mpo_ScreenshotSeqBuilderDialog->UpdateList();
			}
#endif
			
			DDD()->uc_SnapshotCurrent = 0;
			if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureType_Multiple)
			{
//				DDD()->ul_DisplayFlags |= GDI_Cul_DF_ForceOneTrameEngine;
				DDD()->uc_ForceNumFrames = 1;
			}
		}
		break;

	case EOUT_ACTION_DISPLAYCAPTURESTARTSTOPC:
		if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Running)
		{
			DDD()->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_Running;
			if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureType_Multiple)
			{
//				DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ForceOneTrameEngine;
				DDD()->uc_ForceNumFrames = 0;
			}
		}
		else
		{
			DDD()->uc_SnapshotFlag |= GDI_Cc_CaptureFlag_Running;
			if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureType_Multiple)
			{
//				DDD()->ul_DisplayFlags |= GDI_Cul_DF_ForceOneTrameEngine;
				DDD()->uc_ForceNumFrames = 1;
			}
		}
		break;

	case EOUT_ACTION_DISPLAYWOW:
		DP()->DisplayWow();
		break;

#ifdef JADEFUSION
    case EOUT_ACTION_DISPLAYSOFTBODYPLANE :
        SoftBody_gb_DisplayPlaneCollision = !SoftBody_gb_DisplayPlaneCollision;
        break;

    case EOUT_ACTION_DISPLAY_MDFWIND :
        Wind_gb_Display = !Wind_gb_Display;
        break;
#endif
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Camera menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_ENGINECAM:
		if(DP()->mb_GridEdit && DW())
		{
			DP()->Grid_CopySel(DW());
			DP()->Grid_ResetSel();
		}
		else
		{
			DDD()->uc_EditorCamera = 0;
			DDD()->uc_EngineCamera = DDD()->uc_EngineCamera ? 0 : 1;
			mst_Ini.b_EngineCam = DDD()->uc_EngineCamera;
		}
		break;

	case EOUT_ACTION_EDITORCAM:
		mst_Ini.b_EngineCam = FALSE;
		DDD()->uc_EngineCamera = 0;
		DDD()->uc_EditorCamera = DDD()->uc_EditorCamera ? 0 : 1;
		break;

	case EOUT_ACTION_LOCKTOCAM:
		DDD()->uc_LockObjectToCam = !DDD()->uc_LockObjectToCam;
		if(DDD()->uc_LockObjectToCam) DP()->CameraAsObj();
		break;

	case EOUT_ACTION_CAMASOBJ:
		DP()->CameraAsObj();
		break;

	case EOUT_ACTION_OBJASCAM:
		DP()->ObjAsCamera();
		break;

	case EOUT_ACTION_CREATECAMATCAM:
		if(!(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayCamera)) OnAction(EOUT_ACTION_DISPLAYCAMERAS);
		DP()->CreateCamAtCam();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_CAMERA_TOP:
		DP()->Viewpoint_SetStandard(F3D_StdView_Top);
		break;

	case EOUT_ACTION_CAMERA_BOTTOM:
		DP()->Viewpoint_SetStandard(F3D_StdView_Bottom);
		break;

	case EOUT_ACTION_CAMERA_LEFT:
		DP()->Viewpoint_SetStandard(F3D_StdView_Left);
		break;

	case EOUT_ACTION_CAMERA_RIGHT:
		DP()->Viewpoint_SetStandard(F3D_StdView_Right);
		break;

	case EOUT_ACTION_CAMERA_FRONT:
		DP()->Viewpoint_SetStandard(F3D_StdView_Front);
		break;

	case EOUT_ACTION_CAMERA_BACK:
		DP()->Viewpoint_SetStandard(F3D_StdView_Back);
		break;

	case EOUT_ACTION_CAMERA_GOTOORIGIN:
		DP()->Viewpoint_SetStandard(F3D_StdView_Origin);
		break;

	case EOUT_ACTION_CAMERA_NEXT:
		DP()->Viewpoint_Next();
		break;

	case EOUT_ACTION_ZOOMEXTENDSELECTED:
		if(DDD()->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
		{
			DP()->ZoomExtendSelected(&DDD()->st_Camera, DW());
		}
		break;

	case EOUT_ACTION_ZOOMEXTEND:
		DP()->ZoomExtendAll(&DDD()->st_Camera, DW());
		break;

	case EOUT_ACTION_AUTOZOOMEXTENDONSEL:
		if(mpo_EngineFrame->mpo_DisplayView->mb_AutoZoomExtendOnRotate)
			mpo_EngineFrame->mpo_DisplayView->mb_AutoZoomExtendOnRotate = FALSE;
		else
			mpo_EngineFrame->mpo_DisplayView->mb_AutoZoomExtendOnRotate = TRUE;
		break;

	case EOUT_ACTION_CAMERA_PERSPECTIVE:
		CamIso();
		break;

	case EOUT_ACTION_CONSTANTZOOMSPEED:
		mpo_EngineFrame->mpo_DisplayView->mb_ConstantZoomSpeed = !mpo_EngineFrame->mpo_DisplayView->mb_ConstantZoomSpeed;
		break;

	case EOUT_ACTION_CLOSEVIEW:
		// Verif in case user used keyboard
		if (WOR_gpst_CurrentWorld) {
			if ( WOR_gpst_CurrentWorld->b_IsSplitScreen ) {
				UnSplitViewCloseToObject();
			} else {
				// Verif in case user used keyboard
				if(DW() == NULL) return;
				if(SEL_b_IsEmpty(DW()->pst_Selection)) return;
				SplitViewCloseToObject();
			}
		}
		break;
	
	case EOUT_ACTION_ZOOMSPEED:
		sprintf(asz_Tmp, "%d", mpo_EngineFrame->mpo_DisplayView->mi_ZoomSpeed);
		o_Dlg1.mo_Name = asz_Tmp;
		if(o_Dlg1.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dlg1.mo_Name, "%u", &cy);
			mpo_EngineFrame->mpo_DisplayView->mi_ZoomSpeed = cy;
		}
		break;

	case EOUT_ACTION_CAMERA_FOCAL:
		o_Dlg2.mo_Name.Format("%f", DW()->pst_View[mi_NumEdit].st_DisplayInfo.f_FieldOfVision);
		if(o_Dlg2.DoModal() == IDOK)
		{
			/*~~~~~~*/
			float	f;
			/*~~~~~~*/

			f = (float) atof((char *) (LPCSTR) o_Dlg2.mo_Name);
			DW()->pst_View[mi_NumEdit].st_DisplayInfo.f_FieldOfVision = f;
		}
		break;

	case EOUT_ACTION_CAMERA_WF:
		o_Dlg2.mo_Name.Format("%f", mst_Ini.f_CamWheelFactor);
		if(o_Dlg2.DoModal() == IDOK)
		{
			/*~~~~~~*/
			float	f;
			/*~~~~~~*/

			f = (float) atof((char *) (LPCSTR) o_Dlg2.mo_Name);
			mst_Ini.f_CamWheelFactor = f;
		}
		break;

	case EOUT_ACTION_CAMERA_NEAR:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			CPoint							pt;
			OBJ_tdst_GameObject				*obj;
			SOFT_tdst_PickingBuffer_Pixel	*pst_Pixel;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			GetCursorPos(&pt);
			mpo_EngineFrame->mpo_DisplayView->ScreenToClient(&pt);
			obj = (OBJ_tdst_GameObject *) mpo_EngineFrame->mpo_DisplayView->Pick_l_UnderPoint
				(
					&pt,
					SOFT_Cuc_PBQF_GameObject,
					0
				);
			if(!obj) break;
			pst_Pixel = mpo_EngineFrame->mpo_DisplayView->Pick_pst_GetFirst(SOFT_Cuc_PBQF_GameObject, -1);
			obj = (OBJ_tdst_GameObject *) pst_Pixel->ul_Value;
			obj->ul_EditorFlags |= OBJ_C_EditFlags_Ghost;
		}
		break;

	case EOUT_ACTION_CAMERA_RESETNEAR:
		GRO_gb_ResetGhost = TRUE;
		LINK_Refresh();
		GRO_gb_ResetGhost = FALSE;
		return;


	case EOUT_ACTION_LOGUNCOLLIDABLE:
		DP()->LogUnCollidable();
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Edition menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EOUT_ACTION_UNDO:
		DP()->Undo();
		break;

	case EOUT_ACTION_REDO:
		DP()->Redo();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_SNAPTOGRID:
		if(DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapGrid)
			DDD()->pst_Helpers->ul_Flags &= ~SOFT_Cul_HF_SnapGrid;
		else
			DDD()->pst_Helpers->ul_Flags |= SOFT_Cul_HF_SnapGrid;
		break;

	case EOUT_ACTION_SNAPGRIDSIZEXY:
        {
            BOOL bXYSameAsZ = (DDD()->pst_Helpers->f_GridSizeXY == DDD()->pst_Helpers->f_GridSizeZ);
            sprintf(asz_Tmp, "%f", DDD()->pst_Helpers->f_GridSizeXY);
            o_Dlg.mo_Name = asz_Tmp;
            if(o_Dlg.DoModal() == IDOK)
            {
                sscanf((char *) (LPCSTR) o_Dlg.mo_Name, "%f", &DDD()->pst_Helpers->f_GridSizeXY);
                DDD()->pst_Helpers->f_GridSizeXY = (float) fabs(DDD()->pst_Helpers->f_GridSizeXY);
                if (DDD()->pst_Helpers->f_GridSizeXY < 0)
                    DDD()->pst_Helpers->f_GridSizeXY = 0;

                if (bXYSameAsZ)
                    DDD()->pst_Helpers->f_GridSizeZ = DDD()->pst_Helpers->f_GridSizeXY;

                // On coche automatiquement le snap si on change la taille de grille
                DDD()->pst_Helpers->ul_Flags |= SOFT_Cul_HF_SnapGrid;
            }
        }
		break;

	case EOUT_ACTION_SNAPGRIDSIZEZ:
		sprintf(asz_Tmp, "%f", DDD()->pst_Helpers->f_GridSizeZ);
		o_Dlg.mo_Name = asz_Tmp;
		if(o_Dlg.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dlg.mo_Name, "%f", &DDD()->pst_Helpers->f_GridSizeZ);
			DDD()->pst_Helpers->f_GridSizeZ = (float) fabs(DDD()->pst_Helpers->f_GridSizeZ);
            if (DDD()->pst_Helpers->f_GridSizeZ < 0)
                DDD()->pst_Helpers->f_GridSizeZ = 0;

            // On coche automatiquement le snap si on change la taille de grille
            DDD()->pst_Helpers->ul_Flags |= SOFT_Cul_HF_SnapGrid;
		}
		break;

	case EOUT_ACTION_SNAPGRIDRENDER:
		if(DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapGridRender)
			DDD()->pst_Helpers->ul_Flags &= ~SOFT_Cul_HF_SnapGridRender;
		else
			DDD()->pst_Helpers->ul_Flags |= SOFT_Cul_HF_SnapGridRender;
		break;

	case EOUT_ACTION_SNAPGRIDWIDTH:
		sprintf(asz_Tmp, "%f", DDD()->pst_Helpers->f_GridWidth);
		o_Dlg.mo_Name = asz_Tmp;
		if(o_Dlg.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dlg.mo_Name, "%f", &DDD()->pst_Helpers->f_GridWidth);
			DDD()->pst_Helpers->f_GridWidth = (float) fabs(DDD()->pst_Helpers->f_GridWidth);
		}
		break;

	case EOUT_ACTION_SNAPANGLE:
		if(DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapAngle)
			DDD()->pst_Helpers->ul_Flags &= ~SOFT_Cul_HF_SnapAngle;
		else
			DDD()->pst_Helpers->ul_Flags |= SOFT_Cul_HF_SnapAngle;
		break;

	case EOUT_ACTION_ANGLESTEP:
		f_Value = DDD()->pst_Helpers->f_AngleSnap * Cf_180ByPi;
		sprintf(asz_Tmp, "%g", f_Value);
		o_Dlg.mo_Name = asz_Tmp;
		if(o_Dlg.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dlg.mo_Name, "%f", &f_Value);
			DDD()->pst_Helpers->f_AngleSnap = f_Value * Cf_PiBy180;
		}
		break;

	case EOUT_ACTION_FREEZEHELPER:
		if(DP()->mi_FreezeHelper == -1)
			DP()->mi_FreezeHelper = DP()->mst_WinHandles.pst_DisplayData->pst_Helpers->l_Pickable;
		else
			DP()->mi_FreezeHelper = -1;
		break;

	case EOUT_ACTION_FREEZEHELPERX:
		if(DP()->mi_FreezeHelper == SOFT_Cl_Helpers_ScaleLocalY || DP()->mi_FreezeHelper == SOFT_Cl_Helpers_ScaleLocalZ)
			DP()->mi_FreezeHelper = SOFT_Cl_Helpers_ScaleLocalX;
		if(DP()->mi_FreezeHelper == SOFT_Cl_Helpers_MoveLocalY || DP()->mi_FreezeHelper == SOFT_Cl_Helpers_MoveLocalZ)
			DP()->mi_FreezeHelper = SOFT_Cl_Helpers_MoveLocalX;
		if
		(
			DP()->mi_FreezeHelper == SOFT_Cl_Helpers_RotationLocalY
		||	DP()->mi_FreezeHelper == SOFT_Cl_Helpers_RotationLocalZ
		) DP()->mi_FreezeHelper = SOFT_Cl_Helpers_RotationLocalX;
		break;

	case EOUT_ACTION_FREEZEHELPERY:
		if(DP()->mi_FreezeHelper == SOFT_Cl_Helpers_ScaleLocalX || DP()->mi_FreezeHelper == SOFT_Cl_Helpers_ScaleLocalZ)
			DP()->mi_FreezeHelper = SOFT_Cl_Helpers_ScaleLocalY;
		if(DP()->mi_FreezeHelper == SOFT_Cl_Helpers_MoveLocalX || DP()->mi_FreezeHelper == SOFT_Cl_Helpers_MoveLocalZ)
			DP()->mi_FreezeHelper = SOFT_Cl_Helpers_MoveLocalY;
		if
		(
			DP()->mi_FreezeHelper == SOFT_Cl_Helpers_RotationLocalX
		||	DP()->mi_FreezeHelper == SOFT_Cl_Helpers_RotationLocalZ
		) DP()->mi_FreezeHelper = SOFT_Cl_Helpers_RotationLocalY;
		break;

	case EOUT_ACTION_FREEZEHELPERZ:
		if(DP()->mi_FreezeHelper == SOFT_Cl_Helpers_ScaleLocalY || DP()->mi_FreezeHelper == SOFT_Cl_Helpers_ScaleLocalX)
			DP()->mi_FreezeHelper = SOFT_Cl_Helpers_ScaleLocalZ;
		if(DP()->mi_FreezeHelper == SOFT_Cl_Helpers_MoveLocalY || DP()->mi_FreezeHelper == SOFT_Cl_Helpers_MoveLocalX)
			DP()->mi_FreezeHelper = SOFT_Cl_Helpers_MoveLocalZ;
		if
		(
			DP()->mi_FreezeHelper == SOFT_Cl_Helpers_RotationLocalY
		||	DP()->mi_FreezeHelper == SOFT_Cl_Helpers_RotationLocalX
		) DP()->mi_FreezeHelper = SOFT_Cl_Helpers_RotationLocalZ;
		break;

	case EOUT_ACTION_DISPHELPERTOOLTIP:
		if(DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_HideHelperToolTip)
			DDD()->pst_Helpers->ul_Flags &= ~SOFT_Cul_HF_HideHelperToolTip;
		else
			DDD()->pst_Helpers->ul_Flags |= SOFT_Cul_HF_HideHelperToolTip;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_CAMERASYSTEMAXIS:
		DP()->Helper_SetAxis(SOFT_Cul_HF_SACamera);
		break;

	case EOUT_ACTION_WORLDSYSTEMAXIS:
		DP()->Helper_SetAxis(SOFT_Cul_HF_SAWorld);
		break;

	case EOUT_ACTION_OBJECTSYSTEMAXIS:
		DP()->Helper_SetAxis(SOFT_Cul_HF_SAObject);
		break;

	case EOUT_ACTION_CONSTRAINTXY:
		DP()->Helper_SetConstraint(SOFT_Cul_HF_ConstraintXY);
		break;

	case EOUT_ACTION_CONSTRAINTXZ:
		DP()->Helper_SetConstraint(SOFT_Cul_HF_ConstraintXZ);
		break;

	case EOUT_ACTION_CONSTRAINTYZ:
		DP()->Helper_SetConstraint(SOFT_Cul_HF_ConstraintYZ);
		break;

	case EOUT_ACTION_OPTIMIZEOMNI:
		DP()->Optimize_Omni();;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_CREATEWPATCAM:
		if(!(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypoint)) OnAction(EOUT_ACTION_DISPLAYWP);
		pst_GO = DP()->CreateWaypoint(CPoint(0, 0));
		CAM_SetObjectMatrixFromCam(pst_GO->pst_GlobalMatrix, &DDD()->st_Camera.st_Matrix);
		break;

	case EOUT_ACTION_EDITBVMODE:
		if(DP()->Selection_b_IsInSubObjectMode())
		{
			M_MF()->MessageBox("Can't edit BV while editing geometry", "Warning", MB_OK | MB_ICONSTOP);
			DDD()->uc_EditBounding = 0;
			break;
		}

		DDD()->uc_EditBounding = (DDD()->uc_EditBounding) ? 0 : 1;
		if(DDD()->uc_EditBounding)
		{
			LINK_PrintStatusMsg("Entering Bounding volume edition mode.");
			DDD()->uc_EditODE = 0 ;
		}
		else
		{
			LINK_PrintStatusMsg("Leaving Bounding volume edition mode.");
		}

        /* We disable the object mode (but we keep snap modes). */
        DDD()->pst_Helpers->ul_Flags = SOFT_Cul_HF_NoMovers | SOFT_Cul_HF_SAObject | 
            (DDD()->pst_Helpers->ul_Flags & (SOFT_Cul_HF_SnapGrid | SOFT_Cul_HF_SnapGridRender | SOFT_Cul_HF_SnapGridDisplay));
		break;

	case EOUT_ACTION_EDITODEMODE:
		if(DP()->Selection_b_IsInSubObjectMode())
		{
			M_MF()->MessageBox("Can't edit ODE while editing geometry", "Warning", MB_OK | MB_ICONSTOP);
			DDD()->uc_EditODE = 0;
			break;
		}

		DDD()->uc_EditODE = (DDD()->uc_EditODE) ? 0 : 1;
		if(DDD()->uc_EditODE)
		{
			LINK_PrintStatusMsg("Entering ODE Edition Mode.");
			DDD()->uc_EditBounding = 0 ;
		}
		else
		{
			LINK_PrintStatusMsg("Leaving ODE Edition Mode.");
		}
        /* We disable the object mode (but we keep snap modes). */
        DDD()->pst_Helpers->ul_Flags = SOFT_Cul_HF_NoMovers | SOFT_Cul_HF_SAObject | 
            (DDD()->pst_Helpers->ul_Flags & (SOFT_Cul_HF_SnapGrid | SOFT_Cul_HF_SnapGridRender | SOFT_Cul_HF_SnapGridDisplay));


		break;

#ifdef JADEFUSION
	case EOUT_ACTION_HIGHLIGHTMODE:
		/*****************************************/
		GRO_tdst_Struct **ppst_Gro, **ppst_LastGro;
		/*****************************************/

		if(++DDD()->ul_HighlightMode == QMFR_EDINFO_END)
			DDD()->ul_HighlightMode = QMFR_EDINFO_OK;
		
		/* Init for "Branch or Integrate" Highlight Mode */
		switch(DDD()->ul_HighlightMode)
		{
		case QMFR_EDINFO_BR_OR_INTEGRATE:
			vGROIndex.clear();

			ppst_Gro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(&DW()->st_GraphicObjectsTable);
			ppst_LastGro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(&DW()->st_GraphicObjectsTable);

			for(; ppst_Gro <= ppst_LastGro; ppst_Gro++)
			{
				if(TAB_b_IsAHole(*ppst_Gro)) continue;

				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) *ppst_Gro);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					vGROIndex.push_back(ul_Index);
				}
			}
			break;

		default:
			break;
		}

		break;

	case EOUT_ACTION_HIGHLIGHTSELECTION:
		DP()->Selection_HighlightedObjects();
		break;
#endif //JADEFUSION

	case EOUT_ACTION_EDITVERTEX:
		if(DP()->mb_GridEdit && DW())
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			CPoint							pt;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			GetCursorPos(&pt);
			ScreenToClient(&pt);
			DP()->Grid_PasteSel(DW(), pt.x, pt.y);

		}
		else
		{
			if(DP()->Selection_b_IsInSubObjectMode())
				DP()->Selection_EndSubObjectMode();
			else
				DP()->Selection_BeginSubObjectMode();
		}
		break;

#ifdef JADEFUSION
	case EOUT_ACTION_SWITCH_SUBOBJECT_MODE:
		if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_SubObjShortCut))
			if(DP()->Selection_b_IsInSubObjectMode())
				DP()->Selection_SwitchSubObjectMode();
		break;

	case EOUT_ACTION_SWITCH_SUBOBJECT_EDGE_MODE:
		if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_SubObjShortCut))
			if(DP()->Selection_b_IsInSubObjectEdgeMode())
				DP()->Selection_SwitchSubObjectEdgeMode();
		break;
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Selection menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EOUT_ACTION_SELECTALL:
		DP()->Selection_GAO_SelectAll();
		break;

	case EOUT_ACTION_VIEWGIZMO:
		DP()->Selection_GAO_ToggleGizmo();
		break;

	case EOUT_ACTION_SETASINITIALPOSITION:
		DP()->Selection_GAO_SetInitialPosition();
		M_MF()->FlashJade();
		break;

	case EOUT_ACTION_RESTOREINITIALPOSITION:
		DP()->Selection_GAO_RestorePosition();
		break;

	case EOUT_ACTION_ORIENTTOCHILD:
	case EOUT_ACTION_ORIENTPIVOTTOCHILD:
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			OBJ_tdst_GameObject	*apst_Child[10];
			OBJ_tdst_GameObject	*pst_Child;
			MATH_tdst_Vector	st_NewBanking, st_OldSight;
			int					numChild;
			char				asz_Name[128];
			char				*psz_Temp;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


			numChild = WOR_i_GetAllChildsOf(pst_GO->pst_World, pst_GO, apst_Child, FALSE);
			if(numChild == 0) return;

			if(numChild > 1)
			{
				EDIA_cl_NameDialogCombo o_Dialog("Choose Child");
				int	i;


				for(i = 0; i < numChild; i++)
				{
					o_Dialog.AddItem(apst_Child[i]->sz_Name);
				}

				L_strcpy(asz_Name, apst_Child[0]->sz_Name);
				psz_Temp = L_strrchr(asz_Name, '.');
				if(psz_Temp) *psz_Temp = 0;

				o_Dialog.SetDefault(asz_Name);

				if(o_Dialog.DoModal() == IDCANCEL) return;

				pst_Child = NULL;
				for(i = 0; i < numChild; i++)
				{
					L_strcpy(asz_Name, apst_Child[i]->sz_Name);
					psz_Temp = L_strrchr(asz_Name, '.');
					if(psz_Temp) *psz_Temp = 0;

					if(!L_strcmp((char *) (LPCSTR) o_Dialog.mo_Name, asz_Name))
					{
						pst_Child = apst_Child[i];
						break;
					}
				}

				if(!pst_Child)
					pst_Child = apst_Child[0];
			}
			else
				pst_Child = apst_Child[0];

			OBJ_SightGet(pst_GO, &st_OldSight);
			MATH_SubVector(&st_NewBanking, &pst_Child->pst_GlobalMatrix->T, &pst_GO->pst_GlobalMatrix->T);
			MATH_NormalizeEqualVector(&st_NewBanking);

			// TEST
			MATH_tdst_Matrix st_OldMat;
			MATH_CopyMatrix (&st_OldMat,  pst_GO->pst_GlobalMatrix);

			OBJ_BankingGeneralSet(pst_GO, &st_OldSight, &st_NewBanking);

			if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
				OBJ_ComputeLocalWhenHie(pst_GO);

			for(i = 0; i < (ULONG) numChild; i++)
			{
				OBJ_ComputeLocalWhenHie(apst_Child[i]);
			}

			// If we don't want the shape to move with the gao, we need to replace the vertices
			if (_ul_Action == EOUT_ACTION_ORIENTPIVOTTOCHILD)
			{
				MATH_tdst_Matrix st_TransfoMat, st_InvMat;
				MATH_InvertMatrix( &st_InvMat, pst_GO->pst_GlobalMatrix );
				MATH_MulMatrixMatrix( &st_TransfoMat, &st_OldMat, &st_InvMat );

				if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) break;
				GEO_tdst_Object *pst_GeoObj = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
				if (pst_GeoObj->st_Id.i->ul_Type != GRO_Geometric)
				{
					if (pst_GeoObj->st_Id.i->ul_Type == GRO_GeoStaticLOD)
					{
						GEO_tdst_StaticLOD *pst_LOD = (GEO_tdst_StaticLOD *)pst_GeoObj;
						for (int i=0; i<pst_LOD->uc_NbLOD; i++) {
							if (pst_LOD->dpst_Id[i]->i->ul_Type == GRO_Geometric) {
								GEO_ApplyTransfoToVertices(((GEO_tdst_Object *)pst_LOD->dpst_Id[i]), &st_TransfoMat);
							}
						}
					}
					else
					{
						break;
					}
				}
				else
					GEO_ApplyTransfoToVertices(pst_GeoObj, &st_TransfoMat);
			}

		}
		break;


	case EOUT_ACTION_RENAME:
		DP()->Rename();
		break;

#ifdef JADEFUSION
	case EOUT_ACTION_REPLACE:
		if(DP()->mpo_ReplaceDialog == NULL)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_ReplaceDialog	*po_ReplaceDlg;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			po_ReplaceDlg = new EDIA_cl_ReplaceDialog(DW());
			po_ReplaceDlg->mpo_View = DP();
			po_ReplaceDlg->DoModeless();
			po_ReplaceDlg->CenterWindow();
			po_ReplaceDlg->ShowWindow(SW_SHOW);
			DP()->mpo_ReplaceDialog = po_ReplaceDlg;
		}
		break;
#endif

	case EOUT_ACTION_SELECTFATHER:
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(ANI_b_IsGizmoAnim(pst_GO, &pst_Father))
			pst_GO = WOR_pst_GetGizmoFromBone(pst_GO->pst_Base->pst_Hierarchy->pst_Father);
		else
			pst_GO = pst_GO->pst_Base->pst_Hierarchy->pst_Father;
		DP()->Selection_b_Treat(pst_GO, (long) pst_GO, SEL_C_SIF_Object, FALSE);
		break;

	case EOUT_ACTION_SELECTCHILD:
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		numago = WOR_i_GetAllChildsOf(DW(), pst_GO, ago, TRUE);
		if(ANI_b_IsGizmoAnim(pst_GO, &pst_Father))
		{
			if(!(ago[0]->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)) pst_GO = WOR_pst_GetGizmoFromBone(ago[0]);
		}
		else
		{
			pst_GO = ago[0];
		}

		DP()->Selection_b_Treat(pst_GO, (long) pst_GO, SEL_C_SIF_Object, FALSE);
		break;

	case EOUT_ACTION_HIDE:
		DP()->Selection_Hide();
		break;

	case EOUT_ACTION_DISPLAY:
		DP()->Selection_Display();
		break;

	case EOUT_ACTION_HIDEUNSELECTED:
		if(mb_HideUnselected)
			DP()->Selection_HideUnsel();
		else
			DP()->Selection_UnhideAll(TRUE);
		mb_HideUnselected = !mb_HideUnselected;
		break;

	case EOUT_ACTION_ZONE_EDITCOLSET:
		if(DW()->pst_Selection->l_Flag & SEL_C_SF_Editable)
			DW()->pst_Selection->l_Flag &= ~SEL_C_SF_Editable;
		else
		{
			if
			(
				M_MF()->MessageBox
					(
						"The modifications will be reflected to all the instances that use this ColSet. Are you sure?",
						"Warning",
						MB_YESNO
					) == IDYES
			) DW()->pst_Selection->l_Flag |= SEL_C_SF_Editable;
		}

		/*
		 * LINK_UpdatePointer(SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_ZDx)); £
		 * LINK_UpdatePointers();
		 */
		break;

	case EOUT_ACTION_ZONE_DELETE:
		DP()->Zone_Delete();
		break;

	case EOUT_ACTION_ZONE_SETINDEX:
		DP()->Zone_SetIndex();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_OFFSETOBJ:
		DP()->Selection_GAO_OffsetObjects();
		break;

	case EOUT_ACTION_RESETSCALE:
		DP()->Selection_GAO_ResetScale();
		break;

	case EOUT_ACTION_RESETROTATION:
		DP()->Selection_GAO_ResetRotation();
		break;

	case EOUT_ACTION_DUPLICATE:
		{
			/*~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	v;
			/*~~~~~~~~~~~~~~~~~~*/

			MATH_InitVector(&v, 2, 2, 2);
			DP()->Selection_GAO_Duplicate(&v, 0,NULL,NULL,NULL);
		}
		break;
		
	case EOUT_ACTION_DUPLICATEEXT:
		if(mst_Ini.c_DuplicateExtended)
			mst_Ini.c_DuplicateExtended = 0;
		else
			mst_Ini.c_DuplicateExtended = 1;
		break;					

	case EOUT_ACTION_TELEPORT:
		if(DP()->Selection_b_IsInSubObjectMode()) 
			DP()->Selection_SubObjectTeleport(FALSE);
		else
			DP()->Selection_GAO_Teleport(FALSE);
		break;

	case EOUT_ACTION_TELEPORTCENTER:
		if(DP()->Selection_b_IsInSubObjectMode()) 
			DP()->Selection_SubObjectTeleport(TRUE);
		else
			DP()->Selection_GAO_Teleport(TRUE);
		break;
		
	case EOUT_ACTION_TELEPORTKEEPRELATIVEPOS:
		if(mst_Ini.c_Teleport_KeepRelativePos)
			mst_Ini.c_Teleport_KeepRelativePos = 0;
		else
			mst_Ini.c_Teleport_KeepRelativePos = 1;
		break;		

	case EOUT_ACTION_DROPTOFLOOR:
		DP()->Selection_GAO_DropToFloor();
		break;

	case EOUT_ACTION_COPYMATRIX:
		DP()->Selection_GAO_CopyMatrix();
		break;

	case EOUT_ACTION_RESETXFORM:
		DP()->Selection_GAO_ResetXForm();
		break;
#ifdef JADEFUSION
    case EOUT_ACTION_COMPUTETANGENTSPACESMOOTHING:
        DP()->XeComputeTangentSpaceSmoothing(0xffffffff);
        break;

    case EOUT_ACTION_COMPUTETSS_GROUP1:
        DP()->XeComputeTangentSpaceSmoothing(0x00000001);
        break;

    case EOUT_ACTION_COMPUTETSS_GROUP2:
        DP()->XeComputeTangentSpaceSmoothing(0x00000002);
        break;

    case EOUT_ACTION_COMPUTETSS_GROUP3:
        DP()->XeComputeTangentSpaceSmoothing(0x00000004);
        break;

    case EOUT_ACTION_COMPUTETSS_GROUP4:
        DP()->XeComputeTangentSpaceSmoothing(0x00000008);
        break;

    case EOUT_ACTION_COMPUTEFIX_RLI:
        DP()->XeSelectionFixRLI();
        break;
#endif

	case EOUT_ACTION_COMPUTEBONESPIVOT:
		DP()->Selection_GAO_ComputeBonePivot();
		break;

	case EOUT_ACTION_DESTROY:
		if(EDI_go_TheApp.GetFocusedEditor() != this) break;
		DP()->Selection_GAO_Destroy();
		break;

	case EOUT_ACTION_HIDEMOVEROTATE:
		SOFT_Helpers_HideMovers(DDD()->pst_Helpers);
		DDD()->uc_EditBounding = 0;
		DP()->mi_FreezeHelper = -1;
		break;

	case EOUT_ACTION_SELECTIONLOCK:
		if(DW()->pst_Selection->l_Flag & SEL_C_SF_Locked)
			DW()->pst_Selection->l_Flag &= ~SEL_C_SF_Locked;
		else
			DW()->pst_Selection->l_Flag |= SEL_C_SF_Locked;
		break;

	case EOUT_ACTION_SELECTIONDIALOG:
		DP()->Selection_WithDialog();
		break;

	case EOUT_ACTION_TOOLBOX:
		DP()->Selection_ToolBox();
		break;
#ifdef JADEFUSION
	// NB: Added Light Rejection dialog - from POP5 Jade Editor
	case EOUT_ACTION_LIGHTREJECT:
		DP()->Selection_LightReject();
		break;
#endif

	case EOUT_ACTION_BACKGROUNDIMAGEDIALOG:
		DP()->Dialogs_BackgroundImage_SwapDisplay();
		break;

	case EOUT_ACTION_CREATEGEOMETRY:
		DP()->CreateGeometry();
		break;

	case EOUT_ACTION_ZONE_CREATECOLMAP:
		DP()->CreateColMap();
		break;

	case EOUT_ACTION_GMAT:
		DP()->GameMaterial();
		break;

	case EOUT_ACTION_UNDOLIST:
		DP()->mo_UndoManager.DisplayUndoDlg(!(DP()->mo_UndoManager.mpo_UndoDlg));
		break;

#ifdef JADEFUSION
	case EOUT_ACTION_CUBEMAPGENERATORDIALOG:
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_GO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(!DW())
			break;

		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);

		if(pst_GO != NULL)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_CubeMapGenDialog	*po_CubeMapGenDlg;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			po_CubeMapGenDlg = new EDIA_cl_CubeMapGenDialog(pst_GO);
			po_CubeMapGenDlg->mpo_View = DP();
			po_CubeMapGenDlg->DoModeless();
			po_CubeMapGenDlg->CenterWindow();
			po_CubeMapGenDlg->ShowWindow(SW_SHOW);
			DP()->mpo_CubeMapGenDialog = po_CubeMapGenDlg;
		}
		else
			ERR_X_Warning(0, "You must select a Gao first !", NULL);;
		break;

	case EOUT_ACTION_SCREENSHOTSEQBUILDERDLG:
		if(!DW() || DP()->mpo_ScreenshotSeqBuilderDialog != NULL)
			break;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		EDIA_cl_ScreenshotSeqBuilderDlg	*po_ScreenshotSeqBuilderDlg;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		po_ScreenshotSeqBuilderDlg = new EDIA_cl_ScreenshotSeqBuilderDlg();
		po_ScreenshotSeqBuilderDlg->mpo_View = DP();
		po_ScreenshotSeqBuilderDlg->DoModeless();
		po_ScreenshotSeqBuilderDlg->CenterWindow();
		po_ScreenshotSeqBuilderDlg->ShowWindow(SW_SHOW);
		DP()->mpo_ScreenshotSeqBuilderDialog = po_ScreenshotSeqBuilderDlg;

		break;
#endif //JADEFUSION

	case EOUT_ACTION_SELECTIONDIALOGLEFT:
		mpo_EngineFrame->mb_LeftSel = mpo_EngineFrame->mb_LeftSel ? FALSE : TRUE;
		mst_Ini.uc_SelDialogLeft = mst_Ini.uc_SelDialogLeft ? 0 : 1;
		mpo_EngineFrame->SendMessage(WM_SIZE, 0, 0);
		break;

	case EOUT_ACTION_CREATE2DTEXTVISUEL:
		DP()->Selection_Create2DTextVisuel();
		break;

	case EOUT_ACTION_CREATEPARTICLEGENERATOR:
		DP()->Selection_CreateParticleGeneratorVisuel();
		break;

	case EOUT_ACTION_SNAPGEOMETRY:
		DP()->Selection_SnapGeo();
		break;

	case EOUT_ACTION_CLEANGEOMETRY:
		DP()->Selection_CleanGeo();
		break;

	case EOUT_ACTION_DUPLICATEGEOMETRY:
		DP()->Selection_DuplicateGro(F3D_Duplicate_Geometry | F3D_Duplicate_AllLocation);
		break;

	case EOUT_ACTION_DUPLICATEMATERIAL:
		DP()->Selection_DuplicateGro( F3D_Duplicate_Material | F3D_Duplicate_AllLocation );
		break;
		
	case EOUT_ACTION_DUPLICATEMATERIALFULL:
		DP()->Selection_DuplicateGro( F3D_Duplicate_Material | F3D_Duplicate_AllLocation | F3D_Duplicate_SubMatOfMultiMat );
		break;

#ifdef JADEFUSION
	case EOUT_ACTION_DUPLICATEMATERIALFULL_INST:
		DP()->Selection_DuplicateGro( F3D_Duplicate_Material | F3D_Duplicate_AllLocation | F3D_Duplicate_SubMatOfMultiMat | F3D_Duplicate_Material_KeepInst);
		break;
#endif

	case EOUT_ACTION_DUPLICATELIGHT:
		DP()->Selection_DuplicateGro(F3D_Duplicate_Light | F3D_Duplicate_AllLocation);
		break;
		
	case EOUT_ACTION_DUPLICATEPAG:
		DP()->Selection_DuplicateGro(F3D_Duplicate_PAG | F3D_Duplicate_AllLocation);
		break;

	case EOUT_ACTION_DUPLICATECOB:
#ifdef JADEFUSION
		DP()->Selection_DuplicateCob(-1); //?? POPOWARNING
#else
		DP()->Selection_DuplicateCob();
#endif
		break;

	case EOUT_ACTION_CREATECOLSET:
		DP()->Zone_CreateColSet();
		break;

	case EOUT_ACTION_CREATECOB:
		DP()->Zone_CreateCob();
		break;

	case EOUT_ACTION_UPDATECOLMAP:
		DP()->Zone_UpdateCob(FALSE);
		break;

	case EOUT_ACTION_UPDATECOLMAPKEEPIDMAT:
		DP()->Zone_UpdateCob(TRUE);
		break;

	case EOUT_ACTION_UPDATEOK3:
		DP()->Zone_UpdateOK3();
		break;

	case EOUT_ACTION_SELECTOBJECT:
		DP()->Zone_SelectObject();
		break;

	case EOUT_ACTION_CREATEGRO:
		DP()->Zone_CreateGro();
		break;

	case EOUT_ACTION_DESTROYGAMEMAT:
		DP()->Zone_DestroyGameMaterial();
		break;

	case EOUT_ACTION_CREATEEVENTS:
		DP()->CreateEvents();
		break;

	case EOUT_ACTION_DELETEEVENTS:
		DP()->DeleteEvents();
		break;

	case EOUT_ACTION_CREATEANIMATION:
		DP()->CreateAnimation();
		break;

	case EOUT_ACTION_SPEEDSELECT:
		DP()->SpeedSelect();
		break;

	case EOUT_ACTION_ISLIGHTEDBY:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			OBJ_tdst_GameObject *pst_GO;
			char				sz_Text[256];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
			if(pst_GO != NULL)
			{
				sprintf(sz_Text, "%s is lighted by :", pst_GO->sz_Name);
				LINK_PrintStatusMsg(sz_Text);
				DP()->mst_WinHandles.pst_DisplayData->pst_GetLightForGO = pst_GO;
				LINK_gb_EditRefresh = TRUE;
				DP()->Refresh();
				LINK_gb_EditRefresh = FALSE;
				DP()->mst_WinHandles.pst_DisplayData->pst_GetLightForGO = NULL;
			}
		}
		break;

#ifdef JADEFUSION
	case EOUT_ACTION_GO_LIGHT_INFORMATION:
		int i_Gao_InfoType;
		i_Gao_InfoType = 1;
#endif

	case EOUT_ACTION_GOINFORMATION:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			OBJ_tdst_GameObject *pst_GO;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
			if(pst_GO != NULL)
			{
				/*~~~~~~~~~~~~~~~~~~~~*/
				EDIA_cl_GaoInfo *po_Dlg;
				/*~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
				po_Dlg = new EDIA_cl_GaoInfo(pst_GO, i_Gao_InfoType);
#else
				po_Dlg = new EDIA_cl_GaoInfo(pst_GO);
#endif
				po_Dlg->mpo_View = DP();
				po_Dlg->DoModeless();
				po_Dlg->CenterWindow();
				po_Dlg->ShowWindow(SW_SHOW);
				DP()->mpo_GaoInfoDialog = po_Dlg;
			}
		}
		break;

	case EOUT_ACTION_PREFABSELOBJ:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~*/
			EPFB_cl_Frame	*po_Editor;
			/*~~~~~~~~~~~~~~~~~~~~~~~*/

			po_Editor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
			if(po_Editor) po_Editor->PrefabSel(DP(), DW(), DW()->pst_Selection);
		}
		break;

	case EOUT_ACTION_PREFABUPDATE:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~*/
			EPFB_cl_Frame	*po_Editor;
			CRect			o_Rect;
			/*~~~~~~~~~~~~~~~~~~~~~~~*/

			po_Editor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
			mpo_MyView->GetWindowRect( &o_Rect );
			if(po_Editor) po_Editor->PrefabUpdate(DW(), DW()->pst_Selection, &o_Rect );
		}
		break;

	case EOUT_ACTION_PREFABCUTLINK:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			OBJ_tdst_GameObject *pst_GO;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
			if(pst_GO != NULL)
            {
                if (pst_GO->ul_PrefabKey != BIG_C_InvalidIndex)
                    pst_GO->ul_PrefabKey = BIG_C_InvalidIndex;
                if (pst_GO->ul_PrefabObjKey != BIG_C_InvalidIndex)
                    pst_GO->ul_PrefabObjKey = BIG_C_InvalidIndex;
            }
		}
        break;

	case EOUT_ACTION_PREFABADDUNSORTED:
	case EOUT_ACTION_PREFABADD:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~*/
			EPFB_cl_Frame	*po_Editor;
			/*~~~~~~~~~~~~~~~~~~~~~~~*/

			po_Editor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
			if(po_Editor)
				po_Editor->AddPrefab(DW(), DW()->pst_Selection, (_ul_Action == EOUT_ACTION_PREFABADD) ? FALSE : TRUE);
		}
		break;

	case EOUT_ACTION_PREFABMERGE:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~*/
			EPFB_cl_Frame	*po_Editor;
			/*~~~~~~~~~~~~~~~~~~~~~~~*/

			po_Editor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
			if(po_Editor) po_Editor->MergePrefab(DW(), DW()->pst_Selection);
		}
		break;

#ifdef JADEFUSION
	case EOUT_ACTION_COPYGAOPROPERTIES:
		{
			if(DP()->Selection_GAO_CopyProperties(FALSE, NULL))
				DDD()->b_PasteGaoProperties = TRUE;
			else
			{
				DDD()->b_PasteGaoProperties = FALSE;
				F3D_ResetGaoClipboard();
				ERR_X_Warning(0, "You must select an objects before copying Gao properties!", NULL);
			}
		}
		break;

	case EOUT_ACTION_PASTEGAOPROPERTIES:
		{
			mpo_MyView->GetWindowRect( &o_Rect );
			if(!DP()->Selection_GAO_CopyProperties(TRUE, &o_Rect))
			{
				ERR_X_Warning(0, "Please select objects before paste Gao properties!", NULL);
			}
		}
		break;
#endif

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_ZONE_SHARED:
		DP()->Zone_RestoreColSet();
		break;

	case EOUT_ACTION_ZONE_ACTIVATE:
		DP()->Zone_Activate();
		break;

	case EOUT_ACTION_ZONE_DESACTIVATE:
		DP()->Zone_Desactivate();
		break;

	case EOUT_ACTION_ZONE_WIRE:
		pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Cob);
		if(pst_Cob && pst_Cob->pst_GO)
		{
			if(pst_Cob->pst_GO->ul_EditorFlags & OBJ_C_EditFlags_WiredCob)
				pst_Cob->pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_WiredCob;
			else
				pst_Cob->pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_WiredCob;
		}
		break;

	case EOUT_ACTION_ZONE_CHECKBV:
		DP()->Zone_CheckBV();
		break;

	case EOUT_ACTION_ZONE_COMPUTEBV:
		DP()->Zone_ComputeBV();
		break;

	case EOUT_ACTION_ZONE_SPECIFIC:
		DP()->Zone_SwapToSpecific();
		break;

	case EOUT_ACTION_ZONE_RENAME:
		DP()->Zone_Rename();
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Group menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EOUT_ACTION_GROUP:
		DP()->Selection_Group();
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Network menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EOUT_ACTION_NETDISPLAY:
		DP()->DisplayNet();
		break;

	case EOUT_ACTION_SELECTNET:
		DP()->SelectNet();
		break;

	case EOUT_ACTION_SELECTLINKS:
		DP()->SelectLinks();
		break;

	case EOUT_ACTION_REMOVENET:
		DP()->RemoveNet(FALSE);
		break;

	case EOUT_ACTION_REMOVENETANDWP:
		DP()->RemoveNet(TRUE);
		break;

	case EOUT_ACTION_REMOVELINKS:
		DP()->RemoveLinks();
		break;

	case EOUT_ACTION_SETROOT:
		DP()->SetRoot();
		break;

	case EOUT_ACTION_CREATEFROMSELECTION:
		DP()->CreateNetworkFromSelection();
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Compute menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EOUT_ACTION_COMPUTENORMALS:
		DP()->Selection_ComputeNormals();
		break;

    case EOUT_ACTION_RADIOSITYLIGHTMAP:
        DP()->RadiosityLightmap();
        break;

	case EOUT_ACTION_RLI:
		DP()->RLITool();
		break;

	case EOUT_ACTION_RLIAUTO:
		DP()->ComputeRLI(0x00FFFFFF, FALSE, FALSE, FALSE);
		break;

	case EOUT_ACTION_COMPUTESTRIPS:
		ComputeStrips(TRUE);
		break;

	case EOUT_ACTION_COMPUTEUNSTRIPS:
		ComputeStrips(FALSE);
		break;

    case EOUT_ACTION_LODSTRIPS:
        ComputeLODStrips(TRUE);
        break;

	case EOUT_ACTION_CHECKBV:
		OBJ_CheckAllBVs(&DW()->st_ActivObjects, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ColMap);
		break;

	case EOUT_ACTION_CHECKSNP:
		INT_SnP_Check(DW()->pst_SnP, &DW()->st_AllWorldObjects);
		break;

	case EOUT_ACTION_RECOMPUTEALLBV:
		OBJ_ReComputeAllBVs(&DW()->st_ActivObjects);
		break;

	case EOUT_ACTION_CENTERUVS:
		DP()->Selection_CenterUVs();
		break;

	case EOUT_ACTION_CENTERUVSWITHMATERIAL:
		DP()->Selection_CenterUVsWithMaterial();
		break;

	case EOUT_ACTION_SETUV4TEXT:
		DP()->Selection_SetUVs4Text();
		break;

    case EOUT_ACTION_COMPUTELIGHTMAPS:
        DP()->ComputeLightmaps(NULL, false);
        break;

    case EOUT_ACTION_COMPUTELIGHTMAPSSELECT:
        DP()->ComputeLightmaps(DW()->pst_Selection, false);
        break;

    case EOUT_ACTION_COMPUTELIGHTMAPQUICK:
        DP()->ComputeLightmaps(DW()->pst_Selection, true);
        break;

    case EOUT_ACTION_DESTROYLIGHTMAPS:
        DP()->DestroyLightmaps();
        break;

	case EOUT_ACTION_BRICKMAPPER:
		DP()->BrickMapper();
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    General menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EOUT_ACTION_FULL:
		mi_FullScreenResolution = GDI_i_Resolution_GetIndex(&ast_Res[0]);
		M_MF()->LockDisplay(&M_MF()->mo_BigSplitter);
		if(M_MF()->mpo_MaxView && !M_MF()->mst_Desktop.b_VeryMaximized)
			M_MF()->SwapMaximised((EDI_cl_BaseView *) GetParent());
		M_MF()->mst_Desktop.b_VeryMaximized = TRUE;
		M_MF()->SwapMaximised((EDI_cl_BaseView *) GetParent());
		M_MF()->UnlockDisplay(&M_MF()->mo_BigSplitter);
		SetFocus();
		break;

	case EOUT_ACTION_SETWIDTH:
		GetClientRect(&o_Rect);
		sprintf(asz_Tmp, "%u", o_Rect.Width());
		o_Dlg1.mo_Name = asz_Tmp;
		if(o_Dlg1.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dlg1.mo_Name, "%u", &cx);
			SetWidth(cx);
		}
		break;

	case EOUT_ACTION_SETHEIGHT:
		GetClientRect(&o_Rect);
		sprintf(asz_Tmp, "%u", o_Rect.Height());
		o_Dlg1.mo_Name = asz_Tmp;
		if(o_Dlg1.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dlg1.mo_Name, "%u", &cy);
			SetHeight(cy);
		}
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    World grid menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EOUT_ACTION_GRIDCREATE:
		DP()->Grid_Create(1);
		break;

	case EOUT_ACTION_GRIDCREATE1:
		DP()->Grid_Create(2);
		break;

	case EOUT_ACTION_GRIDDESTROY:
		if(MessageBox("Are you sure you want to delete grid ?", "Please confirm", MB_OKCANCEL) == IDOK)
		{
			DP()->Grid_Destroy(1);
		}
		break;

	case EOUT_ACTION_GRIDDESTROY1:
		if(MessageBox("Are you sure you want to delete grid ?", "Please confirm", MB_OKCANCEL) == IDOK)
		{
			DP()->Grid_Destroy(2);
		}
		break;

	case EOUT_ACTION_GRIDEDIT:
		DP()->Grid_EditSwap();
		break;

	case EOUT_ACTION_GRIDPAINTVALUE:
		sprintf(asz_Tmp, "%d", mst_Ini.uc_GridPaintValue);
		o_Dlg.mo_Title = "Enter grid paint value";
		o_Dlg.mo_Name = asz_Tmp;
		if(o_Dlg.DoModal() == IDOK) sscanf((char *) (LPCSTR) o_Dlg.mo_Name, "%u", &mst_Ini.uc_GridPaintValue);
		break;

	case EOUT_ACTION_GRIDSETWIDTH:
		sprintf(asz_Tmp, "%d", DW()->pst_Grid->w_NumGroupsX);
		o_Dlg.mo_Title = "Enter Number of Groups (Width)";
		o_Dlg.mo_Name = asz_Tmp;
		if(o_Dlg.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dlg.mo_Name, "%d", &cx);
			DP()->Grid_SetWidth(1, cx);
		}
		break;

	case EOUT_ACTION_GRIDSETHEIGHT:
		sprintf(asz_Tmp, "%d", DW()->pst_Grid->w_NumGroupsY);
		o_Dlg.mo_Title = "Enter Number of Groups (Height)";
		o_Dlg.mo_Name = asz_Tmp;
		if(o_Dlg.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dlg.mo_Name, "%d", &cy);
			DP()->Grid_SetHeight(1, cy);
		}
		break;

	case EOUT_ACTION_GRIDSETWIDTH1:
		sprintf(asz_Tmp, "%d", DW()->pst_Grid->w_NumGroupsX);
		o_Dlg.mo_Title = "Enter Number of Groups (Width)";
		o_Dlg.mo_Name = asz_Tmp;
		if(o_Dlg.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dlg.mo_Name, "%d", &cx);
			DP()->Grid_SetWidth(2, cx);
		}
		break;

	case EOUT_ACTION_GRIDSETHEIGHT1:
		sprintf(asz_Tmp, "%d", DW()->pst_Grid->w_NumGroupsY);
		o_Dlg.mo_Title = "Enter Number of Groups (Height)";
		o_Dlg.mo_Name = asz_Tmp;
		if(o_Dlg.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dlg.mo_Name, "%d", &cy);
			DP()->Grid_SetHeight(2, cy);
		}
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Animation
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EOUT_ACTION_ANIMADDKEY:
	case EOUT_ACTION_ANIMADDKEYROT:
	case EOUT_ACTION_ANIMADDKEYTRA:
	case EOUT_ACTION_ANIMDELKEY:
	case EOUT_ACTION_ANIMDELKEYROT:
	case EOUT_ACTION_ANIMDELKEYTRA:
	case EOUT_ACTION_ANIMPIVOTTRAJROT:
	case EOUT_ACTION_ANIMPIVOTTRAJTRA:
	case EOUT_ACTION_ANIMTHISTRAJROT:
	case EOUT_ACTION_ANIMMOVEKEYLEFT:
	case EOUT_ACTION_ANIMMOVEKEYLEFTROT:
	case EOUT_ACTION_ANIMMOVEKEYLEFTTRA:
	case EOUT_ACTION_ANIMMOVEKEYRIGHT:
	case EOUT_ACTION_ANIMMOVEKEYRIGHTROT:
	case EOUT_ACTION_ANIMMOVEKEYRIGHTTRA:
	case EOUT_ACTION_ANIMTOOLBOX:
	case EOUT_ACTION_ANIMGOTOFRAME:
	case EOUT_ACTION_ANIMCOPYKEYS:
	case EOUT_ACTION_ANIMPASTEKEYS:
	case EOUT_ACTION_ANIMPASTEKEYSABSOLUTE:
	case EOUT_ACTION_ANIMPASTEKEYSSYMETRYREF:
	case EOUT_ACTION_ANIMPASTEKEYSSYMETRY:
	case EOUT_ACTION_DISPLAYCURVEBONES:
	case EOUT_ACTION_ANIMIK:
	case EOUT_ACTION_ANIMSWITCHTRANS:
		DP()->ActionAnimation(_ul_Action);
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    sound
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EOUT_ACTION_SNDSETSOLO:
		if(DW() == NULL) return;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO) return;
		ESON_SetSoloGao(pst_GO);
		break;

	case EOUT_ACTION_SNDSETMUTE:
		if(DW() == NULL) return;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO) return;
		ESON_SetMuteGao(pst_GO);
		break;

	case EOUT_ACTION_SNDMUTEOFF:
		ESON_MuteOff();
		break;

	case EOUT_ACTION_SNDSOLOOFF:
		ESON_SoloOff();
		break;

#ifdef JADEFUSION
    //-------------------------------------------------------------------------------------------------------------------
    //    Modifier
    //-------------------------------------------------------------------------------------------------------------------

    case EOUT_ACTION_CREATESOFTBODY:
        DP()->MDF_CreateSoftBody();
        break;
#endif

	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Dynamic menu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Action >= WM_USER)
	{

		/*$1- Camera ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		_ul_Action -= WM_USER;
		if(_ul_Action < 100)
		{
			return;
		}

		/*$1- Display world ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		_ul_Action -= 100;
		if(_ul_Action < 100)
		{
			pst_World = WOR_pst_Universe_GetWorldPointer(_ul_Action);
			if(pst_World == DW()) return;
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_World);
			ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
			ChangeWorld(ul_Index);
		}

		/*$1- Activate world ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		_ul_Action -= 100;
		if(_ul_Action < 100)
		{
			if(WOR_b_Universe_IsWorldActive(_ul_Action))
				WOR_Universe_SetWorldInactive(_ul_Action);
			else
				WOR_Universe_SetWorldActive(_ul_Action);
		}
	}

	LINK_Refresh();
	RefreshMenu();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EOUT_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UINT				ui_State;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ui_State = (UINT) - 1;

	switch(_ul_Action)
	{

	/*$1- World menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EOUT_ACTION_EXPORTONLYSELECTED:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.uc_ExportOnlySelection) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_EXPORTTEXTURE:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.uc_ExportTexture) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_ENGINEDISPLAY:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.b_EngineDisplay) ? DFCS_CHECKED : 0);
		break;

	/*$1- display menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EOUT_ACTION_DISPSTRIP:
		ui_State = DFCS_BUTTONCHECK;
		if(OGL_gb_DispStrip) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLOD:
		ui_State = DFCS_BUTTONCHECK;
		if(OGL_gb_DispLOD) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYCAMERAS:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayCamera) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYSOUND:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplaySnd) ui_State |= DFCS_CHECKED;
        break;

    case EOUT_ACTION_DISPLAYSOUNDOPT:
		break;

	case EOUT_ACTION_DISPLAYCURVE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DrawCurve) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYSKELOBBOX:
		ui_State = DFCS_BUTTONCHECK;
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		if(SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) > 1) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) return FALSE;

		if
		(
			pst_GO->pst_Base
		&&	pst_GO->pst_Base->pst_GameObjectAnim
		&&	pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
		)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			TAB_tdst_PFelem		*pst_CurrentBone;
			OBJ_tdst_GameObject *pst_BoneGO;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
			if(pst_BoneGO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV)
				ui_State |= DFCS_CHECKED;
		}
		else
			return FALSE;
		break;

	case EOUT_ACTION_DISPLAYSKELETON:
		ui_State = DFCS_BUTTONCHECK;
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		if(SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) > 1) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) return FALSE;
		if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_DrawSkeleton) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYSKIN:
		ui_State = DFCS_BUTTONCHECK;
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		if(SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) > 1) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) return FALSE;
		if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_HideSkin) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYLIGHTS:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayLight) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYWP:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypoint) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYWPINNETWORK:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypointInNetwork) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYWPASBV:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_WaypointBoundingVolume) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYINVISIBLE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYNOFOG:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DrawMask & GDI_Cul_DM_Fogged) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYNOAUTOCLONE:
		ui_State = DFCS_BUTTONCHECK | ((DDD()->ul_DrawMask & GDI_Cul_DM_NoAutoClone) ? 0 : DFCS_CHECKED);
		break;

#ifdef JADEFUSION
    case EOUT_ACTION_ENABLEELEMENTCULLING:
        ui_State = DFCS_BUTTONCHECK;
        if (DDD()->uc_EnableElementCulling != 0) ui_State |= DFCS_CHECKED;
        break;
#endif

	case EOUT_ACTION_DISPLAYVISIBLE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DrawMask & GDI_Cul_DM_Draw) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYPOSTIT:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->uc_DrawPostIt) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYZDM:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowZDM) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYZDE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowZDE) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYCOB:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowCOB) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_SHOWSLOPE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->uc_ColMapShowSlope == 1) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_SHOWSLOPE45:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->uc_ColMapShowSlope == 2) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYGRID:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYGRID1:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYFORCEDCOLOR:
		ui_State = DFCS_BUTTONCHECK;
		if(!(DDD()->ul_DrawMask & GDI_Cul_DM_DontForceColor)) ui_State |= DFCS_CHECKED;
		break;

#ifdef JADEFUSION
    case EOUT_ACTION_DISPLAY_FORCE_VERTEX_COLOR:
        ui_State = DFCS_BUTTONCHECK;
        if(DDD()->b_ShowVertexColor) ui_State |= DFCS_CHECKED;
        break;

    case EOUT_ACTION_DISPLAY_DISABLE_BASE_MAP:
        ui_State = DFCS_BUTTONCHECK;
        if(DDD()->b_DisableBaseMap) ui_State |= DFCS_CHECKED;
        break;

    case EOUT_ACTION_DISPLAY_DISABLE_MOSS:
        ui_State = DFCS_BUTTONCHECK;
        if(DDD()->b_DisableMoss) ui_State |= DFCS_CHECKED;
        break;

    case EOUT_ACTION_DISPLAY_DISABLE_LIGHTMAPS:
        ui_State = DFCS_BUTTONCHECK;
        if(DDD()->b_DisableLightmap) ui_State |= DFCS_CHECKED;
        break;

    case EOUT_ACTION_DISPLAY_UNLOAD_ALL_SHADERS:
        break;
#endif

	case EOUT_ACTION_DISPLAYPROPORTIONNAL:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_Proportionnal) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYWIREFRAMED:
		ui_State = DFCS_BUTTONCHECK | ((DDD()->ul_DrawMask & GDI_Cul_DM_NotWired) ? 0 : DFCS_CHECKED);
		break;

#ifndef JADEFUSION //POPOWARNING a terminer
	case EOUT_ACTION_DISPLAYFACETMODE:
		ui_State = DFCS_BUTTONCHECK | ((DDD()->ul_DrawMask & GDI_Cul_DM_NoFacetMode) ? 0 : DFCS_CHECKED);
		break;

	case EOUT_ACTION_DISPLAYRLIPLACEMODE:
		ui_State = DFCS_BUTTONCHECK | ((DDD()->ul_DrawMask & GDI_Cul_DM_NoShowRLIPlaceMode) ? 0 : DFCS_CHECKED);
		break;
#endif
	case EOUT_ACTION_SHADESELECTED:
		ui_State = DFCS_BUTTONCHECK | ((DDD()->ul_DisplayFlags & GDI_Cul_DF_ShadeSelected) ? 0 : DFCS_CHECKED);
		break;

	case EOUT_ACTION_DISPLAYHIERARCHY:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayHierarchy) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYOK3:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_cul_DF_DisplayOK3) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_SELECTBONE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_SelectBone) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_SELECTPREFAB:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.ul_SelFlags & EOUT_SelFlags_Prefab) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_DISPLAYCAPTUREONE:
		ui_State = DFCS_BUTTONCHECK;
		if((DDD()->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Type) == GDI_Cc_CaptureType_One) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYCAPTUREREALTIME:
		ui_State = DFCS_BUTTONCHECK;
		if((DDD()->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Type) == GDI_Cc_CaptureType_Multiple)
			ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYCAPTUREINFINITE:
		ui_State = DFCS_BUTTONCHECK;
		if((DDD()->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Type) == GDI_Cc_CaptureType_Infinit) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPLAYCAPTUREBIG:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureFlag_BIG) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_FREEZEHELPER:
		ui_State = DFCS_BUTTONCHECK;
		if(DP()->mi_FreezeHelper != -1) ui_State |= DFCS_CHECKED;
		break;
		
	case EOUT_ACTION_DUPLICATEEXT:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.c_DuplicateExtended) ? DFCS_CHECKED : 0);
		break;
		
	case EOUT_ACTION_TELEPORTKEEPRELATIVEPOS:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.c_Teleport_KeepRelativePos) ? DFCS_CHECKED : 0);
		break;
		
	case EOUT_ACTION_DISPLAYHISTOGRAMM:
		ui_State = DFCS_BUTTONCHECK | ((DDD()->ShowHistogramm) ? DFCS_CHECKED : 0);
		break;


	/*$1- camera menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_ENGINECAM:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->uc_EngineCamera) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_EDITORCAM:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->uc_EditorCamera) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_LOCKTOCAM:
		ui_State = DFCS_BUTTONCHECK | ((DDD()->uc_LockObjectToCam) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_AUTOZOOMEXTENDONSEL:
		ui_State = DFCS_BUTTONCHECK;
		if(DP()->mb_AutoZoomExtendOnRotate) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_CAMERA_PERSPECTIVE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_CONSTANTZOOMSPEED:
		ui_State = DFCS_BUTTONCHECK;
		if(mpo_EngineFrame->mpo_DisplayView->mb_ConstantZoomSpeed) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_CLOSEVIEW:
		ui_State = DFCS_BUTTONCHECK;
		if(WOR_gpst_CurrentWorld && WOR_gpst_CurrentWorld->b_IsSplitScreen) ui_State |= DFCS_CHECKED;
		break;

	/*$1- Edition menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_SNAPTOGRID:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapGrid) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_SNAPGRIDRENDER:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapGridRender) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_SNAPANGLE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapAngle) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_CAMERASYSTEMAXIS:
		ui_State = DFCS_BUTTONRADIO;
		ui_State |= (DP()->Helper_b_IsCurrentAxis(SOFT_Cul_HF_SACamera) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_WORLDSYSTEMAXIS:
		ui_State = DFCS_BUTTONRADIO;
		ui_State |= (DP()->Helper_b_IsCurrentAxis(SOFT_Cul_HF_SAWorld) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_OBJECTSYSTEMAXIS:
		ui_State = DFCS_BUTTONRADIO;
		ui_State |= (DP()->Helper_b_IsCurrentAxis(SOFT_Cul_HF_SAObject) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_CONSTRAINTXY:
		ui_State = DFCS_BUTTONRADIO;
		if(DP()->Helper_b_IsCurrentConstraint(SOFT_Cul_HF_ConstraintXY)) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_CONSTRAINTXZ:
		ui_State = DFCS_BUTTONRADIO;
		if(DP()->Helper_b_IsCurrentConstraint(SOFT_Cul_HF_ConstraintXZ)) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_CONSTRAINTYZ:
		ui_State = DFCS_BUTTONRADIO;
		if(DP()->Helper_b_IsCurrentConstraint(SOFT_Cul_HF_ConstraintYZ)) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_EDITBVMODE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->uc_EditBounding) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_EDITODEMODE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->uc_EditODE) ui_State |= DFCS_CHECKED;
		break;

#ifdef JADEFUSION
	case EOUT_ACTION_HIGHLIGHTMODE:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->ul_HighlightMode) ui_State |= DFCS_CHECKED;
		break;
#endif

    case EOUT_ACTION_SHOWAIVECTORS:
		ui_State = DFCS_BUTTONRADIO;
        if ((DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowAIDebugVectors) && !DDD()->b_DisplayDebugSelected)
            ui_State |= DFCS_CHECKED;
		break;

    case EOUT_ACTION_SHOWNOAIVECTORS:
		ui_State = DFCS_BUTTONRADIO;
        if (!(DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowAIDebugVectors))
            ui_State |= DFCS_CHECKED;
		break;

    case EOUT_ACTION_SHOWSELECTEDAIVECTORS:
		ui_State = DFCS_BUTTONRADIO;
        if ((DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowAIDebugVectors) && DDD()->b_DisplayDebugSelected)
            ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_PHOTOMODE:
		ui_State = DFCS_BUTTONCHECK;
		if(OBJ_gb_DebugPhotoMode) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_SECTORISATION:
		ui_State = DFCS_BUTTONCHECK;
		if(ENG_gb_ActiveSectorization) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_GMAT:
		ui_State = DFCS_BUTTONCHECK;
		if(DP()->mpo_GameMaterialDialog) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_ZONE_GMATSOUND:
		ui_State = DFCS_BUTTONCHECK;
		if(COL_gb_SoundColorDisplay) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_ZONE_SHOWOPTIM:
		ui_State = DFCS_BUTTONCHECK;
		if(COL_s_GhostOptimisation) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_EDITVERTEX:
		ui_State = DFCS_BUTTONCHECK;
		if(DP()->Selection_b_IsInSubObjectMode()) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_DISPHELPERTOOLTIP:
		ui_State = DFCS_BUTTONCHECK;
		if(DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_HideHelperToolTip) ui_State |= DFCS_CHECKED;
		break;

	/*$1- Selection menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_ZONE_EDITCOLSET:
		ui_State = DFCS_BUTTONCHECK;
		if(DW()->pst_Selection->l_Flag & SEL_C_SF_Editable) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_SELECTIONLOCK:
		ui_State = DFCS_BUTTONCHECK | ((DW()->pst_Selection->l_Flag & SEL_C_SF_Locked) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_SELECTIONDIALOG:
		ui_State = DFCS_BUTTONCHECK;
		if(DP()->mb_SelectOn) ui_State |= DFCS_CHECKED;
		break;

	case EOUT_ACTION_TOOLBOX:
		ui_State = DFCS_BUTTONCHECK;
		if(DP()->mb_ToolBoxOn) ui_State |= DFCS_CHECKED;
		break;

#ifdef JADEFUSION
	// NB: Added Light Rejection dialog - from POP5 Jade Editor
	case EOUT_ACTION_LIGHTREJECT:
		ui_State = DFCS_BUTTONCHECK;
		if(DP()->mb_LightRejectOn) ui_State |= DFCS_CHECKED;
		break;
#endif

	case EOUT_ACTION_BACKGROUNDIMAGEDIALOG:
		ui_State = DFCS_BUTTONCHECK | ((DP()->mpo_BackgroundImageDialog) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_CREATEGEOMETRY:
		ui_State = DFCS_BUTTONCHECK | ((DP()->mpo_CreateGeometryDialog) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_SECTO:
		ui_State = DFCS_BUTTONCHECK | (gspo_SectoDialog ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_UNDOLIST:
		ui_State = DFCS_BUTTONCHECK | ((DP()->mo_UndoManager.mpo_UndoDlg) ? DFCS_CHECKED : 0);
		break;

#ifdef JADEFUSION
	case EOUT_ACTION_CUBEMAPGENERATORDIALOG:
		ui_State = DFCS_BUTTONCHECK | ((DP()->mpo_CubeMapGenDialog) ? DFCS_CHECKED : 0);
		break;

	case EOUT_ACTION_SCREENSHOTSEQBUILDERDLG:
		ui_State = DFCS_BUTTONCHECK | ((DP()->mpo_ScreenshotSeqBuilderDialog) ? DFCS_CHECKED : 0);
		break;
#endif

	case EOUT_ACTION_SELECTIONDIALOGLEFT:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.uc_SelDialogLeft) ui_State |= DFCS_CHECKED;
		break;

	/*$1-World grid menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_GRIDEDIT:
		ui_State = DFCS_BUTTONCHECK;
		if(DP()->Grid_b_InEdit()) ui_State |= DFCS_CHECKED;
		break;

	/*$1- son ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_SNDSETMUTE:
		ui_State = DFCS_BUTTONCHECK;
		if(DW() != NULL)
		{
			if(!SEL_b_IsEmpty(DW()->pst_Selection))
			{
				if(!(SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) > 1))
				{
					pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
					if(ESON_b_GaoIsMuted(pst_GO)) ui_State |= DFCS_CHECKED;
				}
			}
		}
		break;

	case EOUT_ACTION_SNDSETSOLO:
		ui_State = DFCS_BUTTONCHECK;
		if(DW() != NULL)
		{
			if(!SEL_b_IsEmpty(DW()->pst_Selection))
			{
				if(!(SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) > 1))
				{
					pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
					if(ESON_b_GaoIsSolo(pst_GO)) ui_State |= DFCS_CHECKED;
				}
			}
		}
		break;

#ifdef JADEFUSION
    case EOUT_ACTION_DISPLAY_MDFWIND:
        ui_State = DFCS_BUTTONCHECK;
        if (Wind_gb_Display)
            ui_State |= DFCS_CHECKED;
        break;

    case EOUT_ACTION_DISPLAYSOFTBODYPLANE:
        ui_State = DFCS_BUTTONCHECK;
        if (SoftBody_gb_DisplayPlaneCollision)
            ui_State |= DFCS_CHECKED;
        break;
#endif

	}

	return ui_State;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EOUT_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL _b_Disp)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Father;
	COL_tdst_ZDx		*pst_ZDx;
	COL_tdst_Cob		*pst_Cob;
	int					numago;
	OBJ_tdst_GameObject *ago[100];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_ul_Action)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_EDITORCAM:
	case EOUT_ACTION_LOCKTOCAM:
		if(DDD()->pst_EditorCamObject == NULL) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_FREEZEHELPER:
		if(!DW()) return FALSE;
		if(DP()->mst_WinHandles.pst_DisplayData->pst_Helpers->l_Pickable == -1) return FALSE;
		break;

	case EOUT_ACTION_FREEZEHELPERX:
	case EOUT_ACTION_FREEZEHELPERY:
	case EOUT_ACTION_FREEZEHELPERZ:
		if(!DW()) return FALSE;
		if(DP()->mi_FreezeHelper == -1) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_CLOSEWORLD:
		if(DW() == NULL) return FALSE;
		if(!DW()->c_EditorReferences) return FALSE; /* Can't close an engine world */
		return TRUE;

	case EOUT_ACTION_SAVEWORLD:
	case EOUT_ACTION_SAVEWORLDDUPLICATERLI:
	case EOUT_ACTION_EXPORTWORLDTOMAD:
	case EOUT_ACTION_EXPORTWORLDTOMADSKIN:
	case EOUT_ACTION_EXPORTONLYSELECTED:
	case EOUT_ACTION_EXPORTTEXTURE:
	case EOUT_ACTION_EXPORTQUICK:
	case EOUT_ACTION_EXPORTDIR:
		return(DW() != NULL);

	case EOUT_ACTION_EXPORTWORLDTOASSOCIATEDMAD:
		return((DW() != NULL) && (*DP()->msz_AssociatedMadFile != 0));

	case EOUT_ACTION_WORLD1:
		if(_b_Disp)
			return FALSE;
		else
			return WOR_ul_Universe_GetNbWorlds() != 0;

	case EOUT_ACTION_WORLD_CHECK:
		if(!DW()) return FALSE;
		return TRUE;

#ifdef JADEFUSION
	case EOUT_ACTION_IMPORT_GRO_MTL_ASSOCIATION:
		if(!DW()) return FALSE;
		return TRUE;

	case EOUT_ACTION_EXPORT_GRO_MTL_ASSOCIATION:
		if(!DW()) return FALSE;
		return TRUE;
#endif

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_NETDISPLAY:
	case EOUT_ACTION_REMOVENET:
	case EOUT_ACTION_REMOVENETANDWP:
		if(DW() == NULL) return FALSE;
		if(DW()->pst_AllNetworks == NULL) return FALSE;
		if(DW()->pst_AllNetworks->ul_Num == 0) return FALSE;
		break;

	case EOUT_ACTION_REMOVELINKS:
	case EOUT_ACTION_SELECTNET:
	case EOUT_ACTION_SELECTLINKS:
	case EOUT_ACTION_SETROOT:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		if(SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) > 1) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO) return FALSE;
		if(!pst_GO->pst_Extended) return FALSE;
		if(!pst_GO->pst_Extended->pst_Links) return FALSE;
		break;

	case EOUT_ACTION_CREATEFROMSELECTION:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO) return FALSE;
		if(!pst_GO->pst_Extended) return FALSE;
		if(!pst_GO->pst_Extended->pst_Links) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_CAMERA_FOCAL:
	case EOUT_ACTION_CAMERA_TOP:
	case EOUT_ACTION_CAMERA_BOTTOM:
	case EOUT_ACTION_CAMERA_LEFT:
	case EOUT_ACTION_CAMERA_RIGHT:
	case EOUT_ACTION_CAMERA_FRONT:
	case EOUT_ACTION_CAMERA_BACK:
	case EOUT_ACTION_CAMERA_GOTOORIGIN:
	case EOUT_ACTION_CAMERA_NEXT:
		if(DW() == NULL) return FALSE;
		break;

	case EOUT_ACTION_ZOOMEXTENDSELECTED:
	case EOUT_ACTION_LOGUNCOLLIDABLE:
	case EOUT_ACTION_CLOSEVIEW:
		if(DW() == NULL) return FALSE;
		if (WOR_gpst_CurrentWorld && WOR_gpst_CurrentWorld->b_IsSplitScreen) return TRUE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		break;

	case EOUT_ACTION_ZOOMEXTEND:
		if(DW() == NULL) return FALSE;
		break;

	case EOUT_ACTION_SPEEDSELECT:
		if(DW() == NULL) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_UNDO:
		if(DP()->mo_UndoManager.mb_IsReDoing) return FALSE;
		if(DP()->mo_UndoManager.mb_IsUnDoing) return FALSE;
		if(DP()->mo_UndoManager.mi_CurrentIndex == 0) return FALSE;
		break;

	case EOUT_ACTION_REDO:
		if(DP()->mo_UndoManager.mb_IsReDoing) return FALSE;
		if(DP()->mo_UndoManager.mb_IsUnDoing) return FALSE;
		if(DP()->mo_UndoManager.mi_CurrentIndex >= DP()->mo_UndoManager.mo_ListOfModifs.GetCount()) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_CAMERASYSTEMAXIS:
		return DP()->Helper_b_AcceptAxis(SOFT_Cul_HF_SACamera);

	case EOUT_ACTION_WORLDSYSTEMAXIS:
		return DP()->Helper_b_AcceptAxis(SOFT_Cul_HF_SAWorld);

	case EOUT_ACTION_OBJECTSYSTEMAXIS:
		return DP()->Helper_b_AcceptAxis(SOFT_Cul_HF_SAObject);

#ifdef JADEFUSION
    case EOUT_ACTION_DRV_DX9:
        return TRUE;
        break;
#endif

	case EOUT_ACTION_OPTIMIZEOMNI:
		if(DW() == NULL) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_DESTROY:
		if(EDI_go_TheApp.GetFocusedEditor() != this) return FALSE;
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO == NULL) break;
		if(ANI_b_IsGizmoAnim(pst_GO, &pst_GO)) return FALSE;	/* Can't delete object in animation edition */
		break;

	case EOUT_ACTION_SELECTALL:
		if(DW() == NULL) return FALSE;
		break;

	case EOUT_ACTION_VIEWGIZMO:
		if(DW() == NULL) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO == NULL) return FALSE;
		break;

	case EOUT_ACTION_SETASINITIALPOSITION:
	case EOUT_ACTION_RESTOREINITIALPOSITION:
		if(DW() == NULL) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO == NULL) return FALSE;
		break;

	case EOUT_ACTION_ORIENTTOCHILD:
	case EOUT_ACTION_ORIENTPIVOTTOCHILD:
		if(DW() == NULL) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO == NULL) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_CREATEEVENTS:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO == NULL) return FALSE;
		if(OBJ_ul_FlagsIdentityGet(pst_GO) & OBJ_C_IdentityFlag_Events) return FALSE;
		break;

	case EOUT_ACTION_DELETEEVENTS:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO == NULL) return FALSE;
		if(!(OBJ_ul_FlagsIdentityGet(pst_GO) & OBJ_C_IdentityFlag_Events)) return FALSE;
		break;

	case EOUT_ACTION_CREATEANIMATION:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO == NULL) return FALSE;

		/* we need a skeleton but no action kit */
		if(OBJ_ul_FlagsIdentityGet(pst_GO) & OBJ_C_IdentityFlag_Anims)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			ANI_st_GameObjectAnim	*pst_GOAnim;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;
			if(!pst_GOAnim->pst_SkeletonModel) return FALSE;
			if(pst_GOAnim->pst_ActionKit) return FALSE;
		}
		else
			return FALSE;
		break;

	case EOUT_ACTION_RESETSCALE:
	case EOUT_ACTION_OFFSETOBJ:
	case EOUT_ACTION_RESETROTATION:
	case EOUT_ACTION_TELEPORT:
	case EOUT_ACTION_TELEPORTCENTER:
	case EOUT_ACTION_DROPTOFLOOR:
	case EOUT_ACTION_DUPLICATE:
	case EOUT_ACTION_COPYMATRIX:
	case EOUT_ACTION_RESETXFORM:
	case EOUT_ACTION_COMPUTEBONESPIVOT:
	case EOUT_ACTION_ISLIGHTEDBY:
	case EOUT_ACTION_GOINFORMATION:
	case EOUT_ACTION_DUPLICATECOB:
		return((DW() != NULL) && (!SEL_b_IsEmpty(DW()->pst_Selection)));

	case EOUT_ACTION_PREFABSELOBJ:
	case EOUT_ACTION_PREFABUPDATE:
	case EOUT_ACTION_PREFABCUTLINK:
        return((DW() != NULL) && (!SEL_b_IsEmpty(DW()->pst_Selection)));

	case EOUT_ACTION_PREFABADDUNSORTED:
	case EOUT_ACTION_PREFABADD:
	case EOUT_ACTION_PREFABMERGE:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~*/
			EPFB_cl_Frame	*po_Editor;
			/*~~~~~~~~~~~~~~~~~~~~~~~*/

			if(DW() == NULL) return FALSE;
			if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
			if(SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) == 0) return FALSE;

			po_Editor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
			if(!po_Editor) return FALSE;
			if(_ul_Action == EOUT_ACTION_PREFABMERGE)
			{
				if(po_Editor->mul_FileIndex == BIG_C_InvalidIndex) return FALSE;
			}
			else if(_ul_Action == EOUT_ACTION_PREFABADD)
			{
				if(po_Editor->mul_DirIndex == BIG_C_InvalidIndex) return FALSE;
				if(po_Editor->mul_DirIndex == BIG_ul_SearchDir(EDI_Csz_Path_PrefabUnsorted)) return FALSE;
			}

			return TRUE;
		}

#ifdef JADEFUSION
	case EOUT_ACTION_COPYGAOPROPERTIES:
		return(DW() != NULL);
	case EOUT_ACTION_PASTEGAOPROPERTIES:
		return((DW() != NULL) && (DDD()->b_PasteGaoProperties));
#endif

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_ZONE_RENAME:
	case EOUT_ACTION_ZONE_ACTIVATE:
	case EOUT_ACTION_ZONE_DESACTIVATE:
	case EOUT_ACTION_ZONE_CHECKBV:
	case EOUT_ACTION_ZONE_COMPUTEBV:
		if(DW() == NULL) return FALSE;
		pst_ZDx = (COL_tdst_ZDx *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_ZDx);
		pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Cob);
		if((pst_ZDx == NULL) && (pst_Cob == NULL)) return FALSE;
		break;

	case EOUT_ACTION_ZONE_WIRE:
		if(DW() == NULL) return FALSE;
		pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Cob);
		if(pst_Cob == NULL) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_DESTROYGAMEMAT:
		int i;

		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if
		(
			!pst_GO
		||	!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ColMap)
		||	!pst_GO->pst_Extended
		||	!pst_GO->pst_Extended->pst_Col
		||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap
		) return FALSE;

		for(i = 0; i < ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->uc_NbOfCob; i++)
		{
			if(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[i]->pst_GMatList) return TRUE;
		}

		return FALSE;

		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_ZONE_SHARED:
	case EOUT_ACTION_ZONE_SPECIFIC:
	case EOUT_ACTION_ZONE_EDITCOLSET:
	case EOUT_ACTION_ZONE_DELETE:
	case EOUT_ACTION_ZONE_SETINDEX:
		if(DW() == NULL) return FALSE;
		pst_ZDx = (COL_tdst_ZDx *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_ZDx);
		if(pst_ZDx == NULL) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_GROUP:
		if(DW() == NULL) return FALSE;
		if(SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) <= 1) return FALSE;
		return TRUE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_HIDEMOVEROTATE:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		if(!SOFT_l_Helpers_MoversAreVisible(DDD()->pst_Helpers)) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_SELECTIONLOCK:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_EDITVERTEX:
		if(DW() == NULL) return FALSE;
		break;

#ifdef JADEFUSION
	case EOUT_ACTION_SWITCH_SUBOBJECT_MODE:
		if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_SubObjShortCut))
			return FALSE;
		if(!DP()->Selection_b_IsInSubObjectMode()) return FALSE;
		break;

	case EOUT_ACTION_SWITCH_SUBOBJECT_EDGE_MODE:
		if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_SubObjShortCut))
			return FALSE;
		if(!DP()->Selection_b_IsInSubObjectEdgeMode()) return FALSE;
		break;

	case EOUT_ACTION_HIGHLIGHTMODE:
		if(DW()== NULL)
			return FALSE;
		break;

	case EOUT_ACTION_HIGHLIGHTSELECTION:
		if(DW()== NULL || DDD()->ul_HighlightMode == 0)
			return FALSE;
		break;
#endif

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_SELECTBONE:
	case EOUT_ACTION_SELECTPREFAB:
	case EOUT_ACTION_CREATEGEOMETRY:
	case EOUT_ACTION_ZONE_CREATECOLMAP:
		if(DW() == NULL) return FALSE;
		break;

	case EOUT_ACTION_SNAPGEOMETRY:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(OBJ_p_GetCurrentGeo(pst_GO) == NULL) return FALSE;

	case EOUT_ACTION_CREATE2DTEXTVISUEL:
	case EOUT_ACTION_CREATEPARTICLEGENERATOR:
	case EOUT_ACTION_DUPLICATEGEOMETRY:
	case EOUT_ACTION_DUPLICATEMATERIAL:
	case EOUT_ACTION_DUPLICATEMATERIALFULL:
#ifdef JADEFUSION
	case EOUT_ACTION_DUPLICATEMATERIALFULL_INST:
#endif
	case EOUT_ACTION_DUPLICATELIGHT:
	case EOUT_ACTION_DUPLICATEPAG:
	case EOUT_ACTION_CREATECOLSET:
	case EOUT_ACTION_UPDATECOLMAP:
	case EOUT_ACTION_UPDATECOLMAPKEEPIDMAT:
	case EOUT_ACTION_UPDATEOK3:
	case EOUT_ACTION_SELECTOBJECT:
	case EOUT_ACTION_CREATEGRO:
	case EOUT_ACTION_CLEANGEOMETRY:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		break;

	case EOUT_ACTION_CREATECOB:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO) return FALSE;
		break;

	case EOUT_ACTION_CAMASOBJ:
	case EOUT_ACTION_OBJASCAM:
	case EOUT_ACTION_RENAME:
#ifdef JADEFUSION
	case EOUT_ACTION_REPLACE:
#endif
	case EOUT_ACTION_HIDE:
	case EOUT_ACTION_DISPLAY:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		break;

	case EOUT_ACTION_HIDEUNSELECTED:
		if(DW() == NULL) return FALSE;
		break;

	case EOUT_ACTION_SELECTFATHER:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO == NULL) return FALSE;
		if(!pst_GO->pst_Base) return FALSE;
		if(!pst_GO->pst_Base->pst_Hierarchy) return FALSE;
		if(!pst_GO->pst_Base->pst_Hierarchy->pst_Father) return FALSE;
		break;

	case EOUT_ACTION_SELECTCHILD:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO == NULL) return FALSE;
		numago = WOR_i_GetAllChildsOf(DW(), pst_GO, ago, TRUE);
		if(!numago) return FALSE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_CREATEWPATCAM:
	case EOUT_ACTION_CREATECAMATCAM:
		return(DW() != NULL);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_COMPUTENORMALS:
    case EOUT_ACTION_RADIOSITYLIGHTMAP:
	case EOUT_ACTION_RLI:
	case EOUT_ACTION_RLIAUTO:
	case EOUT_ACTION_CHECKBV:
	case EOUT_ACTION_CHECKSNP:
	case EOUT_ACTION_RECOMPUTEALLBV:
	case EOUT_ACTION_COMPUTESTRIPS:
	case EOUT_ACTION_COMPUTEUNSTRIPS:
	case EOUT_ACTION_CENTERUVS:
	case EOUT_ACTION_CENTERUVSWITHMATERIAL:
	case EOUT_ACTION_SETUV4TEXT:
	case EOUT_ACTION_BRICKMAPPER:
	case EOUT_ACTION_DISPLAYWOW:
		return(DW() != NULL);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_GRIDCREATE:
		if(!DW()) return FALSE;
		if(DW()->pst_Grid) return FALSE;
		return TRUE;

	case EOUT_ACTION_GRIDCREATE1:
		if(!DW()) return FALSE;
		if(DW()->pst_Grid1) return FALSE;
		return TRUE;

	case EOUT_ACTION_GRIDPAINTVALUE:
		if(!DW()) return FALSE;
		if(!DW()->pst_Grid && !DW()->pst_Grid1) return FALSE;

	case EOUT_ACTION_GRIDDESTROY:
	case EOUT_ACTION_GRIDSETWIDTH:
	case EOUT_ACTION_GRIDSETHEIGHT:
	case EOUT_ACTION_DISPLAYGRID:
		if(!DW()) return FALSE;
		if(!DW()->pst_Grid) return FALSE;
		return TRUE;

	case EOUT_ACTION_GRIDDESTROY1:
	case EOUT_ACTION_GRIDSETWIDTH1:
	case EOUT_ACTION_GRIDSETHEIGHT1:
	case EOUT_ACTION_DISPLAYGRID1:
		if(!DW()) return FALSE;
		if(!DW()->pst_Grid1) return FALSE;
		return TRUE;

	case EOUT_ACTION_GRIDEDIT:
		if(!DW()) return FALSE;
		if(!DW()->pst_Grid && !DW()->pst_Grid1) return FALSE;
		if
		(
			!(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show)
		&&	!(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1)
		) return FALSE;
		break;

	/*$1- Animation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_ANIMPIVOTTRAJROT:
	case EOUT_ACTION_ANIMPIVOTTRAJTRA:
	case EOUT_ACTION_ANIMTHISTRAJROT:
		if(!(DDD()->ul_DisplayFlags & GDI_Cul_DF_DrawCurve)) return FALSE;

	case EOUT_ACTION_DISPLAYCURVEBONES:
	case EOUT_ACTION_ANIMADDKEY:
	case EOUT_ACTION_ANIMADDKEYROT:
	case EOUT_ACTION_ANIMADDKEYTRA:
	case EOUT_ACTION_ANIMDELKEY:
	case EOUT_ACTION_ANIMDELKEYROT:
	case EOUT_ACTION_ANIMDELKEYTRA:
	case EOUT_ACTION_ANIMMOVEKEYLEFT:
	case EOUT_ACTION_ANIMMOVEKEYLEFTROT:
	case EOUT_ACTION_ANIMMOVEKEYLEFTTRA:
	case EOUT_ACTION_ANIMMOVEKEYRIGHT:
	case EOUT_ACTION_ANIMMOVEKEYRIGHTROT:
	case EOUT_ACTION_ANIMMOVEKEYRIGHTTRA:
	case EOUT_ACTION_ANIMTOOLBOX:
	case EOUT_ACTION_ANIMGOTOFRAME:
	case EOUT_ACTION_ANIMCOPYKEYS:
	case EOUT_ACTION_ANIMPASTEKEYS:
	case EOUT_ACTION_ANIMPASTEKEYSABSOLUTE:
	case EOUT_ACTION_ANIMPASTEKEYSSYMETRYREF:
	case EOUT_ACTION_ANIMPASTEKEYSSYMETRY:
	case EOUT_ACTION_ANIMIK:
	case EOUT_ACTION_ANIMSWITCHTRANS:
		if(!DW()) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(pst_GO == NULL) return FALSE;
		if(!ANI_b_IsGizmoAnim(pst_GO, &pst_GO)) return FALSE;
		break;

	case EOUT_ACTION_DISPLAYSKELOBBOX:
	case EOUT_ACTION_DISPLAYSKELETON:
	case EOUT_ACTION_DISPLAYSKIN:
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO) return FALSE;
		pst_Father = ANI_pst_GetReference(pst_GO);
		if(!(pst_Father->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) return FALSE;
		return TRUE;

	/*$1- sound ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_ACTION_DISPLAYSOUNDOPT:
	case EOUT_ACTION_DISPLAYSOUND:
		if(!SND_gst_Params.l_Available) return FALSE;
		break;

	case EOUT_ACTION_SNDSETSOLO:
		if(!SND_gst_Params.l_Available) return FALSE;
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		if(SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) > 1) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO || !pst_GO->pst_Extended || !pst_GO->pst_Extended->pst_Sound) return FALSE;
		break;

	case EOUT_ACTION_SNDSETMUTE:
		if(!SND_gst_Params.l_Available) return FALSE;
		if(DW() == NULL) return FALSE;
		if(SEL_b_IsEmpty(DW()->pst_Selection)) return FALSE;
		if(SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) > 1) return FALSE;
		pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(DW()->pst_Selection, SEL_C_SIF_Object);
		if(!pst_GO || !pst_GO->pst_Extended || !pst_GO->pst_Extended->pst_Sound) return FALSE;
		break;

	case EOUT_ACTION_SNDSOLOOFF:
	case EOUT_ACTION_SNDMUTEOFF:
		if(!SND_gst_Params.l_Available) return FALSE;
		break;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EOUT_cl_Frame::ui_ActionFillDynamic(EDI_cl_ConfigList *_po_List, POSITION _pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_cl_Action	*po_Action;
	int				num, realnum, numsep;
	WOR_tdst_World	*pst_World;
	TAB_tdst_PFelem *pst_CurrentElem;
	TAB_tdst_PFelem *pst_EndElem;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Action = _po_List->mo_List.GetAt(_pos);
	switch((int) po_Action->mul_Action)
	{

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Add list of all loaded world
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EOUT_SEPACTION_WORLD:
		numsep = 0;
		realnum = 0;

		if(WOR_ul_Universe_GetNbWorlds())
		{
			for(i = 0; i < 2; i++)
			{
				/* Insert all words (for display and activation) */
				pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
				pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
				realnum = 0;
				num = 0;
				for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
				{
					pst_World = (WOR_tdst_World *) pst_CurrentElem->p_Pointer;
					if(TAB_b_IsAHole(pst_World))
					{
						num++;
						continue;
					}

					/* Insert a separator */
					if((i == 1) && (realnum == 0))
					{
						po_Action = new EDI_cl_Action;
						po_Action->mo_DisplayName = "Activation";
						po_Action->mo_Key = "";
						po_Action->muw_Key = 0;
						po_Action->mui_Resource = 0;
						po_Action->mui_State = 0;
						po_Action->mul_Action = 0;
						_po_List->mo_List.InsertAfter(_pos, po_Action);
						_po_List->mo_List.GetNext(_pos);
						numsep++;
					}

					po_Action = new EDI_cl_Action;
					po_Action->mo_DisplayName = pst_World->sz_Name;
					if(pst_World == MAI_gst_MainHandles.pst_World) po_Action->mo_DisplayName += "  (ENG)";
					po_Action->mo_Key = "";
					po_Action->muw_Key = 0;
					po_Action->mui_Resource = 0;

					/* To display */
					if(i == 0)
					{
						po_Action->mui_State = DFCS_BUTTONRADIO | ((DW() == pst_World) ? DFCS_CHECKED : 0);
						po_Action->mul_Action = WM_USER + 100 + num++;
					}

					/* Activation */
					else
					{
						po_Action->mui_State = DFCS_BUTTONCHECK | (WOR_b_Universe_IsWorldActive(num) ? DFCS_CHECKED : 0);
						po_Action->mul_Action = WM_USER + 200 + num++;
					}

					_po_List->mo_List.InsertAfter(_pos, po_Action);
					_po_List->mo_List.GetNext(_pos);
					realnum++;
				}
			}
		}

		return(realnum * 2) + numsep;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::OnActionUI(ULONG _ul_Action, CString &_o_Ref, CString &o_Ret)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				sz_Value[100];
	float				f_Value;
	CRect				o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_Ret = _o_Ref;
	switch(_ul_Action)
	{
	case EOUT_ACTION_SETWIDTH:
		GetClientRect(&o_Rect);
		sprintf(sz_Value, " (%u)", o_Rect.Width());
		o_Ret += sz_Value;
		break;

	case EOUT_ACTION_SETHEIGHT:
		GetClientRect(&o_Rect);
		sprintf(sz_Value, " (%u)", o_Rect.Height());
		o_Ret += sz_Value;
		break;


	case EOUT_ACTION_ZOOMSPEED:
		sprintf(sz_Value, " (%u)", mpo_EngineFrame->mpo_DisplayView->mi_ZoomSpeed);
		o_Ret += sz_Value;
		break;

	case EOUT_ACTION_SNAPGRIDSIZEXY:
		sprintf(sz_Value, " (%f)", DDD()->pst_Helpers->f_GridSizeXY);
		o_Ret += sz_Value;
		break;

	case EOUT_ACTION_AUTOSAVEWORLD:
		sprintf(sz_Value, "toto" );
		o_Ret += sz_Value;
        break;

	case EOUT_ACTION_SNAPGRIDSIZEZ:
        if (DDD()->pst_Helpers->f_GridSizeXY == DDD()->pst_Helpers->f_GridSizeZ)
    		sprintf(sz_Value, " (same as XY)");
        else
    		sprintf(sz_Value, " (%f)", DDD()->pst_Helpers->f_GridSizeZ);
		o_Ret += sz_Value;
		break;

	case EOUT_ACTION_SNAPGRIDWIDTH:
		sprintf(sz_Value, " (%f)", DDD()->pst_Helpers->f_GridWidth);
		o_Ret += sz_Value;
		break;

	case EOUT_ACTION_ANGLESTEP:
		f_Value = DDD()->pst_Helpers->f_AngleSnap * Cf_180ByPi;
		sprintf(sz_Value, " (%g)", f_Value);
		o_Ret += sz_Value;
		break;

	case EOUT_ACTION_EXPORTWORLDTOASSOCIATEDMAD:
		o_Ret += DP()->msz_AssociatedMadFile;
		break;

	case EOUT_ACTION_EXPORTDIR:
		o_Ret += DP()->msz_ExportDir;
		break;

	case EOUT_ACTION_EDITORCAM:
		pst_GO = (OBJ_tdst_GameObject *) DDD()->pst_EditorCamObject;
		if(pst_GO)
		{
			o_Ret += " ";
			o_Ret += pst_GO->sz_Name;
		}
		break;

	case EOUT_ACTION_LOCKTOCAM:
		pst_GO = (OBJ_tdst_GameObject *) DDD()->pst_EditorCamObject;
		if(pst_GO)
		{
			o_Ret = "Lock ";
			o_Ret += pst_GO->sz_Name;
			o_Ret += " to cam";
		}
		break;

	case EOUT_ACTION_DISPLAYCAPTURESTARTSTOP:
	case EOUT_ACTION_DISPLAYCAPTURESTARTSTOPC:
		if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Running)
			o_Ret = "Stop !";
		else
		{
			o_Ret += " (";
			if(DDD()->uc_SnapshotFlag & GDI_Cc_CaptureFlag_BIG)
			{
				o_Ret += _itoa(DDD()->st_Device.Vw << 2, sz_Value, 10);
				o_Ret += " x ";
				o_Ret += _itoa(DDD()->st_Device.Vh << 2, sz_Value, 10);
			}
			else
			{
				o_Ret += _itoa(DDD()->st_Device.Vw, sz_Value, 10);
				o_Ret += " x ";
				o_Ret += _itoa(DDD()->st_Device.Vh, sz_Value, 10);
			}

			o_Ret += ")";
		}
		break;

	case EOUT_ACTION_DISPLAYCAPTURENAME:
		o_Ret += " (";
		o_Ret += DDD()->sz_SnapshotName;
		o_Ret += ")";
		break;

	case EOUT_ACTION_GRIDPAINTVALUE:
		sprintf(sz_Value, " (%d)", mst_Ini.uc_GridPaintValue);
		o_Ret += sz_Value;
		break;

	case EOUT_ACTION_HIDEUNSELECTED:
		o_Ret = mb_HideUnselected ? "Hide unselected" : "Unhide all";
		break;

	case EOUT_ACTION_PREFABADD:
	case EOUT_ACTION_PREFABMERGE:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~*/
			EPFB_cl_Frame	*po_Editor;
			/*~~~~~~~~~~~~~~~~~~~~~~~*/

			po_Editor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
			if(!po_Editor) break;
			o_Ret += " : ";
			if(_ul_Action == EOUT_ACTION_PREFABMERGE)
				o_Ret += BIG_NameFile(po_Editor->mul_FileIndex);
			else
				/* EOUT_ACTION_PREFABADD */
				o_Ret += BIG_NameDir(po_Editor->mul_DirIndex);
		}
		break;

#ifdef JADEFUSION
	case EOUT_ACTION_HIGHLIGHTMODE:
		switch(DDD()->ul_HighlightMode)
		{
		case QMFR_EDINFO_OK:
			o_Ret += " - Too many lights";
			break;

		case QMFR_EDINFO_TOO_MANY_LIGHTS:
			o_Ret += " - New or Modified";
			break;

		case QMFR_EDINFO_PIXEL_LIGHTED:
			o_Ret += " - Pixel lighted";
			break;

		case QMFR_EDINFO_CAST_REC_SHADOW:
			o_Ret += " - Cast/Receive shadows";
			break;

		case QMFR_EDINFO_BR_OR_INTEGRATE:
			o_Ret += " - Off";
			break;

		default:
			break;
		}
		break;
#endif

	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EOUT_cl_Frame::i_IsItVarAction(ULONG _ul_Action, EVAV_cl_View *_po_List)
{
	switch(_ul_Action)
	{
	case EOUT_SEPACTION_OPTIONS:
		/* To avoid reset and refresh the list if not necessary */
		if(!_po_List) return 2;
		if(_po_List->mul_ID == EOUT_SEPACTION_OPTIONS) return 2;
		_po_List->mul_ID = EOUT_SEPACTION_OPTIONS;

		_po_List->ResetList();
		_po_List->AddItem("o", EVAV_EVVIT_Pointer, DDD(), EVAV_AutoExpand, LINK_C_StructDisplayData);

		return 1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::OnToolBarCommand(UINT nID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	CPoint			pt;
	int				i_Res;
	ULONG			i;
	BIG_INDEX		ul_Index;
	int				num, numsep;
	char			*sz_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(nID)
	{

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EOUT_ID_EDITGRID:
		GetCursorPos(&pt);
		M_MF()->InitPopupMenuAction(NULL, &o_Menu);
/*$off*/
		M_MF()->AddPopupMenuAction(this, &o_Menu, 1, TRUE, "Display",		DFCS_BUTTONCHECK | (DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show ? DFCS_CHECKED : 0));
		M_MF()->AddPopupMenuAction(this, &o_Menu, 10, TRUE, "Display (1)",	DFCS_BUTTONCHECK | (DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1 ? DFCS_CHECKED : 0));
		M_MF()->AddPopupMenuAction(this, &o_Menu, 2, TRUE, "Content",		DFCS_BUTTONCHECK | (DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_ShowContent ? DFCS_CHECKED : 0));
		M_MF()->AddPopupMenuAction(this, &o_Menu, 3, TRUE, "Edit",			DFCS_BUTTONCHECK | (DP()->Grid_b_InEdit() ? DFCS_CHECKED : 0));
		M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "",				0);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 5, TRUE, "GAO Lock",		DFCS_BUTTONCHECK | (DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_LockZOnObject ? DFCS_CHECKED : 0));
/*$on*/
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
		case 1:
			if(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show)
			{
				DDD()->pst_GridOptions->ul_Flags &= ~GRID_Cul_DO_Show;
				if(DP()->Grid_b_InEdit() && !(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1))
					DP()->Grid_EditSwap();
			}
			else
				DDD()->pst_GridOptions->ul_Flags |= GRID_Cul_DO_Show;
			break;

		case 10:
			if(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1)
			{
				DDD()->pst_GridOptions->ul_Flags &= ~GRID_Cul_DO_Show1;
				if(DP()->Grid_b_InEdit() && !(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show))
					DP()->Grid_EditSwap();
			}
			else
				DDD()->pst_GridOptions->ul_Flags |= GRID_Cul_DO_Show1;
			break;

		case 2:
			if(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_ShowContent)
				DDD()->pst_GridOptions->ul_Flags &= ~GRID_Cul_DO_ShowContent;
			else
				DDD()->pst_GridOptions->ul_Flags |= GRID_Cul_DO_ShowContent;
			break;

		case 3:
			DP()->Grid_EditSwap();
			break;

		case 5:
			if(DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_LockZOnObject)
				DDD()->pst_GridOptions->ul_Flags &= ~GRID_Cul_DO_LockZOnObject;
			else
				DDD()->pst_GridOptions->ul_Flags |= GRID_Cul_DO_LockZOnObject;
			break;
		}

		LINK_Refresh();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EOUT_ID_DISPLAYWP:
		GetCursorPos(&pt);
/*$off*/
		M_MF()->InitPopupMenuAction(NULL, &o_Menu);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 1, TRUE, "Waypoints",			DFCS_BUTTONCHECK | (DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypoint ? DFCS_CHECKED : 0));
		M_MF()->AddPopupMenuAction(this, &o_Menu, 2, TRUE, "Waypoints in Net",	DFCS_BUTTONCHECK | (DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypointInNetwork ? DFCS_CHECKED : 0));
		if(DW()->pst_AllNetworks)
		{
			M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "Networks", 0);
			for(i = 0; i < DW()->pst_AllNetworks->ul_Num; i++)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) DW()->pst_AllNetworks->ppst_AllNetworks[i]);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					M_MF()->AddPopupMenuAction
						(
							this,
							&o_Menu,
							ul_Index,
							TRUE,
							BIG_NameFile(ul_Index),
							DFCS_BUTTONCHECK | (DW()->pst_AllNetworks->ppst_AllNetworks[i]->ul_Flags & WAY_C_DisplayNet ? DFCS_CHECKED : 0)
						);
				}
			}
		}
/*$on*/
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
		case 0:
		case -1:
			break;

		case 1:
			if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypoint)
				DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayWaypoint;
			else
				DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayWaypoint;
			break;

		case 2:
			if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypointInNetwork)
				DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayWaypointInNetwork;
			else
				DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayWaypointInNetwork;
			break;

		default:
			ul_Index = LOA_ul_SearchAddress(BIG_PosFile(i_Res));
			if(((WAY_tdst_Network *) ul_Index)->ul_Flags & WAY_C_DisplayNet)
				((WAY_tdst_Network *) ul_Index)->ul_Flags &= ~WAY_C_DisplayNet;
			else
				((WAY_tdst_Network *) ul_Index)->ul_Flags |= WAY_C_DisplayNet;
			break;
		}

		LINK_Refresh();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EOUT_ID_DISPLAYCAM:
		GetCursorPos(&pt);
		M_MF()->InitPopupMenuAction(NULL, &o_Menu);
/*$off*/
		M_MF()->AddPopupMenuAction(this, &o_Menu, 1, TRUE, "Cameras", DFCS_BUTTONCHECK | (DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayCamera ? DFCS_CHECKED : 0));
/*on*/
		if(DP()->Viewpoint_sz_GetName(0))
		{
			M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "Camera Slot(s)", 0);
			num = 0;
			numsep = 0;
			while(1)
			{
				sz_Name = DP()->Viewpoint_sz_GetName(num);
				if(sz_Name == NULL) break;
				M_MF()->AddPopupMenuAction(this, &o_Menu, WM_USER + num++, TRUE, sz_Name, -1);
			}
		}

		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
		case 0:
		case -1
			: break;
		case 1:
			if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayCamera)
				DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayCamera;
			else
				DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayCamera;
			break;
		default:
			DP()->Viewpoint_b_MoveToCamera(i_Res - WM_USER);
			break;
		}

		LINK_Refresh();
		RefreshMenu();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/
	case EOUT_ID_DISPLAYINVISIBLE:
		GetCursorPos(&pt);
		M_MF()->InitPopupMenuAction(NULL, &o_Menu);
/*$off*/
		M_MF()->AddPopupMenuAction(this, &o_Menu, 40, TRUE, "Invisible objects", DFCS_BUTTONCHECK | (DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible ? DFCS_CHECKED : 0));
		M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "Filter", 0);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 20, TRUE, "None", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 21, TRUE, "All", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 22, TRUE, "Edit", -1);
/*on*/

		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
		case 0:
		case -1:
			break;
		case 20:
			DDD()->l_WPShowStatus = 0;
			DDD()->l_WPShowStatus2 = 0;
			break;
		case 21:
			DDD()->l_WPShowStatus = -1;
			DDD()->l_WPShowStatus2 = -1;
			break;
		case 22:
		{
			EDIA_cl_WPShowStatus *pdlg;
			pdlg = new EDIA_cl_WPShowStatus( (ULONG *) &DDD()->l_WPShowStatus, (ULONG *) &DDD()->l_WPShowStatus2);
			pdlg->DoModeless();
			break;
		}
			
		case 40:
			if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible)
				DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayInvisible;
			else
				DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayInvisible;
			break;
		}

		LINK_Refresh();
		RefreshMenu();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/
	case EOUT_ID_DISPLAYVISIBLE:
		OnAction(EOUT_ACTION_DISPLAYVISIBLE);
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/
	case EOUT_ID_EDITHIE:
		GetCursorPos(&pt);
		M_MF()->InitPopupMenuAction(NULL, &o_Menu);
/*$off*/
		M_MF()->AddPopupMenuAction(this, &o_Menu, 1, TRUE, "Display", DFCS_BUTTONCHECK | (DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayHierarchy ? DFCS_CHECKED : 0));
		M_MF()->AddPopupMenuAction(this, &o_Menu, 2, TRUE, "Edit", DFCS_BUTTONCHECK | (DP()->mb_EditHierarchy ? DFCS_CHECKED : 0));
/*$on*/
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
		case 0:
		case -1:
			break;

		case 1:
			if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayHierarchy)
				DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayHierarchy;
			else
				DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayHierarchy;
			break;

		case 2:
			DP()->mb_EditHierarchy = DP()->mb_EditHierarchy ? FALSE : TRUE;
			if(DP()->mb_EditHierarchy) DDD()->ul_DisplayFlags |= GDI_Cul_DF_DisplayHierarchy;
			break;
		}

		LINK_Refresh();
		RefreshMenu();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EOUT_ID_DISPLAYZ:
		GetCursorPos(&pt);
		M_MF()->InitPopupMenuAction(NULL, &o_Menu);
/*$off*/
		M_MF()->AddPopupMenuAction(this, &o_Menu, 1, TRUE, "ZDE", DFCS_BUTTONCHECK | (DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowZDE ? DFCS_CHECKED : 0));
		M_MF()->AddPopupMenuAction(this, &o_Menu, 2, TRUE, "ZDM", DFCS_BUTTONCHECK | (DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowZDM ? DFCS_CHECKED : 0));
		M_MF()->AddPopupMenuAction(this, &o_Menu, 3, TRUE, "ColMap", DFCS_BUTTONCHECK | (DDD()->ul_DisplayFlags & GDI_Cul_DF_ShowCOB ? DFCS_CHECKED : 0));
		M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "", 0);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 4, TRUE, "Edit BV", DFCS_BUTTONCHECK | (DDD()->uc_EditBounding ? DFCS_CHECKED : 0));
/*on*/
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
		case 0:
		case -1:
			break;
		case 1:
			OnAction(EOUT_ACTION_DISPLAYZDE);
			break;
		case 2:
			OnAction(EOUT_ACTION_DISPLAYZDM);
			break;
		case 3:
			OnAction(EOUT_ACTION_DISPLAYCOB);
			break;
		case 4:
			OnAction(EOUT_ACTION_EDITBVMODE);
			break;
		}
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::OnToolBarCommandUI(UINT nID, CCmdUI *pUI)
{
	if(!DW())
	{
		pUI->Enable(FALSE);
		return;
	}

	switch(nID)
	{
	case EOUT_ID_EDITGRID:
		if((DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show) || (DDD()->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1))
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EOUT_ID_EDITHIE:
		if(DP()->mb_EditHierarchy || DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayHierarchy)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EOUT_ID_DISPLAYWP:
		if(DDD()->ul_DisplayFlags & (GDI_Cul_DF_DisplayWaypoint | GDI_Cul_DF_DisplayWaypointInNetwork))
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EOUT_ID_DISPLAYCAM:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayCamera)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EOUT_ID_DISPLAYINVISIBLE:
		if(DDD()->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EOUT_ID_DISPLAYVISIBLE:
		if(DDD()->ul_DrawMask & GDI_Cul_DM_Draw)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EOUT_ID_DISPLAYZ:
		if(DDD()->uc_EditBounding)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;
	}

	pUI->Enable(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EOUT_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	char c_Key;
	char c_Asc;
	CWnd *po_Wnd;

	c_Key = (char) _uw_Key;
	if((DP()->mpo_SelectionDialog && DP()->mb_SelectOn) || (DP()->mpo_ToolBoxDialog && DP()->mb_ToolBoxOn))
	{
		if(GetFocus())
		{
			if(
				(
					DP()->mpo_SelectionDialog
				&& (GetFocus() == DP()->mpo_SelectionDialog->GetDlgItem(IDC_LIST_OBJ))
				)
				||
				(
					DP()->mpo_ToolBoxDialog
					&& (GetFocus() == DP()->mpo_ToolBoxDialog->mpo_ToolView[2]->GetDlgItem(IDC_LISTBONES))

				)
				||
				(
					DP()->mpo_ToolBoxDialog
					&& (GetFocus() == DP()->mpo_ToolBoxDialog->mpo_ToolView[2]->GetDlgItem(IDC_CANAL))

				)
				||
				(
					DP()->mpo_SelectionDialog
					&& (GetFocus() == DP()->mpo_SelectionDialog->GetDlgItem(IDC_EDIT_GAOF))

				)
				||
				(
					DP()->mpo_ToolBoxDialog
					&& (
						(GetFocus() == DP()->mpo_ToolBoxDialog->mpo_ToolView[0]->GetDlgItem(IDC_EDIT_ID)) ||
						(GetFocus() == DP()->mpo_ToolBoxDialog->mpo_ToolView[0]->GetDlgItem(IDC_EDIT_WELDTHRESH))
						)
				)
			)
			{
				if
				(
					DP()->mpo_SelectionDialog
				&& (GetFocus() == DP()->mpo_SelectionDialog->GetDlgItem(IDC_LIST_OBJ))
				)
				{
					if(_uw_Key == VK_UP) return TRUE;
					if(_uw_Key == VK_DOWN) return TRUE;
					if(_uw_Key == VK_LEFT) return TRUE;
					if(_uw_Key == VK_RIGHT) return TRUE;
					if(_uw_Key == VK_DELETE) return TRUE;
					return FALSE;
				}

				if(_uw_Key == VK_UP) return TRUE;
				if(_uw_Key == VK_DOWN) return TRUE;
				if(_uw_Key == VK_LEFT) return TRUE;
				if(_uw_Key == VK_RIGHT) return TRUE;
				if(_uw_Key == VK_DELETE) return TRUE;
				if(_uw_Key == VK_BACK) return TRUE;
				if(c_Key >= '0' && c_Key <= '9') return TRUE;
				c_Asc = MapVirtualKey(c_Key, 2);
				if(L_isprint(c_Asc) && GetAsyncKeyState(VK_CONTROL) >= 0 && GetAsyncKeyState(VK_SHIFT) >= 0) return TRUE;
				if
				(
					c_Asc == (UCHAR) 'é' ||
					c_Asc == (UCHAR) 'è' ||
					c_Asc == (UCHAR) 'à' ||
					c_Asc == (UCHAR) '@' ||
					c_Asc == (UCHAR) 'ç'
				) return TRUE;
				if(c_Key >= VK_NUMPAD0 && c_Key <= VK_DIVIDE)
					return TRUE;
				if((c_Key >= 0xbc) && (c_Key <= 0xbf)) return TRUE;
				return FALSE;
			}

			po_Wnd = GetFocus();
			while(po_Wnd)
			{
				if( (DP()->mpo_SelectionDialog) && (po_Wnd == DP()->mpo_SelectionDialog->GetDlgItem(IDC_COMBO1)) )
					return TRUE;
				if( (DP()->mpo_SelectionDialog) && (po_Wnd == DP()->mpo_SelectionDialog->GetDlgItem(IDC_COMBO_GOS)) )
					return TRUE;
				po_Wnd = po_Wnd->GetParent();
			}

			return FALSE;
		}
	}
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::SplitViewCloseToObject()
{
	OBJ_tdst_GameObject			*pst_GO;

	if (!WOR_View_SplitView(WOR_gpst_CurrentWorld)) return;

	if(DDD()->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
	{
		pst_GO = DP()->Selection_pst_GetFirstObject();
		if(!pst_GO) return;

		GDI_CopyMainCameraToSplitViewCamera( DDD() );
		GDI_SwapCameras( DDD() );
		GDI_AttachMainCamToObj( DDD(), pst_GO );
		GDI_SwapCameras( DDD() );
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::UnSplitViewCloseToObject()
{
	GDI_SwapCameras( DDD() );
	GDI_DetachMainCamFromObj( DDD() );
	if (!DP()->mb_LastClickInRightView) GDI_SwapCameras( DDD() );
	WOR_View_UnsplitView(WOR_gpst_CurrentWorld);
}

#endif
