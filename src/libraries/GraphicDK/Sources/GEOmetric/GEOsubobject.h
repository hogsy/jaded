/*$T GEOsubobject.h GC! 1.081 03/18/02 17:46:49 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __GEOSUBOBJECT_H__
#define __GEOSUBOBJECT_H__
#include "BASe/BAStypes.h"

/* #include "GEOmetric/GEOobject.h" */
#include "MATHs/MATH.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/* for sub object flags */
#define GEO_Cul_SOF_TopologyChange	        0x00000011
#define GEO_Cul_SOF_NonMagnifold	        0x00000002
#define GEO_Cul_SOF_FaceUseTransformedPoint 0x00000004
#define GEO_Cul_SOF_TopologyChange1	        0x00000010
/* for selection */
#define GEO_SO_Selected		0x01
#define GEO_SO_Selectable	0x02

/*$4
 ***********************************************************************************************************************
    Structure
 ***********************************************************************************************************************
 */

typedef struct	GEO_tdst_SubObject_ETurnData_
{
	ULONG	aul_Old[2];
	ULONG	aul_New[2];
} GEO_tdst_SubObject_ETurnData;

typedef struct	GEO_tdst_SubObject_ECutData_
{
	LONG	al_OppVertex[2];
	LONG	al_AddUV[2];
	LONG	l_AddVertex;
	LONG	al_EdgeVertex[2];
	float	f_Cut;
	int		i_Divide;
} GEO_tdst_SubObject_ECutData;

typedef struct	GEO_tdst_SubObject_Edge_
{
	ULONG	ul_Point[2];
	ULONG	ul_Triangle[4];
	USHORT	uw_Index[4];
} GEO_tdst_SubObject_Edge;

typedef struct	GEO_tdst_SubObject_Face_
{
	MATH_tdst_Vector	st_Center;
	MATH_tdst_Vector	st_Normal;
    float               fNZ;
	unsigned short		uw_Element;
	unsigned short		uw_Index;
} GEO_tdst_SubObject_Face;

typedef struct	GEO_tdst_SubObject_UVUpdate_
{
    void                *TMin, *TMax;   
    int                 uv, xyz;
	MATH_tdst_Vector	C, CA, CB;
	GEO_tdst_UV			c, ca, cb;
} GEO_tdst_SubObject_UVUpdate;

typedef struct	GEO_tdst_SubObject_
{
	ULONG					    ul_Flags;
	ULONG					    ul_NbEdges;
	ULONG					    ul_NbFaces;
	ULONG					    ul_PtInBuffer;

	GEO_tdst_SubObject_Edge     *dst_Edge;
	GEO_tdst_SubObject_Face     *dst_Faces;

    GEO_Vertex                  *dst_PointTransformed;
	MATH_tdst_Vector		    *dst_3D;
	MATH_tdst_Vector		    *dst_2D;

	char					    *dc_VSel;
	ULONG						FloatSelNumber;
	float						*pf_FloatSelection_V;
	char					    *dc_ESel;
	char					    *dc_FSel;

	float					    af_MRMParams[2];
	BOOL					    b_Stripped;

	ULONG					    ul_UVMapper_Param;
	CHAR					    *pc_UVMapper_Sel;
	CHAR					    c_UVMapper_SelInc;
	void                        (*pfn_UVMapper_Update) (ULONG);
	LONG                        (*pfnl_UVMapper_Request) (ULONG, ULONG);

	ULONG	                    ul_Tool_Param;
	void                        (*pfn_Tool_Update) (ULONG, struct GEO_tdst_Object_ *);

    ULONG                       ul_UVUpdate_Nb, ul_UVUpdate_Max;
	GEO_tdst_SubObject_UVUpdate *dst_UVUpdate;

    void                        *pst_MorphData;
} GEO_tdst_SubObject;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
void								GEO_SubObject_AllocBuffers(struct GEO_tdst_Object_ *);
void								GEO_SubObject_Create(struct GEO_tdst_Object_ *);
void								GEO_SubObject_Free(struct GEO_tdst_Object_ *);
LONG								GEO_l_SubObject_IsAllocated(struct GEO_tdst_Object_ *);
void								GEO_SubObject_HideUpdate(struct GEO_tdst_Object_ *, BOOL);

void								GEO_SubObject_UnselAll(struct GEO_tdst_Object_ *);
void								GEO_SubObject_InvertSel(struct GEO_tdst_Object_ *, int);
void								GEO_SubObject_CannotSelAll(struct GEO_tdst_Object_ *);
char								*GEO_pc_SubObject_GetSelData(struct GEO_tdst_Object_ *, int, LONG *);
LONG								GEO_l_SubObject_ComputePickedVerticesCenter
									(
										struct GEO_tdst_Object_ *,
										MATH_tdst_Vector *,
										int
									);
