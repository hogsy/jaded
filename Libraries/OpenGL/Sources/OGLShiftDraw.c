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

#define Outlining

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

extern BOOL		OGL_gb_DispStrip;
extern BOOL		OGL_gb_DispLOD;
extern ULONG	OGL_ulLODAmbient;


/*$4
 ***********************************************************************************************************************
    Public Function
 ***********************************************************************************************************************
 */
extern int					NbrFurLayers ;
extern float				OffsetNormal;
extern float				OffsetU ;
extern float				OffsetV ;
extern int					FurInvertAlpha;
extern int					renderState_OutLine;
extern int					DontDrawFirstLayer;
extern ULONG				exLineColor;
extern int					i_FurNearLod;

#define OGL_SetColorRGBA(a) \
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
	}

#define OGL_SetVertexXYZ(P,N)\
	if ((P) && (N))\
		if (pst_Color){\
			ulOGLSetCol = (ulOGLSetCol)>>4*6;\
			offsetN2 = offsetN*ulOGLSetCol / 256.0f;\
			tempXYZ[0] = ((float*)(P))[0] + offsetN2*((float*)(N))[0];\
			tempXYZ[1] = ((float*)(P))[1] + offsetN2*((float*)(N))[1];\
			tempXYZ[2] = ((float*)(P))[2] + offsetN2*((float*)(N))[2];\
			glVertex3fv(tempXYZ);\
		}else{\
			tempXYZ[0] = ((float*)(P))[0] + offsetN*((float*)(N))[0];\
			tempXYZ[1] = ((float*)(P))[1] + offsetN*((float*)(N))[1];\
			tempXYZ[2] = ((float*)(P))[2] + offsetN*((float*)(N))[2];\
			glVertex3fv(tempXYZ);\
		}

#define OGL_SetVertexUV(uv)\
		{\
			tempXYZ[0] = ((float*) (uv))[0] + offSetU;\
			tempXYZ[1] = ((float*) (uv))[1] + offSetV;\
			glTexCoord2fv(tempXYZ);\
		}

#ifdef ACTIVE_EDITORS
#define OGL_TestHideTriangle(t) if(!((t)->ul_MaxFlags & 0x80000000))
#else
#define OGL_TestHideTriangle(t)
#endif 


LONG OGL_l_ShiftDrawElementIndexedTriangles
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	MATH_tdst_Vector					*_pst_Normal,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OGL_tdst_SpecificData		*pst_SD;
	GEO_tdst_IndexedTriangle	*t, *tend;
	ULONG						*pst_Color;
	ULONG						*pst_Alpha;
	ULONG						ulDrwMskDelta;
	ULONG						ulOGLSetCol_Or;
	ULONG						ulOGLSetCol_XOr;
	ULONG						ulOGLSetCol;
	BOOL						bStrip;
	USHORT						auw_Index, auw_UV;
	GEO_tdst_OneStrip			*pStrip, *pStripEnd;
	float						tempXYZ[3], offsetN,offsetN2, offSetU, offSetV;
	int							i;
	int							i_Alphatest, i_Alphafunc,i_Dephttest,i_Dephfunc,i_CullFaceTest,i_CullFace;
	float						f_Alpharef; 
	float f_dist;
    int i_NbLayers;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
    ULONG ulSaveAmbientColor;
	extern u32 Stats_ulNumberOfTRiangles;
    extern BOOL OGL_bCountTriangles;
    if (OGL_bCountTriangles)
    {
    	Stats_ulNumberOfTRiangles += _pst_Element->l_NbTriangles * ((NbrFurLayers>>1) + 1);
    }
