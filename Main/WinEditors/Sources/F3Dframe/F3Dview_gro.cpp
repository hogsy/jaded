/*$T F3Dview_gro.cpp GC! 1.081 08/01/01 15:46:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "EDIpaths.h"

#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_file.h"

#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORaccess.h"

#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEOload.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "STRing/STRstruct.h"
#include "PArticleGenerator/PAGstruct.h"
#include "SOFT/SOFTpickingbuffer.h"

#include "F3Dframe/F3Dview_undo.h"
#include "F3Dframe/F3Dstrings.h"

#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAdropgro_dlg.h"
#include "DIAlogs/DIAcreategeometry_dlg.h"

#ifdef JADEFUSION
#include "DIAlogs/DIAfile_dlg.h"
#include <map>
#endif
extern ULONG	*F3D_dul_Keys;
extern int	i_InstanceOrCopy;


static BOOL b_GenericNames = 0;

/*$4
 ***********************************************************************************************************************
    Internal functions
 ***********************************************************************************************************************
 */

/*$F
 =======================================================================================================================
    To know where GameObject is in the bigfile (used by duplication function) 
    Return value azre:
        F3D_Duplicate_WhenInMyBank      in current world dir if world is in bank
        F3D_Duplicate_WhenInOtherBank   in a bank dir (not current) 
        F3D_Duplicate_WhenInMyLevel     in current world dir if world is a level 
        F3D_Duplicate_WhenInOtherLevel  in a level dir (not current one) 
        0 elsewhere !!!!!
 =======================================================================================================================
 */

ULONG F3D_GRO_ul_GetLocation(OBJ_tdst_GameObject *_pst_GO, GRO_tdst_Struct *_pst_Gro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX		ul_GroIndex, ul_DirIndex, ul_WorldIndex;
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	ul_GroIndex = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Gro);
	ul_WorldIndex = LOA_ul_SearchIndexWithAddress((ULONG) pst_World);
	ul_DirIndex = BIG_ul_SearchDir(EDI_Csz_Path_Objects);

	if(BIG_b_IsFileInDirRec(ul_DirIndex, ul_GroIndex))
	{
		if(BIG_b_IsFileInDirRec(BIG_ParentFile(ul_WorldIndex), ul_GroIndex)) return F3D_Duplicate_WhenInMyBank;
		return F3D_Duplicate_WhenInOtherBank;
	}
	else
	{
		ul_DirIndex = BIG_ul_SearchDir(EDI_Csz_Path_Levels);
		if(!BIG_b_IsFileInDirRec(ul_DirIndex, ul_GroIndex)) return 0;
		if(BIG_b_IsFileInDirRec(BIG_ParentFile(ul_WorldIndex), ul_GroIndex)) return F3D_Duplicate_WhenInMyLevel;
		return F3D_Duplicate_WhenInOtherLevel;
	}
}

/*$F
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_GRO_Rename(GRO_tdst_Struct * _pst_Gro, char * _asz_Path, char * _asz_Name)
{
    EDIA_cl_NameDialog   o_Dialog(F3D_STR_Csz_NewName);
    char                 asz_NewName[BIG_C_MaxLenName];
    char               * sz_Ext;
    ULONG                ul_Index;

    o_Dialog.mo_Title = "Enter ";
    o_Dialog.mo_Title += _pst_Gro->i->pfnsz_FileExtension();
    o_Dialog.mo_Title += " name";

    if (_asz_Name)
    {
        L_strcpy(asz_NewName, _asz_Name);
        sz_Ext = L_strrchr(asz_NewName, '.');
        if (sz_Ext)
            *sz_Ext = 0;

        o_Dialog.mo_Name = asz_NewName;
    }

    if(o_Dialog.DoModal() == IDOK)
    {
        if(L_strlen((LPSTR)(LPCSTR)o_Dialog.mo_Name) > 0)
        {
            if(BIG_b_CheckName((LPSTR)(LPCSTR) o_Dialog.mo_Name))
            {
                ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Gro);

                L_strcpy(asz_NewName, (LPSTR)(LPCSTR) o_Dialog.mo_Name);
                sz_Ext = L_strrchr(asz_NewName, '.');
                if (sz_Ext)
                    *sz_Ext = 0;
                L_strcat(asz_NewName, _pst_Gro->i->pfnsz_FileExtension());

_Try_
                BIG_RenFile(asz_NewName, _asz_Path, BIG_NameFile(ul_Index));
_Catch_
_End_
               GRO_Struct_SetName(_pst_Gro, asz_NewName);
            }
        }
    }
}


/*$F
 =======================================================================================================================
    Duplicate gro data of a game object 
    _ul_Flags can be a combination of follownig values: 
        type:   F3D_Duplicate_Light             to duplicate light 
                F3D_Duplicate_Geometry          to duplicate graphic object
                F3D_Duplicate_Material          to duplicate material (don't work with multi material) 
        place:  F3D_Duplicate_AllLocation       duplicate Gro wherever it is 
                F3D_Duplicate_WhenInMyBank      duplicate only when gro is in current world dir (if world is in bank) 
                F3D_Duplicate_WhenInOtherBank   duplicate only when gro is in a bank dir (not current) 
                F3D_Duplicate_WhenInMyLevel     duplicate only when gro is in current world dir (if world is a level) 
                F3D_Duplicate_WhenInOtherLevel  duplicate only when gro is in a level dir (not current one) 
                
    Gro used by gameobject are change to duplicates gro.
 =======================================================================================================================
 */
#ifdef JADEFUSION
BOOL b_IsOK = FALSE;
char sz_NewPath[BIG_C_MaxLenPath];
typedef std::map<BIG_KEY, GRO_tdst_Struct *> tGrmMap;
tGrmMap DupGRMMap;
#endif

