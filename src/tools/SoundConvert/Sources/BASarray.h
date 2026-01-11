/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once

#ifndef __BASARRAY_H__
#define __BASARRAY_H__


/*
 ---------------------------------------------------------------------------------------------------
    One element of the array
 ---------------------------------------------------------------------------------------------------
 */
typedef struct BAS_tdst_Key_
{
    ULONG   ul_Key;
    ULONG   ul_Val;
} BAS_tdst_Key;

/*
 ---------------------------------------------------------------------------------------------------
    A structure to describe a binary array
 ---------------------------------------------------------------------------------------------------
 */
typedef struct BAS_tdst_barray_
{
    BAS_tdst_Key    *base;
    int             num;
    int             size;
    int             gran;
} BAS_tdst_barray;

extern BOOL		BAS_bsortmode;
extern ULONG    BAS_bsearch(ULONG, BAS_tdst_barray *);
extern void     BAS_binsert(ULONG, ULONG, BAS_tdst_barray *);
extern void     BAS_bdelete(ULONG, BAS_tdst_barray *);
extern void     BAS_binit(BAS_tdst_barray *, int);
extern void     BAS_bfree(BAS_tdst_barray *);
extern void		BAS_bsort(BAS_tdst_barray *);


#endif /* __BASARRAY_H__ */