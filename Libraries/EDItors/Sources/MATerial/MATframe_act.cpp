/*$T MATframe_act.cpp GC! 1.100 08/24/01 15:09:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EDIbaseframe.h"
#include "MATframe.h"
#include "MATframe_act.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "Res/Res.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/ENGvars.h"
#include "BIGfiles/BIGkey.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "EDIapp.h"
#include "GraphicDK/Sources/TEXture/TEXeditorfct.h"
#include "GraphicDK/Sources/MATerial/MATmulti.h"
#include "GraphicDK/Sources/MATerial/MATsingle.h"
#include "Dialogs/DIAfile_dlg.h"
#include "EDImsg.h"

#include "GEOmetric/GEOload.h"

#ifndef PSX2_TARGET
#include "TIMer/TIMdefs.h"
#endif
#define MAT_ACTION_BASE 256

extern BOOL b_ShowCLPBRDMT;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EMAT_cl_Frame::ui_ActionFillDynamic(EDI_cl_ConfigList *_po_List, POSITION _pos)
{
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EMAT_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	char	c_Key;
	BOOL	b_Shift, b_Ctrl;
	/*~~~~~~~~~~~~~~~~~~~~*/

	b_Shift = _uw_Key & SHIFT;
	b_Ctrl = _uw_Key & CONTROL;
	c_Key = (char) _uw_Key;

	switch(c_Key)
	{
	case VK_NUMPAD0:
	case VK_NUMPAD1:
	case VK_NUMPAD2:
	case VK_NUMPAD3:
	case VK_NUMPAD4:
	case VK_NUMPAD5:
	case VK_NUMPAD6:
	case VK_NUMPAD7:
	case VK_NUMPAD8:
	case VK_NUMPAD9:
	case VK_DECIMAL:
#ifdef JADEFUSION
    case VK_BACK:
#endif
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':
		if(!b_Ctrl && !b_Shift) return TRUE;
		break;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnAction(ULONG _ul_Action)
{
	MAT_tdst_Material	*pst_M;

	if(_ul_Action == EMAT_ACTION_CLOSE)
	{
		st_CDO.pst_GRO = NULL;
		st_CDO.pst_ACTIVE_GRO = NULL;
		::SendDlgItemMessage(stw_MultiSM, IDC_LIST_MSM, LB_RESETCONTENT, 0, 0);
		::SetDlgItemText(stw_MultiSM, IDC_MATNAME, "");
		MUTEX_SetMat(m_hWnd, NULL);
		OnSize(0, CurrentCX, CurrentCY);
		RefreshMenu();
		return;
	}

	if(_ul_Action == EMAT_ACTION_NEWMULTI)
	{
		OnNewMulti();
		return;
	}

	if(_ul_Action == EMAT_ACTION_NEWSINGLE)
	{
		OnNewSingle();
		return;
	}

	if(_ul_Action == EMAT_ACTION_UNDO)
	{
		OnUndo();
		return;
	}

	if(_ul_Action == EMAT_ACTION_REDO)
	{
		OnRedo();
		return;
	}

	if(_ul_Action == EMAT_ACTION_SAVEMATERIAL)
	{
		ul_IsSaved = 0;
		OnSave();

		if(st_CDO.pst_GRO->st_Id.i->ul_Type == GRO_MaterialMulti)
		{
			ul_IsSaved = 0;
			pst_M = st_CDO.pst_ACTIVE_GRO;
			st_CDO.pst_ACTIVE_GRO = st_CDO.pst_GRO;
			OnSave();
			st_CDO.pst_ACTIVE_GRO = pst_M;
		}
		return;
	}

	if(_ul_Action == EMAT_ACTION_COPY_ALL)
	{
		OnCopyAll();
		return;
	}

	if(_ul_Action == EMAT_ACTION_PASTE_ALL)
	{
		OnPasteAll();
		return;
	}

	if(_ul_Action == EMAT_ACTION_SHMLTSUBMAT)
	{
		ShowMLTSBMT ^= 1;
		M_MF()->LockDisplay(this);
		OnSize(0, CurrentCX, CurrentCY);
		M_MF()->UnlockDisplay(this);
		return;
	}
	if(_ul_Action == EMAT_ACTION_SHCLPBRDMAT)
	{
		b_ShowCLPBRDMT = !b_ShowCLPBRDMT;
		M_MF()->LockDisplay(this);
		OnSize(0, CurrentCX, CurrentCY);
		M_MF()->UnlockDisplay(this);
		return;
	}
	if(_ul_Action == EMAT_ACTION_CHECKMULTIMAT)
	{
		TEX_4Edit_CheckMultiMat();
		return;
	}

	if(_ul_Action == EMAT_ACTION_CHECKTEXTURE)
	{
		TEX_4Edit_CheckMatTexture();
		return;
	}

	if(_ul_Action == EMAT_ACTION_CHECKMATOWNER)
	{
		/*~~~~~~~~~~~~~*/
		ULONG	ul_Index;
		/*~~~~~~~~~~~~~*/

		if(st_CDO.pst_ACTIVE_GRO)
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) st_CDO.pst_ACTIVE_GRO);
			TEX_4Edit_WhoUseMaterial(ul_Index);
		}

		return;
	}

	if(_ul_Action == EMAT_ACTION_CHECKMULTIMATOWNER)
	{
		/*~~~~~~~~~~~~~*/
		ULONG	ul_Index;
		/*~~~~~~~~~~~~~*/

		if(st_CDO.pst_GRO)
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) st_CDO.pst_GRO);
			TEX_4Edit_WhoUseMaterial(ul_Index);
		}

		return;
	}
	
	if ( _ul_Action == EMAT_ACTION_CHECKSOUNDID )
	{
		TEX_4Edit_CheckSoundID();
		return;
	}
	
	if ( _ul_Action == EMAT_ACTION_GENERATEHTML )
	{
		GenerateHtml();
		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EMAT_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL _b_Disp)
{
	if(_ul_Action == EMAT_ACTION_CHECKMULTIMATOWNER)
	{
		if(!st_CDO.pst_GRO) return FALSE;
		if(!st_CDO.pst_ACTIVE_GRO) return FALSE;
		if(st_CDO.pst_ACTIVE_GRO == st_CDO.pst_GRO) return FALSE;
	}

	switch(_ul_Action)
	{
	case EMAT_ACTION_CHECKMATOWNER:
	case EMAT_ACTION_CLOSE:
	case EMAT_ACTION_SAVEMATERIAL:
	case EMAT_ACTION_GENERATEHTML:
		if(!st_CDO.pst_GRO) return FALSE;
		if(!st_CDO.pst_ACTIVE_GRO) return FALSE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EMAT_cl_Frame::i_IsItVarAction(ULONG _ul_Action, EVAV_cl_View *_po_List)
{
	switch(_ul_Action)
	{
	case EMAT_ACTION_OPTIONS:
		/* To avoid reset and refresh the list if not necessary */
		if(!_po_List) return 2;
		if(_po_List->mul_ID == EMAT_ACTION_OPTIONS) return 2;
		_po_List->mul_ID = EMAT_ACTION_OPTIONS;

		_po_List->ResetList();

		/* General */
		_po_List->AddItem("Auto save", EVAV_EVVIT_Bool, &mst_Init.bAutoSave, 0, 0, 0, 0);
		return 1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EMAT_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~*/
	UINT	ui_State;
	/*~~~~~~~~~~~~~*/

	ui_State = (UINT) - 1;
	if((_ul_Action == EMAT_ACTION_SAVEMATERIAL) || (_ul_Action == EMAT_ACTION_UNDO) || (_ul_Action == EMAT_ACTION_REDO))
	{
		return ui_State;
	}

	if(_ul_Action == -1)
	{
		return ui_State;
	}

	if(_ul_Action == EMAT_ACTION_SHMLTSUBMAT)
	{
		ui_State = DFCS_BUTTONCHECK;
		if(ShowMLTSBMT) ui_State |= DFCS_CHECKED;
	}

	if(_ul_Action == EMAT_ACTION_SHCLPBRDMAT)
	{
		ui_State = DFCS_BUTTONCHECK;
		if(b_ShowCLPBRDMT) ui_State |= DFCS_CHECKED;
	}

	return ui_State;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnNewMulti(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Material	*pst_Mat;
	CString				o_Temp;
	EDIA_cl_FileDialog	o_File("Choose file", 0, 0, 1);
	BIG_INDEX			ul_Fat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() != IDOK) return;

	o_File.GetItem(o_File.mo_File, 0, o_Temp);
	o_Temp += ".grm";
	if(BIG_b_CheckName((char *) (LPCSTR) o_Temp) == FALSE) return;

	ul_Fat = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
	if(ul_Fat != BIG_C_InvalidIndex)
	{
		if
		(
			M_MF()->MessageBox
				(
					"This material already exists. Do you want to overwrite it ?",
					"Please Confirm",
					MB_ICONQUESTION | MB_YESNO
				) == IDNO
		) return;
	}

	SAV_Begin(o_File.masz_FullPath, (char *) (LPCSTR) (char *) (LPCSTR) o_Temp);
	pst_Mat = (MAT_tdst_Material *) MAT_pst_CreateMulti((char *) (LPCSTR) o_Temp);
	pst_Mat->st_Id.i->pfnl_SaveInBuffer(pst_Mat, &TEX_gst_GlobalList);
	ul_Fat = SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GRO_tdst_Struct *EMAT_cl_Frame::CreateSingleMaterial(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Single		*pst_Mat;
	CString				o_Temp;
	EDIA_cl_FileDialog	o_File("Choose file", 0, 0, 1);
	BIG_INDEX			ul_Fat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() != IDOK) return NULL;

	o_File.GetItem(o_File.mo_File, 0, o_Temp);
	o_Temp += ".grm";
	if(BIG_b_CheckName((char *) (LPCSTR) o_Temp) == FALSE) return NULL;

	ul_Fat = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
	if(ul_Fat != BIG_C_InvalidIndex)
	{
		if
		(
			M_MF()->MessageBox
				(
					"This material already exists. Do you want to overwrite it ?",
					"Please Confirm",
					MB_ICONQUESTION | MB_YESNO
				) == IDNO
		) return NULL;
	}

	SAV_Begin(o_File.masz_FullPath, (char *) (LPCSTR) (char *) (LPCSTR) o_Temp);
	pst_Mat = (MAT_tdst_Single *) MAT_pst_CreateSingle((char *) (LPCSTR) o_Temp);
	pst_Mat->st_Id.i->pfnl_SaveInBuffer(pst_Mat, &TEX_gst_GlobalList);
	ul_Fat = SAV_ul_End();

	pst_Mat->st_Id.i->pfn_Destroy(pst_Mat);

	return GEO_pst_Object_Load(ul_Fat, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnNewSingle(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct	*pst_Mat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mat = CreateSingleMaterial();
	if(!pst_Mat) return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::GenerateHtml( void )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog		o_File( "Choose folder where desc will be created", 2, FALSE);
	char					sz_Path[ 260 ], sz_Name[ 260 ], sz_ExternPath[ 260 ];
	FILE					*hp_File;
	ULONG					ul_Index;
	MAT_tdst_Material		*pst_GRM, *pst_GRMSub;
	MAT_tdst_Multi			*Mtt;
	MAT_tdst_MultiTexture	*Stt;
	MAT_tdst_Single			*SingleMat;
	TEX_tdst_Data	        *pst_TexData;
	MAT_tdst_MTLevel        *MttL;
	int						i_Sub, i_Count, i_Single;
	TEX_tdst_4Edit_CreateBitmapResult	st_Res;
	HBITMAP								h_RGB, h_A;
	int									W, H;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	if (!st_CDO.pst_GRO) return;
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) st_CDO.pst_GRO);
		
	
	if(o_File.DoModal() != IDOK) 
		return;
		
	L_strcpy( sz_Path, o_File.masz_FullPath );
	L_strcat( sz_Path, "/" );
	L_strcat( sz_Path, BIG_NameFile( ul_Index ) );
	if (strrchr( sz_Path, '.' ) )
		*strrchr( sz_Path, '.' ) = 0;


	if ( !SetCurrentDirectory( sz_Path ) )
	{
		if ( !CreateDirectory( sz_Path, NULL ) )
		{
			ERR_X_Warning( 0, "can't create export folder", NULL );
			return;
		}
	}
	L_strcpy( sz_ExternPath, sz_Path );
	pst_GRM = st_CDO.pst_GRO;
	
	st_Res.ppc_Raw = NULL;
	st_Res.ph_Alpha = &h_A;
	st_Res.ph_Colors = &h_RGB;
	st_Res.pi_Height = &H;
	st_Res.pi_Width = &W;
	st_Res.pst_Pal = NULL;
	st_Res.ph_PaletteAlpha = NULL;
	st_Res.ph_PaletteColors = NULL;
	st_Res.b_RawPalPrio = 0;
	
	/* generate html file */
	sprintf( sz_Name, "%s/%s", sz_Path, BIG_NameFile( ul_Index ) );
	if (strrchr( sz_Name, '.' ) )
		*strrchr( sz_Name, '.' ) = 0;
	L_strcat( sz_Name, ".htm" );
	hp_File = L_fopen( sz_Name, "wt" );
	if ( hp_File )
	{
		fprintf( hp_File, "<html><body lang=FR style='tab-interval:35.4pt'>\n" );
		BIG_ComputeFullName( BIG_ParentFile( ul_Index ), sz_Path );
		fprintf( hp_File, "<p>MATERIAL [%08X] %s/%s</p>\n", BIG_FileKey( ul_Index ), sz_Path, BIG_NameFile( ul_Index ) );
		
		i_Single = 0;
		if ( pst_GRM->st_Id.i->ul_Type == GRO_MaterialMultiTexture )
		{
			i_Single = 1;
			Stt = (MAT_tdst_MultiTexture *) pst_GRM;
			goto ONE_MULTITEXTURE;
		}
		
		if ( pst_GRM->st_Id.i->ul_Type == GRO_MaterialSingle)
		{
			i_Single = 1;
			SingleMat = (MAT_tdst_Single *) pst_GRM;
			goto ONE_SINGLE;
		}
		
		if( pst_GRM->st_Id.i->ul_Type == GRO_MaterialMulti )
		{
			Mtt = (MAT_tdst_Multi *) pst_GRM;
			fprintf( hp_File, "<p>.... Multi Material with %d sub material(s)</p>\n", Mtt->l_NumberOfSubMaterials);
			
			for (i_Sub = 0; i_Sub < Mtt->l_NumberOfSubMaterials; i_Sub++)
			{
				pst_GRMSub = Mtt->dpst_SubMaterial[ i_Sub ];
				if ( !pst_GRMSub )
				{
					fprintf( hp_File, "<p>.... Sub Mat %d [ERROR : NO SUB MAT]</p>\n", i_Sub );
				}
				else if ( (ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GRMSub)) == BIG_C_InvalidIndex )
				{
					fprintf( hp_File, "<p>.... Sub Mat %d [ERROR : bad adress]</p>\n", i_Sub );
				}
				else if (pst_GRMSub->st_Id.i->ul_Type == GRO_MaterialSingle )
				{
					SingleMat = (MAT_tdst_Single *) pst_GRMSub;
					
					ONE_SINGLE:
					
					BIG_ComputeFullName( BIG_ParentFile( ul_Index ), sz_Path );
					if (i_Single)
						fprintf( hp_File, "<p>.... (Single) [%08X] %s/%s</p>\n", BIG_FileKey( ul_Index ), sz_Path, BIG_NameFile( ul_Index) );
					else
						fprintf( hp_File, "<p>.... Sub Mat %d : (Single) [%08X] %s/%s</p>\n", i_Sub, BIG_FileKey( ul_Index ), sz_Path, BIG_NameFile( ul_Index) );
					if ( SingleMat->l_TextureId == -1 )
					{
						fprintf( hp_File, "<p>........ Texture [ERROR : No Texture]</p>\n"  );
					}
					else
					{
						pst_TexData = &TEX_gst_GlobalList.dst_Texture[SingleMat->l_TextureId];
						ul_Index = BIG_ul_SearchKeyToFat( pst_TexData->ul_Key );
						if (ul_Index == BIG_C_InvalidIndex )
						{
							fprintf( hp_File, "<p>........ Texture [ERROR : Bad Key] %08X</p>\n", pst_TexData->ul_Key );
						}
						else
						{
							BIG_ComputeFullName( BIG_ParentFile( ul_Index ), sz_Path );
							fprintf( hp_File, "<p>........ Texture [%08X] %s/%s</p>\n", pst_TexData->ul_Key, sz_Path, BIG_NameFile( ul_Index ) );
							if ( TEX_i_4Edit_CreateBitmap( ul_Index, 0, this->GetDC()->GetSafeHdc() , &st_Res) )
							{
								fprintf( hp_File, "<p align=center >", sz_Name );
								if( h_RGB )
								{
									sprintf( sz_Name, "%s/%s", sz_ExternPath, BIG_NameFile( ul_Index ) );
									if (strrchr( sz_Name, '.' ) )
										*strrchr( sz_Name, '.' ) = 0;
									L_strcat( sz_Name, ".bmp" );
									TEX_i_4Edit_SaveBitmap( h_RGB, W, H, sz_Name );
									DeleteObject( h_RGB );
									fprintf( hp_File, "<img src=\"%s\">", sz_Name );
								}
								if( h_A )
								{
									sprintf( sz_Name, "%s/%s", sz_ExternPath, BIG_NameFile( ul_Index ) );
									if (strrchr( sz_Name, '.' ) )
										*strrchr( sz_Name, '.' ) = 0;
									L_strcat( sz_Name, "_alpha.bmp" );
									TEX_i_4Edit_SaveBitmap( h_A, W, H, sz_Name );
									DeleteObject( h_A );
									fprintf( hp_File, "<img src=\"%s\">", sz_Name );
								}
								fprintf( hp_File, "</p>\n", sz_Name );
							}
						}
					}
					if (i_Single)
						goto ONE_END;
				}
				else if (pst_GRMSub->st_Id.i->ul_Type == GRO_MaterialMultiTexture )
				{
					Stt = (MAT_tdst_MultiTexture *) pst_GRMSub;
					
					ONE_MULTITEXTURE:
					
					i_Count = 0;
					MttL = Stt->pst_FirstLevel;
					while (MttL)
					{
						i_Count++;
						MttL = MttL->pst_NextLevel;
					}
					BIG_ComputeFullName( BIG_ParentFile( ul_Index ), sz_Path );
					if (i_Single)
						fprintf( hp_File, "<p>.... (MultiTexture %d) [%08X] %s/%s</p>\n", i_Count, BIG_FileKey( ul_Index ), sz_Path, BIG_NameFile( ul_Index) );
					else
						fprintf( hp_File, "<p>.... Sub Mat %d : (MultiTexture %d) [%08X] %s/%s</p>\n", i_Sub, i_Count, BIG_FileKey( ul_Index ), sz_Path, BIG_NameFile( ul_Index) );
					
					i_Count = 0;
					MttL = Stt->pst_FirstLevel;
					while (MttL)
					{
						if ( MttL->s_TextureId == -1 )
						{
							fprintf( hp_File, "<p>........ Level %d Texture [ERROR : No Texture]</p>\n", i_Count );
						}
						else
						{
							pst_TexData = &TEX_gst_GlobalList.dst_Texture[MttL->s_TextureId];
							ul_Index = BIG_ul_SearchKeyToFat( pst_TexData->ul_Key );
							if (ul_Index == BIG_C_InvalidIndex )
							{
								fprintf( hp_File, "<p>........ Level %d Texture [ERROR : Bad Key] %08X</p>\n", i_Count, pst_TexData->ul_Key );
							}
							else
							{	
								BIG_ComputeFullName( BIG_ParentFile( ul_Index ), sz_Path );
								fprintf( hp_File, "<p>........ Level %d Texture [%08X] %s/%s</p>\n", i_Count, pst_TexData->ul_Key, sz_Path, BIG_NameFile( ul_Index ) );
								
								if ( TEX_i_4Edit_CreateBitmap( ul_Index, 0, this->GetDC()->GetSafeHdc() , &st_Res) )
								{
									fprintf( hp_File, "<p align=center >", sz_Name );
									if( h_RGB )
									{
										sprintf( sz_Name, "%s/%s", sz_ExternPath, BIG_NameFile( ul_Index ) );
										if (strrchr( sz_Name, '.' ) )
											*strrchr( sz_Name, '.' ) = 0;
										L_strcat( sz_Name, ".bmp" );
										TEX_i_4Edit_SaveBitmap( h_RGB, W, H, sz_Name );
										DeleteObject( h_RGB );
										fprintf( hp_File, "<img src=\"%s\">", sz_Name );
									}
									if( h_A )
									{
										sprintf( sz_Name, "%s/%s", sz_ExternPath, BIG_NameFile( ul_Index ) );
										if (strrchr( sz_Name, '.' ) )
											*strrchr( sz_Name, '.' ) = 0;
										L_strcat( sz_Name, "_alpha.bmp" );
										TEX_i_4Edit_SaveBitmap( h_A, W, H, sz_Name );
										DeleteObject( h_A );
										fprintf( hp_File, "<img src=\"%s\">", sz_Name );
									}
									fprintf( hp_File, "</p>\n", sz_Name );
								}
							}
						}
						i_Count++;
						MttL = MttL->pst_NextLevel;
					}
					if (i_Single)
						goto ONE_END;
				}
			}
		}
		else
		{
			fprintf( hp_File, "<p>.... [ERROR : Not a material ]</p>\n" );
		}
		
		ONE_END:
		
		fprintf( hp_File, "<p class=MsoNormal><![if !supportEmptyParas]>&nbsp;<![endif]><o:p></o:p></p>\n" );
		fprintf( hp_File, "</body></html>\n" );
		fclose( hp_File );
	}
	
}

#endif
