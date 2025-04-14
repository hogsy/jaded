/*$T WORsave.c GC! 1.081 04/11/00 18:24:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Save world */
#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGgroup.h"
#include "LINks/LINKmsg.h"
#include "GRObject/GROsave.h"
#include "TEXture/TEXprocedural.h"
#include "TEXture/TEXanimated.h"
#include "ENGine/Sources/ENGstring.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/WAYs/WAYsave.h"
#include "ENGine/Sources/GRId/GRI_save.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/ANImation/ANIsave.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "ENGine/Sources/GRP/GRPsave.h"

#ifdef JADEFUSION
#include "Light/LIGHTrejection.h"
#endif

extern ULONG	MAT_pst_SaveAllSpritesGenerator(void);
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#if defined(XML_CONV_TOOL)
extern BIG_KEY* WOR_gaul_WorldText;
extern BIG_KEY g_ul_GroupKey;
extern ULONG lNamePadding;
extern int g_nWOWVersion;
#endif // !defined(XML_CONV_TOOL)

#if defined(XML_CONV_TOOL)
extern BIG_KEY* WOR_gaul_WorldText;
extern BIG_KEY g_ul_GroupKey;
extern BIG_KEY g_ul_NetworksGroupKey;
extern BIG_KEY g_ul_LightRejectionKey;
extern ULONG lNamePadding;
extern int g_nWOWVersion;
#endif // !defined(XML_CONV_TOOL)

/*$4
 ***********************************************************************************************************************
    Private functions prototypes
 ***********************************************************************************************************************
 */

void	WOR_World_GroupsSave(WOR_tdst_World *);

/*$4
 ***********************************************************************************************************************
    Public functions
 ***********************************************************************************************************************
 */

/* Aim: Saving a world in a file given by its name */
#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WOR_l_World_SaveWithFileName(WOR_tdst_World *_pst_World, char *_sz_Path, char *_sz_Name, LONG _l_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	char				*psz_Ext;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Path1[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenPath];
	char				asz_GroupName[BIG_C_MaxLenPath];
	char				asz_TextName[BIG_C_MaxLenPath];
	BIG_KEY				ul_Key, ul_Group, ul_GOKey;
	LONG				l_Version;
	float				f_Field;
	int					i, j;
	BIG_KEY				ul_Text;
	ULONG				ul_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
	if (!_pst_World)
		return BIG_C_InvalidKey;

	BIG_KEY ulLRLKey = BIG_C_InvalidKey;

#if !defined(XML_CONV_TOOL)
	// Save light rejection list 
	ulLRLKey = LRL_Save(_pst_World);
#endif
#endif

	float startTime = TIM_f_Counter_TrueRead() / TIM_ul_PreciseGetTicksPerSecond();

	sprintf(asz_Path, "%s %s", ENG_STR_Csz_SaveWorld, _sz_Name);
	LINK_PrintStatusMsg(asz_Path);

	/* check name of world */
	WOR_CheckName(_pst_World);

	/* Build full path */
	if(_sz_Path)
		L_strcpy(asz_Path, _sz_Path);
	else
		WOR_GetPath(_pst_World, asz_Path);

	/* build name of file for world */
	if(_sz_Name)
	{
		L_strcpy(asz_Name, _sz_Name);
		psz_Ext = strrchr(asz_Name, '.');
		if(psz_Ext) *psz_Ext = 0;
		L_strcpy(asz_GroupName, asz_Name);
		L_strcpy(asz_TextName, asz_Name);
	}
	else
	{
		L_strcpy(asz_Name, _pst_World->sz_Name);
	}

	L_strcpy(asz_GroupName, asz_Name);
	L_strcpy(asz_TextName, asz_Name);
	L_strcat(asz_Name, EDI_Csz_ExtWorld);
	L_strcat(asz_GroupName, EDI_Csz_ExtGameObjects);
	L_strcat(asz_TextName, EDI_Csz_ExtWorldText);

	/* begin save of world */
	SAV_Begin(asz_Path, asz_Name);

	/* Save save information : type and version of struct */
#if !defined(XML_CONV_TOOL)
	l_Version = WOR_Cl_WorldStructVersion;
#else
	l_Version = g_nWOWVersion;
#endif // !defined(XML_CONV_TOOL)
	SAV_Buffer(EDI_Csz_ExtWorld, 4);
	SAV_Buffer(&l_Version, 4);

	/* Save world structure */
	SAV_Buffer(&_pst_World->ul_NbTotalGameObjects, 4);
	SAV_Buffer(&_pst_World->ul_AmbientColor, 4);
#if !defined(XML_CONV_TOOL)
	SAV_Buffer(&_pst_World->sz_Name, 64);
#else
	SAV_Buffer(&_pst_World->sz_Name, 60);
	SAV_Buffer(&lNamePadding, 4);
#endif
	SAV_Buffer(&_pst_World->st_CameraPosSave, sizeof(MATH_tdst_Matrix));

	f_Field = (_pst_World->pst_View) ? _pst_World->pst_View->st_DisplayInfo.f_FieldOfVision : 1.0f;
	SAV_Buffer(&f_Field, 4);

	SAV_Buffer(&_pst_World->ul_BackgroundColor, 4);
#if !defined(XML_CONV_TOOL)
	_pst_World->ul_AmbientColor2 &= 0xfcfcfcfc; // avoid white 
#endif // !defined(XML_CONV_TOOL)
	SAV_Buffer(&_pst_World->ul_AmbientColor2, 4);

	SAV_Buffer(&_pst_World->i_LODCut, 4);

