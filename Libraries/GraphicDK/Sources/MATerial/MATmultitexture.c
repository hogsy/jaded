/*$T MATmultitexture.c GC!1.55 12/30/99 18:05:47 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#ifndef PSX2_TARGET
#include "MAD_mem/Sources/MAD_mem.h"
#endif
#include "MATerial/MATstruct.h"
#include "MATerial/MATmultitexture.h"
#include "MATerial/MATSingle.h"
#include "GRObject/GROstruct.h"

#ifdef JADEFUSION
#include "GraphicDK/Sources/TEXture/TEXcubemap.h"
#if defined(_XENON_RENDER)
#include <vector>
#include "XenonGraphics/XeMaterial.h"
#endif
#if defined(XML_CONV_TOOL)
#include "../../../Tools/BFObjectXmlConv/XmlConvUtil.h"
#endif

#if defined(ACTIVE_EDITORS)
#include "BIGfiles/BIGfat.h"

extern ULONG LOA_gul_CurrentKey;
#endif

#if defined(_XENON_RENDER)
#define SC_DEV

// Current Xenon material properties version
const ULONG MAT_XE_MTLEVEL_VERSION = 17;

#endif

//#if defined(PSX2_TARGET) && defined(__cplusplus)
//extern "C" {
//#endif

/*
=======================================================================================================================
=======================================================================================================================
*/
#if defined(ACTIVE_EDITORS)
void MAT_CheckForTGAMapping(BIG_KEY _ul_TextureKey, const CHAR* _psz_Type)
{
    BIG_INDEX ulFileIndex;
    CHAR      szLowerCaseName[512];

    if ((_ul_TextureKey == 0) || (_ul_TextureKey == BIG_C_InvalidKey))
        return;

    ulFileIndex = BIG_ul_SearchKeyToFat(_ul_TextureKey);
    if (ulFileIndex == BIG_C_InvalidIndex)
        return;

    strcpy(szLowerCaseName, BIG_NameFile(ulFileIndex));
    strlwr(szLowerCaseName);

    if (strstr(szLowerCaseName, ".tga"))
    {
        CHAR szMsg[1024];

        sprintf(szMsg, "[Material] [%08x] is using a .TGA instead of a .TEX (%s) [%s]", LOA_gul_CurrentKey, szLowerCaseName, _psz_Type);
        ERR_X_Warning(FALSE, szMsg, NULL);
    }
}
#endif
#endif //jadefusion

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_MultiTexture *MAT_pst_CreateMultiTextureFromBuffer
(
    GRO_tdst_Struct *_pst_Id,
    char            **ppc_Buffer,
    WOR_tdst_World  *_pst_World
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_MultiTexture   *pst_Material;
    MAT_tdst_MTLevel        **pst_MLTTXLVL;
    TEX_tdst_Data           *pst_Texture;
    LONG                    Swap;   
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEMpro_StartMemRaster();
    pst_Material = (MAT_tdst_MultiTexture *) MEM_p_Alloc(sizeof(MAT_tdst_MultiTexture));
    L_memset(pst_Material, 0, sizeof(MAT_tdst_MultiTexture));

    GRO_Struct_Init(&pst_Material->st_Id, _pst_Id->i->ul_Type);
 

	pst_Material->ul_Ambiant = LOA_ReadULong(ppc_Buffer);
	pst_Material->ul_Diffuse = LOA_ReadULong(ppc_Buffer);
	pst_Material->ul_Specular = LOA_ReadULong(ppc_Buffer);
	pst_Material->f_SpecularExp = LOA_ReadFloat(ppc_Buffer);
	pst_Material->f_Opacity = LOA_ReadFloat(ppc_Buffer);
	pst_Material->ul_Flags = LOA_ReadULong(ppc_Buffer);
	pst_Material->pst_FirstLevel = (MAT_tdst_MTLevel *) LOA_ReadULong(ppc_Buffer);
	pst_Material->ul_ValidateMask = LOA_ReadULong(ppc_Buffer);

	/* Evil code to save/load new fields */
	if(*(ULONG *)&pst_Material->f_SpecularExp & 0x80000000) 
	{
		pst_Material->uc_Version = LOA_ReadUChar(ppc_Buffer);
		pst_Material->uc_Sound = LOA_ReadUChar(ppc_Buffer);
#if !defined(XML_CONV_TOOL)
		if(pst_Material->uc_Sound >= 9)
		{
			pst_Material->uc_Sound = 0;
		}
#endif
		pst_Material->uw_Dummy = LOA_ReadUShort(ppc_Buffer);
#if !defined(XML_CONV_TOOL)
		*(ULONG *)&pst_Material->f_SpecularExp &= ~0x80000000;
#endif
#ifdef JADEFUSION
#if defined(SC_DEV)
        // SC: REMOVE THIS ONCE WE HAVE A REAL DATA SOLUTION
        if (pst_Material->uc_Version == 3)
        {
            // Skip the Xenon specular color
            LOA_ReadULong(ppc_Buffer);
        }
#endif // defined(SC_DEV)
#endif
	}
	else
	{
		pst_Material->uc_Version = 1;
		pst_Material->uc_Sound = 0;
		pst_Material->uw_Dummy = 0;
	}

    pst_MLTTXLVL = &pst_Material->pst_FirstLevel;
#if defined(XML_CONV_TOOL)
	grmTexLevelPtrSave(*pst_MLTTXLVL, NULL);
#endif
    while(*pst_MLTTXLVL != NULL)
    {
#ifdef JADEFUSION
        *pst_MLTTXLVL = MAT_pst_CreateMTLevel(pst_Material);
#else
		*pst_MLTTXLVL = (MAT_tdst_MTLevel *) MEM_p_Alloc(sizeof(MAT_tdst_MTLevel));
        L_memset(*pst_MLTTXLVL, 0, sizeof(MAT_tdst_MTLevel));
#endif
		(*pst_MLTTXLVL)->s_TextureId = LOA_ReadShort(ppc_Buffer);
		(*pst_MLTTXLVL)->s_AditionalFlags = LOA_ReadUShort(ppc_Buffer);
		(*pst_MLTTXLVL)->ul_Flags = LOA_ReadULong(ppc_Buffer);
		(*pst_MLTTXLVL)->ScaleSPeedPosU = LOA_ReadULong(ppc_Buffer);
		(*pst_MLTTXLVL)->ScaleSPeedPosV = LOA_ReadULong(ppc_Buffer);
		(*pst_MLTTXLVL)->pst_NextLevel = (MAT_tdst_MTLevel *) LOA_ReadULong(ppc_Buffer);
		 	
#if defined(_XENON)
		// Initialize TextureID Cache
		memset((*pst_MLTTXLVL)->al_TextureIDCache, -1, sizeof((*pst_MLTTXLVL)->al_TextureIDCache));
#endif		
        /* Load each texture */
        Swap = (LONG) (*pst_MLTTXLVL)->s_TextureId;
 #if defined(XML_CONV_TOOL)
		grmTexLevelPtrSave((*pst_MLTTXLVL)->pst_NextLevel, NULL);
		pst_Texture = (TEX_tdst_Data* )(*pst_MLTTXLVL)->pst_NextLevel;
#else
        pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, (BIG_KEY) (*pst_MLTTXLVL)->pst_NextLevel);
        if(pst_Texture)
            (*pst_MLTTXLVL)->s_TextureId = pst_Texture->w_Index;
        else
        {
            (*pst_MLTTXLVL)->s_TextureId = TEX_w_List_AddTexture
                (
                    &TEX_gst_GlobalList,
                    (BIG_KEY) (*pst_MLTTXLVL)->pst_NextLevel,
                    1
                );
        }
#endif
#ifdef JADEFUSION
#if defined(ACTIVE_EDITORS)
        MAT_CheckForTGAMapping((BIG_KEY)(*pst_MLTTXLVL)->pst_NextLevel, "Base Texture");
#endif

#if defined(SC_DEV)
        // -----------------------
        // SC: REMOVE THIS SECTION
        if ((pst_Material->uc_Version >= 2) && (pst_Material->uc_Version <= MAT_XE_MTLEVEL_VERSION))
        {
            (*pst_MLTTXLVL)->pst_XeLevel = NULL;

            ULONG ul_Temp = LOA_ReadULong(ppc_Buffer);
            if (ul_Temp != NULL)
            {
                (*pst_MLTTXLVL)->pst_XeLevel = MAT_pst_CreateXeMTLevel();
            }
        }
        else
        {
            (*pst_MLTTXLVL)->pst_XeLevel = NULL;
        }
        // SC: END OF SECTION TO REMOVE
        // ----------------------------

#if defined(_XENON)
		(*pst_MLTTXLVL)->al_TextureIDCache[XeMaterial::TEXTURE_BASE] = (*pst_MLTTXLVL)->s_TextureId;
#endif
#if defined(_XENON_RENDER)
        // SC: Load the Xenon extended layer information
        // SC: FIXME: Needs to be adapted to the official Xenon-PS2 data structure
        if ((*pst_MLTTXLVL)->pst_XeLevel)
        {
            MAT_LoadXeMTLevelFromBuffer((*pst_MLTTXLVL)->pst_XeLevel, pst_Material, ppc_Buffer);
#if defined(_XENON)
			(*pst_MLTTXLVL)->al_TextureIDCache[XeMaterial::TEXTURE_NORMAL     ] = (*pst_MLTTXLVL)->pst_XeLevel->l_NormalMapId;
			(*pst_MLTTXLVL)->al_TextureIDCache[XeMaterial::TEXTURE_SPECULAR   ] = (*pst_MLTTXLVL)->pst_XeLevel->l_SpecularMapId;
			(*pst_MLTTXLVL)->al_TextureIDCache[XeMaterial::TEXTURE_ENVIRONMENT] = XE_CUBE_MAP_MASK | (*pst_MLTTXLVL)->pst_XeLevel->l_EnvMapId;
			(*pst_MLTTXLVL)->al_TextureIDCache[XeMaterial::TEXTURE_MOSS       ] = (*pst_MLTTXLVL)->pst_XeLevel->l_MossMapId;
			(*pst_MLTTXLVL)->al_TextureIDCache[XeMaterial::TEXTURE_DETAILNMAP ] = (*pst_MLTTXLVL)->pst_XeLevel->l_DetailNMapId;
#endif
        }
#endif // defined(_XENON_RENDER)

#elif defined(_XENON_RENDER)
        // No Xenon level
        (*pst_MLTTXLVL)->pst_XeLevel = NULL;
#endif // defined(SC_DEV)
#endif
        pst_MLTTXLVL = &(*pst_MLTTXLVL)->pst_NextLevel;
        *pst_MLTTXLVL = (MAT_tdst_MTLevel *) Swap;
    }

	/* Philippe -> Update from version <= 1 to 2 */
