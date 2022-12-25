/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "DIAlogs/DIAvector_dlg.h"
#include "Res/Res.h"
#include "VAVview/VAVlist.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_VectorDialog::EDIA_cl_VectorDialog(void) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_EVAV_VECTOR)
{
	EDIA_cl_VectorDialog(3);
}

EDIA_cl_VectorDialog::EDIA_cl_VectorDialog(int dim) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_EVAV_VECTOR)
{
	if((dim > 0) && (dim < 4))
		mi_dim = dim;	
	else
		mi_dim = 3;	
}

/*
 ===================================================================================================
 ===================================================================================================
 */

EVAV_CEdit o_De1;
EVAV_CEdit o_De2;
EVAV_CEdit o_De3;

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_VectorDialog::DoDataExchange(CDataExchange *pDX)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Temp[100];
    CString o_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    CDialog::DoDataExchange(pDX);
    if(!pDX->m_bSaveAndValidate)
    {
		o_De1.i_Type = 2;
		o_De1.SubclassDlgItem(IDC_EDITX, this);
		o_De2.i_Type = 2;
		o_De2.SubclassDlgItem(IDC_EDITY, this);
		o_De3.i_Type = 2;
		o_De3.SubclassDlgItem(IDC_EDITZ, this);

        sprintf(asz_Temp, "%.4f", mst_Vector.x);
        DDX_Text(pDX, IDC_EDITX, CString(asz_Temp));
        sprintf(asz_Temp, "%.4f", mst_Vector.y);
        DDX_Text(pDX, IDC_EDITY, CString(asz_Temp));
        sprintf(asz_Temp, "%.4f", mst_Vector.z);
        DDX_Text(pDX, IDC_EDITZ, CString(asz_Temp));
		
		GetDlgItem(IDC_EDITX)->EnableWindow(mi_dim>=1? TRUE : FALSE);
		GetDlgItem(IDC_EDITY)->EnableWindow(mi_dim>=2? TRUE : FALSE);
		GetDlgItem(IDC_EDITZ)->EnableWindow(mi_dim>=3? TRUE : FALSE);
    }
    else
    {
        DDX_Text(pDX, IDC_EDITX, o_String);
        sscanf((char *) (LPCSTR) o_String, "%f", &mst_Vector.x);
        DDX_Text(pDX, IDC_EDITY, o_String);
        sscanf((char *) (LPCSTR) o_String, "%f", &mst_Vector.y);
        DDX_Text(pDX, IDC_EDITZ, o_String);
        sscanf((char *) (LPCSTR) o_String, "%f", &mst_Vector.z);
    }
}
#endif  /* ACTIVE_EDITORS */
