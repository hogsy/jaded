/* GEO_LIGHTCUT.h */
#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
/*
TODO
  objets coupé ou non 
  objets ombrants ou pas
  "Contour" cut (pour les coins de penombre)
  
*/



#include "BASe/BAStypes.h"
#ifndef __GEOGLV_H__
#define __GEOGLV_H__

#define GLV_FLGS_WhiteFront		0x00000001
#define GLV_FLGS_BlackFront		0x00000002
#define GLV_FLGS_Original		0x00000004
#define GLV_FLGS_Kilt			0x00000008
#define GLV_FLGS_DeleteIt		0x00000010
#define GLV_FLGS_AIsVisible		0x00000020
#define GLV_FLGS_BIsVisible		0x00000040
#define GLV_FLGS_CIsVisible		0x00000100
#define GLV_FLGS_ClipResultFF	0x00000200
#define GLV_FLGS_ClipResultBF	0x00000400
#define GLV_FLGS_Clipped		0x00000800
#define GLV_FLGS_RSV1			0x00001000
#define GLV_FLGS_RSV2			0x00002000
#define GLV_FLGS_RSV3			0x00004000
#define GLV_FLGS_RSV4			0x00008000
#define GLV_FLGS_FromEdge		0x00010000
#define GLV_FLGS_FromFace		0x00020000
#define GLV_FLGS_FinalEdge		0x00040000
#define GLV_FLGS_Transparent	0x00080000
#define GLV_FLGS_Transparent2	0x00100000
#define GLV_FLGS_DistanceNear	0x00200000
#define GLV_FLGS_DistanceFar	0x00400000
#define GLV_FLGS_DistanceHSL	0x00800000
#define GLV_FLGS_DistanceHSB	0x01000000
#define GLV_FLGS_DBG1			0x02000000
#define GLV_FLGS_DoNotCut		0x04000000
#define GLV_FLGS_Nopenombria	0x08000000
#define GLV_FLGS_DontCutOther	0x10000000

#define GLV_FreeDomDegree		(GLV_FLGS_FromEdge | GLV_FLGS_FromFace)	


#define GLV_MEMORY_GRANULARITY 1024
#define GLV_USE_POLYLINES
#define GLV_USE_OK3
#define GLV_OVERLAPP
#define GLV_FAST_OVERLAP
//#define GLV_OVERLAPP_With_Nght
//#define GLV_DEBUG
//#define GLV_RemovKilt
#define GLV_StayWithEdges
#define GLV_RemovEdges
#define GLV_Convert2GEO
//#define GLV_MAKE_IT_NICER
#define GLV_LIGHTRADIUSMIN  0.0001f
#define GLF_PrecMetric  0.00000001
#define GLF_PrecMetric_UV 0.0000001
#define GLF_FaceExtraction (GLF_PrecMetric * 100.0)
#define GLV_KILT_LENGHT 10.0
#define GLV_WELD_FCT (0.00000000000001)
#define GLV_GENDEF (GLV_FLGS_WhiteFront | GLV_FLGS_BlackFront)
#define GLV_FastPit /* faast point in triangle test */
#define GLV_OC3_MAX_DEPTH	7
#define GLV_OC3_MAX_FPO		20
#define GLV_OC3_MAX_DEPTH_E	7
#define GLV_OC3_MAX_EPO		20
#define GLV_OC3_SQUARE_TOLERANCE 500
#define GLV_OC3_SQUARE_TOLERANCE_EDGES 100

#ifdef JADEFUSION
// SC: Enable memory tracking
#define _GLV_TRACE_MEMORY
#endif

// SC: Disable radiosity adjust (helps reducing memory usage)
//#define _GLV_KEEP_LAST

// SC: Disable show progress (helps reducing memory usage)
//#define _GLV_KEEP_KILT

#if defined(_GLV_TRACE_MEMORY)

void* GLV_p_Alloc(int _iSize, const char* _szFile, int _iLine);
void* GLV_p_Realloc(void* _pPtr, int _iSize, const char* _szFile, int _iLine);
void GLV_Free(void* _pPtr);

#define GLV_ALLOC(_size_)          GLV_p_Alloc(_size_, __FILE__, __LINE__)
#define GLV_REALLOC(_ptr_, _size_) GLV_p_Realloc(_ptr_, _size_, __FILE__, __LINE__)
#define GLV_FREE(_ptr_)            GLV_Free(_ptr_)

