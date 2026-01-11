/*$T MATframe_Convert.cpp GC!1.71 01/24/00 15:57:45 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#define ACTION_GLOBAL
#include "BASe/MEMory/MEM.h"
#include "EDImainframe.h"
#include "GEOmetric/GEOload.h"

#include "MATframe.h"
#include "GraphicDK/Sources/MATerial/MATstruct.h"
#include "GraphicDK/Sources/MATerial/MATmultitexture.h"
#ifdef JADEFUSION
#include "GraphicDK/Sources/TEXture/TEXcubemap.h"
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define TEXTURE_ID_TO_NAME(a, b)    strcpy(b, BIG_NameFile(BIG_ul_SearchKeyToFat(a)));
#define MAT_GetPos MAT_GetSpeed

#ifdef JADEFUSION
FLOAT XeConvertToPercentage(FLOAT _fValue, FLOAT _fMin, FLOAT _fMax)
{
    _fValue = MATH_f_FloatLimit(_fValue, _fMin, _fMax);
    return 100.0f * ((_fValue - _fMin) / (_fMax - _fMin));
}

FLOAT XeConvertFromPercentage(FLOAT _fPercentage, FLOAT _fMin, FLOAT _fMax)
{
    _fPercentage = MATH_f_FloatLimit(_fPercentage, 0.0f, 100.0f);
    return _fMin + ((_fPercentage / 100.0f) * (_fMax - _fMin));
}
#endif
/*
 =======================================================================================================================
    Convert a JADE Material to a MUTEX material
 =======================================================================================================================
 */
#ifdef JADEFUSION
void EMAT_cl_Frame::GRM_To_MUTEX(MAT_tdst_UndoStruct* pst_CDO, MAT_tdst_MultiTexture* pst_Multi_Sample)
#else
void EMAT_cl_Frame::GRM_To_MUTEX(void)
#endif
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_Single         *Single;
    MAT_tdst_MultiTexture   *Mtt;
    MAT_tdst_MTLevel        *MttL;
    BIG_KEY                 ul_Key;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    //POPOWARNING
	//ATTENTION pour eviter de retartiner le code,
	//dans un 1er temps pour la fusion passe d'une structure par un pointeur
	//au final integrer le undo par pointeur...
#ifndef JADEFUSION
	MAT_tdst_UndoStruct* pst_CDO;
	pst_CDO = &st_CDO;
