/*$T WOR.h GC!1.39 06/28/99 10:44:39 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Initialisation and close of the module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __WORD_H__
#define __WORD_H__

#include "ENGine/Sources/WORld/WORconst.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORinit.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ===================================================================================================
    Initialisation and close of the module
 ===================================================================================================
 */
extern void WOR_InitModule(void);
extern void WOR_CloseModule(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __WORD_H__ */