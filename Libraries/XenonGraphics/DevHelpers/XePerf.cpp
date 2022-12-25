#include "Precomp.h"

#if !defined(_FINAL) && (defined(_XENON) || defined(_XENON_PROFILE))

//#if defined(__cplusplus)
//extern "C"
//{
//#endif

#include "XenonGraphics/DevHelpers/XePerf.h"

tdstPIXOutputInfo g_oPIXOutputInfo[] =
{
    { 0xff000000, "Xe_Stub" },
/*  { 0xffff8080, "Xe_FrustumCulling"},    // FrustumCulling
    { 0xff8080f0, "Xe_Render All GO"},     // RenderAllGO
    { 0xffff4040, "Xe_GO"},                // GO
    { 0xff00ff00, "Xe_Modifier ApplyGao"}, // ModifierApplyGAO
    { 0xfff0fff0, "Xe_RenderAllLightmaps"}, // RenderAllLightmaps,
    { 0xfffff0f0, "Xe_RenderZList"},        // RenderZList
    { 0xff008080, "Xe_ShadowGeneration"},   // ShadowGeneration
    { 0xffff00ff, "Xe_IndexedTriangles"},   // IndexedTriangles
    { 0xff8000ff, "Xe_SetTextureBlending"}, // SetTextureBlending
    { 0xff0080FF, "Xe_GEO"},                // GEO
    { 0xffFF8000, "Xe_GEO with skinning"},  // GEOSkinning
    { 0xff808000, "Xe_SendObjectToLight"},  // SendObjectToLight
    { 0xffffff00, "Xe_SkinningCompute"},    // SkinningCompute
    { 0xffffff00, "Xe_NormalRecompute"},    // NormalRecompute
    { 0xffFfff80, "Xe_RenderAllShadow"},    // RenderAllShadow
    { 0xffD0D0ff, "Xe_PAG Render"},         // PAG
    { 0xffFF0000, "Xe_OneWorldEngineCall"}, // OneWorldEngineCall
    { 0xff00ff00, "Xe_Smart Blur"},         // SmartBlur
    { 0xffff8080, "Xe_OnduleTonCorps"},     // OnduleTonCorps
    { 0xff80ffff, "Xe_SpriteGenerate"},     // SpriteGenerate
    { 0xff8080ff, "Xe_DrawIndexedSprite"},  // DrawIndexSprite
    { 0xff888888, "Xe_Scene"},              // Scene
    { 0xffffffff, "Xe_MainUnApplyGen"},     // MainUnApplyGen
    { 0xffffffff, "Xe_BlendingAndBonesHier"}, // BlendingAndBonesHier
    { 0xffffffff, "Xe_ApplyAllGaoInEngineCall"}, // ApplyAllGaoInEngineCall
    { 0xffffffff, "Xe_AI_ExecCallbackAll"}, // AI_ExecCallbackAll
    { 0xffffffff, "Xe_HierarchyMainCall"},  // HierarchyMainCall
    { 0xffffffff, "Xe_Sticking"},           // Sticking
    { 0xffffffff, "Xe_Collision"},          // Collision
    { 0xffffffff, "Xe_AI"},                 // AI
    { 0xffffffff, "Xe_ANI"},                // ANI
    { 0xffffffff, "Xe_MainApplyGen"},       // MainApplyGen
	{ 0xff1111ff, "Xe_RenderWater"},		 // RenderWater
	{ 0xffff1111, "Xe_LoadBackBufferTexture"},// Back buffer texture loading for water effect
	{ 0xffff1111, "Xe_ApplyWaterModifier"},	 // Water Modifier
	{ 0x1111ffff, "Xe_FillVertexBuffer"},	 // Fill Vertex Buffer
	{ 0x77777777, "Xe_ComputeWaterZandNormal"}, // Computing water z and normals
	{ 0xFF444477, "Xe_HaloRender"},	        // Render the halo sprites
    { 0xffffffff, "Xe_Glow"},              // Glow
    { 0xff00ff00, "Xe_SmoothZoom"},        // SmoothZoom
    { 0xff4080ff, "Xe_ColorDiffusion"},    // ColorDiffusion
	{ 0xff4080ff, "Xe_Push Buffer Recording"}, // push buffer recording
	{ 0xff4f8fff, "Xe_Portal Culling"},    // PortalCulling
	{ 0xff80FF80, "Xe_HierarchyPostEngine"}, // HierarchyPostEngine
	{ 0xff808080, "Xe_Color Burn"},        // ColorBurn
	{ 0xff123456, "Xe_Color Highlight"},   // ColorHighlight
	{ 0xff654321, "Xe_Monochromatic"},     // Monochromatic
	{ 0xff222222, "Xe_Brightness"}, 
    { 0xff00ff00, "Xe_XMen"},              // XMen effect
	{ 0xff00ff00, "Xe_Overlay"},              // XMen effect*/
};