#endif
    if(!pst_CDO->pst_ACTIVE_GRO) return;
    if(pst_CDO->pst_ACTIVE_GRO->st_Id.i->ul_Type == GRO_MaterialSingle)
    {
        /* Then I transforme this in a Multitexture Material */
        if(pst_Multi_Sample == NULL)
        {
            pst_Multi_Sample = MAT_pst_CreateMultiTexture("Sample multitexture");
        }

        pst_CDO->pst_ACTIVE_GRO->st_Id.i = pst_Multi_Sample->st_Id.i;
        if(((MAT_tdst_Single *) pst_CDO->pst_ACTIVE_GRO)->l_TextureId != -1)
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            short                   SaveID;
            MAT_tdst_MultiTexture   *pst_MLTTX;
            TEX_tdst_Data           *pst_TexData;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

            pst_MLTTX = ((MAT_tdst_MultiTexture *) pst_CDO->pst_ACTIVE_GRO);
            SaveID = (short) ((MAT_tdst_Single *) pst_CDO->pst_ACTIVE_GRO)->l_TextureId;
            pst_MLTTX->pst_FirstLevel = (MAT_tdst_MTLevel *) MEM_p_Alloc(sizeof(MAT_tdst_MTLevel));
            L_memset(pst_MLTTX->pst_FirstLevel, 0, sizeof(MAT_tdst_MTLevel));
            pst_MLTTX->pst_FirstLevel->s_TextureId = SaveID;
            pst_MLTTX->pst_FirstLevel->ul_Flags = MAT_C_DefaultFlag;

            pst_TexData = &TEX_gst_GlobalList.dst_Texture[SaveID];
            if(pst_TexData->uw_Flags & TEX_uw_Alpha)
            {
                MAT_SET_Blending(pst_MLTTX->pst_FirstLevel->ul_Flags, MAT_Cc_Op_Alpha);
            }

            if(pst_TexData->uw_Flags & TEX_uw_AlphaTest)
            {
                pst_MLTTX->pst_FirstLevel->ul_Flags |= MAT_Cul_Flag_AlphaTest;
            }

            pst_MLTTX->pst_FirstLevel->ScaleSPeedPosU = MAT_Cc_Identity;
            pst_MLTTX->pst_FirstLevel->ScaleSPeedPosV = MAT_Cc_Identity;
        }
        else
            ((MAT_tdst_MultiTexture *) pst_CDO->pst_ACTIVE_GRO)->pst_FirstLevel = NULL;
    }

    switch(pst_CDO->pst_ACTIVE_GRO->st_Id.i->ul_Type)
    {
    /*$2--------------------------------------------------------------------------------------------------------------*/
    case GRO_MaterialSingle:
        strcpy(pst_CDO->Dest.MaterialName, GRO_sz_Struct_GetName( &pst_CDO->pst_ACTIVE_GRO->st_Id ) );
        Single = (MAT_tdst_Single *) pst_CDO->pst_ACTIVE_GRO;
        if(Single->l_TextureId != -1)
        {
            if(Single->l_TextureId < TEX_gst_GlobalList.l_NumberOfTextures)
            {
                pst_CDO->Dest.AllLine[0].TEXTURE_ID = Single->l_TextureId;
                ul_Key = TEX_gst_GlobalList.dst_Texture[Single->l_TextureId].ul_Key;
                if(BIG_ul_SearchKeyToFat(ul_Key) == -1)
                {
                    sprintf(pst_CDO->Dest.AllLine[0].TextureName, "ERROR (%x)", ul_Key);
                }
                else
                {
                    TEXTURE_ID_TO_NAME(ul_Key, pst_CDO->Dest.AllLine[0].TextureName);
                }
            }
        }

         pst_CDO->Dest.NumberOfSubTextures = 0;
        if(pst_CDO->Dest.AllLine[0].TextureName[0] != 0)
            pst_CDO->Dest.NumberOfSubTextures = 1;
        pst_CDO->Dest.DColor = Single->ul_Diffuse;
        pst_CDO->Dest.AColor = Single->ul_Ambiant;
        pst_CDO->Dest.SColor = Single->ul_Specular;
        pst_CDO->Dest.Opacity = Single->f_Opacity;
        pst_CDO->Dest.MaterialFlag = Single->ul_Flags;
        pst_CDO->Dest.AllLine[0].UVSource = 0;    /* Defined in MUTEX_UVSources */
        pst_CDO->Dest.AllLine[0].bUIsASpeed = 0;
        pst_CDO->Dest.AllLine[0].UPos = 0.0f;
        pst_CDO->Dest.AllLine[0].bVIsASpeed = 0;
        pst_CDO->Dest.AllLine[0].VPos = 0.0f;
        pst_CDO->Dest.AllLine[0].UScale = 1.0f;
        pst_CDO->Dest.AllLine[0].VScale = 1.0f;
        pst_CDO->Dest.AllLine[0].Rotation = 0.0f;
        break;

    /*$2--------------------------------------------------------------------------------------------------------------*/
    case GRO_MaterialMulti:
        pst_CDO->Dest.NumberOfSubTextures = 0;
        break;
    case GRO_MaterialMultiTexture:
        strcpy(pst_CDO->Dest.MaterialName, GRO_sz_Struct_GetName( &pst_CDO->pst_ACTIVE_GRO->st_Id ) );
        Mtt = (MAT_tdst_MultiTexture *) pst_CDO->pst_ACTIVE_GRO;
        pst_CDO->Dest.DColor = Mtt->ul_Diffuse;
        pst_CDO->Dest.AColor = Mtt->ul_Ambiant;
        pst_CDO->Dest.SColor = Mtt->ul_Specular;
        pst_CDO->Dest.Opacity = Mtt->f_Opacity;
        pst_CDO->Dest.MaterialFlag = Mtt->ul_Flags;
        pst_CDO->Dest.NumberOfSubTextures = 0;
		pst_CDO->Dest.Sound = Mtt->uc_Sound;
        MttL = Mtt->pst_FirstLevel;
        while(MttL != NULL)
        {
            /*~~~~~~~~~~~~~~~~~~~~~*/
            ULONG   Flag;
            /*~~~~~~~~~~~~~~~~~~~~~*/

            if ( (MttL->s_TextureId != -1) && ((LONG) MttL->s_TextureId < TEX_gst_GlobalList.l_NumberOfTextures) )
            {
				pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].TEXTURE_ID = (LONG) MttL->s_TextureId;

                ul_Key = TEX_gst_GlobalList.dst_Texture[(LONG) MttL->s_TextureId].ul_Key;
                if(BIG_ul_SearchKeyToFat(ul_Key) == -1)
                {
					sprintf
                    (
						pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].TextureName,
                        "ERROR (%x)",
                        ul_Key
                    );
				}
                else
                {
					TEXTURE_ID_TO_NAME(ul_Key, pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].TextureName);
				}
            }
			else
			{
				pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].TEXTURE_ID = -1;
				sprintf( pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].TextureName, "No or Bad Texture" );
			}

            Flag = MAT_GET_FLAG(MttL->ul_Flags);
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].bInactive = 0;
            if(Flag & MAT_Cul_Flag_InActive   ) pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].bInactive = 1;
			pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].CullUV =  0;


            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].TextureFlags = MAT_GET_FLAG(MttL->ul_Flags);
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].TextureTransparency = MAT_GET_Blending(MttL->ul_Flags);
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].TextureBlending = MAT_GET_ColorOp(MttL->ul_Flags);
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].UVSource = MAT_GET_UVSource(MttL->ul_Flags);
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].UScale = MAT_GetScale(MttL->ScaleSPeedPosU);
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].VScale = MAT_GetScale(MttL->ScaleSPeedPosV);
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].Rotation = MAT_GetRotation
                (
                    MttL->ScaleSPeedPosU,
                    MttL->ScaleSPeedPosV
                );
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].UPos = MAT_GetPos(MttL->ScaleSPeedPosU);
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].VPos = MAT_GetPos(MttL->ScaleSPeedPosV);
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].bUIsASpeed = 0;
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].bVIsASpeed = 0;
            if(MttL->ul_Flags & MAT_Cc_Flag_UDynamicTransEnable)
                pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].bUIsASpeed = 1;
            if(MttL->ul_Flags & MAT_Cc_Flag_VDynamicTransEnable)
                pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].bVIsASpeed = 1;
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].AlphaTestTreshold = (float) MAT_GET_AlphaTresh(MttL->ul_Flags);

			if ( MttL->s_AditionalFlags & MAT_XYZ_Flag_ShiftUsingNormal )
				pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].ShiftFace = 1;
			else
				pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].ShiftFace = 0;

            /* Additiona flags */
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].GizmoFrom = MAT_GET_MatrixFrom(MttL->s_AditionalFlags);
            if(pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].GizmoFrom == MAT_CC_GIZMO)
            {
                pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].GizmoFrom += MAT_GET_GizmoNumber(MttL->s_AditionalFlags);
            }

            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].LocalAlpha = ((float) MAT_GET_LocalAlpha(MttL->s_AditionalFlags)) / 32.0f;
			if ((pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].LocalAlpha * 32.0f) == 31.0f)
				pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].LocalAlpha = 1.0f;

            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].XYZ = MAT_GET_XYZ(MttL->s_AditionalFlags);
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].Negativ = 0;
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].UseScale = 0;
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].Symetric = 0;
            pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].DeductAlpha = 0;
            if(MAT_GET_s_Flags(MttL->s_AditionalFlags) & MAT_Cul_sFlag_USeNegativ)
                pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].Negativ = 1;
            if(MAT_GET_s_Flags(MttL->s_AditionalFlags) & MAT_Cul_sFlag_USeScale)
                pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].UseScale = 1;
            if(MAT_GET_s_Flags(MttL->s_AditionalFlags) & MAT_Cul_sFlag_USeSymetric)
                pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].Symetric = 1;
            /*
			if(MAT_GET_s_Flags(MttL->s_AditionalFlags) & MAT_Cul_sFlag_DeductAlpha)
                pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].DeductAlpha = 1;
			*/
