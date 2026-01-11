/*$T MAD_Struct_V0.h GC!1.32 10/20/99 15:09:15 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include <STDIO.H>
#define MAD_NULL_INDEX              (unsigned long) 0xffffffff
#define MAX_MAX_Mtl_Per_OBJECT      1024

#define ID_MAD_Type						unsigned long
#define ID_MAD_UNDEFINED				0xC0FDFDFD
#define ID_MAD_World					0
#define ID_MAD_Matrix					1
#define ID_MAD_Material					2
#define ID_MAD_Texture					3
#define ID_MAD_GeometricObject_V0		4
#define ID_MAD_Light					5
#define ID_MAD_Camera					6
#define ID_MAD_WorldNode				7
#define ID_MAD_TargetObject				8
#define ID_MAD_Shape					9
#define ID_MAD_Dummy					10
#define ID_MAD_SubFile					11
#define ID_MAD_GeometricObjectAddOn		12
#define ID_MAD_GeometricObjectLOD		13
#define ID_MAD_SkinnedGeometricObject	14

#ifdef JADEFUSION
#define ID_MAD_Type_Mask            0xC0FDFDFF
#define ID_MAD_DontImport           0x01000000  // Flag to not import
#endif

#define MAD_NAME_LENGHT             256


typedef unsigned long MAD_ColorARGB;
#define MAD_GetRed(CARGB)		(((CARGB) & 0xff0000) >> 16)
#define MAD_GetGreen(CARGB)		(((CARGB) & 0xff00) >> 8)
#define MAD_GetBlue(CARGB)		(((CARGB) & 0xff) >> 0)
#define MAD_GetAlpha(CARGB)		(((CARGB) & 0xff000000) >> 24)
#define MAD_SetRed(CARGB,Val) (CARGB) = ((CARGB) & 0xFF00ffff) | ((Val & 0xff) << 16)
#define MAD_SetGreen(CARGB,Val) (CARGB) = ((CARGB) & 0xFFff00ff) | ((Val & 0xff) << 8)
#define MAD_SetBlue(CARGB,Val) (CARGB) = ((CARGB) & 0xFFffff00) | ((Val & 0xff) << 0)
#define MAD_SetAlpha(CARGB,Val) (CARGB) = ((CARGB) & 0x00ffffff) | ((Val & 0xff) << 24)

#define MAD_ColorARGB_to_MAD_Vertex(MV,CARGB)\
{\
	(MV) . x = (float)MAD_GetRed(CARGB);\
	(MV) . y = (float)MAD_GetGreen(CARGB);\
	(MV) . z = (float)MAD_GetBlue(CARGB);\
	((unsigned char *)(&MV))[0] = (char)MAD_GetAlpha(CARGB);\
}

#define MAD_Vertex_to_MAD_ColorARGB(MV,CARGB)\
{\
	MAD_SetRed(CARGB,(unsigned long)(MV . x));\
	MAD_SetGreen(CARGB,(unsigned long)(MV . y));\
	MAD_SetBlue(CARGB,(unsigned long)(MV . z));\
	MAD_SetAlpha(CARGB,(unsigned long)((unsigned char *)(&MV.z))[0]);\
}

typedef struct          MAD_Vertex_
{
    float   x, y, z;
} MAD_Vertex;

typedef struct  MAD_Simple3DVertex_
{
    MAD_Vertex  Point;
    MAD_Vertex  Normale;
} MAD_Simple3DVertex;

typedef struct  MAD_NodeID_
{
    ID_MAD_Type     IDType;
    unsigned long   SizeOfThisOne;
    char            Name[MAD_NAME_LENGHT];

#ifdef JADEFUSION
#ifdef __cplusplus
    ID_MAD_Type GetIDType() const {return IDType&ID_MAD_Type_Mask;}
    void SetIDType(ID_MAD_Type type) {IDType = type;}
    bool DontImport() const {return (IDType&ID_MAD_DontImport) != 0;}
    void SetDontImportFlag() {IDType |= ID_MAD_DontImport;}
    void ClearDontImportFlag() {IDType &= ~ID_MAD_DontImport;}
#endif // __cplusplus
#endif
} MAD_NodeID;

typedef struct  MAD_Undefined_
{
    MAD_NodeID  ID;
} MAD_Undefined;

typedef struct  MAD_Matrix_
{
    MAD_NodeID  ID;                     /* Must stay the first ! */
    MAD_Vertex  Translation;
    MAD_Vertex  I, J, K;
} MAD_Matrix;

