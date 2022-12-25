/*$T TABles.h GC! 1.078 03/13/00 16:50:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * Aim: Handle basic tables and lists structures Note: The handled structures are: £
 * £
 * - Ptable (non sorted table of pointers with holes) £
 * - PFtable (non sorted table of pointers and 32 bits flags with holes) £
 * - DLlist (Double linked list)
 */
#ifndef _TABLES_H_
#define _TABLES_H_

#include "MATHs/MATH.h"
#include "BASe/BASsys.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/BAStypes.h"

#if defined(_XBOX) && !defined(_XENON)
#include <xtl.h>
#endif //_XBOX


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define TAB_Cul_BadIndex	0xFFFFFFFF

/* Types of lists */
#define TAB_Cuc_Ptable	20
#define TAB_Cuc_PFtable 30

/* Default Maximum Holes Ratio for tables (0.1 indicates 10% of holes maximum ) */
#define TAB_Cf_DefaultMaxHolesRatio 0.1f
#define TAB_Cf_IgnoreHoles			1.0f	/* Holes will never be removed for this table */

/* Error Messages in debug */
#define TAB_Csz_Error_PTableIsFull	"a P Table is full"
#define TAB_Csz_Error_PFTableIsFull "a PF Table is full"

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    PTable stands for Pointers Table £
            This structure contains the informations necessary to handle a non sorted table of pointers with holes.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TAB_tdst_Ptable_
{
	void			**p_LastPointer;	/* Pointer to last possible Elem of the table */
	void			**p_NextElem;		/* Pointer to next free Elem of the table */
	ULONG			ul_NbHoles;			/* Number of holes in the table */
	ULONG			ul_NbElems;			/* Number of non empty Elems in the table */
	void			**p_Table;			/* The table of pointers... */
	unsigned char	uc_Type;			/* The type of the table (TAB_Cuc_P for this table) */
	float			f_MaxHolesRatio;	/* The maximal holes ratio wished */
} TAB_tdst_Ptable;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    PFElem stands for Pointer and Flag Elem

    Note:   This is the Elem of a PFTable (see below)
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TAB_tdst_PFelem_
{
	void	*p_Pointer; /* Pointer to an Elem */
	ULONG	ul_Flags;	/* Flags of this Elem */
} TAB_tdst_PFelem;

/*
 =======================================================================================================================
    Aim:    Test a flag of a PFelem
 =======================================================================================================================
 */
_inline_ ULONG TAB_b_PFelem_TestFlag(TAB_tdst_PFelem *pst_Elem, ULONG _ul_Mask)
{
	return((pst_Elem->ul_Flags) & _ul_Mask);
}

typedef struct	TAB_tdst_PFtable_
{
	TAB_tdst_PFelem *p_LastPointer;		/* Pointer to last possible Elem of the table */
	TAB_tdst_PFelem *p_NextElem;		/* Pointer to next free Elem of the table */
	ULONG			ul_NbHoles;			/* Number of holes in the table */
	ULONG			ul_NbElems;			/* Number of non empty Elems in the table */
	TAB_tdst_PFelem *p_Table;			/* The table of pointers and flags */
	unsigned char	uc_Type;			/* The type of the table (TAB_Cuc_PF for this table) */
	float			f_MaxHolesRatio;	/* The maximal holes ratio wished */
} TAB_tdst_PFtable;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Elem of a double linked list (DLlist)
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TAB_tdst_DLlistElem_
{
	struct TAB_tdst_DLlistElem_ *pst_NextElem;
	struct TAB_tdst_DLlistElem_ *pst_PrevElem;
	void						*p_Data;
} TAB_tdst_DLlistElem;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Double Linked list
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TAB_tdst_DLlist_
{
	TAB_tdst_DLlistElem *pst_FirstElem;
	TAB_tdst_DLlistElem *pst_LastElem;
	ULONG				ul_NbElems;
} TAB_tdst_DLlist;

/*$4
 ***********************************************************************************************************************
    Functions related to Elems (from Ptable or PFtable)
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Tests if a pointer is a hole

    Note:   The true pointers are always 4 bytes aligned, so the end with 00 (bits) £
            We chose that the holes end with 01 £
            A pointer ending with 11 is a virtual pointer (pointer on CD-rom, pointer on our disk cache system)
 =======================================================================================================================
 */