#if !defined(XML_CONV_TOOL)
	if (pst_Material->uc_Version > 10) pst_Material->uc_Version = 1; // Merci thierry !!!
#endif
	/*	if (pst_Material->uc_Version < 2)
	{
		ULONG ForgetIt ;
	    MAT_tdst_MTLevel        *pst_MLTTXLVL4UPDATE;
		pst_Material->uc_Version = 2;
		ForgetIt = 0;
		pst_MLTTXLVL4UPDATE = pst_Material->pst_FirstLevel;
		while(pst_MLTTXLVL4UPDATE != NULL)
		{
			switch(MAT_GET_Blending(pst_MLTTXLVL4UPDATE->ul_Flags))
			{
			case MAT_Cc_Op_AlphaDest:
			case MAT_Cc_Op_AlphaDestPremult:
				ForgetIt = 0;
				break; // Mierda !! 
			}
			pst_MLTTXLVL4UPDATE = pst_MLTTXLVL4UPDATE->pst_NextLevel;
		}
		if (!ForgetIt)
		{
			ULONG Flags;
			pst_MLTTXLVL4UPDATE = pst_Material->pst_FirstLevel;
			while(pst_MLTTXLVL4UPDATE != NULL)
			{
				Flags	= MAT_GET_FLAG(pst_MLTTXLVL4UPDATE->ul_Flags);
				Flags	|=MAT_Cul_Flag_HideAlpha;
				MAT_SET_FLAG(pst_MLTTXLVL4UPDATE->ul_Flags,Flags);
				pst_MLTTXLVL4UPDATE = pst_MLTTXLVL4UPDATE->pst_NextLevel;
			}
		}
	}*/
#if !defined(XML_CONV_TOOL)
    MAT_Validate_Multitexture(pst_Material);
#endif

	MEMpro_StopMemRaster(MEMpro_Id_GDK_MultiTexture);
    return pst_Material;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_MultiTexture *MAT_pst_CreateMultiTexture(char *_sz_Name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_MultiTexture   *pst_Material;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Material = (MAT_tdst_MultiTexture *) MEM_p_Alloc(sizeof(MAT_tdst_MultiTexture));
    L_memset(pst_Material, 0, sizeof(MAT_tdst_MultiTexture));

    GRO_Struct_Init(&pst_Material->st_Id, GRO_MaterialMultiTexture);
    GRO_Struct_SetName(&pst_Material->st_Id, _sz_Name);
	MAT_Validate_Multitexture(pst_Material);
    return pst_Material;
}

#ifndef PSX2_TARGET
#ifndef _GAMECUBE
#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_MultiTexture *MAT_p_CreateMultiTextureFromMad(MAD_StandarMaterial *_pst_MadMat)
{
//#ifdef ACTIVE_EDITORS
#if defined(ACTIVE_EDITORS) && !defined(XML_CONV_TOOL)

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_MultiTexture   *pst_Mat;
    short                   w_Tex;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Mat = (MAT_tdst_MultiTexture *) MAT_pst_CreateMaterial(GRO_MaterialMultiTexture, _pst_MadMat->MatRef.Name);

    pst_Mat->ul_Ambiant = _pst_MadMat->Ambient;
    pst_Mat->ul_Diffuse = _pst_MadMat->Diffuse;
    pst_Mat->ul_Specular = _pst_MadMat->Specular;
    pst_Mat->f_SpecularExp = _pst_MadMat->SelfIllum;
    pst_Mat->f_Opacity = _pst_MadMat->Opacity;
	pst_Mat->uc_Version = 1;
	pst_Mat->uc_Sound = 0;
    
    //pst_Mat->ul_Flags = _pst_MadMat->MaterialFlag;
    pst_Mat->ul_Flags = MAT_Cul_Flag_Bilinear;
    if (_pst_MadMat->MaterialFlag & MAD_MTF_UTiling)
        pst_Mat->ul_Flags |= MAT_Cul_Flag_TileU;
    if (_pst_MadMat->MaterialFlag & MAD_MTF_VTiling)
        pst_Mat->ul_Flags |= MAT_Cul_Flag_TileV;


    if (_pst_MadMat->MadTexture != -1)
    {
        w_Tex = (short) TEX_gal_MadToGlobalIndex[_pst_MadMat->MadTexture];
        if (w_Tex != -1)
        {
            pst_Mat->pst_FirstLevel = (MAT_tdst_MTLevel *) MEM_p_Alloc(sizeof(MAT_tdst_MTLevel));
            L_memset( pst_Mat->pst_FirstLevel, 0, sizeof(MAT_tdst_MTLevel));
        
            pst_Mat->pst_FirstLevel->s_TextureId = w_Tex;
            pst_Mat->pst_FirstLevel->ul_Flags = pst_Mat->ul_Flags;
        }
    }
	MAT_Validate_Multitexture(pst_Mat);
    return pst_Mat;

#endif
    return NULL;
}
#endif
#endif /* _GAMECUBE */
#endif /* ! PSX2_TARGET */

#ifdef JADEFUSION
// ------------------------------------------------------------------------------------------------
// Name   : MAT_pst_CreateMTLevel
// Params : _pst_MultiTex : Parent multi-texture
// RetVal : Level
// Descr. : Create a multitexture level
// ------------------------------------------------------------------------------------------------
MAT_tdst_MTLevel* MAT_pst_CreateMTLevel(MAT_tdst_MultiTexture* _pst_MultiTex)
{
    MAT_tdst_MTLevel* pst_Level = (MAT_tdst_MTLevel*)MEM_p_Alloc(sizeof(MAT_tdst_MTLevel));

    L_memset(pst_Level, 0, sizeof(MAT_tdst_MTLevel));

#if defined(_XENON_RENDER) && !defined(XML_CONV_TOOL)
    pst_Level->pst_XeMaterial = XeMaterial_CreateFromJade(_pst_MultiTex, pst_Level);
#endif

    return pst_Level;
}

