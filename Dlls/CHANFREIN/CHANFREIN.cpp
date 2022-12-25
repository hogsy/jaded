/*$T CHANFREIN.cpp GC!1.32 06/29/99 19:54:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#define BEGIN_UV if (SaveUV) {
#define END_UV }

#define BEGIN_VC if (M->vertCol) {
#define END_VC }

#define BEGIN_QUAD if (1) {
#define END_QUAD }
#include "CHANFREIN.h"
/*$off*/
class GouMesh 
{
	friend class Face;
	friend class MeshAccess;
public:
	static intrefCount;
	static HANDLExfmThread;
	static HANDLExfmMutex;
	static HANDLExfmStartEvent;
	static HANDLExfmEndEvent;
	friend DWORD WINAPI xfmFunc(LPVOID ptr);
	static HANDLEfNorThread;
	static HANDLEfNorMutex;
	static HANDLEfNorStartEvent;
	static HANDLEfNorEndEvent;
	friend DWORD WINAPI fNorFunc(LPVOID ptr);
	static HANDLEworkThread;
	static HANDLEworkMutex;
	static HANDLEworkStartEvent;
	static HANDLEworkEndEvent;
	friend DWORD WINAPI workFunc(LPVOID ptr);
	RVertex *rVerts;
	GraphicsWindow *cacheGW; 
	Point3 *faceNormal;
	Box3 bdgBox;
	int numVisEdges;
	int edgeListHasAll;
	VEdge *visEdge;
};
/*$on*/

/*
 * < FILE: CHANFREIN.cpp DESCRIPTION: Appwizard generated plugin CREATED BY: HISTORY: > Copyright
 * (c) 1997, All Rights Reserved.
 */

/* The unique ClassID */
#define CHANFREIN_CLASS_ID  Class_ID(0x4ddbb121, 0x5dbceb21)
typedef struct CMPF_Edge_ CMPF_Edge ;
class CHANFREIN : public Modifier
{
public:
    static IObjParam    *ip;        /* Access to the interface Class vars */
    static IParamMap    *pmapParam; /* Handle to the parammap Parameter block */
    IParamBlock         *pblock;    /* Ref 0 */

    /*
     ===============================================================================================
        Constructor/Destructor
     ===============================================================================================
     */
    CHANFREIN(void);

    ~CHANFREIN();

    /*
     ===============================================================================================
        From Animatable
     ===============================================================================================
     */
    void    DeleteThis(void) { delete this; }

    void    GetClassName(TSTR &s) { s = TSTR(GetString(IDS_CLASS_NAME)); }

    virtual Class_ID    ClassID(void) { return CHANFREIN_CLASS_ID; }

    RefTargetHandle Clone(RemapDir &remap = NoRemap());
    TCHAR           *GetObjectName(void) { return GetString(IDS_CLASS_NAME); }

    IOResult    Load(ILoad *iload);

    /*
     ===============================================================================================
        From Modifier TODO: Add the channels that the modifier needs to perform its modification
     ===============================================================================================
     */
    ChannelMask ChannelsUsed(void)
    {
        return PART_GEOM | PART_TOPO | PART_SELECT | PART_SUBSEL_TYPE | PART_TEXMAP |
            PART_VERTCOLOR;
    }

    /*
     ===============================================================================================
        TODO: Add the channels that the modifier actually modifies
     ===============================================================================================
     */
    ChannelMask ChannelsChanged(void)
    {
        return PART_GEOM | PART_TOPO | PART_SELECT | PART_TEXMAP | PART_VERTCOLOR;
    }

    void        ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

    /*
     ===============================================================================================
        TODO: Return the ClassID of the object that the modifier can modify
     ===============================================================================================
     */
    Class_ID    InputType(void) { return triObjectClassID; }

    Interval    LocalValidity(TimeValue t);

    /*
     ===============================================================================================
        From BaseObject TODO: Return true if the modifier changes topology
     ===============================================================================================
     */
    BOOL        ChangeTopology(void) { return TRUE; }

    int GetParamBlockIndex(int id) { return id; }

    /*
     ===============================================================================================
        From ReferenceMaker TODO::Implement the following methods for references
     ===============================================================================================
     */
    int NumRefs(void) { return 1; }

    RefTargetHandle GetReference(int i) { return pblock; }

    void    SetReference(int i, RefTargetHandle rtarg) { pblock = (IParamBlock *) rtarg; }

    int NumSubs(void) { return 1; }

    Animatable  *SubAnim(int i) { return pblock; }

    TSTR    SubAnimName(int i) { return GetString(IDS_PARAMS); }

    /*
     ===============================================================================================
        TODO: Return NULL if you do no want to make the paramblock available
     ===============================================================================================
     */
    IParamArray *GetParamBlock(void) { return pblock; }

    RefResult           NotifyRefChanged
                        (
                            Interval        changeInt,
                            RefTargetHandle hTarget,
                            PartID          &partID,
                            RefMessage      message
                        );

    CreateMouseCallBack *GetCreateMouseCallBack(void) { return NULL; }

    void            BeginEditParams(IObjParam *ip, ULONG flags, Animatable *prev);
    void            EndEditParams(IObjParam *ip, ULONG flags, Animatable *next);

    Interval        GetValidity(TimeValue t);
    ParamDimension  *GetParameterDim(int pbIndex);
    TSTR            GetParameterName(int pbIndex);

	BOOL			ChmpfrClbk(TimeValue t, IParamMap *map, HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) ;
	/*
     ===============================================================================================
        Automatic texture support
     ===============================================================================================
     */
    BOOL            HasUVW(void);
    void            SetGenUVW(BOOL sw);
	void			UnifyFaces(CMPF_Edge *EDG);
};

/* This is the Class Descriptor for the CHANFREIN plug-in */
class CHANFREINClassDesc : public ClassDesc
{
public:
    int IsPublic(void) { return 1; }

    void    *Create(BOOL loading = FALSE) { return new CHANFREIN(); }

    const TCHAR *ClassName(void) { return GetString(IDS_CLASS_NAME); }

    SClass_ID   SuperClassID(void) { return OSM_CLASS_ID; }

    Class_ID    ClassID(void) { return CHANFREIN_CLASS_ID; }

    const TCHAR *Category(void) { return GetString(IDS_CATEGORY); }

    void    ResetClassParams(BOOL fileReset);
};

static CHANFREINClassDesc   CHANFREINDesc;

/*
 ===================================================================================================
 ===================================================================================================
 */
ClassDesc *GetCHANFREINDesc(void)
{
    return &CHANFREINDesc;
}

/*
 ===================================================================================================
    TODO: Should implement this method to reset the plugin params when Max is reset
 ===================================================================================================
 */
void CHANFREINClassDesc::ResetClassParams(BOOL fileReset)
{
}

/* TODO: Add Parameter block indices for various parameters */
#define PB_SPINNER   0
#define PB_SPINNER2  1
#define PB_CHK1  2
#define PB_CHK2  3
#define PB_CHK3  4
#define PB_CHK4  5
#define PB_CHK5  6
#define PB_CHK6  7
#define PB_CHK7  8
#define PB_CHK8  9
#define PB_CHK9  10
#define PB_CHK10  11
#define PB_CHK11  12

static int typeChk1[] = {IDC_CHECK1};
static int typeChk2[] = {IDC_CHECK2};
/* TODO: Add ParamUIDesc's for the various parameters */
static ParamUIDesc  descParam[] =
{
    /* Spinner */
    ParamUIDesc(PB_SPINNER , EDITTYPE_FLOAT, IDC_EDIT, IDC_SPIN, 0.0f, 100.0f, 0.05f),
    ParamUIDesc(PB_SPINNER2, EDITTYPE_FLOAT, IDC_EDIT2, IDC_SPIN2, 0.0f, 1.0f, 0.005f),
};

/* TODO: Parameter descriptor length */
#define PARAMDESC_LENGTH    2

/* TODO: Add ParamBlockDescID's for the various parameters */
static ParamBlockDescID 
descVer1[] = { 
	{ TYPE_FLOAT, NULL, TRUE, 0 }, 
	{ TYPE_FLOAT, NULL, TRUE, 1 }, 
	{ TYPE_INT,   NULL, TRUE, 2 },
	{ TYPE_INT,   NULL, TRUE, 3 },
	{ TYPE_INT,   NULL, TRUE, 4 },
	{ TYPE_INT,   NULL, TRUE, 5 },
	{ TYPE_INT,   NULL, TRUE, 6 },
	{ TYPE_INT,   NULL, TRUE, 7 },
	{ TYPE_INT,   NULL, TRUE, 8 },
	{ TYPE_INT,   NULL, TRUE, 9 },
	{ TYPE_INT,   NULL, TRUE, 10 },
	{ TYPE_INT,   NULL, TRUE, 11 },
	};