#else

#define GLV_REALLOC realloc
_inline_  void *GLV_ALLOC(int size)	
{
	void *Res;
	Res = malloc(size);
	memset(Res,0,size);
	return Res ;
}
#define GLV_FREE	free
#endif

#define GLV_Scalar double

typedef struct MATHD_tdst_Vector_
{
	GLV_Scalar x,y,z;
} MATHD_tdst_Vector;
/******************************************************* Clusters B *************************************************************/
#define CLUSTER_MODE_SIMPLE

#ifdef CLUSTER_MODE_SIMPLE
#define GLV_ClusterGranularyty 2048
#define GLV_CLUSTER_256_KEY (5 + 5)
typedef struct GLV_Cluster_
{
	ULONG				ulNumFLGS;
	ULONG				**p_AllFLGS_256;
	struct GLV_Cluster_			*p_NextFreeCluster;
} GLV_Cluster;

void		GLV_Clst_SetNumbers_0(GLV_Cluster *p_Dst , ULONG Numbers);
ULONG		*GLV_GetFlags();
void		GLV_ReleaseFlags(ULONG *F);
ULONG		GLV_Clst_IsExist(GLV_Cluster *p_Dst , ULONG Value);
void		GLV_Clst_ADDV(GLV_Cluster *p_Dst , ULONG Value);
GLV_Cluster *GLV_NewCluster();
void		GLV_ResetCluster(GLV_Cluster *pCC);
void		GLV_DelCluster(GLV_Cluster *CC);
void		GLV_Clst_OR(GLV_Cluster *p_Dst , GLV_Cluster *p_Src);
ULONG		GLV_ulGetClusterNumber(GLV_Cluster *CC);
ULONG		GLV_ulGetPos(GLV_Cluster *CC , ULONG Value);

#define GLV_Enum_Cluster_Value(Cluster)										\
{																			\
	ULONG MCL_Value , MCL_ValueLast , MCL_ValueSave , MCL_Counter0 ;		\
	ULONG *p_CurFlags,*p_CurFlagsLast,**p_CurFlags256;						\
	p_CurFlags256 = Cluster->p_AllFLGS_256;									\
	MCL_Value = 0;															\
	MCL_ValueLast = Cluster->ulNumFLGS;										\
	while (MCL_Value < MCL_ValueLast)										\
	{																		\
		if (!*(p_CurFlags256))												\
		{																	\
			MCL_Value += 1 << GLV_CLUSTER_256_KEY;							\
		} else																\
		{																	\
			p_CurFlags = *p_CurFlags256;									\
			p_CurFlagsLast = p_CurFlags + ((1 << GLV_CLUSTER_256_KEY) >> 5);\
			while (p_CurFlags < p_CurFlagsLast)								\
			{																\
				MCL_ValueSave = MCL_Value ;									\
				MCL_Counter0 = *(p_CurFlags++);								\
				while (MCL_Counter0)										\
				{															\
					if (MCL_Counter0 & 1)									\
					{														\
																			
#define GLV_Enum_Cluster_Value_End()										\
					}														\
					MCL_Value++;											\
					MCL_Counter0 >>= 1;										\
					if (!(MCL_Counter0 & 0xff))								\
					{														\
						MCL_Value+= 8;										\
						MCL_Counter0 >>= 8;									\
					}														\
				}															\
				MCL_Value = MCL_ValueSave + 32;								\
			}																\
		}																	\
		p_CurFlags256++;													\
	}																		\
}																			\
																			
																			
																			
#else
#define GLV_ClusterShift 2
typedef struct GLV_Cluster_
{
	ULONG				ulNumFLGS;
	ULONG				ulNumClts0;
	ULONG				ulFirstVoidFLGS;
	ULONG				*p_AllFLGS;
	ULONG				*p_HieClts0;
} GLV_Cluster;

