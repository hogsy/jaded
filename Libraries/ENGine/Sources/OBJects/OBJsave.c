/*$T OBJsave.c GC! 1.081 04/13/01 10:27:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ACtions/ACTsave.h"
#include "ENGine/Sources/COLlision/COLsave.h"
#include "ENGine/Sources/WAYs/WAYsave.h"
#include "AIinterp/Sources/AIsave.h"
#include "AIinterp/Sources/Events/EVEsave.h"
#include "EDIpaths.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "F3Dframe/F3Dstrings.h"
#include "LINKs/LINKmsg.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "GraphicDK/Sources/GEOmetric/GEO_MRM.h"

#ifdef JADEFUSION
#include "GraphicDK/Sources/GEOmetric/GEOstaticLOD.h"
#include "Engine/Sources/Wind/CurrentWind.h"
#endif

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDbank.h"
#include "EDItors/Sources/PreFaB/PFBdata.h"

extern BOOL		EDI_gb_ComputeMap;
extern BOOL		LOA_gb_SpeedMode;

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

LONG	OBJ_gl_SaveOnlyImportedDataFromMad = 0;
BOOL	OBJ_gb_SaveDuplicateRLI = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG OBJ_ul_GameObject_SaveRLI( ULONG *_pul_RLI, char *_sz_Path, char *_sz_Name, int _i_Duplicate )
{
#if !defined(XML_CONV_TOOL)
	char	asz_Name[BIG_C_MaxLenName];
	char	asz_Path[BIG_C_MaxLenPath], *psz_Name;
	char	*sz_Ext;
	ULONG	ul_Index, ul_Key;
	
	if (!_pul_RLI) return BIG_C_InvalidKey;

	ul_Index = LOA_ul_SearchIndexWithAddress( (ULONG) _pul_RLI );
	if ( !_i_Duplicate && (ul_Index != BIG_C_InvalidIndex) )
	{
		BIG_ComputeFullName( BIG_ParentFile( ul_Index ), asz_Path );
		psz_Name = BIG_NameFile( ul_Index );
	}
	else
	{
		L_strcpy( asz_Path, _sz_Path );
		L_strcat( asz_Path, "/Game Objects RLI");
		
		L_strcpy( asz_Name, _sz_Name );
		sz_Ext = L_strrchr( asz_Name, '.' );
		if ( *sz_Ext ) *sz_Ext = 0;
		L_strcat( asz_Name, EDI_Csz_ExtGameObjectRLI );
		psz_Name = asz_Name;
	}
	
	SAV_Begin( asz_Path, psz_Name );
	ul_Key = 0x80 | ('R' << 8) | ('L' << 16) | ('I' << 24);
	SAV_Buffer( &ul_Key, 4 );
	SAV_Buffer( _pul_RLI, 4 * (_pul_RLI[0] + 1));
	ul_Key = SAV_ul_End();
	if (ul_Key == BIG_C_InvalidIndex) return BIG_C_InvalidKey;
	return BIG_FileKey( ul_Key );
#else		//	if !defined(XML_CONV_TOOL)
	return ((ULONG)_pul_RLI);
#endif 
}

#if defined(XML_CONV_TOOL)
#ifdef JADEFUSION
extern ULONG	gGaoVersion;
extern ULONG    gGaoDummy[];
extern ULONG    gGaoBvDummy;
extern ULONG    gGaoHasModifierHack;
extern BOOL gGaoSaveBothPrefabKeyHack;
extern BOOL	gGaoOdeBitHack;
#else
extern ULONG	gXmlGaoVersion = 0;
#endif
#endif



/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG OBJ_ul_GameObject_Save(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GO, char *_sz_Path)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Hierarchy		*pst_Hierarchy;
	GRO_tdst_Visu			*pst_Visu;
	LONG					l_Length, i;
	BIG_KEY					ul_Key, ul_RLIKey;
	char					sz_Name[BIG_C_MaxLenName];
	char					asz_Path[BIG_C_MaxLenPath];
	char					asz_WorldPath[BIG_C_MaxLenPath];
	ANI_st_GameObjectAnim	*pst_GameObjectAnim;
	ACT_st_ActionKit		*pst_ActionKit;
	ULONG					ul_IdentityFlags;
	USHORT					uw_Dummy;
	OBJ_tdst_GameObject		*pst_Father;
	OBJ_tdst_DesignStruct	st_Design;
	int						b_Duplicate;
#ifdef JADEFUSION
	ULONG					ul_tmpVal;
	UCHAR					uc_tmpVal;
	ULONG					ul_Version;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Duplicate = FALSE;
	/* Object without name has been generated */
	if(!_pst_GO->sz_Name) return BIG_C_InvalidIndex;
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Generated) return BIG_C_InvalidIndex;

	if (!_pst_World )
		_pst_World = WOR_World_GetWorldOfObject( _pst_GO );

#if !defined(XML_CONV_TOOL)
	/* Save content of animation ? */
	if(ANI_b_IsGizmoAnim(_pst_GO, &pst_Father))
	{
		EVE_SaveListTracks(_pst_GO, _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks);
	}
