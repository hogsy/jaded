/*$T DIAmorphing_dlg.h GC! 1.081 05/31/00 10:59:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

#include "BASe/BAStypes.h"

class EDIA_cl_MorphingSlidersDialog;
//class EDIA_cl_Morphing3DViewDialog;

class EDIA_cl_MorphingDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_MorphingDialog(struct MDF_tdst_Modifier_ *, struct OBJ_tdst_GameObject_ *, struct GEO_tdst_Object_ * );
	~EDIA_cl_MorphingDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	struct OBJ_tdst_GameObject_                 *mpst_Gao;
    struct GEO_tdst_Object_                     *mpst_Geo;
	struct MDF_tdst_Modifier_					*mpst_Modifier;
	struct GEO_tdst_ModifierMorphing_			*mpst_Morph;
	struct GEO_tdst_ModifierMorphing_Channel_	*mpst_CurChannel;
    struct GRO_tdst_EditOptions_	            *mpst_EditOptions;

	BOOL										mb_LockRefresh;
	int                                         mi_DataPick;
	CPoint										mo_DDPt;
	int											mi_DD;
	int											mi_DDIndex;
    int                                         mi_Capture;

    CRect                                       mo_FactorRect;
    CRect                                       mo_ProgRect;
    float                                       mf_FactorOffset;
    float                                       mf_ProgOffset;
    float                                       mf_Scale;
    float										mf_ScaleDiv;
    float                                       mf_Origine;
    
    EDIA_cl_MorphingSlidersDialog				*mpo_Sliders;

	BOOL										mb_DispDummies;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			OnInitDialog(void);
	int				i_OnMessage(ULONG, ULONG, ULONG);
	void			RefreshFromExt(void);

    int				LB_i_ItemFromPoint( CListBox *, CPoint *);

	void			Data_FillList(void);
	void			Data_Select(int);
	virtual void	Data_Display(int);
	void			Data_Pick(void);
    void			Data_Delete(int);
    int				Data_GetCur(void);

	void			Channel_FillList(void);
	void			Channel_Select(int);
	void			Channel_Display(int);
	void			Channel_GetCur(void);

	void			ChannelData_FillList(void);
	void			ChannelData_Add( BOOL );
    void			ChannelData_Del(void);

    void			Factor_Set( int, int );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			PreTranslateMessage(MSG *);

    afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnGetMinMaxInfo(MINMAXINFO FAR *);

    afx_msg void    OnCheck_VEdit( void );
    afx_msg void    OnButton_VSel( void );
    afx_msg void    OnButton_VDel( void );
    afx_msg void    OnCheck_HideVector( void );
    afx_msg void    OnCheck_HideSubObject( void );
    afx_msg void    OnButton_VSelThresh( void );

	afx_msg void	OnSelChange_DataList(void);
	afx_msg void	OnButton_DataRename(void);
	afx_msg void	OnButton_DataDelete(void);

	afx_msg void	OnButton_ChannelNew(void);
	afx_msg void	OnButton_ChannelDel(void);
	afx_msg void	OnSelChange_ChannelList(void);
	afx_msg void	OnButton_ChannelRename(void);
    afx_msg void	OnButton_InsertHole(void);
    afx_msg void	OnButton_Load(void);
    afx_msg void	OnButton_Save(void);
#ifdef JADEFUSION
    afx_msg void	OnButton_RecalcAllTangentSpaceData(void);
#endif

    afx_msg void	OnPaint(void);
	afx_msg void	OnLButtonUp(UINT , CPoint);

    afx_msg void    OnEdit_Blend(void);
    afx_msg void    OnEdit_ChannelBlend(void);

	afx_msg void	OnCheck_ExtSliders(void);
	afx_msg void	OnCheck_DisplayDummyChannels(void);
	afx_msg void	OnButton_DeleteDummyChannels(void);
	afx_msg void	OnButton_CreateDummyChannels(void);



	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
