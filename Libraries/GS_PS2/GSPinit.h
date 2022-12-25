
/*$T OGLinit.h GC!1.52 11/23/99 15:29:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */



#ifndef __GSPINIT_H__
#define __GSPINIT_H__

#include "MATHs/MATH.h"
#include "BASe/BAStypes.h"
#include "SOFT/SOFTstruct.h"

#ifdef __cplusplus
extern "C"
{
#endif
typedef struct GSP_tdst_SpecificData_
{
	ULONG vide1;
	ULONG vide2;
	ULONG vide3;
} GSP_tdst_SpecificData;



/*$4
 ***************************************************************************************************
    Macros
 ***************************************************************************************************
 */

#define GSP_M_SD(_pst_DD)   ((GSP_tdst_SpecificData *) _pst_DD->pv_SpecificData)
#define GSP_M_RS(_pst_DD)   (&((GSP_tdst_SpecificData *) _pst_DD->pv_SpecificData)->st_RS)

/*$4
 ***************************************************************************************************
    Structure
 ***************************************************************************************************
 */

/*
 ---------------------------------------------------------------------------------------------------
    Specific data for OpenGL Device
 ---------------------------------------------------------------------------------------------------
 */
/*$4
 ***************************************************************************************************
    Function
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Close / init / reinit
 ===================================================================================================
 */
struct GDI_tdst_DisplayData_; 
GSP_tdst_SpecificData   *GSP_pst_CreateDevice(void);
void                    GSP_DestroyDevice(void *);
LONG                    GSP_l_Close(void*);
LONG                    GSP_l_Init(void*);
LONG                    GSP_l_ReadaptDisplay(HWND, void *);
void			GSP_IniModule();

/*
 ===================================================================================================
    Flip / clear / surface status
 ===================================================================================================
 */
void                    GSP_Flip(void);
void                    GSP_Clear(LONG, ULONG);

/*
 ===================================================================================================
    Render function
 ===================================================================================================
 */
struct CAM_tdst_Camera_;
void                    GSP_SetViewMatrix(MATH_tdst_Matrix *);
void                    GSP_SetProjectionMatrix(struct CAM_tdst_Camera_ *);
void                    GSP_SetTextureBlending
                        (
                            ULONG,
                            ULONG,
                            ULONG
                        );

/*
 ===================================================================================================
    World function
 ===================================================================================================
 */
struct WOR_tdst_World_;
LONG                    GSP_l_AttachWorld(struct WOR_tdst_World_ *);
LONG                    GSP_l_DetachWorld(void);
LONG                    GSP_l_WorldChanged(void);

void					GSP_SetTextureTarget(ULONG,ULONG);
void					GSP_SetViewMatrix_SDW(struct MATH_tdst_Matrix_ *_pst_Matrix , float *Limits);

/*
 ===================================================================================================
    Draw primitives
 ===================================================================================================
 */
struct GEO_tdst_UV_;
struct GEO_tdst_ElementIndexedTriangles_;
LONG                    GSP_l_DrawElementIndexedTriangles
                        (
                            struct GEO_tdst_ElementIndexedTriangles_ *,
                            GEO_Vertex *,
                            struct GEO_tdst_UV_ *,
                            ULONG
                        );

#ifdef __cplusplus
}
#endif

#endif /* __OGLINIT_H__ */