void F3D_GRO_Duplicate(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct *pst_Gro, *pst_NewGro;
	char			asz_Path[BIG_C_MaxLenPath];
	WOR_tdst_World	*pst_World;
	BOOL			b_Always;
	int				i, rank;
	ULONG			ul_GroIndex, ul_GroKey;
	ULONG			pos;
#ifdef JADEFUSION
	char			asz_NewName[BIG_C_MaxLenName];
	ULONG           ul_Index;
	ULONG			ul_Dir;
	CString			strTemp;
	BIG_KEY			ul_Key;
	BOOL			b_KeepInst;
#endif

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	//ul_WorldKey = _pst_GO->ul_InitialWorldKey;
	b_Always = ((_ul_Flags & F3D_Duplicate_AllLocation) == F3D_Duplicate_AllLocation);

#ifdef JADEFUSION
	//Get GAO name without extension
	strcpy(asz_NewName, _pst_GO->sz_Name);
	char *psz_Tmp = strrchr(asz_NewName, '.');
	if(psz_Tmp)	*psz_Tmp = 0;
#endif

	/* Light */
	if((_ul_Flags & F3D_Duplicate_Light) && (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights))
	{
		pst_Gro = (GRO_tdst_Struct *) _pst_GO->pst_Extended->pst_Light;
		if(pst_Gro && (b_Always || (F3D_GRO_ul_GetLocation(_pst_GO, pst_Gro) & _ul_Flags)))
		{
			WOR_GetGroPath(pst_World, asz_Path);
			pst_NewGro = (GRO_tdst_Struct *) pst_Gro->i->pfnp_Duplicate(pst_Gro, asz_Path, NULL, 0);
			if(pst_NewGro)
			{
#ifdef JADEFUSION
				//renomme le nouveau LGT avec le nom du GAO
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_NewGro);
				//strcat(asz_NewName, EDI_Csz_ExtGraphicObject);
				strcat(asz_NewName, pst_Gro->i->pfnsz_FileExtension());
				_Try_
					BIG_RenFile(asz_NewName, asz_Path, BIG_NameFile(ul_Index));
				_Catch_
					_End_
					GRO_Struct_SetName(pst_NewGro, asz_NewName);
				sprintf(asz_Path, "Duplicate %s to %s", GRO_sz_Struct_GetName(pst_Gro), asz_NewName);
				LINK_PrintStatusMsg(asz_Path);
#endif
				pst_Gro->i->pfn_AddRef(pst_Gro, -1);
				_pst_GO->pst_Extended->pst_Light = pst_NewGro;
				TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicObjectsTable, pst_NewGro);
				pst_NewGro->i->pfn_AddRef(pst_NewGro, 2);
				// Rename
                F3D_GRO_Rename(pst_NewGro, asz_Path, _pst_GO->sz_Name);
			}
		}
	}

	/* Graphic object */
	if((_ul_Flags & F3D_Duplicate_Geometry) && (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu))
	{
		pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
		if(pst_Gro && (b_Always || (F3D_GRO_ul_GetLocation(_pst_GO, pst_Gro) & _ul_Flags)))
		{
			/* ----------------------------------------------------------------------------------------------------- */
			/* ----------------------------------------------------------------------------------------------------- */
			/* ------  CODE ADDED FOR MULTIPLE SELECTION GRO DUPLICATION TO KEEP INSTANCES AFTER DUPLICATION ------- */
			/* ----------------------------------------------------------------------------------------------------- */
			/* ----------------------------------------------------------------------------------------------------- */
			ul_GroIndex = LOA_ul_SearchIndexWithAddress((ULONG) pst_Gro);
			ul_GroKey = BIG_FileKey(ul_GroIndex);

			/* Current GRO key already registered in the Array*/
			i = 0;
			rank = -1;
			while(F3D_dul_Keys[i])
			{
				if(ul_GroKey == F3D_dul_Keys[i])
				{
					if( !i_InstanceOrCopy)
					{
						if
						(
							M_MF()->MessageBox
							(
								"Some Objects share the same GRO. Do you want to keep those instances (YES) or create a new GRO for each one (NO) ?",
								"Please confirm",
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

			/* Add current Gro key in the array */
			if((i_InstanceOrCopy != 1) || (rank == -1))
			{
				i = 0;
				while(F3D_dul_Keys[i]) i++;
				F3D_dul_Keys[i++] = ul_GroKey;
			}

			
			if((i_InstanceOrCopy == 1) && (rank != -1))
			{
				if(F3D_dul_Keys[rank + 1] == 0xFFFFFFFF)
				{
					sprintf(asz_Path, "Can't duplicate this geometry %s", GRO_sz_Struct_GetName(pst_Gro));
					LINK_PrintStatusMsg(asz_Path);
				}
				else
				{
					pst_Gro->i->pfn_AddRef(pst_Gro, -1);
					pos = BIG_ul_SearchKeyToPos(F3D_dul_Keys[rank + 1]);
					_pst_GO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) LOA_ul_SearchAddress(pos);
					pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;

					pst_Gro->i->pfn_AddRef(pst_Gro, 1);
				}
			}
			/* ------------------------------ */
			/* ------------------------------ */
			/* ------  END ADDED CODE ------- */
			/* ------------------------------ */
			/* ------------------------------ */
			else
			{
				WOR_GetGroPath(pst_World, asz_Path);
				pst_NewGro = (GRO_tdst_Struct *) pst_Gro->i->pfnp_Duplicate(pst_Gro, asz_Path, NULL, 0);
				F3D_dul_Keys[i] = 0xFFFFFFFF;
				if(pst_NewGro)
				{
					ul_GroIndex = LOA_ul_SearchIndexWithAddress((ULONG) pst_NewGro);
					ul_GroKey = BIG_FileKey(ul_GroIndex);
					F3D_dul_Keys[i] = ul_GroKey;

					pst_Gro->i->pfn_AddRef(pst_Gro, -1);
					_pst_GO->pst_Base->pst_Visu->pst_Object = pst_NewGro;
					TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicObjectsTable, pst_NewGro);
					pst_NewGro->i->pfn_AddRef(pst_NewGro, 2);

					// Rename
					if(!b_GenericNames)
						F3D_GRO_Rename(pst_NewGro, asz_Path, _pst_GO->sz_Name);
				}
				else
				{
					sprintf(asz_Path, "Can't duplicate this geometry %s", GRO_sz_Struct_GetName(pst_Gro));
					LINK_PrintStatusMsg(asz_Path);
				}
			}
		}
	}

	/* Graphic material */
	if((_ul_Flags & F3D_Duplicate_Material) && (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu))
	{
		pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Material;
		if(pst_Gro && (b_Always || (F3D_GRO_ul_GetLocation(_pst_GO, pst_Gro) & _ul_Flags)))
		{
#ifdef JADEFUSION
			//test SHIFT key for alternate path
			if(GetAsyncKeyState(VK_SHIFT) < 0)
#else
			WOR_GetGrmPath(pst_World, asz_Path);
			pst_NewGro = (GRO_tdst_Struct *) pst_Gro->i->pfnp_Duplicate(pst_Gro, asz_Path, NULL, _ul_Flags );
			if(pst_NewGro)
#endif
			{
#ifdef JADEFUSION
				EDIA_cl_FileDialog o_FileDlg("Choose a new path", 2, FALSE, TRUE);

				if(o_FileDlg.DoModal() == IDOK)
				{
					b_IsOK = TRUE;
					strcpy(asz_Path, o_FileDlg.masz_FullPath);
					strcpy(sz_NewPath, asz_Path);
				}
				else
					return;
#else
				pst_Gro->i->pfn_AddRef(pst_Gro, -1);
				_pst_GO->pst_Base->pst_Visu->pst_Material = pst_NewGro;
				TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicObjectsTable, pst_NewGro);
				pst_NewGro->i->pfn_AddRef(pst_NewGro, 2);
				// Rename
                F3D_GRO_Rename(pst_NewGro, asz_Path, _pst_GO->sz_Name);
#endif
			}
			else
			{
#ifdef JADEFUSION
				if(b_IsOK)
					strcpy(asz_Path, sz_NewPath);
				else
                	WOR_GetGrmPath(pst_World, asz_Path);
			}

			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG)pst_Gro);
			
			b_KeepInst = (_ul_Flags & F3D_Duplicate_Material_KeepInst) ? TRUE: FALSE;

			tGrmMap::iterator it = DupGRMMap.find(ul_Key);

			//if keepinst is false, we alway want to duplicate the material
			//if it's true, duplicate only if it's not already made
			if(!b_KeepInst || (b_KeepInst && it == DupGRMMap.end()))
			{
				//not in map
				pst_NewGro = (GRO_tdst_Struct *) pst_Gro->i->pfnp_Duplicate(pst_Gro, asz_Path, asz_NewName, _ul_Flags);
				if(pst_NewGro)
				{
					//add pointer to map
					DupGRMMap[ul_Key] = pst_NewGro;

					ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_NewGro);

					//renomme le nouveau GRM avec le nom du GRO + "_Xe"
					if(b_KeepInst)
					{
						strcpy(asz_NewName, GRO_sz_Struct_GetName(pst_Gro));
						strcat(asz_NewName, "_Xe");
					}
					strcat(asz_NewName, pst_Gro->i->pfnsz_FileExtension());
					//we don't want two multi-material with the same name in the same dir 
					strTemp =  asz_NewName;
					ul_Dir = BIG_ul_SearchDir( asz_Path );
					if (ul_Dir != BIG_C_InvalidIndex)
					{
						while( BIG_ul_SearchFile( ul_Dir, (char *) (LPCSTR) strTemp ) != BIG_C_InvalidIndex )
						{
							strTemp = "_" + strTemp;
						}
					}
					strcpy(asz_NewName, (char *) (LPCSTR) strTemp);	//get complete unique name
					_Try_
						BIG_RenFile(asz_NewName, asz_Path, BIG_NameFile(ul_Index));
					_Catch_
						_End_
						GRO_Struct_SetName(pst_NewGro, asz_NewName);
					sprintf(asz_Path, "Duplicate %s to %s", GRO_sz_Struct_GetName(pst_Gro), asz_NewName);
					LINK_PrintStatusMsg(asz_Path);

					pst_Gro->i->pfn_AddRef(pst_Gro, -1);
					_pst_GO->pst_Base->pst_Visu->pst_Material = pst_NewGro;
					TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicObjectsTable, pst_NewGro);
					pst_NewGro->i->pfn_AddRef(pst_NewGro, 2);
				}
				else
				{
					sprintf(asz_Path, "Can't duplicate material for now");
					LINK_PrintStatusMsg(asz_Path);
				}
			}
			else	//the material is already duplicated
			{
				pst_Gro->i->pfn_AddRef(pst_Gro, -1);
				pst_NewGro = (*it).second;
				_pst_GO->pst_Base->pst_Visu->pst_Material = pst_NewGro;
				pst_NewGro->i->pfn_AddRef(pst_NewGro, 1);
#else
				sprintf(asz_Path, "Can't duplicate material for now");
				LINK_PrintStatusMsg(asz_Path);
#endif
			}
		}
	}
	
	// Particle generator
	if((_ul_Flags & F3D_Duplicate_PAG) && (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu))
	{
		pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
		
		if(pst_Gro && (b_Always || (F3D_GRO_ul_GetLocation(_pst_GO, pst_Gro) & _ul_Flags)))
		{
            //WOR_GetGroPathWithKey(ul_WorldKey, asz_Path);
            WOR_GetGroPath(pst_World, asz_Path);

			pst_NewGro = (GRO_tdst_Struct *) pst_Gro->i->pfnp_Duplicate(pst_Gro, asz_Path, NULL, 0);
			if(pst_NewGro)
			{
				//pst_Gro->i->pfn_Release(pst_Gro);
				_pst_GO->pst_Base->pst_Visu->pst_Object = pst_NewGro;
				TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicObjectsTable, pst_NewGro);
				pst_NewGro->i->pfn_AddRef(pst_NewGro, 2);
                // Rename
#ifndef JADEFUSION
				F3D_GRO_Rename(pst_NewGro, asz_Path, _pst_GO->sz_Name);
#endif
			}
			else
			{
				sprintf(asz_Path, "Can't duplicate this particle generator %s", GRO_sz_Struct_GetName(pst_Gro));
				LINK_PrintStatusMsg(asz_Path);
			}
		}
	}
}

