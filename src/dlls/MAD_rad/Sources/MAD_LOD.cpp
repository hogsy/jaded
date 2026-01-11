/*$T MAD_LOD.cpp GC!1.32 07/24/19 09:25:00 */
#pragma optimize("", off)
//#define MAD_SHOW_ERRORS
#define MAD_SHOW_ERROR_COEF 3000.0f
#define KEEP_ORIGINAL_WITHOUT_RADIOSITY
#define LOD_C_MAX_EDGE_TRIANGLE     4
#define LOC_C_Infinite              10.0e37f
#define LOD_C_FUSION_LOCKED_EROOR   LOC_C_Infinite * 0.5f
#define TIME_SQUARE_EDGE_LIMIT_OPTIM 10L /* */
#define LOD_RADIOSITY_PRAGMA        0.01f
#define LOD_TOPOLOGIC_PRAGMA        0.01f    /* Avoid Coplanar errors */
#define MAD_OVERFLOW_INDEX			0x0fffffff
#define MAD_COLINEAR_EDGE_THRESHOLD 0.9999f

//#define LOD_C_RADIOSITY_GRAY /* optimize*/

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


/*$T MAD_LOD.cpp GC!1.32 07/24/19 09:25:00 */

/* COLOR_TO_VERTEX(color,Vetx,x,y,z) */
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>

#include "MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#define LOD_MAD_MALLOC(address, size) \
{ \
	*(void **) &address = malloc(size); \
	memset(*(void **) &address, 0, size); \
}
#define LOD_MAD_FREE(address) \
{ \
	free(address);\
}

#include "SCD_MTH.cpp"
#include <float.h>

#define OBJ_ERR unsigned long

#define LOD_C_RADIOSITY
#define LOD_RADIOSITY_NO_PEAKS
#define LOD_NICE_MESH_ERROR

#define LOD_REORDER
#define LOD_STOP_LEVEL  9

/* #define FINDER_PACK_SIZE_PO2 1 */


#define LOD_NZDIV                   1.00f

typedef struct  MAD_Simple3DVertexLOCAL_
{
    float           x, y, z;
    unsigned long   diffuse;
    unsigned long   nothing1;
    unsigned long   nothing2;
} MAD_Simple3DVertexLOCAL;

/*
===================================================================================================
===================================================================================================
*/
long LOD_ERRPERCENT(float a, float b, float PRAGM)
{
//	return 0;
/*    if(a == b) return 1;
	a -= b;
	MTH_ABSOLUTE(a);
	if (a < PRAGM) 
		return 1;//*/
     if(a == b) return 1;
	MTH_ABSOLUTE(a);
	MTH_ABSOLUTE(b);
	if ((a < PRAGM) && (b < PRAGM)) return 1;//*/
	return 0;
/*	if (a == 0.0f) return 0;
	if (b == 0.0f) return 0;*/
}

/*
* This file created for compute the lod multi-resolution mesh; IT'S works only on non magnifold
* MESH !! One Edge touch 1 or 2 triangles Philippe Vimont
*/
#include <stdlib.h>
#define CCCC    0

typedef struct LOD_C_Edge_          LOD_C_Edge;
typedef struct LOD_C_Triangle_      LOD_C_Triangle;
typedef struct LOD_C_Vertex_        LOD_C_Vertex;

typedef struct LOD_C_DynamicList_   LOD_C_DynamicList;

typedef struct                      MAD_TripledIndex_
{
    unsigned long   Index[3];
} MAD_TripledIndex;

typedef struct  LOD_C_Triangle_
{
    unsigned long   DisapearStage;
	
    /* Unsigned long PLAQUE_NUMBER; */
	
    /* MAD_Face OriginalTriangle; /* OK */
	
    /* LOD_C_Triangle *AbsorberTriangle; */
    MAD_Vertex      CrossProduct;
    MAD_Vertex      OriginalCrossProduct;
    MAD_Face        Triangle;       /* OK */
    unsigned long   OwnerElement;   /* OK */
    LOD_C_Edge      *Edges[3];      /* OK */
} LOD_C_Triangle;

typedef struct  LOD_C_ErrorPack_
{
    float   TopologicError;
#ifdef LOD_NICE_MESH_ERROR
    float   NiceMeshError;
#endif
    float   RadiosityMeshError;
} LOD_C_ErrorPack;

typedef struct  LOD_C_Edge_
{
    unsigned long   DisapearStage;
#ifdef KEEP_ORIGINAL_WITHOUT_RADIOSITY
	unsigned long	bIsVisible;
#endif
	
    unsigned long   PointA;                                         /* OK */
    unsigned long   PointB;                                         /* OK */
    LOD_C_ErrorPack ErrorAB;
    LOD_C_ErrorPack ErrorBA;
	
    LOD_C_Edge      *NextEdgeAX;
    LOD_C_Edge      *NextEdgeBX;
	
    /* NeighBourg */
    unsigned long   NumberOfTriangles;                              /* OK */
    LOD_C_Triangle  *FirstEdgeToTriangle[LOD_C_MAX_EDGE_TRIANGLE];  /* OK */
} LOD_C_Edge;

typedef struct  LOD_C_TRACE_
{
    unsigned long   VertexToDelete;
    unsigned long   VertexToCollapse;
    unsigned long   Reorder;
} LOD_C_TRACE;

typedef struct  LOD_C_Vertex_
{
    unsigned long   DisapearStage;
    LOD_C_ErrorPack MinimalError;
    unsigned long   MinimalErrorIndex;
#ifdef KEEP_ORIGINAL_WITHOUT_RADIOSITY
    unsigned long	ulFreedomDimension;
	unsigned long	OriginalEdgeNum; /* valid only if ulFreedomDimension == 1 */
#endif
	
//    unsigned long   Absorber;
	
//    unsigned long   TAG;
	
    /* NeighBourg */
    unsigned long   NumberOfEdges;                                  /* OK */
    LOD_C_Edge      *BeginEdge;
    LOD_C_Edge      **FirstEdge;                                    /* OK */
	
    unsigned long   NumberOftriangles_VT;                           /* OK */
    LOD_C_Triangle  **FirstTriangle_VT;                             /* OK */
} LOD_C_Vertex;

typedef struct  LOD_C_MiniFinder_
{
    unsigned long   Index;
} LOD_C_MiniFinder;

typedef struct  LOD_C_ComputationalObject_
{
    MAD_GeometricObject *Object;                                    /* OK */
    unsigned long       NumberOfVertices;                           /* OK */
	unsigned long       OriginalNumBerOfPts;
    LOD_C_Vertex        *AllVertices;                               /* OK */
    LOD_C_TRACE         *AllTrace;
    LOD_C_TRACE         *CurrentTrace;
    LOD_C_Triangle      **AllVertexTriangles1;                      /* OK */
    LOD_C_Edge          **AllVertexEdge1;                           /* OK */
	
    unsigned long       NumberOfEdges;                              /* OK */
    LOD_C_Edge          *AllEdges;                                  /* OK */
	
    /* LOD_C_Triangle **AllEdgesTriangles; /* OK */
    unsigned long       NumberOftriangles;                          /* OK */
    LOD_C_Triangle      *AllTriangles;                              /* OK */
	
    unsigned long       ActualStage;
	
    float               ErrMax, ErrMin, ErrorThreshold;
    float               RadiosityErrorMax;
	
    unsigned long       FinderMaxStage;
    LOD_C_MiniFinder    *FirstFinder[32];
    LOD_C_MiniFinder    *AllFinders;
	unsigned long		ComputeUV;
	unsigned long		ulOriginalNumberOfPoints;
} LOD_C_ComputationalObject;

/*
===================================================================================================
Unsigned long LOD_C_MallocEdges(LOD_C_ComputationalObject *CO , )
===================================================================================================
*/
LOD_C_Edge  *LOD_C_GetCommonEdge(LOD_C_ComputationalObject *CO, unsigned long V1, unsigned long V2);

/*
===================================================================================================
Compute the normal of a triangle
===================================================================================================
*/
void LOD_C_ComputeTriangleCrossProduct(LOD_C_ComputationalObject *CO, LOD_C_Triangle *Triangle)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Vertex  V1, V2, *V3, *V4;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    V4 = (MAD_Vertex *) (CO->Object->OBJ_PointList + Triangle->Triangle.Index[0]);
    V3 = (MAD_Vertex *) (CO->Object->OBJ_PointList + Triangle->Triangle.Index[1]);
    SUB_VECTOR(V4, V3, (&V1), x, y, z);
    V3 = (MAD_Vertex *) (CO->Object->OBJ_PointList + Triangle->Triangle.Index[2]);
    SUB_VECTOR(V4, V3, (&V2), x, y, z);
    CROSS_PRODUCT((&V2), (&V1), (&Triangle->CrossProduct), x, y, z);
	
    /* Triangle -> OriginalCrossProduct = Triangle -> CrossProduct ; */
}

#ifdef LOD_NICE_MESH_ERROR

/*
===================================================================================================
Compute the error generated by moving one vertex in one direction; Try To create a nice MESH
===================================================================================================
*/
float LOD_C_ComputeEdgeLenghtForError2
(
 LOD_C_ComputationalObject   *CO,
 MAD_Vertex                  *MovingVector,
 LOD_C_Edge                  *LCE_Ptr,
 unsigned long               VertexToCompute,
 unsigned long               SecondEdge
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Vertex  Edge, Edge2;
    float       Lenght;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Edge = *(MAD_Vertex *) (CO->Object->OBJ_PointList + (LCE_Ptr)->PointA);
    Edge2 = *(MAD_Vertex *) (CO->Object->OBJ_PointList + (LCE_Ptr)->PointB);
    if(LCE_Ptr->PointB == VertexToCompute)
    {
        ADD_VECTOR((&Edge2), MovingVector, (&Edge2), x, y, z);
    }
    else if(LCE_Ptr->PointA == VertexToCompute)
    {
        ADD_VECTOR((&Edge), MovingVector, (&Edge), x, y, z);
    }
    else
        LCE_Ptr = LCE_Ptr;
	
    SUB_VECTOR((&Edge2), (&Edge), (&Edge), x, y, z);
    NORME((&Edge), Lenght, x, y, z);
    return Lenght;
}

