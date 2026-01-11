/*$T TEXT.h GC!1.52 12/06/99 09:48:47 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __TEXT_H__
#define __TEXT_H__

#include "TEXTstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

extern TEXT_tdst_AllText   TEXT_gst_Global;
extern void TEXT_InitModule(void);
extern void TEXT_CloseModule(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __AI_H__ */ 
 