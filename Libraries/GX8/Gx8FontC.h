#if !defined( GX8_FONT_C_H )
#  define GX8_FONT_C_H

#include <xtl.h>

#ifdef __cplusplus
extern "C" {
#endif

	void Gx8CreateFont(void);
	void WriteString(WCHAR *pStr, float posx, float posy, DWORD color);
	void WriteStringEx(WCHAR *pStr, float posx, float posy, DWORD color);
	void GetTextExtent( WCHAR const* strText, FLOAT* pWidth, FLOAT* pHeight, BOOL bFirstLineOnly );

#ifdef __cplusplus
}
#endif


#endif