_inline_ LONG TAB_b_IsAHole(void *_Elem)
{
	/* A hole has the last two bits equal to "01" */
	return(!_Elem || (((LONG) _Elem) & 3) == 1);
}

/*$4
 ***********************************************************************************************************************
    Functions Related to Ptables (Pointers Table)
 ***********************************************************************************************************************
 */

void	TAB_Ptable_Init(TAB_tdst_Ptable *, ULONG, float);
void	TAB_Ptable_Clear(TAB_tdst_Ptable *);
void	TAB_Ptable_Close(TAB_tdst_Ptable *);
void	TAB_Ptable_RemoveHoles(TAB_tdst_Ptable *);
void	TAB_Ptable_RemoveElemAtIndex(TAB_tdst_Ptable *, ULONG _ul_Index);
void	TAB_Ptable_RemoveElem(TAB_tdst_Ptable *, void **);
void	TAB_Ptable_RemoveElemWithPointer(TAB_tdst_Ptable *, void *);
ULONG	TAB_ul_Ptable_GetElemIndexWithPointer(TAB_tdst_Ptable *, void *);
void	**TAB_ppv_Ptable_GetElemWithPointer(TAB_tdst_Ptable *, void *);

void	TAB_Ptable_AddElemFromOtherPTable(TAB_tdst_Ptable *, TAB_tdst_Ptable *, ULONG);
void	TAB_Ptable_TestFill(TAB_tdst_Ptable *);
void	TAB_Ptable_Resize(TAB_tdst_Ptable *, ULONG _ul_NbElems);

void	TAB_Ptable_Merge(TAB_tdst_Ptable *, TAB_tdst_Ptable *);

/*
 =======================================================================================================================
    Aim:    Return a pointer to the first element of the table
 =======================================================================================================================
 */
_inline_ void **TAB_ppv_Ptable_GetFirstElem(TAB_tdst_Ptable *_pst_Table)
{
	return(_pst_Table->p_Table);
}

/*
 =======================================================================================================================
    Aim:    Return a pointer to the Last used element of the table
 =======================================================================================================================
 */
_inline_ void **TAB_ppv_Ptable_GetLastElem(TAB_tdst_Ptable *_pst_Table)
{
	return(_pst_Table->p_NextElem - 1);
}

/*
 =======================================================================================================================
    Aim:    Return the number of holes in the P table
 =======================================================================================================================
 */
_inline_ ULONG TAB_ul_Ptable_GetNbHoles(TAB_tdst_Ptable *_pst_Table)
{
	return(_pst_Table->ul_NbHoles);
}

/*
 =======================================================================================================================
    Aim:    Return the number of non empty Elems in the table
 =======================================================================================================================
 */
_inline_ ULONG TAB_ul_Ptable_GetNbElems(TAB_tdst_Ptable *_pst_Table)
{
	return(_pst_Table->ul_NbElems);
}

/*
 =======================================================================================================================
    Aim:    Returns the maximum number of Elems of a Ptable
 =======================================================================================================================
 */
_inline_ ULONG TAB_ul_Ptable_GetMaxNbElems(TAB_tdst_Ptable *_pst_Table)
{
	return ((LONG) (_pst_Table->p_LastPointer -_pst_Table->p_Table)) +1;
}

/*
 =======================================================================================================================
    Aim:    Returns the percentage (between 0 and 1) of holes in the given Ptable (percentage from the whole table)
 =======================================================================================================================
 */
_inline_ float TAB_f_Ptable_GetHolesRatio(TAB_tdst_Ptable *_pst_Table)
{
	if (_pst_Table->ul_NbElems == 0)
		return 0.f;
	else
		return fDiv(fLongToFloat(_pst_Table->ul_NbHoles), fLongToFloat(_pst_Table->ul_NbElems));
}

#define TAB_f_Ptable_GetMaxHolesRatio(_pst_Table)	((_pst_Table)->f_MaxHolesRatio)

/*
 =======================================================================================================================
    Aim:    Returns the index of the Next free Elem in a Ptable
 =======================================================================================================================
 */
_inline_ ULONG TAB_ul_Ptable_GetNextElemIndex(TAB_tdst_Ptable *_pst_Table)
{
	return(_pst_Table->p_NextElem - _pst_Table->p_Table);
}

