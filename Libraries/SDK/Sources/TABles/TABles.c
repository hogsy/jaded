/*$T TABles.c GC!1.55 01/12/00 09:57:38 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Handle basic tables and lists structures

    Note:   See note in TABles.h
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/ERRors/ERRasser.h"
#include "TABles/TABles.h"
#include "MATHs/MATH.h"
#include "TABvars.h"

/*$4
 ***********************************************************************************************************************
    Functions related to double linked list
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Create a new double linked list
 =======================================================================================================================
 */
void TAB_DLlist_Create(TAB_tdst_DLlist *_pst_List)
{
    _pst_List->ul_NbElems = 0;
    _pst_List->pst_LastElem = 0;
    _pst_List->pst_FirstElem = 0;
}

/*
 =======================================================================================================================
    Aim:    Frees all the Elem of a double linked list

    Note:   The data owned by each Elem must be freed first (because Elem are void*, there is no way to know how to
            free the data at this level )
 =======================================================================================================================
 */
void TAB_DLlist_Remove(TAB_tdst_DLlist *_pst_List)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_DLlistElem *pst_CurrentElem, *pstNextElem, *pst_NewCurrentElem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_CurrentElem = _pst_List->pst_FirstElem;

    while(pst_CurrentElem != 0)
    {
        pstNextElem = pst_CurrentElem->pst_NextElem;
        pst_NewCurrentElem = pst_CurrentElem->pst_NextElem;
        MEM_Free(pst_CurrentElem);
        pst_CurrentElem = pst_NewCurrentElem;
    }
}

/*
 =======================================================================================================================
    Aim:    Creates an new empty double linked list Element

    Note:   Remember you must give a pointer to the element to create it, not the element itself...
 =======================================================================================================================
 */
void TAB_DLlist_CreateElem(TAB_tdst_DLlistElem **ppst_LinkedListElem)
{
    *ppst_LinkedListElem = (TAB_tdst_DLlistElem *) MEM_p_Alloc(sizeof(TAB_tdst_DLlistElem));
    (*ppst_LinkedListElem)->pst_NextElem = 0;
    (*ppst_LinkedListElem)->pst_PrevElem = 0;
    (*ppst_LinkedListElem)->p_Data = 0;
}

/*
 =======================================================================================================================
    Aim:    Creates a new Elem (with a given data) and links it at the end of a given Linked list
 =======================================================================================================================
 */
void TAB_DLlist_CreateAndAddElem(TAB_tdst_DLlist *_pst_List, void *_p_Data)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_DLlistElem *pst_LinkedListElem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* We create the new Elem */
    TAB_DLlist_CreateElem(&pst_LinkedListElem);

    /* And set the data */
    pst_LinkedListElem->p_Data = _p_Data;

    /*
     * If the list is empty, we set the last Elem to the new
     * created Elem
     */
    if(_pst_List->pst_LastElem == 0)
    {
        _pst_List->pst_FirstElem = pst_LinkedListElem;
        _pst_List->pst_LastElem = pst_LinkedListElem;
    }
    else
    {
        /* We chain the new Elem to the last Elem */
        _pst_List->pst_LastElem->pst_NextElem = pst_LinkedListElem;
        pst_LinkedListElem->pst_PrevElem = _pst_List->pst_LastElem;

        /* And make the new Elem become the last of the list */
        _pst_List->pst_LastElem = pst_LinkedListElem;
    }

    _pst_List->ul_NbElems++;
}

/*
 =======================================================================================================================
    Aim:    Remove Elem from double linked list

    Note:   Be sure to free the data pointed by the Elem before freeing the Elem
 =======================================================================================================================
 */
void TAB_DLlist_RemoveElem(TAB_tdst_DLlist *_pst_List, TAB_tdst_DLlistElem *_pst_LinkedListElem)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_DLlistElem *pst_PrevElem;
    TAB_tdst_DLlistElem *pst_NextElem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_PrevElem = (_pst_LinkedListElem->pst_PrevElem);
    pst_NextElem = (_pst_LinkedListElem->pst_NextElem);

    if(pst_PrevElem)
        pst_PrevElem->pst_NextElem = pst_NextElem;
    else
        /* We removed the first element of the list */
        _pst_List->pst_FirstElem = pst_NextElem;

    if(pst_NextElem)
        pst_NextElem->pst_PrevElem = pst_PrevElem;
    else
        /* We remove the last element of the list */
        _pst_List->pst_LastElem = pst_PrevElem;

    _pst_List->ul_NbElems--;
    MEM_Free(_pst_LinkedListElem);
}