/*$F
 =======================================================================================================================
 Create a visuel for a GameObject (selection callback)
    
    _pst_Sel   GameObject
    _ul_Param1 GameObject world
    _ul_Param2 type of Visuel that is to be created (works actually only with 2DSpriteList and 2DText)
 =======================================================================================================================
 */

BOOL F3D_b_GRO_CreateVisuel(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GRO_tdst_Struct		*pst_Gro;
	char				asz_Path[BIG_C_MaxLenPath], *sz_SubDir;
	char				asz_Name[BIG_C_MaxLenPath];
	WOR_tdst_World		*pst_World;
	char				*psz_Temp;
	unsigned long		ul_Index;
	TAB_tdst_Ptable		*pst_PTable;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_World = (WOR_tdst_World *) _ul_Param1;

	if(!pst_GO) return TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
	{
		OBJ_ChangeIdentityFlags(pst_GO, pst_GO->ul_IdentityFlags | OBJ_C_IdentityFlag_Visu, pst_GO->ul_IdentityFlags);
	}
	else
	{
		if(pst_GO->pst_Base->pst_Visu->pst_Object != NULL)
		{
			/* Already a visuel, return */
			return TRUE;
		}
	}

	/*
	 * if(_ul_Param2 == GRO_2DSpriteList) pst_Gro = (GRO_tdst_Struct *)
	 * SPL_pst_Create();
	 */
	if(_ul_Param2 == GRO_2DText)
	{
		pst_Gro = (GRO_tdst_Struct *) STR_pst_Create();
		STR_ChangeNumberMaxOfLetters((STR_tdst_Struct *) pst_Gro, 32);
	}
	else if(_ul_Param2 == GRO_ParticleGenerator)
		pst_Gro = (GRO_tdst_Struct *) PAG_pst_Create();
	else
		return TRUE;

	pst_GO->pst_Base->pst_Visu->pst_Object = pst_Gro;
	pst_Gro->i->pfn_AddRef(pst_Gro, 1);

	/* Add graphic objects in graphics table */
	pst_PTable = &pst_World->st_GraphicObjectsTable;
	if(TAB_ul_Ptable_GetElemIndexWithPointer(pst_PTable, pst_Gro) == TAB_Cul_BadIndex)
	{
		TAB_Ptable_AddElemAndResize(pst_PTable, pst_Gro);
		pst_Gro->i->pfn_AddRef(pst_Gro, 1);
	}

	/* Name of object */
	L_strcpy(asz_Name, pst_GO->sz_Name);
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtGraphicObject);
	GRO_Struct_SetName(pst_GO->pst_Base->pst_Visu->pst_Object, asz_Name);

	/* Create a new file */
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		ul_Index = BIG_ParentFile(ul_Index);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			BIG_ComputeFullName(ul_Index, asz_Path);
			ul_Index = BIG_C_InvalidIndex;
			sz_SubDir = strrchr(asz_Path, '/');
			if(sz_SubDir)
			{
				sz_SubDir++;
				if(L_stricmp(EDI_Csz_Path_GameObject, sz_SubDir) == 0)
				{
					L_strcpy(sz_SubDir, EDI_Csz_Path_GraphicObject);
					ul_Index = 0;
				}
			}
		}
	}

	if(ul_Index == BIG_C_InvalidIndex) WOR_GetGroPath(pst_World, asz_Path);

	SAV_Begin(asz_Path, asz_Name);
	pst_Gro->i->pfnl_SaveInBuffer(pst_Gro, NULL);
	ul_Index = SAV_ul_End();
	LOA_AddAddress(ul_Index, pst_Gro);
	LINK_UpdatePointer(pst_GO);
	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    General function
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::CreateGeometry(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	char	sz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpo_CreateGeometryDialog)
	{
		mpo_CreateGeometryDialog->DestroyWindow();
		delete mpo_CreateGeometryDialog;
		mpo_CreateGeometryDialog = NULL;
	}
	else
	{
        /* interdit la création d'objet si on est en mode sous objet */
        if(Selection_b_IsInSubObjectMode())
        {
            M_MF()->MessageBox("Can't create new geometry during sub object edition", "Warning", MB_OK | MB_ICONSTOP);
            return;
        }

		mpo_CreateGeometryDialog = new EDIA_cl_CreateGeometry(this, NULL, 0);

		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mst_WinHandles.pst_World);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), sz_Path);
			L_strcat(sz_Path, "/");
			L_strcat(sz_Path, EDI_Csz_Path_GraphicObject);
			mpo_CreateGeometryDialog->msz_Path = sz_Path;
		}

		mpo_CreateGeometryDialog->DoModeless();
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::CreateColMap(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	char	sz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpo_CreateGeometryDialog)
	{
		mpo_CreateGeometryDialog->DestroyWindow();
		delete mpo_CreateGeometryDialog;
		mpo_CreateGeometryDialog = NULL;
	}
	else
	{
        /* interdit la création d'objet si on est en mode sous objet */
        if(Selection_b_IsInSubObjectMode())
        {
            M_MF()->MessageBox("Can't create new Colmap during sub object edition", "Warning", MB_OK | MB_ICONSTOP);
            return;
        }

		mpo_CreateGeometryDialog = new EDIA_cl_CreateGeometry(this, NULL, 1);

		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mst_WinHandles.pst_World);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), sz_Path);
			L_strcat(sz_Path, "/");
			L_strcat(sz_Path, EDI_Csz_Path_COLObjects);
			mpo_CreateGeometryDialog->msz_Path = sz_Path;
		}

		mpo_CreateGeometryDialog->DoModeless();
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern void				*GEO_gpst_SnapshotGAOSrc;
extern GEO_tdst_Object  *GEO_gpst_SnapshotSrc;
extern GEO_tdst_Object  *GEO_gpst_SnapshotTgt;

