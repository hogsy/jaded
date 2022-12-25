/*$T SNDrasters.h GC 1.138 09/04/03 10:38:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDrasters_h__
#define __SNDrasters_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#ifndef _FINAL_
#define SND_RASTER
#endif

#ifdef SND_RASTER

/*$5
 #######################################################################################################################
    rasterize
 #######################################################################################################################
 */

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define SND_M_RasterRegisterFile(__key) SND_gi_RasterCurrentFileId = __key
#define SND_IdxRasterStream				0
#define SND_IdxRasterWac				1
#define SND_IdxRasterSound				2
#define SND_IdxRasterTotal				3
#define SND_IdxRasterNb					4

/*$4
 ***********************************************************************************************************************
    struct
 ***********************************************************************************************************************
 */

typedef struct SND_tdst_RamRaster_
{
	int i_ClusterId;
	int i_ClusterSize;
	int i_FileId;
	int i_FileCateg;
}
SND_tdst_RamRaster;

typedef struct	SND_tdst_Stat_
{
	unsigned int	ui_Current;
	unsigned int	ui_Max;
} SND_tdst_Stat;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */
extern int					SND_gi_RasterCurrentFileId;
extern SND_tdst_Stat		SND_gst_InstanceStat;
extern SND_tdst_Stat		SND_gst_SInstanceStat;
extern SND_tdst_Stat		SND_MemoryAllocStat[SND_IdxRasterNb];

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */
int SND_RamRasterGetFree(void);
int SND_RamRasterGetTotal(void);

void	SND_RasterInitModule(void);
void	SND_RasterCloseModule(void);
void	SND_RamRasterAdd(int _i_ClusterId, int _i_ClusterSize);
void	SND_RamRastersDel(int _i_ClusterId);
#define SND_StatAddInstance() \
	do \
	{ \
		SND_gst_InstanceStat.ui_Current++; \
		if(SND_gst_InstanceStat.ui_Max < SND_gst_InstanceStat.ui_Current) \
			SND_gst_InstanceStat.ui_Max = SND_gst_InstanceStat.ui_Current; \
	} while(0)
#define SND_StatSubInstance() \
	do \
	{ \
		SND_gst_InstanceStat.ui_Current--; \
	} while(0)
#define SND_StatAddSInstance() \
	do \
	{ \
		SND_gst_SInstanceStat.ui_Current++; \
		if(SND_gst_SInstanceStat.ui_Max < SND_gst_SInstanceStat.ui_Current) \
			SND_gst_SInstanceStat.ui_Max = SND_gst_SInstanceStat.ui_Current; \
	} while(0)
#define SND_StatSubSInstance() \
	do \
	{ \
		SND_gst_SInstanceStat.ui_Current--; \
	} while(0)
#else

/*$5
 #######################################################################################################################
    does't rasterize
 #######################################################################################################################
 */

#define SND_M_RasterRegisterFile(__key)
#define SND_RasterInitModule()
#define SND_RasterCloseModule()
#define SND_RamRasterAdd(_i_ClusterId, _i_ClusterSize)
#define SND_RamRastersDel(_i_ClusterId)
#define SND_StatAddInstance()
#define SND_StatSubInstance()
#define SND_RamRasterGetFree() 0
#define SND_RamRasterGetTotal() 0
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
			}
#endif
#endif /* __SNDfx_h__ */