#define CURRENT_DESCRIPTOR  descVer1

#define PBLOCK_LENGTH       12

#define CURRENT_VERSION     1

IObjParam *CHANFREIN::  ip = NULL;
IParamMap *CHANFREIN::  pmapParam = NULL;

/*
 ===================================================================================================
    --- CHANFREIN
 ===================================================================================================
 */
CHANFREIN::CHANFREIN(void)
{
    pblock = CreateParameterBlock(CURRENT_DESCRIPTOR, PBLOCK_LENGTH, CURRENT_VERSION);
	pblock->SetValue(PB_SPINNER  , 0 , 0.0f);
	pblock->SetValue(PB_SPINNER2 , 0 , 0.8f);
	pblock->SetValue(PB_CHK1 , 0 , 1);
	pblock->SetValue(PB_CHK2 , 0 , 0);
	pblock->SetValue(PB_CHK3 , 0 , 0);
	pblock->SetValue(PB_CHK4 , 0 , 0);
	pblock->SetValue(PB_CHK5 , 0 , 1);
	pblock->SetValue(PB_CHK6 , 0 , 0);
	pblock->SetValue(PB_CHK7 , 0 , 0);
	pblock->SetValue(PB_CHK8 , 0 , 0);
	pblock->SetValue(PB_CHK9 , 0 , 0);
	pblock->SetValue(PB_CHK10, 0 , 0);
    assert(pblock);
    MakeRefByID(FOREVER, 0, pblock);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
CHANFREIN::~CHANFREIN(void)
{
}

/*
 ===================================================================================================
 ===================================================================================================
 */
Interval CHANFREIN::LocalValidity(TimeValue t)
{
    /* If being edited, return NEVER forces a cache to be built after previous modifier. */
    if(TestAFlag(A_MOD_BEING_EDITED)) return NEVER;

    /* TODO: Return the validity interval of the modifier */
    return NEVER;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
RefTargetHandle CHANFREIN::Clone(RemapDir &remap)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CHANFREIN   *newmod;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    newmod = new CHANFREIN();
    /* TODO: Add the cloning code here */
    newmod->ReplaceReference(0, pblock->Clone(remap));
    return(newmod);
}

typedef struct  CMPF_Edge_
{
	/* BEGIN ORDER IS IMPORTANT!! */
	unsigned long	CutA;
	float			LenghtCutA;
	unsigned long	AIsNotOnLine;
	unsigned long	NewUVIndexA_FaceA;
	unsigned long	NewUVIndexA_FaceB;
	unsigned long	UVIndexA_FaceA;
	unsigned long	UVIndexA_FaceB;
	/* END ORDER IS IMPORTANT!! */
    unsigned long   PointA;
    unsigned long   FaceA;

	/* BEGIN ORDER IS IMPORTANT!! */
	unsigned long	CutB;
	float			LenghtCutB;
	unsigned long	BIsNotOnLine;
	unsigned long	NewUVIndexB_FaceA;
	unsigned long	NewUVIndexB_FaceB;
	unsigned long	UVIndexB_FaceA;
	unsigned long	UVIndexB_FaceB;
	/* END ORDER IS IMPORTANT!! */
    unsigned long   PointB;
    unsigned long   FaceB;

    unsigned long   HaveToBeSubdivide;
	unsigned long   IsVisible;


	unsigned long	UVAreShare;




} CMPF_Edge;

typedef struct  CMPF_VertexComplement_
{
    unsigned long   IStouchADivisibleEdge;
} CMPF_VertexComplement;

typedef struct  CMPF_FaceComplement_
{
	unsigned long	Index[3];
	unsigned long	UVIndex[3];
    unsigned long   NewIndex[3];
	unsigned long	NewUVIndex[3];
    unsigned long   Edge[3];
	Point3			Interpolator[3];
	Point3			InterpolatorUV[3];
	Point3			InterpolatorVC[3];
} CMPF_FaceComplement;

float					f_ChamferLenght;
CMPF_Edge               *AllEdges;
CMPF_FaceComplement     *AllFaces;
CMPF_VertexComplement   *AllVertex;
#define GETCMPFACE(a)   AllFaces[a]
#define GETCMPVERT(a)   AllVertex[a]
#define GETCMPEDGE(a)   AllEdges[a]
#define GETNBCMPFACES   SaveNT
#define GETNBCMPVERTS   SaveNE
#define GETNBCMPEDGES   SaveNP
/*
	this will find PointA , PointB , Edge [a] , Edge [b]
*/
unsigned long *GetSuperIndex_Adress(CMPF_FaceComplement *FC , unsigned long ulSprIndx)
{
	if ((ulSprIndx & 0xf) == 0)
		return &FC-> Index [ulSprIndx >> 4];
	if ((ulSprIndx & 0xf) == 2)
		return &FC-> NewIndex[ulSprIndx >> 4];
	if ((ulSprIndx & 0xf) == 1)
	{
		if (AllEdges[FC -> Edge[ulSprIndx >> 4]] . PointA == FC-> Index [ulSprIndx >> 4])
			return &AllEdges[FC -> Edge[ulSprIndx >> 4]] . CutA;
		else
			return &AllEdges[FC -> Edge[ulSprIndx >> 4]] . CutB;
	}
	if (AllEdges[FC -> Edge[((ulSprIndx >> 4) + 2) % 3]] . PointA == FC-> Index [ulSprIndx >> 4])
		return &AllEdges[FC -> Edge[((ulSprIndx >> 4) + 2) % 3]] . CutA;
	else
		return &AllEdges[FC -> Edge[((ulSprIndx >> 4) + 2) % 3]] . CutB;
}

unsigned long GetSuperIndex(CMPF_FaceComplement *FC , unsigned long ulSprIndx)
{
	unsigned long *Local;
	Local = GetSuperIndex_Adress(FC , ulSprIndx);
	return *Local;
}
unsigned long GetSuperIndex_UV(CMPF_FaceComplement *FC , unsigned long ulSprIndx)
{
	unsigned long ulEdgeIndex;
	if ((ulSprIndx & 0xf) == 0)
		return FC-> UVIndex[ulSprIndx >> 4];
	if ((ulSprIndx & 0xf) == 2)
	{
		if (GetSuperIndex(FC , ulSprIndx) == GetSuperIndex(FC , (ulSprIndx & 0xf0) | 0x01))
		{
			ulSprIndx = (ulSprIndx & 0xf0) | 0x01;
		} else
		if (GetSuperIndex(FC , ulSprIndx) == GetSuperIndex(FC , (ulSprIndx & 0xf0) | 0x03))
		{
			ulSprIndx = (ulSprIndx & 0xf0) | 0x03;
		} else	
			return FC-> NewUVIndex[ulSprIndx >> 4];
	}
	ulEdgeIndex = FC -> Edge[ulSprIndx >> 4];
	if ((ulSprIndx & 0xf) == 1)
	{
		/* Face A Or B ? */
		if (&AllFaces[AllEdges[ulEdgeIndex] . FaceA] == FC)
		{
			/* Face A */
			if (AllEdges[ulEdgeIndex] . PointA == FC-> Index [ulSprIndx >> 4])
				return AllEdges[ulEdgeIndex] . NewUVIndexA_FaceA;
			else
				return AllEdges[ulEdgeIndex] . NewUVIndexB_FaceA;
		} else
		{
			/* Face B */
			if (AllEdges[ulEdgeIndex] . PointA == FC-> Index [ulSprIndx >> 4])
				return AllEdges[ulEdgeIndex] . NewUVIndexA_FaceB;
			else
				return AllEdges[ulEdgeIndex] . NewUVIndexB_FaceB;
		} 
	}
	ulEdgeIndex = FC -> Edge[((ulSprIndx >> 4) + 2) % 3];
	/* Face A Or B ? */
	if (&AllFaces[AllEdges[ulEdgeIndex] . FaceA] == FC)
	{
		/* Face A */
		if (AllEdges[ulEdgeIndex] . PointA == FC-> Index [ulSprIndx >> 4])
			return AllEdges[ulEdgeIndex] . NewUVIndexA_FaceA;
		else
			return AllEdges[ulEdgeIndex] . NewUVIndexB_FaceA;
	} else
	{
		/* Face B */
		if (AllEdges[ulEdgeIndex] . PointA == FC-> Index [ulSprIndx >> 4])
			return AllEdges[ulEdgeIndex] . NewUVIndexA_FaceB;
		else
			return AllEdges[ulEdgeIndex] . NewUVIndexB_FaceB;
	} 
}


unsigned long IsValidTriangle_SI(CMPF_FaceComplement *FC , unsigned long SI1 , unsigned long SI2 , unsigned long SI3  )
{
	if (GetSuperIndex(FC , SI1) == GetSuperIndex(FC , SI2) ) return 0;
	if (GetSuperIndex(FC , SI1) == GetSuperIndex(FC , SI3) ) return 0;
	if (GetSuperIndex(FC , SI2) == GetSuperIndex(FC , SI3) ) return 0;
	return 1;
}

unsigned long ValidateTriangle_SI(Mesh *M,CMPF_FaceComplement *FC , unsigned long SI1 , unsigned long SI2 , unsigned long SI3  , unsigned long FaceInc ,unsigned long Flags)
{
	float fSurface;
	if (IsValidTriangle_SI(FC , SI1 , SI2 , SI3 ))
	{
		fSurface  = Length(
			(M -> verts[GetSuperIndex(FC , SI3)] - M -> verts[GetSuperIndex(FC , SI1)]) ^
			(M -> verts[GetSuperIndex(FC , SI2)] - M -> verts[GetSuperIndex(FC , SI1)]));
		if (fSurface < 0.00001f) 
			return 0;
		M -> faces[FaceInc] . v[0] = GetSuperIndex(FC , SI1);
		M -> faces[FaceInc] . v[1] = GetSuperIndex(FC , SI2);
		M -> faces[FaceInc] . v[2] = GetSuperIndex(FC , SI3);
		M -> faces[FaceInc] . flags = Flags;
		if (M -> tvFace)
		{
			M -> tvFace[FaceInc] . t[0] = GetSuperIndex_UV(FC , SI1);
			M -> tvFace[FaceInc] . t[1] = GetSuperIndex_UV(FC , SI2);
			M -> tvFace[FaceInc] . t[2] = GetSuperIndex_UV(FC , SI3);
		}//*/
		return 1;
	}
	return 0;
}


void CUTEDGE(Mesh  *M , CMPF_Edge *Edge , unsigned long Revert)
{
	if (!Revert)
	{
		if (Edge -> LenghtCutA == 0.0f)
			Edge -> LenghtCutA = f_ChamferLenght / Length(M -> verts[Edge->PointB] - M -> verts[Edge->PointA]);
	}
	else
	{
		if (Edge -> LenghtCutB == 0.0f)
			Edge -> LenghtCutB = f_ChamferLenght / Length(M -> verts[Edge->PointB] - M -> verts[Edge->PointA]);
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void CHANFREIN::ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   C2;
	unsigned long	ulNewNumberOfFaces , lNewNumberOfPoints , lNewNumberOfUV;
	unsigned long	SaveNT, SaveNE, SaveNP , SaveUV;
	int Detect;
	float fAutodetectThresh;
    Mesh            *M;
    GouMesh         *Gouloulou;
    Interval        iv;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pblock->GetValue(PB_SPINNER , t, f_ChamferLenght, iv);
	pblock->GetValue(PB_SPINNER2, t, fAutodetectThresh, iv);

    iv = os->obj->ChannelValidity(t, GEOM_CHAN_NUM);

    iv &= os->obj->ChannelValidity(t, TOPO_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, SELECT_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, SUBSEL_TYPE_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, TEXMAP_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, VERT_COLOR_CHAN_NUM);

    M = &((TriObject *) os->obj)->mesh;
    Gouloulou = (GouMesh *) M;
    M->BuildVisEdgeList();

	SaveNT = M->numFaces;
	SaveNE = M->numVerts;
	SaveNP = Gouloulou->numVisEdges;
	SaveUV = M->getNumTVerts();

	AllEdges	= (CMPF_Edge               *)malloc(sizeof(CMPF_Edge) * Gouloulou->numVisEdges);
	AllFaces	= (CMPF_FaceComplement     *)malloc(sizeof(CMPF_FaceComplement) * M->numFaces);
	AllVertex	= (CMPF_VertexComplement   *)malloc(sizeof(CMPF_VertexComplement) * M->numVerts);
	memset(AllEdges, 0 , sizeof(CMPF_Edge) * Gouloulou->numVisEdges);
	memset(AllFaces, 0 , sizeof(CMPF_FaceComplement) * M->numFaces);
	memset(AllVertex, 0 , sizeof(CMPF_VertexComplement) * M->numVerts);
	/* ================================================================================================ */
    /* BEGIN Compute the mesh connectivity														*/
	/* ================================================================================================ */
    for(C2 = 0; C2 < (unsigned long) GETNBCMPEDGES; C2++)
    {
		GETCMPEDGE(C2) . FaceA = Gouloulou->visEdge[C2].GetFace(0);
		if (GETCMPEDGE(C2) . FaceA == 0x3fffffff) 
		{
			GETCMPEDGE(C2) . FaceA = 0xffffffff;
		} else
		{
			GETCMPFACE(GETCMPEDGE(C2) . FaceA) . Edge[Gouloulou->visEdge[C2].GetWhichSide(0)] = C2;
			GETCMPEDGE(C2) . PointA = M->faces[GETCMPEDGE(C2) . FaceA] . v[Gouloulou->visEdge[C2].GetWhichSide(0)];
			GETCMPEDGE(C2) . PointB = M->faces[GETCMPEDGE(C2) . FaceA] . v[(Gouloulou->visEdge[C2].GetWhichSide(0) + 1)%3];
		}
		GETCMPEDGE(C2) . FaceB = Gouloulou->visEdge[C2].GetFace(1);
		if (GETCMPEDGE(C2) . FaceB == 0x3fffffff) 
		{
			GETCMPEDGE(C2) . FaceB = 0xffffffff;
		} else
		{
			GETCMPFACE(GETCMPEDGE(C2) . FaceB) . Edge[Gouloulou->visEdge[C2].GetWhichSide(1)] = C2;
			GETCMPEDGE(C2) . PointB = M->faces[GETCMPEDGE(C2) . FaceB] . v[Gouloulou->visEdge[C2].GetWhichSide(1)];
			GETCMPEDGE(C2) . PointA = M->faces[GETCMPEDGE(C2) . FaceB] . v[(Gouloulou->visEdge[C2].GetWhichSide(1) + 1)%3];
		}
		GETCMPEDGE(C2) . CutA = GETCMPEDGE(C2) . CutA = 0;
		GETCMPEDGE(C2) . LenghtCutA = GETCMPEDGE(C2) . LenghtCutB = 0.0f;
    }
	/* Write Edge Visibility */
	for(C2 = 0; C2 < (unsigned long) GETNBCMPFACES; C2++)
	{
		unsigned long EdgeCounter;
		if (M->faces[C2].flags & EDGE_A) GETCMPEDGE(GETCMPFACE(C2).Edge[0]) . IsVisible = 1;
		if (M->faces[C2].flags & EDGE_B) GETCMPEDGE(GETCMPFACE(C2).Edge[1]) . IsVisible = 1;
		if (M->faces[C2].flags & EDGE_C) GETCMPEDGE(GETCMPFACE(C2).Edge[2]) . IsVisible = 1;
		GETCMPFACE(C2).NewIndex[0] = GETCMPFACE(C2).Index[0] = M->faces[C2].v[0];
		GETCMPFACE(C2).NewIndex[1] = GETCMPFACE(C2).Index[1] = M->faces[C2].v[1];
		GETCMPFACE(C2).NewIndex[2] = GETCMPFACE(C2).Index[2] = M->faces[C2].v[2];
BEGIN_UV
		GETCMPFACE(C2).NewUVIndex[0] = GETCMPFACE(C2).UVIndex[0] = M->tvFace[C2].t[0];
		GETCMPFACE(C2).NewUVIndex[1] = GETCMPFACE(C2).UVIndex[1] = M->tvFace[C2].t[1];
		GETCMPFACE(C2).NewUVIndex[2] = GETCMPFACE(C2).UVIndex[2] = M->tvFace[C2].t[2];
		GETCMPFACE(C2).InterpolatorUV[0] = M->tVerts[M->tvFace[C2].t[0]];
		GETCMPFACE(C2).InterpolatorUV[1] = M->tVerts[M->tvFace[C2].t[1]];
		GETCMPFACE(C2).InterpolatorUV[2] = M->tVerts[M->tvFace[C2].t[2]];
		for (EdgeCounter = 0 ; EdgeCounter < 3 ; EdgeCounter ++)
		{
			if (GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . FaceA == C2)
			{
				if (GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . PointA == GETCMPFACE(C2).Index[EdgeCounter])
				{
					GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . UVIndexA_FaceA = GETCMPFACE(C2).UVIndex[EdgeCounter];
					GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . UVIndexB_FaceA = GETCMPFACE(C2).UVIndex[(EdgeCounter + 1) % 3];
				}
				else
				{
					GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . UVIndexB_FaceA = GETCMPFACE(C2).UVIndex[EdgeCounter];
					GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . UVIndexA_FaceA = GETCMPFACE(C2).UVIndex[(EdgeCounter + 1) % 3];
				}
			} 
			else
			{
				if (GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . PointA == GETCMPFACE(C2).Index[EdgeCounter])
				{
					GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . UVIndexA_FaceB = GETCMPFACE(C2).UVIndex[EdgeCounter];
					GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . UVIndexB_FaceB = GETCMPFACE(C2).UVIndex[(EdgeCounter + 1) % 3];
				}
				else
				{
					GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . UVIndexB_FaceB = GETCMPFACE(C2).UVIndex[EdgeCounter];
					GETCMPEDGE(GETCMPFACE(C2).Edge[EdgeCounter]) . UVIndexA_FaceB = GETCMPFACE(C2).UVIndex[(EdgeCounter + 1) % 3];
				}
			} 
		}
END_UV
	}//*/
    for(C2 = 0; C2 < (unsigned long) GETNBCMPEDGES; C2++)
    {
		GETCMPEDGE(C2) . UVAreShare = 1;
BEGIN_UV
		if ((GETCMPEDGE(C2) . UVIndexA_FaceA == GETCMPEDGE(C2) . UVIndexA_FaceB) &&
			(GETCMPEDGE(C2) . UVIndexB_FaceA == GETCMPEDGE(C2) . UVIndexB_FaceB))
			GETCMPEDGE(C2) . UVAreShare = 1;
		else 
			GETCMPEDGE(C2) . UVAreShare = 0;

		GETCMPEDGE(C2) . NewUVIndexA_FaceA = GETCMPEDGE(C2) . UVIndexA_FaceA ;
		GETCMPEDGE(C2) . NewUVIndexA_FaceB = GETCMPEDGE(C2) . UVIndexA_FaceB ;
		GETCMPEDGE(C2) . NewUVIndexA_FaceA = GETCMPEDGE(C2) . UVIndexA_FaceA ;
		GETCMPEDGE(C2) . NewUVIndexA_FaceB = GETCMPEDGE(C2) . UVIndexA_FaceB ;
		GETCMPEDGE(C2) . NewUVIndexB_FaceA = GETCMPEDGE(C2) . UVIndexB_FaceA ;
		GETCMPEDGE(C2) . NewUVIndexB_FaceB = GETCMPEDGE(C2) . UVIndexB_FaceB ;
		GETCMPEDGE(C2) . NewUVIndexB_FaceA = GETCMPEDGE(C2) . UVIndexB_FaceA ;
		GETCMPEDGE(C2) . NewUVIndexB_FaceB = GETCMPEDGE(C2) . UVIndexB_FaceB ;
END_UV
    }
	/* ================================================================================================ */
    /* END Compute the mesh connectivity														*/
	/* ================================================================================================ */

	/* ================================================================================================ */
    /* BEGIN EDGE TO SUBDIVIDE DETECTION																*/
	/* ================================================================================================ */
	pblock->GetValue(PB_CHK1, t, Detect, iv);
	if (Detect)
	{
		for(C2 = 0; C2 < (unsigned long) GETNBCMPEDGES; C2++)
		{
			GETCMPEDGE(C2) . HaveToBeSubdivide = 0;
			if ((GETCMPEDGE(C2) . FaceA != 0xffffffff) && (GETCMPEDGE(C2) . FaceB != 0xffffffff))
				if ((M->faces[GETCMPEDGE(C2) . FaceA].smGroup & M->faces[GETCMPEDGE(C2) . FaceB].smGroup ) == 0)
					GETCMPEDGE(C2) . HaveToBeSubdivide = 1;
		}
	}
	pblock->GetValue(PB_CHK2, t, Detect , iv);
	if (Detect)
	{
		for(C2 = 0; C2 < (unsigned long) GETNBCMPFACES; C2++)
		{
			if (M->edgeSel[C2 * 3 + 0]) GETCMPEDGE(GETCMPFACE(C2).Edge[0]) . HaveToBeSubdivide = 1;
			if (M->edgeSel[C2 * 3 + 1]) GETCMPEDGE(GETCMPFACE(C2).Edge[1]) . HaveToBeSubdivide = 1;
			if (M->edgeSel[C2 * 3 + 2]) GETCMPEDGE(GETCMPFACE(C2).Edge[2]) . HaveToBeSubdivide = 1;
		}//*/
	}
	pblock->GetValue(PB_CHK3, t, Detect , iv);
	if (Detect)
	{
		for(C2 = 0; C2 < (unsigned long) GETNBCMPFACES; C2++)
		{
			if (M->faces[C2].flags & EDGE_A) GETCMPEDGE(GETCMPFACE(C2).Edge[0]) . HaveToBeSubdivide = 1;
			if (M->faces[C2].flags & EDGE_B) GETCMPEDGE(GETCMPFACE(C2).Edge[1]) . HaveToBeSubdivide = 1;
			if (M->faces[C2].flags & EDGE_C) GETCMPEDGE(GETCMPFACE(C2).Edge[2]) . HaveToBeSubdivide = 1;
		}//*/
	}
	pblock->GetValue(PB_CHK4, t, Detect , iv);
	if (Detect)
	{
		for(C2 = 0; C2 < (unsigned long) GETNBCMPEDGES; C2++)
		{
			Point3 stN1,stN2;
			if ((GETCMPEDGE(C2) . FaceA != 0xffffffff) && (GETCMPEDGE(C2) . FaceB != 0xffffffff))
			{
				stN1 = Normalize((M-> verts[M->faces[GETCMPEDGE(C2) . FaceA]. v[1]] - M-> verts[M->faces[GETCMPEDGE(C2) . FaceA]. v[0]]) ^
								 (M-> verts[M->faces[GETCMPEDGE(C2) . FaceA]. v[2]] - M-> verts[M->faces[GETCMPEDGE(C2) . FaceA]. v[0]]));
				stN2 = Normalize((M-> verts[M->faces[GETCMPEDGE(C2) . FaceB]. v[1]] - M-> verts[M->faces[GETCMPEDGE(C2) . FaceB]. v[0]]) ^
								 (M-> verts[M->faces[GETCMPEDGE(C2) . FaceB]. v[2]] - M-> verts[M->faces[GETCMPEDGE(C2) . FaceB]. v[0]]));
				if (DotProd(stN2, stN1) < fAutodetectThresh)
					GETCMPEDGE(C2) . HaveToBeSubdivide = 1;
			}
		}
	}
	pblock->GetValue(PB_CHK5, t, Detect , iv);
	for(C2 = 0; C2 < (unsigned long) GETNBCMPFACES; C2++)
	{
		if (!Detect) M->faces[C2].flags &= ~(EDGE_A | EDGE_B | EDGE_C);
		if (GETCMPEDGE(GETCMPFACE(C2).Edge[0]) . HaveToBeSubdivide) M->faces[C2].flags |= EDGE_A;
		if (GETCMPEDGE(GETCMPFACE(C2).Edge[1]) . HaveToBeSubdivide) M->faces[C2].flags |= EDGE_B;
		if (GETCMPEDGE(GETCMPFACE(C2).Edge[2]) . HaveToBeSubdivide) M->faces[C2].flags |= EDGE_C;
	}//*/
	/* ================================================================================================ */
    /* END EDGE TO SUBDIVIDE DETECTION																	*/
	/* ================================================================================================ */
	
	lNewNumberOfPoints = 0;
	ulNewNumberOfFaces  = 0;
	lNewNumberOfUV = 0;

	/* ================================================================================================ */
	/* Begin Compute new number of Points & faces 														*/
	/*	(It will also compute Cut value)																*/
	/* ================================================================================================ */
	/* Cut edge & CutFaces */
	for(C2 = 0; C2 < (unsigned long) GETNBCMPFACES; C2++)
	{
		unsigned long LocaCounter;
		unsigned long *p_ul_Local;
		Point3 stVectorNormale;
		stVectorNormale = Normalize(
				(M -> verts[GETCMPFACE(C2) . Index[1]] - M -> verts[GETCMPFACE(C2) . Index[0]]) ^
				(M -> verts[GETCMPFACE(C2) . Index[2]] - M -> verts[GETCMPFACE(C2) . Index[0]]));
		for (LocaCounter = 0 ; LocaCounter < 3 ; LocaCounter ++ )
		{
			if (GETCMPEDGE(GETCMPFACE(C2) . Edge[LocaCounter]) . HaveToBeSubdivide)
			{
				Point3 stVectorGradient;
				float fFLoc;
				stVectorGradient = Normalize(
					stVectorNormale ^ 
					(M -> verts[GetSuperIndex(&GETCMPFACE(C2), 0x00 | (LocaCounter << 4))] - 
					 M -> verts[GetSuperIndex(&GETCMPFACE(C2), 0x00 | (((LocaCounter + 1) % 3) << 4))]));
				p_ul_Local = GetSuperIndex_Adress(&GETCMPFACE(C2), 0x03 | (LocaCounter << 4));
				*p_ul_Local = 1;
				fFLoc = f_ChamferLenght / (float)fabs(DotProd(
					stVectorGradient , (
					M -> verts[GetSuperIndex(&GETCMPFACE(C2), 0x00 | (LocaCounter << 4))] - 
					M -> verts[GetSuperIndex(&GETCMPFACE(C2), 0x00 | (((LocaCounter + 2) % 3) << 4))])));
				if (*(float *)(p_ul_Local + 1) == 0.0f)
					*(float *)(p_ul_Local + 1) = fFLoc;
				else
				{
					if (*(float *)(p_ul_Local + 1) > fFLoc)
						*(float *)(p_ul_Local + 1) = fFLoc;
				}

				p_ul_Local = GetSuperIndex_Adress(&GETCMPFACE(C2), 0x01 | (((LocaCounter + 1) % 3) << 4));
				*p_ul_Local = 1;
				fFLoc = f_ChamferLenght / (float)fabs(DotProd(
					stVectorGradient , (
					M -> verts[GetSuperIndex(&GETCMPFACE(C2), 0x00 | (((LocaCounter + 2) % 3) << 4))] - 
					M -> verts[GetSuperIndex(&GETCMPFACE(C2), 0x00 | (((LocaCounter + 1) % 3) << 4))])));
				if (*(float *)(p_ul_Local + 1) == 0.0f)
					*(float *)(p_ul_Local + 1) = fFLoc;
				else
				{
					if (*(float *)(p_ul_Local + 1) > fFLoc)
						*(float *)(p_ul_Local + 1) = fFLoc;
				}

			}
		}
		for (LocaCounter = 0 ; LocaCounter < 3 ; LocaCounter ++ )
		{
			Point3 stV1,stV2,stV1p,stV2p;
			float fProjDist,fProjDistMAX;
			float fUVPercent;
			if ((GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 0) % 3)]) . HaveToBeSubdivide) && 
				(GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 2) % 3)]) . HaveToBeSubdivide))
			{
				GETCMPFACE(C2) . NewIndex[LocaCounter] = SaveNP + (lNewNumberOfPoints++);
				stV1 = M -> verts[GETCMPFACE(C2) . Index[((LocaCounter + 1) % 3)]] - M -> verts[GETCMPFACE(C2) . Index[LocaCounter]];
				stV2 = M -> verts[GETCMPFACE(C2) . Index[((LocaCounter + 2) % 3)]] - M -> verts[GETCMPFACE(C2) . Index[LocaCounter]];
				stV1p = Normalize(stV1);
				stV2p = Normalize(stV2);
				stV2p = Normalize(stV1p + stV2p);
				fProjDist = f_ChamferLenght / Length(stV1p ^ stV2p);
				fProjDistMAX = 2.0f / ( 1.0f / DotProd(stV1,stV2p) + 1.0f / DotProd(stV2,stV2p) ) ;
				fUVPercent = fProjDist / fProjDistMAX; 
				fProjDistMAX *= 0.95f;
				if (fProjDist > fProjDistMAX) fProjDist = fProjDistMAX;
				/* Compute point generated from Triangle  */
				GETCMPFACE(C2) . Interpolator[LocaCounter] = 
					M -> verts[GETCMPFACE(C2) . Index[LocaCounter]] + 
					Normalize(
					Normalize(M -> verts[GETCMPFACE(C2) . Index[((LocaCounter + 1) % 3)]] - M -> verts[GETCMPFACE(C2) . Index[LocaCounter]]) + 
					Normalize(M -> verts[GETCMPFACE(C2) . Index[((LocaCounter + 2) % 3)]] - M -> verts[GETCMPFACE(C2) . Index[LocaCounter]])) *
					fProjDist;
BEGIN_VC
				stV1 = M -> vertCol[GETCMPFACE(C2) . Index[((LocaCounter + 1) % 3)]] - M -> vertCol[GETCMPFACE(C2) . Index[LocaCounter]];
				stV2 = M -> vertCol[GETCMPFACE(C2) . Index[((LocaCounter + 2) % 3)]] - M -> vertCol[GETCMPFACE(C2) . Index[LocaCounter]];
				stV1p = Normalize(stV1);
				stV2p = Normalize(stV2);
				stV2p = Normalize(stV1p + stV2p);
				fProjDist = 2.0f / ( 1.0f / DotProd(stV1,stV2p) + 1.0f / DotProd(stV2,stV2p) ) ;
				if (fUVPercent > 0.95f)  fUVPercent = 1.0f;
				fProjDist *= fUVPercent;
				/* Compute point generated from Triangle  */
				GETCMPFACE(C2) . InterpolatorVC[LocaCounter] = 
					M -> vertCol[GETCMPFACE(C2) . Index[LocaCounter]] + 
					Normalize(
					Normalize(M -> vertCol[GETCMPFACE(C2) . Index[((LocaCounter + 1) % 3)]] - M -> vertCol[GETCMPFACE(C2) . Index[LocaCounter]]) + 
					Normalize(M -> vertCol[GETCMPFACE(C2) . Index[((LocaCounter + 2) % 3)]] - M -> vertCol[GETCMPFACE(C2) . Index[LocaCounter]])) *
					fProjDist;
END_VC
BEGIN_UV
				GETCMPFACE(C2) . NewUVIndex[LocaCounter] = SaveUV + (lNewNumberOfUV++);
				stV1 = M -> tVerts[GETCMPFACE(C2) . UVIndex[((LocaCounter + 1) % 3)]] - M -> tVerts[GETCMPFACE(C2) . UVIndex[LocaCounter]];
				stV2 = M -> tVerts[GETCMPFACE(C2) . UVIndex[((LocaCounter + 2) % 3)]] - M -> tVerts[GETCMPFACE(C2) . UVIndex[LocaCounter]];
				stV1p = Normalize(stV1);
				stV2p = Normalize(stV2);
				stV2p = Normalize(stV1p + stV2p);
				fProjDist = 2.0f / ( 1.0f / DotProd(stV1,stV2p) + 1.0f / DotProd(stV2,stV2p) ) ;
				if (fUVPercent > 0.95f)  fUVPercent = 1.0f;
				fProjDist *= fUVPercent;
				/* Compute point generated from Triangle  */
				GETCMPFACE(C2) . InterpolatorUV[LocaCounter] = 
					M -> tVerts[GETCMPFACE(C2) . UVIndex[LocaCounter]] + 
					Normalize(
					Normalize(M -> tVerts[GETCMPFACE(C2) . UVIndex[((LocaCounter + 1) % 3)]] - M -> tVerts[GETCMPFACE(C2) . UVIndex[LocaCounter]]) + 
					Normalize(M -> tVerts[GETCMPFACE(C2) . UVIndex[((LocaCounter + 2) % 3)]] - M -> tVerts[GETCMPFACE(C2) . UVIndex[LocaCounter]])) *
					fProjDist;

END_UV
			}
		}
	}//*/

	/* detect corner points */
    for(C2 = 0; C2 < (unsigned long) GETNBCMPEDGES; C2++)
    {
		if (GETCMPEDGE(C2) . HaveToBeSubdivide)
		{
			GETCMPVERT(GETCMPEDGE(C2) . PointA) . IStouchADivisibleEdge ++;
			GETCMPVERT(GETCMPEDGE(C2) . PointB) . IStouchADivisibleEdge ++;
		}
    }
	/* Cut Edge fromn corner points  */
	for ( C2 = 0 ; C2 < (unsigned long) GETNBCMPEDGES ; C2++ )
	{
		if (GETCMPEDGE(C2) . HaveToBeSubdivide)
		{
			if (GETCMPVERT(GETCMPEDGE(C2).PointA).IStouchADivisibleEdge > 2)
			{
				CUTEDGE(M , &GETCMPEDGE(C2) , 0 /* -> AB */);
				GETCMPEDGE(C2).CutA = 1;
			}
			if (GETCMPVERT(GETCMPEDGE(C2).PointB).IStouchADivisibleEdge > 2)
			{
				CUTEDGE(M , &GETCMPEDGE(C2) , 1 /* -> BA */);
				GETCMPEDGE(C2).CutB = 1;
			}
		} else 
		{
			if (GETCMPVERT(GETCMPEDGE(C2).PointA).IStouchADivisibleEdge)
			{
				CUTEDGE(M , &GETCMPEDGE(C2) , 0 /* -> AB */);
				GETCMPEDGE(C2).CutA = 1;
			}
			if (GETCMPVERT(GETCMPEDGE(C2).PointB).IStouchADivisibleEdge)
			{
				CUTEDGE(M , &GETCMPEDGE(C2) , 1 /* -> BA */);
				GETCMPEDGE(C2).CutB = 1;
			}
		}

	}//*/

	/* Recopmpute Edge New Indexes */
	for(C2 = 0; C2 < (unsigned long) GETNBCMPEDGES; C2++)
	{
		if (GETCMPEDGE(C2).CutA)
			GETCMPEDGE(C2).CutA = SaveNP + (lNewNumberOfPoints++);
		else 
			GETCMPEDGE(C2).CutA = GETCMPEDGE(C2).PointA;
		if (GETCMPEDGE(C2).CutB)
			GETCMPEDGE(C2).CutB = SaveNP + (lNewNumberOfPoints++);
		else 
			GETCMPEDGE(C2).CutB = GETCMPEDGE(C2).PointB;
		/* Edges Overflow treatment */
		if (GETCMPEDGE(C2).LenghtCutA + GETCMPEDGE(C2).LenghtCutB >= 1.0f)
		{
			if ((GETCMPEDGE(C2).CutA != GETCMPEDGE(C2).PointA) &&
				(GETCMPEDGE(C2).CutB != GETCMPEDGE(C2).PointB))
			{
				GETCMPEDGE(C2).CutA = GETCMPEDGE(C2).CutB;
				GETCMPEDGE(C2).LenghtCutA = GETCMPEDGE(C2).LenghtCutB = 0.5f;
			} else
			if (GETCMPEDGE(C2).CutA != GETCMPEDGE(C2).PointA)
			{
				GETCMPEDGE(C2).CutA = GETCMPEDGE(C2).PointB;
				GETCMPEDGE(C2).LenghtCutA = 1.0f;
			} else 
			if (GETCMPEDGE(C2).CutB != GETCMPEDGE(C2).PointB)
			{
				GETCMPEDGE(C2).CutB = GETCMPEDGE(C2).PointA;
				GETCMPEDGE(C2).LenghtCutB = 1.0f;
			}
		}
BEGIN_UV
		if (GETCMPEDGE(C2).CutA != GETCMPEDGE(C2).PointA)
		{
			if (GETCMPEDGE(C2).UVAreShare)
			{
				GETCMPEDGE(C2).NewUVIndexA_FaceB = 
				GETCMPEDGE(C2).NewUVIndexA_FaceA = SaveUV + (lNewNumberOfUV++);
			}
			else
			{
				GETCMPEDGE(C2).NewUVIndexA_FaceB = SaveUV + (lNewNumberOfUV++);
				GETCMPEDGE(C2).NewUVIndexA_FaceA = SaveUV + (lNewNumberOfUV++);
			}
		}
		if (GETCMPEDGE(C2).CutB != GETCMPEDGE(C2).PointB)
		{
			if (GETCMPEDGE(C2).UVAreShare)
			{
				GETCMPEDGE(C2). NewUVIndexB_FaceB = 
				GETCMPEDGE(C2). NewUVIndexB_FaceA = SaveUV + (lNewNumberOfUV++);
			}
			else
			{
				GETCMPEDGE(C2). NewUVIndexB_FaceB = SaveUV + (lNewNumberOfUV++);
				GETCMPEDGE(C2). NewUVIndexB_FaceA = SaveUV + (lNewNumberOfUV++);
			}
		}
END_UV
	}//*/
	/* -------------------------------------------------------------------------------------------------------------*/
	/* Begin QUAD Correction																						*/
	/* -------------------------------------------------------------------------------------------------------------*/
