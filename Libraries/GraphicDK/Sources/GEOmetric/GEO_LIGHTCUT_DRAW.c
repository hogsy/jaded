/* GEO_LIGHTCUT_DRAW.c */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "GDInterface/GDIrequest.h"
#include "SELection/SELection.h"
#include "GRObject/GROstruct.h"
#include "GRObject/GROedit.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOobjectaccess.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_LIGHTCUT.h"

#define GLV_SHORTEDGEF 1.0f//0.98f
//#define GLV_DRAW_OK3
#define GLV_DrawEdges
#define GLV_DrawEdgesDots
#define GLV_DrawTri
#define GLV_DrawOBV
//#define GLV_DrawInvisible
#define GLV_DRAW_Cartoon
//#define GLV_DrawEdgesDots
//#define GLV_DRAW_Collisions
#define GLV_DotSize GLV_WELD_FCT * 100000.0f

#include <math.h>
#include <STDLIB.H>
#include "../dlls/MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "../dlls/MAD_mem/Sources/MAD_mem.h"
#include "GEOmetric/GEO_LODCmpt.h"

#ifdef JADEFUSION
#if defined(_GLV_KEEP_KILT)
extern tdst_GLV *KiltToDraw;
#endif
#else
extern tdst_GLV *KiltToDraw ;
#endif

#ifdef GLV_DRAW_OK3
extern GLV_OCtreeNode *p_Octree ;
extern GLV_OCtreeNode stOctree;
#endif
#ifdef GLV_DEBUG
extern tdst_GLV *BugReport;
#endif

static ULONG bShow = 0;

#ifdef JADEFUSION
extern void OGL_DrawLineEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawLineEx *_pst_Data);
#endif
void GLV_DrawGLV	(GDI_tdst_DisplayData *_pst_DD , tdst_GLV *p_stGLV)
{
	ULONG Counter;
	ULONG LightColor;
	MATH_tdst_Vector *Quad[2],VE[3];
	MATHD_tdst_Vector V[3];
	MATHD_tdst_Vector CENTER;
	tdst_GLV_Face *p_FT,*p_LT;
	if (!p_stGLV) return;
	VE[0].x = 0.0f;
	Quad[0] = NULL;
	Counter = 0;
	V[0].x = 0.0f;
	_pst_DD->ul_ColorConstant = 0xffffff;
	GDI_SetViewMatrix((*_pst_DD) , &_pst_DD->st_Camera.st_InverseMatrix);
	
	p_FT = p_stGLV->p_stFaces;
	p_LT = p_FT + p_stGLV->ulNumberOfFaces;
	CENTER.x = (GLV_Scalar)_pst_DD->st_Camera.st_Matrix.T.x;
	CENTER.y = (GLV_Scalar)_pst_DD->st_Camera.st_Matrix.T.y;
	CENTER.z = (GLV_Scalar)_pst_DD->st_Camera.st_Matrix.T.z;
	while (p_FT < p_LT)
	{
		/* Test backface */
		p_FT ->ulFlags &= ~GLV_FLGS_RSV1;
		if (MATHD_f_DotProduct(&CENTER , &p_FT->Plane.Normale) < p_FT->Plane.fDistanceTo0)
			p_FT ->ulFlags |= GLV_FLGS_RSV1;
		p_FT ++;
	}
#ifdef GLV_DRAW_OK3
	if (p_Octree && (bShow & 2)) GLV_Octree_Draw(p_Octree , _pst_DD);
#endif
	/* Draw OBV */
#ifdef GLV_DrawOBV
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfOBV ;Counter++)
	{
		ULONG AA,AB;
		_pst_DD->ul_ColorConstant = 0x0000FF;
		for (AA = 0 ; AA < 7 ; AA ++)
		{
			for (AB = AA + 1 ; AB < 8 ; AB ++)
			{
				if (((AA ^ AB) == 1) ||
					((AA ^ AB) == 2) ||
					((AA ^ AB) == 4))
				{
					MATHD_To_MATH(&VE[0] ,&p_stGLV->p_OBV[Counter].stVectors[AA] );
					MATHD_To_MATH(&VE[1] ,&p_stGLV->p_OBV[Counter].stVectors[AB] );
					Quad[0] = &VE[0];
					Quad[1] = &VE[1];
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
				}
			}
		}
	}