#endif
/*
===================================================================================================
Compute the error generated by moving one vertex in one direction; This look like a kite
===================================================================================================
*/
#ifdef KEEP_ORIGINAL_WITHOUT_RADIOSITY
float LOD_C_ComputeKiteError
(
 LOD_C_ComputationalObject   *CO,
 MAD_Vertex                  *MovingVector,
 MAD_Vertex                  *MovingVectorNORMALIZED,
 LOD_C_Edge                  *LCE_Ptr,
 unsigned long               VertexToCompute,
 unsigned long               VertexToReceive
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Vertex              Edge, Local;
    MAD_Simple3DVertexLOCAL SaveVertexToMove;
    MAD_Vertex              SAVECP0, SAVECP1;
    float                   Surface, Coef/* , Coef2 */;
    unsigned long           TriangleCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (CO -> AllVertices [VertexToCompute] . ulFreedomDimension < CO -> AllVertices [VertexToReceive] . ulFreedomDimension)
		return LOC_C_Infinite;
	if ((CO -> AllVertices [VertexToCompute] . ulFreedomDimension == 1 ) && ( CO -> AllVertices [VertexToReceive] . ulFreedomDimension == 1))
	{
		if (CO -> AllVertices [VertexToCompute] . OriginalEdgeNum != CO -> AllVertices [VertexToReceive] . OriginalEdgeNum)
			return LOC_C_Infinite;
	}
//	return 0.0f;
    if(LCE_Ptr->NumberOfTriangles == 0) 
		return 0.0f;

    if ((LCE_Ptr->FirstEdgeToTriangle[0]->Triangle.Index[0] != VertexToCompute) &&
        (LCE_Ptr->FirstEdgeToTriangle[0]->Triangle.Index[1] != VertexToCompute) &&
        (LCE_Ptr->FirstEdgeToTriangle[0]->Triangle.Index[2] != VertexToCompute))
		Coef = 1.0f;
	if(LCE_Ptr->NumberOfTriangles > 1) 
    if ((LCE_Ptr->FirstEdgeToTriangle[1]->Triangle.Index[0] != VertexToCompute) &&
        (LCE_Ptr->FirstEdgeToTriangle[1]->Triangle.Index[1] != VertexToCompute) &&
        (LCE_Ptr->FirstEdgeToTriangle[1]->Triangle.Index[2] != VertexToCompute))
		Coef = 1.0f;
	
    Edge = *(MAD_Vertex *) (CO->Object->OBJ_PointList + (LCE_Ptr)->PointA);
    Local = *(MAD_Vertex *) (CO->Object->OBJ_PointList + (LCE_Ptr)->PointB);
    SUB_VECTOR((&Edge), (&Local), (&Edge), x, y, z);

    /*  */
    CROSS_PRODUCT(MovingVector, (&Edge), (&Local), x, y, z);
    NORME((&Local), Surface, x, y, z);
    Surface = MTH_fn_fSquareRootOpt(Surface);

    /* if it's a border, return pure error */
    if  ((LCE_Ptr->NumberOfTriangles != 2)  ||
		(LCE_Ptr->FirstEdgeToTriangle [0]->OwnerElement != 
		 LCE_Ptr->FirstEdgeToTriangle [1]->OwnerElement) ||
		 LCE_Ptr->bIsVisible)
	{
		NORMALIZE((&Edge), x, y, z);
		Coef = DOT_PRODUCT((&Edge), MovingVectorNORMALIZED, x, y, z);
		MTH_ABSOLUTE(Coef);
		if (Coef > MAD_COLINEAR_EDGE_THRESHOLD)
			return 0.0f;
		else
			return Surface + LOD_TOPOLOGIC_PRAGMA;
	}

    LOD_C_ComputeTriangleCrossProduct(CO, LCE_Ptr->FirstEdgeToTriangle[0]);
    LOD_C_ComputeTriangleCrossProduct(CO, LCE_Ptr->FirstEdgeToTriangle[1]);
	
    SAVECP0 = (LCE_Ptr->FirstEdgeToTriangle[0])->CrossProduct;
    SAVECP1 = (LCE_Ptr->FirstEdgeToTriangle[1])->CrossProduct;
	
    SaveVertexToMove = *(MAD_Simple3DVertexLOCAL *) (CO->Object->OBJ_PointList + VertexToCompute);
	*(MAD_Simple3DVertexLOCAL *)(CO->Object->OBJ_PointList + VertexToCompute) = *(MAD_Simple3DVertexLOCAL *)(CO->Object->OBJ_PointList + VertexToReceive);
    TriangleCounter = 0;
	
    LOD_C_ComputeTriangleCrossProduct(CO, LCE_Ptr->FirstEdgeToTriangle[0]);
    LOD_C_ComputeTriangleCrossProduct(CO, LCE_Ptr->FirstEdgeToTriangle[1]);
	
	/* Test flipping on triangle 0 */
    Coef = DOT_PRODUCT((&(LCE_Ptr->FirstEdgeToTriangle[0])->OriginalCrossProduct), (&(LCE_Ptr->FirstEdgeToTriangle[0])->CrossProduct), x, y, z);
	/* Test flipping on triangle 0 */
    if ((LCE_Ptr->FirstEdgeToTriangle[0]->Triangle.Index[0] == VertexToReceive) ||
        (LCE_Ptr->FirstEdgeToTriangle[0]->Triangle.Index[1] == VertexToReceive) ||
        (LCE_Ptr->FirstEdgeToTriangle[0]->Triangle.Index[2] == VertexToReceive))
		Coef = 1.0f;
    if(Coef <= 0.0f) 
		TriangleCounter |= 1;
	/* Test flipping on triangle 1 */
    Coef = DOT_PRODUCT((&(LCE_Ptr->FirstEdgeToTriangle[1])->OriginalCrossProduct), (&(LCE_Ptr->FirstEdgeToTriangle[1])->CrossProduct), x, y, z);
    if ((LCE_Ptr->FirstEdgeToTriangle[1]->Triangle.Index[0] == VertexToReceive) ||
        (LCE_Ptr->FirstEdgeToTriangle[1]->Triangle.Index[1] == VertexToReceive) ||
        (LCE_Ptr->FirstEdgeToTriangle[1]->Triangle.Index[2] == VertexToReceive))
		Coef = 1.0f;
    if(Coef <= 0.0f) 
		TriangleCounter |= 1;

    (LCE_Ptr->FirstEdgeToTriangle[0])->CrossProduct = SAVECP0;
    (LCE_Ptr->FirstEdgeToTriangle[1])->CrossProduct = SAVECP1;

	/* Restore Vertex */
    *(CO->Object->OBJ_PointList + VertexToCompute) = *(MAD_Simple3DVertex *) &SaveVertexToMove;
	
    if(TriangleCounter) 
		return Surface + LOD_TOPOLOGIC_PRAGMA;
	
    /* Return Surface; */
	
    /* Compute Normale of the Edge; */
    return 0.0f;
}
#else
float LOD_C_ComputeKiteError
(
 LOD_C_ComputationalObject   *CO,
 MAD_Vertex                  *MovingVector,
 MAD_Vertex                  *MovingVectorNORMALIZED,
 LOD_C_Edge                  *LCE_Ptr,
 unsigned long               VertexToCompute,
 unsigned long               VertexToReceive
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Vertex              Edge, Local;
    MAD_Simple3DVertexLOCAL SaveVertexToMove;
    MAD_Vertex              SAVECP0, SAVECP1;
    float                   Surface, Coef/* , Coef2 */;
    unsigned long           TriangleCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if(LCE_Ptr->NumberOfTriangles == 0) return 0.0f;
    Edge = *(MAD_Vertex *) (CO->Object->OBJ_PointList + (LCE_Ptr)->PointA);
    Local = *(MAD_Vertex *) (CO->Object->OBJ_PointList + (LCE_Ptr)->PointB);
    SUB_VECTOR((&Edge), (&Local), (&Edge), x, y, z);

    /*  */
    CROSS_PRODUCT(MovingVector, (&Edge), (&Local), x, y, z);
    NORME((&Local), Surface, x, y, z);
    Surface = MTH_fn_fSquareRootOpt(Surface);

    /* if it's a border, return pure error */
    if  ((LCE_Ptr->NumberOfTriangles != 2)  ||
		(LCE_Ptr->FirstEdgeToTriangle [0]->OwnerElement != 
		 LCE_Ptr->FirstEdgeToTriangle [1]->OwnerElement))
	{
		NORMALIZE((&Edge), x, y, z);
		Coef = DOT_PRODUCT((&Edge), MovingVectorNORMALIZED, x, y, z);
		MTH_ABSOLUTE(Coef);
		if (Coef > MAD_COLINEAR_EDGE_THRESHOLD)
			return 0.0f;
		else
			return Surface + LOD_TOPOLOGIC_PRAGMA;
	}

    if ((LCE_Ptr->FirstEdgeToTriangle[0]->Triangle.Index[0] == VertexToReceive) ||
        (LCE_Ptr->FirstEdgeToTriangle[0]->Triangle.Index[1] == VertexToReceive) ||
        (LCE_Ptr->FirstEdgeToTriangle[0]->Triangle.Index[2] == VertexToReceive) ||
        (LCE_Ptr->FirstEdgeToTriangle[1]->Triangle.Index[0] == VertexToReceive) ||
        (LCE_Ptr->FirstEdgeToTriangle[1]->Triangle.Index[1] == VertexToReceive) ||
        (LCE_Ptr->FirstEdgeToTriangle[1]->Triangle.Index[2] == VertexToReceive)) 
		return 0.0f;


    LOD_C_ComputeTriangleCrossProduct(CO, LCE_Ptr->FirstEdgeToTriangle[0]);
    LOD_C_ComputeTriangleCrossProduct(CO, LCE_Ptr->FirstEdgeToTriangle[1]);
	
    SAVECP0 = (LCE_Ptr->FirstEdgeToTriangle[0])->CrossProduct;
    SAVECP1 = (LCE_Ptr->FirstEdgeToTriangle[1])->CrossProduct;
	
    SaveVertexToMove = *(MAD_Simple3DVertexLOCAL *) (CO->Object->OBJ_PointList + VertexToCompute);
	*(MAD_Simple3DVertexLOCAL *)(CO->Object->OBJ_PointList + VertexToCompute) = *(MAD_Simple3DVertexLOCAL *)(CO->Object->OBJ_PointList + VertexToReceive);
    TriangleCounter = 0;
	
    LOD_C_ComputeTriangleCrossProduct(CO, LCE_Ptr->FirstEdgeToTriangle[0]);
    LOD_C_ComputeTriangleCrossProduct(CO, LCE_Ptr->FirstEdgeToTriangle[1]);
	
	/* Test flipping on triangle 0 */
    Coef = DOT_PRODUCT((&(LCE_Ptr->FirstEdgeToTriangle[0])->OriginalCrossProduct), (&(LCE_Ptr->FirstEdgeToTriangle[0])->CrossProduct), x, y, z);
	/* Test flipping on triangle 0 */
    if(Coef <= 0.0f) 
		TriangleCounter |= 1;
	/* Test flipping on triangle 1 */
    Coef = DOT_PRODUCT((&(LCE_Ptr->FirstEdgeToTriangle[1])->OriginalCrossProduct), (&(LCE_Ptr->FirstEdgeToTriangle[1])->CrossProduct), x, y, z);
    if(Coef <= 0.0f) 
		TriangleCounter |= 1;

    (LCE_Ptr->FirstEdgeToTriangle[0])->CrossProduct = SAVECP0;
    (LCE_Ptr->FirstEdgeToTriangle[1])->CrossProduct = SAVECP1;

	/* Restore Vertex */
    *(CO->Object->OBJ_PointList + VertexToCompute) = *(MAD_Simple3DVertex *) &SaveVertexToMove;
	
    if(TriangleCounter) 
		return Surface + LOD_TOPOLOGIC_PRAGMA;
	
    /* Return Surface; */
	
    /* Compute Normale of the Edge; */
    ADD_VECTOR_NORMALIZED((&(LCE_Ptr->FirstEdgeToTriangle[0])->CrossProduct), (&(LCE_Ptr->FirstEdgeToTriangle[1])->CrossProduct), (&Edge), x, y, z);
    NORMALIZE((&Edge), x, y, z);
    Coef = DOT_PRODUCT((&Edge), MovingVectorNORMALIZED, x, y, z);
    MTH_ABSOLUTE(Coef);
	if (Coef < 0.001f /* floating point Error! */) return 0.0f;
    return Surface * Coef;
}
#endif

#ifdef LOD_C_RADIOSITY

/*
===================================================================================================
===================================================================================================
*/
void GRADIENT_DIRECTION
(
 MAD_Simple3DVertexLOCAL *PA,
 MAD_Simple3DVertexLOCAL *PB,
 MAD_Simple3DVertexLOCAL *PC,
 MAD_Simple3DVertexLOCAL *NT,
 float                   GA,
 float                   GB,
 float                   GC,
 MAD_Vertex              *NTP
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Vertex  PAP, PBP, PCP;
    float       DPR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    ADD_MUL_ADD_VECTOR(PA, NT, GA, (&PAP), x, y, z);
    ADD_MUL_ADD_VECTOR(PB, NT, GB, (&PBP), x, y, z);
    ADD_MUL_ADD_VECTOR(PC, NT, GC, (&PCP), x, y, z);
    NORMALE_TRIANGLE((&PAP), (&PBP), (&PCP), NTP, x, y, z);
    DPR = DOT_PRODUCT(NTP, NT, x, y, z);
    ADD_MUL_ADD_VECTOR(NTP, NT, -DPR, NTP, x, y, z);
    DPR = DOT_PRODUCT(NTP, NT, x, y, z);
    ADD_MUL_ADD_VECTOR(NTP, NT, -DPR, NTP, x, y, z);
    DPR = DOT_PRODUCT(NTP, NT, x, y, z);
    ADD_MUL_ADD_VECTOR(NTP, NT, -DPR, NTP, x, y, z);
}

/*
===================================================================================================
===================================================================================================
*/
#ifdef LOD_RADIOSITY_NO_PEAKS
float Compute_Peak( MAD_Simple3DVertexLOCAL *V0, MAD_Simple3DVertexLOCAL *V1 , MAD_Simple3DVertexLOCAL *V2)
{
	MAD_Simple3DVertexLOCAL P;
	float N01, N12 , N20;
	float MAX, MIN;
	SUB_VECTOR(V0, V1, (&P), x, y, z);
	NORME((&P), N01, x, y, z);
	
	SUB_VECTOR(V1, V2, (&P), x, y, z);
	NORME((&P), N12, x, y, z);
	
	SUB_VECTOR(V2, V0, (&P), x, y, z);
	NORME((&P), N20, x, y, z);
	MAX = N01;
	if (N12 > MAX) MAX = N12;
	if (N20 > MAX) MAX = N20;
	MIN = N01;
	if (N12 < MIN) MIN = N12;
	if (N20 < MIN) MIN = N20;
	N01 = 0.0f;
	if (MIN != 0.0f) N01 = MAX * MTH_fn_fInverseOpt(MIN);
	if (N01 > 5.0f) N01 = 5.0f;
	return N01;
		
}
#endif /* LOD_RADIOSITY_NO_PEAKS */

/*
===================================================================================================
===================================================================================================
*/
#define MAD_3Colors21Scalar(Scl,a) \
	Scl = 0.0f;\
	if (FlagRGBA & 1) Scl += (float)MAD_GetRed(*a);\
	if (FlagRGBA & 2) Scl += (float)MAD_GetGreen(*a);\
	if (FlagRGBA & 4) Scl += (float)MAD_GetBlue(*a);\
	if (FlagRGBA & 8) Scl += (float)MAD_GetAlpha(*a);

