/*$T eeRPC_Debug.c GC! 1.097 02/26/02 10:26:13 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
    header
 ***********************************************************************************************************************
 */

#include <sifdev.h>
#include <sifrpc.h>
#include <stdio.h>
#include <string.h>
#include <eekernel.h>
#include <libcdvd.h>

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATHfloat.h"

#include "IOP/ee/eeDebug.h"
#include "IOP/RPC_Manager.h"
#include "IOP/CDV_Manager.h"
#include "IOP/iop/iopMain.h"
#include "IOP/ee/eeRPC_Debug.h"

#include <eeregs.h>
#include "GSP.h"
#include "SouND/Sources/ps2/SPU.h"

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef PSX2_TARGET
#ifdef PSX2_IOP_RASTERIZE

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define eeRPC_Cte_DbgOff		0
#define eeRPC_Cte_DbgRAM		1
#define eeRPC_Cte_DbgStack		2
#define eeRPC_Cte_DbgSPURAM		3
#define eeRPC_Cte_DbgCPU		4
#define eeRPC_Cte_DbgNbOfFlush	5
#define eeRPC_Cte_DbgNbOfCmd	6
#define eeRPC_Cte_DbgSizeOfCmd	7
#define eeRPC_Cte_DbgAllRPC		8
#define eeRPC_Cte_DbgError		9

/*$4
 ***********************************************************************************************************************
    extern
 ***********************************************************************************************************************
 */

extern void					GSP_DrawGlobalRasters(char *Text, float FrameNumb, float YPos, float ARS, u_int TheColor);
extern void					GSP_DrawGlobalRastersArrays(void *Array, int Size, float YPos, u_int TheColor);
extern char					eeRPC_ac_BuffPrioHight[RPC_Cte_BuffSize_Srv]__attribute__((aligned(64)));
extern char					eeRPC_ac_BuffPrioNormal[RPC_Cte_BuffSize_Srv]__attribute__((aligned(64)));
extern char					*eeRPC_gp_CurrentCmdPrioHigth;
extern char					*eeRPC_gp_CurrentCmdPrioNormal;
static int					eeRPC_i_DbgGetCmdNbMax(int idxPrio);
static int					eeRPC_i_DbgGetCmdNb(int idxPrio);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

eeRPC_tdst_CmdScan			eeRPC_x_CmdRasters[RPC_Cte_IDNb][RPC_Cte_PrioNb];
eeRPC_tdst_CmdFlush			eeRPC_x_FlushRasters[RPC_Cte_PrioNb];
int							eeRPC_gi_Error;
tdstRaster					Ram[eeRPC_IdxTotal] =
{
	{ (float) IOP_Cte_ThStackSize_Supervisor, 1.0f, "Sup" },
	{ 0.0f, 0.0f, "" },
	{ (float) IOP_Cte_ThStackSize_RPCServer, 1.0f, "RPC" },
	{ 0.0f, 0.0f, "" },
	{ (float) IOP_Cte_ThStackSize_FileServer, 1.0f, "File" },
	{ 0.0f, 0.0f, "" },
	{ (float) IOP_Cte_ThStackSize_SoundEvent, 1.0f, "Snd" },
	{ 0.0f, 0.0f, "" },
	{ 0.0f, 0.0f, "Alloc" },
	{ 0.0f, 0.0f, "Sys" },
	{ (float) 2 * 1024 * 1024, 1.0f, "Free" },
	{ (float) (2 * 1024), 1.0f, "" },
	{ 0.0f, 0.0f, "" },
	{ 0.0f, 0.0f, "Alloc" },
	{ 0.0f, 0.0f, "" },
	{ 0.0f, 0.0f, "" },
	{ 0.0f, 0.0f, "Free" }
};

