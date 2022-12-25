/*$T MAD_Struct_V0.h GC!1.32 10/20/99 15:09:15 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include <STDIO.H>
#include <memory.H>
//#include "MAD_Struct_V0.h"
#define MAD_0_NULL_INDEX              (unsigned long) 0xffffffff
#define MAX_MAX_Mtl_Per_OBJECT      64

#define ID_MAD_0_Type                 unsigned long
#define ID_MAD_0_UNDEFINED            0xC0FDFDFD
#define ID_MAD_0_World                0
#define ID_MAD_0_Matrix               1
#define ID_MAD_0_Material             2
#define ID_MAD_0_Texture              3
#define ID_MAD_0_GeometricObject_V0   4
#define ID_MAD_0_Light                5
#define ID_MAD_0_Camera               6
#define ID_MAD_0_WorldNode            7
#define ID_MAD_0_TargetObject         8
#define ID_MAD_0_Shape                9
#define ID_MAD_0_Dummy                10
#define ID_MAD_0_SubFile              11

#define MAD_0_NAME_LENGHT             256

typedef unsigned long   MAD_0_ColorARGB;
#define MAD_GetRed(CARGB)		(((CARGB) & 0xff0000) >> 16)
#define MAD_Getgreen(CARGB)		(((CARGB) & 0xff00) >> 8)
#define MAD_GetBlue(CARGB)		(((CARGB) & 0xff) >> 0)
#define MAD_GetAlpha(CARGB)		(((CARGB) & 0xff000000) >> 24)
#define MAD_SetRed(CARGB,Val) (CARGB) = ((CARGB) & 0xFF00ffff) | ((Val & 0xff) << 16)
#define MAD_Setgreen(CARGB,Val) (CARGB) = ((CARGB) & 0xFFff00ff) | ((Val & 0xff) << 8)
#define MAD_SetBlue(CARGB,Val) (CARGB) = ((CARGB) & 0xFFffff00) | ((Val & 0xff) << 0)
#define MAD_SetAlpha(CARGB,Val) (CARGB) = ((CARGB) & 0x00ffffff) | ((Val & 0xff) << 24)

#define MAD_ColorARGB_to_MAD_Vertex(MV,CARGB)\
{\
	MV . x = (float)MAD_GetRed(CARGB);\
	MV . y = (float)MAD_Getgreen(CARGB);\
	MV . z = (float)MAD_GetBlue(CARGB);\
}

#define MAD_Vertex_to_MAD_ColorARGB(MV,CARGB)\
{\
	MAD_SetRed(CARGB,(unsigned long)(MV . x));\
	MAD_Setgreen(CARGB,(unsigned long)(MV . y));\
	MAD_SetBlue(CARGB,(unsigned long)(MV . z));\
}


typedef struct MAD_0_Vertex_
{
    float   x, y, z;
} MAD_0_Vertex;

typedef struct  MAD_0_Simple3DVertex_
{
    MAD_0_Vertex  Point;
    MAD_0_Vertex  Normale;
} MAD_0_Simple3DVertex;

typedef struct  MAD_0_NodeID_
{
    ID_MAD_0_Type     IDType;
    unsigned long   SizeOfThisOne;
    char            Name[MAD_0_NAME_LENGHT];
} MAD_0_NodeID;

typedef struct  MAD_0_Undefined_
{
    MAD_0_NodeID  ID;
} MAD_0_Undefined;

typedef struct  MAD_0_Matrix_
{
    MAD_0_NodeID  ID;                     /* Must stay the first ! */
    MAD_0_Vertex  Translation;
    MAD_0_Vertex  I, J, K;
} MAD_0_Matrix;

/*$5
 ###################################################################################################
    Textures
 ###################################################################################################
 */

typedef struct  MAD_0_texture_
{
    MAD_0_NodeID  ID;                     /* Must stay the first ! */
    char        Texturefile[260];       /* _MAX_PATH (NT maximum name lenght) */
} MAD_0_texture;

/*$5
 ###################################################################################################
    Materials
 ###################################################################################################
 */

#define MAD_0_MATERIALTYPE        unsigned long
#define ID_MAT_NullMat          0
#define ID_MAT_Standard         1
#define ID_MAT_MultiMaterial    2

/*
 ---------------------------------------------------------------------------------------------------
    Simples materials
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  MAD_0_MAT_MatID_
{
    MAD_0_MATERIALTYPE    MaterialType;   /* Must stay the first ! */
    char                Name[MAD_0_NAME_LENGHT];
} MAD_0_MAT_MatID;