/*
 =======================================================================================================================
    Aim:    Increments the next Elem pointer and checks (in debug only) that the list is not full
 =======================================================================================================================
 */
_inline_ void TAB_Ptable_IncNextElem(TAB_tdst_Ptable *_pst_Table)
{
	/* In debug, we check that we have not overpassed the maximal size of the list */
	ERR_X_ErrorAssert(_pst_Table->p_NextElem <= (_pst_Table->p_LastPointer), TAB_Csz_Error_PTableIsFull, NULL);
	_pst_Table->p_NextElem++;
}

/*
 =======================================================================================================================
    Aim:    Add an Elem to a Ptable (in debug, ckecks after the add if the table is full)
 =======================================================================================================================
 */
_inline_ void TAB_Ptable_AddElem(TAB_tdst_Ptable *_pst_Table, void *_p_Elem)
{
	if(_pst_Table->p_NextElem > _pst_Table->p_LastPointer)
		TAB_Ptable_RemoveHoles(_pst_Table);
	ERR_X_ErrorAssert(_pst_Table->p_NextElem <= (_pst_Table->p_LastPointer), TAB_Csz_Error_PTableIsFull, NULL);
	*(_pst_Table->p_NextElem) = _p_Elem;
	_pst_Table->ul_NbElems++;
	TAB_Ptable_IncNextElem(_pst_Table);
}

/*
 =======================================================================================================================
    Aim:    Add an element to a Ptable and resizes the table if necessary
 =======================================================================================================================
 */
_inline_ void TAB_Ptable_AddElemAndResize(TAB_tdst_Ptable *_pst_Table, void *_p_Elem)
{
	if(_pst_Table->p_NextElem > _pst_Table->p_LastPointer)
	{
		TAB_Ptable_Resize
		(
			_pst_Table,
			(_pst_Table->ul_NbElems + _pst_Table->ul_NbHoles) * 2
		);
	}

	*(_pst_Table->p_NextElem) = _p_Elem;
	_pst_Table->ul_NbElems++;
	TAB_Ptable_IncNextElem(_pst_Table);
}

/*
 =======================================================================================================================
    Aim:    Makes an Elem become a hole, increments the number of holes if necessary, decrements the nimber of Elems in
            the table
 =======================================================================================================================
 */
_inline_ void TAB_Ptable_SetAsHole(TAB_tdst_Ptable *_pst_Table, void **_pp_Elem)
{
	/* If the Elem is already a hole nothing is performed */
	if(!TAB_b_IsAHole(*_pp_Elem))
	{
		*_pp_Elem = (void *) 1;
		_pst_Table->ul_NbHoles++;
		_pst_Table->ul_NbElems--;
	}
}

/*
 =======================================================================================================================
    Aim:    Test if an Elem is within the range of a table (mainly for debug)
 =======================================================================================================================
 */
_inline_ int TAB_b_Ptable_IsInRange(TAB_tdst_Ptable *_pst_Table, void **_p_Elem)
{
	return((_p_Elem <= _pst_Table->p_LastPointer) && (_p_Elem >= _pst_Table->p_Table));
}

/*$4
 ***********************************************************************************************************************
    Functions Related to Pointers And Flags (_PF) Table
 ***********************************************************************************************************************
 */

void			TAB_PFtable_Init(TAB_tdst_PFtable *, ULONG, float);
void			TAB_PFtable_Clear(TAB_tdst_PFtable *);
void			TAB_PFtable_Close(TAB_tdst_PFtable *);
void			TAB_PFtable_RemoveHoles(TAB_tdst_PFtable *);
void			TAB_PFtable_RemoveElemAtIndex(TAB_tdst_PFtable *, ULONG);
void			TAB_PFtable_RemoveElem(TAB_tdst_PFtable *, TAB_tdst_PFelem *);
void			TAB_PFtable_RemoveElemWithPointer(TAB_tdst_PFtable *, void *);
ULONG			TAB_ul_PFtable_GetElemIndexWithPointer(TAB_tdst_PFtable *, void *);
TAB_tdst_PFelem *TAB_pst_PFtable_GetElemWithPointer(TAB_tdst_PFtable *, void *);
void			TAB_PFtable_Resize(TAB_tdst_PFtable *, ULONG _ul_NbElems);

