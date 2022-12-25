/*$T WORload.c GC! 1.100 08/31/01 14:50:14 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "TABles/TABles.h"
#include "GRObject/GROsave.h"
#include "MATerial/MATstruct.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJgizmo.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/WAYs/WAYload.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"
#include "EDIpaths.h"
#include "ENGine/Sources/GRP/GRPload.h"
#include "ENGine/Sources/COLlision/COLload.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/ACTions/ACTload.h"
#include "ENGine/Sources/ACTions/ACTinit.h"
#include "ENGine/Sources/ACTions/ACTcompute.h"
#include "ENGine/Sources/GRId/GRI_load.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "GraphicDK/Sources/SELection/SELection.h"
#include "GraphicDK/Sources/GDInterface/GDInterface.h"
#include "SouND/Sources/SND.h"
#include "BASe/BASarray.h"

#ifdef JADEFUSION
#include "Light/LIGHTrejection.h"
#include "Engine/Sources/Wind/CurrentWind.h"
#include "GraphicDK/Sources/GEOmetric/GEOXenonPack.h"
#ifdef ACTIVE_EDITORS
#include "Editors/Sources/OUTput/OUTframe.h"
#include "XenonMeshProcessing/Sources/XMP_XenonMesh.h"
#endif
#endif

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_XMEN.h"

#ifdef _DX8
#include "GX8\Gx8BuildUVs.h"
#include "GX8\Gx8VertexBuffer.h"
#include <assert.h>
#endif
	

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDItors/Sources/OUTput/OUTmsg.h"

#endif /* ACTIVE_EDITORS */

#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#define _ReadLong(_a)	ReadLong((char *) _a)
#define _ReadFloat(_a)	ReadFloat((char *) _a)
#else
#define _ReadLong(_a)	*(LONG *) _a
#define _ReadFloat(_a)	*(float *) _a
#endif

#ifdef JADEFUSION
#ifndef _XENON
extern int Gx8_ComputeVertexBufferFriendlyDataForGeometricObject(GEO_tdst_Object *p_stObject, ULONG *dul_VertexColors);
extern void Gx8_FreeAddInfo(GEO_tdst_Object *p_stObject);
extern void Gx8_SetCurrentGameObject(void *pNextGO);
extern void Gx8_VertexBuffer_Clean( void );
#else
#include "SouND/Sources/Xenon/xeSND_AudioConsole.h"
#include "SouND/Sources/Xenon/xeSND_Engine.h"
#endif
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#if defined(XML_CONV_TOOL)
extern BIG_KEY g_ul_GroupKey;
extern BIG_KEY g_ul_NetworksGroupKey;
extern BIG_KEY g_ul_LightRejectionKey;
extern ULONG lNamePadding;
extern int g_nWOWVersion;
#endif // !defined(XML_CONV_TOOL)

#ifdef JADEFUSION
#if defined(ACTIVE_EDITORS)
extern BOOL EDI_gb_ComputeMap;
#endif
#endif
/*$4
 ***********************************************************************************************************************
    Constanst
 ***********************************************************************************************************************
 */

WOR_tdst_World	*WOR_gpst_WorldToLoadIn;
BOOL			WOR_gb_DoNotLoadLinkedObjects = FALSE;
LONG			WOR_gl_LoadJustWorldStruct;
BOOL			WOR_gb_CanResolve = TRUE;
BOOL			WOR_gb_HasAddedMaterial = FALSE;
BOOL			WOR_gb_FinalWorld = TRUE;
BOOL			WOR_gb_RealAllocWorld = TRUE;
BIG_KEY			*WOR_gaul_WorldText = NULL;
ULONG			WOR_gul_WorldTextSize = 0;
#ifdef ACTIVE_EDITORS
int				WOR_gi_CurrentConsole = 10;
#elif defined(PSX2_TARGET)
int				WOR_gi_CurrentConsole = 1;
#elif defined(_GAMECUBE)
#if defined(_GC2REVO) || defined (_RVL)
int				WOR_gi_CurrentConsole = 4;
#else
int				WOR_gi_CurrentConsole = 2;
#endif
#elif defined(_XBOX)
int				WOR_gi_CurrentConsole = 3;
BOOL			WOR_gi_FixLoaded = FALSE;
#elif defined(_XENON)
int				WOR_gi_CurrentConsole = 5;
BOOL			WOR_gi_FixLoaded = FALSE;
#else // PC
int				WOR_gi_CurrentConsole = 0;
#endif



extern int		BIG_gi_CanConcat;

/*$4
 ***********************************************************************************************************************
    Private functions prototypes
 ***********************************************************************************************************************
 */

ULONG			WOR_ul_World_LoadCallback(ULONG);
ULONG			WOR_ul_GameObjectGroupCallback(ULONG _ul_PosFile);

/*$4
 ***********************************************************************************************************************
    Editor functions/macros:: Check
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    checking light type
 =======================================================================================================================
 */
#define M_4Edit_CheckLight(_pst_GO_, _pst_Gro_) \
	{ \
		char	sz_Warning[512]; \
		if(_pst_Gro_ == NULL) \
		{ \
			sprintf(sz_Warning, "%s has a null light", _pst_GO_->sz_Name); \
			ERR_X_Warning(0, sz_Warning, NULL); \
		} \
		else if(_pst_Gro_->i->ul_Type != GRO_Light) \
		{ \
			sprintf(sz_Warning, "%s has a light %s which is not a light", _pst_GO_->sz_Name, _pst_Gro_->sz_Name); \
			ERR_X_Warning(0, sz_Warning, NULL); \
		} \
	}

/*
 =======================================================================================================================
    checking Material type
 =======================================================================================================================
 */
#define M_4Edit_CheckMaterial(_pst_GO_, _pst_Gro_) \
	{ \
		char	sz_Warning[512]; \
		if(!GRO_b_IsAMaterial(_pst_Gro_)) \
		{ \
			sprintf(sz_Warning, "%s has a material %s which is not a material", _pst_GO_->sz_Name, _pst_Gro_->sz_Name); \
			ERR_X_Warning(0, sz_Warning, NULL); \
		} \
	}

/*
 =======================================================================================================================
    warning if a multi mat is included in another multi mat
 =======================================================================================================================
 */
#define M_4Edit_MultiInMultiWarning( _pst_Multi1_, _pst_Multi2_ )\
    {\
        char	sz_Warning[512]; \
		sprintf(sz_Warning, "(%s) has a sub material (%s) which is a multi mat, set ref to null", _pst_Multi1_->st_Id.sz_Name, _pst_Multi2_->st_Id.sz_Name); \
		ERR_X_Warning(0, sz_Warning, NULL); \
    }
    
#define M_4Edit_BadGroTypeInObjectWarning( _pst_GO_, _pst_Gro_ )\
	{\
		char	sz_Warning[512]; \
		sprintf(sz_Warning, "(%s) has a bad graphic object type (%s), set ref to null", _pst_GO_->sz_Name, _pst_Gro_->sz_Name); \
		ERR_X_Warning(0, sz_Warning, NULL); \
    }

/*
 =======================================================================================================================
    warning if a graphic object has transparent faces and opaque faces
 =======================================================================================================================
 */
void WOR_4Edit_CheckTO( OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *_pst_Geo, MAT_tdst_Multi *_pst_Grm )
{
    int     i, mat, trans, newtrans;
    char	sz_Warning[512];

    if (_pst_Geo->l_NbElements <= 1) return;

    trans = -1;
    for (i = 0; i < _pst_Geo->l_NbElements; i++)
    {
        mat = _pst_Geo->dst_Element[i].l_MaterialId;
        if (mat >= _pst_Grm->l_NumberOfSubMaterials)
            mat = _pst_Grm->l_NumberOfSubMaterials - 1;
        newtrans = MAT_IsMaterialTransparent( _pst_Grm->dpst_SubMaterial[ mat ], 0, _pst_GO->pst_Base->pst_Visu->ul_DrawMask, _pst_Geo );
       
        if (trans == -1)
            trans = newtrans;
        else if ( trans ^ newtrans )
        {
            sprintf(sz_Warning, "[Warning] (%s) use transparent and opaque sub materials", _pst_GO->sz_Name );
            LINK_PrintStatusMsg( sz_Warning );
            return;
        }
    }
}

void WOR_4Edit_CheckTransOpaque( OBJ_tdst_GameObject *_pst_GO )
{
    GRO_tdst_Struct     *pst_Gro;
    GRO_tdst_Struct     *pst_Grm;
    GEO_tdst_StaticLOD	*pst_LOD;

    pst_Grm = _pst_GO->pst_Base->pst_Visu->pst_Material;
    if (!pst_Grm) return;
    if (pst_Grm->i->ul_Type != GRO_MaterialMulti) return;

    pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
    if (!pst_Gro) return;

    if ( pst_Gro->i->ul_Type == GRO_Geometric )
        WOR_4Edit_CheckTO( _pst_GO, (GEO_tdst_Object *) pst_Gro, (MAT_tdst_Multi *) pst_Grm );
    else if (pst_Gro->i->ul_Type == GRO_GeoStaticLOD)
    {
        int i;
        pst_LOD = (GEO_tdst_StaticLOD *) pst_Gro;
        for (i = 0; i < pst_LOD->uc_NbLOD; i++)
        {
            pst_Gro = pst_LOD->dpst_Id[ i ];
            if ( pst_Gro && (pst_Gro->i->ul_Type == GRO_Geometric) )
                WOR_4Edit_CheckTO( _pst_GO, (GEO_tdst_Object *) pst_Gro, (MAT_tdst_Multi *) pst_Grm );
        }
    }
}

//#define M_4Edit_CheckTransOpaque    WOR_4Edit_CheckTransOpaque
#define M_4Edit_CheckTransOpaque( a )

#else /* ACTIVE_EDITORS */

#define M_4Edit_CheckLight(a, b)
#define M_4Edit_CheckMaterial(a, b)
#define M_4Edit_MultiInMultiWarning( a, b )
#define M_4Edit_BadGroTypeInObjectWarning( a, b)
#define M_4Edit_CheckTransOpaque( a )

#endif /* ACTIVE_EDITORS */

