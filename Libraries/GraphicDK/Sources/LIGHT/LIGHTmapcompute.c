#include "Precomp.h"

#ifdef ACTIVE_EDITORS

#include "SELection/SELection.h"
#include "texture/texfile.h"
#include "texture/texstruct.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/INTersection/INTmain.h" 
#include "TABles/TABles.h"
#include "LIGHT/LIGHTmapstruct.h"
#include "LIGHT/LIGHTmapcompute.h"
#include "LIGHT/LIGHTmap.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGfat.h"
#include "GEOmetric/GEO_LIGHTCUT.h"
#include "TEXture/TEXfile.h"
#include "LIGHT/LIGHTmap.h"
#include "BIGfiles/SAVing/SAVdefs.h"

#define  MATH_Max(a, b)	(((a) > (b)) ? (a) : (b))
#define  MATH_Min(a, b)	(((a) < (b)) ? (a) : (b))

#define LM_Free   free
#define LM_Alloc  malloc

// Defines so I can seamlessly switch between double precision and single precision for LM calculation
// I add some precision problems using float so I tried doubles but I'm still not sure if it's worth it
// it kinda sucks to do this but I've got no other alternative I think...
//#define LM_DOUBLE_PRECISION 
#pragma intrinsic( fabs )
#pragma intrinsic( sqrt )


#ifdef LM_DOUBLE_PRECISION
	typedef double LMReal;
	typedef MATHD_tdst_Vector LM_tdst_Vector;
	typedef MATHD_tdst_Matrix LM_tdst_Matrix;	

	inline  void LM_SubVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA, LM_tdst_Vector* pB) {MATHD_SubVector(pDest, pA, pB);}
	inline  void LM_AddVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA, LM_tdst_Vector* pB) {MATHD_AddVector(pDest, pA, pB);}
	inline  void LM_MulVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA, LMReal val) {MATHD_MulVector(pDest, pA, val);}
	inline  void LM_MulEqualVector(LM_tdst_Vector* pDest, LMReal val) {MATHD_MulEqualVector(pDest,val);}
	inline	void LM_CrossProduct(LM_tdst_Vector* pDest, LM_tdst_Vector* pA, LM_tdst_Vector* pB) {MATHD_CrossProduct(pDest, pA, pB);}
	inline	void LM_NormalizeVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA) {MATHD_NormalizeVector(pDest, pA);}
	inline	void LM_NormalizeEqualVector(LM_tdst_Vector* pDest) {MATHD_NormalizeEqualVector(pDest);}
	inline	LMReal LM_f_NormVector(LM_tdst_Vector* pDest) { return MATHD_f_NormVector(pDest);}
	inline	LMReal LM_f_SqrNormVector(LM_tdst_Vector* pDest) {return MATHD_f_SqrNormVector(pDest);}
	inline	LMReal LM_f_DotProduct(LM_tdst_Vector* pV1, LM_tdst_Vector* pV2) {return MATHD_f_DotProduct(pV1, pV2);}
	inline	void LM_CopyVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA) {MATHD_CopyVector(pDest, pA);}
	inline	void LM_AddEqualVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA) {MATHD_AddEqualVector(pDest, pA);}
	inline	void LM_SubEqualVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA) {MATHD_SubEqualVector(pDest, pA);}
	inline	void LM_TransformVector(LM_tdst_Vector* pDest, LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATHD_TransformVector(pDest, pMatrix, pVect);}
	inline	void LM_TransformVertex(LM_tdst_Vector* pDest, LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATHD_TransformVertex(pDest, pMatrix, pVect);}
	inline	void LM_SetXAxis(LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATHD_SetXAxis(pMatrix, pVect);}
	inline	void LM_SetYAxis(LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATHD_SetYAxis(pMatrix, pVect);}
	inline	void LM_SetZAxis(LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATHD_SetZAxis(pMatrix, pVect);}
	inline	void LM_MakeRotationMatrix_ZAxis(LM_tdst_Matrix* pMatrix, LMReal angle, bool initRestOfMatrix) { MATHD_MakeRotationMatrix_ZAxis(pMatrix, angle, initRestOfMatrix);}
	inline	void LM_SetTranslation(LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATHD_SetTranslation(pMatrix, pVect);}
	inline	void LM_SetScale(LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATHD_SetScale(pMatrix, pVect);}
	inline	void LM_InvertMatrix(LM_tdst_Matrix* pMatrix1, LM_tdst_Matrix* pMatrix2) {MATHD_InvertMatrix(pMatrix1, pMatrix2);}
	inline	void LM_SetIdentityMatrix(LM_tdst_Matrix* pMatrix1) {MATHD_SetIdentityMatrix(pMatrix1);}
	inline	void LM_MulMatrixMatrix(LM_tdst_Matrix* pDest, LM_tdst_Matrix* pMatrix1, LM_tdst_Matrix* pMatrix2) {MATHD_MulMatrixMatrix(pDest, pMatrix1, pMatrix2);}
	inline	void LM_CopyMatrix(LM_tdst_Matrix* pDest, LM_tdst_Matrix* pSrc) {MATHD_CopyMatrix(pDest, pSrc);}
	inline	void LM_CopyFromLMVector(MATH_tdst_Vector* pVect, LM_tdst_Vector* pLMVect) {MATHD_CopyFromVectorD(pVect, pLMVect);}
	inline	void LM_CopyFromLMMatrix(MATH_tdst_Matrix* pMatrix, LM_tdst_Matrix* pLMMatrix) {MATHD_CopyFromMatrixD(pMatrix, pLMMatrix);}
	inline	void LM_CopyToLMVector(LM_tdst_Vector* pLMVect, MATH_tdst_Vector* pVect) {MATHD_CopyVectorD(pLMVect, pVect);}
	inline	void LM_CopyToLMMatrix(LM_tdst_Matrix* pLMMatrix, MATH_tdst_Matrix* pMatrix) {MATHD_CopyMatrixD(pLMMatrix, pMatrix);}
	inline	void LM_InitVector(LM_tdst_Vector* pVect, LMReal x, LMReal y, LMReal z) {MATHD_InitVector(pVect, x, y, z);}
	inline	LMReal LM_Distance(LM_tdst_Vector* pV1, LM_tdst_Vector* pV2) { return MATHD_d_Distance(pV1, pV2);}
	inline  BOOL LM_EqVectorWithEpsilon(LM_tdst_Vector* pV1, LM_tdst_Vector* pV2, float epsilon) { return MATHD_b_EqVectorWithEpsilon(pV1, pV2, epsilon); }
#else
	typedef float LMReal;
	typedef MATH_tdst_Vector	LM_tdst_Vector;
	typedef MATH_tdst_Matrix	LM_tdst_Matrix;

	inline  void LM_SubVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA, LM_tdst_Vector* pB) {MATH_SubVector(pDest, pA, pB);}
	inline  void LM_AddVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA, LM_tdst_Vector* pB) {MATH_AddVector(pDest, pA, pB);}
	inline  void LM_MulVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA, LMReal val) {MATH_MulVector(pDest, pA, val);}
	inline  void LM_MulEqualVector(LM_tdst_Vector* pDest, LMReal val) {MATH_MulEqualVector(pDest,val);}
	inline	void LM_CrossProduct(LM_tdst_Vector* pDest, LM_tdst_Vector* pA, LM_tdst_Vector* pB) {MATH_CrossProduct(pDest, pA, pB);}
	inline	void LM_NormalizeVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA) {MATH_NormalizeVector(pDest, pA);}
	inline	void LM_NormalizeEqualVector(LM_tdst_Vector* pDest) {MATH_NormalizeEqualVector(pDest);}
	inline	LMReal LM_f_NormVector(LM_tdst_Vector* pDest) { return MATH_f_NormVector(pDest);}
	inline	LMReal LM_f_SqrNormVector(LM_tdst_Vector* pDest) {return MATH_f_SqrNormVector(pDest);}
	inline	LMReal LM_f_DotProduct(LM_tdst_Vector* pV1, LM_tdst_Vector* pV2) {return MATH_f_DotProduct(pV1, pV2);}
	inline	void LM_CopyVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA) {MATH_CopyVector(pDest, pA);}
	inline	void LM_AddEqualVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA) {MATH_AddEqualVector(pDest, pA);}
	inline	void LM_SubEqualVector(LM_tdst_Vector* pDest, LM_tdst_Vector* pA) {MATH_SubEqualVector(pDest, pA);}
	inline	void LM_CopyToLMVector(LM_tdst_Vector* pLMVect, MATH_tdst_Vector* pVect) {MATH_CopyVector(pLMVect, pVect);}
	inline	void LM_CopyToLMMatrix(LM_tdst_Matrix* pLMMatrix, MATH_tdst_Matrix* pMatrix) {MATH_CopyMatrix(pLMMatrix, pMatrix);}
	inline	void LM_InitVector(LM_tdst_Vector* pVect, LMReal x, LMReal y, LMReal z) {MATH_InitVector(pVect, x, y, z);}
	inline	LMReal LM_Distance(LM_tdst_Vector* pV1, LM_tdst_Vector* pV2) { return MATH_f_Distance(pV1, pV2);}
	inline	void LM_TransformVector(LM_tdst_Vector* pDest, LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATH_TransformVector(pDest, pMatrix, pVect);}
	inline	void LM_TransformVertex(LM_tdst_Vector* pDest, LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATH_TransformVertex(pDest, pMatrix, pVect);}
	inline	void LM_SetXAxis(LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATH_SetXAxis(pMatrix, pVect);}
	inline	void LM_SetYAxis(LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATH_SetYAxis(pMatrix, pVect);}
	inline	void LM_SetZAxis(LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATH_SetZAxis(pMatrix, pVect);}
	inline	void LM_MakeRotationMatrix_ZAxis(LM_tdst_Matrix* pMatrix, LMReal angle, bool initRestOfMatrix) { MATH_MakeRotationMatrix_ZAxis(pMatrix, angle, initRestOfMatrix);}
	inline	void LM_SetTranslation(LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATH_SetTranslation(pMatrix, pVect);}
	inline	void LM_SetScale(LM_tdst_Matrix* pMatrix, LM_tdst_Vector* pVect) { MATH_SetScale(pMatrix, pVect);}
	inline	void LM_InvertMatrix(LM_tdst_Matrix* pMatrix1, LM_tdst_Matrix* pMatrix2) {MATH_InvertMatrix(pMatrix1, pMatrix2);}
	inline	void LM_SetIdentityMatrix(LM_tdst_Matrix* pMatrix1) {MATH_SetIdentityMatrix(pMatrix1);}
	inline	void LM_MulMatrixMatrix(LM_tdst_Matrix* pDest, LM_tdst_Matrix* pMatrix1, LM_tdst_Matrix* pMatrix2) {MATH_MulMatrixMatrix(pDest, pMatrix1, pMatrix2);}
	inline	void LM_CopyMatrix(LM_tdst_Matrix* pDest, LM_tdst_Matrix* pSrc) {MATH_CopyMatrix(pDest, pSrc);}
	inline	void LM_CopyFromLMVector(MATH_tdst_Vector* pVect, LM_tdst_Vector* pLMVect) {MATH_CopyVector(pVect, pLMVect);}
	inline	void LM_CopyFromLMMatrix(MATH_tdst_Matrix* pMatrix, LM_tdst_Matrix* pLMMatrix) {MATH_CopyMatrix(pMatrix, pLMMatrix);}	
	inline  BOOL LM_EqVectorWithEpsilon(LM_tdst_Vector* pV1, LM_tdst_Vector* pV2, float epsilon) { return MATH_b_EqVectorWithEpsilon(pV1, pV2, epsilon); }
#endif

#pragma pack(push, 1)
typedef struct
{
	unsigned char	uc_Size;
	unsigned char	uc_ColorMapType;
	unsigned char	uc_ImageTypeCode;
	unsigned short	uw_Origin;
	unsigned short	uw_PaletteLength;
	unsigned char	uc_BPCInPalette;
	unsigned short	uw_Left;
	unsigned short	uw_Top;
	unsigned short	uw_Width;
	unsigned short	uw_Height;
	unsigned char	uc_BPP;
	unsigned char	ucDescriptorByte;
} TEX_tdst_File_TgaHeader;
#pragma pack(pop) 

#define MAX_ADJ_FACES 32

//#define LM_Epsilon 0.1f
#define LM_Epsilon 0.001f

extern void OBJ_BV_AddPointToAABBox
(
	void				*_pst_BV,
	MATH_tdst_Matrix	*_pst_AbsoluteMatrix,
	MATH_tdst_Vector	*_pst_Point,
	BOOL				_bNoScale
);

typedef enum { XPOS = 0, XNEG = 1, YPOS = 2, YNEG = 3, ZPOS = 4, ZNEG = 5} tdeFaceAxis;

#define MAX_TEMP_LIGHTMAP 500

#define SINGLE_PIXEL_BORDER

typedef struct
{
	int	w;
	int h;
	int borderx;     
	int bordery;
	int* start;
} tdst_LightmapPackPageInfo;

tdst_LightmapPackPageInfo gTempLMPageInfo[MAX_TEMP_LIGHTMAP];
int gNbTempPage;

//#define LOG_RAYTRACE

// custom type and functions for the color since they need to be vector 4
typedef struct 
{
	float r;
	float g;
	float b;
	float a;
}LMColor;

inline void AddLMColor(LMColor* pDest, const LMColor* pCol1, const LMColor* pCol2)
{
	pDest->r = pCol1->r + pCol2->r;
	pDest->g = pCol1->g + pCol2->g;
	pDest->b = pCol1->b + pCol2->b;
	pDest->a = pCol1->a + pCol2->a;
}

inline void SubLMColor(LMColor* pDest, const LMColor* pCol1, const LMColor* pCol2)
{
	pDest->r = pCol1->r - pCol2->r;
	pDest->g = pCol1->g - pCol2->g;
	pDest->b = pCol1->b - pCol2->b;
	pDest->a = pCol1->a - pCol2->a;
}

#if 0 // they were clearly depending on overloads here and compiling as C++, blergh ~hogsy
inline void AddLMColor(LMColor* pDest, LMColor* pCol1)
{
	pDest->r += pCol1->r;
	pDest->g += pCol1->g;
	pDest->b += pCol1->b;
	pDest->a += pCol1->a;
}
#endif

inline void MulLMColorf(LMColor* pDest, float val)
{
	pDest->r *= val;
	pDest->g *= val;
	pDest->b *= val;
	pDest->a *= val;
}

inline void MulLMColorsf(LMColor* pDest, const LMColor* pSrc, float val)
{
	pDest->r = pSrc->r * val;
	pDest->g = pSrc->g * val;
	pDest->b = pSrc->b * val;
	pDest->a = pSrc->a * val;
}

inline void MulLMColorClamp(LMColor* pDest, const LMColor* pSrc, float val, float clamp)
{
	pDest->r = MATH_Min(pSrc->r * val, clamp);
	pDest->g = MATH_Min(pSrc->g * val, clamp);
	pDest->b = MATH_Min(pSrc->b * val, clamp);
	pDest->a = MATH_Min(pSrc->a * val, clamp);
}


inline void MulLMColor(LMColor* pDest, const LMColor* pCol1, const LMColor* pCol2)
{
	pDest->r = pCol1->r * pCol2->r;
	pDest->g = pCol1->g * pCol2->g;
	pDest->b = pCol1->b * pCol2->b;
	pDest->a = pCol1->a * pCol2->a;
}

typedef struct _tdst_LMFace
{
	LM_tdst_Vector		vertex[3];
	LM_tdst_Vector		normal[3];
	int					index[3];
	float				u[3];
	float				v[3];
	float				BSRadiusSquare;
	LM_tdst_Vector		BSCenter;
	struct _tdst_LMFace* adjFaces[MAX_ADJ_FACES];
	int					adjFacesIndex[MAX_ADJ_FACES];
	int					nbAdjFaces;
	LM_tdst_Vector		facenormal;
	tdeFaceAxis			axis;
	int					lmPage;
	int					objfaceindex;
	int					objElementIndex;
	int					lmObjElementIndex;
	// Plane equation coefficients so we don't need to recompute them all the time
	LMReal				A, B, C, D;	
	// face vectors so we don't have to recompute them all the time (must be unormalized for barycoords computation
	LM_tdst_Vector		v1, v2, unnormalizednormal;
	unsigned int		receiveShadows:1;				// it won't make much of a difference but....
	unsigned int		castShadows:1;
} tdst_LMFace;

typedef struct
{
	tdst_LMFace*		p_tdst_faces;
	int					nbFaces;
	GEO_tdst_ElementIndexedTriangles * pElement;
	LIGHT_tdst_LightmapSettings		settings;
} tdst_LMElement;

typedef struct
{
	unsigned char render:1;
	unsigned char elementface:1;	
} tdst_LMAxisFace;

#define MAX_ELEM_LIST_SIZE 30

typedef struct _st_LMAxisElement
{
	tdst_LMAxisFace* pAxisFaces;
	LM_tdst_Matrix worldToCam;
	int				 mapwidth;
	int				 mapheight;
	int				 lmIndex;
	int				 lmPosX;
	int				 lmPosY;
	int				 numFaces;
	tdeFaceAxis		 elemAxis;
	int				 objElemList[MAX_ELEM_LIST_SIZE];
	int				 nbObjElem;
	LM_tdst_Vector	 norm;
	LM_tdst_Vector	 min;
	LM_tdst_Vector   max;
	LMColor			 *pMap;	
	struct _st_LMAxisElement*	pUseMapFromElement;	
	ULONG			 objKey;
	float			 texelPerMeter;
} tdst_LMAxisElement;

//#define PACK_PER_OBJECT		// tried it and it's not that good, the best solution too often is splitted on multiple pages and big pages don't get their free space reused by smaller objects

#define PACK_ONE_PAGE_PER_WORLD 



typedef struct 
{
	int nbPages;
	int* pagesWidth;
	int* pagesHeight;
    int* elementStartPosX;
	int* elementStartPosY;
	int* elementPage;
	int	totalSize;
} tdst_LMPageLayout;

typedef struct
{
	LIGHT_tdst_LightmapSettings		settings;
	tdst_LMElement*					p_tdst_elements;
	int								nbElements;
	float							BSRadiusSquare;
	LM_tdst_Vector					BSCenter;	
	TAB_tdst_Ptable					axisElementsList;
	tdst_LMFace*					p_tdst_faces;
	int								nbFaces;
	LM_tdst_Matrix					toWorld;
	LM_tdst_Matrix					worldToMesh;
	GEO_tdst_Object*				geoObj;
	OBJ_tdst_GameObject*			gameObj;
	LMColor							objAmbient;
#ifdef PACK_PER_OBJECT
	tdst_LMAxisElement**			pSortedAxisElements;
	int								nbAxisElements;
	tdst_LMPageLayout*				pageLayout;
#endif
} tdst_LMObj;

typedef struct
{
	OBJ_tdst_GameObject*			pLight;
	tdst_LMFace*					pLastShadowCastingFace;
	tdst_LMObj*						pLastShadowCastingObject;
} tdst_LMLight;

#define MAX_LIGHTMAP_PAGES 500

LMProgressCallback gGlobalLMCallback = NULL;
void*			   gGlobalThis = NULL;
float			   gTexelPerMeter = 2.0f;

int gMaxWidth = 0;
int gMaxHeight = 0;

int gMinPageSize = 1024;
int gMaxPageSize = 1024;

bool gSuperSampling = true;
bool gComputeShadows = true;
bool gShadowMapMode	= true;
float gSuperSamplingFactor = 2.0f;
bool gFixBackFaceBug = false;

int gTotalNbAxisElement = 0;
float gShadowOpacity = 0.0f;

tdst_LMObj* gObjList;
int gNbObj;
tdst_LMFace* gFaceList;
int gNbFace;
BIG_KEY gCurrentWorld;
tdst_LMPageLayout*	gPageLayout;

OBJ_tdst_GameObject*	pCurrentLight;

tdeFaceAxis	GetVectorMinAxis(LM_tdst_Vector* _pVect)
{
	// no need to normalize we only want to know the strongest axis
	if (fabs(_pVect->x) <= fabs(_pVect->y) && fabs(_pVect->x) <= fabs(_pVect->z))
	{
		if (_pVect->x >= 0) 
			return XPOS;
		else 
			return XNEG;
	}
	else if (fabs(_pVect->y) <= fabs(_pVect->z))
	{
		if (_pVect->y >= 0)
			return YPOS;
		else
			return YNEG;
	}
	else 
	{
		if (_pVect->z >= 0)
			return ZPOS;
		else
			return ZNEG;
	}

}
 
tdeFaceAxis	GetVectorMaxAxis(LM_tdst_Vector* _pVect)
{
	// no need to normalize we only want to know the strongest axis
	if (fabs(_pVect->x) >= fabs(_pVect->y) && fabs(_pVect->x) >= fabs(_pVect->z))
	{
		if (_pVect->x >= 0)
			return XPOS;
		else 
			return XNEG;
	}
	else if (fabs(_pVect->y) >= fabs(_pVect->z))
	{
		if (_pVect->y >= 0)
			return YPOS;
		else
			return YNEG;
	}
	else 
	{
		if (_pVect->z >= 0)
			return ZPOS;
		else
			return ZNEG;
	}

}

tdeFaceAxis	GetFaceAxis(tdst_LMFace* _pFace)
{
	LM_tdst_Vector lNormal, v1, v2;

	// generate face normal
	LM_SubVector(&v1, &_pFace->vertex[1], &_pFace->vertex[0]);
	LM_SubVector(&v2, &_pFace->vertex[2], &_pFace->vertex[0]);
	LM_CrossProduct(&lNormal, &_pFace->v1, &_pFace->v2);

	LM_NormalizeVector(&lNormal, &lNormal);
	LM_CopyVector(&_pFace->facenormal, &lNormal);
	
	return GetVectorMaxAxis(&lNormal);
}

inline void AddElementToList(int* elementIndexList, int* nbElements, int indexToAdd)
{
	int i;

	for (i = 0; i < *nbElements; i++)
	{
		if (elementIndexList[i] == indexToAdd)
			return;
	}

	if (i < MAX_ELEM_LIST_SIZE)
	{ 
		elementIndexList[i] = indexToAdd;
		*nbElements++;
	}
	else
	{
		_breakpoint_; 
	}
}

bool AreElementSettingsCompatible(tdst_LMObj* _pObj, int _face1, int _face2)
{
	tdst_LMElement* pElem1, *pElem2;

	// get element pointer
	pElem1 = _pObj->p_tdst_elements + _pObj->p_tdst_faces[_face1].lmObjElementIndex;
	pElem2 = _pObj->p_tdst_elements + _pObj->p_tdst_faces[_face2].lmObjElementIndex;

	// if it's the same element the settings are compatible
	// it doesn't really matter if both elements cast shadows or not this won't affect the rendering
	if (pElem1 == pElem2)
		return true;
	else
	{
		// at this point fTexelPerMeter contains the texelpermeter ration for the element
		// according to all the possible overrides and in correspondance with the global settings
		//if (pElem1->settings.fTexelPerMeter == pElem2->settings.fTexelPerMeter)
			return true;
	}

	return false;
}

