/*$T OBJload.c GC 1.139 04/15/04 12:17:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */



#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h" 
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "OBJects/OBJload.h"
#include "OBJects/OBJinit.h"
#include "OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WAYs/WAYload.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ACTions/ACTload.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "GEOmetric/GEOload.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLload.h"
#include "ENGine/Sources/COLlision/COLsave.h"
#include "ENGine/Sources/COLlision/COLinit.h"
#include "ENGine/Sources/COLlision/COLset.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/GRP/GRPload.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDbank.h"
#include "EDIpaths.h"
#include "EDItors/Sources/PreFaB/PFBdata.h"
#include "GraphicDK/Sources/GEOmetric/GEO_MRM.h"
#if defined(_XBOX)|| defined(_XENON)
#include "Gx8/Gx8color.h"
#endif

#ifdef JADEFUSION
#include "texture/texfile.h"
#include "texture/texstruct.h"
#include "LIGHT/LIGHTmap.h"
#include "Engine/Sources/Wind/CurrentWind.h"
#endif

#if defined(_XENON_RENDER)
#include "GraphicDK/Sources/GEOmetric/GEOXenonPack.h"
#endif

#ifdef ACTIVE_EDITORS
BAS_tdst_barray WOR_ListAllKeys;
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

OBJ_tdst_GameObject *OBJ_gpst_ObjectToLoadIn;
extern ULONG		LOA_ul_FileTypeSize[40];
extern ULONG		LOA_gul_CurrentKey;

#if defined(XML_CONV_TOOL)
extern ULONG    gGaoVersion;
extern ULONG    gGaoDummy[];
extern ULONG    gGaoBvDummy;
extern ULONG	gGaoHasModifierHack;
BOOL gGaoSaveBothPrefabKeyHack;
BOOL gGaoOdeBitHack;
#endif


/*$4
 ***********************************************************************************************************************
    Define
 ***********************************************************************************************************************
 */
#define UPDATABLE_CONTROLFLAGS \
	(\
		OBJ_C_ControlFlag_EnableSnP | \
		OBJ_C_ControlFlag_ForceRTL |\
		OBJ_C_ControlFlag_ForceNoRTL |\
		OBJ_C_ControlFlag_ForceDetectionList |\
		OBJ_C_ControlFlag_LookAt	 |\
		OBJ_C_ControlFlag_RayInsensitive |\
		OBJ_C_ControlFlag_EditableBV |\
		OBJ_C_ControlFlag_InPause |\
		OBJ_C_ControlFlag_AnimDontTouch\
	)

/*
		OBJ_C_ControlFlag_SectoInvisible |\
		OBJ_C_ControlFlag_SectoInactive |\
		OBJ_C_ControlFlag_ForceInvisibleInit |\
		OBJ_C_ControlFlag_ForceInactiveInit |\
		OBJ_C_ControlFlag_AlwaysActive |\
		OBJ_C_ControlFlag_AlwaysVisible |\
		OBJ_C_ControlFlag_ForceInactive	|\
		OBJ_C_ControlFlag_ForceInvisible |\	
		OBJ_C_ControlFlag_SectoReinit | \
*/	

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */
 extern void ANI_CloneSet(OBJ_tdst_GameObject *_pst_GO1, OBJ_tdst_GameObject *_pst_GO2, ULONG _ul_NumAction);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG OBJ_ul_GameObjectRLICallback(ULONG _ul_PosFile)
{
	ULONG	*pul_Buffer, *dul_RLI, ul_Size, ul_Buffer,*pCurrentVertex;
	int i,iVertexNb;
	
	pul_Buffer = (ULONG *) BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);
	if ( !pul_Buffer ) return 0;
	
	ul_Buffer = LOA_ReadLong((char**)&pul_Buffer);
	if ( ul_Buffer != (0x80 | ('R' << 8) | ('L' << 16) | ('I' << 24)) )
		return 0;
	
	ul_Size -= 4;
	
	LOA_ul_FileTypeSize[27] += ul_Size;
	dul_RLI = (ULONG* )MEM_p_VMAlloc( ul_Size);
	
	// Read longs (for big/little endian pbs)
	iVertexNb = ul_Size/4;
	pCurrentVertex = dul_RLI;
	for (i=0; i<iVertexNb; i++,pCurrentVertex++)
		*pCurrentVertex = LOA_ReadULong((char**)&pul_Buffer);
		
	// LOA_ReadCharArray((char**)&pul_Buffer, (CHAR *) dul_RLI, ul_Size);
	return (ULONG) dul_RLI;
}





/*
 =======================================================================================================================
 =======================================================================================================================
 */

ULONG OBJ_ul_GameObjectCallback(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR					*pc_Buffer;
	OBJ_tdst_GameObject		*pst_GO;
	ULONG					ul_Id;
	ULONG					ul_Size;
	ULONG					ul_EditorFlags;
	BIG_KEY					ul_Key;
	LONG					l_Value, i;
	MATH_tdst_Matrix		st_Matrix;
	ANI_st_GameObjectAnim	*pst_GameObjectAnim;
	OBJ_tdst_Hierarchy		*pst_Hierarchy;
	BOOL					b_HasModifiers;
	ULONG					ul_Mask;
#ifdef ACTIVE_EDITORS
	char					az[200];
	LONG					l_RegisterFlags;
#endif
#ifdef JADEFUSION
	ULONG ul_Version = 0;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(XML_CONV_TOOL)
	gGaoSaveBothPrefabKeyHack = false;
	gGaoOdeBitHack = false;
	gGaoHasModifierHack = false;
#endif

	pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);

	LOA_ReadCharArray(&pc_Buffer, (CHAR *) &ul_Key, 4);
#if defined(PSX2_TARGET) && defined(__CW__)
	if(strncmp((CHAR *) &ul_Key, EDI_Csz_ExtGameObject, 4)) return(ULONG) NULL;
#else
	if(strncmp((CHAR *) &ul_Key, EDI_Csz_ExtGameObject, 4)) return NULL;
#endif

#ifdef JADEFUSION
    ul_Version = LOA_ReadULong(&pc_Buffer);
#else
	/* Version number : old, not used anymore */
	LOA_ReadLong_Ed(&pc_Buffer, NULL);
#endif
#if defined(XML_CONV_TOOL)
	gGaoVersion = ul_Version;
#endif

	/* Get editors flags */
	ul_EditorFlags = LOA_ReadULong(&pc_Buffer);

	/* Get identity flags */
	ul_Id = LOA_ReadULong(&pc_Buffer);

#if !defined(XML_CONV_TOOL)
	/* Clear the identity flag dyna befor saving */
	ul_Id &= (~OBJ_C_IdentityFlag_Dyna);
#endif

	pst_GO = OBJ_GameObject_Create(ul_Id);
	pst_GO->ul_MyKey = LOA_gul_CurrentKey;
	OBJ_gpst_ObjectToLoadIn = pst_GO;

#if !defined(XML_CONV_TOOL)
	pst_GO->ul_StatusAndControlFlags = OBJ_C_ControlFlag_LookAt |
		OBJ_C_ControlFlag_RayInsensitive |
		OBJ_C_ControlFlag_EditableBV |
		OBJ_C_ControlFlag_AlwaysActive |
		OBJ_C_ControlFlag_AlwaysVisible |
		OBJ_C_ControlFlag_ForceRTL |
		OBJ_C_ControlFlag_ForceNoRTL |
		OBJ_C_ControlFlag_ForceDetectionList |
		OBJ_C_ControlFlag_ForceInvisibleInit |
		OBJ_C_ControlFlag_ForceInactiveInit |
		OBJ_C_ControlFlag_EnableSnP;
#else
	pst_GO->ul_StatusAndControlFlags = 0xFFFFFFFF;	// enable all bits for XML conv
#endif

	ul_Mask = LOA_ReadLong(&pc_Buffer);
	pst_GO->ul_StatusAndControlFlags &= ul_Mask;