LONG								GEO_l_SubObject_GetNbPickedVertices(struct GEO_tdst_Object_ *);
LONG								GEO_l_SubObject_GetNbPickedVerticesAndOneIndex(struct GEO_tdst_Object_ *, int *);
void								GEO_SubObject_ColorVertices(struct GEO_tdst_Object_ *, ULONG **, ULONG, float);
void								GEO_SubObject_ColorAlphaVertices(struct GEO_tdst_Object_ *, ULONG **, ULONG, float);
LONG								GEO_l_SubObject_PickVertexColor(struct GEO_tdst_Object_ *, ULONG *, ULONG *);
void								GEO_SubObject_MoveVertices(struct GEO_tdst_Object_ *, MATH_tdst_Vector *, char);
void								GEO_SubObject_RotateVertices(struct GEO_tdst_Object_ *, MATH_tdst_Matrix *, char, int );
void								GEO_SubObject_ScaleVertices
									(
										struct GEO_tdst_Object_ *,
										MATH_tdst_Vector *,
										MATH_tdst_Vector *,
                                        float, GEO_Vertex *, void *, int
									);
LONG                                GEO_l_SubObject_VSet( struct GEO_tdst_Object_ *, MATH_tdst_Vector *);
void								GEO_SubObject_VUpdateESel(struct GEO_tdst_Object_ *);
void								GEO_SubObject_VUpdateFSel(struct GEO_tdst_Object_ *);
void								GEO_SubObject_InitVMoves(struct GEO_tdst_Object_ *, BOOL);
void                                GEO_SubObject_UseVMoves( struct GEO_tdst_Object_ *);

int									GEO_i_SubObject_VPick
									(
										struct GEO_tdst_Object_ *,
										MATH_tdst_Vector *,
										char,
										OBJ_tdst_GameObject *
									);
int									GEO_i_SubObject_VPickBox
									(
										struct GEO_tdst_Object_ *,
										MATH_tdst_Vector *,
										MATH_tdst_Vector *,
										struct OBJ_tdst_GameObject_ *
									);
void								GEO_SubObject_VWeld(struct GEO_tdst_Object_ *, int);
int									GEO_i_SubObject_VWeldThresh(GEO_tdst_Object *, float);
void								GEO_SubObject_VDelSel(struct GEO_tdst_Object_ *, LONG **);
int									GEO_i_SubObject_VBuildFace(struct GEO_tdst_Object_ *, ULONG, ULONG, ULONG, int);
void                                GEO_SubObject_VSym( struct GEO_tdst_Object_ *, int );
void                                GEO_SubObject_VSymAll( struct GEO_tdst_Object_ *, GEO_Vertex *, MATH_tdst_Vector * );

void								GEO_SubObject_EUpdateVSel(struct GEO_tdst_Object_ *);
void								GEO_SubObject_ESelectBad(struct GEO_tdst_Object_ *);
void								GEO_SubObject_BuildEdgeData(struct GEO_tdst_Object_ *);
void								GEO_SubObject_TestEdge(struct GEO_tdst_Object_ *);
int									GEO_i_SubObject_EFind(struct GEO_tdst_Object_ *, ULONG, ULONG);
void								GEO_SubObject_ETurn
									(
										struct GEO_tdst_Object_ *,
										GEO_tdst_SubObject_Edge *,
										GEO_tdst_SubObject_ETurnData *
									);
int									GEO_i_SubObject_ETurnSel(struct GEO_tdst_Object_ *, GEO_tdst_SubObject_ETurnData *);
int									GEO_i_SubObject_ECut
									(
										struct GEO_tdst_Object_ *,
										GEO_tdst_SubObject_Edge *,
										float,
										GEO_tdst_SubObject_ECutData *
									);
int									GEO_i_SubObject_ECutSel
									(
										struct GDI_tdst_DisplayData_ *,
										struct GEO_tdst_Object_ *,
										MATH_tdst_Vector *,
										GEO_tdst_SubObject_ECutData *
									);
int									GEO_i_SubObject_ECutter
									(
										struct GDI_tdst_DisplayData_ *,
										struct GEO_tdst_Object_ *,
										MATH_tdst_Vector *,
										MATH_tdst_Vector *,
										MATH_tdst_Vector *
									);
struct GEO_tdst_IndexedTriangle_	*GEO_SubObject_GetTriangleFromEdgeIndex(struct GEO_tdst_Object_ *, ULONG);
void								GEO_SubObject_EExtrude(struct GEO_tdst_Object_ *, int);

int									GEO_i_SubObject_EPick
									(
										struct GEO_tdst_Object_ *,
										MATH_tdst_Vector *,
										MATH_tdst_Vector *,
										float *,
										OBJ_tdst_GameObject *
									);
int									GEO_i_SubObject_EPickBox
									(
										struct GEO_tdst_Object_ *,
										MATH_tdst_Vector *,
										MATH_tdst_Vector *,
										char,
										struct OBJ_tdst_GameObject_ *
									);

