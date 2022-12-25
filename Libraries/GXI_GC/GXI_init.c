#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "GXI_init.h"
#include "GXI_tex.h"
#include "GXI_font.h"
#include "GXI_render.h"
#include "GXI_shadows.h"
#include "GXI_vertexspace.h"
#include "GXI_objectlist.h"
#include "GXI_AfterFX.h"
#include "GXI_SpecialFX.h"
#include "GXI_bench.h"
#include "GXI_rasters.h"
#include "GEOmetric/GEO_STRIP.h"
#include <dolphin/mtx/mtx44ext.h>
#include "GXI_dbg.h"
#include "GraphicDK\Sources\SOFT\SOFTzlist.h"


#include "BAse/BENch/BENch.h"



#define GXI_OverScanHeight 16
#define GXI_OverScanHeightHalfFrame 32

#ifdef FALSE_640
#define GXI_WIDTH 640
#define SCREEN_DIFF 128
#else
#define GXI_WIDTH 512
#define SCREEN_DIFF 0
#endif

#define GXI_DefaultFIFOSize				(256 * 1024)

// PAL -> max height 528.

#define GXI_MaxBufferHeight (528 - GXI_OverScanHeight*2)
#define GXI_MaxHalfBufferHeight (528/2 - GXI_OverScanHeightHalfFrame)

#define GXI_MaxExternalFrameBufferSize		((GXI_WIDTH*GXI_MaxBufferHeight*2) + 64 & ~63) 
#define GXI_MaxExternalHalfFrameBufferSize	((GXI_WIDTH*GXI_MaxHalfBufferHeight*2) + 64 & ~63) 

//static ULONG GXI_MaxExternalFrameBufferSize = 0;
//static ULONG GXI_MaxExternalHalfFrameBufferSize = 0;


// gamecube render specifics ----------------------------------------------

extern void GXI_Rasters();
void GXIInitGXWithRenderMode(GXRenderModeObj *pmode,BOOL _bInvertField);
void GXI_AllocatePlayModeMem();

extern ULONG GC_ul_ValidFrameBufferNb;

// Pointer to current rendering mode
GXRenderModeObj *g_pst_mode;

// Rendering modes.
GXRenderModeObj  g_st_DefaultMode;
GXRenderModeObj  g_st_HalfFrameMode;

float WIDE_SCREEN_ADJUST = 56.0f;

// Do we use one or two frame buffers ? (if we use one, it must not change)
BOOL GXI_gbOneBufferMode = FALSE;

// Are the frame buffer full size ? (half size frame buffers need one refresh per frame).
BOOL GXI_gbFullFrameMode = TRUE;

// usefull colors definition
GXColor GX_WHITE = {255,255,255,255};
GXColor GX_BLACK = {0,0,0,0};

#ifdef USE_PERPECTIVE_CORRECT_COLORS
GXColor GX_RG= {255,255,0,0};
GXColor GX_BA= {0,0,255,255};
#endif



GXI_HW_States g_GXI_HW_States;


char 				*DefaultFifo;
GXFifoObj*  	  DefaultFifoObj;
SOFT_tdst_ComputingBuffers 		*SOFT_gp_Compute;
SOFT_tdst_ZList 				*SOFT_gst_ZList;
SOFT_tdst_ZList_CommonParrams 	*SOFT_gst_ZList_CP;	
char 							*g_pstFrameBuffer1;
char 							*g_pstFrameBuffer2;

extern u32  *p_AfterFXBuffer;
extern u32  *p_AfterFXBuffer_ZSR;


BOOL 		g_bFirstTimeInit=TRUE;

#ifdef USE_PERPECTIVE_CORRECT_COLORS
u16 		g_RampTex16Data[256*256*2] ATTRIBUTE_ALIGN(32);
GXTexObj 	g_RampTex16;
#endif

GXTexObj g_reflectionDepthTex;

GXI_BigStruct gs_st_Globals;

#ifdef USE_MY_TEX_REGION
	extern GXTexRegion  MyTexRegions[8]; // cache regions
#endif

#ifndef USE_TRIPPLE_BUFFERING
//#define GXI_PERF
#endif

#ifdef GXI_PERF
static OSStopwatch GXI_PERF_TotTime;
#endif

#ifdef USE_TRIPPLE_BUFFERING

//--------------------------------------------------------------------------

static volatile u32 nbFramesInWait = 0;
u_int ul_GC_frameCounter = 0;


static void* myXFB1;    // Pointers to the two XFB's
static void* myXFB2;
static void* copyXFB;   // Which XFB to copy to next
static void* dispXFB;   // Which XFB is being displayed now

static volatile u8 g_bSwitchFrames = 0; // >0 if ready to switch to the next xFB

static volatile u8 g_bSwitched = 0; // >0 if ready to switch to the next xFB

// Set to TRUE when frame buffers are not valid.
static volatile u32 GXI_ulBlackFrameNb = 0;

/*---------------------------------------------------------------------------*
   VI Pre Callback (VBlank interrupt)

   The VI Pre callback should be kept minimal, since the VI registers
   must be set before too much time passes.  Additional bookkeeping is
   done in the VI Post callback.

 *---------------------------------------------------------------------------*/
void VIPreCallback ( u32 retraceCount )
{
   // extern u32      	*p_VideoTextureBuffer;
    u_int Test;
    /*if (p_VideoTextureBuffer != NULL)
    {
    	if ((retraceCount & 1))
	    	Test = g_bSwitchFrames;
	    else
	    	Test = 0;
    }
    else*/
    	Test = g_bSwitchFrames;
    if (Test)
    {
    	ERR_X_Assert(myXFB2 && myXFB1);
        dispXFB = (dispXFB == myXFB1) ? myXFB2 : myXFB1;
		
		if (GXI_ulBlackFrameNb > 0)
		{
			GXI_ulBlackFrameNb--;
			if (!GXI_ulBlackFrameNb)
			{
				VISetBlack(FALSE);
			}
		}

        VISetNextFrameBuffer( dispXFB );

        VIFlush();
       
        g_bSwitchFrames = FALSE; 
        g_bSwitched = TRUE;
    }
    else g_bSwitched = FALSE;
}

/*---------------------------------------------------------------------------*
   VI Post Callback (VBlank interrupt)
 *---------------------------------------------------------------------------*/
#ifndef NO_BINK
extern OSThread* g_pBinkThread;
#endif // NO_BINK
void VIPostCallback ( u32 retraceCount )
{
    #pragma unused (retraceCount)
		
	if(g_bSwitched)
    {
#ifndef USE_TRIPPLE_BUFFERING    
	    GXI_EndRaster(GXI_NRaster);
#ifndef _FINAL_
		RastersSmooth[GXI_NRaster] = RastersSmooth[GXI_NRaster] * AR_SMOOTH + Rasters[GXI_NRaster] * (1.0f - AR_SMOOTH);
		Rasters[GXI_NRaster] = 0;
#endif		
#endif
    	g_bSwitchFrames = 0;
    	if(nbFramesInWait)
    		nbFramesInWait--;
	    g_bSwitched = FALSE;	

	  
#ifndef USE_TRIPPLE_BUFFERING    	    
	    GXI_BeginRaster(GXI_NRaster);    
#endif	    
    }
#ifndef NO_BINK    
    if (g_pBinkThread && !OSIsThreadTerminated(g_pBinkThread))
    {
  		OSResumeThread(g_pBinkThread);
    }
#endif // NO_BINK
}

/*---------------------------------------------------------------------------*
   DrawDone Callback (Ready for next frame)
 *---------------------------------------------------------------------------*/

void DrawDoneCallback()
{
#ifndef _FINAL_
	if(!nbFramesInWait)
	{
	OSReport("WAH\n");
	}
#endif	
	g_bSwitchFrames = 1;	
}


#else
#endif

#ifdef USE_TRIPPLE_BUFFERING    
inline
void GXI_WaitFrameEnd()
{
	//bool old;
	while( nbFramesInWait>0 )
	{
		//old = OSEnableInterrupts();
		//OSYieldThread();		
		//OSRestoreInterrupts(old);
	}
}
#endif		

void GXI_StopFlip(BIG_KEY _ul_WorldFileKey) // == BeginWorldLoad
{
    // Wait for GPU to be done (so that we can display something else).
#ifdef USE_TRIPPLE_BUFFERING    
	GXI_WaitFrameEnd();
#endif		

	// Just to be sure ...
	VIWaitForRetrace();
	VIWaitForRetrace();

	// Display loading bink (optional).    	
    GXI_StartBinkLoad(_ul_WorldFileKey);
}

void GXI_StartFlip()
{
    GXI_StopBinkLoad();

	// Wait fo GPU to be done before displaying something else 
#ifdef USE_TRIPPLE_BUFFERING    
	GXI_WaitFrameEnd();
#endif		

	// Just to be sure
	VIWaitForRetrace();
	VIWaitForRetrace();
}