#ifdef JADEFUSION
    // Xenon glow
    SAV_Buffer(&_pst_World->f_XeGlowLuminosityMin, sizeof(FLOAT));
    SAV_Buffer(&_pst_World->f_XeGlowZNear, sizeof(FLOAT));
    SAV_Buffer(&_pst_World->f_XeGlowZFar, sizeof(FLOAT));
    SAV_Buffer(&_pst_World->ul_XeGlowColor, sizeof(ULONG));
    SAV_Buffer(&_pst_World->f_XeGlowLuminosityMax, sizeof(FLOAT));
    SAV_Buffer(&_pst_World->f_XeGlowIntensity, sizeof(FLOAT));

	// Xenon shadow blur strength
	SAV_Buffer(&_pst_World->f_XeGaussianStrength, sizeof(FLOAT));

	// Xenon scale and offset
	SAV_Buffer(&_pst_World->f_XeRLIScale, sizeof(FLOAT));
	SAV_Buffer(&_pst_World->f_XeRLIOffset, sizeof(FLOAT));

	// Xenon mipmap LOD bias
#if !defined(XML_CONV_TOOL)
	_pst_World->f_XeMipMapLODBias = MATH_f_FloatLimit(_pst_World->f_XeMipMapLODBias, -1.0f, 2.0f);
#endif
	SAV_Buffer(&_pst_World->f_XeMipMapLODBias, sizeof(FLOAT));

	// Xenon specular boost factors
	if (l_Version >= 6)
	{	
		SAV_Buffer(&_pst_World->f_XeDrySpecularBoost, 4);
		SAV_Buffer(&_pst_World->f_XeWetSpecularBoost, 4);
		SAV_Buffer(&_pst_World->f_XeRainEffectDelay, 4);
		SAV_Buffer(&_pst_World->f_XeRainEffectDryDelay, 4);
	}
	if (l_Version >= 7)
	{
		SAV_Buffer(&_pst_World->f_XeDryDiffuseFactor, 4);
		SAV_Buffer(&_pst_World->f_XeWetDiffuseFactor, 4);
	}

    if (l_Version >= 8)
    {
        SAV_Buffer(&_pst_World->ul_XeDiffuseColor, 4);
        SAV_Buffer(&_pst_World->ul_XeSpecularColor, 4);
    }

    if(l_Version >= 9)
    {
        SAV_Buffer(&_pst_World->f_XeGodRayIntensity, 4);
        SAV_Buffer(&_pst_World->ul_XeGodRayIntensityColor, 4);
    }

    if(l_Version >= 10)
    {
        SAV_Buffer(&_pst_World->f_XeSpecularShiny, 4);
        SAV_Buffer(&_pst_World->f_XeSpecularStrength, 4);
    }

    if (l_Version >= 11)
    {
        SAV_Buffer(&_pst_World->b_XeMaterialLODEnable, 4);
        SAV_Buffer(&_pst_World->f_XeMaterialLODNear, 4);
        SAV_Buffer(&_pst_World->f_XeMaterialLODFar, 4);
        SAV_Buffer(&_pst_World->b_XeMaterialLODDetailEnable, 4);
        SAV_Buffer(&_pst_World->f_XeMaterialLODDetailNear, 4);
        SAV_Buffer(&_pst_World->f_XeMaterialLODDetailFar, 4);
    }

    if( l_Version >= 13 )
    {
        SAV_Buffer(&_pst_World->f_XeSaturation, 4);
        SAV_Buffer(&_pst_World->v_XeBrightness.x, 4);
        SAV_Buffer(&_pst_World->v_XeBrightness.y, 4);
        SAV_Buffer(&_pst_World->v_XeBrightness.z, 4);
        SAV_Buffer(&_pst_World->f_XeContrast, 4);
    }
#endif

	ul_Key = BIG_C_InvalidKey;
#ifdef JADEFUSION
#ifdef _XENON_RENDER
	if(_pst_World->pSPG2Light) ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World->pSPG2Light);
	SAV_Buffer(&ul_Key, 4);
#else
	SAV_Buffer(&ul_Key, 4);
#endif
#else //jadefus

	SAV_Buffer(&ul_Key, 4);
	SAV_Buffer(&ul_Key, 4);
	SAV_Buffer(&ul_Key, 4);
	SAV_Buffer(&ul_Key, 4);
	SAV_Buffer(&ul_Key, 4);
	SAV_Buffer(&ul_Key, 4);
	SAV_Buffer(&ul_Key, 4);
	SAV_Buffer(&ul_Key, 4);
	SAV_Buffer(&ul_Key, 4);
	SAV_Buffer(&ul_Key, 4);
	SAV_Buffer(&ul_Key, 4);
#endif

	/* Grid */
	ul_Key = BIG_C_InvalidKey;
	if(_pst_World->pst_Grid1) ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World->pst_Grid1);
	SAV_Buffer(&ul_Key, 4);

	/* Grid */
	ul_Key = BIG_C_InvalidKey;
	if(_pst_World->pst_Grid) ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World->pst_Grid);
	SAV_Buffer(&ul_Key, 4);

	/* Create group to store All game object of world */
#if !defined(XML_CONV_TOOL)
	BIG_ul_CreateDir(asz_Path);
	ul_Group = BIG_ul_CreateFile(asz_Path, asz_GroupName);

	/* Save reference on group */
	ul_Key = BIG_FileKey(ul_Group);
#else
	ul_Key = g_ul_GroupKey;
#endif
	SAV_Buffer(&ul_Key, 4);

	/* Save reference on list of networks */
	LINK_PrintStatusMsg( "Saving network references..." );
#if !defined(XML_CONV_TOOL)
	ul_Key = 0;
	if(_pst_World->pst_AllNetworks)
	{
		ul_Num = _pst_World->pst_AllNetworks->ul_Num;
		for(i = 0; i < (int) _pst_World->pst_AllNetworks->ul_Num; i++)
		{
			if(!_pst_World->pst_AllNetworks->ppst_AllNetworks[i]) ul_Num--;
			else if(_pst_World->pst_AllNetworks->ppst_AllNetworks[i]->ul_Flags & WAY_C_HasBeenMerge) ul_Num--;
		}

		if(!ul_Num)
			ul_Key = BIG_C_InvalidKey;
		else
		{
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World->pst_AllNetworks);
			if(ul_Key == BIG_C_InvalidIndex) ERR_X_Warning(0, "List of networks can't be saved", NULL);
		}
	}
