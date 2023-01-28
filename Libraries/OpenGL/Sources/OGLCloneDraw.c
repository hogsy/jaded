/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "GDInterface/GDIrasters.h"
#include "OGLinit.h"
#include "OGLtex.h"
#include "GEOmetric/GEO_STRIP.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_FCLONE.h"

extern MATH_tdst_Matrix LightMatrixClone[MAXLIGHTPERCLONE];
extern LIGHT_tdst_Light *LightClone[MAXLIGHTPERCLONE];
extern int NbCloneLight;

/*$4
***********************************************************************************************************************
Macros
***********************************************************************************************************************
*/

// HARDWARE LIGHT
//#define Hardware

extern BOOL			ENG_gb_ActiveSectorization;
/*$4
***********************************************************************************************************************
constant
***********************************************************************************************************************
*/

/*$4
***********************************************************************************************************************
Private and external function prototype
***********************************************************************************************************************
*/

void		OGL_SetupRC(OGL_tdst_SpecificData *);

extern void LOA_BeginSpeedMode(BIG_KEY _ul_Key);
extern void LOA_EndSpeedMode(void);
extern BOOL GDI_gb_WaveSprite;

// -- UV --
extern void MAT_UV_Compute_PLANAR_GIZMO(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV);
_inline_ BOOL MAT_VUISIdentity_i(MAT_tdst_MTLevel *p_tdstLevel);


extern BOOL		OGL_gb_DispStrip;
extern BOOL		OGL_gb_DispLOD;
extern ULONG	OGL_ulLODAmbient;

//extern int NbrObjectClone;
extern BOOL b_CloneLighted[MAXCLONE];
extern int NumberMat;
//BOOL b_OnlyOneMat;
#define MAXCLONES 1000
#define MAXLIGHT 2

/*#define OGL_SetColorRGBA(a) \
	if(pst_Color) \
	{ \
		ulOGLSetCol = pst_Color[a] | ulOGLSetCol_Or; \
		ulOGLSetCol ^= ulOGLSetCol_XOr; \
		if(pst_Alpha) \
		{ \
			ulOGLSetCol &= 0x00ffffff; \
			ulOGLSetCol |= pst_Alpha[a]; \
		} \
		glColor4ubv((GLubyte *) &ulOGLSetCol); \
	} \
	else if(pst_Alpha) \
	{ \
		ulOGLSetCol &= 0x00ffffff; \
		ulOGLSetCol |= pst_Alpha[a]; \
		glColor4ubv((GLubyte *) &ulOGLSetCol); \       
	}*/
/*#define OGL_SetColorRGBA(a) \
	if(pst_Color) \
	{ \
		ulOGLSetCol = pst_Color[a] | ulOGLSetCol_Or; \
		ulOGLSetCol ^= ulOGLSetCol_XOr; \
		ulOGLSetCol &= 0x00ffffff; \
		glColor4ubv((GLubyte *) &ulOGLSetCol); \
	} \
*/
#define OGL_SetColorRGBA(a) \
	if(pst_Color) \
	{ \
		ulOGLSetCol = pst_Color[a]; \
		glColor4ubv((GLubyte *) &ulOGLSetCol); \
	} \

/*#define OGL_SetColorRGBA(a) \
	if(pst_Color) \
	{ \
		ulOGLSetCol = pst_Color[a] | ulOGLSetCol_Or; \
		ulOGLSetCol ^= ulOGLSetCol_XOr; \
		ulOGLSetCol &= 0x00ffffff; \
		if ( ulOGLSetColRLI )\
		ulOGLSetCol |= ulOGLSetColRLI[a] & AlphaLight; \
		glColor4ubv((GLubyte *) &ulOGLSetCol); \
	} \
*/
#define dRAW_Normal(a) \
	if(_pst_Normal && Lighted) \
	glNormal3fv((float *) &_pst_Normal[a]); \