/*$4
 ***********************************************************************************************************************
    Public Function
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Create a device
 =======================================================================================================================
 */
 
void GXI_FlushComplete ()
{
	VISetBlack(TRUE);
	VIFlush();
	VIWaitForRetrace();
	//VIWaitForRetrace();
#ifdef USE_TRIPPLE_BUFFERING
	GXI_WaitFrameEnd();
#endif // USE_TRIPPLE_BUFFERING
	GXAbortFrame();
	GXDrawDone();
	VIWaitForRetrace();
	VIWaitForRetrace();
}
void *GXI_pst_CreateDevice(void)
{
	return NULL;
}

/*
 =======================================================================================================================
    Aim:    Destroy a device
 =======================================================================================================================
 */
void GXI_DestroyDevice(void *_pst_SD)
{
}

/*
 =======================================================================================================================
    Aim:    Close OpenGL display
 =======================================================================================================================
 */
HRESULT GXI_l_Close(GDI_tdst_DisplayData *_pst_DD)
{
	return S_OK;
}

/* Aim: Init OpenGL object */
/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GXI_l_Init(GDI_tdst_DisplayData *_pst_DD)
{
	return 1;
}

void GXI_pst_InitDevice(void)
{
}

#ifdef USE_PERPECTIVE_CORRECT_COLORS
void GXI_CreateRampTex16()
{
    u32   size, i, offset;
    u16 j;
  
    // Write ramp data pattern
    for ( i = 0 ; i < (0x10000) ; ++i )
    {
        // IA8 format tile offset
        offset = ((i & 0xFC00) >> 6)
               + ((i & 0x0300) >> 8)
               + ((i & 0x00FC) << 8)
               + ((i & 0x0003) << 2);
        j=i;
        SwapWord(&j);
        *(g_RampTex16Data+offset) = j;
    }
   
    // Initialize texture object properties
    GXInitTexObj(&g_RampTex16, g_RampTex16Data, 256, 256, GX_TF_IA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	
    GXInitTexObjLOD(&g_RampTex16, GX_NEAR, GX_NEAR, 0, 0, 0, 0, 0, GX_ANISO_1);

    // Make sure data is written into the main memory.
    DCFlushRange(g_RampTex16Data, 256*256*2);
    
    GXInitTexObjUserData(&g_RampTex16Data, (void*)1);
}
#endif

ULONG gul_FRAME_BUFFER_WIDTH = GXI_WIDTH;
ULONG gul_FRAME_BUFFER_HEIGHT = 0; // Set by GXI_Init 


void GXI_Init(void)
{
	void*       g_pstCurrentBuffer;

    g_bFirstTimeInit = TRUE;
    
	/*----------------------------------------------------------------*
	// Set up half frame rendering mode (used by Bink)
     *----------------------------------------------------------------*/
    switch (VIGetTvFormat())
    {
      case VI_NTSC:
		L_memcpy(&g_st_HalfFrameMode,&GXNtsc240Int,sizeof(GXRenderModeObj));
        break;
      case VI_PAL:
		L_memcpy(&g_st_HalfFrameMode,&GXPal264Int,sizeof(GXRenderModeObj));
        break;
      case VI_MPAL:
		L_memcpy(&g_st_HalfFrameMode,&GXMpal240Int,sizeof(GXRenderModeObj));
        break;
    }    
    
	GXAdjustForOverscan(&g_st_HalfFrameMode, &g_st_HalfFrameMode, 0,GXI_OverScanHeightHalfFrame );
	g_st_HalfFrameMode.fbWidth = FRAME_BUFFER_WIDTH;
    ERR_X_Assert(g_st_HalfFrameMode.xfbHeight <= GXI_MaxHalfBufferHeight);


	/*----------------------------------------------------------------*
     *  Set up default rendering mode                                 *
     *  (used by GX/VI configurations and XFB size below)     		  *
     *----------------------------------------------------------------*/
    switch (VIGetTvFormat())
    {
      case VI_NTSC:
		L_memcpy(&g_st_DefaultMode,&GXNtsc480IntDf,sizeof(GXRenderModeObj));
        break;
      case VI_PAL:
		L_memcpy(&g_st_DefaultMode,&GXPal528IntDf,sizeof(GXRenderModeObj));
        break;
      case VI_MPAL:
		L_memcpy(&g_st_DefaultMode,&GXMpal480IntDf,sizeof(GXRenderModeObj));
        break;
    }
    
    // Trim off from top & bottom 16 scanlines (which will be overscanned).
    // Only 448 lines are rendered (in NTSC case).
    GXAdjustForOverscan(&g_st_DefaultMode, &g_st_DefaultMode, 0, GXI_OverScanHeight);
	g_st_DefaultMode.fbWidth = FRAME_BUFFER_WIDTH;
	
	// Height is set from TV format (PAL, NTSC, MPAL).
    gul_FRAME_BUFFER_HEIGHT = g_st_DefaultMode.xfbHeight;
    
    ERR_X_Assert(g_st_DefaultMode.xfbHeight <= GXI_MaxBufferHeight);

	GXI_Global_ACCESS(Xsize) = FRAME_BUFFER_WIDTH;
		
	// Compute buffer size
	/*{
	ULONG ulDefaultFrameBufferSize = VIPadFrameBufferWidth(g_st_DefaultMode.fbWidth) * 
		GXGetNumXfbLines(g_st_DefaultMode.efbHeight, GXGetYScaleFactor( g_st_DefaultMode.efbHeight,g_st_DefaultMode.xfbHeight ) ) *
		VI_DISPLAY_PIX_SZ;
		
	ULONG ulHalfFrameBufferSize = VIPadFrameBufferWidth(g_st_HalfFrameMode.fbWidth) * 
		GXGetNumXfbLines(g_st_HalfFrameMode.efbHeight, GXGetYScaleFactor( g_st_HalfFrameMode.efbHeight,g_st_HalfFrameMode.xfbHeight ) ) *
		VI_DISPLAY_PIX_SZ;
		
	//GXI_MaxExternalFrameBufferSize = ulDefaultFrameBufferSize;
	//GXI_MaxExternalHalfFrameBufferSize = ulHalfFrameBufferSize;
	
	if (GXI_MaxExternalFrameBufferSize < ulDefaultFrameBufferSize)
	{
		OSReport("GXI_MaxExternalFrameBufferSize %d\n",GXI_MaxExternalFrameBufferSize);
		OSReport("ulDefaultFrameBufferSize %d\n",ulDefaultFrameBufferSize);
	}
	
	if (GXI_MaxExternalHalfFrameBufferSize < ulHalfFrameBufferSize)
	{
		OSReport("GXI_MaxExternalHalfFrameBufferSize %d\n",GXI_MaxExternalHalfFrameBufferSize);
		OSReport("ulHalfFrameBufferSize %d\n",ulHalfFrameBufferSize);
	}
	}*/
	
    /*----------------------------------------------------------------*
    // Allocate buffers after the size have been set (g_st_mode and gul_FRAME_BUFFER_HEIGHT)
    // and before we need the buffers (GXInit, etc).
     *----------------------------------------------------------------*/
    GXI_AllocatePlayModeMem();

    g_pst_mode = &g_st_DefaultMode;
    GXI_gbFullFrameMode = TRUE;
    
    // Configure VI
    VIConfigure(g_pst_mode);
    
	/*----------------------------------------------------------------*
     *  Initialize Graphics                                           *
     *----------------------------------------------------------------*/
    // Alloc default 256K fifo
	DefaultFifo = (char *)MEM_p_AllocAlign(GXI_DefaultFIFOSize,32); // Never freed
    DefaultFifoObj = GXInit(DefaultFifo, GXI_DefaultFIFOSize);
    
    /*----------------------------------------------------------------*
     *  GX configuration by a render mode obj                         *
     *----------------------------------------------------------------*/

    // Configure GX using current render mode
    GXIInitGXWithRenderMode(g_pst_mode,FALSE);
    //GXSetScissor(0, 0+WIDE_SCREEN_ADJUST, (u32)FRAME_BUFFER_WIDTH, (u32)g_pst_mode->efbHeight-2*WIDE_SCREEN_ADJUST);

	GXSetDither(GX_TRUE); // could be in conflict with blur texture (not yet implemented) : could make an AI function to set it on/off 

	GXSetClipMode(GX_CLIP_ENABLE);
	
    /*----------------------------------------------------------------*
     *  Miscellaneous GX initialization                               *
     *----------------------------------------------------------------*/

	GXSetMisc( GX_MT_DL_SAVE_CONTEXT, 0 );
	
    // Clear embedded framebuffer
    GXCopyDisp(g_pstFrameBuffer2, GX_TRUE);

    // Verify (warning) messages are turned off by default
#ifdef _DEBUG    
    GXSetVerifyLevel(GX_WARN_NONE);
    //GXSetVerifyCallback(verify_callback);
#else
    GXSetVerifyLevel(GX_WARN_NONE);
#endif

    // Gamma correction
    GXSetDispCopyGamma(GX_GM_1_0);

    // Double buffering initialization
    VISetNextFrameBuffer(g_pstFrameBuffer1);
    g_pstCurrentBuffer = g_pstFrameBuffer2;

    // Tell VI device driver to write the current VI settings so far
    VIFlush();
    
    // Wait for retrace to start first frame
    VIWaitForRetrace();

    // Because of hardware restriction, we need to wait one more 
    // field to make sure mode is safely changed when we change
    // INT->DS or DS->INT. (VIInit() sets INT mode as a default)
    VIWaitForRetrace();
 
#ifdef USE_TRIPPLE_BUFFERING    
    // fifo synchro token callback
    GXSetDrawDoneCallback( DrawDoneCallback );

    nbFramesInWait = 0;

    myXFB1 = g_pstFrameBuffer1;
    myXFB2 = g_pstFrameBuffer2;
    dispXFB = myXFB1;
    copyXFB = myXFB2;

    VISetPreRetraceCallback(VIPreCallback);
    VISetPostRetraceCallback(VIPostCallback);
#endif

	MTXOrtho(GXI_Global_ACCESS(orthogonal2D_matrix),
	-1.0f, // t : Top edge of view volume, positive
	1.0f, // b : Bottom edge of view volume, b = -t
	-1.0f, // l : Left edge of view volume. l = -r
	1.0f, // r : Right edge of view volume, positive
	0.0f, // n : Positive distance to the near clipping plane.
	1.0f);// f	: Positive distance to the far clipping plane.	
	
	MTXIdentity(GXI_Global_ACCESS(identity_projection_matrix));		  

	GXI_Global_ACCESS(bUseMegaFogOveride) = FALSE;
	
	GXI_HW_States_Init();

	GXI_init_font();
	
	
	
#ifdef USE_PERPECTIVE_CORRECT_COLORS
	// init color swap	
	// Make a swap table which performs A->G, I->R conversion
    GXSetTevSwapModeTable(GX_TEV_SWAP1,
                          GX_CH_RED, GX_CH_ALPHA,
                          GX_CH_BLUE, GX_CH_ALPHA);
                          
	GXI_CreateRampTex16();
#endif	

	//GXI_ObjectList_Clear();
	
	GXI_AfterFX_Init();
	GXI_SFX_Init();
	
#ifndef USE_FULL_SCENE_SHADOWS	
	GXI_InitShadowTextures();
#endif
	
	// init water clipping texture
	{
	static u8 reflectionTexData[32] ATTRIBUTE_ALIGN(32);    
	reflectionTexData[0] = 0x00;
	reflectionTexData[1] = 0xFF;
		
	GXInitTexObj(&g_reflectionDepthTex, 
			 (void*)&reflectionTexData, 
			 2, 
			 1, 
			 GX_TF_I8, 
			 GX_CLAMP, 
			 GX_CLAMP, 
			 GX_FALSE); 
   	GXInitTexObjLOD(&g_reflectionDepthTex,
    					GX_NEAR,
    					GX_NEAR,
    					0,
    					0,
    					0,
    					0,
    					0,
    					GX_ANISO_1);
    					
	}
    //VISetBlack(FALSE);
    
    //VIFlush();

	// Flush memory
	DCFlushRange(MEM_gst_MemoryInfo.pv_DynamicBloc,MEM_gst_MemoryInfo.ul_RealSize);
	GXFlush();
	GXInvalidateVtxCache();

    
#ifdef GXI_PERF
	OSInitStopwatch(&GXI_PERF_TotTime, "myTotTime");
    OSResetStopwatch(&GXI_PERF_TotTime);    
#endif  

#ifdef _DEBUG
	MEM_vCheckMem(&MEM_gst_GCMemoryInfo);
	MEM_vCheckMem(&MEM_gst_SoundMemoryInfo);
	MEM_vCheckMem(&MEM_gst_MemoryInfo);
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
#endif// _DEBUG
}

/*
 =======================================================================================================================
    Aim:    Adapt OpenGL display to window
 =======================================================================================================================
 */
HRESULT GXI_l_ReadaptDisplay(HWND _hWnd, GDI_tdst_DisplayData *_pst_DD)
{
	return S_OK;
}


/*
 =======================================================================================================================
    Aim:    Wait for previous frame to finish rendering 
 =======================================================================================================================
 */
static u32		gst_Bkcolor = 0;
static u32		gst_BkcolorDef = 0;
 
void GXI_BeforeDisplay()
{
	u32 Color;
	GXI_SwitchVertexSpace();
	GXI_ObjectList_Clear();
	
	GXI_Global_ACCESS(bRendering2D) = FALSE;
    GXSetViewport(0.0f, 0.0f, (f32)FRAME_BUFFER_WIDTH, (f32)g_pst_mode->xfbHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, (u32)FRAME_BUFFER_WIDTH, (u32)g_pst_mode->efbHeight);
	GX_GXSetColorUpdate(GX_TRUE);
	Color = GXI_ColorABGR2RGBA(gst_BkcolorDef);
	GXI_DrawRectangle(0.0f, 0.0f, 1.0f, 1.0f , Color, MAT_Cc_Op_Copy);
    GXSetScissor(0, 0+WIDE_SCREEN_ADJUST, (u32)FRAME_BUFFER_WIDTH, (u32)g_pst_mode->efbHeight-2*WIDE_SCREEN_ADJUST);

#ifndef USE_FULL_SCENE_SHADOWS      
#ifdef USE_MY_TEX_REGION
	GXInvalidateTexRegion(&MyTexRegions[0]);
#else
    GXInvalidateTexAll(); 
#endif
#endif    
}


#ifdef USE_FULL_SCENE_SHADOWS
extern void GXI_ObjectList_SendFullSceneShadow();
extern void GXI_ObjectList_ComputeFullSceneShadowTextures();
#endif
#ifndef _FINAL_
u32 nbMaxTriangles=0;
#endif

int GXI_StopComputeRefectionDuring2D = 0;
int GXI_iTVMode = 0;
int GXI_b3DRendering = TRUE;

/*
 =======================================================================================================================
    Aim:    Flip buffer
 =======================================================================================================================
 */
void GXI_Flip()
{
	GXI_EndRaster(19);

#ifdef USE_FULL_SCENE_SHADOWS
	GXI_ObjectList_SendFullSceneShadow();
	GXI_ObjectList_ComputeFullSceneShadowTextures();
#else
	//GXI_EndShadowRendering();	
#endif	

	GXI_StopComputeRefectionDuring2D = 0;
    GXI_b3DRendering = TRUE;
    
#ifndef _FINAL_
	GX_GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	_GSP_BeginRaster(21);
    GXI_Rasters();
	_GSP_EndRaster(21); 
    GXI_Setup(); // menu
#endif	
    
	/*_GSP_BeginRaster(38);
	GXDrawDone();
	_GSP_EndRaster(38);*/

	GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);// copy : replace the old frame buffer with the new one, no blending of frame buffers
	GX_GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0); // no alpha compare
	GX_GXSetZCompLoc(GX_FALSE);