#endif

	/* Compute path */
	if(!_sz_Path)
	{
		WOR_GetGaoPath(_pst_World, asz_Path);
		_sz_Path = asz_Path;
	}
#ifdef JADEFUSION
	strncpy(sz_Name, _pst_GO->sz_Name, BIG_C_MaxLenName);
#else
	L_strcpy(sz_Name, _pst_GO->sz_Name);
#endif	
#if !defined(XML_CONV_TOOL)
	/*  saving RLI */
	if ( (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_BaseObject) && (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu) )
	{
        // Change RLI to apply indirection, not in preprocess mode
        if (!(EDI_gb_ComputeMap || LOA_gb_SpeedMode) && 
            _pst_GO->pst_Base->pst_Visu->pst_Object &&
            _pst_GO->pst_Base->pst_Visu->dul_VertexColors &&
            (_pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric))
        {
            GEO_tdst_Object *pst_Object = (GEO_tdst_Object *)_pst_GO->pst_Base->pst_Visu->pst_Object;
			WOR_GetPath( _pst_World, asz_WorldPath );
			
			b_Duplicate = ( ( OBJ_gb_SaveDuplicateRLI ) && !(_pst_GO->c_FixFlags & OBJ_C_HasBeenMerge) );
			
            if (pst_Object->p_MRM_Levels && pst_Object->p_MRM_Levels->dus_ReorderBuffer)
            {
                // Change order
                GEO_MRM_v_ApplyReverseReorder((LONG* )(_pst_GO->pst_Base->pst_Visu->dul_VertexColors+1), pst_Object->p_MRM_Levels->dus_ReorderBuffer ,pst_Object->l_NbPoints );

                // Save
                ul_RLIKey = OBJ_ul_GameObject_SaveRLI( _pst_GO->pst_Base->pst_Visu->dul_VertexColors, asz_WorldPath, sz_Name, b_Duplicate );

                // Change back order
                GEO_MRM_v_ApplyReorder((LONG* )(_pst_GO->pst_Base->pst_Visu->dul_VertexColors+1), pst_Object->p_MRM_Levels->dus_ReorderBuffer ,pst_Object->l_NbPoints );
            }
            else
                ul_RLIKey = OBJ_ul_GameObject_SaveRLI( _pst_GO->pst_Base->pst_Visu->dul_VertexColors, asz_WorldPath, sz_Name, b_Duplicate );
        }
        else
#ifdef JADEFUSION
        {
            CHAR  sz_RLIName[BIG_C_MaxLenName + 8];
            CHAR* sz_Pos;
            BOOL  b_WasSaved = FALSE;

            WOR_GetPath(_pst_World, asz_WorldPath);

            // Append a _XMP at the end of the RLI name if it is a Xenon processed mesh
            if ((_pst_GO->pst_Base->pst_Visu->pst_Object             != NULL) &&
                (_pst_GO->pst_Base->pst_Visu->dul_VertexColors       != NULL) &&
                (_pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD))
            {
                GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)_pst_GO->pst_Base->pst_Visu->pst_Object;

                if ((pst_LOD->st_Id.sz_Name != NULL) && (L_strstr(pst_LOD->st_Id.sz_Name, "_XMP_LOD") != NULL))
                {
                    // Set the right name and make sure it's not too long for the file system
                    strcpy(sz_RLIName, sz_Name);
                    sz_Pos = strchr(sz_RLIName, '.');
                    if (sz_Pos != NULL) *sz_Pos = '\0';
                    strcat(sz_RLIName, "_XMP_LOD.");
                    if (strlen(sz_RLIName) > (BIG_C_MaxLenName - 5))
                    {
                        sz_RLIName[BIG_C_MaxLenName - 5] = '.';
                        sz_RLIName[BIG_C_MaxLenName - 4] = '\0';
                    }

                    ul_RLIKey  = OBJ_ul_GameObject_SaveRLI(_pst_GO->pst_Base->pst_Visu->dul_VertexColors, asz_WorldPath, sz_RLIName, b_Duplicate);
                    b_WasSaved = TRUE;
                }
            }

            if (!b_WasSaved)
            {
                ul_RLIKey = OBJ_ul_GameObject_SaveRLI( _pst_GO->pst_Base->pst_Visu->dul_VertexColors, asz_WorldPath, sz_Name, b_Duplicate );
            }
        }
#else
			ul_RLIKey = OBJ_ul_GameObject_SaveRLI( _pst_GO->pst_Base->pst_Visu->dul_VertexColors, asz_WorldPath, sz_Name, b_Duplicate );
#endif
	}