void ComputeUvClone(GEO_tdst_Object *_pst_Geo,GEO_tdst_ElementIndexedTriangles	*_pst_Element,MAT_tdst_MTLevel	*pst_MLTTXLVL,ULONG AddFlags)
{
	MAT_UV_Compute_PLANAR_GIZMO(GDI_gpst_CurDD,_pst_Geo,_pst_Element,AddFlags , 0);

	if(!MAT_VUISIdentity_i(pst_MLTTXLVL))
	{
		MAT_tdst_Decompressed_UVMatrix	TDST_ummAT;
		float							fSaveU;
		SOFT_tdst_UV		*SrcUV, *SrcUVLast, *DstUV;
		unsigned short		*UsdIndx, *LastUsdIndx;

		SrcUV = (SOFT_tdst_UV *) GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current;
		DstUV = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ast_UV;
		GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ast_UV;

		/* Compute new UV */
		if(GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
			SrcUVLast = SrcUV + _pst_Geo->l_NbPoints;
		else
			SrcUVLast = SrcUV + _pst_Geo->l_NbUVs;
		MAT_VUDecompress(pst_MLTTXLVL, &TDST_ummAT);

		if (MAT_VUISIdentity_i(pst_MLTTXLVL)  )
		{
			TDST_ummAT.UVMatrix[0]=1;
			TDST_ummAT.UVMatrix[1]=0;
			TDST_ummAT.UVMatrix[2]=0;
			TDST_ummAT.UVMatrix[3]=1;
		}

		if((GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint) && (_pst_Element->pus_ListOfUsedIndex))
		{
			UsdIndx = _pst_Element->pus_ListOfUsedIndex;
			LastUsdIndx = UsdIndx + _pst_Element->ul_NumberOfUsedIndex;
			while(UsdIndx < LastUsdIndx)
			{
				/* Turn & rotate UV */
				SrcUV = ((SOFT_tdst_UV *) GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current) + *UsdIndx;
				fSaveU = SrcUV->u;
				SrcUV->u = TDST_ummAT.UVMatrix[0] * fSaveU + TDST_ummAT.UVMatrix[2] * SrcUV->v + TDST_ummAT.AddU;
				SrcUV->v = TDST_ummAT.UVMatrix[1] * fSaveU + TDST_ummAT.UVMatrix[3] * SrcUV->v + TDST_ummAT.AddV;
				UsdIndx++;
			}
		}
		else
		{
			while(SrcUV < SrcUVLast)
			{
				/* Turn & rotate UV */
				fSaveU = SrcUV->u;
				DstUV->u = TDST_ummAT.UVMatrix[0] * fSaveU + TDST_ummAT.UVMatrix[2] * SrcUV->v + TDST_ummAT.AddU;
				DstUV->v = TDST_ummAT.UVMatrix[1] * fSaveU + TDST_ummAT.UVMatrix[3] * SrcUV->v + TDST_ummAT.AddV;
				SrcUV++;
				DstUV++;
			}
		}		

	} 
}

#ifdef ACTIVE_EDITORS
extern BOOL OGL_bCountTriangles;
extern u32 Stats_ulNumberOfTRiangles;
#endif //ACTIVE_EDITORS



LONG OGL_l_CloneDrawElementIndexedTriangles
(
 GEO_tdst_ElementIndexedTriangles	*_pst_Element,
 GEO_Vertex					        *_pst_Point,
 MATH_tdst_Vector					*_pst_Normal,
 GEO_tdst_UV							*_pst_UV,
 ULONG								ulnumberOfPoints
 )
{
#ifdef Active_CloneListe
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object				*pst_OriginalGeo;
	OGL_tdst_SpecificData		*pst_SD;
	GEO_tdst_IndexedTriangle	*t, *tend;
	GEO_tdst_IndexedTriangle	*st, *stend;
	ULONG						*pst_Color;
	ULONG						*pst_Alpha;
	ULONG						ulDrwMskDelta;
	ULONG						ulOGLSetCol_Or;
	ULONG						AlphaLight;
	ULONG						ulOGLSetCol_XOr;
	ULONG						ulOGLSetCol;
	ULONG						*ulOGLSetColRLI;
	BOOL						bStrip;
	USHORT						auw_Index, auw_UV;
	GEO_tdst_OneStrip			*pStrip, *pStripEnd;
	OBJ_tdst_GameObject			*_pst_GO;
	OBJ_tdst_GameObject			*GaoLight;
	OBJ_tdst_GameObject			*_pst_GOChaine;
	LightCloneListeGao		*CloneLightList;
	ULONG ul_Ambient;
	int i_CurrentClone;
	MAT_tdst_MultiTexture	*pst_MLTTX;
	MAT_tdst_MTLevel		*pst_MLTTXLVL;
    BOOL                    b_ClearCloneBuffer;

	ULONG AddFlags;
	//BOOL slip;
	BOOL Lighted;
	float rap = 1.f/256;
	LIGHT_tdst_Light * Light;

	//  -- Light --
	MATH_tdst_Vector	st_ScaleVect;//,st_dist;
	float				f_Factor, f_Far, f_Near;
	//float f_dist;
	MATH_tdst_Matrix m_Light;
	int NbLiGhT;

#ifndef Hardware
	float               f_Dist2, f_Far2, f_Near2, f_Div, f_Div2;
	ULONG cont;
	ULONG col;
	MATH_tdst_Vector vDir;
	float coloro;

#else
	GLfloat LightAmbient[4];
	GLfloat LightDiffuse[4];
	GLfloat LightPosition[4];
	f32 k0,k1,k2;
	f32 fInvDelta,fFar2,fNear2,fTempN,fTempF,fFDivN,fMinTempF;
	static f32 fL0 = 1.1f; // Light coef distance 0
	static f32 fLN = 0.9f; // Light coef distance near
	static f32 fLF = 0.2f; // Light coef distance far

#endif
	// -------------

#ifdef ACTIVE_EDITORS
	ULONG ulSaveAmbientColor;
#endif //ACTIVE_EDITORS

	MATH_tdst_Matrix m_LightM;
	MATH_tdst_Matrix *p_MvObj,st_Current;
static float pipi=0;
	//if ( NumberMat ) return 0;
	AddFlags = 0;

	pst_SD = (OGL_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

	pst_MLTTX = GDI_gpst_CurDD_SPR.pst_CurrentMat;
	pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;//GDI_gpst_CurDD_SPR.pst_CurrentMLTTXLVL;//pst_MLTTX->pst_FirstLevel;
    b_ClearCloneBuffer=FALSE;
	if (GDI_gpst_CurDD_SPR.pst_CurrentMLTTXLVL == pst_MLTTX->pst_FirstLevel) b_ClearCloneBuffer=TRUE;
	if ( pst_MLTTXLVL )
	{
		int NukMat = NumberMat;
		while(NukMat--)
		{
			pst_MLTTXLVL=pst_MLTTXLVL->pst_NextLevel;
		}
		if ( pst_MLTTXLVL ) 
//			return 0;
		if((MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags )== MAT_Cc_UV_Planar_GZMO))
			AddFlags = pst_MLTTXLVL->s_AditionalFlags;
	//MAT_CC_OBJECT
	}

	_pst_GO = GDI_gpst_CurDD_SPR.pst_CurrentGameObject;


	t = _pst_Element->dst_Triangle;
	tend = t + _pst_Element->l_NbTriangles;
	st=t;
	stend=tend;

	ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	ulOGLSetCol_XOr ^= 0xff000000;
	//ulOGLSetCol = (ULONG)GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
	ulDrwMskDelta = GDI_gpst_CurDD->LastDrawMask ^ GDI_gpst_CurDD->ul_CurrentDrawMask;

	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;


#ifdef ACTIVE_EDITORS
	if (OGL_gb_DispLOD && OGL_ulLODAmbient)
	{
		pst_Color = NULL;
		ulSaveAmbientColor = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient;
		GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient = OGL_ulLODAmbient; 
	}
#endif // ACTIVE_EDITORS

#ifdef Hardware
	glEnable  ( GL_COLOR_MATERIAL );
	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;

	if(GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_UseAmbient)
	{
		
		ul_Ambient = GDI_gpst_CurDD->pst_World->ul_AmbientColor;
		LightAmbient[0] = 3.0f*rap*(ul_Ambient & 0x000000ff);
		LightAmbient[1] = 3.0f*rap*((ul_Ambient & 0x0000ff00)>>8);
		LightAmbient[2] = 3.0f*rap*((ul_Ambient & 0x00ff0000)>>16);
		LightAmbient[3] = 0;
		glLightModelfv ( GL_LIGHT_MODEL_AMBIENT, LightAmbient );
		
	}

	glLightModelfv ( GL_LIGHT_MODEL_AMBIENT, LightAmbient );
	//L_memset(GDI_gpst_CurDD->pst_ComputingBuffers->aul_Diffuse , 0, sizeof(ULONG) * SOFT_Cul_ComputingBufferSize);
	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->aul_Diffuse;

#else
		if (GDI_gpst_CurDD_SPR.ul_CurrentDrawMask & GDI_Cul_DM_DontUseAmbient2)
			ul_Ambient = GDI_gpst_CurDD->pst_World->ul_AmbientColor;
		else
			ul_Ambient = GDI_gpst_CurDD->pst_World->ul_AmbientColor2;

	//ul_Ambient = GDI_gpst_CurDD->pst_World->ul_AmbientColor2;
	
//	L_memset(GDI_gpst_CurDD->pst_ComputingBuffers->aul_Diffuse , 0x03000000 , sizeof(ULONG) * SOFT_Cul_ComputingBufferSize);

	//old	
	//pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->aul_Diffuse;
	//new
	//pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->aul_ComputeClone[i_CurrentClone];
	//pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
#endif

	//pst_Alpha = NULL;
	ulOGLSetCol_Or = pst_SD->ulColorOr;

	OGL_RS_DrawWired(&pst_SD->st_RS, !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired));
	OGL_RS_CullFace(&pst_SD->st_RS, (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_TestBackFace));
	OGL_RS_CullFaceInverted(&pst_SD->st_RS, !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF));
	OGL_RS_DepthTest( &pst_SD->st_RS, GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest );
	OGL_RS_Fogged(&pst_SD->st_RS, ((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged) && (pst_SD->ulFogState)));

	GDI_gpst_CurDD->LastDrawMask = GDI_gpst_CurDD->ul_CurrentDrawMask;

	if(_pst_Element->pst_StripData != NULL)
	{
		if(_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			bStrip = TRUE;
		else
			bStrip = FALSE;
	}
	else
		bStrip = FALSE;

	_pst_GOChaine = _pst_GO;

 	MATH_SetIdentityMatrix(&st_Current);
	p_MvObj = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix-1;

	pst_OriginalGeo = (GEO_tdst_Object*)_pst_GO->pst_Base->pst_Visu->pst_Object;
	i_CurrentClone =0;
	while(_pst_GOChaine)
	{
			pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->aul_ComputeClone[i_CurrentClone];


		if ( !(_pst_GOChaine->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled) )
		{
			GEO_tdst_Object	*pst_Geo;

			pst_Geo = (GEO_tdst_Object*)_pst_GOChaine->pst_Base->pst_Visu->pst_Object;

			if (_pst_GOChaine->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
			{
				extern GRO_tdst_Struct *GEO_pst_StaticLOD_GetLOD(GEO_tdst_StaticLOD *_pst_LOD, UCHAR _uc_Distance);
				GEO_tdst_StaticLOD *pst_LOD;
				/* Special Case for LOD */  
				/* 1 Find actual LOD */
			    pst_LOD = (GEO_tdst_StaticLOD *) _pst_GOChaine->pst_Base->pst_Visu->pst_Object;
				pst_Geo = (GEO_tdst_Object	*)GEO_pst_StaticLOD_GetLOD(pst_LOD, _pst_GOChaine->uc_LOD_Vis);
				if ((u32 *)pst_OriginalGeo != (u32 *)pst_Geo) 
				{
					_pst_GOChaine=_pst_GOChaine->p_CloneNextGao;
			 		i_CurrentClone++;
					if (_pst_GOChaine == _pst_GO) _pst_GOChaine=NULL;
					continue;
				}
			}
			_pst_GOChaine->ulMoreCloneInfo |= 1; // <- Has Been Drawn

			t=st;
			tend=stend;

			MATH_MulMatrixMatrix(&st_Current,_pst_GOChaine->pst_GlobalMatrix,p_MvObj);
			GDI_SetViewMatrix((*GDI_gpst_CurDD),&st_Current);

#ifdef ACTIVE_EDITORS
            if (OGL_bCountTriangles) Stats_ulNumberOfTRiangles += _pst_Element->l_NbTriangles;
#endif
			if (_pst_GOChaine->pst_Base->pst_Visu->dul_VertexColors) ulOGLSetColRLI=_pst_GOChaine->pst_Base->pst_Visu->dul_VertexColors+1;
			else if (pst_Geo->dul_PointColors) ulOGLSetColRLI=pst_Geo->dul_PointColors+1;
			else ulOGLSetColRLI=NULL;
			//ulOGLSetColRLI = NULL;
			{
				//slip=2;
				if (AddFlags)
				{
					GDI_gpst_CurDD->pst_CurrentGameObject = _pst_GOChaine;
					ComputeUvClone(pst_Geo,_pst_Element,pst_MLTTXLVL,AddFlags);
					GDI_gpst_CurDD->pst_CurrentGameObject = _pst_GO;
				}

				if (!_pst_GOChaine->CloneLightList) 
				{   			
#ifdef Hardware 
					glDisable(GL_LIGHTING);
#endif
					AlphaLight = 0xffffffff;
					Lighted=FALSE;
				}
				else 
				{
					Lighted=TRUE;
					AlphaLight = 0xff000000;
 				}
#ifndef Hardware
			if ( b_ClearCloneBuffer )
			{
					u32 Counter;
					Counter = ulnumberOfPoints;
 					while(Counter --)
					{
						//old
						//GDI_gpst_CurDD->pst_ComputingBuffers->aul_Diffuse[Counter] = ul_Ambient;
						//new
						GDI_gpst_CurDD->pst_ComputingBuffers->aul_ComputeClone[i_CurrentClone][Counter] = ul_Ambient;
						if ( ulOGLSetColRLI )
						LIGHT_AddColor(&GDI_gpst_CurDD->pst_ComputingBuffers->aul_ComputeClone[i_CurrentClone][Counter], ulOGLSetColRLI[Counter] & AlphaLight);
					}
			}

#endif

       			if ( Lighted && _pst_Normal && !NumberMat )
				{
					//ulOGLSetColRLI = NULL;
					//Pas de RLI RGB , en LIGHT seulement alpha
					CloneLightList = _pst_GOChaine->CloneLightList;

					MATH_InvertMatrix(&m_LightM, _pst_GOChaine->pst_GlobalMatrix);
#ifdef Hardware
					glEnable(GL_LIGHTING);
					
#endif

					for (NbLiGhT = 0; NbLiGhT < MAXLIGHT ; NbLiGhT++)//2 au max 
					{
						if (!CloneLightList) break; // y'en a plus
						GaoLight = CloneLightList->p_Gao;
						CloneLightList = CloneLightList->p_Next;

						Light = (LIGHT_tdst_Light*)GaoLight->pst_Extended->pst_Light;
						if (!Light) 
                            break;
						LIGHT_gul_Color = Light->ul_Color;
						//if(!(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_UseColor)) LIGHT_gul_Color &= 0xFF000000;
						//if(!(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_UseAlpha)) LIGHT_gul_Color &= 0x00FFFFFF;

#ifdef Hardware
						//--- HARWARE INIT ---
						LightDiffuse[0] = rap*(Light->ul_Color & 0x000000ff);
						LightDiffuse[1] = rap*((Light->ul_Color & 0x0000ff00)>>8);
						LightDiffuse[2] = rap*((Light->ul_Color & 0x00ff0000)>>16);
						LightDiffuse[3] = 0;

						glEnable  ( GL_LIGHT0+NbLiGhT );
						LightDiffuse[0]*= 1.5f;//??
						LightDiffuse[1]*= 1.5f;//??
						LightDiffuse[2]*= 1.5f;//??
						glLightfv (GL_LIGHT0+NbLiGhT,GL_DIFFUSE, LightDiffuse );
						

						if ((Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Omni)
						{
						MATH_MulMatrixMatrix(&m_Light, GaoLight->pst_GlobalMatrix, &m_LightM);
						MATH_GetScaleVector(&st_ScaleVect, &m_LightM);

						f_Factor = (st_ScaleVect.x + st_ScaleVect.y + st_ScaleVect.z) * .333f;
						f_Far = Light->st_Omni.f_Far * f_Factor ;//*0.333f;
						f_Near = Light->st_Omni.f_Near * f_Factor ;//*0.333f;

						if (f_Far < f_Near)
						{
							f32 fTemp = f_Far;
							f_Far = f_Near;
							f_Near = fTemp;
						}

						k0 = 1.f/fL0;
						fFar2 = f_Far*f_Far;
						fNear2 = f_Near*f_Near;
						fInvDelta = 1.f / (f_Near*fFar2 - f_Far*fNear2);
						fTempN = (1.f/fLN) - k0;
						fTempF = (1.f/fLF) - k0;

						fFDivN = f_Far/f_Near;
						fMinTempF = fTempN * fFDivN;
						fTempF = MATH_f_FloatLimit(fTempF,fMinTempF,fMinTempF*fFDivN);

						k1 = fInvDelta * (fTempN * fFar2 - fTempF * fNear2);
						k2 = fInvDelta * (-fTempN * f_Far + fTempF * f_Near);

						glLightf(GL_LIGHT0+NbLiGhT,GL_CONSTANT_ATTENUATION,k0);
						glLightf(GL_LIGHT0+NbLiGhT,GL_LINEAR_ATTENUATION,k1);
						glLightf(GL_LIGHT0+NbLiGhT,GL_QUADRATIC_ATTENUATION,k2);

						LightPosition[0]=m_Light.T.x;
						LightPosition[1]=m_Light.T.y;
						LightPosition[2]=m_Light.T.z;
						LightPosition[3]=1.f;

						}
						else //Directionel
						{
							m_Light.T = GaoLight->pst_GlobalMatrix->T;

							LightPosition[0]=m_Light.T.x;
							LightPosition[1]=m_Light.T.y;
							LightPosition[2]=m_Light.T.z;
							LightPosition[3]=1.f;

						}

						glLightfv ( GL_LIGHT0+NbLiGhT,GL_POSITION, LightPosition  );

#else	//---------- SOFT LIGHT -----------		

						if ((Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Omni)
						{
							LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
							MATH_GetScaleVector(&st_ScaleVect, &m_LightM);
							f_Factor = (st_ScaleVect.x + st_ScaleVect.y + st_ScaleVect.z) * .333f;

							MATH_MulMatrixMatrix(&m_Light, GaoLight->pst_GlobalMatrix, &m_LightM);

							f_Far = Light->st_Omni.f_Far * f_Factor;
							f_Near = Light->st_Omni.f_Near * f_Factor;

							f_Div = (f_Far - f_Near);
							if(f_Div) f_Div = fOptInv(f_Div);

							f_Far2 = fNormalSqr(f_Far);
							f_Near2 = fNormalSqr(f_Near);
							f_Div2 = (f_Far2 - f_Near2);
							if(f_Div2) f_Div2 = fOptInv(f_Div2);

							cont = ulnumberOfPoints;

							while(cont--)
							{
								vDir.x = _pst_Point[cont].x - m_Light.T.x;
								f_Dist2 = vDir.x * vDir.x;
								if(f_Dist2 > f_Far2) continue; 
								vDir.y = _pst_Point[cont].y - m_Light.T.y;
								f_Dist2 += vDir.y * vDir.y;
								if(f_Dist2 > f_Far2) continue;
								vDir.z = _pst_Point[cont].z - m_Light.T.z;
								f_Dist2 += vDir.z * vDir.z;
								if(f_Dist2 > f_Far2) continue;

								coloro = -MATH_f_DotProduct( &_pst_Normal[cont], &vDir );
								if (coloro <= 0.0f) continue;
								if(f_Dist2 > f_Near2) coloro *= (f_Far2 - f_Dist2) * f_Div2;
								coloro *= fOptInvSqrt( f_Dist2 );

							    LIGHT_AddColor(&pst_Color[cont], LIGHT_ul_MulColor(Light->ul_Color, coloro) );

							}
						}//omni
						else if ((Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct)
						{
							MATH_tdst_Vector	*pst_Dir;
							LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
							//LIGHT_gpul_CurrentTable = LIGHT_gaul_ExponantTable;

							pst_Dir = MATH_pst_GetYAxis(&m_LightM);

							MATH_NegEqualVector(pst_Dir);
							cont = ulnumberOfPoints;
							while(cont--)
							{
								coloro = fMax(-MATH_f_DotProduct(&_pst_Normal[cont], pst_Dir), 0.0f);
							    LIGHT_AddColor(&pst_Color[cont], LIGHT_ul_MulColor(LIGHT_gul_Color, coloro) );
 
							}
							MATH_NegEqualVector(pst_Dir);
						}
#endif
					}//for 2

				}//lighted
				else
				{
					//if ( !ulOGLSetColRLI )
					//pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
					//else
					{
						//pst_Color = ulOGLSetColRLI;//GDI_gpst_CurDD->pst_ComputingBuffers->aul_Diffuse;
					}
				}
	
			}

			// !! MARCHE QUE STRIP POUR L'INSTANT !!
			if(bStrip == FALSE)
			{
				{
					PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);

					glBegin(GL_TRIANGLES);
					PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);

					if(pst_Color == NULL)
					{
						ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
						ulOGLSetCol ^= ulOGLSetCol_XOr;
						glColor4ubv((GLubyte *) &ulOGLSetCol);
					}

					if(!_pst_UV)
					{
						while(t < tend)
						{
							OGL_SetColorRGBA(t->auw_Index[0]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[0]]);
							OGL_SetColorRGBA(t->auw_Index[1]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[1]]);
							OGL_SetColorRGBA(t->auw_Index[2]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[2]]);
							t++;
						}
					}
					else if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
					{
						while(t < tend)
						{
							glTexCoord2fv(&_pst_UV[t->auw_Index[0]].fU);
							OGL_SetColorRGBA(t->auw_Index[0]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[0]]);
							glTexCoord2fv(&_pst_UV[t->auw_Index[1]].fU);
							OGL_SetColorRGBA(t->auw_Index[1]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[1]]);
							glTexCoord2fv(&_pst_UV[t->auw_Index[2]].fU);
							OGL_SetColorRGBA(t->auw_Index[2]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[2]]);
							t++;
						}
					}
					else if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_FaceMap)
					{
						while(t < tend)
						{
							glTexCoord2fv((float *) &_pst_UV[((t->ul_MaxFlags >> 7) & 3)]);
							OGL_SetColorRGBA(t->auw_Index[0]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[0]]);
							glTexCoord2fv((float *) &_pst_UV[((t->ul_MaxFlags >> 9) & 3)]);
							OGL_SetColorRGBA(t->auw_Index[1]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[1]]);
							glTexCoord2fv((float *) &_pst_UV[((t->ul_MaxFlags >> 11) & 3)]);
							OGL_SetColorRGBA(t->auw_Index[2]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[2]]);
							t++;
						}
					}
					else
					{
						while(t < tend)
						{
							glTexCoord2fv(&_pst_UV[t->auw_UV[0]].fU);
							OGL_SetColorRGBA(t->auw_Index[0]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[0]]);
							glTexCoord2fv(&_pst_UV[t->auw_UV[1]].fU);
							OGL_SetColorRGBA(t->auw_Index[1]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[1]]);
							glTexCoord2fv(&_pst_UV[t->auw_UV[2]].fU);
							OGL_SetColorRGBA(t->auw_Index[2]);
							glVertex3fv((float *) &_pst_Point[t->auw_Index[2]]);
							t++;
						}
					}

					PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
					glEnd();

					PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
				}
			}
			else
			{
				/*~~~~~~~~~~~~~~~~*/

				/* *** draw strip *** */
				ULONG	i, j, k;
				ULONG	ulEdgeColor;
				/*~~~~~~~~~~~~~~~~*/

				if  ((OGL_gb_DispStrip || (_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DisplayStrip)) && ((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired)))
				{
					pStrip = _pst_Element->pst_StripData->pStripList;
					pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;
					glDisable(GL_TEXTURE_2D);

					for(j = 0; pStrip < pStripEnd; pStrip++, j++)
					{
						k = j;
						ulEdgeColor = 0;
						while(k)
						{
							if(k & 1) ulEdgeColor += 0x7f;
							if(k & 2) ulEdgeColor += 0x7f00;
							if(k & 4) ulEdgeColor += 0x7f0000;
							k >>= 3;
						}

						PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);
						glBegin(GL_TRIANGLE_STRIP/*GL_TRIANGLE_FAN/*GL_TRIANGLE_STRIP*/);
						PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);

						if(pst_Color == NULL)
						{
							ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
							ulOGLSetCol ^= ulOGLSetCol_XOr;
							glColor4ubv((GLubyte *) &ulOGLSetCol);
						}

						if(!_pst_UV)
						{
							for(i = 0; i < pStrip->ulVertexNumber; i++)
							{
								auw_Index = pStrip->pMinVertexDataList[i].auw_Index;

								OGL_SetColorRGBA(auw_Index);
								//glColor4ubv((GLubyte *) &ulEdgeColor);
#ifdef Hardware
								glNormal3fv((float *) &_pst_Normal[auw_Index]);
#endif
								glVertex3fv((float *) &_pst_Point[auw_Index]);

							}
						}
						else
						{
							for(i = 0; i < pStrip->ulVertexNumber; i++)
							{
								auw_Index = pStrip->pMinVertexDataList[i].auw_Index;
								auw_UV = pStrip->pMinVertexDataList[i].auw_UV;

								glTexCoord2fv(&_pst_UV[auw_UV].fU);
								OGL_SetColorRGBA(auw_Index);
								glColor4ubv((GLubyte *) &ulEdgeColor);
#ifdef Hardware
								dRAW_Normal(auw_Index);
#endif
								glVertex3fv((float *) &_pst_Point[auw_Index]);
							}
						}

						PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
						glEnd();
						PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
					}
				}
				else
				{
					pStrip = _pst_Element->pst_StripData->pStripList;
					pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;

					for(; pStrip < pStripEnd; pStrip++)
					{
						PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);
						glBegin(GL_TRIANGLE_STRIP/*GL_TRIANGLE_FAN/*GL_TRIANGLE_STRIP*/);
						PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);

						if(pst_Color == NULL)
						{
							ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
							ulOGLSetCol ^= ulOGLSetCol_XOr;
							glColor4ubv((GLubyte *) &ulOGLSetCol);
						}

						if(!_pst_UV)
						{
							for(i = 0; i < pStrip->ulVertexNumber; i++)
							{
								auw_Index = pStrip->pMinVertexDataList[i].auw_Index;

								OGL_SetColorRGBA(auw_Index);
#ifdef Hardware
								dRAW_Normal(auw_Index);
#endif
								glVertex3fv((float *) &_pst_Point[auw_Index]);
							}
						}
						else if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
						{
							for(i = 0; i < pStrip->ulVertexNumber; i++)
							{
								auw_Index = pStrip->pMinVertexDataList[i].auw_Index;

								glTexCoord2fv(&_pst_UV[auw_Index].fU);
								OGL_SetColorRGBA(auw_Index);
#ifdef Hardware
								dRAW_Normal(auw_Index);
#endif
								glVertex3fv((float *) &_pst_Point[auw_Index]);
							}
						}
						else if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_FaceMap)
						{
							ERR_X_ForceError("pas de chance, on n'a pas facemap + strip en magasin", NULL);
						}
						else
						{
							for(i = 0; i < pStrip->ulVertexNumber; i++)
							{
								auw_Index = pStrip->pMinVertexDataList[i].auw_Index;
								auw_UV = pStrip->pMinVertexDataList[i].auw_UV;

								glTexCoord2fv(&_pst_UV[auw_UV].fU);
								OGL_SetColorRGBA(auw_Index);
#ifdef Hardware
								dRAW_Normal(auw_Index);
#endif
								glVertex3fv((float *) &_pst_Point[auw_Index]);
							}
						}

						PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
						glEnd();
						PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
					}
				}
			}
		}

		_pst_GOChaine=_pst_GOChaine->p_CloneNextGao;
		i_CurrentClone++;
		if (_pst_GOChaine == _pst_GO) _pst_GOChaine=NULL;
	}

#ifdef ACTIVE_EDITORS
	if (OGL_gb_DispLOD && OGL_ulLODAmbient)
		GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient = ulSaveAmbientColor;
#endif //ACTIVE_EDITORS

	/* fin */
	glDisable  ( GL_LIGHT0 );
	glDisable  ( GL_LIGHT1 );
	glDisable  ( GL_LIGHT2 );
	glDisable  ( GL_LIGHT3 );
	glDisable  ( GL_LIGHT4 );
	glDisable  ( GL_LIGHT5 );
	glDisable  ( GL_LIGHT6 );
	glDisable  ( GL_LIGHT7 );

	glDisable(GL_LIGHTING);
	glDisable( GL_COLOR_MATERIAL );
#endif
	return _pst_Element->l_NbTriangles;
}