#if !defined(XML_CONV_TOOL)
	if
	(
		pst_GO
	&&	pst_GO->pst_Extended
	&&	pst_GO->pst_Extended->pst_Col
	&&	((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_List
	)
	{
		OBJ_SetStatusFlag(pst_GO, OBJ_C_StatusFlag_Detection);
	}
	else
	{
		OBJ_ClearStatusFlag(pst_GO, OBJ_C_StatusFlag_Detection);
	}

	if(ul_Mask & OBJ_C_ControlFlag_ForceInvisibleInit)
	{
		pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceInvisible;
	}

	if(ul_Mask & OBJ_C_ControlFlag_ForceInactiveInit)
	{
		pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceInactive;
	}

	pst_GO->ul_StatusAndControlFlags |= ul_Mask & 0x0000FF00;

	/* Update the status flag */
	OBJ_UpdateStatusFlagRTL(pst_GO);

	/* Update the Detection List Flag */
	if
	(
		!(pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Detection)
	&&	(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceDetectionList)
	)
	{
		OBJ_SetStatusFlag(pst_GO, OBJ_C_StatusFlag_Detection);
		COL_AllocDetectionList(pst_GO);
	}
#endif

	pst_GO->uc_Secto = LOA_ReadUChar(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
	pst_GO->uc_Secto = 0;
#endif
	pst_GO->uc_VisiCoeff = LOA_ReadUChar(&pc_Buffer);

	/* We skip the 2 extra bytes added for alignment */
	LOA_ReadShort_Ed(&pc_Buffer, NULL);

#if !defined(XML_CONV_TOOL)
	/* Zero is a non valid value, default is 16 */
	if(pst_GO->uc_VisiCoeff == 0) pst_GO->uc_VisiCoeff = 16;
#endif

	pst_GO->uc_LOD_Vis = LOA_ReadUChar(&pc_Buffer);
	pst_GO->uc_LOD_Dist = LOA_ReadUChar(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
	pst_GO->uc_LOD_Dist = 0;
#endif
	pst_GO->uc_DesignFlags = LOA_ReadUChar(&pc_Buffer);
/*#ifdef JADEFUSION
	pst_GO->c_FixFlags = LOA_ReadChar(&pc_Buffer);//popowarning a virer ??
#endif*/
#if !defined(XML_CONV_TOOL)
	pst_GO->c_FixFlags = LOA_ReadChar(&pc_Buffer) &~OBJ_C_HasBeenMerge;
	pst_GO->c_FixFlags &= ~OBJ_C_ProcessedAI;
	pst_GO->c_FixFlags &= ~OBJ_C_ProcessedDyn;
	pst_GO->c_FixFlags &= ~OBJ_C_ProcessedHie;
	pst_GO->c_FixFlags &= 0xFC;
	pst_GO->c_FixFlags |= (pst_GO->c_FixFlags & 4) >> 2;
	pst_GO->c_FixFlags |= (pst_GO->c_FixFlags & 8) >> 2;
#endif

	/* Load orientation / position */
	LOA_ReadMatrix(&pc_Buffer, &st_Matrix);

#if !defined(XML_CONV_TOOL)
	/* Some matrix are saved with bad types */
	MATH_SetCorrectType(&st_Matrix);
#endif
	// Do not take hierarchie into account yet
	if(ul_Id & OBJ_C_IdentityFlag_HasInitialPos)
	{
		MATH_CopyMatrix(pst_GO->pst_GlobalMatrix + 1, &st_Matrix);
		if(ul_Id & OBJ_C_IdentityFlag_FlashMatrix)   MATH_CopyMatrix(pst_GO->pst_GlobalMatrix + 2, &st_Matrix);
	}
	else
		if(ul_Id & OBJ_C_IdentityFlag_FlashMatrix)   MATH_CopyMatrix(pst_GO->pst_GlobalMatrix + 1, &st_Matrix);
	MATH_CopyMatrix(pst_GO->pst_GlobalMatrix, &st_Matrix);

	/* Load bounding volume */
	if(pst_GO->pst_BV)
	{
#if defined(XML_CONV_TOOL)
		LOA_ReadLong_Ed(&pc_Buffer, (LONG* )&gGaoBvDummy);	/* skip 4 dummy bytes (== 0xFFFFFFFF) */
#else
		LOA_ReadLong_Ed(&pc_Buffer, NULL);	/* skip 4 dummy bytes (== 0xFFFFFFFF) */
#endif
		if(ul_Id & OBJ_C_IdentityFlag_OBBox)
		{
			LOA_ReadVector(&pc_Buffer, OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));
			MATH_CopyVector(OBJ_pst_BV_GetGMin(pst_GO->pst_BV), OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));

			LOA_ReadVector(&pc_Buffer, OBJ_pst_BV_GetGMaxInit(pst_GO->pst_BV));
			MATH_CopyVector(OBJ_pst_BV_GetGMax(pst_GO->pst_BV), OBJ_pst_BV_GetGMaxInit(pst_GO->pst_BV));

			LOA_ReadVector(&pc_Buffer, OBJ_pst_BV_GetLMin(pst_GO->pst_BV));
			LOA_ReadVector(&pc_Buffer, OBJ_pst_BV_GetLMax(pst_GO->pst_BV));
		}
		else
		{
			LOA_ReadVector(&pc_Buffer, OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));
			MATH_CopyVector(OBJ_pst_BV_GetGMin(pst_GO->pst_BV), OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));
			LOA_ReadVector(&pc_Buffer, OBJ_pst_BV_GetGMaxInit(pst_GO->pst_BV));
			MATH_CopyVector(OBJ_pst_BV_GetGMax(pst_GO->pst_BV), OBJ_pst_BV_GetGMaxInit(pst_GO->pst_BV));
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Load base struct
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(ul_Id & OBJ_C_IdentityFlag_BaseObject)
	{
		/* Visu */
		if(ul_Id & OBJ_C_IdentityFlag_Visu)
		{
			/*~~~~~~~~~~~~~~~~~~~*/
			ULONG	numberOfVertex;
			/*~~~~~~~~~~~~~~~~~~~*/

			/* Gro */
			ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
			if(ul_Key != BIG_C_InvalidIndex)
			{
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_GO->pst_Base->pst_Visu->pst_Object,
					GEO_ul_Load_ObjectCallback,
					LOA_C_MustExists
				);
			}
			else
				pst_GO->pst_Base->pst_Visu->pst_Object = NULL;

			/* GRM */
			ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
			if(ul_Key != BIG_C_InvalidIndex)
			{
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_GO->pst_Base->pst_Visu->pst_Material,
					GEO_ul_Load_ObjectCallback,
					LOA_C_MustExists
				);
			}
			else
				pst_GO->pst_Base->pst_Visu->pst_Material = NULL;

			/* Draw mask */
			pst_GO->pst_Base->pst_Visu->ul_DrawMask = LOA_ReadULong(&pc_Buffer);

#ifdef JADEFUSION
#if !defined(XML_CONV_TOOL)
			// Invert the shadow buffer flags for version 0
            if(ul_Version == 0)
            {
                pst_GO->pst_Base->pst_Visu->ul_DrawMask &= ~(GDI_Cul_DM_EmitShadowBuffer | GDI_Cul_DM_ReceiveShadowBuffer);
            }
#endif
#endif
			/* DUMMY */
			pst_GO->pst_Base->pst_Visu->c_DisplayOrder = (char) (LOA_ReadULong(&pc_Buffer) & 0xff);
#if !defined(XML_CONV_TOOL)
			if(pst_GO->pst_Base->pst_Visu->c_DisplayOrder == -1)
				pst_GO->pst_Base->pst_Visu->c_DisplayOrder = 0;
			else
			{
				if(pst_GO->pst_Base->pst_Visu->c_DisplayOrder > 10) pst_GO->pst_Base->pst_Visu->c_DisplayOrder -= 16;
			}
#endif
#ifdef JADEFUSION
            if(ul_Version>2)
            {
                /* Lightmap settings */
                {
                    // read bit flags
                    //LOA_ReadULong_Ed(&pc_Buffer, ((ULONG*)(&pst_GO->pst_Base->pst_Visu->pst_LightmapSettings)));
                    // read ratio
                    //LOA_ReadFloat_Ed(&pc_Buffer, &pst_GO->pst_Base->pst_Visu->pst_LightmapSettings.fTexelPerMeter);

                  /*  pst_GO->pst_Base->pst_Visu->pst_LightmapSettings.bCastShadows = true;
                    pst_GO->pst_Base->pst_Visu->pst_LightmapSettings.bCustomTexelRatio = false;
                    pst_GO->pst_Base->pst_Visu->pst_LightmapSettings.bReceiveShadows = true;
                    pst_GO->pst_Base->pst_Visu->pst_LightmapSettings.bTemporaryStopUsingLightmaps = false;
                    pst_GO->pst_Base->pst_Visu->pst_LightmapSettings.bUseLightmaps = true;
                    pst_GO->pst_Base->pst_Visu->pst_LightmapSettings.fTexelPerMeter = 2.0f;*/

                    //pst_GO->pst_Base->pst_Visu->pst_LightmapSettings.bReceiveShadows = ((pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_ReceiveShadowBuffer) != 0);
                    //pst_GO->pst_Base->pst_Visu->pst_LightmapSettings.bCastShadows = ((pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_EmitShadowBuffer) != 0);

                }

                /* lightmap data */
                {
                    LIGHT_eLMDataVersion version;
                    BIG_KEY	pageKey;
                    ULONG	nbElements;
                    ULONG	nbTris;
                    ULONG	i, j;
                    float*	pUV;

                    version = (LIGHT_eLMDataVersion) LOA_ReadULong(&pc_Buffer);

#ifdef ACTIVE_EDITORS
                    BOOL bSkipLMLoad = FALSE;
                    // In editor, read the key and check if it exists and then binarize it
                    CHAR * pcTempBuffer = pc_Buffer;
                    BIG_KEY ul_Key;
                    ul_Key = _LOA_ReadULong(&pcTempBuffer, &ul_Key, LOA_eBinLookAheadData);

                    // check if the key is not invalid but does not exist in the bigfile
                    if(ul_Key != BIG_C_InvalidKey && (int) BIG_ul_SearchKeyToPos(ul_Key) == BIG_C_InvalidIndex)
                    {
                        ERR_X_Warning(0, ERR_szFormatMessage("Invalid lightmap texture key [%08x] in object [%08x], it has been removed", ul_Key, LOA_ul_GetCurrentKey()), NULL);
                        // modify the data to be read with an invalid key (so that it will binarize an invalid key)
                        *(ULONG*)pc_Buffer = BIG_C_InvalidKey;
                        bSkipLMLoad = TRUE;
                    }
#endif // #ifdef ACTIVE_EDITORS

                    // read the texture key
                    pageKey = LOA_ReadULong(&pc_Buffer);

#ifdef ACTIVE_EDITORS
                    if((pageKey == BIG_C_InvalidKey) && bSkipLMLoad)
                    {
                        nbElements = _LOA_ReadULong(&pc_Buffer, &nbElements, LOA_eBinLookAheadData);

                        for (i = 0; i < nbElements; i++)
                        {
                            // read the number of triangles
                            nbTris = _LOA_ReadULong(&pc_Buffer, &nbTris, LOA_eBinLookAheadData);

                            pc_Buffer += (nbTris * sizeof(float) * 6);	// 3 uv pairs per triangle
                        }

                        pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords = NULL;
                        pst_GO->pst_Base->pst_Visu->pLMPage = NULL;
                        pst_GO->pst_Base->pst_Visu->usNbElements = 0;
                        pst_GO->pst_Base->pst_Visu->p_us_NbTrianglesInElement = NULL;
                    }
#endif
                    
                    if (pageKey != BIG_C_InvalidKey)
                    {
                        // check if that page is loaded				
                        pst_GO->pst_Base->pst_Visu->pLMPage = LIGHT_Lightmaps_GetPointerForKey(pageKey, true, true, false);

                        // read the number of elements
                        nbElements = LOA_ReadULong(&pc_Buffer);

                        pst_GO->pst_Base->pst_Visu->usNbElements = (unsigned short)nbElements;

                        if (nbElements)
                        {
                            // allocate element UV pointers
                            pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords = (float**) MEM_p_Alloc(sizeof(GEO_tdst_UV*) * nbElements);
                            pst_GO->pst_Base->pst_Visu->p_us_NbTrianglesInElement = (USHORT*) MEM_p_Alloc(sizeof(USHORT) * nbElements);

                            // for each element
                            for (i = 0; i < nbElements; i++)
                            {
                                // read the number of triangles
                                nbTris = LOA_ReadULong(&pc_Buffer);

                                pst_GO->pst_Base->pst_Visu->p_us_NbTrianglesInElement[i] = (unsigned short)nbTris;

                                if (nbTris)
                                {
                                    // allocate space for the UVs
                                    pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords[i] = (float*) MEM_p_Alloc(sizeof(GEO_tdst_UV) * nbTris * 3);						
                                    pUV = pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords[i];

                                    // read the UVs
                                    for (j = 0; j < nbTris * 6; j++)
                                    {
                                        *pUV++ = LOA_ReadFloat(&pc_Buffer);
                                    }
                                }
                                else
                                {
                                    pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords[i] = NULL;
                                }
                            }

#ifdef ACTIVE_EDITORS
                            // check if the object really has lightmaps, (all materials could be set not to have em, or settings could have changed)
                            bool bReallyHasLightmaps = false;

                            for (i = 0; i < nbElements; i++)
                            {
                                if (pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords[i])
                                {
                                    bReallyHasLightmaps = true;
                                    break;						
                                }
                            }

                            if (!bReallyHasLightmaps)// || !pst_GO->pst_Base->pst_Visu->pst_LightmapSettings.bUseLightmaps)
                            {
                                // free the lm coords buffer
                                MEM_Free(pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords);
                                pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords = NULL;
                                //LIGHT_Lightmaps_RemoveRefLightmapPage(pst_GO->pst_Base->pst_Visu->pLMPage);
                                pst_GO->pst_Base->pst_Visu->usNbElements = 0;
                                MEM_Free(pst_GO->pst_Base->pst_Visu->p_us_NbTrianglesInElement);
                                pst_GO->pst_Base->pst_Visu->p_us_NbTrianglesInElement = NULL;
                            }
#endif
                        }
                    }
                    else
                    {
                        pst_GO->pst_Base->pst_Visu->pLMPage = NULL;
                    }
                }
            }
#endif//jadefusion

			/* RLI */
			numberOfVertex = LOA_ReadULong(&pc_Buffer);
			pst_GO->pst_Base->pst_Visu->dul_VertexColors = NULL;
			if (numberOfVertex == (0x80 | ('R' << 8) | ('L' << 16) | ('I' << 24) ) )
			{
				ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
#if defined(XML_CONV_TOOL)
				pst_GO->pst_Base->pst_Visu->dul_VertexColors = (ULONG*)MEM_p_Alloc(2*sizeof(ULONG));
				pst_GO->pst_Base->pst_Visu->dul_VertexColors[0] = numberOfVertex;
				pst_GO->pst_Base->pst_Visu->dul_VertexColors[1] = ul_Key;
#else					
				if ( (ul_Key != 0) && (ul_Key != BIG_C_InvalidKey) )
                {
					LOA_MakeFileRef( ul_Key, (ULONG *) &pst_GO->pst_Base->pst_Visu->dul_VertexColors, OBJ_ul_GameObjectRLICallback, LOA_C_MustExists | LOA_C_OnlyOneRef );

                    // Change RLI to apply indirection, not in bin (speed) mode
                    if (!LOA_gb_SpeedMode && pst_GO->pst_Base->pst_Visu->pst_Object &&
                        pst_GO->pst_Base->pst_Visu->dul_VertexColors &&
                        (pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric))
                    {
                        GEO_tdst_Object *pst_Object = (GEO_tdst_Object *)pst_GO->pst_Base->pst_Visu ->pst_Object;
                        if (pst_Object->p_MRM_Levels && pst_Object->p_MRM_Levels->dus_ReorderBuffer)
                            GEO_MRM_v_ApplyReorder((LONG* )(pst_GO->pst_Base->pst_Visu->dul_VertexColors + 1),  pst_Object->p_MRM_Levels->dus_ReorderBuffer ,pst_Object->l_NbPoints );
                    }
                }
#endif
			}
			else if(numberOfVertex != 0)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				ULONG	i;
				ULONG	*pulVC;
				size_t	arraySize = (numberOfVertex + 1) * sizeof(ULONG);
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_GO->pst_Base->pst_Visu->dul_VertexColors = (ULONG*)MEM_p_Alloc(arraySize);
				LOA_ul_FileTypeSize[27] += arraySize;
				pst_GO->pst_Base->pst_Visu->dul_VertexColors[0] = numberOfVertex;
				pulVC = pst_GO->pst_Base->pst_Visu->dul_VertexColors + 1;
				for(i = 0; i < numberOfVertex; ++i, pulVC++)
					*pulVC = LOA_ReadULong(&pc_Buffer);
			}
#ifdef JADEFUSION
            // Xenon mesh processing flags
            ULONG ul_XMPFlags  = 0;
            ULONG ul_XMPCRC    = 0;
            ULONG ul_XMPRLIKey = BIG_C_InvalidKey;
            if (ul_Version >= 4)
            {
                ul_XMPFlags = LOA_ReadULong(&pc_Buffer);
            }
            if (ul_Version >= 5)
            {
                ul_XMPCRC = LOA_ReadULong(&pc_Buffer);
            }
            if (ul_Version >= 6)
            {
                ul_XMPRLIKey = LOA_ReadULong(&pc_Buffer);
            }
#if defined(ACTIVE_EDITORS)
            pst_GO->pst_Base->pst_Visu->ul_XenonMeshProcessingFlags  = ul_XMPFlags;
            pst_GO->pst_Base->pst_Visu->ul_XenonMeshProcessingCRC    = ul_XMPCRC;
            pst_GO->pst_Base->pst_Visu->ul_XenonMeshProcessingRLIKey = ul_XMPRLIKey;
#endif

            // Xenon Packed Data key
            pst_GO->pst_Base->pst_Visu->ul_PackedDataKey = BIG_C_InvalidKey;
            if (ul_Version >= 7)
            {
#if defined(ACTIVE_EDITORS)
				// SC: Check the key that LOA_ReadULong() would read
				pst_GO->pst_Base->pst_Visu->ul_PackedDataKey = *(ULONG*)pc_Buffer;

				// SC: Check if the corresponding file exists
				ULONG ulTempFilePos = BIG_C_InvalidKey;
				if (pst_GO->pst_Base->pst_Visu->ul_PackedDataKey != BIG_C_InvalidKey)
				{
					ulTempFilePos = BIG_ul_SearchKeyToFat(pst_GO->pst_Base->pst_Visu->ul_PackedDataKey);
				}

				// SC: File doesn't exist, make sure next time we'll read an invalid key
				if (ulTempFilePos == BIG_C_InvalidKey)
				{
					*(ULONG*)pc_Buffer = BIG_C_InvalidKey;
				}
#endif
                pst_GO->pst_Base->pst_Visu->ul_PackedDataKey = LOA_ReadULong(&pc_Buffer);

                // Binary mode: Xenon packed data is right there for us to use
                if (
                    LOA_IsBinaryData()
#if defined(ACTIVE_EDITORS)
                    || LOA_IsBinarizing()
#endif
                   )
                {
                    if ((pst_GO->pst_Base->pst_Visu->ul_PackedDataKey != 0) &&
                        (pst_GO->pst_Base->pst_Visu->ul_PackedDataKey != BIG_C_InvalidKey))
                    {
                        LOA_MakeFileRef(pst_GO->pst_Base->pst_Visu->ul_PackedDataKey,
                                        (ULONG*)&pst_GO->pst_Base->pst_Visu->ul_PackedDataKey,
                                        GEO_ul_PackDataLoadCallback,
                                        LOA_C_MustExists | LOA_C_NotSavePtr);
                    }
                }
            }
#endif
		}

		/* Hierarchy */
		if(ul_Id & OBJ_C_IdentityFlag_Hierarchy)
		{
			pst_Hierarchy = pst_GO->pst_Base->pst_Hierarchy;
			ul_Key = LOA_ReadULong(&pc_Buffer);
#if defined(XML_CONV_TOOL)
			*(ULONG* )&pst_Hierarchy->pst_FatherInit = ul_Key;
			*(ULONG* )&pst_Hierarchy->pst_Father     = ul_Key;
#else
			if(ul_Key != BIG_C_InvalidKey)
			{
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_Hierarchy->pst_FatherInit,
					OBJ_ul_GameObjectCallback,
					LOA_C_MustExists
				);
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_Hierarchy->pst_Father,
					OBJ_ul_GameObjectCallback,
					LOA_C_MustExists
				);
			}
			else
				pst_Hierarchy->pst_FatherInit = pst_Hierarchy->pst_Father = NULL;
