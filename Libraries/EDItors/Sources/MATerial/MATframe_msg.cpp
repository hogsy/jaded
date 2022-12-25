/*$T MATframe_msg.cpp GC!1.60 12/23/99 10:40:36 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIpaths.h"
#include "EDImsg.h"
#include "F3Dframe/F3Dframe.h"

#include "F3Dframe/F3Dview.h"

#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORinit.h"

#include "LINKs/LINKtoed.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"

#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "GEOmetric/GEOload.h"

#include "EDImsg.h"
#include "res/res.h"

#include "MATframe_act.h"
#include "MATframe.h"

#include "GraphicDK/Sources/TEXture/TEXfile.h"
#include "GraphicDK/Sources/MATerial/MATmultitexture.h"
#include "SOFT/SOFTpickingbuffer.h"

#ifdef JADEFUSION
#include "GraphicDK/Sources/TEXture/TEXcubemap.h"
#include "GraphicDK/Sources/GDInterface/GDInterface.h"
#include "GraphicDK/Sources/GEOmetric/GEOstaticLOD.h"
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
#ifdef JADEFUSION
extern ULONG    EDI_OUT_gl_ForceSetMode;
#else
extern "C" {extern ULONG    EDI_OUT_gl_ForceSetMode;};
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *EMAT_cl_Frame::psz_OnActionGetBase(void)
{
    return EMAT_asz_ActionBase;
};

static WOR_tdst_World   *MyWorld;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EMAT_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_tdst_DragDrop       *pst_DragDrop;
    LINK_tdst_Pointer       *p2;
    BIG_KEY                 ul_Key;
    MAT_tdst_Material       *pst_GRM;
    ULONG           ul_SubGRM;
    OBJ_tdst_GameObject     *pst_GO;
    MAT_tdst_MultiTexture   *Mtt;
    MAT_tdst_MTLevel        *MttL;
    MAT_tdst_MTLevel        **MttLParse;
    TEX_tdst_Data           *pst_Texture;
    GEO_tdst_Object         *pst_Obj;
    LONG                    element;
    GRO_tdst_Struct         *pst_Gro2;
	CWnd					*po_Wnd;
	CRect					o_Rect;
	POINT					pt;
	int						i_Level;
	ULONG			ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_ul_Msg)
    {
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EDI_MESSAGE_DATAHASCHANGED:
        if (st_CDO.pst_ACTIVE_GRO)
        {
		    ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) st_CDO.pst_ACTIVE_GRO);
		    if(ul_Index == BIG_C_InvalidIndex)
		    {
			    if(IsWindow(m_hWnd))
			    {
					st_CDO.pst_GRO = NULL;
				    st_CDO.pst_ACTIVE_GRO = NULL;
				    ::SendDlgItemMessage(stw_MultiSM, IDC_LIST_MSM, LB_RESETCONTENT, 0, 0);
				    ::SetDlgItemText(stw_MultiSM, IDC_MATNAME, "");
				    MUTEX_SetMat(m_hWnd, NULL);
				    OnSize(0, CurrentCX, CurrentCY);
				    RefreshMenu();
			    }
		    }
        }
		return TRUE;

#ifdef JADEFUSION
	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EDI_MESSAGE_REFRESHDATA:
		GRM_To_MUTEX(&st_CDO, pst_Multi_Sample);
		OnDo();
		MUTEX_SetMat(m_hWnd, &st_CDO.Dest);
		return TRUE;
#endif
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELDATA:
        p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
        if(!p2) break;
        if(p2->i_Type == LINK_C_ENG_GameObjectOriented)
        {
            pst_GO = (OBJ_tdst_GameObject *) _ul_Param2;
            if((pst_GO->pst_Base) && (pst_GO->pst_Base->pst_Visu) && (pst_GO->pst_Base->pst_Visu->pst_Material))
            {
                return TRUE;
            }
        }

        return FALSE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        if(pst_DragDrop->i_Type == EDI_DD_Long) return FALSE;

        if(pst_DragDrop->i_Type == EDI_DD_File)
        {
            _ul_Param2 = pst_DragDrop->ul_FatFile;
            if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
        }
        else if(pst_DragDrop->i_Type == EDI_DD_Data)
        {
            _ul_Param2 = LOA_ul_SearchKeyWithAddress((ULONG) pst_DragDrop->i_Param2);
            if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
            _ul_Param2 = BIG_ul_SearchKeyToFat(_ul_Param2);
        }

    case EDI_MESSAGE_CANSELFILE:
        if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;

        /* Graphic object */
        if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGraphicMaterial)) return TRUE;

        /* Texture */
        if(TEX_l_File_IsFormatSupported(BIG_NameFile(_ul_Param2), -1)) return TRUE;

