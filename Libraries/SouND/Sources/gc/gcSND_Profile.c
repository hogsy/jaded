/*$T gcSND_Profile.c GC! 1.097 06/03/02 17:37:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
#include <dolphin/ax.h>

#include "CLIbrary/CLIstr.h"
#include "MATerial/MATstruct.h"

#include "gc/gcSND_Profile.h"

#ifdef GC_ENABLE_SOUND_PROFILER

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define gcSND_Cte_MaxProfileFrames	15

/*$4 types */
typedef struct gcSND_tdst_Profiler_
{
    BOOL b_IsActive;
    
    u32 ui_AvgTotal;
    u32 ui_AvgLibAx;
    u32 ui_AvgLibFx;
    u32 ui_AvgUser;
    u32 ui_AvgVoiceNb;
    u32 ui_Nb;

    u32 ui_MaxTotal;
    u32 ui_MaxLibAx;
    u32 ui_MaxLibFx;
    u32 ui_MaxUser;
    u32 ui_MaxVoiceNb;
    
    u32 ui_LastDspCycles;
    u32 ui_MaxLastDspCycles;
    u32 ui_MaxDspCycles;
} gcSND_tdst_Profiler;
/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

static AXPROFILE	gcSND_gax_Profiler[gcSND_Cte_MaxProfileFrames];
static gcSND_tdst_Profiler gcSND_gst_Profiler;
/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_InitProfilerModule(void)
{
	L_memset(gcSND_gax_Profiler, 0, gcSND_Cte_MaxProfileFrames * sizeof(AXPROFILE));
	L_memset(&gcSND_gst_Profiler, 0, sizeof(gcSND_tdst_Profiler));
	gcSND_gst_Profiler.b_IsActive = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_CloseProfilerModule(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_ProfilerEnable(void)
{
	if(gcSND_gst_Profiler.b_IsActive) return;
	AXInitProfile(gcSND_gax_Profiler, gcSND_Cte_MaxProfileFrames);
	gcSND_gst_Profiler.b_IsActive = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_ProfilerDisable(void)
{
	gcSND_gst_Profiler.b_IsActive = FALSE;
}

BOOL gcSND_b_ProfilerIsEnable(void)
{
    return gcSND_gst_Profiler.b_IsActive;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_UpdateProfiler(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	BOOL	old;
	u32		i;
	u32		cpuCycles;
	u32		userCycles;
	u32		axCycles;
	u32		auxCycles;
	u32		voices;
	u32		AcpuCycles;
	u32		AuserCycles;
	u32		AaxCycles;
	u32		AauxCycles;
	u32		Avoices;
	u32 nb;


	if(!gcSND_gst_Profiler.b_IsActive) return;

	old = OSDisableInterrupts();

	nb=i = AXGetProfile();
	AcpuCycles=0;
	AuserCycles=0;
	AaxCycles=0;
	AauxCycles=0;
	Avoices=0;

	if(i)
	{
		/*
		 * up to 4 audio frames can complete within a 60Hz video frame £
		 * so spin thru the accumulated audio frame profiles and find the peak values
		 */
		while(i)
		{
			i--;

			cpuCycles = (u32) (gcSND_gax_Profiler[i].axFrameEnd - gcSND_gax_Profiler[i].axFrameStart);
			auxCycles = (u32) (gcSND_gax_Profiler[i].auxProcessingEnd - gcSND_gax_Profiler[i].auxProcessingStart);
			userCycles = (u32) (gcSND_gax_Profiler[i].userCallbackEnd - gcSND_gax_Profiler[i].userCallbackStart);
			axCycles = cpuCycles - userCycles;
			voices = gcSND_gax_Profiler[i].axNumVoices;

			/* find peak values over the last i audio frames */
			if(cpuCycles > gcSND_gst_Profiler.ui_MaxTotal) gcSND_gst_Profiler.ui_MaxTotal = cpuCycles;
			if(userCycles > gcSND_gst_Profiler.ui_MaxUser) gcSND_gst_Profiler.ui_MaxUser = userCycles;
			if(axCycles > gcSND_gst_Profiler.ui_MaxLibAx) gcSND_gst_Profiler.ui_MaxLibAx = axCycles;
			if(auxCycles > gcSND_gst_Profiler.ui_MaxLibFx) gcSND_gst_Profiler.ui_MaxLibFx = auxCycles;
			if(voices > gcSND_gst_Profiler.ui_MaxVoiceNb) gcSND_gst_Profiler.ui_MaxVoiceNb = voices;

			
			/**/
        	AcpuCycles += cpuCycles;
        	AuserCycles+= userCycles;
        	AaxCycles+=axCycles;
        	AauxCycles+=auxCycles;
        	Avoices+=voices;
		}
		//OSRestoreInterrupts(old);
		
		
		gcSND_gst_Profiler.ui_AvgTotal = AcpuCycles;
		gcSND_gst_Profiler.ui_AvgLibAx = AaxCycles ;
		gcSND_gst_Profiler.ui_AvgLibFx = AauxCycles;
		gcSND_gst_Profiler.ui_AvgUser = AuserCycles;
		gcSND_gst_Profiler.ui_AvgVoiceNb = Avoices;
		gcSND_gst_Profiler.ui_Nb =  nb;
		
	}
	
	gcSND_gst_Profiler.ui_LastDspCycles =AXGetDspCycles();
	if(gcSND_gst_Profiler.ui_LastDspCycles > gcSND_gst_Profiler.ui_MaxLastDspCycles) gcSND_gst_Profiler.ui_MaxLastDspCycles = gcSND_gst_Profiler.ui_LastDspCycles;
	gcSND_gst_Profiler.ui_MaxDspCycles =AXGetMaxDspCycles();

	OSRestoreInterrupts(old);
}

void gcSND_ProfilerDisplay(f32 Y)
{
    
    extern void GXI_DrawText(int X,int Y,char *P_String,long Color,long ColorBack,long TFront,long TBack);
	char 		asz_Log[512];
	float L=24.0f;
	float coeff;
	
	
	if(!gcSND_gst_Profiler.b_IsActive) return;
	
	Y = Y - 12*L;
	coeff = ((1.0f/60.0f)/0.005f)*(1.0f/50000.0f);


	sprintf(asz_Log,"DSP Max %5.2f %", (100.0f*(f32)gcSND_gst_Profiler.ui_MaxLastDspCycles)/(f32)gcSND_gst_Profiler.ui_MaxDspCycles);		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;
	sprintf(asz_Log,"DSP     %5.2f %", (100.0f*(f32)gcSND_gst_Profiler.ui_LastDspCycles)/(f32)gcSND_gst_Profiler.ui_MaxDspCycles);		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;
	
	sprintf(asz_Log,"--- --- ---");
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"[Total] %5.2f %", coeff*(f32)OSTicksToNanoseconds(gcSND_gst_Profiler.ui_MaxTotal));		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"[User ] %5.2f",coeff*(f32) OSTicksToNanoseconds(gcSND_gst_Profiler.ui_MaxUser));		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"[Ax   ] %5.2f",coeff*(f32) OSTicksToNanoseconds(gcSND_gst_Profiler.ui_MaxLibAx));		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"[Aux  ] %5.2f",coeff*(f32) OSTicksToNanoseconds(gcSND_gst_Profiler.ui_MaxLibFx));		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"[Voice] %5d", gcSND_gst_Profiler.ui_MaxVoiceNb);		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"--- MAX ---");
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;
	
	

	sprintf(asz_Log,"[Total] %5.2f %", (coeff*(f32)OSTicksToNanoseconds(gcSND_gst_Profiler.ui_AvgTotal)) / (f32)(gcSND_gst_Profiler.ui_Nb));		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"[User ] %5.2f",(coeff*(f32) OSTicksToNanoseconds(gcSND_gst_Profiler.ui_AvgUser)) / (f32)(gcSND_gst_Profiler.ui_Nb));		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"[Ax   ] %5.2f",(coeff*(f32) OSTicksToNanoseconds(gcSND_gst_Profiler.ui_AvgLibAx)) / (f32)(gcSND_gst_Profiler.ui_Nb));		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"[Aux  ] %5.2f",(coeff*(f32) OSTicksToNanoseconds(gcSND_gst_Profiler.ui_AvgLibFx)) / (f32)(gcSND_gst_Profiler.ui_Nb));		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"[Voice] %5d", gcSND_gst_Profiler.ui_AvgVoiceNb/gcSND_gst_Profiler.ui_Nb);		
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;

	sprintf(asz_Log,"--- AVG ---");
	GXI_DrawText(8L,(LONG)(0.75 * Y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
	Y += L;
}

void gcSND_ProfilerReset(void)
{
    gcSND_gst_Profiler.ui_MaxTotal = 0;
    gcSND_gst_Profiler.ui_MaxLibAx = 0 ;
    gcSND_gst_Profiler.ui_MaxLibFx = 0;
    gcSND_gst_Profiler.ui_MaxUser = 0;
    gcSND_gst_Profiler.ui_MaxVoiceNb = 0;

    gcSND_gst_Profiler.ui_AvgTotal = 0;
    gcSND_gst_Profiler.ui_AvgLibAx = 0 ;
    gcSND_gst_Profiler.ui_AvgLibFx = 0;
    gcSND_gst_Profiler.ui_AvgUser = 0;
    gcSND_gst_Profiler.ui_AvgVoiceNb = 0;
    gcSND_gst_Profiler.ui_Nb =  1;

}
/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* GC_ENABLE_SOUND_PROFILER */
#endif /* _GAMECUBE */
