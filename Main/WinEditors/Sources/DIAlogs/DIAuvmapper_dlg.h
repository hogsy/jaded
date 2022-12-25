/*$T DIAuvmapper_dlg.h GC! 1.081 08/23/01 10:02:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"

#include "DIAlogs/DIAbase.h"
#include "DIALOGS/DIAtoolbox_groview.h"

#include "SOFT/SOFTUVGizmo.h"

typedef struct	EDIA_tdst_UV_
{
    float   u, v;
    int		x, y;
	char	c_Sel;
	char	c_Show;
	char	c_Visible;
	char	c_Used;
	int		i_Ref;
} EDIA_tdst_UV;

class F3D_cl_Undo_UVMoving;

class EDIA_cl_UVMapperDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_UVMapperDialog(EDIA_cl_ToolBox_GROView *);
	~EDIA_cl_UVMapperDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    DATA.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_ToolBox_GROView			*mpo_GroView;
	
	struct SOFT_tdst_Helpers_		*mpst_3DHelpers;
	struct GRO_tdst_EditOptions_	*mpst_EditOptions;

    int                             mi_NbGO;
    struct OBJ_tdst_GameObject_		*mpst_GO[4];
    struct GEO_tdst_Object_			*mpst_Gro[4];
    
	struct MAT_tdst_Material_		*mpst_Mat;
    struct MAT_tdst_Material_       *mpst_MatUsed;
    struct MAT_tdst_MTLevel_	    *mpst_MTLused;
    struct GEO_tdst_UV_				*mpst_SaveUV[4];

	int								mi_NbSubMat;

	CRect							mo_MapRect;
	int								mi_BtMin, mi_BtMax, mi_BtOffset;

	HBITMAP							mh_BitmapRGB, mh_BitmapA;
	int								mi_BmpWidth, mi_BmpHeight;
    CBitmap                         mo_MapBmp;
    CDC                             mo_MapDC;
    BOOL                            mb_MapBmpInit;
    
    BOOL							mb_ShowAdditionalData;

    float                           mf_X0, mf_Y0, mf_Y1, mf_Scale;
    float							mf_ScaleX, mf_ScaleY;
    BOOL							mb_KeepRatio;
    int								mi_KeepRatioType;
    BOOL							mb_FeepRatio_XisRef;
    float							mf_RatioToKeep;
    int								mi_GridWidth;
    int								mi_DisplayGrid;
    int								mi_DisplayOrigin;
        
    HICON							hIconGizmo[ 4 ];

    BOOL                            mb_Transform;
    BOOL                            mb_UseTransform;
    MAT_tdst_Decompressed_UVMatrix  mst_UVTransform, mst_UVTransformInv;
    
	int								mi_Capture;
	CPoint							mo_CapturePt;
    int                             mi_PickableGO;
	int								mi_Pickable;
	CRect							mo_SelRect;
    int                             mi_ColorConfig;

	EDIA_tdst_UV					*mpst_UV[ 4 ];
    long                            ml_NbTotalUV;
	long							ml_NbUV[ 4 ];
    ULONG                           *mpul_Lines[ 4 ];
    ULONG                           mul_NbLines[ 4 ];
    ULONG                           mul_NbLinesMax[ 4 ];

	int								mi_Helper;
	int								mi_HelperVisible;
	int								mi_HelperPick;
	float							mf_HelperX;
	float							mf_HelperY;
	int								mi_HelperX;
	int								mi_HelperY;
	float							mf_HelperRot;
	float							mf_HelperStart, mf_HelperCur, mf_HelperEnd;
	int								mi_HelperStartX;
	int								mi_HelperStartY;

    int                             mai_BandAlignFace[ 4 ];
    
    int								mi_Undo_StackType;
    F3D_cl_Undo_UVMoving			*mpo_Undo_stack;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
#ifdef JADEFUSION
    void    UpdateForXenon(void);
#endif

	void	UpdateGao(void);

    void    UpdateUV_AddLine( int, USHORT, USHORT );
	void	UpdateUV( BOOL _b_ChangeMat = TRUE );
	void	UVSelNone(void);

    void    SetMaterial( int );
	void	SetTexture(int);
    void    SetUVTransform(void);
    void    UV_TransformIn( void );
    void    UV_TransformOut( void );

	int		WheelZoom(int);

	void	Tree_AddSingle(struct MAT_tdst_Single_ *, CTreeCtrl *, HTREEITEM);
	void	Tree_AddMultiTexture(struct MAT_tdst_MultiTexture_ *, CTreeCtrl *, HTREEITEM);
	void	Tree_AddMulti(struct MAT_tdst_Multi_ *, CTreeCtrl *, HTREEITEM);

	void	UV_Rotate( float );
	void	UV_Zoom( float, char );
	void	UV_Move( int, float, float );
	void	UV_Save(void);
	void	UV_Restore(void);
	BOOL	UV_StackUndo( int );
	void	UV_PushUndo(BOOL, int);
	void	UV_Detach(void);
    void    UV_WeldThresh( float );
    BOOL    UV_CanBeAligned( void );
    void    UV_BandAlign( void );

	void	SelRect_Draw(void);
	void	SelRect_Sel(void);

	int		Helper_Pick(CPoint &);
	float	Helper_PickCircle(CPoint &);

	void	DeleteBitmap( void );
    void    UpdateMapBmp( void );
    void	UpdateScale( void );
    
    void	OnReset(void);
	void	OnGotoOrigin(void);
	void	OnUpdateUVTransform(void);
	void	DoZoom( int );
	
	void	RecalcLayout( BOOL);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			PreTranslateMessage(MSG *);

	afx_msg void	OnDestroy(void);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnGetMinMaxInfo(MINMAXINFO FAR *);
	afx_msg void	OnPaint(void);
	afx_msg void	OnChangeTexture(NMHDR *, LRESULT *);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg void	OnLButtonDblClk(UINT, CPoint);
	afx_msg void	OnMButtonDown(UINT, CPoint);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnRedraw(void);
	afx_msg void	OnZoomAll(void);
	afx_msg void	OnUVGizmo(void);
	afx_msg void	OnWeldThresh(void);
    afx_msg void	OnUVTransform(void);
    afx_msg void    OnRefresh( void );
    afx_msg void    OnShowDataEx( void );
	afx_msg void    OnHideDataEx( void );
	afx_msg void    OnKeepRatio( void );
	
	afx_msg void	OnWindowPosChanged( WINDOWPOS * );


	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
