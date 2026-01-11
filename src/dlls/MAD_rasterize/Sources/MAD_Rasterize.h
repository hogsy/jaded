/*$T MAD_Rasterize.h GC!1.32 06/03/99 14:45:43 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
//#include "BASe/BAStypes.h"
#ifndef ULONG 
#define ULONG unsigned long
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define RAS_ZBUFFER_MULT 10000.0f


typedef struct  MAD_R_Vertex_
{
    float   X;
    float   Y;
	union {
		float   OoZ; 
		float   Z; 
	};
    float   SclrToInt[3];
	ULONG Original;
} MAD_R_Vertex;

typedef struct  MAD_R_Raster_
{
    MAD_R_Vertex  P;
	float			XEnd;
} MAD_R_Raster;

typedef struct RADCAM_
{
	ULONG NodeNumber;
	MAD_Matrix    Matrix;
	MAD_NodeID      *Object;
	float  Focale;
	float  CX,CY;
	float  PX,PY;
	ULONG IsOrtho;
} RADCAM;

#define MAD_RASTERIZE_NoClipZ 1
#define MAD_RASTERIZE_NoClipX 2
#define MAD_RASTERIZE_NoClipY 4


typedef struct  MAD_R_Clipping_Info_
{
    float           XMin;
    float           XMax;
    float           YMin;
    float           YMax;
    float           ZMin;
    float           ZMax;
    ULONG			NmbrOfSclrToInt;
	ULONG			Force2sidedFLAG;
	ULONG			InvertBCKFCFlag;
	RADCAM			ProjectCam;
	void			(*MAD_PlotCLBK)(MAD_R_Vertex *);
	void			(*MAD_DrawLineCLBK)(MAD_R_Vertex *A, MAD_R_Vertex * B,ULONG SideNumBerAndIsClipped);
	void			(*RasterizeCallback)(ULONG Y, MAD_R_Raster * R, MAD_R_Vertex * PIncremental);
	void			(*ProjectVertexCLBK)(MAD_R_Vertex *P3D, MAD_R_Vertex *P2D , struct MAD_R_Clipping_Info_ *C);
	ULONG			ClipFlags;
} MAD_R_Clipping_Info;

void MAD_Rasterize_TriQuad3D
(
    MAD_R_Vertex * P3,
    MAD_R_Clipping_Info * C, 
	float *Surface
);
void MAD_Rasterize_Init();
void ProjectVertex(MAD_R_Vertex *P3D, MAD_R_Vertex *P2D , MAD_R_Clipping_Info *C);
void ProjectVertexISO(MAD_R_Vertex *P3D, MAD_R_Vertex *P2D , MAD_R_Clipping_Info *C);
void ClipDrawLine (  MAD_R_Vertex * P2,MAD_R_Clipping_Info * C , void (*MAD_DrawLineCLBK)(MAD_R_Vertex *A, MAD_R_Vertex * B));

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
