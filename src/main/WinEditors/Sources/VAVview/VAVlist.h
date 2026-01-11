/*$T VAVlist.h GC! 1.075 03/08/00 12:25:30 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/BAStypes.h"

#include "BIGfiles/BIGdefs.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AImsg.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EVAV_CEdit : public CEdit
{
public:
    EVAV_CEdit (void)   { b_Inc = FALSE; }
    int             i_Type;
    BOOL            b_Inc;
    CPoint          o_Pt;
    afx_msg void    OnKeyDown(UINT, UINT, UINT);
    afx_msg void    OnChar(UINT, UINT, UINT);
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnRButtonUp(UINT, CPoint);
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    afx_msg void    OnMButtonDown(UINT, CPoint);
    afx_msg void    OnMButtonUp(UINT, CPoint);
    afx_msg BOOL    OnMouseWheel(UINT, short, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
    DECLARE_MESSAGE_MAP()
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Differents data types that can be edit in a var view.
 -----------------------------------------------------------------------------------------------------------------------
 */

#define EVAV_DEFINE_TYPE(a, b, c, d)    a,
typedef enum EVAV_tde_VarsViewItemType_
{
#include "VAVtypes.h"
}
EVAV_tde_VarsViewItemType;

#undef EVAV_DEFINE_TYPE

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Differents data types that can be edit in a var view.
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef enum    EVAV_tde_VarsViewItemEditType_
{
    EVAV_EVVIET_None,
    EVAV_EVVIET_Combo,
    EVAV_EVVIET_Edit,
    EVAV_EVVIET_Button,
    EVAV_EVVIET_Check,
} EVAV_tde_VarsViewItemEditType;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Type for callbacks.
 -----------------------------------------------------------------------------------------------------------------------
 */

class           EVAV_cl_ViewItem;
typedef CString (*tdpfnv_DrawValue) (EVAV_cl_ViewItem *, void *);
typedef void (*tdpfnv_SelectValue) (CWnd *, EVAV_cl_ViewItem *, void *, BOOL);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    User callback
 -----------------------------------------------------------------------------------------------------------------------
 */

class   EVAV_cl_ViewItem;
typedef void (*EVAV_tdpfnv_Change) (void *, void *, void *, LONG);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    External functions for types.
 -----------------------------------------------------------------------------------------------------------------------
 */

/*$off*/
#define EVAV_DEFINE_TYPE(a, b, c, d)    extern CString c(EVAV_cl_ViewItem *, void *);
#include "VAVtypes.h"
#undef EVAV_DEFINE_TYPE

#define EVAV_DEFINE_TYPE(a, b, c, d)    extern void d(CWnd *, EVAV_cl_ViewItem *, void *, BOOL);
#include "VAVtypes.h"
#undef EVAV_DEFINE_TYPE
/*$on*/

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Constants
 -----------------------------------------------------------------------------------------------------------------------
 */

#define EVAV_MaxLenName     255
#define EVAV_None           0x00000000
#define EVAV_ReadOnly       0x00000001
#define EVAV_CanDelete      0x00000002
#define EVAV_Array          0x00000004
#define EVAV_AutoExpand     0x00000008
#define INDENT_STEP         18

#define EVAV_Filter_Init    0x00000001
#define EVAV_Filter_Cur     0x00000002
#define EVAV_Filter_Private 0x00000004
#define EVAV_Filter_Track   0x00000008
#define EVAV_Filter_Sep     0x00000010
#define EVAV_Filter_NoFil   0x80000000

/*$4
 ***********************************************************************************************************************
    Definition of an item.
 ***********************************************************************************************************************
 */

class   EVAV_cl_ViewItem
{
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EVAV_cl_ViewItem (void);
    EVAV_cl_ViewItem
    (
        char *,
        EVAV_tde_VarsViewItemType,
        void *,
        ULONG,
        int,
        int,
        int,
        EVAV_tdpfnv_Change = NULL,
        COLORREF = GetSysColor(COLOR_WINDOW),
        int = 0,
        int = 0,
        int = 0,
        int = 0,
        int = 0,
		int = 0
    );

