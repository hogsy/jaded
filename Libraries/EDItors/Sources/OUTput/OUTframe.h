/*$T OUTframe.h GC! 1.086 07/06/00 17:57:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#include "BASe/BAStypes.h"

#ifdef ACTIVE_EDITORS
#include "EDIbaseframe.h"
#include "OUTframe_act.h"
#include "Res/Res.h"
#include "BIGfiles/BIGfat.h"

#ifdef JADEFUSION
#include "BASe/BASarray.h"
#include <vector>
#endif
/*$4
 ***********************************************************************************************************************
	MACROS
 ***********************************************************************************************************************
 */

#define DP()	(mpo_EngineFrame->mpo_DisplayView)
#define DW()	(DP()->mst_WinHandles.pst_World)
#define DDD()	(DP()->mst_WinHandles.pst_DisplayData)

/*$4
 ***********************************************************************************************************************
	CONSTANTS
 ***********************************************************************************************************************
 */
#define EOUT_SelFlags_Prefab		0x00000001

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
#ifdef JADEFUSION
class clGroup
{
public:
    BIG_KEY             m_ulWOWKey;
    BAS_tdst_barray     m_groupArray;

    M_DeclareOperatorNewAndDelete();
    clGroup( BIG_KEY ulWOWKey ) { m_ulWOWKey = ulWOWKey; }
    ~clGroup() { BAS_bfree( &m_groupArray ); }
};

typedef std::vector< clGroup* >	stGroup_Table;
#endif
typedef struct	EOUT_tdst_CurveParam_
{
	struct OBJ_tdst_GameObject_ *pst_GAO;
	struct EVE_tdst_Data_		*pst_Data;
    struct EVE_tdst_Data_		*pst_NewData;
	struct EVE_tdst_Track_		*pst_Track;
} EOUT_tdst_CurveParam;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	F3D_cl_Frame;
class EOUT_cl_Frame : public EDI_cl_BaseFrame
{
	DECLARE_DYNCREATE(EOUT_cl_Frame)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EOUT_cl_Frame(void);
	~EOUT_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_cl_Frame					*mpo_EngineFrame;

	struct
	{
        /* ini data */
        ULONG           ul_Mark; 
        ULONG           ul_Version;

        /* world */
		BOOL			b_EngineDisplay;
        unsigned char	uc_ExportOnlySelection;
        unsigned char   uc_ExportTexture;
        unsigned char   uc_WorldDummy[2];

        /* camera */
		BOOL			b_EngineCam;

        /* edition */
		BOOL			b_SnapToGrid;
		BOOL			b_SnapAngle;
		float			f_SnapGridSizeXY;
		float			f_SnapAngleStep;

        /* display */
		ULONG			ul_DisplayFlags;
		ULONG			ul_DrawMask;
		ULONG			ul_ConstantColor;

        /* grille */
		float			f_Unused;
		ULONG			ul_GridFlags;
        ULONG			ul_GridKeyForZLocked;
        float			f_GridDeltaZ;
        unsigned char	uc_GridPaintValue;
        unsigned char	uc_GridDummy[3];

        /* view */
		unsigned char	uc_SelDialogLeft;
        unsigned char   uc_CopyMatrixFrom_Flags;
        unsigned char   uv_ViewDummy[2];
        ULONG           ul_BackgroundImage;
        int				DistCompute;
		
        /* screen */
		ULONG			ul_ScreenFlags;
		float			f_PixelYoverX;
		float			f_ScreenYoverX;
		LONG			l_ScreenFormat;

		ULONG			ul_SelMinimize;
		ULONG			ul_PickingBufferFlags;

        /* sub object */
		ULONG			ul_VertexColor[8];
        float           f_VertexSize;
		ULONG			ul_EdgeColor[8];
        float           f_EdgeSize;
        ULONG           ul_FaceColor[8];
        float           f_FaceSize;
        ULONG           ul_NormalColor[4];
        float           f_NormalSize;
		float			f_NormalLength;

		/* texture manager */
        ULONG           ul_TextureManager_Flags;
        ULONG           ul_TextureManager_MaxSize;
        ULONG           ul_TextureManager_MaxMem;

