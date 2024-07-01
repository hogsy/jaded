/*$T AIview.cpp GC 1.134 12/21/01 15:38:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "../AIframe.h"
#include "AIview.h"
#include "AIleftview.h"
#include "EDImainframe.h"
#include "LINKs/LINKmsg.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AIdebug.h"
#include "EDIpaths.h"
#include "LINKs/LINKstruct_reg.h"
#include "EDImsg.h"
#include "EDIapp.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "DIAlogs\DIAname_dlg.h"

extern BOOL										EDI_gb_AutoHideTip;
extern CMapStringToString						go_PPConstants;
extern CMapStringToPtr							go_PPConstantsFiles;
extern CMapStringToPtr							go_PPConstantsLines;
extern CMapStringToString						go_DefineMacrosFiles;
extern CMapStringToString						go_DefineCstFiles;
extern CMapStringToString						go_DefineCstFilesGlob;
extern CMapStringToPtr							go_DefineMacroNames;
extern CList<tdst_PPMacro *, tdst_PPMacro *>	go_DefineMacros;
extern CMapStringToPtr							go_PPMacros;
extern CMapStringToPtr							go_PPMacrosLines;
extern void										EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);

#ifdef JADEFUSION
extern void									AI_AddWatch(void *, int);
extern AI_tdst_GlobalMessage*					MSG_IDTOP(unsigned int a);
extern void										AI_DelAllWatch(void);
#else
extern "C" void									AI_AddWatch(void *, int);
extern "C" AI_tdst_GlobalMessage				*MSG_IDTOP(int);
extern "C" void                                  AI_DelAllWatch(void);
#endif

extern ULONG									ulRealIdleCounter;
extern POINT									stOldCurPos;

BOOL NNN_ForceNow = FALSE;
BOOL NNN_Local = 0;
BOOL NNN_Deb = 0;

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EAI_cl_View, CRichEditCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(EN_PROTECTED, OnProtected)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_WM_PAINT()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_View::EAI_cl_View(void)
{
	mpo_Fnt = new CFont;
	mi_LastColLine = -1;
	mst_OldSel.cpMin = mst_OldSel.cpMax = 0;
	mb_CharTreat = FALSE;
	mi_LockCount = 0;
	mpsz_LastNameVar = NULL;
	mpi_LastBufVar = NULL;
	mi_LastTypeVar = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_View::~EAI_cl_View(void)
{
	mpo_Fnt->DeleteObject();
	delete mpo_Fnt;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	CRichEditCtrl::OnCreate(lpCreateStruct);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::Init(void)
{
	mi_LastColLine = 0;
	mb_FirstColoring = TRUE;
	SetWindowText("");
	ResetDefaultFormat();
	SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_PROTECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::OnPaint(void)
{
	if(mpo_Frame->mul_CurrentEditFile == BIG_C_InvalidIndex)
	{
		SetBackgroundColor(FALSE, GetSysColor(COLOR_3DSHADOW));
		EnableWindow(FALSE);
	}
	else
	{
		EnableWindow(TRUE);
	}

	mpo_Frame->mb_RefreshLeft = TRUE;
	mpo_Frame->mpo_LeftView->Invalidate();
	CRichEditCtrl::OnPaint();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ConvGao(char *az, OBJ_tdst_GameObject *pst_GAO)
{
	if(((int) pst_GAO) == -1)
	{
		sprintf(az, "INVALID REFERENCE");
	}
	else
	{
		if((int) pst_GAO == AI_C_MainActor0) pst_GAO = AI_gpst_MainActors[0];
		if((int) pst_GAO == AI_C_MainActor1) pst_GAO = AI_gpst_MainActors[1];
#	if !defined(NDEBUG)
		if(pst_GAO && !OBJ_IsValidGAO(pst_GAO))

		{
			sprintf(az, "INVALID GAO");
		}
		else 
#	endif
			if(pst_GAO)
		{
			if(pst_GAO->sz_Name)
				sprintf(az, pst_GAO->sz_Name);
			else
				sprintf(az, "I'm here but without name...");
		}
		else
			sprintf(az, "None");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::DisplayVar(char *_psz_Name, int *pi_Val, int _i_Type, BOOL _b_Now, char *out)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				asz_Msg[1000];
	char				asz_Msg1[1000];
	char				asz_Msg2[256];
	char				asz_Msg3[256];
	char				asz_Msg4[256];
	char				asz_Msg5[256];
	char				asz_Msg6[256];
	char				asz_Msg7[256];
	MATH_tdst_Vector	*pst_Vec;
	OBJ_tdst_GameObject **pst_Obj;
	WAY_tdst_Network	**pst_Net;
	BIG_INDEX			ul_Index;
	OBJ_tdst_GameObject *pst_GAO;
	AI_tdst_Message		*pst_Msg;
	AI_tdst_Function	**pst_Func;
	AI_tdst_GlobalMessage	*pgm;
    TEXT_tdst_Eval      *pEvalTxt;
    int                 i;
    TEXT_tdst_OneText   *pOneTxt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

__try
{
	mpsz_LastNameVar = _psz_Name;
	mpi_LastBufVar = pi_Val;
	mi_LastTypeVar = _i_Type;

	sprintf(asz_Msg, "%s = ", _psz_Name);
	*asz_Msg1 = 0;
	switch(_i_Type)
	{
	case TYPE_MESSAGEID:
		pgm = MSG_IDTOP(((AI_tdst_GlobalMessageId *) pi_Val)->msgid);
		if(pgm) pi_Val = (int *) &pgm->msg;
		else break;

	case TYPE_MESSAGE:
		pst_Msg = (AI_tdst_Message *) pi_Val;

		ConvGao(asz_Msg2, pst_Msg->pst_Sender);
		ConvGao(asz_Msg3, pst_Msg->pst_GAO1);
		ConvGao(asz_Msg4, pst_Msg->pst_GAO2);
		ConvGao(asz_Msg5, pst_Msg->pst_GAO3);
		ConvGao(asz_Msg6, pst_Msg->pst_GAO4);
		ConvGao(asz_Msg7, pst_Msg->pst_GAO5);

		sprintf
		(
			asz_Msg1, 
			"%d | %s\n%s | %s | %s | %s | %s\n%.3f,%.3f,%.3f\n%.3f,%.3f,%.3f\n%.3f,%.3f,%.3f\n%.3f,%.3f,%.3f\n%.3f,%.3f,%.3f\n%d | %d | %d | %d | %d", 
			pst_Msg->i_Id,
			asz_Msg2, /* sender */
			asz_Msg3,
			asz_Msg4,
			asz_Msg5,
			asz_Msg6,
			asz_Msg7,
			pst_Msg->st_Vec1.x, 
			pst_Msg->st_Vec1.y, 
			pst_Msg->st_Vec1.z,
			pst_Msg->st_Vec2.x, 
			pst_Msg->st_Vec2.y, 
			pst_Msg->st_Vec2.z,
			pst_Msg->st_Vec3.x, 
			pst_Msg->st_Vec3.y, 
			pst_Msg->st_Vec3.z,
			pst_Msg->st_Vec4.x, 
			pst_Msg->st_Vec4.y, 
			pst_Msg->st_Vec4.z,
			pst_Msg->st_Vec5.x, 
			pst_Msg->st_Vec5.y, 
			pst_Msg->st_Vec5.z,
			pst_Msg->i_Int1,
			pst_Msg->i_Int2,
			pst_Msg->i_Int3,
			pst_Msg->i_Int4,
			pst_Msg->i_Int5
		);

		break;
	case TYPE_INT:
		if(M_MF()->mst_Ini.b_IntAsHex)
			sprintf(asz_Msg1, "%x", *pi_Val);
		else
			sprintf(asz_Msg1, "%d", *pi_Val);
		break;
	case TYPE_COLOR:
		sprintf(asz_Msg1, "%x", *pi_Val);
		break;
	case TYPE_FLOAT:
		sprintf(asz_Msg1, "%f", *(float *) pi_Val);
		break;
	case TYPE_VECTOR:
		pst_Vec = (MATH_tdst_Vector *) pi_Val;
		sprintf(asz_Msg1, "x = %f  y = %f  z = %f", pst_Vec->x, pst_Vec->y, pst_Vec->z);
		break;
	case TYPE_GAMEOBJECT:
		pst_Obj = (OBJ_tdst_GameObject **) pi_Val;
		pst_GAO = *pst_Obj;
		if(((int) pst_GAO) == -1)
		{
			sprintf(asz_Msg1, "INVALID REFERENCE");
		}
		else
		{
			if((int) pst_GAO == AI_C_MainActor0) pst_GAO = AI_gpst_MainActors[0];
			if((int) pst_GAO == AI_C_MainActor1) pst_GAO = AI_gpst_MainActors[1];
#	if !defined( NDEBUG )
			if ( pst_GAO && !OBJ_IsValidGAO( pst_GAO ) )
			{
				sprintf( asz_Msg1, "INVALID GAO" );
			}
			else
#	endif
				if(pst_GAO)
			{
				if(pst_GAO->sz_Name)
					sprintf(asz_Msg1, pst_GAO->sz_Name);
				else
					sprintf(asz_Msg1, "I'me here but without name...");
			}
			else
				sprintf(asz_Msg1, "None");
		}
		break;
	case TYPE_FUNCTIONREF:
		pst_Func = (AI_tdst_Function **) pi_Val;
		if(((int) *pst_Func) == -1)
		{
			sprintf(asz_Msg1, "INVALID REFERENCE");
		}
		else
		{
			ul_Index = BIG_C_InvalidIndex;
			if(*pst_Func) ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) (*pst_Func));
			if(!(*pst_Func) || ul_Index != BIG_C_InvalidIndex)
			{
				if(*pst_Func)
					sprintf(asz_Msg1, BIG_NameFile(ul_Index));
				else
					sprintf(asz_Msg1, "None");
			}
		}
		break;
	case TYPE_NETWORK:
		pst_Net = (WAY_tdst_Network **) pi_Val;
		if(((int) *pst_Net) == -1)
		{
			sprintf(asz_Msg1, "INVALID REFERENCE");
		}
		else
		{
			ul_Index = BIG_C_InvalidIndex;
			if(*pst_Net) ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) (*pst_Net));
			if(!(*pst_Net) || ul_Index != BIG_C_InvalidIndex)
			{
				if(*pst_Net)
					sprintf(asz_Msg1, BIG_NameFile(ul_Index));
				else
					sprintf(asz_Msg1, "None");
			}
		}
		break;
    
    case TYPE_TEXT:
        sprintf(asz_Msg1, "TEXT NOT LOADED");
        pEvalTxt = (TEXT_tdst_Eval*)pi_Val;
        i = TEXT_i_GetOneTextIndex(pEvalTxt->i_FileKey);
        if(i!=-1)
        {
            pOneTxt = TEXT_gst_Global.pst_AllTexts[i];
            if(pOneTxt)
            {
                i = TEXT_i_GetEntryIndex(pOneTxt, pEvalTxt->i_Id);
                if(i != -1)
                {
                    sprintf(asz_Msg1, pOneTxt->psz_Text + pOneTxt->pst_Ids[i].i_Offset);
                }
            }
        }
        break;
	}
}
__except(1)
{
	sprintf(asz_Msg, "EXCEPTION", _psz_Name);
	asz_Msg1[0] = 0;
}

	if(!out)
	{
		L_strcat(asz_Msg, asz_Msg1);
		if(_b_Now)
			EDI_Tooltip_DisplayMessage(asz_Msg, 1);
		else
			EDI_Tooltip_DisplayMessage(asz_Msg, 500);
	}
	else
	{
		L_strcat(asz_Msg, asz_Msg1);
		L_strcpy(out, asz_Msg);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::DisplayVar(CString o_Word, BOOL _b_Now, int what)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_VarDes		*pst_VarDes;
	int					i, j;
	int					*pi_Val;
	AI_tdst_Function	*pst_Func;
	SCR_tt_ProcedureList	*pst_Proc;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenName];
	char				az[1024];
	BIG_INDEX			ul_Index;
	POSITION			pos;
	CString				o_Name, o_Name1, o_Val;
	CString				o_Str;
	char				*psz_Tmp, *psz_Tmp1;
	int					line;
	short				w_Type;
	int					w_Size;
	CPoint				pt1, pt2;
	CString				nname;
	char				*pz;
	SCR_sc_Procedure	*pt_ProcEd;
	void				*p;
	CDC					*pdc;
	extern CString EDIA_cl_Completion_MakeFctName(CDC *pdc, int p, BOOL);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Function ? */
	if(mpo_Frame->mo_FunctionList.Lookup(o_Word, (void * &) p))
	{
		pdc = GetDC();
		nname = EDIA_cl_Completion_MakeFctName(pdc, (int) p, FALSE);
		EDI_Tooltip_DisplayMessage((char *) (LPCSTR) nname, 500);
		ReleaseDC(pdc);
		return;
	}

	/* Constant ? */
	pos = go_PPConstants.GetStartPosition();
	while(pos)
	{
		go_PPConstants.GetNextAssoc(pos, o_Name, o_Val);
		if(o_Word == o_Name)
		{
			o_Name += " = ";
			psz_Tmp = (char *) (LPCSTR) o_Val;
			while(*psz_Tmp && L_isspace(*psz_Tmp)) psz_Tmp++;
			psz_Tmp1 = psz_Tmp;
			while(*psz_Tmp1)
			{
				if(L_isspace(*psz_Tmp1) && *psz_Tmp1 != '\n' && *psz_Tmp1 != '\r') *psz_Tmp1 = ' ';
				psz_Tmp1++;
			}

			o_Name += psz_Tmp;

			if(!go_DefineCstFiles.Lookup(o_Word, o_Val)) go_DefineCstFilesGlob.Lookup(o_Word, o_Val);

			switch(what)
			{
			case 0:
				EDI_Tooltip_DisplayMessage((char *) (LPCSTR) o_Name, 500);
				break;
			case 1:
				go_PPConstantsFiles.Lookup(o_Word, (void * &) ul_Index);
				mpo_Frame->i_OnMessage(EDI_MESSAGE_SELFILE, 0, ul_Index);
				go_PPConstantsLines.Lookup(o_Word, (void * &) line);
				mpo_Frame->ConnectToLine(line);
				break;
			}

			return;
		}
	}

	/* Macro ? */
	pos = go_PPMacros.GetStartPosition();
	while(pos)
	{
		go_PPMacros.GetNextAssoc(pos, o_Name, (void * &) ul_Index);
		if(o_Word == o_Name)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			o_Name = CString(asz_Path) + "\n" + BIG_NameFile(ul_Index);
			switch(what)
			{
			case 0:
				EDI_Tooltip_DisplayMessage((char *) (LPCSTR) o_Name, 500);
				break;
			case 1:
				mpo_Frame->i_OnMessage(EDI_MESSAGE_SELFILE, 0, ul_Index);
				go_PPMacrosLines.Lookup(o_Word, (void * &) line);
				mpo_Frame->ConnectToLine(line);
				break;
			}

			return;
		}
	}

	if(mpo_Frame->mpst_Model && mpo_Frame->mpst_Model->pst_VarDes)
	{
		/* Is it a global var ? */
		pst_VarDes = mpo_Frame->mpst_Model->pst_VarDes;
		for(i = 0; i < (int) pst_VarDes->ul_NbVars; i++)
		{
			if(!_strnicmp((char *) (LPCSTR) o_Word, pst_VarDes->pst_EditorVarsInfos[i].asz_Name, AI_C_MaxLenVar))
			{
				if(mpo_Frame->mpst_Instance)
				{
					pi_Val = (int *) (mpo_Frame->mpst_Instance->pc_VarsBuffer + pst_VarDes->pst_VarsInfos[i].i_Offset);
				}
				else
				{
					pi_Val = (int *) (mpo_Frame->mpst_Model->pst_VarDes->pc_BufferInit + pst_VarDes->pst_VarsInfos[i].i_Offset);
				}

				/* Not an array */
                int iElementNb = (pst_VarDes->pst_VarsInfos[i].i_NumElem & 0x3FFFFFFF);
				if(iElementNb == 1)
				{
					switch(what)
					{
					case 0:
						DisplayVar
						(
							(char *) (LPCSTR) o_Word,
							pi_Val,
							AI_gast_Types[pst_VarDes->pst_VarsInfos[i].w_Type].w_ID,
							_b_Now
						);
						break;
					case 2:
						AI_AddWatch(pi_Val, AI_gast_Types[pst_VarDes->pst_VarsInfos[i].w_Type].w_Size);
						break;
					case 3:
						AI_RemoveWatch(pi_Val);
						break;
					case 10:
					case 11:
					case 12:
					case 13:
						if ( pst_VarDes->pst_VarsInfos[i].w_Type == ENUTYPE_VECTOR )
						{
							EOUT_cl_Frame		*po_Out;
							WOR_tdst_World		*pst_World;
							po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
							pst_World = po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World;
							pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].c_Local = (what >= 12) ? 1 : 0;
							pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].c_Vector = what & 1;
							pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].pst_Gao = (what == 10) ? NULL : (OBJ_tdst_GameObject *) mpo_Frame->mpst_Instance->pst_GameObject;
							pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].pst_Vector = (MATH_tdst_Vector *) pi_Val;
							pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].ul_Color = 0xFFFFFFFF;
							pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].c_Remember = 0;
							pst_World->uc_ShowVector++;
							break;
						}
					}
					return;
				}

				/* Array */
				else
				{
					switch(what)
					{
					case 0:
						w_Type = AI_gast_Types[pst_VarDes->pst_VarsInfos[i].w_Type].w_ID;
						w_Size = AI_gast_Types[pst_VarDes->pst_VarsInfos[i].w_Type].w_Size;
						NNN_Local = 0;
arrayvars:
						GetCursorPos(&pt1);
						o_Str.Empty();
						if(NNN_Local == 1)
						{
							DisplayVar
							(
								(char *) (LPCSTR) o_Word,
								pi_Val,
								w_Type,
								_b_Now,
								az
							);
							o_Str += az;
						}
						else
						{
							for(j = NNN_Deb; j < NNN_Deb + 10; j++)
							{
								o_Name1.Format("[%d]", j);
								o_Name1 = o_Word + o_Name1;
								DisplayVar
								(
									(char *) (LPCSTR) o_Name1,
									pi_Val + 1 + (j * (w_Size >> 2)),
									w_Type,
									_b_Now,
									az
								);
								o_Str += az;
								o_Str += "\n";
							}
						}
						EDI_Tooltip_DisplayMessage((char *) (LPCSTR) o_Str, NNN_ForceNow == FALSE ? 300 : 0);
						break;
					case 2:
                    {
                        // What element of array should be watched ?
                        ULONG ulIndex;
                        int iSize = AI_gast_Types[pst_VarDes->pst_VarsInfos[i].w_Type].w_Size;
                        char sMsg[256];
                        sprintf(sMsg,"Enter index in array (0 to %d)",iElementNb-1);
                        EDIA_cl_NameDialog oIndexDialog(sMsg);
                        while (oIndexDialog.DoModal() == IDOK)
                        {
                            if ((1 != sscanf((char *) (LPCSTR) oIndexDialog.mo_Name, "%u", &ulIndex)) ||
                                (ulIndex >= iElementNb))
                                continue;

    						AI_AddWatch(
                                pi_Val + 1 + ulIndex * (iSize >> 2),
                                iSize);
                            break;
                        }
						break;
                    }
					case 3:
                    {
                        int iSize = AI_gast_Types[pst_VarDes->pst_VarsInfos[i].w_Type].w_Size;
						AI_RemoveWatchOnAddressRange(pi_Val + 1,pi_Val + 1 + iElementNb * (iSize >> 2) );
						break;
                    }
                    }
				}

				return;
			}
		}

		/* Is it a local var ? */
		if(mpo_Frame->mul_CurrentEditFile != BIG_C_InvalidIndex)
		{
			/* Fonction */
			if(BIG_b_IsFileExtension(mpo_Frame->mul_CurrentEditFile, EDI_Csz_ExtAIEditorFct))
			{
				BIG_ComputeFullName(BIG_ParentFile(mpo_Frame->mul_CurrentEditFile), asz_Path);
				L_strcpy(asz_Name, BIG_NameFile(mpo_Frame->mul_CurrentEditFile));
				*L_strrchr(asz_Name, '.') = 0;
				L_strcat(asz_Name, EDI_Csz_ExtAIEngineFct);
				ul_Index = BIG_ul_SearchFileExt(asz_Path, asz_Name);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					pst_Func = (AI_tdst_Function *) LOA_ul_SearchAddress(BIG_PosFile(ul_Index));
					if(pst_Func && (int) pst_Func != -1)
					{
						for(i = 0; i < (int) pst_Func->ul_NbLocals; i++)
						{
							if(!_strnicmp((char *) (LPCSTR) o_Word, pst_Func->pst_LocalVars[i].asz_Name, AI_C_MaxLenVar))
							{
								o_Word = "(Local) " + o_Word;
								pi_Val = (int *) (AI_gac_LocalStack + AI_gul_SizeLocalStack + pst_Func->pst_LocalVars[i].i_Offset);
								w_Type = pst_Func->pst_LocalVars[i].i_Type;
								if(NNN_Local != 2) NNN_Local = 1;
								goto arrayvars;
								return;
							}
						}
					}
				}
			}
		}
	}

	/* Proc goto def ? */
	if(what == 1 && mpo_Frame->mo_Compiler.mo_ProcList.Lookup((char *) (LPCSTR) o_Word, (void * &) pt_ProcEd))
	{
		mpo_Frame->i_OnMessage(EDI_MESSAGE_SELFILE, 0, pt_ProcEd->h_File);
		mpo_Frame->ConnectToLine(pt_ProcEd->i_Line);
		return;
	}

	/* Procedure */
	if(BIG_b_IsFileExtension(mpo_Frame->mul_CurrentEditFile, EDI_Csz_ExtAIEditorFctLib))
	{
		BIG_ComputeFullName(BIG_ParentFile(mpo_Frame->mul_CurrentEditFile), asz_Path);
		L_strcpy(asz_Name, BIG_NameFile(mpo_Frame->mul_CurrentEditFile));
		*L_strrchr(asz_Name, '.') = 0;
		L_strcat(asz_Name, EDI_Csz_ExtAIEngineFctLib);
		ul_Index = BIG_ul_SearchFileExt(asz_Path, asz_Name);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			pst_Proc = (SCR_tt_ProcedureList *) LOA_ul_SearchAddress(BIG_PosFile(ul_Index));
			if(pst_Proc && (int) pst_Proc != -1)
			{
				for(i = 0; i < pst_Proc->u16_Num; i++)
				{
					if(AI_gst_BreakInfo.pt_Node != pst_Proc->pt_All[i].pu32_Nodes) continue;
					for(j = 0; j < (int) pst_Proc->pt_All[i].ul_NbLocals; j++)
					{
						nname = pst_Proc->pt_All[i].pst_LocalVars[j].asz_Name;
						pz = L_strchr((char *) (LPCSTR) nname, '@');
						if(pz) 
							pz++;
						else
							pz = (char *) (LPCSTR) nname;
						if(!_strnicmp((char *) (LPCSTR) o_Word, pz, AI_C_MaxLenVar))
						{
							o_Word = "(Local) " + o_Word;
							pi_Val = (int *) (AI_gac_LocalStack + AI_gul_SizeLocalStack + pst_Proc->pt_All[i].pst_LocalVars[j].i_Offset);
							if(L_strchr(pst_Proc->pt_All[i].pst_LocalVars[j].asz_Name, '@')) pi_Val = *(int **) pi_Val; /* Ref */
							w_Type = pst_Proc->pt_All[i].pst_LocalVars[j].i_Type;
							if(NNN_Local != 2) NNN_Local = 1;
							if ( pi_Val ) goto arrayvars;
							return;
						}
					}

					return;
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::SetDisplayVar(CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHARRANGE	cr;
	CString		o_Left, o_Right, o_Word;
	int			i;
	CPoint		o_Pos;
	CRect		o_Rect;
	CString		o_Txt, o_Str;
	CDC			*pdc;
	CSize		size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_DisplayLine = 0;
	mo_DisplayVar.Empty();
	GetSel(cr);
	if(cr.cpMin == cr.cpMax)
	{
		GetWindowText(o_Txt);

		/* Retreive character under mouse */
		i = LineIndex(GetFirstVisibleLine());
		if(i == 1) return;
		while(i < GetTextLength())
		{
			o_Pos = GetCharPos(i);
			o_Rect.left = o_Pos.x;
			o_Rect.top = o_Pos.y;

			pdc = GetDC();
			o_Str = CString(((char *) (LPCSTR) o_Txt)[i]);
			size = pdc->GetTextExtent(o_Str);
			ReleaseDC(pdc);

			o_Rect.right = o_Rect.left + size.cx;
			o_Rect.bottom = o_Rect.top + size.cy;
			if(o_Rect.PtInRect(pt)) break;
			if(o_Rect.top > pt.y) return;
			if(o_Rect.bottom < pt.y)
			{
				i = LineIndex(LineFromChar(i) + 1);
				if(i == -1) return;
			}
			else
				i++;
		}

		if(i == GetTextLength()) return;

		/* Get word */
		GetSel(cr);
		o_Left = o_GetWordLeft(i);
		o_Right = o_GetWordRight(i);
		o_Word = o_Left + o_Right;
		if(o_Word.IsEmpty()) return;
		if(!L_isalpha(((char *) (LPCSTR) o_Word)[0])) return;

		mo_DisplayVar = o_Word;
		mi_DisplayLine = LineFromChar(i);
	}
	else
	{
		GetSel(cr);
		mi_DisplayLine = LineFromChar(cr.cpMin);
		mo_DisplayVar = GetSelText();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::OnRButtonDown(UINT nId, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	CMenu	menu;
	int		res;
	CString o;
	char	*psz_Beg;
	char	*psz_Last;
	char	asz_Path1[512];
	char	asz_File1[512];
	BIG_INDEX file;
	EOUT_cl_Frame		*po_Out;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~*/

	SetDisplayVar(pt);
	menu.CreatePopupMenu();
	ClientToScreen(&pt);
	menu.AppendMenu(MF_BYCOMMAND, 1, "Go To Definition");
	menu.AppendMenu(MF_BYCOMMAND, 2, "Add AI Watch");
	menu.AppendMenu(MF_BYCOMMAND, 3, "Del AI Watch");
	menu.AppendMenu(MF_BYCOMMAND, 4, "Del All AI Watch");
	menu.AppendMenu(MF_BYCOMMAND, 5, "Open File");
	
	po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	if(po_Out && po_Out->mpo_EngineFrame && po_Out->mpo_EngineFrame->mpo_DisplayView && po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World )
	{
		pst_World = po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World;
		menu.AppendMenu(MF_BYCOMMAND, 10, "Show position (global)" );
		if ( mpo_Frame->mpst_Instance && mpo_Frame->mpst_Instance->pst_GameObject )
		{
			menu.AppendMenu(MF_BYCOMMAND, 11, "Show vector (global)" );
			menu.AppendMenu(MF_BYCOMMAND, 12, "Show position (local)" );
			menu.AppendMenu(MF_BYCOMMAND, 13, "Show vector (local)" );
		}
		if (pst_World->uc_ShowVector)
			menu.AppendMenu(MF_BYCOMMAND, 14, "Del all Show vector/position" );
	}
	
	res = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this);
	if(res <= 0) return;

	switch(res)
	{
	case 1:
		DisplayVar(mo_DisplayVar, FALSE, 1);
		break;
	case 2:
		DisplayVar(mo_DisplayVar, FALSE, 2);
		break;
	case 3:
		DisplayVar(mo_DisplayVar, FALSE, 3);
		break;
	case 4: // Del all watch
        AI_DelAllWatch();
		break;
	case 5:

		o = mo_DisplayVar;
		if(!L_strrchr((char *) (LPCSTR) o, '.')) o += ".fct";

		psz_Beg = (char *) (LPCSTR) o;
		psz_Last = NULL;
		while(*psz_Beg)
		{
			if((*psz_Beg == '\\') || (*psz_Beg == '/'))
			{
				*psz_Beg = '/';
				psz_Last = psz_Beg;
			}

			psz_Beg++;
		}

		/* If no path, take path of current model */
		if(!psz_Last)
		{
			BIG_ComputeFullName(BIG_ParentFile(mpo_Frame->mul_CurrentEditModel), asz_Path1);
			L_strcpy(asz_File1, BIG_NameFile(mpo_Frame->mul_CurrentEditModel));
			*L_strrchr(asz_File1, '.') = 0;
			L_strcat(asz_Path1, "/");
			L_strcat(asz_Path1, asz_File1);
			psz_Beg = asz_Path1;
			psz_Last = (char *) (LPCSTR) o;
		}
		else
		{
			psz_Beg = (char *) (LPCSTR) o;
			*psz_Last = 0;
			psz_Last++;
		}

		file = BIG_ul_SearchFileExt(psz_Beg, psz_Last);
		if(file != BIG_C_InvalidIndex) mpo_Frame->i_OnMessage(EDI_MESSAGE_SELFILE, 0, file);
		break;
	case 10:
	case 11:
	case 12:
	case 13:
		DisplayVar( mo_DisplayVar, FALSE, res );
		break;
	case 14:
		pst_World->uc_ShowVector = 0;
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::OnMouseMove(UINT nId, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	static CPoint	o_LastPt;
	static int rec = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(rec) return;
	CRichEditCtrl::OnMouseMove(nId, pt);
	if(pt == o_LastPt) return;
	o_LastPt = pt;

	rec = TRUE;
	SetDisplayVar(pt);
	DisplayVar(mo_DisplayVar);
	rec = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::OnLButtonDblClk(UINT nId, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString		o_Left, o_Right;
	CHARRANGE	cr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	CRichEditCtrl::OnLButtonDblClk(nId, pt);
	GetSel(cr);
	o_Left = o_GetWordLeft(cr.cpMin);
	o_Right = o_GetWordRight(cr.cpMax);
	cr.cpMin -= o_Left.GetLength();
	cr.cpMax += o_Right.GetLength();
	SetSel(cr);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::LockWindowUpdate(void)
{
	mi_LockCount++;
	if(mi_LockCount == 1) M_MF()->LockWindowUpdate(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::UnlockWindowUpdate(void)
{
	mi_LockCount--;
	if(mi_LockCount == 0) CRichEditCtrl::UnlockWindowUpdate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int dDeb;
	
	if(nFlags & MK_CONTROL)
	{
		dDeb = (nFlags & MK_SHIFT) ? 1 : 10;
		NNN_ForceNow = TRUE;
		if(zDelta < 0)
		{
			if(NNN_Local == 1)
				NNN_Local = 2;
			else
				NNN_Deb += dDeb;
		}
		else 
		{
			if(NNN_Deb)
			{
				NNN_Deb -= dDeb;
				if ( NNN_Deb < 0 ) NNN_Deb = 0;
			}
			else if(NNN_Local == 2)
				NNN_Local = 1;
		}

		DisplayVar(mo_DisplayVar);
		return TRUE;
	}

	if(nFlags & MK_SHIFT)
	{
		M_MF()->LockDisplay(this);
		ColorRange(0, 40);
		mpo_Frame->mst_Ini.i_FontSize += zDelta / 60;
		if(mpo_Frame->mst_Ini.i_FontSize < 4) mpo_Frame->mst_Ini.i_FontSize = 4;
		ResetDefaultFormat(FALSE);
		mpo_Frame->ForceRefresh();
		M_MF()->UnlockDisplay(this);
		return TRUE;
	}

	if(nFlags & MK_CONTROL) return TRUE;
	return CRichEditCtrl::OnMouseWheel(0, zDelta, pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_View::OnMButtonDown(UINT, CPoint)
{
}

int EAI_cl_View::FindText(DWORD uw, FINDTEXTEX *txt)
{
	CString	o_Txt;
	char	*pz, *ppz, *pz1;

	txt->chrgText.cpMin = txt->chrg.cpMin;
	txt->chrgText.cpMax = 0;
	GetWindowText(o_Txt);
	pz = (char *) (LPCSTR) o_Txt;
	pz += txt->chrg.cpMin;

	while(*pz)
	{
		ppz = pz;
		pz1 = (char *) txt->lpstrText;
		if(uw & FR_MATCHCASE)
			while(*ppz == *pz1) ppz++, pz1++;
		else
			while(L_toupper(*ppz) == L_toupper(*pz1)) ppz++, pz1++;
		if(!*pz1)
		{
			if
			(
				(!(uw & FR_WHOLEWORD))
                ||	(!L_isalpha(*ppz) && (*ppz!='_'))
			||	(!(*ppz))
			)
			{
//				txt->chrgText.cpMin -= LineFromChar(txt->chrgText.cpMin);
				txt->chrgText.cpMax = txt->chrgText.cpMin + strlen(txt->lpstrText);
				return 1;
			}
		}

		pz++;
		txt->chrgText.cpMin++;
	}

	return -1;
}

void EAI_cl_View::GetWindowText(CString &res)
{
	CString o;
	char *p;

	res.Empty();
	CRichEditCtrl::GetWindowText(o);
	p = (char *) (LPCSTR) o;
	while(*p)
	{
		if(*p != '\r') res += *p;
		p++;
	}
}

#endif /* ACTIVE_EDITORS */
