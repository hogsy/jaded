/*$T MATstruct.c GC! 1.081 06/21/00 15:58:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"

#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"

#include "GDInterface/GDInterface.h"

#ifndef PSX2_TARGET
#ifndef _GAMECUBE
#ifdef ACTIVE_EDITORS
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#include "EDIpaths.h"
#endif
#endif
#endif

#include "MATerial/MATmultitexture.h"
#include "MATerial/MATSingle.h"
#include "MATerial/MATmulti.h"
#include "MATerial/MATSprite.h"
#include "MATerial/MATstruct.h"

#ifdef JADEFUSION
#include "TEXture/TEXcubemap.h"
#include "TEXture/texfile.h"
#include "LIGHT/LIGHTmap.h"
#endif

#include "GRObject/GROstruct.h"

#include "GEOmetric/GEODebugObject.h"
#ifdef PSX2_TARGET
#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>
#include "GS_PS2/Gsp.h"
#ifdef GSP_PS2_BENCH
extern u_int NoGEODRAW;
#endif
#endif

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeUtils.h"
#include "XenonGraphics/XeLightShaftManager.h"
#include "XenonGraphics/XeShadowManager.h"
#endif

#include "BASe/BENch/BENch.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
MAT_tdst_MultiTexture MAT_gst_DefaultSingleMaterial =
{
#ifdef ACTIVE_EDITORS
	{ NULL, NULL },
#else
	{ NULL },
#endif
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0.0f,
	1.0f,
	0,
	0,
	0xFFFFFFFF
#if defined(_XENON_RENDER)
    , NULL
#endif
};

MAT_tdst_MTLevel MAT_gst_DefaultMatMTLevel =
{
    -1, 0, 0, 0, 0, 0
#if defined(_XENON_RENDER)
    , NULL
    , NULL
#endif // defined(_XENON_RENDER)
};

/*$F
 =======================================================================================================================
 avant on avait en éditeur 
 
 #ifdef ACTIVE_EDITORS
	ULONG ReturnValue;
	ReturnValue = MAT_ValidateMask_RLI | MAT_ValidateMask_Diffuse	| MAT_ValidateMask_Specular_Color;
	if (MAT_IsMaterialTransparent(pst_GRO, Submat, 0, _pst_Obj ))
	{
		ReturnValue |= MAT_ValidateMask_Transparency_Or;
	}
	return ReturnValue ;
#else

 =======================================================================================================================
 */
ULONG MAT_GetValidityMASK(MAT_tdst_Material *pst_GRO, LONG Submat, GEO_tdst_Object *_pst_Obj )
{
    ULONG                               Mask, ValidatedMask , ValidatedMaskAnd;
    GEO_tdst_ElementIndexedTriangles    *p_Elements, *p_ElementsLast;

    if(pst_GRO == NULL) return  MAT_ValidateMask_Diffuse | MAT_ValidateMask_RLI; // Default material is not transparent...

	if ( (pst_GRO->st_Id.i->ul_Type == GRO_MaterialMulti) && _pst_Obj && (Submat == -1))
    {
        ValidatedMask = 0;
		ValidatedMaskAnd = 0xffffffff;
		
        p_Elements = _pst_Obj->dst_Element;
		p_ElementsLast = p_Elements + _pst_Obj->l_NbElements;
		while (p_Elements < p_ElementsLast)
		{
			if(pst_GRO->st_Id.i->pfn_User_0)
				Mask = pst_GRO->st_Id.i->pfn_User_0((GRO_tdst_Struct *)pst_GRO , p_Elements -> l_MaterialId);
			else
				Mask = 0;
			ValidatedMask |= Mask;
			ValidatedMaskAnd &= Mask;
			p_Elements++;
        }
		if (_pst_Obj->l_NbSpritesElements)
		{
			GEO_tdst_ElementIndexedSprite		*p_ElementsSpr,*p_ElementsSprLast;
	        p_ElementsSpr = _pst_Obj->dst_SpritesElements;
			p_ElementsSprLast = p_ElementsSpr + _pst_Obj->l_NbSpritesElements;
			while (p_ElementsSpr < p_ElementsSprLast)
			{
				if(pst_GRO->st_Id.i->pfn_User_0)
					Mask = pst_GRO->st_Id.i->pfn_User_0((GRO_tdst_Struct *)pst_GRO , p_ElementsSpr -> l_MaterialId);
				else
					Mask = 0;
				ValidatedMask |= Mask;
				ValidatedMaskAnd &= Mask;
				p_ElementsSpr++;
	        }
        }
		
        ValidatedMask &= ~MAT_ValidateMask_Transparency_And;
		ValidatedMask |= ValidatedMaskAnd & MAT_ValidateMask_Transparency_And;
		return ValidatedMask;
	}
	else
		return pst_GRO->st_Id.i->pfn_User_0((GRO_tdst_Struct *)pst_GRO , Submat);
}