#endif //TOOL
	/*
	 * If the Gao has a ColMap but no file associated to it, we create it first
	 * because we cannot save something within another save.
	 */
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
	{
		if(LOA_ul_SearchKeyWithAddress((ULONG) ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap) == BIG_C_InvalidIndex)
		{
			/*~~~~~~~~~~~*/
			BIG_KEY ul_Key;
			/*~~~~~~~~~~~*/

			ul_Key = COL_ul_CreateColMapFile(_pst_GO->pst_World, BIG_C_InvalidIndex, _pst_GO->sz_Name);
			LOA_AddAddress
			(
				BIG_ul_SearchKeyToFat(ul_Key),
				((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap
			);
		}
	}

	
	SAV_Begin(_sz_Path, sz_Name);

	/* Save content of structure OBJ_tdst_GameObject */
	SAV_Buffer(EDI_Csz_ExtGameObject, 4);

	/* Save version number */
#ifdef JADEFUSION
#if defined(XML_CONV_TOOL)
	ul_Version = gGaoVersion;
#else
	ul_Version = CURRENT_GAO_VERSION;
#endif
	SAV_Buffer(&ul_Version, 4);

    /* Save editor flags */
#if !defined(XML_CONV_TOOL)
	/* The OBJ_C_EditFlags_Selected flag should never be saved ! */
	ul_tmpVal = _pst_GO->ul_EditorFlags;
	ul_tmpVal &= ~OBJ_C_EditFlags_Selected;
	SAV_Buffer(&ul_tmpVal, 4);
#else
	SAV_Buffer(&_pst_GO->ul_EditorFlags, 4);
#endif
#else
	l_Length = 0;
	SAV_Buffer(&l_Length, 4);
	SAV_Buffer(&_pst_GO->ul_EditorFlags, 4);
#endif

	/* Clear the identity flag dyna before saving */
	ul_IdentityFlags = _pst_GO->ul_IdentityFlags;
#if !defined(XML_CONV_TOOL)
	ul_IdentityFlags &= (~OBJ_C_IdentityFlag_Dyna);
#endif
	/* same for dynamic sounds parameters */
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Sound)
	{
        if(_pst_GO->pst_Extended->pst_Sound)
        {
#if !defined(XML_CONV_TOOL)
		    if(!SND_p_MainGetBank(_pst_GO->pst_Extended->pst_Sound) || !SND_p_MainGetBank(_pst_GO->pst_Extended->pst_Sound)->pi_Bank)
					ul_IdentityFlags &= (~OBJ_C_IdentityFlag_Sound);
#endif
		}
        else
            ul_IdentityFlags &= (~OBJ_C_IdentityFlag_Sound);
	}
#if !defined(XML_CONV_TOOL) 
	/* No hierarchy if only father and not father init */
	if(ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		pst_Hierarchy = _pst_GO->pst_Base->pst_Hierarchy;
		if(!pst_Hierarchy->pst_FatherInit) ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Hierarchy;
	}
#endif

	SAV_Buffer(&ul_IdentityFlags, 4);

#if !defined(XML_CONV_TOOL) && defined JADEFUSION
	/* The Culled flag should not be saved.. ever... */
	ul_tmpVal = _pst_GO->ul_StatusAndControlFlags;
	ul_tmpVal &= ~OBJ_C_StatusFlag_Culled;
	SAV_Buffer(&ul_tmpVal, 4);
#else
	SAV_Buffer(&_pst_GO->ul_StatusAndControlFlags, 4);
#endif
	SAV_Buffer(&_pst_GO->uc_Secto, 1);
	SAV_Buffer(&_pst_GO->uc_VisiCoeff, 1);

	/* Add an extra 2 bytes for alignment */
	uw_Dummy = 0;
	SAV_Buffer(&uw_Dummy, 2);

#if !defined(XML_CONV_TOOL)	&& defined JADEFUSION
	/* For the LOD to a default val.. the actual value should never be saved */
	uc_tmpVal = 255;
	SAV_Buffer(&uc_tmpVal, 1);
#else
	SAV_Buffer(&_pst_GO->uc_LOD_Vis, 1);
#endif
	SAV_Buffer(&_pst_GO->uc_LOD_Dist, 1);
	SAV_Buffer(&_pst_GO->uc_DesignFlags, 1);
	SAV_Buffer(&_pst_GO->c_FixFlags, 1);

	/* Save orientation / position */
#if !defined(XML_CONV_TOOL) 
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_HasInitialPos))
		SAV_Buffer(_pst_GO->pst_GlobalMatrix + 1, sizeof(MATH_tdst_Matrix));
	else
