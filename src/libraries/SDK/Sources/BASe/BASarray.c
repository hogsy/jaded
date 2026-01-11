/*$T BASarray.c GC! 1.081 04/07/00 17:33:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "BASe/BASarray.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIxxx.h"
#include "BASe/CLIbrary/CLImem.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

static BAS_tdst_Key *sgpst_Prev = NULL;
BOOL				BAS_bsortmode = TRUE;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG BAS_bsearch(ULONG key, BAS_tdst_barray *array)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	register int			num;
	register BAS_tdst_Key	*base;
	register BAS_tdst_Key	*lo;
	register BAS_tdst_Key	*hi;
	register BAS_tdst_Key	*mid;
	unsigned int			half;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	num = array->num;
	base = array->base;
	lo = base;
	hi = base + (num - 1);

	sgpst_Prev = NULL;
	while(lo <= hi)
	{
		if((half = num / 2))
		{
			mid = lo + (num & 1 ? half : (half - 1));

			if(key < mid->ul_Key)
			{
				hi = mid - 1;
				num = num & 1 ? half : half - 1;
			}
			else if(key > mid->ul_Key)
			{
				sgpst_Prev = mid;
				lo = mid + 1;
				num = half;
			}
			else
			{
				sgpst_Prev = mid - 1;
				return mid->ul_Val;
			}
		}
		else if(num)
		{
			if(key > lo->ul_Key)
			{
				sgpst_Prev = lo;
				return (ULONG) -1;
			}

			return key != lo->ul_Key ? (ULONG) - 1 : lo->ul_Val;
		}
		else
			break;
	}

	return (ULONG) -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BAS_binsert(ULONG _ul_Key, ULONG _ul_Val, BAS_tdst_barray *array)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int				num;
	BAS_tdst_Key	*base;
	BAS_tdst_Key	*tmp;
	/*~~~~~~~~~~~~~~~~~~*/

	/* Allocate array ? */
	num = array->num;
	if(array->num == array->size)
	{
		array->size += array->gran;
		if(!array->num)
		{
#ifdef ACTIVE_EDITORS
			array->base = (BAS_tdst_Key *) L_malloc(array->size * sizeof(BAS_tdst_Key));
            L_memset(array->base , 0, array->size * sizeof(BAS_tdst_Key));
#else
			array->base = (BAS_tdst_Key *) MEM_p_Alloc(array->size * sizeof(BAS_tdst_Key));
#endif
		}
		else
		{
#ifdef ACTIVE_EDITORS
			array->base = (BAS_tdst_Key *) L_realloc(array->base, array->size * sizeof(BAS_tdst_Key));
            L_memset(array->base + array->size - array->gran , 0, array->gran * sizeof(BAS_tdst_Key));
#else
			array->base = (BAS_tdst_Key *) MEM_p_Realloc(array->base, array->size * sizeof(BAS_tdst_Key));
#endif
		}
	}

	base = array->base;
	if(BAS_bsortmode)
	{
		/* Search place to insert */
		BAS_bsearch(_ul_Key, array);

		/* Was it the same key ? */
		if(array->num)
		{
			tmp = sgpst_Prev;
			if(!tmp)
				tmp = base;
			else
				tmp++;

			if((tmp != base + array->num) && (tmp->ul_Key == _ul_Key))
			{
				tmp->ul_Val = _ul_Val;
				return;
			}
		}

		/* Else insert the new key in the array */
		if(sgpst_Prev)
		{
			if(sgpst_Prev != base + num - 1)
			{
				L_memmove(sgpst_Prev + 2, sgpst_Prev + 1, ((base + num - 1) - sgpst_Prev) * sizeof(BAS_tdst_Key));
				sgpst_Prev++;
			}
			else
			{
				sgpst_Prev = base + num;
			}
		}
		else
		{
			L_memmove(base + 1, base, num * sizeof(BAS_tdst_Key));
			sgpst_Prev = base;
		}
	}
	else
	{
		sgpst_Prev = base + array->num;
	}

	/* Init the new key */
	sgpst_Prev->ul_Key = _ul_Key;
	sgpst_Prev->ul_Val = _ul_Val;
	(array->num)++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BAS_bdelete(ULONG _ul_Key, BAS_tdst_barray *array)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	int				num;
	BAS_tdst_Key	*base;
	ULONG			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	num = array->num;
	if(!num) return;
	base = array->base;

	ul_Index = BAS_bsearch(_ul_Key, array);
	if((int) ul_Index == -1) return;
	if(sgpst_Prev)
	{
		if(sgpst_Prev != base + num - 2)
		{
			L_memmove(sgpst_Prev + 1, sgpst_Prev + 2, ((base + num - 1) - sgpst_Prev) * sizeof(BAS_tdst_Key));
		}
	}
	else
	{
		L_memmove(base, base + 1, (num - 1) * sizeof(BAS_tdst_Key));
	}

	(array->num)--;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BAS_bdeletegetval(ULONG _ul_Key, BAS_tdst_barray *array, ULONG *val)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	int				num;
	BAS_tdst_Key	*base;
	ULONG			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	*val = 0;
	num = array->num;
	if(!num) return;
	base = array->base;

	ul_Index = BAS_bsearch(_ul_Key, array);
	if((int) ul_Index == -1) return;
	*val = ul_Index;

	if(sgpst_Prev)
	{
		if(sgpst_Prev != base + num - 2)
		{
			L_memmove(sgpst_Prev + 1, sgpst_Prev + 2, ((base + num - 1) - sgpst_Prev) * sizeof(BAS_tdst_Key));
		}
	}
	else
	{
		L_memmove(base, base + 1, (num - 1) * sizeof(BAS_tdst_Key));
	}

	(array->num)--;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BAS_binit(BAS_tdst_barray *array, int gran)
{
	array->base = NULL;
	array->num = 0;
	array->size = gran;
	array->gran = gran;
	if(!gran) return;
#ifdef ACTIVE_EDITORS
	array->base = (BAS_tdst_Key *) L_malloc(gran * sizeof(BAS_tdst_Key));
    L_memset(array->base, 0, gran * sizeof(BAS_tdst_Key));
#else
	array->base = (BAS_tdst_Key *) MEM_p_Alloc(gran * sizeof(BAS_tdst_Key));
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BAS_bfree(BAS_tdst_barray *array)
{
	if(array->base) 
	{
#ifdef ACTIVE_EDITORS
		L_free(array->base);
#else
		MEM_Free(array->base);
#endif
	}
	array->base = NULL;
	array->num = array->size = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if defined(_XBOX) || defined(_XENON)
int __cdecl BAS_barray_callback(const void *p1, const void *p2);
int __cdecl BAS_barray_callback(const void *p1, const void *p2)
#else
int BAS_barray_callback(const void *p1, const void *p2)
#endif
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	BAS_tdst_Key	*pp1, *pp2;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pp1 = (BAS_tdst_Key *) p1;
	pp2 = (BAS_tdst_Key *) p2;
	if(pp1->ul_Key < pp2->ul_Key) return -1;
	if(pp1->ul_Key > pp2->ul_Key) return 1;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BAS_bsort(BAS_tdst_barray *array)
{
	L_qsort((void *) array->base, array->num, sizeof(BAS_tdst_Key), BAS_barray_callback);
}

