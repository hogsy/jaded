/*$T GEOobject.h GC! 1.081 01/31/02 10:35:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __GEOMETRIC_OBJECT__
#define __GEOMETRIC_OBJECT__
#include "BASe/BAStypes.h"

#include "GRObject/GROstruct.h"
#include "SOFT/SOFTstruct.h"

#if defined(_PC_RETAIL)
#include "Dx9/Dx9struct.h"
#endif	// defined(_PC_RETAIL)

#if !defined(_XENON)
#if defined(_XBOX)
#include <d3d8.h>
#endif	// defined(_XBOX)
#endif

#if defined(_XENON_RENDER_PC)
#include <d3d9.h>
#endif

//#define FASTDUPLICATE //<<<<<<<<<<<<<<<<<<<<<<<<<< TEST >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><

#ifndef PSX2_TARGET
#pragma once
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/* Chanfrein / mad, encore utilisé ?? */
#define EDGE_A		(1 << 0)
#define EDGE_B		(1 << 1)
#define EDGE_C		(1 << 2)
#define EDGE_ALL	(EDGE_A | EDGE_B | EDGE_C)
#define FACEMAP_B	(1 << 7)

/* editor flags */
#define GEO_CEF_GeomFor3DText	0x00000001

#define GSP_GEO_CacheCode	0xC0DE2004

#ifdef JADEFUSION
typedef struct GEO_tdst_TextureSpaceBasis_
{
    MATH_tdst_Vector S;
    FLOAT            BinormalDirection;
    MATH_tdst_Vector T;
    MATH_tdst_Vector SxT;
    BYTE             Active;
    BYTE             Init;
} GEO_tdst_TextureSpaceBasis;
#endif
/*$4
 ***********************************************************************************************************************
    MRM information
 ***********************************************************************************************************************
 */

/*$1- For Object ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	GEO_tdst_MRM_Object_
{
	short	*Absorbers;				/* Who goes where */
	ULONG	RealNumberOfPoints;
	ULONG	MinimumNumberOfPoints;
	short	*p_us_ReorderBuffer;	/* Reorder buffer of original object */
	float 	*df_Errors; 			// Error, by vertex, in m^2.
} GEO_tdst_MRM_Object;


/*$1- For Element ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	GEO_tdst_MRM_Element_
{
	ULONG			ul_One_UV_Per_Point_Per_Element_Base;
	ULONG			ul_RealNumberOfTriangle;
	unsigned short	*pul_Number_Of_Triangle_vs_Point_Equivalence;
} GEO_tdst_MRM_Element;

/*$4
 ***********************************************************************************************************************
    Skin
 ***********************************************************************************************************************
 */

/*$1- Per vertex ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	GEO_tdst_CompressedVertexPonderation_
{
#if defined(_GAMECUBE) || defined(_XENON)
	unsigned short	Ponderation;
	unsigned short	Index;
#else
	unsigned short	Index;
	unsigned short	Ponderation;
#endif
} GEO_tdst_CompressedVertexPonderation;

/*$1- or ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	GEO_tdst_ExpandedVertexPonderation_
{
	float	f_Ponderation;
} GEO_tdst_ExpandedVertexPonderation;

/*$1- Per Matrix ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if defined(_XBOX) || defined(_XENON_RENDER)
typedef struct	GEO_tdst_VertexPonderationList_
{
	MATH_tdst_Matrix	st_FlashedMatrix;
	unsigned short		us_IndexOfMatrix;
	unsigned short		us_NumberOfPonderatedVertices;
	union
	{
		GEO_tdst_CompressedVertexPonderation	*p_PdrtVrc_C;
		GEO_tdst_ExpandedVertexPonderation		*p_PdrtVrc_E;
	};
}__declspec(align(16)) GEO_tdst_VertexPonderationList;
#else
typedef struct	GEO_tdst_VertexPonderationList_
{
	MATH_tdst_Matrix	st_FlashedMatrix;
	unsigned short		us_IndexOfMatrix;
	unsigned short		us_NumberOfPonderatedVertices;
	union
	{
		GEO_tdst_CompressedVertexPonderation	*p_PdrtVrc_C;
		GEO_tdst_ExpandedVertexPonderation		*p_PdrtVrc_E;
	};
} GEO_tdst_VertexPonderationList;
#endif
/*$1- Per Object ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	GEO_tdst_ObjectPonderation_
{
	/* flag 0 == 1 -> Expanded ponderations */

	/* flag 1 == 1 -> Normalized Mesh */
	unsigned short						flags;
	unsigned short						NumberPdrtLists;
	GEO_tdst_VertexPonderationList		**pp_PdrtLst;
