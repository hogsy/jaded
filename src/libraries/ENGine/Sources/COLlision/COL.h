/*$T COL.h GC!1.41 08/11/99 15:46:14 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __COL_H__
#define __COL_H__

#ifndef PSX2_TARGET
#pragma once
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void COL_InitModule(void);
extern void COL_CloseModule(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __COL_H__ */
