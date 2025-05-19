/*$T VERsion_Update35.cpp GC 1.138 12/09/03 12:09:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/VERsion/VERsion_Update.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGkey.h"

#include "BIGfiles\BIGmerge.h"
#include "LINks\LINKmsg.h"
#include "BIGfiles\BIGfat.h"
#include "BIGfiles\BIGmdfy_dir.h"

#include "EDItors\Sources\BROwser\BROframe.h"

#include "DIAlogs/DIA_UPDATE_dlg.h"

static void LoadUniversRef(void);
extern "C" BOOL b_CanOuputFinal(char *psz_Name);
static void UnloadUniversRef(void);

#define EBRO_C_ListCtrlMaxColumnsOld    8
#define EBRO_C_ListCtrlSpecialOld		7


#define EBRO_C_ListCtrlMaxColumnsNew    9
#define EBRO_C_ListCtrlSpecialNew       8

#define EBRO_C_ListCtrlLastNoEngine     5


#define EBRO_C_GrpCtrlMaxColumns        4

/* Structure that is saved to .ini file */
struct SBrowserIni
{

	struct SBase
	{
		BOOL                b_DialogBarVisible;
		int                 i_NumCopy;
		BOOL                b_IsVisible;
		BOOL                ab_LinkTo[EDI_C_MaxEditors][EDI_C_MaxDuplicate];    /* Linked list */
		BOOL                b_EngineRefresh;                                    /* Need to be
																				* refresh when
																				* engine is
																				* running ? */
		BOOL                b_EditorRefresh;                                    /* Need to be
																				* refresh when
																				* engine is not
																				* running ? */
		int                 i_CountMenu;                                        /* The editor copy
																				* of MEN */
		BOOL                b_TopMenu;                                          /* Top menu visible
																				* ? */
		int                 i_TopInMenu;                                        /* Position of
																				* toolbar in
																				* topmenu */
		BOOL                mb_LeftMode;                                        /* Menu if at left */
		BIG_KEY             aul_History[EDI_C_MaxHistory];                      /* History files */
		GDI_tdst_Resolution st_FullScreenResolution;                            /* Full screen
																				* resolution */
		int					YSplitMenu;
		int					ISplitMenu;
		LONG                al_Dummy[6];
	} mst_base;

	int                 i_HeightFirstPane;
	int                 i_HeightSecondPane;

	int                 ai_WidthListCol[EBRO_C_ListCtrlMaxColumnsOld ];
	int                 ai_NumColListCtrl[EBRO_C_ListCtrlMaxColumnsOld];
	int                 ai_WidthGrpCol[EBRO_C_GrpCtrlMaxColumns];
	int                 ai_NumColGrpCtrl[EBRO_C_GrpCtrlMaxColumns];

	EBRO_tdst_Favorite  ast_Favorites[EBRO_C_MaxFavorites];
	int                 i_NumFavorites;

	int                 i_SortColumnListCtrl;
	int                 i_SortColumnGrpCtrl;

	BOOL                b_DUMMY;
	BOOL                b_EngineMode;

	int                 i_Filter;

	struct
	{
		BOOL    b_RecImp;
		BOOL    b_RecExp;
		BOOL    b_RecIn;
		BOOL    b_RecOut;
		BOOL    b_RecUndoOut;
		BOOL    b_RecGet;
		BOOL    b_AutoOn;
		int     i_AutoFreq;
		BOOL    b_AutoRecImp;
		BOOL    b_AutoRecExp;
		BOOL    b_LinkRefresh;
		int		BBB;
		int		b_MirrorGetLatest;
		int     l[6];
	} mst_Options;

	struct
	{
		BOOL    b_ForceImp;
		BOOL    b_AutoImp;
		char    c_MaterialUpdate;
		char    c_MaterialMerge;
		char    c_GraphicObjectUpdate;
		char    c_GraphicObjectMerge;
		char    c_MaterialNoSingle;
		char    c_dummy[3];
		int     l2[7];
	} mst_MadOptions;

#define EBRO_C_ListModeIcon         1
#define EBRO_C_ListModeSmallIcon    2
#define EBRO_C_ListModeReport       3
	int     i_ListDispMode;
	int     i_GrpDispMode;

	ULONG	i_GroupAutoHide;
	ULONG   l1[7];
};