#endif

	pst_SD = (OGL_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;
	t = _pst_Element->dst_Triangle;
	tend = t + _pst_Element->l_NbTriangles;
	
	ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	ulOGLSetCol_XOr ^= 0xff000000;

	ulDrwMskDelta = GDI_gpst_CurDD->LastDrawMask ^ GDI_gpst_CurDD->ul_CurrentDrawMask;
#ifdef ACTIVE_EDITORS
    if (OGL_gb_DispLOD && OGL_ulLODAmbient)
    {
        pst_Color = NULL;
        ulSaveAmbientColor = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient;
        GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient = OGL_ulLODAmbient; 
    }
    else
#endif // ACTIVE_EDITORS
	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;
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

	/* save alpha test context */
	if ( i_Alphatest = glIsEnabled( GL_ALPHA_TEST ) )
	{
		glGetIntegerv( GL_ALPHA_TEST_FUNC, &i_Alphafunc );
		glGetFloatv( GL_ALPHA_TEST_REF, &f_Alpharef );
	}
#ifdef Outlining
	if (renderState_OutLine == 2)
	{
		MATH_tdst_Matrix m_Temp;
		OBJ_tdst_GameObject *pst_TempGO ;
			
		pst_TempGO = GDI_gpst_CurDD->pst_CurrentGameObject;	
		m_Temp = *pst_TempGO->pst_GlobalMatrix;

		f_dist = MATH_f_Distance(&m_Temp.T,&GDI_gpst_CurDD->st_Camera.st_Matrix.T);
	}

	if ( renderState_OutLine ) 
	{	
		i_Dephttest = glIsEnabled( GL_DEPTH_TEST ) ;
		i_CullFaceTest = glIsEnabled( GL_CULL_FACE ) ;
		glGetIntegerv( GL_DEPTH_FUNC,&i_Dephfunc );
		glGetIntegerv( GL_CULL_FACE_MODE,&i_CullFace );
		NbrFurLayers = 1;
	}
#endif

#ifdef LOD_FUR_POPO // (non défini car il existe déjà un LOD fur dans le modifier, donc valide pour toutes consoles).
	{
		OBJ_tdst_GameObject *pst_TempGO ;
		//UCHAR i_FarLod=66;
		//UCHAR i_FurNearLod=76;
		UCHAR i_DistLod=i_FurNearLod;//-i_FarLod;
		UCHAR i_DistCurr;
		pst_TempGO = GDI_gpst_CurDD->pst_CurrentGameObject;	

		if ( pst_TempGO->uc_LOD_Vis<=i_FurNearLod && NbrFurLayers>3 && i_FurNearLod>0) 
		{
        	FLOAT f_Rap;
			i_DistCurr=pst_TempGO->uc_LOD_Vis;
			f_Rap=(FLOAT)(i_DistCurr*100/i_DistLod);//pourcentage
			i_NbLayers=(int)((NbrFurLayers)*f_Rap/100);
			
			if (i_NbLayers<3) i_NbLayers=3;

			f_Rap=((OffsetNormal*(NbrFurLayers-1))/(i_NbLayers-1))/OffsetNormal;

			OffsetNormal*=f_Rap;
			OffsetU*=f_Rap;
			OffsetV*=f_Rap;
		}
		else i_NbLayers=NbrFurLayers;
	}
#else //LOD_FUR_POPO
    i_NbLayers=NbrFurLayers;
#endif //LOD_FUR_POPO

		
	for (i = DontDrawFirstLayer; i < i_NbLayers ; i++ )
	{
#ifdef Outlining
		if ( renderState_OutLine )
		{
			offsetN = 0;
		}
		else
#endif
		{
			offsetN = OffsetNormal * i;
			offSetU = OffsetU *i;
			offSetV = OffsetV *i;
		}
		glEnable(GL_ALPHA_TEST);
		//glEnable( GL_SAMPLE_ALPHA_TO_COVERAGE );
#ifdef LOD_FUR_POPO
        glAlphaFunc(GL_GEQUAL, ((float)(i))/(float)i_NbLayers);
#else // LOD_FUR_POPO
        glAlphaFunc(GL_GEQUAL, ((float)(i))/(float)NbrFurLayers);
#endif // LOD_FUR_POPO

		t = _pst_Element->dst_Triangle;

		/* if there is no strip */
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
						OGL_TestHideTriangle(t)
						{
							OGL_SetColorRGBA(t->auw_Index[0]);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[0]], (float *) &_pst_Normal[t->auw_Index[0]]);
							OGL_SetColorRGBA(t->auw_Index[1]);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[1]], (float *) &_pst_Normal[t->auw_Index[1]]);
							OGL_SetColorRGBA(t->auw_Index[2]);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[2]], (float *) &_pst_Normal[t->auw_Index[2]]);
						}
						t++;
					}
				}
				else if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
				{
					while(t < tend)
					{
						OGL_TestHideTriangle(t)
						{
							OGL_SetColorRGBA(t->auw_Index[0]);
							OGL_SetVertexUV(&(_pst_UV[t->auw_Index[0]].fU));
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[0]], (float *) &_pst_Normal[t->auw_Index[0]]);
							OGL_SetColorRGBA(t->auw_Index[1]);
							OGL_SetVertexUV(&_pst_UV[t->auw_Index[1]].fU);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[1]], (float *) &_pst_Normal[t->auw_Index[1]]);
							OGL_SetColorRGBA(t->auw_Index[2]);
							OGL_SetVertexUV(&_pst_UV[t->auw_Index[2]].fU);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[2]], (float *) &_pst_Normal[t->auw_Index[2]]);
						}
						t++;
					}
				}
				else if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_FaceMap)
				{
					while(t < tend)
					{
						OGL_TestHideTriangle(t)
						{
							OGL_SetColorRGBA(t->auw_Index[0]);
							OGL_SetVertexUV((float *) &_pst_UV[((t->ul_MaxFlags >> 7) & 3)]);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[0]], (float *) &_pst_Normal[t->auw_Index[0]]);
							OGL_SetColorRGBA(t->auw_Index[1]);
							OGL_SetVertexUV((float *) &_pst_UV[((t->ul_MaxFlags >> 9) & 3)]);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[1]], (float *) &_pst_Normal[t->auw_Index[1]]);
							OGL_SetColorRGBA(t->auw_Index[2]);
							OGL_SetVertexUV((float *) &_pst_UV[((t->ul_MaxFlags >> 11) & 3)]);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[2]], (float *) &_pst_Normal[t->auw_Index[2]]);
						}
						t++;
					}
				}
				else
				{
					while(t < tend)
					{
						OGL_TestHideTriangle(t)
						{
							OGL_SetColorRGBA(t->auw_Index[0]);
							OGL_SetVertexUV(&_pst_UV[t->auw_UV[0]].fU);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[0]], (float *) &_pst_Normal[t->auw_Index[0]]);
							OGL_SetColorRGBA(t->auw_Index[1]);
							OGL_SetVertexUV(&_pst_UV[t->auw_UV[1]].fU);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[1]], (float *) &_pst_Normal[t->auw_Index[1]]);
							OGL_SetColorRGBA(t->auw_Index[2]);
							OGL_SetVertexUV(&_pst_UV[t->auw_UV[2]].fU);
							OGL_SetVertexXYZ((float *) &_pst_Point[t->auw_Index[2]], (float *) &_pst_Normal[t->auw_Index[2]]);
						}
						t++;
					}
				}

	    		PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
				glEnd();
				PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
			}
