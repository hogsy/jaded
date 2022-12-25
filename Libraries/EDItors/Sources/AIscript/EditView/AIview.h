/*$T AIview.h GC!1.52 11/16/99 12:27:27 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EDIundo.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"

typedef enum    tde_ChangeType_
{
    ctUndo,
    ctUnknown,
    ctReplSel,
    ctDelete,
    ctBack,
    ctCut,
    ctPaste,
    ctMove
} tde_ChangeType;

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class           EAI_cl_Frame;

class EAI_cl_View : public CRichEditCtrl
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EAI_cl_View (void);
    ~EAI_cl_View(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EAI_cl_Frame        *mpo_Frame;
    CFont               *mpo_Fnt;
    BOOL                mb_CharTreat;
    int                 mi_FirstColLine;
    int                 mi_LastColLine;
    BOOL                mb_FirstColoring;
    tde_ChangeType      me_ChangeType;
    CHARRANGE           mst_OldSel;
    EDI_cl_UndoManager  mo_UndoManager;
    int                 mi_LockCount;

    char                *mpsz_LastNameVar;
    int                 *mpi_LastBufVar;
    int                 mi_LastTypeVar;

	CString				mo_DisplayVar;
	int					mi_DisplayLine;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void        Init(void);

	void		SetDisplayVar(CPoint);
    void        DisplayVar(char *, int *, int, BOOL = FALSE, char * = NULL);
	void		DisplayVar(CString, BOOL = FALSE, int = 0);

    void        ResetDefaultFormat(BOOL _b_UpdateCol = TRUE);
    COLORREF    x_GetColorWord(char *);
    void        ColorCppComment(char **, LONG &, LONG &, LONG &);
    void        ColorText(LONG, LONG, COLORREF);
    void        ColorRange(LONG, LONG);
    void        ColorSel(LONG, LONG);

    void        LockWindowUpdate(void);
    void        UnlockWindowUpdate(void);

    LONG        l_FirstVisibleLine(void);
    LONG        l_LastVisibleLine(void);
    CString     o_GetWordLeft(LONG);
    CString     o_GetWordRight(LONG);

    void        AddString(CString &, BOOL _b_Locked = FALSE);
    void        DeleteSel(BOOL _b_Locked = FALSE);
    void        CopyIntoClipBoard(CString &);
    CString     o_GetStringFromClipBoard(void);
    void        DelBlockString(CString &);
    void        AddBlockString(CString &);
	int			FindText(DWORD, FINDTEXTEX *);
	void		GetWindowText(CString &);

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg int     OnCreate(LPCREATESTRUCT);
    afx_msg void    OnLButtonDblClk(UINT, CPoint);
    afx_msg void    OnMButtonDown(UINT, CPoint);
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnChar(UINT, UINT, UINT);
    afx_msg void    OnKeyDown(UINT, UINT, UINT);
    afx_msg void    OnKeyUp(UINT, UINT, UINT);
    afx_msg void    OnProtected(NMHDR *, LRESULT *);
    afx_msg void    OnChange(void);
    afx_msg void    OnPaint(void);
    afx_msg void    OnCompletion(int);
    afx_msg BOOL    OnMouseWheel(UINT, short, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
