/*$T GDInterface.h GC! 1.081 08/08/00 10:10:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __GDREQUEST_H__
#define __GDREQUEST_H__

#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/BASsys.h"
#include "MATHs/MATH.h"
#include "CAMera/CAMera.h"
#include "SOFT/SOFTMatrixStack.h"
#include "LIGHT/LIGHTstruct.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOzone.h"
#include "MATerial/MATstruct.h"
#include "WAYpoint/WAYlink.h"
#include "TEXture/TEXmanager.h"
#include "SOFT/SOFTstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/* Request constant */
#define GDI_Cul_Request_SetFogParams			0x00000001
#define GDI_Cul_Request_DrawSoftEllipse			0x00000002
#define GDI_Cul_Request_DrawSoftArrow			0x00000003
#define GDI_Cul_Request_DrawSoftSquare			0x00000004
#define GDI_Cul_Request_DrawTransformedTriangle 0x00000005
#define GDI_Cul_Request_DepthTest				0x00000006
#define GDI_Cul_Request_DepthFunc				0x00000007
#define GDI_Cul_Request_DrawPoint				0x00000008
#define GDI_Cul_Request_DrawLine				0x00000009
#define GDI_Cul_Request_DrawTriangle			0x0000000A
#define GDI_Cul_Request_DrawQuad				0x0000000B
#define GDI_Cul_Request_ReloadTexture			0x0000000C
#define GDI_Cul_Request_SizeOfPoints			0x0000000D
#define GDI_Cul_Request_SizeOfLine				0x0000000E
#define GDI_Cul_Request_DrawLineEx				0x0000000F
#define GDI_Cul_Request_DrawPointEx				0x00000010
#define GDI_Cul_Request_DrawPointSize			0x00000011
#define GDI_Cul_Request_BeforeDrawSprite		0x00000012
#define GDI_Cul_Request_DrawSprite				0x00000013
#define GDI_Cul_Request_AfterDrawSprite			0x00000014
#define GDI_Cul_Request_DrawPointMin			0x00000015
#define GDI_Cul_Request_ReloadPalette           0x00000016
#define GDI_Cul_Request_Draw2DTriangle          0x00000017
#define GDI_Cul_Request_ReadScreen              0x00000018
#define GDI_Cul_Request_EnableFog               0x00000019
#define GDI_Cul_Request_NumberOfTextures        0x0000001A
#define GDI_Cul_Request_ReadPixel               0x0000001B
#define GDI_Cul_Request_Enable                  0x0000001C
#define GDI_Cul_Request_Disable                 0x0000001D
#define GDI_Cul_Request_PolygonOffset           0x0000001E
#define GDI_Cul_Request_LoadInterfaceTex        0x0000001F
#define GDI_Cul_Request_UnloadInterfaceTex      0x00000020
#define GDI_Cul_Request_DrawSpriteUV			0x00000021
#define GDI_Cul_Request_GetInterfaceTexBuffer   0x00000022
#define GDI_Cul_Request_TextureUnloadCompare    0x00000023
#define GDI_Cul_Request_Display169BlackBand     0x00000024
#define GDI_Cul_Request_ReloadTexture2			0x00000025
#define GDI_Cul_Request_DrawBox					0x00000026
#define GDI_Cul_Request_DrawSphere				0x00000027
#define GDI_Cul_Request_DrawCone				0x00000028
#define GDI_Cul_Request_DrawCylinder			0x00000029
#define GDI_Cul_Request_DrawQuadEx				0x0000002A
#define GDI_Cul_Request_PolygonOffsetSave		0x0000002B
#define GDI_Cul_Request_PolygonOffsetRestore	0x0000002C

#define GDI_Cul_Request_PushZBuffer				0x00000030

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    structure for request
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	GDI_tdst_Request_ReloadTextureParams_
{
	char	*pc_Data;
	USHORT	w_Texture;
	char 	IsAlreadySwizzled;	
} GDI_tdst_Request_ReloadTextureParams;

#define GDI_Request_DrawLine_Arrow		0x01
#define GDI_Request_DrawLine_Length		0x02
#define GDI_Request_DrawLine_NoZWrite	0x04