#ifdef USE_TRIPPLE_BUFFERING	
	// no xFB free for copy, so wait...
	_GSP_BeginRaster(38);		
	GXI_WaitFrameEnd();
	_GSP_EndRaster(38);
#endif // USE_TRIPPLE_BUFFERING	
	
	GXSetScissor(SCREEN_DIFF, 0+WIDE_SCREEN_ADJUST, FRAME_BUFFER_WIDTH - SCREEN_DIFF, (u32)FRAME_BUFFER_HEIGTH-2*WIDE_SCREEN_ADJUST);
	#ifdef USE_TRIPPLE_BUFFERING
    ++nbFramesInWait;
    ul_GC_frameCounter ++;
    #endif // USE_TRIPPLE_BUFFERING    
	
    GX_GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_GXSetColorUpdate(GX_TRUE);
    GX_GXSetAlphaUpdate(GX_TRUE);
    {
	    u8 VFilter[7] = {0x00 , 0x00 , 0x00 , 0x3f , 0x00 , 0x00 , 0x00};
	    u8 VFilter_ODD[7] = {0x00 , 0x20 , 0x00 , 0x1f , 0x00 , 0x00 , 0x00};
		GXSetCopyFilter( GX_FALSE,NULL,GX_TRUE,VFilter);
		GXSetDither(GX_TRUE);
		
	    // DE-Fliker -- Put /* here to desactivate 
	    //*
		GXSetFieldMask( GX_TRUE, GX_FALSE );
#ifdef USE_TRIPPLE_BUFFERING		
		ERR_X_Assert(copyXFB);
	    GXCopyDisp( copyXFB, GX_FALSE);
#else	   
	    GXCopyDisp( g_pstFrameBuffer1, GX_FALSE);
#endif // USE_TRIPPLE_BUFFERING	    
		GXSetCopyFilter( GX_FALSE,NULL,GX_TRUE,VFilter_ODD);
		GXSetFieldMask( GX_FALSE, GX_TRUE );//*/
	    GX_GXSetColorUpdate(GX_FALSE);
#ifdef USE_TRIPPLE_BUFFERING	    
	    GXCopyDisp( copyXFB, GX_TRUE);
#endif // USE_TRIPPLE_BUFFERING	    
	    GX_GXSetColorUpdate(GX_TRUE);
		GXSetFieldMask( GX_TRUE, GX_TRUE );
		GXSetCopyFilter(g_pst_mode->aa, g_pst_mode->sample_pattern, GX_TRUE, g_pst_mode->vfilter);
		GXSetDither(GX_FALSE);
		gst_BkcolorDef = gst_Bkcolor;

    }
    {
    	extern void GXI_WTR_ComputeDisruptMap();
		GXI_WTR_ComputeDisruptMap();
	}
    
    GXSetDrawDone();
	GC_ul_ValidFrameBufferNb++;
