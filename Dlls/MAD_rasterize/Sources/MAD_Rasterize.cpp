/*$T MAD_Rasterize.cpp GC!1.32 06/03/99 11:26:07 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "MAD_Rasterize.h"
static MAD_R_Clipping_Info  *CI;

#include <math.h>
#include "MAD_MTH.cpp"
#define FLOATCAST(a) lFloatToLongOpt(a)

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
}

/*
 ===================================================================================================
 ===================================================================================================
 */
unsigned long PLEASE_CUT_ME_IN_PIECE
(
    MAD_R_Vertex * SrcPoints,
    unsigned long NbrOfPtsSrc,
    MAD_R_Vertex * DstPoints,
    int(*IsOut) (MAD_R_Vertex *),
    void(*ComputeCut) (MAD_R_Vertex *, MAD_R_Vertex *,MAD_R_Vertex *)
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_R_Vertex  *CurentP, *CurentPm1;
    unsigned long   Flag, Counter, NbrOfPtsDst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    CurentPm1 = SrcPoints + NbrOfPtsSrc - 1;
    CurentP = SrcPoints;
    Flag = IsOut(CurentPm1);
	NbrOfPtsDst = 0;
    for(Counter = 0; Counter < NbrOfPtsSrc; Counter++)
    {
        Flag = IsOut(CurentP) | (Flag >> 1);
		switch (Flag)
		{
		case 1:
			/* go in  */
            ComputeCut(CurentP, CurentPm1, DstPoints++);
            NbrOfPtsDst++;
		case 0:
            /* Stay in */
            *(DstPoints++) = *CurentP;
            NbrOfPtsDst++;
			break;
		case 2:
			/* Go out */
            ComputeCut(CurentPm1, CurentP, DstPoints++);
			NbrOfPtsDst++;
		}
        CurentPm1 = CurentP++;
    }

    return NbrOfPtsDst;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int IsOut_XMax(MAD_R_Vertex *P)
{
    if(*(long *)&P->X > *(long *)&CI -> XMax) return 2;
    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int IsOut_YMax(MAD_R_Vertex *P)
{
    if(*(long *)&P->Y > *(long *)&CI -> YMax) return 2;
    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int IsOut_XMin(MAD_R_Vertex *P)
{
    if(*(long *)&P->X < *(long *)&CI -> XMin) return 2;
    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int IsOut_YMin(MAD_R_Vertex *P)
{
    if(*(long *)&P->Y < *(long *)&CI -> YMin) return 2;
    return 0;
}
/*
 ===================================================================================================
 ===================================================================================================
 */
int IsOut_ZMin(MAD_R_Vertex *P)
{
//    if(*(long *)&P->Z < *(long *)&CI -> ZMin) return 2;
    if(P->Z < CI -> ZMin) return 2;
    return 0;
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
inline void Compute_One_Gradient(
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
void Compute_Grdients(MAD_R_Vertex *Polygone,unsigned long  NumberOfVertex , MAD_R_Vertex  *PXInc ,MAD_R_Vertex  *PYInc)
{
	unsigned long Counter;
	float 	  XD1,YD1,XD2,YD2,SqrtNorme;
	XD1 = (Polygone[1] . X) - (Polygone[0] . X);
	XD2 = (Polygone[2] . X) - (Polygone[0] . X);
	YD1 = (Polygone[1] . Y) - (Polygone[0] . Y);
	YD2 = (Polygone[2] . Y) - (Polygone[0] . Y);
	/* Compute |XYZ|*/
	SqrtNorme = XD1 * YD2 - YD1 * XD2 ;
	*(unsigned long *)&SqrtNorme &= 0x7FFFFFFF;
	SqrtNorme =  MTH_fn_fInverseOpt(SqrtNorme);
	Compute_One_Gradient(XD1,YD1,XD2,YD2,Polygone->OoZ,(Polygone+1)->OoZ,(Polygone+2)->OoZ,&PXInc -> OoZ,&PYInc -> OoZ,SqrtNorme);
	for (Counter = 0 ; Counter < CI -> NmbrOfSclrToInt; Counter++)
		Compute_One_Gradient(XD1,YD1,XD2,YD2,Polygone->SclrToInt[Counter],(Polygone+1)->SclrToInt[Counter],(Polygone+2)->SclrToInt[Counter],&PXInc -> SclrToInt[Counter],&PYInc -> SclrToInt[Counter],SqrtNorme);
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

/*
 ===================================================================================================
    Z Cliping
 ===================================================================================================
 */
void MAD_Rasterize_TriQuad3D
(
    MAD_R_Vertex * P3,
    MAD_R_Clipping_Info * C,
	float *Surface
)
{
	MAD_R_Vertex ClippingODrome[8] , *Cptr; 
	unsigned long NumberOfVertex,Counter;
	CI = C;
	*Surface = 0.0f;
	NumberOfVertex = PLEASE_CUT_ME_IN_PIECE(P3, 3 , ClippingODrome, IsOut_ZMin , ComputeCut_ZMin );
	if (!NumberOfVertex) return;
	Cptr = ClippingODrome;
	for (Counter = 0 ; Counter < NumberOfVertex; Counter++)
	{
		C -> ProjectVertexCLBK(Cptr , Cptr , C);
		Cptr-> Y= (float)FLOATCAST(Cptr->Y);	
		Cptr++;
		/* all should be multiplly by OoZ Here */
	}
	
	MAD_Rasterize_TriQuad(ClippingODrome,3,CI , Surface);
	if (NumberOfVertex == 4)
	{
		ClippingODrome[1] = ClippingODrome[0];
		MAD_Rasterize_TriQuad(ClippingODrome + 1,3,CI , Surface);
	}
}
/*
 ===================================================================================================
    Z Cliping is done
 ===================================================================================================
 */
#define max_screen_Height 8192
void MAD_Rasterize_TriQuad
(
    MAD_R_Vertex * P3,
	unsigned long NumberOfPoints , /* 3 or 4 */
    MAD_R_Clipping_Info * C ,
	float *Surface
)
{
	MAD_R_Raster RasterODrome[max_screen_Height];
	MAD_R_Raster *CurrentRaster;
	MAD_R_Vertex ClippingODrome_Odd[8]; 
	MAD_R_Vertex ClippingODrome_Evn[8];
    MAD_R_Vertex  *CurentP, *CurentPm1;
	MAD_R_Vertex PXIncremental , PYIncremental;

	unsigned long Counter , YMin , YMax ,Local , InvertRot;

//	if (NumberOfPoints != 3) return;
 
	unsigned long NumberOfVertex;
	*Surface = 0.0f;
	if (!C) return;
	if (NumberOfPoints != 3) 
	{
		if (NumberOfPoints != 4) return;
	}
	CI = C;
/*3[0].Y = (float)FLOATCAST(P3[0].Y);	
	P3[1].Y = (float)FLOATCAST(P3[1].Y);
	P3[2].Y = (float)FLOATCAST(P3[2].Y);
	P3[3].Y = (float)FLOATCAST(P3[3].Y);*/

	InvertRot = 0;
	if (((P3[2].X - P3[1].X) * (P3[0].Y - P3[1].Y)) -
		((P3[2].Y - P3[1].Y) * (P3[0].X - P3[1].X)) < 0.f) 
	{
		InvertRot = 1;
		if (!CI -> Force2sidedFLAG)
		{
			return;
		}
	}

	CurentPm1 = P3 + NumberOfPoints - 1;
	CurentP = P3;
    /* Clip XMin */
	NumberOfVertex = PLEASE_CUT_ME_IN_PIECE(P3, NumberOfPoints , ClippingODrome_Odd, IsOut_XMin , ComputeCut_XMin );
	if (!NumberOfVertex) return;
	NumberOfVertex = PLEASE_CUT_ME_IN_PIECE(ClippingODrome_Odd, NumberOfVertex, ClippingODrome_Evn, IsOut_XMax , ComputeCut_XMax );
	if (!NumberOfVertex) return;
	NumberOfVertex = PLEASE_CUT_ME_IN_PIECE(ClippingODrome_Evn, NumberOfVertex, ClippingODrome_Odd, IsOut_YMin , ComputeCut_YMin );
	if (!NumberOfVertex) return;
	NumberOfVertex = PLEASE_CUT_ME_IN_PIECE(ClippingODrome_Odd, NumberOfVertex, ClippingODrome_Evn, IsOut_YMax , ComputeCut_YMax );
	if (!NumberOfVertex) return;

	{
		float Len;
		for (Counter = 0 ; Counter < NumberOfPoints ; Counter++ )
		{
			Len = (CurentPm1 -> X - CurentP -> X) * (CurentPm1 -> X - CurentP -> X) + (CurentP -> Y - CurentPm1 -> Y) * (CurentP -> Y - CurentPm1 -> Y);
			if (Len > *Surface) *Surface= Len;
			CurentPm1 = CurentP;
			CurentP++;
		}//*/
	}
	/* ClippingODrome_Evn contain clipping result */
	/* Compute RasterODrome */
	Compute_Grdients(P3 , NumberOfPoints , &PXIncremental , &PYIncremental );
	if (InvertRot)
	{
		vctMul(&PXIncremental, -1.0f);
		vctMul(&PYIncremental, -1.0f);
	}

	CurentP = ClippingODrome_Evn;
	CurentPm1 = ClippingODrome_Evn + NumberOfVertex - 1;
	YMin = max_screen_Height;
	YMax = 0;
	if (CI -> NmbrOfSclrToInt == 0)
	{
		for (Counter=0 ; Counter< NumberOfVertex; Counter++)
		{
			Local = FLOATCAST(CurentP -> Y);
			if (Local > YMax) YMax = Local ;
			if (Local < YMin) YMin = Local ;
			if (InvertRot)
				Vectorize_0(RasterODrome , CurentPm1 , CurentP , &PXIncremental , &PYIncremental );
			else
				Vectorize_0(RasterODrome , CurentP , CurentPm1 , &PXIncremental , &PYIncremental );
			CurentPm1 = CurentP++;
		}
	} else
		for (Counter=0 ; Counter< NumberOfVertex; Counter++)
		{
			Local = FLOATCAST(CurentP -> Y);
			if (Local > YMax) YMax = Local ;
			if (Local < YMin) YMin = Local ;
			if (InvertRot)
				Vectorize(RasterODrome , CurentPm1 , CurentP , &PXIncremental , &PYIncremental );
			else
				Vectorize(RasterODrome , CurentP , CurentPm1 , &PXIncremental , &PYIncremental );

			CurentPm1 = CurentP++;
		}
	/* Call RasterizeCallback */
	CurrentRaster = RasterODrome + YMin;
	if (C->RasterizeCallback) 
	for (Counter=YMin ; Counter < YMax; Counter++)
	{
		CI -> RasterizeCallback(Counter , CurrentRaster++ , &PXIncremental);
	}
	CurentP = ClippingODrome_Evn;
	CurentPm1 = ClippingODrome_Evn + NumberOfVertex - 1;
	if (CI -> MAD_DrawLineCLBK)
	{
		for (Counter=0 ; Counter< NumberOfVertex; Counter++)
		{
			CI -> MAD_DrawLineCLBK(CurentP , CurentPm1);
			CurentPm1 = CurentP++;
		}
	}
}

void ClipDrawLine (  MAD_R_Vertex * P2,MAD_R_Clipping_Info * C , void (*MAD_DrawLineCLBK)(MAD_R_Vertex *A, MAD_R_Vertex * B))
{
	MAD_R_Vertex ClippingODrome[8] , *Cptr; 
	MAD_R_Vertex ClippingODrome_Odd[8]; 
	MAD_R_Vertex ClippingODrome_Evn[8];
    MAD_R_Vertex  *CurentP, *CurentPm1 ;
	unsigned long NumberOfVertex , Counter;
	CI = C;
	NumberOfVertex = PLEASE_CUT_ME_IN_PIECE(P2, 2 , ClippingODrome, IsOut_ZMin , ComputeCut_ZMin );
	if (!NumberOfVertex) return;
	Cptr = ClippingODrome;
	for (Counter = 0 ; Counter < NumberOfVertex; Counter++)
	{
		C -> ProjectVertexCLBK(Cptr , Cptr , C);
		Cptr++;
		/* all should be multiplly by OoZ Here */
	}

	Cptr = ClippingODrome;
	NumberOfVertex = PLEASE_CUT_ME_IN_PIECE(ClippingODrome, NumberOfVertex , ClippingODrome_Odd, IsOut_XMin , ComputeCut_XMin );
	if (!NumberOfVertex) return;
	NumberOfVertex = PLEASE_CUT_ME_IN_PIECE(ClippingODrome_Odd, NumberOfVertex, ClippingODrome_Evn, IsOut_XMax , ComputeCut_XMax );
	if (!NumberOfVertex) return;
	NumberOfVertex = PLEASE_CUT_ME_IN_PIECE(ClippingODrome_Evn, NumberOfVertex, ClippingODrome_Odd, IsOut_YMin , ComputeCut_YMin );
	if (!NumberOfVertex) return;
	NumberOfVertex = PLEASE_CUT_ME_IN_PIECE(ClippingODrome_Odd, NumberOfVertex, ClippingODrome_Evn, IsOut_YMax , ComputeCut_YMax );
	if (!NumberOfVertex) return;
	CurentP = ClippingODrome_Evn;
	CurentPm1 = ClippingODrome_Evn + NumberOfVertex - 1;
	MAD_DrawLineCLBK(CurentP , CurentPm1);
}



void MAD_Rasterize_Init()
{
	MTH_fn_vInit();
}