float LOD_C_ComputeKiteErrorRadiosity
(
 LOD_C_ComputationalObject   *CO,
 LOD_C_Edge                  *LCE_Ptr,
 unsigned long               VertexToCompute,
 unsigned long               VertexToReceive,
 unsigned long				 FlagRGBA
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long           NumberOfCatchedIndex, Counter ;
    unsigned long           CounterSI, ChangedIndex;
    MAD_Vertex              GDDIR, DIFF;
    MAD_Simple3DVertexLOCAL V0, V1, V2, NT, VR;
    float                   Error, G0, G1, G2, T1, T2, T1P, T2P, DELTA, SCALARDEST, NTRI, NTRI1 ;
#ifdef LOD_RADIOSITY_NO_PEAKS
	float		Peak_end;
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    NumberOfCatchedIndex = 0;
    if((LCE_Ptr->PointA == VertexToReceive) || (LCE_Ptr->PointB == VertexToReceive)) return 0.0f;
	
    Error = 0;
    Counter = LCE_Ptr->NumberOfTriangles;
    VR = *(MAD_Simple3DVertexLOCAL *) (CO->Object->OBJ_PointList + VertexToCompute);
	MAD_3Colors21Scalar(
		SCALARDEST,
		(CO->Object->Colors + VertexToCompute));
 
    /* ((VR->diffuse & 0xff) + ((VR->diffuse >> 8) & 0xff) + ((VR->diffuse >> 16) & 0xff)); */
    while(Counter--)
    {
        if
			(
            (LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[0] == VertexToReceive) ||
            (LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[1] == VertexToReceive) ||
            (LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[2] == VertexToReceive)
			) continue;
		
        CounterSI = 3;
        ChangedIndex = 0xffffffff;
        while(CounterSI--)
        {
            if(LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[CounterSI] == VertexToCompute)
            {
                ChangedIndex = CounterSI;
                LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[CounterSI] = VertexToReceive;
                CounterSI = 0;
            }
        }
		
        NT = *(MAD_Simple3DVertexLOCAL *) (&LCE_Ptr->FirstEdgeToTriangle[Counter]->CrossProduct);
        NORME((&NT), NTRI1, x, y, z);
        LOD_C_ComputeTriangleCrossProduct(CO, LCE_Ptr->FirstEdgeToTriangle[Counter]);
		
        V0 = *(MAD_Simple3DVertexLOCAL *) (CO->Object->OBJ_PointList + LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[0]);
        V1 = *(MAD_Simple3DVertexLOCAL *) (CO->Object->OBJ_PointList + LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[1]);
        V2 = *(MAD_Simple3DVertexLOCAL *) (CO->Object->OBJ_PointList + LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[2]);
		MAD_3Colors21Scalar(
			G0,
			(CO->Object->Colors + LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[0]));
		
        /* ((V0->diffuse & 0xff) + ((V0->diffuse >> 8) & 0xff) + ((V0->diffuse >> 16) & 0xff)); */
		MAD_3Colors21Scalar(
			G1,
			(CO->Object->Colors + LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[1]));
		
        /* ((V1->diffuse & 0xff) + ((V1->diffuse >> 8) & 0xff) + ((V1->diffuse >> 16) & 0xff)); */
		MAD_3Colors21Scalar(
			G2,
			(CO->Object->Colors + LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[2]));
		
        /* ((V2->diffuse & 0xff) + ((V2->diffuse >> 8) & 0xff) + ((V2->diffuse >> 16) & 0xff)); */
        NT = *(MAD_Simple3DVertexLOCAL *) (&LCE_Ptr->FirstEdgeToTriangle[Counter]->CrossProduct);
        NORME((&NT), NTRI, x, y, z);
#ifdef LOD_RADIOSITY_NO_PEAKS
		Peak_end = Compute_Peak( &V0, &V1 , &V2);
#endif
		
        NT = *(MAD_Simple3DVertexLOCAL *) (&LCE_Ptr->FirstEdgeToTriangle[Counter]->OriginalCrossProduct);
		
        /* NT . x = 0.0f; NT . y = 0.0f; V0 . z = 0.0f; V1 . z = 0.0f; V2 . z = 0.0f; VR . z = 0.0f; */
        NORMALIZE((&NT), x, y, z);
        GRADIENT_DIRECTION(&V0, &V1, &V2, (&NT), G0, G1, G2, (&GDDIR));
        NORMALIZE((&GDDIR), x, y, z);
        T1P = T1 = G0 - G1;
        T2P = T2 = G0 - G2;
        MTH_ABSOLUTE(T1P);
        MTH_ABSOLUTE(T2P);
		
		
        if((T1 == 0.0f) && (T2 == 0.0f))
        {
            DELTA = 0.0f;
        }
        else
        {
            if(T1P > T2P)
            {
                SUB_VECTOR((&V0), (&V1), (&NT), x, y, z);
                T2 = DOT_PRODUCT((&GDDIR), (&NT), x, y, z);
                if(T2 == 0.0f)
                {
                    SUB_VECTOR((&V0), (&V2), (&NT), x, y, z);
                    T1 = DOT_PRODUCT((&GDDIR), (&NT), x, y, z);
                    if(T1 == 0.0f)
                        DELTA = 0.0f;
                    else
                        DELTA = T2 / T1;
                }
                else
                    DELTA = T1 / T2;
            }
            else
            {
                SUB_VECTOR((&V0), (&V2), (&NT), x, y, z);
                T1 = DOT_PRODUCT((&GDDIR), (&NT), x, y, z);
                if(T1 == 0.0f)
                {
                    SUB_VECTOR((&V0), (&V1), (&NT), x, y, z);
                    T2 = DOT_PRODUCT((&GDDIR), (&NT), x, y, z);
                    if(T2 == 0.0f)
                        DELTA = 0.0f;
                    else
                        DELTA = T1 / T2;
                }
                else
                    DELTA = T2 / T1;
            }
        }
		
		
        if(DELTA != 0.0f)
        {
            SUB_VECTOR((&V0), (&VR), (&DIFF), x, y, z);
            G1 = DOT_PRODUCT((&GDDIR), (&DIFF), x, y, z);
            G1 = G0 - G1 * DELTA;
            G1 = SCALARDEST - G1;
//			G1 *= G1 ;
            G1 *= MTH_fn_fSquareRootOpt(NTRI) + MTH_fn_fSquareRootOpt(NTRI1);
#ifdef LOD_RADIOSITY_NO_PEAKS
			G1 *= Peak_end ;
#endif
            MTH_ABSOLUTE(G1);
            Error += G1;
        }
        else
        {
            G0 = G0 - SCALARDEST;
            MTH_ABSOLUTE(G0);
            T1 = G1 - SCALARDEST;
            MTH_ABSOLUTE(T1);
            if(T1 > G0) G0 = T1;
            T1 = G2 - SCALARDEST;
            MTH_ABSOLUTE(T1);
            if(T1 > G0) G0 = T1;
//			G0 *= G0 ;
            G0 *= MTH_fn_fSquareRootOpt(NTRI) + MTH_fn_fSquareRootOpt(NTRI1);
#ifdef LOD_RADIOSITY_NO_PEAKS
			G0 *= Peak_end ;
#endif
            MTH_ABSOLUTE(G0);
            Error += G0;
        }
		if (_isnan((double)Error))
			T2P = T2P;
		
		
        LCE_Ptr->FirstEdgeToTriangle[Counter]->Triangle.Index[ChangedIndex] = VertexToCompute;
        LOD_C_ComputeTriangleCrossProduct(CO, LCE_Ptr->FirstEdgeToTriangle[Counter]);
    }

	
    return Error;
}

#endif

/*
===================================================================================================
===================================================================================================
*/
void LOD_C_SetUntouchablePACK(LOD_C_ComputationalObject *CO, LOD_C_ErrorPack *E)
{
    E->RadiosityMeshError = E->TopologicError = LOC_C_Infinite;
    E->NiceMeshError = LOC_C_Infinite;
}
/*
===================================================================================================
===================================================================================================
*/
void LOD_C_SetWorstPACK(LOD_C_ComputationalObject *CO, LOD_C_ErrorPack *E)
{
    E->RadiosityMeshError = E->TopologicError = LOD_C_FUSION_LOCKED_EROOR;
    E->NiceMeshError = LOD_C_FUSION_LOCKED_EROOR;
}