#define GLV_Enum_Cluster_Value(Cluster)\
{\
	ULONG MCL_Value , MCL_Counter0 , MCL_Counter1 , MCL_Local;\
	ULONG ulFlags1[1L<<GLV_ClusterShift] , *p_CurFlags;\
	for (MCL_Counter0 = 0 ; MCL_Counter0 < 1L << GLV_ClusterShift ; MCL_Counter0 ++ ) ulFlags1[MCL_Counter0] = -1;\
	for (MCL_Counter0 = 0 ; MCL_Counter0 < Cluster->ulNumClts0 ; MCL_Counter0++ )\
	{\
		if (Cluster->p_HieClts0[MCL_Counter0] != -1)\
		{\
			if (Cluster->p_HieClts0[MCL_Counter0] == -2)\
				p_CurFlags = ulFlags1;\
			else\
				p_CurFlags = &Cluster->p_AllFLGS [ Cluster -> p_HieClts0 [ MCL_Counter0 ] ] ;\
			for (MCL_Counter1 = 0 ; MCL_Counter1 < 1L << GLV_ClusterShift ; MCL_Counter1 ++)\
			{\
				MCL_Value = (MCL_Counter0 << (5 + GLV_ClusterShift)) + (MCL_Counter1 << 5);\
				MCL_Local = *(p_CurFlags++);\
				while (MCL_Local)\
				{\
					if (MCL_Local & 1)\
					{\


#define GLV_Enum_Cluster_Value_End()\
					}\
					MCL_Value ++;\
					MCL_Local >>= 1;\
				}\
			}\
		}\
	}\
}\
/* Clusters */
void GLV_Clst_TST();
GLV_Cluster *GLV_NewCluster();
void GLV_DelCluster(GLV_Cluster *CC);
void GLV_ResetCluster(GLV_Cluster *pCC);
ULONG GLV_Clst_IsExist(GLV_Cluster *p_Dst , ULONG Value);
void GLV_Clst_ADDV(GLV_Cluster *p_Dst , ULONG Value);
void GLV_Clst_OR(GLV_Cluster *p_Dst , GLV_Cluster *p_Src);
ULONG GLV_ulGetClusterNumber(GLV_Cluster *CC);
ULONG GLV_ulGetPos(GLV_Cluster *CC , ULONG Value);
#endif

/******************************************************* Clusters E *************************************************************/

typedef struct GLV_OCtreeNode_
{
	struct GLV_OCtreeNode_	*p_OctreeFather;
	MATHD_tdst_Vector		stAxisMax;
	MATHD_tdst_Vector		stAxisMin;
	ULONG					ulNumberOfSubNodes;
	struct GLV_OCtreeNode_	*p_SubOcTree[8];
	GLV_Cluster				*p_stThisCLT;
} GLV_OCtreeNode;

typedef struct tdst_GLV_Source_
{
	MATH_tdst_Matrix	stMatrixOfObject;
	LIGHT_tdst_Light	*p_SourceLight;
} tdst_GLV_Source;

typedef struct tdst_GLV_Reference_
{
	OBJ_tdst_GameObject *pst_Object;
} tdst_GLV_Reference;

typedef struct tdst_GLV_PointUV_
{
	GLV_Scalar	UV[2];
	ULONG		MaterialNum;
} tdst_GLV_PointUV;

typedef struct tdst_GLV_PointPonderation_
{
	float		Ponderation;
	ULONG		MatriceNum;
} tdst_GLV_PointPonderation;

typedef struct tdst_GLV_Plane_
{
	MATHD_tdst_Vector	Normale;
	GLV_Scalar			fDistanceTo0;
}tdst_GLV_Plane;


typedef struct tdst_GLV_Point_
{
	MATHD_tdst_Vector	P3D;
	union {
		struct {
			MATHD_tdst_Vector	UpPoint;	/* Could be the normal */
			MATHD_tdst_Vector	DownPoint;
			tdst_GLV_PointUV	stUVInfo;
			ULONG				DownIndex;
			ULONG       		ulFlags;
			ULONG				ulColor; /* Could be a point, an edge or a face */
			ULONG				ulChannel;
		};
		struct {
			ULONG				ulNumberOfPlanes;
			tdst_GLV_Plane	GLV_Point_Plane[2];	/* Could be the normal */
		};
	};
	ULONG				ulRefIndex; /* Could be a point, an edge or a face */
	ULONG				ulSurfaceOwner;
	GLV_Scalar			Distance2SB;

} tdst_GLV_Point;