#else
	ul_Num = 0;
	ul_Key = (BIG_KEY)_pst_World->pst_AllNetworks;
	//ul_Key = g_ul_NetworksGroupKey; //xenon
#endif // !defined(XML_CONV_TOOL)

	SAV_Buffer(&ul_Key, 4);

	/* World text */
#if !defined(XML_CONV_TOOL)
	ul_Text = BIG_ul_SearchFileExt(asz_Path, asz_TextName);
	if(ul_Text == BIG_C_InvalidIndex)
	{
		ul_Text = BIG_ul_CreateFile(asz_Path, asz_TextName);
	}
	ul_Key = BIG_FileKey(ul_Text);
#else
	ul_Text = 0;
	ul_Key = WOR_gaul_WorldText[0];
#endif // !defined(XML_CONV_TOOL)

	SAV_Buffer(&ul_Key, 4);

	/* Sectors */
#if defined(XML_CONV_TOOL)
	if( g_nWOWVersion > 3 )
#endif // defined(XML_CONV_TOOL)
	for(i = 0; i < WOR_C_MaxSecto; i++)
	{
		ULONG	ul_Flags;

		ul_Flags = _pst_World->ast_AllSectos[i].ul_Flags;
#if !defined(XML_CONV_TOOL)
		if(ul_Flags & WOR_CF_SectorMerged) _pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorValid;
		_pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor1;
		_pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor2;
		_pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorColor3;
#endif
		SAV_Buffer(&_pst_World->ast_AllSectos[i].ul_Flags, 4);
		SAV_Buffer(&_pst_World->ast_AllSectos[i].auc_RefVis[0], WOR_C_MaxSectoRef);
		SAV_Buffer(&_pst_World->ast_AllSectos[i].auc_RefAct[0], WOR_C_MaxSectoRef);
		SAV_Buffer(&_pst_World->ast_AllSectos[i].az_Name[0], WOR_C_MaxLenNameSecto);
		_pst_World->ast_AllSectos[i].ul_Flags = ul_Flags;

		for(j = 0; j < WOR_C_MaxSectoPortals; j++)
		{
			ul_Flags = _pst_World->ast_AllSectos[i].ast_Portals[j].uw_Flags;
			_pst_World->ast_AllSectos[i].ast_Portals[j].uw_Flags &= ~WOR_CF_PortalRender;
			SAV_Buffer(&_pst_World->ast_AllSectos[i].ast_Portals[j].uw_Flags, 2);
			SAV_Buffer(&_pst_World->ast_AllSectos[i].ast_Portals[j].uc_ShareSect, 1);
			SAV_Buffer(&_pst_World->ast_AllSectos[i].ast_Portals[j].uc_SharePortal, 1);
			SAV_Buffer(&_pst_World->ast_AllSectos[i].ast_Portals[j].vA, sizeof(MATH_tdst_Vector));
			SAV_Buffer(&_pst_World->ast_AllSectos[i].ast_Portals[j].vB, sizeof(MATH_tdst_Vector));
			SAV_Buffer(&_pst_World->ast_AllSectos[i].ast_Portals[j].vC, sizeof(MATH_tdst_Vector));
			SAV_Buffer(&_pst_World->ast_AllSectos[i].ast_Portals[j].vD, sizeof(MATH_tdst_Vector));
			SAV_Buffer(&_pst_World->ast_AllSectos[i].ast_Portals[j].az_Name[0], WOR_C_MaxLenNamePortal);
			_pst_World->ast_AllSectos[i].ast_Portals[j].uw_Flags = (USHORT) ul_Flags;
		}
	}

#ifdef JADEFUSION
		// ----------------------------------------------------------------------------------
		// Light rejection saved in version 5 of wow ( see WOR_Cl_WorldStructVersion )	
#if defined(XML_CONV_TOOL)
		if( g_nWOWVersion > 4 )
		{
			ulLRLKey = g_ul_LightRejectionKey;			
			SAV_Buffer(&ulLRLKey, 4);
		}
#else
		SAV_Buffer(&ulLRLKey, 4);
#endif	// defined(XML_CONV_TOOL)
		// ----------------------------------------------------------------------------------
#endif

	/* End of save World */
	ul_Key = SAV_ul_End();

	/* Save all game objects */
	char tmp[ 64 ];
	snprintf( tmp, sizeof( tmp ), "Saving %u game objects...", _pst_World->st_AllWorldObjects.ul_NbElems );
	LINK_PrintStatusMsg( tmp );
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, EDI_Csz_Path_GameObject);
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	while(pst_PFElem <= pst_PFLastElem)
	{
		pst_GO = (OBJ_tdst_GameObject*)pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO))
		{
			pst_PFElem++;
			continue;
		}

		/* Generated or Bone object ! Do not save */
		if
		(
			(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Generated)
		||	(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)
		)
		{
			pst_PFElem++;
			continue;
		}

		/* Try to save the game object at the same path as before */
		ul_GOKey = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
		if(ul_GOKey != BIG_C_InvalidKey)
		{
			ul_GOKey = BIG_ul_SearchKeyToFat(ul_GOKey);
			if(ul_GOKey == BIG_C_InvalidIndex)
			{
				ERR_X_Warning(0, "Game object is in world but file has been destroyed !!! Zapping...", pst_GO->sz_Name);
				pst_PFElem++;
				continue;
			}
			BIG_ComputeFullName(BIG_ParentFile(ul_GOKey), asz_Path1);
		}
		else
		{
			L_strcpy(asz_Path1, asz_Path);
		}

		pst_PFElem++;
		
		if( _l_Flags & WOR_C_SaveJustWolAndGol ) 
		{
			ul_GOKey = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO);
		}
		else
		{
_Try_
			ul_GOKey = OBJ_ul_GameObject_Save(_pst_World, pst_GO, asz_Path1);
_Catch_
			ul_GOKey = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO);