/*$4
 ***********************************************************************************************************************
    Public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_World_ResolveAIRef(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_DesignStruct	*pst_Design;
	ULONG					ul_Pos;
#ifdef ACTIVE_EDITORS
	char					az[1024];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(_pst_World);
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(!pst_GO) continue;

		PROPS2_StartRaster(&PROPS2_gst_WOR_World_ResolveAIRef);
		OBJ_ResolveAIRef(pst_GO);
		PROPS2_StopRaster(&PROPS2_gst_WOR_World_ResolveAIRef);

#ifdef ACTIVE_EDITORS
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
		{
			ANI_ResolveAllTracksAndOptimize(pst_GO);
		}

#endif
		/* Design struct */
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_DesignStruct))
		{
			pst_Design = pst_GO->pst_Extended->pst_Design;
			if(pst_Design->pst_Net1)
			{
				ul_Pos = BIG_ul_SearchKeyToPos((ULONG) pst_Design->pst_Net1);
				if(ul_Pos != -1)
				{
					pst_Design->pst_Net1 = (WAY_tdst_Network *) LOA_ul_SearchAddress(ul_Pos);
#ifdef ACTIVE_EDITORS
					if((int) pst_Design->pst_Net1 == -1)
					{
						sprintf(az, "Invalid network reference in design struct for %s object", pst_GO->sz_Name);
						ERR_X_Warning(0, az, NULL);
					}

#endif
				}
			}

			if(pst_Design->pst_Net2)
			{
				ul_Pos = BIG_ul_SearchKeyToPos((ULONG) pst_Design->pst_Net2);
				if(ul_Pos != -1)
				{
					pst_Design->pst_Net2 = (WAY_tdst_Network *) LOA_ul_SearchAddress(ul_Pos);
#ifdef ACTIVE_EDITORS
					if((int) pst_Design->pst_Net2 == -1)
					{
						sprintf(az, "Invalid network reference in design struct for %s object", pst_GO->sz_Name);
						ERR_X_Warning(0, az, NULL);
					}

#endif
				}
			}

			if(pst_Design->pst_Perso1)
			{
				ul_Pos = BIG_ul_SearchKeyToPos((ULONG) pst_Design->pst_Perso1);
				if(ul_Pos != -1)
				{
					pst_Design->pst_Perso1 = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(ul_Pos);
#ifdef ACTIVE_EDITORS
					if((int) pst_Design->pst_Perso1 == -1)
					{
						sprintf(az, "Invalid object reference in design struct for %s object", pst_GO->sz_Name);
						ERR_X_Warning(0, az, NULL);
					}

#endif
				}
			}

			if(pst_Design->pst_Perso2)
			{
				ul_Pos = BIG_ul_SearchKeyToPos((ULONG) pst_Design->pst_Perso2);
				if(ul_Pos != -1)
				{
					pst_Design->pst_Perso2 = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(ul_Pos);
#ifdef ACTIVE_EDITORS
					if((int) pst_Design->pst_Perso2 == -1)
					{
						sprintf(az, "Invalid object reference in design struct for %s object", pst_GO->sz_Name);
						ERR_X_Warning(0, az, NULL);
					}

#endif
				}
			}
		}
	}

	/* Second pass, IA to C */
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(!pst_GO) continue;
		OBJ_ResolveAIRefSecondPass(pst_GO);
	}
}

extern BOOL		EVE_gb_PlayOnlyAnimTracks;
extern BOOL		WOR_gb_DoNotLoadLinkedObjects;
/*
 =======================================================================================================================
    Aim:    Add the GameObject in the World tables it must be in.

    Note:   We also check the validity of networks.
 =======================================================================================================================
 */
void WOR_World_AttachGameObject(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Visu			*pst_Visu;
	GRO_tdst_Struct			*pst_Gro;
	TAB_tdst_Ptable			*pst_PTable;
	ANI_st_GameObjectAnim	*pst_GameObjectAnim;
	OBJ_tdst_Group			*pst_Skeleton, *pst_SkelModel;
	MAT_tdst_Multi			*pst_MultiMat;
	MAT_tdst_Material		**ppst_SubMat, **ppst_Last;
	ANI_tdst_Shape			*pst_Shape;
	OBJ_tdst_GameObject		*pst_OtherGO;
	GEO_tdst_Object			*pst_Geo;
	ULONG					ul_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    World:: add object in world table
	 -------------------------------------------------------------------------------------------------------------------
	 */

	WOR_World_AddLoadedObject(_pst_World, _pst_GO, 0);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Group:: Update internal Group counter.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Group))
	{
		if(!(_pst_GO->pst_Extended) || !(_pst_GO->pst_Extended->pst_Group))
		{
			OBJ_ChangeIdentityFlags
			(
				_pst_GO,
				OBJ_ul_FlagsIdentityGet(_pst_GO) & (~OBJ_C_IdentityFlag_Group),
				OBJ_C_UnvalidFlags
			);
		}
		else
			_pst_GO->pst_Extended->pst_Group->ul_NbObjectsUsingMe++;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Light:: add light objects in st_GraphicObjectsTable
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Lights))
	{
		pst_Gro = (GRO_tdst_Struct *) _pst_GO->pst_Extended->pst_Light;

		M_4Edit_CheckLight(_pst_GO, pst_Gro);

		if(pst_Gro != NULL)
		{
			pst_Gro->i->pfn_AddRef(pst_Gro, 1);
			pst_PTable = &_pst_World->st_GraphicObjectsTable;
			if(TAB_ul_Ptable_GetElemIndexWithPointer(pst_PTable, pst_Gro) == TAB_Cul_BadIndex)
			{
				TAB_Ptable_AddElemAndResize(pst_PTable, pst_Gro);
				pst_Gro->i->pfn_AddRef(pst_Gro, 1);
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Hierarchy:: inform father that he have a child
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
	{
		pst_OtherGO = _pst_GO->pst_Base->pst_Hierarchy->pst_Father;
		if(pst_OtherGO) pst_OtherGO->ul_StatusAndControlFlags |= OBJ_C_StatusFlag_HasChild;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Visu:: add graphic objects in st_GraphicObjectsTable
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
	{
		pst_Visu = _pst_GO->pst_Base->pst_Visu;

		if((pst_Gro = pst_Visu->pst_Object) != NULL)
		{

			ul_Type = pst_Gro->i->ul_Type;
			if ( (ul_Type == GRO_Light) || (ul_Type == GRO_MaterialSingle) || (ul_Type == GRO_MaterialMulti) || (ul_Type == GRO_MaterialMultiTexture) )
			{
				M_4Edit_BadGroTypeInObjectWarning( _pst_GO, pst_Gro );
				pst_Visu->pst_Object = NULL;
				OBJ_VertexColor_Free( _pst_GO );
			}
			else
			{
				pst_Gro->i->pfn_AddRef(pst_Gro, 1);
				pst_PTable = &_pst_World->st_GraphicObjectsTable;
				if(TAB_ul_Ptable_GetElemIndexWithPointer(pst_PTable, pst_Gro) == TAB_Cul_BadIndex)
				{
					TAB_Ptable_AddElemAndResize(pst_PTable, pst_Gro);
					pst_Gro->i->pfn_AddRef(pst_Gro, 1);
				}

				/* Test RLI number */
				if((pst_Visu->dul_VertexColors) && (ul_Type == GRO_Geometric))
				{
					pst_Geo = (GEO_tdst_Object *) pst_Gro;
					if(*pst_Visu->dul_VertexColors != (ULONG) pst_Geo->l_NbPoints)
						OBJ_VertexColor_Realloc( _pst_GO, pst_Geo->l_NbPoints );
				}
			}
		}
		else
		{
			OBJ_VertexColor_Free( _pst_GO );
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Material:: add materials in st_GraphicMaterialsTable ²
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if((pst_Gro = pst_Visu->pst_Material) != NULL)
		{
			M_4Edit_CheckMaterial(_pst_GO, pst_Gro);

			pst_PTable = &_pst_World->st_GraphicMaterialsTable;

			/*
			 * multi Material : l'incrementation des réferences aux sous matériaux ne doit
			 * être faite qu'après le chargement réel du multi material £
			 * soit seulement si le compteur de ref est à zéro
			 */
			if((pst_Gro->l_Ref == 0) && (((MAT_tdst_Material *) pst_Gro)->st_Id.i->ul_Type == GRO_MaterialMulti))
			{
				pst_MultiMat = (MAT_tdst_Multi *) pst_Gro;
				if(pst_MultiMat->l_NumberOfSubMaterials)
				{
					ppst_SubMat = pst_MultiMat->dpst_SubMaterial;
					ppst_Last = ppst_SubMat + pst_MultiMat->l_NumberOfSubMaterials;
					for(; ppst_SubMat < ppst_Last; ppst_SubMat++)
					{
						if(!(*ppst_SubMat)) continue;

                       	if ( ((*ppst_SubMat)->st_Id.i->ul_Type != GRO_MaterialSingle) && ((*ppst_SubMat)->st_Id.i->ul_Type != GRO_MaterialMultiTexture) )
                        {
                            M_4Edit_MultiInMultiWarning( pst_MultiMat, (*ppst_SubMat) );
                            *ppst_SubMat = 0;
                            continue;
                        }
				
                        (*ppst_SubMat)->st_Id.i->pfn_AddRef((*ppst_SubMat), 1);
						if(TAB_ul_Ptable_GetElemIndexWithPointer(pst_PTable, *ppst_SubMat) == TAB_Cul_BadIndex)
						{
							TAB_Ptable_AddElemAndResize(pst_PTable, *ppst_SubMat);
							(*ppst_SubMat)->st_Id.i->pfn_AddRef(*ppst_SubMat, 1);
							WOR_gb_HasAddedMaterial = TRUE;
						}
					}
				}
			}

			/* ajoute la ref sur le materiau (l'objet l'utilise) */
			pst_Gro->i->pfn_AddRef(pst_Gro, 1);

			/* regarde si le materiau est dans la table */
			if(TAB_ul_Ptable_GetElemIndexWithPointer(pst_PTable, pst_Gro) == TAB_Cul_BadIndex)
			{
				/* il n'est pas dans la table : rajoutons le et incrémentons la ref de 1 */
				TAB_Ptable_AddElemAndResize(pst_PTable, pst_Gro);
				pst_Gro->i->pfn_AddRef(pst_Gro, 1);
				WOR_gb_HasAddedMaterial = TRUE;
			}
		}

        M_4Edit_CheckTransOpaque( _pst_GO );
	}

    
    /*$2
	 -------------------------------------------------------------------------------------------------------------------
	    modifier with material
	 -------------------------------------------------------------------------------------------------------------------
	 */
	if( _pst_GO->pst_Extended )
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
        MDF_tdst_Modifier	    *pst_Mod;
        GAO_tdst_ModifierXMEN   *p_XMEN;
	    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	    pst_Mod = _pst_GO->pst_Extended->pst_Modifiers;
	    while(pst_Mod)
	    {
		    if(pst_Mod->i->ul_Type == (ULONG) MDF_C_Modifier_XMEN) 
            {
                p_XMEN = (GAO_tdst_ModifierXMEN *) pst_Mod->p_Data;
                if (p_XMEN->p_MaterialUsed)
                {
                    /* regarde si le materiau est dans la table */
                    pst_PTable = &_pst_World->st_GraphicMaterialsTable;
                    pst_Gro = (GRO_tdst_Struct *) p_XMEN->p_MaterialUsed;
			        if(TAB_ul_Ptable_GetElemIndexWithPointer(pst_PTable, pst_Gro) == TAB_Cul_BadIndex)
			        {
				        /* il n'est pas dans la table : rajoutons le et incrémentons la ref de 1 */
				        TAB_Ptable_AddElemAndResize(pst_PTable, pst_Gro);
				        pst_Gro->i->pfn_AddRef(pst_Gro, 1);
				        WOR_gb_HasAddedMaterial = TRUE;
			        }
                }
            }
		    pst_Mod = pst_Mod->pst_Next;
	    }
    }


	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Animation:: create the AnimParameter structure and initialize it
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		pst_GameObjectAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

		pst_SkelModel = pst_GameObjectAnim->pst_SkeletonModel;
		pst_Skeleton = pst_GameObjectAnim->pst_Skeleton;
		pst_Shape = pst_GameObjectAnim->pst_Shape;

		if(pst_GameObjectAnim->pst_ActionKit)
		{
			ACT_UseActionKit(pst_GameObjectAnim->pst_ActionKit);
			EVE_gb_PlayOnlyAnimTracks = TRUE;
			ACT_SetAction(_pst_GO, 0, 0, FALSE);
			EVE_gb_PlayOnlyAnimTracks = FALSE;
		}
		else
		{
			if(pst_GameObjectAnim->apst_Anim[0] && (pst_GameObjectAnim->apst_Anim[0] != (ANI_tdst_Anim *) 0xFFFFFFFF))
			{
				pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks->ul_NbOfInstances++;
				pst_GameObjectAnim->apst_Anim[0]->uc_AnimFrequency = ANI_C_DefaultAnimFrequency;
			}

			if(pst_GameObjectAnim->uc_AnimUsed != 0)
			{
				EVE_gb_PlayOnlyAnimTracks = TRUE;
				ANI_PlayGameObjectAnimation(_pst_GO, pst_GameObjectAnim, 0.0f, NULL);
				EVE_gb_PlayOnlyAnimTracks = FALSE;
			}
		}
	}

#ifdef ACTIVE_EDITORS
	_pst_GO->pst_World = _pst_World;
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Links))
	{
		if(!WOR_gb_DoNotLoadLinkedObjects)
			WAY_CheckObjectInNetwork(_pst_GO);
		WAY_RegisterAllLinks(_pst_GO);
	}

#endif
}