// ------------------------------------------------------------------------------------------------
// Name   : MAT_DestroyMTLevel
// Params : _pst_Level : Level to destroy
// RetVal : None
// Descr. : Destroy a multitexture level
// ------------------------------------------------------------------------------------------------
void MAT_DestroyMTLevel(MAT_tdst_MTLevel* _pst_Level)
{
#if defined(_XENON_RENDER)
    if (_pst_Level->pst_XeMaterial)
    {
        XeMaterial_Destroy(_pst_Level->pst_XeMaterial);
    }

    if (_pst_Level->pst_XeLevel)
    {
        MAT_FreeXeMTLevel(_pst_Level->pst_XeLevel);
    }
#endif

    MEM_Free(_pst_Level);
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG MAT_l_SaveMultiTextureInBuffer(MAT_tdst_MultiTexture *_pst_Mat, TEX_tdst_List *_pst_TexList)
{
#ifdef ACTIVE_EDITORS

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_Data       *_pst_Texture;
    short               s_SaveTextureId;
    MAT_tdst_MTLevel    *pst_MLTTXLVL;
    MAT_tdst_MTLevel    st_MLTTXLVL_SAVE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
	unsigned int		stSizeToSave;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_MTLevel* oriPtr = _pst_Mat->pst_FirstLevel;	// store the real FirstLevel value

#if defined(XML_CONV_TOOL)
	_pst_Mat->pst_FirstLevel = (MAT_tdst_MTLevel* )grmTexLevelPtrGet(NULL);
#endif
#endif //JADEFUSION

    GRO_Struct_Save(&_pst_Mat->st_Id);

#ifdef JADEFUSION
	#if !defined(XML_CONV_TOOL)
		/* Evil Code to save/load new field */
		*(ULONG *)&_pst_Mat->f_SpecularExp |= 0x80000000;

	#if defined(SC_DEV)
			// -----------------------
			// SC: REMOVE THIS SECTION
			_pst_Mat->uc_Version = 4;
			// SC: END OF SECTION TO REMOVE
			// ----------------------------
		#else
			_pst_Mat->uc_Version = 1;
		#endif // defined(SC_DEV)
	#endif

	stSizeToSave = sizeof(MAT_tdst_MultiTexture)  - sizeof(GRO_tdst_Struct);
	#if defined(XML_CONV_TOOL)
		if (!(*(ULONG *)&_pst_Mat->f_SpecularExp & 0x80000000))
			stSizeToSave -= 4;
	#endif

	#if !defined(XML_CONV_TOOL)
		// We don't want to save a pointer in the grm object, right ?
		// So we temporarily change its value to indicate if its NULL or not
		if (_pst_Mat->pst_FirstLevel != NULL)
			_pst_Mat->pst_FirstLevel = (MAT_tdst_MTLevel* )1;	
	#endif
	SAV_Buffer(((char *) _pst_Mat) + sizeof(GRO_tdst_Struct), stSizeToSave);

	_pst_Mat->pst_FirstLevel = oriPtr;	// restore the real FirstLevel value

#else //JADEFUSION
	
	/* Evil Code to save/load new field */
	*(ULONG *)&_pst_Mat->f_SpecularExp |= 0x80000000;

	_pst_Mat->uc_Version = 1;

    SAV_Buffer(((char *) _pst_Mat) + sizeof(GRO_tdst_Struct), sizeof(MAT_tdst_MultiTexture)  - sizeof(GRO_tdst_Struct));
#endif //JADEFUSION

    pst_MLTTXLVL = _pst_Mat->pst_FirstLevel;

    while(pst_MLTTXLVL != NULL)
    {
        st_MLTTXLVL_SAVE = *pst_MLTTXLVL;

        s_SaveTextureId = pst_MLTTXLVL->s_TextureId;
        st_MLTTXLVL_SAVE.s_TextureId = 0;
        if(st_MLTTXLVL_SAVE.pst_NextLevel != NULL)
            st_MLTTXLVL_SAVE.s_TextureId = 1;
        *(LONG *) &st_MLTTXLVL_SAVE.pst_NextLevel = -1;
        if(s_SaveTextureId != -1)
        {
#if defined(XML_CONV_TOOL)
			st_MLTTXLVL_SAVE.pst_NextLevel = (MAT_tdst_MTLevel *)grmTexLevelPtrGet(NULL);
			st_MLTTXLVL_SAVE.s_TextureId = s_SaveTextureId;
#else
            _pst_Texture = &_pst_TexList->dst_Texture[s_SaveTextureId];
            st_MLTTXLVL_SAVE.pst_NextLevel = (MAT_tdst_MTLevel *) _pst_Texture->ul_Key;
#endif
		}
#ifdef JADEFUSION
	#if defined(SC_DEV)
        // SC: FIXME: The Xenon extended layer is saved here since the pointer is included in MAT_tdst_MTLevel
        //            It should not be that way since we don't want to add anything in the PS2 data
        st_MLTTXLVL_SAVE.pst_XeLevel = NULL;
        if (pst_MLTTXLVL->pst_XeLevel)
        {
            st_MLTTXLVL_SAVE.pst_XeLevel = (MAT_tdst_XeMTLevel*)1;
        }

        // SC: FIXME: SAV_Buffer() with sizeof(MAT_tdst_MTLevel) - sizeof(MAT_tdst_XeMTLevel*) ?
        // SC: VALIDATE
		ULONG size = sizeof(MAT_tdst_MTLevel) - sizeof(XeMaterial*);
	#if defined(XML_CONV_TOOL)
			// decrease size of struct if xenon specific stuff is not there...
			if (_pst_Mat->uc_Version != 3 && _pst_Mat->uc_Version != 4)
				size -= 4;
	#endif // XML_CONV_TOOL
			SAV_Buffer(((char *) &st_MLTTXLVL_SAVE), size);
	#else
			SAV_Buffer(((char *) &st_MLTTXLVL_SAVE), sizeof(MAT_tdst_MTLevel) - sizeof(MAT_tdst_XeMTLevel*) - sizeof(XeMaterial*));
	#endif // defined(SC_DEV)

	#if defined(SC_DEV)
			// SC: Save the Xenon extended information
			if (pst_MLTTXLVL->pst_XeLevel)
			{
				MAT_l_SaveXeMTLevelInBuffer(pst_MLTTXLVL->pst_XeLevel, _pst_TexList);
			}
	#endif

	#if defined(XML_CONV_TOOL)
			if (s_SaveTextureId == 0)
				break;
	#endif
#else//JADEFUSION
        SAV_Buffer(((char *) &st_MLTTXLVL_SAVE), sizeof(MAT_tdst_MTLevel));
#endif
		pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;

#if defined(XML_CONV_TOOL)
		if (s_SaveTextureId == 0)
			break;
#endif	
	}
#ifdef JADEFUSION
    // SC: Unflag the MSB since it will be considered an invalid value in Jade
    *(ULONG *)&_pst_Mat->f_SpecularExp &= ~0x80000000;
#endif

#endif
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_FreeMultiTexture(MAT_tdst_Material *_pst_Material)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_MultiTexture   *pst_Mat;
    MAT_tdst_MTLevel        *pst_MTL, *pst_NextMTL;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_Material->st_Id.i->ul_Type != GRO_MaterialMultiTexture) return;
    pst_Mat = (MAT_tdst_MultiTexture *) _pst_Material;

    pst_MTL = pst_Mat->pst_FirstLevel;
    while(pst_MTL != NULL)
    {
        pst_NextMTL = pst_MTL->pst_NextLevel;
#ifdef JADEFUSION
        MAT_DestroyMTLevel(pst_MTL);
#else
		MEM_Free(pst_MTL);
#endif
		pst_MTL = pst_NextMTL;
    }
}