/*
===================================================================================================
===================================================================================================
*/
void LOD_C_SetLowestPACK(LOD_C_ComputationalObject *CO, LOD_C_ErrorPack *E)
{
    E->RadiosityMeshError = E->TopologicError = 0;
#ifdef LOD_NICE_MESH_ERROR
    E->NiceMeshError = 0.0f;
#endif
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long LOD_C_ChooseTheBestPACK(LOD_C_ComputationalObject *CO, LOD_C_ErrorPack *E1, LOD_C_ErrorPack *E2)
{
	if (_isnan(E1->TopologicError) || 
		_isnan(E2->TopologicError) ||
		_isnan(E1->RadiosityMeshError) ||
		_isnan(E2->RadiosityMeshError))
		CO->ErrMax = CO->ErrMax ; // BUG!!
	
    if(E1->TopologicError < LOD_C_FUSION_LOCKED_EROOR)
    {
        if(CO->ErrMax < E1->TopologicError)
            CO->ErrMax = E1->TopologicError;
    }
	
    if(E2->TopologicError < LOD_C_FUSION_LOCKED_EROOR)
    {
        if(CO->ErrMax < E2->TopologicError)
            CO->ErrMax = E2->TopologicError;
    }
	
    /*
	* If (CO -> ErrMax != LOD_C_FUSION_LOCKED_EROOR) CO -> ErrorThreshold = CO -> ErrMax *
	* LOD_C_TOPOLOGIE_THRESHOLD; if ((E2 -> TopologicError < CO -> ErrorThreshold) && (E1 ->
	* TopologicError < CO -> ErrorThreshold))
	*/
    if(LOD_ERRPERCENT(E2->TopologicError, E1->TopologicError, LOD_TOPOLOGIC_PRAGMA))
    {
        if(E2->RadiosityMeshError > E1->RadiosityMeshError)
            return 0;
        else
            return 1;
    } //*/
	
    if(E2->TopologicError < E1->TopologicError)
        return 1;
    else 
		return 0;

}

/*
===================================================================================================
===================================================================================================
*/
unsigned long LOD_C_ChooseTheBest(LOD_C_ComputationalObject *CO, unsigned long V1, unsigned long V2)
{
	if (V2 == MAD_OVERFLOW_INDEX)
        return V1;

	if (V1 == MAD_OVERFLOW_INDEX)
        return V2;

    if((CO->AllVertices + V2)->DisapearStage)
        return V1;
	
    if((CO->AllVertices + V1)->DisapearStage)
        return V2;
	
#ifdef KEEP_ORIGINAL_WITHOUT_RADIOSITY
	if ((V1 < CO->ulOriginalNumberOfPoints) &&
		(V2 < CO->ulOriginalNumberOfPoints))
	{
		if (V1 < V2) 
			return V2;
		else
			return V1;
	}
	if (V1 < CO->ulOriginalNumberOfPoints)
	{
		return V2;
	}
	if (V2 < CO->ulOriginalNumberOfPoints)
	{
		return V1;
	}
#endif

    if(LOD_C_ChooseTheBestPACK(CO, &(CO->AllVertices + V1)->MinimalError, &(CO->AllVertices + V2)->MinimalError))
        return V2;
    else
        return V1;
	
    /* NOBUG */
}

/*
===================================================================================================
===================================================================================================
*/
void LOD_C_IncludeErrorInSortTable(LOD_C_ComputationalObject *CO, unsigned long VertexToCompute)
{	
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   EdgeCounter, EdgeCounter2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    EdgeCounter = VertexToCompute >> 1;
    (CO->FirstFinder[0] + EdgeCounter)->Index = LOD_C_ChooseTheBest(CO, (EdgeCounter << 1) + 0, (EdgeCounter << 1) + 1);
	
    for(EdgeCounter2 = 1; EdgeCounter2 < CO->FinderMaxStage; EdgeCounter2++)
    {
        EdgeCounter >>= 1;
        (CO->FirstFinder[EdgeCounter2] +
            EdgeCounter)->Index = LOD_C_ChooseTheBest
			(
			CO,
			(CO->FirstFinder[EdgeCounter2 - 1] + (EdgeCounter << 1) + 0)->Index,
			(CO->FirstFinder[EdgeCounter2 - 1] + (EdgeCounter << 1) + 1)->Index
			);
    }
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long LOD_C_ComputeVertexErrors(LOD_C_ComputationalObject *CO, unsigned long VertexToCompute)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LOD_C_Vertex    *LCV_Ptr;
    LOD_C_Edge      *LCE_Ptr, *LCE_Ptr2;
    MAD_Vertex      MovingVector, MovingVectorNormalized, Local;
    unsigned long   EdgeCounter, EdgeCounter2, SecondEdge;
    LOD_C_ErrorPack *Error, Maxi;
	
    float MaxRadio; 
    LCV_Ptr = CO->AllVertices + VertexToCompute;
    LCE_Ptr2 = LCV_Ptr->BeginEdge;
	
    LOD_C_SetWorstPACK(CO, &LCV_Ptr->MinimalError);
	
    /* LCV_Ptr -> Error2 = LOC_C_Infinite; LCV_Ptr -> ActualError2 = LOD_C_ComputeActualVertexErrors2(CO,VertexToCompute); */
//    LCV_Ptr->TAG = CO->ActualStage;
	if (LCV_Ptr->NumberOfEdges == 0)
	{
		LOD_C_SetLowestPACK(CO, &LCV_Ptr->MinimalError);
	}
	else
#ifdef  KEEP_ORIGINAL_WITHOUT_RADIOSITY
		if (LCV_Ptr->ulFreedomDimension == 0) /* it an original point -> Don't touch that */
		{
			LOD_C_SetUntouchablePACK(CO, &LCV_Ptr->MinimalError);
		} else
#endif
		if (LCV_Ptr->NumberOfEdges > TIME_SQUARE_EDGE_LIMIT_OPTIM) /* Optimization (avoid square time on big meshes) */
		{
			LOD_C_SetWorstPACK(CO, &LCV_Ptr->MinimalError);
		} else 
			for(EdgeCounter = 0; EdgeCounter < LCV_Ptr->NumberOfEdges; EdgeCounter++)
			{
				MovingVector = *(MAD_Vertex *) (CO->Object->OBJ_PointList + (LCE_Ptr2)->PointA);
				Local = *(MAD_Vertex *) (CO->Object->OBJ_PointList + (LCE_Ptr2)->PointB);
				
				if((LCE_Ptr2)->PointA == VertexToCompute)
				{
					SecondEdge = LCE_Ptr2->PointB;
					Error = &(LCE_Ptr2)->ErrorAB;
					SUB_VECTOR((&Local), (&MovingVector), (&MovingVector), x, y, z);
				}
				else
				{
					SecondEdge = LCE_Ptr2->PointA;
					Error = &(LCE_Ptr2)->ErrorBA;
					SUB_VECTOR((&MovingVector), (&Local), (&MovingVector), x, y, z);
				}
				
				/* Error2 = 0.0f; */
				LOD_C_SetLowestPACK(CO, Error);
				MovingVectorNormalized = MovingVector;
				NORMALIZE((&MovingVectorNormalized), x, y, z);
				
#ifdef LOD_NICE_MESH_ERROR
				LCE_Ptr = LCV_Ptr->BeginEdge;
				EdgeCounter2 = LCV_Ptr->NumberOfEdges;
				Error->NiceMeshError = (float)LCV_Ptr ->NumberOfEdges + (float)(CO->AllVertices + SecondEdge) ->NumberOfEdges;
#endif
				Maxi.NiceMeshError = Error->NiceMeshError;
				LCE_Ptr = LCV_Ptr->BeginEdge;
				EdgeCounter2 = LCV_Ptr->NumberOfEdges;
				while(EdgeCounter2--)
				{
					Maxi.TopologicError = LOD_C_ComputeKiteError(CO, &MovingVector, &MovingVectorNormalized, LCE_Ptr, VertexToCompute, SecondEdge);
#ifdef LOD_C_RADIOSITY
#ifdef LOD_C_RADIOSITY_GRAY
					Maxi.RadiosityMeshError = LOD_C_ComputeKiteErrorRadiosity(CO, LCE_Ptr, VertexToCompute, SecondEdge , 1+2+3); /* red */
#else
					Maxi.RadiosityMeshError = LOD_C_ComputeKiteErrorRadiosity(CO, LCE_Ptr, VertexToCompute, SecondEdge , 1); /* red */
					MaxRadio = LOD_C_ComputeKiteErrorRadiosity(CO, LCE_Ptr, VertexToCompute, SecondEdge , 2); /* Green */
					if (Maxi.RadiosityMeshError < MaxRadio ) Maxi.RadiosityMeshError = MaxRadio ;
					MaxRadio = LOD_C_ComputeKiteErrorRadiosity(CO, LCE_Ptr, VertexToCompute, SecondEdge , 4); /* blue */
					if (Maxi.RadiosityMeshError < MaxRadio ) Maxi.RadiosityMeshError = MaxRadio ;
#endif
#endif
					if(LCE_Ptr->PointA == VertexToCompute)
						LCE_Ptr = LCE_Ptr->NextEdgeAX;
					else
						LCE_Ptr = LCE_Ptr->NextEdgeBX;
					
					if(LOD_C_ChooseTheBestPACK(CO, &Maxi, Error)) *Error = Maxi;
				}
				
				/* Error2 = Error2 - LCV_Ptr -> ActualError2 ; */
				if(LOD_C_ChooseTheBestPACK(CO, &LCV_Ptr->MinimalError, Error))
				{
					(LCV_Ptr->MinimalError) = (*Error);
					
					if(CO->RadiosityErrorMax < Error->RadiosityMeshError)
						CO->RadiosityErrorMax = Error->RadiosityMeshError;
					LCV_Ptr->MinimalErrorIndex = EdgeCounter;
				}
				
				if(LCE_Ptr2->PointA == VertexToCompute)
					LCE_Ptr2 = LCE_Ptr2->NextEdgeAX;
				else
					LCE_Ptr2 = LCE_Ptr2->NextEdgeBX;
			}
			
			LOD_C_IncludeErrorInSortTable(CO, VertexToCompute);
			return 0;
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long LOD_C_GetBestVertex(LOD_C_ComputationalObject *CO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* TABLE METHODE */
    unsigned long   result;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    result = CO->FirstFinder[CO->FinderMaxStage - 1]->Index;
	
    /* HARD METHODE */
	
    /*
	* LOD_C_Vertex *LCV_Ptr; float ErrMin ; unsigned long ECounter ; ErrMin = LOC_C_Infinite ;
	* LCV_Ptr = CO->AllVertices ; result = 0; for(ECounter = 0; ECounter < CO->NumberOfVertices;
	* ECounter++) { result = LOD_C_ChooseTheBest(CO,result , ECounter ); LCV_Ptr ++; } if ((CO
	* -> FirstFinder [CO-> FinderMaxStage - 1 ] -> Index ) != result) result = result;
	*/
    return result;
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long LOD_C_GetAllCommonEdge(LOD_C_ComputationalObject *CO, unsigned long V1, unsigned long V2, LOD_C_Edge **EdgeTable)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter, Result;
    LOD_C_Vertex    *LCV_Ptr;
    LOD_C_Edge      *EdgePTR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Result = 0;
	
    LCV_Ptr = CO->AllVertices + V1;
    Counter = LCV_Ptr->NumberOfEdges;
    EdgePTR = LCV_Ptr->BeginEdge;
    while(Counter--)
    {
        if((EdgePTR->PointA == V2) || (EdgePTR->PointB == V2))
            EdgeTable[Result++] = EdgePTR;
		
        if(EdgePTR->PointA == V1)
            EdgePTR = EdgePTR->NextEdgeAX;
        else
            EdgePTR = EdgePTR->NextEdgeBX;
    }
	
    /* No common Edge */
    return Result;
}

/*
===================================================================================================
===================================================================================================
*/
LOD_C_Edge *LOD_C_GetCommonEdge(LOD_C_ComputationalObject *CO, unsigned long V1, unsigned long V2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter;
    LOD_C_Vertex    *LCV_Ptr;
    LOD_C_Edge      *EdgePTR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    LCV_Ptr = CO->AllVertices + V1;
    Counter = LCV_Ptr->NumberOfEdges;
    EdgePTR = LCV_Ptr->BeginEdge;
    if(EdgePTR == NULL)
    {
        while(Counter--)
        {
            EdgePTR = LCV_Ptr->FirstEdge[Counter];
            if((EdgePTR->PointA == V2) || (EdgePTR->PointB == V2)) return EdgePTR;
        }
    }
    else
    {
        while(Counter--)
        {
            if((EdgePTR->PointA == V2) || (EdgePTR->PointB == V2)) return EdgePTR;
			
            if(EdgePTR->PointA == V1)
                EdgePTR = EdgePTR->NextEdgeAX;
            else
                EdgePTR = EdgePTR->NextEdgeBX;
        }
    }
	
    /* No common Edge */
    return NULL;
}

/*
===================================================================================================
===================================================================================================
*/
void LOD_C_ClearEdgesDeleted(LOD_C_ComputationalObject *CO, unsigned long VertexToClean)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter, TriangleMAX;
    LOD_C_Vertex    *LCV_Ptr;
    LOD_C_Edge      *EdgePTR;
    LOD_C_Edge      **EdgePTRToRestore;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    LCV_Ptr = CO->AllVertices + VertexToClean;
    Counter = LCV_Ptr->NumberOfEdges;
    EdgePTR = LCV_Ptr->BeginEdge;
    EdgePTRToRestore = &LCV_Ptr->BeginEdge;
    TriangleMAX = 0;
    while(Counter--)
    {
        if(EdgePTR->DisapearStage != 0)
            if(TriangleMAX < EdgePTR->NumberOfTriangles)
                TriangleMAX = EdgePTR->NumberOfTriangles;
			
			if(EdgePTR->PointA == EdgePTR->PointB) 
				EdgePTR = EdgePTR;  /* BUG! */
			
			if
				(
				(EdgePTR->DisapearStage != 0) ||
				((CO->AllVertices + EdgePTR->PointB)->DisapearStage) ||
				((CO->AllVertices + EdgePTR->PointA)->DisapearStage) ||
				((CO->AllVertices + EdgePTR->NumberOfTriangles) == 0)
				)
			{
				if(EdgePTR->PointA == VertexToClean)
				{
					*EdgePTRToRestore = EdgePTR->NextEdgeAX;
					LCV_Ptr->NumberOfEdges--;
					if(((CO->AllVertices + EdgePTR->PointB)->DisapearStage) && (!EdgePTR->DisapearStage)) 
						EdgePTR = EdgePTR;  /* BUG! */
					EdgePTR = EdgePTR->NextEdgeAX;
				}
				else if(EdgePTR->PointB == VertexToClean)
				{
					*EdgePTRToRestore = EdgePTR->NextEdgeBX;
					LCV_Ptr->NumberOfEdges--;
					if(((CO->AllVertices + EdgePTR->PointA)->DisapearStage) && (!EdgePTR->DisapearStage)) 
						EdgePTR = EdgePTR;  /* BUG! */
					EdgePTR = EdgePTR->NextEdgeBX;
				}
				else
					EdgePTR = EdgePTR;      /* BUG! */
			}
			else
			{
				if(EdgePTR->PointA == VertexToClean)
				{
					EdgePTRToRestore = &EdgePTR->NextEdgeAX;
					if((CO->AllVertices + EdgePTR->PointB)->DisapearStage) 
						EdgePTR = EdgePTR;  /* BUG! */
					EdgePTR = EdgePTR->NextEdgeAX;
				}
				else
				{
					if((CO->AllVertices + EdgePTR->PointA)->DisapearStage) 
						EdgePTR = EdgePTR;  /* BUG! */
					EdgePTRToRestore = &EdgePTR->NextEdgeBX;
					EdgePTR = EdgePTR->NextEdgeBX;
				}
			}
    }
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long LOD_C_CollapseEdge(LOD_C_ComputationalObject *CO, unsigned long VertexToMove, unsigned long VertexToReceive, unsigned long ActualStage)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter, Counter2, Result, EdgesDEBUG, CollapseEdgeCounter, CollapseEdgeNumber;
    LOD_C_Vertex    *LCV_PtrTM;
    LOD_C_Vertex    *LCV_PtrTR;
	
    unsigned long   NumberOfLostEdges;
    unsigned long   EA,EB;
    LOD_C_Edge      *LostEdges;
    LOD_C_Edge      *LE_To_Delete[100L];
    LOD_C_Edge      *LE_To_Modify[100L];
    LOD_C_Edge      *EdgePTR, *NextEdge;
	
    LOD_C_Triangle  *PLAQUETEST_Absorber;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    LCV_PtrTM = CO->AllVertices + VertexToMove;
    LCV_PtrTR = CO->AllVertices + VertexToReceive;
	
    CollapseEdgeNumber = LOD_C_GetAllCommonEdge(CO, VertexToMove, VertexToReceive, CO->AllVertexEdge1);
    if(CollapseEdgeNumber > 1)
        CollapseEdgeNumber = CollapseEdgeNumber;
    if(CollapseEdgeNumber == 0)
        CollapseEdgeNumber = CollapseEdgeNumber;
	
	EA = LCV_PtrTM->NumberOfEdges;
	EB = LCV_PtrTR ->NumberOfEdges;
	
    /* Destroy lost point */
    LCV_PtrTM->DisapearStage = ActualStage;
    LOD_C_IncludeErrorInSortTable(CO, VertexToMove);
	
    /* Destroy lost Triangle */
    CollapseEdgeCounter = CollapseEdgeNumber;
    NumberOfLostEdges = 0;
    while(CollapseEdgeCounter--)
    {
        LostEdges = CO->AllVertexEdge1[CollapseEdgeCounter];
        Counter = LostEdges->NumberOfTriangles;
        LostEdges->DisapearStage = ActualStage;
        while(Counter--)
        {
            LostEdges->FirstEdgeToTriangle[Counter]->DisapearStage = ActualStage;
			
            /* Find modifyed edges */
            if(LostEdges->FirstEdgeToTriangle[Counter]->Edges[0] == LostEdges)
            {
                if  ((LostEdges->FirstEdgeToTriangle[Counter]->Edges[1]->PointA == VertexToMove) ||
					(LostEdges->FirstEdgeToTriangle[Counter]->Edges[1]->PointB == VertexToMove))
                {
                    LE_To_Delete[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[1];
                    LE_To_Modify[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[2];
                }
                else
                {
                    LE_To_Delete[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[2];
                    LE_To_Modify[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[1];
                }
            }
            else if(LostEdges->FirstEdgeToTriangle[Counter]->Edges[1] == LostEdges)
            {
                if
					(
                    (LostEdges->FirstEdgeToTriangle[Counter]->Edges[0]->PointA == VertexToMove) ||
                    (LostEdges->FirstEdgeToTriangle[Counter]->Edges[0]->PointB == VertexToMove)
					)
                {
                    LE_To_Delete[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[0];
                    LE_To_Modify[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[2];
                }
                else
                {
                    LE_To_Delete[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[2];
                    LE_To_Modify[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[0];
                }
            }
            else if(LostEdges->FirstEdgeToTriangle[Counter]->Edges[2] == LostEdges)
            {
                if
					(
                    (LostEdges->FirstEdgeToTriangle[Counter]->Edges[1]->PointA == VertexToMove) ||
                    (LostEdges->FirstEdgeToTriangle[Counter]->Edges[1]->PointB == VertexToMove)
					)
                {
                    LE_To_Delete[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[1];
                    LE_To_Modify[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[0];
                }
                else
                {
                    LE_To_Delete[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[0];
                    LE_To_Modify[NumberOfLostEdges] = LostEdges->FirstEdgeToTriangle[Counter]->Edges[1];
                }
            }
            else
                NumberOfLostEdges = NumberOfLostEdges;  /* BUG! */
/*            if((LE_To_Delete[NumberOfLostEdges]->TAG == 0) || (LE_To_Modify[NumberOfLostEdges]->TAG == 0))
            {
                /* LE_To_Delete[NumberOfLostEdges] -> TAG = 1; LE_To_Modify[NumberOfLostEdges] -> TAG = 1; */
                NumberOfLostEdges++;
				if (NumberOfLostEdges > 100L)
					NumberOfLostEdges = NumberOfLostEdges;
				
  //          }
        }
    }
	
    EdgesDEBUG = LCV_PtrTM->NumberOfEdges - NumberOfLostEdges - 1 + LCV_PtrTR->NumberOfEdges - 1;
	
    /* Mark all deleted Edges */
    Counter = NumberOfLostEdges;
    while(Counter--)
    {
        LE_To_Delete[Counter]->DisapearStage = ActualStage;
    }
	
    /* Modify Edge */
    Counter = NumberOfLostEdges;
    while(Counter--)
    {
        /* BEGIN PLAQUETEST */
        PLAQUETEST_Absorber = NULL;
        for(Counter2 = 0; Counter2 < LE_To_Delete[Counter]->NumberOfTriangles; Counter2++)
        {
            if(LE_To_Delete[Counter]->FirstEdgeToTriangle[Counter2]->DisapearStage != ActualStage)
                PLAQUETEST_Absorber = LE_To_Delete[Counter]->FirstEdgeToTriangle[Counter2];
        }
		/*
		for(Counter2 = 0; Counter2 < LE_To_Delete[Counter]->NumberOfTriangles; Counter2++) 
		{
		if(LE_To_Delete[Counter]->FirstEdgeToTriangle[Counter2]->DisapearStage == ActualStage)
		LE_To_Delete[Counter]->FirstEdgeToTriangle[Counter2]->AbsorberTriangle = PLAQUETEST_Absorber; 
		}
		*/         
		
        /* END PLAQUETEST */
		
        /* First clear deleted triangles in ER */
        Result = 0;
        for(Counter2 = 0; Counter2 < LE_To_Modify[Counter]->NumberOfTriangles; Counter2++)
        {
            if(LE_To_Modify[Counter]->FirstEdgeToTriangle[Counter2]->DisapearStage != ActualStage)
            {
                LE_To_Modify[Counter]->FirstEdgeToTriangle[Result++] = LE_To_Modify[Counter]->FirstEdgeToTriangle[Counter2];
            }
            else
                PLAQUETEST_Absorber = LE_To_Modify[Counter]->FirstEdgeToTriangle[Counter2];
        }
		
        /* Clear deleted triangles in EM */
		
        /* And Collapse the 2 edges; */
        for(Counter2 = 0; Counter2 < LE_To_Delete[Counter]->NumberOfTriangles; Counter2++)
        {
            if(LE_To_Delete[Counter]->FirstEdgeToTriangle[Counter2]->DisapearStage != ActualStage)
            {
                LE_To_Modify[Counter]->FirstEdgeToTriangle[Result] = LE_To_Delete[Counter]->FirstEdgeToTriangle[Counter2];
                if(LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Edges[0] == LE_To_Delete[Counter])
                    LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Edges[0] = LE_To_Modify[Counter];
                else if(LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Edges[1] == LE_To_Delete[Counter])
                    LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Edges[1] = LE_To_Modify[Counter];
                else if(LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Edges[2] == LE_To_Delete[Counter])
                    LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Edges[2] = LE_To_Modify[Counter];
                else
                    Counter2 = Counter2;    /* BUG! */
                if(LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Triangle.Index[0] == VertexToMove)
                    LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Triangle.Index[0] = VertexToReceive;
                else if(LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Triangle.Index[1] == VertexToMove)
                    LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Triangle.Index[1] = VertexToReceive;
                else if(LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Triangle.Index[2] == VertexToMove)
                    LE_To_Modify[Counter]->FirstEdgeToTriangle[Result]->Triangle.Index[2] = VertexToReceive;
                Result++;
            }
        }
		
        LE_To_Modify[Counter]->NumberOfTriangles = Result;
        if(LE_To_Modify[Counter]->NumberOfTriangles >= 5)
            LE_To_Modify[Counter] = LE_To_Modify[Counter];  /* BUG! */
    }
	
    /* Add isolated Edges around TM to TR */
    Counter = LCV_PtrTM->NumberOfEdges;
    EdgePTR = LCV_PtrTM->BeginEdge;
    while(Counter--)
    {
        if(EdgePTR->PointA == VertexToMove)
            NextEdge = EdgePTR->NextEdgeAX;
        else if(EdgePTR->PointB == VertexToMove)
            NextEdge = EdgePTR->NextEdgeBX;
        else
            NextEdge = NextEdge;    /* BUG! */
        if((NextEdge == LCV_PtrTM->BeginEdge) && (Counter)) 
			NextEdge = NextEdge;    /* BUG! */
        if(EdgePTR->PointA == EdgePTR->PointB) 
			EdgePTR = EdgePTR;  /* BUG! */
		
        if(EdgePTR->DisapearStage != ActualStage)
        {
            Counter2 = EdgePTR->NumberOfTriangles;
			
            /* Change false triangle index */
            while(Counter2--)
            {
                if(EdgePTR->FirstEdgeToTriangle[Counter2]->Triangle.Index[0] == VertexToMove)
                    EdgePTR->FirstEdgeToTriangle[Counter2]->Triangle.Index[0] = VertexToReceive;
                else if(EdgePTR->FirstEdgeToTriangle[Counter2]->Triangle.Index[1] == VertexToMove)
                    EdgePTR->FirstEdgeToTriangle[Counter2]->Triangle.Index[1] = VertexToReceive;
                else if(EdgePTR->FirstEdgeToTriangle[Counter2]->Triangle.Index[2] == VertexToMove)
                    EdgePTR->FirstEdgeToTriangle[Counter2]->Triangle.Index[2] = VertexToReceive;
				
                LOD_C_ComputeTriangleCrossProduct(CO, EdgePTR->FirstEdgeToTriangle[Counter2]);
            }
			
            if(EdgePTR->PointA == VertexToMove)
            {
                EdgePTR->NextEdgeAX = LCV_PtrTR->BeginEdge;
                EdgePTR->PointA = VertexToReceive;
				
                /* LOD_C_ComputeVerticeColor_DEBUG(CO,EdgePTR -> PointB, 0xffffffff); */
            }
            else if(EdgePTR->PointB == VertexToMove)
            {
                EdgePTR->NextEdgeBX = LCV_PtrTR->BeginEdge;
                EdgePTR->PointB = VertexToReceive;
				
                /* LOD_C_ComputeVerticeColor_DEBUG(CO,EdgePTR -> PointA, 0xffffffff); */
            }
            else
                EdgePTR = EdgePTR;                      /* BUG! */
			
            LCV_PtrTR->BeginEdge = EdgePTR;
            LCV_PtrTR->NumberOfEdges++;
        }
		
        EdgePTR = NextEdge;
    }
	
    /* Recompute Edges; */
    LOD_C_ClearEdgesDeleted(CO, VertexToReceive);
    Counter = LCV_PtrTR->NumberOfEdges;
    EdgePTR = LCV_PtrTR->BeginEdge;
    while(Counter--)
    {
        if(EdgePTR->PointA == VertexToReceive)
        {
            LOD_C_ClearEdgesDeleted(CO, EdgePTR->PointB);
            EdgePTR = EdgePTR->NextEdgeAX;
        }
        else
        {
            LOD_C_ClearEdgesDeleted(CO, EdgePTR->PointA);
            EdgePTR = EdgePTR->NextEdgeBX;
        }
    }
	
    /* Recompute Errors; */
    LOD_C_ComputeVertexErrors(CO, VertexToReceive);
    Counter = LCV_PtrTR->NumberOfEdges;
    EdgePTR = LCV_PtrTR->BeginEdge;
    while(Counter--)
    {
        if(EdgePTR->PointA == VertexToReceive)
        {
            LOD_C_ComputeVertexErrors(CO, EdgePTR->PointB);
            EdgePTR = EdgePTR->NextEdgeAX;
        }
        else
        {
            LOD_C_ComputeVertexErrors(CO, EdgePTR->PointA);
            EdgePTR = EdgePTR->NextEdgeBX;
        }
    }
	if (EA + EB - 4 != LCV_PtrTR->NumberOfEdges)
		EA = EA;
	
    return 0;
}


/*
===================================================================================================
===================================================================================================
*/
float LOD_C_GetVertexError(LOD_C_ComputationalObject *CO, unsigned long VertexToCollapse)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   EdgeCounter;
    float           Err;
    LOD_C_Edge      *EdgePTR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    EdgePTR = (CO->AllVertices + VertexToCollapse)->BeginEdge;
    for(EdgeCounter = 0; EdgeCounter < (CO->AllVertices + VertexToCollapse)->MinimalErrorIndex; EdgeCounter++)
    {
        if(EdgePTR->PointA == VertexToCollapse)
            EdgePTR = EdgePTR->NextEdgeAX;
        else
            EdgePTR = EdgePTR->NextEdgeBX;
    }
	
    if(EdgePTR->PointA == VertexToCollapse)
    {
        Err = EdgePTR->ErrorAB.TopologicError;
    }
    else
    {
        Err = EdgePTR->ErrorBA.TopologicError;
    }
	
    return Err;
}

/*
===================================================================================================
===================================================================================================
*/
void LOD_C_CollapseVertex(LOD_C_ComputationalObject *CO, unsigned long VertexToCollapse, unsigned long ActualStage)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   EdgeCounter;
    LOD_C_Edge      *EdgePTR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EdgePTR = (CO->AllVertices + VertexToCollapse)->BeginEdge;
	
	if ((CO->AllVertices + VertexToCollapse)->NumberOfEdges == 0)
	{
		(CO->AllVertices + VertexToCollapse)->DisapearStage = ActualStage;
		LOD_C_IncludeErrorInSortTable(CO, VertexToCollapse);
		CO->CurrentTrace->VertexToDelete = VertexToCollapse;
		CO->CurrentTrace->VertexToCollapse = 0xffffffff;
	}
	else
	{
	if (EdgePTR -> PointA == EdgePTR -> PointB)
		EdgePTR = EdgePTR;
		for(EdgeCounter = 0; EdgeCounter < (CO->AllVertices + VertexToCollapse)->MinimalErrorIndex; EdgeCounter++)
		{
			if(EdgePTR->PointA == VertexToCollapse)
				EdgePTR = EdgePTR->NextEdgeAX;
			else
				EdgePTR = EdgePTR->NextEdgeBX;
		}
		
		if ( EdgePTR->PointA == VertexToCollapse)
		{
			LOD_C_CollapseEdge(CO, VertexToCollapse, EdgePTR->PointB, ActualStage);
			CO->CurrentTrace->VertexToDelete = VertexToCollapse;
			CO->CurrentTrace->VertexToCollapse = EdgePTR->PointB;
		}
		else
		{
			LOD_C_CollapseEdge(CO, VertexToCollapse, EdgePTR->PointA, ActualStage);
			CO->CurrentTrace->VertexToDelete = VertexToCollapse;
			CO->CurrentTrace->VertexToCollapse = EdgePTR->PointA;
		}
	}
    CO->CurrentTrace++;
}

//#pragma optimize("", on)
/* Set number of point with MRM algorithm */
//#pragma optimize("", off)


unsigned long CheckOject(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Face        *CurrentFace, *CurrentFace2;
    unsigned long   Counter, Counter2;
    unsigned long   NumberOfFace;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    for(Counter = 0; Counter < MO->NumberOfElements; Counter++)
    {
        CurrentFace = CurrentFace2 = MO->Elements[Counter]->Faces;
        NumberOfFace = 0;
        for(Counter2 = 0; Counter2 < MO->Elements[Counter]->NumberOfTriangles; Counter2++)
        {
            if (CurrentFace->Index[0] >= MO->NumberOfPoints)
            {
				CurrentFace->Index[0] = MO->NumberOfPoints - 1;
            }
            if (CurrentFace->Index[1] >= MO->NumberOfPoints)
            {
				CurrentFace->Index[1] = MO->NumberOfPoints - 1;
            }
            if (CurrentFace->Index[2] >= MO->NumberOfPoints)
            {
				CurrentFace->Index[2] = MO->NumberOfPoints - 1;
            }
            CurrentFace++;
        }
    }
	return 0;
}
/*
===================================================================================================
===================================================================================================
*/

void OBJ_ComputeElementLODForDisplay(MAD_GeometricObject *ObjectToDraw, MAD_GeometricObjectElement *ElementToCompute, unsigned long NewNumberOfPoints , unsigned long *UVREMAP)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   FaceCounter;
    MAD_Face        *Faces1, *Faces2 , Local;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Faces1 = ElementToCompute->Faces;
    Faces2 = ElementToCompute->FacesIIndexedOnIndex2;
    FaceCounter = ElementToCompute->TotalNumberOfTriangles;
    ElementToCompute->NumberOfTriangles = 0;
    while(FaceCounter--)
    {
        Local = *Faces2;
        Local.Index[0] = ObjectToDraw->Index2[Faces2->Index[0]];
        Local.Index[1] = ObjectToDraw->Index2[Faces2->Index[1]];
        Local.Index[2] = ObjectToDraw->Index2[Faces2->Index[2]];
        if((Local.Index[0] != Local.Index[1]) && (Local.Index[0] != Local.Index[2]) && (Local.Index[1] != Local.Index[2]))
        {
            ElementToCompute -> NumberOfTriangles++;
			*Faces1 = Local;
			Faces1++;
		}
        Faces2++;
    }
	/* REcompute UV from main indexes */
	if (UVREMAP)
	{
		for (FaceCounter  = 0 ; FaceCounter < ElementToCompute -> NumberOfUsedIndex ; FaceCounter++)
			UVREMAP [ElementToCompute -> UsedIndex[FaceCounter]] = FaceCounter + ElementToCompute -> OneUVPerVertexPerElementBase;
		
		Faces1 = ElementToCompute->Faces;
		FaceCounter = ElementToCompute->NumberOfTriangles ;
		while(FaceCounter--)
		{
			Faces1->UVIndex[0] = UVREMAP[Faces1->Index[0]];
			Faces1->UVIndex[1] = UVREMAP[Faces1->Index[1]];
			Faces1->UVIndex[2] = UVREMAP[Faces1->Index[2]];
			Faces1++;
		}
	}
}
/*
===================================================================================================
===================================================================================================
*/
void OBJ_ComputeObjectLODForDisplay_Number(MAD_GeometricObject *ObjectToDraw, unsigned long NewNumberOfpoints)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   ECounter, CCounter, MCounter, ActualPosition;
    unsigned long   *UVREMAP;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef KEEP_ORIGINAL_WITHOUT_RADIOSITY
	if (ObjectToDraw->NumberOfPointsWithoutRadiosity > NewNumberOfpoints)
		NewNumberOfpoints = ObjectToDraw->NumberOfPointsWithoutRadiosity;
#endif
	
    ActualPosition = ObjectToDraw->NumberOfPoints;
    MCounter = 0;
    ObjectToDraw->NumberOfPoints = NewNumberOfpoints;
	if (ObjectToDraw->NumberOfPoints == 0) ObjectToDraw->NumberOfPoints = 1;
	if (ObjectToDraw->OBJ_UVtextureList)
	{
		UVREMAP = (unsigned long *)malloc (4L * ObjectToDraw->RealNumberOfPoints);
		memset(UVREMAP , 0 , 4L * ObjectToDraw->RealNumberOfPoints);
	}
	else
		UVREMAP =NULL;
	
    ECounter = 0;
	for (ECounter = 0 ; ECounter < ObjectToDraw->RealNumberOfPoints ; ECounter ++)
    {
        CCounter = ECounter;
		while(CCounter >= ObjectToDraw->NumberOfPoints) 
			CCounter = ObjectToDraw->Absorbers[CCounter];
        ObjectToDraw->Index2[ECounter] = CCounter;
    }
	
    ECounter = ObjectToDraw->NumberOfElements;
    while(ECounter--)
    {
        OBJ_ComputeElementLODForDisplay(ObjectToDraw, *(ObjectToDraw->Elements + ECounter), ObjectToDraw->NumberOfPoints ,UVREMAP);
    }
	CheckOject(ObjectToDraw);
	free(UVREMAP);
}

/*
===================================================================================================
===================================================================================================
*/
/*void OBJ_ComputeObjectLODForDisplay(MAD_GeometricObject *ObjectToDraw, float Quality)
{
    if(Quality > 1.0f) Quality = 1.0f;
    if(Quality < 0.0f) Quality = 0.0f;
	OBJ_ComputeObjectLODForDisplay_Number(ObjectToDraw, (unsigned long) (Quality * (float) ObjectToDraw->RealNumberOfPoints));
}*/


/*
===================================================================================================
Destroy MRM and keep Simple Mesh
===================================================================================================
*/
void LOD_C_FreezeLOD(MAD_GeometricObject *ObjectToDraw)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long               Counter;
    MAD_GeometricObjectElement  *Current;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_RestoreOneIndexPerUVPerElementSolution(ObjectToDraw);
    if(ObjectToDraw->Absorbers)
        free(ObjectToDraw->Absorbers);
    if(ObjectToDraw->Index2)
        free(ObjectToDraw->Index2);
	ObjectToDraw->Absorbers = NULL;
	ObjectToDraw->Index2 = NULL;
    ObjectToDraw->RealNumberOfPoints = ObjectToDraw->NumberOfPoints;
	
    for(Counter = 0; Counter < ObjectToDraw->NumberOfElements; Counter++)
    {
        Current = ObjectToDraw->Elements[Counter];
        if(Current->FacesIIndexedOnIndex2)
            free(Current->FacesIIndexedOnIndex2);
		Current->FacesIIndexedOnIndex2 = NULL;
        Current->TotalNumberOfTriangles = Current->NumberOfTriangles;
    }
}

/*
===================================================================================================
Destroy MRM and keep Simple Mesh
===================================================================================================
*/
unsigned long LOD_C_IsObjectLOD(MAD_GeometricObject *ObjectToDraw)
{
    if ((ObjectToDraw->Absorbers) && (ObjectToDraw->Index2))
		return 1;
	else return 0;
}

//#pragma optimize("", on)

/*
===================================================================================================
Compute connectivity of a mesh; Computing time is linear !!!
===================================================================================================
*/
OBJ_ERR LOD_C_ComputeObjectLOD(MAD_GeometricObject * ObjectToDraw, unsigned long OriginalNumberOfPoints ,unsigned long(*SeprogressPos) (float F01))
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LOD_C_ComputationalObject   CO;
	
    /* GEOMETRIC_OBJECT_ELEMENT *ElementToDraw; */
    unsigned long               ECounter, BCounter, CCounter;
    LOD_C_Triangle              *LCT_Ptr;
    LOD_C_Vertex                *LCV_Ptr;
    LOD_C_Edge                  *LCE_Ptr;
    LOD_C_MiniFinder            *LocalFinder;
    float                       ErrMax, ErrMin, Threserr;
    float                       GROW;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /*
	* If (ObjectToDraw->NumberOfPoints > 500) for(ECounter = 0; ECounter <
	* ObjectToDraw->NumberOfPoints; ECounter++) { (ObjectToDraw -> OBJ_PointList + ECounter)->
	* Point . z = 0.0f; }
	*/
	memset(&CO, 0 , sizeof(CO));
	if(SeprogressPos(0.0f)) return 0;
    /* First compute LOD_C_ComputationalObject */
	//	ObjectToDraw -> OBJ_UVtextureList = NULL;
	MTH_fn_vInit();
    LOD_C_FreezeLOD(ObjectToDraw);
	//	MAD_RemoveIllgalFaces(ObjectToDraw);
	//	MAD_RemoveIsoPoints(ObjectToDraw);
	MAD_RemoveIllgalFaces(ObjectToDraw);
	if (ObjectToDraw -> OBJ_UVtextureList )
	{
		CO.ComputeUV = 1;
		MAD_RestoreOneIndexPerUVPerElementSolution(ObjectToDraw);
//		MAD_ComputeOneIndexPerUVPerElementSolution(ObjectToDraw);
	}
	else
		CO.ComputeUV = 0;

    CO.NumberOftriangles = 0;
    CO.NumberOfVertices = ObjectToDraw->RealNumberOfPoints;
	CO.ulOriginalNumberOfPoints = OriginalNumberOfPoints;
    ECounter = ObjectToDraw->NumberOfElements;
    CO.Object = ObjectToDraw;
	
    /* Compute Number Of triangles; */
    while(ECounter--)
    {
        CO.NumberOftriangles += (*(ObjectToDraw->Elements + ECounter))->NumberOfTriangles;
    }
	
    /* Malloc The new triangle list */
    LOD_MAD_MALLOC(CO.AllTriangles, CO.NumberOftriangles * sizeof(LOD_C_Triangle));
    LOD_MAD_MALLOC(CO.AllVertices, (CO.NumberOfVertices + 2) * sizeof(LOD_C_Vertex));
    LOD_MAD_MALLOC(CO.AllTrace, CO.NumberOfVertices * sizeof(LOD_C_TRACE));
    CO.CurrentTrace = CO.AllTrace;
	
    /* Build the new triangle list */
    ECounter = ObjectToDraw->NumberOfElements;
    LCT_Ptr = CO.AllTriangles;
    for(ECounter = 0; ECounter < ObjectToDraw->NumberOfElements; ECounter++)
    {
        for(BCounter = 0; BCounter < (*(ObjectToDraw->Elements + ECounter))->NumberOfTriangles; BCounter++)
        {
            LCT_Ptr->OwnerElement = ECounter;
            LCT_Ptr++->Triangle = *((*(ObjectToDraw->Elements + ECounter))->Faces + BCounter);
        }
    }
	
    /* Build the new vertex list */
    LCV_Ptr = CO.AllVertices;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        LCV_Ptr->NumberOftriangles_VT = 0;
        LCV_Ptr->FirstTriangle_VT = 0;
        LCV_Ptr++;
    }
	
    /* OK Compute number of vertex -> triangles; Part 1 */
    LCV_Ptr = CO.AllVertices;
    LCT_Ptr = CO.AllTriangles;
    for(ECounter = 0; ECounter < CO.NumberOftriangles; ECounter++)
    {
        (LCV_Ptr + LCT_Ptr->Triangle.Index[0])->NumberOftriangles_VT++;
        (LCV_Ptr + LCT_Ptr->Triangle.Index[1])->NumberOftriangles_VT++;
        (LCV_Ptr + LCT_Ptr->Triangle.Index[2])->NumberOftriangles_VT++;
        LCT_Ptr++;
    }
	
    /* OK Part 2 */
    LCV_Ptr = CO.AllVertices;
    BCounter = 0;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        /* For next compute on EDGES */
        LCV_Ptr->DisapearStage = 0;
        LCV_Ptr->NumberOfEdges = 0;
        BCounter += LCV_Ptr->NumberOftriangles_VT;
        LCV_Ptr++;
    }
	
    LOD_MAD_MALLOC(CO.AllVertexTriangles1, BCounter * sizeof(LOD_C_Triangle *));
	
    /* OK Part 3 */
    LCV_Ptr = CO.AllVertices;
    CCounter = 0;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        LCV_Ptr->FirstTriangle_VT = CO.AllVertexTriangles1 + CCounter;
        CCounter += LCV_Ptr->NumberOftriangles_VT;
        LCV_Ptr->NumberOftriangles_VT = 0;
        LCV_Ptr++;
    }
	
    /* OK Part 4 */
    LCV_Ptr = CO.AllVertices;
    LCT_Ptr = CO.AllTriangles;
    for(ECounter = 0; ECounter < CO.NumberOftriangles; ECounter++)
    {
        (LCV_Ptr + LCT_Ptr->Triangle.Index[0])->FirstTriangle_VT[(LCV_Ptr + LCT_Ptr->Triangle.Index[0])->NumberOftriangles_VT++] = LCT_Ptr;
        (LCV_Ptr + LCT_Ptr->Triangle.Index[1])->FirstTriangle_VT[(LCV_Ptr + LCT_Ptr->Triangle.Index[1])->NumberOftriangles_VT++] = LCT_Ptr;
        (LCV_Ptr + LCT_Ptr->Triangle.Index[2])->FirstTriangle_VT[(LCV_Ptr + LCT_Ptr->Triangle.Index[2])->NumberOftriangles_VT++] = LCT_Ptr;
        LCT_Ptr->Edges[0] = NULL;
        LCT_Ptr->Edges[1] = NULL;
        LCT_Ptr->Edges[2] = NULL;
        LCT_Ptr++;
    }
	
    /* OK Compute number of EDGE; Part 1; */
    LCV_Ptr = CO.AllVertices;
    CO.NumberOfEdges = 0;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        LCV_Ptr->DisapearStage = 1;
        for(BCounter = 0; BCounter < LCV_Ptr->NumberOftriangles_VT; BCounter++)
        {
            if(LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0] == ECounter)
            {
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage == 0) CO.NumberOfEdges++;
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage == 0) CO.NumberOfEdges++;
                if(((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage & 2) == 0)
                    LCV_Ptr->NumberOfEdges++;
                if(((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage & 2) == 0)
                    LCV_Ptr->NumberOfEdges++;
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage |= 2;
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage |= 2;
            }
            else if(LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1] == ECounter)
            {
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage == 0) CO.NumberOfEdges++;
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage == 0) CO.NumberOfEdges++;
                if(((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage & 2) == 0)
                    LCV_Ptr->NumberOfEdges++;
                if(((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage & 2) == 0)
                    LCV_Ptr->NumberOfEdges++;
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage |= 2;
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage |= 2;
            }
            else if(LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2] == ECounter)
            {
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage == 0) CO.NumberOfEdges++;
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage == 0) CO.NumberOfEdges++;
                if(((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage & 2) == 0)
                    LCV_Ptr->NumberOfEdges++;
                if(((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage & 2) == 0)
                    LCV_Ptr->NumberOfEdges++;
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage |= 2;
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage |= 2;
            }
        }
		
        for(BCounter = 0; BCounter < LCV_Ptr->NumberOftriangles_VT; BCounter++)
        {
            (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage &= 1;
            (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage &= 1;
            (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage &= 1;
        }
		
        LCV_Ptr++;
    }
	
    /* OK Here CO . NumberOfEdges is computed */
    LOD_MAD_MALLOC(CO.AllEdges, CO.NumberOfEdges * sizeof(LOD_C_Edge));
    LOD_MAD_MALLOC(CO.AllVertexEdge1, CO.NumberOfEdges * 2L * sizeof(LOD_C_Edge *));
	
    /* OK Reinit */
    LCV_Ptr = CO.AllVertices;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        /* For next compute on EDGES */
        LCV_Ptr->DisapearStage = 0;
        LCV_Ptr++;
    }
	
    /* Now compute Edges; */
    LCV_Ptr = CO.AllVertices;
    LCE_Ptr = CO.AllEdges;
	
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        LCV_Ptr->DisapearStage = 1;
        for(BCounter = 0; BCounter < LCV_Ptr->NumberOftriangles_VT; BCounter++)
        {
            if(LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0] == ECounter)
            {
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage == 0)
                {
                    LCE_Ptr->PointA = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0];
                    LCE_Ptr->PointB = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1];
                    LCE_Ptr++;
                }
				
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage == 0)
                {
                    LCE_Ptr->PointA = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0];
                    LCE_Ptr->PointB = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2];
                    LCE_Ptr++;
                }
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage |= 2;
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage |= 2;
            }
            else if(LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1] == ECounter)
            {
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage == 0)
                {
                    LCE_Ptr->PointA = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1];
                    LCE_Ptr->PointB = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0];
                    LCE_Ptr++;
                }
				
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage == 0)
                {
                    LCE_Ptr->PointA = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1];
                    LCE_Ptr->PointB = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2];
                    LCE_Ptr++;
                }
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage |= 2;
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage |= 2;
            }
            else if(LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2] == ECounter)
            {
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage == 0)
                {
                    LCE_Ptr->PointA = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2];
                    LCE_Ptr->PointB = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0];
                    LCE_Ptr++;
                }
				
                if((CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage == 0)
                {
                    LCE_Ptr->PointA = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2];
                    LCE_Ptr->PointB = LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1];
                    LCE_Ptr++;
                }
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage |= 2;
                (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage |= 2;
            }
        }
		
        for(BCounter = 0; BCounter < LCV_Ptr->NumberOftriangles_VT; BCounter++)
        {
            (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[0])->DisapearStage &= 1;
            (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[1])->DisapearStage &= 1;
            (CO.AllVertices + LCV_Ptr->FirstTriangle_VT[BCounter]->Triangle.Index[2])->DisapearStage &= 1;
        }
		
        LCV_Ptr++;
    }
	
    /* Here we don' t need animore the _VT info */
	
    /* OK PART 2 VERTEX -> EDGES */
    LCV_Ptr = CO.AllVertices;
    CCounter = 0;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        LCV_Ptr->FirstEdge = CO.AllVertexEdge1 + CCounter;
        CCounter += LCV_Ptr->NumberOfEdges;
        LCV_Ptr->NumberOfEdges = 0;
        LCV_Ptr->FirstTriangle_VT = NULL;
        LCV_Ptr++;
    }
	
    LCV_Ptr = CO.AllVertices;
    LCE_Ptr = CO.AllEdges;
    for(ECounter = 0; ECounter < CO.NumberOfEdges; ECounter++)
    {
        (LCV_Ptr + LCE_Ptr->PointA)->FirstEdge[(LCV_Ptr + LCE_Ptr->PointA)->NumberOfEdges++] = LCE_Ptr;
        (LCV_Ptr + LCE_Ptr->PointB)->FirstEdge[(LCV_Ptr + LCE_Ptr->PointB)->NumberOfEdges++] = LCE_Ptr;
        LCE_Ptr->NumberOfTriangles = 0;
        LCE_Ptr++;
    }
	
    /* OK TRIANGLES -> EDGES */
    LCT_Ptr = CO.AllTriangles;
    for(ECounter = 0; ECounter < CO.NumberOftriangles; ECounter++)
    {
        LCT_Ptr->Edges[0] = LOD_C_GetCommonEdge(&CO, LCT_Ptr->Triangle.Index[0], LCT_Ptr->Triangle.Index[1]);
        LCT_Ptr->Edges[1] = LOD_C_GetCommonEdge(&CO, LCT_Ptr->Triangle.Index[1], LCT_Ptr->Triangle.Index[2]);
        LCT_Ptr->Edges[2] = LOD_C_GetCommonEdge(&CO, LCT_Ptr->Triangle.Index[2], LCT_Ptr->Triangle.Index[0]);
        LCT_Ptr->Edges[0]->NumberOfTriangles++;
        LCT_Ptr->Edges[1]->NumberOfTriangles++;
        LCT_Ptr->Edges[2]->NumberOfTriangles++;
        LCT_Ptr++;
    }
	
    /* PART 2 */
    CCounter = 0;
    LCE_Ptr = CO.AllEdges;
    for(ECounter = 0; ECounter < CO.NumberOfEdges; ECounter++)
    {
        CCounter += LCE_Ptr->NumberOfTriangles;
        LCE_Ptr->NumberOfTriangles = 0;
        LCE_Ptr++;
    }
	
    /* PART 3 */
    LCT_Ptr = CO.AllTriangles;
    for(ECounter = 0; ECounter < CO.NumberOftriangles; ECounter++)
    {
        LCT_Ptr->Edges[0]->FirstEdgeToTriangle[LCT_Ptr->Edges[0]->NumberOfTriangles++] = LCT_Ptr;
        LCT_Ptr->Edges[1]->FirstEdgeToTriangle[LCT_Ptr->Edges[1]->NumberOfTriangles++] = LCT_Ptr;
        LCT_Ptr->Edges[2]->FirstEdgeToTriangle[LCT_Ptr->Edges[2]->NumberOfTriangles++] = LCT_Ptr;
		
        /* LCT_Ptr->OriginalTriangle = LCT_Ptr->Triangle; */
        LCT_Ptr++;
    }
	
    /* Set: LOD_C_Edge *NextEdgeAX; LOD_C_Edge *NextEdgeBX; */
    LCV_Ptr = CO.AllVertices;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        if(LCV_Ptr->NumberOfEdges)
        {
            LCV_Ptr->BeginEdge = LCV_Ptr->FirstEdge[0];
            CCounter = LCV_Ptr->NumberOfEdges;
            for(CCounter = 0; CCounter < LCV_Ptr->NumberOfEdges - 1; CCounter++)
            {
                if(LCV_Ptr->FirstEdge[CCounter]->PointA == ECounter)
                    LCV_Ptr->FirstEdge[CCounter]->NextEdgeAX = LCV_Ptr->FirstEdge[CCounter + 1];
                else
                    LCV_Ptr->FirstEdge[CCounter]->NextEdgeBX = LCV_Ptr->FirstEdge[CCounter + 1];
            }
			
            if(LCV_Ptr->FirstEdge[CCounter]->PointA == ECounter)
                LCV_Ptr->FirstEdge[CCounter]->NextEdgeAX = LCV_Ptr->FirstEdge[0];
            else
                LCV_Ptr->FirstEdge[CCounter]->NextEdgeBX = LCV_Ptr->FirstEdge[0];
        }
		
        LCV_Ptr++;
    }
	
    /* FINISH !! RESTORE VALUES */
    LCT_Ptr = CO.AllTriangles;
    for(ECounter = 0; ECounter < CO.NumberOftriangles; ECounter++)
    {
        LCT_Ptr->DisapearStage = 0;
        LOD_C_ComputeTriangleCrossProduct(&CO, LCT_Ptr);
        LCT_Ptr->OriginalCrossProduct = LCT_Ptr->CrossProduct;
#ifdef KEEP_ORIGINAL_WITHOUT_RADIOSITY
		if (LCT_Ptr->Triangle.MAXflags & 1)
			LCT_Ptr->Edges[0] -> bIsVisible = 0xffffffff;
		if (LCT_Ptr->Triangle.MAXflags & 2)
			LCT_Ptr->Edges[1] -> bIsVisible = 0xffffffff;
		if (LCT_Ptr->Triangle.MAXflags & 4)
			LCT_Ptr->Edges[2] -> bIsVisible = 0xffffffff;
#endif
        LCT_Ptr++;
    }
	
    LCV_Ptr = CO.AllVertices;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        LCV_Ptr->DisapearStage = 0;
		LOD_C_SetUntouchablePACK(&CO, &LCV_Ptr->MinimalError);
        LCV_Ptr++;
    }
    ECounter = 2;
    while(ECounter--)
    {
        LCV_Ptr->DisapearStage = 1;
        LOD_C_SetUntouchablePACK(&CO, &LCV_Ptr->MinimalError);
		
        /* LCV_Ptr->Error2 = LOC_C_Infinite; */
        LCV_Ptr++;
    }
	
	
    LCE_Ptr = CO.AllEdges;
    for(ECounter = 0; ECounter < CO.NumberOfEdges; ECounter++)
    {
        LCE_Ptr->DisapearStage = 0;
#ifdef KEEP_ORIGINAL_WITHOUT_RADIOSITY
		if (LCE_Ptr->bIsVisible)
		{
			CO.AllVertices[LCE_Ptr->PointA].ulFreedomDimension ++;
			CO.AllVertices[LCE_Ptr->PointB].ulFreedomDimension ++;
		}
#endif
        LCE_Ptr++;
    }
