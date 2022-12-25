/*$T DIAmorphedit_dlg.cpp */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAmorphedit_dlg.h"
#include "Res/Res.h"
#include "Engine/Sources/MODifier/MDFmodifier_GEO.h"
#include "AIinterp/Sources/AIengine.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "ENGine/Sources/OBJects/OBJorient.h"


BEGIN_MESSAGE_MAP(EDIA_cl_MorphEditDialog, EDIA_cl_MorphingDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SAVEMORPH, OnButton_SaveMorphState )
	ON_BN_CLICKED(IDC_BUTTON_LOADMORPH, OnButton_LoadMorphState )
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MorphEditDialog::EDIA_cl_MorphEditDialog (
		EEVE_cl_Frame *_mpo_Parent,
		MDF_tdst_Modifier *_pst_Modifier,
		OBJ_tdst_GameObject *_pst_GO,
		GEO_tdst_Object *_pst_Obj) : EDIA_cl_MorphingDialog(_pst_Modifier, _pst_GO, _pst_Obj)
{
	mpo_Parent = _mpo_Parent;
	mo_ChannelTracks = (EEVE_cl_Track **)MEM_p_Alloc( mpst_Morph->l_NbChannel * sizeof(EEVE_cl_Track *) );
	L_memset (mo_ChannelTracks, 0, mpst_Morph->l_NbChannel * sizeof(EEVE_cl_Track *));
}