#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAD_MultiTexMaterial *MAT_p_MultiTextureToMad(MAT_tdst_MultiTexture *_pst_Mat, WOR_tdst_World *_pst_World)
{
#ifdef ACTIVE_EDITORS

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_MultiTexMaterial	*pst_MadMat;
	MAT_tdst_MTLevel		*pst_Level;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_MALLOC(MAD_MultiTexMaterial, pst_MadMat, 1);

    pst_MadMat->MatRef.MaterialType = ID_MAT_MultiTexMaterial;
    L_strcpy(pst_MadMat->MatRef.Name, GRO_sz_Struct_GetName( &_pst_Mat->st_Id ) );
    pst_MadMat->Diffuse = _pst_Mat->ul_Diffuse;
    pst_MadMat->Ambient = _pst_Mat->ul_Ambiant;
    pst_MadMat->Specular = _pst_Mat->ul_Specular;
    pst_MadMat->SelfIllum = _pst_Mat->f_SpecularExp;
    pst_MadMat->Opacity = _pst_Mat->f_Opacity;

	// Get levels
	pst_MadMat->NumberOfLevels = 0;
	pst_MadMat->AllLevels = NULL;
	pst_Level = _pst_Mat->pst_FirstLevel;
	while (pst_Level)
	{
		if (pst_MadMat->NumberOfLevels) {
			MAD_REALLOC(MAD_MTLevel, pst_MadMat->AllLevels, pst_MadMat->NumberOfLevels + 1);
		} else {
			MAD_MALLOC(MAD_MTLevel, pst_MadMat->AllLevels, pst_MadMat->NumberOfLevels + 1);
		}

		pst_MadMat->AllLevels[pst_MadMat->NumberOfLevels].s_TextureId = pst_Level->s_TextureId;
		// TODO : write only interesting flags
		pst_MadMat->AllLevels[pst_MadMat->NumberOfLevels].ul_Flags = pst_Level->ul_Flags;

		pst_MadMat->NumberOfLevels++;
		pst_Level = pst_Level->pst_NextLevel;
	}
    //pst_MadMat->MadTexture = (_pst_Mat->pst_FirstLevel) ? _pst_Mat->pst_FirstLevel->s_TextureId : -1;

    //pst_MadMat->MaterialFlag = _pst_Mat->ul_Flags;
    pst_MadMat->MaterialFlag = 0;
    if (_pst_Mat->ul_Flags & MAT_Cul_Flag_TileU)
        pst_MadMat->MaterialFlag |= MAD_MTF_UTiling;
    if (_pst_Mat->ul_Flags & MAT_Cul_Flag_TileV)
        pst_MadMat->MaterialFlag |= MAD_MTF_VTiling;

    return pst_MadMat;
#endif
    return NULL;
}
#endif 

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MAT_p_MultiTextureDuplicate(MAT_tdst_MultiTexture *_pst_Mat, char *_sz_Path, char *_sz_Name, ULONG _ul_Flags)
{
#ifdef ACTIVE_EDITORS

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_MultiTexture *pst_MM;
    MAT_tdst_MTLevel      *pst_MTLsrc, *pst_MTLdup, *pst_MTLprev;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_MM = MAT_pst_CreateMultiTexture( _sz_Name );

    pst_MM->f_Opacity = _pst_Mat->f_Opacity;
    pst_MM->f_SpecularExp = _pst_Mat->f_SpecularExp;
    pst_MM->ul_Ambiant = _pst_Mat->ul_Ambiant;
    pst_MM->ul_Diffuse = _pst_Mat->ul_Diffuse;
    pst_MM->ul_ValidateMask = _pst_Mat->ul_ValidateMask;
    pst_MM->ul_Flags = _pst_Mat->ul_Flags;
    pst_MM->ul_Specular = _pst_Mat->ul_Specular;
    pst_MM->uc_Sound = _pst_Mat->uc_Sound;
	pst_MM->uc_Version = _pst_Mat->uc_Version;

    pst_MTLsrc = _pst_Mat->pst_FirstLevel;
    pst_MTLdup = NULL;
    pst_MTLprev = NULL;

    while(pst_MTLsrc != NULL)
    {
#ifdef JADEFUSION
        pst_MTLdup = MAT_pst_CreateMTLevel(pst_MM);
#else
		pst_MTLdup = (MAT_tdst_MTLevel *) MEM_p_Alloc(sizeof(MAT_tdst_MTLevel));
#endif
#if defined(_XENON_RENDER)
        XeMaterial* pst_XeMaterialSave = pst_MTLdup->pst_XeMaterial;
        L_memcpy( pst_MTLdup, pst_MTLsrc, sizeof(MAT_tdst_MTLevel));
        pst_MTLdup->pst_XeMaterial = pst_XeMaterialSave;
#else
		L_memcpy( pst_MTLdup, pst_MTLsrc, sizeof(MAT_tdst_MTLevel));
#endif

#ifdef JADEFUSION
		// SC: Duplicate the Xenon information
		if (pst_MTLsrc && pst_MTLsrc->pst_XeLevel)
		{
			pst_MTLdup->pst_XeLevel = MAT_pst_DuplicateXeMTLevel(pst_MTLsrc->pst_XeLevel);
		}
#endif		
		if (pst_MTLprev != NULL)
            pst_MTLprev->pst_NextLevel = pst_MTLdup;
        else
            pst_MM->pst_FirstLevel = pst_MTLdup;

        pst_MTLprev = pst_MTLdup;
        
        pst_MTLsrc = pst_MTLsrc->pst_NextLevel;
    }

    GRO_ul_Struct_FullSave(&pst_MM->st_Id, _sz_Path, _sz_Name, &TEX_gst_GlobalList);

    return pst_MM;
#endif
    return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 UpperNumberOFSubMaterial = -1;
void MAT_Validate_Multitexture(MAT_tdst_MultiTexture *_pst_Mat)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_MTLevel    *pst_MLTTXLVL;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    _pst_Mat ->ul_ValidateMask = 0;

    pst_MLTTXLVL = _pst_Mat->pst_FirstLevel;
	if(!pst_MLTTXLVL) return;

	{
	    if (((MAT_GET_Blending(pst_MLTTXLVL->ul_Flags) != MAT_Cc_Op_Copy) && (MAT_GET_Blending(pst_MLTTXLVL->ul_Flags) != MAT_Cc_Op_Glow)) || (pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_HideColor))
	        _pst_Mat->ul_ValidateMask |= MAT_ValidateMask_Transparency;
	}
    
/*    if ((pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_HideColor))
	    pst_MLTTXLVL->ul_Flags = pst_MLTTXLVL->ul_Flags + 1;*/

    while(pst_MLTTXLVL != NULL)
    {
		if((pst_MLTTXLVL->ScaleSPeedPosU == MAT_Cc_Identity) && (pst_MLTTXLVL->ScaleSPeedPosV == MAT_Cc_Identity))
		{
			pst_MLTTXLVL->ScaleSPeedPosU = 0;
		} else
		if((pst_MLTTXLVL->ScaleSPeedPosU == 0) || (pst_MLTTXLVL->ScaleSPeedPosV == 0)) 
		{
			pst_MLTTXLVL->ScaleSPeedPosU = 0;
		}
    
		switch(MAT_GET_ColorOp(pst_MLTTXLVL->ul_Flags))
		{
		case MAT_Cc_ColorOp_Specular:
            _pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_Specular_UV | MAT_ValidateMask_Specular_Color;
			break;
		case MAT_Cc_ColorOp_Diffuse2X:
		case MAT_Cc_ColorOp_InvertDiffuse:
		case MAT_Cc_ColorOp_Diffuse:
            _pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_Diffuse;
			break;
		case MAT_Cc_ColorOp_Disable:
            _pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_Constant;
			break;
		case MAT_Cc_ColorOp_FullLight:
            _pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_FullLight;
			break;
		case MAT_Cc_ColorOp_RLI:
            _pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_RLI;
			break;
		}
		switch(MAT_GET_Blending(pst_MLTTXLVL->ul_Flags))
		{
			case MAT_Cc_Op_Alpha				:
			case MAT_Cc_Op_AlphaPremult			:
				if (!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_UseLocalAlpha))
					_pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_RLIAlpha;
				break;
			case MAT_Cc_Op_AlphaDest			:
			case MAT_Cc_Op_AlphaDestPremult		:

				break;
			case MAT_Cc_Op_Copy					:
			case MAT_Cc_Op_Glow					:
				if (!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_UseLocalAlpha))
				{
					if (pst_MLTTXLVL->pst_NextLevel)
					{
						if ((MAT_GET_Blending(pst_MLTTXLVL->pst_NextLevel->ul_Flags) == MAT_Cc_Op_AlphaDest))
							_pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_RLIAlpha;
						if ((MAT_GET_Blending(pst_MLTTXLVL->pst_NextLevel->ul_Flags) == MAT_Cc_Op_AlphaDestPremult))
							_pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_RLIAlpha;
					}
				}
				break;
			case MAT_Cc_Op_Add					:
			case MAT_Cc_Op_Sub					:
			case MAT_Cc_Op_PSX2ShadowSpecific	:
				break;
		}
		if (pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_AlphaTest)
		{
				if (!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_UseLocalAlpha))
					_pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_RLIAlpha;
		}
		if ( pst_MLTTXLVL->s_AditionalFlags & MAT_XYZ_Flag_ShiftUsingNormal )
			_pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_Fur;

        if (MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags) == MAT_Cc_UV_DF_GZMO)
            _pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_Specular_UV ;
        
        if (MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags) == MAT_Cc_UV_Phong_GZMO)
            _pst_Mat -> ul_ValidateMask |= MAT_ValidateMask_Specular_UV ;
        
