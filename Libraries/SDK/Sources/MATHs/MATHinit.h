/*$T MATHinit.h GC!1.32 05/18/99 15:33:21 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Functions called by main initialisation (MATH_InitModule, dans MATH.h)
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

void    MATH_CreateSqrtTable(void);
void    MATH_CreateInvTable(void);
void    MATH_CreateInvSqrtTable(void);
void    MATH_CreateTrigTables(void);
