/*$T MAD_LOD.c GC!1.32 07/24/19 09:25:00 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"

#ifdef ACTIVE_EDITORS

#define LOD_C_MAX_EDGE_TRIANGLE			4									/* Could be greater ...*/
#define LOC_C_Infinite					10.0e37f							/* Local define of infifnite */
#define LOD_C_FUSION_LOCKED_EROOR		LOC_C_Infinite * 0.5f				/* Low priority fusion lock */
#define TIME_SQUARE_EDGE_LIMIT_OPTIM	(LONG)10							/* Optimization : Avoid big stars */
#define LOD_TOPOLOGIC_PRAGMA			0.001f								/* threshold for using radiosity */
#define MAD_OVERFLOW_INDEX				0x0fffffff							/* Don't touch */
#define MAD_COLINEAR_EDGE_THRESHOLD		0.9999f								/* Used for no face-flipping */
#define LOD_C_RADIOSITY_GRAY												/* optimization : Compute vertex color error in GRAY */
#define LOD_FPSPP						0.05f								/* Don't touch */
#define LOD_C_RADIOSITY														/* Compute Vertex color Error */
#define LOD_RADIOSITY_NO_PEAKS												/* Avoid peaks in vertexcolor simplification */
#define LOD_STOP_LEVEL					9									/* Minimum number of points */
#define MTH_SIMULATELONG(a)				*(LONG *)&(a)						/* */
#define MTH_ABSOLUTE(a)					MTH_SIMULATELONG(a) &= 0x7fffffff;	/* */
#define MTH_IS_NEGATIVE(a)				(MTH_SIMULATELONG(a) < 0)			/* */

#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include "MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "MAD_mem/Sources/MAD_mem.h"
#include "SCD_MTH.c"

#define LOD_MAD_MALLOC(address, size) \
{ \
	*(void **) &address = malloc(size); \
	L_memset(*(void **) &address, 0, size); \
}

#define LOD_MAD_FREE(address) \
{ \
	free(address);\
}


void BUG()
{
	float a;
	a = 0.0f;
}


typedef struct  MAD_Simple3DVertexLOCAL_
{
    float           x, y, z;
    ULONG   diffuse;
    ULONG   nothing1;
    ULONG   nothing2;
} MAD_Simple3DVertexLOCAL;

/*
===================================================================================================
===================================================================================================
*/
LONG LOD_ERRPERCENT(float a, float b, float PRAGM)
{
     if(a == b) return 1;
	MTH_ABSOLUTE(a);
	MTH_ABSOLUTE(b);
	if ((a < PRAGM) && (b < PRAGM)) return 1;//*/
	return 0;
}


typedef struct LOD_C_Edge_          LOD_C_Edge;
typedef struct LOD_C_Triangle_      LOD_C_Triangle;
typedef struct LOD_C_Vertex_        LOD_C_Vertex;
typedef struct LOD_C_DynamicList_   LOD_C_DynamicList;

typedef struct                      MAD_TripledIndex_
{
    ULONG   Index[3];
} MAD_TripledIndex;

typedef struct  LOD_C_Triangle_
{
    ULONG   DisapearStage;
    MAD_Vertex      CrossProduct;
    MAD_Vertex      OriginalCrossProduct;
    MAD_Face        Triangle;       /* OK */
    ULONG   OwnerElement;			/* OK */
    LOD_C_Edge      *Edges[3];      /* OK */
} LOD_C_Triangle;

typedef struct  LOD_C_ErrorPack_
{
    float   TopologicError;
    float   RadiosityMeshError;
} LOD_C_ErrorPack;

typedef struct  LOD_C_Edge_
{
    ULONG   DisapearStage;
    ULONG   PointA;													/* OK */
    ULONG   PointB;													/* OK */
    LOD_C_ErrorPack ErrorAB;
    LOD_C_ErrorPack ErrorBA;
    LOD_C_Edge      *NextEdgeAX;
    LOD_C_Edge      *NextEdgeBX;
    ULONG   NumberOfTriangles;										/* OK */
    LOD_C_Triangle  *FirstEdgeToTriangle[LOD_C_MAX_EDGE_TRIANGLE];  /* OK */
} LOD_C_Edge;

