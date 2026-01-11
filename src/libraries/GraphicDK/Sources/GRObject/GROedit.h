/*$T GROedit.h GC!1.55 01/12/00 10:08:23 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS
#ifndef __GROEDIT_H__
#define __GROEDIT_H__
#include "BASe/BAStypes.h"


#pragma once

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define GRO_Cul_EOF_SubObject               0x00000001
#define GRO_Cul_EOF_Vertex                  0x00000002
#define GRO_Cul_EOF_Edge                    0x00000004
#define GRO_Cul_EOF_Face                    0x00000008
#define GRO_Cul_EOF_UV                      0x00000010
#define GRO_Cul_EOF_HideSubObject			0x00000020
#define GRO_Cul_EOF_ToolAcceptMultiple      0x00000040
#define GRO_Cul_EOF_ToolWhileMouseMove      0x00000080
#define GRO_Cul_EOF_SkinMode			    0x00000100
#define GRO_Cul_EOF_SnapVertex              0x00000200
#define GRO_Cul_EOF_MovePivot               0x00000400
#define GRO_Cul_EOF_FaceLockId              0x00000800
#define GRO_Cul_EOF_ShowEdge                0x00001000
#define GRO_Cul_EOF_EdgeDivide              0x00002000
#define GRO_Cul_EOF_VertexPosGlobal         0x00004000
#define GRO_Cul_EOF_VertexMoveUpdateUV      0x00008000
#define GRO_Cul_EOF_HideMorphVector         0x00010000
#define GRO_Cul_EOF_VertexPosMove           0x00020000
#define GRO_Cul_EOF_FaceBackfaceTest        0x00040000
#define GRO_Cul_EOF_FacePickingBufferTest   0x00080000
#define GRO_Cul_EOF_VertexMoveUpdateMorph   0x00100000
#define GRO_Cul_EOF_SelectVisible			0x00200000
#define GRO_Cul_EOF_HideSubObject4Morph		0x00400000

#define GRO_Cul_EOF_MRM_MASK             	0x01800000
#define GRO_Cul_EOF_MRM_NONE             	0x00000000
#define GRO_Cul_EOF_MRM_CURVE             	0x00800000
#define GRO_Cul_EOF_MRM_LOD             	0x01000000

/*
#define GRO_Cul_EOF_VertexSymetrieAxe       0x03000000
#define GRO_Cul_EOF_VertexSymetrieAxeShift          24
#define GRO_Cul_EOF_VertexSymetrieX         0x00000000
#define GRO_Cul_EOF_VertexSymetrieY         0x01000000
#define GRO_Cul_EOF_VertexSymetrieZ         0x02000000
*/

#define GRO_Cul_EOF_FaceSelMask             0x0C000000
#define GRO_Cul_EOF_FaceSelShift					26
#define GRO_Cul_EOF_FaceSelExt				0x00000000
#define GRO_Cul_EOF_FaceSelNear				0x04000000
#define GRO_Cul_EOF_FaceSelExtUV			0x08000000
#define GRO_Cul_EOF_FaceSelNearEdge			0x0C000000

#define GRO_Cul_EOF_ColorModeMask			0x30000000
#define GRO_Cul_EOF_ColorModeShift			28
#define GRO_Cul_EOF_ColorModeFaceID			0x10000000
#define GRO_Cul_EOF_ColorModeAlpha			0x20000000


#define GRO_Cul_EOF_Mode                    (GRO_Cul_EOF_Vertex | GRO_Cul_EOF_Edge | GRO_Cul_EOF_Face | GRO_Cul_EOF_UV)
#define GRO_Cul_EOF_Tool                    (GRO_Cul_EOF_ToolAcceptMultiple | GRO_Cul_EOF_ToolWhileMouseMove)


#define GRO_i_EOT_NoTool                    0