#endif
		SAV_Buffer(_pst_GO->pst_GlobalMatrix, sizeof(MATH_tdst_Matrix));

	/* Save bounding volume */
	if(_pst_GO->pst_BV)
	{
#if defined(XML_CONV_TOOL)
		ul_Key = gGaoBvDummy;
#else
		ul_Key = 0xFFFFFFFF;
#endif
		SAV_Buffer(&ul_Key, 4);

		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_OBBox))
		{
			SAV_Buffer(OBJ_pst_BV_GetGMinInit(_pst_GO->pst_BV), sizeof(MATH_tdst_Vector));
			SAV_Buffer(OBJ_pst_BV_GetGMaxInit(_pst_GO->pst_BV), sizeof(MATH_tdst_Vector));
			SAV_Buffer(OBJ_pst_BV_GetLMin(_pst_GO->pst_BV), sizeof(MATH_tdst_Vector));
			SAV_Buffer(OBJ_pst_BV_GetLMax(_pst_GO->pst_BV), sizeof(MATH_tdst_Vector));
		}
		else
		{
			SAV_Buffer(OBJ_pst_BV_GetGMinInit(_pst_GO->pst_BV), sizeof(MATH_tdst_Vector));
			SAV_Buffer(OBJ_pst_BV_GetGMaxInit(_pst_GO->pst_BV), sizeof(MATH_tdst_Vector));
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Save base struct
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_BaseObject)
	{
		/* Visuel */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
		{
			pst_Visu = _pst_GO->pst_Base->pst_Visu;

			/* GRO */
			ul_Key = (pst_Visu->pst_Object) ? LOA_ul_SearchKeyWithAddress((ULONG) pst_Visu->pst_Object) : -1;
			SAV_Buffer(&ul_Key, 4);

			/* GRM */
			ul_Key = (pst_Visu->pst_Material) ? LOA_ul_SearchKeyWithAddress((ULONG) pst_Visu->pst_Material) : -1;
			SAV_Buffer(&ul_Key, 4);

			SAV_Buffer(&pst_Visu->ul_DrawMask, 4);

			/* DUMMY */
#if defined(XML_CONV_TOOL)
			ul_Key = 0xFFFFFF00 | pst_Visu->c_DisplayOrder;
#else
			ul_Key = 0xFFFFFF00 | (pst_Visu->c_DisplayOrder + 16);
#endif
			SAV_Buffer(&ul_Key, 4);
#ifdef JADEFUSION
            BOOL bIsValid = FALSE;
            GEO_tdst_Object* pGeoObj = (GEO_tdst_Object*) pst_Visu->pst_Object;

#if !defined(XML_CONV_TOOL)
            if (pGeoObj && pGeoObj->l_NbElements>0 && pGeoObj->l_NbElements == pst_Visu->usNbElements && pst_Visu->pLMPage)
            {
                for (i = 0; i < pGeoObj->l_NbElements; i++)
                {
                    if (pst_Visu->p_us_NbTrianglesInElement[i] && pGeoObj->dst_Element[i].l_NbTriangles != pst_Visu->p_us_NbTrianglesInElement[i])
                        break;
                }	

                if (i == pGeoObj->l_NbElements)
                    bIsValid = TRUE;
            }

			if (!pGeoObj || pGeoObj->p_SKN_Objectponderation)
            {
                bIsValid = FALSE;
                //pst_Visu->pst_LightmapSettings.bReceiveShadows = false;
                //pst_Visu->pst_LightmapSettings.bUseLightmaps = false;
            }
#endif

            /* Lightmap settings */
            // read bit flags
            //SAV_Buffer(((ULONG*)(&pst_Visu->pst_LightmapSettings)), 4);
            // read ratio
            //SAV_Buffer(&pst_Visu->pst_LightmapSettings.fTexelPerMeter, 4);

            // Lightmap data for this GAO
			if(ul_Version>2)
			{
				// Save lightmap data version
				ul_Key  = LMSettingsVersion;
				SAV_Buffer(&ul_Key, 4);

				// Save the key of the texture that's needed
				if (bIsValid)
				{
					ULONG	ulData;
					unsigned int i;

					ul_Key = pst_Visu->pLMPage->texBFKey;
					SAV_Buffer(&ul_Key, 4);

					// Save the number of elements
					ulData = pst_Visu->usNbElements;
					SAV_Buffer(&ulData, 4);

					// For each element
					for (i = 0; i < pst_Visu->usNbElements; i++)
					{
						// Save the number of triangles
						if (pst_Visu->pp_st_LightmapCoords[i])
						{
							ulData = ((GEO_tdst_Object*)pst_Visu->pst_Object)->dst_Element[i].l_NbTriangles;
							SAV_Buffer(&ulData, 4);

							// Save the UVs array
							SAV_Buffer(pst_Visu->pp_st_LightmapCoords[i], ulData * sizeof(GEO_tdst_UV) * 3);
						}
						else
						{
							ulData = 0;
							SAV_Buffer(&ulData, 4);
						}
					}										
				}
				else
				{
					// will mean no lightmaps when loading
					ul_Key = BIG_C_InvalidKey;
					SAV_Buffer(&ul_Key, 4);
				}	
			}
#endif

			/* RLI */
			ul_Key = 0x80 | ('R' << 8) | ('L' << 16) | ('I' << 24);
#if defined(XML_CONV_TOOL) && defined JADEFUSION
			if (pst_Visu->dul_VertexColors != NULL)
			{
				if (pst_Visu->dul_VertexColors[0] == ul_Key)
				{
					SAV_Buffer(&pst_Visu->dul_VertexColors[0], 4);
					SAV_Buffer(&pst_Visu->dul_VertexColors[1], 4);
				}
				else
					SAV_Buffer(pst_Visu->dul_VertexColors, sizeof(LONG)*(pst_Visu->dul_VertexColors[0]+1));
			}
			else			
				SAV_Buffer(&pst_Visu->dul_VertexColors, 4);
#else
			SAV_Buffer(&ul_Key, 4);
			SAV_Buffer(&ul_RLIKey, 4);
#endif

#ifdef JADEFUSION
            // Xenon mesh processing flags
            if (ul_Version >= 4)
            {
                SAV_Buffer(&pst_Visu->ul_XenonMeshProcessingFlags, 4);
            }
            if (ul_Version >= 5)
            {
                SAV_Buffer(&pst_Visu->ul_XenonMeshProcessingCRC, 4);
            }
            if (ul_Version >= 6)
            {
                SAV_Buffer(&pst_Visu->ul_XenonMeshProcessingRLIKey, 4);
            }

            // Key of the Xenon Packed Data
            if (ul_Version >= 7)
            {
                SAV_Buffer(&pst_Visu->ul_PackedDataKey, 4);
            }

#endif


		}

		/* Hierarchy */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		{
			pst_Hierarchy = _pst_GO->pst_Base->pst_Hierarchy;
			if(pst_Hierarchy->pst_FatherInit)
			{
				ul_Key = (pst_Hierarchy->pst_FatherInit) ? LOA_ul_SearchKeyWithAddress((ULONG) pst_Hierarchy->pst_FatherInit) : -1;
				SAV_Buffer(&ul_Key, 4);
				SAV_Buffer(&pst_Hierarchy->st_LocalMatrix, sizeof(MATH_tdst_Matrix));
			}
#if defined(XML_CONV_TOOL)
			else
			{
				ul_Key = BIG_C_InvalidKey;
				SAV_Buffer(&ul_Key, 4);
				SAV_Buffer(&pst_Hierarchy->st_LocalMatrix, sizeof(MATH_tdst_Matrix));			
			}
#endif
		}

		/* Animation */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EVE_tdst_ListTracks *pst_TrackList;
			OBJ_tdst_Group		*pst_Model;
			ANI_tdst_Shape		*pst_Shape;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_GameObjectAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

			/* For the moment, save only the first anim */
			ul_Key = BIG_C_InvalidIndex;
#if defined(XML_CONV_TOOL)
			ul_Key = (ULONG)pst_GameObjectAnim->apst_Anim[0];
#else
			if(pst_GameObjectAnim->uc_AnimUsed & 1)
			{
				if(pst_GameObjectAnim->apst_Anim[0])
				{
					pst_TrackList = pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks;
					if(pst_TrackList) ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_TrackList);
				}
				else
				{
					ul_Key = BIG_C_InvalidKey;
				}
			}
