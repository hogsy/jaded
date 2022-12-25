/*$T WAY.h GC!1.52 10/25/99 09:55:10 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __WAY_H__
#define __WAY_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" {
#endif

extern BAS_tdst_barray  WAY_gst_Seen;
extern BAS_tdst_barray  WAY_gst_SeenDist;
extern void             WAY_InitModule(void);
extern void             WAY_CloseModule(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WAY_H__ */
