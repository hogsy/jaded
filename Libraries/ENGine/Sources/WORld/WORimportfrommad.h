/*$T WORimportfrommad.h GC!1.52 12/02/99 16:30:10 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Initialisations du module World
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS
#pragma once
#include "BASe/BAStypes.h"
#include "BIGfiles/BIGkey.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Globals
 ***************************************************************************************************
 */

extern LONG WOR_gl_ImportForced;
extern LONG WOR_gl_ImportAutomatic;
extern LONG WOR_gl_ImportGO;
extern LONG WOR_gl_ImportGrm;
extern LONG WOR_gl_ImportGro;


/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

BOOL        WOR_b_World_Import(char *, char *, char *);
void        WOR_Import_ResetFlags( void );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* ACTIVE_EDITORS */