_End_
		}
        
		if(ul_GOKey != BIG_C_InvalidIndex)
		{
            LOA_AddAddress(ul_GOKey, pst_GO);
			/* Object has been merge */
			if(!(pst_GO->c_FixFlags & OBJ_C_HasBeenMerge))
				BIG_AddRefInGroup(ul_Group, BIG_NameFile(ul_GOKey), BIG_FileKey(ul_GOKey));
		}
	}
	
	if( _l_Flags & WOR_C_SaveJustWolAndGol ) 
		return ul_Key;

	/* Save all gro */
	snprintf( tmp, sizeof( tmp ), "Saving %u graphic objects...", _pst_World->st_GraphicObjectsTable.ul_NbElems );
	LINK_PrintStatusMsg( tmp );
	GRO_Struct_SaveTable(&_pst_World->st_GraphicObjectsTable);
	
#ifdef JADEFUSION
		// SC: Only save the materials when the texture list is not empty
	if (TEX_gst_GlobalList.l_NumberMaxOfTextures > 0)
#endif	
	{
		GRO_Struct_SaveTable(&_pst_World->st_GraphicMaterialsTable);
	}
	
	/* Save networks */
	if(!(_l_Flags & WOR_C_DoNotSaveNetwork)) 
	{
		if ( _pst_World->pst_AllNetworks != NULL )
		{
			snprintf( tmp, sizeof( tmp ), "Saving %u networks...", _pst_World->pst_AllNetworks->ul_Num );
			LINK_PrintStatusMsg( tmp );
		}
_Try_
		WAY_SaveWorldNetworks(_pst_World);
_Catch_
_End_
	}

	/* Save all sprite generators */
	LINK_PrintStatusMsg( "Saving sprite generators..." );
_Try_
	MAT_pst_SaveAllSpritesGenerator();
_Catch_
_End_

	/* Save all procedural textures */
	LINK_PrintStatusMsg( "Saving procedural textures..." );
_Try_
	TEX_Procedural_Save();
    TEX_Anim_Save();
_Catch_
_End_

	/* nothing more to save */
	_pst_World->c_HaveToBeSaved = 0;

#ifdef ACTIVE_EDITORS && !defined(XML_CONV_TOOL)
	WORGos_Save(_pst_World);
#endif

	float endTime = TIM_f_Counter_TrueRead() / TIM_ul_PreciseGetTicksPerSecond();
	snprintf( tmp, sizeof( tmp ), "World saved successfully! (took %.2f/s)", endTime - startTime );
	LINK_PrintStatusMsg( tmp );

	return ul_Key;
}

/*
 =======================================================================================================================
    Aim:    Save a world in file used to load it
 =======================================================================================================================
 */
#define EDI_Csz_Path_Cines BIG_Csz_Root "/EngineDatas/07 Cinematiques"

LONG WOR_l_World_Save(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_KEY		ul_FileKey;
	BIG_INDEX	ul_FileIndex;
	BIG_INDEX	ul_DirIndex;
	LONG		l_Flag;
	char		asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Search file key associated with world pointer */
	ul_FileKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_World);
	if(ul_FileKey == BIG_C_InvalidKey) return BIG_C_InvalidKey;

	/* Search index associated with key */
	ul_FileIndex = BIG_ul_SearchKeyToFat(ul_FileKey);
	if(ul_FileIndex == BIG_C_InvalidIndex) return BIG_C_InvalidKey;

	ul_DirIndex = BIG_ul_CreateDir(EDI_Csz_Path_Levels);
	if(BIG_b_IsFileInDirRec(ul_DirIndex, ul_FileIndex) || 
       BIG_b_IsFileInDirRec(BIG_ul_CreateDir(EDI_Csz_Path_Cines),ul_FileIndex))
	{
		l_Flag = 0;
	}
	else
	{
		ul_DirIndex = BIG_ul_CreateDir(EDI_Csz_Path_Objects);
		if(BIG_b_IsFileInDirRec(ul_DirIndex, ul_FileIndex))
			l_Flag = WOR_C_IsABank;
		else
		{
			ERR_X_Warning(0, "World is not in Levels or Objects dir => stop saving operation", NULL);
			return 0;
		}
	}

	/* Save world in file associated with world */
	BIG_ComputeFullName(BIG_ParentFile(ul_FileIndex), asz_Path);
	return WOR_l_World_SaveWithFileName(_pst_World, asz_Path, BIG_NameFile(ul_FileIndex), l_Flag);
}

/*$4
 ***********************************************************************************************************************
    Private functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Compute directory name where graphic objects files are located assume that _sz_Path is a BIG_C_MaxLenPath
            length buffer
 =======================================================================================================================
 */