/*$4
 ***********************************************************************************************************************
    Tables manager related functions
 ***********************************************************************************************************************
 */

/*
 * Aim: check that a table is correct (avoid crashes because
 * of memory was overwriten of freed)
 */
#if defined(_DEBUG) || defined(ACTIVE_EDITORS)

/*
 =======================================================================================================================
    Aim:    Check if a Ptable or a PFtable is OK
 =======================================================================================================================
 */
char TAB_b_Check(void *p_CurrentTable)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char            b_BadCheck;

    TAB_tdst_Ptable *pst_CurrentTable;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_BadCheck = 0;
    if(!MEM_b_CheckPointer(p_CurrentTable))
    {
        b_BadCheck++;
        goto _end;
    }

    pst_CurrentTable = (TAB_tdst_Ptable *) p_CurrentTable;

    if(!TAB_b_CheckType(pst_CurrentTable->uc_Type))
    {
        b_BadCheck++;
        goto _end;
    }

    if(pst_CurrentTable->p_LastPointer && !MEM_b_CheckPointer((void *) pst_CurrentTable->p_LastPointer))
    {
        b_BadCheck++;
        goto _end;
    }

    if(pst_CurrentTable->p_NextElem && !MEM_b_CheckPointer((void *) pst_CurrentTable->p_NextElem))
    {
        b_BadCheck++;
        goto _end;
    }

_end:
    if(b_BadCheck)
        return(0);
    else
        return(1);
}

#endif

/*
 =======================================================================================================================
    Aim:    Remove holes of all the tables in the given tables list

    In:     _pst_List   Double linked list that holds all the tables _f_HolesRatio Maximum allowed percentage of holes
                        in tables
 =======================================================================================================================
 */
void TAB_TablesManager_RemoveHolesFromTablesList(TAB_tdst_DLlist *_pst_List)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_DLlistElem *pst_CurrentElem, *pstNextElem;
    TAB_tdst_Ptable     *pst_CurrentTable;
	ULONG	ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_CurrentElem = _pst_List->pst_FirstElem;
	ul_Index = 0;
	

    while(pst_CurrentElem != 0)
    {
		if(ul_Index >= _pst_List->ul_NbElems) 
		{
			ERR_X_Warning((ul_Index < _pst_List->ul_NbElems), "TAB overflow", NULL);
			break;
		}

        pstNextElem = pst_CurrentElem->pst_NextElem;
        pst_CurrentTable = (TAB_tdst_Ptable *) (pst_CurrentElem->p_Data);

#if defined(_DEBUG) || defined(ACTIVE_EDITORS)
        if(!TAB_b_Check(pst_CurrentTable)) break;
#endif
        /* Case of the normal Ptable */
        if(pst_CurrentTable->uc_Type == TAB_Cuc_Ptable)
        {
            if(fSup(TAB_f_Ptable_GetHolesRatio(pst_CurrentTable), TAB_f_Ptable_GetMaxHolesRatio(pst_CurrentTable)))
                TAB_Ptable_RemoveHoles(pst_CurrentTable);
        }
        else
            /* Case of the PFtable */
        if(pst_CurrentTable->uc_Type == TAB_Cuc_PFtable)
        {
            if
            (
                fSup
                (
                    TAB_f_PFtable_GetHolesRatio((TAB_tdst_PFtable *) pst_CurrentTable),
                    TAB_f_PFtable_GetMaxHolesRatio((TAB_tdst_PFtable *) pst_CurrentTable)
                )
            ) TAB_PFtable_RemoveHoles((TAB_tdst_PFtable *) pst_CurrentTable);
        }

        pst_CurrentElem = pst_CurrentElem->pst_NextElem;
		ul_Index++;
    }
}

/*
 =======================================================================================================================
    Aim:    Tables manager function
 =======================================================================================================================
 */
void TAB_TablesManager_Call(void)
{
    TAB_TablesManager_RemoveHolesFromTablesList(&gst_TablesManagerList);
}

