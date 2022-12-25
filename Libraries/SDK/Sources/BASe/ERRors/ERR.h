/*$T ERR.h GC!1.41 08/17/99 17:37:26 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __ERR_H__
#define __ERR_H__

#include "BASe/ERRors/ERRasser.h"
#include "BASe/ERRors/ERRdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void ERR_InitModule(void);
extern void ERR_CloseModule(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __ERR_H__ */ 
 