#if defined(_XENON_RENDER)
        if (pst_MLTTXLVL->pst_XeLevel != NULL)
        {
            // Disable range remapping when local alpha is used (also adjust local alpha)
            if (pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_UseLocalAlpha)
            {
                ULONG ulLocalAlpha = MAT_GET_LocalAlpha(pst_MLTTXLVL->s_AditionalFlags);
                ulLocalAlpha     <<= 3;

                FLOAT fAlphaStart = -pst_MLTTXLVL->pst_XeLevel->f_AlphaOffset * 255.0f / pst_MLTTXLVL->pst_XeLevel->f_AlphaScale;
                FLOAT fAlphaEnd   = (255.0f - (pst_MLTTXLVL->pst_XeLevel->f_AlphaOffset * 255.0f)) / pst_MLTTXLVL->pst_XeLevel->f_AlphaScale;
                FLOAT fAlpha      = MATH_f_FloatBlend(fAlphaStart, fAlphaEnd, (FLOAT)ulLocalAlpha / 255.0f) / 255.0f;
                fAlpha            = MATH_f_FloatLimit(fAlpha, 0.0f, 1.0f);

                ulLocalAlpha = (ULONG)(fAlpha * 255.0f) >> 3;
                MAT_SET_LocalAlpha(pst_MLTTXLVL->s_AditionalFlags, (USHORT)ulLocalAlpha);

                pst_MLTTXLVL->pst_XeLevel->f_AlphaScale  = 1.0f;
                pst_MLTTXLVL->pst_XeLevel->f_AlphaOffset = 0.0f;
            }

            // Disable moss when alpha test is used
            if (pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_AlphaTest)
            {
                pst_MLTTXLVL->pst_XeLevel->l_MossMapId = MAT_Xe_InvalidTextureId;
#if defined(_XENON)
                pst_MLTTXLVL->al_TextureIDCache[XeMaterial::TEXTURE_MOSS] = MAT_Xe_InvalidTextureId;
#endif

            }
        }
#endif
        pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
    }

#ifdef JADEFUSION

	if (UpperNumberOFSubMaterial == 15)
	{
        /* King kong ? */
	    if ((_pst_Mat -> ul_ValidateMask & (MAT_ValidateMask_Fur & MAT_ValidateMask_Transparency) ) == (MAT_ValidateMask_Fur & MAT_ValidateMask_Transparency))
	    {
	        _pst_Mat->ul_ValidateMask |= MAT_ValidateMask_KingKong;
	    }
	}
	UpperNumberOFSubMaterial = -1;
	
#endif
}
// ------------------------------------------------------------------------------------------------
// Xenon extended material properties
// ------------------------------------------------------------------------------------------------
#if defined(_XENON_RENDER)

// ------------------------------------------------------------------------------------------------
// Name   : MAT_XeResetTransform
// Params : _pst_Transform : Transform structure to reset
// RetVal : None
// Descr. : Reset a transform to identity
// ------------------------------------------------------------------------------------------------
void MAT_XeResetTransform(MAT_tdst_XeTransform* _pst_Transform)
{
    ERR_X_Assert(_pst_Transform != NULL);

    _pst_Transform->f_ScaleU    = 1.0f;
    _pst_Transform->f_ScaleV    = 1.0f;
    _pst_Transform->f_Angle     = 0.0f;
    _pst_Transform->f_SpeedU    = 0.0f;
    _pst_Transform->f_SpeedV    = 0.0f;
    _pst_Transform->f_RollSpeed = 0.0f;
}

// ------------------------------------------------------------------------------------------------
// Name   : MAT_pst_CreateXeMTLevel
// Params : None
// RetVal : Xenon MT level extended information structure
// Descr. : Create a Xenon MT level information structure
// ------------------------------------------------------------------------------------------------
MAT_tdst_XeMTLevel* MAT_pst_CreateXeMTLevel(void)
{
    MAT_tdst_XeMTLevel* pst_Level = (MAT_tdst_XeMTLevel*)MEM_p_Alloc(sizeof(MAT_tdst_XeMTLevel));

    L_memset(pst_Level, 0, sizeof(MAT_tdst_XeMTLevel));

    // Default flags
    pst_Level->ul_Flags = 0;

    // Diffuse
    pst_Level->ul_DiffuseColor  = 0x40808080;

    // Specular
    pst_Level->ul_SpecularColor = 0x40808080;
    pst_Level->f_SpecularExp    = 29.2f;
    pst_Level->f_SpecularBias   = -0.6f;

    // Mipmap bias
    pst_Level->f_BaseMipMapLODBias   = 0.0f;
    pst_Level->f_NormalMipMapLODBias = 0.0f;

    // No textures
    pst_Level->l_NormalMapId    = MAT_Xe_InvalidTextureId;
    pst_Level->l_SpecularMapId  = MAT_Xe_InvalidTextureId;
    pst_Level->l_EnvMapId       = MAT_Xe_InvalidTextureId;
    pst_Level->l_DetailNMapId   = MAT_Xe_InvalidTextureId;
    pst_Level->l_MossMapId      = MAT_Xe_InvalidTextureId;

    // Transforms
    MAT_XeResetTransform(&pst_Level->st_NormalMapTransform);
    MAT_XeResetTransform(&pst_Level->st_SpecularMapTransform);
    MAT_XeResetTransform(&pst_Level->st_DetailNMapTransform);

    // Detail normal map LOD range - Always at full scale
    pst_Level->uc_DetailNMapStartLOD = 0;
    pst_Level->uc_DetailNMapFullLOD  = 0;

    // Detail normal map at full strength
    pst_Level->f_DetailNMapStrength = 1.0f;

    // alpha threshold
    pst_Level->f_AlphaScale = 1.0f;
    pst_Level->f_AlphaOffset = 0.0f;

    // Rim Light
    pst_Level->f_RimLightWidthMin       = 0.1f;
    pst_Level->f_RimLightWidthMax       = 0.5f;
    pst_Level->f_RimLightIntensity      = 0.7f;
    pst_Level->f_RimLightNormalMapRatio = 0.7f;

    // Moss color
    pst_Level->ul_MossMapColor = 0x003c3c3c;

    // Moss specular factor
    pst_Level->f_MossSpecularFactor = 0.3f;

    // Mesh processing
#if defined(ACTIVE_EDITORS)
    pst_Level->ul_MeshProcessingFlags =   0;
    pst_Level->f_DisplacementOffset   =  -0.05f;
    pst_Level->f_DisplacementHeight   =   0.1f;
    pst_Level->f_TessellationArea     =   0.005f;
    pst_Level->f_ChamferThreshold     =  70.0f;
    pst_Level->f_ChamferLength        =   0.05f;
    pst_Level->f_ChamferWeldThreshold =   0.0005f;
#endif
    pst_Level->f_SmoothThreshold      = 180.0f;

    return pst_Level;
}

// ------------------------------------------------------------------------------------------------
// Name   : MAT_pst_DuplicateXeMTLevel
// Params : _pstLevel : Source level
// RetVal : Copy of the source level
// Descr. : Duplicate a Xenon MT level
// ------------------------------------------------------------------------------------------------
MAT_tdst_XeMTLevel* MAT_pst_DuplicateXeMTLevel(MAT_tdst_XeMTLevel* _pst_Level)
{
    MAT_tdst_XeMTLevel* pst_New = (MAT_tdst_XeMTLevel*)MEM_p_Alloc(sizeof(MAT_tdst_XeMTLevel));

    ERR_X_Assert(_pst_Level != NULL);

    L_memcpy(pst_New, _pst_Level, sizeof(MAT_tdst_XeMTLevel));

    return pst_New;
}

// ------------------------------------------------------------------------------------------------
// Name   : MAT_FreeXeMTLevel
// Params : _pst_Level : Level to free
// RetVal : None
// Descr. : Destroy a Xenon MT level information structure
// ------------------------------------------------------------------------------------------------
void MAT_FreeXeMTLevel(MAT_tdst_XeMTLevel* _pst_Level)
{
    ERR_X_Assert(_pst_Level != NULL);

    // Unreference the cube map
    if (_pst_Level->l_EnvMapId != MAT_Xe_InvalidTextureId)
    {
        TEX_CubeMap_Remove((SHORT)_pst_Level->l_EnvMapId);
    }

    // TOADD: Free the other components if necessary

    MEM_Free(_pst_Level);
}

#if defined(XML_CONV_TOOL)
extern ULONG gGrmXeVersion;
#endif

