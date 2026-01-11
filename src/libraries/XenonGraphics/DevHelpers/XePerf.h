#ifndef INC_XEPERF_H
#define INC_XEPERF_H

#if !defined(_FINAL) && (defined(_XENON) || defined(_XENON_PROFILE))

#if defined(_XENON)
#include <xtl.h>
#endif

typedef enum ePIXEventType_
{
	PIX_Evt_Stub               = 0,
/*  PIX_Evt_FrustumCulling     = 1,
	PIX_Evt_RenderAllGO        = 2,
	PIX_Evt_GO                 = 3,
	PIX_Evt_ModifierApplyGAO   = 4,
	PIX_Evt_RenderAllLightmaps = 5,
	PIX_Evt_RenderZList        = 6,
	PIX_Evt_ShadowGeneration   = 7,
	PIX_Evt_IndexedTriangles   = 0,
	PIX_Evt_SetTextureBlending = 0,
	PIX_Evt_GEO                = 10,
	PIX_Evt_GEOSkinning        = 11,
	PIX_Evt_SendObjectToLight  = 12,
	PIX_Evt_SkinningCompute    = 13,
	PIX_Evt_NormalRecompute    = 14,
	PIX_Evt_RenderAllShadow    = 15,
	PIX_Evt_PAG                = 16,
	PIX_Evt_OneWorldEngineCall = 17,
	PIX_Evt_SmartBlur          = 18,
	PIX_Evt_OnduleTonCorps     = 19,
	PIX_Evt_SpriteGenerate     = 20,
	PIX_Evt_DrawIndexedSprite  = 21,
	PIX_Evt_Scene              = 0,
	PIX_Evt_MainUnApplyGen     = 23,
	PIX_Evt_BlendingAndBonesHier    = 24,
	PIX_Evt_ApplyAllGaoInEngineCall = 25,
	PIX_Evt_AI_ExecCallbackAll = 26,
	PIX_Evt_HierarchyMainCall  = 27,
	PIX_Evt_Sticking           = 28,
	PIX_Evt_Collision          = 29,
	PIX_Evt_AI                 = 30,
	PIX_Evt_ANI                = 31,
	PIX_Evt_MainApplyGen       = 32,
	PIX_Evt_RenderWater		   =33 ,
	PIX_Evt_LoadBackBufferTexture = 34,
	PIX_Evt_ApplyWaterModifier =35,
	PIX_Evt_FillVertexBuffer = 36,
	PIX_Evt_ComputeWaterZandNormal = 37,
	PIX_Evt_HaloRender = 38,
	PIX_Evt_Glow = 39,
	PIX_Evt_SmoothZoom = 40,
	PIX_Evt_ColorDiffusion = 41,
	PIX_Evt_RecordPushBuffer = 42,
	PIX_Evt_PortalCulling = 43,
	PIX_Evt_HierarchyPostEngine = 44,
	PIX_Evt_ColorBurn = 45,
	PIX_Evt_ColorHighlight = 46,
	PIX_Evt_Monochromatic = 47,
	PIX_Evt_Brightness = 48,
	PIX_Evt_Xmen = 49,
	PIX_Evt_Overlay = 50,*/
	PIX_Evt_Num
} ePIXEventType;

#if defined(_XENON_PROFILE)

#define XE_PROFILE_MAX_NODES   2048
#define XE_PROFILE_STACK_DEPTH 16384

struct XEProfileNode
{
    const CHAR* szName;
    ULONG64     ul64TotalTicks;
    ULONG       ulNbTimes;
};

struct XEProfileStack
{
    ULONG   ulCurrentNode;
    ULONG64 ul64StartTick;
    ULONG   ulRecursion;
};

void XEInitializeProfile(void);
void XEShutdownProfile(void);
void XEResetProfile(void);
void XEDumpProfile(void);

#endif

typedef struct tdstPIXOutputInfo_
{
	D3DCOLOR m_dwColor;
	const char* m_szText;
} tdstPIXOutputInfo;

extern tdstPIXOutputInfo g_oPIXOutputInfo[];
void PIX_XeBeginEvent(ePIXEventType eType);
void PIX_XeBeginEventSpecific(D3DCOLOR iCol, const char* poString);
void PIX_XeEndEvent(ePIXEventType eType);
void PIX_XeEndEventSpecific(D3DCOLOR iCol);

#define PIX_XePerf_BeginEvent(a) PIX_XeBeginEvent(a)
#define PIX_XePerf_BeginEventSpecific(a, b) PIX_XeBeginEventSpecific(a, b)
#define PIX_XePerf_EndEvent(a)   PIX_XeEndEvent(a)
#define PIX_XePerf_EndEventSpecific(a)   PIX_XeEndEventSpecific(a)

#pragma message("Using PIX performance tracker")
#else

#define PIXPerf_EndEvent(a)
#define PIXPerf_BeginEvent(a)

#define PIX_XeBeginEventSpecific(a, b)
#define PIX_XeEndEventSpecific(a)

#endif //#if !defined(_FINAL) && (defined(_XENON) || defined(_XENON_PROFILE))

#if defined(_XENON_PROFILE)

#define XEProfileScope(_name_) XEScopeProfiler oScopeProfiler(_name_)
#define XEProfileFunction()    XEScopeProfiler oFunctionProfiler(__FUNCTION__)
#define XEProfileStart(_name_) PIX_XeBeginEventSpecific(0, _name_)
#define XEProfileEnd()         PIX_XeEndEventSpecific(0)

// ----------------------------------------------------------------------------
// CLASSES
// ----------------------------------------------------------------------------
class XEScopeProfiler
{
public:

    inline XEScopeProfiler(const char* _szName)
    {
        XEProfileStart(_szName);
    }

    inline ~XEScopeProfiler(void)
    {
        XEProfileEnd();
    }
};

#else

#define XEProfileScope(_name_)
#define XEProfileFunction()
#define XEProfileStart(_name_)
#define XEProfileEnd()

#endif

#endif // INC_PIXPerf_H