ULONG MAT_GetFirstTransparentMaterialIndex(MAT_tdst_Material *pst_GRO, GEO_tdst_Object *_pst_Obj )
{
    ULONG                               Mask;
    GEO_tdst_ElementIndexedTriangles    *p_Elements, *p_ElementsLast;

    if(pst_GRO == NULL) return (ULONG)-1; // Default material is not transparent...

	if ( (pst_GRO->st_Id.i->ul_Type == GRO_MaterialMulti) && _pst_Obj)
    {
		
        p_Elements = _pst_Obj->dst_Element;
		p_ElementsLast = p_Elements + _pst_Obj->l_NbElements;
		while (p_Elements < p_ElementsLast)
		{
			if(pst_GRO->st_Id.i->pfn_User_0)
				Mask = pst_GRO->st_Id.i->pfn_User_0((GRO_tdst_Struct *)pst_GRO , p_Elements -> l_MaterialId);
			else
				Mask = 0;
            if (Mask & MAT_ValidateMask_Transparency)
                return p_Elements -> l_MaterialId;
			p_Elements++;
        }
		/*if (_pst_Obj->l_NbSpritesElements)
		{
			GEO_tdst_ElementIndexedSprite		*p_ElementsSpr,*p_ElementsSprLast;
	        p_ElementsSpr = _pst_Obj->dst_SpritesElements;
			p_ElementsSprLast = p_ElementsSpr + _pst_Obj->l_NbSpritesElements;
			while (p_ElementsSpr < p_ElementsSprLast)
			{
				if(pst_GRO->st_Id.i->pfn_User_0)
					Mask = pst_GRO->st_Id.i->pfn_User_0((GRO_tdst_Struct *)pst_GRO , p_ElementsSpr -> l_MaterialId);
				else
					Mask = 0;
                if (Mask & MAT_ValidateMask_Transparency)
                    return p_Elements -> l_MaterialId;
				p_ElementsSpr++;
	        }
        }*/
		
		return (ULONG)-1;
	}
	else
    {
		if (MAT_ValidateMask_Transparency & pst_GRO->st_Id.i->pfn_User_0((GRO_tdst_Struct *)pst_GRO , -1))
            return 0;
        else
            return (ULONG)-1;
    }
    return (ULONG)-1;
}

