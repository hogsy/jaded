/*$T DIA_SKN_dlg.h GC! 1.081 05/15/00 16:32:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define PAINT_SKIN_MAXACTIVEMATRIXNUM	4
typedef struct	tdst_PAINT_SKIN_
{
	ULONG					ul_NumberOfActivesMatrix;
	ULONG					ul_ActivesMatrixIndex[PAINT_SKIN_MAXACTIVEMATRIXNUM];
	float					tf_CurrentColors[PAINT_SKIN_MAXACTIVEMATRIXNUM];
	struct GEO_tdst_Object_ *pst_CurrentObject;
} tdst_PAINT_SKIN;

typedef struct	tdst_PAINT_SKIN_LOCAL_
{
	LONG	GizmoIndex;
	float	fValue;
	ULONG	Item;
} tdst_PAINT_SKIN_LOCAL;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_ToolBox_GROView;

class EDIA_cl_SKN_Dialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_ToolBox_GROView *mpo_GroView;

	tdst_PAINT_SKIN			*p_PSS;
	tdst_PAINT_SKIN_LOCAL	Gizmo[4];
	ULONG					UnderMouseItemIndex;
	ULONG					UnderMouseItem;
	RECT					OldClip;
	ULONG					bMousebIsDown;
	ULONG					bRMousebIsDown;

	DWORD					TimeDBCLK;
	ULONG					ItemDBCLK;
	ULONG					GizmoAreLocked;
	ULONG					GizmoAreLockedIndex;
	CString					mo_Title;

	ULONG					WasUpdateBySel;

	OBJ_tdst_GameObject		*mpst_Skn_Gao;
	ULONG					ul_NumberOfGEOM;
	GEO_tdst_Object			*mpst_Skn_Gro[256];
    POINT                   mst_Position;



/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	EDIA_cl_SKN_Dialog
	(
		char *,
		ULONG *,
		void (*) (ULONG, ULONG),
		ULONG USRPARAM1,
		ULONG USRPARAM2,
		struct OBJ_tdst_GameObject_ *,
		struct GRO_tdst_Struct_ *
	);
	~EDIA_cl_SKN_Dialog(void);
	ULONG	AllItemsNum;
	ULONG	bLocked;
	ULONG	AllItem[100];
	ULONG	AllItemStates[100];
	ULONG	CurrentColor;
	ULONG	NumberOfSelectedPoints;

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	BOOL	IF_IS_IN(int ITEM);
	ULONG	GetItemFromPoint(void);
	char	*GetGizmoName(ULONG Num);
	LONG	GetGizmoNumber(void);
	void	ComputeCurrentColor(void);
	void	UpdateSelection(void);
	void	Command(ULONG ul_CommandNum);
	int		i_OnMessage(ULONG, ULONG, ULONG);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg BOOL	OnInitDialog(void);
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnPaint(void);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint );
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnRButtonUp(UINT, CPoint);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	afx_msg BOOL	OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
	afx_msg void	OnDestroy(void);

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