/*
 =======================================================================================================================
    Aim:    Checks if the Father of the current GameObject has been loaded anf if not, removes it from memory. Checks
            also if the Object has the Hierarchy Identity Flag AND a Father.
 =======================================================================================================================
 */
void WOR_World_CheckFather(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	char	asz_Log[300], asz_Name[100], asz_FatherName[100];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
	{
		if(_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Hierarchy && _pst_GO->pst_Base->pst_Hierarchy->pst_Father)
		{
			if
			(
				TAB_ul_PFtable_GetElemIndexWithPointer
					(
						&_pst_World->st_AllWorldObjects,
						_pst_GO->pst_Base->pst_Hierarchy->pst_Father
					) == TAB_Cul_BadIndex
			)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				OBJ_tdst_GameObject *pst_Father;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_Father = _pst_GO->pst_Base->pst_Hierarchy->pst_Father;
#ifdef ACTIVE_EDITORS
				if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone))
				{
					L_strcpy(asz_Name, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) _pst_GO)));
					L_strcpy(asz_FatherName, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_Father)));

					sprintf
					(
						asz_Log,
						"[WARNING] The Father [%s] of the GameObject [%s] was not in the World and has been removed",
						asz_FatherName,
						asz_Name
					);
					LINK_PrintStatusMsg(asz_Log);
				}
				else
				{
					L_strcpy(asz_Name, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) _pst_GO)));
                    L_strcpy(asz_FatherName, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_Father)));

					sprintf
					(
						asz_Log,
						"[WARNING] The Father [%s] of the Bone [%s] was not in the World and has been removed",
						asz_FatherName,
						asz_Name
					);
					LINK_PrintStatusMsg(asz_Log);
				}
#endif
				WOR_World_DetachObject(_pst_World, pst_Father);
				OBJ_GameObject_Remove(pst_Father, 0);
			}
		}
		else
		{
#ifdef ACTIVE_EDITORS
			L_strcpy(asz_Name, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) _pst_GO)));
			sprintf(asz_Log, "[WARNING] [%s] has the Hierarchy Identity Flag but has no Father. Flag erased", asz_Name);
			LINK_PrintStatusMsg(asz_Log);
#endif
			_pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Hierarchy;
			if(_pst_GO->pst_Base->pst_Hierarchy) MEM_Free(_pst_GO->pst_Base->pst_Hierarchy);
			_pst_GO->pst_Base->pst_Hierarchy = NULL;
		}
	}
}


/*
 =======================================================================================================================
    Aim:    Add ref on graphics object for GAO that are in group but not in world.
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS

void WOR_World_AddRefBeforeCheckGroup( WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_Group		*pst_Group;
	TAB_tdst_PFelem		*pst_Current, *pst_End;
	OBJ_tdst_GameObject *pst_GroupGO;
	GRO_tdst_Struct		*pst_Gro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);

	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
	
		if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Group)) continue;
		if(!pst_GO->pst_Extended || !pst_GO->pst_Extended->pst_Group) continue;

		pst_Group = pst_GO->pst_Extended->pst_Group;
		if(!pst_Group->pst_AllObjects)
			continue;

		pst_Current = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
		pst_End = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
		for(; pst_Current <= pst_End; pst_Current++)
		{
			pst_GroupGO = (OBJ_tdst_GameObject *) pst_Current->p_Pointer;
			if(TAB_b_IsAHole(pst_GroupGO)) continue;
			if(TAB_ul_PFtable_GetElemIndexWithPointer(&_pst_World->st_AllWorldObjects, pst_GroupGO) == TAB_Cul_BadIndex)
			{
				if(OBJ_b_TestIdentityFlag(pst_GroupGO, OBJ_C_IdentityFlag_BaseObject))
				{
					/* Object with visu */
					if(OBJ_b_TestIdentityFlag(pst_GroupGO, OBJ_C_IdentityFlag_Visu))
					{
						pst_Gro = pst_GroupGO->pst_Base->pst_Visu->pst_Object;
						if (pst_Gro) pst_Gro->i->pfn_AddRef(pst_Gro, 1);
						pst_Gro = pst_GroupGO->pst_Base->pst_Visu->pst_Material;
						if (pst_Gro) pst_Gro->i->pfn_AddRef(pst_Gro, 1);
					}
				}
			}
		}
	}
}

#else

#define WOR_World_AddRefBeforeCheckGroup( a )		

#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
    Aim:    Checks if all the Objects of one object's group have been added in the World.
 =======================================================================================================================
 */
void WOR_World_CheckGroup(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group		*pst_Group;
	TAB_tdst_PFelem		*pst_Current, *pst_End;
	OBJ_tdst_GameObject *pst_GroupGO;
#ifdef ACTIVE_EDITORS
	char				asz_Log[300];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Group)) return;
	if(!_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Group) return;

	pst_Group = _pst_GO->pst_Extended->pst_Group;
#ifdef ACTIVE_EDITORS
	if(!pst_Group->pst_AllObjects)
	{
		sprintf(asz_Log, "Group association for %s does not have a gao list !", _pst_GO->sz_Name);
		ERR_X_Warning(0, asz_Log, NULL);
		return;
	}

#endif
	pst_Current = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
	pst_End = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
	for(; pst_Current <= pst_End; pst_Current++)
	{
		pst_GroupGO = (OBJ_tdst_GameObject *) pst_Current->p_Pointer;

		if(TAB_b_IsAHole(pst_GroupGO)) continue;
		if(TAB_ul_PFtable_GetElemIndexWithPointer(&_pst_World->st_AllWorldObjects, pst_GroupGO) == TAB_Cul_BadIndex)
		{
#ifdef ACTIVE_EDITORS
			sprintf
			(
				asz_Log,
				"[%s] of the group of [%s] was not in the World and has been removed",
				pst_GroupGO->sz_Name,
				_pst_GO->sz_Name
			);
			ERR_X_Warning(0, asz_Log, NULL);
#endif
			WOR_World_DetachObject(_pst_World, pst_GroupGO);
			OBJ_GameObject_Remove(pst_GroupGO, 1);

			/* We removed a GO referenced in a Group. Set the Reference as a Hole */
			TAB_PFtable_SetAsHole(pst_Group->pst_AllObjects, pst_Current);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_World_JustAfterLoadObject
(
	WOR_tdst_World		*_pst_World,
	OBJ_tdst_GameObject *_pst_GO,
	BOOL				_b_ResolveAI,
	BOOL				_b_Register
)
{
#ifdef ACTIVE_EDITORS
	extern ULONG LOA_gul_MakeFileRefSpyCaller;
	_pst_GO->pst_World = _pst_World;
	LOA_gul_MakeFileRefSpyCaller = _pst_GO->ul_MyKey;
#endif
	/* Update init pos */
	OBJ_Reinit(_pst_GO);

	/* Update gizmo ptr */
	OBJ_Gizmo_UpdateGizmoPtr(_pst_GO);

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
	{
		dMass			Mass;
		DYN_tdst_ODE	*pst_ODE;

		pst_ODE = _pst_GO->pst_Base->pst_ODE;
		/* anti bug */
		if(pst_ODE->uc_Type == ODE_TYPE_PLAN)
			pst_ODE->uc_Flags &=  ~ODE_FLAGS_RIGIDBODY;

		if(pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
		{
			pst_ODE->ode_id_body = dBodyCreate(_pst_World->ode_id_world);
			pst_ODE->uc_Flags |=  ODE_FLAGS_ENABLE;

			dBodySetData(pst_ODE->ode_id_body, (void *) _pst_GO);

			if(pst_ODE->uc_Flags & ODE_FLAGS_PAUSEINIT)
				dBodyDisable(pst_ODE->ode_id_body);
			else
				dBodyEnable(pst_ODE->ode_id_body);
		}
		else
			pst_ODE->ode_id_body = 0;

		pst_ODE->uc_Flags |=  ODE_FLAGS_MATHCOLMAP;

		switch(pst_ODE->uc_Type)
		{
		case ODE_TYPE_SPHERE:
			{
				/* Anti Bug */
				if(pst_ODE->f_X == 0.0f) pst_ODE->f_X = 0.5f;

				pst_ODE->ode_id_geom = dCreateSphere(_pst_World->ode_id_space, pst_ODE->f_X);
				dGeomSetData(pst_ODE->ode_id_geom, (void *) _pst_GO);

				if(pst_ODE->ode_id_body)
				{
					dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
					dMassSetSphereTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X);
					dBodySetMass(pst_ODE->ode_id_body, &Mass);
				}
			}
			break;

		case ODE_TYPE_BOX:
			{
				pst_ODE->ode_id_geom = dCreateBox(_pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
				dGeomSetData(pst_ODE->ode_id_geom, (void *) _pst_GO);

				if(pst_ODE->ode_id_body)
				{
					dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
					dMassSetBoxTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
					dBodySetMass(pst_ODE->ode_id_body, &Mass);
				}

			}
			break;

		case ODE_TYPE_CYLINDER:
			{
				pst_ODE->ode_id_geom = dCreateCylinder(_pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y);
				dGeomSetData(pst_ODE->ode_id_geom, (void *) _pst_GO);

				if(pst_ODE->ode_id_body)
				{
					dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
					dMassSetCylinderTotal(&Mass, pst_ODE->mass_init, 2, pst_ODE->f_X, pst_ODE->f_Y);
					dBodySetMass(pst_ODE->ode_id_body, &Mass);
				}

			}
			break;

		case ODE_TYPE_PLAN:
			{
				MATH_tdst_Vector	st_Norm;

				MATH_InitVector(&st_Norm, _pst_GO->pst_GlobalMatrix->Kx, _pst_GO->pst_GlobalMatrix->Ky, _pst_GO->pst_GlobalMatrix->Kz);
				pst_ODE->f_X = MATH_f_DotProduct(&_pst_GO->pst_GlobalMatrix->T, &st_Norm);

				pst_ODE->ode_id_geom = dCreatePlane(_pst_World->ode_id_space, _pst_GO->pst_GlobalMatrix->Kx, _pst_GO->pst_GlobalMatrix->Ky, _pst_GO->pst_GlobalMatrix->Kz, _pst_GO->pst_Base->pst_ODE->f_X);
				dGeomSetData(pst_ODE->ode_id_geom, (void *) _pst_GO);
			}
			break;


		default:
			pst_ODE->uc_Flags &=  ~ODE_FLAGS_MATHCOLMAP;
			break;

		}

		if(pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
		{
			if(pst_ODE->uc_Flags & ODE_FLAGS_AUTODISABLE)
			{
				dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 1);
				dBodySetAutoDisableLinearThreshold(pst_ODE->ode_id_body, pst_ODE->f_LinearThres);
				dBodySetAutoDisableAngularThreshold(pst_ODE->ode_id_body, pst_ODE->f_AngularThres);
				dBodySetAutoDisableSteps(pst_ODE->ode_id_body, 10);
			}
			else 
			{
				dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 0);
			}
		}
//		else
		{
			if((pst_ODE->ode_id_geom) && (pst_ODE->uc_Type != ODE_TYPE_PLAN))
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				float				rot[12];
				MATH_tdst_Vector	st_Offset;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_TransformVector(&st_Offset, _pst_GO->pst_GlobalMatrix, &pst_ODE->st_Offset);

				dGeomSetPosition(pst_ODE->ode_id_geom, _pst_GO->pst_GlobalMatrix->T.x + st_Offset.x, _pst_GO->pst_GlobalMatrix->T.y + + st_Offset.y, _pst_GO->pst_GlobalMatrix->T.z + + st_Offset.z);

				ODE_ComputeODEMatrixFromJade(_pst_GO->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot);

				dGeomSetRotation(pst_ODE->ode_id_geom, &rot[0]);
			}

		}

	}
#endif

	/* Update Sound */
    if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ExtendedObject))
    {
        if(_pst_GO->pst_Extended->pst_Modifiers) SND_ResolveRefsForMdF(_pst_GO->pst_Extended->pst_Modifiers);
    }


	/* Update Collision */
	if(OBJ_b_TestIdentityFlag(_pst_GO, (OBJ_C_IdentityFlag_ColMap | OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)))
		COL_ResolveRefs(_pst_GO);

	/* Update Anims */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)) ANI_ResolveRefs(_pst_GO);

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims) && !_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit && _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]) 	
		EVE_ResolveListTrackRef(_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks);

	if(_b_Register) WOR_World_AttachGameObject(_pst_World, _pst_GO);
	if(_b_ResolveAI)
	{
		OBJ_ResolveAIRef(_pst_GO);
#ifdef ACTIVE_EDITORS
		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)) ANI_ResolveAllTracksAndOptimize(_pst_GO);