#ifdef USE_TRIPPLE_BUFFERING
	// copyXFB contains the next frame -> set it to a new (empty buffer).
	ERR_X_Assert(myXFB1 && myXFB2);
    copyXFB = (copyXFB == myXFB1) ? myXFB2 : myXFB1;
#endif // USE_TRIPPLE_BUFFERING    

	GXI_BeginRaster(19);	
}

void GXI_VideoFlip()
{
#ifndef NO_BINK
	// no xFB free for copy, so wait...
	GXI_WaitFrameEnd();

    ++nbFramesInWait;
    ul_GC_frameCounter ++;

    // Set Z/Color update to make sure eFB will be cleared at GXCopyDisp.
    // (If you want to control these modes by yourself in your application,
    //  please comment out this part.)
    GX_GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_GXSetColorUpdate(GX_TRUE);

    // Issue display copy command
	ERR_X_Assert(copyXFB);
    GXCopyDisp(copyXFB, GX_TRUE);

    // Wait until everything is drawn and copied into XFB.
    GXSetDrawDone();
	
	GC_ul_ValidFrameBufferNb++;
    // Set the next frame buffer
	// copyXFB contains the next frame -> set it to a new (empty buffer).
	ERR_X_Assert(myXFB1 && myXFB2);
    copyXFB = (copyXFB == myXFB1) ? myXFB2 : myXFB1;
#endif // NO_BINK
}
    

void GXI_SwitchDisplay(int _iTVMode)
{
	if (GXI_iTVMode == _iTVMode)
		return;
	
	GXI_iTVMode = _iTVMode;

	if (_iTVMode == 0)
		WIDE_SCREEN_ADJUST = 56.f;
	else 
		WIDE_SCREEN_ADJUST = 1.f;
}



int gi_crashX;
int gi_crashY;
int gi_crashH;
int gi_crash=0;

void GXI_SimpleBefore2D();
void GXI_Before2D();

void GXI_CrashBegin(void)
{

    gi_crash = 1;
    gi_crashX = (int)(0.05*GXI_Global_ACCESS(Xsize));
    gi_crashY = (int)(0.85*GXI_Global_ACCESS(Ysize));
    gi_crashH = 20;
	GXSetCopyClear(*(GXColor *)&gst_Bkcolor, 0x00ffffff);
#ifdef USE_TRIPPLE_BUFFERING	
	ERR_X_Assert(copyXFB);
	GXCopyDisp(copyXFB, GX_TRUE);
#endif // USE_TRIPPLE_BUFFERING	

}
void GXI_CrashEnd(void)
{

    GXI_Flip();

}
void GXI_CrashPrint(char*str)
{

    GXI_DrawText(gi_crashX,gi_crashY,str,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy);	
    gi_crashY -= gi_crashH;

}

void GXI_ErrBegin(void)
{
	GXI_BeforeDisplay();
	GXI_SimpleBefore2D();
	
	GXI_Global_ACCESS(Xsize) = FRAME_BUFFER_WIDTH; 
   	GXI_Global_ACCESS(Ysize) = FRAME_BUFFER_HEIGTH; 

	gi_crash = 1;
    gi_crashX = (int)(0.05*GXI_Global_ACCESS(Xsize));
    gi_crashY = (int)(0.85*GXI_Global_ACCESS(Ysize));
    gi_crashH = 20;

    VISetBlack(FALSE);	
	VIFlush();
	
	GXI_DrawRectangle(0.0f, 0.0f, 1.0f, 1.0f, 0, MAT_Cc_Op_Copy);

}
void GXI_ErrEnd(void)
{
    GXI_Flip();
}
void GXI_ErrPrint(char*str)
{
    int L,l;
    
    L = (int)(0.90*GXI_Global_ACCESS(Xsize));
    l = strlen(str)*8;
    
//    gi_crashX = max((L - l)/2 + gi_crashX, gi_crashX);
    

    GXI_DrawText(gi_crashX,gi_crashY,str,0xffffff,0,MAT_Cc_Op_Copy,MAT_Cc_Op_Copy/*MAT_Cc_Op_Sub*/);	
    gi_crashY -= gi_crashH;
}
/*
 =======================================================================================================================
    Aim:    Clear buffer
 =======================================================================================================================
 */
void GXI_Clear(LONG _l_Buffer, ULONG _ul_Color)
{
	static GXColor default_clear_color = { 0x00,0x00,0x00,0xFF };
	static GXColor *color = &default_clear_color;
	static u32 col;

	GX_GXSetZMode((_l_Buffer & GDI_Cl_ZBuffer)>0 ? GX_ENABLE : GX_DISABLE, GX_ALWAYS, (_l_Buffer & GDI_Cl_ZBuffer)>0 ? GX_ENABLE : GX_DISABLE);
		
	if (_l_Buffer & GDI_Cl_ColorBuffer)
	{
		if(_ul_Color==0x12345678) // shadow temporary color change
		{
			GXSetCopyClear(GX_BLACK, GX_MAX_Z24);     
			return;
		}
		
		col = GXI_ColorABGR2RGBA(_ul_Color);
		color = (GXColor*)&col;
		//GXSetCopyClear(*color, GX_MAX_Z24);    
	}
	
	// Div clear color by 2 because of mul2X
	if (g_bUseHWMul2x)
	{
		color->r >>= 1;
		color->g >>= 1;
		color->b >>= 1;
	}

	
//	else GXSetCopyClear(default_clear_color, 0);    
//	else GXSetCopyClear(default_clear_color, GX_MAX_Z24);   
	gst_Bkcolor = *(u32 *)color;	
	GXSetCopyClear(*color, GX_MAX_Z24);     
	
	if(g_bFirstTimeInit)
	{		 
	    GXI_Flip();
	    GXI_Flip();
	    VISetBlack(FALSE);
	    VIFlush();
	    g_bFirstTimeInit = FALSE;
	}
}


void GXI_SetModelViewMatrix(MATH_tdst_Matrix *_pst_Matrix, GXPosNrmMtx _mtxID )
{
	static MATH_tdst_Matrix		st_OGLMatrix;
	
#ifdef USE_HARDWARE_LIGHTS	
	Mtx m, m2;
	u32	ulLIGHT_XOR;
	u32	ulLIGHT_NORMAL_XSYM_XOR;
#endif

	// eye matrix
	//GDI_gpst_CurDD->st_Camera.st_InverseMatrix
	
	
	MATH_MakeOGLMatrix(&st_OGLMatrix, _pst_Matrix);

	// transpose the matrix and inverse all z components

	GXI_Global_ACCESS(current_modelview_matrix)[0][0] = st_OGLMatrix.Ix;
	GXI_Global_ACCESS(current_modelview_matrix)[1][0] = -st_OGLMatrix.Iy;
	GXI_Global_ACCESS(current_modelview_matrix)[2][0] = -st_OGLMatrix.Iz;

	GXI_Global_ACCESS(current_modelview_matrix)[0][1] = st_OGLMatrix.Jx;
	GXI_Global_ACCESS(current_modelview_matrix)[1][1] = -st_OGLMatrix.Jy;
	GXI_Global_ACCESS(current_modelview_matrix)[2][1] = -st_OGLMatrix.Jz;

	GXI_Global_ACCESS(current_modelview_matrix)[0][2] = st_OGLMatrix.Kx;
	GXI_Global_ACCESS(current_modelview_matrix)[1][2] = -st_OGLMatrix.Ky;
	GXI_Global_ACCESS(current_modelview_matrix)[2][2] = -st_OGLMatrix.Kz;

	GXI_Global_ACCESS(current_modelview_matrix)[0][3] = st_OGLMatrix.T.x;
	GXI_Global_ACCESS(current_modelview_matrix)[1][3] = -st_OGLMatrix.T.y;
	GXI_Global_ACCESS(current_modelview_matrix)[2][3] = -st_OGLMatrix.T.z;
	
#ifdef GIGSCALE0	
	// to use Gigscale.. if any clipping bug.. not used for now ...
	MTXScaleApply(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), *(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), GIGSCALE0, GIGSCALE0, GIGSCALE0);