BEGIN_QUAD
	for(C2 = 0; C2 < (unsigned long) GETNBCMPFACES; C2++)
	{
		unsigned long LocaCounter;
		CMPF_FaceComplement *st_LocalFC;
		unsigned long *p_ul_Local;
		unsigned long *p_ul_Local2;
		st_LocalFC = &GETCMPFACE(C2);
		for (LocaCounter = 0 ; LocaCounter < 3 ; LocaCounter ++ )
		{
			if ((!GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 0) % 3)]) . HaveToBeSubdivide) &&
				(!GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 2) % 3)]) . HaveToBeSubdivide))
			{
				if ((!GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 0) % 3)]) . IsVisible)	&&
					(GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 2) % 3)]) . IsVisible)	&&
					( GETCMPEDGE(GETCMPFACE(C2) . Edge[LocaCounter]) . UVAreShare))
				{
					p_ul_Local =  GetSuperIndex_Adress	(st_LocalFC, 0x01 | (LocaCounter << 4));
					p_ul_Local2 = GetSuperIndex_Adress  (st_LocalFC , 0x03 | (LocaCounter << 4));
					*p_ul_Local = *p_ul_Local2;
					*(p_ul_Local + 2) = 1; /* IS NOT ON LINE */
BEGIN_UV
						*(p_ul_Local + 3) = *(p_ul_Local2 + 3); /* UV !! */
						*(p_ul_Local + 4) = *(p_ul_Local2 + 4); /* UV !! */
END_UV
				}
				if ((GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 0) % 3)]) . IsVisible)	&&
					(!GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 2) % 3)]) . IsVisible)	&&
					( GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 2) % 3)]) . UVAreShare) )
				{
					p_ul_Local =  GetSuperIndex_Adress	(st_LocalFC, 0x03 | (LocaCounter << 4));
					p_ul_Local2 = GetSuperIndex_Adress  (st_LocalFC , 0x01 | (LocaCounter << 4));
					*p_ul_Local = *p_ul_Local2;
					*(p_ul_Local + 2) = 1; /* IS NOT ON LINE */
BEGIN_UV
						*(p_ul_Local + 3) = *(p_ul_Local2 + 3); /* UV !! */
						*(p_ul_Local + 4) = *(p_ul_Local2 + 4); /* UV !! */
END_UV
				}
				if ((!GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 0) % 3)]) . IsVisible)	&&
					(!GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 2) % 3)]) . IsVisible)	&&
					( GETCMPEDGE(GETCMPFACE(C2) . Edge[LocaCounter]) . UVAreShare)				&&	
					( GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 2) % 3)]) . UVAreShare) )
				{
					p_ul_Local  = GetSuperIndex_Adress(st_LocalFC , 0x03 | (LocaCounter << 4));
					p_ul_Local2 = GetSuperIndex_Adress(st_LocalFC , 0x01 | (LocaCounter << 4));
					if (!(*(p_ul_Local + 2) & 2))
					{
						*p_ul_Local = *p_ul_Local2;
						*(p_ul_Local + 2) |= 2;
						*(p_ul_Local2 + 2) |= 2;
BEGIN_UV
						*(p_ul_Local + 3) = *(p_ul_Local2 + 3); /* UV !! */
						*(p_ul_Local + 4) = *(p_ul_Local2 + 4); /* UV !! */
END_UV
					}
				}//*/
			}
		}
	}//*/