// ------------------------------------------------------------------------------------------------
// Name   : MAT_LoadXeMTLevelFromBuffer
// Params : _pst_Level  : Level to load information to
//          _ppc_Buffer : Source buffer [in|out]
// RetVal : None
// Descr. : Load a Xenon MT level from a buffer
// ------------------------------------------------------------------------------------------------
void MAT_LoadXeMTLevelFromBuffer(MAT_tdst_XeMTLevel* _pst_Level, MAT_tdst_MultiTexture* _pst_Parent, char** _ppc_Buffer)
{
    TEX_tdst_Data* pst_TexData;
    ULONG   ul_Version;
    BIG_KEY ul_Key;

    ERR_X_Assert(_pst_Level != NULL);
    ERR_X_Assert(_ppc_Buffer != NULL);

    // Version
    ul_Version = LOA_ReadULong(_ppc_Buffer);
#if defined(XML_CONV_TOOL)
	gGrmXeVersion = ul_Version;
#endif

    if (ul_Version >= 1)
    {
        // Flags
        _pst_Level->ul_Flags = LOA_ReadULong(_ppc_Buffer);

        // Diffuse color
        if (ul_Version >= 6)
        {
            _pst_Level->ul_DiffuseColor = LOA_ReadULong(_ppc_Buffer);
        }
        else
        {
            _pst_Level->ul_DiffuseColor = _pst_Parent->ul_Diffuse;
        }

#if !defined(XML_CONV_TOOL)
        if (ul_Version <= 11)
        {
            // force diffuse alpha to 0.25 since the value is always multiplied by 4
            _pst_Level->ul_DiffuseColor &= 0x00FFFFFF;
            _pst_Level->ul_DiffuseColor |= 0x40000000;
        }
#endif

        // Specular color
        if (ul_Version >= 4)
        {
            _pst_Level->ul_SpecularColor = LOA_ReadULong(_ppc_Buffer);
            _pst_Level->f_SpecularExp    = LOA_ReadFloat(_ppc_Buffer);
        }
        else
        {
            _pst_Level->ul_SpecularColor = 0x00000000;
            _pst_Level->f_SpecularExp    = 1.0f;
        }

#if !defined(XML_CONV_TOOL)
        if (ul_Version <= 11)
        {
            // force specular alpha to 0.25 since the value is always multiplied by 4
            _pst_Level->ul_SpecularColor &= 0x00FFFFFF;
            _pst_Level->ul_SpecularColor |= 0x40000000;
        }
#endif

        // Normal map
        _pst_Level->l_NormalMapId = MAT_Xe_InvalidTextureId;
        ul_Key = LOA_ReadULong(_ppc_Buffer);
#if defined(XML_CONV_TOOL)
		_pst_Level->l_NormalMapId = ul_Key;
#else
        if (
            (ul_Key != BIG_C_InvalidKey)
#if defined(_XENON)
            && ((_pst_Level->ul_Flags & MAT_Xe_Flag_NMapDisable) == 0)
#endif
           )
        {
#if defined(ACTIVE_EDITORS)
			if (!LOA_IsBinarizing() || ((_pst_Level->ul_Flags & MAT_Xe_Flag_NMapDisable) == 0))
#endif
			{
				pst_TexData = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, ul_Key);
				if (pst_TexData)
				{
					_pst_Level->l_NormalMapId = pst_TexData->w_Index;
				}
				else
				{
					_pst_Level->l_NormalMapId = TEX_w_List_AddTexture(&TEX_gst_GlobalList, ul_Key, 1);
				}
			}
        }
#endif

#if defined(ACTIVE_EDITORS)
        MAT_CheckForTGAMapping(ul_Key, "Normal Map");
#endif

        // Specular map
        _pst_Level->l_SpecularMapId = MAT_Xe_InvalidTextureId;
        ul_Key = LOA_ReadULong(_ppc_Buffer);
#if defined(XML_CONV_TOOL)
		_pst_Level->l_SpecularMapId = ul_Key;
#else
        if (
            (ul_Key != BIG_C_InvalidKey)
#if defined(_XENON)
            && ((_pst_Level->ul_Flags & MAT_Xe_Flag_SMapDisable) == 0)
#endif
           )
        {
#if defined(ACTIVE_EDITORS)
			if (!LOA_IsBinarizing() || ((_pst_Level->ul_Flags & MAT_Xe_Flag_SMapDisable) == 0))
#endif
			{
				pst_TexData = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, ul_Key);
				if (pst_TexData)
				{
					_pst_Level->l_SpecularMapId = pst_TexData->w_Index;
				}
				else
				{
					_pst_Level->l_SpecularMapId = TEX_w_List_AddTexture(&TEX_gst_GlobalList, ul_Key, 1);
				}
			}
        }
#endif

#if defined(ACTIVE_EDITORS)
        MAT_CheckForTGAMapping(ul_Key, "Specular Map");
#endif

        // Environement map
        _pst_Level->l_EnvMapId = MAT_Xe_InvalidTextureId;
        ul_Key = LOA_ReadULong(_ppc_Buffer);
#if defined(XML_CONV_TOOL)
		_pst_Level->l_EnvMapId = ul_Key;
#else
        if (
            (ul_Key != BIG_C_InvalidKey)
#if defined(_XENON)
            && ((_pst_Level->ul_Flags & MAT_Xe_Flag_EMapDisable) == 0)
#endif
           )
        {
#if defined(ACTIVE_EDITORS)
			if (!LOA_IsBinarizing() || ((_pst_Level->ul_Flags & MAT_Xe_Flag_EMapDisable) == 0))
#endif
			{
	            _pst_Level->l_EnvMapId = TEX_CubeMap_Add(ul_Key);
			}
        }
#endif

        // moss map texture key (v10+)
        _pst_Level->l_MossMapId = MAT_Xe_InvalidTextureId;
        if (ul_Version >= 10)
        {            
            ul_Key = LOA_ReadULong(_ppc_Buffer);
#if defined(XML_CONV_TOOL)
            _pst_Level->l_MossMapId = ul_Key;
#else
            if (
                (ul_Key != BIG_C_InvalidKey)
#if defined(_XENON)
                && ((_pst_Level->ul_Flags & MAT_Xe_Flag_MossMapDisable) == 0)
#endif
               )
            {
#if defined(ACTIVE_EDITORS)
				if (!LOA_IsBinarizing() || ((_pst_Level->ul_Flags & MAT_Xe_Flag_MossMapDisable) == 0))
#endif
				{
					pst_TexData = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, ul_Key);
					if (pst_TexData)
					{
						_pst_Level->l_MossMapId = pst_TexData->w_Index;
					}
					else
					{
						_pst_Level->l_MossMapId = TEX_w_List_AddTexture(&TEX_gst_GlobalList, ul_Key, 1);
					}
				}
            }
#endif

#if defined(ACTIVE_EDITORS)
            MAT_CheckForTGAMapping(ul_Key, "Moss Map");
#endif
        }

        // Detail normal map texture key (v2+)
        if (ul_Version >= 2)
        {
            _pst_Level->l_DetailNMapId = MAT_Xe_InvalidTextureId;
            ul_Key = LOA_ReadULong(_ppc_Buffer);
#if defined(XML_CONV_TOOL)
			_pst_Level->l_DetailNMapId = ul_Key;
#else
            if (
                (ul_Key != BIG_C_InvalidKey)
#if defined(_XENON)
                && ((_pst_Level->ul_Flags & MAT_Xe_Flag_DNMapDisable) == 0)
#endif
               )
            {
#if defined(ACTIVE_EDITORS)
				if (!LOA_IsBinarizing() || ((_pst_Level->ul_Flags & MAT_Xe_Flag_DNMapDisable) == 0))
#endif
				{
					pst_TexData = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, ul_Key);
					if (pst_TexData)
					{
						_pst_Level->l_DetailNMapId = pst_TexData->w_Index;
					}
					else
					{
						_pst_Level->l_DetailNMapId = TEX_w_List_AddTexture(&TEX_gst_GlobalList, ul_Key, 1);
					}
				}
            }
#endif

#if defined(ACTIVE_EDITORS)
            MAT_CheckForTGAMapping(ul_Key, "Detail Normal Map");