#ifdef ACTIVE_EDITORS
	struct GEO_tdst_ObjectPonderation_	*PushStack;										/* Used for computtations &
																						 * do-undo... */

	struct GEO_tdst_ObjectPonderation_	*REDOStack;
	ULONG								*dul_PointColors;								/* Used for edit colors... */
	void (*SelectionCLBK) (struct GEO_tdst_Object_ * p_stObject, void *EditorClass);	/* Used for skin editor... */
	void	*ClassPtr;
#endif

#ifdef PSX2_TARGET
	ULONG FUCKING_ANTIBUG_OF_THE_NORMALS;
#endif
} GEO_tdst_ObjectPonderation;

/*$4
 ***********************************************************************************************************************
    Strip data
 ***********************************************************************************************************************
 */

#if defined(_PC_RETAIL) || defined(_XBOX) || defined(_XENON_RENDER)
typedef struct	GEO_tdst_DrawStrip_
{
	ULONG					vertexNumber;		// Number of Vertices to draw
	ULONG					startingIndex;		// first index in Index Buffer
	ULONG					UVbufferIndex;		// index to the right UV Vertex Buffer to use
} GEO_tdst_DrawStrip;
#endif	// defined(_PC_RETAIL) || defined(_XBOX)

typedef struct	GEO_tdst_MinVertexData_
{
	unsigned short	auw_Index;
	unsigned short	auw_UV;
} GEO_tdst_MinVertexData;

typedef struct	GEO_tdst_OneStrip_
{
	ULONG					ulVertexNumber; /* Number of vertex */
	GEO_tdst_MinVertexData	*pMinVertexDataList;
#if defined(_PC_RETAIL) || defined(_XBOX) || defined(_XENON_RENDER)
	ULONG					UVbufferIndex;		// index to the right UV Vertex Buffer to use
#endif	// defined(_PC_RETAIL) || defined(_XBOX)
} GEO_tdst_OneStrip;

typedef struct	GEO_tdst_StripData_
{
	ULONG					ulFlag;			/* flag */
	ULONG					ulStripNumber;	/* number of strips */
	GEO_tdst_OneStrip		*pStripList;	/* strip list */
#ifndef ACTIVE_EDITORS
	GEO_tdst_MinVertexData	*pStripDatas;
#endif
	ULONG					ulMaxLength;
	ULONG					ulAveLength;
	ULONG					ulMinLength;
#if defined(_PC_RETAIL) || defined(_XBOX) || defined(_XENON_RENDER)
	GEO_tdst_DrawStrip		* pDrawStripList;
	ULONG					drawStripNumber;
#endif	// defined(_PC_RETAIL) || defined(_XBOX)
} GEO_tdst_StripData;

/*$4
 ***********************************************************************************************************************
    PS2 specific
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET
typedef struct	GEO_tdst_StripDataPS2_
{
	ULONG	ulNumberOfAtoms;
	USHORT	*pVertexIndexes;
	USHORT	*pVertexUVIndexes;
} GEO_tdst_StripDataPS2;

typedef struct	GSP_tdstTransfertOptimizer_
{
	/* This is used for optimise Transfert of Pack's to CPU. */

	/* Each Pack concern MaxNumT triangles; */
	unsigned short	ulNumberOfUsedIndex_GSP_PACK;
	unsigned short	*pus_ListOfUsedIndex_GSP_PACK;
	unsigned char	*SourceIndexes;
} GSP_tdstTransfertOptimizer;

#endif

/*$4
 ***********************************************************************************************************************
    mesh
 ***********************************************************************************************************************
 */

/*$1- one UV ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	GEO_tdst_UV_
{
	float	fU;
	float	fV;
} GEO_tdst_UV;

/*$1- one triangle ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	GEO_tdst_IndexedTriangle_
{
    // Index to the vertex buffer (and to the UV buffer in MRM mode)
	unsigned short	auw_Index[3];

    // In MRM mode, the vertex buffer and UV buffer have the same size and order : 
    // auw_UV is used to store the original index (full quality), 
    // while auw_Index stores the index at the current MRM quality level.
	unsigned short	auw_UV[3]; 

	ULONG			ul_SmoothingGroup;	/* because Lenght = 16 = 1 qword */