typedef struct	GDI_tdst_Request_DrawLineEx_
{
	ULONG				ul_Flags;
	MATH_tdst_Vector	*A, *B;
	float				f_Width;
	ULONG				ul_Color;
	ULONG				ul_TextColor;
	ULONG				ul_TextBackColor;
} GDI_tdst_Request_DrawLineEx;

#define GDI_Request_DrawVector_Arrow		0x01
#define GDI_Request_DrawVector_Length		0x02

typedef struct	GDI_tdst_Request_DrawPointEx_
{
	MATH_tdst_Vector	*A;
	float				f_Size;
	ULONG				ul_Color;
} GDI_tdst_Request_DrawPointEx;

typedef struct GDI_tdst_Request_RWPixels_
{
    char    c_Write;            /* 0 = read; 1 = write , 2 = copy*/
    char    c_Buffer;           /* 0 = Back buffer, 1 = Front buffer, 2 = depth buffer */
                                /* for copy : + 16 : write to back buffer, + 32 to front buffer */
    char    c_Schmurtz[2];
    int     x, y, w, h;
    char    *p_Bitmap;
    float   f_Bias[3];          
    float   f_Scale[3];
} GDI_tdst_Request_RWPixels;

typedef struct GDI_tdst_Request_DrawQuad_
{
	ULONG				ul_Flags;
	MATH_tdst_Vector	*V[4];
	float				f_LineWidth;
	ULONG				ul_EdgeColor;
	ULONG				ul_SolidColor;
} GDI_tdst_Request_DrawQuad;

#define GDI_Request_DrawQuad_Line			0x00000001
#define GDI_Request_DrawQuad_OnlyLine		0x00000002
#define GDI_Request_DrawQuad_Normal			0x00000004
#define GDI_Request_DrawQuad_NoZWrite		0x00000010
#define GDI_Request_DrawQuad_CullFace		0x00000020

typedef struct GDI_tdst_Request_DrawBox_
{
	ULONG				ul_Flags;
	MATH_tdst_Vector	*pst_Center;
	MATH_tdst_Vector	*pst_Min;
	MATH_tdst_Vector	*pst_Max;
	MATH_tdst_Matrix	*pst_M;
	float				f_Width;
	ULONG				ul_EdgeColor;
	ULONG				ul_SolidColor;
} GDI_tdst_Request_DrawBox;

#define GDI_Request_DrawBox_NoZWrite   0x1
#define GDI_Request_DrawBox_CullFace   0x2

typedef struct GDI_tdst_Request_DrawSphere_
{
	ULONG				ul_Flags;
	MATH_tdst_Vector	*pst_Center;
	MATH_tdst_Vector	*pst_Axe;
	float				f_Radius;
	float				f_LineWidth;
	ULONG				ul_EdgeColor;
	ULONG				ul_SolidColor;
} GDI_tdst_Request_DrawSphere;

typedef struct GDI_tdst_Request_DrawCone_
{
	ULONG				ul_Flags;
	MATH_tdst_Vector	*pst_Pos;
	MATH_tdst_Vector	*pst_Axe;
	float				f_Alpha;
	float				f_Beta;
	float				f_LineWidth;
	ULONG				ul_EdgeColor;
	ULONG				ul_SolidColor;
} GDI_tdst_Request_DrawCone;

#define GDI_Request_DrawCylinder_Line		0x1
#define GDI_Request_DrawCylinder_OnlyLine	0x2
#define GDI_Request_DrawCylinder_Capsule	0x4
#define GDI_Request_DrawCylinder_NoZWrite	0x8
#define GDI_Request_DrawCylinder_CullFace  0x10

typedef struct GDI_tdst_Request_DrawCylinder_
{
	ULONG				ul_Flags;
	MATH_tdst_Vector	*pst_Pos;
	MATH_tdst_Vector	*pst_Axe;
	float				f_Radius;
	float				f_LineWidth;
	ULONG				ul_EdgeColor;
	ULONG				ul_SolidColor;
} GDI_tdst_Request_DrawCylinder;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GDREQUEST_H__ */