LONG								GEO_l_SubObject_FNbSel(struct GEO_tdst_Object_ *);
void								GEO_SubObject_FUpdateVSel(struct GEO_tdst_Object_ *);
void								GEO_SubObject_FSelectBad(struct GEO_tdst_Object_ *);
void								GEO_SubObject_BuildFaceData(struct GEO_tdst_Object_ *);
void								GEO_SubObject_FlipNormals(struct GEO_tdst_Object_ *);
void								GEO_SubObject_ChangeID(struct GEO_tdst_Object_ *, int, BOOL);
int									GEO_i_SubObject_FPick(struct GEO_tdst_Object_ *, MATH_tdst_Vector *, char, int, char);
int									GEO_i_SubObject_FPickBox
									(
										struct GEO_tdst_Object_ *,
										MATH_tdst_Vector *,
										MATH_tdst_Vector *,
										int, char, 
										struct OBJ_tdst_GameObject_ *
									);
int									GEO_i_SubObject_FPickBoxWithPB
									(
										struct GEO_tdst_Object_ *,
										MATH_tdst_Vector *,
										MATH_tdst_Vector *,
										int, 
										struct OBJ_tdst_GameObject_ *,
										struct SOFT_tdst_PickingBuffer_ *
									);
int									GEO_i_SubObject_FSelBad(struct GEO_tdst_Object_ *);
void								GEO_SubObject_FDelSel(struct GEO_tdst_Object_ *, BOOL);
void								GEO_SubObject_FDetach(struct GEO_tdst_Object_ *);
void								GEO_SubObject_FAttach
									(
										struct GEO_tdst_Object_ *,
										struct GEO_tdst_Object_ *,
                                        ULONG *,
										MATH_tdst_Matrix *,
                                        OBJ_tdst_GameObject *, 
                                        OBJ_tdst_GameObject *
                                    );
void								GEO_SubObject_FDuplicate(struct GEO_tdst_Object_ *);
void								GEO_SubObject_FExtrude(struct GEO_tdst_Object_ *);
int									GEO_i_SubObject_FCut
									(
										struct GEO_tdst_Object_ *,
										MATH_tdst_Matrix *,
										MATH_tdst_Vector *,
										MATH_tdst_Vector *,
										int,
										int
									);
int									GEO_i_SubObject_FSelectExt(struct GEO_tdst_Object_ *, int);
int									GEO_i_SubObject_FSelectExtUV(struct GEO_tdst_Object_ *);
int									GEO_i_SubObject_FSelectNear(struct GEO_tdst_Object_ *, int, int);
void								GEO_i_SubObject_FHarmonizeIndex
									(
										struct GEO_tdst_Object_ *,
										struct GEO_tdst_Object_ *
									);
#ifdef JADEFUSION
void								GEO_SubObject_HideSelectedFaces(struct GEO_tdst_Object_ *, struct GRO_tdst_Visu_*, BOOL, BOOL);
#else
void								GEO_SubObject_HideSelectedFaces(struct GEO_tdst_Object_ *, BOOL, BOOL);
#endif
void								GEO_SubObject_RemoveBadFaces(GEO_tdst_Object *);
void								GEO_SubObject_UVClean(struct GEO_tdst_Object_ *);
void								GEO_SubObject_UVCenter(struct GEO_tdst_Object_ *);
void								GEO_SubObject_UVCenterWithMaterial(struct GEO_tdst_Object_ *, struct MAT_tdst_Material_ *);
void								GEO_SubObject_SetUVfor3DText(struct GEO_tdst_Object_ *, int );
void								GEO_PickUV( GEO_tdst_Object *, GEO_tdst_Object *, char );

/*$4
 ***********************************************************************************************************************
    Macros for non editors versions
 ***********************************************************************************************************************
 */

#else /* ACTIVE_EDITORS */

#define GEO_SubObject_AllocBuffers(a)
#define GEO_SubObject_Create(a)
#define GEO_SubObject_Free(a)
#define GEO_l_SubObject_IsAllocated(a)						0

#define GEO_l_SubObject_ComputePickedVerticesCenter(a, b, c)	0
#define GEO_l_SubObject_GetNbPickedVertices(a)				0
#define GEO_SubObject_ColorVertices(a, b, c, d)
#define GEO_SubObject_ColorAlphaVertices(a, b, c, d)
#define GEO_l_SubObject_PickVertexColor(a, b, c)	0
#define GEO_SubObject_MoveVertices(a, b, c)
#define GEO_SubObject_RotateVertices(a, b, c, d)
#define GEO_SubObject_ScaleVertices(a, b, c, d, e, f, g)
#define GEO_SubObject_VWeld(a, b)

#define GEO_SubObject_BuildEdgeData(a)
#define GEO_SubObject_TestEdge(a)
#define GEO_i_SubObject_EFind(a, b, c)	-1
#define GEO_SubObject_ETurn(a, b, c)
#define GEO_i_SubObject_ETurnSel(a, b)		0
#define GEO_i_SubObject_ECut(a, b, c, d)	0
#define GEO_i_SubObject_ECutSel(a, b, c, d) 0

#define GEO_SubObject_BuildFaceData(a)
#define GEO_SubObject_FlipNormals(a)
#define GEO_SubObject_ChangeID(a, b, c)

#define GEO_SubObject_UnselAll(a)
#endif /* ACTIVE_EDITORS */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEOSUBOBJECT_H */