typedef struct tdst_GLV_Face_
{
	ULONG			 Index[3];					
	ULONG			 Nghbr[3];
		
	/* Original mesh saving */
	union {
		GLV_Scalar		 UV[6];
		ULONG			 UVIndex[6];
	};
	ULONG			 Colours[3];
	float			 falpha[3];
	MATH_tdst_Vector OriginalPointsNormales[3];


	tdst_GLV_Plane	 Plane;
	tdst_GLV_Plane	 PenombriaPlane;
	ULONG	         ulFlags; /* Is WhiteFront , BlackFront , */
	ULONG			 ulMARK;  
	ULONG			 ulKey;
	ULONG			 ulElementJADE;
	ULONG			 ulPolylineNum;
	ULONG			 ulSubdivideFlags;
	ULONG			 ulSurfaceNumber;
	ULONG			 ulDistanceToBorder;

	ULONG			 ulChannel;
	ULONG		     PassivityFlags;


} tdst_GLV_Face;

typedef struct tdst_GLV_Edge_
{
	ULONG				Index[2];
	ULONG				ulFlags;
	ULONG				FromFaces[2];
} tdst_GLV_Edge;

typedef struct tdst_LightDescription_
{
	MATHD_tdst_Vector	stLightCenterOS;
	GLV_Scalar			fLightRadius;
	GLV_Scalar			fLightNear;
	GLV_Scalar			fLightFar;
	GLV_Scalar			fCosBigAlpha;
	GLV_Scalar			fCosLittleAlpha;
	ULONG				ulColor;
	ULONG				ulLightFlags;
	ULONG				bLightPenombriaOn;
	GLV_OCtreeNode		*p_Octree; /* one OCTREE per light */
	MATHD_tdst_Vector	LDir;
	ULONG				ulChannel;
}
tdst_LightDescription;

typedef struct tdst_ObjectBDVolume_
{
	MATHD_tdst_Vector	stVectors[8];
	ULONG				flag;
	ULONG				Clippingflag;
	ULONG				ulMARK;
} tdst_ObjectBDVolume;


typedef struct tdst_GLV_
{
	ULONG				ulNumberOfOBV;
	tdst_ObjectBDVolume *p_OBV;
//	tdst_GLV_Source		stSource;
	ULONG ulNumberOfReferences;
	tdst_GLV_Reference	*p_stRefs;
	ULONG ulRealNumberOfPoints;
	ULONG ulNumberOfPoints;
	tdst_GLV_Point		*p_stPoints;
	ULONG ulRealNumberOfEdges;
	ULONG ulNumberOfEdges;
	tdst_GLV_Edge		*p_stEdges;
	ULONG ulRealNumberOfFaces;
	ULONG ulNumberOfFaces;
	tdst_GLV_Face		*p_stFaces;
	/* LightSource */
	ULONG				ulNumberOfLight;
	tdst_LightDescription *p_Lights;

	/* Radiosity matrice */
	ULONG			 **p_FormFactors;

	/* Normalization */
	MATHD_tdst_Vector	OriginalPos,OriginalScale;

}
 tdst_GLV;