#endif 
	/* Draw EDges */
#ifdef GLV_DrawEdges
		 
	for (Counter = 0 ; Counter < p_stGLV->ulNumberOfEdges ;Counter++)
	{
		GDI_tdst_Request_DrawLineEx _pst_Data;
		ULONG Color ;
		Color = 0x80808000;
		_pst_DD->ul_ColorConstant = 0xb0b0b0;
		
		
		if (p_stGLV->p_stEdges[Counter].ulFlags & GLV_FLGS_DBG1)
			Color = 0x80000080;
		

		MATHD_BlendVector(&V[0] , &p_stGLV->p_stPoints[p_stGLV->p_stEdges[Counter].Index[0]].P3D ,&p_stGLV->p_stPoints[p_stGLV->p_stEdges[Counter].Index[1]].P3D , GLV_SHORTEDGEF);
		MATHD_BlendVector(&V[1] , &p_stGLV->p_stPoints[p_stGLV->p_stEdges[Counter].Index[0]].P3D ,&p_stGLV->p_stPoints[p_stGLV->p_stEdges[Counter].Index[1]].P3D , 1.f - GLV_SHORTEDGEF);
		MATHD_To_MATH(&VE[0] ,&V[0] );
		MATHD_To_MATH(&VE[1] ,&V[1] );
		_pst_Data.ul_Flags = 0;
		_pst_Data.A = &VE[0];
		_pst_Data.B = &VE[1];
		_pst_Data.ul_Color = Color;
		_pst_Data.f_Width = 4.0f;

		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) &_pst_Data);
/*
		_pst_DataPoint.A = &VE[0];
		_pst_DataPoint.ul_Color = Color;
		_pst_DataPoint.f_Size = 8.0f * p_stGLV->p_stPoints[p_stGLV->p_stEdges[Counter].Index[0]].DistanceFromSource;

		OGL_DrawPointEx(_pst_DD, &_pst_DataPoint);*/
	}
