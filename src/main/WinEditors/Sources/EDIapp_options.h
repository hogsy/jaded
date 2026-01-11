/*$T EDIapp_options.h GC!1.52 11/10/99 09:35:50 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"

extern BOOL EDI_gb_SlashQ;
#ifdef JADEFUSION
extern BOOL EDI_gb_SlashC;
#else
extern "C" BOOL EDI_gb_SlashC;
#endif
extern char	EDI_gaz_SlashC[512];
extern BOOL EDI_gb_SlashL;
#ifdef JADEFUSION
extern char	EDI_gaz_SlashL[1024];
extern BOOL EDI_gb_SlashLKTM;
#else
extern "C" char	EDI_gaz_SlashL[1024];
#endif

#endif /* ACTIVE_EDITORS */
