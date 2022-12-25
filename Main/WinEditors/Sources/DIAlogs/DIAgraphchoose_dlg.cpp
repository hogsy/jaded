/*$T DIAbrickmapper_dlg.cpp GC 1.134 06/08/04 13:34:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "EDIpaths.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "Res/Res.h"
#include "DIAlogs/DIAgraphchoose_dlg.h"
#include "SELection/SELection.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "BIGfiles/LOAding/LOAdefs.h"


/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_GraphChoose, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(IDOKGRAPHCHOOSE, OnChoose)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */

EDIA_cl_GraphChoose::EDIA_cl_GraphChoose(CString brickName, BIG_INDEX h_File) :
	EDIA_cl_BaseDialog(IDD_CHOOSE_GRAPH_VERSION)	//(struct WOR_tdst_World_ *W)
{
		p_BrickName = new CString(brickName);
		p_File = h_File;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GraphChoose::~EDIA_cl_GraphChoose()
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GraphChoose::OnInitDialog(void)
{
	CComboBox *o_ComboGraph;
	EDIA_cl_BaseDialog::OnInitDialog();

	CString graphName(BIG_NameFile(p_File));

	while (graphName.Find(*p_BrickName) != -1)
	{
		CString graphVersion;
		graphVersion.AppendChar(graphName[p_BrickName->GetLength()+1]);

		o_ComboGraph = (CComboBox *) GetDlgItem(IDC_GRAPHVERSION);		
		o_ComboGraph->AddString(graphVersion);
        
		p_File = BIG_NextFile(p_File);
	}

	return true;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GraphChoose::OnChoose(void)
{
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GraphChoose::OnDestroy(void)
{
	delete p_BrickName;
}



#endif