/*$T BIGmerge.h GC!1.52 11/05/99 15:50:46 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern BIG_tdst_BigFile BIG_gst1;
extern BIG_tdst_BigFile BIG_gst2;
extern BIG_tdst_BigFile BIG_gst3;
extern BIG_tdst_BigFile BIG_gst4;
extern BOOL             BIG_gb_NotCurrent;
extern BIG_KEY          BIG_gul_GlobalKey;
extern L_time_t         BIG_gx_GlobalTime;
extern ULONG			BIG_gx_GlobalClientRev;
extern char             *BIG_gpsz_GlobalName;

/*
 ===================================================================================================
    Macro to process with 2 bigfiles. This is used to open 2 bigfiles at the same time even if it's
    not really possible ! In fact, we used BIG_gst global struct and a temporary one, and swap
    between them when needed.
 ===================================================================================================
 */
#define SWAP_BIG() \
    { \
        BIG_tdst_BigFile    BIG_gstx; \
        L_memcpy(&BIG_gstx, &BIG_gst, sizeof(BIG_tdst_BigFile)); \
        L_memcpy(&BIG_gst, &BIG_gst1, sizeof(BIG_tdst_BigFile)); \
        L_memcpy(&BIG_gst1, &BIG_gstx, sizeof(BIG_tdst_BigFile)); \
        BIG_gb_NotCurrent = BIG_gb_NotCurrent ? 0 : 1; \
    }

/*
 ===================================================================================================
    Copy actual official bigfile into the temporary struct
 ===================================================================================================
 */
#define COPY_BIG() \
    { \
        L_memcpy(&BIG_gst1, &BIG_gst, sizeof(BIG_tdst_BigFile)); \
        BIG_Handle() = NULL; \
        BIG_gb_NotCurrent = 1; \
    }

#define COPY_BIG2() \
    { \
        L_memcpy(&BIG_gst2, &BIG_gst, sizeof(BIG_tdst_BigFile)); \
    }

#define COPY_BIG3() \
    { \
        L_memcpy(&BIG_gst3, &BIG_gst, sizeof(BIG_tdst_BigFile)); \
    }

#define COPY_BIG4() \
    { \
        L_memcpy(&BIG_gst4, &BIG_gst, sizeof(BIG_tdst_BigFile)); \
    }

#define RESTORE_BIG4() \
    { \
        L_memcpy(&BIG_gst, &BIG_gst4, sizeof(BIG_tdst_BigFile)); \
    }

#define COPY_BIG2TO1() \
    { \
        L_memcpy(&BIG_gst1, &BIG_gst2, sizeof(BIG_tdst_BigFile)); \
    }

#define COPY_BIG3TO1() \
    { \
        L_memcpy(&BIG_gst1, &BIG_gst3, sizeof(BIG_tdst_BigFile)); \
    }

/*
 ===================================================================================================
    Copy the temporary bigfile struct to the "official" one.
 ===================================================================================================
 */
#define RESTORE_BIG() \
    { \
        if(BIG_gb_NotCurrent) \
        { \
            L_memcpy(&BIG_gst, &BIG_gst1, sizeof(BIG_tdst_BigFile)); \
        } \
        BIG_gb_NotCurrent = 0; \
    }

#define RESTORE_BIG1() \
    { \
        L_memcpy(&BIG_gst, &BIG_gst2, sizeof(BIG_tdst_BigFile)); \
    }

extern BOOL         BIG_gb_MergeRecurse;

extern void         BIG_ExportMergeDirRec(char *, BIG_INDEX);
extern void         BIG_ExportMergeDir(char *, char *);
extern BIG_INDEX    BIG_ul_ExportMergeFileRec(char *, char *);
extern BIG_INDEX    BIG_ul_ExportMergeFile(char *, char *, char *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