END_QUAD
	/* -------------------------------------------------------------------------------------------------------------*/
	/* END QUAD Correction																							*/
	/* -------------------------------------------------------------------------------------------------------------*/
	/* Compute new number of faces*/
	for(C2 = 0; C2 < (unsigned long) GETNBCMPFACES; C2++)
	{
		unsigned long LocalCounter;
		CMPF_FaceComplement *st_LocalFC;
		unsigned long *p_ul_Local;
		st_LocalFC = &GETCMPFACE(C2);
		for (LocalCounter = 0 ; LocalCounter < 3 ; LocalCounter ++)
		{
			p_ul_Local =GetSuperIndex_Adress(st_LocalFC, 0x02 | (LocalCounter << 4));
			if (*p_ul_Local == GetSuperIndex(st_LocalFC, 0x00 | (LocalCounter << 4)))
			{
				if (GETCMPEDGE(GETCMPFACE(C2) . Edge[LocalCounter]) . HaveToBeSubdivide)
					*p_ul_Local =  GetSuperIndex(st_LocalFC, 0x03 | (LocalCounter << 4));
				else 
					*p_ul_Local =  GetSuperIndex(st_LocalFC, 0x01 | (LocalCounter << 4));
			}
			if (*p_ul_Local == GetSuperIndex(st_LocalFC, 0x00 | (LocalCounter << 4)))
				*p_ul_Local =  GetSuperIndex(st_LocalFC, 0x03 | (LocalCounter << 4));
			if (*p_ul_Local == GetSuperIndex(st_LocalFC, 0x00 | (LocalCounter << 4)))
				*p_ul_Local =  GetSuperIndex(st_LocalFC, 0x01 | (LocalCounter << 4));
		}

		/* Corners */
		if (IsValidTriangle_SI(st_LocalFC,0x00,0x01,0x03)) ulNewNumberOfFaces++;
		if (IsValidTriangle_SI(st_LocalFC,0x01,0x02,0x03)) ulNewNumberOfFaces++;
		if (IsValidTriangle_SI(st_LocalFC,0x10,0x11,0x13)) ulNewNumberOfFaces++;
		if (IsValidTriangle_SI(st_LocalFC,0x11,0x12,0x13)) ulNewNumberOfFaces++;
		if (IsValidTriangle_SI(st_LocalFC,0x20,0x21,0x23)) ulNewNumberOfFaces++;
		if (IsValidTriangle_SI(st_LocalFC,0x21,0x22,0x23)) ulNewNumberOfFaces++;
		/* Edges */
		if (IsValidTriangle_SI(st_LocalFC,0x02,0x01,0x13)) ulNewNumberOfFaces++;
		if (IsValidTriangle_SI(st_LocalFC,0x02,0x13,0x12)) ulNewNumberOfFaces++;
		if (IsValidTriangle_SI(st_LocalFC,0x12,0x11,0x23)) ulNewNumberOfFaces++;
		if (IsValidTriangle_SI(st_LocalFC,0x12,0x23,0x22)) ulNewNumberOfFaces++;
		if (IsValidTriangle_SI(st_LocalFC,0x22,0x21,0x03)) ulNewNumberOfFaces++;
		if (IsValidTriangle_SI(st_LocalFC,0x22,0x03,0x02)) ulNewNumberOfFaces++;
		/*
		Except Central face !!!
		*/
	}//*/

	M -> setNumVerts	(SaveNP + lNewNumberOfPoints	, TRUE, TRUE);
	M -> setNumFaces	(SaveNT + ulNewNumberOfFaces	, TRUE, TRUE);