typedef struct  LOD_C_TRACE_
{
    ULONG   VertexToDelete;
    ULONG   VertexToCollapse;
    ULONG   Reorder;
} LOD_C_TRACE;

typedef struct  LOD_C_Vertex_
{
    ULONG   DisapearStage;
    LOD_C_ErrorPack MinimalError;
    ULONG   MinimalErrorIndex;
    ULONG   NumberOfEdges;											/* OK */
    LOD_C_Edge      *BeginEdge;
    LOD_C_Edge      **FirstEdge;                                    /* OK */
    ULONG   NumberOftriangles_VT;									/* OK */
    LOD_C_Triangle  **FirstTriangle_VT;                             /* OK */
} LOD_C_Vertex;

typedef struct  LOD_C_MiniFinder_
{
    ULONG   Index;
} LOD_C_MiniFinder;

typedef struct  LOD_C_ComputationalObject_
{
    MAD_GeometricObject *Object;                                    /* OK */
    ULONG       NumberOfVertices;									/* OK */
	ULONG       OriginalNumBerOfPts;
    LOD_C_Vertex        *AllVertices;                               /* OK */
    LOD_C_TRACE         *AllTrace;
    LOD_C_TRACE         *CurrentTrace;
    LOD_C_Triangle      **AllVertexTriangles1;                      /* OK */
    LOD_C_Edge          **AllVertexEdge1;                           /* OK */
    ULONG       NumberOfEdges;										/* OK */
    LOD_C_Edge          *AllEdges;                                  /* OK */
    ULONG       NumberOftriangles;									/* OK */
    LOD_C_Triangle      *AllTriangles;                              /* OK */
    ULONG       ActualStage;
    float               ErrMax, ErrMin, ErrorThreshold;
    float               RadiosityErrorMax;
    ULONG       FinderMaxStage;
    LOD_C_MiniFinder    *FirstFinder[32];
    LOD_C_MiniFinder    *AllFinders;
	ULONG		ComputeUV;
} LOD_C_ComputationalObject;

/*
===================================================================================================
Unsigned LONG LOD_C_MallocEdges(LOD_C_ComputationalObject *CO , )
===================================================================================================
*/
LOD_C_Edge  *LOD_C_GetCommonEdge(LOD_C_ComputationalObject *CO, ULONG V1, ULONG V2);

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


/*
===================================================================================================
Compute the error generated by moving one vertex in one direction; This look like a kite
===================================================================================================
*/
float LOD_C_ComputeKiteError
(
 LOD_C_ComputationalObject   *CO,
 MAD_Vertex                  *MovingVector,
 MAD_Vertex                  *MovingVectorNORMALIZED,
 LOD_C_Edge                  *LCE_Ptr,
 ULONG               VertexToCompute,
 ULONG               VertexToReceive
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Vertex              Edge, Local;
    MAD_Simple3DVertexLOCAL SaveVertexToMove;
    MAD_Vertex              SAVECP0, SAVECP1;
    float                   Surface, Coef , EdgeLenght/* , Coef2 */;
    ULONG           TriangleCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if(LCE_Ptr->NumberOfTriangles == 0) return 0.0f;
    Edge = *(MAD_Vertex *) (CO->Object->OBJ_PointList + (LCE_Ptr)->PointA);
    Local = *(MAD_Vertex *) (CO->Object->OBJ_PointList + (LCE_Ptr)->PointB);
    SUB_VECTOR((&Edge), (&Local), (&Edge), x, y, z);

    
    NORME((&Edge), EdgeLenght, x, y, z);
    /*  */
    CROSS_PRODUCT(MovingVector, (&Edge), (&Local), x, y, z);
    NORME((&Local), Surface, x, y, z);
    Surface = LOD_MTH_fn_fSquareRootOpt(Surface);

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
    Coef = DOT_PRODUCT((&Edge), MovingVector , x, y, z);
    MTH_ABSOLUTE(Coef);
    return EdgeLenght * Coef / 2;
}

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
	if (MIN != 0.0f) N01 = MAX * LOD_MTH_fn_fInverseOpt(MIN);
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
 ULONG               VertexToCompute,
 ULONG               VertexToReceive,
 ULONG				 FlagRGBA
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG           NumberOfCatchedIndex, Counter ;
    ULONG           CounterSI, ChangedIndex;
    MAD_Vertex              GDDIR, DIFF;
    MAD_Simple3DVertexLOCAL V0, V1, V2, NT, VR;
    float                   Error, G0, G1, G2, T1, T2, T1P, T2P, DELTA, SCALARDEST, NTRI, NTRI1 ;
