/*$T AIframe_msg.cpp GC!1.71 01/27/00 17:02:36 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDImainframe.h"
#include "AIframe.h"
#include "EDImsg.h"
#include "Res/Res.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIpaths.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "EditView/AIleftview.h"
#include "EditView/AIview.h"
#include "AIleftframe.h"
#include "EditView/AIpane.h"

#include "DATaControl/DATCPerforce.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::b_IsModelFile(char *_psz_Name)
{
    /*~~~~~~~~~~~~~~~~~~*/
    char    *psz_Temp;
    /*~~~~~~~~~~~~~~~~~~*/

    psz_Temp = L_strrchr(_psz_Name, '.');
    if(!psz_Temp) return FALSE;
    if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorModel))
        return TRUE;
    if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEngineModel))
        return TRUE;
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::b_IsInstanceFile(char *_psz_Name)
{
    /*~~~~~~~~~~~~~~~~~~*/
    char    *psz_Temp;
    /*~~~~~~~~~~~~~~~~~~*/

    psz_Temp = L_strrchr(_psz_Name, '.');
    if(!psz_Temp) return FALSE;
    if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEngineInstance))
        return TRUE;
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::b_IsAIFile(BIG_INDEX _ul_File)
{
    if(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtAIEditorVars))
        return TRUE;
    if(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtAIEditorFct))
        return TRUE;
    if(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtAIEditorFctLib))
        return TRUE;
    if(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtAIEditorModel))
        return TRUE;
    if(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtAIEngineModel))
        return TRUE;
    if(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtAIEngineInstance))
        return TRUE;
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::SelFile(BIG_INDEX _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Path[BIG_C_MaxLenPath];
    char    asz_Name[BIG_C_MaxLenPath];
    char    *psz_Temp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_LeftView->Invalidate();

    /* D&D of an engine model. Change to editor one */
    if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtAIEngineModel))
    {
        BIG_ComputeFullName(BIG_ParentFile(_ul_Param2), asz_Path);
        psz_Temp = L_strrchr(asz_Path, '/');
        if(!psz_Temp) return FALSE;
        *psz_Temp = 0;
        L_strcpy(asz_Name, BIG_NameFile(_ul_Param2));
        psz_Temp = L_strrchr(asz_Name, '.');
        if(!psz_Temp) return FALSE;
        *psz_Temp = 0;
        L_strcat(asz_Name, EDI_Csz_ExtAIEditorModel);
        _ul_Param2 = BIG_ul_SearchFileExt(asz_Path, asz_Name);
        if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
    }

    /* Have we sel a model ? */
    if(b_IsModelFile(BIG_NameFile(_ul_Param2)))
    {
        if((_ul_Param2 != mul_CurrentEditModel) || (mpst_Instance))
        {
            if(b_CanClose())
            {
				if(!mb_CompileAll) M_MF()->AddHistoryFile(this, BIG_FileKey(_ul_Param2));
                OnClose();
                return SetCurrentModel(_ul_Param2);
            }
        }
    }
    else if(b_IsInstanceFile(BIG_NameFile(_ul_Param2)))
    {
        if(_ul_Param2 != mul_CurrentEditInstance)
        {
            if(b_CanClose())
            {
                if(!mb_CompileAll) M_MF()->AddHistoryFile(this, BIG_FileKey(_ul_Param2));
                OnClose();
                return SetCurrentInstance(_ul_Param2);
            }
        }
    }
    else
    {
        if(_ul_Param2 != mul_CurrentEditFile)
        {
            if(b_CanClose())
            {
                Load(_ul_Param2);
                return TRUE;
            }
        }
    }

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::SelData(BIG_INDEX _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LINK_tdst_Pointer   *p2;
    BIG_KEY             ul_Key;
    BIG_INDEX           ul_File;
    char                asz_Path[BIG_C_MaxLenPath];
    char                asz_Name[BIG_C_MaxLenName];
    char                *psz_Temp;
    AI_tdst_Model       *pst_Model;
    AI_tdst_Instance    *pst_Instance;
    BOOL                b_Ok;
    OBJ_tdst_GameObject *pst_GO;
	BOOL				b_Gener;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_LeftView->Invalidate();

	b_Gener = FALSE;
    p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
    if(!p2) return FALSE;

    /* Is it a gameobject ? */
    if(p2->i_Type == LINK_C_ENG_GameObjectOriented)
    {
        pst_GO = (OBJ_tdst_GameObject *) _ul_Param2;
        _ul_Param2 = (ULONG) pst_GO->pst_Extended->pst_Ai;
        p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
        if(!p2) return FALSE;
    }

    /* Retreive fat index of the file associated with the registered pointer */
    ul_File = BIG_C_InvalidIndex;
    ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) p2->pv_Data);
    if(ul_Key != BIG_C_InvalidIndex)
    {
        ul_Key = BIG_ul_SearchKeyToFat(ul_Key);
        ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);
    }
    else
    {
        /*
         * Unable to retreive file. It's propably a generated object, so we will load its
         * model
         */
        pst_Instance = (AI_tdst_Instance *) _ul_Param2;
        pst_Model = pst_Instance->pst_Model;
        ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Instance->pst_Model);
        if(ul_Key != BIG_C_InvalidKey)
        {
            ul_Key = BIG_ul_SearchKeyToFat(ul_Key);
            ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);
        }
		b_Gener = TRUE;
    }

    if(b_IsModelFile(p2->asz_Name))
    {
        ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);

        /* Compute editor model name */
        BIG_ComputeFullName(BIG_ParentFile(ul_Key), asz_Path);
        psz_Temp = L_strrchr(asz_Path, '/');
        if(*psz_Temp) *psz_Temp = 0;
        L_strcpy(asz_Name, BIG_NameFile(ul_Key));
        psz_Temp = L_strrchr(asz_Name, '.');
        if(*psz_Temp) *psz_Temp = 0;
        L_strcat(asz_Name, EDI_Csz_ExtAIEditorModel);
        ul_File = BIG_ul_SearchFileExt(asz_Path, asz_Name);
        ERR_X_Error(ul_File != BIG_C_InvalidIndex, "Can't find editor model in logical path", asz_Name);

        pst_Instance = NULL;
        pst_Model = (AI_tdst_Model *) _ul_Param2;

		/***********************************
			TODO_Jaffier : The following code should be placed after the "else" statement
			(out of this "if") but there is a problem with the cursor when the user select
			a gao in the 3D view : the "wait" cursor keeps being displayed until the user
			click somewhere.
		 ***********************************/
		// Get stats again to be sure the panes are displayed with the right color.
		if ( DAT_CPerforce::GetInstance()->P4Connect()  )
		{
			// Get P4 file path
			std::vector<std::string> vP4Files;
			std::string asz_P4FilePath;
			DAT_CUtils::GetP4FileFromKey(BIG_FileKey(ul_Key), asz_P4FilePath, DAT_CPerforce::GetInstance()->GetP4Root());
			vP4Files.push_back(asz_P4FilePath);
			DAT_CPerforce::GetInstance()->P4Fstat(vP4Files);
			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
    }
    else
    {
        if(_ul_Param2)
        {
            pst_Instance = (AI_tdst_Instance *) _ul_Param2;
            pst_Model = pst_Instance->pst_Model;
        }

        ul_File = ul_Key;
    }

    /* Si on ne change pas de model, inutile de tout recharger... */
    if(mpst_Model == pst_Model)
    {
        if(mpst_Instance == pst_Instance) return TRUE;
        mpst_Instance = pst_Instance;
        if(!mpst_Instance || b_Gener)
        {
            mul_CurrentEditInstance = BIG_C_InvalidIndex;
        }
        else
        {
            mul_CurrentEditInstance = ul_File;
        }

        SetEngineBreakPoint();
        return TRUE;
    }

    /* Select the file */
    b_Ok = TRUE;
    if(ul_File != BIG_C_InvalidIndex) b_Ok = SelFile(ul_File);
    if(b_Ok)
    {
        /* Set current engine model/instance */
        mpst_Instance = pst_Instance;
        mpst_Model = pst_Model;
        SetEngineBreakPoint();
    }
    return b_Ok;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_tdst_DragDrop   *pst_DragDrop;
    BIG_INDEX           ul_File, ul_Group;
    int                 i_Msg;
    BOOL                b_File, b_Model, b_Instance;
    LINK_tdst_Pointer   *p2;
    BOOL                b_Ok;
    int                 p;
    OBJ_tdst_GameObject *pst_GO;
    CRect               o_Rect;
    char                asz_Temp[BIG_C_MaxLenPath];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_ul_Msg)
    {
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_DATAHASCHANGED:

        if(LINK_gx_PointersJustDeleted.Lookup(mpst_Model, (void * &) p))
        {
            mpst_Model = NULL;
            mpst_Instance = NULL;
            DisplayPaneNames();
        }

        if(LINK_gx_PointersJustDeleted.Lookup(mpst_Instance, (void * &) p))
        {
            mpst_Instance = NULL;
            DisplayPaneNames();
        }

        return 1;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_FATHASCHANGED:
        if
        (
            (mul_CurrentEditFile == BIG_C_InvalidIndex) &&
            (mul_CurrentEditModel == BIG_C_InvalidIndex) &&
            (mul_CurrentEditInstance == BIG_C_InvalidIndex)
        ) return 0;

		OnSave();

        /* Have we changed the file ? */
		RefreshDialogBar();
        b_File = EDI_go_FHCFile.Lookup(mul_CurrentEditFile, i_Msg);
        b_Model = EDI_go_FHCFile.Lookup(mul_CurrentEditModel, i_Msg);
        b_Instance = EDI_go_FHCFile.Lookup(mul_CurrentEditInstance, i_Msg);
        if(b_File || b_Model || b_Instance)
        {
            switch(i_Msg)
            {
            case EDI_FHC_Delete:
                if(b_Model || b_Instance)
                    OnClose(TRUE, TRUE);
                else
                    OnClose(TRUE, FALSE);
                return TRUE;

            case EDI_FHC_Rename:
            case EDI_FHC_Move:
                DisplayPaneNames();
                return TRUE;

            case EDI_FHC_AddUpdate:
            case EDI_FHC_Loaded:
                if(mb_CanReload && b_File)
                {
                    ul_File = mul_CurrentEditFile;
                    OnClose(TRUE, FALSE);
                    Load(ul_File);
                }

                InitListAI();
                return TRUE;
            }
        }
        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        if(pst_DragDrop->i_Type == EDI_DD_Long) return FALSE;
        _ul_Param1 = pst_DragDrop->ul_FatDir;
        _ul_Param2 = pst_DragDrop->ul_FatFile;

        /* D&D in AI list */
        if(mul_CurrentEditModel != BIG_C_InvalidIndex)
        {
            mpo_ListAI->GetWindowRect(&o_Rect);
            if(o_Rect.PtInRect(pst_DragDrop->o_Pt))
            {
                if
                (
                    (BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtAIEditorFct)) ||
                    (BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtAIEditorFctLib)) ||
                    (BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtAIEditorVars))
                ) return TRUE;
            }
        }

        /* Drag & drop of a data */
        if(pst_DragDrop->i_Type == EDI_DD_Data)
        {
            _ul_Param2 = (ULONG) pst_DragDrop->i_Param2;

        case EDI_MESSAGE_CANSELDATA:
            p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
            if(!p2) return FALSE;
            if((p2->i_Type == LINK_C_AI_Model) || (p2->i_Type == LINK_C_AI_Instance))
                return TRUE;
            if(p2->i_Type == LINK_C_ENG_GameObjectOriented)
            {
                pst_GO = (OBJ_tdst_GameObject *) p2->pv_Data;
                if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_AI))
				{
					if(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Ai)
						return TRUE;
				}
            }

            return FALSE;
        }

    case EDI_MESSAGE_CANSELFILE:
        if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
        if(!b_IsAIFile(_ul_Param2)) return FALSE;
        if(_ul_Param2 == mul_CurrentEditFile) return FALSE;
        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_REFRESH:
		mpo_Pane1->Invalidate();
		mpo_Pane2->Invalidate();
		mpo_Pane3->Invalidate();
		DisplayPaneNames();
		OneTrameEnding();
		break;

    case EDI_MESSAGE_REFRESHMENU:
		RefreshMenu();
		break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELDATA:
        b_Ok = SelData(_ul_Param2);
        DisplayPaneNames();
        return b_Ok;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_ENDDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        _ul_Param1 = pst_DragDrop->ul_FatDir;
        _ul_Param2 = pst_DragDrop->ul_FatFile;
        if(pst_DragDrop->i_Type == EDI_DD_Data)
        {
            _ul_Param2 = pst_DragDrop->i_Param2;
            b_Ok = SelData(_ul_Param2);
            DisplayPaneNames();
            return b_Ok;
        }

        /* D&D in AI list ? */
        if(mul_CurrentEditModel != BIG_C_InvalidIndex)
        {
            mpo_ListAI->GetWindowRect(&o_Rect);
            if(o_Rect.PtInRect(pst_DragDrop->o_Pt))
            {
                if
                (
                    (BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtAIEditorFct)) ||
                    (BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtAIEditorFctLib)) ||
                    (BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtAIEditorVars))
                )
                {
                    BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Temp);
                    ul_Group = BIG_ul_SearchFileExt(asz_Temp, BIG_NameFile(mul_CurrentEditModel));
					ERR_X_Error(ul_Group != BIG_C_InvalidIndex, "Can't find editor model in logical path", BIG_NameFile(mul_CurrentEditModel));
                    BIG_AddRefInGroup
                    (
                        ul_Group,
                        BIG_NameFile(pst_DragDrop->ul_FatFile),
                        BIG_FileKey(pst_DragDrop->ul_FatFile)
                    );
                    M_MF()->FatHasChanged();
                    return TRUE;
                }
            }
        }

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELFILE:

        // Je remets cette ligne, sinon ça plante à la compil.
        ul_File = mo_Compiler.ul_GetEngineModelIndex(_ul_Param2, NULL, NULL);

        if(ul_File != BIG_C_InvalidIndex)
        {
            ul_File = LOA_ul_SearchAddress(BIG_PosFile(ul_File));
            if(ul_File != BIG_C_InvalidIndex)
            {
                b_Ok = SelData(ul_File);
                DisplayPaneNames();
                return b_Ok;
            }
        }

        ul_File = LOA_ul_SearchAddress(BIG_PosFile(_ul_Param2));
        if(ul_File != BIG_C_InvalidIndex)
        {
            b_Ok = SelData(ul_File);
            DisplayPaneNames();
            return b_Ok;
        }

        b_Ok = SelFile(_ul_Param2);
        DisplayPaneNames();
        return b_Ok;
    }

    return TRUE;
}

#endif /* ACTIVE_EDITORS */
