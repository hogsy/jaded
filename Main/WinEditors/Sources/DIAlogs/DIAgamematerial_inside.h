/*$T DIAcreategeometry_inside.h GC! 1.081 03/23/01 15:57:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __DIAGAMEMATERIALINSIDE_H__
#define __DIAGAMEMATERIALINSIDE_H__

#pragma once
#ifdef ACTIVE_EDITORS

#include "BASe/BAStypes.h"
#include "ENGine/Sources/COLlision/COLstruct.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_GameMaterial;

class EDIA_cl_GameMaterialView : public CFormView
{
	DECLARE_DYNCREATE(EDIA_cl_GameMaterialView)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_GameMaterialView(void);
	~EDIA_cl_GameMaterialView();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_GameMaterial		*mpo_Dialog;
    char                        msz_Path[ BIG_C_MaxLenPath ];
    char                        msz_Name[ BIG_C_MaxLenName ];
	BIG_INDEX					mul_Index; /* Index of the open GameMaterial file */
	COL_tdst_GameMatList		*mpst_GameMatList;
	BOOL						Link3D_On;
	BOOL						Show_Sel;


/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	Create(LPCTSTR, LPCTSTR, DWORD, const RECT &, CWnd *);
	void	AddID(void);
	void	DeleteID(int);
	void	PaintColMap(int, BOOL);
	void	OnRightClick();
	int		UpdateGameMaterial(COL_tdst_GameMatList *, int);

	void	OnButtonBrowsePath(void);
	void	OnChangeDisplay(void);
	void	OnLink(void);
	void	OnPaint(void);
	void	OnPaintSound(void);
	void	OnDisplaySound(void);
	void	OnGoto(void);
	void	OnElemID(void);

	void	UpdateName();
	void	UpdateList();
	void	OnIDDoubleClick(NMHDR *, LRESULT *);
	void	OnChangeGameMat(NMHDR * notify = NULL, LRESULT * res = NULL);
	BOOL	PreTranslateMessage(MSG *);
	void	OnKeyDown(UINT);



	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */

#endif
