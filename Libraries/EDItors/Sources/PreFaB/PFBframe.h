/*$T PFBframe.h GC! 1.081 01/29/04 17:12:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "EDIbaseframe.h"
#include "PFBframe_act.h"
#include "PFBdata.h"

/*$4
 ***********************************************************************************************************************
	CONSTANTS 
 ***********************************************************************************************************************
 */
#define EPFB_SelType_Nothing		0
#define EPFB_SelType_TreeDir		1
#define EPFB_SelType_TreePrefab		2
#define EPFB_SelType_RefGao			3
#define EPFB_SelType_RefPrefab		4


/*$4
 ***********************************************************************************************************************
	EPFB_cl_Frame class : base class of PreFaB editor
 ***********************************************************************************************************************
 */

class	EPFB_cl_View;
class	EPFB_cl_TreeView;

class EPFB_cl_Frame : public EDI_cl_BaseFrame
{
	DECLARE_DYNCREATE(EPFB_cl_Frame)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EPFB_cl_Frame(void);
	~EPFB_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	CSplitterWnd		*mpo_Splitter;
	EPFB_cl_TreeView	*mpo_TreeView;
	EPFB_cl_View		*mpo_DataView;

	struct
	{
		int			i_Version;
		int			i_Pane0Width;
		BOOL		b_SelIsDir;
		BIG_INDEX	ul_DirIndex;
		BIG_INDEX	ul_FileKey;
		int			i_SaveAuto;
		char		sz_DirName[ BIG_C_MaxLenPath ];
		int			i_Pane0Height;
		int			i_VerticalLayout;
		int			i_DisplayPoint;
		int			i_DisplayBV;
		int			i_MoveToPrefabMap;
		BIG_INDEX	ul_PrefabMapIndex;
	}
	mst_Ini;
	
	/* selection state */
	int				mi_SelType;

	/* Tree data */
	BOOL			mb_SelIsDir;
	BIG_INDEX		mul_DirIndex;
	BIG_INDEX		mul_FileIndex;

	/* prefab data */
	OBJ_tdst_Prefab *mpst_Prefab;
	int				mi_PrefabModif;
	int 			mi_PrefabSaveAuto;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	Tree_SelChange(void);

	void	AddPrefab(struct WOR_tdst_World_ *, struct SEL_tdst_Selection_ *, BOOL );
	void	MergePrefab(struct WOR_tdst_World_ *, struct SEL_tdst_Selection_ *);
	void	PrefabSel(class F3D_cl_View *, struct WOR_tdst_World_ *, struct SEL_tdst_Selection_ *);
	void	PrefabUpdate(struct WOR_tdst_World_ *, struct SEL_tdst_Selection_ *, CRect *);
	void	Prefab_MoveToMap( struct WOR_tdst_World_ *, OBJ_tdst_Prefab * );
	void	Prefab_MoveToMapExt( struct WOR_tdst_World_ *, OBJ_tdst_Prefab *, BOOL, ULONG, ULONG );
	void	Prefab_MoveToMap_MoveObject( OBJ_tdst_GameObject *, char *, char *);
	void	Prefab_MoveToMap_MoveFile( ULONG, char *, char *);
	
	int		UpdateSelType( void );
	void	CommonVss( int );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INTERFACE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	b_CanBeLinkedToEngine(void) { return FALSE; };
	void	CloseProject(void);
	BOOL	b_CanActivate(void);
	char	*psz_OnActionGetBase(void)	{ return EPFB_asz_ActionBase; };
	void	Refresh(void);
	void	OnAction(ULONG);
	BOOL	b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
	BOOL    b_KnowsKey(USHORT);
	
	int		i_OnMessage(ULONG, ULONG, ULONG);
	int		i_CanDragDrop_User( struct EDI_tdst_DragDrop_ * );
	void	EndDragDrop_User( struct EDI_tdst_DragDrop_ * );
	
	UINT	ui_OnActionState(ULONG);
	void	ReinitIni(void);
	void	LoadIni(void);
	void	TreatIni(void);
	void	SaveIni(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	PreTranslateMessage( MSG * );
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnClose(void);

	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