#endif
			SAV_Buffer(&ul_Key, 4);

			ul_Key = BIG_C_InvalidIndex;
#if defined(XML_CONV_TOOL)
			ul_Key = (ULONG )pst_GameObjectAnim->pst_Shape;
#else
			pst_Shape = pst_GameObjectAnim->pst_DefaultShape;
			if(pst_Shape) ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Shape);
#endif
			SAV_Buffer(&ul_Key, 4);

			ul_Key = BIG_C_InvalidIndex;
#if defined(XML_CONV_TOOL)
			ul_Key = (ULONG )pst_GameObjectAnim->pst_SkeletonModel;
#else
			pst_Model = pst_GameObjectAnim->pst_SkeletonModel;
			if(pst_Model) ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Model);
#endif
			SAV_Buffer(&ul_Key, 4);
		}

		/* Action kit */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
		{
			pst_ActionKit = pst_GameObjectAnim->pst_ActionKit;
			if(pst_ActionKit)
				ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_ActionKit);
			else
				ul_Key = BIG_C_InvalidIndex;

			SAV_Buffer(&ul_Key, 4);
		}

#ifdef ODE_INSIDE
		/* ODE */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE
#if defined(XML_CONV_TOOL)
			&& !gGaoOdeBitHack		
#endif
			)
		{
#if !defined(XML_CONV_TOOL)
			//update to version 7 for the rotation matrix in ODE
			_pst_GO->pst_Base->pst_ODE->uc_Version = 7;
#endif
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->uc_Version, 1);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->uc_Type, 1);
#if !defined(XML_CONV_TOOL)
			/* Remove Hypothetical ForceImmovable Flag */
			_pst_GO->pst_Base->pst_ODE->uc_Flags &= ~ODE_FLAGS_FORCEIMMOVABLE;
#endif
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->uc_Flags, 1);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->uc_Sound, 1);

			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->st_Offset, 12);

			if(_pst_GO->pst_Base->pst_ODE->uc_Version >= 7)
				SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->st_RotMatrix, sizeof(MATH_tdst_Matrix));

#ifdef JADEFUSION
			if(_pst_GO->pst_Base->pst_ODE->uc_Version >= 6)
			{
#endif
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->f_LinearThres, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->f_AngularThres, 4);
#ifdef JADEFUSION
			}
#endif
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->mass_init, 4);

			if(_pst_GO->pst_Base->pst_ODE->uc_Type)
			{
				SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->f_X, 4);
				SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->f_Y, 4);
				SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->f_Z, 4);
			}