#endif	
	
	//if(GXI_Global_ACCESS(bPutObjectInObjectList))
	//	return;
	
	GXLoadPosMtxImm(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), _mtxID);

#ifdef USE_HARDWARE_LIGHTS

	/* symétrie ? */
	if (GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF)
		ulLIGHT_XOR = 0;
	else
		ulLIGHT_XOR = 1;

	if (GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_Symetric)
		ulLIGHT_NORMAL_XSYM_XOR  = 0;
	else
	{
		ulLIGHT_NORMAL_XSYM_XOR  = 1;
		ulLIGHT_XOR ^= 1;
	}
	
	if  (
	g_bUseHardwareLights && 
		(ulLIGHT_XOR || ulLIGHT_NORMAL_XSYM_XOR)
	)
	{
	    MTXIdentity(m2);
	    
	   	if  (ulLIGHT_XOR && ulLIGHT_NORMAL_XSYM_XOR)
		{
			m2[1][1] = - m2[1][1];
			m2[2][2] = - m2[2][2];
		} 
		else if  (ulLIGHT_NORMAL_XSYM_XOR)
		{
			m2[0][0] = - m2[0][0];
		} 
		else//ulLIGHT_XOR
		{
			m2[0][0] = - m2[0][0];
			m2[1][1] = - m2[1][1];
			m2[2][2] = - m2[2][2];
		} 	
		
		MTXConcat(GXI_Global_ACCESS(current_modelview_matrix), m2, m);

		// set the inverse transpose for hardware lighting
	    MTXInvXpose(m, m); 
	}
	else
	{
		// set the inverse transpose for hardware lighting
	    MTXInvXpose(GXI_Global_ACCESS(current_modelview_matrix), m); 
	}
    	
    GXLoadNrmMtxImm(m, _mtxID);
#endif   
}