#endif

			LOA_ReadMatrix(&pc_Buffer, &pst_Hierarchy->st_LocalMatrix);

			// Update initial and flash matrix
			if(ul_Id & OBJ_C_IdentityFlag_HasInitialPos)
			{
                // May produce a bug with hierarchies
				//MATH_CopyMatrix(pst_GO->pst_GlobalMatrix + 1, &pst_Hierarchy->st_LocalMatrix);
				if(ul_Id & OBJ_C_IdentityFlag_FlashMatrix)   MATH_CopyMatrix(pst_GO->pst_GlobalMatrix + 2, &pst_Hierarchy->st_LocalMatrix);
			}
			else
				if(ul_Id & OBJ_C_IdentityFlag_FlashMatrix)   MATH_CopyMatrix(pst_GO->pst_GlobalMatrix + 1, &pst_Hierarchy->st_LocalMatrix);
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Animation
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(ul_Id & OBJ_C_IdentityFlag_Anims)
		{
			pst_GameObjectAnim = pst_GO->pst_Base->pst_GameObjectAnim;

#ifdef ACTIVE_EDITORS
			ul_Key = LOA_ReadULong(&pc_Buffer);
			if(BIG_ul_SearchKeyToFat(ul_Key) != BIG_C_InvalidKey)
				pst_GameObjectAnim->apst_Anim[0] = (ANI_tdst_Anim *) ul_Key;
			else
				pst_GameObjectAnim->apst_Anim[0] = (ANI_tdst_Anim *) BIG_C_InvalidKey;

			ul_Key = LOA_ReadULong(&pc_Buffer);
			if(BIG_ul_SearchKeyToFat(ul_Key) != BIG_C_InvalidKey)
				pst_GameObjectAnim->pst_Shape = (ANI_tdst_Shape *) ul_Key;
			else
			{
#if !defined(XML_CONV_TOOL)
				if(ul_Key != BIG_C_InvalidKey)
				{
					sprintf(az, "A shape does not exists in that bigfile (%x)", ul_Key);
					ERR_X_Warning(0, az, NULL);
				}
#endif
				pst_GameObjectAnim->pst_Shape = (ANI_tdst_Shape *) BIG_C_InvalidKey;
			}

			ul_Key = LOA_ReadULong(&pc_Buffer);
			if(BIG_ul_SearchKeyToFat(ul_Key) != BIG_C_InvalidKey)
				pst_GameObjectAnim->pst_SkeletonModel = (OBJ_tdst_Group *) ul_Key;
			else
			{
#if !defined(XML_CONV_TOOL)
				if(ul_Key != BIG_C_InvalidKey)
				{
					sprintf(az, "A skeleton does not exists in that bigfile (%x)", ul_Key);
					ERR_X_Warning(0, az, NULL);
				}
#endif
				pst_GameObjectAnim->pst_SkeletonModel = (OBJ_tdst_Group *) BIG_C_InvalidKey;
			}

#else
			ul_Key = LOA_ReadULong(&pc_Buffer);
			pst_GameObjectAnim->apst_Anim[0] = (ANI_tdst_Anim *) ul_Key;

			ul_Key = LOA_ReadULong(&pc_Buffer);
			pst_GameObjectAnim->pst_Shape = (ANI_tdst_Shape *) ul_Key;

			ul_Key = LOA_ReadULong(&pc_Buffer);
			pst_GameObjectAnim->pst_SkeletonModel = (OBJ_tdst_Group *) ul_Key;
#endif

			/*$1
			 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			    Action kit
			 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			 */

			ul_Key = LOA_ReadULong(&pc_Buffer);
			if(ul_Key != BIG_C_InvalidIndex && ul_Key != 0)
			{
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit,
					ACT_ul_ActionKitCallback,
					LOA_C_MustExists
				);
			}
		}