#ifdef JADEFUSION
			// SC: Xenon specific information
            if (MttL->pst_XeLevel != NULL)
            {
                pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].b_XeUseExtendedProperties = 1;
                ConvertXeLevelToMUTEX(&pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].st_XeInfo, MttL->pst_XeLevel);
            }
            else
            {
                pst_CDO->Dest.AllLine[pst_CDO->Dest.NumberOfSubTextures].b_XeUseExtendedProperties = 0;
            }
#endif
            pst_CDO->Dest.NumberOfSubTextures++;
            MttL = MttL->pst_NextLevel;
        }
        break;
    }
#ifndef JADEFUSION
    OnDo();//POPOWARNING ?? est pas sur la version xenon
#endif
}

/*
 =======================================================================================================================
    Convert a MUTEX Material to a JADE material
 =======================================================================================================================
 */
#ifdef JADEFUSION
void EMAT_cl_Frame::MUTEX_To_GRM(MAT_tdst_UndoStruct* pst_CDO)
#else
void EMAT_cl_Frame::MUTEX_To_GRM(void)
#endif
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_Single *Single;
	BIG_KEY			ul_Key;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    //POPOWARNING
	//ATTENTION pour eviter de retartiner le code,
	//dans un 1er temps pour la fusion passe d'une structure par un pointeur
	//au final integrer le undo par pointeur...
#ifndef JADEFUSION
	MAT_tdst_UndoStruct* pst_CDO;
	pst_CDO = &st_CDO;
