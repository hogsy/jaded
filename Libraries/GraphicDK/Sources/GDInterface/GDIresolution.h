/*$T GDIresolution.h GC!1.67 01/03/00 13:12:03 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __GDIRESOLUTION_H__
#define __GDIRESOLUTION_H__
#include "BASe/BAStypes.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Resolution structure description
 ***********************************************************************************************************************
 */

typedef struct  GDI_tdst_Resolution_
{
    LONG    w, h, bpp, freq;
} GDI_tdst_Resolution;

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
extern LONG                 GDI_Resolution_Number;
extern GDI_tdst_Resolution  *GDI_gdst_Resolution;

#endif /* ACTIVE_EDITORS */

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
void                        GDI_Resolution_Init(void);
void                        GDI_Resolution_Close(void);
void                        GDI_Resolution_Change(int);
char                        *GDI_sz_Resolution_Desc(int);
int                         GDI_i_Resolution_GetIndex(GDI_tdst_Resolution *);
GDI_tdst_Resolution         *GDI_pst_Resolution_Get(int);
float						GDI_f_ResolutionGetFreq(void);

#else
#define GDI_Resolution_Init()
#define GDI_Resolution_Close()
#define GDI_Resolution_Change(a)
#define GDI_sz_Resolution_Desc(a)   NULL
#define GDI_i_Resolution_GetIndex(a)
#define GDI_pst_Resolution_Get(a)   NULL
#endif /* ACTIVE_EDITORS */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GDIRESOLUTION_H */