void GXI_SetReflectionMatrix(MATH_tdst_Matrix* _pst_Matrix)
{
	MATH_tdst_Matrix mtxObject;
//	MATH_tdst_Matrix mtxView;
	MATH_tdst_Matrix mtxModelView;
//	MATH_tdst_Matrix mtxReflect;
//	MATH_tdst_Matrix mtxObjectReflect;

	static MATH_tdst_Matrix		st_OGLClipMatrix;
	static Mtx m1 = {0.0f, 0.0f, 1.0f, 0.5f,
		  0.0f, 1.0f, 0.0f, 0.0f,
		  1.0f, 0.0f, 0.0f, 0.0f};

	float fEcrasement;
	extern float GlobalWaterZ;
	
	fEcrasement = 1.f;

	m1[0][3] = GlobalWaterZ;
	
	if(!GDI_gpst_CurDD_SPR.pst_CurrentGameObject)
		return;
	MATH_MulMatrixMatrix(&mtxObject, _pst_Matrix, &GDI_gpst_CurDD->st_Camera.st_Matrix);

	mtxObject.Iz = -mtxObject.Iz * fEcrasement;
	mtxObject.Jz = -mtxObject.Jz * fEcrasement;
	mtxObject.Kz = -mtxObject.Kz * fEcrasement;
	mtxObject.T.z -= (1.0f + fEcrasement) * (mtxObject.T.z - GlobalWaterZ);

	MATH_MulMatrixMatrix(&mtxModelView, &mtxObject, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
	mtxModelView.lType = _pst_Matrix->lType;
	GXI_SetModelViewMatrix(&mtxModelView, GX_PNMTX1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GXI_SetViewMatrix(MATH_tdst_Matrix *_pst_Matrix)
{
	GXI_SetReflectionMatrix(_pst_Matrix);
	GXI_SetModelViewMatrix(_pst_Matrix, GX_PNMTX0);
}

/*
 =======================================================================================================================
    Aim:    Send View matrix for render
 =======================================================================================================================
 */
void GXI_SetProjectionMatrix(CAM_tdst_Camera *_pst_Cam)
{
	float					f, f_ScreenRatio;
	LONG					w, h, W, H;
	LONG					x, y;
	ULONG					Flags;
	float					l, r, t, b;
	float					f_near, f_far;
		
#ifdef GIGSCALE0	
	f_near = _pst_Cam->f_NearPlane;
	f_far = _pst_Cam->f_FarPlane*GIGSCALE0;
#else
	f_near = _pst_Cam->f_NearPlane/20.0f;
	f_far = _pst_Cam->f_FarPlane;
#endif
	
	
	//(_pst_Cam->f_NearPlane/20.0f)*GIGSCALE0;
	//f_far = _pst_Cam->f_FarPlane;///20.0f)*GIGSCALE0;
	
    if (!(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective) )
        f_ScreenRatio = GDI_gaf_ScreenRation[ GDI_Cul_SRC_Square ];
    else
    {
        /*x = GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst;
    	if ( (x <= 0) ||	(x >= GDI_Cul_SRC_Number) ) 
            f_ScreenRatio = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
	    else
		    f_ScreenRatio = GDI_gaf_ScreenRation[ x ];*/
		
		if (GXI_iTVMode == 2 && GXI_b3DRendering)
			f_ScreenRatio = 0.5625f; //16/9;
		else
			f_ScreenRatio = 0.75f; //4/3;
    }

	f = _pst_Cam->f_YoverX * GDI_gpst_CurDD->st_ScreenFormat.f_PixelYoverX * f_ScreenRatio;
	Flags = GDI_gpst_CurDD->st_ScreenFormat.ul_Flags;

	w = GDI_gpst_CurDD->st_Device.l_Width;
	h = GDI_gpst_CurDD->st_Device.l_Height;
		

	if(Flags & GDI_Cul_SFF_ReferenceIsY)
	{
		H = h;
		W = (LONG) (h / f);

		if(((Flags & GDI_Cul_SFF_CropToWindow) && (W > w)) || ((Flags & GDI_Cul_SFF_OccupyAll) && (W < w)))
		{
			H = (LONG) (w * f);
			W = w;
		}
	}
	else
	{
		H = (LONG) (w * f);
		W = w;

		if(((Flags & GDI_Cul_SFF_CropToWindow) && (H > h)) || ((Flags & GDI_Cul_SFF_OccupyAll) && (H < h)))
		{
			W = (LONG) (h / f);
			H = h;
		}
	}


	if(_pst_Cam->f_ViewportWidth == 0)
	{
		_pst_Cam->f_ViewportWidth = 1.0f;
		_pst_Cam->f_ViewportHeight = 1.0f;
		_pst_Cam->f_ViewportLeft = 0.0f;
		_pst_Cam->f_ViewportTop = 0.0f;
	}

	x = (int) (_pst_Cam->f_ViewportLeft * W + ((w - W) / 2));
	y = (int) (_pst_Cam->f_ViewportTop * H + ((h - H) / 2));
	h = (int) (_pst_Cam->f_ViewportHeight * H);
	w = (int) (_pst_Cam->f_ViewportWidth * W);	

	_pst_Cam->l_ViewportRealLeft = x;
	_pst_Cam->l_ViewportRealTop = y;

	if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
	{
		/*f = (_pst_Cam->f_FieldOfVision) * 45.0f;
		
		MTXPerspective (*(Mtx44*)&_pst_Cam->st_ProjectionMatrix, f, 1.0f/f_ScreenRatio, _pst_Cam->f_NearPlane, _pst_Cam->f_FarPlane);
	
		GXSetProjection(*(Mtx44*)&_pst_Cam->st_ProjectionMatrix, GX_PERSPECTIVE);
		*/
		f = 1.0f / fNormalTan(_pst_Cam->f_FieldOfVision / 2);
		MATH_SetIdentityMatrix(&_pst_Cam->st_ProjectionMatrix);
		
		//if (GXI_iTVMode == )
		

		if(GDI_gpst_CurDD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_ReferenceIsY)
		{
			_pst_Cam->st_ProjectionMatrix.Ix = f * f_ScreenRatio;
			_pst_Cam->st_ProjectionMatrix.Jy = f;
		}
		else
		{
			_pst_Cam->st_ProjectionMatrix.Ix = f;
			_pst_Cam->st_ProjectionMatrix.Jy = f / f_ScreenRatio;
		}

		_pst_Cam->st_ProjectionMatrix.Kz = -f_near/f_far;//(_pst_Cam->f_FarPlane + _pst_Cam->f_NearPlane) / (_pst_Cam->f_FarPlane - _pst_Cam->f_NearPlane);
		_pst_Cam->st_ProjectionMatrix.T.z = -1.0f;
		_pst_Cam->st_ProjectionMatrix.Sz = -1.0f * f_near;
		_pst_Cam->st_ProjectionMatrix.w = 0.0f; // MATRIX W!

		GXSetProjection(*(Mtx44*)&_pst_Cam->st_ProjectionMatrix, GX_PERSPECTIVE);
		
		// fog adjustment
		{
		static GXFogAdjTable fogTable;
		GXInitFogAdjTable(&fogTable, (u16) FRAME_BUFFER_WIDTH, *(Mtx44*)&_pst_Cam->st_ProjectionMatrix);
        GXSetFogRangeAdj(GX_ENABLE, (u16) (FRAME_BUFFER_WIDTH/2.0f), &fogTable);
        }
		
	}
	else
	{
		float	f_IsoFactorZoom, f_Scale;
		
		MATH_SetIdentityMatrix(&_pst_Cam->st_ProjectionMatrix);

		f_IsoFactorZoom = _pst_Cam->f_IsoFactor * _pst_Cam->f_IsoZoom;
		f_Scale = f_IsoFactorZoom;

		if(GDI_gpst_CurDD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_ReferenceIsY)
		{
			f = 1 / GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;

			if(_pst_Cam->f_IsoFactor == 0)
			{
				MTXOrtho(*(Mtx44*)&_pst_Cam->st_ProjectionMatrix, 1.0f, 0.0f, (1 - f) / 2, (1 + f) / 2, -f_near, -_pst_Cam->f_FarPlane);
			}
			else
			{
				l = f_Scale * (-f);
				r = f_Scale * (f);
				b = f_Scale * -1;
				t = f_Scale * 1;

				MTXOrtho(*(Mtx44*)&_pst_Cam->st_ProjectionMatrix, t, b, l, r, _pst_Cam->f_FarPlane, -_pst_Cam->f_FarPlane);
			}
		}
		else
		{
			f = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
			if(_pst_Cam->f_IsoFactor == 0)
				MTXOrtho(*(Mtx44*)&_pst_Cam->st_ProjectionMatrix, (1 + f) / 2, (1 - f) / 2, 0, 1, -f_near, -_pst_Cam->f_FarPlane);
			else
			{
				t = f_Scale * (-f);
				b = f_Scale * (f);
				l = f_Scale * -1;
				r = f_Scale * 1;

				MTXOrtho(*(Mtx44*)&_pst_Cam->st_ProjectionMatrix, t, b, l, r, _pst_Cam->f_FarPlane, -_pst_Cam->f_FarPlane);
			}
		}
		
		GXSetProjection(*(Mtx44*)&_pst_Cam->st_ProjectionMatrix, GX_ORTHOGRAPHIC);
		
	}
	GXI_Global_ACCESS(fFocale) = f;
	GXI_Global_ACCESS(fScreenRatio) = f_ScreenRatio; 	
}

void GXI_SetViewMatrix_SDW(MATH_tdst_Matrix *pst_Matrix , float *Limits)
{
	static MATH_tdst_Matrix		st_OGLMatrix;
	
	MATH_MakeOGLMatrix(&st_OGLMatrix, pst_Matrix);

	// transpose the matrix and inverse all z components

	GXI_Global_ACCESS(current_modelview_matrix)[0][0] = st_OGLMatrix.Ix;
	GXI_Global_ACCESS(current_modelview_matrix)[1][0] = -st_OGLMatrix.Iy;
	GXI_Global_ACCESS(current_modelview_matrix)[2][0] = -st_OGLMatrix.Iz;

	GXI_Global_ACCESS(current_modelview_matrix)[0][1] = st_OGLMatrix.Jx;
	GXI_Global_ACCESS(current_modelview_matrix)[1][1] = -st_OGLMatrix.Jy;
	GXI_Global_ACCESS(current_modelview_matrix)[2][1] = -st_OGLMatrix.Jz;

	GXI_Global_ACCESS(current_modelview_matrix)[0][2] = st_OGLMatrix.Kx;
	GXI_Global_ACCESS(current_modelview_matrix)[1][2] = -st_OGLMatrix.Ky;
	GXI_Global_ACCESS(current_modelview_matrix)[2][2] = -st_OGLMatrix.Kz;

	GXI_Global_ACCESS(current_modelview_matrix)[0][3] = st_OGLMatrix.T.x;
	GXI_Global_ACCESS(current_modelview_matrix)[1][3] = -st_OGLMatrix.T.y;
	GXI_Global_ACCESS(current_modelview_matrix)[2][3] = -st_OGLMatrix.T.z;
	
	GXLoadPosMtxImm(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), GX_PNMTX0);
}

void GXI_SetTextureTarget(u32 Num , u32 Clear)
{
	GXI_Clear(GDI_Cl_ColorBuffer|GDI_Cl_ZBuffer, 0x12345678 );
}

void GXI_SimpleBefore2D()
{
	extern void GXI_CompleteShadowRenderingB2D();
	extern void GXI_GETZ_OneFrameEnding();
	
	GXI_CompleteShadowRenderingB2D();
	GXI_GETZ_OneFrameEnding();
	GXSetScissor(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGTH);
	
	GXI_Global_ACCESS(bRendering2D) = TRUE;
	
	GXI_StopComputeRefectionDuring2D = 1;
	GXI_b3DRendering = FALSE;
}


void GXI_Before2D()
{
	extern void GXI_CompleteShadowRenderingB2D();
	extern void GXI_GETZ_OneFrameEnding();
	extern int ControlMode;	
	
	GXI_CompleteShadowRenderingB2D();
	GXI_GETZ_OneFrameEnding();
	

	GXI_Global_ACCESS(bRendering2D) = TRUE;
	
	GXSetScissor(0, WIDE_SCREEN_ADJUST, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGTH-2*WIDE_SCREEN_ADJUST);

	GXI_AfterFX();
	
	GXSetScissor(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGTH);
	
	// draw widescreen bands
	if(WIDE_SCREEN_ADJUST)
	{
		//extern void GXI_DrawRectangle_Z(f32 fx, f32 fy, f32 fw, f32 fh, u32 C , u32 T , u32 WriteZ , f32 ZValue);
		//GX_GXSetColorUpdate(GX_FALSE);
		//GXI_DrawRectangle_Z(0.0f, 0.0f, 1.0f, 1.0f, 0x00000040, MAT_Cc_Op_Add , 1 , -0.9999f);
		//GX_GXSetColorUpdate(GX_TRUE);
		GXI_DrawRectangle(0.0f, 0.0f, 1.0f, (WIDE_SCREEN_ADJUST+2.0f)/(float)FRAME_BUFFER_HEIGTH, 0x00000000, MAT_Cc_Op_Copy);
		GXI_DrawRectangle(0.0f, ((float)FRAME_BUFFER_HEIGTH-(WIDE_SCREEN_ADJUST+2.0f))/(float)FRAME_BUFFER_HEIGTH, 1.0f, (WIDE_SCREEN_ADJUST+2.0f)/(float)FRAME_BUFFER_HEIGTH, 0x00000000, MAT_Cc_Op_Copy);
	}

//#define DRAW_SAFE_FRAME	// Pour s'assurer que les textes sont bien dans la zone autorisée.
#ifdef DRAW_SAFE_FRAME	
	{
		//	NTSC / MPAL: Approximately 608 x 388
		u32 ulSafeHeight = 480 - 388;
		u32 ulSafeWidth = 640 - 608;
	
		GXI_DrawRectangle(
			.5f * (float)ulSafeWidth/(float)640, 
			.5f * (float)ulSafeHeight/(float)480, 
			(float)608/(float)640, 
			(float)388/(float)480, 
			0x60000060, MAT_Cc_Op_Alpha);
	}
#endif // DRAW_SAFE_FRAME	
	{
		// Pour mettre les textes dans la safe frame.
		static float fXModifierInGame = 0.f;
		static float fYModifierInGame = 0.f;
		static float fWModifierInGame = 10.f;
		static float fHModifierInGame = 10.f;
		
		static float fXModifierMenu = 5.f;
		static float fYModifierMenu = -11.f;
		static float fWModifierMenu = 0.f;
		static float fHModifierMenu = 24.f;
	
		if (ControlMode == 2) // Menu
		    GXSetViewport(fXModifierMenu + fWModifierMenu, fYModifierMenu + fHModifierMenu, (f32)FRAME_BUFFER_WIDTH - 2 * fWModifierMenu , (f32)g_pst_mode->xfbHeight - 2 * fHModifierMenu, 0.0f, 1.0f);
		else
		    GXSetViewport(fXModifierInGame + fWModifierInGame, fYModifierInGame + fHModifierInGame, (f32)FRAME_BUFFER_WIDTH - 2 * fWModifierInGame , (f32)g_pst_mode->xfbHeight - 2 * fHModifierInGame, 0.0f, 1.0f);
	}
	
	GXI_StopComputeRefectionDuring2D = 1;
	GXI_b3DRendering = FALSE;
}

void GXI_SetDCPixelFormat(HDC _hDC)
{
}

void GXI_SetupRC(void *_pst_SD)
{
}


// This function performs GX configuration by using current rendering mode
void GXIInitGXWithRenderMode(GXRenderModeObj *pmode,BOOL _bInvertField)
{
		
    /*----------------------------------------------------------------*
     *  GX configuration by a render mode obj                         *
     *----------------------------------------------------------------*/

    // These are all necessary function calls to take a render mode
    // object and set up the relevant GX configuration.

    // jitter frustum for field rendering
    if (pmode->field_rendering)
    {
    	BOOL field = VIGetNextField();
    	
    	if (_bInvertField)
    		field = !field;
    	
        GXSetViewportJitter(
                    0.0f, 0.0f,
                    (f32)(pmode->fbWidth),
                    (f32)(pmode->efbHeight),
                     0.0f, 1.0f,
                    field);
    }
    else
        GXSetViewport(
                   0.0f, 0.0f,
                   (f32)(pmode->fbWidth),
                   (f32)(pmode->efbHeight),
                   0.0f, 1.0f );
                   
    GXSetFieldMode(pmode->field_rendering, pmode->xfbHeight < pmode->viHeight); 

    GXSetScissor(0, 0, (U32)pmode->fbWidth, (U32)pmode->efbHeight);
    GXSetDispCopySrc(0, 0, pmode->fbWidth, pmode->efbHeight);
    GXSetDispCopyDst(pmode->fbWidth, pmode->xfbHeight);
    GXSetDispCopyYScale((F32)(pmode->xfbHeight) / (F32)(pmode->efbHeight));
    GXSetCopyFilter(pmode->aa, pmode->sample_pattern, GX_TRUE, pmode->vfilter);
    
    // Note that following is an appropriate setting for full-frame AA mode.
    // You should use "xfbHeight" instead of "efbHeight" to specify actual
    // view size. Since this library doesn't support such special case, please
    // call these in each application to override the normal setting.
    if (pmode->aa)
        GXSetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    else
#ifdef DONT_USE_ALPHA_DEST    
        GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
#else
        GXSetPixelFmt(GX_PF_RGBA6_Z24, GX_ZC_LINEAR);
#endif
}
  
  
// Allocate memory used by gameplay, and change render mode
extern GDI_tdst_DisplayData *GDI_pst_GlobalDD;
extern SOFT_tdst_ZList	*p_Current_SOFT_gst_ZList;

void GXI_AllocatePlayModeMem()
{
	u32 ulAfterFXBufferSize, ulAfterFX_ZRSBufferSize; 
	
#ifdef _DEBUG
	MEM_vCheckMem(&MEM_gst_GCMemoryInfo);
	MEM_vCheckMem(&MEM_gst_SoundMemoryInfo);
	MEM_vCheckMem(&MEM_gst_MemoryInfo);
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
#endif// _DEBUG
	
	ERR_X_Assert(MEM_gst_GCMemoryInfo.ul_DynamicCurrentAllocated == 0);
	
	g_pstFrameBuffer1 = (char *)MEM_p_AllocAlignMem(GXI_MaxExternalFrameBufferSize*2,64,&MEM_gst_GCMemoryInfo);
	g_pstFrameBuffer2 = g_pstFrameBuffer1 + GXI_MaxExternalFrameBufferSize;
	L_memset(g_pstFrameBuffer1,0,GXI_MaxExternalFrameBufferSize*2);

	
	ulAfterFXBufferSize = OSRoundUp32B( GXGetTexBufferSize( FRAME_BUFFER_WIDTH, 
									 	 GXI_MaxBufferHeight, 
									 	 GX_TF_RGBA8, 
									 	 FALSE, 
									 	 0));
								 	 
	ulAfterFX_ZRSBufferSize = OSRoundUp32B(GXGetTexBufferSize( (FRAME_BUFFER_WIDTH ),//>>1, 
									 	 GXI_MaxBufferHeight,//>>1, 
									 	 GX_TF_RGBA8, //GX_TF_RGB565, 
									 	 FALSE, 
									 	 0));
	
	SOFT_gp_Compute = (SOFT_tdst_ZList *)MEM_p_AllocAlignMem(size_SOFT_Compute,64,&MEM_gst_GCMemoryInfo);	
    L_memset( SOFT_gp_Compute, 0, sizeof( SOFT_tdst_ComputingBuffers ) );
    
	if (GDI_pst_GlobalDD)
		GDI_pst_GlobalDD->pst_ComputingBuffers = (SOFT_tdst_ComputingBuffers *) SOFT_gp_Compute;
  
    
	SOFT_gst_ZList = (SOFT_tdst_ZList *) MEM_p_AllocAlignMem(size_SOFT_gst_ZList,64,&MEM_gst_GCMemoryInfo);
    L_memset( SOFT_gst_ZList, 0, size_SOFT_gst_ZList );
    
	SOFT_gst_ZList_CP = (SOFT_tdst_ZList_CommonParrams *) MEM_p_AllocAlignMem(size_SOFT_gst_ZList_CP,64,&MEM_gst_GCMemoryInfo);
    L_memset( SOFT_gst_ZList_CP, 0, size_SOFT_gst_ZList_CP );
    
	p_AfterFXBuffer = (char *) MEM_p_AllocAlignMem(ulAfterFXBufferSize,64,&MEM_gst_GCMemoryInfo);
    L_memset( p_AfterFXBuffer, 0, ulAfterFXBufferSize );
	
	p_AfterFXBuffer_ZSR = (char *) MEM_p_AllocAlignMem(ulAfterFX_ZRSBufferSize,64,&MEM_gst_GCMemoryInfo);
    L_memset( p_AfterFXBuffer_ZSR, 0, ulAfterFX_ZRSBufferSize );

	SOFT_ZList_Init();
	SOFT_ZList_Clear();
#ifdef USE_TRIPPLE_BUFFERING
	myXFB1 = g_pstFrameBuffer1;
    myXFB2 = g_pstFrameBuffer2;
    dispXFB = myXFB1;
    copyXFB = myXFB2;
#endif // USE_TRIPPLE_BUFFERING
        
#ifdef _DEBUG
	MEM_vCheckMem(&MEM_gst_GCMemoryInfo);
	MEM_vCheckMem(&MEM_gst_SoundMemoryInfo);
	MEM_vCheckMem(&MEM_gst_MemoryInfo);
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
#endif// _DEBUG
}
	
// Free memory used by gameplay	
void GXI_FreePlayModeMem()
{	
    GXBool overhi,underlow,readIdle,cmdIdle,brkpt;
#ifdef _DEBUG
	MEM_vCheckMem(&MEM_gst_GCMemoryInfo);
	MEM_vCheckMem(&MEM_gst_SoundMemoryInfo);
	MEM_vCheckMem(&MEM_gst_MemoryInfo);
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
#endif// _DEBUG

	// Wait until we can free display buffers
	// Display black
    GC_ul_ValidFrameBufferNb = 0;
#ifdef USE_TRIPPLE_BUFFERING    
	GXI_ulBlackFrameNb = 3;
#endif // USE_TRIPPLE_BUFFERING	
	VISetBlack(TRUE);
	VIFlush();
	GXFlush();
	
	VIWaitForRetrace();
	VIWaitForRetrace();
	
#ifdef USE_TRIPPLE_BUFFERING	
	// Wait for last copy to be done (so that GPU is inactive). 
	GXI_WaitFrameEnd();
#endif // USE_TRIPPLE_BUFFERING
			
	// Check that GPU is inactive
	GXGetGPStatus(&overhi,&underlow,&readIdle,&cmdIdle,&brkpt);
	while (!readIdle || !cmdIdle)
		GXGetGPStatus(&overhi,&underlow,&readIdle,&cmdIdle,&brkpt);
	
	// Now the vi displays black and the GPU is inactive -> we can free buffers.
	SPG2_Modifier_FlushModifiers();
	SPG2_DestroyCachedData();
	
	GXI_FreeTmpDLBuff();
	GXI_v_FreeRenderBuff();
	SOFT_ZList_Clear();

	MEM_Free(SOFT_gp_Compute); SOFT_gp_Compute = NULL;
	GDI_pst_GlobalDD->pst_ComputingBuffers = NULL;
	
	MEM_Free(SOFT_gst_ZList); SOFT_gst_ZList = NULL;
	MEM_Free(SOFT_gst_ZList_CP); SOFT_gst_ZList_CP = NULL;
	p_Current_SOFT_gst_ZList = NULL;
	
	MEM_Free(p_AfterFXBuffer); p_AfterFXBuffer = NULL;
	MEM_Free(p_AfterFXBuffer_ZSR); p_AfterFXBuffer_ZSR = NULL;
		
	MEM_Free(g_pstFrameBuffer1);
	g_pstFrameBuffer1 = NULL;
	g_pstFrameBuffer2 = NULL;
#ifdef USE_TRIPPLE_BUFFERING	
	myXFB1 = NULL;
	myXFB2 = NULL;
	dispXFB = NULL;
	copyXFB = NULL;
#endif // USE_TRIPPLE_BUFFERING	
	
	ERR_X_Assert(MEM_gst_GCMemoryInfo.ul_DynamicCurrentAllocated == 0);
	
#ifdef _DEBUG
	MEM_vCheckMem(&MEM_gst_GCMemoryInfo);
	MEM_vCheckMem(&MEM_gst_SoundMemoryInfo);
	MEM_vCheckMem(&MEM_gst_MemoryInfo);
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
#endif// _DEBUG
	
}

// Allocate memory used by loading
void GXI_AllocateLoadModeMem(BOOL _bHalfFrameRenderMode)
{
	// Total buffer size must contain 2 buffers (half or full) or one full buffer.
	u32 uTotalBufferSize;
	u32 uBufferSize;
	if (_bHalfFrameRenderMode)
	{
		uBufferSize = GXI_MaxExternalHalfFrameBufferSize;
		uTotalBufferSize = 2*GXI_MaxExternalHalfFrameBufferSize;
		if (uTotalBufferSize < GXI_MaxExternalFrameBufferSize)
			uTotalBufferSize = GXI_MaxExternalFrameBufferSize;
	}
	else
	{
		uBufferSize = GXI_MaxExternalFrameBufferSize;
		uTotalBufferSize = 2*GXI_MaxExternalFrameBufferSize;
	}
		
#ifdef _DEBUG
	MEM_vCheckMem(&MEM_gst_GCMemoryInfo);
	MEM_vCheckMem(&MEM_gst_SoundMemoryInfo);
	MEM_vCheckMem(&MEM_gst_MemoryInfo);
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
#endif// _DEBUG

	g_pstFrameBuffer1 = (char *)MEM_p_AllocAlignMem(uTotalBufferSize,64,&MEM_gst_GCMemoryInfo);
	g_pstFrameBuffer2 = g_pstFrameBuffer1 + uBufferSize;
#ifdef USE_TRIPPLE_BUFFERING	
	myXFB1 = g_pstFrameBuffer1;
    myXFB2 = g_pstFrameBuffer2;
    dispXFB = myXFB1;
    copyXFB = myXFB2;
#endif // USE_TRIPPLE_BUFFERING    
    
    // Set new render mode 
    if (_bHalfFrameRenderMode)
    {
	    g_pst_mode = &g_st_HalfFrameMode;
    	VIConfigure(g_pst_mode);
	    GXIInitGXWithRenderMode(g_pst_mode,FALSE);
	    GXI_gbFullFrameMode = FALSE;
    }
    
#ifdef _DEBUG
	MEM_vCheckMem(&MEM_gst_GCMemoryInfo);
	MEM_vCheckMem(&MEM_gst_SoundMemoryInfo);
	MEM_vCheckMem(&MEM_gst_MemoryInfo);
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
#endif// _DEBUG	
}

    
// Free memory used by loading
void GXI_FreeLoadModeMem()
{
    GXBool overhi,underlow,readIdle,cmdIdle,brkpt;
    
#ifdef _DEBUG
	MEM_vCheckMem(&MEM_gst_GCMemoryInfo);
	MEM_vCheckMem(&MEM_gst_SoundMemoryInfo);
	MEM_vCheckMem(&MEM_gst_MemoryInfo);
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
#endif// _DEBUG
	
	// Wait until we can free frame buffers
	// Display black
    GC_ul_ValidFrameBufferNb = 0;
#ifdef USE_TRIPPLE_BUFFERING    
	GXI_ulBlackFrameNb = 3;
#endif // USE_TRIPPLE_BUFFERING	
	VISetBlack(TRUE);
	VIFlush();
	GXFlush();
	
	VIWaitForRetrace();
    VIWaitForRetrace();
	
#ifdef USE_TRIPPLE_BUFFERING	
	// Wait for last copy to be done (so that GPU is inactive). 
	GXI_WaitFrameEnd();
#endif // USE_TRIPPLE_BUFFERING	
		
	// Check that GPU is inactive
	GXGetGPStatus(&overhi,&underlow,&readIdle,&cmdIdle,&brkpt);
	while (!readIdle || !cmdIdle)
		GXGetGPStatus(&overhi,&underlow,&readIdle,&cmdIdle,&brkpt);
	
	// Now the vi displays black and the GPU is inactive -> we can switch buffers.
	MEM_Free(g_pstFrameBuffer1);
	g_pstFrameBuffer1 = NULL;
	g_pstFrameBuffer2 = NULL;
#ifdef USE_TRIPPLE_BUFFERING	
	myXFB1 = NULL;
	myXFB2 = NULL;
	dispXFB = NULL;
	copyXFB = NULL;
#endif // USE_TRIPPLE_BUFFERING	
	
#ifdef _DEBUG
	MEM_vCheckMem(&MEM_gst_GCMemoryInfo);
	MEM_vCheckMem(&MEM_gst_SoundMemoryInfo);
	MEM_vCheckMem(&MEM_gst_MemoryInfo);
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
#endif// _DEBUG
}	

void GXI_SetPlayMode()
{
	// Free load mode buffers
	GXI_FreeLoadModeMem();

	// Allocate play mode buffers
	GXI_AllocatePlayModeMem();
	
    // Set new render mode (if it has changed)
    if (g_pst_mode != &g_st_DefaultMode)
    {
	    g_pst_mode = &g_st_DefaultMode;
	    GXI_gbFullFrameMode = TRUE;
	    VIConfigure(g_pst_mode);
	    GXIInitGXWithRenderMode(g_pst_mode,FALSE);
	}
}

void GXI_SetLoadMode(BOOL _bHalfFrameRenderMode)
{
    // Free memory used by gameplay
    GXI_FreePlayModeMem();
    
    // Allocate memory used by loading
    GXI_AllocateLoadModeMem(_bHalfFrameRenderMode);
}



// Fixed image (error message)  -> one buffer is enough
void GXI_vUseOneBuffer()
{
	BOOL bDisableInterrupts;
	if (GXI_gbOneBufferMode)
		return;
		
	bDisableInterrupts = OSDisableInterrupts(); 
	GXI_gbOneBufferMode = TRUE;
#ifdef USE_TRIPPLE_BUFFERING	
	nbFramesInWait = 0;
	g_bSwitchFrames = 0;
	g_bSwitched = 0;
	myXFB1 = g_pstFrameBuffer1;
    myXFB2 = g_pstFrameBuffer1;
    dispXFB = g_pstFrameBuffer1;
    copyXFB = g_pstFrameBuffer1;
#endif // USE_TRIPPLE_BUFFERING        
	OSRestoreInterrupts(bDisableInterrupts); 
    
    if (g_pst_mode != &g_st_DefaultMode)
    {
	    g_pst_mode = &g_st_DefaultMode;
	    VIConfigure(g_pst_mode);
	    GXIInitGXWithRenderMode(g_pst_mode,FALSE);
	}
}

// Default mode (moving images).
void GXI_vUseTwoBuffers()
{
	u32 uBufferSize = (GXI_gbFullFrameMode ? GXI_MaxExternalFrameBufferSize : GXI_MaxExternalHalfFrameBufferSize);
	BOOL bDisableInterrupts;

	if (!GXI_gbOneBufferMode)
		return;

	bDisableInterrupts = OSDisableInterrupts(); 
	GXI_gbOneBufferMode = FALSE;
#ifdef USE_TRIPPLE_BUFFERING
	nbFramesInWait = 0;
	g_bSwitchFrames = 0;
	g_bSwitched = 0;

	myXFB1 = g_pstFrameBuffer1;
    myXFB2 = g_pstFrameBuffer2 = g_pstFrameBuffer1 + uBufferSize;
    dispXFB = myXFB1;
    copyXFB = myXFB2;
#endif // USE_TRIPPLE_BUFFERING    
	OSRestoreInterrupts(bDisableInterrupts); 

    // Set new render mode 
    if (!GXI_gbFullFrameMode)
    {
	    g_pst_mode = &g_st_HalfFrameMode;
    	VIConfigure(g_pst_mode);
	    GXIInitGXWithRenderMode(g_pst_mode,FALSE);
    }
}