#endif

    if(!pst_CDO->pst_ACTIVE_GRO) return;
    switch(pst_CDO->pst_ACTIVE_GRO->st_Id.i->ul_Type)
    {
    /*$2--------------------------------------------------------------------------------------------------------------*/
    case GRO_MaterialSingle:
        strcpy(pst_CDO->Dest.MaterialName, GRO_sz_Struct_GetName( &pst_CDO->pst_ACTIVE_GRO->st_Id ) );
        Single = (MAT_tdst_Single *) pst_CDO->pst_ACTIVE_GRO;
        if(Single->l_TextureId != -1)
        {
            if(Single->l_TextureId < TEX_gst_GlobalList.l_NumberOfTextures)
            {
                ul_Key = TEX_gst_GlobalList.dst_Texture[Single->l_TextureId].ul_Key;
                if(BIG_ul_SearchKeyToFat(ul_Key) == -1)
                {
                    sprintf(pst_CDO->Dest.AllLine[0].TextureName, "ERROR (%x)", ul_Key);
                }
                else
                {
                    TEXTURE_ID_TO_NAME(ul_Key, pst_CDO->Dest.AllLine[0].TextureName);
                }
            }
        }

       pst_CDO->Dest.NumberOfSubTextures = 0;
       if(pst_CDO->Dest.AllLine[0].TextureName[0] != 0)
           pst_CDO->Dest.NumberOfSubTextures = 1;
       Single->ul_Diffuse = pst_CDO->Dest.DColor;
       Single->ul_Ambiant = pst_CDO->Dest.AColor;
       Single->ul_Specular = pst_CDO->Dest.SColor;
       Single->f_Opacity = pst_CDO->Dest.Opacity;
       Single->ul_Flags = pst_CDO->Dest.MaterialFlag;

       pst_CDO->Dest.AllLine[0].UVSource = 0;    /* Defined in MUTEX_UVSources */
       pst_CDO->Dest.AllLine[0].bUIsASpeed = 0;
       pst_CDO->Dest.AllLine[0].UPos = 0.0f;
       pst_CDO->Dest.AllLine[0].bVIsASpeed = 0;
       pst_CDO->Dest.AllLine[0].VPos = 0.0f;
       pst_CDO->Dest.AllLine[0].UScale = 1.0f;
       pst_CDO->Dest.AllLine[0].VScale = 1.0f;
       pst_CDO->Dest.AllLine[0].Rotation = 0.0f;
       break;

    /*$2--------------------------------------------------------------------------------------------------------------*/
    case GRO_MaterialMulti:
        break;
    case GRO_MaterialMultiTexture:
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            ULONG           TextureConter;
            MAT_tdst_MultiTexture   *Mtt;
            MAT_tdst_MTLevel        **MttL;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

            strcpy(pst_CDO->Dest.MaterialName, GRO_sz_Struct_GetName( &pst_CDO->pst_ACTIVE_GRO->st_Id ));
            Mtt = (MAT_tdst_MultiTexture *) pst_CDO->pst_ACTIVE_GRO;
            Mtt->ul_Diffuse = pst_CDO->Dest.DColor;
            Mtt->ul_Ambiant = pst_CDO->Dest.AColor;
            Mtt->ul_Specular = pst_CDO->Dest.SColor;
            Mtt->f_Opacity = pst_CDO->Dest.Opacity;
            Mtt->ul_Flags = pst_CDO->Dest.MaterialFlag;
			Mtt->uc_Sound = pst_CDO->Dest.Sound;

            if(pst_CDO->Dest.NumberOfSubTextures == 0)
                Mtt->pst_FirstLevel = NULL;
            MttL = &Mtt->pst_FirstLevel;
            TextureConter = 0;
            for(TextureConter = 0; TextureConter < pst_CDO->Dest.NumberOfSubTextures; TextureConter++)
            {
                if((*MttL) == NULL)
                {
#ifdef JADEFUSION 
					(*MttL) = MAT_pst_CreateMTLevel(Mtt);
#else
					
					(*MttL) = (MAT_tdst_MTLevel *) MEM_p_Alloc(sizeof(MAT_tdst_MTLevel));
                    L_memset((*MttL), 0, sizeof(MAT_tdst_MTLevel));
#endif 
				}
                (*MttL)->s_TextureId = (short) pst_CDO->Dest.AllLine[TextureConter].TEXTURE_ID;
                pst_CDO->Dest.AllLine[TextureConter].TextureFlags &= ~MAT_Cul_Flag_InActive;
                if(pst_CDO->Dest.AllLine[TextureConter].bInactive)
                    pst_CDO->Dest.AllLine[TextureConter].TextureFlags |= MAT_Cul_Flag_InActive;
                MAT_SET_FLAG((*MttL)->ul_Flags, pst_CDO->Dest.AllLine[TextureConter].TextureFlags);
                MAT_SET_Blending((*MttL)->ul_Flags, pst_CDO->Dest.AllLine[TextureConter].TextureTransparency);
                MAT_SET_ColorOp((*MttL)->ul_Flags, pst_CDO->Dest.AllLine[TextureConter].TextureBlending);
                MAT_SET_UVSource((*MttL)->ul_Flags, pst_CDO->Dest.AllLine[TextureConter].UVSource);
                MAT_SetScale(&(*MttL)->ScaleSPeedPosU, pst_CDO->Dest.AllLine[TextureConter].UScale);
                MAT_SetScale(&(*MttL)->ScaleSPeedPosV, pst_CDO->Dest.AllLine[TextureConter].VScale);
                MAT_SetRotation
                (
                    &(*MttL)->ScaleSPeedPosU,
                    &(*MttL)->ScaleSPeedPosV,
                    pst_CDO->Dest.AllLine[TextureConter].Rotation
                );
                MAT_SetPos(&(*MttL)->ScaleSPeedPosU, pst_CDO->Dest.AllLine[TextureConter].UPos);
                MAT_SetPos(&(*MttL)->ScaleSPeedPosV, pst_CDO->Dest.AllLine[TextureConter].VPos);
                (*MttL)->ul_Flags &= ~(MAT_Cc_Flag_UDynamicTransEnable | MAT_Cc_Flag_VDynamicTransEnable);
                if(pst_CDO->Dest.AllLine[TextureConter].bUIsASpeed)
                    (*MttL)->ul_Flags |= MAT_Cc_Flag_UDynamicTransEnable;
                if(pst_CDO->Dest.AllLine[TextureConter].bVIsASpeed)
                    (*MttL)->ul_Flags |= MAT_Cc_Flag_VDynamicTransEnable;
                MAT_SET_AlphaTresh
                (
                    (*MttL)->ul_Flags,
                    (ULONG) pst_CDO->Dest.AllLine[TextureConter].AlphaTestTreshold
                );

				if(pst_CDO->Dest.AllLine[TextureConter].ShiftFace == 1)
					( *MttL )->s_AditionalFlags |= MAT_XYZ_Flag_ShiftUsingNormal;
				else
					( *MttL )->s_AditionalFlags &= ~MAT_XYZ_Flag_ShiftUsingNormal;



                /* Additiona flags */
                if(pst_CDO->Dest.AllLine[TextureConter].GizmoFrom < MAT_CC_GIZMO)
                {
                    MAT_SET_MatrixFrom
                    (
                        (*MttL)->s_AditionalFlags,
                        (unsigned short) pst_CDO->Dest.AllLine[TextureConter].GizmoFrom
                    );
                    MAT_SET_GizmoNumber((*MttL)->s_AditionalFlags, (LONG) (unsigned short) 0);
                }
                else
                {
                    MAT_SET_MatrixFrom((USHORT) (*MttL)->s_AditionalFlags, (LONG) (unsigned short) MAT_CC_GIZMO);
                    MAT_SET_GizmoNumber
                    (
                        (*MttL)->s_AditionalFlags,
                        (unsigned short) (pst_CDO->Dest.AllLine[TextureConter].GizmoFrom - MAT_CC_GIZMO)
                    );
                }

                if(pst_CDO->Dest.AllLine[TextureConter].LocalAlpha == 1.0f)
                {
                    MAT_SET_LocalAlpha
                    (
                        (*MttL)->s_AditionalFlags,
                        (unsigned short) (pst_CDO->Dest.AllLine[TextureConter].LocalAlpha * 31.0f)
                    );
                }
                else
                {
                    MAT_SET_LocalAlpha
                    (
                        (*MttL)->s_AditionalFlags,
                        (unsigned short) (pst_CDO->Dest.AllLine[TextureConter].LocalAlpha * 32.0f)
                    );
                }

                MAT_SET_XYZ((*MttL)->s_AditionalFlags, (unsigned short) pst_CDO->Dest.AllLine[TextureConter].XYZ);
               // (*MttL)->s_AditionalFlags &= ~MAT_Cul_sFlag_MASK;
                if(pst_CDO->Dest.AllLine[TextureConter].Negativ)
                    (*MttL)->s_AditionalFlags |= MAT_Cul_sFlag_USeNegativ;
                if(pst_CDO->Dest.AllLine[TextureConter].UseScale)
                    (*MttL)->s_AditionalFlags |= MAT_Cul_sFlag_USeScale;
                if(pst_CDO->Dest.AllLine[TextureConter].Symetric)
                    (*MttL)->s_AditionalFlags |= MAT_Cul_sFlag_USeSymetric;
                /*
				if(pst_CDO->Dest.AllLine[TextureConter].DeductAlpha)
                    (*MttL)->s_AditionalFlags |= MAT_Cul_sFlag_DeductAlpha;
				*/
#ifdef JADEFUSION
                // SC: Xenon extended layer information
                if (pst_CDO->Dest.AllLine[TextureConter].b_XeUseExtendedProperties)
                {
                    if (!(*MttL)->pst_XeLevel)
                    {
                        (*MttL)->pst_XeLevel = MAT_pst_CreateXeMTLevel();
                        FetchTextureName(pst_CDO->Dest.AllLine[TextureConter].st_XeInfo.sz_NMapName,        MAT_Xe_InvalidTextureId, false);
                        FetchTextureName(pst_CDO->Dest.AllLine[TextureConter].st_XeInfo.sz_SpecularMapName, MAT_Xe_InvalidTextureId, false);
                        FetchTextureName(pst_CDO->Dest.AllLine[TextureConter].st_XeInfo.sz_EnvMapName,      MAT_Xe_InvalidTextureId, true);
                        FetchTextureName(pst_CDO->Dest.AllLine[TextureConter].st_XeInfo.sz_DNMapName,       MAT_Xe_InvalidTextureId, false);
                        FetchTextureName(pst_CDO->Dest.AllLine[TextureConter].st_XeInfo.sz_MossMapName,     MAT_Xe_InvalidTextureId, false);

                        ConvertXeLevelToMUTEX(&pst_CDO->Dest.AllLine[TextureConter].st_XeInfo, (*MttL)->pst_XeLevel);
                    }
                    else
                    {
                        // Update the Xenon structure
                        ConvertMUTEXToXeLevel((*MttL)->pst_XeLevel, &pst_CDO->Dest.AllLine[TextureConter].st_XeInfo);
                    }
                }
                else
                {
                    if ((*MttL)->pst_XeLevel)
                    {
                        MAT_FreeXeMTLevel((*MttL)->pst_XeLevel);
                        (*MttL)->pst_XeLevel = NULL;
                    }
                }

#endif

                MttL = &(*MttL)->pst_NextLevel;
            }

            if ( *MttL != NULL )
            {
                MAT_tdst_MTLevel  *pMttL, *pTemp;
                pMttL = *MttL;
                *MttL = NULL;
                while (pMttL != NULL)
                {
                    pTemp = pMttL->pst_NextLevel;
#ifdef JADEFUSION
                    MAT_DestroyMTLevel(pMttL);
#else 
					MEM_Free( pMttL );
#endif

					pMttL = pTemp;
                }
            }

            /* ancien qui plante à cause du MEM_Free qui réécrit dans la zone libérée
			while ((*MttL) != NULL)
			{
				MAT_tdst_MTLevel        **MttL_Local;
				MttL_Local = MttL;
				MttL = &(*MttL)->pst_NextLevel;
				MEM_Free(*MttL_Local);
				*MttL_Local = NULL;
			}
            */

//            *MttL = NULL;
            MAT_Validate_Multitexture(Mtt);
        }
        break;
    }
}
#ifdef JADEFUSION
// ------------------------------------------------------------------------------------------------
// Name   : EMAT_cl_Frame::FetchTextureName
// Params : _sz_TexName : Texture name [out]
//          _l_TexId    : Texture Id in the global list
//          _b_CubeMap  : Is a cube map?
// RetVal : None
// Descr. : Get the name of a texture
// ------------------------------------------------------------------------------------------------
void EMAT_cl_Frame::FetchTextureName(CHAR* _sz_TexName, LONG _l_TexId, bool _b_CubeMap)
{
    ERR_X_Assert(_sz_TexName != NULL);

    ULONG ul_Key;

    // Default name
    sprintf(_sz_TexName, "None");

    if (_b_CubeMap)
    {
        if (_l_TexId >= 0)
        {
            TEX_tdst_CubeMapInfo* pst_CubeMap = TEX_CubeMap_GetInfo((SHORT)_l_TexId);
            if (pst_CubeMap != NULL)
            {
                ul_Key = pst_CubeMap->ul_Key;

                if (BIG_ul_SearchKeyToFat(ul_Key) != BIG_C_InvalidKey)
                {
                    TEXTURE_ID_TO_NAME(ul_Key, _sz_TexName);
                }
            }
        }
    }
    else if ((_l_TexId >= 0) && (_l_TexId < TEX_gst_GlobalList.l_NumberOfTextures))
    {
        ul_Key = TEX_gst_GlobalList.dst_Texture[_l_TexId].ul_Key;

        if (BIG_ul_SearchKeyToFat(ul_Key) != BIG_C_InvalidKey)
        {
            TEXTURE_ID_TO_NAME(ul_Key, _sz_TexName);
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : EMAT_cl_Frame::ConvertXeLevelToMUTEX
// Params : _pst_Mtx : MUTEX version of the Xenon MT layer [out]
//          _pst_XeLevel : Source Xenon MT layer
// RetVal : None
// Descr. : Convert a Xenon MT layer to a MUTEX editable Xenon layer
// ------------------------------------------------------------------------------------------------
void EMAT_cl_Frame::ConvertXeLevelToMUTEX(MUTEX_XenonTextureLine* _pst_Mtx, const MAT_tdst_XeMTLevel* _pst_XeLevel)
{
    ERR_X_Assert(_pst_Mtx != NULL);
    ERR_X_Assert(_pst_XeLevel != NULL);

    // Flags
    _pst_Mtx->b_NMapTransform   = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapTransform)         != 0);
    _pst_Mtx->b_NMapAbsolute    = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapAbsolute)          != 0);
    _pst_Mtx->b_NMapScrollU     = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapScrollU)           != 0);
    _pst_Mtx->b_NMapScrollV     = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapScrollV)           != 0);
    _pst_Mtx->b_SMapTransform   = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapTransform)         != 0);
    _pst_Mtx->b_SMapAbsolute    = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapAbsolute)          != 0);
    _pst_Mtx->b_SMapScrollU     = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapScrollU)           != 0);
    _pst_Mtx->b_SMapScrollV     = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapScrollV)           != 0);
    _pst_Mtx->b_DNMapTransform  = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapTransform)        != 0);
    _pst_Mtx->b_DNMapScrollU    = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapScrollU)          != 0);
    _pst_Mtx->b_DNMapScrollV    = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapScrollV)          != 0);
    _pst_Mtx->b_NMapDisabled    = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapDisable)           != 0);
    _pst_Mtx->b_SMapDisabled    = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapDisable)           != 0);
    _pst_Mtx->b_EMapDisabled    = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_EMapDisable)           != 0);
    _pst_Mtx->b_DNMapDisabled   = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapDisable)          != 0);
    _pst_Mtx->b_TwoSided        = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_TwoSided)              != 0);
    _pst_Mtx->b_ReflectOnWater  = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_ReflectOnWater)              != 0);
    _pst_Mtx->b_MossMapDisabled = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_MossMapDisable)        != 0);
    _pst_Mtx->b_RimLightEnable  = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_RimLightEnable)        != 0); 
    _pst_Mtx->b_InvertMoss      = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_InvertMoss)            != 0);
    _pst_Mtx->b_GlowEnable      = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_GlowEnable)            != 0); 
    _pst_Mtx->b_RimLightSMapAttenuationEnabled  = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_RimLightSMapAttenuation) != 0); 

	//OFFSET
    _pst_Mtx->b_OffsetMap      = ((_pst_XeLevel->ul_Flags & MAT_Xe_Flag_OffsetMap)            != 0);
	
	// Ambient
    _pst_Mtx->ul_AmbientSel = MAT_Xe_GetAmbientSel(_pst_XeLevel->ul_Flags);

    // Diffuse color source
    _pst_Mtx->ul_DiffuseSourceSel = MAT_Xe_GetDiffuseSel(_pst_XeLevel->ul_Flags);

    // Specular color source
    _pst_Mtx->ul_SpecularSourceSel = MAT_Xe_GetSpecularSel(_pst_XeLevel->ul_Flags);

    // Diffuse
    _pst_Mtx->ul_DiffuseColor = _pst_XeLevel->ul_DiffuseColor;

    // Specular
    _pst_Mtx->ul_SpecularColor = _pst_XeLevel->ul_SpecularColor;
    _pst_Mtx->f_SpecularExp    = XeConvertToPercentage(_pst_XeLevel->f_SpecularExp, MAT_Xe_Specular_Exponent_Min, MAT_Xe_Specular_Exponent_Max);
    _pst_Mtx->f_SpecularBias   = XeConvertToPercentage(_pst_XeLevel->f_SpecularBias, MAT_Xe_Specular_Bias_Min, MAT_Xe_Specular_Bias_Max);

    // Mipmap LOD bias
    _pst_Mtx->f_BaseMipMapLODBias   = _pst_XeLevel->f_BaseMipMapLODBias;
    _pst_Mtx->f_NormalMipMapLODBias = _pst_XeLevel->f_NormalMipMapLODBias;

    // Normal map
    _pst_Mtx->l_NMapId = _pst_XeLevel->l_NormalMapId;
    FetchTextureName(_pst_Mtx->sz_NMapName, _pst_Mtx->l_NMapId, false);

    // Specular map
    _pst_Mtx->l_SpecularMapId = _pst_XeLevel->l_SpecularMapId;
    FetchTextureName(_pst_Mtx->sz_SpecularMapName, _pst_Mtx->l_SpecularMapId, false);

    _pst_Mtx->ul_SpecularMapChannel = MAT_Xe_GetSpecularMapChannel(_pst_XeLevel->ul_Flags);

    // Environment map
    _pst_Mtx->l_EnvMapId = _pst_XeLevel->l_EnvMapId;
    FetchTextureName(_pst_Mtx->sz_EnvMapName, _pst_Mtx->l_EnvMapId, true);

    _pst_Mtx->ul_EnvMapColor = _pst_XeLevel->ul_EnvMapColor;

    // Moss map
    _pst_Mtx->l_MossMapId = _pst_XeLevel->l_MossMapId;
    FetchTextureName(_pst_Mtx->sz_MossMapName, _pst_Mtx->l_MossMapId, false);

    _pst_Mtx->ul_MossMapColor = _pst_XeLevel->ul_MossMapColor;
    _pst_Mtx->f_MossSpecularFactor = _pst_XeLevel->f_MossSpecularFactor;

    // Detail normal map
    _pst_Mtx->l_DNMapId = _pst_XeLevel->l_DetailNMapId;
    FetchTextureName(_pst_Mtx->sz_DNMapName, _pst_Mtx->l_DNMapId, false);
    _pst_Mtx->ul_DNMapLODStart = _pst_XeLevel->uc_DetailNMapStartLOD;
    _pst_Mtx->ul_DNMapLODFull  = _pst_XeLevel->uc_DetailNMapFullLOD;
    _pst_Mtx->f_DNMapStrength  = XeConvertToPercentage(_pst_XeLevel->f_DetailNMapStrength, MAT_Xe_DNMap_Strength_Min, MAT_Xe_DNMap_Strength_Max);

    // Transforms
    _pst_Mtx->st_NMapTransform  = _pst_XeLevel->st_NormalMapTransform;
    _pst_Mtx->st_SMapTransform  = _pst_XeLevel->st_SpecularMapTransform;
    _pst_Mtx->st_DNMapTransform = _pst_XeLevel->st_DetailNMapTransform;

    // Alpha range remapping
    if (_pst_XeLevel->f_AlphaScale != 0.0f)
    {
        _pst_Mtx->f_AlphaStart  = -_pst_XeLevel->f_AlphaOffset * 255.0f / _pst_XeLevel->f_AlphaScale;
        _pst_Mtx->f_AlphaEnd    = (255.0f - (_pst_XeLevel->f_AlphaOffset * 255.0f)) / _pst_XeLevel->f_AlphaScale;
    }
    else
    {
        _pst_Mtx->f_AlphaStart  = 0.0f;
        _pst_Mtx->f_AlphaEnd    = 1.0f;
    }

    // Rim Light
    _pst_Mtx->f_RimLightWidthMin        = _pst_XeLevel->f_RimLightWidthMin;
    _pst_Mtx->f_RimLightWidthMax        = _pst_XeLevel->f_RimLightWidthMax;
    _pst_Mtx->f_RimLightIntensity       = _pst_XeLevel->f_RimLightIntensity;
    _pst_Mtx->f_RimLightNormalMapRatio  = _pst_XeLevel->f_RimLightNormalMapRatio;

    // Xenon Mesh Processing
    _pst_Mtx->b_AllowChamfer         = (_pst_XeLevel->ul_MeshProcessingFlags & GRO_XMPF_EnableChamfer)      != 0;
    _pst_Mtx->b_AllowTessellation    = (_pst_XeLevel->ul_MeshProcessingFlags & GRO_XMPF_EnableTessellation) != 0;
    _pst_Mtx->b_AllowDisplacement    = (_pst_XeLevel->ul_MeshProcessingFlags & GRO_XMPF_EnableDisplacement) != 0;
    _pst_Mtx->f_TessellationArea     = _pst_XeLevel->f_TessellationArea;
    _pst_Mtx->f_DisplacementOffset   = _pst_XeLevel->f_DisplacementOffset;
    _pst_Mtx->f_DisplacementHeight   = _pst_XeLevel->f_DisplacementHeight;
    _pst_Mtx->f_SmoothThreshold      = _pst_XeLevel->f_SmoothThreshold;
    _pst_Mtx->f_ChamferLength        = _pst_XeLevel->f_ChamferLength;
    _pst_Mtx->f_ChamferThreshold     = _pst_XeLevel->f_ChamferThreshold;
    _pst_Mtx->f_ChamferWeldThreshold = _pst_XeLevel->f_ChamferWeldThreshold;
}

