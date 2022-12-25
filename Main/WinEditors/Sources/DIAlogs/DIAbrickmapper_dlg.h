/*$T DIAbrickmapper_dlg.h GC 1.139 03/26/04 11:06:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
class	F3D_cl_View;

class EDIA_cl_BrickMapper :
	public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_BrickMapper(F3D_cl_View *_po_View);//struct WOR_tdst_World_ *);
	~EDIA_cl_BrickMapper();


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    F3D_cl_View	    *mpo_3DView;

public:
	BOOL FillAlternative(void);
	BOOL FillAmbience(void);

private:
	CString GetBrickName(CString &fileName);
	CString GetBrickNameWithAlt(CString &fileName);
	CString GetAmbienceName(CString &fileName);
	char GetTheme(CString &graphName);
	CString ChangeBrickName(CString &fileName, CString &brickName);


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	BOOL OnInitDialog(void);
	void OnClose();
	void OnDestroy(void);
	afx_msg void OnAmbienceChanged(void);
	afx_msg void OnBnMap(void);
	DECLARE_MESSAGE_MAP()
};


#endif