#endif
        }

        // Specular bias
        _pst_Level->f_SpecularBias = LOA_ReadFloat(_ppc_Buffer);

        // Normal map transfrom
        _pst_Level->st_NormalMapTransform.f_ScaleU    = LOA_ReadFloat(_ppc_Buffer);
        _pst_Level->st_NormalMapTransform.f_ScaleV    = LOA_ReadFloat(_ppc_Buffer);
        _pst_Level->st_NormalMapTransform.f_SpeedU    = LOA_ReadFloat(_ppc_Buffer);
        _pst_Level->st_NormalMapTransform.f_SpeedV    = LOA_ReadFloat(_ppc_Buffer);
        _pst_Level->st_NormalMapTransform.f_Angle     = LOA_ReadFloat(_ppc_Buffer);
        _pst_Level->st_NormalMapTransform.f_RollSpeed = LOA_ReadFloat(_ppc_Buffer);

        // Specular map transform
        _pst_Level->st_SpecularMapTransform.f_ScaleU    = LOA_ReadFloat(_ppc_Buffer);
        _pst_Level->st_SpecularMapTransform.f_ScaleV    = LOA_ReadFloat(_ppc_Buffer);
        _pst_Level->st_SpecularMapTransform.f_SpeedU    = LOA_ReadFloat(_ppc_Buffer);
        _pst_Level->st_SpecularMapTransform.f_SpeedV    = LOA_ReadFloat(_ppc_Buffer);
        _pst_Level->st_SpecularMapTransform.f_Angle     = LOA_ReadFloat(_ppc_Buffer);
        _pst_Level->st_SpecularMapTransform.f_RollSpeed = LOA_ReadFloat(_ppc_Buffer);

        // Detail normal map transform (v2+)
        if (ul_Version >= 2)
        {
            _pst_Level->st_DetailNMapTransform.f_ScaleU    = LOA_ReadFloat(_ppc_Buffer);
            _pst_Level->st_DetailNMapTransform.f_ScaleV    = LOA_ReadFloat(_ppc_Buffer);
            _pst_Level->st_DetailNMapTransform.f_SpeedU    = LOA_ReadFloat(_ppc_Buffer);
            _pst_Level->st_DetailNMapTransform.f_SpeedV    = LOA_ReadFloat(_ppc_Buffer);

            // Skip the angle and roll speed
            if (ul_Version <= 4)
            {
                LOA_ReadFloat(_ppc_Buffer);
                LOA_ReadFloat(_ppc_Buffer);
            }
        }

        // Environment map color
        _pst_Level->ul_EnvMapColor = LOA_ReadULong(_ppc_Buffer);

        // Moss map color
        if (ul_Version >= 10)
        {
            _pst_Level->ul_MossMapColor = LOA_ReadULong(_ppc_Buffer);
        }
        else
        {
            _pst_Level->ul_MossMapColor = 0;
        }

        // Moss specular factor
        if (ul_Version >= 15)
        {
            _pst_Level->f_MossSpecularFactor = LOA_ReadFloat(_ppc_Buffer);
        }
        else
        {
            _pst_Level->f_MossSpecularFactor = 0.3f;
        }

        // Detail normal map LOD levels (v2+)
        if (ul_Version >= 2)
        {
            _pst_Level->uc_DetailNMapStartLOD = LOA_ReadUChar(_ppc_Buffer);
            _pst_Level->uc_DetailNMapFullLOD  = LOA_ReadUChar(_ppc_Buffer);
        }
        else
        {
            _pst_Level->uc_DetailNMapStartLOD = 0;
            _pst_Level->uc_DetailNMapFullLOD  = 0;
        }

        // Strength of the detail normal map
        if (ul_Version >= 3)
        {
            _pst_Level->f_DetailNMapStrength = LOA_ReadFloat(_ppc_Buffer);
        }
        else
        {
            _pst_Level->f_DetailNMapStrength = 1.0f;
        }

        // Mipmap LOD bias
        if (ul_Version >= 7)
        {
            _pst_Level->f_BaseMipMapLODBias   = LOA_ReadFloat(_ppc_Buffer);
            _pst_Level->f_NormalMipMapLODBias = LOA_ReadFloat(_ppc_Buffer);
        }

        if (ul_Version >= 8)
        {
            _pst_Level->f_AlphaScale          = LOA_ReadFloat(_ppc_Buffer);
            _pst_Level->f_AlphaOffset         = LOA_ReadFloat(_ppc_Buffer);
        }

        if (ul_Version >= 11)
        {
            _pst_Level->f_RimLightWidthMin       = LOA_ReadFloat(_ppc_Buffer);
            _pst_Level->f_RimLightWidthMax       = LOA_ReadFloat(_ppc_Buffer);
            _pst_Level->f_RimLightIntensity      = LOA_ReadFloat(_ppc_Buffer);
            _pst_Level->f_RimLightNormalMapRatio = LOA_ReadFloat(_ppc_Buffer);
        }

        // Mesh processing
        if (ul_Version >= 13)
        {
            ULONG ul_XMP               = LOA_ReadULong(_ppc_Buffer);
            FLOAT f_DisplacementOffset = LOA_ReadFloat(_ppc_Buffer);
            FLOAT f_DisplacementHeight = LOA_ReadFloat(_ppc_Buffer);
            FLOAT f_TessellationArea   = LOA_ReadFloat(_ppc_Buffer);

#if defined(ACTIVE_EDITORS)
            _pst_Level->ul_MeshProcessingFlags = ul_XMP;
            _pst_Level->f_DisplacementOffset   = f_DisplacementOffset;
            _pst_Level->f_DisplacementHeight   = f_DisplacementHeight;
            _pst_Level->f_TessellationArea     = f_TessellationArea;
#endif
        }
        if (ul_Version >= 14)
        {
            _pst_Level->f_SmoothThreshold = LOA_ReadFloat(_ppc_Buffer);
        }
        if (ul_Version >= 16)
        {
            FLOAT f_ChamferThreshold = LOA_ReadFloat(_ppc_Buffer);
            FLOAT f_ChamferLength    = LOA_ReadFloat(_ppc_Buffer);

#if defined(ACTIVE_EDITORS)
            _pst_Level->f_ChamferThreshold = f_ChamferThreshold;
            _pst_Level->f_ChamferLength    = f_ChamferLength;
#endif
        }
        if (ul_Version >= 17)
        {
            FLOAT f_ChamferWeldThreshold = LOA_ReadFloat(_ppc_Buffer);

#if defined(ACTIVE_EDITORS)
            _pst_Level->f_ChamferWeldThreshold = f_ChamferWeldThreshold;
#endif
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : MAT_l_SaveXeMTLevelInBuffer
// Params : _pst_Level   : Level to save
//          _pst_TexList : Current texture list
// RetVal : Always 0
// Descr. : Save a Xenon MT level in the current output buffer
// ------------------------------------------------------------------------------------------------
LONG MAT_l_SaveXeMTLevelInBuffer(MAT_tdst_XeMTLevel* _pst_Level, TEX_tdst_List* _pst_TexList)
{
#if defined(SC_DEV)
#if defined(ACTIVE_EDITORS)

	ULONG ul_Version;
#if defined(XML_CONV_TOOL)
	ul_Version = gGrmXeVersion;
#else
	ul_Version = MAT_XE_MTLEVEL_VERSION;
#endif

    ULONG ul_NormalMapKey   = BIG_C_InvalidKey;
    ULONG ul_SpecularMapKey = BIG_C_InvalidKey;
    ULONG ul_EnvMapKey      = BIG_C_InvalidKey;
    ULONG ul_MossMapKey      = BIG_C_InvalidKey;
    ULONG ul_DetailNMapKey  = BIG_C_InvalidKey;

    ERR_X_Assert(_pst_Level != NULL);
    ERR_X_Assert(_pst_TexList != NULL);

#if defined(XML_CONV_TOOL)
	ul_NormalMapKey   = _pst_Level->l_NormalMapId;
	ul_SpecularMapKey = _pst_Level->l_SpecularMapId;
	ul_EnvMapKey      = _pst_Level->l_EnvMapId;
    ul_MossMapKey      = _pst_Level->l_MossMapId;
	ul_DetailNMapKey  = _pst_Level->l_DetailNMapId;
#else
    // FIXME: All the information saved by this function should be in the Xenon-specific data structure

    // Normal map
    if (_pst_Level->l_NormalMapId != MAT_Xe_InvalidTextureId)
    {
        ul_NormalMapKey = _pst_TexList->dst_Texture[_pst_Level->l_NormalMapId].ul_Key;
    }

    // Specular map
    if (_pst_Level->l_SpecularMapId != MAT_Xe_InvalidTextureId)
    {
        ul_SpecularMapKey = _pst_TexList->dst_Texture[_pst_Level->l_SpecularMapId].ul_Key;
    }

    // Environment map
    if (_pst_Level->l_EnvMapId >= 0)
    {
        ul_EnvMapKey = TEX_CubeMap_GetKey((SHORT)_pst_Level->l_EnvMapId);
    }

    // Moss map
    if (_pst_Level->l_MossMapId != MAT_Xe_InvalidTextureId)
    {
        ul_MossMapKey = _pst_TexList->dst_Texture[_pst_Level->l_MossMapId].ul_Key;
    }

    // Detail normal map
    if (_pst_Level->l_DetailNMapId != MAT_Xe_InvalidTextureId)
    {
        ul_DetailNMapKey = _pst_TexList->dst_Texture[_pst_Level->l_DetailNMapId].ul_Key;
    }
#endif

    // Save the structure
    SAV_Buffer(&ul_Version,                                         sizeof(ULONG));
    SAV_Buffer(&_pst_Level->ul_Flags,                               sizeof(ULONG));
    SAV_Buffer(&_pst_Level->ul_DiffuseColor,                        sizeof(ULONG));
    SAV_Buffer(&_pst_Level->ul_SpecularColor,                       sizeof(ULONG));
    SAV_Buffer(&_pst_Level->f_SpecularExp,                          sizeof(FLOAT));
    SAV_Buffer(&ul_NormalMapKey,                                    sizeof(ULONG));
    SAV_Buffer(&ul_SpecularMapKey,                                  sizeof(ULONG));
    SAV_Buffer(&ul_EnvMapKey,                                       sizeof(ULONG));

    if (ul_Version >= 10)
        SAV_Buffer(&ul_MossMapKey,                                  sizeof(ULONG));

    SAV_Buffer(&ul_DetailNMapKey,                                   sizeof(ULONG));
    SAV_Buffer(&_pst_Level->f_SpecularBias,                         sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_NormalMapTransform.f_ScaleU,         sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_NormalMapTransform.f_ScaleV,         sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_NormalMapTransform.f_SpeedU,         sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_NormalMapTransform.f_SpeedV,         sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_NormalMapTransform.f_Angle,          sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_NormalMapTransform.f_RollSpeed,      sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_SpecularMapTransform.f_ScaleU,       sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_SpecularMapTransform.f_ScaleV,       sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_SpecularMapTransform.f_SpeedU,       sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_SpecularMapTransform.f_SpeedV,       sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_SpecularMapTransform.f_Angle,        sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_SpecularMapTransform.f_RollSpeed,    sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_DetailNMapTransform.f_ScaleU,        sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_DetailNMapTransform.f_ScaleV,        sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_DetailNMapTransform.f_SpeedU,        sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->st_DetailNMapTransform.f_SpeedV,        sizeof(FLOAT));
    SAV_Buffer(&_pst_Level->ul_EnvMapColor,                         sizeof(ULONG));

    if (ul_Version >= 10)
        SAV_Buffer(&_pst_Level->ul_MossMapColor,                    sizeof(ULONG));

    if (ul_Version >= 15)
        SAV_Buffer(&_pst_Level->f_MossSpecularFactor,               sizeof(FLOAT));

    SAV_Buffer(&_pst_Level->uc_DetailNMapStartLOD,                  sizeof(UCHAR));
    SAV_Buffer(&_pst_Level->uc_DetailNMapFullLOD,                   sizeof(UCHAR));
    SAV_Buffer(&_pst_Level->f_DetailNMapStrength,                   sizeof(FLOAT));
	if (ul_Version >= 7)
	{
		SAV_Buffer(&_pst_Level->f_BaseMipMapLODBias,                sizeof(FLOAT));
	    SAV_Buffer(&_pst_Level->f_NormalMipMapLODBias,              sizeof(FLOAT));
	}

    if (ul_Version >= 8)
    {
        SAV_Buffer(&_pst_Level->f_AlphaScale,                       sizeof(FLOAT));
        SAV_Buffer(&_pst_Level->f_AlphaOffset,                      sizeof(FLOAT));
    }

    if (ul_Version >= 11)
    {
        SAV_Buffer(&_pst_Level->f_RimLightWidthMin,                 sizeof(FLOAT));
        SAV_Buffer(&_pst_Level->f_RimLightWidthMax,                 sizeof(FLOAT));
        SAV_Buffer(&_pst_Level->f_RimLightIntensity,                sizeof(FLOAT));
        SAV_Buffer(&_pst_Level->f_RimLightNormalMapRatio,           sizeof(FLOAT));
    }

    if (ul_Version >= 13)
    {
        SAV_Buffer(&_pst_Level->ul_MeshProcessingFlags,             sizeof(ULONG));
        SAV_Buffer(&_pst_Level->f_DisplacementOffset,               sizeof(FLOAT));
        SAV_Buffer(&_pst_Level->f_DisplacementHeight,               sizeof(FLOAT));
        SAV_Buffer(&_pst_Level->f_TessellationArea,                 sizeof(FLOAT));
    }
    if (ul_Version >= 14)
    {
        SAV_Buffer(&_pst_Level->f_SmoothThreshold,                  sizeof(FLOAT));
    }
    if (ul_Version >= 16)
    {
        SAV_Buffer(&_pst_Level->f_ChamferThreshold,                 sizeof(FLOAT));
        SAV_Buffer(&_pst_Level->f_ChamferLength,                    sizeof(FLOAT));
    }
    if (ul_Version >= 17)
    {
        SAV_Buffer(&_pst_Level->f_ChamferWeldThreshold,             sizeof(FLOAT));
    }

#endif // defined(ACTIVE_EDITORS)
#endif // defined(SC_DEV)

    return 0;
}

#if defined(ACTIVE_EDITORS)

// ------------------------------------------------------------------------------------------------
// Name   : MAT_ValidateXeMTLevel
// Params : _pst_Level : Level to validate
// RetVal : None
// Descr. : Validate a Xenon MT layer
// ------------------------------------------------------------------------------------------------
void MAT_ValidateXeMTLevel(MAT_tdst_XeMTLevel* _pst_Level)
{
    ERR_X_Assert(_pst_Level != NULL);

    // Specular
    _pst_Level->f_SpecularExp = MATH_f_FloatLimit(_pst_Level->f_SpecularExp, 0.0001f, 1000.0f);

    // Normal map - No validation

    // Specular map
    // ------------
    _pst_Level->f_SpecularBias = MATH_f_FloatLimit(_pst_Level->f_SpecularBias, -1.0f, 1.0f);

    // Environment map - No validation

    // Detail normal map
    // -----------------
    // Remove any rotation and angle from the transform
    _pst_Level->st_DetailNMapTransform.f_Angle     = 0.0f;
    _pst_Level->st_DetailNMapTransform.f_RollSpeed = 0.0f;

    // Validate the start and full LOD range
    if (_pst_Level->uc_DetailNMapFullLOD < _pst_Level->uc_DetailNMapStartLOD)
    {
        _pst_Level->uc_DetailNMapFullLOD = _pst_Level->uc_DetailNMapStartLOD;
    }

    // Strength
    _pst_Level->f_DetailNMapStrength = MATH_f_FloatLimit(_pst_Level->f_DetailNMapStrength, 0.0f, 1.0f);

    // Mipmap LOD bias
    _pst_Level->f_BaseMipMapLODBias   = MATH_f_FloatLimit(_pst_Level->f_BaseMipMapLODBias,   -1.0f, 2.0f);
    _pst_Level->f_NormalMipMapLODBias = MATH_f_FloatLimit(_pst_Level->f_NormalMipMapLODBias, -1.0f, 2.0f);

    // Rim Light
    _pst_Level->f_RimLightWidthMin       = MATH_f_FloatLimit(_pst_Level->f_RimLightWidthMin, 0.0f, 1.0f);
    _pst_Level->f_RimLightWidthMax       = MATH_f_FloatLimit(_pst_Level->f_RimLightWidthMax, 0.0f, 1.0f);
    _pst_Level->f_RimLightNormalMapRatio = MATH_f_FloatLimit(_pst_Level->f_RimLightNormalMapRatio, 0.0f, 1.0f);

    // Mesh processing
    _pst_Level->f_DisplacementOffset   = MATH_f_FloatLimit(_pst_Level->f_DisplacementOffset,   -1.0f,       1.0f);
    _pst_Level->f_DisplacementHeight   = MATH_f_FloatLimit(_pst_Level->f_DisplacementHeight,    0.001f,     1.0f);
    _pst_Level->f_TessellationArea     = MATH_f_FloatLimit(_pst_Level->f_TessellationArea,      0.00001f, 100.0f);
    _pst_Level->f_SmoothThreshold      = MATH_f_FloatLimit(_pst_Level->f_SmoothThreshold,       0.0f,     180.0f);
    _pst_Level->f_ChamferThreshold     = MATH_f_FloatLimit(_pst_Level->f_ChamferThreshold,     30.0f,     180.0f);
    _pst_Level->f_ChamferLength        = MATH_f_FloatLimit(_pst_Level->f_ChamferLength,         0.01f,      4.0f);
    _pst_Level->f_ChamferWeldThreshold = MATH_f_FloatLimit(_pst_Level->f_ChamferWeldThreshold,  0.0005f,    1.0f);
}

#endif // defined(ACTIVE_EDITORS)

#endif // defined(_XENON_RENDER)

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