#define MAT_M_DetectTransparencyFromColor(MAT) \
	{ \
		if(MAT->f_Opacity < 0.98f) return TRUE; \
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL MAT_IsMaterialTransparent(MAT_tdst_Material *pst_GRO, LONG Submat, ULONG ulCurrentDrawMask, GEO_tdst_Object *_pst_Obj )
{
#ifndef ACTIVE_EDITORS
/* 
--------------------------------------------------------------------------------------------------------------------------------
Methode relative... On renvoie le flag de validitée du mnatériau
Cette methode peut bugger. Pour tester vire le #if 1 pour un #if 0
--------------------------------------------------------------------------------------------------------------------------------
*/
	if(pst_GRO == NULL) return FALSE; // Default material is not transparent...
	if (pst_GRO->st_Id.i->ul_Type == GRO_MaterialMulti)
	{
		ULONG ValidatedMask;
		ValidatedMask = pst_GRO->st_Id.i->pfn_User_0((GRO_tdst_Struct *)pst_GRO , Submat);
		if (ValidatedMask & MAT_ValidateMask_Transparency_And) return TRUE;
		if (!(ValidatedMask & MAT_ValidateMask_Transparency_Or)) return FALSE;
		if (_pst_Obj && (Submat == -1))
		{
		    int             i_Element;
		    GEO_tdst_ElementIndexedTriangles *p_Elements;
			ValidatedMask = 0;
			i_Element = _pst_Obj->l_NbElements;
			p_Elements = _pst_Obj->dst_Element;
			while (i_Element--)
			{
				ValidatedMask |= pst_GRO->st_Id.i->pfn_User_0((GRO_tdst_Struct *)pst_GRO , p_Elements -> l_MaterialId);
				if (ValidatedMask & MAT_ValidateMask_Transparency) return TRUE;
				p_Elements++;
			}
			return FALSE;
		} else
		{
		if (ValidatedMask & MAT_ValidateMask_Transparency)
			return TRUE;
		else
			return FALSE;
		}
	}
	else
	{
		if ((pst_GRO->st_Id.i->pfn_User_0((GRO_tdst_Struct *)pst_GRO , Submat)) & MAT_ValidateMask_Transparency)
			return TRUE;
		else
			return FALSE;
	}

#else 
/* 
--------------------------------------------------------------------------------------------------------------------------------
Methode Absolue... Tous les sous-materiaux sont parsé 1 par 1
--------------------------------------------------------------------------------------------------------------------------------
*/
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_Data	        *pst_TexData;
	MAT_tdst_Single         *pst_SMat;
    ULONG	                ulBldngMde;
    MAT_tdst_MultiTexture	*pst_MltTxt;
    MAT_tdst_Multi		    *pst_MMat;
    MAT_tdst_Material	    **pst_Bgin, **pst_Last;
    int                     i_Element;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_GRO == NULL) pst_GRO = (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

	switch(pst_GRO->st_Id.i->ul_Type)
	{
	case GRO_MaterialSingle:
		pst_SMat = (MAT_tdst_Single *) pst_GRO;
		MAT_M_DetectTransparencyFromColor(pst_SMat);
		if(pst_SMat->l_TextureId != -1)
		{
			pst_TexData = &TEX_gst_GlobalList.dst_Texture[pst_SMat->l_TextureId];
			if(pst_TexData->uw_Flags & TEX_uw_Alpha) return TRUE;
		}
		break;
	case GRO_MaterialMultiTexture:
		pst_MltTxt = (MAT_tdst_MultiTexture *) pst_GRO;
		if(pst_MltTxt->pst_FirstLevel)
		{
			ulBldngMde = MAT_GET_Blending(pst_MltTxt->pst_FirstLevel->ul_Flags);
			if((ulBldngMde != MAT_Cc_Op_Copy) && (ulBldngMde != MAT_Cc_Op_Glow)) return TRUE;
			if(pst_MltTxt->pst_FirstLevel->ul_Flags & MAT_Cul_Flag_HideColor) return TRUE;
		}
		break;
	case GRO_MaterialMulti:
		pst_MMat = (MAT_tdst_Multi *) pst_GRO;
		if(Submat == -1)
		{
            if (_pst_Obj)
            {
				if(pst_MMat->l_NumberOfSubMaterials)
				{
					pst_Bgin = pst_MMat->dpst_SubMaterial;
					for (i_Element = 0; i_Element < _pst_Obj->l_NbElements; i_Element++)
					{
						pst_Last = pst_Bgin + lMin(_pst_Obj->dst_Element[ i_Element ].l_MaterialId , pst_MMat->l_NumberOfSubMaterials - 1 );
						if (MAT_IsMaterialTransparent(*pst_Last, -1, ulCurrentDrawMask, NULL)) return TRUE;
					}
				}
            }
            else
            {
    			pst_Bgin = pst_MMat->dpst_SubMaterial;
	    		pst_Last = pst_Bgin + pst_MMat->l_NumberOfSubMaterials;
		    	while(pst_Bgin < pst_Last)
			    {
				    if((*pst_Bgin) && ((*pst_Bgin)->st_Id.i->ul_Type != GRO_MaterialMulti))
				    {
					    if(MAT_IsMaterialTransparent(*pst_Bgin, Submat, ulCurrentDrawMask, NULL)) return TRUE;
				    }
				    pst_Bgin++;
			    }
            }
		}
		else
		{
			return
				(
					MAT_IsMaterialTransparent
					(
						pst_MMat->dpst_SubMaterial[lMin(Submat , pst_MMat->l_NumberOfSubMaterials - 1)],
						-1,
						ulCurrentDrawMask,
                        NULL
					)
				);
		}
    	break;
	}

	return FALSE;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_Material *MAT_pst_CreateMaterial(LONG _l_Type, char *_sz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Material	*pst_Material;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Material = NULL;

	switch(_l_Type)
	{
	case GRO_MaterialSingle:
		pst_Material = (MAT_tdst_Material *) MEM_p_Alloc(sizeof(MAT_tdst_Single));
		L_memset(pst_Material, 0, sizeof(MAT_tdst_Single));
		break;
	case GRO_MaterialMultiTexture:
		pst_Material = (MAT_tdst_Material *) MEM_p_Alloc(sizeof(MAT_tdst_MultiTexture));
		L_memset(pst_Material, 0, sizeof(MAT_tdst_MultiTexture));
		break;
	case GRO_MaterialMulti:
		pst_Material = (MAT_tdst_Material *) MEM_p_Alloc(sizeof(MAT_tdst_Multi));
		L_memset(pst_Material, 0, sizeof(MAT_tdst_Multi));
		break;
	}

	GRO_Struct_Init(&pst_Material->st_Id, _l_Type);
	GRO_Struct_SetName(&pst_Material->st_Id, _sz_Name);

	return pst_Material;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_DestroyMaterial(MAT_tdst_Material *_pst_Material)
{
	if(_pst_Material == NULL) return;
	if(_pst_Material->st_Id.l_Ref > 0) return;
    ERR_X_Assert(_pst_Material->st_Id.l_Ref == 0);

	MAT_FreeMultiMaterial(_pst_Material);
	MAT_FreeMultiTexture(_pst_Material);
	GRO_Struct_Free(&_pst_Material->st_Id);

	/* Delete pointer from table */
	LOA_DeleteAddress(_pst_Material);

	MEM_Free(_pst_Material);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_SetRenderState(MAT_tdst_Material *_pst_Material, LONG _l_SubMaterial)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi			*pst_MMat;
	MAT_tdst_MultiTexture	*pst_MTMat;
	ULONG					 flag;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor = 0;

	if(_pst_Material == NULL) _pst_Material = (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

	if(_pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti)
	{
		pst_MMat = (MAT_tdst_Multi *) _pst_Material;
		_l_SubMaterial = lMin(_l_SubMaterial , pst_MMat->l_NumberOfSubMaterials -1 );
		pst_MTMat = (MAT_tdst_MultiTexture *) pst_MMat->dpst_SubMaterial[_l_SubMaterial];
		if(!pst_MTMat) pst_MTMat = &MAT_gst_DefaultSingleMaterial;
	}
	else
	{
		pst_MTMat = (MAT_tdst_MultiTexture *) _pst_Material;
	}

	if(pst_MTMat->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
	{
		if(pst_MTMat->pst_FirstLevel && (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_UseTexture))
		{
			if(pst_MTMat->pst_FirstLevel->ul_Flags & MAT_Cul_Flag_UseLocalAlpha)
			{
				GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor |= (((ULONG) (pst_MTMat->pst_FirstLevel->s_AditionalFlags)) << 16) & 0xff000000;
				GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor ^= 0xff000000;
			}

			GDI_SetTextureBlending((*GDI_gpst_CurDD), pst_MTMat->pst_FirstLevel->s_TextureId, pst_MTMat->pst_FirstLevel->ul_Flags, pst_MTMat->pst_FirstLevel->s_AditionalFlags);
		}
		else
			GDI_SetTextureBlending((*GDI_gpst_CurDD),(ULONG) - 1, MAT_C_DefaultFlag ,0 );
	} else
	if(pst_MTMat->st_Id.i->ul_Type == GRO_MaterialSingle)
	{
		flag = MAT_C_DefaultFlag;
		if(pst_MTMat->f_Opacity <= .98f)
			MAT_SET_Blending(flag, MAT_Cc_Op_Alpha);
		else
			MAT_SET_Blending(flag, MAT_Cc_Op_Copy);

		if(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_UseTexture)
			GDI_SetTextureBlending((*GDI_gpst_CurDD),((MAT_tdst_Single *) pst_MTMat)->l_TextureId, flag ,0);
		else
			GDI_SetTextureBlending((*GDI_gpst_CurDD),(ULONG) - 1, flag, 0); 
	} 

	GDI_gpst_CurDD->pst_CurrentMat = pst_MTMat;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if defined(ACTIVE_EDITORS)
char *MAT_sz_FileExtension(void)
{
	return EDI_Csz_ExtGraphicMaterial;
}
#endif // defined(ACTIVE_EDITORS)

ULONG MAT_GetValidatedMask_MSM(GRO_tdst_Struct *p_Gro , ULONG P0)
{
	if (P0 == -1)
	{
		return ((MAT_tdst_Multi *)p_Gro)->ul_ValidateMask;
	}
	else
	{
		P0 = lMin(P0 , ((MAT_tdst_Multi *)p_Gro)->l_NumberOfSubMaterials - 1);
        if (((MAT_tdst_Multi *)p_Gro)->dpst_SubMaterial[P0] == NULL)
		{
#ifdef ACTIVE_EDITORS
            ULONG ulKey = LOA_ul_SearchKeyWithAddress((ULONG)p_Gro);
            char sMsg[256];
            sprintf(sMsg,"Sous Matériau invalide détecté (%08x) : ",ulKey);
			ERR_X_Warning(0, sMsg, p_Gro->sz_Name ? p_Gro->sz_Name : "Unknown");
#endif			
            return 0;
		}
		return ((MAT_tdst_Multi *)p_Gro)->dpst_SubMaterial[P0]->st_Id.i->pfn_User_0((GRO_tdst_Struct *)((MAT_tdst_Multi *)p_Gro)->dpst_SubMaterial[P0] , 0);
	}
}

ULONG MAT_GetValidatedMask_SIN(GRO_tdst_Struct *p_Gro , ULONG P0)
{
	return ((MAT_tdst_Single *)p_Gro)->ul_ValidateMask;
}

ULONG MAT_GetValidatedMask_MTT(GRO_tdst_Struct *p_Gro , ULONG P0)
{
	return ((MAT_tdst_MultiTexture *)p_Gro)->ul_ValidateMask;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Interface	*i;
	/*~~~~~~~~~~~~~~~~~~~*/
	
	MAT_Validate_Multitexture(&MAT_gst_DefaultSingleMaterial);

#if defined(PSX2_TARGET) || defined(_GAMECUBE)
	i = &GRO_gast_Interface[GRO_MaterialSingle];

#ifdef __CW__
	i->pfnp_CreateFromBuffer = MAT_pst_CreateSingleFromBuffer;
#else
	i->pfnp_CreateFromBuffer = (void *(*) (GRO_tdst_Struct_ *, char **, void *)) MAT_pst_CreateSingleFromBuffer;
#endif
	i->pfn_Destroy = MAT_DestroyMaterial;
	i->pfnp_Duplicate = MAT_p_SingleDuplicate;
	i->pfn_User_0 = MAT_GetValidatedMask_SIN;
	

	i = &GRO_gast_Interface[GRO_MaterialMulti];
#ifdef __CW__
	i->pfnp_CreateFromBuffer = MAT_pst_CreateMultiFromBuffer;
#else
	i->pfnp_CreateFromBuffer = (void *(*) (GRO_tdst_Struct_ *, char **, void *)) MAT_pst_CreateMultiFromBuffer;
#endif
	i->pfn_Destroy = MAT_DestroyMaterial;
	i->pfn_User_0 = MAT_GetValidatedMask_MSM;

	i = &GRO_gast_Interface[GRO_MaterialMultiTexture];
#ifdef __CW__
	i->pfnp_CreateFromBuffer = MAT_pst_CreateMultiTextureFromBuffer;
#else
	i->pfnp_CreateFromBuffer = (void *(*) (GRO_tdst_Struct_ *, char **, void *)) MAT_pst_CreateMultiTextureFromBuffer;
#endif
	i->pfn_Destroy = MAT_DestroyMaterial;
	i->pfnp_Duplicate = MAT_p_MultiTextureDuplicate;
	i->pfn_User_0 = MAT_GetValidatedMask_MTT;

#else /* PSX2_TARGET */
	i = &GRO_gast_Interface[GRO_MaterialSingle];

    i->pfnp_CreateDefault = MAT_p_CreateSingleDefault;
	i->pfnp_CreateFromBuffer = (void *(__cdecl *)(GRO_tdst_Struct *,char ** ,void *))MAT_pst_CreateSingleFromBuffer;
    i->pfnp_Duplicate = (void *(__cdecl *)(void *,char *,char *,ULONG))MAT_p_SingleDuplicate;
	i->pfn_Destroy = (void (__cdecl *)(void *))MAT_DestroyMaterial;
	i->pfn_User_0 = MAT_GetValidatedMask_SIN;

#ifdef ACTIVE_EDITORS
	i->pfnl_SaveInBuffer = (LONG (__cdecl *)(void *,void *))MAT_l_SaveSingleInBuffer;
	i->pfnp_CreateFromMad = (void* (__cdecl *)(void *))MAT_p_CreateSingleFromMad;
	i->pfnp_ToMad = (void* (__cdecl *)(void *,void *))MAT_p_SingleToMad;
	i->pfnsz_FileExtension = MAT_sz_FileExtension;
#endif

	i = &GRO_gast_Interface[GRO_MaterialMulti];
    i->pfnp_CreateDefault = MAT_p_CreateMultiDefault;

	i->pfnp_Duplicate = (void*(__cdecl*)(void*,char*,char *,ULONG))MAT_p_MultiDuplicate;
	i->pfnp_CreateFromBuffer = (void *(__cdecl *)(GRO_tdst_Struct *,char ** ,void *))MAT_pst_CreateMultiFromBuffer;
	i->pfn_Destroy = (void (__cdecl *)(void *))MAT_DestroyMaterial;
	i->pfn_User_0 = MAT_GetValidatedMask_MSM;
#ifdef ACTIVE_EDITORS
	i->pfnl_SaveInBuffer = (LONG (__cdecl *)(void *,void *))MAT_l_SaveMultiInBuffer;
	i->pfnp_CreateFromMad = (void* (__cdecl *)(void *))MAT_p_CreateMultiFromMad;
	i->pfnp_ToMad = (void* (__cdecl *)(void *,void *))MAT_p_MultiToMad;
	i->pfnsz_FileExtension = MAT_sz_FileExtension;
#endif

	i = &GRO_gast_Interface[GRO_MaterialMultiTexture];
	i->pfnp_CreateFromBuffer = (void *(__cdecl *)(GRO_tdst_Struct *,char ** ,void *))MAT_pst_CreateMultiTextureFromBuffer;
    i->pfnp_Duplicate = (void *(__cdecl *)(void *,char *,char *,ULONG))MAT_p_MultiTextureDuplicate;
	i->pfn_Destroy = (void (__cdecl *)(void *))MAT_DestroyMaterial;

	i->pfn_User_0 = MAT_GetValidatedMask_MTT;
#ifdef ACTIVE_EDITORS
	i->pfnl_SaveInBuffer = (LONG (__cdecl *)(void *,void *))MAT_l_SaveMultiTextureInBuffer;
	i->pfnp_CreateFromMad = (void* (__cdecl *)(void *))MAT_p_CreateMultiTextureFromMad;
	i->pfnp_ToMad = (void* (__cdecl *)(void *,void *))MAT_p_MultiTextureToMad;
	i->pfnsz_FileExtension = MAT_sz_FileExtension;
#endif

#endif /* not PSX2_TARGET */

    i = &GRO_gast_Interface[GRO_MaterialMultiTexture];
	MAT_gst_DefaultSingleMaterial.st_Id.i = i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_TestUsedForMat(GRO_tdst_Struct *_pst_GRO, char *pc_UsedIndex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				i;
	MAT_tdst_Multi		*pst_MMat;
	MAT_tdst_Material	**pst_Begin, **pst_Last;
	MAT_tdst_MTLevel	*pst_MTLTXLVL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GRO->i->ul_Type == GRO_MaterialSingle)
	{
		i = ((MAT_tdst_Single *) _pst_GRO)->l_TextureId;
		if(i != -1) pc_UsedIndex[i] = 1;
		return;
	}

	if(_pst_GRO->i->ul_Type == GRO_MaterialMulti)
	{
		pst_MMat = (MAT_tdst_Multi *) _pst_GRO;
		pst_Begin = pst_MMat->dpst_SubMaterial;
		pst_Last = pst_Begin + pst_MMat->l_NumberOfSubMaterials;
		while(pst_Begin < pst_Last)
		{
            if ( (*pst_Begin) && (*pst_Begin)->st_Id.i->ul_Type != GRO_MaterialMulti)
                MAT_TestUsedForMat(&(*pst_Begin)->st_Id, pc_UsedIndex);
			pst_Begin++;
		}

		return;
	}

	if(_pst_GRO->i->ul_Type == GRO_MaterialMultiTexture)
	{
		pst_MTLTXLVL = ((MAT_tdst_MultiTexture *) _pst_GRO)->pst_FirstLevel;
		while(pst_MTLTXLVL != NULL)
		{
			if(pst_MTLTXLVL->s_TextureId != -1)
			{
                pc_UsedIndex[pst_MTLTXLVL->s_TextureId] = 1;
            }
#if defined(_XENON_RENDER)
            if (pst_MTLTXLVL->pst_XeLevel != NULL)
            {
                if (pst_MTLTXLVL->pst_XeLevel->l_SpecularMapId != MAT_Xe_InvalidTextureId)
                {
                    pc_UsedIndex[pst_MTLTXLVL->pst_XeLevel->l_SpecularMapId] = 1;
                }

                if (pst_MTLTXLVL->pst_XeLevel->l_NormalMapId != MAT_Xe_InvalidTextureId)
                {
                    pc_UsedIndex[pst_MTLTXLVL->pst_XeLevel->l_NormalMapId] = 1;
                }

                if (pst_MTLTXLVL->pst_XeLevel->l_DetailNMapId != MAT_Xe_InvalidTextureId)
                {
                    pc_UsedIndex[pst_MTLTXLVL->pst_XeLevel->l_DetailNMapId] = 1;
                }

                if (pst_MTLTXLVL->pst_XeLevel->l_MossMapId != MAT_Xe_InvalidTextureId)
                {
                    pc_UsedIndex[pst_MTLTXLVL->pst_XeLevel->l_MossMapId] = 1;
                }
            }
#endif // defined(_XENON_RENDER)

			pst_MTLTXLVL = pst_MTLTXLVL->pst_NextLevel;
		}
	}
}

extern void GAO_ModifierShadow_DetectUsedTextures(char *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *MAT_pc_GetAllUsedTextureIndex(GDI_tdst_DisplayData *_pst_DD, LONG ul_NbIndex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*pc_UsedIndex;
	TAB_tdst_Ptable		*pst_Table;
	MAT_tdst_Material	**ppst_First, **ppst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_UsedIndex = (char *) MEM_p_Alloc(ul_NbIndex);
	L_memset(pc_UsedIndex, 0, ul_NbIndex);

	pst_Table = &_pst_DD->pst_World->st_GraphicMaterialsTable;
	ppst_First = (MAT_tdst_Material **) TAB_ppv_Ptable_GetFirstElem(pst_Table);
	ppst_Last = (MAT_tdst_Material **) TAB_ppv_Ptable_GetLastElem(pst_Table);

	for(; ppst_First <= ppst_Last; ppst_First++)
	{
		if(!(*ppst_First)) continue;
		if(TAB_b_IsAHole(*ppst_First)) continue;
		MAT_TestUsedForMat(&(*ppst_First)->st_Id, pc_UsedIndex);
		if((*ppst_First)->st_Id.i->ul_Type == GRO_MaterialMulti) MAT_Validate_Multi((MAT_tdst_Multi *)*ppst_First);
	}

#if defined(_XENON_RENDER) && !defined(XML_CONV_TOOL)
    g_oXeLightShaftManager.GetUsedTextures(pc_UsedIndex);
    g_oXeShadowManager.GetUsedTextures(pc_UsedIndex);
#endif

	GAO_ModifierShadow_DetectUsedTextures(pc_UsedIndex);

#ifdef JADEFUSION
    LIGHT_Lightmaps_UsedTexture(pc_UsedIndex);
#endif
    GEO_DebugObject_UsedTexture(pc_UsedIndex);

#if defined(TEX_USE_CUBEMAPS)
    // Check the cube map list for used textures
    TEX_CubeMap_CheckUsedTextures(pc_UsedIndex);
#endif

	return pc_UsedIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_GetColorPtr(GRO_tdst_Struct *_pst_GRO, ULONG **Color, ULONG SubMatNum, ULONG TypeOfColor)
{
	*Color = NULL;

	if(_pst_GRO->i->ul_Type == GRO_MaterialMulti)
	{
		_pst_GRO = (GRO_tdst_Struct *) ((MAT_tdst_Multi *) _pst_GRO)->dpst_SubMaterial[lMin(SubMatNum , ((MAT_tdst_Multi *) _pst_GRO)->l_NumberOfSubMaterials - 1)];
	}

	if(_pst_GRO->i->ul_Type == GRO_MaterialMulti) return;

	if(_pst_GRO->i->ul_Type == GRO_MaterialSingle)
	{
		switch(TypeOfColor)
		{
		case 0:
			*Color = &((MAT_tdst_Single *) _pst_GRO)->ul_Diffuse;
			break;
		case 1:
			*Color = &((MAT_tdst_Single *) _pst_GRO)->ul_Specular;
			break;
		case 2:
			*Color = &((MAT_tdst_Single *) _pst_GRO)->ul_Ambiant;
			break;
		}

		return;
	}

	if(_pst_GRO->i->ul_Type == GRO_MaterialMultiTexture)
	{
		switch(TypeOfColor)
		{
		case 0:
			*Color = &((MAT_tdst_MultiTexture *) _pst_GRO)->ul_Diffuse;
			break;
		case 1:
			*Color = &((MAT_tdst_MultiTexture *) _pst_GRO)->ul_Specular;
			break;
		case 2:
			*Color = &((MAT_tdst_MultiTexture *) _pst_GRO)->ul_Ambiant;
			break;
		}

		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_GetMtxLvlPtr(GRO_tdst_Struct *_pst_GRO, MAT_tdst_MTLevel **MtxLvl, ULONG SubMatNum, ULONG Lvl)
{
	*MtxLvl = NULL;

	if(_pst_GRO->i->ul_Type == GRO_MaterialMulti)
	{
		_pst_GRO = (GRO_tdst_Struct *) ((MAT_tdst_Multi *) _pst_GRO)->dpst_SubMaterial[lMin(SubMatNum , ((MAT_tdst_Multi *) _pst_GRO)->l_NumberOfSubMaterials)];
	}

	if(_pst_GRO->i->ul_Type != GRO_MaterialMultiTexture) return;

	*MtxLvl = ((MAT_tdst_MultiTexture *) _pst_GRO)->pst_FirstLevel;
	while((*MtxLvl != NULL) && (Lvl))
	{
		*MtxLvl = (*MtxLvl)->pst_NextLevel;
		Lvl--;
	}
}
#ifdef JADEFUSION
MAT_tdst_Material* MAT_GetElementMaterial(GRO_tdst_Visu* _pst_Visu, GEO_tdst_ElementIndexedTriangles* _pst_Element)
{
    if (!_pst_Visu || !_pst_Visu->pst_Material) return NULL;

    MAT_tdst_Material* pst_Material = (MAT_tdst_Material *) _pst_Visu->pst_Material;

    if(pst_Material  && (pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti))
    {
        if(((MAT_tdst_Multi *)pst_Material)->l_NumberOfSubMaterials == 0)
            pst_Material = NULL;
        else
            pst_Material = ((MAT_tdst_Multi *)pst_Material)->dpst_SubMaterial[lMin(_pst_Element->l_MaterialId , ((MAT_tdst_Multi *)pst_Material)->l_NumberOfSubMaterials - 1)];
    }

    if(pst_Material == NULL) pst_Material = (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

    return pst_Material;
}
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