#ifndef PSX2_TARGET
#ifndef _GAMECUBE
	ULONG			ul_MaxFlags;
#endif	
#endif
} GEO_tdst_IndexedTriangle;

/*$1- one triangle element : several triangles using same material ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	GEO_tdst_ElementIndexedTriangles_
{
	LONG						l_NbTriangles;
	LONG						l_MaterialId;
	GEO_tdst_IndexedTriangle	*dst_Triangle;
	ULONG						ul_NumberOfUsedIndex;
	unsigned short				*pus_ListOfUsedIndex;
	GEO_tdst_MRM_Element		*p_MrmElementAdditionalInfo;	/* NULL MEAN NO MRM */
	GEO_tdst_StripData			*pst_StripData;
#ifdef PSX2_TARGET
	GEO_tdst_StripDataPS2		*pst_StripDataPS2;
	struct GSP_GEOBackCache_	*p_ElementCache;
#endif
#ifdef _GAMECUBE
	void						*dl;
	u32							dl_size;
/*	void						*dl_pos;
	u32							dl_pos_size;*/
#endif
	/* for DX8 version, additionnal structure */
	struct Gx8_stAddInfo		*pst_Gx8Add;
} GEO_tdst_ElementIndexedTriangles;

#define GEO_MRM_MAX_LEVEL_NB 6

typedef struct	GEO_tdst_MRM_Levels_
{
    // 2 to 6 levels. Level 0 is always the most detailled.
    ULONG ul_LevelNb; 

    // Level that is used for display. 
    ULONG ul_CurrentLevel;

    // Buffer that contains all the elements of all the levels.
    // This is the buffer used in geometry, except when displaying data.
    // It should not be freed or allocated (it is the same as the pointer in the geometry).
    GEO_tdst_ElementIndexedTriangles *a_ElementBase;

    // Elements, by level (aa_Element[i] is the array of elements at level i).
    // They are used only for rendering.
    // They should not be allocated or freed (they are only pointers on a_ElementBase).
	GEO_tdst_ElementIndexedTriangles *aa_Element[GEO_MRM_MAX_LEVEL_NB];

    // Element nb, by level.
    int dl_ElementNb[GEO_MRM_MAX_LEVEL_NB];
    
    // Point nb, by level.
    int dl_PointNb[GEO_MRM_MAX_LEVEL_NB];

    // UV nb, by level.
    int dl_UVNb[GEO_MRM_MAX_LEVEL_NB];

    // Total element nb.
    int l_TotalElementNb;
    
	// Total point nb    
    int l_TotalPointNb;
    
	// Total UV nb
    int l_TotalUVNb;

    // importance_on_screen *= f_DistanceCoef; (if the Bounding Box is bigger than the object)
    float   f_DistanceCoef;         

    // f_Thresholds is used to choose the level from the importance on screen.
    // f_Thresholds[i] is used for transition between dst_Element[i] and dst_Element[i+1] :
    // If f_Thresholds[i] > fQuality > f_Thresholds[i+1], then ul_CurrentLevel = i+1
    // On the X axis in the editor 
    float f_Thresholds[GEO_MRM_MAX_LEVEL_NB-1]; 

#ifdef ACTIVE_EDITORS
    // Percentage of vertices at each level : f_MRMLevels[i] = nb_vertices_level[i+1] / nb_vertices_level[0]
    // On the Y axis in the editor 
    float f_MRMLevels[GEO_MRM_MAX_LEVEL_NB-1];
#endif //ACTIVE_EDITORS

    // Reorder buffer used when saving/loading RLIs in non-binarized mode.
    // Index_with_MRM_Levels = dus_ReorderBuffer[Index_Origin]. NULL when binarized.
    unsigned short *dus_ReorderBuffer; 

} GEO_tdst_MRM_Levels;