void WOR_GetGroPath(WOR_tdst_World *W, char *_sz_Path)
{
	WOR_GetPath(W, _sz_Path);
	L_strcat(_sz_Path, "/");
	L_strcat(_sz_Path, EDI_Csz_Path_GraphicObject);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_GetGroPathWithKey(BIG_KEY _ul_WorldKey, char *_sz_Path)
{
	WOR_GetPathWithKey(_ul_WorldKey , _sz_Path);
	L_strcat(_sz_Path, "/");
	L_strcat(_sz_Path, EDI_Csz_Path_GraphicObject);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_GetGrmPath(WOR_tdst_World *W, char *_sz_Path)
{
	WOR_GetPath(W, _sz_Path);
	L_strcat(_sz_Path, "/");
	L_strcat(_sz_Path, EDI_Csz_Path_GraphicMaterial);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void WOR_GetGrmPathWithKey(BIG_KEY _ul_WorldKey, char *_sz_Path)
{
	WOR_GetPathWithKey(_ul_WorldKey , _sz_Path);
	L_strcat(_sz_Path, "/");
	L_strcat(_sz_Path, EDI_Csz_Path_GraphicMaterial);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_GetGaoPath(WOR_tdst_World *W, char *_sz_Path)
{
	WOR_GetPath(W, _sz_Path);
	L_strcat(_sz_Path, "/");
	L_strcat(_sz_Path, EDI_Csz_Path_GameObject);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void WOR_GetGaoPathWithKey(BIG_KEY _ul_WorldKey, char *_sz_Path)
{
	WOR_GetPathWithKey(_ul_WorldKey , _sz_Path);
	L_strcat(_sz_Path, "/");
	L_strcat(_sz_Path, EDI_Csz_Path_GameObject);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_GetSubPath(WOR_tdst_World *W, char *_sz_SubPath, char *_sz_Path)
{
	WOR_GetPath(W, _sz_Path);
	L_strcat(_sz_Path, "/");
	L_strcat(_sz_Path, _sz_SubPath);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void WOR_GetSubPathWithKey(BIG_KEY _ul_WorldKey, char *_sz_SubPath, char *_sz_Path)
{
	WOR_GetPathWithKey(_ul_WorldKey, _sz_Path);
	L_strcat(_sz_Path, "/");
	L_strcat(_sz_Path, _sz_SubPath);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_CheckName(WOR_tdst_World *W)
{
	/*~~~~~~~~~~~~~~*/
	char	*sz_Point;
	/*~~~~~~~~~~~~~~*/

	if(!W) return;
	sz_Point = strchr(W->sz_Name, '.');
	if(sz_Point) *sz_Point = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *WOR_psz_GetShortName(WOR_tdst_World *W)
{
	/*~~~~~~~~~~~~~*/
	char	*sz_Name;
	/*~~~~~~~~~~~~~*/

	if(!W) return NULL;
	sz_Name = strrchr(W->sz_Name, '/');
	if(!sz_Name) return W->sz_Name;
	return sz_Name + 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_GetPath(WOR_tdst_World *W, char *_sz_Path)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	BIG_KEY		ul_FileKey;
	BIG_INDEX	ul_FileIndex;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(!W) return;

	ul_FileKey = LOA_ul_SearchKeyWithAddress((ULONG) W);
	if(ul_FileKey == BIG_C_InvalidKey)
		ul_FileIndex = BIG_C_InvalidIndex;
	else
		ul_FileIndex = BIG_ul_SearchKeyToFat(ul_FileKey);

	if(ul_FileIndex == BIG_C_InvalidIndex)
	{
		L_strcpy(_sz_Path, EDI_Csz_Path_Levels);
		L_strcat(_sz_Path, "/");
		L_strcat(_sz_Path, W->sz_Name);
	}
	else
		BIG_ComputeFullName(BIG_ParentFile(ul_FileIndex), _sz_Path);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_GetPathWithKey(BIG_KEY _ul_WorldKey, char * _sz_Path)
{
	BIG_INDEX	ul_FileIndex;

	if(_ul_WorldKey == BIG_C_InvalidKey)
		ul_FileIndex = BIG_C_InvalidIndex;
	else
		ul_FileIndex = BIG_ul_SearchKeyToFat(_ul_WorldKey);

	if(ul_FileIndex == BIG_C_InvalidIndex)
		L_strcpy(_sz_Path, EDI_Csz_Path_Levels);
	else
		BIG_ComputeFullName(BIG_ParentFile(ul_FileIndex), _sz_Path);
}


#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

// SHOWIN ADDED THIS ENTIRE OPTION
LONG WOR_l_World_SaveWithFileName_SELECTED_ONLY( WOR_tdst_World *_pst_World, char *_sz_Path, char *_sz_Name, LONG _l_Flags )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem *pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	char *psz_Ext;
	char asz_Path[ BIG_C_MaxLenPath ];
	char asz_Path1[ BIG_C_MaxLenPath ];
	char asz_Name[ BIG_C_MaxLenPath ];
	char asz_GroupName[ BIG_C_MaxLenPath ];
	char asz_TextName[ BIG_C_MaxLenPath ];
	BIG_KEY ul_Key, ul_Group, ul_GOKey;
	LONG l_Version;
	float f_Field;
	int i, j;
	BIG_KEY ul_Text;
	ULONG ul_Num;
	GRO_tdst_Struct *pst_Gro; // showin added
	ULONG ul_Value; // showin added
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	float startTime = TIM_f_Counter_TrueRead() / TIM_ul_PreciseGetTicksPerSecond();

	sprintf( asz_Path, "%s %s", ENG_STR_Csz_SaveWorld, _sz_Name );
	LINK_PrintStatusMsg( asz_Path );

	/* check name of world */
	WOR_CheckName( _pst_World );

	/* Build full path */
	if ( _sz_Path )
		L_strcpy( asz_Path, _sz_Path );
	else
		WOR_GetPath( _pst_World, asz_Path );

	/* build name of file for world */
	if ( _sz_Name )
	{
		L_strcpy( asz_Name, _sz_Name );
		psz_Ext = strrchr( asz_Name, '.' );
		if ( psz_Ext ) *psz_Ext = 0;
		L_strcpy( asz_GroupName, asz_Name );
		L_strcpy( asz_TextName, asz_Name );
	}
	else
	{
		L_strcpy( asz_Name, _pst_World->sz_Name );
	}

	L_strcpy( asz_GroupName, asz_Name );
	L_strcpy( asz_TextName, asz_Name );
	L_strcat( asz_Name, EDI_Csz_ExtWorld );
	L_strcat( asz_GroupName, EDI_Csz_ExtGameObjects );
	L_strcat( asz_TextName, EDI_Csz_ExtWorldText );

	/* begin save of world */
	SAV_Begin( asz_Path, asz_Name );

	/* Save save information : type and version of struct */
#if !defined( XML_CONV_TOOL )
	l_Version = WOR_Cl_WorldStructVersion;
#else
	l_Version = g_nWOWVersion;
#endif// !defined(XML_CONV_TOOL)
	SAV_Buffer( EDI_Csz_ExtWorld, 4 );
	SAV_Buffer( &l_Version, 4 );

	/* Save world structure */
	SAV_Buffer( &_pst_World->ul_NbTotalGameObjects, 4 );
	SAV_Buffer( &_pst_World->ul_AmbientColor, 4 );
#if !defined( XML_CONV_TOOL )
	SAV_Buffer( &_pst_World->sz_Name, 64 );
#else
	SAV_Buffer( &_pst_World->sz_Name, 60 );
	SAV_Buffer( &lNamePadding, 4 );
#endif
	SAV_Buffer( &_pst_World->st_CameraPosSave, sizeof( MATH_tdst_Matrix ) );

	f_Field = ( _pst_World->pst_View ) ? _pst_World->pst_View->st_DisplayInfo.f_FieldOfVision : 1.0f;
	SAV_Buffer( &f_Field, 4 );

	SAV_Buffer( &_pst_World->ul_BackgroundColor, 4 );
#if !defined( XML_CONV_TOOL )
	_pst_World->ul_AmbientColor2 &= 0xfcfcfcfc;// avoid white
#endif                                         // !defined(XML_CONV_TOOL)
	SAV_Buffer( &_pst_World->ul_AmbientColor2, 4 );

	SAV_Buffer( &_pst_World->i_LODCut, 4 );

	ul_Key = BIG_C_InvalidKey;

	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( &ul_Key, 4 );

	/* Grid */
	ul_Key = BIG_C_InvalidKey;
	if ( _pst_World->pst_Grid1 ) ul_Key = LOA_ul_SearchKeyWithAddress( ( ULONG ) _pst_World->pst_Grid1 );
	SAV_Buffer( &ul_Key, 4 );

	/* Grid */
	ul_Key = BIG_C_InvalidKey;
	if ( _pst_World->pst_Grid ) ul_Key = LOA_ul_SearchKeyWithAddress( ( ULONG ) _pst_World->pst_Grid );
	SAV_Buffer( &ul_Key, 4 );

	/* Create group to store All game object of world */
#if !defined( XML_CONV_TOOL )
	BIG_ul_CreateDir( asz_Path );
	ul_Group = BIG_ul_CreateFile( asz_Path, asz_GroupName );

	/* Save reference on group */
	ul_Key = BIG_FileKey( ul_Group );
#else
	ul_Key = g_ul_GroupKey;
#endif
	SAV_Buffer( &ul_Key, 4 );

	/* Save reference on list of networks */
	LINK_PrintStatusMsg( "Saving network references..." );
#if !defined( XML_CONV_TOOL )
	ul_Key = 0;
	if ( _pst_World->pst_AllNetworks )
	{
		ul_Num = _pst_World->pst_AllNetworks->ul_Num;
		for ( i = 0; i < ( int ) _pst_World->pst_AllNetworks->ul_Num; i++ )
		{
			if ( !_pst_World->pst_AllNetworks->ppst_AllNetworks[ i ] ) ul_Num--;
			else if ( _pst_World->pst_AllNetworks->ppst_AllNetworks[ i ]->ul_Flags & WAY_C_HasBeenMerge )
				ul_Num--;
		}

		if ( !ul_Num )
			ul_Key = BIG_C_InvalidKey;
		else
		{
			ul_Key = LOA_ul_SearchKeyWithAddress( ( ULONG ) _pst_World->pst_AllNetworks );
			if ( ul_Key == BIG_C_InvalidIndex ) ERR_X_Warning( 0, "List of networks can't be saved", NULL );
		}
	}
#else
	ul_Num = 0;
	ul_Key = ( BIG_KEY ) _pst_World->pst_AllNetworks;
	//ul_Key = g_ul_NetworksGroupKey; //xenon
#endif// !defined(XML_CONV_TOOL)

	SAV_Buffer( &ul_Key, 4 );

	/* World text */
#if !defined( XML_CONV_TOOL )
	ul_Text = BIG_ul_SearchFileExt( asz_Path, asz_TextName );
	if ( ul_Text == BIG_C_InvalidIndex )
	{
		ul_Text = BIG_ul_CreateFile( asz_Path, asz_TextName );
	}
	ul_Key = BIG_FileKey( ul_Text );
#else
	ul_Text = 0;
	ul_Key  = WOR_gaul_WorldText[ 0 ];
#endif// !defined(XML_CONV_TOOL)

	SAV_Buffer( &ul_Key, 4 );

	/* Sectors */
#if defined( XML_CONV_TOOL )
	if ( g_nWOWVersion > 3 )
#endif// defined(XML_CONV_TOOL)
		for ( i = 0; i < WOR_C_MaxSecto; i++ )
		{
			ULONG ul_Flags;

			ul_Flags = _pst_World->ast_AllSectos[ i ].ul_Flags;
#if !defined( XML_CONV_TOOL )
			if ( ul_Flags & WOR_CF_SectorMerged ) _pst_World->ast_AllSectos[ i ].ul_Flags &= ~WOR_CF_SectorValid;
			_pst_World->ast_AllSectos[ i ].ul_Flags &= ~WOR_CF_SectorColor1;
			_pst_World->ast_AllSectos[ i ].ul_Flags &= ~WOR_CF_SectorColor2;
			_pst_World->ast_AllSectos[ i ].ul_Flags &= ~WOR_CF_SectorColor3;
#endif
			SAV_Buffer( &_pst_World->ast_AllSectos[ i ].ul_Flags, 4 );
			SAV_Buffer( &_pst_World->ast_AllSectos[ i ].auc_RefVis[ 0 ], WOR_C_MaxSectoRef );
			SAV_Buffer( &_pst_World->ast_AllSectos[ i ].auc_RefAct[ 0 ], WOR_C_MaxSectoRef );
			SAV_Buffer( &_pst_World->ast_AllSectos[ i ].az_Name[ 0 ], WOR_C_MaxLenNameSecto );
			_pst_World->ast_AllSectos[ i ].ul_Flags = ul_Flags;

			for ( j = 0; j < WOR_C_MaxSectoPortals; j++ )
			{
				ul_Flags = _pst_World->ast_AllSectos[ i ].ast_Portals[ j ].uw_Flags;
				_pst_World->ast_AllSectos[ i ].ast_Portals[ j ].uw_Flags &= ~WOR_CF_PortalRender;
				SAV_Buffer( &_pst_World->ast_AllSectos[ i ].ast_Portals[ j ].uw_Flags, 2 );
				SAV_Buffer( &_pst_World->ast_AllSectos[ i ].ast_Portals[ j ].uc_ShareSect, 1 );
				SAV_Buffer( &_pst_World->ast_AllSectos[ i ].ast_Portals[ j ].uc_SharePortal, 1 );
				SAV_Buffer( &_pst_World->ast_AllSectos[ i ].ast_Portals[ j ].vA, sizeof( MATH_tdst_Vector ) );
				SAV_Buffer( &_pst_World->ast_AllSectos[ i ].ast_Portals[ j ].vB, sizeof( MATH_tdst_Vector ) );
				SAV_Buffer( &_pst_World->ast_AllSectos[ i ].ast_Portals[ j ].vC, sizeof( MATH_tdst_Vector ) );
				SAV_Buffer( &_pst_World->ast_AllSectos[ i ].ast_Portals[ j ].vD, sizeof( MATH_tdst_Vector ) );
				SAV_Buffer( &_pst_World->ast_AllSectos[ i ].ast_Portals[ j ].az_Name[ 0 ], WOR_C_MaxLenNamePortal );
				_pst_World->ast_AllSectos[ i ].ast_Portals[ j ].uw_Flags = ( USHORT ) ul_Flags;
			}
		}

//#ifdef JADEFUSION
//	// ----------------------------------------------------------------------------------
//	// Light rejection saved in version 5 of wow ( see WOR_Cl_WorldStructVersion )
//#	if defined( XML_CONV_TOOL )
//	if ( g_nWOWVersion > 4 )
//	{
//		ulLRLKey = g_ul_LightRejectionKey;
//		SAV_Buffer( &ulLRLKey, 4 );
//	}
//#	else
//	SAV_Buffer( &ulLRLKey, 4 );
//#	endif  // defined(XML_CONV_TOOL) \
//	        // ----------------------------------------------------------------------------------
//#endif

	/* End of save World */
	ul_Key = SAV_ul_End();

	/* Save all game objects */
	char tmp[ 64 ];
	snprintf( tmp, sizeof( tmp ), "Saving %u game objects...", _pst_World->st_AllWorldObjects.ul_NbElems );
	LINK_PrintStatusMsg( tmp );
	L_strcat( asz_Path, "/" );
	L_strcat( asz_Path, EDI_Csz_Path_GameObject );
	pst_PFElem     = TAB_pst_PFtable_GetFirstElem( &_pst_World->st_AllWorldObjects );
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem( &_pst_World->st_AllWorldObjects );
	while ( pst_PFElem <= pst_PFLastElem )
	{
		pst_GO = ( OBJ_tdst_GameObject * ) pst_PFElem->p_Pointer;
		if ( TAB_b_IsAHole( pst_GO ) )
		{
			pst_PFElem++;
			continue;
		}

		/* Generated or Bone object ! Do not save */
		if (
		        ( pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Generated ) || ( pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone ) )
		{
			pst_PFElem++;
			continue;
		}

		/* Try to save the game object at the same path as before */
		ul_GOKey = LOA_ul_SearchKeyWithAddress( ( ULONG ) pst_GO );
		if ( ul_GOKey != BIG_C_InvalidKey )
		{
			ul_GOKey = BIG_ul_SearchKeyToFat( ul_GOKey );
			if ( ul_GOKey == BIG_C_InvalidIndex )
			{
				ERR_X_Warning( 0, "Game object is in world but file has been destroyed !!! Zapping...", pst_GO->sz_Name );
				pst_PFElem++;
				continue;
			}
			BIG_ComputeFullName( BIG_ParentFile( ul_GOKey ), asz_Path1 );
		}
		else
		{
			L_strcpy( asz_Path1, asz_Path );
		}

		pst_PFElem++;

		if ( _l_Flags & WOR_C_SaveJustWolAndGol )
		{
			ul_GOKey = LOA_ul_SearchIndexWithAddress( ( ULONG ) pst_GO );
		}
		// showin makes sure it only saves selected objects
		// we have to do it here so it actually keeps all other objects in the gol file
		else if ( ( pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected ) ) 
		{
			_Try_
			        ul_GOKey = OBJ_ul_GameObject_Save( _pst_World, pst_GO, asz_Path1 );
			_Catch_
			        ul_GOKey = LOA_ul_SearchIndexWithAddress( ( ULONG ) pst_GO );
			_End_

			// Showin also makes it so it manually saves the gro for selected object
			if ( pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu )
			{
				pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
				GRO_tdst_Struct **ppst_First;
				char sz_Path[ BIG_C_MaxLenPath ];
				if ( pst_Gro && TAB_ppv_Ptable_GetElemWithPointer( &_pst_World->st_GraphicObjectsTable, pst_Gro ) != NULL )
				{
					ppst_First = ( GRO_tdst_Struct ** ) TAB_ppv_Ptable_GetElemWithPointer( &_pst_World->st_GraphicObjectsTable, pst_Gro );
					ul_Value   = LOA_ul_SearchKeyWithAddress( ( ULONG ) *ppst_First );
					if ( ul_Value != BIG_C_InvalidKey )
					{
						ul_Value = BIG_ul_SearchKeyToFat( ul_Value );
						if ( ul_Value != BIG_C_InvalidIndex )
						{
							BIG_ComputeFullName( BIG_ParentFile( ul_Value ), sz_Path );
							SAV_Begin( sz_Path, BIG_NameFile( ul_Value ) );
							( *ppst_First )->i->pfnl_SaveInBuffer( ( *ppst_First ), &TEX_gst_GlobalList );
							SAV_ul_End();
						}
					}
				}
			}
		}

		if ( ul_GOKey != BIG_C_InvalidIndex )
		{
			LOA_AddAddress( ul_GOKey, pst_GO );
			/* Object has been merge */
			if ( !( pst_GO->c_FixFlags & OBJ_C_HasBeenMerge ) )
				BIG_AddRefInGroup( ul_Group, BIG_NameFile( ul_GOKey ), BIG_FileKey( ul_GOKey ) );
		}
	}

	if ( _l_Flags & WOR_C_SaveJustWolAndGol )
		return ul_Key;

	/* Save all gro */
//	snprintf( tmp, sizeof( tmp ), "Saving %u graphic objects...", _pst_World->st_GraphicObjectsTable.ul_NbElems );
//	LINK_PrintStatusMsg( tmp );
//	GRO_Struct_SaveTable( &_pst_World->st_GraphicObjectsTable );

//#ifdef JADEFUSION
//	// SC: Only save the materials when the texture list is not empty
//	if ( TEX_gst_GlobalList.l_NumberMaxOfTextures > 0 )
//#endif
//	{
//		GRO_Struct_SaveTable( &_pst_World->st_GraphicMaterialsTable );
//	}

//	/* Save networks */
	if ( !( _l_Flags & WOR_C_DoNotSaveNetwork ) )
	{
		if ( _pst_World->pst_AllNetworks != NULL )
		{
			snprintf( tmp, sizeof( tmp ), "Saving %u networks...", _pst_World->pst_AllNetworks->ul_Num );
			LINK_PrintStatusMsg( tmp );
		}
		_Try_
		        WAY_SaveWorldNetworks( _pst_World );
		_Catch_ _End_
	}

	/* Save all sprite generators */
//	LINK_PrintStatusMsg( "Saving sprite generators..." );
//	_Try_
//	MAT_pst_SaveAllSpritesGenerator();
//	_Catch_
//	        _End_
//
//	                /* Save all procedural textures */
//	                LINK_PrintStatusMsg( "Saving procedural textures..." );
//	_Try_
//	TEX_Procedural_Save();
//	TEX_Anim_Save();
//	_Catch_
//	        _End_

	                /* nothing more to save */
	                _pst_World->c_HaveToBeSaved = 0;

//#ifdef ACTIVE_EDITORS && !defined( XML_CONV_TOOL )
//	WORGos_Save( _pst_World );
//#endif

	float endTime = TIM_f_Counter_TrueRead() / TIM_ul_PreciseGetTicksPerSecond();
	snprintf( tmp, sizeof( tmp ), "World saved successfully! (took %.2f/s)", endTime - startTime );
	LINK_PrintStatusMsg( tmp );

	return ul_Key;
}

// SHOWIN ADDED THIS ENTIRE OPTION
LONG WOR_l_World_Save_ONLYSELECTED( WOR_tdst_World *_pst_World )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_KEY ul_FileKey;
	BIG_INDEX ul_FileIndex;
	BIG_INDEX ul_DirIndex;
	LONG l_Flag;
	char asz_Path[ BIG_C_MaxLenPath ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Search file key associated with world pointer */
	ul_FileKey = LOA_ul_SearchKeyWithAddress( ( ULONG ) _pst_World );
	if ( ul_FileKey == BIG_C_InvalidKey ) return BIG_C_InvalidKey;

	/* Search index associated with key */
	ul_FileIndex = BIG_ul_SearchKeyToFat( ul_FileKey );
	if ( ul_FileIndex == BIG_C_InvalidIndex ) return BIG_C_InvalidKey;

	ul_DirIndex = BIG_ul_CreateDir( EDI_Csz_Path_Levels );
	if ( BIG_b_IsFileInDirRec( ul_DirIndex, ul_FileIndex ) ||
	     BIG_b_IsFileInDirRec( BIG_ul_CreateDir( EDI_Csz_Path_Cines ), ul_FileIndex ) )
	{
		l_Flag = 0;
	}
	else
	{
		ul_DirIndex = BIG_ul_CreateDir( EDI_Csz_Path_Objects );
		if ( BIG_b_IsFileInDirRec( ul_DirIndex, ul_FileIndex ) )
			l_Flag = WOR_C_IsABank;
		else
		{
			ERR_X_Warning( 0, "World is not in Levels or Objects dir => stop saving operation", NULL );
			return 0;
		}
	}

	/* Save world in file associated with world */
	BIG_ComputeFullName( BIG_ParentFile( ul_FileIndex ), asz_Path );
	return WOR_l_World_SaveWithFileName_SELECTED_ONLY( _pst_World, asz_Path, BIG_NameFile( ul_FileIndex ), l_Flag );
}