/*$T MATSingle.c GC!1.71 02/14/00 15:50:16 */

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
#include "MATerial/MATSingle.h"
#include "GRObject/GROstruct.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif
void MAT_Validate_Single(MAT_tdst_Single *_pst_Mat)
{
    _pst_Mat ->ul_ValidateMask = MAT_ValidateMask_Diffuse | MAT_ValidateMask_RLI;
	if (_pst_Mat ->f_Opacity < 0.98f)
	{
		_pst_Mat ->ul_ValidateMask |= MAT_ValidateMask_Transparency;
	} else
	if(_pst_Mat->l_TextureId != -1)
	{
	    TEX_tdst_Data	        *pst_TexData;
		pst_TexData = &TEX_gst_GlobalList.dst_Texture[_pst_Mat->l_TextureId];
		if(pst_TexData->uw_Flags & TEX_uw_Alpha) 
			_pst_Mat ->ul_ValidateMask |= MAT_ValidateMask_Transparency;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_Single *MAT_pst_CreateSingleFromBuffer(GRO_tdst_Struct *_pst_Id, char **ppc_Buffer, WOR_tdst_World *_pst_World)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_Single *pst_Material;
    TEX_tdst_Data   *pst_Texture;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEMpro_StartMemRaster();
    pst_Material = (MAT_tdst_Single *) MEM_p_Alloc(sizeof(MAT_tdst_Single));
    L_memset(pst_Material, 0, sizeof(MAT_tdst_Single));

    GRO_Struct_Init(&pst_Material->st_Id, _pst_Id->i->ul_Type);

	pst_Material->ul_Ambiant = LOA_ReadULong(ppc_Buffer);
	pst_Material->ul_Diffuse = LOA_ReadULong(ppc_Buffer);
	pst_Material->ul_Specular = LOA_ReadULong(ppc_Buffer);
	pst_Material->f_SpecularExp = LOA_ReadFloat(ppc_Buffer);
	pst_Material->f_Opacity = LOA_ReadFloat(ppc_Buffer);
	pst_Material->ul_Flags = LOA_ReadULong(ppc_Buffer);
	pst_Material->l_TextureId = LOA_ReadLong(ppc_Buffer);
	pst_Material->ul_ValidateMask = LOA_ReadULong(ppc_Buffer);
	
#if !defined(XML_CONV_TOOL)
    if(pst_Material->l_TextureId != -1)
    {
        pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_Material->l_TextureId);
        if(pst_Texture)
            pst_Material->l_TextureId = pst_Texture->w_Index;
        else
        {
            pst_Material->l_TextureId = TEX_w_List_AddTexture(&TEX_gst_GlobalList, pst_Material->l_TextureId, 1);
        }
    }

    if(pst_Material->ul_Flags > 1) pst_Material->ul_Flags = 0;

	MEMpro_StopMemRaster(MEMpro_Id_GDK_SingleTexture);
	MAT_Validate_Single((MAT_tdst_Single *)pst_Material);
#endif 
	return pst_Material;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_Single *MAT_pst_CreateSingle(char *_sz_Name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_Single *pst_Material;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Material = (MAT_tdst_Single *) MEM_p_Alloc(sizeof(MAT_tdst_Single));
    L_memset(pst_Material, 0, sizeof(MAT_tdst_Single));

    GRO_Struct_Init(&pst_Material->st_Id, GRO_MaterialSingle);
    GRO_Struct_SetName(&pst_Material->st_Id, _sz_Name);
	MAT_Validate_Single((MAT_tdst_Single *)pst_Material);
    return pst_Material;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MAT_p_CreateSingleDefault( void )
{
    return (void *) MAT_pst_CreateSingle( NULL );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG MAT_l_SaveSingleInBuffer(MAT_tdst_Single *_pst_Mat, TEX_tdst_List *_pst_TexList)
{
#ifdef ACTIVE_EDITORS

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_Data   *_pst_Texture;
    LONG            l_SaveTextureId;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GRO_Struct_Save(&_pst_Mat->st_Id);

    l_SaveTextureId = _pst_Mat->l_TextureId;
#if !defined(XML_CONV_TOOL)
	if(_pst_Mat->l_TextureId != -1)
    {
        _pst_Texture = &_pst_TexList->dst_Texture[_pst_Mat->l_TextureId];
        _pst_Mat->l_TextureId = _pst_Texture->ul_Key;
    }
#endif
    SAV_Buffer(((char *) _pst_Mat) + sizeof(GRO_tdst_Struct), sizeof(MAT_tdst_Single) - sizeof(GRO_tdst_Struct));
    _pst_Mat->l_TextureId = l_SaveTextureId;
#endif
    return 0;
}

#ifndef PSX2_TARGET
#ifndef _GAMECUBE
#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_Single *MAT_p_CreateSingleFromMad(MAD_StandarMaterial *_pst_MadMat)
{
//#ifdef ACTIVE_EDITORS
#if defined(ACTIVE_EDITORS) && !defined(XML_CONV_TOOL)

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_Single *pst_SingleMat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SingleMat = (MAT_tdst_Single *) MAT_pst_CreateMaterial(GRO_MaterialSingle, _pst_MadMat->MatRef.Name);
    pst_SingleMat->l_TextureId = _pst_MadMat->MadTexture;
    if(pst_SingleMat->l_TextureId != -1)
    {
        pst_SingleMat->l_TextureId = TEX_gal_MadToGlobalIndex[pst_SingleMat->l_TextureId];
        if(pst_SingleMat->l_TextureId == 0xffff)
            pst_SingleMat->l_TextureId = -1;
    }

    pst_SingleMat->ul_Ambiant = _pst_MadMat->Ambient;
    pst_SingleMat->ul_Diffuse = _pst_MadMat->Diffuse;
    pst_SingleMat->ul_Specular = _pst_MadMat->Specular;
    pst_SingleMat->f_SpecularExp = _pst_MadMat->SelfIllum;
    pst_SingleMat->f_Opacity = _pst_MadMat->Opacity;
    pst_SingleMat->ul_Flags = _pst_MadMat->MaterialFlag;

	MAT_Validate_Single((MAT_tdst_Single *)pst_SingleMat);
    return pst_SingleMat;
#endif
    return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAD_StandarMaterial *MAT_p_SingleToMad(MAT_tdst_Single *_pst_Mat, WOR_tdst_World *_pst_World)
{
#ifdef ACTIVE_EDITORS

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_StandarMaterial *pst_MadMat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_MALLOC(MAD_StandarMaterial, pst_MadMat, 1);

    pst_MadMat->MatRef.MaterialType = ID_MAT_Standard;
    L_strcpy(pst_MadMat->MatRef.Name, GRO_sz_Struct_GetName( &_pst_Mat->st_Id ));
    pst_MadMat->Diffuse = _pst_Mat->ul_Diffuse;
    pst_MadMat->Ambient = _pst_Mat->ul_Ambiant;
    pst_MadMat->Specular = _pst_Mat->ul_Specular;
    pst_MadMat->SelfIllum = _pst_Mat->f_SpecularExp;
    pst_MadMat->Opacity = _pst_Mat->f_Opacity;
    pst_MadMat->MaterialFlag = _pst_Mat->ul_Flags;
    pst_MadMat->MadTexture = _pst_Mat->l_TextureId;

    return pst_MadMat;
#endif
    return NULL;
}
#endif
#endif /* _GAMECUBE */
#endif /* ! PSX2_TARGET */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MAT_p_SingleDuplicate(MAT_tdst_Single *_pst_Mat, char *_sz_Path, char *_sz_Name, ULONG _ul_Flags)
{
#ifdef ACTIVE_EDITORS

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_Single *pst_SingleMat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SingleMat = (MAT_tdst_Single *) MAT_pst_CreateMaterial(GRO_MaterialSingle, NULL);

    pst_SingleMat->f_Opacity = _pst_Mat->f_Opacity;
    pst_SingleMat->f_SpecularExp = _pst_Mat->f_SpecularExp;
    pst_SingleMat->l_TextureId = _pst_Mat->l_TextureId;
    pst_SingleMat->ul_Ambiant = _pst_Mat->ul_Ambiant;
    pst_SingleMat->ul_Diffuse = _pst_Mat->ul_Diffuse;
    pst_SingleMat->ul_ValidateMask = _pst_Mat->ul_ValidateMask;
    pst_SingleMat->ul_Flags = _pst_Mat->ul_Flags;
    pst_SingleMat->ul_Specular = _pst_Mat->ul_Specular;

    GRO_ul_Struct_FullSave(&pst_SingleMat->st_Id, _sz_Path, NULL, &TEX_gst_GlobalList);

    return pst_SingleMat;
#endif
    return NULL;
}
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