#define MAD_0_MTF_TransparencyModeMask    0xF
#define MAD_0_MTF_Trans_Copy              0x0
#define MAD_0_MTF_Trans_Add               0x1
#define MAD_0_MTF_Trans_AddSmooth         0x2
#define MAD_0_MTF_Trans_Alpha             0x3
#define MAD_0_MTF_Trans_AlphaPremult      0x4
#define MAD_0_MTF_Trans_Invert            0x5
#define MAD_0_MTF_Trans_Mul               0x6
#define MAD_0_MTF_Trans_Mul2X             0x7
#define MAD_0_MTF_UTiling                 0x10
#define MAD_0_MTF_VTiling                 0x20
#define MAD_0_MTF_UMirror                 0x40
#define MAD_0_MTF_VMirror                 0x80
#define MAD_0_MTF_AlphaThreshold          0x100
#define MAD_0_MTF_InvertColor             0x200
#define MAD_0_MTF_PerspectivCorrection    0x400

typedef struct  MAD_0_StandarMaterial_
{
    MAD_0_MAT_MatID   MatRef;             /* Must stay the first ! */
    MAD_0_ColorARGB   Diffuse;
    MAD_0_ColorARGB   Ambient;
    MAD_0_ColorARGB   Specular;
    float           SelfIllum;
    float           Opacity;
    unsigned long   MaterialFlag;
    unsigned long   MadTexture;         /* Index in **AllTexture - MAD_0_NULL_INDEX mean no texture */
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_0_StandarMaterial;

/*
 ---------------------------------------------------------------------------------------------------
    Complex materials
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  MAD_0_MultiMaterial_
{
    MAD_0_MAT_MatID   MatRef;             /* Must stay the first ! */
    unsigned long   NumberOfSubMaterials;
    unsigned long   *SubMats;           /* Index in AllMaterial */
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_0_MultiMaterial;

/*$5
 ###################################################################################################
    Light
 ###################################################################################################
 */

#define MAD_0_LightType               unsigned long
#define MAD_0_LightOmni               0
#define MAD_0_LightSpot               1
#define MAD_0_LightDirect             2

#define MAD_0_LightFlag               unsigned long
#define MAD_0_LightFlag_CastShadow    1
#define MAD_0_LightFlag_PaintLight    2
#define MAD_0_LightFlag_OverWalls     4

typedef struct  MAD_0_tdst_SpotLight_
{
    float   LittleAlpha;
    float   BigAlpha;
    float   Near;
    float   Far;
    float   RadiusOfLight;
} MAD_0_tdst_SpotLight;

typedef struct  MAD_0_tdst_DirectLight_
{
    float   Near;
    float   Far;
    float   RadiusOfLight;
} MAD_0_tdst_DirectLight;

typedef struct  MAD_0_tdst_OmniLight_
{
    float   Near;
    float   Far;
    float   RadiusOfLight;
} MAD_0_tdst_OmniLight;

typedef struct  MAD_0_Light_
{
    MAD_0_NodeID      ID;                 /* Must stay the first ! */
    MAD_0_LightType   LightType;          /* MAD_0_OMNI_LIGHT MAD_0_TSPOT_LIGHT MAD_0_DIR_LIGHT */
    MAD_0_ColorARGB   LightColor;
    int             IsOn;
    union
    {
        MAD_0_tdst_OmniLight      Omni;
        MAD_0_tdst_DirectLight    Direct;
        MAD_0_tdst_SpotLight      Spot;
    };
    MAD_0_LightFlag   LightFlags;
    float           Multiplier;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
} MAD_0_Light;

/*$5
 ###################################################################################################
    Camera
 ###################################################################################################
 */

#define MAD_0_CameraType      unsigned long
#define MAD_0_e_OrthoCamera   0
#define MAD_0_e_PerspCamera   1

typedef struct  MAD_0_OrthoCamera_
{
    float   ChannelSize;
    float   Znear;
    float   Zfar;
} MAD_0_OrthoCamera;

typedef struct  MAD_0_PerspCamera_
{
    float   Fov;
    float   Znear;
    float   Zfar;
} MAD_0_PerspCamera;

typedef struct  MAD_0_Camera_
{
    MAD_0_NodeID      ID; /* Must stay the first ! */
    MAD_0_CameraType  CameraType;
    union
    {
        MAD_0_OrthoCamera OrthoInf;
        MAD_0_PerspCamera PerspInf;
    };
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_0_Camera;

/*$5
 ###################################################################################################
    Shape
 ###################################################################################################
 */

#define MAD_0_SHAPETYPE       unsigned long
#define ID_MAT_SPLINE3D     0
#define ID_MAT_NURB         1

#define MAD_0_SHAPEKNOTTYPE   unsigned long
#define ID_MAT_SMOOTH       0
#define ID_MAT_CORNER       1
#define ID_MAT_BEZIER       2
#define ID_MAT_BEZIERCORNER 3

typedef struct  MAD_0_SplineKnot_
{
    MAD_0_SHAPEKNOTTYPE   KnotType;
    MAD_0_Vertex          ControlPoint;
    MAD_0_Vertex          PreviousTangent;
    MAD_0_Vertex          NexTangent;
} MAD_0_SplineKnot;

typedef struct  MAD_0_NurbKnot_
{
    float       Weight;
    MAD_0_Vertex  ControlPoint;
} MAD_0_NurbKnot;

typedef struct  MAD_0_Spline3D_
{
    unsigned long   NumberOfKnots;
    MAD_0_SplineKnot  *Knots;
} MAD_0_Spline3D;

typedef struct  MAD_0_Nurb_
{
    unsigned long   Degree;
    unsigned long   NumberOfKnots;
    MAD_0_NurbKnot        *Knots;
} MAD_0_Nurb;

typedef struct  MAD_0_Shape_
{
    MAD_0_NodeID      ID; /* Must stay the first ! */
    MAD_0_SHAPETYPE   TypeOfShape;
    unsigned long   ShapeClosed;
    union
    {
        MAD_0_Spline3D    Spline;
        MAD_0_Nurb        Nurb;
    };
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_0_Shape;

/*$5
 ###################################################################################################
    Dummy
 ###################################################################################################
 */

#define MAD_0_DUMMYTYPE   unsigned long
#define DUMMY_POINT     1
#define DUMMY_BOX       2
#define DUMMY_BONE      3
typedef struct  MAD_0_Dummy_
{
    MAD_0_NodeID      ID;         /* Must stay the first ! */
    MAD_0_DUMMYTYPE   Type;
    MAD_0_Vertex      BOXMin;
    MAD_0_Vertex      BOXMax;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_0_Dummy;

/*$5
 ###################################################################################################
    Target
 ###################################################################################################
 */

typedef struct  MAD_0_Target_
{
    MAD_0_NodeID      ID;         /* Must stay the first ! */
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_0_Target;

/*$5
 ###################################################################################################
    Geometric Objects
 ###################################################################################################
 */

#define MAD_0_GEOMETRICOBJECTTYPE unsigned long
#define MAD_0_MAXMESH_V0          0

typedef struct  MAD_0_Face_
{
    unsigned long   Index[3];
    unsigned long   UVIndex[3];
    unsigned long   SmoothingGroup;
    unsigned long   MAXflags;   /* Edge Visibility, Material ID , */
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
} MAD_0_Face;

typedef struct  MAD_0_GeometricObjectElement_
{
    unsigned long   MaterialID; /* This index is used only if the MAD_0_node contain a multimaterial
                                 * object */
    unsigned long   NumberOfTriangles;
    MAD_0_Face        *Faces;
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
        MAD_0_Face        *FacesIIndexedOnIndex2;
    };
}
MAD_0_GeometricObjectElement;

#define GO_Flags        unsigned long
#define GO_ShadowCut    1
#define GO_NonMagnifold 2

typedef struct  MAD_0_GeometricObject_
{
    MAD_0_NodeID                  ID;                 /* Must stay the first ! */
    MAD_0_GEOMETRICOBJECTTYPE     TypeOfThis;         /* To permit Evolutivity */
    unsigned long               NumberOfPoints;
    MAD_0_Simple3DVertex          *OBJ_PointList;
    unsigned long               NumberOfUV;
    MAD_0_Vertex                  *OBJ_UVtextureList; /* X = u , y = v , z = W */
    unsigned long               NumberOfElements;
    MAD_0_GeometricObjectElement  **Elements;
    unsigned long               Undefined0;
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
        MAD_0_Vertex      *Colors;
    };
    GO_Flags    ObjectFlags;
} MAD_0_GeometricObject;

/*$5
 ###################################################################################################
    Hierarchie
 ###################################################################################################
 */

#define MAD_0_NODEFLAG        unsigned long
#define MNF_IsGroupHead     1
#define MNF_IsGroupMember   2
#define MNF_IsLookAt        4

typedef struct  MAD_0_BoundingVolume_
{
    float		XMin , YMin , ZMin;
    float		XMax , YMax , ZMax;
} MAD_0_BoundingVolume;

typedef struct  MAD_0_WorldNode_
{
    MAD_0_NodeID      ID;             /* Must stay the first ! */
    MAD_0_Matrix      Matrix;         /* Absolute Matrix */
    MAD_0_Matrix      RelativeMatrix; /* Hierachicaly relativ matrix */
    MAD_0_Vertex      Pivot;          /* Ca va bien servir a quelque chose. */
    MAD_0_ColorARGB   WiredColor;

    unsigned long   Object;         /* Index in **AllObjects - MAD_0_NULL_INDEX mean no object */
    unsigned long   Parent;         /* Index in *Hierarchie - MAD_0_NULL_INDEX mean no parent */
    unsigned long   Target;         /* Index in *Hierarchie - MAD_0_NULL_INDEX mean no target */
    unsigned long   Material;       /* Index in **AllMaterial - MAD_0_NULL_INDEX mean no material */
    union
    {
        unsigned long   Undefined0;
        MAD_0_BoundingVolume   *p_stBDV; /* Boundiing volume is used in MAD_0_RAD */
    };
    union
    {
        unsigned long   Undefined1;
        MAD_0_Vertex	*p_stVertexColor; /* Boundiing volume is used in MAD_0_RAD */
    };
    unsigned long   Undefined2;
	unsigned long   Undefined3;
	unsigned long   Undefined4;
	unsigned long   Undefined5;
    MAD_0_NODEFLAG    NODE_Flags;
} MAD_0_WorldNode;

#define MAD_0_WORLD_VERION_0  0xC0DE0000

typedef struct  MAD_0_World_
{
    unsigned long   MAD_0_Version;        /* File identification, Must stay the first ! */
    MAD_0_NodeID      ID;
    MAD_0_ColorARGB   AmbientColor;
    unsigned long   NumberOftexture;
    MAD_0_texture     **AllTextures;
    unsigned long   NumberOfMaterials;
    MAD_0_MAT_MatID   **AllMaterial;
    unsigned long   NumberOfObjects;    /* Lights, models, shapes, camera in disorder */
    MAD_0_NodeID      **AllObjects;
    unsigned long   NumberOfHierarchieNodes;
    MAD_0_WorldNode   *Hierarchie;
    unsigned long   Undefined0;
    unsigned long   Undefined1;
    unsigned long   Undefined2;
    unsigned long   Undefined3;
    unsigned long   Undefined4;
    unsigned long   Undefined5;
    unsigned long   Undefined6;
} MAD_0_World;

/*$5
 ###################################################################################################
    Utils
 ###################################################################################################
 */

unsigned long   MAD_0_GetNumberOfFace(MAD_0_GeometricObject *MADMesh);

unsigned long   MAD_0_GetFace(MAD_0_GeometricObject *MADMesh,
                    unsigned long       FaceNum,
                    MAD_0_Face            **Face,
                    unsigned long       *ElementNumber,
                    unsigned long       *MaterialID
                );                      /* Return 0 if OK */

/*$5
 ###################################################################################################
    Load & Save
 ###################################################################################################
 */

/*$T MAD_0_Load.c GC!1.32 05/27/99 15:23:29 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "MAD_mem/Sources/MAD_mem.h"

#define MAD_0_Load_MainStruct(type, address, file) \
    MAD_MALLOC(type, address, 1); \
    fread(&(address)->ID.SizeOfThisOne, sizeof(type) - sizeof(ID_MAD_0_Type), 1, file); \
    (address)->ID.IDType = IDType;

#define MAD_0_Load_NormalStruct(type, address, size, file) \
    MAD_MALLOC(type, address, size); \
    fread(address, sizeof(type), size, file);

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadUndefined(MAD_0_World *MW, MAD_0_Undefined **MN, FILE *File, ID_MAD_0_Type IDType)
{
    MAD_0_Load_MainStruct(MAD_0_Undefined, *MN, File);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadDummy(MAD_0_World *MW, MAD_0_Dummy **MD, FILE *File, ID_MAD_0_Type IDType)
{
    MAD_0_Load_MainStruct(MAD_0_Dummy, *MD, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadTarget(MAD_0_World *MW, MAD_0_Target **MT, FILE *File, ID_MAD_0_Type IDType)
{
    MAD_0_Load_MainStruct(MAD_0_Target, *MT, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadGeometric_V0(MAD_0_World *MW, MAD_0_GeometricObject **MO, FILE *File, ID_MAD_0_Type IDType)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_0_Load_MainStruct(MAD_0_GeometricObject, *MO, File);
    switch((*MO)->TypeOfThis)
    {
    case MAD_0_MAXMESH_V0:
        MAD_0_Load_NormalStruct
        (
            MAD_0_Simple3DVertex,
            (*MO)->OBJ_PointList,
            (*MO)->NumberOfPoints,
            File
        );
        MAD_0_Load_NormalStruct(MAD_0_Vertex, (*MO)->OBJ_UVtextureList, (*MO)->NumberOfUV, File);
		if ((*MO)->Colors) 
		{
			MAD_0_Load_NormalStruct(MAD_0_Vertex, (*MO)->Colors, (*MO)->NumberOfPoints, File);
		} else
			MAD_MALLOC(MAD_0_Vertex, (*MO)->Colors, (*MO)->NumberOfPoints);

        MAD_0_Load_NormalStruct
        (
            MAD_0_GeometricObjectElement *,
            (*MO)->Elements,
            (*MO)->NumberOfElements,
            File
        );
        for(Counter = 0; Counter < (*MO)->NumberOfElements; Counter++)
        {
            MAD_0_Load_NormalStruct(MAD_0_GeometricObjectElement, (*MO)->Elements[Counter], 1, File);
            MAD_0_Load_NormalStruct
            (
                MAD_0_Face,
                (*MO)->Elements[Counter]->Faces,
                (*MO)->Elements[Counter]->NumberOfTriangles,
                File
            );
        }

        break;
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadLight(MAD_0_World *MW, MAD_0_Light **ML, FILE *File, ID_MAD_0_Type IDType)
{
    MAD_0_Load_MainStruct(MAD_0_Light, *ML, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadShape(MAD_0_World *MW, MAD_0_Shape **ML, FILE *File, ID_MAD_0_Type IDType)
{
    MAD_0_Load_MainStruct(MAD_0_Shape, *ML, File);
    switch((*ML)->TypeOfShape)
    {
    case ID_MAT_NURB:
        MAD_0_Load_NormalStruct(MAD_0_NurbKnot, (*ML)->Nurb.Knots, (*ML)->Nurb.NumberOfKnots, File);
        break;
    case ID_MAT_SPLINE3D:
        MAD_0_Load_NormalStruct(MAD_0_SplineKnot, (*ML)->Spline.Knots, (*ML)->Spline.NumberOfKnots, File);
        break;
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadCamera(MAD_0_World *MW, MAD_0_Camera **MC, FILE *File, ID_MAD_0_Type IDType)
{
    MAD_0_Load_MainStruct(MAD_0_Camera, *MC, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadMaterial(MAD_0_World *MW, MAD_0_MAT_MatID **MT, FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_0_MAT_MatID   NM;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    fread(&NM, sizeof(MAD_0_MAT_MatID), 1, File);
    fseek(File, -(long) sizeof(MAD_0_MAT_MatID), SEEK_CUR);
    switch(NM.MaterialType)
    {
    case ID_MAT_NullMat:
        MAD_0_Load_NormalStruct(MAD_0_MAT_MatID, *MT, 1, File);
        **MT = NM;
        break;
    case ID_MAT_Standard:
        MAD_0_Load_NormalStruct(MAD_0_StandarMaterial, *(MAD_0_StandarMaterial **) MT, 1, File);
        **MT = NM;
        break;
    case ID_MAT_MultiMaterial:
        MAD_0_Load_NormalStruct(MAD_0_MultiMaterial, *(MAD_0_MultiMaterial **) MT, 1, File);
        MAD_0_Load_NormalStruct
        (
            unsigned long,
            (*(MAD_0_MultiMaterial **) MT)->SubMats,
            (*(MAD_0_MultiMaterial **) MT)->NumberOfSubMaterials,
            File
        );
        **MT = NM;
        break;
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadTexture(MAD_0_World *MW, MAD_0_texture **MT, FILE *File)
{
    MAD_0_Load_NormalStruct(MAD_0_texture, *MT, 1, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadAllNodes(MAD_0_World *MW, FILE *File)
{
    MAD_0_Load_NormalStruct(MAD_0_WorldNode, MW->Hierarchie, MW->NumberOfHierarchieNodes, File);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadAllObjects(MAD_0_World *MW, FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned    Counter;
    ID_MAD_0_Type IDType;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(Counter = 0; Counter < MW->NumberOfObjects; Counter++)
    {
        fread(&IDType, sizeof(ID_MAD_0_Type), 1, File);
        switch(IDType)
        {
        case ID_MAD_0_GeometricObject_V0:
            MAD_0_LoadGeometric_V0
            (
                MW,
                (MAD_0_GeometricObject **) &MW->AllObjects[Counter],
                File,
                IDType
            );
            break;
        case ID_MAD_0_Light:
            MAD_0_LoadLight(MW, (MAD_0_Light **) &MW->AllObjects[Counter], File, IDType);
            break;
        case ID_MAD_0_Camera:
            MAD_0_LoadCamera(MW, (MAD_0_Camera **) &MW->AllObjects[Counter], File, IDType);
            break;
        case ID_MAD_0_Dummy:
            MAD_0_LoadDummy(MW, (MAD_0_Dummy **) &MW->AllObjects[Counter], File, IDType);
            break;
        case ID_MAD_0_Shape:
            MAD_0_LoadShape(MW, (MAD_0_Shape **) &MW->AllObjects[Counter], File, IDType);
            break;
        case ID_MAD_0_TargetObject:
            MAD_0_LoadTarget(MW, (MAD_0_Target **) &MW->AllObjects[Counter], File, IDType);
            break;
        case ID_MAD_0_UNDEFINED:
            MAD_0_LoadUndefined(MW, (MAD_0_Undefined **) &MW->AllObjects[Counter], File, IDType);
            break;
        }
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadAllMaterials(MAD_0_World *MW, FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned    Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(Counter = 0; Counter < MW->NumberOfMaterials; Counter++)
        MAD_0_LoadMaterial(MW, &MW->AllMaterial[Counter], File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_0_LoadAllTextures(MAD_0_World *MW, FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned    Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(Counter = 0; Counter < MW->NumberOftexture; Counter++)
        MAD_0_LoadTexture(MW, &MW->AllTextures[Counter], File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
MAD_0_World *MAD_0_Load_Version_0(FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_0_World   *MW;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_MALLOC(MAD_0_World, MW, 1);
    fread(MW, sizeof(*MW), 1, File);
    if(MW->MAD_0_Version != MAD_0_WORLD_VERION_0) return NULL;
    MAD_MALLOC(MAD_0_texture *, MW->AllTextures, MW->NumberOftexture);
    MAD_MALLOC(MAD_0_MAT_MatID *, MW->AllMaterial, MW->NumberOfMaterials);
    MAD_MALLOC(MAD_0_NodeID *, MW->AllObjects, MW->NumberOfObjects);
    MAD_MALLOC(MAD_0_WorldNode, MW->Hierarchie, MW->NumberOfHierarchieNodes);
    MAD_0_LoadAllTextures(MW, File);
    MAD_0_LoadAllMaterials(MW, File);
    MAD_0_LoadAllObjects(MW, File);
    MAD_0_LoadAllNodes(MW, File);
    return MW;
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void *MAD_Load_V0(FILE *File)
{
    return (void *)MAD_0_Load_Version_0(File);
};

MAD_0_World *MAD_Update_0_to_1(MAD_0_World *MW)
{
	unsigned long Counter,ColorCounter;
	MAD_0_GeometricObject *GO;
	MAD_0_Vertex *OldColor;
	MAD_0_ColorARGB *NewColors;

	/* Convert all vertex colors */
	for (Counter = 0 ; Counter < MW -> NumberOfObjects;Counter++)
	{
		if (MW ->AllObjects[Counter]->IDType == ID_MAD_0_GeometricObject_V0)
		{
			GO = (MAD_0_GeometricObject *)MW ->AllObjects[Counter];
			OldColor = GO -> Colors;
			MAD_MALLOC(MAD_0_ColorARGB, NewColors, GO->NumberOfPoints);
			for (ColorCounter = 0 ; ColorCounter < GO->NumberOfPoints ; ColorCounter++)
			{
				MAD_Vertex_to_MAD_ColorARGB(GO -> Colors[ColorCounter],NewColors[ColorCounter]);
			}
			GO -> Colors = (MAD_0_Vertex *)NewColors;
		}
	}

	for (Counter = 0 ; Counter < MW ->NumberOfHierarchieNodes ;Counter++)
	{
		MW ->Hierarchie[Counter].Undefined1 = 0;
		MW ->Hierarchie[Counter].Undefined2 = MAD_0_NULL_INDEX;
	}
	return MW; /* nothing is here */
};

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