/* Public   functions */
tdst_GLV	*NewGLV();
tdst_GLV *	GLV_Create			(LIGHT_tdst_Light *p_SourceLight , MATH_tdst_Matrix	*p_stMatrix);
void		GLV_Destroy			(tdst_GLV *p_GLV);
#ifdef JADEFUSION
BOOL		GLV_AddGRO			(tdst_GLV *p_KiltGLV , OBJ_tdst_GameObject *p_stGRO , ULONG (*SeprogressPos) (float F01, char *) , ULONG Mark , ULONG BE, float ET );
#else
void		GLV_AddGRO			(tdst_GLV *p_KiltGLV , OBJ_tdst_GameObject *p_stGRO , ULONG (*SeprogressPos) (float F01, char *) , ULONG Mark , ULONG BE, float ET );
#endif
void		GLV_ComputeAnObject	(struct WOR_tdst_World_ *_pst_World , ULONG OnlySel , ULONG OPTIMIZE , ULONG BreakEdge , float BreakEdgeThresh , ULONG bEnablePenombria , ULONG SmoothFronteer , ULONG ComputeRadiosity , ULONG RadiosityBackColor , ULONG bSmoothRadioResult , ULONG bFastRadio , ULONG (*SeprogressPos) (float F01, char *));
void		GLV_AdjustRadiosity	(struct WOR_tdst_World_ *_pst_World , ULONG OnlySel , ULONG OPTIMIZE , ULONG BreakEdge , float BreakEdgeThresh , ULONG bEnablePenombria , ULONG SmoothFronteer , ULONG ComputeRadiosity , ULONG RadiosityBackColor , ULONG bSmoothRadioResult , ULONG bFastRadio , ULONG (*SeprogressPos) (float F01, char *));
ULONG		GLV_ComputeRadiosity_Compute_FF_Precut (tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *));
void		GLV_DestroyRadiosity(struct WOR_tdst_World_ *_pst_World , ULONG OnlySel );
tdst_GLV *	GLV_Duplicate		(tdst_GLV *	p_stGLV);
void		GLV_COPY_VARS		(tdst_GLV *	p_stGLVDst , tdst_GLV *	p_stGLVSrc);
ULONG		GLV_CutAll			(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *) , GLV_OCtreeNode *p_stOctreeROOT);
ULONG		GLV_bIsCut			(tdst_GLV * p_GLV , ULONG FSource , ULONG F2 );
ULONG		GLV_DetectOO		(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *));
ULONG		GLV_RemoveOverLap	(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *));
ULONG		GLV_RemoveOverlapedEdges(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *) , GLV_OCtreeNode *p_stOctreeROOT);
void		GLV_RemoveOverLap_LOCAL(tdst_GLV *p_stGLVSrc , tdst_GLV *p_stGLV_LOCAL , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill );
void		GLV_CutAndLightGRO	(OBJ_tdst_GameObject *p_stGRO , tdst_GLV *	p_stGLV);
void		GLV_AddGLV			(tdst_GLV *p_Dst,tdst_GLV *p_Src);
void		GLV_GenerateFinalEdges(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *));
void		GLV_DraweTheKilt	(GDI_tdst_DisplayData *_pst_DD);
void		GLV_ShowTheKilt	(ULONG bV);
ULONG		GLV_IsEqualPlanes(tdst_GLV_Plane *P1, tdst_GLV_Plane *P2);
ULONG		GLV_IsEqualPlanesNoBackface(tdst_GLV_Plane *P1, tdst_GLV_Plane *P2);
void		GLV_ComputeNormale(tdst_GLV *p_GLV,tdst_GLV_Face *p_FT);
void		GLD_BreakEdge(  tdst_GLV * p_GLV , float thresh);
void		GLV_ComputeRadiosity (tdst_GLV *p_stGLV ,  ULONG RadiosityColor , ULONG bSmoothResult , ULONG bFastRadio , ULONG (*SeprogressPos) (float F01, char *));
void		GLV_ComputeRadiosity_Compute_FF (tdst_GLV *p_stGLV ,  ULONG RadiosityColor , ULONG bSmoothResult , ULONG bFastRadio , ULONG (*SeprogressPos) (float F01, char *));
void		GLV_ComputeRadiosity_Use_FF (tdst_GLV *p_stGLV ,  ULONG RadiosityColor , ULONG bSmoothResult , ULONG bFastRadio , ULONG (*SeprogressPos) (float F01, char *));
void		GLV_Normalize_GLV(tdst_GLV *p_GLV , ULONG Expand);