EDIA_cl_MorphEditDialog::~EDIA_cl_MorphEditDialog ()
{
	if (mpo_Parent) mpo_Parent->mo_MorphEditor = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MorphEditDialog::OnInitDialog(void)
{
	EDIA_cl_MorphingDialog::OnInitDialog();

	// Show new buttons
	GetDlgItem(IDC_BUTTON_LOADMORPH)->EnableWindow( TRUE );
	GetDlgItem(IDC_BUTTON_SAVEMORPH)->EnableWindow( TRUE );

	// Hide unused items
	GetDlgItem(IDC_BUTTON_DATARENAME)->EnableWindow( FALSE );
	GetDlgItem(IDC_EDIT_DATANAME)->EnableWindow( FALSE );
    GetDlgItem( IDC_STATIC_NBVERTEX )->EnableWindow( FALSE );
    GetDlgItem( IDC_EDIT_DATANBPOINTS )->EnableWindow( FALSE );
    GetDlgItem( IDC_CHECK_VEDIT )->EnableWindow( FALSE );
    GetDlgItem( IDC_BUTTON_VSEL )->EnableWindow( FALSE );
    GetDlgItem( IDC_BUTTON_VDELSEL )->EnableWindow( FALSE );
    GetDlgItem( IDC_CHECK_HIDEVECTOR )->EnableWindow( FALSE );
    GetDlgItem( IDC_CHECK_HIDESUBOBJECT )->EnableWindow( FALSE );
    GetDlgItem( IDC_BUTTON_SELTHRESH )->EnableWindow( FALSE );
    GetDlgItem( IDC_EDIT_SELTHRESH )->EnableWindow( FALSE );
    GetDlgItem( IDC_BUTTON_DATADELETE )->EnableWindow( FALSE );
    GetDlgItem( IDC_STATIC_PICK )->EnableWindow( FALSE );
    GetDlgItem( IDC_CHECK_DATAPICK )->EnableWindow( FALSE );
    GetDlgItem( IDC_CHECK_DATAPICKUPDATE )->EnableWindow( FALSE );
    GetDlgItem( IDC_CHECK_DATAPICKEXT )->EnableWindow( FALSE );
    GetDlgItem( IDC_CHECK_DATAPICKEXTUPDATE )->EnableWindow( FALSE );
    GetDlgItem( IDC_CHECK_PICKONLYSEL )->EnableWindow( FALSE );
    GetDlgItem( IDC_BUTTON_SAVE )->EnableWindow( FALSE );
    GetDlgItem( IDC_BUTTON_LOAD )->EnableWindow( FALSE );

	OnButton_LoadMorphState();

	return TRUE;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphEditDialog::OnSize(UINT n, int x, int y)
{
	EDIA_cl_MorphingDialog::OnSize(n, x, y - 45);
	EDIA_cl_BaseDialog::OnSize(n, x, y);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphEditDialog::Data_Display(int _i_Data)
{
	EDIA_cl_MorphingDialog::Data_Display(_i_Data);
	GetDlgItem(IDC_CHECK_DATAPICKUPDATE)->EnableWindow( FALSE );
	GetDlgItem(IDC_CHECK_DATAPICKEXTUPDATE)->EnableWindow( FALSE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphEditDialog::OnButton_SaveMorphState(void)
{
	//TODO
	//po_Track = o_GetFirstNotNULLChannelTrack();
	//po_Track->mo_ListEvents[po_Track->mpst_Param->uw_CurrentEvent]
	return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
extern void AI_EvalFunc_OBJ_SetChannelFactor_C(OBJ_tdst_GameObject *_pst_Obj, int i, float f);
extern void AI_EvalFunc_OBJ_SetChannelProg_C(OBJ_tdst_GameObject *_pst_Obj, int i, float f);
#else
extern "C" void AI_EvalFunc_OBJ_SetChannelFactor_C(OBJ_tdst_GameObject *_pst_Obj, int i, float f);
extern "C" void AI_EvalFunc_OBJ_SetChannelProg_C(OBJ_tdst_GameObject *_pst_Obj, int i, float f);
#endif

 void EDIA_cl_MorphEditDialog::OnButton_LoadMorphState(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Track						*po_Track;
	EEVE_cl_Event						*po_Event;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	POSITION							pos_TrackPos;
	MATH_tdst_Vector					*pst_TempVect;
	int									i_DataChainIdx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Update datas. Search for the tracks associated to the channels
	UpdateChannelTracks();

	// For each channel, load data chain, progression and factor from the tracks
	for (int i_ChannelIdx=0; i_ChannelIdx < mpst_Morph->l_NbChannel; i_ChannelIdx++)
	{
		po_Track = mo_ChannelTracks[i_ChannelIdx];
		if (!po_Track) continue;   // TODO : créer les tracks manquantes

		pst_Channel = mpst_Morph->dst_MorphChannel + i_ChannelIdx;
		// Reset data chain
		MEM_Free(pst_Channel->dl_DataIndex);
		pst_Channel->dl_DataIndex = NULL;
		pst_Channel->l_NbData = 0;
		// Create data chain
		// First data is always neutral pose. WE ASSUME NEUTRAL POSE IS INDEX 0 !
		GEO_l_ModifierMorphing_AddChannelData(mpst_Modifier, i_ChannelIdx);
		pst_Channel->dl_DataIndex[pst_Channel->l_NbData - 1] = 0;
		for (int i_EventIdx=0; i_EventIdx < po_Track->mo_ListEvents.GetSize(); i_EventIdx++)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(i_EventIdx);
			if (!po_Event->b_TestAIFunctionType(FUNCTION_OBJ_MODULEMORPHSETCHDATA))
				continue;
			// Add data to the data chain of the channel
			// First create the datas
			i_DataChainIdx = po_Event->i_GetIntParameter(2);
			if (i_DataChainIdx < 0) i_DataChainIdx = pst_Channel->l_NbData;
			for (int idx = pst_Channel->l_NbData; idx <= i_DataChainIdx; idx++)
				GEO_l_ModifierMorphing_AddChannelData(mpst_Modifier, i_ChannelIdx);
			// Then set the value
			pst_Channel->dl_DataIndex[i_DataChainIdx] = po_Event->i_GetIntParameter(3);
		}

		// Get next track. This should contain the interpolation keys which control the
		// OBJ_MorphAnimFactorNoKey function
		pos_TrackPos = mpo_Parent->mo_ListTracks.Find(po_Track);
		po_Track = mpo_Parent->mo_ListTracks.GetNext(pos_TrackPos);
		po_Track = mpo_Parent->mo_ListTracks.GetNext(pos_TrackPos);

		// Get the position of the Gao associated to this track
		pst_TempVect = OBJ_pst_GetAbsolutePosition( po_Track->mpst_Track->pst_GO );

		// Set progression and factor according to this position
		AI_EvalFunc_OBJ_SetChannelFactor_C(
			mpst_Gao, 
			i_ChannelIdx,
			pst_TempVect->x);
		AI_EvalFunc_OBJ_SetChannelProg_C(
			mpst_Gao,
			i_ChannelIdx,
			pst_TempVect->y + 1.0f);
	}

	return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphEditDialog::UpdateChannelTracks()
{
	POSITION					pos;
	int							index;
	EEVE_cl_Track				*po_Track;
	EEVE_cl_Event				*po_Event;
	OBJ_tdst_GameObject			*pst_GO;

	// Reset
	L_memset (mo_ChannelTracks, 0, mpst_Morph->l_NbChannel * sizeof(EEVE_cl_Track *));

	// Find selected track
	pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
		for(index = 0; index < po_Track->mo_ListEvents.GetSize(); index++)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(index);
			if (po_Event->b_TestAIFunctionType(FUNCTION_OBJ_MODULEMORPHSETCHDATA))
			{
				// We've got the right AI function. Is it the right GAO ?
				pst_GO = po_Track->mpst_Track->pst_GO;
				pst_GO = ANI_pst_GetObjectByAICanal( pst_GO, (UCHAR)po_Event->i_GetIntParameter(0));
				if (pst_GO == mpst_Gao) break;
			}
		}
		if (index != po_Track->mo_ListEvents.GetSize())
		{
			// Fill mo_ChannelTracks with new track
			index = po_Event->i_GetIntParameter(1);
			mo_ChannelTracks[index] = po_Track;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Track *EDIA_cl_MorphEditDialog::o_GetFirstNotNULLChannelTrack()
{
	int idx;
	for (idx = 0; idx < mpst_Morph->l_NbChannel; idx++)
	{
		if (mo_ChannelTracks[idx] != NULL) return mo_ChannelTracks[idx];
	}
	return NULL;
}

#endif /* ACTIVE_EDITORS */