#ifdef KEEP_ORIGINAL_WITHOUT_RADIOSITY
    LCV_Ptr = CO.AllVertices;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
		LCV_Ptr->OriginalEdgeNum = 0xffffffff;
		if (ECounter < OriginalNumberOfPoints)
			LCV_Ptr->ulFreedomDimension = 0; /* this is an original point */
		else
		{
			if (LCV_Ptr->ulFreedomDimension == 2)
			{
				LCV_Ptr->ulFreedomDimension = 1; /* it's an edge point */
				LCV_Ptr->OriginalEdgeNum = ECounter + 1; /* + to avoid 0 */
			}
			else
				LCV_Ptr->ulFreedomDimension = 2; /* it's a triangle point */
		}
        LCV_Ptr++;
    }
	CCounter = 1;
	while (CCounter)
	{
		CCounter = 0;
		LCE_Ptr = CO.AllEdges;
		for(ECounter = 0; ECounter < CO.NumberOfEdges; ECounter++)
		{
			if (LCE_Ptr->bIsVisible) /* == OriginalEdge */
			{
				if (CO.AllVertices[LCE_Ptr->PointA].ulFreedomDimension == 1)
				{
					if (LCE_Ptr->bIsVisible < CO.AllVertices[LCE_Ptr->PointA].OriginalEdgeNum)
					{
						CO.AllVertices[LCE_Ptr->PointA].OriginalEdgeNum = LCE_Ptr->bIsVisible ;
						CCounter = 1;
					} else
						if (LCE_Ptr->bIsVisible > CO.AllVertices[LCE_Ptr->PointA].OriginalEdgeNum)
						{
							LCE_Ptr->bIsVisible = CO.AllVertices[LCE_Ptr->PointA].OriginalEdgeNum;
							CCounter = 1;
						}
				}
				if (CO.AllVertices[LCE_Ptr->PointB].ulFreedomDimension == 1)
				{
					if (LCE_Ptr->bIsVisible < CO.AllVertices[LCE_Ptr->PointB].OriginalEdgeNum)
					{
						CO.AllVertices[LCE_Ptr->PointB].OriginalEdgeNum = LCE_Ptr->bIsVisible ;
						CCounter = 1;
					} else
						if (LCE_Ptr->bIsVisible > CO.AllVertices[LCE_Ptr->PointB].OriginalEdgeNum)
						{
							LCE_Ptr->bIsVisible = CO.AllVertices[LCE_Ptr->PointB].OriginalEdgeNum;
							CCounter = 1;
						}
				}
			}
			LCE_Ptr++;
		}
	}