/*
 =======================================================================================================================
    Aim:    Tables manager creation and initialisation
 =======================================================================================================================
 */
void TAB_TablesManager_Init(void)
{
    TAB_DLlist_Create(&gst_TablesManagerList);
    gb_TablesManagerListInitialized = 1;
}

/*
 =======================================================================================================================
    Aim:    Destroy the tables manager
 =======================================================================================================================
 */
void TAB_TablesManager_Close(void)
{
    if(gb_TablesManagerListInitialized)
    {
        TAB_DLlist_Remove(&gst_TablesManagerList);
    }

    gb_TablesManagerListInitialized = 0;
}

/*
 =======================================================================================================================
    Aim:    Let the main tables manager know about a table

    Note:   It is not tested if the table is already registered. So be careful, tables could be registered more than
            once.£
            the list manager must have been initialized before
 =======================================================================================================================
 */
void TAB_TablesManager_RegisterTable(void *_pst_Table)
{
    /* We check that the list manager has been initialized */
    ERR_X_Assert(gb_TablesManagerListInitialized);

    /*
     * If the maxholes ratio is not set to "Ignore Holes", we
     * register the table to the Tables double linked list
     */
    if(fDiff(((TAB_tdst_Ptable *) _pst_Table)->f_MaxHolesRatio, TAB_Cf_IgnoreHoles))
    {
        TAB_DLlist_CreateAndAddElem(&gst_TablesManagerList, (void *) _pst_Table);
    }
}

/*
 =======================================================================================================================
    Aim:    Remove a table from the tables manager list of tables
 =======================================================================================================================
 */
void TAB_TablesManager_UnregisterTable(void *_pst_Table)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_DLlistElem *pst_CurrentElem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_CurrentElem = gst_TablesManagerList.pst_FirstElem;

    /* We loop thru the list to find our table */
    while(pst_CurrentElem)
    {
        if(pst_CurrentElem->p_Data == _pst_Table)
        {
            /* We found our table, we remove it from the list */
            TAB_DLlist_RemoveElem(&gst_TablesManagerList, pst_CurrentElem);
            break;
        }

        pst_CurrentElem = TAB_pst_DLlist_GetNextElem(pst_CurrentElem);
    }
}

/*$4
 ***********************************************************************************************************************
    Functions related to pointers table
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Create the pointers table

    In:     _pst_Table          : Table to initialize
            _ul_NbElems         : Maximum Nb of elements for this table
            _f_MaxHolesRatio    : The maximal ratio of holes for this table (0.1f indicates 10% of holes max)
 =======================================================================================================================
 */
