/*$T PROdisplay.h GC!1.67 01/03/00 11:45:38 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __PRODISPLAY_H__
#define __PRODISPLAY_H__


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


struct                              PRO_tdst_TrameRaster_;
extern struct PRO_tdst_TrameRaster_ *mpst_CurrentCategory;
extern struct PRO_tdst_TrameRaster_ *mpst_CurrentSubCategory;
extern struct PRO_tdst_TrameRaster_ *mpst_CurrentName;

#if (defined (PSX2_TARGET) || defined (_GAMECUBE) || defined (_XBOX) || defined(_XENON))
extern int                          PRO_OneTrameEnding(int i_Y);
#else
extern int                          PRO_OneTrameEnding(HWND hwnd, HDC _hdc, int i_Y);
#endif



#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __PRODISPLAY_H__ */
