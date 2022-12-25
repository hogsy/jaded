/*$T PROPS2.h GC! 1.097 12/15/00 10:50:23 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __PROPS2_H__
#define __PROPS2_H__

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/* #include "TIMer/PROfiler/PROdefs.h" */
#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    public macros
 ***********************************************************************************************************************
 */

#define PROPS2_M_RasterNameSize 32

/*$4
 ***********************************************************************************************************************
    private macros
 ***********************************************************************************************************************
 */

#ifdef __PROPS2_C__
#define __EXTERN
#else
#define __EXTERN	extern
#endif

/*$4
 ***********************************************************************************************************************
    public types
 ***********************************************************************************************************************
 */

#include "BASe/BAStypes.h"

typedef struct			PROPS2_tdst_RasterStatistics_
{
	u_long64	ul_Value;
	u_long64	ul_Max;
	u_long64	ul_Min;
	u_long64	ul_Ave;
} PROPS2_tdst_RasterStatistics;

typedef struct	PROPS2_tdst_Raster_
{
	struct PROPS2_tdst_Raster_		*pst_NextCell;
	u_long64						ul_Criterion;
	struct PROPS2_tdst_Raster_		*pst_NextRaster;
	struct PROPS2_tdst_Raster_		*pst_NextCalled;
	u_long64						ul_Nb;
	PROPS2_tdst_RasterStatistics	st_pc1;
	PROPS2_tdst_RasterStatistics	st_pc0;
	long64							l_StartCount;
	char							asz_Name[PROPS2_M_RasterNameSize];
} PROPS2_tdst_Raster;

/*$4
 ***********************************************************************************************************************
    public variables
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET
//#define PROPS2_M_ActiveRasters
#endif

#ifdef _FINAL_
#ifdef PROPS2_M_ActiveRasters
#undef PROPS2_M_ActiveRasters
#endif 
#endif // _FINAL_


#ifdef PROPS2_M_ActiveRasters
__EXTERN PROPS2_tdst_Raster PROPS2_gst_none;
#define __M_Macro__(a, b, c)	__EXTERN PROPS2_tdst_Raster	   PROPS2_gst_##a;
#include "TIMer/PROfiler/PROPS2_def.h"
#undef __M_Macro__

/*$4
 ***********************************************************************************************************************
    public prototypes
 ***********************************************************************************************************************
 */

#include <libpc.h>
void	PROPS2_Init(int);
#define PROPS2_StartRaster(a)	{ scePcStop(); _PROPS2_StartRaster(a); }
#define PROPS2_StopRaster(a)	{ scePcStop(); _PROPS2_StopRaster(a); }
void	_PROPS2_StartRaster(PROPS2_tdst_Raster *);
void	_PROPS2_StopRaster(PROPS2_tdst_Raster *);

#define PROPS2_StopAll()	{ scePcStop(); _PROPS2_StopAll(); }
void _PROPS2_StopAll(void);
void PROPS2_StartAll(void);

void	PROPS2_PrintRaster(void);
void	PROPS2_PrintInactiveRaster(void);
void	PROPS2_ChangeDisplay(void);
void	PROPS2_InitRaster(PROPS2_tdst_Raster *_pst_raster, char *_asz_name);
void	PROPS2_PrintChain(PROPS2_tdst_Raster *_pRaster);
void	PROPS2_MakeLink
		(
			PROPS2_tdst_Raster	*_pst_raster,
			PROPS2_tdst_Raster	*_pst_next,
			PROPS2_tdst_Raster	*_pst_called
		);

#else
#define PROPS2_Init(a)
#define PROPS2_StartRaster(a)
#define PROPS2_StopRaster(a)
#define PROPS2_StartAll()
#define PROPS2_StopAll()
#define PROPS2_PrintRaster()
#define PROPS2_PrintInactiveRaster()
#define PROPS2_ChangeDisplay()
#define PROPS2_InitRaster(a, b)
#define PROPS2_PrintChain(a)
#define PROPS2_MakeLink(a, b, c)
#endif
#undef __EXTERN
#ifdef __cplusplus
}
#endif
#endif /* __PROPS2_H__ */
