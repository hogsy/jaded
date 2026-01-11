/*$T BAS.h GC!1.41 08/17/99 17:38:58 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __BAS_H__
#define __BAS_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void BAS_InitModule(void);
extern void BAS_CloseModule(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __BAS_H__ */ 
 