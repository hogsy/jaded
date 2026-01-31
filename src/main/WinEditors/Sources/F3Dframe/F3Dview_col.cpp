/*$T F3Dview_col.cpp GC! 1.081 10/22/01 17:19:12 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS

/* GC Dependencies 11/22/99 */
#include "BASe/MEMory/Mem.h"
#include "BASe/BAStypes.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"

#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLsave.h"
#include "ENGine/Sources/COLlision/COLload.h"
#include "ENGine/Sources/COLlision/COLinit.h"
#include "ENGine/Sources/COLlision/COLedit.h"
#include "ENGine/Sources/COLlision/COLset.h"
#include "ENGine/Sources/COLlision/COLcob.h"

#include "ENGine/Sources/INTersection/INTSnP.h"

#include "GraphicDK/Sources/GEOmetric/GEOload.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORsave.h"

#include "SOFT/SOFTpickingbuffer.h"
#include "GDInterface/GDInterface.h"

#include "SELection/SELection.h"
#include "DIAlogs/DIAname_dlg.h"

#include "DIAlogs/DIAGameMaterial_dlg.h"

#include "F3Dview.h"
#include "F3Dstrings.h"
#include "EDIpaths.h"
#include "EDIstrings.h"
#include "EDImainframe.h"

#ifdef JADEFUSION
#include "BIGfiles/BIGmdfy_dir.h"
#endif

char	*F3D_STR_Csz_Zone_AI[16] =
{
	"00 - ZDM Pied",
	"01 - ZDE Corps",
	"02 - ZDE Vision",
	"03 - ZDE Action",
	"04 - ZDM Extra",
	"05 - ZDE Saut",
	"06 - ZDE Fight",
	"07 - Zone 07",
	"08 - Zone 08",
	"09 - Zone 09",
	"10 - Zone 10",
	"11 - Zone 11",
	"12 - Zone 12",
	"13 - Zone 13",
	"14 - Zone 14",
	"15 - Zone 15"
};

#ifdef JADEFUSION
extern BIG_INDEX    BIG_CopyFile(char *, char *, char *);
extern void GEO_CreateGeoFromCob(OBJ_tdst_GameObject *, GEO_tdst_Object *, COL_tdst_Cob *);
#else
extern "C" BIG_INDEX    BIG_CopyFile(char *, char *, char *);
extern "C" void GEO_CreateGeoFromCob(OBJ_tdst_GameObject *, GEO_tdst_Object *, COL_tdst_Cob *);
#endif

