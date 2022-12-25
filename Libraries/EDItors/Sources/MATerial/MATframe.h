/*$T MATframe.h GC! 1.100 08/24/01 16:31:23 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EDIbaseframe.h"
#include "MuTex.h"
#include "GraphicDK/Sources/MATerial/MATsingle.h"
#ifdef JADEFUSION
#include "DIAlogs/DIAfindfile_dlg.h"
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef struct MAT_tdst_Material_	MAT_tdst_Material;
typedef struct						MAT_tdst_UndoStruct_
{
	MUTEX_Material		Dest;
	MAT_tdst_Material	*pst_GRO;
	MAT_tdst_Material	*pst_ACTIVE_GRO;
	LONG				ActiveSubMaterial;
} MAT_tdst_UndoStruct;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define MAT_C_MAX_UNDO	100
#ifdef JADEFUSION
#define MAT_C_PS2_START_DIR			EDI_Csz_Path_Textures
#define MAT_C_XENON_START_DIR		EDI_Csz_Path_Textures "/_Xenon"
#define MAT_C_XENON_NMAP_SUFFIXE	"_XE_N"
#endif

class EMAT_cl_Frame : public EDI_cl_BaseFrame
{
	DECLARE_DYNCREATE(EMAT_cl_Frame)
	MAT_tdst_UndoStruct				ast_UndoDrome[MAT_C_MAX_UNDO];
	ULONG							ul_DoPos;
	ULONG							ul_ReDoNumber;
	ULONG							ul_IsSaved;

	MAT_tdst_UndoStruct				st_CDO;
	HWND							stw_MultiSM;
	HWND							stw_ClipBoard;
	MUTEX_Material					st_ClipBoardMat;
	ULONG							ulMSMWidth;
	ULONG							ulCurrentMSMWidth;
	ULONG							CurrentCX, CurrentCY;
	ULONG							ShowMLTSBMT;
	struct MAT_tdst_MultiTexture_	*pst_Multi_Sample;

	CString							CurrentTextureDir;

	struct
	{
		BOOL	bAutoSave;
		ULONG	Undefined[7];
	} mst_Init;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EMAT_cl_Frame(void);
	~EMAT_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
#ifdef JADEFUSION
	// -----------------------------------------------
	// -NOTE- These functions are also used by the frame SELection to convert GRMs
	// PS2
	static void	GRM_To_MUTEX(MAT_tdst_UndoStruct* pst_CDO, MAT_tdst_MultiTexture* pst_Multi_Sample);
	static void	MUTEX_To_GRM(MAT_tdst_UndoStruct* pst_CDO);
	
	// Xenon (SC)
	static void	FetchTextureName(CHAR* _sz_TexName, LONG _l_TexId, bool _b_CubeMap);
    static void	ConvertXeLevelToMUTEX(MUTEX_XenonTextureLine* _pst_Mtx, const MAT_tdst_XeMTLevel* _pst_XeLevel);
    static void	ConvertMUTEXToXeLevel(MAT_tdst_XeMTLevel* _pst_XeLevel, const MUTEX_XenonTextureLine* _pst_Mtx);
    void	BrowseForTexture(LONG* _pul_TexId, CHAR* _psz_TexName, CHAR* _psz_RefTexName, bool _b_CubeMap, LONG _l_RefTex_id = BIG_C_InvalidIndex);
	// -----------------------------------------------
#else
	void				GRM_To_MUTEX(void);
	void				MUTEX_To_GRM(void);
#endif
	void				OnSave(void);
	void				OnSetMaterial(MAT_tdst_Material *pst_GRM, ULONG SubMaterialNum);
	void				OnUndo(void);
	void				OnRedo(void);
	void				OnCopyAll(void);
	void				OnPasteAll(void);
	void				OnDo(void);
	void				ReinitIni(void);
	void				LoadIni(void);
	void				TreatIni(void);
	void				SaveIni(void);

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	int					i_OnMessage(ULONG, ULONG, ULONG);
	void				OnAction(ULONG);
	char				*psz_OnActionGetBase(void);
	UINT				ui_OnActionState(ULONG);
	UINT				RefreshStruct(BOOL);
	BOOL				b_KnowsKey(USHORT _uw_Key);
	UINT				ui_ActionFillDynamic(EDI_cl_ConfigList *_po_List, POSITION _pos);
	BOOL				b_OnActionValidate(ULONG, BOOL);
	BOOL				b_AcceptToCloseProject(void);
	int					i_IsItVarAction(ULONG _ul_Action, EVAV_cl_View *_po_List);

	void				OnNewMulti(void);
	void				OnNewSingle(void);
	GRO_tdst_Struct		*CreateSingleMaterial(void);

	BOOL				PreTranslateMessage(MSG *);

	void				List_OnRButtonDown(void);
	void				MatName_OnRButtonDown( void );
	GRO_tdst_Struct		*pst_OpenSingleMat(void);
	
	void				GenerateHtml( void );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