/* Internal functions */
tdst_GLV	*GLV_Geo2GLV 		(GEO_tdst_Object *pst_Object,MATH_tdst_Matrix	*p_Matrix , ULONG *p_ulColours , ULONG ulFlags , tdst_ObjectBDVolume *p_stOBV , ULONG MaterialFlags);
GEO_tdst_Object *GLV_GLV2GEO	(tdst_GLV *p_stGLV,MATH_tdst_Matrix	*p_Matrix,ULONG ulMArk);
ULONG		GLV_FlicTst			(tdst_GLV *p_GLV,ULONG FNum);
ULONG		GLV_FlicTst_With_0	(tdst_GLV *p_GLV,ULONG FNum);
void		GLV_ComputeNormales	(tdst_GLV *p_GLV);
GLV_Scalar	GLV_GetSurf			(tdst_GLV *p_GLV,tdst_GLV_Face *p_FT);
void		GLV_ComputeNghbr	(tdst_GLV *p_GLV);
void		GLV_BreakUncompatibleLinks( tdst_GLV * p_GLV );
void		GLV_BreakUncompatibleLinks_JadeElement( tdst_GLV * p_GLV );
void		GLV_BreakUncompatibleLinks_JadeElement_and_UV( tdst_GLV * p_GLV );
ULONG		GLV_Unify2Face		(tdst_GLV *p_GLV, ULONG N1, ULONG N2);
void		GLV_MinMaxFace		(tdst_GLV *p_stGLV , ULONG F1 , MATHD_tdst_Vector *p_PMIN , MATHD_tdst_Vector *p_PMAX);
void		GLV_MinMaxEdge		(MATHD_tdst_Vector    *p_bA , MATHD_tdst_Vector    *p_bB , MATHD_tdst_Vector    *pstMin , MATHD_tdst_Vector    *pstMax);
void		GLD_Remove2BD		(tdst_GLV * p_GLV );
int			GLV_IsBoxTouchBox	(MATHD_tdst_Vector *p_PMIN1 , MATHD_tdst_Vector *p_PMAX1,MATHD_tdst_Vector *p_PMIN2 , MATHD_tdst_Vector *p_PMAX2);
int			GLV_IsPointInBox(MATHD_tdst_Vector *p_PMIN1 , MATHD_tdst_Vector *p_PMAX1,MATHD_tdst_Vector *p_P );
void		GLV_ComputeGLVBox	(tdst_GLV * p_GLV ,MATHD_tdst_Vector *p_PMIN , MATHD_tdst_Vector *p_PMAX , ULONG Flags);
void		GLV_ExpandBox( MATHD_tdst_Vector *p_PMIN , MATHD_tdst_Vector *p_PMAX , GLV_Scalar Value);
void		GLV_SetNumbers		(tdst_GLV *p_GLV,ULONG Points , ULONG Edges , ULONG Triangles,ULONG ulFlag);
void		GLV_Clip			(tdst_GLV *p_GLV,tdst_GLV_Plane *p_Plane);
void		GLD_RemoveUnusedIndexes( tdst_GLV * p_GLV );
void		GLD_RemoveIllegalFaces( tdst_GLV * p_GLV );
void		GLD_RemoveZeroSurfaces( tdst_GLV * p_GLV );
void		GLD_RemoveBadEdges( tdst_GLV * p_GLV );
void		GLD_RemoveDoubleEdges( tdst_GLV * p_GLV );
void		GLD_OptimizeEdges( tdst_GLV * p_GLV );
void		GLD_MakeItShorter( tdst_GLV * p_GLV , ULONG Mode);

void		GLV_ComputeSurfaces(tdst_GLV *p_stGLV , u32 ulColorCompatible);
void		GLV_ClearSurfaces(tdst_GLV *p_stGLV );

void		GLV_Optimize(tdst_GLV *p_stGLV , ULONG OptimizeLVL , ULONG bUseScalars , ULONG (*SeprogressPos) (float F01, char *));
int			GLD_IsIllegalFaces( tdst_GLV * p_GLV , tdst_GLV_Face *p_F );
void		GLD_Weld( tdst_GLV * p_GLV , GLV_Scalar Thresh , ULONG ulMode);
void		GLD_BlowUpIndexion(  tdst_GLV * p_GLV , ULONG MOde);
void		GLD_SortFaceIndexion( tdst_GLV * p_GLV );
tdst_GLV *	GLD_CompressUVIndexion( tdst_GLV * p_GLV );
int			GLV_IsColinear( MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_P2 , MATHD_tdst_Vector *p_P3);
void		GLV_MARK(tdst_GLV *p_GLV , ULONG MARK , ULONG ulChannel);
void		GLV_ComputeColors(tdst_GLV *p_stGLV , ULONG SmoothFronteer , ULONG (*SeprogressPos) (float F01, char *));
void		GLV_FirstLighSubdivide(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *));
void		GLV_CreateSphere(tdst_GLV*p_stGLV , ULONG ulFlags, ULONG MArk , MATHD_tdst_Vector *p_Center , GLV_Scalar Radius);
void		GLV_CreateCone(tdst_GLV*p_stGLV , ULONG ulFlags , ULONG lightNum , MATHD_tdst_Vector *p_Center , MATHD_tdst_Vector *pDirection , GLV_Scalar Alpha , GLV_Scalar Lenght);
void		GLV_SubdivideLenght(tdst_GLV *p_stGLV , float fLenghtMax);//
void		GLV_Subdivide(tdst_GLV *p_stGLV , ULONG KeepItSquare);//


