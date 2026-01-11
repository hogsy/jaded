/*$T BIGio.h GC! 1.100 07/03/01 16:06:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#include "BASe/CLIbrary/CLIfile.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


#if defined( _XBOX )
#include "BASe/XBox/XBCompositeFile.h"
#endif

#if defined( _XBOX )
    typedef XBCompositeFile* BIGFileHandle;
#else
    typedef L_FILE BIGFileHandle;
#endif


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define PRELOAD_MAGIC	0x1ABCDEF1
#define PRELOAD_GRAN	30 * 1024
#define PRELOAD_MAXTAB	100

extern int	*gap_PreLoadArray[PRELOAD_MAXTAB];
extern int	*gap_PreLoadArrayPtrFree[PRELOAD_MAXTAB];
extern int	gai_PreLoadArrayOk[PRELOAD_MAXTAB];
extern int	gai_PreLoadArrayFree[PRELOAD_MAXTAB];
extern int	gi_PreLoadNum;
extern int	*gpi_PreLoadBuf;
extern int	*gpi_PreLoadBufPtrFree;
#ifdef PSX2_TARGET
extern int	gi_PreLoadFinished;
#else
extern volatile int	gi_PreLoadFinished;
#endif
extern void PreLoad_Finished(int *);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef BIGio_M_ActiveRasters
void		BIGio_InitStat(void);
void		BIGio_PrintRasters(void);
#else
#define BIGio_InitStat()
#define BIGio_PrintRasters()
#endif
extern int	BIG_fread(void *, int, BIGFileHandle);
extern unsigned int BIG_ftell(BIGFileHandle);
extern int	BIG_fseek(BIGFileHandle _hfile,unsigned int _offset, int _origin);
void BIG_UpdateCache(void);

#ifdef ACTIVE_EDITORS
extern int	BIG_fwrite(void *, int, L_FILE);
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
