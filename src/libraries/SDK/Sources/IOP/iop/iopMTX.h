/*$T iopMTX.h GC 1.138 09/08/03 11:05:10 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP
#ifndef __iopMTX_h__
#define __iopMTX_h__

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define iopMTX_Cte_VideoStream	0
#define iopMTX_Cte_SoundStream	1
#define iopMTX_Cte_Sound2Stream 2

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

void	iopMTX_InitModule(void);
void	iopMTX_CloseModule(void);
int		iopMTX_i_Open(struct RPC_tdst_VArg_FileStreamCreate_ *pVArgs);
void	iopMTX_Close(void);
void	iopMTX_Reinit(struct RPC_tdst_VArg_FileStreamReinit_ *pVArgs);
int		iopMTX_i_Read(int, unsigned int, char *, unsigned int, int);
BOOL	iopMTX_b_EnableReading(int i_voice);

extern volatile struct iopMTX_tdst_MuxStream_*iopMTX_gpst_MuxStream;

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* __iopMTX_h__ */
#endif /* PSX2_IOP */
