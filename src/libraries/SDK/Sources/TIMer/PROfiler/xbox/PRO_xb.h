/* PRO_xb.h */
/* headers for profile on xbox */


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

void XB_fn_vDisplayInfoRaster (float);
void XB_fn_vCreate (void);
void XBOX_vUpdatePerfResultMenu (void);
void XB_fn_vDisplayAvailableMemory (char *_p_szText);
void XBOX_vDisplayPerfResult (void);
void XB_fn_vDrawText (int _x, int _y, WCHAR* _lpString);
void XB_fn_vDrawColoredZoomText (int _x, int _y, ULONG color, float fZoom, WCHAR* _lpString);
void XB_fn_vBeginDrawText (void);
void XB_fn_vEndDrawText (void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
