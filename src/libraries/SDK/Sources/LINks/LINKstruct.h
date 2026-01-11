/*$T LINKstruct.h GC!1.63 12/29/99 09:36:54 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __LINKSTRUCT_H__
#define __LINKSTRUCT_H__

#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/BAStypes.h"
#include "BASe/BASarray.h"
#include "BIGfiles/BIGdefs.h"

/*$4
 ***********************************************************************************************************************
    C++
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
#include "VAVview/VAVlist.h"

#define MAX_NAME_KIT        16
#define MAX_KIT_PER_NAME    50

/*
 -----------------------------------------------------------------------------------------------------------------------
    A structure definition to register
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  LINK_tdst_Struct_
{
    int                                             i_NumFields;
    char                                            *psz_Name;
    EVAV_tdpfnv_Change                              pfn_CB;
    CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>   *po_Fields;
    int                                             i_NameKit;
    int                                             i_MaxNameKit;
    char                                            *apsz_NameKits[MAX_NAME_KIT][MAX_KIT_PER_NAME];
} LINK_tdst_Struct;

extern LINK_tdst_Struct LINK_gast_StructTypes[];
extern CMapStringToPtr  LINK_gx_TreeToPointers;
extern CMapPtrToPtr     LINK_gx_PointersToTree;
extern CMapPtrToPtr     LINK_gx_PointersJustDeleted;
extern CMapPtrToPtr     LINK_gx_PointersJustAdded;
extern CMapPtrToPtr     LINK_gx_PointersJustUpdated;
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Constant for a pointer definition
 -----------------------------------------------------------------------------------------------------------------------
 */

#define LINK_C_MaxLenNamePointer    64
#define LINK_C_MaxLenTreePointer    255

/*
 -----------------------------------------------------------------------------------------------------------------------
    The structure to describe a pointer
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  LINK_tdst_Pointer_
{
    void    *pv_Data;                           /* The real pointer */
    int     i_Type;                             /* The type of the structure */
    char    asz_Name[LINK_C_MaxLenNamePointer]; /* Name of the pointer */
    char    asz_Path[BIG_C_MaxLenPath];         /* Group of pointer (path) */
} LINK_tdst_Pointer;

/*$4
 ***********************************************************************************************************************
    C protos
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void                 LINK_RegisterStructType(int, char *, void (*) (void *, void *, void *, LONG), ...);
extern void                 LINK_ReadNameKits(void);
extern void                 LINK_DeleteNameKits(void);
extern void                 LINK_LoadKitNumbers(void);
extern void                 LINK_SaveKitNumbers(void);
extern void                 LINK_InitRegisterStructType(void);
extern void                 LINK_DelRegisterStructType(void);

extern void                 LINK_RegisterPointer(void *, int, char *, char *);
extern void                 LINK_DelRegisterPointer(void *);
extern void                 LINK_ResetRegisterPointer(void);
extern void                 LINK_UpdatePointer(void *);
extern void                 LINK_UpdatePointerAndName(void *, char *);
extern void					LINK_SetDelPointer(void *);

extern void                 LINK_RegisterDuplicate(void *, void *, ULONG );
extern LINK_tdst_Pointer    *LINK_p_SearchPointer(void *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#else /* !ACTIVE_EDITORS */
#define LINK_RegisterPointer(a, b, c, d)
#define LINK_DelRegisterPointer(a)
#define LINK_RegisterDuplicate(a, b, c)
#endif /* ACTIVE_EDITORS */

#endif /* __LINKSTRUCT_H__ */