#if defined(_XENON_PROFILE)
ULONG          g_ulXEProfileIndex;
XEProfileStack g_oXEProfileStack[XE_PROFILE_STACK_DEPTH];
XEProfileNode  g_oXEProfileNodes[XE_PROFILE_MAX_NODES];

inline void XEGetTicks(ULONG64* _i64Ticks)
{
    // RDTSC
    __asm
    {
        mov    edi, _i64Ticks
        __emit  0fh
        __emit 031h
        mov    dword ptr [edi], eax
        mov    dword ptr [edi + 4], edx
    }
}

inline void XEBeginProfile(const CHAR* _szName)
{
    ULONG i;

    for (i = 0; i < XE_PROFILE_MAX_NODES; ++i)
    {
        if (g_oXEProfileNodes[i].szName == _szName)
            break;

        if (g_oXEProfileNodes[i].szName == NULL)
        {
            g_oXEProfileNodes[i].szName = _szName;
            break;
        }
    }

    if ((i == XE_PROFILE_MAX_NODES) || (g_ulXEProfileIndex == (XE_PROFILE_STACK_DEPTH - 1)))
    {
        __asm int 03h;
        return;
    }

    if (i != g_oXEProfileStack[g_ulXEProfileIndex].ulCurrentNode)
    {
        ++g_ulXEProfileIndex;

        g_oXEProfileStack[g_ulXEProfileIndex].ulRecursion   = 0;
        g_oXEProfileStack[g_ulXEProfileIndex].ulCurrentNode = i;
        XEGetTicks(&g_oXEProfileStack[g_ulXEProfileIndex].ul64StartTick);
    }
    else
    {
        ++g_oXEProfileStack[g_ulXEProfileIndex].ulRecursion;
    }
}

inline void XEEndProfile(void)
{
    ULONG64 ul64CurTicks;

    if (g_ulXEProfileIndex == 0)
    {
        return;
    }

    if (g_oXEProfileStack[g_ulXEProfileIndex].ulRecursion == 0)
    {
        XEGetTicks(&ul64CurTicks);

        g_oXEProfileNodes[g_oXEProfileStack[g_ulXEProfileIndex].ulCurrentNode].ul64TotalTicks += ul64CurTicks - g_oXEProfileStack[g_ulXEProfileIndex].ul64StartTick;
        ++g_oXEProfileNodes[g_oXEProfileStack[g_ulXEProfileIndex].ulCurrentNode].ulNbTimes;

        g_oXEProfileStack[g_ulXEProfileIndex].ulCurrentNode = 0xffffffff;

        --g_ulXEProfileIndex;
    }
    else
    {
        ++g_oXEProfileNodes[g_oXEProfileStack[g_ulXEProfileIndex].ulCurrentNode].ulNbTimes;
        --g_oXEProfileStack[g_ulXEProfileIndex].ulRecursion;
    }
}

void XEInitializeProfile(void)
{
    ULONG i;

    for (i = 0; i < XE_PROFILE_MAX_NODES; ++i)
    {
        g_oXEProfileNodes[i].szName         = NULL;
        g_oXEProfileNodes[i].ul64TotalTicks = 0;
        g_oXEProfileNodes[i].ulNbTimes      = 0;
    }

    for (i = 0; i < XE_PROFILE_STACK_DEPTH; ++i)
    {
        g_oXEProfileStack[i].ulCurrentNode = 0xffffffff;
    }

    g_ulXEProfileIndex = 0;
}

void XEShutdownProfile(void)
{
}