/*$1- one sprite (Same principe as skin structure (float *)GEO_tdst_IndexedSprite ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	GEO_tdst_IndexedSprite_
{
#if defined(_GAMECUBE) || defined(_XENON)
	unsigned short	fSize;
	unsigned short	auw_Index;
#else
	unsigned short	auw_Index;
	unsigned short	fSize;
#endif
} GEO_tdst_IndexedSprite;

/*$1- one sprite element : several sprites sharing material and ratio ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	GEO_tdst_ElementIndexedSprite_
{
	LONG					l_NbSprites;
	LONG					l_MaterialId;
	GEO_tdst_IndexedSprite	*dst_Sprite;
	ULONG					ul_NumberOfUsedIndex;
	unsigned short			*pus_ListOfUsedIndex;
	float					fGlobalSize;
	float					fGlobalRatio;	/* 0.0f mean ratio of 1.0f */
} GEO_tdst_ElementIndexedSprite;

#if defined( _M_IX86 ) || defined( _XENON_RENDER )
typedef struct _CUSTOM_NORMAL
{
	float x,y,z,w;
}__declspec(align(16))_Custom_Normal_;
#endif

/*$4
 ***********************************************************************************************************************
    main structure
 ***********************************************************************************************************************
 */

typedef struct	GEO_tdst_Object_
{
	GRO_tdst_Struct						st_Id;
	LONG								l_NbPoints;
	GEO_Vertex							*dst_Point;
	MATH_tdst_Vector					*dst_PointNormal;
#ifdef PSX2_TARGET	
	ULONG								*p_CompressedNormals;
#endif	
    // This is the default geometry vertex color (can be overridden by the vertex color in the instance (dul_VertexColors of the rli))
    // WARNING : The first element is the size of the array !!!
	ULONG								*dul_PointColors;
	LONG								l_NbUVs;
	GEO_tdst_UV							*dst_UV;
	LONG								l_NbElements;
	GEO_tdst_ElementIndexedTriangles	*dst_Element;
	void								*pst_BV;
	struct GEO_tdst_MRM_Object_			*p_MRM_ObjectAdditionalInfo;	/* NULL MEAN NO MRM */
	struct GEO_tdst_ObjectPonderation_	*p_SKN_Objectponderation;
	GEO_tdst_MRM_Levels			        *p_MRM_Levels;	// NULL means no MRM Levels.

#ifdef ACTIVE_EDITORS
	struct GEO_tdst_SubObject_			*pst_SubObject;
	ULONG								ul_EditorFlags;
#endif /* ACTIVE_EDITORS */

	ULONG								ulStripFlag;

	LONG								l_NbSpritesElements;
	GEO_tdst_ElementIndexedSprite		*dst_SpritesElements;
	struct COL_tdst_OK3_				*pst_OK3;

#ifdef _XBOX

	//CARLONE
	//Tell if color are already filtered for PS2->XBOX
	int colorAlreadyFiltered;
	// Yoann Tangente
	SOFT_tdst_AVertex *CotangantesU;
	MATH_tdst_Vector *Binormales;//test
#endif

/*#ifdef FASTDUPLICATE 
	int	NbDuplicate;
	MATH_tdst_Vector *Pos; //essai juste avec un vecteur de position pour commencer ?!?!!??
#endif*/
	int	NbDuplicate;

#if defined(_PC_RETAIL)
	IDirect3DVertexBuffer9				* pVB_Position;			// VB that holds positions of every vertex
	IDirect3DVertexBuffer9				* pVB_Normal;			// VB that holds normals of every vertex
	IDirect3DVertexBuffer9				* pVB_Color;			// VB that holds colors of every vertex

	ULONG								UVbufferNumber;			// number of UV buffers in the list
	IDirect3DVertexBuffer9				* * pVBlist_UV;			// List of VBs that hold tex coords of every vertex

	IDirect3DIndexBuffer9				* pIB;					// Index Buffer that holds every element and evey strip
#endif // defined(_PC_RETAIL)

#if defined(_XBOX)
	IDirect3DVertexBuffer8				* pVB_Position;			// VB that holds positions of every vertex
	IDirect3DVertexBuffer8				* pVB_Normal;			// VB that holds normals of every vertex
	IDirect3DVertexBuffer8				* pVB_Color;			// VB that holds colors of every vertex
	IDirect3DVertexBuffer8				* pVB_Tangente;			// VB that holds Tangente of every vertex
	IDirect3DVertexBuffer8				* pVB_Binormales;			// VB that holds Tangente of every vertex

	ULONG								UVbufferNumber;			// number of UV buffers in the list
	IDirect3DVertexBuffer8				* * pVBlist_UV;			// List of VBs that hold tex coords of every vertex

	IDirect3DIndexBuffer8				* pIB;					// Index Buffer that holds every element and evey strip
#endif // defined(_XBOX)

	ULONG								*pBiNormales;

	_Custom_Normal_ *dst_OriginalPointNormal;

#ifdef _XENON_RENDER
    BOOL                                b_ForceSoftSkinning;
    BOOL                                b_CreateMorphedMesh;
    BOOL                                b_Particles;
    void*                               m_pWYB1;
    void*                               m_pWYB2;
#endif

} GEO_tdst_Object;