/* Ray-tracing functions */
int			GLV_IsEdgeTouchFace	(tdst_GLV *p_stGLV , ULONG F , tdst_GLV_Face *p_FTSourceIfExist , MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_P2 , GLV_Scalar *Distance);
int			GLV_PointFromEdgeTouchFace(tdst_GLV_Face *p_FT , tdst_GLV_Face *p_FTSourceIfExist , MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_P2, MATHD_tdst_Vector *p_PR);
int			GLV_IsFaceTouchFace	(tdst_GLV *p_stGLV , ULONG F1 , ULONG F2);
void		GLV_GetTriRayPoint(tdst_GLV *p_stGLV , ULONG C1, MATHD_tdst_Vector *p_PR );
ULONG		GLV_RT_LightGeomRemove(tdst_GLV *p_stGLV , MATHD_tdst_Vector *stDirection, ULONG LightNum);
void		GLV_RT_ComputeLightCenter(tdst_GLV *p_stGLV, ULONG LightNum , MATHD_tdst_Vector  *pstTriCenter , MATHD_tdst_Vector  *pstLightCenter);

ULONG		GLV_RT_GetIsHit(tdst_GLV *p_stGLV , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , tdst_GLV_Face *p_F , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill );
ULONG		GLV_RT_GetIsHitChannel(tdst_GLV *p_stGLV , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , tdst_GLV_Face *p_F , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill , ULONG Channel);
ULONG		GLV_RT_GetIsHitAll(tdst_GLV *p_stGLV , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , tdst_GLV_Face *p_F , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill , GLV_Cluster *p_ClusterResult );
ULONG		GLV_RT_GetIsHitNearest(tdst_GLV *p_stGLV , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , tdst_GLV_Face *p_F , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill , ULONG *FaceResult);

/* Tools */
void GLV_CutBooleanOverlad(tdst_GLV *p_stGLV , u32 DestroyInvisible);
int GLV_IsBetween( MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_PM , MATHD_tdst_Vector *p_P2);
int GLV_IsColinear( MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_P2 , MATHD_tdst_Vector *p_P3);
int GLV_PointTouchPlane(tdst_GLV_Face *p_FT , MATHD_tdst_Vector *p_P1 );
int GLV_IsPointInTriangle(tdst_GLV *p_stGLV , ULONG F , MATHD_tdst_Vector *p_P3D);
void GLV_ResolvePolyline(tdst_GLV *p_stGLVToClip );
ULONG GLV_ul_EdgeCutEdge2D(GLV_Scalar *p2DCoords , ULONG E1I1 , ULONG E1I2 , ULONG E2I1 , ULONG E2I2 );
GLV_Scalar  GLV_2DDotProduct(ULONG I1 , ULONG I2 , ULONG IT , GLV_Scalar *p2DCoords);
GLV_Scalar  GLV_DistanceEdgePoint( MATHD_tdst_Vector *p_E1 , MATHD_tdst_Vector *p_E2 , MATHD_tdst_Vector *p_P);
void GLV_CutWithCoplanrEdges(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *));
void GLV_RemoveKILT(tdst_GLV *p_GLV);
ULONG GLV_f_IsPointOnEdge( MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_P2 , MATHD_tdst_Vector *p_P);
void GLV_ULto4Scalar(ULONG Col, GLV_Scalar *p4);
ULONG GLV_4ScalarToUL( GLV_Scalar *p4);
ULONG GLV_EqualColor(ULONG A , ULONG B );
ULONG GLV_EqualScalar(GLV_Scalar A , GLV_Scalar B );
void GLV_OPT_GetScalarPlane ( tdst_GLV *p_stGLV , ULONG Face , GLV_Scalar *p_3SCL , tdst_GLV_Plane *p_P);

#ifdef JADEFUSION
void GLV_FreeFlagMem(void);
void GLV_FreeClusterMem(void);
#endif