#endif
	}

#if defined(_XENON_RENDER)
    // Pack the graphic object for Xenon (or use the existing packed data if available)
    if (
        !LOA_IsBinaryData()
#if defined(ACTIVE_EDITORS)
        && !LOA_IsBinarizing()
#endif
       )
    {
#if defined(ACTIVE_EDITORS)
        // Direct GAO to Geo association for _XMP_LOD objects
        if (EDI_gb_ComputeMap)
        {
            if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
            {
                if (_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Visu && _pst_GO->pst_Base->pst_Visu->pst_Object)
                {
                    if (_pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
                    {
                        GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)_pst_GO->pst_Base->pst_Visu->pst_Object;

                        if (strstr(pst_LOD->st_Id.sz_Name, "_XMP_LOD") && pst_LOD->dpst_Id[0])
                        {
                            _pst_GO->pst_Base->pst_Visu->pst_Object = pst_LOD->dpst_Id[0];
                        }
                    }
                }
            }
        }
#endif

        GEO_PackGameObject(_pst_GO, FALSE);
    }
	else if (LOA_IsBinaryData())
	{
		if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
		{
			GRO_tdst_Visu* pst_Visu = _pst_GO->pst_Base->pst_Visu;

			if (pst_Visu->ul_VBObjectValidate == XENON_VB_OBJECT_VALIDATE_MAGIC)
			{
				GEO_tdst_Object* pst_Geo = NULL;

				if (pst_Visu->pst_Object)
				{
					if (pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
					{
						GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;

						if ((pst_LOD->uc_NbLOD > 0) && (pst_LOD->dpst_Id[0]) && (pst_LOD->dpst_Id[0]->i->ul_Type == GRO_Geometric))
						{
							pst_Geo = (GEO_tdst_Object*)pst_LOD->dpst_Id[0];
						}
					}
					else if (pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
					{
						pst_Geo = (GEO_tdst_Object*)pst_Visu->pst_Object;
					}
				}

				pst_Visu->ul_VBObjectValidate = (ULONG)pst_Geo;
			}
		}
	}	
#endif

#if defined(ACTIVE_EDITORS) && defined(_XENON_RENDER)
	if (!LOA_IsBinarizing())
		XMP_CheckConsistency(_pst_World, _pst_GO);
#endif

#if defined(ACTIVE_EDITORS)
    // Issue a warning if the number of colors in the RLI is wrong for the
    // geometric object used by the game object
    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        GRO_tdst_Visu*   pst_Visu = _pst_GO->pst_Base->pst_Visu;
        GEO_tdst_Object* pst_Geo  = NULL;

        if (pst_Visu->dul_VertexColors && pst_Visu->pst_Object)
        {
            if (pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
            {
                pst_Geo = (GEO_tdst_Object*)pst_Visu->pst_Object;
            }
            else if (pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
            {
                GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;

                if ((pst_LOD->uc_NbLOD > 0) && (pst_LOD->dpst_Id[0] != NULL) &&
                    (pst_LOD->dpst_Id[0]->i->ul_Type == GRO_Geometric))
                {
                    pst_Geo = (GEO_tdst_Object*)pst_LOD->dpst_Id[0];
                }
            }

            if (pst_Geo && (pst_Geo->l_NbPoints > 0))
            {
                if (*pst_Visu->dul_VertexColors != (ULONG)pst_Geo->l_NbPoints)
                {
                    CHAR sz_Msg[512];

                    sprintf(sz_Msg, "%s - RLI does not have the same number of colors than the object",
                            _pst_GO->sz_Name ? _pst_GO->sz_Name : "<Unknown>");

                    ERR_X_Warning(FALSE, sz_Msg, NULL);
                }
            }
        }
    }
#endif

#ifdef ACTIVE_EDITORS
	LOA_gul_MakeFileRefSpyCaller = -1;
#endif
}

#if defined(_XBOX)

void WOR_vResetDX8FriendlyDataForVisuInFix(OBJ_tdst_GameObject *pst_GO)
{
	if (OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        if ((pst_GO->pst_Base->pst_Visu->pst_Object != NULL) &&
			(pst_GO->pst_Base->pst_Visu->pst_Object->l_Ref >= 1000000) )		// compute only objects in FIX
		{
			switch (pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type)
			{
			case GRO_Geometric :
				{
					GEO_tdst_Object *p_stObject = (GEO_tdst_Object *)pst_GO->pst_Base->pst_Visu->pst_Object;
					Gx8_FreeAddInfo(p_stObject);
				} break;
			case GRO_GeoStaticLOD :
				{
					int i;
					GEO_tdst_StaticLOD *p_stLOD = (GEO_tdst_StaticLOD *)pst_GO->pst_Base->pst_Visu->pst_Object;
					for( i = 0; i < p_stLOD->uc_NbLOD; i++)
					{
						GEO_tdst_Object *p_stObject = (GEO_tdst_Object *)p_stLOD->dpst_Id[i];
						Gx8_FreeAddInfo(p_stObject);
					}
				} break;
			default:
				break;
			}
		}
		pst_GO->pst_Base->pst_Visu->IndexInMultipleVBList = 0;	// reset Index
		// else this object will be used by GFX to render generated geometries... or it has already been treated.
	}
}

void WOR_vResetDX8FriendlyDataForAllObjectsInFix(WOR_tdst_World *_pst_Dest)
{
	TAB_tdst_PFelem *pst_PFElem, *pst_PFLastElem;

		// reset Gx8AddInfo for each element in fix
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_Dest->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_Dest->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		OBJ_tdst_GameObject *pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;

        WOR_vResetDX8FriendlyDataForVisuInFix( pst_GO );

        if ((OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims)) &&
            (pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton != NULL))
        {
            TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	        pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	        pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	        for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	        {
                OBJ_tdst_GameObject *pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

                WOR_vResetDX8FriendlyDataForVisuInFix( pst_BoneGO );
            }
        }
	}
    // max: Empty the one vertex buffer
    Gx8_VertexBuffer_Clean();
}


void WOR_vComputeDX8FriendlyDataForVisuIfNeeded(OBJ_tdst_GameObject *pst_GO)
{
	// if IndexInMultipleVBList != 0 then Visu has already been optimised
	if ( (OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu) ) && ( pst_GO->pst_Base->pst_Visu->IndexInMultipleVBList == 0 ) )
	{
		if (pst_GO->pst_Base->pst_Visu->pst_Object != NULL) 
		{
			switch (pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type)
			{
			case GRO_Geometric :
				{
					GEO_tdst_Object *p_stObject = (GEO_tdst_Object *)pst_GO->pst_Base->pst_Visu->pst_Object;


					//CONVERT COLOR IN XBOX FORMAT (CROMINACE LUMINACE)///////////////////////////////////////////////
					if(!p_stObject->colorAlreadyFiltered)
					{
							p_stObject->colorAlreadyFiltered=1;
							if(p_stObject->dul_PointColors)
								GEO_FilterColorLuminanceCrominace(p_stObject->dul_PointColors);

					}

					if(!pst_GO->pst_Base->pst_Visu->colorAlreadyFiltered)
					{
							pst_GO->pst_Base->pst_Visu->colorAlreadyFiltered=1;
							if(pst_GO->pst_Base->pst_Visu->dul_VertexColors)
                            {
								GEO_FilterColorLuminanceCrominace(pst_GO->pst_Base->pst_Visu->dul_VertexColors);
                            }
					}
					///////////////////////////////////////////////////////////////////////////////////////////////////

					if ((p_stObject->l_NbElements == 0) || (p_stObject->dst_Element == NULL) || !p_stObject->l_NbPoints)
					{
						pst_GO->pst_Base->pst_Visu->IndexInMultipleVBList = 0;
					}
					else
					{
						Gx8_SetCurrentGameObject(pst_GO);
						pst_GO->pst_Base->pst_Visu->IndexInMultipleVBList = Gx8_ComputeVertexBufferFriendlyDataForGeometricObject(p_stObject, pst_GO->pst_Base->pst_Visu->dul_VertexColors);
					}
				} break;

			case GRO_GeoStaticLOD :
				{
					int i;
					GEO_tdst_StaticLOD *p_stLOD = (GEO_tdst_StaticLOD *)pst_GO->pst_Base->pst_Visu->pst_Object;

					//CONVERT COLOR IN XBOX FORMAT (CROMINACE LUMINACE)
					if(!pst_GO->pst_Base->pst_Visu->colorAlreadyFiltered)
					{
							pst_GO->pst_Base->pst_Visu->colorAlreadyFiltered=1;
							if(pst_GO->pst_Base->pst_Visu->dul_VertexColors)
                            {
								GEO_FilterColorLuminanceCrominace(pst_GO->pst_Base->pst_Visu->dul_VertexColors);
                            }
					}
	

					for( i = 0; i < p_stLOD->uc_NbLOD; i++)
					{
						GEO_tdst_Object *p_stObject = (GEO_tdst_Object *)p_stLOD->dpst_Id[i];
						

						//CONVERT COLOR IN XBOX FORMAT (CROMINACE LUMINACE)
						if(!p_stObject->colorAlreadyFiltered)
						{
							p_stObject->colorAlreadyFiltered=1;
							if(p_stObject->dul_PointColors)
                            {
								GEO_FilterColorLuminanceCrominace(p_stObject->dul_PointColors);
                            }
						}



						if ((p_stObject->l_NbElements == 0) || (p_stObject->dst_Element == NULL) || !p_stObject->l_NbPoints)
						{
							pst_GO->pst_Base->pst_Visu->IndexInMultipleVBList = 0;
						}
						else
						{
							Gx8_SetCurrentGameObject(pst_GO);
							pst_GO->pst_Base->pst_Visu->IndexInMultipleVBList = Gx8_ComputeVertexBufferFriendlyDataForGeometricObject(p_stObject, pst_GO->pst_Base->pst_Visu->dul_VertexColors);
						}
					}
				} break;
			default:
				break;
			}
		}
		// else this object will be used by GFX to render generated geometries... or it has already been treated.
	}
}

