/*$T DIAcreategeometry_dlg.h GC! 1.081 03/26/01 09:21:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"
#include "VAVview/VAVlist.h"
#include "VAVview/VAVview.h"

class	F3D_cl_View;
class   EDIA_cl_GameMaterialView;

/*$4
 ***********************************************************************************************************************
    Class EDIA_cl_CreateGeometry
 ***********************************************************************************************************************
 */

class EDIA_cl_GameMaterial : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_GameMaterial(F3D_cl_View *, char *);
	~EDIA_cl_GameMaterial(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_cl_View										*mpo_3DView;
	CFrameWnd										*mpo_Frame;
    EDIA_cl_GameMaterialView	                    *mpo_View;
	CSplitterWnd									*mpo_MainSplitter;
	EVAV_cl_View									*mpo_DataView;
	EVAV_tdst_VarsViewStruct						mst_DataView;
	CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>	mo_ListItems;
	char											*msz_Path;

public:
	BOOL	OnInitDialog(void);
	void	OnSize(UINT, int, int);
	BOOL	PreTranslateMessage(MSG *);

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
