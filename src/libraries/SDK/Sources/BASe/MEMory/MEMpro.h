/*$T MEMpro.h GC! 1.081 06/22/00 17:54:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __MEMPRO_H__
#define __MEMPRO_H__

#pragma once

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#include "BASe/BAStypes.h"

#ifndef _GAMECUBE
#  include <malloc.h>
#endif
#include "BASe/BASsys.h"

#if defined(ACTIVE_EDITORS) 
#  define MEMpro_M_ActiveRasters 
#endif

#if defined(PSX2_TARGET)
//#define MEMpro_M_ActiveRasters 
#endif

#if defined( _FINAL_ ) && defined( MEMpro_M_ActiveRasters )
#  undef MEMpro_M_ActiveRasters
#endif // _FINAL_

/*$4
 ***********************************************************************************************************************
    private macro
 ***********************************************************************************************************************
 */

#ifdef __MEMPRO_C__
#  define __EXTERN
#else
#  define __EXTERN	extern
#endif

/*$4
 ***********************************************************************************************************************
    public macros
 ***********************************************************************************************************************
 */



/*$4
 ***********************************************************************************************************************
    public types
 ***********************************************************************************************************************
 */

#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j) MEMpro_Id_##f, 
	
/* rasters list */
typedef enum	MEMpro_tden_MemRasterId_
{
	MEMpro_Id_BIG_Open=0,
	MEMpro_Id_LOA_ResolveArray,
	MEMpro_Id_GDI_fnpst_CreateDisplayData_1,
	MEMpro_Id_OGL_ul_Texture_Create,
	MEMpro_Id_ANI_Load,
	MEMpro_Id_EVE_ListEvents,
	MEMpro_Id_AI_Models,
	MEMpro_Id_AI_Functions,
	MEMpro_Id_AI_Vars,
	MEMpro_Id_GDK_ObjectCallback,
	MEMpro_Id_GDK_MultiTexture,
	MEMpro_Id_GDK_SingleTexture,
	MEMpro_Id_Bigfread,
	MEMpro_Id_BeginSpeedMode,
	MEMpro_Id_EndSpeedMode,
	MEMpro_Id_GSP,
	MEMpro_M_Number0fRasters
} MEMpro_tden_MemRasterId;

#undef DEFINE_FUNCTION


typedef struct	MEMpro_tdst_MemManager_
{
	unsigned int	ui_Alloc;
	unsigned int	ui_Free;
	unsigned int	ui_RTalloc;
} MEMpro_tdst_MemManager;


/*$4
 ***********************************************************************************************************************
    public variables
 ***********************************************************************************************************************
 */

__EXTERN MEMpro_tdst_MemManager *MEMpro_gp_StackPointer;
__EXTERN BOOL					MEMpro_gb_StackOverflow;

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

#ifdef MEMpro_M_ActiveRasters
void	MEMpro_Init(void);
void	MEMpro_StartMemRaster(void);
void	MEMpro_StopMemRaster(MEMpro_tden_MemRasterId);
void    MEMpro_Print(void);
void MEMpro_ChangeDisplay(void);
#ifdef JADEFUSION
ULONG	MEMpro_getAnimMemory(void);
int		MEMpro_GetAnimNumber(void);
ULONG	MEMpro_getAICBMemory(void);
int		MEMpro_GetAICBNumber(void);
ULONG	MEMpro_getAIFuncMemory(void);
int		MEMpro_GetAIFuncNumber(void);
ULONG	MEMpro_getAIVarsMemory(void);
int		MEMpro_GetAIVarsNumber(void);
ULONG	MEMpro_getObjMemory(void);
int		MEMpro_GetObjNumber(void);
#endif
#else
#  define MEMpro_Init()
#  define MEMpro_StartMemRaster()
#  define MEMpro_StopMemRaster(a)
#  define MEMpro_Print()
#  define MEMpro_ChangeDisplay()
#endif

#undef __EXTERN

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __MEMPRO_H__ */
