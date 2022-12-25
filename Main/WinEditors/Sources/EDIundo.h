/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"

class EDIA_cl_UndoDialog;

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EDI_cl_UndoModif : public CObject
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_UndoModif(BOOL _b_Locked = FALSE);
	~EDI_cl_UndoModif(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES
 ---------------------------------------------------------------------------------------------------
 */

public:
    ULONG           mul_Serial;
    static ULONG    mul_LastSerial;
    BOOL            mb_HasBeenDone;
    char            *msz_Desc;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS
 ---------------------------------------------------------------------------------------------------
 */

public:
    virtual BOOL b_Do(void) { return TRUE; };
    virtual BOOL b_Undo(void) { return TRUE; };

    void    Done(void) { mb_HasBeenDone = TRUE; };
    void    Undone(void) { mb_HasBeenDone = FALSE; };

    void    SetDesc( char * );
};

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EDI_cl_UndoManager : public CObject
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_UndoManager(int _i_MaxUndo = 50);
    ~EDI_cl_UndoManager(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES
 ---------------------------------------------------------------------------------------------------
 */

public:
    POSITION                                        mx_PrevPosition;
    POSITION                                        mx_NextPosition;
    EDI_cl_UndoModif                                *mpo_PrevModif;
    EDI_cl_UndoModif                                *mpo_NextModif;
    int                                             mi_CurrentIndex;
    int                                             mi_MarkedIndex;
    int                                             mi_MaxUndo;
    BOOL                                            mb_IsReDoing;
    BOOL                                            mb_IsUnDoing;

    CList<EDI_cl_UndoModif *, EDI_cl_UndoModif *>   mo_ListOfModifs;

    EDIA_cl_UndoDialog                              *mpo_UndoDlg;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS
 ---------------------------------------------------------------------------------------------------
 */

public:
    BOOL    b_AskFor(EDI_cl_UndoModif *, BOOL _b_MustDo = TRUE);
    BOOL    b_Undo(void);
    BOOL    b_Redo(void);
	void	CopyFrom(EDI_cl_UndoManager *);
    void    Clean(void);
    
    void    DisplayUndoDlg( BOOL );
    void    UpdateUndoDlg( void );
};

#endif /* ACTIVE_EDITORS */
