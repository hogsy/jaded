/*$T DYN.h GC!1.40 07/31/99 12:01:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Initialisation and close of the module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __DYN_H__
#define __DYN_H__
/*
#include "BASe/BASsys.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/DYNamics/DYNstruct.h"
#include "ENGine/Sources/DYNamics/DYNinit.h"
#include "ENGine/Sources/DYNamics/DYNconst.h"
#include "ENGine/Sources/DYNamics/DYNvars.h"
#include "ENGine/Sources/DYNamics/DYNmain.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "ENGine/Sources/WORld/WORstruct.h"
*/
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ===================================================================================================
    Initialisation and close of the module
 ===================================================================================================
 */
extern void DYN_InitModule(void);
extern void DYN_CloseModule(void);




#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __DYN_H__ */