#endif
	
    CCounter = 0;
    ECounter = CO.NumberOfVertices;
    while(ECounter)
    {
        ECounter >>= 1;
        CCounter++;
    }
	
    LOD_MAD_MALLOC(CO.AllFinders, (1 << CCounter) * sizeof(LOD_C_MiniFinder));
    CO.FinderMaxStage = 0;
    ECounter = 1 << CCounter;
    ECounter >>= 1;
    LocalFinder = CO.AllFinders;
    while(ECounter)
    {
        CO.FirstFinder[CO.FinderMaxStage++] = LocalFinder;
        LocalFinder += ECounter;
        ECounter >>= 1;
    };
	
    ECounter = 1 << CCounter;
    while(ECounter--)
    {
        CO.AllFinders[ECounter].Index = MAD_OVERFLOW_INDEX;
    };
	
    CO.RadiosityErrorMax = 0.0f;
    CO.ErrMax = -LOC_C_Infinite;
    ErrMax = -LOC_C_Infinite;
    ErrMin = LOC_C_Infinite;
    LCV_Ptr = CO.AllVertices;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        LOD_C_ComputeVertexErrors(&CO, ECounter);
		
        /* If (LCV_Ptr->MinimalError > ErrMax) ErrMax = LCV_Ptr->MinimalError; if (LCV_Ptr->MinimalError < ErrMin) ErrMin = LCV_Ptr->MinimalError ; */
        LCV_Ptr++;
    }
	
    LCV_Ptr = CO.AllVertices;
    Threserr = 1.0f / (ErrMax - ErrMin);
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
//        CO.AllVertices[ECounter].TAG = 0;
        LCE_Ptr++;
    }
	
    /* End Display Error */
    ECounter = CO.NumberOfVertices;
	
    /* ECounter = 16384L; ECounter = 15000L; */
	
    /* ECounter = 4096L - 32 /*+ 17 */
    CCounter = 1;
    CO.ActualStage = 0;
    ECounter = CO.NumberOfVertices;
	
    /* MESH SINPLIFICATION */
    ErrMax = (CO.AllVertices + LOD_C_GetBestVertex(&CO))->MinimalError.RadiosityMeshError;
    GROW = 0.0f;
    while(ECounter--)
    {
        CCounter = LOD_C_GetBestVertex(&CO);
        CO.ActualStage++;
		
        /* CCounter = 20L * 20L + 10L; */
        if(SeprogressPos((float) (CO.NumberOfVertices - ECounter) / (float) CO.NumberOfVertices)) goto STOP;
		
        /* If ((ErrMin / ErrMax) > 20.0f) ECounter = 0; */
		
        /* If (((float) ECounter / (float) CO.NumberOfVertices < 0.015f) /*&& (ErrMin == 0.0f) */
		
        /* If (ErrMax > 0.999f) ECounter = 0; */
		
        /* If((CO.AllVertices + CCounter)->MinimalError.TopologicError != 0.0f) ECounter = 0; */
#ifndef LOD_REORDER
		float   Local12;
		Local12 = (float) ObjectToDraw->NumberOfPointsWithoutRadiosity / (float) ObjectToDraw->RealNumberOfPoints;
		Local12 += (1.0f - Local12) * 0.010f;
		if (ECounter < (Local12 * ObjectToDraw->RealNumberOfPoints)) ECounter = 0;
#endif
		
		LOD_C_CollapseVertex(&CO, CCounter, 1);
    }
	
	
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        if(!(CO.AllVertices + ECounter)->DisapearStage)
            LOD_C_ClearEdgesDeleted(&CO, ECounter);
    }
	