void TraverseElement(tdst_LMObj* _pObj, int _face, TAB_tdst_Ptable* _pAxisFaces, tdst_LMAxisElement* _pElem, LM_tdst_Vector* _pNorm)
{
	int i;

	// remove the face of axis list of face
	TAB_Ptable_RemoveElemWithPointer(_pAxisFaces, &_pObj->p_tdst_faces[_face]);

	_pElem->pAxisFaces[_face].elementface = 1;
	_pElem->pAxisFaces[_face].render = 1;
	_pElem->numFaces++;	

	LM_AddEqualVector(_pNorm, &_pObj->p_tdst_faces[_face].facenormal);
	
	// flag all adjacent face for render
	// traverse and flag all adjacent faces on the same axis
	tdst_LMFace* face = &_pObj->p_tdst_faces[_face];
	int adjfaceindex;

	AddElementToList(_pElem->objElemList, &_pElem->nbObjElem, face->objElementIndex);

	for (i = 0; i < face->nbAdjFaces; i++)
   	{
		adjfaceindex = ((unsigned int)face->adjFaces[i] - (unsigned int)_pObj->p_tdst_faces) / sizeof(tdst_LMFace);

		_pElem->pAxisFaces[adjfaceindex].render = 1;

		// do not traverse allready traversed faces
		if (face->adjFaces[i]->axis == face->axis && !_pElem->pAxisFaces[adjfaceindex].elementface && AreElementSettingsCompatible(_pObj, _face, face->adjFacesIndex[i]) )
		{
			TraverseElement(_pObj, adjfaceindex, _pAxisFaces, _pElem, _pNorm);
		}
	}
}

void SnapToWorldTexel(LM_tdst_Vector* _pVect, float _pGridSize, bool _roundDown)
{
	LM_tdst_Vector vect;
	
	if (_roundDown)
	{
		vect.x = floorf(_pVect->x / _pGridSize) * _pGridSize;
		vect.y = floorf(_pVect->y / _pGridSize) * _pGridSize;
		vect.z = floorf(_pVect->z / _pGridSize) * _pGridSize;
	}
	else
	{
		vect.x = ceilf(_pVect->x / _pGridSize) * _pGridSize;
		vect.y = ceilf(_pVect->y / _pGridSize) * _pGridSize;
		vect.z = ceilf(_pVect->z / _pGridSize) * _pGridSize;
	}

	LM_CopyVector(_pVect, &vect);
}

int CompareIndex(const void* index1, const void* index2)
{
	int i1, i2;

	i1 = *(int*)index1;
	i2 = *(int*)index2;

	if (i1 < i2) 
		return -1;
	else if (i1 == i2)
		return 0;
	else
		return 1;
}

void Get2DVectorFromFace(tdst_LMFace* _pFace, int index, LM_tdst_Vector* pVect)
{
	switch (_pFace->axis)
	{
		case XPOS:
		case XNEG:
			pVect->x = _pFace->vertex[index].y;
			pVect->y = _pFace->vertex[index].z;
			pVect->z = 0;
			break;
		case YPOS:
		case YNEG:
			pVect->x = _pFace->vertex[index].x;
			pVect->y = _pFace->vertex[index].z;
			pVect->z = 0;
			break;
		case ZPOS:
		case ZNEG:
			pVect->x = _pFace->vertex[index].x;
			pVect->y = _pFace->vertex[index].y;
			pVect->z = 0;
			break;
	}
	
}

// function taken from http://www.magic-software.com/Source/Intersection2D/MgcIntr2DTriTri.cpp

int WhichSide(LM_tdst_Vector* _pVertexT1, LM_tdst_Vector* _pVertex2, LM_tdst_Vector* kd)
{
	LM_tdst_Vector p;
	int positive = 0, negative = 0, zero = 0;
	int i;
	float dot;

	for (i = 0; i < 3; i++)
	{
		LM_SubVector(&p, &_pVertexT1[i], _pVertex2);
		dot = LM_f_DotProduct(kd, &p);

		if (dot > 0)
			positive++;
		else if (dot < 0)
			negative++;
//		else 
//			zero++;

		if (positive > 0 && negative > 0)
			return 0;
	}

// zero means some vertex are touching each other, it's allright for the way I want it
//	if (zero == 0)
//	{
		if (positive > 0)
		{
			return 1;
		}
		else
		{
			return -1;
		}
//	}
//	else
//	{
//		return 0;
//	}	
}

bool AreFacesOverlapping(tdst_LMFace* _pFace1, tdst_LMFace* _pFace2)
{
	int index0, index1;
	LM_tdst_Vector	vect;

	LM_tdst_Vector VertexT1[3];
	LM_tdst_Vector VertexT2[3];
	vect.z = 0;

	for (index0 = 0; index0 < 3; index0 ++)
	{
		Get2DVectorFromFace(_pFace1, index0, &VertexT1[index0]);
		Get2DVectorFromFace(_pFace2, index0, &VertexT2[index0]);
	}

	// test edges of face1 for separation
	for (index0 = 0, index1 = 2; index0 < 3; index1 = index0, index0 ++)
	{
		vect.x = VertexT1[index0].y - VertexT1[index1].y;
		vect.y = VertexT1[index1].x - VertexT1[index0].x;

		if (WhichSide(VertexT2, &VertexT1[index1], &vect) > 0)
		{
			return false;
		}
	}

	// test edges of face2 for separation
	for (index0 = 0, index1 = 2; index0 < 3; index1 = index0, index0 ++)
	{
		vect.x = VertexT2[index0].y - VertexT2[index1].y;
		vect.y = VertexT2[index1].x - VertexT2[index0].x;

		if (WhichSide(VertexT1, &VertexT2[index1], &vect) > 0)
		{
			return false;
		}
	}

	return true;
}

bool AreElementsOverlapping(tdst_LMAxisElement* _axisElement, tdst_LMAxisElement* _axisElement2, tdst_LMObj* _pObj)
{
	int i, j;

	// quick and easy rejection test
	if (_axisElement->elemAxis != _axisElement2->elemAxis)
		return true;

	// check all triangles of element1 against the triangles of element2 to see if they overlap
	for (i = 0; i < _pObj->nbFaces; i++)
	{
		if (_axisElement->pAxisFaces[i].elementface)
		{
			for (j = 0; j < _pObj->nbFaces; j++)
			{
				if (_axisElement2->pAxisFaces[j].elementface)
				{
					// check for triangle overlap
					if (AreFacesOverlapping(&_pObj->p_tdst_faces[i], &_pObj->p_tdst_faces[j]))
					{
 						return true;
					}
				}
			}
		}
	}

	return false;
}

bool AreElementsCloseToEachOther(tdst_LMAxisElement* _axisElement, tdst_LMAxisElement* _axisElement2, tdst_LMObj* _pObj)
{
	// to be done
	return true;
}

void MergeAxisElements(tdst_LMAxisElement* _axisElement, tdst_LMAxisElement* _axisElement2, tdst_LMObj* _pObj)
{
	int i;
	int newNbFaces = _axisElement->numFaces + _axisElement2->numFaces;

	// add the stuff into _axisElement 
	_axisElement->numFaces = newNbFaces;

	// merge the geo element list 
	for (i = 0; i < _axisElement2->nbObjElem; i++)
	{
		AddElementToList(_axisElement->objElemList, &_axisElement->nbObjElem, _axisElement2->objElemList[i]);
	}

	// merge the faces
	for (i = 0; i < _pObj->nbFaces; i++)
	{
		if (_axisElement2->pAxisFaces[i].render)
			_axisElement->pAxisFaces[i].render = true;

		if (_axisElement2->pAxisFaces[i].elementface)
			_axisElement->pAxisFaces[i].elementface = true;
	}
}

void GenerateAxisElements(tdst_LMObj* _pObj)
{
	ULONG i, j;
	tdst_LMFace* pFace;
	TAB_tdst_Ptable axisFaces[6];
	tdst_LMAxisElement*	 axisElement;
#ifdef MERGE_NON_OVERLAPPING_AXIS_ELEMENTS
	tdst_LMAxisElement*	 axisElement2;	
#endif // #ifdef MERGE_NON_OVERLAPPING_AXIS_ELEMENTS
	
	for (i = 0; i < 6; i++)
	{
		TAB_Ptable_Init(&axisFaces[i], _pObj->nbFaces, 2.0f);
	}

	if (!(_pObj->settings.bReceiveShadows && _pObj->settings.bUseLightmaps) || _pObj->settings.bTemporaryStopUsingLightmaps)
		return;

	// compute the axis of each face
	pFace = _pObj->p_tdst_faces;  

	for (i = 0 ; i < (ULONG)_pObj->nbFaces; i++)
	{
		pFace->axis = GetFaceAxis(pFace);

		TAB_Ptable_AddElemAndResize(&axisFaces[pFace->axis], pFace);

		pFace++;
	}

	// init the axis element table
	TAB_Ptable_Init(&_pObj->axisElementsList, _pObj->nbElements, 2.0f);

	// for each axis create the mesh elements (using adjacent faces)
	for (i = 0; i < 6; i++)
	{
		while (TAB_ul_Ptable_GetNbElems(&axisFaces[i]))
		{
			LM_tdst_Vector norm = {0, 0, 0};

			// create a new axis element
			axisElement = (tdst_LMAxisElement*)LM_Alloc(sizeof(tdst_LMAxisElement));
			TAB_Ptable_AddElemAndResize(&_pObj->axisElementsList, axisElement);
			axisElement->nbObjElem = 0;
			axisElement->pUseMapFromElement = NULL;	// initially it's not equivalent 
			axisElement->objKey = (ULONG)_pObj;
			
			// increment the counter of total axi elements
			gTotalNbAxisElement++;

			// allocate face info
			axisElement->pAxisFaces = (tdst_LMAxisFace*)LM_Alloc(sizeof(tdst_LMAxisFace) * _pObj->nbFaces);
			memset(axisElement->pAxisFaces, 0, sizeof(tdst_LMAxisFace) * _pObj->nbFaces);            
			axisElement->numFaces = 0;

			// set the face information
			j = 0;

			// skip all holes to get the first face still in the table
			while (TAB_b_IsAHole(axisFaces[i].p_Table[j]))
				j++;

			int faceindex = ((unsigned int)axisFaces[i].p_Table[j] - (unsigned int)_pObj->p_tdst_faces) / sizeof(tdst_LMFace);

			TraverseElement(_pObj, faceindex, &axisFaces[i], axisElement, &norm);

			// set the axis element ratio
			axisElement->texelPerMeter = _pObj->p_tdst_elements[_pObj->p_tdst_faces[faceindex].lmObjElementIndex].settings.fTexelPerMeter;

			// Transform norm into world coordinates....
			LM_tdst_Vector temp;

			LM_TransformVector(&temp, &_pObj->toWorld, &norm);
			LM_NormalizeVector(&temp, &temp);

			// Adjust camera orientation
			LM_tdst_Vector camOrientation = {0, 0, 0};
			
			int maxaxis = GetVectorMaxAxis(&temp);            	
			axisElement->elemAxis = (tdeFaceAxis) maxaxis;

			LM_CopyVector(&axisElement->norm, &norm);

		}
	}

#ifdef MERGE_NON_OVERLAPPING_AXIS_ELEMENTS
	// check the axis elements on the same axis if they don't overlap and are relatively close together merge them
	for (i = 0; i < TAB_ul_Ptable_GetMaxNbElems(&_pObj->axisElementsList); i++)
	{
		axisElement = (tdst_LMAxisElement*) _pObj->axisElementsList.p_Table[i];

		// compare it with the other elements		
		if (TAB_b_IsAHole(axisElement) || &_pObj->axisElementsList.p_Table[i] > TAB_ppv_Ptable_GetLastElem(&_pObj->axisElementsList) )
			continue;

		for (j = i+1;j < TAB_ul_Ptable_GetMaxNbElems(&_pObj->axisElementsList); j++)
		{
			axisElement2 = (tdst_LMAxisElement*) _pObj->axisElementsList.p_Table[j];

			// compare it with the other elements
			if (TAB_b_IsAHole(axisElement2) || &_pObj->axisElementsList.p_Table[i] > TAB_ppv_Ptable_GetLastElem(&_pObj->axisElementsList))
				continue;

			// check if elements do not overlap on their axis and if they are close enough to merge em
			if (!AreElementsOverlapping(axisElement, axisElement2, _pObj))
			{
				// check spatial similarity
				if (AreElementsCloseToEachOther(axisElement, axisElement2, _pObj))
				{
					MergeAxisElements(axisElement, axisElement2, _pObj);

					TAB_Ptable_RemoveElemAtIndex(&_pObj->axisElementsList, j);

					gTotalNbAxisElement --;
					LM_Free(axisElement2->pAxisFaces);
					LM_Free(axisElement2);
				}
			}
		}

	}
#endif

	TAB_Ptable_RemoveHoles(&_pObj->axisElementsList);

	// loop on all the axis elements and calculate the stuff
	for (i = 0; i < TAB_ul_Ptable_GetMaxNbElems(&_pObj->axisElementsList); i++)
	{  
		axisElement = (tdst_LMAxisElement*) _pObj->axisElementsList.p_Table[i];

		if (TAB_b_IsAHole(axisElement) || &_pObj->axisElementsList.p_Table[i] > TAB_ppv_Ptable_GetLastElem(&_pObj->axisElementsList))
			continue;

		// sort the element index list
		qsort(axisElement->objElemList, axisElement->nbObjElem, sizeof(axisElement->objElemList[0]), CompareIndex);
		
		LM_tdst_Vector norm;

		// Transform norm into world coordinates....
		LM_CopyVector(&norm, &axisElement->norm);
		LM_tdst_Vector temp; 

		LM_TransformVector(&temp, &_pObj->toWorld, &norm);
		LM_NormalizeVector(&temp, &temp);

        // Adjust camera orientation
		LM_tdst_Vector camOrientation = {0, 0, 0};
		
		int maxaxis = GetVectorMaxAxis(&temp);            	
		axisElement->elemAxis = (tdeFaceAxis) maxaxis;

		// ugly, but this sets 1 or -1 in the right coordinate depending on the max axis of the vector
		*(((LMReal*)&camOrientation) + (maxaxis >> 1)) = ((maxaxis&0x1)? -1.0: 1.0);

		LM_NormalizeVector(&norm, &norm);

		LM_tdst_Matrix camToWorld;
		memset(&camToWorld, 0, sizeof(LM_tdst_Matrix));

		temp.x = temp.y = temp.z = 0.0;
		int minaxis = GetVectorMinAxis(&camOrientation);
		*(((LMReal*)&temp) + (minaxis >> 1)) = 1.0;

		// build camera matrix for this element
		LM_tdst_Vector lRow, lRow2;

		LM_CrossProduct(&lRow, &camOrientation, &temp);
		LM_NormalizeVector(&lRow, &lRow);
		LM_SetXAxis(&camToWorld, &lRow);

		LM_CrossProduct(&lRow2, &camOrientation, &lRow);
		LM_NormalizeVector(&lRow2, &lRow2);
		LM_SetYAxis(&camToWorld, &lRow2);

		LM_SetZAxis(&camToWorld, &camOrientation);
		camToWorld.lType = MATH_Ci_Rotation;

		// find bounding box for element
		LM_tdst_Matrix worldToCam;
		LM_tdst_Matrix objToCam;

		memset(&worldToCam, 0, sizeof(LM_tdst_Matrix));
		memset(&objToCam, 0, sizeof(LM_tdst_Matrix));

		LM_InvertMatrix(&worldToCam, &camToWorld);
		LM_MulMatrixMatrix(&objToCam, &_pObj->toWorld, &worldToCam);

		// transform all vertex into cam space and compute bounding box
		OBJ_tdst_SingleBV box;
		box.pst_GO = NULL;
		box.st_GMin.x = box.st_GMin.y = box.st_GMin.z = Cf_Infinit;
		box.st_GMax.x = box.st_GMax.y = box.st_GMax.z = -Cf_Infinit;
		MATH_tdst_Matrix identity;
		MATH_SetIdentityMatrix(&identity);
		MATH_tdst_Vector fVect;

		for (j = 0; j < (ULONG)_pObj->nbFaces; j++)
		{
			if (axisElement->pAxisFaces[j].elementface)
			{
				LM_TransformVertex(&temp, &objToCam, &_pObj->p_tdst_faces[j].vertex[0]);
				LM_CopyFromLMVector(&fVect, &temp);
				OBJ_BV_AddPointToAABBox(&box, &identity, &fVect, false);

				LM_TransformVertex(&temp, &objToCam, &_pObj->p_tdst_faces[j].vertex[1]);
				LM_CopyFromLMVector(&fVect, &temp);
				OBJ_BV_AddPointToAABBox(&box, &identity, &fVect, false);

				LM_TransformVertex(&temp, &objToCam, &_pObj->p_tdst_faces[j].vertex[2]);
				LM_CopyFromLMVector(&fVect, &temp);
				OBJ_BV_AddPointToAABBox(&box, &identity, &fVect, false);
			}
		}
		
		// copy min and max for debugging help
		LM_CopyToLMVector(&axisElement->min, &box.st_GMin);
		LM_CopyToLMVector(&axisElement->max, &box.st_GMax);

        // Snap box position to 
		SnapToWorldTexel(&axisElement->max, 1.0f / axisElement->texelPerMeter, false);
		SnapToWorldTexel(&axisElement->min, 1.0f / axisElement->texelPerMeter, true);

		// Set the translation
		LM_tdst_Vector bWidth;
		LM_SubVector(&bWidth, &axisElement->max, &axisElement->min);

		//LM_AddVector(&temp, &box.st_GMin, &box.st_GMax);
		LM_CopyVector(&temp, &bWidth);
		LM_MulEqualVector(&temp, 0.5f);
		LM_AddEqualVector(&temp, &axisElement->min); // temp is now the center of the box

		LM_TransformVertex(&lRow, &camToWorld, &temp);
		LM_MulEqualVector(&camOrientation, bWidth.z * 0.5f);
		LM_AddEqualVector(&lRow, &camOrientation);			
		LM_SetTranslation(&camToWorld, &lRow);

		// store the world to cam matrix
		LM_SetIdentityMatrix(&axisElement->worldToCam);
		LM_InvertMatrix(&axisElement->worldToCam, &camToWorld);

		axisElement->mapwidth = ceilf(bWidth.x * axisElement->texelPerMeter);
		axisElement->mapheight= ceilf(bWidth.y * axisElement->texelPerMeter);

		axisElement->mapwidth = MATH_Max(axisElement->mapwidth, 2);
		axisElement->mapheight = MATH_Max(axisElement->mapheight, 2);

#ifdef SINGLE_PIXEL_BORDER
		axisElement->mapwidth += (axisElement->mapwidth %2 ) + 2;
		axisElement->mapheight += (axisElement->mapheight % 2) + 2;
#else
		axisElement->mapwidth += (axisElement->mapwidth %2 ) + 4;
		axisElement->mapheight += (axisElement->mapheight % 2) + 4;
#endif

		if (axisElement->mapwidth > axisElement->mapheight)
		{	
			LM_tdst_Matrix rotMatrix;
			LM_tdst_Matrix res;

			memset(&rotMatrix, 0, sizeof(LM_tdst_Matrix));
			memset(&res, 0, sizeof(LM_tdst_Matrix));

			LM_MakeRotationMatrix_ZAxis(&rotMatrix, Cf_PiBy2 , true);

			LM_MulMatrixMatrix(&res, &axisElement->worldToCam, &rotMatrix);

			LM_CopyMatrix(&axisElement->worldToCam, &res);
			
			float fTemp = axisElement->mapwidth;
			axisElement->mapwidth = axisElement->mapheight;
			axisElement->mapheight = fTemp;
		}

		axisElement->lmPosX = -1;
		axisElement->lmPosY = -1;
		axisElement->lmIndex = -1;


		gMaxHeight = MATH_Max(gMaxHeight, axisElement->mapheight);
		gMaxWidth = MATH_Max(gMaxWidth, axisElement->mapwidth);
	}    

	// free the axis face list
	for (i = 0; i < 6; i++)
	{
		TAB_Ptable_Close(&axisFaces[i]);
	}
}

void FreeAxisElements(tdst_LMObj* _pObj)
{
	ULONG i;
	tdst_LMAxisElement* elem;

	if (_pObj->nbElements == 0)
		return;

	// free the face list in the axiselements
	for (i = 0; i < TAB_ul_Ptable_GetMaxNbElems(&_pObj->axisElementsList); i++)
	{
		elem = (tdst_LMAxisElement*) _pObj->axisElementsList.p_Table[i];

		if (TAB_b_IsAHole(elem) || &_pObj->axisElementsList.p_Table[i] > TAB_ppv_Ptable_GetLastElem(&_pObj->axisElementsList))
			continue;

		LM_Free(elem->pAxisFaces);

		// free the axis element table
		LM_Free(elem);
	}

	// close the axis element table
	TAB_Ptable_Close(&_pObj->axisElementsList);
	
}


#define VERTEX_EPSILON	0.001f