/*$5
 ###################################################################################################
    Textures
 ###################################################################################################
 */

typedef struct  MAD_texture_
{
    MAD_NodeID  ID;                     /* Must stay the first ! */
    char        Texturefile[260];       /* _MAX_PATH (NT maximum name lenght) */
} MAD_texture;

/*$5
 ###################################################################################################
    Materials
 ###################################################################################################
 */

#define MAD_MATERIALTYPE        unsigned long
#define ID_MAT_NullMat          0
#define ID_MAT_Standard         1
#define ID_MAT_MultiMaterial    2
#define ID_MAT_MultiTexMaterial	3
#ifdef JADEFUSION
#define ID_MAD_MaterialType_Mask    0x0000ffff
#define ID_MAD_MaterialDontImport   0x01000000  // Flag to not import
#endif
/*
 ---------------------------------------------------------------------------------------------------
    Simples materials
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  MAD_MAT_MatID_
{
    MAD_MATERIALTYPE    MaterialType;   /* Must stay the first ! */
    char                Name[MAD_NAME_LENGHT];

#ifdef JADEFUSION
#ifdef __cplusplus
    MAD_MATERIALTYPE GetMaterialType() const {return MaterialType&ID_MAD_MaterialType_Mask;}
    void SetMaterialType(MAD_MATERIALTYPE type) {MaterialType = type;}
    bool DontImport() const {return (MaterialType&ID_MAD_MaterialDontImport) != 0;}
    void SetDontImportFlag() {MaterialType |= ID_MAD_MaterialDontImport;}
    void ClearDontImportFlag() {MaterialType &= ~ID_MAD_MaterialDontImport;}
#endif // __cplusplus
#endif

} MAD_MAT_MatID;

#define MAD_MTF_TransparencyModeMask    0xF
#define MAD_MTF_Trans_Copy              0x0
#define MAD_MTF_Trans_Add               0x1
#define MAD_MTF_Trans_AddSmooth         0x2
#define MAD_MTF_Trans_Alpha             0x3
#define MAD_MTF_Trans_AlphaPremult      0x4
#define MAD_MTF_Trans_Invert            0x5
#define MAD_MTF_Trans_Mul               0x6
#define MAD_MTF_Trans_Mul2X             0x7
#define MAD_MTF_UTiling                 0x10
#define MAD_MTF_VTiling                 0x20
#define MAD_MTF_UMirror                 0x40
#define MAD_MTF_VMirror                 0x80
#define MAD_MTF_AlphaThreshold          0x100
#define MAD_MTF_InvertColor             0x200
#define MAD_MTF_PerspectivCorrection    0x400

