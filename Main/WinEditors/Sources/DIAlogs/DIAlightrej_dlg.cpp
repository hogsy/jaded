/*$T DIAtoolbox_dlg.cpp GC! 1.100 06/15/01 12:26:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAlightrej_dlg.h"
#include "Res/Res.h"
#include "EDIpaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "engine/sources/objects/objload.h"
#include "GraphicDK\Sources\GEOmetric\GEOload.h"
#include "SDK\Sources\LINks\LINKtoed.h"
#include "BIGfiles\BIGread.h"
#include "BIGfiles\SAVing\SAVdefs.h"
#include <algorithm>


/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */


/*$4
 ***********************************************************************************************************************
    EDIA_cl_LightRejectDialog Message map
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_LightRejectDialog, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(IDC_CLOSE_LIGHTREJECT, OnCloseLightReject)
	ON_BN_CLICKED(IDC_REJECTION_LIGHT_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_REJECTION_LIGHT_REM, OnButtonRem)
	ON_BN_CLICKED(IDC_REJECTION_LIGHT_UPDATE, OnButtonUpdate)
	ON_BN_CLICKED(IDC_REJECTION_LIGHT_SCAN, OnButtonRefresh)
	ON_BN_CLICKED(IDC_REJECTION_LIGHT_STAT, OnButtonStatistics)
	ON_BN_CLICKED(IDC_REJECTION_LIGHT_CLEAR, OnButtonClear)
	ON_LBN_DBLCLK(IDC_REJECTION_LIGHT_LIST, OnListDblClk)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    EDIA_cl_LightRejectDialog implementation
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_LightRejectDialog::EDIA_cl_LightRejectDialog(F3D_cl_View *_po_View) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_TOOLBOX)
{
	mpo_View = _po_View;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_LightRejectDialog::~EDIA_cl_LightRejectDialog(void)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_LightRejectDialog::Refresh()
{
	// reset listbox content
	CListBox* pListBox = (CListBox *) GetDlgItem(IDC_REJECTION_LIGHT_LIST);

	// no listbox
	if(! pListBox ) 
        return;

	// reset content of listbox
	pListBox->ResetContent();

	// no world loaded
	if ( (*mpo_View).mst_WinHandles.pst_World == NULL) 
		return;

	const WOR_World_LightsVector *world_lights = ( WOR_World_LightsVector * ) ( mpo_View->mst_WinHandles.pst_World->st_Lights );
		
	// no lights
	if ( world_lights->empty() )
		return;

	for ( auto it = world_lights->begin(); it != world_lights->end(); ++it )
	{
		OBJ_tdst_GameObject *pst_LightGO = it->first;

		LIGHT_tdst_Light* pst_Light = (LIGHT_tdst_Light*)pst_LightGO->pst_Extended->pst_Light;
		if ( ! (pst_Light->ul_Flags & LIGHT_Cul_LF_ExtendedLight) ) 
			continue;

		int index = pListBox->AddString(pst_LightGO->sz_Name);
		pListBox->SetItemDataPtr(index, pst_LightGO);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_LightRejectDialog::OnInitDialog(void)
{
	/* Base class call */
	EDIA_cl_BaseDialog::OnInitDialog();

	OnButtonRefresh();

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LightRejectDialog::OnSize(UINT id, int cx, int cy)
{
	CWnd *pWnd;

	pWnd = GetDlgItem(IDC_REJECTION_LIGHT_LIST);
	if(pWnd)
		pWnd->MoveWindow(CRect(10,145,cx-12,cy-5));

	EDIA_cl_BaseDialog::OnSize(id, cx, cy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LightRejectDialog::OnCloseLightReject(void)
{
	mpo_View->Selection_LightReject();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LightRejectDialog::OnListDblClk()
{
	CListBox					*LB;
	int							nbItems;
	int							index = -1;
	OBJ_tdst_GameObject			*pLight;
	SEL_tdst_SelectedItem		*pst_Sel, *pst_Next;

	// if no world loaded...
	if(!mpo_View->mst_WinHandles.pst_World) 
		return;

	// if no lights selected...
	LB = (CListBox *) GetDlgItem(IDC_REJECTION_LIGHT_LIST);
	if(!LB) 
		return;

	nbItems = LB->GetCount();

	if(nbItems==0) 
		return;

	// only one item can be selected
	while(nbItems--)
		if(LB->GetSel(nbItems))
		{
			if(index==-1)
				index = nbItems;
			else 
				return;
		}

	if(index==-1) 
		return;
	
	pLight = (OBJ_tdst_GameObject *)LB->GetItemDataPtr(index);

	// Selection is now this light's rejected objects

	// clear selection	
	if(mpo_View->mst_WinHandles.pst_World->pst_Selection)
	{
		pst_Sel = mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
		while(pst_Sel)
		{
			pst_Next = pst_Sel->pst_Next;
			mpo_View->Selection_Unselect(pst_Sel->p_Content, pst_Sel->l_Flag);
			pst_Sel = pst_Next;
		}
	}

	// add rejected objects to selection
    for( ULONG ulObj = 0; ulObj < g_LightRejectionList.GetObjectCount(); ++ulObj )
    {
        RejectedLights * pLights = g_LightRejectionList.GetRejectionList( ulObj );
        OBJ_tdst_GameObject * pObject = g_LightRejectionList.GetObject( ulObj );
        RejectedLights::iterator itLight = pLights->begin();
        for( ; itLight != pLights->end(); ++itLight )
        {
            if( *itLight == pLight )
            {
                mpo_View->Selection_Object(pObject, SEL_C_SIF_Object);
            }
        }
    }

	LINK_Refresh();

	mpo_View->mb_SelectionChange = TRUE;
	mpo_View->Selection_Change();
}

/*
 =======================================================================================================================
 add light/object reference for all selected light/object
 =======================================================================================================================
 */
void EDIA_cl_LightRejectDialog::OnButtonAdd(void)
{
	CListBox				*LB;
	int						i;
	int						nbItems;
	OBJ_tdst_GameObject		*pObject;
	OBJ_tdst_GameObject		*pLight;
	SEL_tdst_SelectedItem	*pst_Sel;

	// if no world loaded...
	if(!mpo_View->mst_WinHandles.pst_World) return;

	// if no lights selected...
	LB = (CListBox *) GetDlgItem(IDC_REJECTION_LIGHT_LIST);
	if(!LB) 
		return;
	nbItems = LB->GetCount();
	
	if(nbItems==0) 
		return;

	// if no objects selected...
	if(!mpo_View->mst_WinHandles.pst_World->pst_Selection) 
		return;
	
	pst_Sel = mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
	
	if(!pst_Sel) 
		return;

	// for all selected objects
	while(pst_Sel)
	{
		if(pst_Sel->l_Flag & SEL_C_SIF_Object)
		{
			// for all selected lights
			for(i=0; i<nbItems; i++)
			{
				if(LB->GetSel(i))
				{
					// create light/object reference
					pLight = (OBJ_tdst_GameObject *)LB->GetItemDataPtr(i);
					pObject = (OBJ_tdst_GameObject *)pst_Sel->p_Content;
					LRL_Add(pLight, pObject);
				}
			}
		}
		pst_Sel = pst_Sel->pst_Next;
	}	

    LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LightRejectDialog::OnButtonRem(void)
{
	CListBox				*LB;
	int						i;
	int						nbItems;
	OBJ_tdst_GameObject		*pObject;
	OBJ_tdst_GameObject		*pLight;
	SEL_tdst_SelectedItem	*pst_Sel;

	// if no world loaded...
	if(!mpo_View->mst_WinHandles.pst_World) return;

	// if no lights selected...
	LB = (CListBox *) GetDlgItem(IDC_REJECTION_LIGHT_LIST);
	if(!LB) 
		return;
	
	nbItems = LB->GetCount();
	
	if(nbItems==0) 
		return;

	// if no objects selected...
	if(!mpo_View->mst_WinHandles.pst_World->pst_Selection) 
		return;
	
	pst_Sel = mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
	
	if(!pst_Sel) 
		return;

	// for all selected objects
	while(pst_Sel)
	{
		if(pst_Sel->l_Flag & SEL_C_SIF_Object)
		{
			// for all selected lights
			for(i=0; i<nbItems; i++)
			{
				if(LB->GetSel(i))
				{
					// create light/object reference
					pLight = (OBJ_tdst_GameObject *)LB->GetItemDataPtr(i);
					pObject = (OBJ_tdst_GameObject *)pst_Sel->p_Content;
					LRL_Rem(pLight, pObject);
				}
			}
		}

		pst_Sel = pst_Sel->pst_Next;
	}	

    LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LightRejectDialog::OnButtonUpdate(void)
{
	CListBox				*LB;
	int						i;
	int						nbItems;
	OBJ_tdst_GameObject		*pLight;

	// if no world loaded...
	if(!mpo_View->mst_WinHandles.pst_World) return;

	// if no lights selected...
	LB = (CListBox *) GetDlgItem(IDC_REJECTION_LIGHT_LIST);
	if(!LB) 
		return;
	
	nbItems = LB->GetCount();
	
	if(nbItems==0) 
		return;

	// for all selected lights
	for(i=0; i<nbItems; i++)
	{
		if(LB->GetSel(i))
		{
			pLight = (OBJ_tdst_GameObject *)LB->GetItemDataPtr(i);

			// remove all light/object reference for selected lights
            for( ULONG ulObj = 0; ulObj < g_LightRejectionList.GetObjectCount(); ++ulObj )
            {
                RejectedLights * pLights = g_LightRejectionList.GetRejectionList( ulObj );
                pLights->erase( std::remove(pLights->begin(), pLights->end(), pLight), pLights->end() );
            }
		}
	}

	// add selected objects
	OnButtonAdd();

    LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_LightRejectDialog::OnButtonRefresh(void)
{
	Refresh();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_LightRejectDialog::OnButtonClear(void)
{
	g_LightRejectionList.Clear();
	Refresh();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_LightRejectDialog::OnButtonStatistics(void)
{
	typedef std::set< OBJ_tdst_GameObject * > tdsetGAOsNotLighted;
	typedef std::map< OBJ_tdst_GameObject *, tdsetGAOsNotLighted * > tdmapGAOsNotLightedByLight;

	char szOutput[1024];
		
	tdmapGAOsNotLightedByLight mapGAONotLightedByLight;
	tdmapGAOsNotLightedByLight::iterator iterGAONotLightedByLight;

	// Title:
	LINK_PrintStatusMsg("Light Rejection List: Statistics");
	LINK_PrintStatusMsg("--------------------------------");

	// Output number of GAO in list
	UINT ul_ObjectCount = g_LightRejectionList.GetObjectCount();
	sprintf(szOutput, "Total of GAOs in list : %d", ul_ObjectCount);
	LINK_PrintStatusMsg(szOutput);

	// Output number of Lights in list
	RejectedLights setLightsAll;
	for( ULONG ulObj = 0; ulObj < g_LightRejectionList.GetObjectCount(); ++ulObj )
	{
		RejectedLights * pLightsRejected = g_LightRejectionList.GetRejectionList( ulObj );
		setLightsAll.insert(pLightsRejected->begin(), pLightsRejected->end());
	}
	UINT ul_LightCount = setLightsAll.size();
	sprintf(szOutput, "Total of LIGHTs in list : %d", ul_LightCount);
	LINK_PrintStatusMsg(szOutput);
		
	// Output each GAO rejected by each lights
	RejectedLights::iterator iterLight;
	for( iterLight = setLightsAll.begin(); iterLight != setLightsAll.end(); iterLight++ )
	{
		tdsetGAOsNotLighted* pGAOsNotLighted = new tdsetGAOsNotLighted; 
		iterGAONotLightedByLight = mapGAONotLightedByLight.insert(tdmapGAOsNotLightedByLight::value_type((*iterLight), pGAOsNotLighted)).first;

		for( ULONG ulObj = 0; ulObj < g_LightRejectionList.GetObjectCount(); ++ulObj )
		{
			RejectedLights* pLightsRejected = g_LightRejectionList.GetRejectionList( ulObj );
			RejectedLights::iterator iterFind = pLightsRejected->find((*iterLight));
			if ( iterFind != pLightsRejected->end() )
			{
				(iterGAONotLightedByLight->second)->insert(g_LightRejectionList.GetObject(ulObj));
			}
		}

		tdsetGAOsNotLighted::iterator IterGAOsNotLighted = (iterGAONotLightedByLight->second)->begin();
		tdsetGAOsNotLighted::iterator IterGAOsNotLightedEnd = (iterGAONotLightedByLight->second)->end();

		LINK_PrintStatusMsg("");
		sprintf(szOutput, "Light %s doesn't light %d GAOs : ", (iterGAONotLightedByLight->first)->sz_Name, (iterGAONotLightedByLight->second)->size() );
		LINK_PrintStatusMsg(szOutput);
		
		memset(szOutput, '-', strlen(szOutput));
		LINK_PrintStatusMsg(szOutput);
				
		memset(szOutput, 0, 1024);
		for ( ; IterGAOsNotLighted != IterGAOsNotLightedEnd ; IterGAOsNotLighted++ )
		{
			OBJ_tdst_GameObject* pGAO5 = (*IterGAOsNotLighted);
			strcat(szOutput, (*IterGAOsNotLighted)->sz_Name);
			strcat(szOutput, ",  ");

			if ( strlen(szOutput) > 80)
			{
				LINK_PrintStatusMsg(szOutput);
				memset(szOutput, 0, 1024);
			}
		}
		LINK_PrintStatusMsg(szOutput);
	}
}

#endif