/* Structure that is saved to .ini file */
struct SBrowserIniNew
{
	struct SBase
	{
		BOOL                b_DialogBarVisible;
		int                 i_NumCopy;
		BOOL                b_IsVisible;
		BOOL                ab_LinkTo[EDI_C_MaxEditors][EDI_C_MaxDuplicate];    /* Linked list */
		BOOL                b_EngineRefresh;                                    /* Need to be
																				* refresh when
																				* engine is
																				* running ? */
		BOOL                b_EditorRefresh;                                    /* Need to be
																				* refresh when
																				* engine is not
																				* running ? */
		int                 i_CountMenu;                                        /* The editor copy
																				* of MEN */
		BOOL                b_TopMenu;                                          /* Top menu visible
																				* ? */
		int                 i_TopInMenu;                                        /* Position of
																				* toolbar in
																				* topmenu */
		BOOL                mb_LeftMode;                                        /* Menu if at left */
		BIG_KEY             aul_History[EDI_C_MaxHistory];                      /* History files */
		GDI_tdst_Resolution st_FullScreenResolution;                            /* Full screen
																				* resolution */
		int					YSplitMenu;
		int					ISplitMenu;
		LONG                al_Dummy[6];
	} mst_base;

	int                 i_HeightFirstPane;
	int                 i_HeightSecondPane;

	int                 ai_WidthListCol[EBRO_C_ListCtrlMaxColumnsNew ];
	int                 ai_NumColListCtrl[EBRO_C_ListCtrlMaxColumnsNew];
	int                 ai_WidthGrpCol[EBRO_C_GrpCtrlMaxColumns];
	int                 ai_NumColGrpCtrl[EBRO_C_GrpCtrlMaxColumns];

	EBRO_tdst_Favorite  ast_Favorites[EBRO_C_MaxFavorites];
	int                 i_NumFavorites;

	int                 i_SortColumnListCtrl;
	int                 i_SortColumnGrpCtrl;

	BOOL                b_DUMMY;
	BOOL                b_EngineMode;

	int                 i_Filter;

	struct
	{
		BOOL    b_RecImp;
		BOOL    b_RecExp;
		BOOL    b_RecIn;
		BOOL    b_RecOut;
		BOOL    b_RecUndoOut;
		BOOL    b_RecGet;
		BOOL    b_AutoOn;
		int     i_AutoFreq;
		BOOL    b_AutoRecImp;
		BOOL    b_AutoRecExp;
		BOOL    b_LinkRefresh;
		int		BBB;
		int		b_MirrorGetLatest;
		int     l[6];
	} mst_Options;

	struct
	{
		BOOL    b_ForceImp;
		BOOL    b_AutoImp;
		char    c_MaterialUpdate;
		char    c_MaterialMerge;
		char    c_GraphicObjectUpdate;
		char    c_GraphicObjectMerge;
		char    c_MaterialNoSingle;
		char    c_dummy[3];
		int     l2[7];
	} mst_MadOptions;

#define EBRO_C_ListModeIcon         1
#define EBRO_C_ListModeSmallIcon    2
#define EBRO_C_ListModeReport       3
	int     i_ListDispMode;
	int     i_GrpDispMode;

	ULONG	i_GroupAutoHide;
	ULONG   l1[7];
} ;



#define BIG_VERSION_36 36
/*
 =======================================================================================================================
    Aim: Update bigfile from version 35 to Version 36
 =======================================================================================================================
 */