bool AreVertexEqual(tdst_LMFace* _pFace1, tdst_LMFace* _pFace2, int vertex1, int vertex2)
{
	if (_pFace1->index[vertex1] == _pFace2->index[vertex2])
	{
		return true;
	}

	if (LM_EqVectorWithEpsilon(&_pFace1->vertex[vertex1], &_pFace2->vertex[vertex2], VERTEX_EPSILON) &&
		LM_EqVectorWithEpsilon(&_pFace1->normal[vertex1], &_pFace2->normal[vertex2], VERTEX_EPSILON) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FaceAreAdjacent(tdst_LMFace* _pFace1, tdst_LMFace* _pFace2)
{
	
	// compare the vertex index to see if the face share an edge
	if (AreVertexEqual(_pFace1, _pFace2, 0, 0) ||
		AreVertexEqual(_pFace1, _pFace2, 0, 1) ||
		AreVertexEqual(_pFace1, _pFace2, 0, 2) )
	{
		if (AreVertexEqual(_pFace1, _pFace2, 1, 0) ||
			AreVertexEqual(_pFace1, _pFace2, 1, 1) ||
			AreVertexEqual(_pFace1, _pFace2, 1, 2) )
		{
			return true;
		}
		else if (AreVertexEqual(_pFace1, _pFace2, 2, 0) ||
				 AreVertexEqual(_pFace1, _pFace2, 2, 1) ||
				 AreVertexEqual(_pFace1, _pFace2, 2, 2) )
		{
			return true;
		}
		else 
			return false;		
	}
	else if (AreVertexEqual(_pFace1, _pFace2, 1, 0) ||
			 AreVertexEqual(_pFace1, _pFace2, 1, 1) ||
			 AreVertexEqual(_pFace1, _pFace2, 1, 2) )
	{
		if (AreVertexEqual(_pFace1, _pFace2, 2, 0) ||
			AreVertexEqual(_pFace1, _pFace2, 2, 1) ||
			AreVertexEqual(_pFace1, _pFace2, 2, 2) )
		{
			return true;
		}
		else 
			return false;		
	}	
	else
		return false;
}

void GenerateAdjFaces(tdst_LMObj* _p_tdst_Obj)
{
	tdst_LMFace* pFace;
	tdst_LMFace* pOtherFace;
	int i, j;

	pFace = _p_tdst_Obj->p_tdst_faces;

	for (i = 0; i < _p_tdst_Obj->nbFaces - 1; i++)
	{				
		pOtherFace = pFace + 1;

		for (j = i + 1; j < _p_tdst_Obj->nbFaces;  j++)
		{
			if (FaceAreAdjacent(pFace, pOtherFace))
			{
				ERR_X_Assert(pFace->nbAdjFaces < MAX_ADJ_FACES);
				ERR_X_Assert(pOtherFace->nbAdjFaces < MAX_ADJ_FACES);

#ifndef _DEBUG
				ERR_X_Warning((pFace->nbAdjFaces < MAX_ADJ_FACES && pOtherFace->nbAdjFaces < MAX_ADJ_FACES), ERR_szFormatMessage("Object %s has faces with too many adjoining faces", _p_tdst_Obj->gameObj->sz_Name), 0);
#endif

				pOtherFace->adjFaces[pOtherFace->nbAdjFaces] = pFace;
				pOtherFace->adjFacesIndex[pOtherFace->nbAdjFaces] = i;
				pOtherFace->nbAdjFaces++;

				pFace->adjFaces[pFace->nbAdjFaces] = pOtherFace;
				pFace->adjFacesIndex[pFace->nbAdjFaces] = j; 
				pFace->nbAdjFaces++;
			}
			pOtherFace++;
		}    
		pFace++;
	}    
}

void ComputeBSphere(GEO_tdst_Object* _pGeo, LM_tdst_Vector* _pCenter, float* _pRadius)
{
	int i;
	LM_tdst_Vector center;
	LM_tdst_Vector distance;
	LM_tdst_Vector temp;
	float radius = 0.0f;
	float lMaxX, lMaxY, lMaxZ;
	float lMinX, lMinY, lMinZ;

	lMaxX = lMaxY = lMaxZ = -Cf_Infinit;
	lMinX = lMinY = lMinZ = Cf_Infinit;

	center.x = center.y = center.z = 0;

	// compute center
	for (i = 0; i < _pGeo->l_NbPoints; i++)
	{
		lMaxX = MATH_Max(lMaxX, _pGeo->dst_Point[i].x);
		lMaxY = MATH_Max(lMaxY, _pGeo->dst_Point[i].y);
		lMaxZ = MATH_Max(lMaxZ, _pGeo->dst_Point[i].z);

		lMinX = MATH_Min(lMinX, _pGeo->dst_Point[i].x);
		lMinY = MATH_Min(lMinY, _pGeo->dst_Point[i].y);
		lMinZ = MATH_Min(lMinZ, _pGeo->dst_Point[i].z);

	}

	center.x = lMinX + (lMaxX - lMinX) * 0.5f;
	center.y = lMinY + (lMaxY - lMinY) * 0.5f;
	center.z = lMinZ + (lMaxZ - lMinZ) * 0.5f;
	
	// find radius
	for (i = 0; i < _pGeo->l_NbPoints; i++)
	{
		temp.x = _pGeo->dst_Point[i].x;
		temp.y = _pGeo->dst_Point[i].y;
		temp.z = _pGeo->dst_Point[i].z;

		LM_SubVector(&distance, &temp, &center);
		radius = MATH_Max(fabs(LM_f_NormVector(&distance)), radius);		
	}

	*_pRadius = radius;
	LM_CopyVector(_pCenter, &center);
}

void ComputePlanEqn(tdst_LMFace* _pFace)
{
	LM_tdst_Vector e1, e2, p;

	LM_SubVector(&e1, &_pFace->vertex[1], &_pFace->vertex[0]);
	LM_SubVector(&e2, &_pFace->vertex[2], &_pFace->vertex[0]);
	
	LM_CopyVector(&_pFace->v1, &e1);
	LM_CopyVector(&_pFace->v2, &e2);

	LM_CrossProduct(&p, &e1, &e2);

	LM_CopyVector(&_pFace->unnormalizednormal, &p);

	LM_NormalizeEqualVector(&p);
	_pFace->A = p.x;
	_pFace->B = p.y;
	_pFace->C = p.z;
	_pFace->D = -LM_f_DotProduct(&_pFace->vertex[0], &p);

	_pFace->axis = GetFaceAxis(_pFace);
}

void ComputeBaryCoords(tdst_LMFace* _pFace, LM_tdst_Vector* _point, LM_tdst_Vector* _bCoords)
{
#if 0
	LM_tdst_Vector* _pV0 = &_pFace->vertex[0]; 
	LM_tdst_Vector* _pV1 = &_pFace->vertex[1];  
	LM_tdst_Vector* _pV2 = &_pFace->vertex[2]; 
	LM_tdst_Vector* _pPoint = _point; 
	LM_tdst_Vector* _pCoords = _bCoords; 

	LM_tdst_Vector V02, V12, PV2;
	LMReal	fM00, fM01, fM11, fR0, fR1, fDet, fInvDet; 

	LM_SubVector(&V02, _pV0, _pV2);
	LM_SubVector(&V12, _pV1, _pV2);
	LM_SubVector(&PV2, _pPoint, _pV2);

	fM00 = LM_f_DotProduct(&V02, &V02);
	fM01 = LM_f_DotProduct(&V02, &V12);
	fM11 = LM_f_DotProduct(&V12, &V12);
	fR0 = LM_f_DotProduct(&V02, &PV2);
	fR1 = LM_f_DotProduct(&V12, &PV2);

	fDet = fM00 * fM11 - fM01 * fM01;

	ERR_X_Assert(fabs(fDet) > 0.0);
	
	fInvDet = 1.0f/ fDet;

	_pCoords->x = (fM11 * fR0 - fM01 * fR1) * fInvDet;
	_pCoords->y = (fM00 * fR1 - fM01 * fR0) * fInvDet;
	_pCoords->z = 1.0f - _pCoords->x - _pCoords->y;
#else
	LM_tdst_Vector b, p; //, v1, v2, cp;
	LM_tdst_Vector minus; 

	minus.x = -1;
	minus.y = minus.z = 1;

	//LM_SubVector(&v1, &_pFace->vertex[1], &_pFace->vertex[0]);
	//LM_SubVector(&v2, &_pFace->vertex[2], &_pFace->vertex[0]);

	LM_SubVector(&p, _point, &_pFace->vertex[0]);

	//LM_CrossProduct(&cp, &v1, &v2);

	/*ERR_X_Assert(_pFace->v2.x == v2.x);
	ERR_X_Assert(_pFace->v2.y == v2.y);
	ERR_X_Assert(_pFace->v2.z == v2.z);

	ERR_X_Assert(_pFace->v1.x == v1.x);
	ERR_X_Assert(_pFace->v1.y == v1.y);
	ERR_X_Assert(_pFace->v1.z == v1.z);

	ERR_X_Assert(_pFace->unnormalizednormal.x == cp.x);
	ERR_X_Assert(_pFace->unnormalizednormal.y == cp.y);
	ERR_X_Assert(_pFace->unnormalizednormal.z == cp.z);*/


	switch (_pFace->axis)
	//switch (GetVectorMaxAxis(&cp))
	{
		case ZNEG:
		case ZPOS:
			if (_pFace->unnormalizednormal.z == 0.0)
			{
				LM_CopyVector(_bCoords, &minus);
				return;
			}

			b.y = (p.x * _pFace->v2.y - p.y * _pFace->v2.x)/_pFace->unnormalizednormal.z;
			b.z = (p.y * _pFace->v1.x - p.x * _pFace->v1.y)/_pFace->unnormalizednormal.z;

			//b.y = (p.x * v2.y - p.y * v2.x)/cp.z;
			//b.z = (p.y * v1.x - p.x * v1.y)/cp.z;

			//b.y = (p.x * v2.y - p.y * v2.x)/cp.z;
			//b.z = (p.y * v1.x - p.x * v1.y)/cp.z;

			break;
		case YNEG:
		case YPOS:
			if (_pFace->unnormalizednormal.y == 0.0)
			{
				LM_CopyVector(_bCoords, &minus);
				return;
			}

			b.y = (p.z*_pFace->v2.x - p.x*_pFace->v2.z)/_pFace->unnormalizednormal.y;
			b.z = (p.x*_pFace->v1.z - p.z*_pFace->v1.x)/_pFace->unnormalizednormal.y;


			//b.y = (p.z*v2.x - p.x*v2.z)/cp.y;
			//b.z = (p.x*v1.z - p.z*v1.x)/cp.y;


			//b.y = (p.z*v2.x - p.x*v2.z)/cp.y;
			//b.z = (p.x*v1.z - p.z*v1.x)/cp.y;

			break;
		case XNEG:
		case XPOS:
			if (_pFace->unnormalizednormal.x == 0.0)
			{
				LM_CopyVector(_bCoords, &minus);
				return;
			}

			b.y = (p.y * _pFace->v2.z - p.z * _pFace->v2.y) / _pFace->unnormalizednormal.x;
			b.z = (p.z * _pFace->v1.y - p.y * _pFace->v1.z) / _pFace->unnormalizednormal.x;

			//b.y = (p.y * v2.z - p.z * v2.y) / cp.x;
			//b.z = (p.z * v1.y - p.y * v1.z) / cp.x;

			//b.y = (p.y * v2.z - p.z * v2.y) / cp.x;
			//b.z = (p.z * v1.y - p.y * v1.z) / cp.x;

			break;
	}

	b.x = 1.0 - b.y - b.z;

	LM_CopyVector(_bCoords, &b);
#endif
}

bool intersectTri(LM_tdst_Vector* _rayPos, LM_tdst_Vector* _rayDir, tdst_LMFace* _pFace, LM_tdst_Vector* _bc, LM_tdst_Vector* _hitPos, LMReal* _distance, bool _backFaceCull)
{
	LM_tdst_Vector p, pi;
	LMReal vD, v0;
	
	p.x = _pFace->A;
	p.y = _pFace->B;
	p.z = _pFace->C;

	vD = LM_f_DotProduct(&p, _rayDir);

	if (_backFaceCull && vD >= 0)
	{
		return false;
	}

	v0 = LM_f_DotProduct(&p, _rayPos);
	v0 += _pFace->D;
	v0 = -v0;
	
	*_distance= v0/vD;

	if (*_distance < LM_Epsilon)
	{
		return false;
	}

	LM_MulVector(&p, _rayDir, *_distance);
	LM_AddVector(&pi, _rayPos, &p);

	// Get the barycentric coordinates of the hitPoint.
    // If any of the components are > 1.0 the hit is outside the triangle

	ComputeBaryCoords(_pFace, &pi, _bc);

	if (_bc->x >= 0.0f && _bc->x <= 1.0f)
	{
		if (_bc->y >= 0.0f && _bc->y <= 1.0f)
		{
			if (_bc->z >= 0.0f && _bc->z <= 1.0f)
			{
				LM_CopyVector(_hitPos, &pi);
				return true;
			}
		}
	}
	return false;
}

int GetFace(tdst_LMFace* _elementRenderFaces, int _nbElementRenderFaces, LM_tdst_Vector* _rPos, LM_tdst_Vector* _rDir, float texelRatio, LM_tdst_Vector* _xVect, LM_tdst_Vector* _yVect, LM_tdst_Vector*  _bc, LM_tdst_Vector* _hitPos, tdst_LMFace** _ppLastFace, int* _lastFaceIndex)
{
	int i;
	LMReal f;

	// quick check for the last face
	if (*_ppLastFace)
	{
		if (intersectTri(_rPos, _rDir, *_ppLastFace, _bc, _hitPos, &f, true ))
		{
			return *_lastFaceIndex;	
		}
	}

	for (i = 0; i < _nbElementRenderFaces; i++)
	{
		if (intersectTri(_rPos, _rDir, &_elementRenderFaces[i], _bc, _hitPos, &f, true ))
		{
			*_ppLastFace = &_elementRenderFaces[i];
			*_lastFaceIndex = i;

			return i;	
		}
	}
	
	*_ppLastFace = NULL;
	*_lastFaceIndex = -1;

	return -1;
}

bool ComputeLightIntensity(float* _lightIntensity, LIGHT_tdst_Light* _lightObj, LM_tdst_Vector* _lightDir, LM_tdst_Vector* _lightPos, LM_tdst_Vector*_texelPos)
{
	LM_tdst_Vector incident;
	LMReal      distance = 0.0f;                        //Distance between the light source and the lit point
	float       cosAngle;
	float		attenuation;

	//Compute the incident vector and make it UnitVector
	LM_SubVector(&incident, _lightPos, _texelPos);
	//Compute the distance before normalizing the vector
	distance = LM_f_NormVector(&incident);

	*_lightIntensity = 0.0f;

	// compute attenuation
	switch (_lightObj->ul_Flags & LIGHT_Cul_LF_Type)
	{
		case LIGHT_Cul_LF_Omni:
			if (distance > _lightObj->st_Omni.f_Far)
			{
				return false;
			}
			else if (distance > _lightObj->st_Omni.f_Near)
			{
				attenuation = (_lightObj->st_Omni.f_Far - distance) / (_lightObj->st_Omni.f_Far - _lightObj->st_Omni.f_Near);
			}
			else
				attenuation = 1.0f;

            break;

		case LIGHT_Cul_LF_Direct:
            attenuation = 1.0f;
			break;
		case LIGHT_Cul_LF_Spot:

            if(_lightObj->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
            {
                // apply cylinder (1 - ((d    - inner) / (outer - inner)))

                distance = LM_f_DotProduct(_lightDir, &incident);
                FLOAT ZFactor = 1.0f;

                if(distance>0.0f)
                {
                    FLOAT fRadiusLittleAlpha = _lightObj->st_Spot.f_Near * fTan(_lightObj->st_Spot.f_LittleAlpha);
                    FLOAT fRadiusBigAlpha = _lightObj->st_Spot.f_Far * fTan(_lightObj->st_Spot.f_BigAlpha);

                    LM_tdst_Vector vTmp1;
                    LM_tdst_Vector vTmp2;
                    LM_MulVector(&vTmp1, _lightDir, distance);
                    LM_SubVector(&vTmp2, &incident, &vTmp1);                
                    FLOAT d = LM_f_NormVector(&vTmp2);
                    ZFactor = ((d - fRadiusLittleAlpha)/(fRadiusBigAlpha-fRadiusLittleAlpha));
                    if(ZFactor<0.0f) 
                        ZFactor = 0.0f;
                    if(ZFactor>1.0f) 
                        ZFactor = 1.0f;
                    ZFactor = 1.0f - ZFactor;
                }

                if (distance > _lightObj->st_Spot.f_Far)
                {
                    return false;
                }
                else if (distance > _lightObj->st_Spot.f_Near)
                {
                    attenuation = (_lightObj->st_Spot.f_Far - distance) / (_lightObj->st_Spot.f_Far - _lightObj->st_Spot.f_Near);
                }
                else
                    attenuation = 1.0f;

                attenuation *= ZFactor;

                //static FLOAT fval = 1.0f;
                //attenuation = fval;
/*
                //Normalize the incident vector
                LM_NormalizeEqualVector(&incident);
                cosAngle = LM_f_DotProduct(&incident, _lightDir);

                // also compute falloff for spot and concatenate it with the accumulation
                float hot_cos = fCos(_lightObj->st_Spot.f_LittleAlpha);
                float fall_cos = fCos(_lightObj->st_Spot.f_BigAlpha);

                if ( (cosAngle < hot_cos) && (hot_cos != fall_cos))
                {                   
                    // check if outside falloff
                    if (cosAngle < fall_cos)
                        return false;

                    attenuation *= ZFactor;//((cosAngle - fall_cos) / (hot_cos - fall_cos));
                }
                */
            }
            else
            {
			    if (distance > _lightObj->st_Spot.f_Far)
			    {
				    return false;
			    }
			    else if (distance > _lightObj->st_Spot.f_Near)
			    {
				    attenuation = (_lightObj->st_Spot.f_Far - distance) / (_lightObj->st_Spot.f_Far - _lightObj->st_Spot.f_Near);
			    }
			    else
				    attenuation = 1.0f;

			    //Normalize the incident vector
			    LM_NormalizeEqualVector(&incident);
			    cosAngle = LM_f_DotProduct(&incident, _lightDir);

			    // also compute falloff for spot and concatenate it with the accumulation
			    float hot_cos = fCos(_lightObj->st_Spot.f_LittleAlpha);
			    float fall_cos = fCos(_lightObj->st_Spot.f_BigAlpha);

			    if ( (cosAngle < hot_cos) && (hot_cos != fall_cos))
			    {                   
				    // check if outside falloff
				    if (cosAngle < fall_cos)
					    return false;

				    attenuation *= ((cosAngle - fall_cos) / (hot_cos - fall_cos));
			    }
            }
			break;
  	}

	*_lightIntensity = attenuation;
	return true;

}

bool IntersectRay(tdst_LMObj* _pObj, int _faceIndex, LM_tdst_Vector* _rayPos, LM_tdst_Vector* _rayDir, float _rayLength, tdst_LMFace** _pFace, bool _directLight, bool _bDoingShadow)
{
	LM_tdst_Vector pc;
	tdst_LMFace* face;
	LMReal v;
	int i;

	// reject object on bounding sphere
	LM_SubVector(&pc, &_pObj->BSCenter, _rayPos);
	v = LM_f_DotProduct(&pc, _rayDir);

	if (_pObj->BSRadiusSquare - LM_f_SqrNormVector(&pc) + (v*v) < 0.0f)
		return false;

	// check all the faces of the obj
	for (i = 0; i < _pObj->nbFaces; i++)
	{
		if (i == _faceIndex)
			continue;

		face = &_pObj->p_tdst_faces[i];

		// check if this face cast shadows
		if (_bDoingShadow && !face->castShadows)
			continue;

		// reject face on bounding sphere
		LM_SubVector(&pc, &face->BSCenter, _rayPos);
		v = LM_f_DotProduct(&pc, _rayDir);

		if (face->BSRadiusSquare - LM_f_SqrNormVector(&pc) + (v*v) < 0.0f)
			continue;

		if (intersectTri(_rayPos, _rayDir, face, &pc, &pc, &v, false))
		{
			if (_directLight)
			{
				if (v > 0)
				{
					*_pFace = face;
					return true;
				}
			}
			else
			{
				if (v < _rayLength - LM_Epsilon)
				{
					*_pFace = face;
					return true;
				}
			}
		}
	}


	return false;

}

bool ComputeShadow(LM_tdst_Vector* _texelPos, LM_tdst_Vector* _lightDir, LM_tdst_Vector* _lightPos, LIGHT_tdst_Light* _lightObj,  tdst_LMObj* _pObj, int _faceIndex, LMColor* _shadowColor, tdst_LMLight* _pLight, bool _directLight)
{
	
	LM_tdst_Vector worldLightRayPos;
	LM_tdst_Vector worldLightRayDir;
	LM_tdst_Vector lightRayPos;
	LM_tdst_Vector lightRayDir;
	bool isInShadow;

	float distanceToLight;
	int i;

	//The ray's origin is the light source and it's direction is towards the point.
	LM_CopyVector(&worldLightRayPos, _lightPos);
	LM_SubVector(&worldLightRayDir, _texelPos, _lightPos);

	//Get the distance (on the ray) to the object currently being lit
	distanceToLight = LM_f_NormVector(&worldLightRayDir);

	if (_directLight)
	{
		// switch _texelPos to world pos
		//LM_TransformVertex(&worldLightRayPos, &_pObj->toWorld, _texelPos);
		LM_CopyVector(&worldLightRayPos, _texelPos);
	}

	if (_lightDir) 
	{
		LM_CopyVector(&worldLightRayDir, _lightDir);

		if (!_directLight)
			LM_MulEqualVector(&worldLightRayDir, -1);
	}
	else
	{
		LM_NormalizeEqualVector(&worldLightRayDir);
	}

  	//// first try the last shadow casting face, there's still good chances it's still valid 
	if (_pLight->pLastShadowCastingFace)	
	{ 
		LM_tdst_Vector dummy;
		LMReal	dist;
 
		// find light ray in obj pos
		LM_TransformVertex(&lightRayPos, &_pLight->pLastShadowCastingObject->worldToMesh, &worldLightRayPos); 
		LM_TransformVector(&lightRayDir, &_pLight->pLastShadowCastingObject->worldToMesh, &worldLightRayDir);

		isInShadow = intersectTri(&lightRayPos, &lightRayDir, _pLight->pLastShadowCastingFace, &dummy, &dummy, &dist, false);
 
		// check the face  
		if (_directLight)
		{
			if (isInShadow && (dist > 0))
				isInShadow = true;
			else 
				isInShadow = false;
		}
		else
		{
			if (isInShadow && (dist < distanceToLight - LM_Epsilon))
				isInShadow = true;
			else 
				isInShadow = false;
		}

		if (isInShadow)
		{			
#ifdef SHADOW_TRACE
			_shadowColor->r = 1.0f; _shadowColor->g = 1.0f; _shadowColor->b = 0.0f;
#else
			_shadowColor->r = _shadowColor->g = _shadowColor->b = gShadowOpacity;
#endif
			_shadowColor->a = 1.0f;       

			return true;
		}
		else
		{
			// face is not valid anymore
			_pLight->pLastShadowCastingFace = NULL;
			_pLight->pLastShadowCastingObject = NULL;
		}
	}
	

	for (i = 0; i < gNbObj; i++)
	{
		if (!gObjList[i].settings.bCastShadows)
			continue;

		// find light ray
		LM_TransformVertex(&lightRayPos, &gObjList[i].worldToMesh, &worldLightRayPos);
		LM_TransformVector(&lightRayDir, &gObjList[i].worldToMesh, &worldLightRayDir);

		isInShadow = IntersectRay(&gObjList[i], (&gObjList[i] == _pObj)? _faceIndex: -1, &lightRayPos, &lightRayDir, distanceToLight, &_pLight->pLastShadowCastingFace, _directLight, true);

		if (isInShadow)
		{
			// there's been a shadow for now we do not use the texture alpha when comuting the shadow
			_pLight->pLastShadowCastingObject = &gObjList[i];

#ifdef SHADOW_TRACE
			_shadowColor->r = 1.0f; _shadowColor->g = 1.0f; _shadowColor->b = 0.0f;
#else
			_shadowColor->r = _shadowColor->g = _shadowColor->b = gShadowOpacity;
#endif
			_shadowColor->a = 1.0f;
            return true;
		}
 		
	}

	// no shadow
	 _shadowColor->a  = _shadowColor->r = _shadowColor->g = _shadowColor->b = 1.0f;
	return false;

}

bool  ComputeTexelColor(LMColor* _Color, LM_tdst_Vector*  _texelPos, LM_tdst_Vector* _normal, LM_tdst_Vector* _lightDir, LM_tdst_Vector* _lightPos, LIGHT_tdst_Light* _lightObj, tdst_LMObj* _pObj, int _faceIndex, bool _computeShadows, tdst_LMLight* _pLight)
{
	LM_tdst_Vector incident;
	float cosIncidentNormal = 0, lightIntensity = 0;
	LMColor shadowColor;
	bool	inShadow;
	bool	forceFindShadow = false;

	if ((_lightObj->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct)
        //|| 
		//( ((_lightObj->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Omni) && (_lightObj->ul_Flags & LIGHT_Cul_LF_OmniIsDirectional) ))
	{
		// if it's a direct light the incident vector is the light direction
		LM_CopyVector(&incident, _lightDir); 
		LM_NormalizeEqualVector(&incident);

		//Compute the cos of the angle between Incident and the normal. 
		//Since both are Unit vectors, the dot product gives this value		
		cosIncidentNormal = LM_f_DotProduct(_normal, &incident);
	}
	else
	{ 
		//Compute the incident vector 
		LM_SubVector(&incident, _lightPos, _texelPos);   
 		LM_NormalizeEqualVector(&incident); 

		//Compute the cos of the angle between Incident and the normal. 
		//Since both are Unit vectors, the dot product gives this value		
		cosIncidentNormal = LM_f_DotProduct(_normal, &incident);
	}	
	 
	/*if (_lightObj->ul_Flags & LIGHT_Cul_LF_UseDifferentColorForStatic)
	{
		_Color->r = ((_lightObj->ul_StaticColor& 0x000000FF) * (1.0f/255.0f));
		_Color->g = (((_lightObj->ul_StaticColor & 0x0000FF00) >> 8)  * (1.0f/255.0f));
		_Color->b = (((_lightObj->ul_StaticColor & 0x00FF0000) >> 16) * (1.0f/255.0f));
		_Color->a = 1.0f;
	}
	else*/
	{
		_Color->r = 1.0f;//((_lightObj->ul_Color & 0x000000FF) * (1.0f/255.0f));
		_Color->g = 1.0f;//(((_lightObj->ul_Color & 0x0000FF00) >> 8)  * (1.0f/255.0f));
		_Color->b = 1.0f;//(((_lightObj->ul_Color & 0x00FF0000) >> 16) * (1.0f/255.0f));
		_Color->a = 1.0f;
	}

    //if CosIncidentNormal is negative, this means that the point being lit is not facing the light,
    //thus is not illuminated by that light.
    if (cosIncidentNormal < 0.0f)
	{ 
#ifdef LOG_RAYTRACE
		ERR_OutputDebugString("Light: %s, Point: %f %f %f, normal %f %f %f, incident cos: %f\n", pCurrentLight->sz_Name, _texelPos->x, _texelPos->y, _texelPos->z, _normal->x, _normal->y, _normal->z, cosIncidentNormal);
#endif
		// if we're only generating shadows for this light we have to assume full lighting on the back side to get the correct shadows by color difference
        forceFindShadow = true;
        cosIncidentNormal = 1.0f;   // so we don't get the backfacing part considered
	}	 
	
	//Compute the intensity of the light ray that arrives at the lit point
	if (ComputeLightIntensity(&lightIntensity, _lightObj, _lightDir, _lightPos, _texelPos) || forceFindShadow) 
	{
#ifdef LOG_RAYTRACE
		ERR_OutputDebugString("Light: %s, Point: %f %f %f, normal %f %f %f, incident cos: %f, light intensity: %f\n", pCurrentLight->sz_Name, _texelPos->x, _texelPos->y, _texelPos->z, _normal->x, _normal->y, _normal->z, cosIncidentNormal, lightIntensity);
		ERR_OutputDebugString("Light pos: %f %f %f, Light Dir: %f %f %f, Incident Vector: %f %f %f\n", _lightPos->x, _lightPos->y, _lightPos->z, _lightDir->x, _lightDir->y, _lightDir->z, incident.x, incident.y, incident.z);		
#endif

		// Compute the shadows if necessary
		if (_computeShadows && _pObj->p_tdst_faces[_faceIndex].receiveShadows)
  		{
			shadowColor.a  = shadowColor.r = shadowColor.g = shadowColor.b = 1.0f;

			if ((_lightObj->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct
                || 
				( ((_lightObj->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Spot) && (_lightObj->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical) ))
 			{
				// if it's a direct we need to cast the ray from the texel world pos
				// and see if it's 

				//if the light is a direct light, it is necessary to compute a new origin for the
                //light ray (that new origin is the point for which the ray from it to the lit point
                //is parallel to the light's direction) since we want a shadow computation reflecting a
				//parrallel light ray for every texel, and it must be removed enough so that 
				LM_tdst_Vector delta, temp;

				// light dir is normalized, so if we multiply by a big enough value we should have the effect of 
				// a very removed light source, since 1 is a meter 500 is  500m, if our maps are bigger than
				// this it's gonna blow somewhere else so this should be a big enough value
				LM_MulVector(&temp, _lightDir, 500.0f);
				LM_SubVector(&delta, &temp, _texelPos);		 	

				inShadow = ComputeShadow(_texelPos, _lightDir, &delta, _lightObj, _pObj, _faceIndex, &shadowColor, _pLight, true);
			}
			else
			{
				inShadow = ComputeShadow(_texelPos, NULL, _lightPos, _lightObj, _pObj, _faceIndex, &shadowColor, _pLight, false);
			}

#ifdef LOG_RAYTRACE
			if(inShadow)
			{
				ERR_OutputDebugString("In Shadow\n");
			}
#endif

			MulLMColor( _Color, _Color, & shadowColor );

            // alpha of the light color is the opacity of the lightmap for this light
            FLOAT fOpacity = 1.0f - (((_lightObj->ul_Color & 0xFF000000) >> 24) * (1.0f/255.0f));
            _Color->r += fOpacity;
            _Color->g += fOpacity;
            _Color->b += fOpacity;
            if(_Color->r > 1.0f) _Color->r = 1.0f;
            if(_Color->g > 1.0f) _Color->g = 1.0f;
            if(_Color->b > 1.0f) _Color->b = 1.0f;
		} 
		
	}

    // DJ_TEMP : no attenuation/lighting in lightmap
	//MulLMColor(_Color, lightIntensity);// * cosIncidentNormal);

#ifdef LOG_RAYTRACE
	if (_Color->r || _Color->g || _Color->b)
	{
		ERR_OutputDebugString("Light Contribution: %f %f %f\n", _Color->r, _Color->g, _Color->b);
	}
#endif

	return inShadow;
}

void ComputeLumelColor(LMColor* _color, LM_tdst_Vector* _texelPos, LM_tdst_Vector* _normal, tdst_LMObj* _pObj, int _faceIndex, bool _computeShadows, TAB_tdst_Ptable* _pst_LightList)
{
	OBJ_tdst_GameObject* gameObj;
	OBJ_tdst_GameObject* gameGeoObj;
	LIGHT_tdst_Light* lightObj;
	tdst_LMLight* pLight;
	LM_tdst_Vector temp, lightDir, lightPos;
	ULONG i;
	LMColor	lightColor;
	LMColor	lightNoShadowColor;
	//tdstLightRejectionListElem *pItem;
	bool	goToNextLight;

	_color->a = _color->r = _color->g = _color->b = 0.0f;
	lightNoShadowColor.a = lightNoShadowColor.r = lightNoShadowColor.g = lightNoShadowColor.b = 0.0f;

	// loop on all lights of the scene
	for (i = 0; i < TAB_ul_Ptable_GetMaxNbElems(_pst_LightList); i++)
	{
		pLight = (tdst_LMLight*) _pst_LightList->p_Table[i];

		if (TAB_b_IsAHole(pLight))
			continue;

		gameObj = pLight->pLight;
		goToNextLight = false;

		// cast as light
		lightObj = (LIGHT_tdst_Light*)gameObj->pst_Extended->pst_Light;
		
		// check if it's active
#if 0 //TODO: ~hogsy
		if (!(lightObj->ul_Flags & LIGHT_Cul_LF_Active) || !(lightObj->ul_Flags & LIGHT_Cul_LF_UseLightMap))
#else
		if ( !( lightObj->ul_Flags & LIGHT_Cul_LF_Active ) )
#endif
		{
			continue;
		}

		// if it's a directional and the object rejects directional to not use it
		/*if ( (((lightObj->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct) ||
			(((lightObj->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Omni) && (lightObj->ul_Flags & LIGHT_Cul_LF_OmniIsDirectional) )  )
			&&  _pObj->gameObj->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_RejectDirectionals)
		{
			if (!(lightObj->ul_Flags & LIGHT_Cul_LF_DoNotExcludeFromDirectionals))
				continue;
		}*/

		// check if object is in light rejection list
		gameGeoObj = _pObj->gameObj;

		// check the lightset rejection mask
		//if (gameGeoObj->pst_Base->pst_Visu->c_LightSetMask & LIGHT_GetLightSetIDMask(lightObj->ul_Flags))
		//	continue;


		/*pItem = g_LightRejectionList.pFirst;
		while(pItem && !goToNextLight)
		{
			if(pItem->pLight == gameObj && pItem->pObject == gameGeoObj)
				goToNextLight = true;
			pItem = pItem->next;
		}

		if (goToNextLight)
			continue;*/

		// get direction and orientation
		temp.x = temp.z = 0.0f; 
		temp.y = 1.0f;

		LM_tdst_Matrix globalMatrix;
		LM_CopyToLMMatrix(&globalMatrix, gameObj->pst_GlobalMatrix);
		LM_TransformVector(&lightDir, &globalMatrix, &temp);
		LM_NormalizeEqualVector(&lightDir);

		LM_CopyToLMVector(&lightPos, &gameObj->pst_GlobalMatrix->T);

		pCurrentLight = gameObj;

 		ComputeTexelColor(&lightColor, _texelPos, _normal, &lightDir, &lightPos, lightObj, _pObj, _faceIndex, _pObj->settings.bReceiveShadows && gComputeShadows, pLight); 
		AddLMColor( _color, _color, &lightColor );

		if (gShadowMapMode)		
		{
			ComputeTexelColor(&lightColor, _texelPos, _normal, &lightDir, &lightPos, lightObj, _pObj, _faceIndex, false, pLight);
			AddLMColor( &lightNoShadowColor, &lightNoShadowColor, &lightColor );
		}
		 	
	}
 
	// add ambient	 
	AddLMColor( &lightNoShadowColor, &lightNoShadowColor, &_pObj->objAmbient );
	AddLMColor( _color, _color, &_pObj->objAmbient );	

	if (gShadowMapMode)	  
	{
		// compute the shadow factor 
		_color->r = MATH_Min(_color->r, 2); 
		_color->g = MATH_Min(_color->g, 2);
		_color->b = MATH_Min(_color->b, 2);

		lightNoShadowColor.r = MATH_Min(lightNoShadowColor.r, 2);
		lightNoShadowColor.g = MATH_Min(lightNoShadowColor.g, 2);
		lightNoShadowColor.b = MATH_Min(lightNoShadowColor.b, 2);

        // apply attenuation
        // DJ_TEMP : off for now
		_color->a = 1.0f;                  
		/*_color->r += 1.0f-lightNoShadowColor.r; 
		_color->g += 1.0f-lightNoShadowColor.g;
		_color->b += 1.0f-lightNoShadowColor.b;
        */

		// convert to B&W shadow (for PS2)
		_color->r  = _color->g = _color->b = (_color->r + _color->g + _color->b) /3.0f;
 
		_color->r = MATH_Min(  _color->r, 1);
		_color->g = MATH_Min(_color->g, 1);
		_color->b = MATH_Min(_color->b, 1);

	}
}

#define MAX_ELEMENT_RENDERFACES 5000

void ComputeLighting(tdst_LMObj* _pObj, int _nbObj, TAB_tdst_Ptable* _pst_LightList)
{
	int i, k,x, y, y1, x1;
	UINT j;
	LMColor* colorBuffer;
	LMColor *index, *start;
	LMColor color;
	tdst_LMObj* pObj = _pObj;
	tdst_LMAxisElement* pElement;
	float texelPerMeter;
	int currentWidth, currentHeight;
	tdst_LMFace* elementRenderFaces;
	tdst_LMFace* lastFace = NULL;
	int	lastFaceIndex = 0;
	int nbElementRenderFaces = 0;
	LM_tdst_Vector rayDir, rayPos;
	LM_tdst_Matrix worldToMesh;
	LM_tdst_Matrix camToWorld;
	LM_tdst_Matrix camToMesh;
	LM_tdst_Vector minusZ, posX, posY, zero, temp, temp2, temp3;
	LM_tdst_Vector xVect, yVect, origin;
	LM_tdst_Vector bc, normal, hitPos;
	bool emptymap;
	int faceIndex;
#ifdef OLD_ELEMENT_PACKING
	unsigned int intcolor;
#endif // #ifdef OLD_ELEMENT_PACKING
	char message[1024];
 
	minusZ.x = minusZ.y = 0.0;
	minusZ.z = -1.0;

	posX.x = 1.0;
	posX.y = posX.z = 0.0;

	posY.y = 1.0;
	posY.x = posY.z = 0.0;
	
	zero.x = zero.y = zero.z = 0;

#ifdef OLD_ELEMENT_PACKING
	if (gNbTempPage == 0)
		return;
#endif

	// since we compute the packing after determining the max page size we use an arbitrary one here@
	int currentMaxWidth = gMaxWidth * (gSuperSampling? gSuperSamplingFactor: 1);
	int currentMaxHeight = gMaxHeight * (gSuperSampling? gSuperSamplingFactor: 1);

#ifdef OLD_ELEMENT_PACKING
	gLightmapPages = (LIGHT_tdst_LightmapPageInfo*)LM_Alloc(sizeof(LIGHT_tdst_LightmapPageInfo) * gNbTempPage);
#endif

	// allocate memory for elementRenderFaces
	elementRenderFaces =(tdst_LMFace*)LM_Alloc( sizeof(tdst_LMFace) * MAX_ELEMENT_RENDERFACES);

#ifdef OLD_ELEMENT_PACKING
	// allocate data for the maps
	for (i = 0 ; i < gNbTempPage; i++)
	{
		gLightmapPages[i].pData = LM_Alloc(gTempLMPageInfo[i].h * gTempLMPageInfo[i].w * 4);
		memset(gLightmapPages[i].pData, 0xFF, gTempLMPageInfo[i].h * gTempLMPageInfo[i].w * 4);
		gLightmapPages[i].pageHeight = gTempLMPageInfo[i].h;
		gLightmapPages[i].pageWidth = gTempLMPageInfo[i].w;		
	}
#endif

	// allocate buffer for rendering (we don't render in the page in case we use supersampling)
	colorBuffer = (LMColor*) LM_Alloc(currentMaxHeight * currentMaxWidth * sizeof(LMColor));
	if ( colorBuffer == NULL )
	{
		ERR_OutputDebugString( "Failed to allocate colour buffer for lightmap, aborting!\n" );
		LM_Free( elementRenderFaces );
		return;
	}

	int elementCounter = 0;
	// for each object
 	for (i = 0; i < _nbObj; i++, pObj++)
	{
		if (!(pObj->settings.bReceiveShadows && pObj->settings.bUseLightmaps) || pObj->settings.bTemporaryStopUsingLightmaps) 
			continue;

		if (pObj->nbElements == 0)
			continue;

		lastFace = NULL;           

		// for each axis element
		for (j = 0; j < TAB_ul_Ptable_GetNbElems(&pObj->axisElementsList); j++)
		{
			pElement = (tdst_LMAxisElement*)pObj->axisElementsList.p_Table[j];

			// skip hole
			if (TAB_b_IsAHole(pElement))
				continue;

#ifdef LOG_RAYTRACE
			ERR_OutputDebugString("\n===========================================================================\nLighting Axis Element: %i, axis: %i, width: %i, height: %i, normal: %f %f %f, %i\n===========================================================================\n\n",
				j, pElement->elemAxis, pElement->mapwidth, pElement->mapheight, pElement->norm.x, pElement->norm.y, pElement->norm.z, pElement->numFaces);
#endif

	 		pElement->pMap = (LMColor*) LM_Alloc(pElement->mapheight * pElement->mapwidth * sizeof(LMColor));

			// determine pixel ratio
			texelPerMeter = (gSuperSampling)? pElement->texelPerMeter * gSuperSamplingFactor: pElement->texelPerMeter;
			
			currentWidth = pElement->mapwidth * ((gSuperSampling)? gSuperSamplingFactor: 1);
			currentHeight = pElement->mapheight * ((gSuperSampling)? gSuperSamplingFactor: 1);
 
			sprintf(message, "Lighting %i of %i, faces %i, %i X %i", elementCounter, gTotalNbAxisElement, pElement->numFaces, currentWidth, currentHeight); 
			gGlobalLMCallback((0.2 + (((float)elementCounter / (float)gTotalNbAxisElement) * 0.7)), message, gGlobalThis); 
			elementCounter++;
			ERR_OutputDebugString("%s\n", message);

			L_memset(colorBuffer, 0, currentMaxHeight * currentMaxWidth * sizeof(LMColor));

			start = colorBuffer;
			nbElementRenderFaces= 0;

			// generate list of element render faces
			for (k = 0; k < pObj->nbFaces; k++)
			{
				if (pElement->pAxisFaces[k].render)				
				{
					memcpy(&elementRenderFaces[nbElementRenderFaces], &pObj->p_tdst_faces[k], sizeof(tdst_LMFace));
					nbElementRenderFaces++;

					ERR_X_ErrorAssert(nbElementRenderFaces < MAX_ELEMENT_RENDERFACES, "Too many render faces", "");
				} 
			}

			// prepare the ray casting
			LM_InvertMatrix(&worldToMesh, &pObj->toWorld);
			LM_InvertMatrix(&camToWorld, &pElement->worldToCam);
			LM_MulMatrixMatrix(&camToMesh, &camToWorld, &worldToMesh);

			LM_TransformVector(&rayDir, &camToMesh, &minusZ);		
			LM_NormalizeEqualVector(&rayDir);

			LM_TransformVector(&xVect, &camToMesh, &posX);
			LM_TransformVector(&yVect, &camToMesh, &posY);
			LM_MulVector(&temp, &xVect, ((currentWidth * 0.5f) - 0.5f) * (1.0f / texelPerMeter));
			LM_MulVector(&temp2, &yVect, ((currentHeight * 0.5f) - 0.5f) * (1.0f / texelPerMeter));
			//LM_AddVector(&temp3, &temp, &temp2);
			//LM_SubVector(&temp, &zero, &temp3);
  
			LM_TransformVertex(&temp3, &camToMesh, &zero);
			LM_SubEqualVector(&temp3, &temp);
			LM_AddVector(&origin, &temp3, &temp2);

			LM_MulVector(&temp, &rayDir, -10.0f);
			LM_AddEqualVector(&origin, &temp);

			emptymap = true;

			// loop on all the points and cast rays

			LM_MulVector(&temp3, &xVect, 1.0f / texelPerMeter);

			for  (y = 0; y < currentHeight; y++)
			{
				LM_CopyVector(&rayPos, &origin);
				LM_MulVector( &temp, &yVect, ( float ) ( y ) *1.0f / texelPerMeter );
				LM_SubEqualVector(&rayPos, &temp);

				
				for (x = 0; x < currentWidth; x++)
				{
					faceIndex = GetFace(elementRenderFaces, nbElementRenderFaces, &rayPos, &rayDir, 1.0f / texelPerMeter, &xVect, &yVect, &bc, &hitPos, &lastFace, &lastFaceIndex);
					lastFaceIndex = -1;
					lastFace = NULL;

					if (faceIndex == -1)
					{
//#define FACEHIT_TRACE

#ifdef FACEHIT_TRACE
						start->r = 1;
						start->g = 0;
						start->b = 0;
						start->a = 0;
#else
						start->r = 1;
						start->g = 1;      
						start->b = 1;
						start->a = 0;
#endif
					}
					else
					{
						// transform the face index from an elementrenderfaceindex to a 
						emptymap = false;
						faceIndex = (elementRenderFaces + faceIndex)->objfaceindex;

#ifdef LOG_RAYTRACE
						ERR_OutputDebugString("\n\n\n\nLigting texel in face: %i, axis: %i, face indexes: %i %i %i, facenormal: %f %f %f\n", faceIndex, pObj->p_tdst_faces[faceIndex].axis,pObj->p_tdst_faces[faceIndex].index[0], pObj->p_tdst_faces[faceIndex].index[1], pObj->p_tdst_faces[faceIndex].index[2], pObj->p_tdst_faces[faceIndex].facenormal.x, pObj->p_tdst_faces[faceIndex].facenormal.y, pObj->p_tdst_faces[faceIndex].facenormal.z);
#endif

						// TODO: Do normal interpolation, this is crap
						LM_MulVector(&normal, &pObj->p_tdst_faces[faceIndex].normal[0], bc.x);  
						LM_MulVector(&temp, &pObj->p_tdst_faces[faceIndex].normal[1], bc.y);
						LM_AddEqualVector(&normal, &temp);
						LM_MulVector(&temp, &pObj->p_tdst_faces[faceIndex].normal[2], bc.z);
						LM_AddEqualVector(&normal, &temp); 
						
						//LM_CopyVector(&normal, &pObj->p_tdst_faces[faceIndex].facenormal);		 				 
		
						/*LM_MulVector(&normal, &pObj->p_tdst_faces[faceIndex].normal[0], 0.3f);
						LM_MulVector(&temp, &pObj->p_tdst_faces[faceIndex].normal[1], 0.3f);
						LM_AddEqualVector(&normal, &temp);
						LM_MulVector(&temp, &pObj->p_tdst_faces[faceI  ndex].normal[2], 0.3f);
						LM_AddEqualVector(&normal, &temp);*/

						LM_NormalizeEqualVector(&normal); 

#ifdef LOG_RAYTRACE
						ERR_OutputDebugString("Interpolated normal: %f %f %f, coords %f %f %f, vertex normals %f %f %f, %f %f %f, %f %f %f\n",
											normal.x, normal.y, normal.z, bc.x, bc.y, bc.z, pObj->p_tdst_faces[faceIndex].normal[0].x, pObj->p_tdst_faces[faceIndex].normal[0].y, pObj->p_tdst_faces[faceIndex].normal[0].z,
											pObj->p_tdst_faces[faceIndex].normal[1].x, pObj->p_tdst_faces[faceIndex].normal[1].y, pObj->p_tdst_faces[faceIndex].normal[1].z, 
											pObj->p_tdst_faces[faceIndex].normal[2].x, pObj->p_tdst_faces[faceIndex].normal[2].y, pObj->p_tdst_faces[faceIndex].normal[2].z); 

						ERR_OutputDebugString("Vertex 0: %f %f %f, Vertex 1: %f %f %f, Vertex 2: %f %f %f\n", 
											pObj->p_tdst_faces[faceIndex].vertex[0].x, pObj->p_tdst_faces[faceIndex].vertex[0].y, pObj->p_tdst_faces[faceIndex].vertex[0].z,
											pObj->p_tdst_faces[faceIndex].vertex[1].x, pObj->p_tdst_faces[faceIndex].vertex[1].y, pObj->p_tdst_faces[faceIndex].vertex[1].z, 
											pObj->p_tdst_faces[faceIndex].vertex[2].x, pObj->p_tdst_faces[faceIndex].vertex[2].y, pObj->p_tdst_faces[faceIndex].vertex[2].z);						
#endif

						LM_TransformVertex(&temp, &pObj->toWorld, &hitPos);        
						LM_TransformVector(&temp2, &pObj->toWorld, &normal);
						LM_NormalizeEqualVector(&temp2);

						ComputeLumelColor(&color, &temp, &temp2, pObj, faceIndex, gComputeShadows, _pst_LightList); 

						// TODO: Add posibility to go to x2 exposition....
						// Clamp the color
						start->a  = MATH_Min(color.a, 1);
						start->r = MATH_Min(color.r, 1);
						start->g = MATH_Min(color.g, 1);						
						start->b = MATH_Min(color.b, 1);							

						// it's either there or not there, we assume alpha = 1.0f means the texel is in the map
						// 0 means it's not in it
						if (start->a != 0.0f)
							start->a = 1.0f;

#define TEST_QUANTIZATION
#ifdef TEST_QUANTIZATION
						int quantize;
						// quantize to 16 steps (4 bits)
						start->r *= 16.0f; quantize = start->r; start->r = quantize * (1.0f/16.0f);
						start->g *= 16.0f; quantize = start->g; start->g = quantize * (1.0f/16.0f);
						start->b *= 16.0f; quantize = start->b; start->b = quantize * (1.0f/16.0f);
#endif

					}

//#define BORDER_TRACE

					#ifdef BORDER_TRACE

					#ifdef SINGLE_PIXEL_BORDER
					if (x == 0 || x == (currentWidth -1))
					#else
					if (x == 0 || x == 1 || x == (currentWidth -1) || x == (currentWidth - 2))
					#endif
					{
						//start->a = 1.0f;
						start->a = 0.0f;
						start->r = 0.0f;
						start->g = 0.0f;
						start->b = 1.0f;
					}

					#ifdef SINGLE_PIXEL_BORDER
					if (y == 0 || y == (currentHeight - 1))
					#else
					if (y == 0 || y == 1 || y == (currentHeight - 1) || y == (currentHeight - 2))
					#endif
					{
						//start->a = 1.0f;
						start->a = 0.0f;
						start->r = 0.0f;
						start->g = 1.0f;
						start->b = 0.0f;
					} 
					#endif

					start++;

					// go to next texel
					LM_MulVector(&temp3, &xVect, 1.0f/ texelPerMeter);
					LM_AddEqualVector(&rayPos, &temp3);
				}

			}


  			if (gSuperSampling)
			{
				LMColor* writeIndex = colorBuffer;
				index = colorBuffer;				

				for (y =0; y < pElement->mapheight; y++)
				{
					for (x = 0; x < pElement->mapwidth; x++)
     				{
						LMColor* newIndex;
						LMColor newColor = {0, 0, 0, 0};
						int numValid= 0;

						for (y1 = 0; y1 < gSuperSamplingFactor; y1++)
						{
							for (x1= 0; x1 < gSuperSamplingFactor; x1++)
							{
								newIndex = index + x1 + (y1 * currentWidth);								
 
								if (newIndex->a > 0.0f)
								{
									AddLMColor( &newColor, &newColor, newIndex );
									numValid++;
								}
							}
						}
						if (numValid)
						{
							MulLMColorClamp(writeIndex, &newColor, 1.0f/numValid, 1.0f);
							writeIndex->a = 1.0f;
						}
						else
						{
							writeIndex->a = 0.0f;
							writeIndex->r = 0.0f;
							writeIndex->g = 0.0f;
							writeIndex->b = 0.0f;
						}

						writeIndex++;
						index += (int)gSuperSamplingFactor;						
					}
					
					index += (currentWidth * ((int)gSuperSamplingFactor - 1));
				}
			}

			// smooth the border to eleminate filtering artifacts
			LMColor* neighbor;
			index = colorBuffer;
			int	deltax, deltay;

			for (y =0; y < pElement->mapheight; y++)
			{
				for (x = 0; x < pElement->mapwidth; x++)
     			{
					// if the alpha value is 1.0f it's in the map so we check the neighbors
					// to see if they are <= 0.0f and accumulate the color of the current texel
					// in them, we'll do another pass after this to put them back in the correct
					// range
					if (index->a == 1.0f)
					{
						// check the neighboors
						for (deltax = -1; deltax < 2; deltax++)
						{
							// make sure we don't go over the border
							if ((x == 0 && deltax == -1) || (x == (pElement->mapwidth-1) && deltax == 1))
			 					continue;

							for (deltay = -1; deltay < 2; deltay++)
							{
								// make sure we don't go over the border
								if ((y == 0 && deltay == -1) || (y == (pElement->mapheight-1) && deltay == 1))
									continue;

								if (deltax != 0 || deltay != 0)
								{
									 neighbor = index + deltax + (deltay * pElement->mapwidth);

									 if (neighbor->a <= 0.0f)
									 {
										 // accumulate index in it an increment the counter
										 // if it's the first one don't acummulate just assign
										 if (neighbor->a == 0.0f)
										 {
											 neighbor->r = index->r;
											 neighbor->g = index->g;
											 neighbor->b = index->b;
										 }
										 else
										 {
											 neighbor->r += index->r;
											 neighbor->g += index->g;
											 neighbor->b += index->b;
										 }

										 neighbor->a -= 1.0f;
									 }
								}
							}
						}
					}
					index++;
				}				
			}

			index = colorBuffer;

			for (y =0; y < pElement->mapheight; y++)
			{
				for (x = 0; x < pElement->mapwidth; x++)
     			{
					if (index->a < 0.0f)
					{
//#define DEBUG_BORDER_SMOOTH

#ifndef DEBUG_BORDER_SMOOTH
						index->r /= -index->a;
						index->g /= -index->a;
						index->b /= -index->a;
						index->a = 1.0f;
#else
						index->r = 1;
						index->g = 0;
						index->b = 1;
						index->a = 0.0f;
#endif
					}

					index++;
				}				
			}


			// copy calculated map to axiselement
			index = colorBuffer;
			LMColor* writeIndex = pElement->pMap;

			for (y = 0; y < pElement->mapheight; y++)				
			{
				for (x = 0; x < pElement->mapwidth; x++)	
				{
					*writeIndex = *index;
					writeIndex++;
					index++;
				}
			}
			

#ifdef OLD_ELEMENT_PACKING
			index = colorBuffer;			
			unsigned int* writePtr;

//			if (!emptymap) 
			{
   				gLightmapPages[pElement->lmIndex].empty = false;
				
				writePtr = ((unsigned int*)gLightmapPages[pElement->lmIndex].pData) +  (pElement->lmPosY * gLightmapPages[pElement->lmIndex].pageWidth) + pElement->lmPosX;

				for (y = 0; y < pElement->mapheight; y++)				
				{
					for (x = 0; x < pElement->mapwidth; x++)	
					{
						// TODO: add processing to convert lightmaps to PS2 format around here

						// convert to 32bits RGBA
						intcolor = 0;
						intcolor |= (unsigned char)(index->r* 255.0f);
						intcolor |= ((unsigned char)(index->g* 255.0f)) << 8;
						intcolor |= ((unsigned char)(index->b* 255.0f)) << 16;
						intcolor |= 0xFF000000;

						*writePtr = intcolor;
						index++;
						writePtr++;
					}					
					
					writePtr += (gLightmapPages[pElement->lmIndex].pageWidth -  pElement->mapwidth);
				}
			}
#endif

		}		
	}

	LM_Free(elementRenderFaces);
	LM_Free(colorBuffer);

//#define DEBUG_LIGHTMAPS

#ifdef DEBUG_LIGHTMAPS
// for debug we dump the lightmaps on the disk 

	char mapfilename[1024];
	FILE* file = NULL;

	for(i = 0; i < gNbTempPage; i++)
	{

		sprintf(mapfilename, "e:\\lightmappage%isize%ix%i.raw", i, gLightmapPages[i].pageWidth, gLightmapPages[i].pageHeight);
		file = fopen(mapfilename, "wb");

        fwrite(gLightmapPages[i].pData, gLightmapPages[i].pageWidth * gLightmapPages[i].pageHeight * 4, 1,file);
		fflush(file);
		fclose(file);
	}


#endif	

}

int axisElementCompare(const void* v1, const void* v2)
{
	tdst_LMAxisElement *pElem1, *pElem2;

	pElem1 = *(tdst_LMAxisElement**)v1;
	pElem2 = *(tdst_LMAxisElement**)v2;
 
	// check width first 
	if (pElem1->mapwidth > pElem2->mapwidth)
		return -1;
	if (pElem1->mapwidth < pElem2->mapwidth)
		return 1;
	if (pElem1->mapheight > pElem2->mapheight)
		return -1;
	if (pElem1->mapheight < pElem2->mapheight)
		return 1;

	// they are equal if we get to here
	return 0;
}

//#define TRACE_ELEMENT_PACKING

void DoElementPacking(tdst_LMObj* _pObj, int _nbObj)
{
	// get pow2 size
	int i, k;
	UINT j;
	int lmsize = MATH_Max(gMaxHeight, gMaxWidth);
	int size = MATH_Max(gMinPageSize, (int)L_pow(2, ceil(log10f(lmsize)/0.30103)));
	tdst_LMObj* pObj = _pObj;
	tdst_LMAxisElement* pElement;
	tdst_LMAxisElement** pGlobalElementList;
	tdst_LMAxisElement** pListNext;
	int currentMapIndex = 0;
	int index;
	bool elementplaced = false;

	gNbTempPage = 0;

	// init packing info
	for (i = 0; i < MAX_LIGHTMAP_PAGES; i++)
	{
		gTempLMPageInfo[i].w = size;
		gTempLMPageInfo[i].h = size;
		gTempLMPageInfo[i].borderx = 0;
		gTempLMPageInfo[i].bordery = 0;
		gTempLMPageInfo[i].start = (int*)LM_Alloc(size * sizeof(int));
		L_memset(gTempLMPageInfo[i].start, 0, size * sizeof(int));
	}

	// allocate global list of axis elements
	pGlobalElementList = (tdst_LMAxisElement**) LM_Alloc(gTotalNbAxisElement * sizeof(tdst_LMAxisElement*));
	pListNext = pGlobalElementList;

	// for each object
	for (i = 0; i < _nbObj; i++, pObj++)
	{
		if (!(pObj->settings.bReceiveShadows && pObj->settings.bUseLightmaps) || pObj->settings.bTemporaryStopUsingLightmaps)
			continue;

		// for each axis element
		for (j = 0; j < TAB_ul_Ptable_GetMaxNbElems(&pObj->axisElementsList); j++)
		{
			pElement = (tdst_LMAxisElement*)pObj->axisElementsList.p_Table[j];

			// skip hole
			if (TAB_b_IsAHole(pElement) || &pObj->axisElementsList.p_Table[j] > TAB_ppv_Ptable_GetLastElem(&pObj->axisElementsList))
				continue;

			// place element in list
			*pListNext = pElement;
			pListNext++;
		} 
	}

	// sort the list accoring to object element, material, width and height
	qsort(pGlobalElementList, gTotalNbAxisElement, sizeof(tdst_LMAxisElement*), axisElementCompare);

	// loop on all element and place them
	for (i = 0; i < gTotalNbAxisElement; i++)
	{
		pElement = pGlobalElementList[i];

		index = 0;  

		// find a place in the maps
		while (!elementplaced)
		{
			if (currentMapIndex+1 > gNbTempPage)				
			{
				gNbTempPage++;
			}

			while (index < size)
			{
				while(gTempLMPageInfo[currentMapIndex].start[index] + pElement->mapheight > size && index < size) 
					index++;

				if (index + pElement->mapwidth > size)
				{
					index = size;
					break;
				}

				if (index < size)
				{
					int indexstart = gTempLMPageInfo[currentMapIndex].start[index];

					for (k = 0; k < pElement->mapwidth; k++)
					{
						if (gTempLMPageInfo[currentMapIndex].start[index + k] != indexstart)
						{
							break;
						}
					}

					if (k != pElement->mapwidth)
					{
						index += k;
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}			

			if (index < size)
			{
				pElement->lmIndex = currentMapIndex;
				pElement->lmPosX = index;
				pElement->lmPosY = gTempLMPageInfo[currentMapIndex].start[index];

				gTempLMPageInfo[currentMapIndex].borderx = MATH_Max(gTempLMPageInfo[currentMapIndex].borderx, pElement->lmPosX + pElement->mapwidth -1);
				gTempLMPageInfo[currentMapIndex].bordery = MATH_Max(gTempLMPageInfo[currentMapIndex].bordery, pElement->lmPosY + pElement->mapheight -1);

				for (k = 0; k < pElement->mapwidth; k++)
				{
					gTempLMPageInfo[currentMapIndex].start[index + k] += pElement->mapheight;
				}
				
				currentMapIndex = 0;                
				elementplaced = true;

#ifdef TRACE_ELEMENT_PACKING
				char sz_Message[1024];
				sprintf(sz_Message, "Element %i placed in LM Page, page: %i, X: %i, Y: %i, W: %i, H: %i", i, pElement->lmIndex, pElement->lmPosX, pElement->lmPosY, pElement->mapwidth, pElement->mapheight);
				LINK_PrintStatusMsg(sz_Message);					
#endif
			}
			else
			{
#ifdef TRACE_ELEMENT_PACKING
				char sz_Message[1024 ];
				sprintf(sz_Message, "Element %i placed in can't be placed in LM Page %i, W: %i, H: %i", i, currentMapIndex, pElement->mapwidth, pElement->mapheight);
				LINK_PrintStatusMsg(sz_Message);					
#endif
				currentMapIndex ++;
				index = 0;
				ERR_X_ErrorAssert(currentMapIndex < MAX_TEMP_LIGHTMAP, "Too many lightmap pages", "");
			}
		}

		elementplaced = false;

	}

	// reducing phase
	for (i = 0; i < gNbTempPage; i++)
	{
		gTempLMPageInfo[i].w = (int) L_pow(2, ceil(log10f(gTempLMPageInfo[i].borderx) / 0.30103));
		gTempLMPageInfo[i].h = (int) L_pow(2, ceil(log10f(gTempLMPageInfo[i].bordery) / 0.30103));
	}

	// free global element list
	LM_Free(pGlobalElementList);

}

float gSemblanceThreshold = 0.9f;

float GetMapSimilarityCoefficient(tdst_LMAxisElement* _pElement1, tdst_LMAxisElement* _pElement2)
{
	int i, j;
	LMColor* pPixel1, *pPixel2;
	LMColor diff;
	float	distance = 0;
	bool	reject = false;

	if (_pElement1->mapwidth != _pElement2->mapwidth ||
		_pElement1->mapheight != _pElement2->mapheight)
	{
		return 0.0f;
	}

	// substract the maps for the pixel with alpha values different than zero
	pPixel1 = _pElement1->pMap;
	pPixel2 = _pElement2->pMap;

	for (i = 0; i < _pElement1->mapwidth && !reject; i++)
	{
		for (j = 0; j < _pElement1->mapheight && !reject; j++)
		{
			if (pPixel1->a == 1.0f && pPixel2->a == 1.0f)
			{
				// both pixels are used compare them				
				SubLMColor(&diff, pPixel1, pPixel2);
				diff.a = 0.0f;

				// must not use LM_NormVector since we cast from LMColor to MATH_tdst_Vector and LM_Vector can be float or double
				distance += MATH_f_NormVector((MATH_tdst_Vector*)&diff);					

				if (distance >= 1.0f/16.0f)
				{
					reject = true;
				}
			}
			else if (pPixel1->a == 1.0f)
			{
				// pixel1 is used copy it into the second map so both are now equal
				*pPixel2 = *pPixel1;
				
			}
			else if (pPixel2->a == 1.0f)
			{
				// pixel2 is used copy it into the first map so both are now equal
				*pPixel1 = *pPixel2;
			}
			else
			{
				// none of them are used they are irrevelant
			}		
			pPixel1++;
			pPixel2++;
		}
	}	

	if (reject)
	{
		return 0.0f;
	}
	else
	{
		return 1.0f;
	}

}

void DoMapCompression(tdst_LMAxisElement** _pElement, int _nbElement)
{
	int i, j;
	int	size1, size2;
	float semblance; 

	// loop on all the the maps
	for (i = 0; i < _nbElement; i++)
	{		
		// loop all on the other maps
		for (j = i +1; j < _nbElement; j++)
		{
			// compare maps to see if they are equal
			semblance = GetMapSimilarityCoefficient(_pElement[i], _pElement[j]);

			if (semblance > gSemblanceThreshold)
			{
				// keep the smallest one
				size1 = _pElement[i]->mapheight * _pElement[i]->mapwidth;
				size2 = _pElement[j]->mapheight * _pElement[j]->mapwidth;

				if (size1 <= size2)
				{
					// indicate map2 must use map1 has source
					_pElement[j]->pUseMapFromElement = _pElement[i];
				}
				else
				{
					// indicate map1 must use map2 has source
					_pElement[i]->pUseMapFromElement = _pElement[j];
				}
			}
		}
	}

		
}

tdst_LMPageLayout* GeneratePageLayout(int _startPageHeight, int _startPageWidth, tdst_LMAxisElement** _pAxisElementsList, int _nbAxisElements, bool _forceAllInSamePage)
{
	tdst_LMAxisElement* pElement;
	int*	elementPosX;
	int*	elementPosY;
	int*	elementPage;
	int currentMapIndex = 0;
	int index;
	bool elementplaced = false;
	int i, k;

	gNbTempPage = 0;

	// init packing info
	for (i = 0; i < MAX_LIGHTMAP_PAGES; i++)
	{
		gTempLMPageInfo[i].w = _startPageWidth;
		gTempLMPageInfo[i].h = _startPageHeight;
		gTempLMPageInfo[i].borderx = 0;
		gTempLMPageInfo[i].bordery = 0;
		gTempLMPageInfo[i].start = (int*)LM_Alloc(_startPageWidth * sizeof(int));
		L_memset(gTempLMPageInfo[i].start, 0, _startPageWidth * sizeof(int));
	}

	elementPosX = (int*) LM_Alloc(_nbAxisElements * sizeof(int));
	elementPosY = (int*) LM_Alloc(_nbAxisElements * sizeof(int));
	elementPage = (int*) LM_Alloc(_nbAxisElements * sizeof(int));

	// loop on all element and place them
	for (i = 0; i < _nbAxisElements; i++)
	{
		pElement = _pAxisElementsList[i];

		if (pElement->pUseMapFromElement)
		{
			// element is mapped on another axis element, do not pack it
			continue;
		}

		if (pElement->mapheight > _startPageHeight || pElement->mapwidth > _startPageWidth)
		{
			// free everything and return NULL it's impossible to fit this into the layout with the page size restrictions
			LM_Free(elementPosX);
			LM_Free(elementPosY);
			LM_Free(elementPage);

			for (i = 0; i < MAX_LIGHTMAP_PAGES; i++)
			{
				LM_Free(gTempLMPageInfo[i].start);
			}

			return NULL;
		}	

		index = 0;  

		// find a place in the maps
		while (!elementplaced)
		{
			if (currentMapIndex+1 > gNbTempPage)				
			{
				gNbTempPage++;
			}

			while (index < _startPageWidth)
			{
				while(gTempLMPageInfo[currentMapIndex].start[index] + pElement->mapheight > _startPageHeight && index < _startPageWidth) 
					index++;

				if (index + pElement->mapwidth > _startPageWidth)
				{
					index = _startPageWidth;
					break;
				}

				if (index < _startPageWidth)
				{
					int indexstart = gTempLMPageInfo[currentMapIndex].start[index];

					for (k = 0; k < pElement->mapwidth; k++)
					{
						if (gTempLMPageInfo[currentMapIndex].start[index + k] != indexstart)
						{
							break;
						}
					}

					if (k != pElement->mapwidth)
					{
						index += k;
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}			

			if (index < _startPageWidth)
			{
				elementPage[i] = currentMapIndex;
				elementPosX[i] = index;
				elementPosY[i] = gTempLMPageInfo[currentMapIndex].start[index];

				gTempLMPageInfo[currentMapIndex].borderx = MATH_Max(gTempLMPageInfo[currentMapIndex].borderx, elementPosX[i] + pElement->mapwidth -1);
				gTempLMPageInfo[currentMapIndex].bordery = MATH_Max(gTempLMPageInfo[currentMapIndex].bordery, elementPosY[i] + pElement->mapheight -1);

				for (k = 0; k < pElement->mapwidth; k++)
				{
					gTempLMPageInfo[currentMapIndex].start[index + k] += pElement->mapheight;
				}
				
				currentMapIndex = 0;                
				elementplaced = true;

			}
			else
			{
				if (_forceAllInSamePage)
				{
					LM_Free(elementPosX);
					LM_Free(elementPosY);
					LM_Free(elementPage);

					for (i = 0; i < MAX_LIGHTMAP_PAGES; i++)
					{
						LM_Free(gTempLMPageInfo[i].start);
					}

					return NULL;
				}
				currentMapIndex ++;
				index = 0;
				ERR_X_ErrorAssert(currentMapIndex < MAX_TEMP_LIGHTMAP, "Too many lightmap pages", "");
			}
		}

		elementplaced = false;

	}

	// reducing phase
	for (i = 0; i < gNbTempPage; i++)
	{
		gTempLMPageInfo[i].w = (int) L_pow(2, ceil(log10f(gTempLMPageInfo[i].borderx) / 0.30103));
		gTempLMPageInfo[i].h = (int) L_pow(2, ceil(log10f(gTempLMPageInfo[i].bordery) / 0.30103));
	}

	// copy the configuration to the layout	
	tdst_LMPageLayout* pLayout = (tdst_LMPageLayout*) LM_Alloc(sizeof(tdst_LMPageLayout));

	pLayout->pagesHeight = (int*) LM_Alloc(sizeof(int) * gNbTempPage);
	pLayout->pagesWidth = (int*) LM_Alloc(sizeof(int) * gNbTempPage);

	pLayout->nbPages = gNbTempPage;	
	pLayout->elementPage = elementPage;
	pLayout->elementStartPosX = elementPosX;
	pLayout->elementStartPosY = elementPosY;
	pLayout->totalSize = 0;

	for (i = 0; i < gNbTempPage; i++)
	{
		pLayout->pagesWidth[i] = gTempLMPageInfo[i].w;
		pLayout->pagesHeight[i] = gTempLMPageInfo[i].h;
		pLayout->totalSize += (gTempLMPageInfo[i].w * gTempLMPageInfo[i].h);
	}

	// free the temp info
	for (i = 0; i < MAX_LIGHTMAP_PAGES; i++)
	{
		LM_Free(gTempLMPageInfo[i].start);
	}

	return pLayout;

}

void FreePageLayout(tdst_LMPageLayout* _pLayout)
{
	LM_Free(_pLayout->elementPage);
	LM_Free(_pLayout->elementStartPosX);
	LM_Free(_pLayout->elementStartPosY);
	LM_Free(_pLayout->pagesHeight);
	LM_Free(_pLayout->pagesWidth);
}

#ifdef PACK_ONE_PAGE_PER_WORLD
void DoElementPackingAndCompressionPerWorld(tdst_LMObj* _pObj, int _nbObj)
{
	// get pow2 size
	int i;
	UINT j;
	tdst_LMObj* pObj = _pObj;          
	tdst_LMAxisElement* pElement;
	tdst_LMAxisElement** pGlobalElementList;
	tdst_LMAxisElement** pListNext;
	int currentMapIndex = 0;
	int testPageWidth, testPageHeight;
	bool elementplaced = false;
	tdst_LMPageLayout*	bestLayout = NULL;	


	// allocate global list of axis elements
	pGlobalElementList = (tdst_LMAxisElement**) LM_Alloc(gTotalNbAxisElement * sizeof(tdst_LMAxisElement*));
	pListNext = pGlobalElementList;

	// for each object
	for (i = 0; i < _nbObj; i++, pObj++)
	{
		if (!(pObj->settings.bReceiveShadows && pObj->settings.bUseLightmaps) || pObj->settings.bTemporaryStopUsingLightmaps)
			continue;

		if (pObj->nbElements == 0)
			continue;

		// for each axis element
		for (j = 0; j < TAB_ul_Ptable_GetNbElems(&pObj->axisElementsList); j++)
		{
			pElement = (tdst_LMAxisElement*)pObj->axisElementsList.p_Table[j];

			// skip hole
			if (TAB_b_IsAHole(pElement))
				continue;

			// place element in list
			*pListNext = pElement;
			pListNext++;
		} 
	}

	// sort the list accoring to object element, material, width and height
	qsort(pGlobalElementList, gTotalNbAxisElement, sizeof(tdst_LMAxisElement*), axisElementCompare);

	// compare maps between themselves to remove duplicates
	DoMapCompression(pGlobalElementList, gTotalNbAxisElement);


	// test all the possible size of maps for the one that offers the best configuration for the object
	for (testPageWidth = 8; testPageWidth <= 1024; testPageWidth <<= 1)                
	{
		for (testPageHeight = 8; testPageHeight <= 1024; testPageHeight <<= 1)
		{	
			tdst_LMPageLayout* testLayout;
			
			testLayout = GeneratePageLayout(testPageHeight, testPageWidth, pGlobalElementList, gTotalNbAxisElement, true);

			if (bestLayout)
			{
				if (testLayout && testLayout->totalSize < bestLayout->totalSize)
				{
					// this one is better keep it
					FreePageLayout(bestLayout);
					bestLayout = testLayout;
				}
				if (testLayout && testLayout->totalSize == bestLayout->totalSize)
				{
					// keep the layout closent to being square, and keep larger one by default
					float testRatio, bestRatio;

					bestRatio = (float)bestLayout->pagesWidth[0] / (float)bestLayout->pagesHeight[0];
					testRatio = (float)testLayout->pagesWidth[0] / (float)testLayout->pagesHeight[0];

					if (fabs(1 - testRatio) < fabs(1 - bestRatio))
					{
						// this one is better keep it
						FreePageLayout(bestLayout);
						bestLayout = testLayout;						
					}
				}	
			}
			else
			{	
				bestLayout = testLayout;
			}		
		}
	}

	// keep the best layout
	gPageLayout = bestLayout;
	gNbTempPage = gPageLayout->nbPages;

	// copy the calculated lighting data into texture pages
	LIGHT_tdst_LightmapPageInfo* pPage;

	// allocate data for the maps

	for (i = 0 ; i < gNbTempPage; i++)
	{
		pPage = LIGHT_Lightmaps_GetPointerForKey(i, true, false, true);

		pPage->pData = MEM_p_Alloc(gPageLayout->pagesHeight[i] * gPageLayout->pagesWidth[i] * 4);
		memset(pPage->pData, 0xFF, gPageLayout->pagesHeight[i] * gPageLayout->pagesWidth[i] * 4);
		pPage->pageHeight = gPageLayout->pagesHeight[i];
		pPage->pageWidth = gPageLayout->pagesWidth[i];			
	}

	// loop on all axis elements	
	LMColor* readptr;
	ULONG* writePtr;
	ULONG  intcolor;
	int x, y;

	for (i = 0; i < gTotalNbAxisElement; i++)
	{
		pElement = pGlobalElementList[i];
		
		readptr = pElement->pMap;

		// do not copy data from axiselements that use another element map
		if (pElement->pUseMapFromElement)
		{
			LM_Free(pElement->pMap);
			continue;
		}
	
		pElement->lmPosX = gPageLayout->elementStartPosX[i];
		pElement->lmPosY = gPageLayout->elementStartPosY[i];
		pElement->lmIndex = gPageLayout->elementPage[i];

		pPage = LIGHT_Lightmaps_GetPointerForKey(pElement->lmIndex, false, false, true);

		writePtr = ((ULONG*)pPage->pData) +  (pElement->lmPosY * pPage->pageWidth) + pElement->lmPosX;

		for (y = 0; y < pElement->mapheight; y++)			
		{
			for (x = 0; x < pElement->mapwidth; x++)
			{
				// convert to 32bits RGBA
				intcolor = 0;
				intcolor |= (unsigned char)(readptr->r* 255.0f);
				intcolor |= ((unsigned char)(readptr->g* 255.0f)) << 8;
				intcolor |= ((unsigned char)(readptr->b* 255.0f)) << 16;
				intcolor |= 0xFF000000;

				*writePtr = intcolor;
				readptr++;
				writePtr++;
			}					
			
			writePtr += (pPage->pageWidth -  pElement->mapwidth);
		}

		// free the axis element map
		LM_Free(pElement->pMap);		
	}

	// free global element list
	LM_Free(pGlobalElementList);  
}

#endif

#ifdef PACK_PER_OBJECT
void DoElementPackingAndCompressionPerObject(tdst_LMObj* _pObj, int _nbObj)
{
	// get pow2 size
	int i, k;
	UINT j;
	tdst_LMObj* pObj = _pObj;          
	tdst_LMAxisElement* pElement;
	tdst_LMAxisElement** pListNext;
	int currentMapIndex = 0;
	int index;
	int testPageWidth, testPageHeight;
	bool elementplaced = false;
	tdst_LMPageLayout*	bestLayout = NULL;	
	int		objBasePageIndex = 0;
	LIGHT_tdst_LightmapPageInfo tempPage;
	
	// for each object
	for (i = 0; i < _nbObj; i++, pObj++)
	{
		if (!(pObj->settings.bReceiveShadows && pObj->settings.bUseLightmaps) || pObj->settings.bTemporaryStopUsingLightmaps)
			continue;

		// allocate linear list of axis elements
		pObj->pSortedAxisElements = (tdst_LMAxisElement**) LM_Alloc(pObj->axisElementsList.ul_NbElems * sizeof(tdst_LMAxisElement*));		
		pObj->nbAxisElements = pObj->axisElementsList.ul_NbElems;

		pListNext = pObj->pSortedAxisElements;

		// for each axis element
		for (j = 0; j < TAB_ul_Ptable_GetNbElems(&pObj->axisElementsList); j++)
		{
			pElement = (tdst_LMAxisElement*)pObj->axisElementsList.p_Table[j];

			// skip hole
			if (TAB_b_IsAHole(pElement))
				continue;

			// place element in list
			*pListNext = pElement;
			pListNext++;
		} 

		// sort the list accoring to object element, material, width and height
		qsort(pObj->pSortedAxisElements, pObj->nbAxisElements, sizeof(tdst_LMAxisElement*), axisElementCompare);

		// compare maps between themselves to remove duplicates
		DoMapCompression(pObj->pSortedAxisElements, pObj->nbAxisElements);

		// test all the possible size of maps for the one that offers the best configuration for the object
		for (testPageWidth = 8; testPageWidth <= 512; testPageWidth <<= 1)
		{
			for (testPageHeight = 8; testPageHeight <= 512; testPageHeight <<= 1)
			{	
				tdst_LMPageLayout* testLayout;
				
				testLayout = GeneratePageLayout(testPageHeight, testPageWidth, pObj->pSortedAxisElements, pObj->nbAxisElements, false);

				if (bestLayout)
				{
					if (testLayout && testLayout->totalSize < bestLayout->totalSize && testLayout->nbPages <= bestLayout->nbPages)
					{
						// this one is better keep it
						FreePageLayout(bestLayout);
						bestLayout = testLayout;
					}
				}
				else
				{	
					bestLayout = testLayout;
				}		
			}
		}

		// keep the best layout
		pObj->pageLayout = bestLayout;

		// add pages to lightmaps

		// allocate data for the maps
		for (k = 0 ; k < pObj->pageLayout->nbPages; k++)
		{
			tempPage.pData = LM_Alloc(pObj->pageLayout->pagesHeight[k] * pObj->pageLayout->pagesWidth[k] * 4);
			memset(tempPage.pData, 0xFF, pObj->pageLayout->pagesHeight[k] * pObj->pageLayout->pagesWidth[k] * 4);
			tempPage.pageHeight = pObj->pageLayout->pagesHeight[k];
			tempPage.pageWidth = pObj->pageLayout->pagesWidth[k];		
			tempPage.pWorld = gCurrentWorld;
			tempPage.empty = true;
			
			LIGHT_Lightmaps_AddLightmapPage(&tempPage);			
		}

		// transfer data to lightmap page
		LIGHT_tdst_LightmapPageInfo* pPage;
		LMColor* readptr;
		ULONG* writePtr;
		ULONG  intcolor;
		int x, y;

		// copy the calculated lighting data into texture pages	
		for (k = 0; k < pObj->nbAxisElements; k++)
		{
			pElement = pObj->pSortedAxisElements[k];
		
			readptr = pElement->pMap;

			// do not copy data from axiselements that use another element map
			if (pElement->pUseMapFromElement)
			{
				LM_Free(pElement->pMap);
				continue;
			}

			pElement->lmPosX = pObj->pageLayout->elementStartPosX[k];
			pElement->lmPosY = pObj->pageLayout->elementStartPosY[k];
			pElement->lmIndex = pObj->pageLayout->elementPage[k] + objBasePageIndex;

			pPage = LIGHT_Lightmaps_GetPointerForIndex(gCurrentWorld ,pElement->lmIndex); 

			pPage->empty = false;
					
			writePtr = ((ULONG*)pPage->pData) +  (pElement->lmPosY * pPage->pageWidth) + pElement->lmPosX;

			for (y = 0; y < pElement->mapheight; y++)			
			{
				for (x = 0; x < pElement->mapwidth; x++)
				{
					// TODO: add processing to convert lightmaps to PS2 format around here

					// convert to 32bits RGBA
					intcolor = 0;
					intcolor |= (unsigned char)(readptr->r* 255.0f);
					intcolor |= ((unsigned char)(readptr->g* 255.0f)) << 8;
					intcolor |= ((unsigned char)(readptr->b* 255.0f)) << 16;
					intcolor |= 0xFF000000;

					*writePtr = intcolor;
					readptr++;
					writePtr++;
				}					
				
				writePtr += (pPage->pageWidth -  pElement->mapwidth);
			}

			// free the axis element map
			LM_Free(pElement->pMap);			
		}

		bestLayout = NULL;

		objBasePageIndex += pObj->pageLayout->nbPages;
	}

	gNbTempPage = objBasePageIndex;	
	
}
#endif

void DoElementPackingAndCompression(tdst_LMObj* _pObj, int _nbObj)
{
	// get pow2 size
	int i, k;
	UINT j;
	int lmsize = MATH_Max(gMaxHeight, gMaxWidth);
	int size = MATH_Max(gMinPageSize, (int)L_pow(2, ceilf(log10f(lmsize)/0.30103)));
	tdst_LMObj* pObj = _pObj;          
	tdst_LMAxisElement* pElement;
	tdst_LMAxisElement** pGlobalElementList;
	tdst_LMAxisElement** pListNext;
	int currentMapIndex = 0;
	int index;
	bool elementplaced = false;

	gNbTempPage = 0;

	// init packing info
	for (i = 0; i <  MAX_LIGHTMAP_PAGES; i++)
	{
		gTempLMPageInfo[i].w = size;
		gTempLMPageInfo[i].h = size;
		gTempLMPageInfo[i].borderx = 0;
		gTempLMPageInfo[i].bordery = 0;
		gTempLMPageInfo[i].start = (int*)LM_Alloc(size * sizeof(int));
		L_memset(gTempLMPageInfo[i].start, 0, size * sizeof(int));
	}

	// allocate global list of axis elements
	pGlobalElementList = (tdst_LMAxisElement**) LM_Alloc(gTotalNbAxisElement * sizeof(tdst_LMAxisElement*));
	pListNext = pGlobalElementList;

	// for each object
	for (i = 0; i < _nbObj; i++, pObj++)
	{
		if (!(pObj->settings.bReceiveShadows && pObj->settings.bUseLightmaps) || pObj->settings.bTemporaryStopUsingLightmaps)
			continue;

		// for each axis element
		for (j = 0; j < TAB_ul_Ptable_GetNbElems(&pObj->axisElementsList); j++)
		{
			pElement = (tdst_LMAxisElement*)pObj->axisElementsList.p_Table[j];

			// skip hole
			if (TAB_b_IsAHole(pElement))
				continue;

			// place element in list
			*pListNext = pElement;
			pListNext++;
		} 
	}

	// sort the list accoring to object element, material, width and height
	qsort(pGlobalElementList, gTotalNbAxisElement, sizeof(tdst_LMAxisElement*), axisElementCompare);

	// compare maps between themselves to remove duplicates
	DoMapCompression(pGlobalElementList, gTotalNbAxisElement);

	// loop on all element and place them
	for (i = 0; i < gTotalNbAxisElement; i++)
	{
		pElement = pGlobalElementList[i];

		if (pElement->pUseMapFromElement)
		{
			// element is mapped on another axis element, do not pack it
			continue;
		}

		index = 0;  

		// find a place in the maps
		while (!elementplaced)
		{
			if (currentMapIndex+1 > gNbTempPage)				
			{
				gNbTempPage++;
			}

			while (index < size)
			{
				while(gTempLMPageInfo[currentMapIndex].start[index] + pElement->mapheight > size && index < size) 
					index++;

				if (index + pElement->mapwidth > size)
				{
					index = size;
					break;
				}

				if (index < size)
				{
					int indexstart = gTempLMPageInfo[currentMapIndex].start[index];

					for (k = 0; k < pElement->mapwidth; k++)
					{
						if (gTempLMPageInfo[currentMapIndex].start[index + k] != indexstart)
						{
							break;
						}
					}

					if (k != pElement->mapwidth)
					{
						index += k;
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}			

			if (index < size)
			{
				pElement->lmIndex = currentMapIndex;
				pElement->lmPosX = index;
				pElement->lmPosY = gTempLMPageInfo[currentMapIndex].start[index];

				gTempLMPageInfo[currentMapIndex].borderx = MATH_Max(gTempLMPageInfo[currentMapIndex].borderx, pElement->lmPosX + pElement->mapwidth -1);
				gTempLMPageInfo[currentMapIndex].bordery = MATH_Max(gTempLMPageInfo[currentMapIndex].bordery, pElement->lmPosY + pElement->mapheight -1);

				for (k = 0; k < pElement->mapwidth; k++)
				{
					gTempLMPageInfo[currentMapIndex].start[index + k] += pElement->mapheight;
				}
				
				currentMapIndex = 0;                
				elementplaced = true;

#ifdef TRACE_ELEMENT_PACKING
				char sz_Message[1024];
				sprintf(sz_Message, "Element %i placed in LM Page, page: %i, X: %i, Y: %i, W: %i, H: %i", i, pElement->lmIndex, pElement->lmPosX, pElement->lmPosY, pElement->mapwidth, pElement->mapheight);
				LINK_PrintStatusMsg(sz_Message);					
#endif
			}
			else
			{
#ifdef TRACE_ELEMENT_PACKING
				char sz_Message[1024 ];
				sprintf(sz_Message, "Element %i placed in can't be placed in LM Page %i, W: %i, H: %i", i, currentMapIndex, pElement->mapwidth, pElement->mapheight);
				LINK_PrintStatusMsg(sz_Message);					
#endif
				currentMapIndex ++;
				index = 0;
				ERR_X_ErrorAssert(currentMapIndex < MAX_TEMP_LIGHTMAP, "Too many lightmap pages", "");
			}
		}

		elementplaced = false;

	}

	// reducing phase
	for (i = 0; i < gNbTempPage; i++)
	{
		gTempLMPageInfo[i].w = (int) L_pow(2, ceil(log10f(gTempLMPageInfo[i].borderx) / 0.30103));
		gTempLMPageInfo[i].h = (int) L_pow(2, ceil(log10f(gTempLMPageInfo[i].bordery) / 0.30103));
	}

	// copy the calculated lighting data into texture pages
	LIGHT_tdst_LightmapPageInfo* pPage;

	// allocate data for the maps
	for (i = 0 ; i < gNbTempPage; i++)
	{
		pPage = LIGHT_Lightmaps_GetPointerForKey(i, true, false, true);

		pPage->pData = MEM_p_Alloc(gPageLayout->pagesHeight[i] * gPageLayout->pagesWidth[i] * 4);
		memset(pPage->pData, 0xFF, gPageLayout->pagesHeight[i] * gPageLayout->pagesWidth[i] * 4);
		pPage->pageHeight = gPageLayout->pagesHeight[i];
		pPage->pageWidth = gPageLayout->pagesWidth[i];			
	}

	// loop on all axis elements
	
	LMColor* readptr;
	ULONG* writePtr;
	ULONG  intcolor;
	int x, y;

	for (i = 0; i < gTotalNbAxisElement; i++)
	{
		pElement = pGlobalElementList[i];
		readptr = pElement->pMap;

		// do not copy data from axiselements that use another element map
		if (pElement->pUseMapFromElement)
		{
			LM_Free(pElement->pMap);
			continue;
		}

		pPage = LIGHT_Lightmaps_GetPointerForKey(pElement->lmIndex, false, false, true);

		writePtr = ((ULONG*)pPage->pData) +  (pElement->lmPosY * pPage->pageWidth) + pElement->lmPosX;

		for (y = 0; y < pElement->mapheight; y++)			
		{
			for (x = 0; x < pElement->mapwidth; x++)
			{
				// TODO: add processing to convert lightmaps to PS2 format around here

				// convert to 32bits RGBA
				intcolor = 0;
				intcolor |= (unsigned char)(readptr->r* 255.0f);
				intcolor |= ((unsigned char)(readptr->g* 255.0f)) << 8;
				intcolor |= ((unsigned char)(readptr->b* 255.0f)) << 16;
				intcolor |= 0xFF000000;

				*writePtr = intcolor;
				readptr++;
				writePtr++;
			}					
			
			writePtr += (pPage->pageWidth -  pElement->mapwidth);
		}

		// free the axis element map
		LM_Free(pElement->pMap);
		
	}

	// free global element list
	LM_Free(pGlobalElementList);  

}


bool ElementShouldBeSkipped(GEO_tdst_ElementIndexedTriangles* _pElement, GEO_tdst_Object* _pObj, OBJ_tdst_GameObject* _pGameObj)
{
	if (_pElement->l_NbTriangles == 0)
		return true;

    if (!_pGameObj->pst_Base->pst_Visu->pst_Material)
        return true;

    return false;

    // DJ_TEMP : no material settings for now
    /*
    int submatindex;
    MAT_tdst_MultiTexture* pMat;
    MAT_tdst_Multi* pMatMulti;

	// check the flag in the material so see if it's excluded from lightmaps
	submatindex = _pElement->l_MaterialId;

	if (_pGameObj->pst_Base->pst_Visu->pst_Material->i->ul_Type == GRO_MaterialMulti)
	{
		pMatMulti = (MAT_tdst_Multi*) _pGameObj->pst_Base->pst_Visu->pst_Material;

		if (submatindex < pMatMulti->l_NumberOfSubMaterials)
			pMat = (MAT_tdst_MultiTexture*) pMatMulti->dpst_SubMaterial[submatindex];
		else
			pMat = NULL;
	}
	else
	{
		pMat = (MAT_tdst_MultiTexture*) _pGameObj->pst_Base->pst_Visu->pst_Material;
	}

	if (pMat)
	{
		// if it doesn't cast or receive shadow, it's no use to add it
		if (!(pMat->ul_Flags & MAT_Cc_MaterialReceiveLMShadows) && !(pMat->ul_Flags & MAT_Cc_MaterialCastLMShadows))
		{
			return true;
		}

		// these materials can't receive shadows
		if (pMat->ul_Flags & MAT_Cc_MaterialIsCrackOrDirt)
		{
			return true;
		}        
	}
	
	return false;
    */
}

void GetSettingsFromMaterial(LIGHT_tdst_LightmapSettings* _pSettings, GEO_tdst_ElementIndexedTriangles* _pElement, GEO_tdst_Object* _pObj, OBJ_tdst_GameObject* _pGameObj)
{
    return;

    /* DJ_TEMP : no settings per material at the moment

    int submatindex;
    MAT_tdst_MultiTexture* pMat;
    MAT_tdst_Multi* pMatMulti;

	// start with obj settings
	memcpy(_pSettings, &_pGameObj->pst_Base->pst_Visu->pst_LightmapSettings, sizeof(LIGHT_tdst_LightmapSettings));

	if (!_pGameObj->pst_Base->pst_Visu->pst_LightmapSettings.bCustomTexelRatio)
	{
		_pSettings->fTexelPerMeter = gTexelPerMeter;
	}

	// check the flag in the material so see if it's excluded from lightmaps
	submatindex = _pElement->l_MaterialId;

	if (_pGameObj->pst_Base->pst_Visu->pst_Material->i->ul_Type == GRO_MaterialMulti)
	{
		pMatMulti = (MAT_tdst_Multi*) _pGameObj->pst_Base->pst_Visu->pst_Material;

		if (submatindex < pMatMulti->l_NumberOfSubMaterials)
			pMat = (MAT_tdst_MultiTexture*) pMatMulti->dpst_SubMaterial[submatindex];
		else
			pMat = NULL;
	}
	else
	{
		pMat = (MAT_tdst_MultiTexture*) _pGameObj->pst_Base->pst_Visu->pst_Material;
	}

	if (pMat)
	{
		// this should not happen since elements who are not using lightmaps are rejected
		//if (!(pMat->ul_Flags & MAT_Cc_MaterialUseLightmaps))
		//{
		//	_pSettings->bUseLightmaps = false;
		//}

		if (!(pMat->ul_Flags & MAT_Cc_MaterialCastLMShadows))
		{
			_pSettings->bCastShadows = false;
		}
		
		// this should not happen in shadow map  
		if (!(pMat->ul_Flags & MAT_Cc_MaterialReceiveLMShadows))
		{
			_pSettings->bReceiveShadows = false;
			_pSettings->bUseLightmaps = false;
		}

		if (pMat->ul_Flags & MAT_Cc_MaterialCustomTexelRatio)
		{
			_pSettings->bCustomTexelRatio = true;
			_pSettings->fTexelPerMeter = pMat->f_CustomTexelRatio;
		}
	}	
    */
}

void GenerateUVCoords(tdst_LMObj* _pObj, int _nbObj)
{
	int i, k;
	UINT j;
	tdst_LMAxisElement* pElement;
	LM_tdst_Matrix vertexToUV;
	LM_tdst_Matrix temp, temp2, temp3;
	int lmWidth, lmHeight;
	float trueWidth, trueHeight;
	float scalex, scaley;
	float transx, transy;
	LM_tdst_Vector v;
	tdst_LMElement*	pLMObjElement;
	GEO_tdst_ElementIndexedTriangles* pIndexTriangleElement;
	LIGHT_tdst_LightmapPageInfo* pPage;
	tdst_LMAxisElement*	pMappingElement;
	int	nbElements;

	// for each object
	for (i = 0; i < _nbObj; i++)
	{      
		if (!(_pObj[i].settings.bReceiveShadows && _pObj[i].settings.bUseLightmaps) || _pObj[i].settings.bTemporaryStopUsingLightmaps)
			continue; 

		if (_pObj[i].nbElements == 0)
			continue;

		// scan the axis elements and set the UV for their faces
		for (j = 0; j < TAB_ul_Ptable_GetMaxNbElems(&_pObj[i].axisElementsList) ; j++)
		{			
			pElement = (tdst_LMAxisElement*) _pObj[i].axisElementsList.p_Table[j];

			if (TAB_b_IsAHole(pElement) || &_pObj[i].axisElementsList.p_Table[j] > TAB_ppv_Ptable_GetLastElem(&_pObj[i].axisElementsList))
				continue;


			pMappingElement = pElement;

			// find the final element to map from
			while (pMappingElement->pUseMapFromElement)
			{
				pMappingElement = pMappingElement->pUseMapFromElement;
			}

			pElement->lmPosX = pMappingElement->lmPosX;
			pElement->lmPosY = pMappingElement->lmPosY;
			pElement->lmIndex = pMappingElement->lmIndex;

			pPage = LIGHT_Lightmaps_GetPointerForKey(pMappingElement->lmIndex, false, false, true);

			// generate matrix to go from vertex pos to UV pos
			lmWidth =  pPage->pageWidth;
			lmHeight=  pPage->pageHeight;

			trueWidth = pMappingElement->mapwidth * (1.0f / pElement->texelPerMeter);
			trueHeight = pMappingElement->mapheight * (1.0f / pElement->texelPerMeter);

			//trueWidth = pMappingElement->mapwidth * ((_pObj[i].settings.bCustomTexelRatio)? 1.0f / _pObj[i].settings.fTexelPerMeter : 1.0f / gTexelPerMeter );
			//trueHeight = pMappingElement->mapheight * ((_pObj[i].settings.bCustomTexelRatio)? 1.0f / _pObj[i].settings.fTexelPerMeter : 1.0f / gTexelPerMeter );

			scalex = pMappingElement->mapwidth/(LMReal)lmWidth;
			scaley = pMappingElement->mapheight/(LMReal)lmHeight;
			transx = pMappingElement->lmPosX/(LMReal)lmWidth;
			transy = (lmHeight - pMappingElement->lmPosY - pMappingElement->mapheight) / (double)lmHeight;

			LM_SetIdentityMatrix(&temp);
			LM_SetIdentityMatrix(&temp2);
			LM_SetIdentityMatrix(&vertexToUV);

			LM_MulMatrixMatrix(&temp3, &_pObj[i].toWorld, &pElement->worldToCam);
			
			LM_InitVector(&v, 2.0f / trueWidth, 2.0f / trueHeight, 1.0f);								
			LM_SetScale(&temp, &v);

			LM_MulMatrixMatrix(&temp2, &temp3, &temp);
			
			LM_SetIdentityMatrix(&temp);
			LM_InitVector(&v, 0.5f, 0.5f, 1.0f);								
			LM_SetScale(&temp, &v);

			LM_MulMatrixMatrix(&temp3, &temp2, &temp);

			LM_SetIdentityMatrix(&temp);
			LM_InitVector(&v, 0.5f, 0.5f, 0.0f);								
			LM_SetTranslation(&temp, &v);

			LM_MulMatrixMatrix(&temp2, &temp3, &temp);
			
			LM_SetIdentityMatrix(&temp);
			LM_InitVector(&v, scalex, scaley, 1.0f);								
			LM_SetScale(&temp, &v);

			LM_MulMatrixMatrix(&temp3, &temp2, &temp);

			LM_SetIdentityMatrix(&temp);
			LM_InitVector(&v, transx, transy, 0.0f);								
			LM_SetTranslation(&temp, &v);

			LM_MulMatrixMatrix(&vertexToUV, &temp3, &temp);

  			for (k =0; k < _pObj[i].nbFaces; k++)
			{
				if (pElement->pAxisFaces[k].elementface)
				{
					_pObj[i].p_tdst_faces[k].lmPage = pMappingElement->lmIndex;

					// transform vertex position into UV pos....
					LM_TransformVertex(&v, &vertexToUV, &_pObj[i].p_tdst_faces[k].vertex[0]);
					_pObj[i].p_tdst_faces[k].u[0] = v.x;
					_pObj[i].p_tdst_faces[k].v[0] = v.y;

					LM_TransformVertex(&v, &vertexToUV, &_pObj[i].p_tdst_faces[k].vertex[1]);
					_pObj[i].p_tdst_faces[k].u[1] = v.x;
					_pObj[i].p_tdst_faces[k].v[1] = v.y;

					LM_TransformVertex(&v, &vertexToUV, &_pObj[i].p_tdst_faces[k].vertex[2]);
					_pObj[i].p_tdst_faces[k].u[2] = v.x;
					_pObj[i].p_tdst_faces[k].v[2] = v.y;	
				}
			}
		}
	}

	// update the geo objects

	// for each object
 	for (i = 0; i < _nbObj; i++)
	{
		if (!(_pObj[i].settings.bReceiveShadows && _pObj[i].settings.bUseLightmaps) || _pObj[i].settings.bTemporaryStopUsingLightmaps)
			continue;

		if (_pObj[i].nbElements == 0)
			continue;

		nbElements = _pObj[i].geoObj->l_NbElements;
		pIndexTriangleElement = _pObj[i].geoObj->dst_Element;

		pLMObjElement = _pObj[i].p_tdst_elements;

		int nbskipped = 0;

		// for each "REAL" element (not the axis elements but the geometric elements who share a material) 

		_pObj[i].gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords = (float**) MEM_p_Alloc(sizeof(float*) * nbElements);	
		_pObj[i].gameObj->pst_Base->pst_Visu->p_us_NbTrianglesInElement = (USHORT*) MEM_p_Alloc(sizeof(USHORT) * nbElements);	
		_pObj[i].gameObj->pst_Base->pst_Visu->usNbElements = nbElements;

		L_memset(_pObj[i].gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords, 0, sizeof(float*) * nbElements);

		for (int j =0 ; j < nbElements; j++, pIndexTriangleElement++)
		{		
			_pObj[i].gameObj->pst_Base->pst_Visu->p_us_NbTrianglesInElement[j] = pIndexTriangleElement->l_NbTriangles;

			if (ElementShouldBeSkipped(pIndexTriangleElement, _pObj[i].geoObj, _pObj[i].gameObj))
			{
				_pObj[i].gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords[j] = NULL;
				nbskipped++;
				continue;
			}

			if (pIndexTriangleElement->l_NbTriangles)
			{
				// allocate data for lightmap UVs....
				_pObj[i].gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords[j] = (float*) MEM_p_Alloc(pIndexTriangleElement->l_NbTriangles * sizeof(GEO_tdst_UV) * 3);	
								
				// if it's the first initializing the visu struct for this GAO
				if (_pObj[i].gameObj->pst_Base->pst_Visu->pLMPage == NULL)
					_pObj[i].gameObj->pst_Base->pst_Visu->pLMPage = LIGHT_Lightmaps_GetPointerForKey(pLMObjElement->p_tdst_faces[0].lmPage, false, true, true);
									
				// for all element faces
				for (k = 0; k < pIndexTriangleElement->l_NbTriangles; k++)			
   				{									
					_pObj[i].gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords[j][(k * 6)] =  pLMObjElement->p_tdst_faces[k].u[0];
					_pObj[i].gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords[j][(k * 6)+1] =  pLMObjElement->p_tdst_faces[k].v[0];
					_pObj[i].gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords[j][(k * 6)+2] =  pLMObjElement->p_tdst_faces[k].u[1];
					_pObj[i].gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords[j][(k * 6)+3] =  pLMObjElement->p_tdst_faces[k].v[1];
					_pObj[i].gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords[j][(k * 6)+4] =  pLMObjElement->p_tdst_faces[k].u[2];
					_pObj[i].gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords[j][(k * 6)+5] =  pLMObjElement->p_tdst_faces[k].v[2];
				}
			}

			pLMObjElement++;
		}
	}

}

// the bitmap doesn't fit a TGA so swap red and blue 
void InvertColorChannels(unsigned int* _pData, unsigned int _nbPixels)
{
	UINT i;             
	unsigned int lColor;

	// switch ARGB to ABGR 
	for (i = 0; i < _nbPixels; i++)
	{
		lColor = _pData[i];
		lColor = (lColor & 0xFF00FF00) | // keep green and alpha
				 ((lColor & 0x00FF0000) >> 16) |	
				 ((lColor & 0x000000FF) << 16);

		_pData[i] = lColor;
	}
}

void InvertLines(unsigned int* _pData, int _width, int _height)
{
    int i;
	void* pTemp= LM_Alloc(_width * 4);

	for (i =0; i < _height>>1; i++)
	{
		memcpy(pTemp, _pData + (_width * i), _width * 4);
		memcpy(_pData + (_width * i), _pData + (_width * (_height - (i+1))), _width * 4);
		memcpy(_pData + (_width * (_height - (i+1))), pTemp, _width * 4);
	}	
	
	LM_Free(pTemp);

}

void Convert32bitsTo4bits(char* pWriterBuffer, char* pSrcBuffer, int nbPixels)
{
	int i; 
	ULONG* pReadBuffer = (ULONG*) pSrcBuffer;
	UCHAR* pWriteBuffer = (UCHAR*) pWriterBuffer;
	ULONG color;
	UCHAR double4bits;

	for (i  = 0; i < nbPixels >> 1; i++)
	{
		color = *pReadBuffer;
		pReadBuffer++;
 
		double4bits = (UCHAR)(color & 0x000000F0);

		color = *pReadBuffer;
		pReadBuffer++;

		double4bits |= (color & 0x000000F0) >> 4;

		*pWriteBuffer = double4bits;
		pWriteBuffer++;
	}
}

#undef SAVE_TO_JTX

void SaveLightmapsToBigFile(BIG_KEY _worldKey)
{
	int i;
	unsigned int worldFileFatIndex = _worldKey;
	unsigned int worldDirectoryIndex;
	unsigned int lmDirectoryIndex;
	char lmTempDirName[2048];
	char temp[2048];
	unsigned int tmpIndex;
#ifndef SAVE_TO_JTX
	TEX_tdst_File_TgaHeader tgaHeader;
#endif // #ifndef SAVE_TO_JTX
	TEX_tdst_File_Params texParams;
	LIGHT_tdst_LightmapPageInfo* pPage;

	// find path of the world the lightmaps belongs to
	worldFileFatIndex = BIG_ul_SearchKeyToFat(_worldKey);

	if (worldFileFatIndex != BIG_C_InvalidIndex)
	{    
		// create directory for lightmaps or delete the existing one		
		worldDirectoryIndex = BIG_ParentFile(worldFileFatIndex);
		tmpIndex = worldDirectoryIndex;
		sprintf(lmTempDirName, "%s/LightMaps", BIG_NameDir(worldDirectoryIndex));
		tmpIndex = BIG_ParentDir(tmpIndex);

		while (tmpIndex != BIG_C_InvalidIndex)
		{			
			sprintf(temp, "%s/%s", BIG_NameDir(tmpIndex), lmTempDirName);
			strcpy(lmTempDirName, temp);
			tmpIndex = BIG_ParentDir(tmpIndex);
		}

		// this was causing too much trouble, this dir has to be cleaned up when we save
		/*if (BIG_ul_SearchDir(lmTempDirName) != BIG_C_InvalidIndex)
			BIG_DelDir(lmTempDirName);*/

		lmDirectoryIndex = BIG_ul_CreateDir(lmTempDirName);

#ifdef SAVE_TO_JTX

		JTX_FileHeaderV1 header;		
		char* pBuffer, *pWriterPtr;
		
		// prepare tex params
 		texParams.uc_Type = TEX_FP_JtxFile;
		texParams.ul_Color = 0;
		texParams.ul_Mark = 0xFFFFFFFF;
		texParams.uw_Flags = TEX_FP_QualityVeryHigh;		
		texParams.ul_Params[3] = 0xC0DEC0DE;
		texParams.ul_Params[2] = 0x00FF00FF;
		texParams.ul_Params[1] = 0xCAD01234;

		header.mNbMipmaps = 0;
		header.mVersion = 1;

		// save the textures
		for (i = 0; i < gNbTempPage; i++)
		{			
			pPage = LIGHT_Lightmaps_GetPointerForKey(i, false, false, true);
			//pPage = LIGHT_Lightmaps_GetPointerForIndex(gCurrentWorld, i);

			InvertLines((unsigned int*)pPage->pData, pPage->pageWidth, pPage->pageHeight);
			InvertColorChannels((unsigned int*)pPage->pData, pPage->pageWidth * pPage->pageHeight);

            // update header
			header.mHeight = pPage->pageHeight;
			header.mWidth = pPage->pageWidth;

			texParams.uw_Height = pPage->pageHeight;
			texParams.uw_Width = pPage->pageWidth;

			// allocate temp buffer for saving
			pBuffer = (char*) L_malloc((pPage->pageHeight * pPage->pageWidth * 4) + sizeof(header) + sizeof(texParams));
			pWriterPtr = pBuffer;
	
			char wowName[1024];
			strcpy(wowName, BIG_NameFile(worldFileFatIndex));

			if (strstr(wowName, ".wow"))
				*strstr(wowName, ".wow") = '\0';

			// create the file
			sprintf(temp, "LM_%s_%i_%ix%i_%p.jtx", wowName, i, pPage->pageWidth, pPage->pageHeight, timeGetTime());

			// save the lightmap texture
			SAV_Begin(lmTempDirName, temp);
			

			// save the PS2 version (which is used in all the other platforms)
			pWriterPtr = pBuffer;
			header.mType = JTX_Format_AlphaIntensity_4;

			// copy the header
			memcpy(pWriterPtr, &header, sizeof(header));
			pWriterPtr += sizeof(header);

			// copy the lightmap data
			Convert32bitsTo4bits(pWriterPtr, (char*)pPage->pData, pPage->pageWidth * pPage->pageHeight);
			pWriterPtr += ((pPage->pageWidth * pPage->pageHeight) >> 1);

			// write the tex params
			texParams.uc_Format = TEX_FP_4bpp;
			memcpy(pWriterPtr, &texParams, sizeof(texParams));
            			
			SAV_Buffer(pBuffer, ((pPage->pageHeight * pPage->pageWidth) >> 1) + sizeof(header) + sizeof(texParams));            
			SAV_ul_End();
			
			BIG_INDEX fileIndex;
			fileIndex = BIG_ul_SearchFileExt(lmTempDirName, temp);

			pPage->texBFKey = BIG_FileKey(fileIndex);			
			pPage->texIndex = TEX_w_List_AddTexture(&TEX_gst_GlobalList, (ULONG)pPage->texBFKey, false);

			// we could not increment the refcount on the texture for each GAO who's using it since it wasn't created
			// so we do a really thing here to make it equal to the number of references to the lightmap page
			// which should match since we just created it
			// let me restate this.... THIS IS UGLY... but it works
			TEX_tdst_Data* pTexData = TEX_List_GetTexture(pPage->texIndex);

			while (pTexData->ul_NbRefs != pPage->nbRef)
				TEX_List_AddRefTexture(pPage->texIndex);


			L_free(pBuffer);
		}

#else
		// init tga header
		tgaHeader.uc_Size = 0;		// no character identification field
		tgaHeader.uc_ColorMapType = 0; // no color map
		tgaHeader.uc_ImageTypeCode = 2; //  2 for unmapped RGB
		tgaHeader.uw_Origin = 0; // should be ignored since no palette
		tgaHeader.uw_PaletteLength = 0; // should be ignored since no palette
		tgaHeader.uc_BPCInPalette = 0; // should be ignored since no palette
		tgaHeader.uw_Left = 0;
		tgaHeader.uw_Top = 0;
		tgaHeader.uc_BPP = 32; 
		tgaHeader.ucDescriptorByte = 8;

		// prepare tex params
		texParams.uc_Format = TEX_FP_32bpp;
		texParams.uc_Type = TEX_FP_TgaFile;
		texParams.ul_Color = 0;
		texParams.ul_Mark = 0xFFFFFFFF;
		texParams.uw_Flags = TEX_FP_QualityVeryHigh;		
		texParams.ul_Params[3] = 0xC0DEC0DE;
		texParams.ul_Params[2] = 0x00FF00FF;
		texParams.ul_Params[1] = 0xCAD01234;

		// save the textures
		for (i = 0; i < gNbTempPage; i++)
		{			
			pPage = LIGHT_Lightmaps_GetPointerForKey(i, false, false, true);

            // update TGA header
			tgaHeader.uw_Height = pPage->pageHeight;
			tgaHeader.uw_Width = pPage->pageWidth;

			texParams.uw_Height = pPage->pageHeight;
			texParams.uw_Width = pPage->pageWidth;

			// create the file
			sprintf(temp, "Lightmap%i.tga", i, pPage->pageWidth, pPage->pageHeight);

			// write the header
			SAV_Begin(lmTempDirName, temp);
			SAV_Buffer(&tgaHeader, sizeof(tgaHeader));            

			// write the file
			InvertColorChannels((unsigned int*)pPage->pData, pPage->pageWidth * pPage->pageHeight);
			InvertLines((unsigned int*)pPage->pData, pPage->pageWidth, pPage->pageHeight);
			SAV_Buffer(pPage->pData, pPage->pageWidth * pPage->pageHeight * 4);

            // write the tex params
			SAV_Buffer(&texParams, sizeof(texParams));

			tmpIndex = SAV_ul_End();
			pPage->texBFKey = BIG_FileKey(tmpIndex);			
			pPage->texIndex = TEX_w_List_AddTexture(&TEX_gst_GlobalList, (ULONG)pPage->texBFKey, false);
			//TEX_List_AddRefTexture(pPage->texIndex);
		}
#endif
	}
}

void ComputeLightMaps(TAB_tdst_Ptable* _pst_LMUsers, TAB_tdst_Ptable* _pst_ShadowCasters, TAB_tdst_Ptable* _pst_LightList, int _nbElem, int _nbFaces, BIG_KEY _worldKey, SEL_tdst_Selection* _pSelection)
{
	OBJ_tdst_GameObject* gameObj;
	GEO_tdst_Object* geoObj;
	int j, k, v;
	UINT i;
	tdst_LMFace* p_tdst_SceneFaces;
	tdst_LMElement* p_tdst_SceneElements;
	tdst_LMElement*	pLMElement;
	tdst_LMObj* p_tdst_SceneObjects;
	int elemCount = 0, faceCount = 0;
	tdst_LMFace* currentFace;
	float		temp;
	int objNbElement;
	int objFaceIndex;
	int facecountbeforeelements;
	int	lmObjIndex;
	GEO_tdst_ElementIndexedTriangles*	pElement;
	ULONG objAmbient=0;

	gGlobalLMCallback(0, "Clearing current lightmaps", gGlobalThis);

	// Clear the current lightmaps
	//LIGHT_Lightmaps_DestroyLightmaps(gCurrentWorld);

	if (TAB_ul_Ptable_GetNbElems(_pst_LMUsers) == 0)
	{
		return;
	}

	// Generate our lists
	p_tdst_SceneFaces = (tdst_LMFace*)LM_Alloc(sizeof(tdst_LMFace) * _nbFaces);
	p_tdst_SceneElements = (tdst_LMElement*)LM_Alloc(sizeof(tdst_LMElement) * _nbElem);
	p_tdst_SceneObjects = (tdst_LMObj*)LM_Alloc(sizeof(tdst_LMObj) * TAB_ul_Ptable_GetNbElems(_pst_LMUsers));

	gObjList = p_tdst_SceneObjects;
	gNbObj = TAB_ul_Ptable_GetNbElems(_pst_LMUsers);
	gFaceList = p_tdst_SceneFaces;
	gNbFace = _nbFaces;
	gTotalNbAxisElement = 0;

	gGlobalLMCallback(0, "Precomputing bounding volumes", gGlobalThis);

	// precomputing bounding volumes and other stuff
    for (i =0; i < TAB_ul_Ptable_GetMaxNbElems(_pst_LMUsers); i++)
	{
		gameObj = (OBJ_tdst_GameObject*)_pst_LMUsers->p_Table[i];

		// skip holes
		if (TAB_b_IsAHole(gameObj))
			continue;


		// get ambient for this object
		// DJ_TEMP : no ambient... ambient is in the lighting equation ... objAmbient = LIGHT_GetAmbientForGAO(gameObj, &gameObj->pst_Base->pst_Visu->lInfo, gameObj->pst_Base->pst_Visu->pst_AmbientOfGAO, gameObj->pst_Base->pst_Visu->ul_DrawMask, NULL, NULL);

		p_tdst_SceneObjects[i].objAmbient.r = ((objAmbient & 0x000000FF) * (1.0f/255.0f)); 
		p_tdst_SceneObjects[i].objAmbient.g = (((objAmbient & 0x0000FF00) >> 8)  * (1.0f/255.0f));
		p_tdst_SceneObjects[i].objAmbient.b = (((objAmbient & 0x00FF0000) >> 16) * (1.0f/255.0f));
		p_tdst_SceneObjects[i].objAmbient.a = 1.0f;
		
		geoObj = (GEO_tdst_Object*)gameObj->pst_Base->pst_Visu->pst_Object;

		// BV is a bounding sphere set the info
		float radius;
		ComputeBSphere(geoObj, &p_tdst_SceneObjects[i].BSCenter, &radius);
		p_tdst_SceneObjects[i].BSRadiusSquare = radius * radius;
		p_tdst_SceneObjects[i].nbElements = geoObj->l_NbElements;
		p_tdst_SceneObjects[i].p_tdst_elements = &p_tdst_SceneElements[elemCount];
		
		L_memcpy(&p_tdst_SceneObjects[i].settings, &gameObj->pst_Base->pst_Visu->pst_LightmapSettings, sizeof(LIGHT_tdst_LightmapSettings));		

		// if there's a selection and the object is not in it we flag it not to use the lightmaps
		if (_pSelection && !SEL_RetrieveItem(_pSelection, gameObj))
		{
			p_tdst_SceneObjects[i].settings.bUseLightmaps = false;
		}

		LM_CopyToLMMatrix(&p_tdst_SceneObjects[i].toWorld, gameObj->pst_GlobalMatrix);
		memset(&p_tdst_SceneObjects[i].worldToMesh, 0  , sizeof(LM_tdst_Matrix));
		LM_InvertMatrix(&p_tdst_SceneObjects[i].worldToMesh, &p_tdst_SceneObjects[i].toWorld);
		p_tdst_SceneObjects[i].geoObj = geoObj;
		p_tdst_SceneObjects[i].gameObj = gameObj;
		objFaceIndex = 0;		
		elemCount += geoObj->l_NbElements;
		pElement = geoObj->dst_Element;
		objNbElement = geoObj->l_NbElements;

		pLMElement = p_tdst_SceneObjects[i].p_tdst_elements;
 
		facecountbeforeelements = faceCount;
 
		lmObjIndex = 0;

		for (j = 0; j < objNbElement; j++, pElement++)
		{
			if (ElementShouldBeSkipped(pElement, geoObj, gameObj))
			{				
				p_tdst_SceneObjects[i].nbElements--;
				elemCount--;			
				continue;
			}

			// loop on all the faces of the element and flag em 
			
            // no material settings for now.. use object settings only
            //GetSettingsFromMaterial (&pLMElement->settings, pElement, geoObj, gameObj);
            L_memcpy(&pLMElement->settings, &gameObj->pst_Base->pst_Visu->pst_LightmapSettings, sizeof(LIGHT_tdst_LightmapSettings));		

			pLMElement->nbFaces = pElement->l_NbTriangles;   
			pLMElement->p_tdst_faces = &p_tdst_SceneFaces[faceCount];
			pLMElement->pElement = pElement;

			faceCount += pElement->l_NbTriangles;
			
			for (k = 0; k < pElement->l_NbTriangles; k++)
			{
				currentFace = &pLMElement->p_tdst_faces[k];
				currentFace->nbAdjFaces = 0;
				currentFace->objfaceindex = objFaceIndex;
				currentFace->objElementIndex = j;
				currentFace->lmObjElementIndex = lmObjIndex;
				currentFace->castShadows = pLMElement->settings.bCastShadows;
				currentFace->receiveShadows = pLMElement->settings.bReceiveShadows;

				objFaceIndex++;

				// copy data to face
				currentFace->index[0] = pElement->dst_Triangle[k].auw_Index[0];
				currentFace->index[1] = pElement->dst_Triangle[k].auw_Index[1];
				currentFace->index[2] = pElement->dst_Triangle[k].auw_Index[2];

				currentFace->vertex[0].x = geoObj->dst_Point[pElement->dst_Triangle[k].auw_Index[0]].x;
				currentFace->vertex[0].y = geoObj->dst_Point[pElement->dst_Triangle[k].auw_Index[0]].y;
				currentFace->vertex[0].z = geoObj->dst_Point[pElement->dst_Triangle[k].auw_Index[0]].z;
				currentFace->vertex[1].x = geoObj->dst_Point[pElement->dst_Triangle[k].auw_Index[1]].x;
				currentFace->vertex[1].y = geoObj->dst_Point[pElement->dst_Triangle[k].auw_Index[1]].y;
				currentFace->vertex[1].z = geoObj->dst_Point[pElement->dst_Triangle[k].auw_Index[1]].z;
				currentFace->vertex[2].x = geoObj->dst_Point[pElement->dst_Triangle[k].auw_Index[2]].x;
				currentFace->vertex[2].y = geoObj->dst_Point[pElement->dst_Triangle[k].auw_Index[2]].y;
				currentFace->vertex[2].z = geoObj->dst_Point[pElement->dst_Triangle[k].auw_Index[2]].z;

				currentFace->normal[0].x = geoObj->dst_PointNormal[pElement->dst_Triangle[k].auw_Index[0]].x;
				currentFace->normal[0].y = geoObj->dst_PointNormal[pElement->dst_Triangle[k].auw_Index[0]].y;
				currentFace->normal[0].z = geoObj->dst_PointNormal[pElement->dst_Triangle[k].auw_Index[0]].z;
				currentFace->normal[1].x = geoObj->dst_PointNormal[pElement->dst_Triangle[k].auw_Index[1]].x;
				currentFace->normal[1].y = geoObj->dst_PointNormal[pElement->dst_Triangle[k].auw_Index[1]].y;
				currentFace->normal[1].z = geoObj->dst_PointNormal[pElement->dst_Triangle[k].auw_Index[1]].z;
				currentFace->normal[2].x = geoObj->dst_PointNormal[pElement->dst_Triangle[k].auw_Index[2]].x;
				currentFace->normal[2].y = geoObj->dst_PointNormal[pElement->dst_Triangle[k].auw_Index[2]].y;
				currentFace->normal[2].z = geoObj->dst_PointNormal[pElement->dst_Triangle[k].auw_Index[2]].z;

				// compute plane equation
				ComputePlanEqn(currentFace);

				// compute BS
				LM_AddVector(&currentFace->BSCenter, &currentFace->vertex[0], &currentFace->vertex[1]);
				LM_AddEqualVector(&currentFace->BSCenter, &currentFace->vertex[2]);
				LM_MulEqualVector(&currentFace->BSCenter, 1.0f/3.0f);
			
				currentFace->BSRadiusSquare = 0.0f;

				for(v = 0; v < 3; v++)
				{
					temp = LM_Distance(&currentFace->vertex[v], &currentFace->BSCenter);
					
					if (temp * temp > currentFace->BSRadiusSquare)
						currentFace->BSRadiusSquare = temp * temp;
				}
			}
			
			lmObjIndex++;
			pLMElement++;
		}

		if (p_tdst_SceneObjects[i].nbElements)
			p_tdst_SceneObjects[i].p_tdst_faces = p_tdst_SceneObjects[i].p_tdst_elements->p_tdst_faces;
		else
			p_tdst_SceneObjects[i].p_tdst_faces = NULL;

		p_tdst_SceneObjects[i].nbFaces = faceCount - facecountbeforeelements;

		if (p_tdst_SceneObjects[i].nbElements)
		{
			// generate adj faces    
			GenerateAdjFaces(&p_tdst_SceneObjects[i]);

			// generate axis elements
			GenerateAxisElements(&p_tdst_SceneObjects[i]);
		}
	}

	if (gTotalNbAxisElement)
	{
#ifdef OLD_ELEMENT_PACKING
		// Do the packing
		gGlobalLMCallback(0.1f, "Packing elements", gGlobalThis);
		DoElementPacking(p_tdst_SceneObjects, TAB_ul_Ptable_GetNbElems(_pst_LMUsers));
#endif

		// Do the lighting
		gGlobalLMCallback(0.2f, "Computing lighting", gGlobalThis);
		ComputeLighting(p_tdst_SceneObjects, TAB_ul_Ptable_GetNbElems(_pst_LMUsers), _pst_LightList);

		// Do the packing and equivalent axis page computation
		gGlobalLMCallback(0.9f, "Packing elements", gGlobalThis);
#if defined(PACK_PER_OBJECT)
		DoElementPackingAndCompressionPerObject(p_tdst_SceneObjects, TAB_ul_Ptable_GetNbElems(_pst_LMUsers));
#elif defined (PACK_ONE_PAGE_PER_WORLD)
		DoElementPackingAndCompressionPerWorld(p_tdst_SceneObjects, TAB_ul_Ptable_GetNbElems(_pst_LMUsers));
#else
		DoElementPackingAndCompression(p_tdst_SceneObjects, TAB_ul_Ptable_GetNbElems(_pst_LMUsers));
#endif
		
		// Generate the UV coords for the elements
		gGlobalLMCallback(0.95f, "Generating UV coords", gGlobalThis);
		GenerateUVCoords(p_tdst_SceneObjects, TAB_ul_Ptable_GetNbElems(_pst_LMUsers));

		// Save the lightmaps textures in the bigfile
		SaveLightmapsToBigFile(_worldKey);		 
	}

	// Free up the stuff
	for (i =0; i < TAB_ul_Ptable_GetMaxNbElems(_pst_LMUsers); i++)
	{
		if (!TAB_b_IsAHole(_pst_LMUsers->p_Table[i])) 
			if ((p_tdst_SceneObjects[i].settings.bReceiveShadows && p_tdst_SceneObjects[i].settings.bUseLightmaps)
				&& !p_tdst_SceneObjects[i].settings.bTemporaryStopUsingLightmaps)
				FreeAxisElements(&p_tdst_SceneObjects[i]);
	}
	
	LM_Free(p_tdst_SceneFaces);
	LM_Free(p_tdst_SceneElements);
	LM_Free(p_tdst_SceneObjects);

}

inline bool WorldIsAlreadyProcessed(BIG_KEY* _pWorlds, ULONG _ulNbWorlds, BIG_KEY _worldKey)
{
	ULONG i;

	for (i = 0; i < _ulNbWorlds; i++)
	{
		if (_pWorlds[i] == _worldKey)
			return true;
	}

	return false;
}

BIG_KEY GetBestWorldKeyForGAO(OBJ_tdst_GameObject* pGAO)
{
	OBJ_tdst_GameObject* pNotABoneGAO;

	// if it's a bone find it's father who's not a bone and get his key 
	if (pGAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)
	{
		// find it's father
		OBJ_tdst_GameObject* pFather = NULL;

		if (pGAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		{
			if (pGAO->pst_Base && pGAO->pst_Base->pst_Hierarchy)
			{
				pFather = pGAO->pst_Base->pst_Hierarchy->pst_Father;

				while (pFather)
				{
					if (pFather->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
					{						
						pFather = pFather->pst_Base->pst_Hierarchy->pst_Father;
					}
					else
					{
						break;
					}
				}
			}
		}

		pNotABoneGAO = pFather;
	}
	else
	{
		pNotABoneGAO = pGAO;
	}

	if (!pNotABoneGAO)
	{
		return BIG_C_InvalidKey;
	}

	// get the initial world key of the not a bone GAO
	// DJ_TEMP : return pNotABoneGAO->ul_InitialWorldKey;
    return pNotABoneGAO->pst_World->h_WorldKey;
}

void LIGHT_ComputeAllLightmaps(WOR_tdst_World*  _pst_World, BIG_KEY _worldKey, tdst_LightmapsComputationOptions* _pOptions, LMProgressCallback _fn_callback , void* _CallbackUserData, SEL_tdst_Selection * _pSelection)
{
	unsigned int i, j;
	OBJ_tdst_GameObject* gameObj; 
	LIGHT_tdst_Light* lightObj; 
	tdst_LMLight*	pLight;
	int	nbFaces = 0;
	int nbElem = 0;
	bool	bComputeForWorld;
   
    //LIGHT_g_NbOfLightMappedObjects = 0;

#define MAX_CONCURRENT_WORLDS	100

	BIG_KEY		allreadyProcessedWorld[MAX_CONCURRENT_WORLDS];
	ULONG		nbWorlds = 0;
	BIG_KEY		currentWorld = BIG_C_InvalidKey;

	gGlobalLMCallback = _fn_callback;
	gGlobalThis = _CallbackUserData;

	// set the options in globals (this sucks, I know)
	gComputeShadows = _pOptions->computeShadows;
	gTexelPerMeter = _pOptions->texelPerMeter;
	gSuperSamplingFactor = _pOptions->superSamplingFactor;
	gSuperSampling = _pOptions->doSuperSampling;
	gShadowMapMode = _pOptions->shadowMapMode;
	gShadowOpacity = 1.0f - _pOptions->shadowOpacity;
	gFixBackFaceBug = _pOptions->fixBackFaceBug;

	// scan all the GO in the map to find who need to have his lightmaps computed
	// and also get the list of static lights
	TAB_tdst_Ptable shadowCastersList;	// will contain OBJ_tdst_GameObject*
	TAB_tdst_Ptable lightmapUsersList; // will contain OBJ_tdst_GameObject*
	TAB_tdst_Ptable lightList; // will contain OBJ_tdst_GameObject*


	// THIS IS AN UGLY PATCH UNTIL I'M SURE IT'S THE WAY IT'S GOING TO WORK
	do
	{
		currentWorld = BIG_C_InvalidKey;

		TAB_Ptable_Init(&shadowCastersList, TAB_ul_Ptable_GetNbElems(&_pst_World->st_GraphicObjectsTable) , 2.0f);
		TAB_Ptable_Init(&lightmapUsersList, TAB_ul_Ptable_GetNbElems(&_pst_World->st_GraphicObjectsTable) , 2.0f);
		TAB_Ptable_Init(&lightList, TAB_ul_Ptable_GetNbElems(&_pst_World->st_GraphicObjectsTable) , 2.0f);
		nbFaces = 0;
		nbElem = 0;
		bComputeForWorld = false;

        // update lightmap settings for the objects
        for (i =0; i < TAB_ul_PFtable_GetMaxNbElems(&_pst_World->st_AllWorldObjects); i++)
        {
            // get the object
            gameObj = (OBJ_tdst_GameObject*) _pst_World->st_AllWorldObjects.p_Table[i].p_Pointer;

            // skip holes
            if (TAB_b_IsAHole(gameObj))
                continue;

            if (gameObj->ul_IdentityFlags &  OBJ_C_IdentityFlag_BaseObject && gameObj->ul_IdentityFlags &  OBJ_C_IdentityFlag_Visu)
            {

                // check if it's a geometric object
                if (gameObj->pst_Base && gameObj->pst_Base->pst_Visu && gameObj->pst_Base->pst_Visu->pst_Object && gameObj->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
                {
                    GEO_tdst_Object* pGeoObj = (GEO_tdst_Object*) gameObj->pst_Base->pst_Visu->pst_Object;
                    
                    // remove actors
                    BOOL bHasAnAnim = FALSE;
                    if(gameObj->pst_Base->pst_Hierarchy)
                    {
                        OBJ_tdst_GameObject	*pst_Father;
                        pst_Father = gameObj->pst_Base->pst_Hierarchy->pst_Father;
                        while(pst_Father && pst_Father->pst_Base && pst_Father->pst_Base->pst_Hierarchy && pst_Father->pst_Base->pst_Hierarchy->pst_Father) 
                        {
                            pst_Father = pst_Father->pst_Base->pst_Hierarchy->pst_Father;
                        }
                        if(pst_Father && (pst_Father->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims))
                        {
                            bHasAnAnim = TRUE;
                        }
                    }
                    
                    if ((gameObj->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) || bHasAnAnim)
                    {
                        gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bReceiveShadows = false;
                        gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bCastShadows = false;
                        gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bUseLightmaps = false;
                    }
                    else
                    {
#if 0 //TODO: aaahhhh don't have room for another flag, just uh, true for now!? ~hogsy
                        gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bReceiveShadows = ((gameObj->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_DontReceiveLM) == 0);
#else
						gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bReceiveShadows = true;
#endif
                        gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bCastShadows = false;//popowarning((gameObj->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_DontCastLM) == 0);
                        gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bUseLightmaps = true; // will become false for unselected objects when doing ligthmaps for selected objects only
                        gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bTemporaryStopUsingLightmaps = false; // will become true/false depending on which world is processed
                        gameObj->pst_Base->pst_Visu->pst_LightmapSettings.fTexelPerMeter = gTexelPerMeter;
                    }
                }
            }
        }

		for (i =0; i < TAB_ul_PFtable_GetMaxNbElems(&_pst_World->st_AllWorldObjects); i++)
		{
			// get the object
			gameObj = (OBJ_tdst_GameObject*) _pst_World->st_AllWorldObjects.p_Table[i].p_Pointer;

			// skip holes
			if (TAB_b_IsAHole(gameObj))
				continue;
			
			BIG_KEY ulGaoWorldKey = GetBestWorldKeyForGAO(gameObj);

			// check for base object
			if (gameObj->ul_IdentityFlags &  OBJ_C_IdentityFlag_BaseObject && gameObj->ul_IdentityFlags &  OBJ_C_IdentityFlag_Visu)
			{
				
				// check if it's a geometric object
				if (gameObj->pst_Base && gameObj->pst_Base->pst_Visu && gameObj->pst_Base->pst_Visu->pst_Object && gameObj->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
				{
					// add the object to the right lists
					if (gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bReceiveShadows && gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bUseLightmaps)
					{
						if (!WorldIsAlreadyProcessed(allreadyProcessedWorld, nbWorlds, ulGaoWorldKey) && currentWorld == BIG_C_InvalidKey)
						{
							currentWorld = ulGaoWorldKey;
							gCurrentWorld = currentWorld;
							allreadyProcessedWorld[nbWorlds++] = currentWorld;										
							bComputeForWorld = true;
						}
 
					}

					if (gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bReceiveShadows || gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bCastShadows)
					{
						
						// check if there's scale and print a warning if there's one
						if (MATH_b_TestScaleType(gameObj->pst_GlobalMatrix) ||
							MATH_b_TestOScaleType(gameObj->pst_GlobalMatrix) ||
							MATH_b_TestAnyScaleType(gameObj->pst_GlobalMatrix) )
						{
							ERR_X_Warning(0, ERR_szFormatMessage("Object %s has scale, shadows and lighting will not be correct", gameObj->sz_Name), NULL);
						}

						nbElem += ((GEO_tdst_Object*)gameObj->pst_Base->pst_Visu->pst_Object)->l_NbElements;

						for (j = 0; j < (UINT)((GEO_tdst_Object*)gameObj->pst_Base->pst_Visu->pst_Object)->l_NbElements; j++)
						{
							nbFaces += ((GEO_tdst_Object*)gameObj->pst_Base->pst_Visu->pst_Object)->dst_Element[j].l_NbTriangles;								
						}				

						if (currentWorld == ulGaoWorldKey)
						{
							gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bTemporaryStopUsingLightmaps = false;

							if (gameObj->pst_Base->pst_Visu->pLMPage)
							{
								// unref the lightmap page
								LIGHT_Lightmaps_RemoveRefLightmapPage(gameObj->pst_Base->pst_Visu->pLMPage);

								if (gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords)
								{
									// computing lightmaps for this object so erase em first
									for (j = 0; j < gameObj->pst_Base->pst_Visu->usNbElements; j++)
									{								
										if (gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords[j])
											MEM_Free(gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords[j]);
									}

									MEM_Free(gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords);
								}

								if (gameObj->pst_Base->pst_Visu->p_us_NbTrianglesInElement)
									MEM_Free(gameObj->pst_Base->pst_Visu->p_us_NbTrianglesInElement);
								
								gameObj->pst_Base->pst_Visu->p_us_NbTrianglesInElement = NULL;
								gameObj->pst_Base->pst_Visu->pp_st_LightmapCoords = NULL;
								gameObj->pst_Base->pst_Visu->pLMPage = NULL;

							}
						}
						else
						{
							gameObj->pst_Base->pst_Visu->pst_LightmapSettings.bTemporaryStopUsingLightmaps = true;
						}

						TAB_Ptable_AddElemAndResize(&lightmapUsersList, gameObj);
					}
				}
			}

			// check for lights
			if (gameObj->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)
			{
				if (gameObj->pst_Extended && gameObj->pst_Extended->pst_Light)
				{
					if (gameObj->pst_Extended->pst_Light->i->ul_Type == GRO_Light)
					{
						lightObj = (LIGHT_tdst_Light*)gameObj->pst_Extended->pst_Light;

						// exclude fog
						if ((lightObj->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Omni ||
							(lightObj->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Spot ||
							(lightObj->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct )
						{
							pLight = (tdst_LMLight*) LM_Alloc(sizeof(tdst_LMLight));
							pLight->pLight = gameObj; 
							pLight->pLastShadowCastingFace = NULL;
							pLight->pLastShadowCastingObject = NULL;

							// add to list						
							TAB_Ptable_AddElemAndResize(&lightList, pLight);
						}
					}
				}
			} 
		}

		// start computing the lightmaps 
		if (currentWorld != BIG_C_InvalidKey && bComputeForWorld) 
			ComputeLightMaps(&lightmapUsersList, &shadowCastersList, &lightList, nbElem, nbFaces, currentWorld, _pSelection);
		
		// unalloc all the lights
		for(i = 0; i < TAB_ul_Ptable_GetMaxNbElems(&lightList); i++)
		{
			if (TAB_b_IsAHole(lightList.p_Table[i]))
				continue;

			LM_Free(lightList.p_Table[i]);
		}

		// clean up
		TAB_Ptable_Close(&shadowCastersList);
		TAB_Ptable_Close(&lightmapUsersList);
		TAB_Ptable_Close(&lightList);		 
		
	} while (currentWorld != BIG_C_InvalidKey);

    //LIGHT_g_NbOfLightMappedObjects = 0;
    // flush/reload all textures
    WOR_tdst_World	*pst_SaveWorld = GDI_gpst_CurDD->pst_World;
    GDI_l_DetachWorld( GDI_gpst_CurDD );
    GDI_l_AttachWorld( GDI_gpst_CurDD, pst_SaveWorld );

#ifdef _XENON_RENDER
    GEO_ResetAllXenonMesh(FALSE, FALSE, FALSE);
#endif
}


#endif 