#ifdef JADEFUSION
			if(_pst_GO->pst_Base->pst_ODE->uc_Version >= 4)
			{
#endif
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->SurfaceMode, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->mu, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->mu2, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->bounce, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->bounce_vel, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->soft_erp, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->soft_cfm, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->motion1, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->motion2, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->slip1, 4);
			SAV_Buffer(&_pst_GO->pst_Base->pst_ODE->slip2, 4);
#ifdef JADEFUSION
			}
#endif
		}
#endif


		/* Additional matrix */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			OBJ_tdst_AdditionalMatrix	*pst_AM;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_AM = _pst_GO->pst_Base->pst_AddMatrix;
			SAV_Buffer(&pst_AM->l_Number, 4);
			if(pst_AM->l_Number)
			{
				if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
				{
					for(i = 0; i < pst_AM->l_Number; i++)
					{
						if(pst_AM->dst_GizmoPtr[i].pst_GO)
							ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_AM->dst_GizmoPtr[i].pst_GO);
						else
							ul_Key = BIG_C_InvalidKey;
						SAV_Buffer(&ul_Key, 4);
						SAV_Buffer(&pst_AM->dst_GizmoPtr[i].l_MatrixId, 4);
					}
				}
				else
				{
					SAV_Buffer
					(
						_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo,
						_pst_GO->pst_Base->pst_AddMatrix->l_Number * sizeof(OBJ_tdst_Gizmo)
					);
				}
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Save extended struct
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject)
	{
		ul_Key = BIG_C_InvalidKey;

		/* Groups */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Group)
		{
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GO->pst_Extended->pst_Group);
			ERR_X_Warning
			(
				ul_Key != BIG_C_InvalidIndex,
				"Identity (Group) without a corresponding struct",
				_pst_GO->sz_Name
			);
		}

		/* Saves the Group key or BIG_C_InvalidKey if not. */
		SAV_Buffer(&ul_Key, 4);

		if(_pst_GO->pst_Extended->pst_Modifiers)
		{			
			ul_Key = (BIG_KEY) TRUE;
#if defined(XML_CONV_TOOL)
			if (gGaoHasModifierHack != 0)
		ul_Key = gGaoHasModifierHack;
#endif
		}
		else
			ul_Key = (BIG_KEY) FALSE;

		/* Saves a (BIG_KEY) BOOL that indicates if the GameObject has Modifiers. */
		SAV_Buffer(&ul_Key, 4);

#if !defined(XML_CONV_TOOL)
		ul_Key = BIG_C_InvalidIndex;

		/* Dummy 1 */
		SAV_Buffer(&ul_Key, 4);

		/* Dummy 2 */
		SAV_Buffer(&ul_Key, 4);

		/* Dummy 3 */
		SAV_Buffer(&ul_Key, 4);

		/* Dummy 4 */
		SAV_Buffer(&ul_Key, 4);

		/* Dummy 5 */
		SAV_Buffer(&ul_Key, 4);
#else
		for (i = 0; i < 5; i++)	
			SAV_Buffer(&gGaoDummy[i], 4);
