/*$T eeRPC_Debug.h GC! 1.097 11/28/01 16:49:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __eeRPC_Debug_h__
#define __eeRPC_Debug_h__
#ifdef PSX2_TARGET
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _FINAL_
#define PSX2_IOP_RASTERIZE
#endif

#ifdef PSX2_IOP_RASTERIZE
#include "IOP/iop/iopMain.h"
#include "IOP/RPC_Manager.h"
/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define eeRPC_IdxThStackUsedSupervisor	0
#define eeRPC_IdxThStackSupervisor		(eeRPC_IdxThStackUsedSupervisor + 1)

#define eeRPC_IdxThStackUsedRPC			(eeRPC_IdxThStackSupervisor + 1)
#define eeRPC_IdxThStackRPC				(eeRPC_IdxThStackUsedRPC + 1)

#define eeRPC_IdxThStackUsedFile		(eeRPC_IdxThStackRPC + 1)
#define eeRPC_IdxThStackFile			(eeRPC_IdxThStackUsedFile + 1)

#define eeRPC_IdxThStackUsedSound		(eeRPC_IdxThStackFile + 1)
#define eeRPC_IdxThStackSound			(eeRPC_IdxThStackUsedSound + 1)

#define eeRPC_IdxAlloc					(eeRPC_IdxThStackSound + 1)
#define eeRPC_IdxSysAlloc				(eeRPC_IdxAlloc + 1)
#define eeRPC_IdxFree					(eeRPC_IdxAlloc + 2)

#define eeRPC_IdxSPUSys					(eeRPC_IdxFree + 1)
#define eeRPC_IdxSPUFx					(eeRPC_IdxSPUSys + 1)
#define eeRPC_IdxSPUAlloc				(eeRPC_IdxSPUSys + 2)
#define eeRPC_IdxSPUHoles				(eeRPC_IdxSPUSys + 3)
#define eeRPC_IdxSPUFree				(eeRPC_IdxSPUSys + 4)
#define eeRPC_IdxSPUDefrag				(eeRPC_IdxSPUSys + 5)

#define eeRPC_IdxTotal					(eeRPC_IdxSPUDefrag + 1)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	tdstRaster_
{
	float	f_Val;
	float	f_Relative;
	char	asz_Name[8];
} tdstRaster;

typedef struct	eeRPC_tdst_CmdScan_
{
	int i_ErrorCount;

	int i_CmdCountByFlush;
	int i_CmdCountByFlushMax;
	int i_CmdCount;
} eeRPC_tdst_CmdScan;

typedef struct	eeRPC_tdst_CmdFlush_
{
	int i_FlushByFrame;
	int i_FlushByFrameMax;
	int i_Flush;

	int i_SizeByFlush;
	int i_SizeByFlushMax;
} eeRPC_tdst_CmdFlush;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern eeRPC_tdst_CmdScan	eeRPC_x_CmdRasters[RPC_Cte_IDNb][RPC_Cte_PrioNb];
extern eeRPC_tdst_CmdFlush	eeRPC_x_FlushRasters[RPC_Cte_PrioNb];
extern int					eeRPC_gi_Error;
extern tdstRaster			Ram[eeRPC_IdxTotal];
extern int					eeRPC_gai_CheckIOP[ThIdxNumber + 8];
extern int					eeRPC_gi_CurrentDebugRq;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

void						eeRPC_DebugSetRequest(int _i_rq);
int							eeRPC_i_DebugGetStatus(int rr);
void						eeRPC_DbgScanCommand
							(
								int		_i_Command,
								void	*_pv_InData,
								int		_i_InSize,
								void	*_pv_OutData,
								int		_i_OutSize
							);
void						eeRPC_DbgScanFlush(char *_p_Buff, char *_p_Current);
void						eeRPC_DbgScanUpdate(void);
void						eeRPC_InitDebugModule(void);

#else	/* FINAL */
#define eeRPC_DebugSetRequest(_i_rq)
#define eeRPC_i_DebugGetStatus(rr) (0)
#define eeRPC_DbgScanCommand(_i_Command, _pv_InData, _i_InSize, _pv_OutData, _i_OutSize)
#define eeRPC_DbgScanFlush(_p_Buff, _p_Current)
#define eeRPC_DbgScanUpdate()
#define eeRPC_InitDebugModule()

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* _FINAL_ */
#ifdef __cplusplus
}
#endif
#endif /* PSX2_TARGET */
#endif /* __eeRPC_Debug_h__ */