/*
 =======================================================================================================================
    Aim:    Return a pointer to the first element of the table
 =======================================================================================================================
 */
_inline_ TAB_tdst_PFelem *TAB_pst_PFtable_GetFirstElem(TAB_tdst_PFtable *_pst_Table)
{
	return(_pst_Table->p_Table);
}

/*
 =======================================================================================================================
    Aim:    Return a pointer to the Last used element of the table
 =======================================================================================================================
 */
_inline_ TAB_tdst_PFelem *TAB_pst_PFtable_GetLastElem(TAB_tdst_PFtable *_pst_Table)
{
	return(_pst_Table->p_NextElem - 1);
}

/*
 =======================================================================================================================
    Aim:    Return the number of holes in the P table
 =======================================================================================================================
 */
_inline_ ULONG TAB_ul_PFtable_GetNbHoles(TAB_tdst_PFtable *_pst_Table)
{
	return(_pst_Table->ul_NbHoles);
}

/*
 =======================================================================================================================
    Aim:    Return the number of non empty Elems in the table
 =======================================================================================================================
 */
_inline_ ULONG TAB_ul_PFtable_GetNbElems(TAB_tdst_PFtable *_pst_Table)
{
	return(_pst_Table->ul_NbElems);
}

/*
 =======================================================================================================================
    Aim:    Returns the maximum number of Elems of a PFtable
 =======================================================================================================================
 */
_inline_ ULONG TAB_ul_PFtable_GetMaxNbElems(TAB_tdst_PFtable *_pst_Table)
{
	return ((LONG) (_pst_Table->p_LastPointer -_pst_Table->p_Table)) +1;
}

/*
 =======================================================================================================================
    Aim:    Returns the percentage (between 0 and 1) of holes in the given PFtable (percentage from the whole table)
 =======================================================================================================================
 */
_inline_ float TAB_f_PFtable_GetHolesRatio(TAB_tdst_PFtable *_pst_Table)
{
	return fDiv(fLongToFloat(_pst_Table->ul_NbHoles), fLongToFloat(_pst_Table->ul_NbElems));
}

#define TAB_f_PFtable_GetMaxHolesRatio	TAB_f_Ptable_GetMaxHolesRatio

/*
 =======================================================================================================================
    Aim:    Makes an Elem become a hole in a pointers and flags list, increments the number of holes if necessary,
            decrements the number of Elems in the table
 =======================================================================================================================
 */
_inline_ void TAB_PFtable_SetAsHole(TAB_tdst_PFtable *_pst_Table, TAB_tdst_PFelem *_pst_Elem)
{
	ERR_X_Assert(_pst_Elem->p_Pointer != (_pst_Table->p_NextElem - 1))

	/* If the Elem is already a hole nothing is performed */
	if(!TAB_b_IsAHole(_pst_Elem->p_Pointer))
	{
		_pst_Elem->p_Pointer = (void *) 1;
		_pst_Table->ul_NbHoles++;
		_pst_Table->ul_NbElems--;
	}

	_pst_Elem->ul_Flags = 0;
}

/*
 =======================================================================================================================
    Aim:    Returns the index of the Next free Elem in a PFtable
 =======================================================================================================================
 */
_inline_ ULONG TAB_ul_PFtable_GetNextElemIndex(TAB_tdst_PFtable *_pst_Table)
{
	return(_pst_Table->p_NextElem - _pst_Table->p_Table);
}

/*
 =======================================================================================================================
    Aim:    Increments the next Elem pointer and checks (in debug only) that the list is not full
 =======================================================================================================================
 */
_inline_ void TAB_PFtable_IncNextElem(TAB_tdst_PFtable *_pst_Table)
{
	/*
	 * In debug, we check that we are not going to overpass the maximal size of the
	 * list
	 */
	ERR_X_ErrorAssert(_pst_Table->p_NextElem <= _pst_Table->p_LastPointer, TAB_Csz_Error_PFTableIsFull, NULL);

	_pst_Table->p_NextElem++;
}

/*
 =======================================================================================================================
    Aim:    Add an Elem to a table (in debug, ckecks after the add if the table is full)
 =======================================================================================================================
 */
_inline_ void TAB_PFtable_AddElem(TAB_tdst_PFtable *_pst_Table, TAB_tdst_PFelem *_p_Elem)
{
	_pst_Table->p_NextElem->p_Pointer = _p_Elem->p_Pointer;
	_pst_Table->p_NextElem->ul_Flags = _p_Elem->ul_Flags;
	_pst_Table->ul_NbElems++;

	TAB_PFtable_IncNextElem(_pst_Table);
}

/*
 =======================================================================================================================
    Aim:    Add an Elem to a table (in debug, ckecks after the add if the table is full)
 =======================================================================================================================
 */
_inline_ void TAB_PFtable_AddElemWithData(TAB_tdst_PFtable *_pst_Table, void *_p_Pointer, ULONG _ul_Flags)
{
	_pst_Table->p_NextElem->p_Pointer = _p_Pointer;
	_pst_Table->p_NextElem->ul_Flags = _ul_Flags;
	_pst_Table->ul_NbElems++;

	TAB_PFtable_IncNextElem(_pst_Table);
}

/*
 =======================================================================================================================
    Aim:    Add an element to a table and resizes the table if necessary
 =======================================================================================================================
 */
_inline_ void TAB_PFtable_AddElemAndResize(TAB_tdst_PFtable *_pst_Table, TAB_tdst_PFelem *_p_Elem)
{
	if(_pst_Table->p_NextElem > _pst_Table->p_LastPointer)
	{
		TAB_PFtable_Resize
		(
			_pst_Table,
			(_pst_Table->ul_NbElems + _pst_Table->ul_NbHoles) * 2
		);
	}

	_pst_Table->p_NextElem->p_Pointer = _p_Elem->p_Pointer;
	_pst_Table->p_NextElem->ul_Flags = _p_Elem->ul_Flags;
	_pst_Table->ul_NbElems++;
	TAB_PFtable_IncNextElem(_pst_Table);
}

/*
 =======================================================================================================================
    Aim:    Add an element to a table and resizes the table if necessary
 =======================================================================================================================
 */
_inline_ void TAB_PFtable_AddElemWithDataAndResize(TAB_tdst_PFtable *_pst_Table, void *_p_Pointer, ULONG _ul_Flags)
{
	if(_pst_Table->p_NextElem > _pst_Table->p_LastPointer)
	{
		if(_pst_Table->ul_NbHoles)
		{
			TAB_PFtable_RemoveHoles(_pst_Table);
		}

		if(_pst_Table->p_NextElem > _pst_Table->p_LastPointer)
		{
			TAB_PFtable_Resize
			(
				_pst_Table,
				(_pst_Table->ul_NbElems + _pst_Table->ul_NbHoles) * 2
			);
		}
	}

	_pst_Table->p_NextElem->p_Pointer = _p_Pointer;
	_pst_Table->p_NextElem->ul_Flags = _ul_Flags;
	_pst_Table->ul_NbElems++;
	TAB_PFtable_IncNextElem(_pst_Table);
}

/*
 =======================================================================================================================
    Aim:    Test if an Elem is within the range of a table (mainly for debug)
 =======================================================================================================================
 */
_inline_ int TAB_b_PFtable_IsInRange( const TAB_tdst_PFtable *_pst_Table, const TAB_tdst_PFelem *_p_Elem)
{
	return((_p_Elem <= _pst_Table->p_LastPointer) && (_p_Elem >= _pst_Table->p_Table));
}

/*
 =======================================================================================================================
    Aim:    Tests if a table is full
 =======================================================================================================================
 */
_inline_ int TAB_b_PFtable_isFull(const TAB_tdst_PFtable *_pst_Table)
{
	return(_pst_Table->p_LastPointer < _pst_Table->p_NextElem);
}

/*$4
 ***********************************************************************************************************************
    Functions Related to both Ptable & PFtable
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Move an element of a Ptable to a PFtable
 =======================================================================================================================
 */
_inline_ void TAB_MoveElementFromPtableToPFtable
(
	TAB_tdst_Ptable		*_pst_SrcPtable,	/* Source table */
	TAB_tdst_PFtable	*_pst_DestPFtable,	/* Destination Table */
	void				**_ppv_Element,		/* Pointer to the element */
	ULONG				_ul_Flags			/* Flags to set (copy of the identity flags) */
)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem p_Elem;
	/*~~~~~~~~~~~~~~~~~~~*/

	p_Elem.p_Pointer = *_ppv_Element;
	p_Elem.ul_Flags = _ul_Flags;
	TAB_PFtable_AddElem(_pst_DestPFtable, &p_Elem);
	TAB_Ptable_RemoveElem(_pst_SrcPtable, _ppv_Element);
}

/*
 =======================================================================================================================
    Aim:    Copy an element from a Ptable to a PFtable
 =======================================================================================================================
 */
_inline_ void TAB_CopyElementFromPtableToPFtable
(
	TAB_tdst_Ptable		*_pst_SrcPtable,	/* Source table */
	TAB_tdst_PFtable	*_pst_DestPFtable,	/* Destination Table */
	void				**_ppv_Element,		/* Pointer to the element */
	ULONG				_ul_Flags			/* Flags to set (copy of the identity flags) */
)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem p_Elem;
	/*~~~~~~~~~~~~~~~~~~~*/

#ifdef _GAMECUBE
#pragma unused(_pst_SrcPtable)
#endif

#if defined(_PC_RETAIL)
(void) _pst_SrcPtable;
#endif

	p_Elem.p_Pointer = *_ppv_Element;
	p_Elem.ul_Flags = _ul_Flags;
	TAB_PFtable_AddElem(_pst_DestPFtable, &p_Elem);
}

/*
 =======================================================================================================================
    Aim:    Move an element of a Ptable to another Ptable
 =======================================================================================================================
 */
_inline_ void TAB_MoveElementFromPtableToPtable
(
	TAB_tdst_Ptable *_pst_SrcPtable,	/* Source table */
	TAB_tdst_Ptable *_pst_DestPFtable,	/* Destination Table */
	void			**_ppv_Element		/* Pointer to the element */
)
{
	TAB_Ptable_AddElem(_pst_DestPFtable, *_ppv_Element);
	TAB_Ptable_RemoveElem(_pst_SrcPtable, _ppv_Element);
}

/*$4
 ***********************************************************************************************************************
    Functions related to link list
 ***********************************************************************************************************************
 */

void	TAB_DLlist_Create(TAB_tdst_DLlist *);
void	TAB_DLlist_Remove(TAB_tdst_DLlist *);
void	TAB_DLlist_CreateElem(TAB_tdst_DLlistElem **);
void	TAB_DLlist_RemoveElem(TAB_tdst_DLlist *, TAB_tdst_DLlistElem *);
void	TAB_DLlist_CreateAndAddElem(TAB_tdst_DLlist *, void *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ TAB_tdst_DLlistElem *TAB_pst_DLlist_GetNextElem(TAB_tdst_DLlistElem *_pst_Elem)
{
	return(_pst_Elem->pst_NextElem);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ TAB_tdst_DLlistElem *TAB_pst_DLlist_GetPrevElem(TAB_tdst_DLlistElem *_pst_Elem)
{
	return(_pst_Elem->pst_PrevElem);
}

/*$4
 ***********************************************************************************************************************
    Functions related to tables manager
 ***********************************************************************************************************************
 */

void	TAB_TablesManager_Init(void);
void	TAB_TablesManager_Close(void);
void	TAB_TablesManager_Call(void);
void	TAB_TablesManager_UnregisterTable(void *);
void	TAB_TablesManager_RegisterTable(void *);

/*$4
 ***********************************************************************************************************************
    Main Module Initialisation and Destruction
 ***********************************************************************************************************************
 */

void	TAB_InitModule(void);
void	TAB_CloseModule(void);

/*$4
 ***********************************************************************************************************************
    Debug functions
 ***********************************************************************************************************************
 */

#if defined(_DEBUG) || defined(ACTIVE_EDITORS)

/*
 =======================================================================================================================
    Check the type of a table
 =======================================================================================================================
 */
_inline_ char TAB_b_CheckType(unsigned char _uc_type)
{
	if((_uc_type == TAB_Cuc_Ptable) || (_uc_type == TAB_Cuc_PFtable))
		return 1;
	else
		return 0;
}

char	TAB_b_Check(void *);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