        /* dummy buffer */
        BOOL            b_SnapGridDisplay;
        float           f_SnapGridWidth;
        float           f_UVWeldThresh;
		BOOL			DispStrips;
        float           f_VertexWeldThresh;
        ULONG           ul_TextureManager_MaxMemInterface;

		ULONG			ul_BoxDlg;
        float           f_WPSize;
        ULONG           ul_WPFlags;

        ULONG           ul_WiredColorSel;
        ULONG           ul_WiredColor;

        ULONG           ul_UVMapperColors[14];

        /* morph vector */
        ULONG           ul_MorphVectorColor[2];
        ULONG           ul_TextureManager_MinSize;
		ULONG			ul_GridPaintMode;
		
		ULONG			ul_SelFlags;
		char			c_Teleport_KeepRelativePos;
		char			c_DuplicateExtended;
		char			c_ShowHistogramme;

		char			c_Dummy[ 1 ];
		ULONG			ul_FaceIDColor[ 32 ];
        ULONG           ul_Dummy[ 2 ];

		float			f_CamWheelFactor;
		ULONG			ul_ColorStock[ 10 ];
		int				i_EditOptions_SymetrieOp;
		ULONG			ul_Unused[ 9 ];

        //ULONG			ul_MRMLevelNb;
        //float			f_MRMLevels[ 5 ];
		BOOL			DispLOD;

		float			f_SnapGridSizeZ;
#ifdef JADEFUSION
		ULONG           ul_HighlightColors[5];
#endif

	} mst_Ini;

	BIG_INDEX	mul_CurrentWorld;
	BIG_INDEX	mul_LoadedWorld;
	BOOL		mb_CanDestroy;
	BOOL		mb_LockUpdate;
    BOOL        mb_HideUnselected;
    ULONG       mul_SaveDMBeforeWired;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    void    SaveWorld(ULONG _ul_Action);
	void	CloseWorld(void);
	void	ChangeWorld(BIG_INDEX, BOOL _b_CanSend = TRUE);
	void	SetWidth(int);
	void	SetHeight(int);

	void	SplitViewCloseToObject();
	void	UnSplitViewCloseToObject();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INTERFACE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	b_AcceptToCloseProject(void);
	char	*psz_OnActionGetBase(void)	{ return EOUT_asz_ActionBase; };
	void	OneTrameEnding(void);
	int		i_OnMessage(ULONG, ULONG, ULONG);
	void	OnAction(ULONG);
	UINT	ui_OnActionState(ULONG);
	UINT	ui_ActionFillDynamic(EDI_cl_ConfigList *, POSITION);
	BOOL	b_OnActionValidate(ULONG, BOOL);
	void	OnActionUI(ULONG, CString &, CString &);
	int		i_IsItVarAction(ULONG, EVAV_cl_View *);
	
    /* ini file */
    void	ReinitIni(void);
	void	LoadIni(void);
	void	TreatIni(void);
	void	SaveIni(void);
    void    IniUpdate_0to1( void );

	void OnActivate( void ) override;
	void OnDisactivate() override;

	BOOL	b_CanHaveMultipleFullScreenResolution(void)		{ return TRUE; };
	void	OnToolBarCommand(UINT);
	void	OnToolBarCommandUI(UINT, CCmdUI *);
	BOOL	b_KnowsKey(USHORT);
    void    OnRealIdle(void);

	void	OnGaoSelect(struct OBJ_tdst_GameObject_*pst_GO, BOOL b_Multi, CPoint point);

	void	CamPlan(void);
	void	CamIso(void);
	void	ComputeStrips(BOOL = TRUE);
	void	ComputeLODStrips(BOOL = TRUE);

#ifdef JADEFUSION
    void    PackOneDirectory(ULONG _ul_DirIndex, BOOL _b_IsRoot);
    void    PackBasicWorlds(void);
#endif

private:
    void    UpdateShowAiVectors();

	//BOOL	PreTranslateMessage( MSG * );
	

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnDestroy(void);
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnSetMode(void);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void    OnMoving( UINT, LPRECT );
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
