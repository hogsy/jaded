/*$T VAVview.h GC!1.52 10/07/99 17:10:56 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "VAVview/VAVlist.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ---------------------------------------------------------------------------------------------------
    To define columns (for creation of view).
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  EVAV_tdst_VarsViewStruct_
{
    char                                            *psz_NameCol1;
    char                                            *psz_NameCol2;
    int                                             i_WidthCol1;
    CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>   *po_ListItems;
} EVAV_tdst_VarsViewStruct;

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class EVAV_cl_View : public CWnd
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EVAV_cl_View (void);
    ~EVAV_cl_View(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EVAV_cl_ListBox             *mpo_ListBox;
    class CFlatHeaderCtrl		*mpo_HeaderCtrl;
    EVAV_tdst_VarsViewStruct    *mpst_Define;
    BOOL                        mb_CanDragDrop;
    ULONG                       mul_ID; /* A user ID associated with the view */

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void                        MyCreate
                                (
                                    CWnd *,
                                    EVAV_tdst_VarsViewStruct *,
                                    void *_p_Owner = NULL,
                                    int _i_ID = 0
                                );
    void                        SetColWidth(int, int);
    void                        GetColWidth(int &, int &);
    void                        SetChangeCallback(EVAV_tdpfnv_Change);
    void                        UnExpandAll(void);

    void                        SetItemList(CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *> *);
    void                        ResetList(void);
    EVAV_cl_ViewItem            *AddItem
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
                                    int _i_Param7 = 0,
                                    void  (*_pf_CB)(void*, void*, void*, long) = NULL
                                );

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnSize(UINT, int, int);
    afx_msg void    OnHeaderNotify(NMHDR *, LRESULT *);
    afx_msg void    OnSelChangeList(void);
    afx_msg BOOL    OnEraseBkgnd(CDC *pDC);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
