/*$T EACTframe_msg.cpp GC!1.71 03/01/00 10:48:44 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "EDImainframe.h"
#include "EACTframe.h"
#include "EACTview.h"
#include "EDImsg.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmerge.h"
#include "EDItors/Sources/BROwser/BROstrings.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "OUTput/OUTmsg.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "Res/Res.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EACT_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_tdst_DragDrop       *pst_DragDrop;
    BIG_INDEX               ul_File;
    int                     i_Msg;
    int                     p;
    CRect                   o_Rect;
    OBJ_tdst_GameObject     *pst_GAO;
    ANI_st_GameObjectAnim   *pst_Anim;
    LINK_tdst_Pointer       *p2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_ul_Msg)
    {
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_FATHASCHANGED:
        if(!mpst_CurrentActionKit) return 0;

        /* Have we changed the file ? */
        if(EDI_go_FHCFile.Lookup(mul_CurrentActionKit, i_Msg))
        {
            switch(i_Msg)
            {
            case EDI_FHC_Delete:
                OnClose();
                return TRUE;

            case EDI_FHC_AddUpdate:
            case EDI_FHC_Loaded:
                ul_File = mul_CurrentActionKit;
                OnClose();
                LoadActionKit(ul_File);
                return TRUE;
            }
        }
        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_DATAHASCHANGED:
        if(LINK_gx_PointersJustDeleted.Lookup(mpst_CurrentActionKit, (void * &) p))
            OnClose();
        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_REFRESH:
        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        if((pst_DragDrop->i_Type == EDI_DD_Long) || (pst_DragDrop->i_Type == EDI_DD_User))
            return FALSE;

        /* A data */
        if(pst_DragDrop->i_Type == EDI_DD_Data)
        {
            _ul_Param2 = (ULONG) pst_DragDrop->i_Param2;

        case EDI_MESSAGE_CANSELDATA:
            p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
            if(!p2) return FALSE;
            if(p2->i_Type == LINK_C_ENG_ActionKit) return TRUE;
            if(p2->i_Type == LINK_C_ENG_GameObjectOriented)
            {
                pst_GAO = (OBJ_tdst_GameObject *) p2->pv_Data;
                if(OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Anims))
                {
                    pst_Anim = (ANI_st_GameObjectAnim *) pst_GAO->pst_Base->pst_GameObjectAnim;
                    if(pst_Anim->pst_ActionKit) return TRUE;
                }
            }

            return FALSE;
        }

        /* Animation */
        if(BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtAnimation))
        {
            if(!mpst_CurrentActionKit) return FALSE;
            if(!mpst_CurrentActionItem) return FALSE;
            mpo_View->GetDlgItem(IDC_EDIT_ANIM)->GetWindowRect(&o_Rect);
            if(o_Rect.PtInRect(pst_DragDrop->o_Pt)) return TRUE;
            return FALSE;
        }

        /* Game object */
        if(BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtGameObject))
        {
            pst_GAO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(pst_DragDrop->ul_FatFile));
            if(pst_GAO && ((int) pst_GAO != -1))
            {
                if(OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Anims))
                {
                    pst_Anim = (ANI_st_GameObjectAnim *) pst_GAO->pst_Base->pst_GameObjectAnim;
                    if(pst_Anim->pst_ActionKit) return TRUE;
                    return FALSE;
                }
            }
        }

        _ul_Param2 = pst_DragDrop->ul_FatFile;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELFILE:
        if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtActionKit))
        {
            if(LOA_ul_SearchAddress(BIG_PosFile(_ul_Param2)) != BIG_C_InvalidIndex)
                return TRUE;
        }

        return FALSE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_ENDDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        _ul_Param2 = pst_DragDrop->ul_FatFile;

        if(pst_DragDrop->i_Type == EDI_DD_Data)
        {
            _ul_Param2 = (ULONG) pst_DragDrop->i_Param2;
	case EDI_MESSAGE_SELDATA:
            p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
            ERR_X_Assert(p2);
            _ul_Param2 = LOA_ul_SearchIndexWithAddress((ULONG) p2->pv_Data);
			if(_ul_Param2 == BIG_C_InvalidIndex) break;
        }

    case EDI_MESSAGE_SELFILE:

        /* GAO */
        if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGameObject))
        {
            pst_GAO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(_ul_Param2));
            if(pst_GAO && ((int) pst_GAO != -1))
            {
                if(pst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
                {
                    pst_Anim = (ANI_st_GameObjectAnim *) pst_GAO->pst_Base->pst_GameObjectAnim;
                    _ul_Param2 = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_ActionKit);
                    if(_ul_Param2 == BIG_C_InvalidIndex) break;
                }
            }
        }

        /* Action kit */
        if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtActionKit))
        {
            M_MF()->AddHistoryFile(this, BIG_FileKey(_ul_Param2));
            LoadActionKit(_ul_Param2);
            break;
        }

        /* Animation */
        if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtAnimation))
        {
            LoadAnim(_ul_Param2);
            break;
        }

        return TRUE;
    }

    return TRUE;
}

#endif /* ACTIVE_EDITORS */