BEGIN_VC
	M -> setNumVertCol	(SaveNP + lNewNumberOfPoints	, TRUE);
	M -> setNumVCFaces	(SaveNT + ulNewNumberOfFaces	, TRUE , SaveNT);
END_VC
BEGIN_UV
	M -> setNumTVerts	(SaveUV + lNewNumberOfUV		, TRUE);
	M -> setNumTVFaces	(SaveNT + ulNewNumberOfFaces	, TRUE , SaveNT);//*/
END_UV
	/* ================================================================================================ */
	/* end Compute new number of Points & faces 														*/
	/* ================================================================================================ */
	/* ================================================================================================ */
	/* Compute new Points						 														*/
	/* ================================================================================================ */
	for (C2 = 0; C2 < (unsigned long) GETNBCMPFACES; C2++)
	{
		unsigned long LocaCounter;
		for (LocaCounter = 0 ; LocaCounter < 3 ; LocaCounter ++ )
		{
			if ((GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 0) % 3)]) . HaveToBeSubdivide) && 
				(GETCMPEDGE(GETCMPFACE(C2) . Edge[((LocaCounter + 2) % 3)]) . HaveToBeSubdivide))
			{
				M -> verts[GETCMPFACE(C2) . NewIndex[LocaCounter]] = 
					GETCMPFACE(C2) . Interpolator[LocaCounter];
BEGIN_UV
				M -> tVerts[GETCMPFACE(C2) . NewUVIndex[LocaCounter]] = 
					GETCMPFACE(C2) . InterpolatorUV[LocaCounter];//*/
END_UV
BEGIN_VC
				M -> vertCol[GETCMPFACE(C2) . NewIndex[LocaCounter]] = 
					GETCMPFACE(C2) . InterpolatorVC[LocaCounter];//*/
END_VC
			}
		}
	}
	for(C2 = 0; C2 < (unsigned long) GETNBCMPEDGES; C2++)
	{
		if (GETCMPEDGE(C2).CutA != GETCMPEDGE(C2).PointA)
		{
			if (!(GETCMPEDGE(C2).AIsNotOnLine & 1))
			{
				M->verts[GETCMPEDGE(C2).CutA] = 
					M->verts[GETCMPEDGE(C2).PointA] + (
					M->verts[GETCMPEDGE(C2).PointB] -
					M->verts[GETCMPEDGE(C2).PointA] ) * 
					GETCMPEDGE(C2).LenghtCutA;
BEGIN_VC
				M-> vertCol[GETCMPEDGE(C2).CutA] = 
					M->vertCol[GETCMPEDGE(C2).PointA] + (
					M->vertCol[GETCMPEDGE(C2).PointB] -
					M->vertCol[GETCMPEDGE(C2).PointA] ) * 
					GETCMPEDGE(C2).LenghtCutA;
END_VC
BEGIN_UV
				M->tVerts[GETCMPEDGE(C2).NewUVIndexA_FaceA] = 
					M->tVerts[GETCMPEDGE(C2).UVIndexA_FaceA] + (
					M->tVerts[GETCMPEDGE(C2).UVIndexB_FaceA] -
					M->tVerts[GETCMPEDGE(C2).UVIndexA_FaceA] ) * 
					GETCMPEDGE(C2).LenghtCutA;
				M->tVerts[GETCMPEDGE(C2).NewUVIndexA_FaceB] = 
					M->tVerts[GETCMPEDGE(C2).UVIndexA_FaceB] + (
					M->tVerts[GETCMPEDGE(C2).UVIndexB_FaceB] -
					M->tVerts[GETCMPEDGE(C2).UVIndexA_FaceB] ) * 
					GETCMPEDGE(C2).LenghtCutA;//*/
END_UV
			}
		}
		if (GETCMPEDGE(C2).CutB != GETCMPEDGE(C2).PointB)
		{
			if (!(GETCMPEDGE(C2).BIsNotOnLine & 1))
			{
				M-> verts[GETCMPEDGE(C2).CutB] = 
					M->verts[GETCMPEDGE(C2).PointB] + (
					M->verts[GETCMPEDGE(C2).PointA] -
					M->verts[GETCMPEDGE(C2).PointB] ) * 
					GETCMPEDGE(C2).LenghtCutB;
BEGIN_VC
				M-> vertCol[GETCMPEDGE(C2).CutB] = 
					M->vertCol[GETCMPEDGE(C2).PointB] + (
					M->vertCol[GETCMPEDGE(C2).PointA] -
					M->vertCol[GETCMPEDGE(C2).PointB] ) * 
					GETCMPEDGE(C2).LenghtCutB;
END_VC
BEGIN_UV
				M-> tVerts[GETCMPEDGE(C2).NewUVIndexB_FaceA] = 
					M->tVerts[GETCMPEDGE(C2).UVIndexB_FaceA] + (
					M->tVerts[GETCMPEDGE(C2).UVIndexA_FaceA] -
					M->tVerts[GETCMPEDGE(C2).UVIndexB_FaceA] ) * 
					GETCMPEDGE(C2).LenghtCutB;
				M-> tVerts[GETCMPEDGE(C2).NewUVIndexB_FaceB] = 
					M->tVerts[GETCMPEDGE(C2).UVIndexB_FaceB] + (
					M->tVerts[GETCMPEDGE(C2).UVIndexA_FaceB] -
					M->tVerts[GETCMPEDGE(C2).UVIndexB_FaceB] ) * 
					GETCMPEDGE(C2).LenghtCutB;//*/
END_UV
			}
		}
	}//*/
	/* ================================================================================================ */
	/* End Compute new Points						 														*/
	/* ================================================================================================ */

	/* ================================================================================================ */
    /* BEGIN RETRIANGULATION																			*/
	/* ================================================================================================ */
	{
		unsigned long Gololo;
		Gololo = SaveNT + ulNewNumberOfFaces;
		ulNewNumberOfFaces = SaveNT;
		for (C2 = 0; C2 < (unsigned long) ulNewNumberOfFaces; C2++)
		{
			CMPF_FaceComplement *st_LocalFC;
			unsigned long ulFlags;
			st_LocalFC = &GETCMPFACE(C2);
			ulFlags = M->faces[C2].flags;
			ValidateTriangle_SI(M,st_LocalFC,0x02,0x12,0x22,C2,ulFlags);
			ulFlags &= ~(EDGE_A | EDGE_B | EDGE_C);
			/* Corners */
			if (ValidateTriangle_SI(M,st_LocalFC,0x00,0x01,0x03,SaveNT,ulFlags | EDGE_A | EDGE_B | EDGE_C)) SaveNT++;
			if (ValidateTriangle_SI(M,st_LocalFC,0x01,0x02,0x03,SaveNT,ulFlags | EDGE_A | EDGE_B | EDGE_C)) SaveNT++;
			if (ValidateTriangle_SI(M,st_LocalFC,0x10,0x11,0x13,SaveNT,ulFlags | EDGE_A | EDGE_B | EDGE_C)) SaveNT++;
			if (ValidateTriangle_SI(M,st_LocalFC,0x11,0x12,0x13,SaveNT,ulFlags | EDGE_A | EDGE_B | EDGE_C)) SaveNT++;
			if (ValidateTriangle_SI(M,st_LocalFC,0x20,0x21,0x23,SaveNT,ulFlags | EDGE_A | EDGE_B | EDGE_C)) SaveNT++;
			if (ValidateTriangle_SI(M,st_LocalFC,0x21,0x22,0x23,SaveNT,ulFlags | EDGE_A | EDGE_B | EDGE_C)) SaveNT++;
			/* Edges */
			if (ValidateTriangle_SI(M,st_LocalFC,0x02,0x01,0x13,SaveNT,ulFlags | EDGE_B)	) SaveNT++;
			if (ValidateTriangle_SI(M,st_LocalFC,0x02,0x13,0x12,SaveNT,ulFlags | 0)			) SaveNT++;
			if (ValidateTriangle_SI(M,st_LocalFC,0x12,0x11,0x23,SaveNT,ulFlags | EDGE_B)	) SaveNT++;
			if (ValidateTriangle_SI(M,st_LocalFC,0x12,0x23,0x22,SaveNT,ulFlags | 0)			) SaveNT++;
			if (ValidateTriangle_SI(M,st_LocalFC,0x22,0x21,0x03,SaveNT,ulFlags | EDGE_B)	) SaveNT++;
			if (ValidateTriangle_SI(M,st_LocalFC,0x22,0x03,0x02,SaveNT,ulFlags | 0)			) SaveNT++;

		}
		M -> setNumFaces	(SaveNT , TRUE, TRUE);
BEGIN_UV
		M -> setNumTVFaces	(SaveNT , TRUE , Gololo);
END_UV
BEGIN_VC
		M -> setNumVCFaces	(SaveNT , FALSE);
END_VC
	}
	/* ================================================================================================ */
    /* END RETRIANGULATION																			*/
	/* ================================================================================================ */
	/* finish ! */
	for(C2 = 0; C2 < (unsigned long) GETNBCMPFACES; C2++)
    {
        M->faces[C2].smGroup = 1;
BEGIN_VC
		M->vcFace[C2].t[0] = M->faces[C2].v[0];
		M->vcFace[C2].t[1] = M->faces[C2].v[1];
		M->vcFace[C2].t[2] = M->faces[C2].v[2];
END_VC
	}
	/* This will remove all triangle with duplicated indexes */
	M->RemoveIllegalFaces();
	M->RemoveDegenerateFaces();
	M->DeleteIsoVerts();

	free(AllEdges);
	free(AllFaces);
	free(AllVertex);
    /* Compute the new number of triangles */
    os->obj->UpdateValidity(GEOM_CHAN_NUM, FOREVER);
    os->obj->UpdateValidity(GEOM_CHAN_NUM, iv);
    os->obj->UpdateValidity(TOPO_CHAN_NUM, iv);
    os->obj->UpdateValidity(SELECT_CHAN_NUM, iv);
    os->obj->UpdateValidity(SUBSEL_TYPE_CHAN_NUM, iv);
    os->obj->UpdateValidity(TEXMAP_CHAN_NUM, iv);
    os->obj->UpdateValidity(VERT_COLOR_CHAN_NUM, iv);
}