#ifdef Outlining		
		// ****************** OUT LINE CELL SHADING NO STRIP ***************************

			if ( renderState_OutLine )
			{
				MATH_tdst_Vector	Vertex;
				glDisable(GL_TEXTURE_2D);
				glEnable (GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glPolygonMode (GL_BACK, GL_FILL);
				glCullFace (GL_FRONT);

				glDepthFunc (GL_LEQUAL);
				glColor4ubv((GLubyte *) &exLineColor);
				//glColor3f (0.0f, 0.0f, 0.0f);

				if ( renderState_OutLine == 2)
				offsetN = OffsetNormal * f_dist/100;
				else offsetN = OffsetNormal;


				PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);
				glBegin(GL_TRIANGLES);
				PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);

				t = _pst_Element->dst_Triangle;

				while(t < tend)
				{
					OGL_TestHideTriangle(t)
					{
							//OGL_SetVertexXYZ((float *) &_pst_Point[auw_Index],(float *) &_pst_Normal[auw_Index]);
							
							Vertex.x =_pst_Point[t->auw_Index[0]].x + offsetN* _pst_Normal[t->auw_Index[0]].x;
							Vertex.y =_pst_Point[t->auw_Index[0]].y + offsetN* _pst_Normal[t->auw_Index[0]].y;
							Vertex.z =_pst_Point[t->auw_Index[0]].z + offsetN* _pst_Normal[t->auw_Index[0]].z;
							glVertex3fv((float *) &Vertex);
							
							Vertex.x =_pst_Point[t->auw_Index[1]].x + offsetN* _pst_Normal[t->auw_Index[1]].x;
							Vertex.y =_pst_Point[t->auw_Index[1]].y + offsetN* _pst_Normal[t->auw_Index[1]].y;
							Vertex.z =_pst_Point[t->auw_Index[1]].z + offsetN* _pst_Normal[t->auw_Index[1]].z;
							glVertex3fv((float *) &Vertex);

							Vertex.x =_pst_Point[t->auw_Index[2]].x + offsetN* _pst_Normal[t->auw_Index[2]].x;
							Vertex.y =_pst_Point[t->auw_Index[2]].y + offsetN* _pst_Normal[t->auw_Index[2]].y;
							Vertex.z =_pst_Point[t->auw_Index[2]].z + offsetN* _pst_Normal[t->auw_Index[2]].z;
							glVertex3fv((float *) &Vertex);

					}
				t++;
				}

	    		PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
				glEnd();
				PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
			}

		// *****************************************************************************		
