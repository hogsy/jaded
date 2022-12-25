/*$T PROframe.h GC!1.52 10/27/99 15:39:44 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "VAVview/VAVview.h"
#include "PROframe_act.h"
#include "AIinterp/Sources/AIstruct.h"
#include "LINKs/LINKstruct.h"


/*$4
 ***************************************************************************************************
	This classe implements a tree structure to store the informations about expanded items so
	that we can keep expanded the items when the user changes the selection and go back to the
	previous selection.
 ***************************************************************************************************
 */

class ExpandedItemNode
{
public:
	ExpandedItemNode()
	{
		mi_ChildrenNb = 0;
		mast_Children = NULL;
		msz_Name[0] = 0;
		mi_Indent = -1;
	}
	~ExpandedItemNode() {
		DestroyAllChildren();
	}

	// An item is identified by its name and its path from the root of the tree.
	char				msz_Name[EVAV_MaxLenName];
	// For double check
	int					mi_Indent;

	int					mi_ChildrenNb;
	ExpandedItemNode	*mast_Children;

	ExpandedItemNode	*pst_FindChildNamed(char *_sz_Name);
	ExpandedItemNode	*AddChild(char *_sz_Name, int _i_Indent);
	void				DestroyChild(char *_sz_Name);
	void				DestroyAllChildren();
	ExpandedItemNode	*pst_FindFatherOf(ExpandedItemNode *_pst_Child);
};

typedef ExpandedItemNode ExpandedItemTree;

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class EPRO_cl_Frame : public EDI_cl_BaseFrame
{
    DECLARE_DYNCREATE(EPRO_cl_Frame)


/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EPRO_cl_Frame (void);
    ~EPRO_cl_Frame(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    struct
    {
        int i_CxCol1;
        int i_CxCol2;
        int i_DspGroup;
        int i1, i2, i3, i4;
    } mst_Ini;

    EVAV_cl_View                                    *mpo_VarsView;
    EVAV_tdst_VarsViewStruct                        mst_VarsViewStruct;
    CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>   mo_ListItems;

	ExpandedItemTree								mst_ExpandedItemTree;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EVAV_cl_ViewItem    *pst_GetGroupByMouse(CPoint);

    POSITION            x_AddItemByCopy
                        (
                            EVAV_cl_ViewItem *,
                            POSITION _posref = NULL,
                            int _i_Head = 0,
                            BOOL _b_First = FALSE
                        );

    EVAV_cl_ViewItem    *AddItemByPointer
                        (
                            LINK_tdst_Pointer *,
                            POSITION posgrp = NULL,
                            BOOL _b_Forced = FALSE,
                            BOOL _b_DD = FALSE,
                            POSITION posafter = NULL
                        );
    EVAV_cl_ViewItem    *ReplaceItemByPointer
                        (
                            LINK_tdst_Pointer *,
                            LINK_tdst_Pointer *,
                            POSITION posgrp = NULL,
                            BOOL _b_CanAdd = TRUE,
                            POSITION *_px_RetPos = NULL
                        );
    POSITION            DelItemByPointer
                        (
                            void *,
                            POSITION posgrp = NULL,
                            POSITION *_px_RetPos = NULL,
                            BOOL _b_DelGrp = FALSE
                        );

    POSITION            DragDropAddContent(POSITION, EPRO_cl_Frame *, EVAV_cl_ViewItem *);
    POSITION            DragDropPasteContent
                        (
                            POSITION,
                            EPRO_cl_Frame *,
                            EVAV_cl_ViewItem *,
                            EVAV_cl_ViewItem *
                        );
    void                DragDropContent(CPoint, EPRO_cl_Frame *, EVAV_cl_ViewItem *);

    void                Refresh(void);

	void				SynchronizeExpandTree(void);
	void				SynchronizeItemList(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    INTERFACE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    int     i_OnMessage(ULONG, ULONG, ULONG);
    void    ReinitIni(void);
    void    LoadIni(void);
    void    TreatIni(void);
    void    SaveIni(void);
    void    OneTrameEnding(void);
    char    *psz_OnActionGetBase(void) { return EPRO_asz_ActionBase; };
    void    OnAction(ULONG);
    BOOL    b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
    BOOL    b_KnowsKey(USHORT);
    void    AfterEngine(void);
    void    OnToolBarCommand(UINT);
    void    OnToolBarCommandUI(UINT, CCmdUI *);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg int     OnCreate(LPCREATESTRUCT);
    afx_msg void    OnSize(UINT, int, int);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
