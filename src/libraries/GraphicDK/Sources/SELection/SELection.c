/*$T SELection.c GC! 1.077 03/15/00 10:11:53 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLImem.h"
#include "SELection/SELection.h"
#include "Engine/sources/WORld/WORstruct.h"

#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Init selection data
 =======================================================================================================================
 */
void SEL_Init(SEL_tdst_Selection *_pst_Sel)
{
	_pst_Sel->l_Flag = 0;
	_pst_Sel->pst_FirstItem = NULL;
	_pst_Sel->pst_LastItem = NULL;
}

/*
 =======================================================================================================================
    Aim:    Close selection data
 =======================================================================================================================
 */
void SEL_Close(SEL_tdst_Selection *_pst_Sel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Sel->l_Flag = 0;

	pst_Next = _pst_Sel->pst_FirstItem;
	while(pst_Next != NULL)
	{
		_pst_Sel->pst_FirstItem = pst_Next->pst_Next;
		L_free(pst_Next);
		pst_Next = _pst_Sel->pst_FirstItem;
	}

	_pst_Sel->pst_LastItem = NULL;
}

/*
 =======================================================================================================================
    Aim:    Return true if there's no selected item
 =======================================================================================================================
 */
BOOL SEL_b_IsEmpty(SEL_tdst_Selection *_pst_Sel)
{
	BOOL empty = FALSE;

	if (!_pst_Sel || !_pst_Sel->pst_FirstItem)
		empty = TRUE;

	return(empty);
}

/*
 =======================================================================================================================
    Aim:    Return number of selected item with given flags
 =======================================================================================================================
 */
LONG SEL_l_CountItem(SEL_tdst_Selection *_pst_Sel, LONG _l_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item;
	LONG					l_Number;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (!_pst_Sel)
        return 0;

	pst_Item = _pst_Sel->pst_FirstItem;
	l_Number = 0;

	while(pst_Item != NULL)
	{
		if(pst_Item->l_Flag & _l_Flag) l_Number++;
		pst_Item = pst_Item->pst_Next;
	}

	return l_Number;
}

/*
 =======================================================================================================================
    Aim:    Add an item in selection list
 =======================================================================================================================
 */
SEL_tdst_SelectedItem *SEL_pst_AddItem(SEL_tdst_Selection *_pst_Sel, void *_p_Content, LONG _l_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Item = (SEL_tdst_SelectedItem *) L_malloc(sizeof(SEL_tdst_SelectedItem));
	pst_Item->p_Content = _p_Content;
	pst_Item->l_Flag = _l_Flag;
	pst_Item->ul_User = 0;
	pst_Item->pst_Next = NULL;

	if(_pst_Sel->pst_FirstItem)
		_pst_Sel->pst_LastItem->pst_Next = pst_Item;
	else
		_pst_Sel->pst_FirstItem = pst_Item;

	_pst_Sel->pst_LastItem = pst_Item;

	return pst_Item;
}

/*
 =======================================================================================================================
    Aim:    Delete an item
 =======================================================================================================================
 */
void SEL_DelItem(SEL_tdst_Selection *_pst_Sel, void *_p_Content)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item, *pst_Previous;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Previous = NULL;
	pst_Item = _pst_Sel->pst_FirstItem;

	while(pst_Item != NULL)
	{
		if(pst_Item->p_Content == _p_Content)
		{
			if(pst_Previous)
				pst_Previous->pst_Next = pst_Item->pst_Next;
			else
				_pst_Sel->pst_FirstItem = pst_Item->pst_Next;
			if(pst_Item == _pst_Sel->pst_LastItem) _pst_Sel->pst_LastItem = pst_Previous;

			L_free(pst_Item);
			return;
		}

		pst_Previous = pst_Item;
		pst_Item = pst_Item->pst_Next;
	}
}

/*
 =======================================================================================================================
    Aim:    Delete all items that match given content and flags
 =======================================================================================================================
 */