#define GRO_i_EOT_VertexPaint               1
#define GRO_i_EOT_VertexPaintAlpha          2
#define GRO_i_EOT_VertexPickColor           3
//trou
#define GRO_i_EOT_VertexWeld                5
#define GRO_i_EOT_VertexBuildFace           6

#define GRO_i_EOT_EdgeTurn                  1
#define GRO_i_EOT_EdgeCut                   2
#define GRO_i_EOT_EdgeCutter                3

#define GRO_i_EOT_FaceDelete                2
#define GRO_i_EOT_FaceCut                   3

#define GRO_UVMapper_ColorPoint             0
#define GRO_UVMapper_ColorSelPoint          1
#define GRO_UVMapper_ColorLines1            2
#define GRO_UVMapper_ColorLines2            3
#define GRO_UVMapper_ColorGrid              4
#define GRO_UVMapper_ColorHelper1           5
#define GRO_UVMapper_ColorHelper2           6

#define GRO_UVMapper_NbColors               7

#define GRO_Cul_EOUVF_GizmoPlanar           0
#define GRO_Cul_EOUVF_GizmoCylindrical      1
#define GRO_Cul_EOUVF_GizmoBox              2
#define GRO_Cul_EOUVF_GizmoSpherical        3
#define GRO_Cul_EOUVF_GizmoTypeMask         0x00000007

#define GRO_Cul_EOSYM_PivotLocal_X			0
#define GRO_Cul_EOSYM_PivotLocal_Y			1
#define GRO_Cul_EOSYM_PivotLocal_Z			2
#define GRO_Cul_EOSYM_PivotHelper_X			3
#define GRO_Cul_EOSYM_PivotHelper_Y			4
#define GRO_Cul_EOSYM_PivotHelper_Z			5
#define GRO_Cul_EOSYM_HelperHelper_X		6
#define GRO_Cul_EOSYM_HelperHelper_Y		7
#define GRO_Cul_EOSYM_HelperHelper_Z		8
#define GRO_Cul_EOSYM_Pivot_O				9
#define GRO_Cul_EOSYM_Helper_O				10
#define GRO_Cul_EOSYM_Number				11

/*$4
 ***********************************************************************************************************************
    Structure
 ***********************************************************************************************************************
 */

typedef struct  GRO_tdst_EditOptions_
{
    ULONG   ul_Flags;
    int     i_VertexTool;
    int     i_EdgeTool;
    int     i_FaceTool;
    int     i_UVTool;
    ULONG	ul_VertexPaintColor;
    float   f_VertexBlendColor;
    float   f_ScaleForNormals;
    LONG    l_FaceId;
    ULONG   ul_VertexColor[8];
    ULONG   ul_EdgeColor[8];
    ULONG   ul_FaceColor[8];
    ULONG   ul_NormalColor[8];
    float   f_VertexSize;
    float   f_EdgeSize;
    float   f_FaceSize;
    float   f_NormalSize;
    float   f_UVWeldThresh;
    float   f_VertexWeldThresh;
    ULONG   ul_WiredColorSel;
    ULONG   ul_WiredColor;
    ULONG   ul_UVMapperColor[2][GRO_UVMapper_NbColors];
    ULONG   ul_UVMapperFlags;
    ULONG   ul_MorphVectorColor[2];
    ULONG	ul_FaceIDColor[ 32 ];
    ULONG	ul_ColorStock[ 10 ];
    int		i_UVMapper_X;
    int		i_UVMapper_Y;
    int		i_UVMapper_W;
    int		i_UVMapper_H;
    int		i_RLIAdjust_X;
    int		i_RLIAdjust_Y;
    int		i_RLIAdjust_HideSub;
    int		i_RLIAdjust_Preview;
    int		i_RLIAdjust_Colorize;
    int		i_RLIAdjust_Mode;
    //ULONG	ul_MRMLevelNb;
    //float	f_MRMLevels[ 5 ];
	int		i_SymetrieOp;
} GRO_tdst_EditOptions;

#endif /* __GROEDIT_H */

#endif /* ACTIVE_EDITORS */