void TAB_Ptable_Init(TAB_tdst_Ptable *_pst_Table, ULONG _ul_NbElems, float _f_MaxHolesRatio)
{
    /* At least space for 2 Elements */
    if(_ul_NbElems < 2)
    {
        _ul_NbElems = 2;
    }

    _pst_Table->p_Table = (void **) MEM_p_Alloc(_ul_NbElems * 4);
    _pst_Table->p_LastPointer = (_pst_Table->p_Table) + (_ul_NbElems - 1);
    _pst_Table->p_NextElem = (_pst_Table->p_Table);
    _pst_Table->ul_NbHoles = 0;
    _pst_Table->ul_NbElems = 0;
    _pst_Table->uc_Type = TAB_Cuc_Ptable;
    _pst_Table->f_MaxHolesRatio = _f_MaxHolesRatio;

    /*
     * We let the tables manager know about the new table, so the
     * holes we automaticaly be removed when the holes ratio is
     * high
     */
    TAB_TablesManager_RegisterTable((void *) _pst_Table);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TAB_Ptable_Clear(TAB_tdst_Ptable *_pst_Table)
{
    _pst_Table->p_NextElem = (_pst_Table->p_Table);
    _pst_Table->ul_NbHoles = 0;
    _pst_Table->ul_NbElems = 0;
}

/*
 =======================================================================================================================
    Aim:    Resize a pointers table

    Note:   The object pointers of the table given in parameters are preserved
 =======================================================================================================================
 */
void TAB_Ptable_Resize(TAB_tdst_Ptable *_pst_Table, ULONG _ul_NbElems)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul_OldMaxNbElems;   /* Maximum numbers of elements of the old table */
    ULONG   ul_MaxNbElems;      /* Maximum numbers of elements of the new table */
    void    **p_OldTable;       /* Old pointers table */
    ULONG   i;                  /* Loop index */
    ULONG   ul_NextIndex;       /* Previous next index */
    ULONG   ul_Max;             /* Previous next index */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_OldMaxNbElems = TAB_ul_Ptable_GetMaxNbElems(_pst_Table);

	/* Gran */
	if(abs((int)(_ul_NbElems - ul_OldMaxNbElems)) < 256) 
		_ul_NbElems = ul_OldMaxNbElems + 256;

    ul_NextIndex = TAB_ul_Ptable_GetNextElemIndex(_pst_Table);
    p_OldTable = _pst_Table->p_Table;

    /* Leave at least space for 2 Elements */
    if(_ul_NbElems < 2) _ul_NbElems = 2;

    /* Allocates a new table */
    _pst_Table->p_Table = (void **) MEM_p_Alloc(_ul_NbElems * 4);

    /* Changes the last possible pointer of the table */
    _pst_Table->p_LastPointer = (_pst_Table->p_Table) + (_ul_NbElems - 1);

    ul_MaxNbElems = TAB_ul_Ptable_GetMaxNbElems(_pst_Table);

    /*
     * Copies the pointers into the new table. Note that if the
     * new table is smaller, pointers may be lost...
     */
    if(ul_MaxNbElems < ul_OldMaxNbElems)
        ul_Max = ul_MaxNbElems;
    else
        ul_Max = ul_OldMaxNbElems;
    for(i = 0; i < ul_Max; i++)
        _pst_Table->p_Table[i] = p_OldTable[i];

    /* VL set next element pointer into new table */
    if(ul_NextIndex >= _ul_NbElems)
        ul_NextIndex = _ul_NbElems - 1;

	ERR_X_Assert(ul_NextIndex >= 0);
    _pst_Table->p_NextElem = _pst_Table->p_Table + ul_NextIndex;

    /* Frees the old table */
    MEM_Free(p_OldTable);
}

/*
 =======================================================================================================================
    Aim:    Remove a PTable

    In:     _pst_Table  
 =======================================================================================================================
 */
void TAB_Ptable_Close(TAB_tdst_Ptable *_pst_Table)
{
    MEM_Free(_pst_Table->p_Table);
    _pst_Table->p_Table = 0;
    _pst_Table->ul_NbElems = 0;
    _pst_Table->ul_NbHoles = 0;
    _pst_Table->p_NextElem = 0;
    _pst_Table->p_LastPointer = 0;

    /* Unregister the table */
    TAB_TablesManager_UnregisterTable((void *) _pst_Table);
}

/*
 =======================================================================================================================
    Aim:    Remove all the holes of a Ptable
 =======================================================================================================================
 */
void TAB_Ptable_RemoveHoles(TAB_tdst_Ptable *_pst_Table)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* PElem1Pointer points to a hole */
    void    **pElem1Pointer;
    void    **pLastLoopPointer;

    /* PElem2Pointer points to a "non-hole" */
    void    **pElem2Pointer;

    /* Unsigned LONG ul_OldCurrentIndex2; */
    void    **pNextIndex;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pElem1Pointer = _pst_Table->p_Table;
    pLastLoopPointer = _pst_Table->p_NextElem;
    pElem2Pointer = pElem1Pointer;

    /*
     * We know that at the end, the nextindex will be the current
     * next index - the number of holes...
     */
    pNextIndex = _pst_Table->p_NextElem;

    _pst_Table->p_NextElem -= _pst_Table->ul_NbHoles;
	ERR_X_Assert(_pst_Table->p_NextElem >= _pst_Table->p_Table);

    for(; pElem1Pointer < pLastLoopPointer; pElem1Pointer++)
    {
        /* If we have a hole pointed by pElem1Pointer */
        if(TAB_b_IsAHole(*pElem1Pointer))
        {
            if(pElem2Pointer < pElem1Pointer)
            {
                pElem2Pointer = pElem1Pointer;
            }

            /*
             * We look for the next Elem which is not a hole and make
             * pElem2Pointer point to this Elem
             */
            do
            {
                pElem2Pointer++;

                if(pElem2Pointer == pNextIndex)
                {
					if (_pst_Table->ul_NbHoles==0)
					{
						_pst_Table->p_NextElem--;
					}
                    _pst_Table->ul_NbHoles = 0;
                    goto _TheEnd;
                }
            } while(TAB_b_IsAHole(*pElem2Pointer)) ;

            /*
             * Here, pElem2Pointer points to a "non-hole" Elem so we move
             * this Elem to the location pointed by ul_CurrentIndex1
             */
            *pElem1Pointer = *pElem2Pointer;
            (*pElem2Pointer) = (void *) 1;
        }
    }

