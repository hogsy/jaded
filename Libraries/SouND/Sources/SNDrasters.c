/*$T SNDrasters.c GC! 1.097 06/28/01 08:42:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
#ifdef PSX2_TARGET
#define _WINSOCK_H
#endif

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/PROfiler/PROdefs.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDstruct.h"

#ifdef ACTIVE_EDITORS
#include "LINks/LINKtoed.h"
#include "EDItors/Sources/SOuNd/SONmsg.h"
#endif

/*$4 macros */

#define SND_Cte_PS2RamSize	6465252 // => ((0x001C7FBF - 0x00005010) * 2 * 28) / 16
#define SND_Cte_MaxRamSize	6000000 // let some security space

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */
#ifdef SND_RASTER

int                  SND_gi_RasterCurrentFileId=0;
SND_tdst_Stat        SND_gst_InstanceStat;
SND_tdst_Stat        SND_gst_SInstanceStat;
SND_tdst_Stat		 SND_MemoryAllocStat[SND_IdxRasterNb];
BAS_tdst_barray		SND_gst_RamClustersSize;
BAS_tdst_barray		SND_gst_RamClustersCateg;
/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 init of rasters : 
 in engine mode One call during engine init
 in editor mode Two calls : during editor init and during project opening
 =======================================================================================================================
 */
void SND_RasterInitModule(void)
{
	SND_gi_RasterCurrentFileId=0;
	L_memset(SND_MemoryAllocStat, 0, sizeof(SND_tdst_Stat)*SND_IdxRasterNb);
    L_memset(&SND_gst_InstanceStat, 0, sizeof(SND_tdst_Stat));
    L_memset(&SND_gst_SInstanceStat, 0, sizeof(SND_tdst_Stat));
	BAS_binit(&SND_gst_RamClustersSize, 200);
	BAS_binit(&SND_gst_RamClustersCateg, 200);

	if(!SND_gst_Params.l_Available)  return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_RasterCloseModule(void)
{
	BAS_bfree(&SND_gst_RamClustersSize);
	BAS_bfree(&SND_gst_RamClustersCateg);
	SND_gi_RasterCurrentFileId=0;
	L_memset(SND_MemoryAllocStat, 0, sizeof(SND_tdst_Stat)*SND_IdxRasterNb);
    L_memset(&SND_gst_InstanceStat, 0, sizeof(SND_tdst_Stat));
    L_memset(&SND_gst_SInstanceStat, 0, sizeof(SND_tdst_Stat));    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_RamRasterAdd(int _i_ClusterId, int _i_ClusterSize)
{
	if(!SND_gst_Params.l_Available)  return;

	BAS_binsert(_i_ClusterId, _i_ClusterSize, &SND_gst_RamClustersSize);
	BAS_binsert(_i_ClusterId, SND_gi_RasterCurrentFileId, &SND_gst_RamClustersCateg);
    
    switch(SND_gi_RasterCurrentFileId)
    {
    case SND_Cul_SF_Music:
    case SND_Cul_SF_Dialog:
    case SND_Cul_SF_Ambience:
		SND_MemoryAllocStat[SND_IdxRasterStream].ui_Current += _i_ClusterSize;
        break;
    case SND_Cul_SF_LoadingSound:
		SND_MemoryAllocStat[SND_IdxRasterWac].ui_Current += _i_ClusterSize;
        break;
    default:
		SND_MemoryAllocStat[SND_IdxRasterSound].ui_Current += _i_ClusterSize;
        break;
	}
	SND_MemoryAllocStat[SND_IdxRasterTotal].ui_Current += _i_ClusterSize;
	SND_MemoryAllocStat[SND_IdxRasterTotal].ui_Max = SND_RamRasterGetFree();
	
	if(SND_Cte_MaxRamSize < SND_MemoryAllocStat[SND_IdxRasterTotal].ui_Current)
	{
		ERR_X_Warning(0, "[SND] No more space in the sound Ram", NULL);
	}

#ifdef ACTIVE_EDITORS
    LINK_SendMessageToEditors(ESON_MESSAGE_UPDATERASTER, 0, 0);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_RamRastersDel(int _i_ClusterId)
{
	/*~~*/
	int i, j;
	extern void BAS_bdeletegetval(ULONG, BAS_tdst_barray*, ULONG*);
	/*~~*/ 
	if(!SND_gst_Params.l_Available)  return;

	BAS_bdeletegetval(_i_ClusterId, &SND_gst_RamClustersCateg, (ULONG*)&i);
	BAS_bdeletegetval(_i_ClusterId, &SND_gst_RamClustersSize, (ULONG*)&j);
	if((i != -1) && (j!=-1))
	{
		switch(i)
		{ 
		case SND_Cul_SF_Music:
		case SND_Cul_SF_Dialog:
		case SND_Cul_SF_Ambience:
			SND_MemoryAllocStat[SND_IdxRasterStream].ui_Current -= j;
			break;
		case SND_Cul_SF_LoadingSound:
			SND_MemoryAllocStat[SND_IdxRasterWac].ui_Current -= j;
			break;
		default:
			SND_MemoryAllocStat[SND_IdxRasterSound].ui_Current -= j;
			break;
		}
		SND_MemoryAllocStat[SND_IdxRasterTotal].ui_Current -= j;
		SND_MemoryAllocStat[SND_IdxRasterTotal].ui_Max = SND_RamRasterGetFree();
	}
#ifdef ACTIVE_EDITORS
    LINK_SendMessageToEditors(ESON_MESSAGE_UPDATERASTER, 0, 0);
#endif
}

int SND_RamRasterGetFree(void)
{
	return (SND_Cte_MaxRamSize - SND_MemoryAllocStat[SND_IdxRasterTotal].ui_Current);
}

float SND_f_RamRasterGetUsedRatio(void)
{
	return ((float)SND_MemoryAllocStat[SND_IdxRasterTotal].ui_Current / (float)SND_Cte_MaxRamSize);
}

int SND_RamRasterGetTotal(void)
{
	return SND_MemoryAllocStat[SND_IdxRasterTotal].ui_Current;
}

#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