/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

#ifdef MEM_OPT 
#ifdef MEM_SPY 

extern void *_MEM_GEO_p_Alloc(MEM_tdst_MainStruct *_pMem,ULONG MemSize, char *_sFile, int _lLine);
extern void *_MEM_GEO_p_AllocAlign(MEM_tdst_MainStruct *_pMem,ULONG MemSize , ULONG Align , char *_sFile, int _lLine) ;


#define MEM_GEO_p_Alloc(a) _MEM_GEO_p_Alloc(&MEM_gst_MemoryInfo,a, __FILE__, __LINE__)
#define MEM_GEO_p_AllocAlign(a,b) _MEM_GEO_p_AllocAlign(&MEM_gst_MemoryInfo,a,b, __FILE__, __LINE__)
#ifdef _GAMECUBE
#ifndef _RVL
	#define MEM_GEO_p_VMAlloc(a) _MEM_GEO_p_Alloc(&MEM_gst_VirtualMemoryInfo, a,__FILE__, __LINE__)
	#define MEM_GEO_p_VMAllocAlign(a,b) _MEM_GEO_p_AllocAlign(&MEM_gst_VirtualMemoryInfo,a,b, __FILE__, __LINE__)
#else
	#define MEM_GEO_p_VMAlloc(a) _MEM_GEO_p_Alloc(&MEM_gst_GCMemoryInfo, a,__FILE__, __LINE__)
	#define MEM_GEO_p_VMAllocAlign(a,b) _MEM_GEO_p_AllocAlign(&MEM_gst_GCMemoryInfo,a,b, __FILE__, __LINE__)
#endif 
#else // _GAMECUBE
	#define MEM_GEO_p_VMAlloc(a) _MEM_GEO_p_Alloc(&MEM_gst_MemoryInfo, a,__FILE__, __LINE__)
	#define MEM_GEO_p_VMAllocAlign(a,b) _MEM_GEO_p_AllocAlign(&MEM_gst_MemoryInfo,a,b, __FILE__, __LINE__)
#endif // _GAMECUBE

#else // MEM_SPY 

extern void *_MEM_GEO_p_Alloc(MEM_tdst_MainStruct *_pMem,ULONG MemSize);
extern void *_MEM_GEO_p_AllocAlign(MEM_tdst_MainStruct *_pMem,ULONG MemSize , ULONG Align );
#define MEM_GEO_p_Alloc(a) _MEM_GEO_p_Alloc(&MEM_gst_MemoryInfo, a)
#define MEM_GEO_p_AllocAlign(a,b) _MEM_GEO_p_AllocAlign(&MEM_gst_MemoryInfo, a,b)
#ifdef _GAMECUBE
#ifndef _RVL
	#define MEM_GEO_p_VMAlloc(a) _MEM_GEO_p_Alloc(&MEM_gst_VirtualMemoryInfo,a)
	#define MEM_GEO_p_VMAllocAlign(a,b) _MEM_GEO_p_AllocAlign(&MEM_gst_VirtualMemoryInfo, a,b)
#else
	#define MEM_GEO_p_VMAlloc(a) _MEM_GEO_p_Alloc(&MEM_gst_GCMemoryInfo,a)
	#define MEM_GEO_p_VMAllocAlign(a,b) _MEM_GEO_p_AllocAlign(&MEM_gst_GCMemoryInfo, a,b)
#endif // _RVL
#else // _GAMECUBE
	#define MEM_GEO_p_VMAlloc(a) _MEM_GEO_p_Alloc(&MEM_gst_MemoryInfo,a)
	#define MEM_GEO_p_VMAllocAlign(a,b) _MEM_GEO_p_AllocAlign(&MEM_gst_MemoryInfo, a,b)
#endif // _GAMECUBE