typedef struct  MAD_StandarMaterial_
{
    MAD_MAT_MatID   MatRef;             /* Must stay the first ! */
    MAD_ColorARGB   Diffuse;
    MAD_ColorARGB   Ambient;
    MAD_ColorARGB   Specular;
    float           SelfIllum;
    float           Opacity;
    unsigned long   MaterialFlag;
    unsigned long   MadTexture;         /* Index in **AllTexture - MAD_NULL_INDEX mean no texture */
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_StandarMaterial;

/*
 ---------------------------------------------------------------------------------------------------
    Complex materials
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  MAD_MultiMaterial_
{
    MAD_MAT_MatID   MatRef;             /* Must stay the first ! */
    unsigned long   NumberOfSubMaterials;
    unsigned long   *SubMats;           /* Index in AllMaterial */
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_MultiMaterial;

/*
 ---------------------------------------------------------------------------------------------------
    Multi textures materials
 ---------------------------------------------------------------------------------------------------
 */
typedef struct	MAD_MTLevel_
{
	short					s_TextureId; /* Index in AllTextures */
	unsigned long			ul_Flags;
} MAD_MTLevel;

typedef struct  MAD_MultiTexMaterial_
{
    MAD_MAT_MatID   MatRef;             /* Must stay the first ! */
	MAD_ColorARGB   Diffuse;
    MAD_ColorARGB   Ambient;
    MAD_ColorARGB   Specular;
    float           SelfIllum;
    float           Opacity;
    unsigned long   MaterialFlag;
    unsigned long   NumberOfLevels;
    MAD_MTLevel		*AllLevels;           /* Array of MAD_MTLevel */
    unsigned long   *Undefined0;
    unsigned long   *Undefined1;
    unsigned long   *Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_MultiTexMaterial;

/*$5
 ###################################################################################################
    Light
 ###################################################################################################
 */

#define MAD_LightType               unsigned long
#define MAD_LightOmni               0
#define MAD_LightSpot               1
#define MAD_LightDirect             2

#define MAD_LightFlag               unsigned long
#define MAD_LightFlag_CastShadow    1
#define MAD_LightFlag_PaintLight    2
#define MAD_LightFlag_OverWalls     4

typedef struct  MAD_tdst_SpotLight_
{
    float   LittleAlpha;
    float   BigAlpha;
    float   Near;
    float   Far;
    float   RadiusOfLight;
} MAD_tdst_SpotLight;

typedef struct  MAD_tdst_DirectLight_
{
    float   Near;
    float   Far;
    float   RadiusOfLight;
} MAD_tdst_DirectLight;

typedef struct  MAD_tdst_OmniLight_
{
    float   Near;
    float   Far;
    float   RadiusOfLight;
} MAD_tdst_OmniLight;

typedef struct  MAD_Light_
{
    MAD_NodeID      ID;                 /* Must stay the first ! */
    MAD_LightType   LightType;          /* MAD_OMNI_LIGHT MAD_TSPOT_LIGHT MAD_DIR_LIGHT */
    MAD_ColorARGB   LightColor;
    int             IsOn;
    union
    {
        MAD_tdst_OmniLight      Omni;
        MAD_tdst_DirectLight    Direct;
        MAD_tdst_SpotLight      Spot;
    };
    MAD_LightFlag   LightFlags;
    float           Multiplier;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
} MAD_Light;

/*$5
 ###################################################################################################
    Camera
 ###################################################################################################
 */

#define MAD_CameraType      unsigned long
#define MAD_e_OrthoCamera   0
#define MAD_e_PerspCamera   1

typedef struct  MAD_OrthoCamera_
{
    float   ChannelSize;
    float   Znear;
    float   Zfar;
} MAD_OrthoCamera;

typedef struct  MAD_PerspCamera_
{
    float   Fov;
    float   Znear;
    float   Zfar;
} MAD_PerspCamera;

typedef struct  MAD_Camera_
{
    MAD_NodeID      ID; /* Must stay the first ! */
    MAD_CameraType  CameraType;
    union
    {
        MAD_OrthoCamera OrthoInf;
        MAD_PerspCamera PerspInf;
    };
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_Camera;

/*$5
 ###################################################################################################
    Shape
 ###################################################################################################
 */

#define MAD_SHAPETYPE       unsigned long
#define ID_MAT_SPLINE3D     0
#define ID_MAT_NURB         1

#define MAD_SHAPEKNOTTYPE   unsigned long
#define ID_MAT_SMOOTH       0
#define ID_MAT_CORNER       1
#define ID_MAT_BEZIER       2
#define ID_MAT_BEZIERCORNER 3

typedef struct  MAD_SplineKnot_
{
    MAD_SHAPEKNOTTYPE   KnotType;
    MAD_Vertex          ControlPoint;
    MAD_Vertex          PreviousTangent;
    MAD_Vertex          NexTangent;
} MAD_SplineKnot;

typedef struct  NurbKnot_
{
    float       Weight;
    MAD_Vertex  ControlPoint;
} NurbKnot;

typedef struct  MAD_Spline3D_
{
    unsigned long   NumberOfKnots;
    MAD_SplineKnot  *Knots;
} MAD_Spline3D;

typedef struct  Nurb_
{
    unsigned long   Degree;
    unsigned long   NumberOfKnots;
    NurbKnot        *Knots;
} Nurb;

typedef struct  MAD_Shape_
{
    MAD_NodeID      ID; /* Must stay the first ! */
    MAD_SHAPETYPE   TypeOfShape;
    unsigned long   ShapeClosed;
    union
    {
        MAD_Spline3D    Spline;
        Nurb            Nurb;
    };
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_Shape;

/*$5
 ###################################################################################################
    Dummy
 ###################################################################################################
 */

#define MAD_DUMMYTYPE   unsigned long
#define DUMMY_POINT     1
#define DUMMY_BOX       2
#define DUMMY_BONE      3
typedef struct  MAD_Dummy_
{
    MAD_NodeID      ID;         /* Must stay the first ! */
    MAD_DUMMYTYPE   Type;
    MAD_Vertex      BOXMin;
    MAD_Vertex      BOXMax;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_Dummy;

/*$5
 ###################################################################################################
    Target
 ###################################################################################################
 */

typedef struct  MAD_Target_
{
    MAD_NodeID      ID;         /* Must stay the first ! */
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_Target;

/*$5
 ###################################################################################################
    Geometric Objects
 ###################################################################################################
 */

#define MAD_GEOMETRICOBJECTTYPE unsigned long
#define MAD_MAXMESH_V0          0

typedef struct  MAD_Face_
{
    unsigned long   Index[3];
    unsigned long   UVIndex[3];
    unsigned long   SmoothingGroup;
    unsigned long   MAXflags;   /* Edge Visibility, Material ID , */
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
} MAD_Face;

typedef struct  MAD_GeometricObjectElement_
{
    unsigned long   MaterialID; /* This index is used only if the mad_node contain a multimaterial
                                 * object */
    unsigned long   NumberOfTriangles;
    MAD_Face        *Faces;
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   NumberOfUsedIndex;
    unsigned long   *UsedIndex;
    unsigned long   OneUVPerVertexPerElementBase;

    /* MRM info */
    union
    {
        unsigned long   Undefined5;
        unsigned long   TotalNumberOfTriangles;
    };
    union
    {
        unsigned long   Undefined6;
        MAD_Face        *FacesIIndexedOnIndex2;
    };
}
MAD_GeometricObjectElement;

#define GO_Flags        unsigned long
#define GO_ShadowCut    1
#define GO_NonMagnifold 2
#ifdef JADEFUSION
#define GO_IsCollisionObject    0x00000010
#endif

typedef struct  MAD_GeometricObject_
{
    MAD_NodeID                  ID;                 /* Must stay the first ! */
    MAD_GEOMETRICOBJECTTYPE     TypeOfThis;         /* To permit Evolutivity */
    unsigned long               NumberOfPoints;
    MAD_Simple3DVertex          *OBJ_PointList;
    unsigned long               NumberOfUV;
    MAD_Vertex                  *OBJ_UVtextureList; /* X = u , y = v , z = W */
    unsigned long               NumberOfElements;
    MAD_GeometricObjectElement  **Elements;
    MAD_ColorARGB				*SecondRLIField; /* This is used for LOD. This is the original rli fields*/
    union
    {
        unsigned long   Undefined1;
        unsigned long   *Absorbers;
    };
    union
    {
        unsigned long   Undefined2;
        unsigned long   *Index2;
    };
    union
    {
        unsigned long   Undefined3;
        unsigned long   RealNumberOfPoints;
    };
    union
    {
        unsigned long   Undefined4;
        unsigned long   NumberOfPointsWithoutRadiosity;
    };
    union
    {
        unsigned long   Undefined5;
        MAD_ColorARGB  *Colors;
    };
    GO_Flags    ObjectFlags;
} MAD_GeometricObject;

typedef struct MAD_CompressedPonderatedVertex_
{
	unsigned short	Index;
	unsigned short	Ponderation;
} MAD_CompressedPonderatedVertex;

typedef struct	MAD_PonderationList_
{
	MAD_Matrix						st_FlashedMatrix;
	unsigned short					us_MatrixIdx;
	unsigned short					us_NumberOfPonderatedVertices;
	MAD_CompressedPonderatedVertex	*p_CmpPdrtVrt;
} MAD_PonderationList;

typedef struct	MAD_SkinnedGeometricObject_
{
	/* Def.: MAD Id (must stay the first !). */
    MAD_NodeID				ID;
	/* Def.: Pointer to his geometric object. */
	MAD_GeometricObject		*pst_GeoObj;
	/* Def.: Number of matrix that ponderates the object. */
	unsigned short			us_NumberOfPonderationLists;
	/* Def.: List of matrix that ponderates the object. */
	MAD_PonderationList		*pst_PonderationList;
} MAD_SkinnedGeometricObject;

/*$5
 ###################################################################################################
    Hierarchie
 ###################################################################################################
 */

#define MAD_NODEFLAG        unsigned long
#define MNF_IsGroupHead     1
#define MNF_IsGroupMember   2
#define MNF_IsLookAt        4
#define MNF_MustBeMadRaded	8
#ifdef JADEFUSION
#define MNF_IsCollisionNode 0x00000010
#endif
typedef struct  MAD_BoundingVolume_
{
    float		XMin , YMin , ZMin;
    float		XMax , YMax , ZMax;
} MAD_BoundingVolume;

typedef struct  MAD_WorldNode_
{
    MAD_NodeID			ID;             /* Must stay the first ! */
    MAD_Matrix			Matrix;         /* Absolute Matrix */
    MAD_Matrix			RelativeMatrix; /* Hierachicaly relativ matrix */
    MAD_Vertex			Pivot;          /* Ca va bien servir a quelque chose. */
    MAD_ColorARGB		WiredColor;

    unsigned long		Object;			/* Index in **AllObjects - MAD_NULL_INDEX mean no object */
    unsigned long		Parent;			/* Index in *Hierarchie - MAD_NULL_INDEX mean no parent */
    unsigned long		Target;			/* Index in *Hierarchie - MAD_NULL_INDEX mean no target */
    unsigned long		Material;		/* Index in **AllMaterial - MAD_NULL_INDEX mean no material */
    MAD_BoundingVolume  *p_stBDV;		/* Boundiing volume is used in MAD_RAD */
    MAD_ColorARGB		*RLIOfObject;
    unsigned long		ObjectWithRadiosity; /* Object computed in MAD_RAD - MAD_NULL_INDEX mean no object */
	unsigned long		Undefined3;
	unsigned long		Undefined4;		/* Used to store Gizmo number for the bone hierarchy nodes */
	MAD_ColorARGB		*UsersRLIOfObject; /* Used only in MADRAD for keeping user's RLI (especially Alpha)*/
    MAD_NODEFLAG		NODE_Flags;
} MAD_WorldNode;

#define MAD_WORLD_VERION_0  0xC0DE0000
#define MAD_WORLD_VERION_2  0xC0DE0002 /* New version. Stores Gizmo number in MAD file, using the "Undefined4" data of the "MAD_WorldNode" structure. */

typedef struct  MAD_World_
{
    unsigned long   MAD_Version;        /* File identification, Must stay the first ! */
    MAD_NodeID      ID;
    MAD_ColorARGB   AmbientColor;
    unsigned long   NumberOftexture;
    MAD_texture     **AllTextures;
    unsigned long   NumberOfMaterials;
    MAD_MAT_MatID   **AllMaterial;
    unsigned long   NumberOfObjects;    /* Lights, models, shapes, camera in disorder */
    MAD_NodeID      **AllObjects;
    unsigned long   NumberOfHierarchieNodes;
    MAD_WorldNode   *Hierarchie;
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_World;

/*$5
 ###################################################################################################
    Utils
 ###################################################################################################
 */

unsigned long   MAD_GetNumberOfFace(MAD_GeometricObject *MADMesh);

unsigned long   MAD_GetFace(MAD_GeometricObject *MADMesh,
                    unsigned long       FaceNum,
                    MAD_Face            **Face,
                    unsigned long       *ElementNumber,
                    unsigned long       *MaterialID
                );                      /* Return 0 if OK */

/*$5
 ###################################################################################################
    Load & Save
 ###################################################################################################
 */

typedef struct  MAD_SubFile_
{
    MAD_NodeID      ID;
    char            FileName[260];      /* _MAX_PATH (NT maximum name lenght) */
    unsigned long   TextureIndex;
    unsigned long   FileSize;
} MAD_SubFile;

#ifdef JADEFUSION
struct  MAD_0_World_;
void		*MAD_Load_V0(FILE *File);
struct MAD_0_World_	*MAD_Update_0_to_1(struct MAD_0_World_ *MW);
#else
MAD_World		*MAD_Load_V0(FILE *File);
MAD_World		*MAD_Update_0_to_1(void *MW);
#endif
#define MAD_WORLD_FILE_VERION_0		0xC0DE0000
#define MAD_WORLD_FILE_VERION_1		0xC0DE0001
#define MAD_WORLD_FILE_VERION_2		0xC0DE0002  // Can contain Multi Texture Materials
#define MAD_WORLD_FILE_LAST_VERION	MAD_WORLD_FILE_VERION_2

#define MAD_FileFlag            unsigned long
#define MAD_SaveTextureFiles    1
#define MAD_RelativeTexFileName	2

void            FileCopy(FILE *F1, FILE *F2, int size);
int             FileEqual(FILE *F1, FILE *F2, int size);
void            MAD_SeparateDirNameFromName(char *Filename, char *DirectoryName, char *TextureName);
void            MAD_Save(MAD_World *MW, FILE *File, MAD_FileFlag SF);
MAD_World      *MAD_Load(char *FileName,void(*LoadCallback) (FILE * f, int size, char *Name, int USERPARRAM),int USERPARRAM);
void            MAD_NodeEnum(MAD_World * MW,unsigned long FILTER,void(*NodeCallback) (MAD_World * MW, MAD_WorldNode * MWN, unsigned long NodeNumber));

/*
 ===================================================================================================
    MAD RAD STUFF
 ===================================================================================================
 */

void					MAD_OptimizeElements(MAD_GeometricObject *MO);
void					MAD_ReComputeElements(MAD_GeometricObject *MO);
void					MAD_ComputeNeightbouring(MAD_World *MW);
void					MAD_SubdiviseGO(MAD_World *MW, MAD_WorldNode *MWN,MAD_GeometricObject *MO);
void					MAD_ComputeAllNormales(MAD_World *MW);
void					MAD_ComputeNormales(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber);
void					MAD_Numerotize(MAD_World *MW, MAD_GeometricObject *MO);
void					MAD_RemoveIllgalFaces(MAD_GeometricObject *MO);
void					MAD_RemoveIsoPoints(MAD_GeometricObject *MO);
void					MAD_ComputeOneIndexPerUVPerElementSolution(MAD_GeometricObject *MO,unsigned long FromJade);
void					MAD_RestoreOneIndexPerUVPerElementSolution(MAD_GeometricObject *MO);
void					MAD_TurnEdges(MAD_World *MW, MAD_GeometricObject *MO);
unsigned long			MAD_IsMagnifold(MAD_GeometricObject *MO);
unsigned long			MAD_VerifyVertexColorAreNotTo0(MAD_GeometricObject *MO);
void					MAD_MakeNoHinstancesGO(MAD_World *MW);
MAD_GeometricObject	*	MAD_CopyGo(MAD_GeometricObject *GO);
void					MAD_CleanUnusedObject(MAD_World *MW);
void					MAD_ComputeNeightbouringOnObject(MAD_GeometricObject *MO);
unsigned long *			MAD_MakeItMagnifold(MAD_GeometricObject *MO);
void					MAD_RestoreNonMagnifold(MAD_GeometricObject *MO , unsigned long *pGetFroms);


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