    ~EVAV_cl_ViewItem(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    void    CopyFrom(EVAV_cl_ViewItem *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    char                        masz_Name[EVAV_MaxLenName];
    char                        masz_Group[BIG_C_MaxLenPath];
    EVAV_tde_VarsViewItemType   me_Type;
    void                        *mp_Data;
    int                         mul_Offset;
    int                         mi_NumGroup;
    int                         mi_Display;
    int                         mi_DisplayGroup;
    ULONG                       mul_Flags;
    int                         mi_Param1;
    int                         mi_Param2;
    int                         mi_Param3;
    int                         mi_Param4;
    int                         mi_Param5;
    int                         mi_Param6;
    int                         mi_Param7;
    int                         mi_Param8;
    int                         mi_Indent;
    int                         mi_ID;
    int                         mi_SubID;
    int                         mi_NumFields;
    BOOL                        mb_Expand;
    BOOL                        mb_ExpandOnce;
    EVAV_tdpfnv_Change          mpfn_CB;
    COLORREF                    mx_Color;
    BOOL                        mb_Locked;
    CString                     o_LastValue;
    char                        *psz_Help;
	char						*psz_NameKit;

	int							size_array_father;
	int							size_elem_father;
	int							index_me_father;
	void						*p_data_father;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    void    SetFilterString(char *);
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>   tdListItems;
extern EVAV_cl_ViewItem                                 *EVAV_gpo_GlobalItem;
class EVAV_cl_ListBox : public CListBox
{
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EVAV_cl_ListBox (void);
    ~EVAV_cl_ListBox(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    CBrush                  mo_BrushActive;
    CBrush                  mo_BrushBack;
    CPen                    mo_PenGray;
    CPen                    mo_PenBlack;

    int                     mi_WidthCol1;
    CComboBox               mo_ComboSelect;
    EVAV_CEdit              mo_EditSelect;
    CButton                 mo_ButtonSelect;
    CButton                 mo_CheckSelect;
    CWnd                    *mpo_CurrentWnd;
    tdListItems             *mpo_ListItems;
    BOOL                    mb_CanDragDrop;
    int                     mi_DisplayGroup;

    void                    *mp_Owner;      /* Owner for callback below */
    EVAV_tdpfnv_Change      mpfnv_Callback; /* Callback when change */

    BOOL                    mab_AutoExpandStruct[1000];
    CList<CString, CString> mas_AutoExpandStruct[1000];

    CList<void *, void *>   mo_MulSel;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void				MoveArray(int, EVAV_cl_ViewItem *);
	EVAV_cl_ViewItem	*po_GetParentGAO(EVAV_cl_ViewItem *);
    EVAV_cl_ViewItem    *po_GetTypedParent(EVAV_cl_ViewItem *, int );
    void                EnableScroll(void);
    BOOL                b_DispGrp(EVAV_cl_ViewItem *);
    EVAV_cl_ViewItem    *po_IsVolatileSep(EVAV_cl_ViewItem *);
    EVAV_cl_ViewItem    *po_SearchSeparatorBefore(EVAV_cl_ViewItem *);
    void                DrawSeparator(DRAWITEMSTRUCT *, CDC *);
    void                SortList(void);
    void                SetItemList(CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *> *);
    EVAV_cl_ViewItem    *po_GetCurrentItem(void);
    void                OnDelete(void);
    void                OnSelect(void);
    POSITION            TreatArray(EVAV_cl_ViewItem *, POSITION);
    void                Copy(EVAV_cl_ViewItem *, EVAV_cl_ViewItem *);
    BOOL                DragALong(CPoint, void *, int, int, EVAV_cl_ViewItem * = NULL, BOOL = TRUE);
    EVAV_cl_ViewItem    *GetItemParent(EVAV_cl_ViewItem *);
    void                AIAddInstance(POSITION, AI_tdst_Instance *);
    void                COLAddInstance(POSITION, struct COL_tdst_Instance_ *);
    void                MsgAddList(POSITION, AI_tdst_MessageLiFo *);
    void                OBJAddBoundingVolume(POSITION, void *);
    void                OBJAddZone(POSITION, void *);
    void                OBJAddCob(POSITION, void *);
#ifdef ODE_INSIDE
    void                OBJAddODE(POSITION, void *);
#endif
    void                OBJAddGraphicObject(POSITION, void *);
    void                AddModifierAnimatedGAOProperties(POSITION &pos, EVAV_cl_ViewItem	*po_Item, int _AnimType, void *_pParams);
    void                OBJAddModifier(POSITION, void *);
    void                OBJAddBaseAdditionalMatrix(POSITION, void *);
    void                ANIAddGameObjectAnim(POSITION, void *);
    void                ANIAddAnim(POSITION, void *);
    void                ANIAddBlendAnim(POSITION, void *);
    void                ACTAddAction(POSITION, void *);
    void                ACTAddActionKit(POSITION, void *);
    void                TEXAddProcedural(POSITION, void *);
    void                TEXAddAnimated(POSITION, void *);
    void                TEXAddSpriteGen(POSITION, void *);
    void                SNDAddBank(POSITION pos, union SND_tdun_Main_ *_pst_Instance);
    void                SNDAddSound(POSITION pos, struct SND_tdst_OneSound_ *_pst_Instance);
    int                 OBJ_i_AddGraphicObject_Geometric(POSITION, EVAV_cl_ViewItem *, void *);
    int                 OBJ_i_AddGraphicObject_Light(POSITION, EVAV_cl_ViewItem *, void *);
    int                 OBJ_i_AddGraphicObject_MaterialSingle(POSITION, EVAV_cl_ViewItem *, void *);
    int                 OBJ_i_AddGraphicObject_MaterialMulti(POSITION, EVAV_cl_ViewItem *, void *);
    int                 OBJ_i_AddGraphicObject_Camera(POSITION, EVAV_cl_ViewItem *, void *);
    int                 OBJ_i_AddGraphicObject_StaticLOD(POSITION, EVAV_cl_ViewItem *, void *);
    int                 OBJ_i_AddGraphicObject_Text(POSITION, EVAV_cl_ViewItem *, void *);
    int                 OBJ_i_AddGraphicObject_ParticleGenerator(POSITION, EVAV_cl_ViewItem *, void *);

#ifdef JADEFUSION
    void                OBJAddSoftBodyListRods(POSITION pos, void *_pst_Data);
    void                OBJAddSoftBodyListVertices(POSITION pos, void *_pst_Data);
    void                OBJAddSoftBodyListPlanes(POSITION pos, void *_pst_Data);
#endif
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    virtual void    DrawItem(LPDRAWITEMSTRUCT);
    virtual void    MeasureItem(LPMEASUREITEMSTRUCT);
    void            DeleteString(int);
    void            BeforeRefresh(void);
    void            AddMano(CPoint);
    void            MulSel(EVAV_cl_ViewItem *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnSize(UINT, int, int);
    afx_msg void    OnComboSelect(void);
    afx_msg void    OnComboChangeSelect(void);
    afx_msg void    OnEditSelect(void);
    afx_msg void    OnEditChangeSelect(void);
    afx_msg void    OnButtonChangeSelect(void);
    afx_msg void    OnCheckSelect(void);
    afx_msg void    OnCheckChangeSelect(void);
    afx_msg void    OnExpand(UINT);
    afx_msg BOOL    OnEraseBkgnd(CDC *);
    afx_msg void    OnKeyDown(UINT, UINT, UINT);
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    afx_msg void    OnLButtonDblClk(UINT, CPoint);
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnRButtonUp(UINT, CPoint);
    afx_msg void    OnMButtonDown(UINT, CPoint);
    afx_msg void    OnMButtonUp(UINT, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
    afx_msg void    OnKillFocus(CWnd *);
    afx_msg void    OnVScroll(UINT, UINT, CScrollBar *);
    afx_msg BOOL    OnMouseWheel(UINT, short, CPoint);
	afx_msg void	OnChar(UINT, UINT, UINT);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */

