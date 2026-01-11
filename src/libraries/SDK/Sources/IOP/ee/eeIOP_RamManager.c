/*$T eeIOP_RamManager.c GC! 1.097 01/21/02 08:56:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_TARGET


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <eekernel.h>
#include "IOP/ee/eeDebug.h"
#include "IOP/ee/eeIOP_RamManager.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    memory manager configuration
 -----------------------------------------------------------------------------------------------------------------------
 */

#define IOP_Cte_MemHoleNb		200
#define IOP_Cte_MemInitHoleRank 1
#define IOP_Cte_MemReservedHole (void *) 0x00FFFFFF
#define IOP_Cte_MemNoPrevHole	(IOP_tdst_HoleDescriptor *) - 1

/*
 =======================================================================================================================
    memory mamnager macros
 =======================================================================================================================
 */
#define IOP_M_MakeMark(__addr, __id)	(void *) ((unsigned int) __addr | (__id & 0x000000FF) << 24)
#define IOP_M_GetId(__a)				(unsigned int) (((unsigned int) __a & 0xFF000000) >> 24)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	IOP_tdst_HoleDescriptor_
{
	void							*pv_Addr;
	void							*pv_AlignAddr;
	unsigned int					ui_Size;
	struct IOP_tdst_HoleDescriptor_ *p_NextHole;
	unsigned int					ui_AllocSize;
} IOP_tdst_HoleDescriptor;

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

static void						IOP_s_InsertHole(IOP_tdst_HoleDescriptor *_p_Hole);
static IOP_tdst_HoleDescriptor	*IOP_spx_FindPrevHole(unsigned int _ui_Size);
static IOP_tdst_HoleDescriptor	*IOP_spx_CreateHole(void);
IOP_tdst_HoleDescriptor			*IOP_spx_NewHole(unsigned int _ui_Size);
IOP_tdst_HoleDescriptor			*IOP_spx_FindHole(unsigned int _ui_Size, IOP_tdst_HoleDescriptor **_pp_Prev);

/*$4
 ***********************************************************************************************************************
    public variables
 ***********************************************************************************************************************
 */

int								IOP_gi_HeapInitResult = -1;

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

static IOP_tdst_HoleDescriptor	IOP_sa_HoleArray[IOP_Cte_MemHoleNb];
static IOP_tdst_HoleDescriptor	*IOP_spx_FirstHole;
static char						*IOP_pc_EndOfMemory;
static char						*IOP_pc_EndOfAllocated;

#ifndef _FINAL_
int								IOP_gi_AllocSize = 0;
int								IOP_gi_AllocClusterSize = 0;
#endif
#define IOP_Cte_RamAlloc	(1 * 1024 * 1024)
static int						IOP_gi_MaxRamSize = 0;

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int IOP_i_InitModule(void)
{
	L_memset(IOP_sa_HoleArray, 0, IOP_Cte_MemHoleNb * sizeof(IOP_tdst_HoleDescriptor));

	IOP_spx_FirstHole = NULL;
	IOP_pc_EndOfAllocated = NULL;
	IOP_pc_EndOfMemory = NULL;
	IOP_gi_MaxRamSize = 0;

	if(IOP_gi_HeapInitResult != 0)
	{
		eeDbg_M_Err(eeDbg_Err_0031);
		return -1;
	}

	return 0; 
	
	IOP_gi_MaxRamSize = IOP_Cte_RamAlloc;
	IOP_pc_EndOfAllocated = (char *) sceSifAllocSysMemory(0, IOP_gi_MaxRamSize, NULL);

	if(!IOP_pc_EndOfAllocated)
	{
		IOP_gi_MaxRamSize = 0;
		eeDbg_M_Err(eeDbg_Err_0031);
		return -1;
	}

	IOP_pc_EndOfMemory = (char *) (IOP_pc_EndOfAllocated + IOP_gi_MaxRamSize);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void IOP_CloseModule(void)
{
	L_memset(IOP_sa_HoleArray, 0, IOP_Cte_MemHoleNb * sizeof(IOP_tdst_HoleDescriptor));

	IOP_spx_FirstHole = NULL;
	IOP_pc_EndOfAllocated = NULL;
	IOP_pc_EndOfMemory = NULL;
	IOP_gi_MaxRamSize = 0;

	IOP_gi_HeapInitResult = -1;
	if(IOP_pc_EndOfAllocated) sceSifFreeSysMemory(IOP_pc_EndOfAllocated);
	IOP_pc_EndOfAllocated = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *IOP_pv_Alloc(unsigned int _ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	IOP_tdst_HoleDescriptor *p_Hole, *p_Prev;
	unsigned int			ui_RSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!IOP_gi_MaxRamSize) return NULL;

	if(_ui_Size == 0)
	{
		eeDbg_M_Err(eeDbg_Err_002F);
		return NULL;
	}

	ui_RSize = _ui_Size;

	/* find one freed hole */
	p_Hole = IOP_spx_FindHole(ui_RSize, &p_Prev);

	/* no freed hole */
	if(p_Hole == NULL)
	{
		p_Hole = IOP_spx_NewHole(ui_RSize);
		if(p_Hole == NULL)
		{
			eeDbg_M_Err(eeDbg_Err_002E);
			return NULL;
		}
	}
	else if(p_Prev)
	{
		p_Prev->p_NextHole = p_Hole->p_NextHole;
	}
	else
	{
		IOP_spx_FirstHole = p_Hole->p_NextHole;
	}

#ifndef _FINAL_
	p_Hole->ui_AllocSize = _ui_Size;
	IOP_gi_AllocSize += _ui_Size;
	IOP_gi_AllocClusterSize += p_Hole->ui_Size;
#endif
	return p_Hole->pv_Addr;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void IOP_Free(void *_pv)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	IOP_tdst_HoleDescriptor *p_Next, *p_Freed, *p_Prev;
	unsigned int			ui_Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!IOP_gi_MaxRamSize) return;
	if(!_pv) return;

	/* get the corresponding deleted hole */
	ui_Id = IOP_M_GetId(_pv);
	p_Freed = &IOP_sa_HoleArray[ui_Id];

	/* check the hole */
	if(IOP_M_GetId(p_Freed->pv_Addr) == ui_Id)
	{
#ifndef _FINAL_
		IOP_gi_AllocSize -= p_Freed->ui_AllocSize;
		IOP_gi_AllocClusterSize -= p_Freed->ui_Size;
#endif
		p_Next = IOP_spx_FindHole(p_Freed->ui_Size, &p_Prev);

		if(p_Next && p_Prev)
		{
			p_Freed->p_NextHole = p_Next;
			p_Prev->p_NextHole = p_Freed;
		}
		else if(p_Next)
		{
			p_Freed->p_NextHole = IOP_spx_FirstHole;
			IOP_spx_FirstHole = p_Freed;
		}
		else if(p_Prev)
		{
			p_Freed->p_NextHole = NULL;
			p_Prev->p_NextHole = p_Freed;
		}
		else
		{
			p_Freed->p_NextHole = NULL;
			IOP_spx_FirstHole = p_Freed;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *IOP_pv_AllocAlign(unsigned int _ui_BlockSize, unsigned int _ui_Alignment)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	IOP_tdst_HoleDescriptor *p_Hole, *p_Prev;
	unsigned int			ui_RSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define __ALIGN 64

	if(!IOP_gi_MaxRamSize) return NULL;

	if(_ui_BlockSize == 0)
	{
		eeDbg_M_Err(eeDbg_Err_002F);
		return NULL;
	}

	ui_RSize = _ui_BlockSize + (2 * _ui_Alignment);

	/* find one freed hole */
	p_Hole = IOP_spx_FindHole(ui_RSize, &p_Prev);

	/* no freed hole */
	if(p_Hole == NULL)
	{
		p_Hole = IOP_spx_NewHole(ui_RSize);
		if(p_Hole == NULL)
		{
			eeDbg_M_Err(eeDbg_Err_002E);
			return NULL;
		}
	}
	else if(p_Prev)
	{
		p_Prev->p_NextHole = p_Hole->p_NextHole;
	}
	else
	{
		IOP_spx_FirstHole = p_Hole->p_NextHole;
	}

#ifndef _FINAL_
	p_Hole->ui_AllocSize = _ui_BlockSize;
	IOP_gi_AllocSize += _ui_BlockSize;
	IOP_gi_AllocClusterSize += p_Hole->ui_Size;
#endif
	p_Hole->pv_AlignAddr = (void *) (((unsigned int) p_Hole->pv_Addr + _ui_Alignment - 1) &~(_ui_Alignment - 1));
	return p_Hole->pv_AlignAddr;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void IOP_FreeAlign(void *_pv)
{
	if(!IOP_gi_MaxRamSize) return;
	if(!_pv) return;

	IOP_Free(_pv);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
IOP_tdst_HoleDescriptor *IOP_spx_NewHole(unsigned int _ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	IOP_tdst_HoleDescriptor *pst_Hole;
	unsigned int			ui_Id;
	unsigned int			ui_Addr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((_ui_Size) > (unsigned int) (IOP_pc_EndOfMemory - IOP_pc_EndOfAllocated)) return NULL;

	for(pst_Hole = &IOP_sa_HoleArray[0]; pst_Hole != &IOP_sa_HoleArray[IOP_Cte_MemHoleNb]; pst_Hole++)
	{
		if(pst_Hole->pv_Addr == NULL) break;
	}

	if(pst_Hole == &IOP_sa_HoleArray[IOP_Cte_MemHoleNb])
	{
		return NULL;
	}

	ui_Id = ((unsigned int) pst_Hole - (unsigned int) IOP_sa_HoleArray) / sizeof(IOP_tdst_HoleDescriptor);

	ui_Addr = (unsigned int) IOP_pc_EndOfAllocated;

	pst_Hole->pv_Addr = IOP_M_MakeMark(ui_Addr, ui_Id);
	pst_Hole->ui_Size = _ui_Size;
	pst_Hole->p_NextHole = NULL;

	IOP_pc_EndOfAllocated = (char *) (ui_Addr + _ui_Size);

	return pst_Hole;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
IOP_tdst_HoleDescriptor *IOP_spx_FindHole(unsigned int _ui_Size, IOP_tdst_HoleDescriptor **_pp_Prev)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	IOP_tdst_HoleDescriptor *p_Hole;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_pp_Prev = NULL;
	p_Hole = IOP_spx_FirstHole;
	while(p_Hole)
	{
		if(p_Hole->ui_Size >= _ui_Size) break;
		*_pp_Prev = p_Hole;
		p_Hole = p_Hole->p_NextHole;
	}

	return p_Hole;
}

#ifndef _FINAL_

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int IOP_i_GetFragmentSize(void)
{
	return IOP_Cte_RamAlloc - ((int) IOP_pc_EndOfMemory - (int) IOP_pc_EndOfAllocated);
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void IOP_FreeAll(void)
{
	if(!IOP_gi_MaxRamSize) return;
	IOP_pc_EndOfAllocated = IOP_pc_EndOfMemory - IOP_gi_MaxRamSize;
	L_memset(IOP_sa_HoleArray, 0, IOP_Cte_MemHoleNb * sizeof(IOP_tdst_HoleDescriptor));
	IOP_spx_FirstHole = NULL;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