ULONG	*F3D_dul_Keys;
int	i_InstanceOrCopy = 0;
/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL F3D_b_DuplicateCob(SEL_tdst_SelectedItem *_pst_Sel, ULONG Dummy, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap		*pst_ColMap, *pst_NewColMap;
	COL_tdst_Cob		*pst_Cob, *pst_NewCob;
	COL_tdst_Cob		**dpst_Cob, **dpst_LastCob;
	OBJ_tdst_GameObject *pst_GO;
	UCHAR				uc_Cob;
	BIG_KEY				ul_Key;
	char				asz_Path[500];
	char				asz_CobPath[500];
	char				asz_CobName[200];
	char				asz_Name[BIG_C_MaxLenName];
	char				asz_Name1[BIG_C_MaxLenName];
	BIG_INDEX			ul_CobIndex, ul_ColMapIndex;
	BIG_INDEX			ul_NewCobIndex, ul_CobKey;
	MAI_tdst_WinHandles	*pst_WinHandles;
	int					i, rank;
	ULONG				pos;
	ULONG				ul_Dir;
	UCHAR				uc_Temp;
	char				*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_WinHandles = (MAI_tdst_WinHandles*) p2;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *)_pst_Sel->p_Content;
		if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)) return TRUE;

		pst_ColMap = ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap;
		pst_Cob = pst_ColMap->dpst_Cob[0];
	}
	else
	{
		pst_Cob = (COL_tdst_Cob *)_pst_Sel->p_Content;
		pst_GO = pst_Cob->pst_GO;
		if(!pst_GO) return TRUE;
	}

	uc_Cob = 0;
	while(pst_Cob)
	{
		ul_CobIndex = LOA_ul_SearchIndexWithAddress((ULONG) pst_Cob);
		if(ul_CobIndex == BIG_C_InvalidIndex) 
		{
			LINK_PrintStatusMsg("Cob Unknown during duplication. Abort");
			return TRUE;
		}

		ul_Key = BIG_FileKey(ul_CobIndex);

		/* Current Cob key already registered */
		i = 0;
		rank = -1;
		while(F3D_dul_Keys[i])
		{
			if(ul_Key == F3D_dul_Keys[i])
			{
				if( !i_InstanceOrCopy)
				{
					if
					(
						M_MF()->MessageBox
						(
							"Some Objects share the same cob. Do you want to keep those instances (YES) or create a new cob for each one (NO) ?",
							EDI_STR_Csz_TitleConfirm,
							MB_ICONQUESTION | MB_YESNO
						) == IDYES
					)
						i_InstanceOrCopy = 1;
					else
						i_InstanceOrCopy = 2;
				}
				rank = i;
			}

			i+=2;
		}

		/* Add current Cob key in the array */
		if((i_InstanceOrCopy != 1) || (rank == -1))
		{
			i = 0;
			while(F3D_dul_Keys[i]) i++;
			F3D_dul_Keys[i++] = ul_Key;
		}

		L_strcpy(asz_CobName, BIG_NameFile(ul_CobIndex));
		BIG_ComputeFullName(BIG_ParentFile(ul_CobIndex), asz_CobPath);

		WOR_GetPath(pst_GO->pst_World, asz_Path);
		L_strcat(asz_Path, "/");
		L_strcat(asz_Path, EDI_Csz_Path_COLObjects);

		COL_FreeCob(pst_Cob);

		if((i_InstanceOrCopy == 1) && (rank != -1))
		{
			pos = BIG_ul_SearchKeyToPos(F3D_dul_Keys[rank + 1]);
			((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[uc_Cob] = (COL_tdst_Cob *) LOA_ul_SearchAddress(pos);

			pst_NewCob = ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[uc_Cob];
			pst_NewCob->uw_NbOfInstances ++;
		}
		else
		{
			ul_Dir = BIG_ul_SearchDir(asz_Path);
			if(ul_Dir == BIG_C_InvalidIndex) BIG_ul_CreateDir(asz_Path);

			ul_NewCobIndex = BIG_CopyFile(asz_Path, asz_CobPath, asz_CobName);

			LOA_MakeFileRef
			(
				BIG_FileKey(ul_NewCobIndex),
				(ULONG *) &(((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[uc_Cob]),
				COL_ul_CallBackLoadCob,
				LOA_C_MustExists
			);
			LOA_Resolve();

			F3D_dul_Keys[i] = BIG_FileKey(ul_NewCobIndex);

			pst_NewCob = ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[uc_Cob];

			COL_GMat_UpdateFileName(pst_NewCob);

			pst_NewCob->uw_NbOfInstances = 1;

			if(pst_NewCob->pst_GMatList)
				pst_NewCob->pst_GMatList->ul_NbOfInstances ++;
		}

		// -- Update Selection --
		if(SEL_RetrieveItem(pst_WinHandles->pst_World->pst_Selection, pst_Cob))
		{
			SEL_ReplaceItemContent(pst_WinHandles->pst_World->pst_Selection, pst_Cob, pst_NewCob);
		}

		/* Next Cob if multiple cob on object */
		pst_Cob = NULL;
		if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		{
			uc_Cob ++;
			if(uc_Cob < pst_ColMap->uc_NbOfCob)
			{
				pst_Cob = pst_ColMap->dpst_Cob[uc_Cob];
			}
		}
	}

	/* Create a new ColMap to have a new .map */
	if((GetAsyncKeyState( VK_CONTROL ) < 0) && ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap)
	{
		pst_NewColMap = (COL_tdst_ColMap *) MEM_p_Alloc(sizeof(COL_tdst_ColMap));

		pst_NewColMap->uc_NbOfCob = ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->uc_NbOfCob;
		pst_NewColMap->uc_Activation = ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->uc_Activation;
		pst_NewColMap->uc_CustomBits1 = ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->uc_CustomBits1;
		pst_NewColMap->uc_CustomBits2 = ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->uc_CustomBits2;

		pst_NewColMap->dpst_Cob = (COL_tdst_Cob **)MEM_p_Alloc(pst_NewColMap->uc_NbOfCob * sizeof(COL_tdst_Cob *));
		for(i = 0; i < pst_NewColMap->uc_NbOfCob; i++)
		{
			pst_NewColMap->dpst_Cob[i] = ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[i];
		}

		LOA_DeleteAddress((void *) ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap);

		MEM_Free(((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap);

		((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap = pst_NewColMap;

		WOR_GetSubPath(pst_GO->pst_World, EDI_Csz_Path_COLInstances, asz_Path);

		if(LOA_ul_SearchKeyWithAddress((ULONG) pst_GO) != BIG_C_InvalidIndex)
		{
			ul_ColMapIndex = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
			ul_ColMapIndex = BIG_ul_SearchKeyToFat(ul_ColMapIndex);

			L_strcpy(asz_Name, BIG_NameFile(ul_ColMapIndex));
			psz_Temp = L_strrchr(asz_Name, '.');
			if(psz_Temp) *psz_Temp = 0;
			L_strcat(asz_Name, EDI_Csz_ExtCOLMap);
		}
		else
		{
			L_strcpy(asz_Name, pst_GO->sz_Name);
			psz_Temp = L_strrchr(asz_Name, '.');
			if(psz_Temp) *psz_Temp = 0;
			L_strcat(asz_Name, EDI_Csz_ExtCOLMap);
		}

		while(BIG_ul_SearchFileExt(asz_Path, asz_Name) != BIG_C_InvalidIndex)
		{
			if(L_strlen(asz_Name) == BIG_C_MaxLenName)
			{
				ERR_X_Error(0, "Cannot create duplicated Cob File. Name too long", NULL);
			}

			L_strcpy(asz_Name1, "_");
			L_strcat(asz_Name1, asz_Name);
			L_strcpy(asz_Name, asz_Name1);
		}

		SAV_Begin(asz_Path, asz_Name);

		SAV_Buffer(&pst_NewColMap->uc_NbOfCob, 1);

		/* Save all Cobs activated. */
		uc_Temp = 0xFF;
		SAV_Buffer(&uc_Temp, sizeof(UCHAR));

		SAV_Buffer(&pst_NewColMap->uc_CustomBits1, 1);
		SAV_Buffer(&pst_NewColMap->uc_CustomBits2, 1);

		/* Saves the keys in the ColMap File. */
		dpst_Cob = pst_NewColMap->dpst_Cob;
		dpst_LastCob = dpst_Cob + pst_NewColMap->uc_NbOfCob;
		for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
		{
			ul_CobKey = LOA_ul_SearchKeyWithAddress((ULONG) (*dpst_Cob));
			SAV_Buffer(&ul_CobKey, sizeof(BIG_KEY));
		}

		ul_ColMapIndex = SAV_ul_End();

		LOA_AddAddress(ul_ColMapIndex, (void *) pst_NewColMap);
	}


	return TRUE;
}
/*
=======================================================================================================================
=======================================================================================================================
*/
#ifdef JADEFUSION
void F3D_cl_View::Selection_DuplicateCob(int i_Cob_num, OBJ_tdst_GameObject * _pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob		*pst_Cob, *pst_NewCob;
	OBJ_tdst_GameObject *pst_GO;
	char				asz_Path[500];
	char				asz_NewName[BIG_C_MaxLenName];
	char				asz_CobPath[500];
	char				asz_CobName[200];
	char				*psz_Ext;
	BIG_INDEX			ul_CobIndex;
	BIG_INDEX			ul_NewCobIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(i_Cob_num != -1)
	{
		pst_GO = _pst_GO ? _pst_GO : (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);			
		
		COL_tdst_ColMap *pst_ColMap = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap;
		if(pst_ColMap)
		{
			if(!pst_ColMap->dpst_Cob[i_Cob_num]) return;
			pst_Cob = pst_ColMap->dpst_Cob[i_Cob_num];
			//		pst_Cob = pst_GO->pst_Extended->pst_Col->pst_ColMap->dpst_Cob[i_Cob_num];
		}
	}	
	else	//We have a Cob in the selection list
{
        	pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);
	}

	if(!pst_Cob) return;

	ul_CobIndex = LOA_ul_SearchIndexWithAddress((ULONG) pst_Cob);
	if(ul_CobIndex == BIG_C_InvalidIndex) return;

    BIG_ComputeFullName(BIG_ParentFile(ul_CobIndex), asz_CobPath);

	//pour éviter d'écraser la référence vers le gao dupliqué
	if(i_Cob_num == -1)
        	pst_GO = pst_Cob->pst_GO;

	if
	(
		!pst_GO
	||	!pst_GO->pst_Extended
	||	!pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap
	||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob
	||	(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles)
	)
	{
		return;
	}
	else
	{
		WOR_GetPath(pst_GO->pst_World, asz_Path);
		L_strcat(asz_Path, "/");
		L_strcat(asz_Path, EDI_Csz_Path_COLObjects);

		//create directory if not exist
		BIG_ul_CreateDir(asz_Path);

		L_strcpy(asz_CobName, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_Cob)));

		ul_NewCobIndex = BIG_CopyFile(asz_Path, asz_CobPath, asz_CobName);
		
		strcpy(asz_NewName, pst_GO->sz_Name);
		psz_Ext = L_strrchr(asz_NewName, '.');
		if(psz_Ext)
			*psz_Ext = 0;
		strcat(asz_NewName, EDI_Csz_ExtCOLObject);

		//rename cob avec le nom du GAO sélectionné
		_Try_
			BIG_RenFile(asz_NewName, asz_Path, BIG_NameFile(ul_NewCobIndex));
		_Catch_
			_End_

	
		((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[i_Cob_num == -1 ? 0:i_Cob_num]->uw_NbOfInstances --;

		if(!(((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[i_Cob_num == -1 ? 0:i_Cob_num]->uw_NbOfInstances))
{
			SEL_DelItem(mst_WinHandles.pst_World->pst_Selection, pst_Cob);
			COL_FreeTrianglesIndexedCob(((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[i_Cob_num == -1 ? 0:i_Cob_num]);
		}

		LOA_MakeFileRef
		(
			BIG_FileKey(ul_NewCobIndex),
			(ULONG *) &(((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]),
			COL_ul_CallBackLoadCob,
			LOA_C_MustExists
		);
		LOA_Resolve();

		COL_GMat_UpdateFileName(((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]);

		((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uw_NbOfInstances = 1;


		//Select new Cob
		if((i_Cob_num == -1) && SEL_RetrieveItem(mst_WinHandles.pst_World->pst_Selection, pst_Cob))
		{
			pst_NewCob = (((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]);
			SEL_ReplaceItemContent(mst_WinHandles.pst_World->pst_Selection, pst_Cob, pst_NewCob);
		}
	}

	return;
}
#else
void F3D_cl_View::Selection_DuplicateCob(int i_Cob_num)
{
	LONG	l_NumGOs;
	if(M_F3D_World == NULL) return;

	l_NumGOs = SEL_l_CountItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob);
	if(!l_NumGOs) return;

	F3D_dul_Keys = (ULONG *) MEM_p_Alloc(l_NumGOs * 8);
	L_memset(F3D_dul_Keys, 0, l_NumGOs * 8);
	i_InstanceOrCopy = 0;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_DuplicateCob, -1, (ULONG) &mst_WinHandles);
	i_InstanceOrCopy = 0;
	MEM_Free(F3D_dul_Keys);

	SEL_Close(M_F3D_Sel);
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_BeforeColSetCreation(OBJ_tdst_GameObject *_pst_GO)
{
	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		OBJ_SetIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM);
		OBJ_SetIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDE);

		/* No extended struct, allocate */
		OBJ_GameObject_CreateExtendedIfNot(_pst_GO);

		/* If the Object has no Extended Col Struct, Allocate */
		if(!_pst_GO->pst_Extended->pst_Col)
		{
			_pst_GO->pst_Extended->pst_Col = (COL_tdst_Base *) MEM_p_Alloc(sizeof(COL_tdst_Base));
			L_memset(_pst_GO->pst_Extended->pst_Col, 0, sizeof(COL_tdst_Base));
		}
	}
	else
	{
		if
		(
			M_MF()->MessageBox
				(
					"This object has already a ColSet. Are you sure you want to overwrite it ?",
					EDI_STR_Csz_TitleConfirm,
					MB_ICONQUESTION | MB_YESNO
				) == IDNO
		) return FALSE;
		else
		{
			/* We unload/free the instance. */
			COL_FreeInstance((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col);
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_AfterColSetCreation(OBJ_tdst_GameObject *_pst_GO, BIG_KEY _ul_ColSetKey, BIG_KEY _ul_InstanceKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Pos, ul_Size;
	COL_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * We must be sure that the ColSet file is not empty. If it is the case, we create
	 * a default 1 meter Sphere in it.
	 */
	ul_Pos = BIG_ul_SearchKeyToPos(_ul_ColSetKey);
	ul_Size = BIG_ul_GetLengthFile(ul_Pos);
	if(!ul_Size)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_ColSet st_DefaultColSet;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		st_DefaultColSet.uc_Flag = 0;
		st_DefaultColSet.uc_NbOfZDx = 1;
		st_DefaultColSet.uw_NbOfInstances = 0;
		st_DefaultColSet.past_ZDx = (COL_tdst_ZDx *) MEM_p_Alloc(sizeof(COL_tdst_ZDx));

		/* Init of the default AI-ENG Indexes. */
		st_DefaultColSet.pauc_AI_Indexes = (UCHAR *) MEM_p_Alloc(COL_Cul_MaxNbOfZDx * sizeof(UCHAR));
		L_memset(st_DefaultColSet.pauc_AI_Indexes, 0xFF, COL_Cul_MaxNbOfZDx * sizeof(UCHAR));
		COL_ColSet_AddSphere(st_DefaultColSet.past_ZDx, NULL, NULL);
		st_DefaultColSet.pauc_AI_Indexes[0] = 0;
		COL_SaveColSet(&st_DefaultColSet, _ul_ColSetKey);
	}

	/* Load the instance */
	pst_Instance = COL_pst_LoadInstance(_ul_InstanceKey);
	((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance = pst_Instance;

	/* If GO is not is SnP, add it */
	if(!(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP))
	{
		_pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
		INT_SnP_AddObject(_pst_GO, _pst_GO->pst_World->pst_SnP);
	}

	/* Add Detection List */
	if
	(
		!(_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Detection)
	&&	!(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceDetectionList)
	)
	{
		_pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceDetectionList;
		OBJ_SetStatusFlag(_pst_GO, OBJ_C_StatusFlag_Detection);
		COL_AllocDetectionList(_pst_GO);
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_AddCob(OBJ_tdst_GameObject *_pst_GO, BIG_INDEX _ul_CobIndex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap *pst_ColMap;
	COL_tdst_Cob	*pst_Cob;
	BIG_KEY			ul_Key;
	ULONG			ul_Pos, ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * We must be sure that the Cob file is not empty. If it is the case, we create a
	 * default 1 meter Sphere in it.
	 */
	ul_Pos = BIG_ul_SearchKeyToPos(BIG_FileKey(_ul_CobIndex));
	ul_Size = BIG_ul_GetLengthFile(ul_Pos);
	if(!ul_Size)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Cob	st_DefaultCob;
		COL_tdst_Sphere *pst_Sphere;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		L_memset(&st_DefaultCob, 0, sizeof(COL_tdst_Cob));
		st_DefaultCob.uc_Type = COL_C_Zone_Sphere;
		st_DefaultCob.uc_Flag = 0;
		st_DefaultCob.pst_MathCob = (COL_tdst_Mathematical *) MEM_p_Alloc(sizeof(COL_tdst_Mathematical));
		st_DefaultCob.pst_MathCob->p_Shape = (COL_tdst_Sphere *) MEM_p_Alloc(sizeof(COL_tdst_Sphere));
		st_DefaultCob.pst_MathCob->l_MaterialId = 0;
		pst_Sphere = (COL_tdst_Sphere *) st_DefaultCob.pst_MathCob->p_Shape;
		MATH_InitVector(&pst_Sphere->st_Center, 0.0f, 0.0f, 0.0f);
		pst_Sphere->f_Radius = 0.75f;

		COL_SaveCob(&st_DefaultCob, BIG_FileKey(_ul_CobIndex));

		MEM_Free(st_DefaultCob.pst_MathCob->p_Shape);
		MEM_Free(st_DefaultCob.pst_MathCob);
	}

	if
	(
		_pst_GO
	&&	_pst_GO->pst_Extended
	&&	_pst_GO->pst_Extended->pst_Col
	&&	((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap
	)
	{
		pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;

		/* If Gao has a Triangles-based Cob, we free it. */
		if(pst_ColMap->dpst_Cob[0]->uc_Type == COL_C_Zone_Triangles)
		{
			COL_FreeColMap((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col);
			OBJ_ClearIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap);
		}
	}

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
	{
		/* No extended struct, allocate */
		OBJ_GameObject_CreateExtendedIfNot(_pst_GO);
		OBJ_SetIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap);

		/* If the Object has no Extended Col Struct, Allocate */
		if(!_pst_GO->pst_Extended->pst_Col)
		{
			_pst_GO->pst_Extended->pst_Col = (COL_tdst_Base *) MEM_p_Alloc(sizeof(COL_tdst_Base));
			L_memset(_pst_GO->pst_Extended->pst_Col, 0, sizeof(COL_tdst_Base));
		}

		/* Create a ColMap File with the drag and drop file. */
		ul_Key = COL_ul_CreateColMapFile(_pst_GO->pst_World, _ul_CobIndex, _pst_GO->sz_Name);
		pst_ColMap = COL_pst_LoadColMap(ul_Key);
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap = pst_ColMap;

		pst_Cob = *(pst_ColMap->dpst_Cob);

		if(pst_Cob)
		{
			pst_ColMap->uc_NbOfCob = 1;
			pst_Cob->uw_NbOfInstances++;
		}
		else
		{
			OBJ_ClearIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap);
			MEM_Free(pst_ColMap->dpst_Cob);
			LOA_DeleteAddress(pst_ColMap);
			MEM_Free(pst_ColMap);
			((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap = NULL;
		}
	}
	else
	{
		pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;
		if(pst_ColMap->uc_NbOfCob == COL_Cul_MaxNbOfCob)
		{
			ERR_X_ForceError("Sorry, Max number of Cobs reached", NULL);
			return FALSE;
		}

		LOA_MakeFileRef(BIG_FileKey(_ul_CobIndex), (ULONG *) &pst_Cob, COL_ul_CallBackLoadCob, LOA_C_MustExists);
		LOA_Resolve();

		/* We have DnD a Triangles-based cob. Free mathematical cobs if any */
		if((pst_Cob->uc_Type == COL_C_Zone_Triangles) && (pst_ColMap->uc_NbOfCob > 0))
		{
			COL_FreeColMap((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col);
			OBJ_ClearIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap);
			OBJ_GameObject_CreateExtendedIfNot(_pst_GO);
			OBJ_SetIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap);
			
			/* If the Object has no Extended Col Struct, Allocate */
			if(!_pst_GO->pst_Extended->pst_Col)
			{
				_pst_GO->pst_Extended->pst_Col = (COL_tdst_Base *) MEM_p_Alloc(sizeof(COL_tdst_Base));
				L_memset(_pst_GO->pst_Extended->pst_Col, 0, sizeof(COL_tdst_Base));
			}
			
			pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;
			pst_ColMap->dpst_Cob = (COL_tdst_Cob **) MEM_p_Alloc(sizeof(COL_tdst_Cob *));
			pst_ColMap->dpst_Cob[0] = pst_Cob;
		}
		else
		{
			pst_ColMap->dpst_Cob = (COL_tdst_Cob **) MEM_p_Realloc
				(
					pst_ColMap->dpst_Cob,
					(pst_ColMap->uc_NbOfCob + 1) * sizeof(COL_tdst_Cob *)
				);

			*(pst_ColMap->dpst_Cob + pst_ColMap->uc_NbOfCob) = pst_Cob;
		}

		if(pst_Cob)
		{
			pst_ColMap->uc_NbOfCob++;
			pst_Cob->uw_NbOfInstances++;
		}
	}

	if(pst_Cob)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~*/
		char	asz_GMatName[100];
		/*~~~~~~~~~~~~~~~~~~~~~~*/

		if(pst_Cob->pst_GMatList)
		{
			L_strcpy(asz_GMatName, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_Cob->pst_GMatList)));
			pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(L_strlen(asz_GMatName) + 1);
			L_strcpy(pst_Cob->sz_GMatName, asz_GMatName);
		}
		else
		{
			L_strcpy(asz_GMatName, "None");
			pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(L_strlen(asz_GMatName) + 1);
			L_strcpy(pst_Cob->sz_GMatName, asz_GMatName);
		}
	}

	return TRUE;
}

/*$F
 =======================================================================================================================


									ZONE SELECTION FUNCTIONS


 =======================================================================================================================
 */

void F3D_cl_View::Zone_SetIndex(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ZDx			*pst_ZDx;
	COL_tdst_ColSet			*pst_ColSet;
	EDIA_cl_NameDialogCombo o_Dialog("Select Zone Index");
	UCHAR					uc_Index, i, j;
	BOOL					b_Log;
	char					asz_Log[100];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Log = FALSE;
	pst_ZDx = (COL_tdst_ZDx *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_ZDx);

	/* Gets the ColSet from the ZDx */
	pst_ColSet = ((COL_tdst_Base *) pst_ZDx->pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_ColSet;

	if(!pst_ZDx) return;

	uc_Index = pst_ZDx->uc_AI_Index;
	if(uc_Index < 16) o_Dialog.SetDefault(F3D_STR_Csz_Zone_AI[uc_Index]);

	if(b_Log)
	{
		LINK_PrintStatusMsg("Before change");
		for(j = 0; j < 16; j++)
		{
			sprintf
			(
				asz_Log,
				"Indirection [IA] <-> [ENG]: %s <-> %u",
				F3D_STR_Csz_Zone_AI[j],
				pst_ColSet->pauc_AI_Indexes[j]
			);
			LINK_PrintStatusMsg(asz_Log);
		}
	}

	for(i = 0; i < 16; i++) o_Dialog.AddItem(F3D_STR_Csz_Zone_AI[i]);

	if(o_Dialog.DoModal() == IDOK)
	{
		for(i = 0; i < 16; i++)
		{
			if(!L_strcmpi((char *) (LPCSTR) o_Dialog.mo_Name, F3D_STR_Csz_Zone_AI[i]))
			{
				/* Gets the index of the ZDx in the ColSet. */
				uc_Index = COL_uc_ColSet_GetIndexWithZone(pst_ColSet, pst_ZDx);

				/* We update the array needed to translate AI indexes to real Engine ones. */
				pst_ColSet->pauc_AI_Indexes[i] = uc_Index;

				/*
				 * We dont want the array to have the Same ENG index twice, so, we check if there
				 * is not another one.
				 */
				for(j = 0; j < 16; j++)
				{
					if(j == i) continue;

					if(pst_ColSet->pauc_AI_Indexes[j] == uc_Index) pst_ColSet->pauc_AI_Indexes[j] = 0xFF;
				}

				if(b_Log)
				{
					LINK_PrintStatusMsg("After change");
					for(j = 0; j < 16; j++)
					{
						sprintf
						(
							asz_Log,
							"Indirection [IA] <-> [ENG]: %s <-> %u",
							F3D_STR_Csz_Zone_AI[j],
							pst_ColSet->pauc_AI_Indexes[j]
						);
						LINK_PrintStatusMsg(asz_Log);
					}
				}

				pst_ZDx->uc_AI_Index = i;


				if(pst_ColSet->uw_NbOfInstances)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					COL_tdst_Instance	*pst_Instance;
					int					i, j;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					for(i = 0; i < (int) pst_ColSet->uw_NbOfInstances; i++)
					{
						pst_Instance = pst_ColSet->dpst_Instances[i];
						if(pst_Instance->uc_NbOfSpecific)
						{
							for(j = 0; j < (int) pst_Instance->uc_NbOfSpecific; j++)
							{
								if(pst_Instance->past_Specific[j].pst_ColSetZDx->uc_AI_Index != pst_Instance->past_Specific[j].uc_AI_Index)
								{
									pst_Instance->past_Specific[j].uc_AI_Index = pst_Instance->past_Specific[j].pst_ColSetZDx->uc_AI_Index;
								}
							}
						}
					}
				}

				/* Refresh */
				LINK_UpdatePointers();
				if(mpo_AssociatedEditor) mpo_AssociatedEditor->RefreshMenu();
				return;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_Rename(void)
{
}

/*
 =======================================================================================================================
    Aim:    Delete a Zone from a ColSet.

    Note:   We cannot delete a specific zone from an instance.
 =======================================================================================================================
 */
void F3D_cl_View::Zone_Delete(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet *pst_ColSet;
	COL_tdst_ZDx	*pst_ZDx;
	UCHAR			uc_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Gets the ZDx... */
	pst_ZDx = (COL_tdst_ZDx *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_ZDx);

	/* Gets the ColSet from the ZDx */
	pst_ColSet = ((COL_tdst_Base *) pst_ZDx->pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_ColSet;

	/* Gets the index of the Zone in the instance's array of pointers. */
	uc_Index = COL_uc_ColSet_GetIndexWithZone(pst_ColSet, pst_ZDx);

	if(uc_Index == 255)
	{
		uc_Index = 0;
	}

	/* We cannot delete the last zone of a ColSet */
	if(pst_ColSet->uc_NbOfZDx > 1)
	{
		if(M_MF()->MessageBox("Remove this zone from the current ColSet?", "Jaded", MB_YESNO) == IDYES)
		{
			/* We effectively delete the ZDx from the ColSet */
			COL_ColSet_RemoveZDxWithIndex(pst_ColSet, uc_Index);
			LINK_UpdatePointers();
			LINK_Refresh();
		}
	}
	else
		ERR_X_ForceError("Sorry, cannot delete the last zone of a ColSet", NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_Activate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UCHAR			uc_ENG_Index, uc_AI_Index, i;
	COL_tdst_ZDx	*pst_ZDx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Gets the ZDx... */
	pst_ZDx = (COL_tdst_ZDx *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_ZDx);
	if(pst_ZDx)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_ColSet		*pst_ColSet;
		COL_tdst_Instance	*pst_Instance;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Gets the instance from the ZDx */
		pst_Instance = ((COL_tdst_Base *) pst_ZDx->pst_GO->pst_Extended->pst_Col)->pst_Instance;

		/* Gets the ColSet of the instance. */
		pst_ColSet = pst_Instance->pst_ColSet;

		/* Gets the index of the Zone in the instance's array of pointers. */
		uc_ENG_Index = COL_uc_Instance_GetEngineIndexWithZone(pst_Instance, pst_ZDx);

		uc_AI_Index = 0xFF;
		for(i = 0; i < 16; i++)
		{
			if(pst_ColSet->pauc_AI_Indexes[i] == uc_ENG_Index) uc_AI_Index = i;
		}

		if(uc_AI_Index != 0xFF) COL_Instance_SetActivationFlag(pst_Instance, uc_AI_Index);
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_ColMap *pst_ColMap;
		COL_tdst_Cob	*pst_Cob;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);
		if(!pst_Cob) return;

		/* Gets the ColMap from the Cob */
		pst_ColMap = ((COL_tdst_Base *) pst_Cob->pst_GO->pst_Extended->pst_Col)->pst_ColMap;

		/* Gets the index of the Cob the ColMap. */
		uc_ENG_Index = COL_uc_ColMap_GetEngineIndexWithCob(pst_ColMap, pst_Cob);

		COL_ColMap_SetActivationFlag(pst_ColMap, uc_ENG_Index);
	}

	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_Desactivate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UCHAR			uc_ENG_Index, uc_AI_Index, i;
	COL_tdst_ZDx	*pst_ZDx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Gets the ZDx... */
	pst_ZDx = (COL_tdst_ZDx *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_ZDx);
	if(pst_ZDx)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_ColSet		*pst_ColSet;
		COL_tdst_Instance	*pst_Instance;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Gets the instance from the ZDx */
		pst_Instance = ((COL_tdst_Base *) pst_ZDx->pst_GO->pst_Extended->pst_Col)->pst_Instance;

		/* Gets the ColSet of the instance. */
		pst_ColSet = pst_Instance->pst_ColSet;

		/* Gets the index of the Zone in the instance's array of pointers. */
		uc_ENG_Index = COL_uc_Instance_GetEngineIndexWithZone(pst_Instance, pst_ZDx);

		uc_AI_Index = 0xFF;
		for(i = 0; i < 16; i++)
		{
			if(pst_ColSet->pauc_AI_Indexes[i] == uc_ENG_Index) uc_AI_Index = i;
		}

		if(uc_AI_Index != 0xFF) COL_Instance_ResetActivationFlag(pst_Instance, uc_AI_Index);
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_ColMap *pst_ColMap;
		COL_tdst_Cob	*pst_Cob;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);
		if(!pst_Cob) return;

		/* Gets the ColMap from the Cob */
		pst_ColMap = ((COL_tdst_Base *) pst_Cob->pst_GO->pst_Extended->pst_Col)->pst_ColMap;

		/* Gets the index of the Cob the ColMap. */
		uc_ENG_Index = COL_uc_ColMap_GetEngineIndexWithCob(pst_ColMap, pst_Cob);

		COL_ColMap_ResetActivationFlag(pst_ColMap, uc_ENG_Index);
	}

	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_CheckBV(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ZDx	*pst_ZDx;
	COL_tdst_Cob	*pst_Cob;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	/* Gets the ZDx... */
	pst_ZDx = (COL_tdst_ZDx *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_ZDx);
	if(pst_ZDx)
	{
		if(OBJ_CheckOneBV(pst_ZDx->pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ColMap))
		{
			ERR_X_ForceError("No Problem", NULL);
		}
	}
	else
	{
		pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);
		if(OBJ_CheckOneBV(pst_Cob->pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ColMap))
		{
			ERR_X_ForceError("No Problem", NULL);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_ComputeBV(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob		*pst_Cob;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);

	if(pst_Cob)
	{
		pst_GO = pst_Cob->pst_GO;

		/* Synchronise Cob to its linked Edited Geometric. */
		if(pst_Cob->uc_Flag & COL_C_Cob_Updated) COL_SynchronizeCob(pst_Cob, FALSE,FALSE);

		pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_EditableBV;
		OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_AABBox);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_SwapToSpecific(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ZDx		*pst_ZDx, *pst_CurrentZDx, *pst_LastZDx;
	COL_tdst_ZDx		**dpst_ZDx, **dpst_LastZDx;
	UCHAR				uc_Index, uc_Cpt;
	ULONG				ul_Offset, ul_OldPosition;
	BIG_INDEX			ul_Key;
	BIG_INDEX			ul_Fat;
	char				asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Gets the ZDx... */
	pst_ZDx = (COL_tdst_ZDx *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_ZDx);

	/* Gets the instance from the ZDx */
	pst_Instance = ((COL_tdst_Base *) pst_ZDx->pst_GO->pst_Extended->pst_Col)->pst_Instance;

	/* Gets the ColSet of the instance. */
	pst_ColSet = pst_Instance->pst_ColSet;

	/* Gets the index of the Zone in the instance's array of pointers. */
	uc_Index = COL_uc_ColSet_GetIndexWithZone(pst_ColSet, pst_ZDx);

	/* Allocation/Reallocation. */
	if(!pst_Instance->uc_NbOfSpecific)
		pst_Instance->past_Specific = (COL_tdst_ZDx *) MEM_p_Alloc(sizeof(COL_tdst_ZDx));
	else
	{
		/*
		 * We unregister all the specific zones cause the realloc can change the place of
		 * the array in memory.
		 */
		pst_CurrentZDx = pst_Instance->past_Specific;
		pst_LastZDx = pst_CurrentZDx + pst_Instance->uc_NbOfSpecific;
		for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++) LINK_DelRegisterPointer(pst_CurrentZDx);

		/*
		 * We are about to make a Realloc of the instance's specific array of zones. That
		 * means that all the pointers that points to thoses zones in the instance array
		 * of pointers are going to be obsoletes. So we compute the "offset" of the
		 * realloc in order to be able to recompute the real position of the pointers to
		 * those zones in memory.
		 */
		ul_OldPosition = (ULONG) pst_Instance->past_Specific;
		pst_Instance->past_Specific = (COL_tdst_ZDx *) MEM_p_Realloc
			(
				pst_Instance->past_Specific,
				(pst_Instance->uc_NbOfSpecific + 1) * sizeof(COL_tdst_ZDx)
			);
		ul_Offset = (ULONG) pst_Instance->past_Specific - ul_OldPosition;

		/* We loop thru the instance's array of pointers to update pointers. */
		dpst_ZDx = pst_Instance->dpst_ZDx;
		dpst_LastZDx = dpst_ZDx + pst_Instance->uc_NbOfZDx;
		for(uc_Cpt = 0; dpst_ZDx < dpst_LastZDx; dpst_ZDx++, uc_Cpt++)
		{
			if(COL_b_Instance_IsSpecific(pst_Instance, uc_Cpt))
			{
				ul_OldPosition = (ULONG) * dpst_ZDx;
				*(dpst_ZDx) = (COL_tdst_ZDx *) (ul_OldPosition + ul_Offset);
				(*(dpst_ZDx))->pst_Itself = *(dpst_ZDx);
			}
		}
	}

	/* We get the pointer on the new structure just allocated. */
	pst_CurrentZDx = pst_Instance->past_Specific + pst_Instance->uc_NbOfSpecific;
	*pst_CurrentZDx = *pst_ZDx;

	switch(pst_ZDx->uc_Type)
	{
	case COL_C_Zone_Sphere:
		pst_CurrentZDx->p_Shape = MEM_p_Alloc(sizeof(COL_tdst_Sphere));
		*(COL_tdst_Sphere *) (pst_CurrentZDx->p_Shape) = *(COL_tdst_Sphere *) (pst_ZDx->p_Shape);
		pst_CurrentZDx->uc_Type = pst_ZDx->uc_Type;
		pst_CurrentZDx->uc_Flag = pst_ZDx->uc_Flag;
		pst_CurrentZDx->uc_BoneIndex = pst_ZDx->uc_BoneIndex;
		pst_CurrentZDx->uc_Design = pst_ZDx->uc_Design;
		pst_CurrentZDx->pst_Itself = pst_CurrentZDx;
		pst_CurrentZDx->sz_Name = (char *) MEM_p_Alloc(30);
		strcpy(pst_CurrentZDx->sz_Name, "Specific");
		sprintf(pst_CurrentZDx->sz_Name + L_strlen(pst_CurrentZDx->sz_Name), "@%x", (ULONG) pst_CurrentZDx->p_Shape);
		break;

	case COL_C_Zone_Box:
		pst_CurrentZDx->p_Shape = MEM_p_Alloc(sizeof(COL_tdst_Box));
		*(COL_tdst_Box *) (pst_CurrentZDx->p_Shape) = *(COL_tdst_Box *) (pst_ZDx->p_Shape);
		pst_CurrentZDx->uc_Type = pst_ZDx->uc_Type;
		pst_CurrentZDx->uc_Flag = pst_ZDx->uc_Flag;
		pst_CurrentZDx->uc_BoneIndex = pst_ZDx->uc_BoneIndex;
		pst_CurrentZDx->uc_Design = pst_ZDx->uc_Design;
		pst_CurrentZDx->pst_Itself = pst_CurrentZDx;
		pst_CurrentZDx->sz_Name = (char *) MEM_p_Alloc(30);
		strcpy(pst_CurrentZDx->sz_Name, "Specific");
		sprintf(pst_CurrentZDx->sz_Name + L_strlen(pst_CurrentZDx->sz_Name), "@%x", (ULONG) pst_CurrentZDx->p_Shape);
		break;
	}

	/* We remember the ColSet ZDx; */
	pst_CurrentZDx->pst_ColSetZDx = pst_ZDx;

	/* We set the flag that indicates that this zone is specific in the INSTANCE. */
	COL_Instance_SetSpecificFlag(pst_Instance, uc_Index);

	/* We set the flag that indicates that this zone is specific in the zone itself. */
	COL_Zone_SetFlag(pst_CurrentZDx, COL_C_Zone_Specific);

	/* We update the Instance Array of pointers. */
	COL_Instance_UpdatePointerAtIndex(pst_Instance, pst_CurrentZDx, uc_Index);
	(*(pst_Instance->dpst_ZDx + uc_Index))->pst_Itself = *(pst_Instance->dpst_ZDx + uc_Index);

	/* The number of Specific zones increases. */
	pst_Instance->uc_NbOfSpecific++;

	/* The number of Shared zones decreases. */
	pst_Instance->uc_NbOfShared--;

	/* We have to re-register all the previous specific zones + the new one. */
	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Instance);
	ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
	BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);

	pst_CurrentZDx = pst_Instance->past_Specific;
	pst_LastZDx = pst_CurrentZDx + pst_Instance->uc_NbOfSpecific;
	for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
		LINK_RegisterPointer(pst_CurrentZDx, LINK_C_Zone, BIG_NameFile(ul_Fat), asz_Path);

	LINK_UpdatePointers();
//	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_RestoreColSet(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ZDx		*pst_ZDx, *pst_CurrentZDx, *pst_LastZDx;
	COL_tdst_ZDx		**dpst_ZDx, **dpst_LastZDx;
	UCHAR				uc_Specific, uc_Index, uc_Cpt, uc_Rank;
	ULONG				ul_OldPosition;
	BIG_KEY				ul_Key;
	BIG_INDEX			ul_Fat;
	char				asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Gets the ZDx... */
	pst_ZDx = (COL_tdst_ZDx *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_ZDx);

	/* Gets the instance from the ZDx */
	pst_Instance = ((COL_tdst_Base *) pst_ZDx->pst_GO->pst_Extended->pst_Col)->pst_Instance;

	/* Gets the index of the Zone in the instance's array of pointers. */
	uc_Index = COL_uc_Instance_GetEngineIndexWithZone(pst_Instance, pst_ZDx);

	/* Gets the index of the zone in the specific array of zones */
	uc_Specific = COL_uc_Instance_GetSpecificIndexWithZone(pst_Instance, pst_ZDx);

	/*
	 * We have to unregister all the specific zones because the memcpy will move those
	 * zone pointers in memory.
	 */
	pst_CurrentZDx = pst_Instance->past_Specific;
	pst_LastZDx = pst_CurrentZDx + pst_Instance->uc_NbOfSpecific;
	for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++) LINK_DelRegisterPointer(pst_CurrentZDx);

	/*
	 * If this specific ZDx is derived from a ColSet ZDx, we come back to the ZDx
	 * Shape;
	 */
	if(pst_ZDx->pst_ColSetZDx)
	{
		/* We update the Instance Specific Flag. */
		COL_Instance_ResetSpecificFlag(pst_Instance, uc_Index);

		/* We update the Instance Array of pointers. */
		COL_Instance_UpdatePointerAtIndex(pst_Instance, pst_ZDx->pst_ColSetZDx, uc_Index);
		(*(pst_Instance->dpst_ZDx + uc_Index))->pst_Itself = *(pst_Instance->dpst_ZDx + uc_Index);

		/* Zone's Shape desallocation. */
		MEM_Free((pst_Instance->past_Specific + uc_Specific)->p_Shape);

		/* Free zone's name */
		if((pst_Instance->past_Specific + uc_Specific)->sz_Name)
			MEM_Free((pst_Instance->past_Specific + uc_Specific)->sz_Name);

		if(pst_Instance->uc_NbOfSpecific - uc_Specific - 1)
		{
			/* We move the following specific zones. */
			L_memcpy
			(
				pst_Instance->past_Specific + uc_Specific,
				pst_Instance->past_Specific + uc_Specific + 1,
				(pst_Instance->uc_NbOfSpecific - uc_Specific - 1) * sizeof(COL_tdst_ZDx)
			);

			/*
			 * We have performed a memcpy in the Instance's specific array of zones. We have
			 * to loop thru all this instance's array of pointers to find the pointers that
			 * point to specific zones whose rank were "after" the deleted one to update them.
			 */
			dpst_ZDx = pst_Instance->dpst_ZDx;
			dpst_LastZDx = dpst_ZDx + pst_Instance->uc_NbOfZDx;
			for(uc_Cpt = 0; dpst_ZDx < dpst_LastZDx; dpst_ZDx++, uc_Cpt++)
			{
				if(COL_b_Instance_IsSpecific(pst_Instance, uc_Cpt))
				{
					uc_Rank = COL_uc_Instance_GetSpecificIndexWithZone(pst_Instance, *dpst_ZDx);
					if(uc_Rank < uc_Specific) continue;
					ul_OldPosition = (ULONG) * dpst_ZDx;
					*(dpst_ZDx) = (COL_tdst_ZDx *) (ul_OldPosition - sizeof(COL_tdst_ZDx));
					(*(dpst_ZDx))->pst_Itself = *(dpst_ZDx);

				}
			}
		}

		/* The number of Specific zones decreases. */
		pst_Instance->uc_NbOfSpecific--;

		/* If there is no more specific zone. Free the pointer */
		if(!pst_Instance->uc_NbOfSpecific)
		{
			MEM_Free(pst_Instance->past_Specific);
			pst_Instance->past_Specific = NULL;
		}

		/* The number of Shared zones increases. */
		pst_Instance->uc_NbOfShared++;
	}

	/* We have to re-register all the previous specific zones except the deleted one. */
	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Instance);
	ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
	BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);

	pst_CurrentZDx = pst_Instance->past_Specific;
	pst_LastZDx = pst_CurrentZDx + pst_Instance->uc_NbOfSpecific;
	for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
		LINK_RegisterPointer(pst_CurrentZDx, LINK_C_Zone, BIG_NameFile(ul_Fat), asz_Path);

	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_CreateColSet(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO;
	char					asz_Default[BIG_C_MaxLenName];
	char					*psz_Temp;
	WOR_tdst_World			*pst_World;
	char					asz_ColSetName[BIG_C_MaxLenName];
	EDIA_cl_NameDialogCombo o_Dialog("Enter ColSet Name");
	BIG_KEY					ul_InstanceKey, ul_ColSetKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Gets the GameObject... */
	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	if ( !pst_GO ) return;
	pst_World = pst_GO->pst_World;

	if
	(
		M_MF()->MessageBox
			(
				"You are about to create a new ColSet. Are you sure ?",
				EDI_STR_Csz_TitleConfirm,
				MB_ICONQUESTION | MB_YESNO
			) == IDNO
	) return;
	else
	{
		/* Init before creation. */
		if(!F3D_BeforeColSetCreation(pst_GO)) return;

		L_strcpy(asz_Default, pst_GO->sz_Name);
		psz_Temp = L_strrchr(asz_Default, '.');
		if(psz_Temp) *psz_Temp = 0;

		o_Dialog.SetDefault(asz_Default);
		if((o_Dialog.DoModal() == IDOK) && (o_Dialog.mo_Name))
		{
			L_strcpy(asz_ColSetName, (char *) (LPCSTR) o_Dialog.mo_Name);
			L_strcat(asz_ColSetName, EDI_Csz_ExtCOLSetModel);

			if(BIG_ul_SearchFileExt(EDI_Csz_Path_COLModels, asz_ColSetName) != BIG_C_InvalidIndex)
			{
				ERR_X_ForceError("Sorry, this ColSet alreay exists.", asz_ColSetName);
				return;
			}

			/* Create a dummy instance */
			ul_ColSetKey = BIG_C_InvalidKey;
			ul_InstanceKey = COL_ul_CreateInstanceFile
				(
					pst_World,
					&ul_ColSetKey,
					pst_GO->sz_Name,
					(char *) (LPCSTR) o_Dialog.mo_Name
				);

			/* Load the created file. */
			F3D_AfterColSetCreation(pst_GO, ul_ColSetKey, ul_InstanceKey);

			/* Update GO */
			LINK_UpdatePointer(pst_GO);
			LINK_UpdatePointers();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_CreateCob(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO;
	char					asz_Default[BIG_C_MaxLenName];
	char					*psz_Temp;
	WOR_tdst_World			*pst_World;
	char					asz_CobName[BIG_C_MaxLenName];
	char					asz_PathToCob[BIG_C_MaxLenPath];
	EDIA_cl_NameDialogCombo o_Dialog("Enter Cob Name");
	BIG_KEY					ul_CobKey;
	char					*psz_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Gets the GameObject... */
	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	psz_Name = pst_GO->sz_Name;
	pst_World = pst_GO->pst_World;
	psz_Name = pst_World->sz_Name;

	if
	(
		M_MF()->MessageBox
			(
				"You are about to create a new Cob. Are you sure ?",
				EDI_STR_Csz_TitleConfirm,
				MB_ICONQUESTION | MB_YESNO
			) == IDNO
	) return;
	else
	{
		L_strcpy(asz_Default, pst_GO->sz_Name);
		psz_Temp = L_strrchr(asz_Default, '.');
		if(psz_Temp) *psz_Temp = 0;

		o_Dialog.SetDefault(asz_Default);

		if((o_Dialog.DoModal() == IDOK) && (o_Dialog.mo_Name))
		{
			L_strcpy(asz_CobName, (char *) (LPCSTR) o_Dialog.mo_Name);
			L_strcat(asz_CobName, EDI_Csz_ExtCOLObject);
			WOR_GetSubPath(pst_World, EDI_Csz_Path_COLObjects, asz_PathToCob);

			if(BIG_ul_SearchFileExt(asz_PathToCob, asz_CobName) != BIG_C_InvalidIndex)
			{
				ERR_X_ForceError("Sorry, this Cob alreay exists.", asz_CobName);
				return;
			}

			ul_CobKey = COL_ul_CreateCobFile(pst_World, (char *) (LPCSTR) o_Dialog.mo_Name);

			F3D_AddCob(pst_GO, BIG_ul_SearchKeyToFat(ul_CobKey));

			pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
			INT_SnP_AddObject(pst_GO, pst_World->pst_SnP);
			INT_SnP_InsertionSort(pst_World->pst_SnP, 0);

		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_UpdateCob(BOOL _b_KeepIDMaterial)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	COL_tdst_ColMap		*pst_ColMap;
	COL_tdst_Cob		*pst_Cob;
	GEO_tdst_Object		*pst_GeoCob;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Gets the GameObject... */
	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);

	if
	(
		!pst_GO
	||	!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ColMap)
	||	!pst_GO->pst_Extended
	||	!pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap
	) return;

	pst_ColMap = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap;

	pst_Cob = pst_ColMap->dpst_Cob[0];

	/* anti bug */
	pst_Cob->pst_GO = pst_GO;

	if(!pst_Cob->p_GeoCob)
	{
		pst_Cob->p_GeoCob = MEM_p_Alloc(sizeof(GEO_tdst_Object));

		pst_GeoCob = (GEO_tdst_Object *) pst_Cob->p_GeoCob;

		/* Fill the st_GeoCob structure with info needed to see the Cob. */
		L_memset(pst_GeoCob, 0, sizeof(GEO_tdst_Object));

		pst_GeoCob->st_Id.i = &GRO_gast_Interface[GRO_Geometric];
		GEO_CreateGeoFromCob(pst_Cob->pst_GO, pst_GeoCob, pst_Cob);
	}

	COL_SynchronizeCob(pst_Cob, TRUE,_b_KeepIDMaterial);
	LINK_UpdatePointer(pst_Cob);

	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_SelectObject(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob	*pst_Cob;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);
	if(!pst_Cob || !pst_Cob->pst_GO) return;
	
	Selection_b_Treat
	(
		pst_Cob->pst_GO,
		(long) pst_Cob->pst_GO,
		SEL_C_SIF_Object,
		FALSE
	);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_UpdateOK3(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob	*pst_Cob;
	GEO_tdst_Object	*pst_GeoCob;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);
	if(!pst_Cob) return;

	if(!pst_Cob->p_GeoCob)
	{
		pst_Cob->p_GeoCob = MEM_p_Alloc(sizeof(GEO_tdst_Object));

		pst_GeoCob = (GEO_tdst_Object *) pst_Cob->p_GeoCob;

		/* Fill the st_GeoCob structure with info needed to see the Cob. */
		L_memset(pst_GeoCob, 0, sizeof(GEO_tdst_Object));

		pst_GeoCob->st_Id.i = &GRO_gast_Interface[GRO_Geometric];
		GEO_CreateGeoFromCob(pst_Cob->pst_GO, pst_GeoCob, pst_Cob);
	}

	COL_SynchronizeCob(pst_Cob, FALSE,FALSE);
	COL_OK3_Build(pst_Cob->pst_GO, TRUE, TRUE);
}

#ifdef JADEFUSION
extern void COL_CreateGroFromCob(COL_tdst_Cob *);
#else
extern "C" void COL_CreateGroFromCob(COL_tdst_Cob *);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_CreateGro(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob	*pst_Cob;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);
	if(!pst_Cob) return;

	COL_CreateGroFromCob(pst_Cob);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Zone_DestroyGameMaterial(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	COL_tdst_ColMap		*pst_ColMap;
	COL_tdst_Cob		*pst_Cob;
	UCHAR				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Gets the GameObject... */
	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);

	if
	(
		!pst_GO
	||	!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ColMap)
	||	!pst_GO->pst_Extended
	||	!pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap
	) return;

	pst_ColMap = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap;

	for(i = 0; i < pst_ColMap->uc_NbOfCob; i++)
	{
		pst_Cob = pst_ColMap->dpst_Cob[i];
		LINK_UpdatePointer(pst_Cob);
		COL_FreeGameMaterial(pst_Cob);
		COL_GMat_UpdateFileName(pst_Cob);
		pst_Cob->uc_Flag &= ~COL_C_Cob_GameMat;
	}

	LINK_UpdatePointers();
	LINK_Refresh();
}

/*$F
 =======================================================================================================================


									DRAG AND DROP FUNCTIONS


 =======================================================================================================================
 */

void F3D_cl_View::DropColSet(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject				*pst_GO;
	WOR_tdst_World					*pst_World;
	GDI_tdst_Device					*pst_Dev;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	BIG_KEY							ul_ColSetKey, ul_InstanceKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->FatHasChanged();

	/* Find object where to drop ColSet */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);
	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();
	if(!pst_GO) return;
	pst_World = pst_GO->pst_World;

	/* Init before creation. */
	if(!F3D_BeforeColSetCreation(pst_GO)) return;

	/* Create a dummy instance */
	ul_ColSetKey = BIG_FileKey(_pst_DragDrop->ul_FatFile);
	ul_InstanceKey = COL_ul_CreateInstanceFile(pst_World, &ul_ColSetKey, pst_GO->sz_Name, NULL);

	/* Load the created file. */
	F3D_AfterColSetCreation(pst_GO, ul_ColSetKey, ul_InstanceKey);

	/* Update GO */
	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();

	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    We have just drag and drop a Collision object File.
 =======================================================================================================================
 */
void F3D_cl_View::DropCob(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject				*pst_GO;
	WOR_tdst_World					*pst_World;
	GDI_tdst_Device					*pst_Dev;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->FatHasChanged();

	/* Find object where to drop Collision object */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);
	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();
	if(!pst_GO) return;
	pst_World = mst_WinHandles.pst_World;

	F3D_AddCob(pst_GO, _pst_DragDrop->ul_FatFile);

	pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
	INT_SnP_AddObject(pst_GO, pst_World->pst_SnP);
	INT_SnP_InsertionSort(pst_World->pst_SnP, 0);


	/* Update GO */
	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropZone(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject	*pst_GO;
	MATH_tdst_Vector	v, v2;
	GDI_tdst_Device		*pst_Dev;
	CAM_tdst_Camera		*pst_Cam;
	COL_tdst_ZDx		*pst_ZDx;
	COL_tdst_ColSet		*pst_ColSet;
	UCHAR				uc_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Pick to know pos */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);
	v.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	v.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	b_PickDepth(&v, &v.z);
	pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
	v.x = (float) _pst_DragDrop->o_Pt.x;
	v.y = (float) _pst_DragDrop->o_Pt.y;
	CAM_2Dto3D(pst_Cam, &v, &v);

	pst_ZDx = (COL_tdst_ZDx *) _pst_DragDrop->i_Param2;
	if(COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_Specific))
	{
		ERR_X_ForceError("Sorry, cannot add a specific zone this way", NULL);
		return;
	}

	pst_GO = pst_ZDx->pst_GO;
	pst_ColSet = ((COL_tdst_Base *) pst_ZDx->pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_ColSet;
	MATH_SubVector(&v2, &v, OBJ_pst_GetAbsolutePosition(pst_GO));

	if(M_MF()->MessageBox("Add a zone to the current ColSet?", "Jaded", MB_YESNO) == IDYES)
		COL_ColSet_AddZDx(pst_ColSet, (USHORT) COL_Zone_GetType(pst_ZDx), pst_ZDx, &v2);

	/* Gets the index of the Zone in the instance's array of pointers. */
	uc_Index = COL_uc_ColSet_GetIndexWithZone(pst_ColSet, pst_ColSet->past_ZDx + pst_ColSet->uc_NbOfZDx);

//	COL_Instance_ResetSpecificFlag(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance, uc_Index);
//	COL_Zone_ResetFlag(pst_ColSet->past_ZDx + pst_ColSet->uc_NbOfZDx - 1, COL_C_Zone_Specific);



	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropGameMaterial(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Device					*pst_Dev;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	OBJ_tdst_GameObject				*pst_GO;
	COL_tdst_Cob					*pst_Cob;
	COL_tdst_ColMap					*pst_ColMap;
	GEO_tdst_GraphicZone			*pst_GZone;
	BIG_KEY							ul_GMatKey;
	char							asz_Log[200];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Pick to know pos */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);

	ul_GMatKey = BIG_FileKey(_pst_DragDrop->ul_FatFile);
	if(ul_GMatKey == BIG_C_InvalidKey) return;

	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Tolerance = 0;

	if(mst_WinHandles.pst_DisplayData->ul_DisplayFlags & GDI_Cul_DF_ShowCOB)
	{
		pst_Query->l_Filter = SOFT_Cuc_PBQF_Zone;
		pst_GZone = mst_WinHandles.pst_DisplayData->st_DisplayedZones.dpst_GraphicZone + (ULONG) ul_PickExt();
		if(!pst_GZone || (pst_GZone->uc_Type != GEO_Cul_GraphicCob))
		{
			ERR_X_ForceError("Please pick a Cob.", NULL);
			return;
		}

		pst_Cob = (COL_tdst_Cob *) pst_GZone->pv_Data;
	}
	else
	{
		pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
		pst_Query->l_Tolerance = 0;
		pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();
		if
		(
			!pst_GO
		||	!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ColMap)
		||	!pst_GO->pst_Extended
		||	!pst_GO->pst_Extended->pst_Col
		||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap
		)
		{
			ERR_X_ForceError("This GameObject has no ColMap.", NULL);
			return;
		}

		pst_ColMap = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap;
		if(pst_ColMap->uc_NbOfCob != 1)
		{
			ERR_X_ForceError("This GameObject has more than one Cob.", NULL);
			return;
		}

		pst_Cob = pst_ColMap->dpst_Cob[0];
	}

	if(pst_Cob->pst_GMatList)
	{
		/* If we have DnD the same material, we do nothing ... */
		if(LOA_ul_SearchKeyWithAddress((ULONG) pst_Cob->pst_GMatList) == ul_GMatKey)
		{
			ERR_X_ForceError("This Cob has already THIS Game Material.", NULL);
			return;
		}

		COL_FreeGameMaterial(pst_Cob);
	}

	/*
	 * Load the GameMaterial if needed or just get the pointer if it has been already
	 * loaded.
	 */
	LOA_MakeFileRef(ul_GMatKey, (ULONG *) &pst_Cob->pst_GMatList, COL_ul_CallBackLoadGameMaterial, LOA_C_MustExists);
	LOA_Resolve();

	/* We may have loaded a corrupted Game Material File. */
	if(!pst_Cob->pst_GMatList) return;

	/* Sets the Cob flag that indicates that this cob has a Game Material. */
	pst_Cob->uc_Flag |= COL_C_Cob_GameMat;

	/*
	 * The Cob may have just been created and so, there can be no file on BigFile for
	 * it yet.
	 */
	if(LOA_ul_SearchIndexWithAddress((ULONG) pst_Cob) != BIG_C_InvalidKey)
	{
		sprintf
		(
			asz_Log,
			"Associate %s to %s",
			BIG_NameFile(BIG_ul_SearchKeyToFat(ul_GMatKey)),
			BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_Cob))
		);

		LINK_PrintStatusMsg(asz_Log);
	}

	/* Update Game Maetrial Name. */
	if(pst_Cob->sz_GMatName) MEM_Free(pst_Cob->sz_GMatName);
	pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(L_strlen(BIG_NameFile(BIG_ul_SearchKeyToFat(ul_GMatKey))) + 1);
	L_strcpy(pst_Cob->sz_GMatName, BIG_NameFile(BIG_ul_SearchKeyToFat(ul_GMatKey)));

	pst_Cob->pst_GMatList->ul_NbOfInstances++;

	LINK_UpdatePointer(pst_Cob);
	LINK_UpdatePointers();
	LINK_Refresh();
}


void F3D_cl_View::GameMaterial(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	sz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpo_GameMaterialDialog)
	{
		mpo_GameMaterialDialog->DestroyWindow();
		delete mpo_GameMaterialDialog;
		mpo_GameMaterialDialog = NULL;
	}
	else
	{
		mpo_GameMaterialDialog = new EDIA_cl_GameMaterial(this, NULL);

		L_strcpy(sz_Path, EDI_Csz_Path_COLGameMaterials);
		mpo_GameMaterialDialog->msz_Path = sz_Path;

		mpo_GameMaterialDialog->DoModeless();
	}
}


#endif /* ACTIVE_EDITORS */
