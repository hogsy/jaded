/*$T MAD_Rasterize.cpp GC!1.32 06/03/99 11:26:07 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "MAD_Rasterize.h"
static MAD_R_Clipping_Info  *CI;
static MAD_R_Vertex ClippingODrome_Cache[32];
static MAD_R_Vertex *p_ClippingODrome_Cache;
#include <math.h>
#include "MAD_MTH.c"
#define FLOATCAST(a) lFloatToLongOpt(a)
unsigned long Flag2NumEdge(unsigned long X)
{
	if (X == 1) return 2 ;
		else
		if (X == 2) return 1 ;
			else
			return 0 ;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void ProjectVertex(MAD_R_Vertex *P3D, MAD_R_Vertex *P2D , MAD_R_Clipping_Info *C)
{
		P2D -> OoZ = MTH_fn_fInverseOpt(P3D -> Z);
		P2D -> X = P3D -> X * P2D -> OoZ * C -> ProjectCam.CX + C -> ProjectCam.PX;
		P2D -> Y = P3D -> Y * P2D -> OoZ * C -> ProjectCam.CY + C -> ProjectCam.PY;
		P2D -> OoZ *= RAS_ZBUFFER_MULT;/*arbitrary multiplication*/
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void ProjectVertexISO(MAD_R_Vertex *P3D, MAD_R_Vertex *P2D , MAD_R_Clipping_Info *C)
{
		P2D -> X = P3D -> X * C -> ProjectCam.CX + C -> ProjectCam.PX;
		P2D -> Y = P3D -> Y * C -> ProjectCam.CY + C -> ProjectCam.PY;
		P2D -> Z = -P2D -> Z ;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void Cut(MAD_R_Vertex *in, MAD_R_Vertex *out, MAD_R_Vertex *Result, float Interp)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter;
    float           *R, *Pfin, *Pfout;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    R = *(float **) &Result;
    Pfin = *(float **) &in;
    Pfout = *(float **) &out;
    for(Counter = 0; Counter < CI -> NmbrOfSclrToInt + 3; Counter++)
        *(R++) = (*(Pfout++) - *(Pfin++)) * Interp + *Pfin;

	Result->Original = in->Original & out->Original;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
unsigned long MAD_RAS_Clip
(
    MAD_R_Vertex ** SrcPoints,
    unsigned long NbrOfPtsSrc,
    MAD_R_Vertex ** DstPointsOr,
    int(*IsOut) (MAD_R_Vertex *),
    void(*ComputeCut) (MAD_R_Vertex *, MAD_R_Vertex *,MAD_R_Vertex *)
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_R_Vertex  **CurentP, **CurentPm1 , ** DstPoints;
    unsigned long   Flag, Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DstPoints	= DstPointsOr;
    CurentPm1	= SrcPoints + NbrOfPtsSrc - 1;
    CurentP		= SrcPoints;
    Flag = IsOut(*CurentPm1);
	Counter = NbrOfPtsSrc;
    while(Counter--)
    {
        Flag = IsOut(*CurentP) | (Flag >> 1);
		switch (Flag)
		{
		case 1:
			/* go in  */
			*(DstPoints) = p_ClippingODrome_Cache++; /* Get a Cache Point */
            ComputeCut(*CurentP, *CurentPm1, *(DstPoints++));
		case 0:
            /* Stay in */
            *(DstPoints++) = *CurentP;
			break;
		case 2:
			/* Go out */
			*(DstPoints) = p_ClippingODrome_Cache++; /* Get a Cache Point */
            ComputeCut(*CurentPm1, *CurentP, *(DstPoints++));
		}
        CurentPm1 = CurentP++;
    }

    return DstPoints - DstPointsOr;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int IsOut_XMax(MAD_R_Vertex *P)
{
    if(*(long *)&P->X > *(long *)&CI -> XMax) return 2;
    return 0;//*/
}
/*
 ===================================================================================================
 ===================================================================================================
 */
int IsOut_XMin(MAD_R_Vertex *P)
{
    if(*(long *)&P->X < *(long *)&CI -> XMin) return 2;
    return 0;//*/
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int IsOut_YMax(MAD_R_Vertex *P)
{
    if(*(long *)&P->Y > *(long *)&CI -> YMax) return 2;
    return 0;//*/
}


/*
 ===================================================================================================
 ===================================================================================================
 */
int IsOut_YMin(MAD_R_Vertex *P)
{
   if(*(long *)&P->Y < *(long *)&CI -> YMin) return 2;
    return 0;//*/
}
/*
 ===================================================================================================
 ===================================================================================================
 */
int IsOut_ZMin(MAD_R_Vertex *P)
{
    if(P->Z < CI -> ZMin) return 2;
    return 0;//*/
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void ComputeCut_XMax(MAD_R_Vertex *In, MAD_R_Vertex *Out , MAD_R_Vertex *R)
{
	Cut(In, Out, R, (CI -> XMax - In -> X) / (Out -> X - In -> X));
	R -> X = CI -> XMax;
}
/*
 ===================================================================================================
 ===================================================================================================
 */
void ComputeCut_XMin(MAD_R_Vertex *In, MAD_R_Vertex *Out , MAD_R_Vertex *R)
{
	Cut(In, Out, R, (CI -> XMin - In -> X) / (Out -> X - In -> X));
	R -> X = CI -> XMin;
}
/*
 ===================================================================================================
 ===================================================================================================
 */
void ComputeCut_YMax(MAD_R_Vertex *In, MAD_R_Vertex *Out , MAD_R_Vertex *R)
{
	Cut(In, Out, R, (CI -> YMax - In -> Y) / (Out -> Y - In -> Y));
	R -> Y = CI -> YMax;
}
/*
 ===================================================================================================
 ===================================================================================================
 */
void ComputeCut_YMin(MAD_R_Vertex *In, MAD_R_Vertex *Out , MAD_R_Vertex *R)
{
	Cut(In, Out, R, (CI -> YMin - In -> Y) / (Out -> Y - In -> Y));
	R -> Y = CI -> YMin;
}
/*
 ===================================================================================================
 ===================================================================================================
 */
void ComputeCut_ZMin(MAD_R_Vertex *In, MAD_R_Vertex *Out , MAD_R_Vertex *R)
{
	Cut(In, Out, R, (CI -> ZMin - In -> Z) / (Out -> Z - In -> Z));
	R -> Z = CI -> ZMin;
}
/*
 ===================================================================================================
 ===================================================================================================
 */
__inline void Compute_One_Gradient(
						  float XD1,
						  float YD1,
						  float XD2,
						  float YD2,
						  float G1,
						  float G2,
						  float G3,
						  float *RX,
						  float *RY,
						  float SqrtNorme)
{
	float ZD1;
	float ZD2;
	float X,Y;
	ZD1 = G2 - G1;
	ZD2 = G3 - G1;
	/* Compute Cross Product */
	X = YD1 *  ZD2 - ZD1 *  YD2;
	Y = ZD1 *  XD2 - XD1 *  ZD2;
	*RX = -SqrtNorme * X;
	*RY = -SqrtNorme * Y;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void Compute_Grdients(MAD_R_Vertex **Polygone,unsigned long  NumberOfVertex , MAD_R_Vertex  *PXInc ,MAD_R_Vertex  *PYInc)
{
	unsigned long Counter;
	float 	  XD1,YD1,XD2,YD2,SqrtNorme;
	XD1 = (Polygone[1] -> X) - (Polygone[0] -> X);
	XD2 = (Polygone[2] -> X) - (Polygone[0] -> X);
	YD1 = (Polygone[1] -> Y) - (Polygone[0] -> Y);
	YD2 = (Polygone[2] -> Y) - (Polygone[0] -> Y);
	/* Compute |XYZ|*/
	SqrtNorme = XD1 * YD2 - YD1 * XD2 ;
	*(unsigned long *)&SqrtNorme &= 0x7FFFFFFF;
	SqrtNorme =  MTH_fn_fInverseOpt(SqrtNorme);
	Compute_One_Gradient(XD1,YD1,XD2,YD2,(*Polygone)->OoZ,(*(Polygone+1))->OoZ,(*(Polygone+2))->OoZ,&PXInc -> OoZ,&PYInc -> OoZ,SqrtNorme);
	for (Counter = 0 ; Counter < CI -> NmbrOfSclrToInt; Counter++)
		Compute_One_Gradient(XD1,YD1,XD2,YD2,(*Polygone)->SclrToInt[Counter],(*(Polygone+1))->SclrToInt[Counter],(*(Polygone+2))->SclrToInt[Counter],&PXInc -> SclrToInt[Counter],&PYInc -> SclrToInt[Counter],SqrtNorme);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void vctMulAdd(MAD_R_Vertex *P1 , float Factor , MAD_R_Vertex *Result )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float           *R, *Pf1, *Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    R = *(float **) &Result;
    Pf1 = *(float **) &P1;
	Last = R + CI -> NmbrOfSclrToInt+ 3;
	while (R < Last) *(R++) += *(Pf1++) * Factor;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void vctMul(MAD_R_Vertex *Result , float Factor  )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float           *R, *Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    R = *(float **) &Result;
	Last = R + CI -> NmbrOfSclrToInt+ 3;
	while (R < Last) *(R++) = *R * Factor;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void vctNULL(MAD_R_Vertex *Result )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float           *R , *Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    R = *(float **) &Result;
	Last = R + CI -> NmbrOfSclrToInt+ 3;
	while (R < Last) *(R++) = 0.f;
        
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void vctAdd(MAD_R_Vertex *P1 , MAD_R_Vertex *P2 , MAD_R_Vertex *Result )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float           *R, *Pf1 , *Pf2 , *Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    R = *(float **) &Result;
    Pf1 = *(float **) &P1;
    Pf2 = *(float **) &P2;
	Last = R + CI -> NmbrOfSclrToInt + 3;
    while (R < Last) *(R++) = *(Pf1++) - *(Pf2++);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void Vectorize_0(MAD_R_Raster *RasterODrome , MAD_R_Vertex *A , MAD_R_Vertex *B , MAD_R_Vertex *PXinc , MAD_R_Vertex *PYinc )
{
	unsigned long LeftRight,YStart,YEnd;
	MAD_R_Vertex Localnc, Local;
	MAD_R_Raster *CurrentRaster , *CurrentRasterLast;
	float XInc,XCurrent;
	LeftRight = 0;
	if (A -> Y > B -> Y)
	{
		YStart = FLOATCAST(B -> Y);
		YEnd   = FLOATCAST(A -> Y);
		XInc = ( A -> X - B -> X ) * MTH_fn_fInverseOpt((float)(YEnd-YStart));
		XCurrent = B -> X;
		CurrentRaster = &RasterODrome[YStart];
		CurrentRasterLast = &RasterODrome[YEnd];
		while (CurrentRaster < CurrentRasterLast)
		{
			CurrentRaster -> XEnd = XCurrent ;
			XCurrent+=XInc;
			CurrentRaster ++;
		}
		return;
	}
	YStart = FLOATCAST(A -> Y);
	YEnd   = FLOATCAST(B -> Y);
	if (YStart == YEnd ) return;
	Localnc . OoZ = PXinc -> OoZ * (B -> X - A -> X) ;
	Localnc . OoZ += PYinc -> OoZ * (B -> Y - A -> Y) ;
	Localnc . OoZ *= - MTH_fn_fInverseOpt((float)(YEnd-YStart));
	Localnc . X = -( B -> X - A -> X ) * MTH_fn_fInverseOpt((float)(YEnd-YStart));
	Local . X =  A -> X;
	Local . OoZ =  A -> OoZ;
	CurrentRaster = &RasterODrome[YStart];
	CurrentRasterLast = &RasterODrome[YEnd];
	while (CurrentRaster < CurrentRasterLast)
	{
		CurrentRaster -> P . X = Local . X;
		CurrentRaster -> P . OoZ = Local . OoZ;
		Local . X -=  Localnc . X;
		Local . OoZ -=  Localnc . OoZ;
		CurrentRaster ++;
	}
}
/*
 ===================================================================================================
 ===================================================================================================
 */
void Vectorize(MAD_R_Raster *RasterODrome , MAD_R_Vertex *A , MAD_R_Vertex *B , MAD_R_Vertex *PXinc , MAD_R_Vertex *PYinc )
{
	unsigned long LeftRight,YStart,YEnd;
	MAD_R_Vertex Localnc, Local;
	MAD_R_Raster *CurrentRaster , *CurrentRasterLast;
	float XInc,XCurrent;
	LeftRight = 0;
	if (A -> Y > B -> Y)
	{
		YStart = FLOATCAST(B -> Y);
		YEnd   = FLOATCAST(A -> Y);
		XInc = ( A -> X - B -> X ) * MTH_fn_fInverseOpt((float)(YEnd-YStart));
		XCurrent = B -> X;
		CurrentRaster = &RasterODrome[YStart];
		CurrentRasterLast = &RasterODrome[YEnd];
		while (CurrentRaster < CurrentRasterLast)
		{
			CurrentRaster -> XEnd = XCurrent ;
			XCurrent+=XInc;
			CurrentRaster ++;
		}
		return;
	}
	YStart = FLOATCAST(A -> Y);
	YEnd   = FLOATCAST(B -> Y);
	if (YStart == YEnd ) return;
	vctNULL(&Localnc);
	vctMulAdd(PXinc , (B -> X - A -> X) , &Localnc);
	vctMulAdd(PYinc , (B -> Y - A -> Y) , &Localnc);
	vctMul( &Localnc , - MTH_fn_fInverseOpt((float)(YEnd-YStart)));
	Localnc . X = -( B -> X - A -> X ) * MTH_fn_fInverseOpt((float)(YEnd-YStart));
	Local = *A;
	CurrentRaster = &RasterODrome[YStart];
	CurrentRasterLast = &RasterODrome[YEnd];
		while (CurrentRaster < CurrentRasterLast)
		{
			CurrentRaster -> P = Local ;
			vctAdd( &Local , &Localnc , &Local );
			CurrentRaster ++;
		}
}

/* rasterize Z Clipped Triangles */
void MAD_Rasterize_Ngone_2D(MAD_R_Vertex ** P3, ULONG NumberOfPoints, /* 3 or 4 */MAD_R_Clipping_Info * C ,float *Surface);

/*
 ===================================================================================================
    Z Cliping
 ===================================================================================================
 */
void MAD_Rasterize_TriQuad3D
(
    MAD_R_Vertex * P32,
    MAD_R_Clipping_Info * C,
	float *Surface
)
{
	MAD_R_Vertex **Cptr , P3[3] , *pp_ClippingODrome[4] , *pp_P3[3]; 
	unsigned long NumberOfVertex,Counter;
	p_ClippingODrome_Cache = ClippingODrome_Cache;
	P3[0] = P32[0];
	P3[1] = P32[1];
	P3[2] = P32[2];
	pp_P3[0] = &P3[0];
	pp_P3[1] = &P3[1];
	pp_P3[2] = &P3[2];
	CI = C;
	*Surface = 0.0f;
	P3[0] .Original = 5;
	P3[1] .Original = 3;
	P3[2] .Original = 6;
	if (C->ClipFlags & MAD_RASTERIZE_NoClipZ)
	{
		NumberOfVertex = 3;
		pp_ClippingODrome[0] = pp_P3[0];
		pp_ClippingODrome[1] = pp_P3[1];
		pp_ClippingODrome[2] = pp_P3[2];
	} else
	{
		NumberOfVertex = MAD_RAS_Clip(pp_P3, 3 , pp_ClippingODrome, IsOut_ZMin , ComputeCut_ZMin );
		if (!NumberOfVertex) 
			return;
	}
	Cptr = pp_ClippingODrome;
	Counter = NumberOfVertex;
	while (Counter--)
	{
		C -> ProjectVertexCLBK(*Cptr,*Cptr,C);
		(*Cptr)-> Y = (float)FLOATCAST((*Cptr)->Y);	
		Cptr++;
	}

	MAD_Rasterize_Ngone_2D(pp_ClippingODrome , 3 , CI , Surface);
	if (NumberOfVertex == 4)
	{
		pp_ClippingODrome[1] = pp_ClippingODrome[0];
		MAD_Rasterize_Ngone_2D(pp_ClippingODrome + 1 , 3 , CI , Surface);
	}//*/
}
/*
 ===================================================================================================
    Z Cliping is done
 ===================================================================================================
 */
#define max_screen_Height 8192
void MAD_Rasterize_Ngone_2D
(
    MAD_R_Vertex ** pp_P3,
	unsigned long NumberOfPoints , /* 3 or 4 */
    MAD_R_Clipping_Info * C ,
	float *Surface
)
{
	MAD_R_Raster RasterODrome2[max_screen_Height + 4L];
	MAD_R_Raster *RasterODrome;
	MAD_R_Raster *CurrentRaster;
	MAD_R_Vertex *pp_ClippingODrome_Odd[8],*pp_ClippingODrome_Evn[8];
    MAD_R_Vertex  **CurentP, **CurentPm1;
	MAD_R_Vertex PXIncremental , PYIncremental;

	unsigned long Counter , YMin , YMax ,Local , InvertRot;
	unsigned long NumberOfVertex;

//	*Surface = 0.0f;
	if (!C) return;
	if (NumberOfPoints != 3) 
	{
		if (NumberOfPoints != 4) return;
	}
	RasterODrome = RasterODrome2 + 2;
	CI = C;
	InvertRot = 0;
	if (((pp_P3[2]->X - pp_P3[1]->X) * (pp_P3[0]->Y - pp_P3[1]->Y)) -
		((pp_P3[2]->Y - pp_P3[1]->Y) * (pp_P3[0]->X - pp_P3[1]->X)) < 0.f) 
	{
		/* backface */
		InvertRot = 1;
		if ((!CI -> Force2sidedFLAG) && (!CI -> InvertBCKFCFlag))
			return;
	} else
		if ((CI -> InvertBCKFCFlag) && (!CI -> Force2sidedFLAG))
			return;

    
	NumberOfVertex = NumberOfPoints;
	if (C->ClipFlags & MAD_RASTERIZE_NoClipX)
	{
		ULONG Swap;
		Swap = NumberOfVertex;
		while (Swap--) 
			pp_ClippingODrome_Evn[Swap] = pp_P3[Swap];
	}
	else
	{
		/* Clip XMin */
		NumberOfVertex = MAD_RAS_Clip(pp_P3, NumberOfPoints , pp_ClippingODrome_Odd, IsOut_XMin , ComputeCut_XMin );
		if (!NumberOfVertex) return;
		/* Clip XMax */
		NumberOfVertex = MAD_RAS_Clip(pp_ClippingODrome_Odd, NumberOfVertex, pp_ClippingODrome_Evn, IsOut_XMax , ComputeCut_XMax );
		if (!NumberOfVertex) return;
	}

	if (!(C->ClipFlags & MAD_RASTERIZE_NoClipY))
	{
		// Clip YMin 
		NumberOfVertex = MAD_RAS_Clip(pp_ClippingODrome_Evn, NumberOfVertex, pp_ClippingODrome_Odd, IsOut_YMin , ComputeCut_YMin );
		if (!NumberOfVertex) return;
		// Clip YMax 
		NumberOfVertex = MAD_RAS_Clip(pp_ClippingODrome_Odd, NumberOfVertex, pp_ClippingODrome_Evn, IsOut_YMax , ComputeCut_YMax );
		if (!NumberOfVertex) return;
	}//*/

	CurentPm1 = pp_ClippingODrome_Evn + NumberOfPoints - 1;
	CurentP = pp_ClippingODrome_Evn;
	{
		// Compute surface 
/*		float Len,L2;
		for (Counter = 0 ; Counter < NumberOfVertex ; Counter++ )
		{
			Len = (*CurentPm1) -> X - (*CurentP) -> X;
			L2  = (*CurentPm1) -> Y - (*CurentP) -> Y;
			Len = Len*Len+L2*L2;
			if (Len > *Surface) *Surface= Len;
			CurentPm1 = CurentP;
			CurentP++;
		}//*/
	}
	/* ClippingODrome_Evn contain clipping result */
	/* Compute RasterODrome */
	Compute_Grdients(pp_P3 , NumberOfPoints , &PXIncremental , &PYIncremental );
	if (InvertRot)
	{
		vctMul(&PXIncremental, -1.0f);
		vctMul(&PYIncremental, -1.0f);
	}

	CurentP = pp_ClippingODrome_Evn;
	CurentPm1 = pp_ClippingODrome_Evn + NumberOfVertex - 1;
	YMin = max_screen_Height;
	YMax = 0;
	if (CI -> NmbrOfSclrToInt == 0)
	{
		for (Counter=0 ; Counter < NumberOfVertex; Counter++)
		{
			Local = FLOATCAST((*CurentP) -> Y);
			if (Local > YMax) YMax = Local ;
			if (Local < YMin) YMin = Local ;
			if (InvertRot)
				Vectorize_0(RasterODrome , *CurentPm1 , *CurentP , &PXIncremental , &PYIncremental );
			else
				Vectorize_0(RasterODrome , *CurentP , *CurentPm1 , &PXIncremental , &PYIncremental );
			CurentPm1 = CurentP++;
		}
	} else
		for (Counter=0 ; Counter < NumberOfVertex; Counter++)
		{
			Local = FLOATCAST((*CurentP) -> Y);
			if (Local > YMax) YMax = Local ;
			if (Local < YMin) YMin = Local ;
			if (InvertRot)
				Vectorize(RasterODrome , *CurentPm1 , *CurentP , &PXIncremental , &PYIncremental );
			else
				Vectorize(RasterODrome , *CurentP , *CurentPm1 , &PXIncremental , &PYIncremental );

			CurentPm1 = CurentP++;
		}
	/* Call RasterizeCallback */
	CurrentRaster = RasterODrome + YMin;
	if (C->RasterizeCallback) 
	for (Counter=YMin ; Counter < YMax; Counter++)
	{
		CI -> RasterizeCallback(Counter , CurrentRaster++ , &PXIncremental);
	}
	if (CI -> MAD_PlotCLBK)
	{
		CurentP = pp_ClippingODrome_Evn;
		Counter = NumberOfVertex;
		while (Counter--)
			CI ->MAD_PlotCLBK((*CurentP) );
	}
	if (CI -> MAD_DrawLineCLBK)
	{
		CurentP = pp_ClippingODrome_Evn;
		CurentPm1 = pp_ClippingODrome_Evn + NumberOfVertex - 1;
		for (Counter=0 ; Counter< NumberOfVertex; Counter++)
		{
			if ((*CurentP)->Original & (*CurentPm1) ->Original)
				CI -> MAD_DrawLineCLBK(*CurentP , *CurentPm1 , Flag2NumEdge((*CurentP)->Original & (*CurentPm1) -> Original));
			CurentPm1 = CurentP++;
		}
	}
}

void ClipDrawLine (  MAD_R_Vertex * P12,MAD_R_Clipping_Info * C , void (*MAD_DrawLineCLBK)(MAD_R_Vertex *A, MAD_R_Vertex * B))
{
	MAD_R_Vertex ClippingODrome[8] , **Cptr; 
	MAD_R_Vertex *pp_ClippingODrome[8] , *pp_ClippingODrome_Odd[8],*pp_ClippingODrome_Evn[8],*pp_P2[2];
    MAD_R_Vertex  **CurentP, **CurentPm1 ;
	MAD_R_Vertex P2[2] ;

	unsigned long NumberOfVertex , Counter;
	p_ClippingODrome_Cache = ClippingODrome_Cache;
	CI = C;
	Counter = 8;
	while (Counter--)
	{
		pp_ClippingODrome[Counter] = &ClippingODrome[Counter];
	}
	P2[0] = P12[0] ;
	P2[1] = P12[1] ;
	pp_P2[0] = &P2[0];
	pp_P2[1] = &P2[1];
	NumberOfVertex = MAD_RAS_Clip(pp_P2, 2 , pp_ClippingODrome, IsOut_ZMin , ComputeCut_ZMin );
	if (!NumberOfVertex) return;
	Cptr = pp_ClippingODrome;
	for (Counter = 0 ; Counter < NumberOfVertex; Counter++)
	{
		C -> ProjectVertexCLBK(*Cptr , *Cptr , C);
		Cptr++;
		/* all should be multiplly by OoZ Here */
	}

	NumberOfVertex = MAD_RAS_Clip(pp_ClippingODrome, NumberOfVertex , pp_ClippingODrome_Odd, IsOut_XMin , ComputeCut_XMin );
	if (!NumberOfVertex) return;
	NumberOfVertex = MAD_RAS_Clip(pp_ClippingODrome_Odd, NumberOfVertex, pp_ClippingODrome_Evn, IsOut_XMax , ComputeCut_XMax );
	if (!NumberOfVertex) return;
	NumberOfVertex = MAD_RAS_Clip(pp_ClippingODrome_Evn, NumberOfVertex, pp_ClippingODrome_Odd, IsOut_YMin , ComputeCut_YMin );
	if (!NumberOfVertex) return;
	NumberOfVertex = MAD_RAS_Clip(pp_ClippingODrome_Odd, NumberOfVertex, pp_ClippingODrome_Evn, IsOut_YMax , ComputeCut_YMax );
	if (!NumberOfVertex) return;
	CurentP = pp_ClippingODrome_Evn;
	CurentPm1 = pp_ClippingODrome_Evn + NumberOfVertex - 1;
	MAD_DrawLineCLBK(*CurentP , *CurentPm1);
}



void MAD_Rasterize_Init()
{
	MTH_fn_vInit();
}