// ------------------------------------------------------------------------------------------------
// Name   : EMAT_cl_Frame::ConvertMUTEXToXeLevel
// Params : 
// RetVal : 
// Descr. : Convert a MUTEX editable Xenon layer to a Xenon MT layer
// ------------------------------------------------------------------------------------------------
void EMAT_cl_Frame::ConvertMUTEXToXeLevel(MAT_tdst_XeMTLevel* _pst_XeLevel, const MUTEX_XenonTextureLine* _pst_Mtx)
{
    ERR_X_Assert(_pst_XeLevel != NULL);
    ERR_X_Assert(_pst_Mtx != NULL);

    // Flags
    _pst_XeLevel->ul_Flags = 0;
    if (_pst_Mtx->b_NMapTransform)   _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_NMapTransform;
    if (_pst_Mtx->b_NMapAbsolute)    _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_NMapAbsolute;
    if (_pst_Mtx->b_NMapScrollU)     _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_NMapScrollU;
    if (_pst_Mtx->b_NMapScrollV)     _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_NMapScrollV;
    if (_pst_Mtx->b_SMapTransform)   _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_SMapTransform;
    if (_pst_Mtx->b_SMapAbsolute)    _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_SMapAbsolute;
    if (_pst_Mtx->b_SMapScrollU)     _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_SMapScrollU;
    if (_pst_Mtx->b_SMapScrollV)     _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_SMapScrollV;
    if (_pst_Mtx->b_DNMapTransform)  _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_DNMapTransform;
    if (_pst_Mtx->b_DNMapScrollU)    _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_DNMapScrollU;
    if (_pst_Mtx->b_DNMapScrollV)    _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_DNMapScrollV;
    if (_pst_Mtx->b_NMapDisabled)    _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_NMapDisable;
    if (_pst_Mtx->b_SMapDisabled)    _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_SMapDisable;
    if (_pst_Mtx->b_EMapDisabled)    _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_EMapDisable;
    if (_pst_Mtx->b_DNMapDisabled)   _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_DNMapDisable;
    if (_pst_Mtx->b_TwoSided)        _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_TwoSided;
    if (_pst_Mtx->b_ReflectOnWater)  _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_ReflectOnWater;
    if (_pst_Mtx->b_MossMapDisabled) _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_MossMapDisable;
    if (_pst_Mtx->b_RimLightEnable)  _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_RimLightEnable;
    if (_pst_Mtx->b_RimLightSMapAttenuationEnabled) _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_RimLightSMapAttenuation;
    if (_pst_Mtx->b_InvertMoss)      _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_InvertMoss;
    if (_pst_Mtx->b_GlowEnable)      _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_GlowEnable;

	//OFFSET
    if (_pst_Mtx->b_OffsetMap)      _pst_XeLevel->ul_Flags |= MAT_Xe_Flag_OffsetMap;

    // Ambient
    MAT_Xe_SetAmbientSel(_pst_XeLevel->ul_Flags, _pst_Mtx->ul_AmbientSel);

    // Diffuse color source
    MAT_Xe_SetDiffuseSel(_pst_XeLevel->ul_Flags, _pst_Mtx->ul_DiffuseSourceSel);

    // Specular color source
    MAT_Xe_SetSpecularSel(_pst_XeLevel->ul_Flags, _pst_Mtx->ul_SpecularSourceSel);

    // Diffuse
    _pst_XeLevel->ul_DiffuseColor = _pst_Mtx->ul_DiffuseColor;

    // Specular
    _pst_XeLevel->ul_SpecularColor = _pst_Mtx->ul_SpecularColor;
    _pst_XeLevel->f_SpecularExp    = XeConvertFromPercentage(_pst_Mtx->f_SpecularExp, MAT_Xe_Specular_Exponent_Min, MAT_Xe_Specular_Exponent_Max);
    _pst_XeLevel->f_SpecularBias   = XeConvertFromPercentage(_pst_Mtx->f_SpecularBias, MAT_Xe_Specular_Bias_Min, MAT_Xe_Specular_Bias_Max);

    // Mipmap LOD bias
    _pst_XeLevel->f_BaseMipMapLODBias   = _pst_Mtx->f_BaseMipMapLODBias;
    _pst_XeLevel->f_NormalMipMapLODBias = _pst_Mtx->f_NormalMipMapLODBias;

    // Normal map
    _pst_XeLevel->l_NormalMapId = _pst_Mtx->l_NMapId;

    // Specular map
    _pst_XeLevel->l_SpecularMapId = _pst_Mtx->l_SpecularMapId;
    MAT_Xe_SetSpecularMapChannel(_pst_XeLevel->ul_Flags, _pst_Mtx->ul_SpecularMapChannel);

    // Environment map
    _pst_XeLevel->l_EnvMapId     = _pst_Mtx->l_EnvMapId;
    _pst_XeLevel->ul_EnvMapColor = _pst_Mtx->ul_EnvMapColor;

    // Moss map
    _pst_XeLevel->l_MossMapId     = _pst_Mtx->l_MossMapId;
    _pst_XeLevel->ul_MossMapColor = _pst_Mtx->ul_MossMapColor;
    _pst_XeLevel->f_MossSpecularFactor = _pst_Mtx->f_MossSpecularFactor;

    // Detail normal map
    _pst_XeLevel->l_DetailNMapId        = _pst_Mtx->l_DNMapId;
    _pst_XeLevel->uc_DetailNMapStartLOD = (UCHAR)_pst_Mtx->ul_DNMapLODStart;
    _pst_XeLevel->uc_DetailNMapFullLOD  = (UCHAR)_pst_Mtx->ul_DNMapLODFull;
    _pst_XeLevel->f_DetailNMapStrength   = XeConvertFromPercentage(_pst_Mtx->f_DNMapStrength, MAT_Xe_DNMap_Strength_Min, MAT_Xe_DNMap_Strength_Max);

    // Transforms
    _pst_XeLevel->st_NormalMapTransform   = _pst_Mtx->st_NMapTransform;
    _pst_XeLevel->st_SpecularMapTransform = _pst_Mtx->st_SMapTransform;
    _pst_XeLevel->st_DetailNMapTransform  = _pst_Mtx->st_DNMapTransform;

    // Alpha Threshold
    FLOAT fRange = (_pst_Mtx->f_AlphaEnd - _pst_Mtx->f_AlphaStart);
    if (fRange > 0.0f)
    {
        _pst_XeLevel->f_AlphaScale        = 255.0f / fRange;
        _pst_XeLevel->f_AlphaOffset       = -_pst_XeLevel->f_AlphaScale * _pst_Mtx->f_AlphaStart / 255.0f;
    }
    else
    {
         _pst_XeLevel->f_AlphaScale = 1.0f;
         _pst_XeLevel->f_AlphaOffset = 0.0f;
    }

    // Rim Light
    _pst_XeLevel->f_RimLightWidthMin        = _pst_Mtx->f_RimLightWidthMin;
    _pst_XeLevel->f_RimLightWidthMax        = _pst_Mtx->f_RimLightWidthMax;
    _pst_XeLevel->f_RimLightIntensity       = _pst_Mtx->f_RimLightIntensity;
    _pst_XeLevel->f_RimLightNormalMapRatio  = _pst_Mtx->f_RimLightNormalMapRatio;

    // Xenon Mesh Processing
    _pst_XeLevel->ul_MeshProcessingFlags = 0;
    if (_pst_Mtx->b_AllowChamfer)      _pst_XeLevel->ul_MeshProcessingFlags |= GRO_XMPF_EnableChamfer;
    if (_pst_Mtx->b_AllowTessellation) _pst_XeLevel->ul_MeshProcessingFlags |= GRO_XMPF_EnableTessellation;
    if (_pst_Mtx->b_AllowDisplacement) _pst_XeLevel->ul_MeshProcessingFlags |= GRO_XMPF_EnableDisplacement;

    _pst_XeLevel->f_TessellationArea     = _pst_Mtx->f_TessellationArea;
    _pst_XeLevel->f_DisplacementOffset   = _pst_Mtx->f_DisplacementOffset;
    _pst_XeLevel->f_DisplacementHeight   = _pst_Mtx->f_DisplacementHeight;
    _pst_XeLevel->f_SmoothThreshold      = _pst_Mtx->f_SmoothThreshold;
    _pst_XeLevel->f_ChamferLength        = _pst_Mtx->f_ChamferLength;
    _pst_XeLevel->f_ChamferThreshold     = _pst_Mtx->f_ChamferThreshold;
    _pst_XeLevel->f_ChamferWeldThreshold = _pst_Mtx->f_ChamferWeldThreshold;

    // Validation
    MAT_ValidateXeMTLevel(_pst_XeLevel);
}
#endif
#endif /* ACTIVE_EDITORS */