#ifdef JADEFUSION
        // Cube map
        if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtTextureCubeMap)) return TRUE;
#endif		
		return FALSE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_ENDDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        if(pst_DragDrop->ul_FatFile == BIG_C_InvalidIndex) return NULL;
        _ul_Param2 = pst_DragDrop->ul_FatFile;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELFILE:
        if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGraphicMaterial))
        {
            if(_ul_Param2 == BIG_C_InvalidIndex) return NULL;
            ul_Key = BIG_FileKey(_ul_Param2);
            if(ul_Key == BIG_C_InvalidKey) return NULL;
            LOA_MakeFileRef(ul_Key, (ULONG *) &pst_GRM, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
            LOA_Resolve();
            OnSetMaterial(pst_GRM, 0);

			M_MF()->AddHistoryFile(this, ul_Key);

        }
#ifdef JADEFUSION
        if (BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtTextureCubeMap))
        {
            if((st_CDO.pst_ACTIVE_GRO != NULL) && 
               (st_CDO.pst_ACTIVE_GRO->st_Id.i->ul_Type == GRO_MaterialMultiTexture))
            {
                ul_Key = BIG_FileKey(_ul_Param2);

                Mtt = (MAT_tdst_MultiTexture *) st_CDO.pst_ACTIVE_GRO;

                // Supporting information
                int i_XeTexture = MUTEX_GSBR_XE_NONE;

				/* D&D of a existant texture ? */
				MttL = NULL;
				if(_ul_Msg == EDI_MESSAGE_ENDDRAGDROP)
				{
					po_Wnd = WindowFromPoint(pst_DragDrop->o_Pt);
					if(po_Wnd)
					{
						pt.x = pst_DragDrop->o_Pt.x;
						pt.y = pst_DragDrop->o_Pt.y;
						i_Level = MUTEX_GetSubByRect(m_hWnd, pt);

                        i_XeTexture = MUTEX_ExtractXeTexture(i_Level);
                        i_Level     = MUTEX_ExtractLayer(i_Level);

						if(i_Level != -1)
						{
							MttLParse = &Mtt->pst_FirstLevel;
							while(*MttLParse != NULL)
							{
								if(!i_Level)
								{
									MttL = *MttLParse;
									break;
								}
								i_Level--;
								o_Rect.top += VSIZE;
								o_Rect.bottom += VSIZE;
								MttLParse = &(*MttLParse)->pst_NextLevel;
							}
						}
					}
				}

				if(!MttL)
				{
					MttL = MAT_pst_CreateMTLevel(Mtt);
					MttLParse = &Mtt->pst_FirstLevel;
					while(*MttLParse != NULL)
					{
						MttLParse = &(*MttLParse)->pst_NextLevel;
					}
					*MttLParse = MttL;
					MttL->ul_Flags = MAT_C_DefaultFlag;
					MttL->ScaleSPeedPosU = MAT_Cc_Identity;
					MttL->ScaleSPeedPosV = MAT_Cc_Identity;
				}

                if (i_XeTexture == MUTEX_GSBR_XE_ENVMAP)
                {
                    if(_ul_Param2 == BIG_C_InvalidIndex) return NULL;
                    ul_Key = BIG_FileKey(_ul_Param2);

                    SHORT s_PrevIndex = (SHORT)MttL->pst_XeLevel->l_EnvMapId;

                    MttL->pst_XeLevel->l_EnvMapId = TEX_CubeMap_Add(ul_Key);
                    EDI_OUT_gl_ForceSetMode = 1;

                    if (s_PrevIndex >= 0)
                    {
                        TEX_CubeMap_Remove(s_PrevIndex);
                    }

                    LINK_gb_AllRefreshEnable = FALSE;
                    
					GRM_To_MUTEX(&st_CDO, pst_Multi_Sample);
					OnDo();

					MUTEX_SetMat(m_hWnd, &st_CDO.Dest);
                    LINK_gb_AllRefreshEnable = TRUE;
                    LINK_Refresh();
                    EDI_OUT_gl_ForceSetMode = 0;
                    RefreshMenu();
                }
            }
        }
        else 
