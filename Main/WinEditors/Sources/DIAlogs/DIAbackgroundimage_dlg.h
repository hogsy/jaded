/*$T DIAbackgroundimage_dlg.h GC! 1.081 05/09/00 12:03:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"

class	F3D_cl_View;

class EDIA_cl_BackgroundImage : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_BackgroundImage(F3D_cl_View *);
	~EDIA_cl_BackgroundImage(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_cl_View							*mpo_View;
	struct SOFT_tdst_BackgroundImage_	*mpst_BI;
    struct SOFT_tdst_BackgroundImage_   *mpst_LocalBI;

	CRect								mo_SizeAndPos;
    int									mi_MoveOrSize;
	
    /*$F
    CRect								mo_DepthRect;
    CPoint								mo_DepthPoint;
    */
    int									mi_DepthMove;
	
    CPoint								mo_PtWhenLBDown;
	RECT								mst_OldClipCursor;
	int									mi_WindowWidth;
	int									mi_WindowHeight;
	

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);

	ULONG	ul_GetPicture(char *, char *);
	void	UpdateData(void);

	void	RectBI2W(void);
	void	RectW2BI(void);

    void    UpdateFreq( int );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnPaint(void);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT , CPoint);
    afx_msg void	OnLButtonDblClk(UINT, CPoint);
    afx_msg void	OnRButtonDown(UINT, CPoint);
    afx_msg void	OnLoadClick(void);
    afx_msg void	OnSaveClick(void);
	afx_msg void	OnUpdateDataClick(void);
	afx_msg void	OnFirstPictureClick(void);
	afx_msg void	OnLastPictureClick(void);
	afx_msg void	OnMpegClick(void);
	afx_msg void	OnVisibleClick(void);
	afx_msg void	OnFreezeClick(void);
    afx_msg void	OnMpegBWClick(void);
    afx_msg void	OnOutOfBFClick(void);
    afx_msg void    OnRadioMpeg(void);
    afx_msg void    OnRadioPictureList(void);
    afx_msg void    OnAnimFreqChange(void);
    afx_msg void    OnFilmFreqChange(void);
    afx_msg void    OnAnimFreqSelChange(void);
    afx_msg void    OnFilmFreqSelChange(void);
    afx_msg void    OnEditDepth(void);
    afx_msg void    OnUpdateDepth(void);
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