_TheEnd: ;
	_pst_Table->ul_NbHoles = 0;
}

/*
 =======================================================================================================================
    Aim:    Remove an Elem from the table

    Note:   If the Elem is already a hole nothing is performed, except if it's the last elem of the table.£
 =======================================================================================================================
 */
void TAB_Ptable_RemoveElem(TAB_tdst_Ptable *_pst_Table, void **_ppv_Elem)
{
    /* In debug, checks that the Elem is within the range */
    ERR_X_Assert(TAB_b_Ptable_IsInRange(_pst_Table, _ppv_Elem))

	/* We handle the case of the last elem of the list */
    if((_ppv_Elem) == (_pst_Table->p_NextElem) - 1)
    {
		/* if the last elem was a hole we decrement the number of holes by 1 */
		if(TAB_b_IsAHole(_ppv_Elem))
			_pst_Table->ul_NbHoles--;
		else
			_pst_Table->ul_NbElems--;
		_pst_Table->p_NextElem=_ppv_Elem;
		*_ppv_Elem = (void *) 1;
		return;
    }

    /* We handle the normal case */
    TAB_Ptable_SetAsHole(_pst_Table, _ppv_Elem);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TAB_Ptable_RemoveElemAtIndex(TAB_tdst_Ptable *_pst_Table, ULONG _ul_Index)
{
    TAB_Ptable_RemoveElem(_pst_Table, &_pst_Table->p_Table[_ul_Index]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TAB_Ptable_RemoveElemWithPointer(TAB_tdst_Ptable *_pst_Table, void *_p_Pointer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    void    **ppv_RemovedElem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ppv_RemovedElem = TAB_ppv_Ptable_GetElemWithPointer(_pst_Table, _p_Pointer);
    if(ppv_RemovedElem)
        TAB_Ptable_RemoveElem(_pst_Table, ppv_RemovedElem);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TAB_ul_Ptable_GetElemIndexWithPointer(TAB_tdst_Ptable *_pst_Table, void *_p_Pointer)
{
    /*~~~~~~~~~~~~~~~~~~~~*/
    void    **ppv_First;
    ULONG   ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~*/

    ppv_First = _pst_Table->p_Table;
    ul_Index = 0;
    while(ppv_First < _pst_Table->p_NextElem)
    {
        if(_p_Pointer == *ppv_First) return ul_Index;
        ppv_First++;
        ul_Index++;
    }

    return TAB_Cul_BadIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void **TAB_ppv_Ptable_GetElemWithPointer(TAB_tdst_Ptable *_pst_Table, void *_p_Pointer)
{
    /*~~~~~~~~~~~~~~~~~~~~*/
    void    **ppv_First;
    /*~~~~~~~~~~~~~~~~~~~~*/

    ppv_First = _pst_Table->p_Table;
    while(ppv_First < _pst_Table->p_NextElem)
    {
        if(_p_Pointer == *ppv_First) return ppv_First;
        ppv_First++;
    }

    return NULL;
}

/*
 =======================================================================================================================
    Aim:    Move an Elem from a table to another
 =======================================================================================================================
 */
void TAB_Ptable_AddElemFromOtherPtableAtIndex
(
    TAB_tdst_Ptable *_pst_DstTable,
    TAB_tdst_Ptable *_pst_SrcTable,
    ULONG   _ul_SrcIndex
)
{
    TAB_Ptable_AddElem(_pst_DstTable, _pst_SrcTable->p_Table[_ul_SrcIndex]);
    TAB_Ptable_RemoveElemAtIndex(_pst_SrcTable, _ul_SrcIndex);
}

/*
 =======================================================================================================================
    Aim:    Fills a table with random pointers (for tests only)
 =======================================================================================================================
 */
void TAB_Ptable_TestFill(TAB_tdst_Ptable *_pst_Table)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    void    **pElem1Pointer;
    void    **pLastLoopPointer;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pElem1Pointer = _pst_Table->p_Table;
    pLastLoopPointer = _pst_Table->p_LastPointer;

    for(; pElem1Pointer <= pLastLoopPointer; pElem1Pointer++)
    {
        /* Simule une adresse bidon alignee sur 4 octets */
        *pElem1Pointer = (void *) (rand() & 0xFFFC);
        _pst_Table->p_NextElem++;
        _pst_Table->ul_NbElems++;
    }
}

/*
 =======================================================================================================================
    Aim:    Merge two tables:: For each element of the Src table, add it into the Dst table if it is not already into

    Note:   The Dst table is automatically resized if it becomes full
 =======================================================================================================================
 */
void TAB_Ptable_Merge(TAB_tdst_Ptable *_pst_Dst, TAB_tdst_Ptable *_pst_Src)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
    void    **pp_Elem;
    void    **pp_LastElem;
    /*~~~~~~~~~~~~~~~~~~~~~~*/

    pp_Elem = _pst_Src->p_Table;
    pp_LastElem = _pst_Src->p_NextElem;

    for(; pp_Elem < pp_LastElem; pp_Elem++)
    {
        /* For each element of the Dst table */
        if(!TAB_b_IsAHole(*pp_Elem))
        {
            /* If it is not into the Dst table, add it */
            if(TAB_ul_Ptable_GetElemIndexWithPointer(_pst_Dst, *pp_Elem) == TAB_Cul_BadIndex)
                TAB_Ptable_AddElemAndResize(_pst_Dst, *pp_Elem);
        }
    }
}

/*$4
 ***********************************************************************************************************************
    Functions related to pointers and flags table £
    they have the same name as the pointers table related functions, with a _PF at the end (PF for Pointers and flags)
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Create the PF table

    In:     _pst_Table          : Table to initialize
            _ul_NbElems         : Maximum Nb of elements for this table
            _f_MaxHolesRatio    : The maximal ratio of holes for this table (0.1f indicates 10% of holes max)
 =======================================================================================================================
 */
void TAB_PFtable_Init(TAB_tdst_PFtable *_pst_Table, ULONG _ul_NbElems, float _f_MaxHolesRatio)
{
    /* At least space for 2 Elements */
    if(_ul_NbElems < 2)
    {
        _ul_NbElems = 2;
    }

    _pst_Table->p_Table = (TAB_tdst_PFelem *) MEM_p_Alloc(_ul_NbElems * 8);
    _pst_Table->p_LastPointer = (_pst_Table->p_Table) + (_ul_NbElems - 1);
    _pst_Table->p_NextElem = (_pst_Table->p_Table);
    _pst_Table->ul_NbHoles = 0;
    _pst_Table->ul_NbElems = 0;
    _pst_Table->f_MaxHolesRatio = _f_MaxHolesRatio;

    /* Set the correct type (FPtable) */
    _pst_Table->uc_Type = TAB_Cuc_PFtable;

    /*
     * We let the tables manager know about the new table, so the
     * holes we automaticaly be removed when the holes ratio is
     * high
     */
    TAB_TablesManager_RegisterTable((void *) _pst_Table);
}

/*
 =======================================================================================================================
    Aim:    Clear the table

    Note:   No need to change anything in the table, just reset the NextElem, nb of holes and nb of Elems £
 =======================================================================================================================
 */
void TAB_PFtable_Clear(TAB_tdst_PFtable *_pst_Table)
{
    _pst_Table->p_NextElem = (_pst_Table->p_Table);
    _pst_Table->ul_NbHoles = 0;
    _pst_Table->ul_NbElems = 0;
}

/*
 =======================================================================================================================
    Aim:    Resize a pointers & flags table

    Note:   The object pointers of the table given in parameters are preserved
 =======================================================================================================================
 */
void TAB_PFtable_Resize(TAB_tdst_PFtable *_pst_Table, ULONG _ul_NbElems)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG           ul_OldMaxNbElems;   /* Maximum numbers of elements of the old table */
    ULONG           ul_MaxNbElems;      /* Maximum numbers of elements of the new table */
    TAB_tdst_PFelem *p_OldTable;        /* Old pointers table */
    ULONG           i;                  /* Loop index */
    ULONG           ul_NextIndex;       /* Previous next index */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_OldMaxNbElems = TAB_ul_PFtable_GetMaxNbElems(_pst_Table);

	/* Gran */
	if(abs((int)(ul_OldMaxNbElems - _ul_NbElems)) < 256)
		_ul_NbElems = ul_OldMaxNbElems  + 256;

    ul_NextIndex = TAB_ul_PFtable_GetNextElemIndex(_pst_Table);
    p_OldTable = _pst_Table->p_Table;

    /* Leave at least space for 2 Elements */
    if(_ul_NbElems < 2) _ul_NbElems = 2;

    /* Allocates a new table */
    _pst_Table->p_Table = (TAB_tdst_PFelem *) MEM_p_VMAlloc(_ul_NbElems * 8);

    /* Changes the last possible pointer of the table */
    _pst_Table->p_LastPointer = (_pst_Table->p_Table) + (_ul_NbElems - 1);

    ul_MaxNbElems = TAB_ul_PFtable_GetMaxNbElems(_pst_Table);

    /*
     * Copies the pointers into the new table. Note that if the
     * new table is smaller, pointers may be lost...
     */
    for(i = 0; i < (ul_MaxNbElems < ul_OldMaxNbElems ? ul_MaxNbElems : ul_OldMaxNbElems); i++)
        _pst_Table->p_Table[i] = p_OldTable[i];

    /* VL set next element pointer into new table */
    if(ul_NextIndex >= _ul_NbElems)
        ul_NextIndex = _ul_NbElems - 1;

    _pst_Table->p_NextElem = _pst_Table->p_Table + ul_NextIndex;

    /* Frees the old table */
    MEM_Free(p_OldTable);
}

/*
 =======================================================================================================================
    In:     _pst_Table  
 =======================================================================================================================
 */
void TAB_PFtable_Close(TAB_tdst_PFtable *_pst_Table)
{
    if(_pst_Table->p_Table)
        MEM_Free(_pst_Table->p_Table);
    _pst_Table->p_Table = 0;
    _pst_Table->ul_NbElems = 0;
    _pst_Table->ul_NbHoles = 0;
    _pst_Table->p_NextElem = 0;
    _pst_Table->p_LastPointer = 0;

    /* Unregister the table */
    TAB_TablesManager_UnregisterTable((void *) _pst_Table);
}

/*
 =======================================================================================================================
    Aim:    Remove all the holes of a PF table
 =======================================================================================================================
 */
void TAB_PFtable_RemoveHoles(TAB_tdst_PFtable *_pst_Table)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* PElem1Pointer points to a hole */
    TAB_tdst_PFelem *pElem1Pointer;
    TAB_tdst_PFelem *pLastLoopPointer;

    /* PElem2Pointer points to a "non-hole" */
    TAB_tdst_PFelem *pElem2Pointer;

    TAB_tdst_PFelem *pNextIndex;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pElem1Pointer = _pst_Table->p_Table;
    pLastLoopPointer = _pst_Table->p_NextElem;
    pElem2Pointer = pElem1Pointer;

    /*
     * We know that at the end, the nextindex will be the current
     * next index - the number of holes...
     */
    pNextIndex = _pst_Table->p_NextElem;

    _pst_Table->p_NextElem -= _pst_Table->ul_NbHoles;
	ERR_X_Assert(_pst_Table->p_NextElem >= _pst_Table->p_Table);

    for(; pElem1Pointer < pLastLoopPointer; pElem1Pointer++)
    {
        /* If we have a hole pointed by pElem1Pointer */
        if(TAB_b_IsAHole(pElem1Pointer->p_Pointer))
        {
            if(pElem2Pointer < pElem1Pointer)
            {
                pElem2Pointer = pElem1Pointer;
            }

            /*
             * We look for the next Elem which is not a hole and make
             * pElem2Pointer point to this Elem
             */
            do
            {
                pElem2Pointer++;
                if(pElem2Pointer == pNextIndex)
                {
					/* anti-bug */
					if (_pst_Table->ul_NbHoles==0)
					{
						_pst_Table->p_NextElem--;
					}
                    _pst_Table->ul_NbHoles = 0;
                    goto _TheEnd;
                }
            } while(TAB_b_IsAHole(pElem2Pointer->p_Pointer)) ;

            /*
             * Here, pElem2Pointer points to a "non-hole" Elem so we move
             * this Elem to the location pointed by ul_CurrentIndex1
             */
            *pElem1Pointer = *pElem2Pointer;
            (pElem2Pointer->p_Pointer) = (void *) 1;
        }
    }

_TheEnd: ;
	ERR_X_Assert(_pst_Table->ul_NbHoles == 0);
}

/*
 =======================================================================================================================
    Aim:    Remove an Elem given by its index from a PF table

    Note:   Search element with index and call RemoveElem function
 =======================================================================================================================

 */
void TAB_PFtable_RemoveElemAtIndex(TAB_tdst_PFtable *_pst_Table, ULONG _ul_Index)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_PFelem *p_RemovedElem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    p_RemovedElem = (TAB_tdst_PFelem *) &_pst_Table->p_Table[_ul_Index];
    TAB_PFtable_RemoveElem(_pst_Table, p_RemovedElem);
}