#endif
		if(TEX_l_File_IsFormatSupported(BIG_NameFile(_ul_Param2), -1))
		{
            if(st_CDO.pst_ACTIVE_GRO)
            {
                if(st_CDO.pst_ACTIVE_GRO->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
                {
                    if(_ul_Param2 == BIG_C_InvalidIndex) return NULL;
                    ul_Key = BIG_FileKey(_ul_Param2);

                    Mtt = (MAT_tdst_MultiTexture *) st_CDO.pst_ACTIVE_GRO;
#ifdef JADEFUSION
                    // Supporting information
                    int i_XeTexture = MUTEX_GSBR_XE_NONE;
#endif
					/* D&D of a existant texture ? */
					MttL = NULL;
					if(_ul_Msg == EDI_MESSAGE_ENDDRAGDROP)
					{
						po_Wnd = WindowFromPoint(pst_DragDrop->o_Pt);
						if(po_Wnd)
						{
							pt.x = pst_DragDrop->o_Pt.x;
							pt.y = pst_DragDrop->o_Pt.y;
							i_Level = MUTEX_GetSubByRect(m_hWnd, pt);
#ifdef JADEFUSION
                            i_XeTexture = MUTEX_ExtractXeTexture(i_Level);
                            i_Level     = MUTEX_ExtractLayer(i_Level);
#endif
							if(i_Level != -1)
							{
								MttLParse = &Mtt->pst_FirstLevel;
								while(*MttLParse != NULL)
								{
									if(!i_Level)
									{
										MttL = *MttLParse;
										break;
									}
									i_Level--;
									o_Rect.top += VSIZE;
									o_Rect.bottom += VSIZE;
									MttLParse = &(*MttLParse)->pst_NextLevel;
								}
							}
						}
					}

					if(!MttL)
					{
#ifdef JADEFUSION
						MttL = MAT_pst_CreateMTLevel(Mtt);
#else
						MttL = (MAT_tdst_MTLevel *) MEM_p_Alloc(sizeof(MAT_tdst_MTLevel));
						L_memset(MttL, 0, sizeof(MAT_tdst_MTLevel));
#endif
						MttLParse = &Mtt->pst_FirstLevel;
						while(*MttLParse != NULL)
						{
							MttLParse = &(*MttLParse)->pst_NextLevel;
						}
						*MttLParse = MttL;
						MttL->ul_Flags = MAT_C_DefaultFlag;
						MttL->ScaleSPeedPosU = MAT_Cc_Identity;
						MttL->ScaleSPeedPosV = MAT_Cc_Identity;
					}
#ifdef JADEFUSION
					SHORT s_TextureIndex;
                    BOOL  bNewTexture = FALSE;
#endif
                    pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, ul_Key);
                    if(pst_Texture)
#ifdef JADEFUSION
                        s_TextureIndex = pst_Texture->w_Index;
#else
						MttL->s_TextureId = pst_Texture->w_Index;
#endif
					else
					{
#ifdef JADEFUSION 
						s_TextureIndex = TEX_w_List_AddTexture(&TEX_gst_GlobalList, ul_Key, 1);
                        bNewTexture    = TRUE;
#else
						MttL->s_TextureId = TEX_w_List_AddTexture(&TEX_gst_GlobalList, ul_Key, 1);
						EDI_OUT_gl_ForceSetMode = 1;
#endif
					}

#ifdef JADEFUSION
                    switch (i_XeTexture)
                    {
                        case MUTEX_GSBR_XE_NORMALMAP:
                            MttL->pst_XeLevel->l_NormalMapId = s_TextureIndex;
                            break;

                        case MUTEX_GSBR_XE_SPECULARMAP:
                            MttL->pst_XeLevel->l_SpecularMapId = s_TextureIndex;
                            break;

                        case MUTEX_GSBR_XE_ENVMAP:
                            // Nothing to be done, only accepting cube maps
                            break;

                        case MUTEX_GSBR_XE_MOSSMAP:
                            MttL->pst_XeLevel->l_MossMapId = s_TextureIndex;
                            break;

                        case MUTEX_GSBR_XE_DETAILNMAP:
                            MttL->pst_XeLevel->l_DetailNMapId = s_TextureIndex;
                            break;

                        default:
                            MttL->s_TextureId = s_TextureIndex;
                            break;
                    }
#endif
					LINK_gb_AllRefreshEnable = FALSE;
#ifdef JADEFUSION
					GRM_To_MUTEX(&st_CDO, pst_Multi_Sample);
					OnDo();
#else
					GRM_To_MUTEX();
#endif
					MUTEX_SetMat(m_hWnd, &st_CDO.Dest);
#if defined(_XENON_RENDER)
                    if (GDI_b_IsXenonGraphics())
                    {
                        if (bNewTexture)
                        {
                            GDI_Xe_ForceLoadLastTexture();
                        }
                    }
                    else
                    {
                        EDI_OUT_gl_ForceSetMode = 1;
                    }
#else
                    EDI_OUT_gl_ForceSetMode = 1;
#endif
					LINK_gb_AllRefreshEnable = TRUE;
                    LINK_Refresh();
                    EDI_OUT_gl_ForceSetMode = 0;
                    RefreshMenu();
                }
            }
        }

        return TRUE;
        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELDATA:
        p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
        if(!p2) break;
        if(p2->i_Type == LINK_C_ENG_GameObjectOriented)
        {
            pst_GO = (OBJ_tdst_GameObject *) _ul_Param2;
            if
            (
                (pst_GO->pst_Base) &&
                (OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) &&
                (pst_GO->pst_Base->pst_Visu) &&
                (pst_GO->pst_Base->pst_Visu->pst_Material)
            )
            {
                pst_GRM = (MAT_tdst_Material_ *) pst_GO->pst_Base->pst_Visu->pst_Material;
                ul_SubGRM = 0;
                if(pst_GRM->st_Id.i->ul_Type == GRO_MaterialMulti)
                {
                    if((_ul_Param1) && (pst_GO->pst_Base->pst_Visu->pst_Object))
                    {
                        pst_Gro2 = pst_GO->pst_Base->pst_Visu->pst_Object;
#ifdef JADEFUSION
						// SC: Fetch the first Geo if it is a LOD
                        if (pst_Gro2->i->ul_Type == GRO_GeoStaticLOD)
                        {
                          GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Gro2;

                          if ((pst_LOD->uc_NbLOD > 0) && 
                              (pst_LOD->dpst_Id[0] != NULL) &&
                              (pst_LOD->dpst_Id[0]->i->ul_Type == GRO_Geometric))
                          {
                            pst_Gro2 = pst_LOD->dpst_Id[0];
                          }
                        }
#endif                        
						if(pst_Gro2->i->ul_Type == GRO_Geometric)
                        {
                            pst_Obj = (GEO_tdst_Object *) pst_Gro2;

                            element = (_ul_Param1 & SOFT_Cul_PBQF_ElementMask) >> SOFT_Cul_PBQF_ElementShift;
                            if(element < pst_Obj->l_NbElements)
                                ul_SubGRM = pst_Obj->dst_Element[element].l_MaterialId;
                        }
                    }

					if(((MAT_tdst_Multi *) pst_GRM)->l_NumberOfSubMaterials == 0)
						ul_SubGRM = 0;
					else if ((LONG) ul_SubGRM >= ((MAT_tdst_Multi *) pst_GRM)->l_NumberOfSubMaterials)
						ul_SubGRM = ((MAT_tdst_Multi *) pst_GRM)->l_NumberOfSubMaterials - 1;
						//ul_SubGRM %= ((MAT_tdst_Multi *) pst_GRM)->l_NumberOfSubMaterials;
                }

                OnSetMaterial(pst_GRM, ul_SubGRM);
				ul_Index = LOA_ul_SearchKeyWithAddress((ULONG)pst_GRM);
				if(ul_Index != BIG_C_InvalidKey)
					M_MF()->AddHistoryFile(this, ul_Index);
                return TRUE;
            }
        }

        return FALSE;
    }

    return TRUE;
}

#endif /* ACTIVE_EDITORS */