void SEL_DelItems(SEL_tdst_Selection *_pst_Sel, void *_p_Content, LONG _l_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item, *pst_Previous, *pst_Next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Previous = NULL;
	pst_Item = _pst_Sel->pst_FirstItem;

	while(pst_Item != NULL)
	{
		pst_Next = pst_Item->pst_Next;

		if((pst_Item->l_Flag & _l_Flag) && (pst_Item->p_Content == _p_Content))
		{
			if(pst_Previous)
				pst_Previous->pst_Next = pst_Item->pst_Next;
			else
				_pst_Sel->pst_FirstItem = pst_Item->pst_Next;
			if(pst_Item == _pst_Sel->pst_LastItem) _pst_Sel->pst_LastItem = pst_Previous;

			L_free(pst_Item);
		}
		else
			pst_Previous = pst_Item;
		pst_Item = pst_Next;
	}
}

/*
 =======================================================================================================================
    Aim:    Retrieve an item in selected list
 =======================================================================================================================
 */
SEL_tdst_SelectedItem *SEL_RetrieveItem(SEL_tdst_Selection *_pst_Sel, void *_p_Content)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Item = _pst_Sel->pst_FirstItem;

	while(pst_Item != NULL)
	{
		if(pst_Item->p_Content == _p_Content)
		{
			return pst_Item;
		}

		pst_Item = pst_Item->pst_Next;
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Retrieve an item in selected list
 =======================================================================================================================
 */
SEL_tdst_SelectedItem *SEL_RetrieveItemWithType(SEL_tdst_Selection *_pst_Sel, void *_p_Content, LONG _l_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Item = _pst_Sel->pst_FirstItem;

	while(pst_Item != NULL)
	{
		if((pst_Item->p_Content == _p_Content) && (pst_Item->l_Flag & _l_Flag))
			return pst_Item;
		pst_Item = pst_Item->pst_Next;
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Replace selected item content
 =======================================================================================================================
 */
void SEL_ReplaceItemContent(SEL_tdst_Selection *_pst_Sel, void *_p_Old, void *_p_New)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_SelItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SelItem = SEL_RetrieveItem(_pst_Sel, _p_Old);
	if(_p_Old) pst_SelItem->p_Content = _p_New;
}

/*
 =======================================================================================================================
    Aim:    Enumerate all item that match flags and for each call the given callback function if callback function
            return TRUE enumeration continue else it stop
 =======================================================================================================================
 */
void SEL_EnumItem
(
	SEL_tdst_Selection	*_pst_Sel,
	LONG				_l_Flag,
	BOOL (*pfnb_Callback) (SEL_tdst_SelectedItem *, ULONG, ULONG),
	ULONG _ul_Param1,
	ULONG _ul_Param2
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Item = _pst_Sel->pst_FirstItem;
	while(pst_Item != NULL)
	{
	    SEL_tdst_SelectedItem *pst_NextItem = pst_Item->pst_Next;
		if(pst_Item->l_Flag & _l_Flag)
		{
			if(!pfnb_Callback(pst_Item, _ul_Param1, _ul_Param2)) return;
		}

		pst_Item = pst_NextItem;
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SEL_EnumWorldGao( WOR_tdst_World *_pst_World, BOOL (*pfnb_Callback) (SEL_tdst_SelectedItem *, ULONG, ULONG), ULONG _ul_Param1, ULONG _ul_Param2 )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	    st_Item;
    TAB_tdst_PFelem		        *pst_PFElem, *pst_PFLastElem;
    OBJ_tdst_GameObject         *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    st_Item.l_Flag = SEL_C_SIF_Object; 
    st_Item.ul_User = 0;
    st_Item.pst_Next = NULL;

    pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	while(pst_PFElem <= pst_PFLastElem)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		pst_PFElem++;
		if(TAB_b_IsAHole(pst_GO)) continue;

        st_Item.p_Content = (void *) pst_GO;
	
    	if( !pfnb_Callback( &st_Item, _ul_Param1, _ul_Param2) ) return;
	}
}

/*
 =======================================================================================================================
    Aim:    Add item callback (used by selection list duplication)
 =======================================================================================================================
 */
BOOL SEL_b_AddItemClbk(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*s;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	s = SEL_pst_AddItem((SEL_tdst_Selection *) _ul_Param1, _pst_Sel->p_Content, _pst_Sel->l_Flag);
	s->ul_User = _pst_Sel->ul_User;
	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Duplicate a selection list
 =======================================================================================================================
 */
void SEL_Duplicate(SEL_tdst_Selection *_pst_TgtSel, SEL_tdst_Selection *_pst_SrcSel)
{
	SEL_Init(_pst_TgtSel);
	SEL_EnumItem(_pst_SrcSel, SEL_C_SIF_All, SEL_b_AddItemClbk, (ULONG) _pst_TgtSel, (ULONG)NULL);
}

/*
 =======================================================================================================================
    Aim:    Return first item in selected list that match given flags
 =======================================================================================================================
 */
void *SEL_pst_GetFirstItem(SEL_tdst_Selection *_pst_Sel, LONG _l_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (!_pst_Sel)
        return NULL;

	pst_Item = _pst_Sel->pst_FirstItem;
	while(pst_Item != NULL)
	{
		if(pst_Item->l_Flag & _l_Flag) return pst_Item->p_Content;
		pst_Item = pst_Item->pst_Next;
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Return first item in selected list that match given flags
 =======================================================================================================================
 */
SEL_tdst_SelectedItem *SEL_pst_GetFirst(SEL_tdst_Selection *_pst_Sel, LONG _l_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Item = _pst_Sel->pst_FirstItem;
	while(pst_Item != NULL)
	{
		if(pst_Item->l_Flag & _l_Flag) return pst_Item;
		pst_Item = pst_Item->pst_Next;
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Return next item in selected list that match given flags
 =======================================================================================================================
 */
SEL_tdst_SelectedItem *SEL_pst_GetNext(SEL_tdst_Selection *_pst_Sel, SEL_tdst_SelectedItem *_pst_Item, LONG _l_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* vérifie que l'item est dans la liste */
    pst_Next = _pst_Sel->pst_FirstItem;
    while (pst_Next != NULL)
    {
        if (pst_Next == _pst_Item) break;
        pst_Next = pst_Next->pst_Next;
    }

    if (!pst_Next) return NULL;

	pst_Next = pst_Next->pst_Next;
	while(pst_Next != NULL)
	{
		if(pst_Next->l_Flag & _l_Flag) return pst_Next;
		pst_Next = pst_Next->pst_Next;
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim:    circle through selection (second becomes first, first becomes last)
 =======================================================================================================================
 */
void SEL_Circle( SEL_tdst_Selection *_pst_Sel, int dir)
{
    SEL_tdst_SelectedItem *pst_First, *pst_NewLast;

    if (_pst_Sel->pst_FirstItem == _pst_Sel->pst_LastItem) return;

    if (dir)
    {
        pst_First = _pst_Sel->pst_FirstItem;
        _pst_Sel->pst_LastItem->pst_Next = pst_First;
        _pst_Sel->pst_FirstItem = pst_First->pst_Next;
        pst_First->pst_Next = NULL;
        _pst_Sel->pst_LastItem = pst_First;
        return;
    }

    pst_NewLast = _pst_Sel->pst_FirstItem;
    while (pst_NewLast->pst_Next != _pst_Sel->pst_LastItem)
        pst_NewLast = pst_NewLast->pst_Next;
    

    _pst_Sel->pst_LastItem->pst_Next = _pst_Sel->pst_FirstItem;
    _pst_Sel->pst_FirstItem = _pst_Sel->pst_LastItem;
    _pst_Sel->pst_LastItem = pst_NewLast;
    pst_NewLast->pst_Next = NULL;
}

#endif ACTIVE_EDITORS