#endif
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
							glColor4ubv((GLubyte *) &ulEdgeColor);
							OGL_SetVertexXYZ((float *) &_pst_Point[auw_Index],(float *) &_pst_Normal[auw_Index]);
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
							OGL_SetVertexXYZ((float *) &_pst_Point[auw_Index],(float *) &_pst_Normal[auw_Index]);
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
							OGL_SetVertexXYZ((float *) &_pst_Point[auw_Index],(float *) &_pst_Normal[auw_Index]);
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
							OGL_SetVertexXYZ((float *) &_pst_Point[auw_Index],(float *) &_pst_Normal[auw_Index]);
						}
					}

					PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
					glEnd();
					PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
				}
#ifdef Outlining
			// ****************** OUT LINE CELL SHADING STRIP ***************************
			if ( renderState_OutLine )
			{
				MATH_tdst_Vector	Vertex;
				if ( renderState_OutLine == 2)
				offsetN = OffsetNormal * f_dist/100;
				else offsetN = OffsetNormal;

				glDisable(GL_TEXTURE_2D);
				glEnable(GL_CULL_FACE);
				glEnable( GL_DEPTH_TEST );
				glCullFace (GL_FRONT);
				glDepthFunc (GL_LEQUAL);
			
				//glColor3f (0.0f, 0.0f, 0.0f);
				glColor4ubv((GLubyte *) &exLineColor);
				glPolygonMode (GL_BACK, GL_FILL);

				pStrip = _pst_Element->pst_StripData->pStripList;
				pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;

				for(; pStrip < pStripEnd; pStrip++)
				{
					PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);

					glBegin(GL_TRIANGLE_STRIP);
					PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);

						for(i = 0; i < pStrip->ulVertexNumber; i++)
						{
							auw_Index = pStrip->pMinVertexDataList[i].auw_Index;
							//OGL_SetVertexXYZ((float *) &_pst_Point[auw_Index],(float *) &_pst_Normal[auw_Index]);
							
							Vertex.x =_pst_Point[auw_Index].x + offsetN* _pst_Normal[auw_Index].x;
							Vertex.y =_pst_Point[auw_Index].y + offsetN* _pst_Normal[auw_Index].y;
							Vertex.z =_pst_Point[auw_Index].z + offsetN* _pst_Normal[auw_Index].z;
							glVertex3fv((float *) &Vertex);
						}

					PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
					glEnd();
					PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);
				}
			}
			// ******************************************************************************
#endif
			}
		}
	}
#ifdef Outlining
	if ( renderState_OutLine)
	{
		glPolygonMode (GL_FRONT, GL_FILL);	
	
		/* restore test context */
		if ( i_CullFaceTest ) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		if ( i_Dephttest ) glEnable( GL_DEPTH_TEST );
		else glDisable(GL_DEPTH_TEST);
		glDepthFunc(i_Dephfunc);
		glCullFace ( i_CullFace );
	}
#endif
	/* restore test context */
	if ( i_Alphatest )
	{
		glEnable( GL_ALPHA_TEST );
		//glEnable( GL_SAMPLE_ALPHA_TO_COVERAGE );
		glAlphaFunc( i_Alphafunc, f_Alpharef );
	}
	else
	{
		glDisable(GL_ALPHA_TEST);	
		//glDisable( GL_SAMPLE_ALPHA_TO_COVERAGE );
		glAlphaFunc(GL_ALWAYS, 0 );
	}

#ifdef ACTIVE_EDITORS
    if (OGL_gb_DispLOD && OGL_ulLODAmbient)
        GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient = ulSaveAmbientColor;
#endif //ACTIVE_EDITORS

	/* fin */
	return _pst_Element->l_NbTriangles*6;
}