void F3D_cl_View::Selection_SnapGeo()
{
    OBJ_tdst_GameObject     *pst_GO, *pst_TgtGO;
    char			        asz_Path[BIG_C_MaxLenPath];
	WOR_tdst_World	        *pst_World;
    GRO_tdst_Struct         *pst_Mat;
    GEO_tdst_Object         *pst_Geo, *pst_NewGeo;
    ULONG                   ul_Flag, ul_OldFlag;
    SEL_tdst_SelectedItem   *pst_Sel;
    

	if(M_F3D_World == NULL) return;

    pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem( M_F3D_Sel, SEL_C_SIF_Object);
    pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo( pst_GO );

    pst_Sel = SEL_pst_GetFirst( M_F3D_Sel, SEL_C_SIF_Object);
    pst_Sel = SEL_pst_GetNext( M_F3D_Sel, pst_Sel, SEL_C_SIF_Object );
    pst_TgtGO = (pst_Sel) ? (OBJ_tdst_GameObject *) pst_Sel->p_Content : NULL;
    
    pst_World = WOR_World_GetWorldOfObject(pst_GO);
    
    WOR_GetGroPath(pst_World, asz_Path);
	pst_NewGeo = (GEO_tdst_Object *) pst_Geo->st_Id.i->pfnp_Duplicate(pst_Geo, asz_Path, NULL, 0);
    if(pst_NewGeo)
    {
        GEO_gpst_SnapshotSrc = pst_Geo;
		GEO_gpst_SnapshotGAOSrc = (void *) pst_GO;
        GEO_gpst_SnapshotTgt = pst_NewGeo;

        Refresh();

        if ( (pst_TgtGO) && (OBJ_p_GetCurrentGeo( pst_TgtGO) == NULL) )
        {
            ul_OldFlag = ul_Flag = pst_TgtGO->ul_IdentityFlags;
            ul_Flag |= OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_BaseObject;

            OBJ_ChangeIdentityFlags( pst_TgtGO, ul_Flag, ul_OldFlag );
	    
	        pst_TgtGO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) pst_NewGeo;
		    TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicObjectsTable, pst_NewGeo);
		    pst_NewGeo->st_Id.i->pfn_AddRef(pst_NewGeo, 2);

            if ( pst_GO && pst_GO->pst_Base && pst_GO->pst_Base->pst_Visu && (pst_Mat = pst_GO->pst_Base->pst_Visu->pst_Material) )
            {
                pst_TgtGO->pst_Base->pst_Visu->pst_Material = pst_Mat;
                pst_Mat->i->pfn_AddRef( pst_Mat, 1 );
            }
        }
    }
	else
	{
	    sprintf(asz_Path, "Can't duplicate this geometry %s", GRO_sz_Struct_GetName((GRO_tdst_Struct *) pst_Geo));
		LINK_PrintStatusMsg(asz_Path);
    }
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Selection_DuplicateGro(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Flags, ULONG p2)
{
	F3D_GRO_Duplicate((OBJ_tdst_GameObject *) _pst_Sel->p_Content, _ul_Flags);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_DuplicateGro(ULONG _ul_Flags)
{
	LONG	l_NumGOs;
	if(M_F3D_World == NULL) return;
#ifdef JADEFUSION
	b_IsOK = FALSE;
#endif
	l_NumGOs = SEL_l_CountItem(M_F3D_Sel, SEL_C_SIF_Object);
	if(!l_NumGOs) return;

	F3D_dul_Keys = (ULONG *) MEM_p_Alloc(l_NumGOs * 8);
	L_memset(F3D_dul_Keys, 0, l_NumGOs * 8);
	i_InstanceOrCopy = 0;

	b_GenericNames = FALSE;
	if(l_NumGOs > 10)
	{

		if
		(
			M_MF()->MessageBox
			(
				"Do you want to enter all the new GRO names one by one(YES) or do you want generic names (NO) ??",
				"Please confirm",
				MB_ICONQUESTION | MB_YESNO
			) == IDYES
		)
			b_GenericNames = FALSE;
		else
			b_GenericNames = TRUE;
	}

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_Selection_DuplicateGro, (ULONG) _ul_Flags, 0);

	i_InstanceOrCopy = 0;
	MEM_Free(F3D_dul_Keys);

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::GRO_Drop(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CAM_tdst_Camera					*pst_Cam;
	GRO_tdst_Struct					*pst_GRO;
	GRO_tdst_Struct					*pst_LOD;
	WOR_tdst_World					*pst_World;
	MATH_tdst_Vector				v;
	GDI_tdst_Device					*pst_Dev;
	char							asz_Path[BIG_C_MaxLenPath];
	char							asz_Name[BIG_C_MaxLenName];
	unsigned long					ul_GOKey;
	GEO_tdst_Object					*pst_Geo;
	OBJ_tdst_GameObject				*pst_GO, *apst_GO[32];
	LONG							l_Number;
	SOFT_tdst_PickingBuffer_Pixel	*pst_Pick;
	char							Create; /* O => attach geo, 1 => create new GO */
	EDIA_cl_DropGro					*po_Dlg;
    ULONG                           *pul_RLI, ul_RLInb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* chargement du gro et ajout dans la table du monde */
	pst_World = mst_WinHandles.pst_World;
	pst_GRO = GEO_pst_Object_Load(_pst_DragDrop->ul_FatFile, pst_World);
	if(TAB_ul_Ptable_GetElemIndexWithPointer(&pst_World->st_GraphicObjectsTable, pst_GRO) == TAB_Cul_BadIndex)
	{
		TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicObjectsTable, pst_GRO);
		pst_GRO->i->pfn_AddRef(pst_GRO, 1);
	}

	/* pick object under mouse */
	ScreenToClient(&_pst_DragDrop->o_Pt);
	l_Number = 0;
	if(Pick_l_UnderPoint(&_pst_DragDrop->o_Pt, SOFT_Cuc_PBQF_GameObject, 0))
	{
		pst_Pick = Pick_pst_GetFirst(SOFT_Cuc_PBQF_GameObject, -1);
		while(pst_Pick)
		{
			apst_GO[l_Number++] = (OBJ_tdst_GameObject *) pst_Pick->ul_Value;
			if(l_Number == 32) break;
			pst_Pick = Pick_pst_GetNext(SOFT_Cuc_PBQF_GameObject, -1, pst_Pick);
		}
	}

	/* find path for gao */
	WOR_GetGaoPath(pst_World, asz_Path);

	/* Build a default name */
	strcpy(asz_Name, BIG_NameFile( _pst_DragDrop->ul_FatFile ) );
	BIG_BuildNewName(asz_Path, asz_Name, EDI_Csz_ExtGameObject);

	Create = 0;
	if(l_Number == 0)
		Create = 1;
	else
	{
		/* choose action */
		po_Dlg = new EDIA_cl_DropGro( asz_Path, asz_Name, l_Number, (void **) apst_GO);
		po_Dlg->DoModal();

		/* treat action choosen */
		if(po_Dlg->mi_Choice == EDIA_DropGro_Attach)
			pst_GO = apst_GO[po_Dlg->mi_Index];
		else if(po_Dlg->mi_Choice == EDIA_DropGro_Create)
		{
			L_strcpy(asz_Name, po_Dlg->msz_Name);
			Create = 1;
		}
		else
		{
			delete po_Dlg;
			return;
		}

		delete po_Dlg;
	}

	if(Create)
	{
		pst_GO = OBJ_GameObject_Create(OBJ_C_IdentityFlag_BaseObject | OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_HasInitialPos);
		pst_GO->pst_World = pst_World;

		/* Move object to point where it has been dropped */
		pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
		v.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
		v.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
		b_PickDepth(&v, &v.z);
		pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
		v.x = (float) _pst_DragDrop->o_Pt.x;
		v.y = (float) _pst_DragDrop->o_Pt.y;
		CAM_2Dto3D(pst_Cam, &v, &v);

		OBJ_SetInitialAbsolutePosition(pst_GO, &v);
		OBJ_RestoreInitialPos(pst_GO);

		WOR_gpst_WorldToLoadIn = pst_World;
		OBJ_GameObject_RegisterWithName(pst_GO, BIG_C_InvalidIndex, asz_Name, OBJ_GameObject_RegSetName);
		ul_GOKey = OBJ_ul_GameObject_Save(pst_World, pst_GO, asz_Path);
		LOA_AddAddress(ul_GOKey, pst_GO);
		WOR_World_JustAfterLoadObject(pst_World, pst_GO, TRUE, TRUE);
	}

	mpst_LockPickObj = pst_GO;

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		ul_GOKey = pst_GO->ul_IdentityFlags;
		OBJ_ChangeIdentityFlags(pst_GO, ul_GOKey &~OBJ_C_IdentityFlag_Anims, ul_GOKey);
	}

	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
	{
		ul_GOKey = pst_GO->ul_IdentityFlags;
		OBJ_ChangeIdentityFlags(pst_GO, ul_GOKey | OBJ_C_IdentityFlag_Visu, ul_GOKey);
	}

	pst_LOD = pst_GO->pst_Base->pst_Visu->pst_Object;
	if(pst_LOD)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EDIA_cl_NameDialogCombo o_Dialog("Choose Drag'n drop action");
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(pst_LOD->i->ul_Type == GRO_GeoStaticLOD)
		{
			o_Dialog.AddItem("Change geometry");
			o_Dialog.AddItem("Add new LOD");
			o_Dialog.AddItem("Replace current LOD");

			o_Dialog.SetDefault("Change geometry");

			if(o_Dialog.DoModal() != IDOK) return;

			if(L_stricmp(o_Dialog.mo_Name, "Add new LOD") == 0)
			{
				GEO_StaticLOD_Add((GEO_tdst_StaticLOD *) pst_LOD, pst_GRO, pst_GO->uc_LOD_Vis );
				LINK_Refresh();
				return;
			}
			else if(L_stricmp(o_Dialog.mo_Name, "Replace current LOD") == 0)
			{
				GEO_StaticLOD_Replace((GEO_tdst_StaticLOD *) pst_LOD, pst_GRO, pst_GO->ul_ForceLODIndex);
				LINK_Refresh();
				return;
			}
		}

		pst_LOD->i->pfn_AddRef(pst_LOD, -1);
	}

	pst_GO->pst_Base->pst_Visu->pst_Object = pst_GRO;
	pst_GRO->i->pfn_AddRef(pst_GRO, 1);

    /* readapt number of vertex color */
    pst_Geo = (GEO_tdst_Object *) pst_GRO;
    pul_RLI = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
    if ( pul_RLI && pst_Geo )
    {
        if ( pst_Geo->st_Id.i->ul_Type == GRO_GeoStaticLOD )
        {
            int i;
            pst_LOD = (GRO_tdst_Struct *) pst_Geo;
            pst_Geo = NULL;
            
            for (i = 0; i < ((GEO_tdst_StaticLOD *) pst_LOD)->uc_NbLOD; i++)
            {
                if ( ((GEO_tdst_StaticLOD *) pst_LOD)->dpst_Id[ i ] != NULL) 
                {
                    pst_Geo = (GEO_tdst_Object *) ((GEO_tdst_StaticLOD *) pst_LOD)->dpst_Id[ i ];
                    if ( pst_Geo->dul_PointColors )
                        pst_Geo = NULL;
                    break;
                }
            }
        }

        if (pst_Geo == NULL)
        {
			OBJ_VertexColor_Free( pst_GO );
            //MEM_Free( pul_RLI );
            //pst_GO->pst_Base->pst_Visu->dul_VertexColors = NULL;
        }
        else if ( pst_Geo->st_Id.i->ul_Type == GRO_Geometric )
        {
            if (*pul_RLI != (ULONG) pst_Geo->l_NbPoints)
            {
				ul_RLInb = *pul_RLI;
				pul_RLI = OBJ_VertexColor_Realloc( pst_GO, pst_Geo->l_NbPoints );
				if ( ul_RLInb < (ULONG) pst_Geo->l_NbPoints )
                    L_memset( pul_RLI + (1 + ul_RLInb), 0, 4 * (pst_Geo->l_NbPoints - ul_RLInb) );
                //pul_RLI = (ULONG *) MEM_p_Realloc( pul_RLI, 4 * (pst_Geo->l_NbPoints + 1) );
                //if ( *pul_RLI < (ULONG) pst_Geo->l_NbPoints )
                //    L_memset( pul_RLI + (1 + *pul_RLI), 0, 4 * (pst_Geo->l_NbPoints - *pul_RLI) );
                //*pul_RLI = pst_Geo->l_NbPoints;
                //pst_GO->pst_Base->pst_Visu->dul_VertexColors = pul_RLI;
            }
        }
        
    }

    /* recompute BV */
	OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_AABBox);

	/* Force display */
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)
		mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayLight;
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Waypoints)
	{
		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Links)
			mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayWaypointInNetwork;
		else
			mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayWaypoint;
	}

	if(pst_GO->pst_Base)
	{
		pst_Geo = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
		if(pst_Geo->st_Id.i->ul_Type == GRO_Camera)
			mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayCamera;
	}

	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
    Geometry functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_CleanGeometry(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Geo) GEO_Clean( pst_GO, pst_Geo, pst_GO->pst_Base->pst_Visu->dul_VertexColors );
	return TRUE;
}
/**/
void F3D_cl_View::Selection_CleanGeo(void)
{
	if(M_F3D_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_CleanGeometry, 0, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_ComputeNormals(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Geo) GEO_ComputeNormals(pst_Geo);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_ComputeNormals(void)
{
	if(M_F3D_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_ComputeNormals, 0, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_CenterUVs(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Geo) 
        GEO_SubObject_UVCenter(pst_Geo);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_CenterUVs(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Locked;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(M_F3D_World == NULL) return;

	b_Locked = mpo_CurrentModif != NULL;
	if(b_Locked) mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this, b_Locked);
	mpo_CurrentModif->SetDesc("Center UVs with material");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_CenterUVs, (ULONG) this, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_CenterUVsWithMaterial(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Geo) 
        GEO_SubObject_UVCenterWithMaterial(pst_Geo, (MAT_tdst_Material *) OBJ_p_GetGrm(pst_GO) );
	return TRUE;
}
/**/
void F3D_cl_View::Selection_CenterUVsWithMaterial(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Locked;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(M_F3D_World == NULL) return;

	b_Locked = mpo_CurrentModif != NULL;
	if(b_Locked) mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this, b_Locked);
	mpo_CurrentModif->SetDesc("Center UVs with material");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;
	
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_CenterUVsWithMaterial, (ULONG) this, 0);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_SetUVs4Text(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_GO;
	GEO_tdst_Object				*pst_Geo;
	//EDIA_cl_NameDialogCombo		o_Dialog("Select element");
	//int							i, i_Element;
	//char						sz_String[16][ 64 ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Geo) 
	{
		if (pst_Geo->st_Id.i->ul_Type != GRO_Geometric) return TRUE;
		if (pst_Geo->l_NbPoints == 0) return TRUE;
		if (pst_Geo->l_NbElements == 0) return TRUE;
		if (pst_Geo->dst_Element->l_NbTriangles == 0) return TRUE;
		/*
		i_Element = 0;
		if (pst_Geo->l_NbElements > 1 )
		{
			for (i = 0; i < pst_Geo->l_NbElements; i++ )
			{
				sprintf( sz_String[ i ], "Element %d, ID %d", i, pst_Geo->dst_Element[ i ].l_MaterialId );
				o_Dialog.AddItem( sz_String[ i ], i );
				if (i == 0) o_Dialog.SetDefault( sz_String[ i ] );
			}
			if(o_Dialog.DoModal() != IDOK) return TRUE;
			i_Element = o_Dialog.mi_CurSelData;
		}
		GEO_SubObject_SetUVfor3DText(pst_Geo, i_Element);
		*/
		GEO_SubObject_SetUVfor3DText(pst_Geo, 0);
	}
	return TRUE;
}
/**/
void F3D_cl_View::Selection_SetUVs4Text(void)
{
	if(M_F3D_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_SetUVs4Text, (ULONG) this, 0);
}

/*$4
 ***********************************************************************************************************************
    LOD functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_ChangeVisuFromGeoToLOD( OBJ_tdst_GameObject *_pst_Gao)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct		*pst_Gro;
	GEO_tdst_StaticLOD	*pst_LOD;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenName];
	char				*psz_Temp;
	unsigned long		ul_Index;
	TAB_tdst_Ptable		*pst_PTable;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_Gao) return;
    if(M_F3D_World == NULL) return;
	if(!OBJ_b_TestIdentityFlag(_pst_Gao, OBJ_C_IdentityFlag_Visu)) return;

	pst_Gro = _pst_Gao->pst_Base->pst_Visu->pst_Object;
	if(!pst_Gro) return;
	if(pst_Gro->i->ul_Type != GRO_Geometric) return;

	pst_LOD = GEO_pst_StaticLOD_Create(1);
	pst_LOD->auc_EndDistance[0] = 60;
	pst_LOD->dpst_Id[0] = pst_Gro;

	_pst_Gao->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) pst_LOD;
	pst_LOD->st_Id.i->pfn_AddRef(pst_LOD, 1);

	/* Name of object */
	L_strcpy(asz_Name, "LOD_");
	L_strcat(asz_Name, _pst_Gao->sz_Name);
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;

	GRO_Struct_SetName(_pst_Gao->pst_Base->pst_Visu->pst_Object, asz_Name);

	/* Create a new file */
	WOR_GetGroPath(M_F3D_World, asz_Path);
	L_strcat(asz_Name, EDI_Csz_ExtGraphicObject);
	SAV_Begin(asz_Path, asz_Name);
	pst_LOD->st_Id.i->pfnl_SaveInBuffer(pst_LOD, NULL);
	ul_Index = SAV_ul_End();
	LOA_AddAddress(ul_Index, pst_LOD);
	LINK_Refresh();

	/* add new object into table */
	pst_PTable = &M_F3D_World->st_GraphicObjectsTable;
	if(TAB_ul_Ptable_GetElemIndexWithPointer(pst_PTable, pst_LOD) == TAB_Cul_BadIndex)
	{
		TAB_Ptable_AddElemAndResize(pst_PTable, pst_LOD);
		pst_LOD->st_Id.i->pfn_AddRef(pst_LOD, 1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_ChangeVisuFromLODToGeo( OBJ_tdst_GameObject *_pst_Gao )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct		*pst_Gro;
	GEO_tdst_StaticLOD	*pst_LOD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_Gao) return;
    if(M_F3D_World == NULL) return;
	if(!OBJ_b_TestIdentityFlag( _pst_Gao, OBJ_C_IdentityFlag_Visu)) return;

	pst_LOD = (GEO_tdst_StaticLOD *) _pst_Gao->pst_Base->pst_Visu->pst_Object;
	if(!pst_LOD) return;
	if(pst_LOD->st_Id.i->ul_Type != GRO_GeoStaticLOD) return;

	pst_LOD->st_Id.i->pfn_AddRef(pst_LOD, -1);

	pst_Gro = pst_LOD->dpst_Id[0];
	if(pst_Gro) pst_Gro->i->pfn_AddRef(pst_Gro, 1);
	_pst_Gao->pst_Base->pst_Visu->pst_Object = pst_Gro;
}

/*$4
 ***********************************************************************************************************************
    2D Text function
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_Create2DTextVisuel(void)
{
	if(M_F3D_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GRO_CreateVisuel, (ULONG) M_F3D_World, GRO_2DText);
	LINK_UpdatePointers();
	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
    Particle generator functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_CreateParticleGeneratorVisuel(void)
{
	if(M_F3D_World == NULL) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_GRO_CreateVisuel, (ULONG) M_F3D_World, GRO_ParticleGenerator);
	LINK_UpdatePointers();
	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
    Light functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::LIGHT_ChangeOmniNearFar(LIGHT_tdst_Light *_pst_Light, float _f_Near, float _f_Far)
{
	if(*(long *) &_f_Far != -1) _pst_Light->st_Omni.f_Far = _f_Far;

	if(*(long *) &_f_Near == -1)
	{
		if(_pst_Light->st_Omni.f_Near > _pst_Light->st_Omni.f_Far)
			_pst_Light->st_Omni.f_Near = _pst_Light->st_Omni.f_Far;
	}
	else
	{
		_pst_Light->st_Omni.f_Near = _f_Near;
		if(_pst_Light->st_Omni.f_Far < _f_Near) _pst_Light->st_Omni.f_Far = _f_Near;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::LIGHT_ChangeSpotAlpha(LIGHT_tdst_Light *_pst_Light, float _f_Little, float _f_Big, BOOL _b_Radius)
{
	/*~~~~~~~~~~~~~~~~~*/
	float	alpha, ALPHA;
	/*~~~~~~~~~~~~~~~~~*/

	alpha = _f_Little;
	ALPHA = _f_Big;

	if(_b_Radius)
	{
		if(*(long *) &alpha != -1) alpha = fAtan(alpha / _pst_Light->st_Spot.f_Near);
		if(*(long *) &ALPHA != -1) ALPHA = fAtan(ALPHA / _pst_Light->st_Spot.f_Far);
	}

#ifdef JADEFUSION
    if (_pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
    {

        if (*(long *) &ALPHA != -1)
        {
            FLOAT fLittleRadius = _pst_Light->st_Spot.f_Near * fTan(_pst_Light->st_Spot.f_LittleAlpha);
			FLOAT fShadowRadius = _pst_Light->st_Spot.f_Far * fTan(_pst_Light->f_HiResFOV);

            if (_f_Big < fLittleRadius)
            {
                // clamp little radius to big redius
                _pst_Light->st_Spot.f_LittleAlpha = fAtan((_f_Big - Cf_EpsilonBig) / _pst_Light->st_Spot.f_Near);
            }
			
			if (_f_Big < fShadowRadius)
			{
				// clamp little radius to big redius
				_pst_Light->f_HiResFOV = fAtan((_f_Big - Cf_EpsilonBig) / _pst_Light->st_Spot.f_Far);
			}

            _pst_Light->st_Spot.f_BigAlpha = ALPHA;
        }
        else if (*(long *) &alpha != -1)
        {
            FLOAT fBigRadius = _pst_Light->st_Spot.f_Far * fTan(_pst_Light->st_Spot.f_BigAlpha);

            if (_f_Little > fBigRadius)
            {
                // clamp big radius to little radius
                _pst_Light->st_Spot.f_BigAlpha = fAtan((_f_Little + Cf_EpsilonBig) / _pst_Light->st_Spot.f_Far);
            }

            _pst_Light->st_Spot.f_LittleAlpha = alpha;
        }
    }
    else
#endif
	{
	if(*(long *) &ALPHA != -1) _pst_Light->st_Spot.f_BigAlpha = ALPHA;

	if(*(long *) &alpha == -1)
	{
		if(_pst_Light->st_Spot.f_LittleAlpha > _pst_Light->st_Spot.f_BigAlpha)
			_pst_Light->st_Spot.f_LittleAlpha = _pst_Light->st_Spot.f_BigAlpha;

#ifdef JADEFUSION
		if(_pst_Light->f_HiResFOV > _pst_Light->st_Spot.f_BigAlpha)
			_pst_Light->f_HiResFOV = _pst_Light->st_Spot.f_BigAlpha;

#endif
	}
	else
	{
		_pst_Light->st_Spot.f_LittleAlpha = alpha;
		if(_pst_Light->st_Spot.f_BigAlpha < alpha) _pst_Light->st_Spot.f_BigAlpha = alpha;
	}
}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::LIGHT_ChangeSpotNearFar(LIGHT_tdst_Light *_pst_Light, float _f_Near, float _f_Far)
{
#ifdef JADEFUSION
	if((*(long *) &_f_Near != -1) && _f_Near < Cf_EpsilonBig)
		_f_Near = Cf_EpsilonBig;

	if((*(long *) &_f_Far != -1) && _f_Far < Cf_EpsilonBig)
		_f_Far = Cf_EpsilonBig;

	FLOAT fBigRadius    = _pst_Light->st_Spot.f_Far * fTan(_pst_Light->st_Spot.f_BigAlpha);
    FLOAT fLittleRadius = _pst_Light->st_Spot.f_Near * fTan(_pst_Light->st_Spot.f_LittleAlpha);

	FLOAT fShadowRadius = _pst_Light->st_Spot.f_Far * fTan(_pst_Light->f_HiResFOV);
#endif

	if(*(long *) &_f_Far != -1) _pst_Light->st_Spot.f_Far = _f_Far;

	if(*(long *) &_f_Near == -1)
	{
        // changing Far
		if(_pst_Light->st_Spot.f_Near > _pst_Light->st_Spot.f_Far)
			_pst_Light->st_Spot.f_Near = _pst_Light->st_Spot.f_Far;

#ifdef JADEFUSION
		// - Cf_Epsilon to avoid potential null vector division

		if(_pst_Light->f_ShadowNear > _pst_Light->st_Spot.f_Far)
			_pst_Light->f_ShadowNear = _pst_Light->st_Spot.f_Far - Cf_Epsilon;
#endif
	}
	else
	{
        // changing Near
		_pst_Light->st_Spot.f_Near = _f_Near;
#ifdef JADEFUSION
		if(_pst_Light->st_Spot.f_Far < _f_Near) 
            _pst_Light->st_Spot.f_Far = _f_Near;
	}

    if (_pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
    {
        // adjust angles to keep same radius
        _pst_Light->st_Spot.f_BigAlpha = fAtan(fBigRadius / _pst_Light->st_Spot.f_Far);
        _pst_Light->st_Spot.f_LittleAlpha = fAtan(fLittleRadius / _pst_Light->st_Spot.f_Near);
		_pst_Light->f_HiResFOV = fAtan(fShadowRadius / _pst_Light->st_Spot.f_Far);
	}
#else
		if(_pst_Light->st_Spot.f_Far < _f_Near) _pst_Light->st_Spot.f_Far = _f_Near;
	}
#endif
}

#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
void F3D_cl_View::LIGHT_ChangeShadowAlpha(LIGHT_tdst_Light *_pst_Light, float _f_Little, float _f_Big, BOOL _b_Radius)
{
	/*~~~~~~~~~~~~~~~~~*/
	float	alpha, ALPHA;
	/*~~~~~~~~~~~~~~~~~*/

	alpha = _f_Little;
	ALPHA = _f_Big;

	if(_b_Radius)
	{
		if(*(long *) &alpha != -1) alpha = fAtan(alpha / _pst_Light->f_ShadowNear);

		//divide by Spot Far instead it's used for shadow Far
		if(*(long *) &ALPHA != -1) ALPHA = fAtan(ALPHA / _pst_Light->st_Spot.f_Far);
	}

	if (_pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
	{

		if (*(long *) &ALPHA != -1)
		{
			FLOAT fShadowRadius = _pst_Light->st_Spot.f_Far * fTan(_pst_Light->f_HiResFOV);;

			_pst_Light->f_HiResFOV = ALPHA;

			if(_pst_Light->f_HiResFOV > _pst_Light->st_Spot.f_BigAlpha)
				_pst_Light->f_HiResFOV = _pst_Light->st_Spot.f_BigAlpha;
		}
		else if (*(long *) &alpha != -1)
		{
			FLOAT fShadowRadius = _pst_Light->f_ShadowNear * fTan(_pst_Light->f_HiResFOV);;

			_pst_Light->f_HiResFOV = alpha;

			if(_pst_Light->f_HiResFOV > _pst_Light->st_Spot.f_BigAlpha)
				_pst_Light->f_HiResFOV = _pst_Light->st_Spot.f_BigAlpha;
		}
	}
	else
	{
		//never happen; we only have one alpha
		if(*(long *) &ALPHA != -1) _pst_Light->f_HiResFOV = ALPHA;

		if(*(long *) &alpha == -1)
		{
			if(_pst_Light->f_HiResFOV > _pst_Light->st_Spot.f_BigAlpha)
				_pst_Light->f_HiResFOV = _pst_Light->st_Spot.f_BigAlpha;
		}
		else
		{
			_pst_Light->f_HiResFOV = alpha;
			if(_pst_Light->f_HiResFOV > _pst_Light->st_Spot.f_BigAlpha)
				_pst_Light->f_HiResFOV = _pst_Light->st_Spot.f_BigAlpha;
		}
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void F3D_cl_View::LIGHT_ChangeShadowNearFar(LIGHT_tdst_Light *_pst_Light, float _f_Near, float _f_Far)
{
	if((*(long *) &_f_Near != -1) && _f_Near < Cf_EpsilonBig)
		_f_Near = Cf_EpsilonBig;

	if((*(long *) &_f_Far != -1) && _f_Far < Cf_EpsilonBig)
		_f_Far = Cf_EpsilonBig;

	if(*(long *) &_f_Far == -1)
	{
		// changing Near
		_pst_Light->f_ShadowNear = _f_Near;
		if(_pst_Light->f_ShadowNear > _pst_Light->st_Spot.f_Far) 
			_pst_Light->f_ShadowNear = _pst_Light->st_Spot.f_Far - Cf_EpsilonBig;
	}
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::LIGHT_Drop(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CAM_tdst_Camera		*pst_Cam;
	GRO_tdst_Struct		*pst_Gro;
	GRO_tdst_Struct		*pst_Light;
	OBJ_tdst_GameObject *pst_GO;
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector	v;
	GDI_tdst_Device		*pst_Dev;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenName];
	unsigned long		ul_GOKey;
	unsigned long		ul_Flags;
	//int					i_NameNumber;
	//BIG_INDEX			ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = mst_WinHandles.pst_World;
	pst_Gro = GEO_pst_Object_Load(_pst_DragDrop->ul_FatFile, pst_World);
	if(TAB_ul_Ptable_GetElemIndexWithPointer(&pst_World->st_GraphicObjectsTable, pst_Gro) == TAB_Cul_BadIndex)
	{
		TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicObjectsTable, pst_Gro);
		pst_Gro->i->pfn_AddRef(pst_Gro, 1);
	}

	ScreenToClient(&_pst_DragDrop->o_Pt);

	pst_GO = pst_Pick((POINT) _pst_DragDrop->o_Pt);
	if(pst_GO != NULL)
	{
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Lights))
		{
			pst_Light = pst_GO->pst_Extended->pst_Light;
			if(pst_Light) pst_Light->i->pfn_AddRef(pst_Light, -1);
		}
		else
		{
			if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ExtendedObject))
			{
				ul_Flags = OBJ_ul_FlagsIdentityGet(pst_GO);
				OBJ_ChangeIdentityFlags(pst_GO, ul_Flags | OBJ_C_IdentityFlag_ExtendedObject, ul_Flags);
			}
		}

		OBJ_SetIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Lights);
		pst_GO->pst_Extended->pst_Light = pst_Gro;
		pst_Gro->i->pfn_AddRef(pst_Gro, 1);
		return;
	}

	mpst_LockPickObj = pst_GO = OBJ_GameObject_Create(OBJ_C_IdentityFlag_ExtendedObject | OBJ_C_IdentityFlag_Lights | OBJ_C_IdentityFlag_HasInitialPos);
	pst_GO->pst_Extended->pst_Light = pst_Gro;
	pst_GO->pst_World = pst_World;
	OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_AABBox);

	/* Move object to point where it has been dropped */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	v.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	v.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	b_PickDepth(&v, &v.z);
	pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
	v.x = (float) _pst_DragDrop->o_Pt.x;
	v.y = (float) _pst_DragDrop->o_Pt.y;
	CAM_2Dto3D(pst_Cam, &v, &v);

	OBJ_SetInitialAbsolutePosition(pst_GO, &v);
	OBJ_RestoreInitialPos(pst_GO);

	WOR_GetGaoPath(pst_World, asz_Path);
    strcpy( asz_Name, BIG_NameFile( _pst_DragDrop->ul_FatFile ) );
    BIG_BuildNewName( asz_Path , asz_Name, EDI_Csz_ExtGameObject );

	/* Register and set a default name */
	/*
    i_NameNumber = 0;
	sprintf(asz_Name, "Unnamed%i", i_NameNumber);
	L_strcat(asz_Name, EDI_Csz_ExtGameObject);
	while(1)
	{
		ul_Key = BIG_ul_SearchFileExt(asz_Path, asz_Name);
		if(ul_Key == BIG_C_InvalidIndex) break;
		i_NameNumber++;
		sprintf(asz_Name, "Unnamed%i", i_NameNumber);
		L_strcat(asz_Name, EDI_Csz_ExtGameObject);
	};
    */

	WOR_gpst_WorldToLoadIn = pst_World;
	OBJ_GameObject_RegisterWithName(pst_GO, BIG_C_InvalidIndex, asz_Name, OBJ_GameObject_RegSetName);
	ul_GOKey = OBJ_ul_GameObject_Save(pst_World, pst_GO, asz_Path);
	LOA_AddAddress(ul_GOKey, pst_GO);
	WOR_World_JustAfterLoadObject(pst_World, pst_GO, TRUE, TRUE);

	/* Status msg */
	sprintf(asz_Path, "%s%s", F3D_STR_Csz_DropGRL, pst_GO->sz_Name);
	LINK_PrintStatusMsg(asz_Path);

	/* Force display lights */
	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayLight;
    LINK_Refresh();
}

#endif