#endif //  MEM_SPY 
#else // MEM_OPT
extern void *MEM_GEO_p_Alloc(ULONG MemSize );
extern void *MEM_GEO_p_AllocAlign(ULONG MemSize , ULONG Align );
#define MEM_GEO_p_VMAlloc(a) MEM_GEO_p_Alloc(a)
#define MEM_GEO_p_VMAllocAlign(a,b) MEM_GEO_p_AllocAlign(a,b)
#endif // MEM_OPT

#ifdef JADEFUSION
extern void		MEM_GEO_v_FreeAlign(void *ptr );
#endif

extern void     GEO_FreeElementContent(GEO_tdst_ElementIndexedTriangles *_pst_Element);
extern void		GEO_AllocElementContent(GEO_tdst_ElementIndexedTriangles *);
extern void		GEO_AllocContent(GEO_tdst_Object *);
GEO_tdst_Object *GEO_pst_Create(LONG, LONG, LONG, LONG);
extern void		GEO_Free(GEO_tdst_Object *);
extern void		GEO_Init(void);
extern void		GEO_I_Need_The_Triangles_Begin(GEO_tdst_Object *,ULONG FromEnd);
extern void		GEO_I_Need_The_Triangles_End(GEO_tdst_Object *,ULONG FromEnd);
extern void 	GEO_I_Need_The_Triangles_Begin_Element(GEO_tdst_ElementIndexedTriangles    *pst_Element,ULONG FromEnd);
extern void 	GEO_I_Need_The_Triangles_End_Element(GEO_tdst_ElementIndexedTriangles    *pst_Element,ULONG FromEnd);
extern void		GEO_ComputeBiNormals(GEO_tdst_Object *_pst_Object,ULONG *pDest);
extern struct MAT_tdst_Material_ *GEO_GetMaterial(struct MAT_tdst_Material_ *, ULONG );

#if defined(_XBOX) || defined(_XENON_RENDER)
void GEO_FilterColorLuminanceCrominace(DWORD *pColors);
#endif
	// GEO_UseNormals allocates memory and computes normals if needed
//#ifdef _XBOX
//#define			GEO_UseNormals(a) ((void)(a))
//#else
extern void		GEO_UseNormals(GEO_tdst_Object *);
//#endif

#if defined(_XBOX) || defined(_XENON_RENDER)
void GEO_FilterColorLuminanceCrominace(DWORD *pColors);
#endif

#ifdef ACTIVE_EDITORS
extern void		GEO_AdjustForSymetrie(GEO_tdst_Object *);
extern void		GEO_ResetRLI(GEO_tdst_Object *, ULONG);
extern void		GEO_DestroyRLI(GEO_tdst_Object *);
extern void		GEO_RLIAlphaToColor(ULONG *);
extern void		GEO_RLIInvertAlpha(ULONG *);
extern void		GEO_ApplyTransfoToVertices(GEO_tdst_Object *, MATH_tdst_Matrix *);
#endif

#if defined(_XENON_RENDER)
void GEO_CreateXenonMesh(OBJ_tdst_GameObject *_pst_GO, GRO_tdst_Visu *_pst_Visu, GEO_tdst_Object *_pst_Object, BOOL _bForceRecomputeTangentSpace = FALSE, BOOL _b_PreserveColors = FALSE);
void GEO_CreateXenonDynamicMesh(GRO_tdst_Visu *_pst_Visu, GEO_tdst_Object *_pst_Object);
void GEO_ClearXenonMesh(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Object, BOOL _b_PreserveColors = FALSE, BOOL _b_KeepTangents = FALSE);
void GEO_ResetXenonMesh(OBJ_tdst_GameObject *_pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, BOOL _b_PreserveColors, BOOL _b_InvalidateVertexCount, BOOL _b_IgnoreTangentSpace);
void GEO_ResetAllXenonMesh(BOOL _b_PreserveColors, BOOL _b_InvalidateVertexCount, BOOL _b_IgnoreTangentSpace);

void GEO_CreateMorphedXenonMesh( GRO_tdst_Visu * _pst_Visu, GEO_tdst_Object *_pst_Object );

#if defined(ACTIVE_EDITORS)
void *GEO_p_Duplicate(GEO_tdst_Object *_pst_Geo, char *_asz_Path, char *_sz_Name, ULONG _ul_Flags);

#endif
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEOMETRIC_OBJECT__ */

