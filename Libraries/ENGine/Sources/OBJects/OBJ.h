/*$T OBJ.h GC!1.37 05/21/99 12:05:22 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */



#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __OBJ_H__
#define __OBJ_H__
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJinit.h"


/*
 ===================================================================================================
    Aim:    Initialisation of the World module
 ===================================================================================================
 */
extern void OBJ_InitModule(void);
extern void OBJ_CloseModule (void);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __OBJ_H__ */