void XEResetProfile(void)
{
    ULONG i;

    for (i = 0; i < XE_PROFILE_MAX_NODES; ++i)
    {
        g_oXEProfileNodes[i].ul64TotalTicks = 0;
        g_oXEProfileNodes[i].ulNbTimes      = 0;
    }

    for (i = 0; i < XE_PROFILE_STACK_DEPTH; ++i)
    {
        g_oXEProfileStack[i].ulCurrentNode = 0xffffffff;
    }

    g_ulXEProfileIndex = 0;
}

static inline FLOAT XEGetTickFrequency(void)
{
    static FLOAT fFrequency = -1.0f;

    if (fFrequency == -1.0f)
    {
        LARGE_INTEGER liPerfFrequency;
        LARGE_INTEGER liStartTime;
        LARGE_INTEGER liEndTime;
        ULONG64       i64StartTick;
        ULONG64       i64EndTick;

        QueryPerformanceFrequency(&liPerfFrequency);
        QueryPerformanceCounter(&liStartTime);
        XEGetTicks(&i64StartTick);

        // Waiste some time
        volatile ULONG i;
        for (i = 0; i < 1000000; ++i)
        {
        }

        XEGetTicks(&i64EndTick);
        QueryPerformanceCounter(&liEndTime);

        fFrequency = ((FLOAT)(i64EndTick - i64StartTick)  * 
                      (FLOAT)liPerfFrequency.QuadPart / 
                      (FLOAT)(liEndTime.QuadPart - liStartTime.QuadPart));
    }

    return fFrequency;
}

void XEDumpProfile(void)
{
    SYSTEMTIME oSysTime;
    CHAR       szFileName[256];
    FILE*      pFile = NULL;
    ULONG      i;
    FLOAT      fFreq = XEGetTickFrequency();

    GetLocalTime(&oSysTime);
    sprintf(szFileName, "Capture/XeProfile_%d-%02d-%02d_%02dh%02dm%02ds%04dms.LOG", 
                        (LONG)oSysTime.wYear, (LONG)oSysTime.wMonth, (LONG)oSysTime.wDay,
                        (LONG)oSysTime.wHour, (LONG)oSysTime.wMinute, (LONG)oSysTime.wSecond, (LONG)oSysTime.wMilliseconds);

    pFile = fopen(szFileName, "wt");
    if (pFile != NULL)
    {
        for (i = 0; i < XE_PROFILE_MAX_NODES; ++i)
        {
            if (g_oXEProfileNodes[i].szName == NULL)
                break;

            if (g_oXEProfileNodes[i].ulNbTimes == 0)
                continue;

            fprintf(pFile, "%s\t%I64u\t%f\t%u\t%f\n", g_oXEProfileNodes[i].szName, 
                                                      g_oXEProfileNodes[i].ul64TotalTicks, 
                                                      (FLOAT)g_oXEProfileNodes[i].ul64TotalTicks / fFreq,
                                                      g_oXEProfileNodes[i].ulNbTimes,
                                                      ((FLOAT)g_oXEProfileNodes[i].ul64TotalTicks / fFreq) / (FLOAT)g_oXEProfileNodes[i].ulNbTimes);
        }

        fclose(pFile);
    }
}

#endif

void PIX_XeBeginEvent(ePIXEventType eType)
{
	if (eType != PIX_Evt_Stub)
	{
#if defined(_XENON)
		PIXBeginNamedEvent(g_oPIXOutputInfo[eType].m_dwColor, g_oPIXOutputInfo[eType].m_szText);
#else
        XEBeginProfile(g_oPIXOutputInfo[eType].m_szText);
#endif
	}
}

void PIX_XeBeginEventSpecific(D3DCOLOR iCol, const char* poString)
{
#if defined(_XENON)
	PIXBeginNamedEvent(iCol, poString);
#else
    XEBeginProfile(poString);
#endif
}

void PIX_XeEndEvent(ePIXEventType eType)
{
	if (eType != PIX_Evt_Stub)
	{
#if defined(_XENON)
		PIXEndNamedEvent();
#else
        XEEndProfile();
#endif
	}
}

void PIX_XeEndEventSpecific(D3DCOLOR iCol)
{
#if defined(_XENON)
	PIXEndNamedEvent();
#else
    XEEndProfile();
#endif
}

//#if defined(__cplusplus)
//}
//#endif

#endif // !defined(_FINAL) && (defined(_XENON) || defined(_XENON_PROFILE))