// Face-type dlg user proc
class CmpfrDlgProc : public ParamMapUserDlgProc {
public:
	CHANFREIN *mod;
	CmpfrDlgProc () { mod = NULL; }
	BOOL DlgProc (TimeValue t, IParamMap *map, HWND hWnd,
		UINT msg, WPARAM wParam, LPARAM lParam);
	void DeleteThis () { }
};


BOOL CmpfrDlgProc::DlgProc (TimeValue t, IParamMap *map, HWND hWnd,
							  UINT msg,WPARAM wParam,LPARAM lParam) {
	int detect;
	Interval     iv;
	if (mod == NULL)		return FALSE;

	switch (msg)
	{
		case WM_INITDIALOG:
			mod->pblock->GetValue(PB_CHK1, t, detect , iv);
			if (detect)
				CheckDlgButton( hWnd, IDC_CHECK1 , BST_CHECKED );
			else
				CheckDlgButton( hWnd, IDC_CHECK1 , BST_UNCHECKED );
			mod->pblock->GetValue(PB_CHK2, t, detect , iv);
			if (detect)
				CheckDlgButton( hWnd, IDC_CHECK2 , BST_CHECKED );
			else
				CheckDlgButton( hWnd, IDC_CHECK2 , BST_UNCHECKED );
			mod->pblock->GetValue(PB_CHK3, t, detect , iv);
			if (detect)
				CheckDlgButton( hWnd, IDC_CHECK3 , BST_CHECKED );
			else
				CheckDlgButton( hWnd, IDC_CHECK3 , BST_UNCHECKED );
			mod->pblock->GetValue(PB_CHK4, t, detect , iv);
			if (detect)
				CheckDlgButton( hWnd, IDC_CHECK4 , BST_CHECKED );
			else
				CheckDlgButton( hWnd, IDC_CHECK4 , BST_UNCHECKED );
			mod->pblock->GetValue(PB_CHK5, t, detect , iv);
			if (detect)
				CheckDlgButton( hWnd, IDC_CHECK5 , BST_CHECKED );
			else
				CheckDlgButton( hWnd, IDC_CHECK5 , BST_UNCHECKED );

			break;
	case WM_COMMAND:
		switch (LOWORD (wParam)) 
		{
		case IDC_CHECK1:
			detect = IsDlgButtonChecked(hWnd, IDC_CHECK1);
			mod->pblock->SetValue (PB_CHK1 , t , detect);
			break;
		case IDC_CHECK2:
			detect = IsDlgButtonChecked(hWnd, IDC_CHECK2);
			mod->pblock->SetValue (PB_CHK2 , t , detect);
			break;
		case IDC_CHECK3:
			detect = IsDlgButtonChecked(hWnd, IDC_CHECK3);
			mod->pblock->SetValue (PB_CHK3 , t , detect);
			break;
		case IDC_CHECK4:
			detect = IsDlgButtonChecked(hWnd, IDC_CHECK4);
			mod->pblock->SetValue (PB_CHK4 , t , detect);
			break;
		case IDC_CHECK5:
			detect = IsDlgButtonChecked(hWnd, IDC_CHECK5);
			mod->pblock->SetValue (PB_CHK5 , t , detect);
			break;
		}
		break;
	}
	return FALSE;
}