void VERsion_ConvertBrowserIni(BIG_INDEX ul_Index,char* _psz_Path,char* _psz_Name)
{
	SWAP_BIG(); // switching to current

	/* Remember key of source file */
	BIG_gul_GlobalKey = BIG_FileKey(ul_Index);
	BIG_gx_GlobalTime = BIG_TimeFile(ul_Index);
	BIG_gpsz_GlobalName = BIG_NameFile(ul_Index);

	SBrowserIni BrowserIniOld;
	SBrowserIniNew BrowserIniNew;

	/* Read the file in the buffer */
	BIG_gp_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), &BIG_gul_Length);
	memcpy(&BrowserIniOld,BIG_gp_Buffer,BIG_gul_Length);

	memcpy(&BrowserIniNew.mst_base,&BrowserIniOld.mst_base,sizeof(BrowserIniOld.mst_base));

	BrowserIniNew.i_HeightFirstPane =  BrowserIniOld.i_HeightFirstPane;
	BrowserIniNew.i_HeightSecondPane =  BrowserIniOld.i_HeightSecondPane;

	for ( int i = 0 ; i < EBRO_C_ListCtrlMaxColumnsOld;i++)
	{
		BrowserIniNew.ai_WidthListCol[i] =  BrowserIniOld.ai_WidthListCol[i];
		BrowserIniNew.ai_NumColListCtrl[i] =  BrowserIniOld.ai_NumColListCtrl[i];
	}

	for ( int i = 0 ; i < EBRO_C_GrpCtrlMaxColumns;i++)
	{
		BrowserIniNew.ai_WidthGrpCol[i] =  BrowserIniOld.ai_WidthGrpCol[i];
		BrowserIniNew.ai_NumColGrpCtrl[i] =  BrowserIniOld.ai_NumColGrpCtrl[i];
	}

	for ( int i = 0; i < EBRO_C_MaxFavorites; i++ ) 
	{
		memcpy ( &BrowserIniNew.ast_Favorites[i],&BrowserIniOld.ast_Favorites[i],sizeof(BrowserIniOld.ast_Favorites[i]));
	}

	BrowserIniNew.i_NumFavorites =  BrowserIniOld.i_NumFavorites;
	BrowserIniNew.i_SortColumnListCtrl =  BrowserIniOld.i_SortColumnListCtrl;
	BrowserIniNew.i_SortColumnGrpCtrl =  BrowserIniOld.i_SortColumnGrpCtrl;
	BrowserIniNew.b_EngineMode =  BrowserIniOld.b_EngineMode;
	BrowserIniNew.b_DUMMY =  BrowserIniOld.b_DUMMY;
	BrowserIniNew.i_Filter =  BrowserIniOld.i_Filter;

	memcpy(&BrowserIniNew.mst_Options,&BrowserIniOld.mst_Options,sizeof(BrowserIniOld.mst_Options));
	memcpy(&BrowserIniNew.mst_MadOptions,&BrowserIniOld.mst_MadOptions,sizeof(BrowserIniOld.mst_MadOptions));

	BrowserIniNew.i_ListDispMode = BrowserIniOld.i_ListDispMode;
	BrowserIniNew.i_GrpDispMode = BrowserIniOld.i_GrpDispMode;

	BrowserIniNew.i_GroupAutoHide = BrowserIniOld.i_GroupAutoHide;;

	for ( int i = 0; i < 7; i ++ )
	{
		BrowserIniNew.l1[i] = BrowserIniOld.l1[i];
	}

	BIG_gp_Buffer = &BrowserIniNew;
	BIG_gul_Length = sizeof(BrowserIniNew);

	SWAP_BIG(); // switching to clean
	BIG_ul_UpdateCreateFile
		(
		_psz_Path,
		_psz_Name,
		NULL,
		FALSE
		);
	SWAP_BIG(); // switching to current
}

// this is a specific version of the clean function 
// il will recreated a new BF_clean and will convert browser*.ini files 
// to support a new colomn in the browser frame
void VERsion_UpdateVersion36()
{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		char		asz_Name[L_MAX_PATH];
		char		asz_Path[BIG_C_MaxLenPath];
		char		asz_Msg[512];
		ULONG		ul_NumFiles, ul_NumDirs, ul_SizeFat;
		BIG_INDEX	ul_Index;
		char		*psz_Name;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		LoadUniversRef();

		M_MF()->BeginWaitCursor();
		EDIA_cl_UPDATEDialog* mpo_Progress = new EDIA_cl_UPDATEDialog("Converting....");
		mpo_Progress->DoModeless();

		/* Count number of files without holes */
		ul_NumFiles = BIG_gst.st_ToSave.ul_MaxFile;
		ul_Index = BIG_gst.st_ToSave.ul_FirstFreeFile;
		while(ul_Index != BIG_C_InvalidIndex)
		{
			ul_NumFiles--;
			BIG_FileChanged(ul_Index) = EDI_FHC_Deleted;
			ul_Index = BIG_NextFile(ul_Index);
		}

		ul_NumDirs = BIG_gst.st_ToSave.ul_MaxDir;
		ul_Index = BIG_gst.st_ToSave.ul_FirstFreeDir;
		while(ul_Index != BIG_C_InvalidIndex)
		{
			ul_NumDirs--;
			BIG_DirChanged(ul_Index) = EDI_FHC_Deleted;
			ul_Index = BIG_NextDir(ul_Index);
		}

		/* Size of fat is the max */
		if(ul_NumDirs > ul_NumFiles)
			ul_SizeFat = ul_NumDirs + 1;
		else
			ul_SizeFat = ul_NumFiles + 1;

		if(ul_NumDirs > ul_SizeFat) 
			ul_SizeFat = ul_NumDirs + 1;


		/* Create a new bigfile to copy into. Allocate the exact fat size */
		L_strcpy(asz_Name, BIG_gst.asz_Name);
		if(L_strrchr(asz_Name, '.')) *L_strrchr(asz_Name, '.') = 0;
		L_strcat(asz_Name, "_convert");
		L_strcat(asz_Name, BIG_Csz_BigExt);

		SWAP_BIG(); // switching to clean
		BIG_CreateEmptyPriv(asz_Name, BIG_VERSION_36, ul_SizeFat, 0, 0, 0);

		sprintf(asz_Msg, "*** Converting to bigfile %s ***", asz_Name);
		LINK_PrintStatusMsg(asz_Msg);

		BIG_Open(asz_Name);

		/* Copy infos of header */
		BIG_gst.st_ToSave.ul_UniverseKey = BIG_gst1.st_ToSave.ul_UniverseKey;
		BIG_WriteHeader();

		/* Write all files */
		SWAP_BIG(); // switching to current
		for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
		{
			if ( (ul_Index % 250) == 0 )
			{
				M_MF()->BeginWaitCursor();
				mpo_Progress->OnRefreshBarText((float)ul_Index/(float)BIG_gst.st_ToSave.ul_MaxFile, "Converting... Please wait");
			}
			
			if(BIG_FileChanged(ul_Index) != EDI_FHC_Deleted)
			{
				BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
				psz_Name = BIG_NameFile(ul_Index);
				

				SWAP_BIG(); // switching to clean

				BIG_ul_CreateDir(asz_Path);
				if(BIG_ul_SearchFileExt(asz_Path, psz_Name) != BIG_C_InvalidIndex)
				{
					sprintf(asz_Msg, "%s/%s", asz_Path, psz_Name);
					ERR_X_Warning(0, "Two files with the same name !!!", asz_Msg);
				}

				
				char* psz_ext;
				/* Find extension of file */
				psz_ext = strchr(psz_Name, '.');
				if(	psz_ext && !strcmp(psz_ext,".ini") && 
					psz_Name && strstr(psz_Name,"Browser" ) != NULL)
				{
					// this will convert browser .ini file and 
					// write buffers into the clean_bf
					VERsion_ConvertBrowserIni(ul_Index,asz_Path,psz_Name);
				}
				else 
				{
					SWAP_BIG(); // switching to clean
					
					// this function will switch to current automaticly
					BIG_ul_ExportMergeFileRec(asz_Path, psz_Name);
				}
			}
		}
		/* Close copy */
		SWAP_BIG(); // switching to clean
		BIG_Close();  
		SWAP_BIG(); // switching to current

		if ( mpo_Progress )
			delete mpo_Progress;
		M_MF()->EndWaitCursor();

		UnloadUniversRef();
		LINK_PrintStatusMsg("... Finished");
}




