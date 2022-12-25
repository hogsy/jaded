/*$T DIAchecklist_dlg.h GC!1.52 11/12/99 11:30:53 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "VAVview/VAVview.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EDI_cl_BaseFrame;
class EDIA_cl_CheckListDialog : public EDIA_cl_BaseDialog
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_CheckListDialog
    (
        char                *_psz_Title,
        CPoint              _o_Pos,
        EDI_cl_BaseFrame    *_po_Owner = NULL,
        BOOL                _b_CanFree = TRUE
    );
    ~EDIA_cl_CheckListDialog(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    BOOL                                            mb_CanFree;
    char                                            *mpsz_Title;
    CPoint                                          mo_InitPos;
    EVAV_cl_View                                    *mpo_VarsView;
    EVAV_tdst_VarsViewStruct                        mst_VarsViewStruct;
    CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>   mo_ListItems;
    CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>   *mpl;
    EDI_cl_BaseFrame                                *mpo_Owner;
    CRect                                           o_BeforeMin;
    BOOL                                            mb_Minimize;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EVAV_cl_ViewItem    *AddItem
                        (
                            char *,
                            EVAV_tde_VarsViewItemType,
                            void *,
                            ULONG _ul_Flags = EVAV_None,
                            int _i_Param1 = 0,
                            int _i_Param2 = 0,
                            int _i_Param3 = 0,
                            POSITION pos = NULL,
                            int _i_Param4 = 0,
                            int _i_Param5 = 0,
                            int _i_Param6 = 0,
                            int _i_Param7 = 0
                        );

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>   *GetList(void);
    BOOL                                            OnInitDialog(void);
    void                                            OnCancel(void);
    void                                            OnOK(void);
    int                                             i_MaxSize(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

protected:
    afx_msg void    OnGetMinMaxInfo(MINMAXINFO FAR *);
    afx_msg void    OnSize(UINT, int, int);
    afx_msg BOOL    OnEraseBkgnd(CDC *);
    afx_msg void    OnNcLButtonDblClk(UINT, CPoint);
    DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