#ifdef ODE_INSIDE
		/* ODE */
		if(ul_Id & OBJ_C_IdentityFlag_ODE)
		{
			DYN_tdst_ODE	*pst_ODE;

			pst_ODE = pst_GO->pst_Base->pst_ODE;

			/* Version for Load/save process for ode */
			pst_ODE->uc_Version = LOA_ReadUChar(&pc_Buffer);
#if defined(XML_CONV_TOOL)
			// some old files seem to have the ODE bit set even if no ODE data was saved
			// hack around it
			if (pst_ODE->uc_Version == 0xff)
			{
				pc_Buffer--;	// un-read uc_Version
				gGaoOdeBitHack = true;
			}
			else
			{
#endif			
			/* Type od ODE Primitive */
			pst_ODE->uc_Type = LOA_ReadUChar(&pc_Buffer);

			/* Flags */
			pst_ODE->uc_Flags = LOA_ReadUChar(&pc_Buffer);

			/* Sound */
			pst_ODE->uc_Sound = LOA_ReadUChar(&pc_Buffer);

			if(pst_ODE->uc_Version >= 2)
			{
				LOA_ReadVector(&pc_Buffer, &pst_ODE->st_Offset);
			}
			else
			{
				MATH_InitVector(&pst_ODE->st_Offset, 0.0f, 0.0f, 0.0f);
			}

			if(pst_ODE->uc_Version >= 7)
			{
				LOA_ReadMatrix(&pc_Buffer, &pst_ODE->st_RotMatrix);
			}
			else
			{
				MATH_SetIdentityMatrix(&pst_ODE->st_RotMatrix);
			}

			if(pst_ODE->uc_Version >= 6)
			{
				pst_ODE->f_LinearThres = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->f_AngularThres = LOA_ReadFloat(&pc_Buffer);
			}
			else
			{
				pst_ODE->f_LinearThres = 0.2f;
				pst_ODE->f_AngularThres = 0.2f;
			}


			/* Read Mass */
			pst_ODE->mass_init = LOA_ReadFloat(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
			/* Anti Internal ODE Bug */
			if(pst_ODE->mass_init == 0.0f)
				pst_ODE->mass_init = 0.1f;
#endif
			if(pst_ODE->uc_Type)
			{
				pst_ODE->f_X = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->f_Y = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->f_Z = LOA_ReadFloat(&pc_Buffer);
			}

			if(pst_ODE->uc_Version >= 4)
			{
				pst_ODE->SurfaceMode = LOA_ReadULong(&pc_Buffer);
				pst_ODE->mu_init = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->mu = pst_GO->pst_Base->pst_ODE->mu_init;
				pst_ODE->mu2 = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->bounce_init = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->bounce = pst_GO->pst_Base->pst_ODE->bounce_init;
				pst_ODE->bounce_vel = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->soft_erp = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->soft_cfm = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->motion1 = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->motion2 = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->slip1 = LOA_ReadFloat(&pc_Buffer);
				pst_ODE->slip2 = LOA_ReadFloat(&pc_Buffer);
			}
#if defined(XML_CONV_TOOL)
			// end of ode hack
			}
#endif
		}
#else
	if(ul_Id & OBJ_C_IdentityFlag_ODE)
	{
		UCHAR				uc_Type;
		UCHAR				Version;
		MATH_tdst_Vector	st_Vect;

		/* Version */
		Version = LOA_ReadUChar(&pc_Buffer);

		/* Type od ODE Primitive */
		uc_Type = LOA_ReadUChar(&pc_Buffer);

		/* Flags */
		LOA_ReadUChar(&pc_Buffer);

		/* Dummy */
		LOA_ReadUChar(&pc_Buffer);

		LOA_ReadVector(&pc_Buffer, &st_Vect);

		if(Version >= 6)
		{
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
		}

		/* Read Mass */
		LOA_ReadFloat(&pc_Buffer);

		if(uc_Type)
		{
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
		}

		if(Version >= 4)
		{
			LOA_ReadULong(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
			LOA_ReadFloat(&pc_Buffer);
		}
	}

#endif

		/* Additional matrix */
		if(ul_Id & OBJ_C_IdentityFlag_AdditionalMatrix)
		{
#ifdef ACTIVE_EDITORS
			pst_GO->pst_Base->pst_AddMatrix->ul_GrpIndex = 0;
#endif
			l_Value = LOA_ReadLong(&pc_Buffer);
			pst_GO->pst_Base->pst_AddMatrix->l_Number = l_Value;
			if(l_Value)
			{
				if(ul_Id & OBJ_C_IdentityFlag_AddMatArePointer)
				{
					pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr = (OBJ_tdst_GizmoPtr *) MEM_p_Alloc(l_Value * sizeof(OBJ_tdst_GizmoPtr));
					LOA_ul_FileTypeSize[34] += l_Value * sizeof(OBJ_tdst_GizmoPtr);
					for(i = 0; i < l_Value; i++)
					{
						pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_Matrix = NULL;
						ul_Key = LOA_ReadULong(&pc_Buffer);
						pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO = (OBJ_tdst_GameObject *) ul_Key;
						if((int) pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO != -1)
						{
							LOA_MakeFileRef
							(
								(BIG_KEY) ul_Key,
								(ULONG *) &pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO,
								OBJ_ul_GameObjectCallback,
								LOA_C_MustExists
							);
						}
						else
						{
							pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO = NULL;
						}

						pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].l_MatrixId = LOA_ReadLong(&pc_Buffer);
					}
				}
				else
				{
#if defined(_XBOX) || defined(_XENON)
					pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_AllocAlign
						(
							l_Value * sizeof(OBJ_tdst_Gizmo),
							16
						);
#else
					pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_Alloc(l_Value * sizeof(OBJ_tdst_Gizmo));
#endif
					LOA_ul_FileTypeSize[34] += l_Value * sizeof(OBJ_tdst_Gizmo);
#ifdef ACTIVE_EDITORS
					LOA_ul_FileTypeSize[34] -= OBJ_C_Gizmo_MaxLenName;
#endif
					for(i = 0; i < l_Value; i++)
					{
						LOA_ReadMatrix(&pc_Buffer, &pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[i].st_Matrix);
						LOA_ReadString_Ed
						(
							&pc_Buffer,
							(CHAR *) pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[i].sz_Name,
							OBJ_C_Gizmo_MaxLenName
						);
					}
				}
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Load extended struct
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(ul_Id & OBJ_C_IdentityFlag_ExtendedObject)
	{
        ULONG ulVersionNb;
		ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
		if((ul_Id & OBJ_C_IdentityFlag_Group) && ul_Key)
		{
			LOA_MakeFileRef
			(
				ul_Key,
				(ULONG *) &pst_GO->pst_Extended->pst_Group,
				GRP_ul_LoadGrpCallback,
				LOA_C_MustExists
			);
		}

		/* The next ULONG is a casted BOOL that indicates if the GameObject has modifiers. */
		ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
#if defined(XML_CONV_TOOL)
		gGaoHasModifierHack = ul_Key;
#endif
		if(ul_Key)
			b_HasModifiers = TRUE;
		else
			b_HasModifiers = FALSE;
#if !defined(XML_CONV_TOOL)
		/* Dummy 1 */
		LOA_ReadULong_Ed(&pc_Buffer, NULL);

		/* Dummy 2 */
		LOA_ReadULong_Ed(&pc_Buffer, NULL);

		/* Dummy 3 */
		LOA_ReadULong_Ed(&pc_Buffer, NULL);

		/* Dummy 4 */
		LOA_ReadULong_Ed(&pc_Buffer, NULL);

		/* Dummy 5 */
		LOA_ReadULong_Ed(&pc_Buffer, NULL);
#else
		for (i = 0; i < 5; i++)		
			LOA_ReadULong_Ed(&pc_Buffer, &gGaoDummy[i]);
#endif

		/* Version Nb (can be 0xFFFFFFFF) */
		ulVersionNb = LOA_ReadULong(&pc_Buffer);

		/* Secto */
		pst_GO->pst_Extended->auc_Sectos[0] = LOA_ReadUChar(&pc_Buffer);
		pst_GO->pst_Extended->auc_Sectos[1] = LOA_ReadUChar(&pc_Buffer);
		pst_GO->pst_Extended->auc_Sectos[2] = LOA_ReadUChar(&pc_Buffer);
		pst_GO->pst_Extended->auc_Sectos[3] = LOA_ReadUChar(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
		if(pst_GO->pst_Extended->auc_Sectos[0] == 255) pst_GO->pst_Extended->auc_Sectos[0] = 0;
		if(pst_GO->pst_Extended->auc_Sectos[1] == 255) pst_GO->pst_Extended->auc_Sectos[1] = 0;
		if(pst_GO->pst_Extended->auc_Sectos[2] == 255) pst_GO->pst_Extended->auc_Sectos[2] = 0;
		if(pst_GO->pst_Extended->auc_Sectos[3] == 255) pst_GO->pst_Extended->auc_Sectos[3] = 0;
#endif
		/* Capacities */
		//pst_GO->pst_Extended->uw_CapacitiesInit = LOA_ReadUShort(&pc_Buffer);

		/* We skip the uw_Capacities that has also been saved but is useless. */
//		LOA_ReadUShort_Ed(&pc_Buffer, &pst_GO->pst_Extended->uw_Capacities);
//#if !defined(XML_CONV_TOOL)
//		pst_GO->pst_Extended->uw_Capacities = pst_GO->pst_Extended->uw_CapacitiesInit;
//#endif
		//popoverif
        if (ulVersionNb == 0)
        {
#if !defined(XML_CONV_TOOL)
			pst_GO->pst_Extended->uw_CapacitiesInit = LOA_ReadULong(&pc_Buffer);
#endif
			pst_GO->pst_Extended->uw_Capacities = pst_GO->pst_Extended->uw_CapacitiesInit;
        }
        else
        {
#if !defined(XML_CONV_TOOL)
			pst_GO->pst_Extended->uw_CapacitiesInit = LOA_ReadUShort(&pc_Buffer);
#endif
			pst_GO->pst_Extended->uw_Capacities = pst_GO->pst_Extended->uw_CapacitiesInit;

	    	/* We skip the uw_Capacities that has also been saved but is useless. */
    		LOA_ReadUShort_Ed(&pc_Buffer, NULL);
        }


		pst_GO->pst_Extended->uc_AiPrio = LOA_ReadUChar(&pc_Buffer);
		pst_GO->pst_Extended->uc_Blank = LOA_ReadUChar(&pc_Buffer);
		pst_GO->pst_Extended->uw_ExtraFlags = LOA_ReadUShort(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
		pst_GO->pst_Extended->uw_ExtraFlags &= ~OBJ_C_ExtraFlag_LODai;
#endif
		/* AI */
		if(ul_Id & OBJ_C_IdentityFlag_AI)
		{
			ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
#if defined(XML_CONV_TOOL)
			*(ULONG* )&pst_GO->pst_Extended->pst_Ai = ul_Key;
#else
			if(ul_Key && (ul_Key != BIG_C_InvalidKey))
			{
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_GO->pst_Extended->pst_Ai,
					AI_ul_CallbackLoadInstance,
					LOA_C_MustExists
				);
			}
#endif
		}

		/* Events */
		if(ul_Id & OBJ_C_IdentityFlag_Events)
		{
			ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
			if(ul_Key && (ul_Key != BIG_C_InvalidKey))
			{
				pst_GO->pst_Extended->pst_Events = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
				LOA_ul_FileTypeSize[37] += sizeof(EVE_tdst_Data);
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_GO->pst_Extended->pst_Events->pst_ListTracks,
					EVE_ul_CallbackLoadListTracks,
					LOA_C_MustExists
				);
				pst_GO->pst_Extended->pst_Events->pst_ListParam = NULL;
			}
		}

		/* Sound */
		if(ul_Id & OBJ_C_IdentityFlag_Sound)
		{
			ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
			if(ul_Key && (ul_Key != BIG_C_InvalidKey))
			{
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_GO->pst_Extended->pst_Sound,
					SND_ul_CallbackUnknownBank,
					LOA_C_MustExists | LOA_C_HasUserCounter
				);
			}
		}

		/* Links */
		if(ul_Id & OBJ_C_IdentityFlag_Links)
		{
			ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
#if defined(XML_CONV_TOOL)
			*(ULONG *) &pst_GO->pst_Extended->pst_Links = ul_Key;
#else
			if(ul_Key && (ul_Key != BIG_C_InvalidKey))
			{
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_GO->pst_Extended->pst_Links,
					WAY_ul_AllLinkListsCallback,
					LOA_C_MustExists
				);
			}
#endif
		}

		/* Light */
		if(ul_Id & OBJ_C_IdentityFlag_Lights)
		{
			ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
#if defined(XML_CONV_TOOL)
			*(ULONG *) &pst_GO->pst_Extended->pst_Light = ul_Key;
#else
			if(ul_Key && (ul_Key != BIG_C_InvalidKey))
			{
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_GO->pst_Extended->pst_Light,
					GEO_ul_Load_ObjectCallback,
					LOA_C_MustExists
				);
			}
#endif
#ifdef JADEFUSION
#if defined(ACTIVE_EDITORS) && !defined(XML_CONV_TOOL)
			/* Does it have a scaling component ? */
			MATH_tdst_Vector st_Scale;
			MATH_GetScale(&st_Scale, pst_GO->pst_GlobalMatrix);
			
			if(st_Scale.x != 1.0f || st_Scale.y != 1.0f || st_Scale.z != 1.0f)
			{
				//reset scaling component
				MATH_ClearScale(pst_GO->pst_GlobalMatrix, TRUE);

				BIG_INDEX ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
				sprintf(az, "Light (key: [%08X] name: %s) Resetting scaling component!", ul_Key, BIG_NameFile(ul_Index));
				LINK_PrintStatusMsg(az);
			}
#endif	/* ACTIVE_EDITORS */
#endif
		}

		/* Design struct */
		if(ul_Id & OBJ_C_IdentityFlag_DesignStruct)
		{
			l_Value = LOA_ReadLong(&pc_Buffer); /* should normally be sizeof(OBJ_tdst_DesignStruct); */
#ifdef ACTIVE_EDITORS
			sprintf(az, "A design struct does have the correct size in that bigfile (%x)", ul_Key);
			ERR_X_Warning(l_Value == sizeof(OBJ_tdst_DesignStruct), az, NULL);
#endif /* #ifdef ACTIVE_EDITORS */

			pst_GO->pst_Extended->pst_Design = (OBJ_tdst_DesignStruct *) MEM_p_Alloc(sizeof(OBJ_tdst_DesignStruct));
			L_memset(pst_GO->pst_Extended->pst_Design, 0, sizeof(OBJ_tdst_DesignStruct));

			pst_GO->pst_Extended->pst_Design->flags = LOA_ReadInt(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->i1 = LOA_ReadInt(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->i2 = LOA_ReadInt(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->f1 = LOA_ReadFloat(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->f2 = LOA_ReadFloat(&pc_Buffer);
			LOA_ReadVector(&pc_Buffer, &pst_GO->pst_Extended->pst_Design->st_Vec1);
			LOA_ReadVector(&pc_Buffer, &pst_GO->pst_Extended->pst_Design->st_Vec2);
			pst_GO->pst_Extended->pst_Design->pst_Perso1 = (struct OBJ_tdst_GameObject_ *) LOA_ReadULong(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->pst_Perso2 = (struct OBJ_tdst_GameObject_ *) LOA_ReadULong(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->pst_Net1 = (struct WAY_tdst_Network_ *) LOA_ReadULong(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->pst_Net2 = (struct WAY_tdst_Network_ *) LOA_ReadULong(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->st_Text1.i_FileKey = LOA_ReadInt(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->st_Text1.i_Id = LOA_ReadInt(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->st_Text2.i_FileKey = LOA_ReadInt(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->st_Text2.i_Id = LOA_ReadInt(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->ui_DesignFlags = LOA_ReadUInt(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->ui_DesignFlagsInit = LOA_ReadUInt(&pc_Buffer);
			pst_GO->pst_Extended->pst_Design->i3 = LOA_ReadInt(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
			pst_GO->pst_Extended->pst_Design->ui_DesignFlags = pst_GO->pst_Extended->pst_Design->ui_DesignFlagsInit;
#endif
		}

		/* Modifiers */
		if(b_HasModifiers)
		{
			pc_Buffer += MDF_ul_Modifier_Load(&pst_GO->pst_Extended->pst_Modifiers, pc_Buffer, 1, pst_GO);
			LOA_ReadULong(&pc_Buffer);			/* Read the end modifier list mark dummy == 0xFFFFFFFF */
		}
#ifdef JADEFUSION
        /* Wind */
#if !defined(XML_CONV_TOOL)
        if (pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_AffectedByStaticWind)
        {
            CCurrentStaticWind * po_CurrentStaticWind = new CCurrentStaticWind();
            pst_GO->pst_Extended->po_CurrentWind = po_CurrentStaticWind;

            //pc_Buffer += po_CurrentStaticWind->Load(pc_Buffer);
        }
        else if (pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_AffectedByDynamicWind)
        {
            pst_GO->pst_Extended->po_CurrentWind = new CCurrentDynamicWind();
        }
        else
#endif
        {
            pst_GO->pst_Extended->po_CurrentWind = NULL;
        }
#endif

	}

	if(ul_Id & (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
#if defined(XML_CONV_TOOL)
		*(ULONG *) &((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance = ul_Key;
#else
		if(ul_Key && (ul_Key != BIG_C_InvalidKey))
		{
			LOA_MakeFileRef
			(
				ul_Key,
				(ULONG *) &((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance,
				COL_ul_CallBackLoadInstance,
				LOA_C_MustExists
			);
		}
#endif
	}

	if(ul_Id & OBJ_C_IdentityFlag_ColMap)
	{
		ul_Key = (BIG_KEY) LOA_ReadULong(&pc_Buffer);
#if defined(XML_CONV_TOOL)
		*(ULONG *) &((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap = ul_Key;
#else
		if(ul_Key && (ul_Key != BIG_C_InvalidKey))
		{
			LOA_MakeFileRef
			(
				ul_Key,
				(ULONG *) &((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap,
				COL_ul_CallBackLoadColMap,
				LOA_C_MustExists | LOA_C_OnlyOneRef
			);

			pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
		}
#endif
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Object name
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ul_Size = LOA_ReadULong(&pc_Buffer);

#ifdef _DEBUG
//#ifndef _FINAL //xenon
	if(ul_Size)
	{
		pst_GO->sz_Name = (char*)MEM_p_Alloc(ul_Size);
		LOA_ReadString(&pc_Buffer, pst_GO->sz_Name, ul_Size);
	}
	else
		pst_GO->sz_Name = NULL;
#else
	if(ul_Size) LOA_ReadString(&pc_Buffer, NULL, ul_Size);	/* skip the string when not in debug mode (it's still there
															 * in the binary data */
#endif /* #ifdef _DEBUG */
 

	{
		u32 DummyVersion,Readed;
		DummyVersion = LOA_ReadLong(&pc_Buffer);						/* read version */
		if (DummyVersion >= 1)
		{
			Readed = LOA_ReadLong(&pc_Buffer);						/* read version */
			pst_GO->ucCullingVisibility = Readed & 0xff;
		}
	}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Load editor data
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
	BAS_binsert(LOA_ul_GetCurrentKey(), 0, &WOR_ListAllKeys);
#ifdef XML_CONV_TOOL
	pst_GO->ul_EditorFlags = ul_EditorFlags;	// load all flags for XML convert
#else
	pst_GO->ul_EditorFlags = ul_EditorFlags & OBJ_C_EditorFlags_ToBeLoaded;

	/* ARGGHHHHH !!!! ENFIIIIIIINNNNNN */
	pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Hidden;
	//pst_GO->ul_AdditionalFlags &= ~OBJ_C_EdAddFlags_HiddenByWowFilter;
#endif // XML_CONV_TOOL

#if !defined(XML_CONV_TOOL)
	/* Register the object */
	ul_Key = LOA_ul_GetCurrentKey();
	ul_Key = BIG_ul_SearchKeyToFat(ul_Key);

	l_RegisterFlags = 0;
	if(BIG_b_IsFileInDirRec(BIG_ul_SearchDir(EDI_Csz_Path_Objects), ul_Key))
		l_RegisterFlags |= OBJ_GameObject_RegInBank;

	OBJ_GameObject_RegisterWithName(pst_GO, ul_Key, BIG_NameFile(ul_Key), l_RegisterFlags);

	/* Associated world */
	pst_GO->pst_World = WOR_gpst_WorldToLoadIn;
#endif
#endif /* #ifdef ACTIVE_EDITORS */

	/* Get object model */
	LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&pst_GO->ul_ObjectModel);

	/* Get object invisible object index */
	LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&pst_GO->ul_InvisibleObjectIndex);

	/* Get dummy data */
	LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&pst_GO->ul_ForceLODIndex);
	LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&pst_GO->ul_User3);

#ifdef ACTIVE_EDITORS
	/* prefab reference */
	pst_GO->ul_PrefabKey = BIG_C_InvalidKey;
	pst_GO->ul_PrefabObjKey = BIG_C_InvalidKey;

	LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&ul_Size);
	if(ul_Size == PREFABFILE_Mark)
	{
		LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&pst_GO->ul_PrefabKey);
	}
	else if(ul_Size == PREFABFILE_Mark1)
	{
#if defined(XML_CONV_TOOL)
		gGaoSaveBothPrefabKeyHack = true;
#endif
		LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&pst_GO->ul_PrefabKey);
		LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&pst_GO->ul_PrefabObjKey);
	}
#ifdef JADEFUSION
	// -NOTE- Added a bitfield to know if this GAO's properties can be
	// merged when integrating from PS2 Data. 
	if ( ul_Version > 1 )
	{
		LOA_ReadLong_Ed(&pc_Buffer, (LONG*)&pst_GO->ul_XeMergeFlags);
	}

#endif

#ifdef ACTIVE_EDITORS
{
extern BOOL EDI_gb_MorphLog;
	if( EDI_gb_MorphLog )
    {
        if( ul_Id & OBJ_C_IdentityFlag_ExtendedObject )
        {
            MDF_tdst_Modifier * pst_Modifier = pst_GO->pst_Extended->pst_Modifiers;
            while( pst_Modifier )
            {
                if( pst_Modifier->i->ul_Type == MDF_C_Modifier_Morphing )
                {
                    char buffer[256];
                    sprintf(buffer, "MorpLog - object: %s", pst_GO->sz_Name);
                    ERR_X_Warning(0, buffer, NULL);
                }
                pst_Modifier = pst_Modifier->pst_Next;
            }
        }
    }
}
#endif
#endif
	return(ULONG) pst_GO;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_BeforeColSetCreation(OBJ_tdst_GameObject *_pst_GO)
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
		COL_FreeInstance((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col);
	}
}

#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_AfterColSetCreation(OBJ_tdst_GameObject *_pst_GO, BIG_KEY _ul_ColSetKey, BIG_KEY _ul_InstanceKey)
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
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ReperIdentity(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *pst_Ref, ULONG flag)
{
	if( (_pst_GO->ul_IdentityFlags & flag) && !(pst_Ref->ul_IdentityFlags & flag))
	{
		OBJ_ChangeIdentityFlags(_pst_GO, _pst_GO->ul_IdentityFlags &~flag, _pst_GO->ul_IdentityFlags);
	}
	else if(!(_pst_GO->ul_IdentityFlags & flag) && (pst_Ref->ul_IdentityFlags & flag))
	{
		OBJ_ChangeIdentityFlags(_pst_GO, _pst_GO->ul_IdentityFlags | flag, _pst_GO->ul_IdentityFlags);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if 0
OBJ_tdst_Prefab *OBJ_LoadPrefab(ULONG ul_Index)
{
#ifdef ACTIVE_EDITORS
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				l_Length;
	ULONG				ul_Chunk;
	LONG				l_Size, l_CurRef;
	char				*pc_Buffer;
	OBJ_tdst_Prefab		*pst_PrefabRef;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	/* get content of file */
	pc_Buffer = BIG_pc_ReadFileTmp(ul_Index, (ULONG *) &l_Length);

	/* check file length */
	if(l_Length == 0)
	{
		return;
	}

	/* check file content */
	if(*(ULONG *) pc_Buffer != PREFABFILE_Mark)
	{
		return;
	}

	pc_Buffer += 4;
	l_Length -= 4;
	l_CurRef = -1;

	/* alloc / init data */
	pst_PrefabRef = (OBJ_tdst_Prefab *) L_malloc(sizeof(OBJ_tdst_Prefab));
	L_memset(pst_PrefabRef, 0, sizeof(OBJ_tdst_Prefab));

	pst_PrefabRef->ul_Index = ul_Index;
	pst_PrefabRef->c_CurrentRef = -1;

	/* get data */
	while(l_Length > 0)
	{
		ul_Chunk = *(ULONG *) pc_Buffer;
		pc_Buffer += 4;
		l_Length -= 4;
		l_Size = ul_Chunk & 0xFFFFFF;
		ul_Chunk >>= 24;

		switch(ul_Chunk)
		{
		case PREFABFILE_Comment:
			pst_PrefabRef->sz_Comment = (char *) L_malloc(l_Size);
			L_memcpy(pst_PrefabRef->sz_Comment, pc_Buffer, l_Size);
			break;
		case PREFABFILE_Refs:
			pst_PrefabRef->l_NbRef = *(ULONG *) pc_Buffer;
			pst_PrefabRef->dst_Ref = (OBJ_tdst_PrefabRef *) L_malloc(pst_PrefabRef->l_NbRef * sizeof(OBJ_tdst_PrefabRef));
			L_memcpy(pst_PrefabRef->dst_Ref, pc_Buffer + 4, l_Size - 4);

			/* check ref */
			for(i = 0; i < pst_PrefabRef->l_NbRef; i++)
			{
				pst_PrefabRef->dst_Ref[i].ul_Index = BIG_ul_SearchKeyToFat(pst_PrefabRef->dst_Ref[i].ul_Key);
				MATH_InitVectorToZero(&pst_PrefabRef->dst_Ref[i].st_BVMin);
				MATH_InitVectorToZero(&pst_PrefabRef->dst_Ref[i].st_BVMax);
			}
			break;
		case PREFABFILE_Type:
			pst_PrefabRef->l_Type = *(ULONG *) pc_Buffer;
			break;

		case PREFABFILE_Offset:
			MATH_CopyVector(&pst_PrefabRef->st_Offset, (MATH_tdst_Vector *) pc_Buffer);
			break;

		case PREFABFILE_RefNumber:
			pst_PrefabRef->l_NbRef = *(ULONG *) pc_Buffer;
			pst_PrefabRef->dst_Ref = (OBJ_tdst_PrefabRef *) L_malloc(pst_PrefabRef->l_NbRef * sizeof(OBJ_tdst_PrefabRef));
			L_memset(pst_PrefabRef->dst_Ref, 0, pst_PrefabRef->l_NbRef * sizeof(OBJ_tdst_PrefabRef));
			break;

		case PREFABFILE_Refi:
			l_CurRef = *(ULONG *) pc_Buffer;
			if((l_CurRef < 0) || (l_CurRef >= pst_PrefabRef->l_NbRef)) break;
			pst_PrefabRef->dst_Ref[l_CurRef].ul_Key = *(ULONG *) (pc_Buffer + 4);
			pst_PrefabRef->dst_Ref[l_CurRef].ul_Index = BIG_ul_SearchKeyToFat(pst_PrefabRef->dst_Ref[l_CurRef].ul_Key);
			i = *(ULONG *) (pc_Buffer + 8);
			pst_PrefabRef->dst_Ref[l_CurRef].uc_Type = (char) (i >> 24);
			pst_PrefabRef->dst_Ref[l_CurRef].c_RandomPercent = (char) ((i >> 16) & 0xFF);
			/* check type */
			pst_PrefabRef->dst_Ref[l_CurRef].uc_Type = OBJPREFAB_C_RefIsUnknow;
			if ( pst_PrefabRef->dst_Ref[l_CurRef].ul_Index != BIG_C_InvalidIndex )
			{
				if (BIG_b_IsFileExtension(pst_PrefabRef->dst_Ref[l_CurRef].ul_Index, EDI_Csz_ExtPrefab )) 
					pst_PrefabRef->dst_Ref[l_CurRef].uc_Type = OBJPREFAB_C_RefIsPrefab;
				else if (BIG_b_IsFileExtension(pst_PrefabRef->dst_Ref[l_CurRef].ul_Index, EDI_Csz_ExtGameObject)) 
					pst_PrefabRef->dst_Ref[l_CurRef].uc_Type = OBJPREFAB_C_RefIsGao;
			}
			break;

		case PREFABFILE_RefiMatrix:
			if((l_CurRef < 0) || (l_CurRef >= pst_PrefabRef->l_NbRef)) break;
			MATH_CopyMatrix(&pst_PrefabRef->dst_Ref[l_CurRef].st_Matrix, (MATH_tdst_Matrix *) pc_Buffer);
			break;

		case PREFABFILE_RefiBV:
			if((l_CurRef < 0) || (l_CurRef >= pst_PrefabRef->l_NbRef)) break;
			MATH_CopyVector(&pst_PrefabRef->dst_Ref[l_CurRef].st_BVMin, (MATH_tdst_Vector *) pc_Buffer);
			MATH_CopyVector
			(
				&pst_PrefabRef->dst_Ref[l_CurRef].st_BVMax,
				(MATH_tdst_Vector *) (pc_Buffer + sizeof(MATH_tdst_Vector))
			);

			break;
		}

		pc_Buffer += l_Size;
		l_Length -= l_Size;
	}

	return pst_PrefabRef;

#endif
}
#endif

extern int WOR_gb_DoNotLoadLinkedObjects;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_UpdateGaoGao(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GO, BIG_KEY _ul_Ref, ULONG flags)
{
#ifdef ACTIVE_EDITORS
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Ref;
	OBJ_tdst_Prefab		*pst_PrefabRef = NULL;
	char				az[1024];
	MDF_tdst_Modifier	*pst_NextMdf, *pst_Modifier;
	//AI_tdst_VarDes		*pst_VarDesSave;
	//char				asz_Path[BIG_C_MaxLenPath];
	//ULONG				ul_GOKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*	if (!flags)
		return;*/

	/* Load reference object */
	WOR_gpst_WorldToLoadIn = _pst_World;
	WOR_gb_DoNotLoadLinkedObjects = TRUE;
	
	LOA_MakeFileRef(_ul_Ref, (ULONG *) &pst_Ref, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
	LOA_Resolve();
		
	if(!pst_Ref)
	{
		sprintf(az, "Object %x can't be loaded", _ul_Ref);
		ERR_X_Warning(0, az, NULL);
		return;
	}

	WOR_World_JustAfterLoadObject(_pst_World, pst_Ref, FALSE, TRUE);
	WOR_gb_DoNotLoadLinkedObjects = FALSE;


	/*$2- AI model -----------------------------------------------------------------------------------------------------*/
	if(pst_Ref->pst_Extended && pst_Ref->pst_Extended->pst_Ai && pst_Ref->pst_Extended->pst_Ai->pst_Model)
	{
		/* Infamie made in CB */
		pst_Ref->pst_Extended->pst_Ai->pst_Model->uw_NbInstances ++;	

		//if	(flags & UPDATE_AIMODEL)
		//{
		//	/*OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_AI);
		//	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AI ) &&
		//		_pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Ai && _pst_GO->pst_Extended->pst_Ai->pst_Model && 
		//		pst_Ref->pst_Extended && pst_Ref->pst_Extended->pst_Ai && pst_Ref->pst_Extended->pst_Ai->pst_Model)
		//	{
		//		pst_VarDesSave = _pst_GO->pst_Extended->pst_Ai->pst_Model->pst_VarDes;
		//		L_memcpy( _pst_GO->pst_Extended->pst_Ai->pst_Model, pst_Ref->pst_Extended->pst_Ai->pst_Model, sizeof( AI_tdst_Model ) );
		//		_pst_GO->pst_Extended->pst_Ai->pst_Model->pst_VarDes = pst_VarDesSave;
		//	}*/
		//}
	}

	/*$2- Visuel -----------------------------------------------------------------------------------------------------*/

	/* Materiaux */
	if(flags & UPDATE_MATERIAL)
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_Visu);
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
		{
			if(_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Visu && _pst_GO->pst_Base->pst_Visu->pst_Material)
            {
                if (_pst_GO->pst_Base->pst_Visu->pst_Material != pst_Ref->pst_Base->pst_Visu->pst_Material)
                {
                    _pst_GO->pst_Base->pst_Visu->pst_Material->i->pfn_AddRef(_pst_GO->pst_Base->pst_Visu->pst_Material, -1);
                    pst_Ref->pst_Base->pst_Visu->pst_Material->i->pfn_AddRef(pst_Ref->pst_Base->pst_Visu->pst_Material, 1);
                    _pst_GO->pst_Base->pst_Visu->pst_Material = pst_Ref->pst_Base->pst_Visu->pst_Material;
                }
            }
            else 
                ERR_X_Warning(0,"No material in instance, cannot update material",_pst_GO->sz_Name);
		}
	}

	/* Visuel */
	if(flags & UPDATE_GEOMETRY)
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_Visu);
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
		{
			if(_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Visu && _pst_GO->pst_Base->pst_Visu->pst_Object)
            {
                if (_pst_GO->pst_Base->pst_Visu->pst_Object != pst_Ref->pst_Base->pst_Visu->pst_Object)
                {
                    _pst_GO->pst_Base->pst_Visu->pst_Object->i->pfn_AddRef(_pst_GO->pst_Base->pst_Visu->pst_Object, -1);
                    pst_Ref->pst_Base->pst_Visu->pst_Object->i->pfn_AddRef(pst_Ref->pst_Base->pst_Visu->pst_Object, 1);
                    _pst_GO->pst_Base->pst_Visu->pst_Object = pst_Ref->pst_Base->pst_Visu->pst_Object;
                }
            }
            else
                ERR_X_Warning(0,"No geometry in instance, cannot update geometry",_pst_GO->sz_Name);
		}
	}

	/*$2- modifier -----------------------------------------------------------------------------------------------------*/

	if(flags & UPDATE_MODIFIERS)
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_ExtendedObject);
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject)
		{
			/* Modifiers */
			pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
			while(pst_Modifier)
			{
				pst_NextMdf = pst_Modifier->pst_Next;
				MDF_Modifier_Destroy(pst_Modifier);
				pst_Modifier = pst_NextMdf;
			}
			
			_pst_GO->pst_Extended->pst_Modifiers = pst_Ref->pst_Extended->pst_Modifiers;
			pst_Ref->pst_Extended->pst_Modifiers = NULL;
			
			/* special modifier update */
			pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
			while(pst_Modifier)
			{
				pst_Modifier->pst_GO = _pst_GO;
				pst_Modifier = pst_Modifier->pst_Next;
			}
		}
	}

	/*$2- sound  -----------------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_SOUND)
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_Sound);
		if(pst_Ref->ul_IdentityFlags & OBJ_C_IdentityFlag_Sound)
		{
			if(_pst_GO->pst_Extended->pst_Sound) SND_FreeMainStruct(_pst_GO);
			_pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_Sound;
			_pst_GO->pst_Extended->pst_Sound = pst_Ref->pst_Extended->pst_Sound;
			_pst_GO->pst_Extended->pst_Sound->st_Bank.ul_UserCount++;
		}
	}

	/*$2- colmap -----------------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_COLMAP)
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_ColMap);
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
			COL_DuplicateColMap(_pst_GO, pst_Ref, TRUE);
	}
	
	/*$2- colset -----------------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_COLSET)
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_ZDM);
		if (pst_Ref->ul_IdentityFlags & OBJ_C_IdentityFlag_ZDM)
		{
			ULONG	ul_ColSetKey;
			ULONG	ul_InstanceKey;
			void	*p_ColSet;
			
			p_ColSet = ((COL_tdst_Base *) pst_Ref->pst_Extended->pst_Col)->pst_Instance->pst_ColSet;
			ul_ColSetKey = LOA_ul_SearchKeyWithAddress( (ULONG) p_ColSet );
			if (ul_ColSetKey != BIG_C_InvalidKey)
			{
				/* Init before creation. */
				OBJ_BeforeColSetCreation(_pst_GO);
				/* Create a dummy instance */
				ul_InstanceKey = COL_ul_CreateInstanceFile( _pst_World, &ul_ColSetKey, _pst_GO->sz_Name, NULL);
				/* Load the created file. */
				OBJ_AfterColSetCreation(_pst_GO, ul_ColSetKey, ul_InstanceKey);
			}
		}
	}
	
	/*$2- design struct -----------------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_DESIGNSTRUCT)
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_DesignStruct);
		if (
			(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_DesignStruct ) &&
			_pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Design && 
			pst_Ref->pst_Extended && pst_Ref->pst_Extended->pst_Design 
		)
		{
			L_memcpy( _pst_GO->pst_Extended->pst_Design, pst_Ref->pst_Extended->pst_Design, sizeof( OBJ_tdst_DesignStruct ) );
		}
	}

	/*$2- control flags -------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_CONTROLFLAGS )
	{
		_pst_GO->ul_StatusAndControlFlags &= ~UPDATABLE_CONTROLFLAGS;
		_pst_GO->ul_StatusAndControlFlags |= (pst_Ref->ul_StatusAndControlFlags & UPDATABLE_CONTROLFLAGS);
	}
	
	/*$2- custom bits -------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_CUSTOMBITS )
	{
		_pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_CustomBitAll;
		_pst_GO->ul_StatusAndControlFlags |= (pst_Ref->ul_StatusAndControlFlags & OBJ_C_CustomBitAll);
	}	
	
	/*$2- Skeleton and shape --------------------------------------------------------------------------------------*/
	if(flags & UPDATE_SKELETONANDSHAPE )
		ANI_CloneSet( _pst_GO, pst_Ref, 0 );//popowarning xenon -1

#if 0
	/*$2- additionnal matrix -------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_ADDITIONALMATRIX )
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_AdditionalMatrix);
		if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix) &&
			_pst_GO->pst_Base && _pst_GO->pst_Base->pst_AddMatrix &&
			pst_Ref->pst_Base && pst_Ref->pst_Base->pst_AddMatrix)
        {
			if (_pst_GO->pst_Base->pst_AddMatrix != pst_Ref->pst_Base->pst_AddMatrix)
            {
                L_memcpy(_pst_GO->pst_Base->pst_AddMatrix, pst_Ref->pst_Base->pst_AddMatrix, sizeof(OBJ_tdst_AdditionalMatrix));
            }
        }
	}

	/*$2- hierarchy -------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_HIERARCHY)
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_Hierarchy);
		if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) &&
			_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Hierarchy &&
			pst_Ref->pst_Base && pst_Ref->pst_Base->pst_Hierarchy)
        {

			// Load prefab
			pst_PrefabRef = OBJ_LoadPrefab(BIG_ul_SearchKeyToPos(_pst_GO->ul_PrefabKey));

			/* Scan all group */
			for(i = 0; i < pst_PrefabRef->l_NbRef ; ++i)
			{
				if(pst_PrefabRef->dst_Ref[i].uc_Type == OBJPREFAB_C_RefIsGao &&
					pst_Ref->pst_Base->pst_Hierarchy->pst_Father->ul_MyKey == pst_PrefabRef->dst_Ref[i].ul_Key)
				{
					_pst_GO->pst_Base->pst_Hierarchy->pst_Father->ul_PrefabObjKey = pst_PrefabRef->dst_Ref[i].ul_Key;
					_pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit->ul_PrefabObjKey = pst_PrefabRef->dst_Ref[i].ul_Key;
					MATH_CopyMatrix(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, &pst_Ref->pst_Base->pst_Hierarchy->st_LocalMatrix);
#ifdef USE_DOUBLE_RENDERING
					MATH_CopyMatrix(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrixA, &pst_Ref->pst_Base->pst_Hierarchy->st_LocalMatrixA);
					MATH_CopyMatrix(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrixB, &pst_Ref->pst_Base->pst_Hierarchy->st_LocalMatrixB);
#endif
					break;
				}
			}

			// Unload prefab
			if(pst_PrefabRef->l_NbRef != 0) L_free(pst_PrefabRef->dst_Ref);
			if(pst_PrefabRef->sz_Comment) L_free(pst_PrefabRef->sz_Comment);
			L_memset(pst_PrefabRef, 0, sizeof(OBJ_tdst_Prefab));
		}
	}

	/*$2- events -------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_EVENTS)
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_Events);
		if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Events) &&
			_pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Events &&
			pst_Ref->pst_Extended && pst_Ref->pst_Extended->pst_Events)
        {
			L_memcpy( _pst_GO->pst_Extended->pst_Events, pst_Ref->pst_Extended->pst_Events, sizeof( EVE_tdst_Data ) );
        }
	}

#ifdef ODE_INSIDE
	/*$2- ODE -------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_ODE)
	{
		OBJ_ReperIdentity(_pst_GO, pst_Ref, OBJ_C_IdentityFlag_ODE);
		if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) &&
			_pst_GO->pst_Base && _pst_GO->pst_Base->pst_ODE &&
			pst_Ref->pst_Base && pst_Ref->pst_Base->pst_ODE)
        {
			L_memcpy( _pst_GO->pst_Base->pst_ODE, pst_Ref->pst_Base->pst_ODE, sizeof( DYN_tdst_ODE ) );
        }
	}
#endif

	/*$2- type flags -------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_TYPEFLAGS)
	{
		_pst_GO->uc_DesignFlags &= ~_pst_GO->uc_DesignFlags;
		_pst_GO->uc_DesignFlags |= pst_Ref->uc_DesignFlags;
	}

	/*$2- fix flags -------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_FIXFLAGS)
	{
		_pst_GO->c_FixFlags &= ~_pst_GO->c_FixFlags;
		_pst_GO->c_FixFlags |= pst_Ref->c_FixFlags;
	}

	/*$2- extra flags -------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_EXTRALAGS)
	{
		if ((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) &&
			_pst_GO->pst_Extended && pst_Ref->pst_Extended)
		{
			_pst_GO->pst_Extended->uw_ExtraFlags &= ~_pst_GO->pst_Extended->uw_ExtraFlags;
			_pst_GO->pst_Extended->uw_ExtraFlags |= pst_Ref->pst_Extended->uw_ExtraFlags;
		}
	}

	/*$2- BV -------------------------------------------------------------------------------------------*/
	if(flags & UPDATE_BV)
	{
		if ((flags & UPDATE_FORCE_BV || _pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EditableBV) &&
			_pst_GO->pst_BV && pst_Ref->pst_BV)
		{
			L_memcpy( _pst_GO->pst_BV, pst_Ref->pst_BV, sizeof( OBJ_tdst_SingleBV ) );
			((OBJ_tdst_SingleBV *)_pst_GO->pst_BV)->pst_GO = _pst_GO;
			OBJ_ComputeBV(_pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);
		}
	}
	#endif

	if(pst_Ref->pst_Extended && pst_Ref->pst_Extended->pst_Ai && pst_Ref->pst_Extended->pst_Ai->pst_Model)
	{
		//pst_Ref->pst_Extended->pst_Ai->pst_Model->uw_NbInstances --;	
	}
	WOR_World_DetachObject(_pst_World, pst_Ref);
	OBJ_GameObject_Remove(pst_Ref, 1);
#endif




	/*ul_GOKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GO);
	if(ul_GOKey != BIG_C_InvalidKey)
	{
		ul_GOKey = BIG_ul_SearchKeyToFat(ul_GOKey);
		if(ul_GOKey == BIG_C_InvalidIndex)
		{
			ERR_X_Warning(0, "Game object is in world but file has been destroyed !!! Zapping...", _pst_GO->sz_Name);
		}
		BIG_ComputeFullName(BIG_ParentFile(ul_GOKey), asz_Path);
	}*/

	//WOR_GetPath(mst_WinHandles.pst_World, asz_Path);
	//OBJ_ul_GameObject_Save(_pst_World, _pst_GO, NULL);

}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

