/*$T iopMain.h GC! 1.097 02/26/02 16:54:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#if defined(PSX2_IOP) || defined(PSX2_TARGET)
#ifndef __iopMain_h__
#define __iopMain_h__

/*$4
 ***********************************************************************************************************************
    thread config
 ***********************************************************************************************************************
 */

/*$2- thread index ---------------------------------------------------------------------------------------------------*/

#define ThIdxSupervisor 0
#define ThIdxRPCServer	1
#define ThIdxFileServer 2
#define ThIdxSoundEvent 3
#define ThIdxNumber		4

/*$2- stack size -----------------------------------------------------------------------------------------------------*/

#define IOP_Cte_ThStackSize_Supervisor	(10 * 1024)
#define IOP_Cte_ThStackSize_RPCServer	(10 * 1024)
#define IOP_Cte_ThStackSize_FileServer	(10 * 1024)
#define IOP_Cte_ThStackSize_SoundEvent	(10 * 1024)

/*$2- priority -------------------------------------------------------------------------------------------------------*/

#define IOP_Cte_ThPriority_Supervisor		26
#define IOP_Cte_ThPriority_RPCServer		48
#define IOP_Cte_ThPriority_FileServer		50
#define IOP_Cte_ThPriority_SoundEvent		100
#define IOP_Cte_ThPriority_SoundEventLow	105
#define IOP_Cte_ThPriority_SoundEventBin	55

/*$2- config type ----------------------------------------------------------------------------------------------------*/

#ifdef PSX2_IOP
typedef struct	IOP_tdst_ThreadConfig_
{
	struct ThreadParam	st_ThSettings;
	volatile int		vi_ThId;
	volatile int		vi_CurrentStackSize;
} IOP_tdst_ThreadConfig;

/*$4
 ***********************************************************************************************************************
    thread variables
 ***********************************************************************************************************************
 */

extern IOP_tdst_ThreadConfig	IOP_gav_ThCfg[ThIdxNumber];
extern volatile int				IOP_vi_StackSize[ThIdxNumber];
extern int						IOP_gi_RPC_Thread;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

int								start(void);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* PSX2_IOP */
#endif /* __iopMain_h__ */
#endif /* PSX2_IOP || PSX2_TARGET */