static CmpfrDlgProc CmpfrDlgProcVAR;
/*
 ===================================================================================================
 ===================================================================================================
 */
void CHANFREIN::BeginEditParams(IObjParam *ip, ULONG flags, Animatable *prev)
{
    this->ip = ip;
    pmapParam = CreateCPParamMap
        (
            descParam,
            PARAMDESC_LENGTH,
            pblock,
            ip,
            hInstance,
            MAKEINTRESOURCE(IDD_PANEL),
            GetString(IDS_PARAMS),
            0
        );
	CmpfrDlgProcVAR.mod = this;
	pmapParam -> SetUserDlgProc  (&CmpfrDlgProcVAR);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void CHANFREIN::EndEditParams(IObjParam *ip, ULONG flags, Animatable *next)
{
    DestroyCPParamMap(pmapParam);
    this->ip = NULL;
}

/*
 ===================================================================================================
    From ReferenceMaker
 ===================================================================================================
 */
RefResult CHANFREIN::NotifyRefChanged
(
    Interval        changeInt,
    RefTargetHandle hTarget,
    PartID          &partID,
    RefMessage      message
)
{
    /* TODO: Add code to handle the various reference changed messages */
    return REF_SUCCEED;
}

/*
 ===================================================================================================
    From Object
 ===================================================================================================
 */
BOOL CHANFREIN::HasUVW(void)
{
    /* TODO: Return whether the object has UVW coordinates or not */
    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void CHANFREIN::SetGenUVW(BOOL sw)
{
    if(sw == HasUVW()) return;

    /* TODO: Set the plugin internal value to sw */
}

/* TODO: This is the callback object used by ILoad::RegisterPostLoadCallback() */
class CHANFREINPostLoadCallback : public PostLoadCallback
{
public:
    ParamBlockPLCB  *cb;
    CHANFREINPostLoadCallback(ParamBlockPLCB *c) { cb = c; }

    /*
     ===============================================================================================
        TODO: This method is called when everything has been loaded
     ===============================================================================================
     */
    void    proc(ILoad *iload) { };
};

/*
 ===================================================================================================
 ===================================================================================================
 */
IOResult CHANFREIN::Load(ILoad *iload)
{
    /* TODO: Add code to allow plugin to load its data */
    return IO_OK;
}