#endif
		/* Version Nb */
        ul_Key = 0;
		SAV_Buffer(&ul_Key, 4);

		/* Secto */
		SAV_Buffer(&_pst_GO->pst_Extended->auc_Sectos[0], 1);
		SAV_Buffer(&_pst_GO->pst_Extended->auc_Sectos[1], 1);
		SAV_Buffer(&_pst_GO->pst_Extended->auc_Sectos[2], 1);
		SAV_Buffer(&_pst_GO->pst_Extended->auc_Sectos[3], 1);

		SAV_Buffer(&_pst_GO->pst_Extended->uw_CapacitiesInit, 4);
        // Version 0xFFFFFFFF :
		//SAV_Buffer(&_pst_GO->pst_Extended->uw_CapacitiesInit, 2);
		//SAV_Buffer(&_pst_GO->pst_Extended->uw_Capacities, 2);
		SAV_Buffer(&_pst_GO->pst_Extended->uc_AiPrio, 1);
		SAV_Buffer(&_pst_GO->pst_Extended->uc_Blank, 1);
		SAV_Buffer(&_pst_GO->pst_Extended->uw_ExtraFlags, 2);

		/* AI */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AI)
		{
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GO->pst_Extended->pst_Ai);
			ERR_X_Warning
			(
				ul_Key != BIG_C_InvalidIndex,
				"Identity (AI) without a corresponding struct",
				_pst_GO->sz_Name
			);
			SAV_Buffer(&ul_Key, 4);
		}

		/* Events */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Events)
		{
			if(!_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Events)
			{
				ERR_X_Warning(0, "Identity (Event) without corresponding struct", _pst_GO->sz_Name);
				ul_Key = 0;
			}
			else
			{
				if(_pst_GO->pst_Extended->pst_Events->pst_ListTracks)
				{
					ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GO->pst_Extended->pst_Events->pst_ListTracks);
					ERR_X_Warning(ul_Key != BIG_C_InvalidIndex, "Invalid track list in event struct", _pst_GO->sz_Name);
					if(ul_Key == BIG_C_InvalidIndex) ul_Key = 0;
				}
				else
				{
					ul_Key = 0;
				}
			}

			SAV_Buffer(&ul_Key, 4);
		}

		/* Sound */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Sound)
		{
			if(!_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Sound)
			{
				ERR_X_Warning(0, "Identity (sound) without a corresponding struct1", _pst_GO->sz_Name);
			}
			else
			{
#if defined(XML_CONV_TOOL)
				SAV_Buffer(&_pst_GO->pst_Extended->pst_Sound, 4);
#else
				if(SND_p_MainGetBank(_pst_GO->pst_Extended->pst_Sound) && SND_p_MainGetBank(_pst_GO->pst_Extended->pst_Sound)->pi_Bank)
				{
					ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GO->pst_Extended->pst_Sound);
					ERR_X_Warning
					(
						ul_Key != BIG_C_InvalidIndex,
						"Identity (sound) without a corresponding struct2",
						_pst_GO->sz_Name
					);
					SAV_Buffer(&ul_Key, 4);
				}
#endif
			}
		}

		/* Links */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Links)
		{
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GO->pst_Extended->pst_Links);
			ERR_X_Warning
			(
				ul_Key != BIG_C_InvalidIndex,
				"Identity (links) without a corresponding struct",
				_pst_GO->sz_Name
			);
			SAV_Buffer(&ul_Key, 4);
		}

		/* Light */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)
		{
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GO->pst_Extended->pst_Light);
			ERR_X_Warning
			(
				ul_Key != BIG_C_InvalidIndex,
				"Identity (lights) without a corresponding struct",
				_pst_GO->sz_Name
			);
			SAV_Buffer(&ul_Key, 4);
		}

		/* Design struct */
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_DesignStruct)
		{
			ul_Key = sizeof(OBJ_tdst_DesignStruct);
			SAV_Buffer(&ul_Key, 4);
			L_memcpy(&st_Design, _pst_GO->pst_Extended->pst_Design, sizeof(OBJ_tdst_DesignStruct));
			if(st_Design.pst_Net1) st_Design.pst_Net1 = (WAY_tdst_Network *)LOA_ul_SearchKeyWithAddress((ULONG) st_Design.pst_Net1);
			if(st_Design.pst_Net2) st_Design.pst_Net2 = (WAY_tdst_Network *)LOA_ul_SearchKeyWithAddress((ULONG) st_Design.pst_Net2);
			if(st_Design.pst_Perso1) st_Design.pst_Perso1 = (OBJ_tdst_GameObject *)LOA_ul_SearchKeyWithAddress((ULONG) st_Design.pst_Perso1);
			if(st_Design.pst_Perso2) st_Design.pst_Perso2 = (OBJ_tdst_GameObject *)LOA_ul_SearchKeyWithAddress((ULONG) st_Design.pst_Perso2);
			SAV_Buffer(&st_Design, sizeof(OBJ_tdst_DesignStruct));
		}

		/* Modifiers */
		if(_pst_GO->pst_Extended->pst_Modifiers)
		{
			MDF_Modifier_Save(_pst_GO->pst_Extended->pst_Modifiers, 1);
			ul_Key = 0xFFFFFFFF;
			SAV_Buffer(&ul_Key, 4);
		}
#ifdef JADEFUSION
        /* Wind */
        if (_pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_AffectedByStaticWind)
        {
            //((CCurrentStaticWind *)_pst_GO->pst_Extended->po_CurrentWind)->Save();
        }
#endif
	}

	if(_pst_GO->ul_IdentityFlags & (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance);
		ERR_X_Warning
		(
			ul_Key != BIG_C_InvalidIndex,
			"Identity (ZDM | ZDE) without a corresponding struct",
			_pst_GO->sz_Name
		);
		SAV_Buffer(&ul_Key, 4);
	}

	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
	{
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap);
		ERR_X_Warning
		(
			ul_Key != BIG_C_InvalidIndex,
			"Identity (ColMap) without a corresponding struct",
			_pst_GO->sz_Name
		);
		SAV_Buffer(&ul_Key, 4);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Save editor data
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_pst_GO->sz_Name)
	{
		l_Length = L_strlen(_pst_GO->sz_Name) + 1;
		SAV_Buffer(&l_Length, 4);
		SAV_Buffer(_pst_GO->sz_Name, l_Length);
	}
	else
	{
		l_Length = 0;
		SAV_Buffer(&l_Length, 4);
	}

//	/* Empty 4 bytes that can be used... */
	l_Length = 1;
	SAV_Buffer(&l_Length, 4);
	{
		u32 Value;
		Value = _pst_GO->ucCullingVisibility;
		SAV_Buffer(&Value, 4);
	}

	SAV_Buffer(&_pst_GO->ul_ObjectModel, 4);
	SAV_Buffer(&_pst_GO->ul_InvisibleObjectIndex, 4);
	SAV_Buffer(&_pst_GO->ul_ForceLODIndex, 4);
	SAV_Buffer(&_pst_GO->ul_User3, 4);