void WOR_vComputeDX8FriendlyDataForAllObjects(WOR_tdst_World *_pst_Dest)
{
	TAB_tdst_PFelem *pst_PFElem, *pst_PFLastElem;

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_Dest->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_Dest->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		OBJ_tdst_GameObject *pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;

        WOR_vComputeDX8FriendlyDataForVisuIfNeeded(pst_GO);

        if ((OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims)) &&
            (pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton != NULL))
        {
            TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	        pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	        pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	        for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	        {
                OBJ_tdst_GameObject *pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

                WOR_vComputeDX8FriendlyDataForVisuIfNeeded(pst_BoneGO);
            }
        }
	}
}

// Precompute stuff for the Xbox/DX8 engine - we need to build as much vertex/index buffers as we can
// and to gather any info we can...
void WOR_vComputeDX8FriendlyData(WOR_tdst_World *_pst_Dest)
{
		// reset Gx8AddInfo for each object in fix
	WOR_vResetDX8FriendlyDataForAllObjectsInFix(_pst_Dest);

		// compute everything
	WOR_vComputeDX8FriendlyDataForAllObjects(_pst_Dest);
}

#endif /* _XBOX */
/*
 =======================================================================================================================
 =======================================================================================================================
 */
WOR_tdst_World *WOR_pst_World_Load(WOR_tdst_World *_pst_Dest, BIG_KEY _ul_FileKey, BOOL _b_ResolveAIRef)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Pos;
	LONG				i, LastElem, indexgro, num;
	ULONG				ul_Index;
	TAB_tdst_Ptable		*pst_MatTable, *pst_GroTable;
	GRO_tdst_Struct		**ppst_Gro, **ppst_Last, *pst_Gro;
	ULONG				ul_MatEnd, ul_GroEnd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_GroEnd = 0;
	ul_MatEnd = 0;
	if(_ul_FileKey == BIG_C_InvalidKey) return NULL;
	ul_Pos = BIG_ul_SearchKeyToPos(_ul_FileKey);
	if(ul_Pos != (ULONG) - 1)
	{
		pst_World = (WOR_tdst_World *) LOA_ul_SearchAddress(ul_Pos);
		if((int) pst_World != -1)
		{
			return pst_World;
		}
	}

#if _XENON_SOUND_ENGINE
	GetAudioEngine()->GetPrefetchList()->Flush();
	GetAudioEngine()->GetHeaderList()->OpenMapFile(_ul_FileKey);
#endif

	/* LOA_BeginSpeedMode(_ul_FileKey); */
	LOA_MakeFileRef(_ul_FileKey, (ULONG *) &pst_World, WOR_ul_World_LoadCallback, LOA_C_MustExists);
	PROPS2_StartRaster(&PROPS2_gst_LOA_Resolve);
	LOA_Resolve();
	PROPS2_StopRaster(&PROPS2_gst_LOA_Resolve);

	if(!pst_World)
	{
		/*~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
		char	asz_Msg[256];
		/*~~~~~~~~~~~~~~~~~*/

		sprintf(asz_Msg, "Unable to load world (%x)", _ul_FileKey);
		ERR_X_Warning(0, asz_Msg, NULL);
#endif
		/* LOA_EndSpeedMode(); */
		return NULL;
	}

	/* Merge with an existing world ? */
	if(!_pst_Dest) _pst_Dest = pst_World;

	if(pst_World)
	{
		pst_MatTable = &_pst_Dest->st_GraphicMaterialsTable;
		pst_GroTable = &_pst_Dest->st_GraphicObjectsTable;
		if(pst_World != _pst_Dest)
		{
			TAB_Ptable_RemoveHoles(pst_MatTable);
			ul_MatEnd = TAB_ul_Ptable_GetNbElems(pst_MatTable);
			TAB_Ptable_RemoveHoles(pst_GroTable);
			ul_GroEnd = TAB_ul_Ptable_GetNbElems(pst_GroTable);
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    First init game object
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(pst_World != _pst_Dest)
		{
			/* For a merge, force all actual object to be "merge" */
			pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_Dest->st_AllWorldObjects);
			pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_Dest->st_AllWorldObjects);
			for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
			{
				pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
				if(TAB_b_IsAHole(pst_GO)) continue;
				if(pst_GO) pst_GO->c_FixFlags |= OBJ_C_HasBeenMerge;
			}
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    We go thru all the new loaded world to call the WOR_World_JustAfterLoaded function for all the GameObjects. This
		    function can create new Gao and so resize the Table in memory. It is the reason why we cannot just go thru the
		    Table as usual but must use a counter.
		 ---------------------------------------------------------------------------------------------------------------
		 */

		LastElem = TAB_ul_PFtable_GetNbElems(&pst_World->st_AllWorldObjects) + TAB_ul_PFtable_GetNbHoles(&pst_World->st_AllWorldObjects);
		for(i = 0; i < LastElem; i++)
		{
			/* DO NOT put the following line outside the loop !!! */
			pst_PFElem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_AllWorldObjects);
			pst_PFElem += i;
			pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;

			/* If the Gao is already in the destination world, we do not reload it !!! */
			if((TAB_b_IsAHole(pst_GO)) || (pst_GO->c_FixFlags & OBJ_C_HasBeenMerge)) continue;
			WOR_gpst_CurrentWorld = _pst_Dest;
			WOR_World_JustAfterLoadObject(_pst_Dest, pst_GO, _b_ResolveAIRef, TRUE);
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Merge objects
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(pst_World != _pst_Dest)
		{
			pst_PFElem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_AllWorldObjects);
			pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&pst_World->st_AllWorldObjects);
			for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
			{
				pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
				if(TAB_b_IsAHole(pst_GO)) continue;
				if(pst_GO)
				{
					/*
					 * If merge flag is already here, there's an object in the new world that is
					 * already in the dest one. So no need to add it again but we reinit it.
					 */
					if(pst_GO->c_FixFlags & OBJ_C_HasBeenMerge)
						ENG_ReinitOneObject(pst_GO, UNI_Cuc_AfterLoadWorld);

					/*
					 * An object in the new world is force to be not merge (even if it's already in
					 * the dest world)
					 */
					pst_GO->c_FixFlags &= ~OBJ_C_HasBeenMerge;

					if(pst_World != _pst_Dest)
					{
						if
						(
							TAB_ul_PFtable_GetElemIndexWithPointer
								(
									&_pst_Dest->st_AllWorldObjects,
									pst_GO
								) == TAB_Cul_BadIndex
						)
						{
							pst_GO->c_FixFlags &= ~OBJ_C_HasBeenMerge;
							TAB_PFtable_AddElemWithDataAndResize(&_pst_Dest->st_AllWorldObjects, pst_GO, 0);
						}
					}
				}
			}
		}


		WOR_World_AddRefBeforeCheckGroup( _pst_Dest );

		pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_Dest->st_AllWorldObjects);
		pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_Dest->st_AllWorldObjects);
		for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			WOR_World_CheckFather(pst_GO, _pst_Dest);
			WOR_World_CheckGroup(pst_GO, _pst_Dest);
		}

#ifdef ACTIVE_EDITORS
		{
			extern void COL_Check_Cobs(WOR_tdst_World *);
			COL_Check_Cobs(_pst_Dest);
		}