/* OK3 */
void GLV_Octree_DiveAndCompute(tdst_GLV *p_stGLV , GLV_OCtreeNode *p_Octree , ULONG MAXDepth , ULONG MAXFaces , ULONG (*SeprogressPos) (float F01, char *));
void GLV_Octree_DiveAndCompute_for_edges(tdst_GLV *p_stGLV , GLV_OCtreeNode *p_Octree , ULONG MAXDepth , ULONG MAXEdges , ULONG (*SeprogressPos) (float F01, char *));
void GLV_Octree_DiveAndIntesectLine( GLV_OCtreeNode *p_Octree , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , GLV_Cluster *p_stCluster);
void GLV_Octree_DiveAndIntesectTriangle( GLV_OCtreeNode *p_Octree , ULONG Triangle , GLV_Cluster *p_stCluster );
void GLV_Octree_DiveAndIntesectTriangle_On_Line_OK3( GLV_OCtreeNode *p_Octree , tdst_GLV *p_stGLV , ULONG Triangle , GLV_Cluster *p_stCluster );
void GLV_Octree_Destroy(GLV_OCtreeNode *p_Octree );
void GLV_Octree_Draw(GLV_OCtreeNode *p_Octree , GDI_tdst_DisplayData *_pst_DD);
void GLV_Octree_Expand_REC(GLV_OCtreeNode *p_Octree , float fExpand);
/* MATHD */
void MATHD_BlendVector(MATHD_tdst_Vector *p_Dst , MATHD_tdst_Vector *p_A , MATHD_tdst_Vector *p_B , GLV_Scalar F);
void MATHD_ScaleVector(MATHD_tdst_Vector *p_Dst , MATHD_tdst_Vector *p_Src , GLV_Scalar F);
void MATHD_ScaleEqualVector(MATHD_tdst_Vector *p_Dst , GLV_Scalar F);
void MATHD_AddScaleVector(MATHD_tdst_Vector *p_Dst , MATHD_tdst_Vector *p_A , MATHD_tdst_Vector *p_B , GLV_Scalar F);
void MATHD_NormalizeAnyVector(MATHD_tdst_Vector *p_Dst , MATHD_tdst_Vector *p_A );
void MATHD_SetNormVector(MATHD_tdst_Vector *p_Dst , MATHD_tdst_Vector *p_A , GLV_Scalar F);
GLV_Scalar MATHD_f_NormVector(MATHD_tdst_Vector *p_Dst );

void MATHD_AddVector(MATHD_tdst_Vector *VDst, MATHD_tdst_Vector *A, MATHD_tdst_Vector *B);
void MATHD_SubVector(MATHD_tdst_Vector *VDst, MATHD_tdst_Vector *A, MATHD_tdst_Vector *B);
void MATHD_CrossProduct(MATHD_tdst_Vector *Dest, MATHD_tdst_Vector *A, MATHD_tdst_Vector *B);
GLV_Scalar MATHD_f_InvNormVector(MATHD_tdst_Vector *A);
void MATHD_NormalizeVector(MATHD_tdst_Vector *Dest, MATHD_tdst_Vector *A);
void MATHD_AbsoluteVector(MATHD_tdst_Vector *Dest);


GLV_Scalar MATHD_f_SqrNormVector(MATHD_tdst_Vector *p_Dst );
//GLV_Scalar MATHD_f_DotProduct(MATHD_tdst_Vector *p_A , MATHD_tdst_Vector *p_B);
void MATHD_InitVector(MATHD_tdst_Vector *p_A ,GLV_Scalar , GLV_Scalar , GLV_Scalar );

void MATHD_To_MATH(MATH_tdst_Vector *p_A ,MATHD_tdst_Vector *p_B );
void MATH_To_MATHD(MATHD_tdst_Vector *p_A ,MATH_tdst_Vector *p_B );

_inline_ GLV_Scalar MATHD_f_DotProduct(MATHD_tdst_Vector *A, MATHD_tdst_Vector *B)
{
    return((A->x) * (B->x) + (A->y) * (B->y) + (A->z) * (B->z));
}
_inline_ int ulDifferentSign(GLV_Scalar	_fV1 , GLV_Scalar	_fV2 )
{
	if ((_fV1  * _fV2 ) < 0.0f) 
		return 1;
	else
		return 0;
}
_inline_ GLV_Scalar MATHD_f_DistanceToPlane(tdst_GLV_Plane *p , MATHD_tdst_Vector *p_P)
{
	return MATHD_f_DotProduct(p_P , &p->Normale) - p->fDistanceTo0;
}

_inline_ GLV_Scalar MATHD_f_Distance(MATHD_tdst_Vector *p_PA , MATHD_tdst_Vector *p_PB)
{
	MATHD_tdst_Vector st_substract;
	MATHD_SubVector(&st_substract , p_PA , p_PB);
	return MATHD_f_NormVector(&st_substract);
}

/* DBUG Functions */
void		GLV_Verify(tdst_GLV *p_GLV);
void		GLV_BUG(ULONG ulLevel , char *String);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif 
