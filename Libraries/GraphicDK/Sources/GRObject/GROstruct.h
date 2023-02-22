/*$T GROstruct.h GC! 1.075 03/06/00 14:42:22 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GROSTRUCT_H__
#define __GROSTRUCT_H__

#include "TABles/TABles.h"
#include "BASe/BAStypes.h"

#include "LIGHT/LIGHTmapstruct.h"
//#include <set>

struct OBJ_tdst_GameObject_;

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/* constants for GRO type */
#define GRO_Unknown                 0
#define GRO_Geometric               1
#define GRO_Light                   2
#define GRO_MaterialSingle          3
#define GRO_MaterialMulti           4
#define GRO_MaterialMultiTexture    5
#define GRO_Camera                  6
#define GRO_Waypoint                7
#define GRO_GeoStaticLOD            8
#define GRO_Unused                  9
#define GRO_2DText                  10
#define GRO_ParticleGenerator       11

#define GRO_Cl_NumberOfInterfaces   12

/*constants for display position */
#define GRO_DisplayPos_Normal       0
#define GRO_DisplayPos_Interface    1   

#ifdef JADEFUSION
// Xenon mesh processing flags
#define GRO_XMPF_EnableTessellation     0x00000001
#define GRO_XMPF_EnableDisplacement     0x00000002
#define GRO_XMPF_EnableChamfer          0x00000004
#define GRO_XMPF_IgnoreWeights          0x00000008
#define GRO_XMPF_SafeMode               0x00000010
#define GRO_XMPF_SmoothGroupMask        0x00ff0000
#define GRO_XMPF_SmoothGroupShift       16
#define GRO_XMPF_ConsistencyFailed      0x80000000
#endif
/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

struct GRO_tdst_Struct_;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct GRO_tdst_Interface_
{
    ULONG   ul_Type;

    void * (*pfnp_CreateDefault) (void);
    void * (*pfnp_CreateFromBuffer) (struct GRO_tdst_Struct_ *, char **, void *);
    void * (*pfnp_Duplicate) (void *, char *, char *, ULONG);
    void (*pfn_Destroy) (void *);
    
    LONG (*pfnl_HasSomethingToRender) (void *, void *);
    void (*pfn_Render) (void *);

    void (*pfn_AddRef) (void *, LONG);
    void (*pfn_Reinit) (struct GRO_tdst_Struct_ *);

    ULONG (*pfn_User_0) (struct GRO_tdst_Struct_ * , ULONG P0); 

#ifdef ACTIVE_EDITORS
    LONG (*pfnl_SaveInBuffer) (void *, void *);
    void * (*pfnp_CreateFromMad) (void *);
    void * (*pfnp_ToMad) (void *, void *);
    LONG (*pfnl_PushSpecialMatrix) (void *);
    char * (*pfnsz_FileExtension) (void);
#endif
}
GRO_tdst_Interface;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct  GRO_tdst_Struct_
{
    GRO_tdst_Interface  *i;
    LONG                l_Ref;
#ifdef ACTIVE_EDITORS
    char                *sz_Name;
#endif
}
GRO_tdst_Struct;

