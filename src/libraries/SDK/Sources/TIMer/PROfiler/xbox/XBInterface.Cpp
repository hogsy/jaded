//-----------------------------------------------------------------------------
// File: XBFont.cpp
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <stdio.h>
#include <d3d8.h>
#include "XBFont.h"
#include <XbDm.h>
#include <d3d8perf.h>
#include "GX8/Gx8init.h"
#include "GX8/Gx8renderstate.h"
#include "pro_xb.h"
#include "Sdk\Sources\TIMer\PROfiler\PROdefs.h"
#include "BASe\BENch\BENch.h"

CXBFont							g_Font;// Font class

extern "C" Gx8_tdst_SpecificData *p_gGx8SpecificData;

float       g_fRefreshRate;

#ifdef XBOX_TUNING
extern void XBOX_vDisplayPerfResult( CXBFont* pFont );
#endif // XBOX_TUNING

void XB_fn_vDisplayInfoRaster(float _fLastFrameActualLength)
{
//#ifdef _RETAIL_SHOWS_FRAMERATE_
	float fps;
	WCHAR Text[256];

	fps = g_fRefreshRate /_fLastFrameActualLength;
	swprintf(Text, L"%2.2f tr ; %0.2f Fps", _fLastFrameActualLength, fps);
	g_Font.DrawText(55, 50,0xff101010, Text);
//#endif // _RETAIL_SHOWS_FRAMERATE_

#ifdef XBOX_TUNING
	XBOX_vDisplayPerfResult(&g_Font);
//	D3DPERF_SetShowFrameRateInterval(10000);
#endif // XBOX_TUNING
}


void XB_fn_vBeginDrawText (void)
{
	g_Font.Begin();
}

void XB_fn_vDrawText (int _x, int _y, WCHAR* _lpString)
{
	g_Font.DrawText ((float)_x, (float)_y, 0xffff00ff, _lpString);
}

void XB_fn_vDrawColoredZoomText (int _x, int _y, ULONG color, float fZoom, WCHAR* _lpString)
{
	// RFE 2003-01-10 left alignement for string of the debug menu
	//g_Font.DrawText ((float)_x, (float)_y, color, _lpString, XBFONT_LEFT,fZoom);
	g_Font.SetScaleFactors(fZoom,fZoom);
	g_Font.DrawText ((float)_x, (float)_y, color, _lpString, XBFONT_LEFT,NULL);
}

void XB_fn_vEndDrawText (void)
{
	g_Font.End();
}

void XB_fn_vCreate( void )
{
#if defined (RASTERS_ON) || defined (XBOX_TUNING) || defined (GX8_BENCH)
#ifndef _FINAL_  
	D3DDISPLAYMODE stDisplayModeMode;

    //g_Font.Create(p_gGx8SpecificData->mp_D3DDevice , "Font.xpr" );
    g_Font.Create( "Font.xpr" );
	IDirect3DDevice8_GetDisplayMode(p_gGx8SpecificData->mp_D3DDevice, &stDisplayModeMode);
    g_fRefreshRate = (float)stDisplayModeMode.RefreshRate;
#endif //_FINAL_
#ifdef XBOX_TUNING
//	DmEnableGPUCounter(TRUE);
#endif // XBOX_TUNING

#endif // RASTERS_ON || XBOX_TUNING
}

#ifdef XBOX_TUNING
char g_cTextInfo[256];

void XB_fn_vDisplayAvailableMemory(char *_p_szText)
{
    MEMORYSTATUS	stat;
	char			cBuffer[256];

    GlobalMemoryStatus( &stat );

	sprintf(cBuffer, " ================> %s : %1.3f MB \n",_p_szText, ((float)(stat.dwAvailPhys / (1024.f * 1024.0f))));
	OutputDebugString(cBuffer);
}
#endif
