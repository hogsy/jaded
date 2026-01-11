/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __LINK_H__
#define __LINK_H__

#ifdef ACTIVE_EDITORS
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

extern void LINK_InitModule(void);
extern void LINK_CloseModule(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif

#endif /* __LINK_H__ */