/*$1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#ifdef JADEFUSION
struct GEO_tdst_TextureSpaceBasis_;

#if defined(_XENON_RENDER)
typedef struct GRO_tdst_XeElement_
{
    class XeMesh*        pst_Mesh;
    class XeIndexBuffer* pst_IndexBuffer;
    MATH_tdst_Vector     st_AABBMin;
    MATH_tdst_Vector     st_AABBMax;

    GEO_tdst_TextureSpaceBasis_ * dst_TangentSpace;

#if defined(ACTIVE_EDITORS)
    USHORT*              puw_Indices;
#endif
} GRO_tdst_XeElement;
#endif
#endif
/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct  GRO_tdst_Visu_
{
    GRO_tdst_Struct             *pst_Object;
    GRO_tdst_Struct             *pst_Material;
    ULONG                       ul_DrawMask;
    // This is the instance (rli) vertex color (overrides by the default vertex color of the geometry (dul_PointColors))
    // WARNING : The first element is the size of the array !!!
    ULONG                       *dul_VertexColors;
    char                        c_DisplayPos;
    char                        c_CullingMask;
    unsigned char				c_LightAttenuation;
    char						c_DisplayOrder;

#ifdef JADEFUSION
	#define GRO_VISU_FLAG_WATERFX		        0x01
    #define GRO_VISU_FLAG_SYMMETRY              0x02
    #define GRO_VISU_FLAG_HIDE_MESH             0x04
    unsigned char				ucFlag;
#endif

#if defined(_XBOX)
	//Tell if color are already filtered for PS2->XBOX
	int colorAlreadyFiltered;
	int	IndexInMultipleVBList;

	//tell if already rendered in this frame
	int playSkinning;

#endif

#ifdef _XENON_RENDER
	ULONG                    ul_VBObjectValidate;
	LONG                     l_NbXeElements;
	GRO_tdst_XeElement*      p_XeElements;
	LONG                     l_VBVertexCount;
	class XeBuffer*          p_VBVertex;
    class XeBuffer*          p_VBFurOffsets;
    struct FurOffsetVertex_* p_FurOffsetVertex;
    class XeBuffer *         pVB_MorphStatic;
    class XeBuffer *         pVB_MorphDynamic;
    ULONG                    ul_PackedDataKey;

    std::set< OBJ_tdst_GameObject_ * > * pRejectedLights;
#endif

    // lightmap support
    float**						pp_st_LightmapCoords;	// lightmap data is per Game Object, contains an array of pointers to UV*, one pointer per element of the object
    USHORT						usNbElements;
    USHORT*						p_us_NbTrianglesInElement;	// juste so we can make sure that the object hasn't changed
    LIGHT_tdst_LightmapPageInfo	*pLMPage;	

#ifdef ACTIVE_EDITORS	
    LIGHT_tdst_LightmapSettings pst_LightmapSettings;
#endif

#if defined(ACTIVE_EDITORS)
    // Xenon automatic mesh processing
    ULONG ul_XenonMeshProcessingFlags;
    ULONG ul_XenonMeshProcessingCRC;
    ULONG ul_XenonMeshProcessingRLIKey;
#endif
} GRO_tdst_Visu;

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

extern GRO_tdst_Interface   GRO_gast_Interface[GRO_Cl_NumberOfInterfaces];
#ifdef ACTIVE_EDITORS
extern char                 *GRO_gasz_InterfaceName[GRO_Cl_NumberOfInterfaces];
#endif /* ACTIVE_EDITORS */

extern GRO_tdst_Struct      GRO_gst_Unknown;

/*$4
 ***********************************************************************************************************************
    Interface functions
 ***********************************************************************************************************************
 */

#if (defined(PSX2_TARGET) || defined(_GAMECUBE))
struct OBJ_tdst_GameObject_;
struct GDI_tdst_DisplayData_;
extern LONG GRO_PushSpecialMatrixForProportionnal(struct OBJ_tdst_GameObject_ *);
#elif defined (JADEFUSION)
struct OBJ_tdst_GameObject_;
extern LONG GRO_PushSpecialMatrixForProportionnal(struct OBJ_tdst_GameObject_ *_pst_Node);
#else
extern LONG GRO_PushSpecialMatrixForProportionnal(void *);
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

extern GRO_tdst_Struct *GRO_pst_Struct_Create(LONG);
extern void GRO_Struct_Init(GRO_tdst_Struct *, LONG);
extern void GRO_Struct_Free(GRO_tdst_Struct *);
extern LONG GRO_l_Struct_Load(GRO_tdst_Struct *, char *);
extern void GRO_Struct_InitInterfaces(void);
extern void GRO_Struct_DestroyTable(TAB_tdst_Ptable *, int );

#ifdef ACTIVE_EDITORS
extern void     GRO_Struct_SetName(GRO_tdst_Struct *, char *);
extern char *   GRO_sz_Struct_GetName(GRO_tdst_Struct *);
extern void     GRO_Struct_Save(GRO_tdst_Struct *);
extern ULONG    GRO_ul_Struct_FullSave(GRO_tdst_Struct *, char *, char *, void *);
extern BOOL     GRO_b_IsAMaterial( GRO_tdst_Struct * );

#else

#define GRO_Struct_SetName(a, b)
#define GRO_sz_Struct_GetName(a) NULL
#define GRO_Struct_Save(a)
#define GRO_ul_Struct_FullSave(a, b, c, d)  0xFFFFFFFF
#define GRO_b_IsAMaterial( a ) 0

#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GROSTRUCT_H__ */