#endif
	if (bShow & 2)
	{
		p_FT = p_stGLV->p_stFaces;
		p_LT = p_FT + p_stGLV->ulNumberOfFaces;
		while (p_FT < p_LT)
		{
			MATH_tdst_Vector NE[3];
			MATHD_tdst_Vector N[3];
			float	fBlender;
			ULONG Convert;
			MATHD_tdst_Vector CENTER2;
			
			ULONG Col1,Col2;
			
			LightColor = ((p_FT->ulMARK) & 0x3) << 6;
			LightColor |= ((p_FT->ulMARK) & 0xC) << 12;
			LightColor |= ((p_FT->ulMARK) & 0x30) << 18;
			LightColor ^= 0x00c0c0c0;
			fBlender = 0.1f;
			if (p_FT->ulFlags & GLV_FLGS_BlackFront)
				_pst_DD->ul_ColorConstant = 0xf00000;
			else
				_pst_DD->ul_ColorConstant = 0xf0;
			fBlender = 0.0f;
			if (!(p_FT->ulFlags &	GLV_FLGS_Original	)) 
			{
				_pst_DD->ul_ColorConstant |= 0xF000;
				fBlender = 0.0f;
			}
			/*		if (p_FT->ulFlags &	GLV_FLGS_Original	)
			{
			p_FT++;
			continue;
			}
			/*		if (p_FT->ulFlags &	GLV_FLGS_Clipped) 
			{
			_pst_DD->ul_ColorConstant = 0xF000F0;
			//			fBlender = 0.0f;
		}*/
			
			Col1 = _pst_DD->ul_ColorConstant;
			Col2 = (_pst_DD->ul_ColorConstant>>32) + (_pst_DD->ul_ColorConstant>>1);
			MATHD_AddVector(&CENTER , &p_stGLV->p_stPoints[p_FT->Index[0]].P3D , &p_stGLV->p_stPoints[p_FT->Index[1]].P3D );
			MATHD_AddVector(&CENTER , &CENTER , &p_stGLV->p_stPoints[p_FT->Index[2]].P3D );
			MATHD_ScaleVector(&CENTER , &CENTER , 0.333333f );
			CENTER2 = CENTER;
			MATHD_AddScaleVector(&CENTER , &CENTER , &p_FT->Plane.Normale , 0.01f);
			MATHD_BlendVector(&V[0] , &p_stGLV->p_stPoints[p_FT->Index[0]].P3D , &CENTER , fBlender);
			MATHD_BlendVector(&V[1] , &p_stGLV->p_stPoints[p_FT->Index[1]].P3D , &CENTER , fBlender);
			MATHD_BlendVector(&V[2] , &p_stGLV->p_stPoints[p_FT->Index[2]].P3D , &CENTER , fBlender);
			MATHD_AddScaleVector(&N[0] , &V[0] , &p_FT->Plane.Normale , 0.01f);
			MATHD_AddScaleVector(&N[1] , &V[1] , &p_FT->Plane.Normale , 0.01f);
			MATHD_AddScaleVector(&N[2] , &V[2] , &p_FT->Plane.Normale , 0.01f);
			
			/* DRAW THE NORMALE */
			MATHD_To_MATH(&VE[0] ,&CENTER );
			MATHD_To_MATH(&VE[1] ,&CENTER2 );
			Quad[0] = &VE[0];
			Quad[1] = &VE[1];
			if (GLD_IsIllegalFaces( p_stGLV  , p_FT))
				_pst_DD->ul_ColorConstant = 0xff00ff;
			else
				_pst_DD->ul_ColorConstant = 0xff;
			//		_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
			/*		if (!(p_FT->ulFlags &	GLV_FLGS_Original	)) 
			{
			MATHD_To_MATH(&VE[1] ,&p_stGLV->p_stPoints[p_FT->Index[0]].P3D );
			_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
			MATHD_To_MATH(&VE[1] ,&p_stGLV->p_stPoints[p_FT->Index[1]].P3D );
			_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
			MATHD_To_MATH(&VE[1] ,&p_stGLV->p_stPoints[p_FT->Index[2]].P3D );
			_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
		}//*/
			
			
			for (Convert = 0 ; Convert < 3 ; Convert ++)
				MATHD_To_MATH(&VE[Convert] ,&V[Convert] );
			/* Draw visible */ 
			_pst_DD->ul_ColorConstant = LightColor;
			Quad[0] = &VE[0];
			Quad[1] = &VE[1];
			if (p_FT ->ulFlags & GLV_FLGS_AIsVisible)
				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
			Quad[0] = &VE[1];
			Quad[1] = &VE[2];
			if (p_FT ->ulFlags & GLV_FLGS_BIsVisible)
				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
			Quad[0] = &VE[2];
			Quad[1] = &VE[0];
			if (p_FT ->ulFlags & GLV_FLGS_CIsVisible)
				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
			/*#draw invisible */
#ifdef GLV_DrawInvisible
			_pst_DD->ul_ColorConstant = LightColor;
			Quad[0] = &VE[0];
			Quad[1] = &VE[1];
			if (!(p_FT ->ulFlags & GLV_FLGS_AIsVisible))
				_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
			Quad[0] = &VE[1];
			Quad[1] = &VE[2];
			if (!(p_FT ->ulFlags & GLV_FLGS_BIsVisible))
				_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
			Quad[0] = &VE[2];
			Quad[1] = &VE[0];
			if (!(p_FT ->ulFlags & GLV_FLGS_CIsVisible))
				_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
#endif
			MATHD_AddScaleVector(&N[0] , &V[0] , &p_FT->Plane.Normale , GLF_PrecMetric * 10000.0f);
			MATHD_AddScaleVector(&N[1] , &V[1] , &p_FT->Plane.Normale , GLF_PrecMetric * 10000.0f);
			MATHD_AddScaleVector(&N[2] , &V[2] , &p_FT->Plane.Normale , GLF_PrecMetric * 10000.0f);
			for (Convert = 0 ; Convert < 3 ; Convert ++)
				MATHD_To_MATH(&NE[Convert] ,&N[Convert] );
				/*		if (!(p_FT->ulFlags &	GLV_FLGS_Original )) 
				{
				_pst_DD->ul_ColorConstant = p_stGLV->p_stPoints[p_FT->Index[0]].ulColor;
				Quad[0] = &VE[0];
				Quad[1] = &NE[0];
				_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
				_pst_DD->ul_ColorConstant = p_stGLV->p_stPoints[p_FT->Index[1]].ulColor;
				Quad[0] = &VE[1];
				Quad[1] = &NE[1];
				_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
				_pst_DD->ul_ColorConstant = p_stGLV->p_stPoints[p_FT->Index[2]].ulColor;
				Quad[0] = &VE[2];
				Quad[1] = &NE[2];
				_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
		}//*/
			MATHD_AddScaleVector(&N[0] , &V[0] , &p_FT->Plane.Normale , 0.00f);
			MATHD_AddScaleVector(&N[1] , &V[1] , &p_FT->Plane.Normale , 0.00f);
			MATHD_AddScaleVector(&N[2] , &V[2] , &p_FT->Plane.Normale , 0.00f);
			for (Convert = 0 ; Convert < 3 ; Convert ++)
				MATHD_To_MATH(&NE[Convert] ,&N[Convert] );
			if (!(p_FT->ulFlags &	GLV_FLGS_Original	)) 
				for (Counter = 0 ; Counter < 3 ; Counter ++)
				{
#ifdef GLV_DRAW_Cartoon
					if ((p_FT ->Nghbr[Counter] < 0xffff) && (GLV_FLGS_RSV1 & (p_FT->ulFlags ^ p_stGLV->p_stFaces[p_FT ->Nghbr[Counter]].ulFlags)))
					{
						_pst_DD->ul_ColorConstant = LightColor;
						Quad[0] = &NE[Counter];
						Quad[1] = &NE[( Counter+1)% 3];
						_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
					}
					if ((p_FT ->Nghbr[Counter] < 0xffff) && (MATHD_f_DotProduct(&p_FT->Plane.Normale , &p_stGLV->p_stFaces[p_FT ->Nghbr[Counter]].Plane.Normale) < 0.8f))
					{
						_pst_DD->ul_ColorConstant = LightColor;
						Quad[0] = &NE[Counter];
						Quad[1] = &NE[( Counter+1)% 3];
						_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
					}
#endif
				}
				//			/*
				if (p_FT->ulFlags &  GLV_FLGS_Original)
					for (Counter = 0 ; Counter < 3 ; Counter ++)
					{
						_pst_DD->ul_ColorConstant = 0;
						if (p_stGLV->p_stFaces[p_FT ->Nghbr[Counter]].ulFlags & GLV_FLGS_BlackFront) _pst_DD->ul_ColorConstant = 0xff0000;
						if (p_stGLV->p_stFaces[p_FT ->Nghbr[Counter]].ulFlags & GLV_FLGS_WhiteFront) _pst_DD->ul_ColorConstant |= 0xff00;
						Quad[0] = &NE[Counter];
						Quad[1] = &NE[( Counter+1)% 3];
						_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
					}//*/
					
#ifdef GLV_DRAW_Collisions
					for (Counter = 0 ; Counter < 3 ; Counter ++) 
					{
						if ((p_FT ->Nghbr[Counter] < 0xffff) && ((p_stGLV->p_stFaces[p_FT ->Nghbr[Counter]].ulFlags ^ p_FT->ulFlags) & GLV_FLGS_Kilt))
						{
							_pst_DD->ul_ColorConstant = 0xffffff;
							Quad[0] = &VE[Counter];
							Quad[1] = &VE[( Counter+1)% 3];
							_pst_DD->st_GDI.pfnl_Request(_pst_DD, GDI_Cul_Request_DrawLine, (ULONG) Quad);
						}
					}//*/
#endif
					p_FT++;
	}
	}
}
void GLV_ShowTheKilt	(ULONG bShowV)
{
	bShow = bShowV;
}
void GLV_DraweTheKilt(GDI_tdst_DisplayData *_pst_DD)
{
#ifdef GLV_DEBUG
	GLV_DrawGLV(_pst_DD , BugReport);
#endif
	if (!bShow) return;

#ifdef JADEFUSION
	#if defined(_GLV_KEEP_KILT)
	GLV_DrawGLV(_pst_DD , KiltToDraw);
	#endif
#else
	GLV_DrawGLV(_pst_DD , KiltToDraw);
#endif
}


#endif
