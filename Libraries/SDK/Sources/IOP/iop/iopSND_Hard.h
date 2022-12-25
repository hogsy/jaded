/*$T iopSND_Hard.h GC! 1.097 10/04/01 07:21:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP
#ifndef __iopSND_hard_h__
#define __iopSND_hard_h__

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2- state ----------------------------------------------------------------------------------------------------------*/

#define iopSND_Cte_HardFree		0x01000000
#define iopSND_Cte_HardPause	0x02000000
#define iopSND_Cte_HardPlay		0x04000000
#define iopSND_Cte_HardLock		0x00000001

/*$2- flag -----------------------------------------------------------------------------------------------------------*/

#define iopSND_Cte_HardStartedOnce	0x00000001
#define iopSND_Cte_HardUseFx	    0x00000002

/*$2- mask -----------------------------------------------------------------------------------------------------------*/

#define iopSND_Cte_HardStatusMask	0xFF000000
#define iopSND_Cte_HardFlagMask		(~iopSND_Cte_HardStatusMask)
#define iopSND_Cte_HardDynMask		(iopSND_Cte_HardPause|iopSND_Cte_HardPlay|iopSND_Cte_HardStartedOnce|iopSND_Cte_HardLock)


/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	iopSND_tdst_HardBuffer_
{
	int				i_Core;
	int				i_Voice;
	unsigned int	ui_Flag;
	unsigned int    ui_LoopPos;
	unsigned short  us_Pitch;
	unsigned short  us_VolLeft;
	unsigned short  us_VolRight;	
	unsigned short  us_Dummy;	
} iopSND_tdst_HardBuffer;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern volatile iopSND_tdst_HardBuffer	iopSND_sa_HardBuffer[SND_Cte_MaxHardBufferNb];
extern int								iopSND_gi_HardLock;
extern volatile int						iopSND_si_ActiveHardBuffer[iopSND_Cte_CoreNb];

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

void					iopSND_HardPlay(iopSND_tdst_HardBuffer *_pst_hbl, iopSND_tdst_HardBuffer *_pst_hbr);
void					iopSND_HardSetLoopPos(iopSND_tdst_HardBuffer *_pst_hb, int _i_loop_pos);
void					iopSND_HardSetStartPos(iopSND_tdst_HardBuffer *_pst_hb, int _i_loop_pos);
void					iopSND_HardSetPitch(iopSND_tdst_HardBuffer *_pst_hb, unsigned short us_pitch);
iopSND_tdst_HardBuffer	*iopSND_pst_HardAllocBuffer(int);
void					iopSND_HardSetVol
						(
							iopSND_tdst_HardBuffer	*_pst_hb,
							unsigned short			us_left,
							unsigned short			us_right
						);
int						iopSND_i_HardGetNAX(iopSND_tdst_HardBuffer *);
void					iopSND_HardFreeBuffer(iopSND_tdst_HardBuffer *_pst_hard_buffer);
void					iopSND_HardInitModule(void);
void					iopSND_HardStop(iopSND_tdst_HardBuffer *, iopSND_tdst_HardBuffer *);
int						iopSND_i_HardGetPlayingStatus(iopSND_tdst_HardBuffer *_pst_hb);
void                    iopSND_HardGetVol(iopSND_tdst_HardBuffer *_pst_hb, unsigned short* pus_left, unsigned short* pus_right);
void                    iopSND_HardSetPos(iopSND_tdst_HardBuffer *_pst_hbl, iopSND_tdst_HardBuffer *_pst_hbr, int i_Posd, int i_Pos);

#endif /* __SND_hard_h__ */
#endif /* PSX2_IOP */
