/*$T TEXprocedural.h GC! 1.081 03/06/02 09:46:10 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/* Name : TEXstruct.h Description : */
#ifndef __TEXBINK_H__
#define __TEXBINK_H__

#ifndef PSX2_TARGET
#pragma once
#endif

#include "BINK/Bink.H"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#ifndef NO_BINK
#ifdef _GAMECUBE
#define BINK_VIDEO

/*
	Asynchronous Load
*/
#define BJ_AL_RDY_FOR_STR	0
#define BJ_AL_RDY_FOR_CPU	1
#define BJ_AL_USED_BY_STR	2
#define BJ_AL_USED_BY_TRASH	3

#define BJ_AL_BufferSizePo2 12
#define BJ_AL_BufferSize (1 << BJ_AL_BufferSizePo2)

typedef struct BJ_AL_Buffer_
{
	char *Buffer;
	volatile 		ULONG State;
	struct BJ_AL_Buffer_ *pNextBuffer;
#ifdef _GAMECUBE
	ULONG	SHNum;
#endif	
} BJ_AL_Buffer;



typedef struct	TEX_tdst_Bink_
{
	HBINK 			Bink;
	ULONG 			BinkKey;
	struct TEX_tdst_Procedural_ *_pst_Pro;
	ULONG 			BinkOffset;
	ULONG			FileSize;
	ULONG			AudioTrackID;
	
	ULONG 			ulActualPosInCpuBuffer;
	ULONG			ulMinimumBuffer;
	BJ_AL_Buffer	*pCPU_Buffer;
	BJ_AL_Buffer	*pSTR_Buffer;
	void 			*pBigAllocation;
} TEX_tdst_Bink;

void BINK_JADE_Create(TEX_tdst_Bink *TB,ULONG ul_BINK_Key);
void BINK_JADE_Update(TEX_tdst_Bink *TB);
void BINK_JADE_Restart(TEX_tdst_Bink *TB);
void BINK_JADE_Destroy(TEX_tdst_Bink *TB);

#endif

#endif // NO_BINK
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __TEXBINK_H__ */

