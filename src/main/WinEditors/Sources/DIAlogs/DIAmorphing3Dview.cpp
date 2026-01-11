/*$T DIAmorphing_dlg.cpp GC! 1.081 06/14/00 10:03:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAmorphing_dlg.h"
#include "DIAlogs/DIAmorphing3Dview.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dframe.h"
#include "Editors/Sources/OUTput/OUTframe.h"
#include "EDImsg.h"

#include "EDIpaths.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKstruct_reg.h"

#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORaccess.h"



//BEGIN_MESSAGE_MAP(EDIA_cl_Morphing3DViewDialog, EDIA_cl_BaseDialog)
//	ON_WM_PAINT()
//	ON_WM_SIZE()
//END_MESSAGE_MAP()

///*
// =======================================================================================================================
// =======================================================================================================================
// */
//EDIA_cl_Morphing3DViewDialog::EDIA_cl_Morphing3DViewDialog( EDI_cl_BaseFrame *_po_Owner ) :
//	EDIA_cl_BaseDialog(DIALOGS_IDD_MORPHING3DVIEW)
//{	
//    mpo_Owner = _po_Owner;   
//    //mpst_Gao = mpo_Owner->mpst_Gao;
//	mpst_Gao = NULL;
//	mpo_EngineFrame = NULL;
//	mi_ViewIdx = 0;
//}
//
///*
// =======================================================================================================================
// =======================================================================================================================
// */
//EDIA_cl_Morphing3DViewDialog::~EDIA_cl_Morphing3DViewDialog(void)
//{
//
//	WOR_tdst_World		*pst_World;
//
//	pst_World = WOR_gpst_CurrentWorld; //WOR_World_GetWorldOfObject(mpst_Gao);
//	WOR_View_Close(&pst_World->pst_View[mi_ViewIdx]);
//	pst_World->pst_View[mi_ViewIdx].uc_Flags &= ~WOR_Cuc_View_Activ;
//
//
//// TODO : Truc plus propre avec messages (le owner pourrait ne pas être une EOUT_cl_Frame)
//	((EOUT_cl_Frame *)mpo_Owner)->mpo_CloseView = NULL;
//}
//
///*
// =======================================================================================================================
// =======================================================================================================================
// */
//BOOL EDIA_cl_Morphing3DViewDialog::OnInitDialog(void)
//{
//	WOR_tdst_World			*pst_World;
//	struct WOR_tdst_View_	*pst_View, *pst_LastView;
//	WOR_tdst_WorldDisplay	*pst_ViewDispInfo;
//
//	/* Create output frame. */
//	mpo_EngineFrame = new F3D_cl_Frame;
//	mpo_EngineFrame->MyCreate(mpo_Owner, this);
//	///* Register display view */
//	//LINK_RegisterPointer
//	//(
//	//	DP()->mst_WinHandles.pst_DisplayData,
//	//	LINK_C_StructDisplayData,
//	//	mpo_Owner->mst_Def.asz_Name,
//	//	EDI_Csz_Path_DisplayData
//	//);
//	//LINK_UpdatePointers();
//	
//	pst_World = WOR_gpst_CurrentWorld;
//		//WOR_World_GetWorldOfObject(mpst_Gao);
//	mi_ViewIdx = WOR_View_Create(pst_World);
//
//	WOR_View_Init (&pst_World->pst_View[mi_ViewIdx], &MATH_gst_IdentityMatrix, NULL);
//
//	pst_ViewDispInfo = &(pst_World->pst_View[mi_ViewIdx].st_DisplayInfo);
//	
//	//L_memcpy(
//	//	pst_ViewDispInfo->pst_DisplayDatas,
//	//	&mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData,
//	//	sizeof (WOR_tdst_WorldDisplay));
//
//	// pst_ViewDispInfo->pst_DisplayDatas = NULL;
//	pst_View = pst_World->pst_View;
//	pst_LastView = pst_View + pst_World->ul_NbViews;
//	for(; pst_View < pst_LastView; pst_View++) {
//		if ( (pst_View->uc_Flags & WOR_Cuc_View_Activ)
//			   && (pst_View->st_DisplayInfo.pst_DisplayDatas == GDI_gpst_CurDD)
//			   && (pst_View != &pst_World->pst_View[mi_ViewIdx])) {
//			L_memcpy( pst_ViewDispInfo, &pst_View->st_DisplayInfo, sizeof (WOR_tdst_WorldDisplay));
//			pst_World = ((GDI_tdst_DisplayData *)pst_View->st_DisplayInfo.pst_DisplayDatas)->pst_World;
//			L_memcpy(
//				&DDD()->st_Camera,
//				&((GDI_tdst_DisplayData *)pst_View->st_DisplayInfo.pst_DisplayDatas)->st_Camera,
//				sizeof(CAM_tdst_Camera));
//			//pst_ViewDispInfo->f_ViewportWidth =  pst_View->st_DisplayInfo.f_ViewportWidth / 2;
//			//pst_ViewDispInfo->f_ViewportHeight =  pst_View->st_DisplayInfo.f_ViewportHeight / 2;
//		}
//	}
//	// Blindage
//	if (pst_ViewDispInfo->pst_DisplayDatas == NULL) {
//		pst_ViewDispInfo->f_FieldOfVision =  0.27f;
//		pst_ViewDispInfo->f_ViewportWidth =  1.0f;
//		pst_ViewDispInfo->f_ViewportHeight =  1.0f;
//		pst_ViewDispInfo->f_ViewportLeft = 0.0f;
//		pst_ViewDispInfo->f_ViewportTop = 0.0f;
//	}
//	pst_ViewDispInfo->pst_DisplayDatas = DP()->mst_WinHandles.pst_DisplayData;
//	GDI_l_AttachWorld( (GDI_tdst_DisplayData *)pst_ViewDispInfo->pst_DisplayDatas, pst_World);
//	//DW() = NULL;
//	//if(pst_World != MAI_gst_MainHandles.pst_World) {
//	//	DW() = pst_World;
//	//	MATH_CopyMatrix(&DP()->mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix, &DW()->st_CameraPosSave);
//	// }
//	
//
//	// TEST
//	//pst_ViewDispInfo->pst_DisplayDatas = L_malloc(sizeof(GDI_tdst_DisplayData));
//	//L_memcpy(
//	//	pst_ViewDispInfo->pst_DisplayDatas,
//	//	GDI_gpst_CurDD,
//	//	sizeof(GDI_tdst_DisplayData));
//	// pst_ViewDispInfo->f_ViewportHeight /= 2;
//	// pst_ViewDispInfo->f_ViewportWidth /= 2;
//	// ((GDI_tdst_DisplayData *)pst_ViewDispInfo->pst_DisplayDatas)->st_Device.Vx += 10;
//	// ((GDI_tdst_DisplayData *)pst_ViewDispInfo->pst_DisplayDatas)->st_Device.Vy += 10;
//
//	// Flag pour le rendu
//	pst_World->pst_View[mi_ViewIdx].uc_Flags |= WOR_Cuc_View_AimObject;
//
//	SetWindowPos( mpo_Owner, 0, 0, 560, 400, SWP_NOMOVE );
//	CenterWindow();
//	return TRUE;
//}
//
///*
// =======================================================================================================================
// =======================================================================================================================
// */
//int EDIA_cl_Morphing3DViewDialog::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2) 
//{ 
//	switch(_ul_Msg)
//	{
//		case EDI_MESSAGE_REFRESHDLG:
//		case EDI_MESSAGE_REFRESH:
//			Refresh();
//			break;
//	}
//	return 1;
//}
//
///*
// =======================================================================================================================
// =======================================================================================================================
// */
//void EDIA_cl_Morphing3DViewDialog::Refresh(void)
//{
//	M_MF()->LockDisplay(this);
//
//	if(!ENG_gb_EngineRunning || (GetAsyncKeyState(VK_SPACE) < 0)) {
//		int temp = mpo_EngineFrame->mpo_AssociatedEditor->mi_NumEdit;
//		mpo_EngineFrame->mpo_AssociatedEditor->mi_NumEdit = mi_ViewIdx;
//		mpo_EngineFrame->mpo_DisplayView->Refresh();
//		mpo_EngineFrame->mpo_AssociatedEditor->mi_NumEdit = temp;
//	}
//
//	M_MF()->UnlockDisplay(this);
//}
//
///*
// =======================================================================================================================
// =======================================================================================================================
// */
//void EDIA_cl_Morphing3DViewDialog::OnPaint(void)
//{
//	/*~~~~~~~~~~~*/
//	CDC		*pDC;
//	/*~~~~~~~~~~~*/
//
//	EDIA_cl_BaseDialog::OnPaint();
//	
//	pDC = GetDC();
//	
//	// TODO
//		
//	ReleaseDC(pDC);
//}
//
///*
// =======================================================================================================================
// =======================================================================================================================
// */
//void EDIA_cl_Morphing3DViewDialog::OnSize(UINT n, int x, int y)
//{
//	EDIA_cl_BaseDialog::OnSize(n, x, y);
//	if (mpo_EngineFrame) mpo_EngineFrame->UpdatePosSize();
//}


#endif /* ACTIVE_EDITORS */

