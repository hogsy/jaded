/*$T EDIundo.cpp GC!1.40 09/02/99 11:16:34 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EDIundo.h"
#include "DIAlogs/DIAundo_dlg.h"

ULONG EDI_cl_UndoModif::    mul_LastSerial = 0;


/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_UndoModif::EDI_cl_UndoModif(BOOL _b_Locked)
{
    mb_HasBeenDone = FALSE;
    if(!_b_Locked) mul_LastSerial++;
    mul_Serial = mul_LastSerial;
    msz_Desc = NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_UndoModif::~EDI_cl_UndoModif()
{
    if (msz_Desc)
        L_free( msz_Desc );
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_UndoModif::SetDesc( char *sz_Desc )
{
    if (msz_Desc) L_free( msz_Desc );

    if (!sz_Desc)
    {
        msz_Desc = NULL;
        return;
    }

    msz_Desc = (char *) L_malloc( strlen( sz_Desc ) + 1 );
    L_strcpy( msz_Desc, sz_Desc );
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_UndoManager::EDI_cl_UndoManager(int _i_MaxUndo)
{
    mx_PrevPosition = NULL;
    mx_NextPosition = NULL;
    mpo_PrevModif = NULL;
    mpo_NextModif = NULL;
    mi_MaxUndo = _i_MaxUndo;
    mi_CurrentIndex = 0;
    mi_MarkedIndex = 0;
    mb_IsReDoing = FALSE;
    mb_IsUnDoing = FALSE;
    mpo_UndoDlg = NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_UndoManager::~EDI_cl_UndoManager(void)
{
    Clean();
    if (mpo_UndoDlg) delete mpo_UndoDlg;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDI_cl_UndoManager::b_AskFor(EDI_cl_UndoModif *_po_Modif, BOOL _b_MustDo)
{
    /* Cannot insert a new modification while we are undoing or redoing another one */
    if(mb_IsReDoing || mb_IsUnDoing) return FALSE;

    /* First do the modif */
    if(_b_MustDo && !_po_Modif->b_Do()) return FALSE;
    _po_Modif->Done();

    /* Delete all the folowing modifs in the list */
    while(mi_CurrentIndex < mo_ListOfModifs.GetCount())
        delete mo_ListOfModifs.RemoveTail();

    /* Be careful of the max undo limit (delete head until under limit) */
    if(mi_MaxUndo)
    {
        while(mo_ListOfModifs.GetCount() >= mi_MaxUndo)
        {
            delete mo_ListOfModifs.RemoveHead();
            mi_CurrentIndex--;
            mi_MarkedIndex--;
            if (mi_MarkedIndex < 0) mi_MarkedIndex = 0;
        }
    }

    /* Insert the new modif at the end of the list */
    mx_PrevPosition = mo_ListOfModifs.AddTail(_po_Modif);
    mx_NextPosition = NULL;
    mpo_PrevModif = _po_Modif;
    mpo_NextModif = NULL;
    mi_CurrentIndex++;

    /* display undo */
    UpdateUndoDlg();

    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDI_cl_UndoManager::b_Undo(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul_Serial;
    BOOL    b_Res, b_Ret;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!mi_CurrentIndex) return FALSE;

    mb_IsUnDoing = TRUE;
    b_Res = TRUE;
    ul_Serial = mpo_PrevModif->mul_Serial;

    /* Undo all modifs with the same serial */
    while(mpo_PrevModif && mpo_PrevModif->mul_Serial == ul_Serial)
    {
        b_Ret = mpo_PrevModif->b_Undo();
        if(b_Ret) mpo_PrevModif->Undone();
        b_Res = b_Res && b_Ret;

        mpo_NextModif = mpo_PrevModif;
        mx_NextPosition = mx_PrevPosition;

        mpo_PrevModif = NULL;
        if(mx_PrevPosition)
        {
            mo_ListOfModifs.GetPrev(mx_PrevPosition);
            if(mx_PrevPosition)
                mpo_PrevModif = mo_ListOfModifs.GetAt(mx_PrevPosition);
        }

        mi_CurrentIndex--;
    }

    mb_IsUnDoing = FALSE;

    /* display undo */
    UpdateUndoDlg();

    return b_Res;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDI_cl_UndoManager::b_Redo(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul_Serial;
    BOOL    b_Res, b_Ret;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* No more redo to do */
    if(mi_CurrentIndex >= mo_ListOfModifs.GetCount()) return FALSE;

    mb_IsReDoing = TRUE;
    b_Res = TRUE;
    ul_Serial = mpo_NextModif->mul_Serial;

    /* Redo all the modifs with the same serial */
    while(mpo_NextModif && mpo_NextModif->mul_Serial == ul_Serial)
    {
        b_Ret = mpo_NextModif->b_Do();
        if(b_Ret) mpo_NextModif->Done();
        b_Res = b_Res && b_Ret;

        mpo_PrevModif = mpo_NextModif;
        mx_PrevPosition = mx_NextPosition;

        mpo_NextModif = NULL;
        if(mx_NextPosition)
        {
            mo_ListOfModifs.GetNext(mx_NextPosition);
            if(mx_NextPosition)
                mpo_NextModif = mo_ListOfModifs.GetAt(mx_NextPosition);
        }

        mi_CurrentIndex++;
    }

    mb_IsReDoing = FALSE;

    /* display undo */
    UpdateUndoDlg();

    return b_Res;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_UndoManager::CopyFrom(EDI_cl_UndoManager *_po_Src)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            pos, posd;
    EDI_cl_UndoModif    *po_Modif, *po_NewModif;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Delete current list */
    Clean();

    /* Scan reference list, and copy */
    pos = _po_Src->mo_ListOfModifs.GetHeadPosition();
    while(pos)
    {
        /* Make a copy of the modif */
        po_Modif = _po_Src->mo_ListOfModifs.GetAt(pos);
        po_NewModif = new EDI_cl_UndoModif;
        po_NewModif->mul_Serial = po_Modif->mul_Serial;
        po_NewModif->mul_LastSerial = po_Modif->mul_LastSerial;
        po_NewModif->mb_HasBeenDone = po_Modif->mb_HasBeenDone;

        /* Insert modif */
        posd = mo_ListOfModifs.AddTail(po_NewModif);

        if(pos == _po_Src->mx_PrevPosition) mx_PrevPosition = posd;
        if(pos == _po_Src->mx_NextPosition) mx_NextPosition = posd;
        if(po_Modif == _po_Src->mpo_PrevModif)
            mpo_PrevModif = po_NewModif;
        if(po_Modif == _po_Src->mpo_NextModif)
            mpo_NextModif = po_NewModif;

        _po_Src->mo_ListOfModifs.GetNext(pos);
    }

    /* Copy fields of manager */
    mi_MaxUndo = _po_Src->mi_MaxUndo;
    mi_CurrentIndex = _po_Src->mi_CurrentIndex;
    mb_IsReDoing = _po_Src->mb_IsReDoing;
    mb_IsUnDoing = _po_Src->mb_IsUnDoing;

    /* display undo */
    UpdateUndoDlg();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_UndoManager::Clean(void)
{
    while(mo_ListOfModifs.GetCount())
	{
        delete mo_ListOfModifs.RemoveTail();
	}
    mx_PrevPosition = NULL;
    mx_NextPosition = NULL;
    mpo_PrevModif = NULL;
    mpo_NextModif = NULL;
    mi_CurrentIndex = 0;
    mi_MarkedIndex = 0;
    mb_IsReDoing = FALSE;
    mb_IsUnDoing = FALSE;

    /* display undo */
    UpdateUndoDlg();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_UndoManager::DisplayUndoDlg( BOOL _b_Show )
{
    if (_b_Show)
    { 
        if (mpo_UndoDlg) return;
        mpo_UndoDlg = new EDIA_cl_UndoDialog( this );
        mpo_UndoDlg->DoModeless();
        UpdateUndoDlg();
    }
    else
    {
        if ( !mpo_UndoDlg ) return;
        mpo_UndoDlg->CloseWindow();
        mpo_UndoDlg = NULL;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_UndoManager::UpdateUndoDlg( void )
{
    if (!mpo_UndoDlg) return;
    mpo_UndoDlg->UpdateUndoList();
}


#endif /* ACTIVE_EDITORS */