#endif

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Default cam
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(pst_World != _pst_Dest)
		{
			L_memcpy(&_pst_Dest->st_CameraPosSave, &pst_World->st_CameraPosSave, sizeof(MATH_tdst_Matrix));
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Other fields
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(pst_World != _pst_Dest)
		{
#ifdef JADEFUSION
            _pst_Dest->f_XeGlowIntensity = pst_World->f_XeGlowIntensity;
		    _pst_Dest->f_XeGlowLuminosityMin = pst_World->f_XeGlowLuminosityMin;
            _pst_Dest->f_XeGlowLuminosityMax = pst_World->f_XeGlowLuminosityMax;
            _pst_Dest->ul_XeGlowColor = pst_World->ul_XeGlowColor;   
            _pst_Dest->ul_XeDiffuseColor = pst_World->ul_XeDiffuseColor;   
            _pst_Dest->ul_XeSpecularColor = pst_World->ul_XeSpecularColor;   
            _pst_Dest->f_XeSpecularShiny = pst_World->f_XeSpecularShiny;   
            _pst_Dest->f_XeSpecularStrength = pst_World->f_XeSpecularStrength;   
            _pst_Dest->f_XeGlowZNear = pst_World->f_XeGlowZNear;   
            _pst_Dest->f_XeGlowZFar = pst_World->f_XeGlowZFar;   
            _pst_Dest->f_XeGaussianStrength = pst_World->f_XeGaussianStrength;
            _pst_Dest->f_XeRLIScale = pst_World->f_XeRLIScale;
            _pst_Dest->f_XeRLIOffset = pst_World->f_XeRLIOffset;
            _pst_Dest->f_XeMipMapLODBias = pst_World->f_XeMipMapLODBias;
			_pst_Dest->f_XeDryDiffuseFactor = pst_World->f_XeDryDiffuseFactor;
			_pst_Dest->f_XeWetDiffuseFactor = pst_World->f_XeWetDiffuseFactor;
			_pst_Dest->f_XeDrySpecularBoost = pst_World->f_XeDrySpecularBoost;
			_pst_Dest->f_XeWetSpecularBoost = pst_World->f_XeWetSpecularBoost;
			_pst_Dest->f_XeRainEffectDelay = pst_World->f_XeRainEffectDelay;
			_pst_Dest->f_XeRainEffectDryDelay = pst_World->f_XeRainEffectDryDelay;
			_pst_Dest->pSPG2Light = pst_World->pSPG2Light;
            _pst_Dest->f_XeGodRayIntensity = pst_World->f_XeGodRayIntensity;
            _pst_Dest->ul_XeGodRayIntensityColor = pst_World->ul_XeGodRayIntensityColor;
            _pst_Dest->b_XeMaterialLODEnable = pst_World->b_XeMaterialLODEnable;
            _pst_Dest->f_XeMaterialLODNear = pst_World->f_XeMaterialLODNear;
            _pst_Dest->f_XeMaterialLODFar = pst_World->f_XeMaterialLODFar;
            _pst_Dest->b_XeMaterialLODDetailEnable = pst_World->b_XeMaterialLODDetailEnable;
            _pst_Dest->f_XeMaterialLODDetailNear = pst_World->f_XeMaterialLODDetailNear;
            _pst_Dest->f_XeMaterialLODDetailFar = pst_World->f_XeMaterialLODDetailFar;
            _pst_Dest->f_XeSaturation = pst_World->f_XeSaturation;
            _pst_Dest->v_XeBrightness = pst_World->v_XeBrightness;
            _pst_Dest->f_XeContrast = pst_World->f_XeContrast;
 #endif
			_pst_Dest->ul_AmbientColor = pst_World->ul_AmbientColor;
			_pst_Dest->ul_AmbientColor2 = pst_World->ul_AmbientColor2;
			_pst_Dest->ul_BackgroundColor = pst_World->ul_BackgroundColor;
			_pst_Dest->i_LODCut = pst_World->i_LODCut;
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Merge sectors
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(pst_World != _pst_Dest)
		{
			for(i = 0; i < WOR_C_MaxSecto; i++)
			{
				if(_pst_Dest->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)
				{
					_pst_Dest->ast_AllSectos[i].ul_Flags |= WOR_CF_SectorMerged;
				}

				if(pst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)
				{
					/* Two sectors with the same id */
#ifdef ACTIVE_EDITORS
					if(_pst_Dest->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)
					{
						ERR_X_Warning(0, "Two sectors with the same ID", NULL);
					}
#endif
					L_memcpy(&_pst_Dest->ast_AllSectos[i], &pst_World->ast_AllSectos[i], sizeof(WOR_tdst_Secto));
					_pst_Dest->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorMerged;
				}
			}
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Merge networks
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(pst_World != _pst_Dest)
		{
			if(_pst_Dest->pst_AllNetworks)
			{
				if(pst_World->pst_AllNetworks)
				{
					for(i = 0; i < (int) _pst_Dest->pst_AllNetworks->ul_Num; i++)
					{
						if(_pst_Dest->pst_AllNetworks->ppst_AllNetworks[i])
							_pst_Dest->pst_AllNetworks->ppst_AllNetworks[i]->ul_Flags |= WAY_C_HasBeenMerge;
					}

					num = _pst_Dest->pst_AllNetworks->ul_Num;
					_pst_Dest->pst_AllNetworks->ul_Num += pst_World->pst_AllNetworks->ul_Num;
					_pst_Dest->pst_AllNetworks->ppst_AllNetworks = (WAY_tdst_Network**)MEM_p_Realloc
						(
							_pst_Dest->pst_AllNetworks->ppst_AllNetworks,
							_pst_Dest->pst_AllNetworks->ul_Num * sizeof(void *)
						);
					for(i = 0; i < (int) pst_World->pst_AllNetworks->ul_Num; i++)
					{
						_pst_Dest->pst_AllNetworks->ppst_AllNetworks[i + num] = pst_World->pst_AllNetworks->ppst_AllNetworks[i];
					}

					MEM_Free(pst_World->pst_AllNetworks->ppst_AllNetworks);
					pst_World->pst_AllNetworks->ppst_AllNetworks = _pst_Dest->pst_AllNetworks->ppst_AllNetworks;
					pst_World->pst_AllNetworks->ul_Num = _pst_Dest->pst_AllNetworks->ul_Num;

					LOA_DeleteAddress(_pst_Dest->pst_AllNetworks);
					MEM_Free(_pst_Dest->pst_AllNetworks);
					_pst_Dest->pst_AllNetworks = pst_World->pst_AllNetworks;
					pst_World->pst_AllNetworks = NULL;
				}
				else
				{
					for(i = 0; i < (int) _pst_Dest->pst_AllNetworks->ul_Num; i++)
					{
						if(_pst_Dest->pst_AllNetworks->ppst_AllNetworks[i])
							_pst_Dest->pst_AllNetworks->ppst_AllNetworks[i]->ul_Flags |= WAY_C_HasBeenMerge;
					}
				}
			}
			else
			{
				_pst_Dest->pst_AllNetworks = pst_World->pst_AllNetworks;
				pst_World->pst_AllNetworks = NULL;
			}
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Merge grid
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(pst_World != _pst_Dest)
		{
			if(pst_World->pst_Grid || pst_World->pst_Grid1)
			{
				if(_pst_Dest->pst_Grid && (_pst_Dest->pst_Grid != pst_World->pst_Grid)) GRI_Destroy(_pst_Dest->pst_Grid);
				if(_pst_Dest->pst_Grid1 && (_pst_Dest->pst_Grid1 != pst_World->pst_Grid1)) GRI_Destroy(_pst_Dest->pst_Grid1);

				_pst_Dest->pst_Grid = pst_World->pst_Grid;
				_pst_Dest->pst_Grid1 = pst_World->pst_Grid1;
				pst_World->pst_Grid = NULL;
				pst_World->pst_Grid1 = NULL;
			}
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Add ref on geometric used by static LOD graphic object
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(pst_World != _pst_Dest)
			ppst_Gro = (GRO_tdst_Struct **) pst_GroTable->p_Table + ul_GroEnd;
		else
			ppst_Gro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(pst_GroTable);
		ppst_Last = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(pst_GroTable);
		for(; ppst_Gro <= ppst_Last; ppst_Gro++)
		{
			if((*ppst_Gro)->i->ul_Type == GRO_GeoStaticLOD)
			{
				for(i = 0; i < ((GEO_tdst_StaticLOD *) (*ppst_Gro))->uc_NbLOD; i++)
				{
					pst_Gro = ((GEO_tdst_StaticLOD *) (*ppst_Gro))->dpst_Id[i];
					if(!pst_Gro) continue;
					if(pst_Gro->i->ul_Type != GRO_Geometric)
					{
						((GEO_tdst_StaticLOD *) (*ppst_Gro))->dpst_Id[i] = NULL;
						continue;
					}

					pst_Gro->i->pfn_AddRef(pst_Gro, 1);
					ul_Index = TAB_ul_Ptable_GetElemIndexWithPointer(pst_GroTable, pst_Gro);
					if(ul_Index == TAB_Cul_BadIndex)
					{
						if(pst_GroTable->p_NextElem > pst_GroTable->p_LastPointer)
							indexgro = ppst_Gro - (GRO_tdst_Struct **) pst_GroTable->p_Table;
						else
							indexgro = -1;

						TAB_Ptable_AddElemAndResize(pst_GroTable, pst_Gro);
						pst_Gro->i->pfn_AddRef(pst_Gro, 1);

						if(indexgro != -1)
						{
							ppst_Gro = (GRO_tdst_Struct **) (pst_GroTable->p_Table + indexgro);
							ppst_Last = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(pst_GroTable);
						}
					}
				}
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Destroy source world ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_pst_Dest != pst_World)
	{
		/* Merge SnP */
		_pst_Dest->pst_SnP = pst_World->pst_SnP;
		pst_World->pst_SnP = NULL;

		/* Change source world to have properties of merge one */
		L_strcpy(_pst_Dest->sz_Name, pst_World->sz_Name);
		TAB_PFtable_Close(&pst_World->st_AllWorldObjects);
		WOR_World_Destroy(pst_World);
		LOA_DeleteAddress((void *) _pst_Dest);
#ifdef ACTIVE_EDITORS
		LOA_AddAddress(BIG_ul_SearchKeyToFat(_ul_FileKey), (void *) _pst_Dest);
#else
		LOA_AddPosAddress(BIG_ul_SearchKeyToPos(_ul_FileKey), (void *) _pst_Dest);
#endif
	}

#ifdef ACTIVE_EDITORS
	if(_pst_Dest) 
	{
		SEL_Close(_pst_Dest->pst_Selection);
		WORGos_Load(_pst_Dest);
	}
#endif

#ifdef JADEFUSION
	// Now that world data is fully loaded, resolve the light rejection 
	// list. Add rejected list of light to each gao in list that is loaded 
	// in memory.
	LRL_Resolve();
#endif
	/* LOA_EndSpeedMode(); */
	return _pst_Dest;
}

/*$4
 ***********************************************************************************************************************
    Private functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef MEM_SPY
float WOR_TimeSinceLastWorldLoad;
#endif

BIG_KEY g_ul_GroupKey;
ULONG WOR_ul_World_LoadCallback(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	ULONG			ul_Length;
	CHAR			*pc_Buffer;
	BIG_KEY			ul_GroupKey;
	int				i, j;
	int				version;
#ifdef ACTIVE_EDITORS
#if !defined(XML_CONV_TOOL)
	char			asz_Path[BIG_C_MaxLenPath];
#endif // !defined(XML_CONV_TOOL)
	BIG_INDEX		ul_Key;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef MEM_SPY
#ifdef ACTIVE_EDITORS
	WOR_TimeSinceLastWorldLoad = 0;
#else
	WOR_TimeSinceLastWorldLoad = TIM_gf_MainClock;
#endif
#endif

	pst_World = WOR_pst_World_Create();
	pst_World->h_WorldKey = LOA_ul_GetCurrentKey();

	/* Get data */
	pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Length);

	LOA_ReadCharArray(&pc_Buffer, (CHAR*)&ul_GroupKey, 4);

	/* Read save data (type and version of struct) */
	if(strncmp((CHAR*)&ul_GroupKey, EDI_Csz_ExtWorld, 4))
	{
		ERR_X_Warning(0, "Bad World file", NULL);
		return 0;
	}

	version = LOA_ReadULong(&pc_Buffer);
#if defined(XML_CONV_TOOL)
	g_nWOWVersion = version;
#endif // defined(XML_CONV_TOOL)
	pst_World->i_NumDelObj = 0;

	/* Read world data for init and init it */
	pst_World->ul_NbTotalGameObjects = LOA_ReadULong(&pc_Buffer);

	WOR_World_Init(pst_World, pst_World->ul_NbTotalGameObjects);

	/* Read other world data */
	pst_World->ul_AmbientColor = LOA_ReadULong(&pc_Buffer);
#ifdef ACTIVE_EDITORS
#if !defined(XML_CONV_TOOL)
	ul_Key = BIG_ul_SearchKeyToFat(LOA_ul_GetCurrentKey());
	LOA_ReadString(&pc_Buffer, pst_World->sz_Name, 60); // Read the name from the bigfile so it will be available to the non editor build
	L_strcpy(pst_World->sz_Name, BIG_NameFile(ul_Key)); // Replace the name that was just read with the name from the bigfile.
	LOA_ReadLong_Ed(&pc_Buffer, NULL); // Skip 4 extra bytes.
#else
#ifdef JADEFUSION
	ul_Key = 0;
	LOA_ReadString(&pc_Buffer, pst_World->sz_Name, 60); // We need all 64 bytes available for the conversion tool
	//popowarning LOA_ReadLong_Ed(&pc_Buffer, (LONG* )&lNamePadding); // Store 4 extra bytes
#endif // jadefusion
#endif
#else
	LOA_ReadString(&pc_Buffer, pst_World->sz_Name, 60); // Read the name from the bigfile
	LOA_ReadLong_Ed(&pc_Buffer, NULL); // Skip 4 extra bytes.
#endif
	LOA_ReadMatrix(&pc_Buffer, &pst_World->st_CameraPosSave);
#if !defined(XML_CONV_TOOL)
	if(!MATH_b_TypeIsCorrect(&pst_World->st_CameraPosSave))
	{
		MATH_SetIdentityMatrix(&pst_World->st_CameraPosSave);
		MATH_InitVectorToZero(&pst_World->st_CameraPosSave.T);
	}
#endif // XML_CONV_TOOL
	WOR_gpst_WorldToLoadIn = pst_World;

	/* Register world */
#ifdef ACTIVE_EDITORS
	ul_GroupKey = LOA_ul_GetCurrentKey();
	ul_GroupKey = BIG_ul_SearchKeyToFat(ul_GroupKey);
#if !defined(XML_CONV_TOOL)
	BIG_ComputeFullName(BIG_ParentFile(ul_GroupKey), asz_Path);
	LINK_RegisterPointer(pst_World, LINK_C_ENG_World, BIG_NameFile(ul_GroupKey), asz_Path);
#endif // !defined(XML_CONV_TOOL)
#endif
	pst_World->pst_View->st_DisplayInfo.f_FieldOfVision = LOA_ReadFloat(&pc_Buffer);

	pst_World->ul_BackgroundColor = LOA_ReadULong(&pc_Buffer);

	pst_World->ul_AmbientColor2 = LOA_ReadULong(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
	if(pst_World->ul_AmbientColor2 == 0xFFFFFFFF) pst_World->ul_AmbientColor2 = pst_World->ul_AmbientColor;
#endif // !defined(XML_CONV_TOOL)
	pst_World->i_LODCut = LOA_ReadInt(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
	if(pst_World->i_LODCut < 0 || pst_World->i_LODCut > 255) pst_World->i_LODCut = 0;
#endif // !defined(XML_CONV_TOOL)

#ifdef _XENON_RENDER

    // Xenon glow luminosity cutoff min
    pst_World->f_XeGlowLuminosityMin  = LOA_ReadFloat(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
    if (*(ULONG*)&pst_World->f_XeGlowLuminosityMin == BIG_C_InvalidKey)
        pst_World->f_XeGlowLuminosityMin = 1.0f;
#endif //#if !defined(XML_CONV_TOOL)

    // Xenon glow z-near
    pst_World->f_XeGlowZNear  = LOA_ReadFloat(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
    if (*(ULONG*)&pst_World->f_XeGlowZNear == BIG_C_InvalidKey)
        pst_World->f_XeGlowZNear = 0.0f;
#endif //#if !defined(XML_CONV_TOOL)

    // Xenon glow z-far
    pst_World->f_XeGlowZFar  = LOA_ReadFloat(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
    if (*(ULONG*)&pst_World->f_XeGlowZFar == BIG_C_InvalidKey)
        pst_World->f_XeGlowZFar = 10000.0f;
#endif //#if !defined(XML_CONV_TOOL)

    // Xenon glow color
    pst_World->ul_XeGlowColor  = LOA_ReadULong(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
    if (*(ULONG*)&pst_World->ul_XeGlowColor == BIG_C_InvalidKey)
        pst_World->ul_XeGlowColor = 0xFFFFFFFF;
#endif //#if !defined(XML_CONV_TOOL)

    // Xenon glow luminosity cutoff max
    pst_World->f_XeGlowLuminosityMax  = LOA_ReadFloat(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
    if (*(ULONG*)&pst_World->f_XeGlowLuminosityMax == BIG_C_InvalidKey)
        pst_World->f_XeGlowLuminosityMax = 0.8f;
#endif //#if !defined(XML_CONV_TOOL)

    if( pst_World->f_XeGlowLuminosityMin > pst_World->f_XeGlowLuminosityMax )
    {
        pst_World->f_XeGlowLuminosityMin = pst_World->f_XeGlowLuminosityMax;
    }

    // Xenon glow intensity
    pst_World->f_XeGlowIntensity  = LOA_ReadFloat(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
    if (*(ULONG*)&pst_World->f_XeGlowIntensity == BIG_C_InvalidKey)
        pst_World->f_XeGlowIntensity = 0.0f;
#endif //#if !defined(XML_CONV_TOOL)

    // Xenon shadow blur strength
    pst_World->f_XeGaussianStrength  = LOA_ReadFloat(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
    if (*(ULONG*)&pst_World->f_XeGaussianStrength == BIG_C_InvalidKey)
        pst_World->f_XeGaussianStrength = 0.005f;
#endif //#if !defined(XML_CONV_TOOL)

    // Xenon RLI scale and offset
    pst_World->f_XeRLIScale  = LOA_ReadFloat(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
	if (*(ULONG*)&pst_World->f_XeRLIScale == BIG_C_InvalidKey)
        pst_World->f_XeRLIScale = 1.0f;
#endif //#if !defined(XML_CONV_TOOL)

    pst_World->f_XeRLIOffset = LOA_ReadFloat(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
	if (*(ULONG*)&pst_World->f_XeRLIOffset == BIG_C_InvalidKey)
        pst_World->f_XeRLIOffset = 0.0f;
#endif //#if !defined(XML_CONV_TOOL)

    // Xenon mipmap LOD bias
    pst_World->f_XeMipMapLODBias = LOA_ReadFloat(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
    if (*(ULONG*)&pst_World->f_XeMipMapLODBias == 0xffffffff)
    {
        // Value was not saved, reset it
        pst_World->f_XeMipMapLODBias = 0.0f;
    }
    else
    {
        pst_World->f_XeMipMapLODBias = MATH_f_FloatLimit(pst_World->f_XeMipMapLODBias, -1.0f, 2.0f);
    }
#endif //#if !defined(XML_CONV_TOOL)

	// Xenon specular boost factors
	if (version >= 6)
	{	
		pst_World->f_XeDrySpecularBoost = LOA_ReadFloat(&pc_Buffer);
		pst_World->f_XeWetSpecularBoost = LOA_ReadFloat(&pc_Buffer);
		pst_World->f_XeRainEffectDelay = LOA_ReadFloat(&pc_Buffer);
		pst_World->f_XeRainEffectDryDelay = LOA_ReadFloat(&pc_Buffer);
	}
	else
	{
		pst_World->f_XeDrySpecularBoost = 0.0f;
		pst_World->f_XeWetSpecularBoost = 0.0f;
		pst_World->f_XeRainEffectDelay = 10.0f;
		pst_World->f_XeRainEffectDryDelay = 20.0f;
	}

	if (version >= 7)
	{
		pst_World->f_XeDryDiffuseFactor = LOA_ReadFloat(&pc_Buffer);
		pst_World->f_XeWetDiffuseFactor = LOA_ReadFloat(&pc_Buffer);
	}
	else
	{
		pst_World->f_XeDryDiffuseFactor = 0.0f;
		pst_World->f_XeWetDiffuseFactor = 0.0f;
	}

    if (version >= 8)
    {
        pst_World->ul_XeDiffuseColor = LOA_ReadULong(&pc_Buffer);
        pst_World->ul_XeSpecularColor = LOA_ReadULong(&pc_Buffer);
    }

#if !defined(XML_CONV_TOOL)
    if (version < 10)
    {
        pst_World->ul_XeDiffuseColor = 0xff808080;
        pst_World->ul_XeSpecularColor = 0xff808080;
    }
#endif //#if !defined(XML_CONV_TOOL)

    if( version >= 9 )
    {
        pst_World->f_XeGodRayIntensity = LOA_ReadFloat(&pc_Buffer);
        pst_World->ul_XeGodRayIntensityColor = LOA_ReadULong(&pc_Buffer);
    }
    else
    {
        pst_World->f_XeGodRayIntensity = 1.0f;
        pst_World->ul_XeGodRayIntensityColor = 0xFFFFFFFF;
    }

    if (version >= 10)
    {
        pst_World->f_XeSpecularShiny = LOA_ReadFloat(&pc_Buffer);
        pst_World->f_XeSpecularStrength = LOA_ReadFloat(&pc_Buffer);
    }
    else
    {
        pst_World->f_XeSpecularShiny = 1.0f;
        pst_World->f_XeSpecularStrength = 1.0f;
    }

    if (version >= 11)
    {
        pst_World->b_XeMaterialLODEnable = LOA_ReadULong(&pc_Buffer);
        pst_World->f_XeMaterialLODNear = LOA_ReadFloat(&pc_Buffer);
        pst_World->f_XeMaterialLODFar = LOA_ReadFloat(&pc_Buffer);
        pst_World->b_XeMaterialLODDetailEnable = LOA_ReadULong(&pc_Buffer);
        pst_World->f_XeMaterialLODDetailNear = LOA_ReadFloat(&pc_Buffer);
        pst_World->f_XeMaterialLODDetailFar = LOA_ReadFloat(&pc_Buffer);
    }
    else
    {
        pst_World->b_XeMaterialLODEnable = FALSE;
        pst_World->f_XeMaterialLODNear = 35.0f;
        pst_World->f_XeMaterialLODFar = 40.0f;
        pst_World->b_XeMaterialLODDetailEnable = FALSE;
        pst_World->f_XeMaterialLODDetailNear = 20.0f;
        pst_World->f_XeMaterialLODDetailFar = 25.0f;
    }

    MATH_InitVector( &pst_World->v_XeBrightness, 0.0f, 0.0f, 0.0f );
    pst_World->f_XeSaturation = 1.0f;
    pst_World->f_XeContrast  = 0.0f;

    if(version == 12)
    {
        pst_World->f_XeSaturation = LOA_ReadFloat(&pc_Buffer);
        pst_World->v_XeBrightness.x = 5.0f * LOA_ReadFloat(&pc_Buffer);
        pst_World->v_XeBrightness.y = pst_World->v_XeBrightness.x;
        pst_World->v_XeBrightness.z = pst_World->v_XeBrightness.x;
        pst_World->f_XeContrast = 5.0f * LOA_ReadFloat(&pc_Buffer);
    }
    else if(version >= 13)
    {
        pst_World->f_XeSaturation = LOA_ReadFloat(&pc_Buffer);

        pst_World->v_XeBrightness.x = LOA_ReadFloat(&pc_Buffer);
        pst_World->v_XeBrightness.y = LOA_ReadFloat(&pc_Buffer);
        pst_World->v_XeBrightness.z = LOA_ReadFloat(&pc_Buffer);

        pst_World->f_XeContrast = LOA_ReadFloat(&pc_Buffer);
    }
   
	ULONG ulLightGAOKey = LOA_ReadULong(&pc_Buffer);
	if(ulLightGAOKey && ulLightGAOKey != BIG_C_InvalidKey)
	{
		LOA_MakeFileRef(ulLightGAOKey, (ULONG *) &pst_World->pSPG2Light, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
	}
#else // #if _XENON_RENDER
	LOA_ReadCharArray_Ed(&pc_Buffer, NULL, 44); // Skip 44 dummy bytes
#endif // #if  _XENON_RENDER

	/* Read grid */
	ul_GroupKey = LOA_ReadULong(&pc_Buffer);
	if(ul_GroupKey && ul_GroupKey != BIG_C_InvalidKey)
	{
		LOA_MakeFileRef(ul_GroupKey, (ULONG *) &pst_World->pst_Grid1, GRID_ul_CallbackWorldGrid, LOA_C_MustExists);
	}

	/* Read grid */
	ul_GroupKey = LOA_ReadULong(&pc_Buffer);
	if(ul_GroupKey && ul_GroupKey != BIG_C_InvalidKey)
	{
		LOA_MakeFileRef(ul_GroupKey, (ULONG *) &pst_World->pst_Grid, GRID_ul_CallbackWorldGrid, LOA_C_MustExists);
	}

	/* Read all game objects */
    g_ul_GroupKey = LOA_ReadULong(&pc_Buffer);
	LOA_MakeFileRef(g_ul_GroupKey, NULL, WOR_ul_GameObjectGroupCallback, LOA_C_MustExists | LOA_C_NotSavePtr);

#if defined(XML_CONV_TOOL)
	g_ul_GroupKey = ul_GroupKey;
#endif
	/* Read networks */
	ul_GroupKey = LOA_ReadULong(&pc_Buffer);
#if defined(XML_CONV_TOOL)
	g_ul_NetworksGroupKey = ul_GroupKey;
#else
	if(ul_GroupKey && ul_GroupKey != BIG_C_InvalidKey)
	{
		LOA_MakeFileRef
		(
			ul_GroupKey,
			(ULONG *) &pst_World->pst_AllNetworks,
			WAY_ul_AllNetWorksCallback,
			LOA_C_MustExists
		);
	}
#endif // defined(XML_CONV_TOOL)

	/* World text */
	ul_GroupKey = LOA_ReadULong(&pc_Buffer);
	if(ul_GroupKey && ul_GroupKey != -1)
	{
		ul_Length = BIG_ul_SearchKeyToPos(ul_GroupKey);
		if(ul_Length != -1)
		{
			if(WOR_gul_WorldTextSize)
                WOR_gaul_WorldText = (ULONG*)MEM_p_Realloc(WOR_gaul_WorldText, (WOR_gul_WorldTextSize+1)*sizeof(BIG_KEY));
            else
                WOR_gaul_WorldText = (ULONG*)MEM_p_Alloc(sizeof(BIG_KEY));
            
#if defined(XML_CONV_TOOL)
			WOR_gaul_WorldText[0] = ul_GroupKey;
#else
			WOR_gaul_WorldText[WOR_gul_WorldTextSize] = ul_GroupKey;
#endif
			WOR_gul_WorldTextSize++;
		}
	}

	/* Sectorisation */
	if(version > 3)
	{
		for(i = 0; i < WOR_C_MaxSecto; i++)
		{
			pst_World->ast_AllSectos[i].ul_Flags = LOA_ReadULong(&pc_Buffer);
#if !defined(XML_CONV_TOOL)
			pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorMerged;
			pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorActive;
			pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorVisible;
#endif
			LOA_ReadCharArray(&pc_Buffer, (CHAR*)&pst_World->ast_AllSectos[i].auc_RefVis[0], WOR_C_MaxSectoRef);
			LOA_ReadCharArray(&pc_Buffer, (CHAR*)&pst_World->ast_AllSectos[i].auc_RefAct[0], WOR_C_MaxSectoRef);
			LOA_ReadCharArray_Ed(&pc_Buffer, &pst_World->ast_AllSectos[i].az_Name[0], WOR_C_MaxLenNameSecto);
			for(j = 0; j < WOR_C_MaxSectoPortals; j++)
			{
				pst_World->ast_AllSectos[i].ast_Portals[j].uw_Flags = LOA_ReadUShort(&pc_Buffer);
				pst_World->ast_AllSectos[i].ast_Portals[j].uc_ShareSect = LOA_ReadUChar(&pc_Buffer);
				pst_World->ast_AllSectos[i].ast_Portals[j].uc_SharePortal = LOA_ReadUChar(&pc_Buffer);
				LOA_ReadVector(&pc_Buffer, &pst_World->ast_AllSectos[i].ast_Portals[j].vA);
				LOA_ReadVector(&pc_Buffer, &pst_World->ast_AllSectos[i].ast_Portals[j].vB);
				LOA_ReadVector(&pc_Buffer, &pst_World->ast_AllSectos[i].ast_Portals[j].vC);
				LOA_ReadVector(&pc_Buffer, &pst_World->ast_AllSectos[i].ast_Portals[j].vD);
				LOA_ReadCharArray_Ed(&pc_Buffer, &pst_World->ast_AllSectos[i].ast_Portals[j].az_Name[0], WOR_C_MaxLenNamePortal);
			}
		}
	}
	else
	{
		L_memset(pst_World->ast_AllSectos, 0, sizeof(pst_World->ast_AllSectos));
	}

#ifdef JADEFUSION

	// ----------------------------------------------------------------------------------
	// Light rejection loaded in version 5 of wow ( see WOR_Cl_WorldStructVersion )	
	if ( version > 4 )	
	{
		// Read light rejection list 
		BIG_KEY ulLRLKey = LOA_ReadULong(&pc_Buffer);

#if defined(XML_CONV_TOOL)
		g_ul_LightRejectionKey = ulLRLKey;
#else
		// Read light rejection list 
		if((ulLRLKey != BIG_C_InvalidKey))
		{
          #ifdef ACTIVE_EDITORS
            // Verify that Light Rejection List corresponds to proper WOW
			BIG_INDEX ulLRLIndex = BIG_ul_SearchKeyToFat(ulLRLKey);
			BIG_INDEX ulWowIndex = BIG_ul_SearchKeyToFat(pst_World->h_WorldKey);
			if(ulLRLIndex != BIG_C_InvalidIndex && ulWowIndex != BIG_C_InvalidIndex)
			{
				char* szMark = strchr(BIG_NameFile(ulLRLIndex), '.');			
				char* szMark2 = strchr(pst_World->sz_Name, '.');			
				ULONG ulLength = szMark - BIG_NameFile(ulLRLIndex);
				ULONG ulLength2 = szMark2 - pst_World->sz_Name;
				
				if((ulLength==ulLength2) && (0 == strnicmp(BIG_NameFile(ulLRLIndex), BIG_NameFile(LRL_GetWorldKey()), ulLength))) // load only main LRL (ie for G61.wol, load GP61.lrl and no other)
				{
					// Load LRL list
					LRL_Load(ulLRLKey);	
				}
				else
				{
					ERR_X_Warning(0, ERR_szFormatMessage("Skipped LRL loading of world %s (%s [%08X] is invalid for this world)", pst_World->sz_Name, BIG_NameFile(ulLRLIndex), ulLRLKey), NULL);
				}
			}
			else
			{	
					ERR_X_Warning(0, ERR_szFormatMessage("LRLLoad: LRL key [%08X] for %s is not present in BF. It can't be loaded.", ulLRLKey, pst_World->sz_Name), NULL);
			}

          #else
			// Load LRL list
		    LRL_Load(ulLRLKey);	
          #endif
		}
#endif // defined(XML_CONV_TOOL)
	}
	// ----------------------------------------------------------------------------------
#endif

	return (ULONG) pst_World;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG WOR_ul_GameObjectGroupCallback(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR			*pc_Buffer, *pc_BufferInit;
	ULONG			ul_Size;
	TAB_tdst_PFelem *pst_Elem;
	ULONG			ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Buffer = pc_BufferInit = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);

	/* Resize all world object table to ensure all object fit in */
	TAB_PFtable_Resize(&WOR_gpst_WorldToLoadIn->st_AllWorldObjects, ul_Size);

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~*/
		BAS_tdst_barray Seen;
		char			az[512];
		BIG_INDEX		ul_Fat;
		ULONG			*pul_Key;
		/*~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
        clGroup	*pGOL = new clGroup( WOR_gpst_WorldToLoadIn->h_WorldKey );
		BAS_binit(&pGOL->m_groupArray, 100);
#else
		BAS_binit(&Seen, 100);
#endif
		while(pc_Buffer < (pc_BufferInit+ul_Size))
		{
#ifdef JADEFUSION
			pul_Key = (ULONG*) pc_Buffer;
#else
			pul_Key = (LONG*) pc_Buffer;
#endif
			_LOA_ReadLong(&pc_Buffer, (LONG*)&ul_Key, LOA_eBinLookAheadData);
			_LOA_ReadLong(&pc_Buffer, NULL, LOA_eBinLookAheadData); // Skip the group element type
			if(!ul_Key) continue;
#ifdef JADEFUSION			
			if(BAS_bsearch(ul_Key, &pGOL->m_groupArray) != -1)
#else
			if(BAS_bsearch(ul_Key, &Seen) != -1)
#endif
			{
				ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
				if(ul_Fat != BIG_C_InvalidIndex)
					sprintf(az, "Object %s is twice in .gol of %s", BIG_NameFile(ul_Fat), WOR_gpst_WorldToLoadIn->sz_Name);
				else
					sprintf(az, "Object 0x%x is twice in .gol of %s, and key seems to not exist !!", ul_Key, WOR_gpst_WorldToLoadIn->sz_Name);
				ERR_X_Warning(0, az, NULL);
				*pul_Key = 0;
			}
#ifdef JADEFUSION
			BAS_binsert(ul_Key, ul_Key, &pGOL->m_groupArray);
		}
		pc_Buffer = pc_BufferInit;

        delete pGOL;
	}
#else
			BAS_binsert(ul_Key, ul_Key, &Seen);
		}
		BAS_bfree(&Seen);
		pc_Buffer = pc_BufferInit;
	}
#endif
#endif
	while(pc_Buffer < (pc_BufferInit+ul_Size))
	{
		ul_Key = LOA_ReadLong(&pc_Buffer);
		LOA_ReadLong_Ed(&pc_Buffer, NULL); // Skip group element type
		if(!ul_Key) continue;

		TAB_PFtable_AddElemWithData(&WOR_gpst_WorldToLoadIn->st_AllWorldObjects, 0, 0);
		pst_Elem = TAB_pst_PFtable_GetLastElem(&WOR_gpst_WorldToLoadIn->st_AllWorldObjects);

		LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Elem->p_Pointer, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
	}

	return 0;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