/*
 =======================================================================================================================
    Aim:    Remove an Elem from a PF table
 =======================================================================================================================
 */
void TAB_PFtable_RemoveElem(TAB_tdst_PFtable *_pst_Table, TAB_tdst_PFelem *_pst_Elem)
{
    /* In debug, checks that the Elem is within the range */
    ERR_X_Assert(TAB_b_PFtable_IsInRange(_pst_Table, _pst_Elem))

	/* We handle the case of the last elem of the list */
    if((_pst_Elem) == (_pst_Table->p_NextElem) - 1)
    {
		/* if the last elem was a hole we decrement the number of holes by 1 */
		if(TAB_b_IsAHole(_pst_Elem))
			_pst_Table->ul_NbHoles--;
		else
			_pst_Table->ul_NbElems--;
		_pst_Table->p_NextElem=_pst_Elem;
		_pst_Elem->p_Pointer = (void *) 1;
		_pst_Elem->ul_Flags = 0;
		return;
	}

    /* We handle the normal case */
    TAB_PFtable_SetAsHole(_pst_Table, _pst_Elem);
}

/*
 =======================================================================================================================
    Aim:    Remove an Elem given by its pointer from a PF table

    Note:   Search element with index and call RemoveElem function
 =======================================================================================================================
 */
void TAB_PFtable_RemoveElemWithPointer(TAB_tdst_PFtable *_pst_Table, void *_p_Pointer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_PFelem *p_RemovedElem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    p_RemovedElem = TAB_pst_PFtable_GetElemWithPointer(_pst_Table, _p_Pointer);
    if(p_RemovedElem)
        TAB_PFtable_RemoveElem(_pst_Table, p_RemovedElem);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TAB_ul_PFtable_GetElemIndexWithPointer(TAB_tdst_PFtable *_pst_Table, void *_p_Pointer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_PFelem *pst_First;
    ULONG           ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_First = _pst_Table->p_Table;
    ul_Index = 0;
    while(pst_First < _pst_Table->p_NextElem)
    {
        if(_p_Pointer == pst_First->p_Pointer) return ul_Index;
        pst_First++;
        ul_Index++;
    }

    return TAB_Cul_BadIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
TAB_tdst_PFelem *TAB_pst_PFtable_GetElemWithPointer(TAB_tdst_PFtable *_pst_Table, void *_p_Pointer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_PFelem *pst_First;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_First = _pst_Table->p_Table;
    while(pst_First < _pst_Table->p_NextElem)
    {
        if(_p_Pointer == pst_First->p_Pointer) return pst_First;
        pst_First++;
    }

    return NULL;
}

/*
 =======================================================================================================================
    Aim:    TAB module initilisation and close
 =======================================================================================================================
 */
void TAB_InitModule(void)
{
    TAB_TablesManager_Init();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TAB_CloseModule(void)
{
    TAB_TablesManager_Close();
}