int						eeRPC_gi_CurrentDebugRq;
RPC_tdst_VArg_Check	    eeRPC_gst_CheckIOP;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_InitDebugModule(void)
{
	eeRPC_gi_CurrentDebugRq = 0;
	eeRPC_gi_Error = 0;
	L_memset(eeRPC_x_CmdRasters, 0, RPC_Cte_IDNb * RPC_Cte_PrioNb * sizeof(eeRPC_tdst_CmdScan));
	L_memset(eeRPC_x_FlushRasters, 0, RPC_Cte_PrioNb * sizeof(eeRPC_tdst_CmdFlush));

	eeRPC_gst_CheckIOP.ui_StackSizeOfSupervisorThread = IOP_Cte_ThStackSize_Supervisor;
	eeRPC_gst_CheckIOP.ui_StackSizeOfRPCThread = IOP_Cte_ThStackSize_RPCServer;
	eeRPC_gst_CheckIOP.ui_StackSizeOfSoundThread = IOP_Cte_ThStackSize_SoundEvent;
	eeRPC_gst_CheckIOP.ui_SysMemoryFreeMaxSize = 2 * 1024 * 1024;
	eeRPC_gst_CheckIOP.ui_MemoryAlloc = 0;
	eeRPC_gst_CheckIOP.ui_MemoryAllocMax = 2 * 1024 * 1024;
	eeRPC_gst_CheckIOP.i_MemoryFreeSpaceMin = 2 * 1024 * 1024;
	eeRPC_gst_CheckIOP.i_LastError=0;
	eeRPC_gst_CheckIOP.i_ErrorNumber=0;
	eeRPC_gst_CheckIOP.i_LastMapError=0;
	eeRPC_gst_CheckIOP.i_MapErrorNumber=0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_DebugSetRequest(int _i_rq)
{
	switch(_i_rq)
	{
	case eeRPC_Cte_DbgOff:			/* off */eeRPC_gi_CurrentDebugRq = eeRPC_Cte_DbgOff; break;
	case eeRPC_Cte_DbgRAM:			/* RAM */eeRPC_gi_CurrentDebugRq = eeRPC_Cte_DbgRAM; break;
	case eeRPC_Cte_DbgStack:		/* stack */eeRPC_gi_CurrentDebugRq = eeRPC_Cte_DbgStack; break;
	case eeRPC_Cte_DbgSPURAM:		/* SPU RAM */eeRPC_gi_CurrentDebugRq = eeRPC_Cte_DbgSPURAM; break;
	case eeRPC_Cte_DbgCPU:			/* CPU */eeRPC_gi_CurrentDebugRq = eeRPC_Cte_DbgCPU; break;
	case eeRPC_Cte_DbgNbOfFlush:	/* Nb OF Flush */eeRPC_gi_CurrentDebugRq = eeRPC_Cte_DbgNbOfFlush; break;
	case eeRPC_Cte_DbgNbOfCmd:		/* Nb of Cmd */eeRPC_gi_CurrentDebugRq = eeRPC_Cte_DbgNbOfCmd; break;
	case eeRPC_Cte_DbgSizeOfCmd:	/* Size of Cmd */eeRPC_gi_CurrentDebugRq = eeRPC_Cte_DbgSizeOfCmd; break;
	case eeRPC_Cte_DbgAllRPC:		/* All RPC */eeRPC_gi_CurrentDebugRq = eeRPC_Cte_DbgAllRPC; break;
	case eeRPC_Cte_DbgError:        /* errors */eeRPC_gi_CurrentDebugRq = eeRPC_Cte_DbgError; break;
	default:						break;
	}
}

extern BOOL GSP_gb_NoFloatingFlip;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_DebugGetStatus(int rr)
{
	/*~~~~~~~~~~~~~~~*/
	float	ypos;
	ULONG	Ytext;
	char	String[64];
	/*~~~~~~~~~~~~~~~*/

	GSP_gb_NoFloatingFlip = FALSE;
	switch(eeRPC_gi_CurrentDebugRq)
	{
	case eeRPC_Cte_DbgOff:
		break;

	case eeRPC_Cte_DbgRAM:	/* RAM */

		/*$1- IOP Ram ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GSP_gb_NoFloatingFlip = TRUE;
		ypos = 0.95f;

		Ytext = 16 + GspGlobal_ACCESS(Morfling);
		Ytext += 16;
		sprintf(String, "IOP RAM");
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "Alloc    = %d Ko", eeRPC_gst_CheckIOP.ui_MemoryAlloc / 1024);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "Free     = %d Ko", eeRPC_gst_CheckIOP.ui_SysMemoryFreeMaxSize / 1024);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "MaxAlloc = %d Ko", eeRPC_gst_CheckIOP.ui_MemoryAllocMax / 1024);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "MinFree  = %d Ko", eeRPC_gst_CheckIOP.i_MemoryFreeSpaceMin / 1024);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		break;

	case eeRPC_Cte_DbgStack:	/* stack */

		/*$1- stack sizes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GSP_gb_NoFloatingFlip = TRUE;
		ypos = 0.95f;

		Ram[eeRPC_IdxThStackSupervisor].f_Val = fLongToFloat(eeRPC_gst_CheckIOP.ui_StackSizeOfSupervisorThread);
		Ram[eeRPC_IdxThStackSupervisor].f_Relative = Ram[eeRPC_IdxThStackSupervisor].f_Val / (float) IOP_Cte_ThStackSize_Supervisor;
		Ram[eeRPC_IdxThStackUsedSupervisor].f_Val = (float) IOP_Cte_ThStackSize_Supervisor - Ram[eeRPC_IdxThStackSupervisor].f_Val;
		Ram[eeRPC_IdxThStackUsedSupervisor].f_Relative = 1.0f - Ram[eeRPC_IdxThStackSupervisor].f_Relative;
		GSP_DrawGlobalRastersArrays(&Ram[eeRPC_IdxThStackUsedSupervisor], 2, ypos /* ypos */ , 0xf0800080 /* color */ );
		ypos -= 0.06f;

		Ram[eeRPC_IdxThStackRPC].f_Val = fLongToFloat(eeRPC_gst_CheckIOP.ui_StackSizeOfRPCThread);
		Ram[eeRPC_IdxThStackRPC].f_Relative = Ram[eeRPC_IdxThStackRPC].f_Val / (float) IOP_Cte_ThStackSize_RPCServer;
		Ram[eeRPC_IdxThStackUsedRPC].f_Val = (float) IOP_Cte_ThStackSize_RPCServer - Ram[eeRPC_IdxThStackRPC].f_Val;
		Ram[eeRPC_IdxThStackUsedRPC].f_Relative = 1.0f - Ram[eeRPC_IdxThStackRPC].f_Relative;
		GSP_DrawGlobalRastersArrays(&Ram[eeRPC_IdxThStackUsedRPC], 2, ypos /* ypos */ , 0xf0800080 /* color */ );
		ypos -= 0.06f;

		Ram[eeRPC_IdxThStackSound].f_Val = fLongToFloat(eeRPC_gst_CheckIOP.ui_StackSizeOfSoundThread);
		Ram[eeRPC_IdxThStackSound].f_Relative = Ram[eeRPC_IdxThStackSound].f_Val / (float) IOP_Cte_ThStackSize_SoundEvent;
		Ram[eeRPC_IdxThStackUsedSound].f_Val = (float) IOP_Cte_ThStackSize_SoundEvent - Ram[eeRPC_IdxThStackSound].f_Val;
		Ram[eeRPC_IdxThStackUsedSound].f_Relative = 1.0f - Ram[eeRPC_IdxThStackSound].f_Relative;
		GSP_DrawGlobalRastersArrays(&Ram[eeRPC_IdxThStackUsedSound], 2, ypos /* ypos */ , 0xf0800080 /* color */ );
		ypos -= 0.06f;
		break;

	case eeRPC_Cte_DbgSPURAM:	/* SPU RAM */

		/*$1- SPU RAM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GSP_gb_NoFloatingFlip = TRUE;

		Ytext = 16 + GspGlobal_ACCESS(Morfling);
		Ytext += 16;
		sprintf(String, "SPU RAM");
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "Free = %d Ko", SPU_i_GetFreeSize() / 1024);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		break;

	case eeRPC_Cte_DbgCPU:	/* CPU */

		/*$1- CPU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		break;

	case eeRPC_Cte_DbgNbOfFlush:	/* Nb OF Flush */

		/*$1- RPC flush ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GSP_gb_NoFloatingFlip = TRUE;
		Ytext = 16 + GspGlobal_ACCESS(Morfling);
		Ytext += 32;
		sprintf(String, "NbOfFlushByFrame");
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "Hi    = %d", eeRPC_x_FlushRasters[0].i_FlushByFrame);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "Hi Max= %d", eeRPC_x_FlushRasters[0].i_FlushByFrameMax);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "N1    = %d", eeRPC_x_FlushRasters[1].i_FlushByFrame);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "N1 Max= %d", eeRPC_x_FlushRasters[1].i_FlushByFrameMax);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		break;

	case eeRPC_Cte_DbgNbOfCmd:	/* Nb of Cmd */

		/*$1- RPC flush size ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GSP_gb_NoFloatingFlip = TRUE;
		Ytext = 16 + GspGlobal_ACCESS(Morfling);
		Ytext += 32;
		sprintf(String, "NbOfCmdByFlush");
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "Hi    = %5d", eeRPC_i_DbgGetCmdNb(0));
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "Hi Max= %5d", eeRPC_i_DbgGetCmdNbMax(0));
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "N1    = %5d", eeRPC_i_DbgGetCmdNb(1));
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "N1 Max= %5d", eeRPC_i_DbgGetCmdNbMax(1));
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		break;

	case eeRPC_Cte_DbgSizeOfCmd:	/* Size of Cmd */

		/*$1- RPC count ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GSP_gb_NoFloatingFlip = TRUE;
		Ytext = 16 + GspGlobal_ACCESS(Morfling);
		Ytext += 32;
		sprintf(String, "SizeOfCmdByFlush");
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "Hi    = %5d", eeRPC_x_FlushRasters[0].i_SizeByFlush);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "Hi Max= %5d", eeRPC_x_FlushRasters[0].i_SizeByFlushMax);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "N1    = %5d", eeRPC_x_FlushRasters[1].i_SizeByFlush);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "N1 Max= %5d", eeRPC_x_FlushRasters[1].i_SizeByFlushMax);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		break;

    case eeRPC_Cte_DbgError:
		/*$1- error ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GSP_gb_NoFloatingFlip = TRUE;
		Ytext = 16 + GspGlobal_ACCESS(Morfling);
		Ytext += 32;
		sprintf(String, "Errors");
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "Nb       = %04X", eeRPC_gst_CheckIOP.i_ErrorNumber);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "LastCode = %04X", eeRPC_gst_CheckIOP.i_LastError);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "Nb in map= %04X", eeRPC_gst_CheckIOP.i_MapErrorNumber);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "LastCode = %04X", eeRPC_gst_CheckIOP.i_LastMapError);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
        break;
        
	case eeRPC_Cte_DbgAllRPC:	/* All RPC */

		/*$1- RPC flush ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GSP_gb_NoFloatingFlip = TRUE;
		Ytext = 16 + GspGlobal_ACCESS(Morfling);
		Ytext += 32;
		sprintf(String, "NbOfFlushByFrame");
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "Hi    = %5d", eeRPC_x_FlushRasters[0].i_FlushByFrame);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "Hi Max= %5d", eeRPC_x_FlushRasters[0].i_FlushByFrameMax);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "N1    = %5d", eeRPC_x_FlushRasters[1].i_FlushByFrame);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "N1 Max= %5d", eeRPC_x_FlushRasters[1].i_FlushByFrameMax);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		/*$1- RPC flush size ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		Ytext += 16;
		sprintf(String, "NbOfCmdByFlush");
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "Hi    = %5d", eeRPC_i_DbgGetCmdNb(0));
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "Hi Max= %5d", eeRPC_i_DbgGetCmdNbMax(0));
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "N1    = %5d", eeRPC_i_DbgGetCmdNb(1));
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "N1 Max= %5d", eeRPC_i_DbgGetCmdNbMax(1));
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		/*$1- RPC count ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		Ytext += 16;
		sprintf(String, "SizeOfCmdByFlush");
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "Hi    = %5d", eeRPC_x_FlushRasters[0].i_SizeByFlush);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "Hi Max= %5d", eeRPC_x_FlushRasters[0].i_SizeByFlushMax);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);

		Ytext += 16;
		sprintf(String, "N1    = %5d", eeRPC_x_FlushRasters[1].i_SizeByFlush);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		Ytext += 16;
		sprintf(String, "N1 Max= %5d", eeRPC_x_FlushRasters[1].i_SizeByFlushMax);
		GSP_DrawTExt
		(
			GspGlobal_ACCESS(Xsize) - (Gsp_Stp_GetLenght(String) + 10) * 8,
			Ytext,
			String,
			0xffffff,
			0x606060,
			GST_COPY,
			GST_SUB
		);
		break;

	default:

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		break;
	}

	return eeRPC_gi_CurrentDebugRq;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_DbgScanCommand(int _i_Command, void *_pv_InData, int _i_InSize, void *_pv_OutData, int _i_OutSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					idxModule;
	int					idxPrio;
	eeRPC_tdst_CmdScan	*p_Scan;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_i_Command & RPC_Msk_Priority)
	{
	case RPC_Cte_PrioHight:		idxPrio = 0; break;
	case RPC_Cte_PrioNormal:	idxPrio = 1; break;
	case RPC_Cte_PrioNormal2:	idxPrio = 2; break;
	default:					eeRPC_gi_Error++; return;
	}

	switch(_i_Command & RPC_Msk_ModuleId)
	{
	case RPC_Cte_IDDbg:		idxModule = 0; break;
	case RPC_Cte_IDCDV:		idxModule = 1; break;
	case RPC_Cte_IDFile:	idxModule = 2; break;
	case RPC_Cte_IDSnd:		idxModule = 3; break;
	case RPC_Cte_IDRPC:		idxModule = 4; break;
	default:				eeRPC_gi_Error++; return;
	}

	p_Scan = &eeRPC_x_CmdRasters[idxModule][idxPrio];
	p_Scan->i_CmdCount++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_DbgScanFlush(char *_p_Buff, char *_p_Current)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	eeRPC_tdst_CmdFlush *p_Flush;
	int					size;
	int					idxModule;
	int					idxPrio;
	eeRPC_tdst_CmdScan	*p_Scan;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_p_Buff == eeRPC_ac_BuffPrioHight)
	{
		p_Flush = &eeRPC_x_FlushRasters[0];
		size = (int) (eeRPC_gp_CurrentCmdPrioHigth - eeRPC_ac_BuffPrioHight);
		idxPrio = 0;
	}
	else if(_p_Buff == eeRPC_ac_BuffPrioNormal)
	{
		p_Flush = &eeRPC_x_FlushRasters[1];
		size = (int) (eeRPC_gp_CurrentCmdPrioNormal - eeRPC_ac_BuffPrioNormal - 4);
		idxPrio = 1;
	}
	else
	{
		eeRPC_gi_Error++;
		return;
	}

	p_Flush->i_Flush++;
	p_Flush->i_SizeByFlush = size;
	if(p_Flush->i_SizeByFlushMax < p_Flush->i_SizeByFlush) p_Flush->i_SizeByFlushMax = p_Flush->i_SizeByFlush;

	for(idxModule = 0; idxModule < RPC_Cte_IDNb; idxModule++)
	{
		p_Scan = &eeRPC_x_CmdRasters[idxModule][idxPrio];

		p_Scan->i_CmdCountByFlush = p_Scan->i_CmdCount;
		p_Scan->i_CmdCount = 0;

		if(p_Scan->i_CmdCountByFlushMax < p_Scan->i_CmdCountByFlush)
			p_Scan->i_CmdCountByFlushMax = p_Scan->i_CmdCountByFlush;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_DbgScanUpdate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					idxPrio;
	eeRPC_tdst_CmdFlush *p_Flush;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* rasters refresh */
	if((eeRPC_gi_CurrentDebugRq == eeRPC_Cte_DbgError) ||(eeRPC_gi_CurrentDebugRq == eeRPC_Cte_DbgRAM) || (eeRPC_gi_CurrentDebugRq == eeRPC_Cte_DbgStack))
	{
		eeRPC_i_PushCommand(RPC_Cmd_Check, NULL, 0, &eeRPC_gst_CheckIOP, sizeof(RPC_tdst_VArg_Check));
	}

	for(idxPrio = 0; idxPrio < RPC_Cte_PrioNb; idxPrio++)
	{
		p_Flush = &eeRPC_x_FlushRasters[idxPrio];

		p_Flush->i_FlushByFrame = p_Flush->i_Flush;
		p_Flush->i_Flush = 0;
		if(p_Flush->i_FlushByFrameMax < p_Flush->i_FlushByFrame) p_Flush->i_FlushByFrameMax = p_Flush->i_FlushByFrame;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int eeRPC_i_DbgGetCmdNb(int idxPrio)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					idxModule;
	eeRPC_tdst_CmdScan	*p_Scan;
	int					cmd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	cmd = 0;
	for(idxModule = 0; idxModule < RPC_Cte_IDNb; idxModule++)
	{
		p_Scan = &eeRPC_x_CmdRasters[idxModule][idxPrio];

		cmd += p_Scan->i_CmdCountByFlush;
	}

	return cmd;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int eeRPC_i_DbgGetCmdNbMax(int idxPrio)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					idxModule;
	eeRPC_tdst_CmdScan	*p_Scan;
	int					cmd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	cmd = 0;
	for(idxModule = 0; idxModule < RPC_Cte_IDNb; idxModule++)
	{
		p_Scan = &eeRPC_x_CmdRasters[idxModule][idxPrio];

		cmd += p_Scan->i_CmdCountByFlushMax;
	}

	return cmd;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* _FINAL_ */
#endif /* PSX2_TARGET */