static BAS_tdst_barray BIG_gst_UniversRef;
static BOOL BIG_gb_UniversRefLoaded=FALSE;

static void LoadUniversRef(void)
{
	ULONG ul_fat,ul_key;
	char	*psz_Temp;
	char	asz_Name[BIG_C_MaxLenPath];
	char	asz_Dir[BIG_C_MaxLenPath];
	char	asz_Path[BIG_C_MaxLenPath];
	char*pc;
	ULONG ul_length;

	if(BIG_gb_UniversRefLoaded) return;

	ul_key = BIG_UniverseKey(); //univers.oin key
	if(ul_key != BIG_C_InvalidKey)
	{
		//get univers.oin fat index 
		ul_fat = BIG_ul_SearchKeyToFat(ul_key);

		//search univers.omd
		L_strcpy(asz_Name, BIG_NameFile(ul_fat));
		psz_Temp = L_strrchr(asz_Name, '.');
		if(psz_Temp) *psz_Temp = 0;
		L_strcpy(asz_Dir, asz_Name);
		L_strcat(asz_Name, EDI_Csz_ExtAIEngineModel);
		BIG_ComputeFullName(BIG_ParentFile(ul_fat), asz_Path);
		psz_Temp = L_strrchr(asz_Path, '/');
		if(psz_Temp) *psz_Temp = 0;
		ul_fat = BIG_ul_SearchFileExt(asz_Path, asz_Name);

		//now load the contents
		if(ul_fat != BIG_C_InvalidIndex)
		{
			BAS_binit(&BIG_gst_UniversRef, 10);

			ul_key = BIG_FileKey(ul_fat);
			pc = BIG_pc_ReadFileTmp(BIG_ul_SearchKeyToPos(ul_key), &ul_length);
			ul_length = ul_length / 8 ;
			while(ul_length)
			{
				if(*(ULONG*)pc && (*(ULONG*)pc != BIG_C_InvalidKey)) 
					BAS_binsert(*(ULONG*)pc, 1, &BIG_gst_UniversRef);

				ul_length--;
				pc += 8;
			}
			BIG_gb_UniversRefLoaded = TRUE;
		}
	}
}

static void UnloadUniversRef(void)
{
	if(BIG_gb_UniversRefLoaded) BAS_bfree(&BIG_gst_UniversRef);
	BIG_gb_UniversRefLoaded = FALSE;
}

#endif