#ifdef LOD_RADIOSITY_NO_PEAKS
	float		Peak_end;
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (!CO->Object->Colors) return 0.0f;
	
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
            G1 *= LOD_MTH_fn_fSquareRootOpt(NTRI) + LOD_MTH_fn_fSquareRootOpt(NTRI1);
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
            G0 *= LOD_MTH_fn_fSquareRootOpt(NTRI) + LOD_MTH_fn_fSquareRootOpt(NTRI1);
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
}
/*
===================================================================================================
===================================================================================================
*/
void LOD_C_SetWorstPACK(LOD_C_ComputationalObject *CO, LOD_C_ErrorPack *E)
{
    E->RadiosityMeshError = E->TopologicError = LOD_C_FUSION_LOCKED_EROOR;
}

/*
===================================================================================================
===================================================================================================
*/
void LOD_C_SetLowestPACK(LOD_C_ComputationalObject *CO, LOD_C_ErrorPack *E)
{
    E->RadiosityMeshError = E->TopologicError = 0;
}

/*
===================================================================================================
===================================================================================================
*/
ULONG LOD_C_ChooseTheBestPACK(LOD_C_ComputationalObject *CO, LOD_C_ErrorPack *E1, LOD_C_ErrorPack *E2)
{
	if (_isnan(E1->TopologicError) || 
		_isnan(E2->TopologicError) ||
		_isnan(E1->RadiosityMeshError) ||
		_isnan(E2->RadiosityMeshError))
		BUG();
	
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
ULONG LOD_C_ChooseTheBest(LOD_C_ComputationalObject *CO, ULONG V1, ULONG V2)
{
	if (V2 == MAD_OVERFLOW_INDEX)
        return V1;

	if (V1 == MAD_OVERFLOW_INDEX)
        return V2;

    if((CO->AllVertices + V2)->DisapearStage)
        return V1;
	
    if((CO->AllVertices + V1)->DisapearStage)
        return V2;
	

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
void LOD_C_IncludeErrorInSortTable(LOD_C_ComputationalObject *CO, ULONG VertexToCompute)
{	
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   EdgeCounter, EdgeCounter2;
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
ULONG LOD_C_ComputeVertexErrors(LOD_C_ComputationalObject *CO, ULONG VertexToCompute)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LOD_C_Vertex    *LCV_Ptr;
    LOD_C_Edge      *LCE_Ptr, *LCE_Ptr2;
    MAD_Vertex      MovingVector, MovingVectorNormalized, Local;
    ULONG   EdgeCounter, EdgeCounter2, SecondEdge;
    LOD_C_ErrorPack *Error, Maxi;
	
    /* Float ; */
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
ULONG LOD_C_GetBestVertex(LOD_C_ComputationalObject *CO)
{
    return CO->FirstFinder[CO->FinderMaxStage - 1]->Index;
}

/*
===================================================================================================
===================================================================================================
*/
ULONG LOD_C_GetAllCommonEdge(LOD_C_ComputationalObject *CO, ULONG V1, ULONG V2, LOD_C_Edge **EdgeTable)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   Counter, Result;
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
	
    return Result;
}

/*
===================================================================================================
===================================================================================================
*/
LOD_C_Edge *LOD_C_GetCommonEdge(LOD_C_ComputationalObject *CO, ULONG V1, ULONG V2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   Counter;
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
void LOD_C_ClearEdgesDeleted(LOD_C_ComputationalObject *CO, ULONG VertexToClean)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   Counter, TriangleMAX;
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
				BUG();
			
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
						BUG();
					EdgePTR = EdgePTR->NextEdgeAX;
				}
				else if(EdgePTR->PointB == VertexToClean)
				{
					*EdgePTRToRestore = EdgePTR->NextEdgeBX;
					LCV_Ptr->NumberOfEdges--;
					if(((CO->AllVertices + EdgePTR->PointA)->DisapearStage) && (!EdgePTR->DisapearStage)) 
						BUG();
					EdgePTR = EdgePTR->NextEdgeBX;
				}
				else
					BUG();
			}
			else
			{
				if(EdgePTR->PointA == VertexToClean)
				{
					EdgePTRToRestore = &EdgePTR->NextEdgeAX;
					if((CO->AllVertices + EdgePTR->PointB)->DisapearStage) 
						BUG();
					EdgePTR = EdgePTR->NextEdgeAX;
				}
				else
				{
					if((CO->AllVertices + EdgePTR->PointA)->DisapearStage) 
						BUG();
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
ULONG LOD_C_CollapseEdge(LOD_C_ComputationalObject *CO, ULONG VertexToMove, ULONG VertexToReceive, ULONG ActualStage)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   Counter, Counter2, Result, EdgesDEBUG, CollapseEdgeCounter, CollapseEdgeNumber;
    LOD_C_Vertex    *LCV_PtrTM;
    LOD_C_Vertex    *LCV_PtrTR;
	
    ULONG   NumberOfLostEdges;
    ULONG   EA,EB;
    LOD_C_Edge      *LostEdges;
    LOD_C_Edge      *LE_To_Delete[100];
    LOD_C_Edge      *LE_To_Modify[100];
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
                BUG();  

			NumberOfLostEdges++;
			if (NumberOfLostEdges > (LONG)100)
				BUG();

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
                    BUG();
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
            BUG();
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
            BUG();
        if((NextEdge == LCV_PtrTM->BeginEdge) && (Counter)) 
			BUG();
        if(EdgePTR->PointA == EdgePTR->PointB) 
			BUG();
		
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
            }
            else if(EdgePTR->PointB == VertexToMove)
            {
                EdgePTR->NextEdgeBX = LCV_PtrTR->BeginEdge;
                EdgePTR->PointB = VertexToReceive;
            }
            else
                BUG();
			
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
void LOD_C_CollapseVertex(LOD_C_ComputationalObject *CO, ULONG VertexToCollapse, ULONG ActualStage)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   EdgeCounter;
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


/*
===================================================================================================
Compute connectivity of a mesh; Computing time is linear !!!
===================================================================================================
*/
ULONG LOD_C_ComputeObjectLOD(MAD_GeometricObject * ObjectToDraw, ULONG(*SeprogressPos) (float F01,char *),unsigned short **pp_ReorderBuffer, float **pdf_Errors)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LOD_C_ComputationalObject   CO;
    ULONG               ECounter, BCounter, CCounter;
    LOD_C_Triangle              *LCT_Ptr;
    LOD_C_Vertex                *LCV_Ptr;
    LOD_C_Edge                  *LCE_Ptr;
    LOD_C_MiniFinder            *LocalFinder;
    float                       ErrMax, ErrMin;
    float                       GROW;
    MAD_Simple3DVertex  *OBJ_PointList;
    MAD_ColorARGB		*Colors;
    MAD_Vertex          *SavedUV;
    MAD_Vertex          *SavedUVDecomp;
	ULONG MostImportantPoint;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	L_memset(&CO, 0 , sizeof(CO));
	if(SeprogressPos(0.0f * LOD_FPSPP,"Compute object connectivity...")) return 0;
    /* First compute LOD_C_ComputationalObject */

	LOD_MTH_fn_vInit();
//	MAD_RemoveIllgalFaces(ObjectToDraw);

	SeprogressPos(0.1f * LOD_FPSPP,NULL);

	if (ObjectToDraw -> OBJ_UVtextureList )
	{
		CO.ComputeUV = 1;
	}
	else
		CO.ComputeUV = 0;

    CO.NumberOftriangles = 0;
    CO.NumberOfVertices = ObjectToDraw->RealNumberOfPoints;
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
	
	SeprogressPos(0.2f * LOD_FPSPP,NULL);
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

	SeprogressPos(0.3f * LOD_FPSPP,NULL);
	
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
    LOD_MAD_MALLOC(CO.AllVertexEdge1, CO.NumberOfEdges * (LONG)2 * sizeof(LOD_C_Edge *));

	SeprogressPos(0.4f * LOD_FPSPP,NULL);
	
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
        LCE_Ptr++;
    }
	
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

		if (!(ECounter & 0xff))
			if (SeprogressPos(0.5f * LOD_FPSPP + 0.5f * LOD_FPSPP * (float)ECounter / (float)CO.NumberOfVertices,"First compute of errors...")) goto STOP;
        /* If (LCV_Ptr->MinimalError > ErrMax) ErrMax = LCV_Ptr->MinimalError; if (LCV_Ptr->MinimalError < ErrMin) ErrMin = LCV_Ptr->MinimalError ; */
        LCV_Ptr++;
    }
	
    /* End Display Error */
    ECounter = CO.NumberOfVertices;
	
    /* ECounter = 16384L; ECounter = 15000L; */
	
    /* ECounter = 4096L - 32 /*+ 17 */
    CCounter = 1;
    CO.ActualStage = 0;
    ECounter = CO.NumberOfVertices;
    (*pdf_Errors) = (float*) MEM_p_Alloc(sizeof(float) * CO.NumberOfVertices);

    /* MESH SINPLIFICATION */
    ErrMax = (CO.AllVertices + LOD_C_GetBestVertex(&CO))->MinimalError.RadiosityMeshError;
    GROW = 0.0f;
    while(ECounter--)
    {
        CCounter = LOD_C_GetBestVertex(&CO);
        (*pdf_Errors)[ECounter] = (CO.AllVertices + LOD_C_GetBestVertex(&CO))->MinimalError.TopologicError;
        CO.ActualStage++;
		if (!(ECounter & 0x07))
		{
        if(SeprogressPos(LOD_FPSPP +  (1.0f - LOD_FPSPP) * (float) (CO.NumberOfVertices - ECounter) / (float) CO.NumberOfVertices, "Compute MRM")) goto STOP;
		}
		
		LOD_C_CollapseVertex(&CO, CCounter, 1);
    }
	
	
    for(ECounter = 0; ECounter < CO.NumberOfVertices; ECounter++)
    {
        if(!(CO.AllVertices + ECounter)->DisapearStage)
            LOD_C_ClearEdgesDeleted(&CO, ECounter);
    }
	
	
    LOD_MAD_MALLOC(OBJ_PointList, ObjectToDraw->RealNumberOfPoints * sizeof(MAD_Simple3DVertex));
	L_memcpy(OBJ_PointList, ObjectToDraw->OBJ_PointList ,  ObjectToDraw->RealNumberOfPoints * sizeof(MAD_Simple3DVertex));
	Colors = NULL;
	if (ObjectToDraw->Colors)
	{
		LOD_MAD_MALLOC(Colors, ObjectToDraw->RealNumberOfPoints * sizeof(MAD_ColorARGB));
		L_memcpy(Colors, ObjectToDraw->Colors ,  ObjectToDraw->RealNumberOfPoints * sizeof(MAD_ColorARGB));
	}
	
	if (CO.ComputeUV)
	{
		LOD_MAD_MALLOC(SavedUV, ObjectToDraw->NumberOfUV * sizeof(MAD_Vertex));
		LOD_MAD_MALLOC(SavedUVDecomp, ObjectToDraw->RealNumberOfPoints * sizeof(MAD_Vertex));
		L_memcpy(SavedUV , ObjectToDraw->OBJ_UVtextureList ,  ObjectToDraw->NumberOfUV * sizeof(MAD_Vertex));
	}
	
	MAD_MALLOC(ULONG, ObjectToDraw->Absorbers	, ObjectToDraw->RealNumberOfPoints);
	MAD_MALLOC(ULONG, ObjectToDraw->Index2		, ObjectToDraw->RealNumberOfPoints);

	/* reorder Vertex */
	MostImportantPoint = (CO.CurrentTrace - 1) -> VertexToDelete;
    for(ECounter = 0; ECounter < ObjectToDraw->RealNumberOfPoints; ECounter++)
    {
        CO.CurrentTrace--;
		if (CO.CurrentTrace->VertexToCollapse == 0xffffffff)
			CO.CurrentTrace->VertexToCollapse = MostImportantPoint;
        ObjectToDraw->OBJ_PointList[ECounter] = OBJ_PointList[CO.CurrentTrace->VertexToDelete];
		if (Colors)
			ObjectToDraw->Colors[ECounter] = Colors[CO.CurrentTrace->VertexToDelete];
        ObjectToDraw->Absorbers[ECounter] = CO.CurrentTrace->VertexToCollapse;
		CO.AllTrace[CO.CurrentTrace->VertexToDelete].Reorder = ECounter;
        ObjectToDraw->Index2[ECounter] = ECounter;
    }



    for(ECounter = 0; ECounter < ObjectToDraw->RealNumberOfPoints; ECounter++)
    {
        ObjectToDraw->Absorbers[ECounter] = CO.AllTrace[ObjectToDraw->Absorbers[ECounter]].Reorder;
		(*pp_ReorderBuffer)[CO.AllTrace[ECounter].Reorder] = (unsigned short)ECounter;
        CO.CurrentTrace++;
    }
	ObjectToDraw->Absorbers[0] = 0;


    free((void *) OBJ_PointList);
	if (Colors)
	{
		free((void *) Colors);
	}

	/* reorder indexes */
    for(ECounter = 0; ECounter < ObjectToDraw->NumberOfElements; ECounter++)
    {
		MAD_MALLOC(MAD_Face, (*(ObjectToDraw->Elements + ECounter))->FacesIIndexedOnIndex2, (*(ObjectToDraw->Elements + ECounter))->NumberOfTriangles);

        (*(ObjectToDraw->Elements + ECounter))->TotalNumberOfTriangles = (*(ObjectToDraw->Elements + ECounter))->NumberOfTriangles;
        for(BCounter = 0; BCounter < (*(ObjectToDraw->Elements + ECounter))->NumberOfTriangles; BCounter++)
        {
            ObjectToDraw->Elements [ ECounter ]->FacesIIndexedOnIndex2 [ BCounter ] = ObjectToDraw->Elements [ECounter]->Faces [ BCounter ];
            ObjectToDraw->Elements [ ECounter ]->FacesIIndexedOnIndex2 [ BCounter ] .Index[0] = CO.AllTrace[ObjectToDraw->Elements [ ECounter ]->Faces [ BCounter ] .Index[0]].Reorder;
            ObjectToDraw->Elements [ ECounter ]->FacesIIndexedOnIndex2 [ BCounter ] .Index[1] = CO.AllTrace[ObjectToDraw->Elements [ ECounter ]->Faces [ BCounter ] .Index[1]].Reorder;
            ObjectToDraw->Elements [ ECounter ]->FacesIIndexedOnIndex2 [ BCounter ] .Index[2] = CO.AllTrace[ObjectToDraw->Elements [ ECounter ]->Faces [ BCounter ] .Index[2]].Reorder;
			ObjectToDraw->Elements [ ECounter ]->Faces [ BCounter ] = ObjectToDraw->Elements [ ECounter ]->FacesIIndexedOnIndex2 [ BCounter ];
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

	ObjectToDraw -> NumberOfPoints = ObjectToDraw->RealNumberOfPoints;

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

#endif /* ACTIVE_EDITOR */