#ifdef MAD_SHOW_ERRORS
    LCV_Ptr = CO.AllVertices;
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
		if (LCV_Ptr->MinimalError.TopologicError == LOC_C_Infinite)
		{
			*(CO.Object->Colors + ECounter ) = 0x0000ff;
		} else
		if (LCV_Ptr->MinimalError.TopologicError == LOD_C_FUSION_LOCKED_EROOR)
		{
			*(CO.Object->Colors + ECounter ) = 0x00ff00;
		} else
		if (LCV_Ptr->MinimalError.TopologicError == 0.0f)
		{
			*(CO.Object->Colors + ECounter ) = 0x00ffff;
		} else
		{
			long K;
			K = (long)(LCV_Ptr->MinimalError.TopologicError * MAD_SHOW_ERROR_COEF);
			if (K > 255) K = 255;
			if (K < 0) K = 0;
			*(CO.Object->Colors + ECounter ) = K | (K<<8) | (K<<16);
		} 
        LCV_Ptr++;
    }
#endif
    /*
	* LCT_Ptr = CO.AllTriangles; CCounter = 0; for(ECounter = 0; ECounter <
	* CO.NumberOftriangles; ECounter++) { if (LCT_Ptr -> AbsorberTriangle == NULL) LCT_Ptr ->
	* PLAQUE_NUMBER = CCounter++; LCT_Ptr++; }
	*/
	
    /*
	* LCT_Ptr2 = CO.AllTriangles; for(ECounter = 0; ECounter < CO.NumberOftriangles; ECounter++)
	* { LCT_Ptr = LCT_Ptr2; while (LCT_Ptr -> AbsorberTriangle != NULL) LCT_Ptr = LCT_Ptr ->
	* AbsorberTriangle ; (CO . AllVertices + LCT_Ptr2 -> OriginalTriangle . Index[0]) ->
	* MinimalError = (float)LCT_Ptr -> PLAQUE_NUMBER; (CO . AllVertices + LCT_Ptr2 ->
	* OriginalTriangle . Index[1]) -> MinimalError = (float)LCT_Ptr -> PLAQUE_NUMBER; (CO .
	* AllVertices + LCT_Ptr2 -> OriginalTriangle . Index[2]) -> MinimalError = (float)LCT_Ptr ->
	* PLAQUE_NUMBER; LCT_Ptr2++; }
	*/
	
    /* END PLQUE TEST */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* LOD_C_TRACE *CurrentTraceLocal; */
    MAD_Simple3DVertex  *OBJ_PointList;
    MAD_ColorARGB		*Colors;
    MAD_ColorARGB		*OriginalColors;
    MAD_Vertex          *SavedUV;
    MAD_Vertex          *SavedUVDecomp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    LOD_MAD_MALLOC(OBJ_PointList, ObjectToDraw->RealNumberOfPoints * sizeof(MAD_Simple3DVertex));
	memcpy(OBJ_PointList, ObjectToDraw->OBJ_PointList ,  ObjectToDraw->RealNumberOfPoints * sizeof(MAD_Simple3DVertex));
    LOD_MAD_MALLOC(Colors, ObjectToDraw->RealNumberOfPoints * sizeof(MAD_ColorARGB));
	LOD_MAD_MALLOC(OriginalColors, ObjectToDraw->RealNumberOfPoints * sizeof(MAD_ColorARGB));
	memcpy(Colors, ObjectToDraw->Colors ,  ObjectToDraw->RealNumberOfPoints * sizeof(MAD_ColorARGB));
	memcpy(OriginalColors, ObjectToDraw->SecondRLIField,  ObjectToDraw->RealNumberOfPoints * sizeof(MAD_ColorARGB));
	
	if (CO.ComputeUV)
	{
		LOD_MAD_MALLOC(SavedUV, ObjectToDraw->NumberOfUV * sizeof(MAD_Vertex));
		LOD_MAD_MALLOC(SavedUVDecomp, ObjectToDraw->RealNumberOfPoints * sizeof(MAD_Vertex));
		memcpy(SavedUV , ObjectToDraw->OBJ_UVtextureList ,  ObjectToDraw->NumberOfUV * sizeof(MAD_Vertex));
	}
	
    LOD_MAD_MALLOC(ObjectToDraw->Absorbers, sizeof(unsigned long) * ObjectToDraw->RealNumberOfPoints);
    LOD_MAD_MALLOC(ObjectToDraw->Index2, sizeof(unsigned long) * ObjectToDraw->RealNumberOfPoints);

	/* reorder Vertex */
	unsigned long MostImportantPoint;
	MostImportantPoint = (CO.CurrentTrace - 1) -> VertexToDelete;
    for(ECounter = 0; ECounter < ObjectToDraw->RealNumberOfPoints; ECounter++)
    {
        CO.CurrentTrace--;
		if (CO.CurrentTrace->VertexToCollapse == 0xffffffff)
			CO.CurrentTrace->VertexToCollapse = MostImportantPoint;
        ObjectToDraw->OBJ_PointList[ECounter] = OBJ_PointList[CO.CurrentTrace->VertexToDelete];
        ObjectToDraw->Colors[ECounter] = Colors[CO.CurrentTrace->VertexToDelete];
        ObjectToDraw->SecondRLIField[ECounter] = OriginalColors[CO.CurrentTrace->VertexToDelete];
        ObjectToDraw->Absorbers[ECounter] = CO.CurrentTrace->VertexToCollapse;
		CO.AllTrace[CO.CurrentTrace->VertexToDelete].Reorder = ECounter;
        ObjectToDraw->Index2[ECounter] = ECounter;
    }


    for(ECounter = 0; ECounter < ObjectToDraw->RealNumberOfPoints; ECounter++)
    {
        ObjectToDraw->Absorbers[ECounter] = CO.AllTrace[ObjectToDraw->Absorbers[ECounter]].Reorder;
        CO.CurrentTrace++;
    }
	ObjectToDraw->Absorbers[0] = 0;


    free((void *) OBJ_PointList);
    free((void *) OriginalColors);
    free((void *) Colors);

	/* reorder indexes */
    for(ECounter = 0; ECounter < ObjectToDraw->NumberOfElements; ECounter++)
    {
        LOD_MAD_MALLOC
			(
            (*(ObjectToDraw->Elements + ECounter))->FacesIIndexedOnIndex2,
            sizeof(MAD_Face) * (*(ObjectToDraw->Elements + ECounter))->NumberOfTriangles
			);
        (*(ObjectToDraw->Elements + ECounter))->TotalNumberOfTriangles = (*(ObjectToDraw->Elements + ECounter))->NumberOfTriangles;
        for(BCounter = 0; BCounter < (*(ObjectToDraw->Elements + ECounter))->NumberOfTriangles; BCounter++)
        {
            ObjectToDraw->Elements [ ECounter ]->FacesIIndexedOnIndex2 [ BCounter ] = ObjectToDraw->Elements [ECounter]->Faces [ BCounter ];
            ObjectToDraw->Elements [ ECounter ]->FacesIIndexedOnIndex2 [ BCounter ] .Index[0] = CO.AllTrace[ObjectToDraw->Elements [ ECounter ]->Faces [ BCounter ] .Index[0]].Reorder;
            ObjectToDraw->Elements [ ECounter ]->FacesIIndexedOnIndex2 [ BCounter ] .Index[1] = CO.AllTrace[ObjectToDraw->Elements [ ECounter ]->Faces [ BCounter ] .Index[1]].Reorder;
            ObjectToDraw->Elements [ ECounter ]->FacesIIndexedOnIndex2 [ BCounter ] .Index[2] = CO.AllTrace[ObjectToDraw->Elements [ ECounter ]->Faces [ BCounter ] .Index[2]].Reorder;
        }
		
		if (CO.ComputeUV)
		{
			for (BCounter = 0; BCounter < ObjectToDraw->Elements[ECounter] -> NumberOfUsedIndex;BCounter++)
			{
				/* decompress UV in SavedUV */
				SavedUVDecomp[CO.AllTrace[ ObjectToDraw->Elements[ECounter] -> UsedIndex[BCounter]].Reorder] = SavedUV[BCounter + ObjectToDraw->Elements[ECounter] -> OneUVPerVertexPerElementBase];
				/* reorder indexes */
				ObjectToDraw->Elements[ECounter] -> UsedIndex[BCounter] = CO.AllTrace[ ObjectToDraw->Elements[ECounter] -> UsedIndex[BCounter]].Reorder;
			}
			/* reorder UV Coordinates */
			for (BCounter = 0; BCounter < ObjectToDraw->Elements[ECounter] -> NumberOfUsedIndex;BCounter++)
			{
				ObjectToDraw-> OBJ_UVtextureList [BCounter + ObjectToDraw->Elements[ECounter] -> OneUVPerVertexPerElementBase] = SavedUVDecomp[ObjectToDraw->Elements[ECounter] -> UsedIndex[BCounter]];
			}
		}
    }
	if (CO.ComputeUV)
	{
		free(SavedUVDecomp);
		free(SavedUV);
	}

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   Local12;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Local12 = (float) ObjectToDraw->NumberOfPointsWithoutRadiosity / (float) ObjectToDraw->RealNumberOfPoints;
    Local12 += (1.0f - Local12) * 0.005f;
	OBJ_ComputeObjectLODForDisplay_Number(ObjectToDraw, (unsigned long)(Local12 * ObjectToDraw->RealNumberOfPoints));

    /* RE - ORDER TRIANGLES */
STOP:
    free((void *) CO.AllFinders);
    free((void *) CO.AllVertexEdge1);
    free((void *) CO.AllEdges);
    free((void *) CO.AllVertexTriangles1);
    free((void *) CO.AllTriangles);
    free((void *) CO.AllTrace);
    free((void *) CO.AllVertices);
    return 0;
}

#pragma optimize("", on)