#if defined(XML_CONV_TOOL)
	if ((_pst_GO->ul_PrefabKey != 0) && (_pst_GO->ul_PrefabKey != BIG_C_InvalidKey))
	{
		if (!gGaoSaveBothPrefabKeyHack)
		{
			l_Length = PREFABFILE_Mark;
			SAV_Buffer(&l_Length, 4);
			SAV_Buffer(&_pst_GO->ul_PrefabKey, 4);
		}
		else
		{
		l_Length = PREFABFILE_Mark1;
		SAV_Buffer(&l_Length, 4);
		SAV_Buffer(&_pst_GO->ul_PrefabKey, 4);
		SAV_Buffer(&_pst_GO->ul_PrefabObjKey, 4);
	}
	}
	else if (ul_Version > 1)
		SAV_Buffer(&l_Length, 4);
#else
	if ((_pst_GO->ul_PrefabKey != 0) && (_pst_GO->ul_PrefabKey != BIG_C_InvalidKey))
	{
		l_Length = PREFABFILE_Mark1;
		SAV_Buffer(&l_Length, 4);
		SAV_Buffer(&_pst_GO->ul_PrefabKey, 4);
		SAV_Buffer(&_pst_GO->ul_PrefabObjKey, 4);
	}
#ifdef JADEFUSION
	else
		SAV_Buffer(&l_Length, 4);
#endif
#endif

	// -NOTE- Added a bitfield to know if this GAO's properties can be
	// merged when integrating from PS2 Data. 
#ifdef JADEFUSION
#if defined(XML_CONV_TOOL)
	if (ul_Version > 1 && _pst_GO->ul_XeMergeFlags != 0xCDCDCDCD)
		SAV_Buffer(&_pst_GO->ul_XeMergeFlags, 4);
#else
	SAV_Buffer(&_pst_GO->ul_XeMergeFlags, 4);
#endif
#endif
	ul_Key = SAV_ul_End();

	if(OBJ_gl_SaveOnlyImportedDataFromMad) return ul_Key;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Save linked structures
 -----------------------------------------------------------------------------------------------------------------------
 */
#if !defined(XML_CONV_TOOL)
_Try_
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AI)
	{
		AI_SaveInstance(_pst_GO->pst_Extended->pst_Ai);
	}

	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Events)
	{
		EVE_SaveListTracks(_pst_GO, _pst_GO->pst_Extended->pst_Events->pst_ListTracks);
	}

	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Links)
	{
		if(_pst_GO->pst_Extended->pst_Links) WAY_ul_SaveStruct(_pst_GO, (WAY_tdst_Struct*)_pst_GO->pst_Extended->pst_Links);
	}

	if(_pst_GO->ul_IdentityFlags & (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		COL_SaveInstance(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance, BIG_C_InvalidIndex);
	}

	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
	{
		COL_SaveColMap(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap, BIG_C_InvalidIndex);
	}

	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	{
		if(_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit)
			ACT_ul_SaveActionKit(_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit);
	}

_Catch_
_End_
#endif // XML_CONV_TOOL
return ul_Key;
}

//#if !defined(XML_CONV_TOOL) //POPOWARNING est plus
/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *OBJ_pst_DuplicateGameObjectFile(WOR_tdst_World *pst_World, BIG_INDEX _ul_FatFile, ULONG _ul_PrefabKey, char * asz_NewName, MATH_tdst_Vector * _pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				asz_Path[BIG_C_MaxLenPath];
	OBJ_tdst_GameObject *pst_GO, *pst_NewGO;
	BOOL				b_MustDel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Load the gameobject if it is not present */
	b_MustDel = FALSE;
	pst_GO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(_ul_FatFile));
	if((BIG_INDEX) pst_GO == BIG_C_InvalidIndex)
	{
		/* Load the game object */
		WOR_gpst_WorldToLoadIn = pst_World;
		LOA_MakeFileRef(BIG_FileKey(_ul_FatFile), (ULONG *) &pst_GO, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
		LOA_Resolve();
		WOR_World_JustAfterLoadObject(pst_World, pst_GO, TRUE, TRUE);
		b_MustDel = TRUE;
	}

	/* Duplicate the object */
	pst_NewGO = OBJ_GameObject_Duplicate(pst_World, pst_GO, TRUE, TRUE, _pst_Pos, _ul_PrefabKey, asz_NewName);

	/* Remove gameobject if needed */
	if(b_MustDel)
	{
		WOR_World_DetachObject(pst_World, pst_GO);
		INT_SnP_DetachObject(pst_GO, pst_World);
		OBJ_GameObject_Remove(pst_GO, 1);
	}

	/* Status msg */
	sprintf(asz_Path, "%s%s", F3D_STR_Csz_DropGO, pst_NewGO->sz_Name);
	LINK_PrintStatusMsg(asz_Path);

	return pst_NewGO;
}

//#endif //if !defined(XML_CONV_TOOL)
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
