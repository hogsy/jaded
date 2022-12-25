/*$T GEOzone.c GC! 1.081 08/08/02 14:50:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$F
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Colors:
		0x000000FF: Red
		0xFF000000: Black
		0x00FF0000: Blue
		0x0000FF00: Green
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#include "BASe/BAStypes.h"
#include "BASe/BASsys.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"

#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SOUNDFX.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SOUNDLOADING.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SNDVOL.h"

#ifdef JADEFUSION
#include "ENGine/Sources/MoDiFier/MDFmodifier_SoftBody.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_Wind.h"
#endif

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDmodifier.h"

#include "GEOmetric/GEODebugObject.h"

#include "SELection/SELection.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "SOFT/SOFTPickingBuffer.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOzone.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GRObject/GROrender.h"
#include "GRObject/GROedit.h"

#include "BIGfiles/LOAding/LOAdefs.h"

#define GEO_Cul_Zone_DrawMask	(GDI_Cul_DM_All - GDI_Cul_DM_NotWired - GDI_Cul_DM_DontForceColor - GDI_Cul_DM_Lighted)
#define GEO_Cul_Cob_DrawMask	(GDI_Cul_DM_All - GDI_Cul_DM_Fogged)
#define GEO_Cul_MdF_DrawMask (GDI_Cul_DM_All - GDI_Cul_DM_NotWired - GDI_Cul_DM_DontForceColor - GDI_Cul_DM_Lighted - GDI_Cul_DM_Fogged)

#define GEO_Cul_White			0xFFFFFFFF
#define GEO_Cul_WhiteWhite		0xFF666666
#define GEO_Cul_Black			0x00000000
#define GEO_Cul_WhiteBlack		0x00666666
#define GEO_Cul_Red				0x000000FF
#define GEO_Cul_WhiteRed		0x006666FF
#define GEO_Cul_Blue			0x00FF0000
#define GEO_Cul_WhiteBlue		0x00FF6666
#define GEO_Cul_Yellow			0x0000FFFF
#define GEO_Cul_WhiteYellow		0x0099FFFF
#define GEO_Cul_Green			0xFF00FF00
#define GEO_Cul_WhiteGreen		0xFFAAFFAA

#define GEO_Cul_PinkS			0x00FF00FF
#define GEO_Cul_PurpleS			0x00FA2076
#define GEO_Cul_OrangeS			0x00FA9F9F	/* 0x000066FF */
#define GEO_Cul_SpecialBlue1S	0x00FFFF00
#define GEO_Cul_SpecialBlue2S	0x00C0FF00
#define GEO_Cul_FxRedS			0x000000FF
#define GEO_Cul_FxRed2S			0x00F000FF
#define GEO_Cul_FxYellowS		0x0000FFFF
#define GEO_Cul_FxYellow2S		0x00F0FFFF
#define GEO_Cul_SpecialFxRedS	0x000066FF

#define GEO_Cul_Pink			0x00AF00AF
#define GEO_Cul_Purple			0x00AA0026
#define GEO_Cul_Orange			0x00AA4F4F	/* 0x000016AF */
#define GEO_Cul_Orange2			0x000040EF
#define GEO_Cul_Orange3			0x000096FF
#define GEO_Cul_SpecialBlue1	0x00AFAF00
#define GEO_Cul_SpecialBlue2	0x0070AF00
#define GEO_Cul_FxRed			0x000000AF
#define GEO_Cul_FxRed2			0x00A000AF
#define GEO_Cul_FxYellow		0x0000AFAF
#define GEO_Cul_FxYellow2		0x00A0AFAF
#define GEO_Cul_SpecialFxRed	0x000016AF


#define GEO_Cul_Cob 0
#define GEO_Cul_ZDx 1
#define GEO_Cul_ODE 2

extern LIGHT_tdst_List	GEO_gst_DebugObject_LightList;
static void GEO_DrawSoundCapsule(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Matrix*pMatrix, ULONG ulAxis, float fScale, float fRadius, ULONG ulColor);
void					GEO_Zone_DisplaySoundInstance(GDI_tdst_DisplayData *, SND_tdst_SoundInstance *);
void					GEO_Zone_DisplaySoundMicro(GDI_tdst_DisplayData *);
void					GEO_Zone_DisplaySoundModifier(GDI_tdst_DisplayData *, MDF_tdst_Modifier *);
void					GEO_Zone_DisplaySoundFxModifier(GDI_tdst_DisplayData *, MDF_tdst_Modifier *);
void					GEO_Zone_DisplaySoundVolModifier(GDI_tdst_DisplayData *, MDF_tdst_Modifier *);
void					GEO_Zone_DisplayLoadingSoundModifier(GDI_tdst_DisplayData *, MDF_tdst_Modifier *);
#ifdef JADEFUSION
void                    GEO_MDFWind_Display(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *pst_MdF);
void                    GEO_ModifierSoftBody_Display(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *pst_MdF );
#endif
void GEO_DrawSoundBox
(
	GDI_tdst_DisplayData	*_pst_DD,
    MATH_tdst_Matrix        *pCenter,
    float deltafar,
	float					f_NearX,
	float					f_MiddleX,
	float					f_FarX,
	float					f_NearY,
	float					f_MiddleY,
	float					f_FarY,
	float					f_NearZ,
	float					f_MiddleZ,
	float					f_FarZ
);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_Init(GEO_tdst_GraphicZones *_pst_GraphicZones)
{
	L_memset(_pst_GraphicZones, 0, sizeof(GEO_tdst_GraphicZones));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_Close(GEO_tdst_GraphicZones *_pst_GraphicZones)
{
	if(_pst_GraphicZones->dpst_GraphicZone) MEM_Free(_pst_GraphicZones->dpst_GraphicZone);
	L_memset(_pst_GraphicZones, 0, sizeof(GEO_tdst_GraphicZones));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_Clear(GEO_tdst_GraphicZones *_pst_GraphicZones)
{
	_pst_GraphicZones->ul_Next = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_GraphicZone *GEO_Zone_GetNextZone(GEO_tdst_GraphicZones *_pst_GraphicZones)
{
	if(_pst_GraphicZones->ul_Next == _pst_GraphicZones->ul_Max)
	{
		if(!_pst_GraphicZones->ul_Max)
		{
			_pst_GraphicZones->ul_Max = 10;
			_pst_GraphicZones->dpst_GraphicZone = (GEO_tdst_GraphicZone *) MEM_p_Alloc(_pst_GraphicZones->ul_Max * sizeof(GEO_tdst_GraphicZone));
		}
		else
		{
			_pst_GraphicZones->ul_Max += 10;
			_pst_GraphicZones->dpst_GraphicZone = (GEO_tdst_GraphicZone *) MEM_p_Realloc
				(
					_pst_GraphicZones->dpst_GraphicZone,
					_pst_GraphicZones->ul_Max * sizeof(GEO_tdst_GraphicZone)
				);
		}
	}

	return(_pst_GraphicZones->dpst_GraphicZone + _pst_GraphicZones->ul_Next++);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Displays a Cubic Zone.

    Note:   The Zone have not the scale in their structure. We have to add it before sending them to viewport except if
            they hae the NoScale Flag.
 =======================================================================================================================
 */
void GEO_Zone_DisplayBox(GDI_tdst_DisplayData *_pst_DD, void *_pst_Data, OBJ_tdst_GameObject *_pst_GO, UCHAR _uc_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Box			*pst_Box;
	GEO_tdst_GraphicZone	*pst_GraphicZone;
	MATH_tdst_Vector		st_Min, st_Max, st_Scale;
	MATH_tdst_Vector		*pst_Min, *pst_Max;
	ULONG					ul_SaveColor;
	MATH_tdst_Matrix		st_NoScaleMatrix;
	BOOL					b_Selected, b_Specific, b_Active;
	BOOL					b_ObjectMode, b_NoScaleMode;
	COL_tdst_GameMat		*pst_GMat;
#ifdef ODE_INSIDE
	COL_tdst_Box			st_ODE_Box;
	MATH_tdst_Matrix		st_ODEMatrix;
	BOOL					b_Plane;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ODE_INSIDE
	if(_uc_Type > 2)
	{
		b_Plane = (_uc_Type == 6);
		_uc_Type = GEO_Cul_ODE;
	}
	else
		b_Plane = FALSE;
#endif
	ul_SaveColor = _pst_DD->ul_ColorConstant;
	switch(_uc_Type)
	{
	case GEO_Cul_ZDx:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_ZDx		*pst_ZDx;
			COL_tdst_Instance	*pst_Instance;
			UCHAR				uc_ENG_Index;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
			pst_ZDx = (COL_tdst_ZDx *) _pst_Data;
			pst_Box = (COL_tdst_Box *) pst_ZDx->p_Shape;
			b_ObjectMode = TRUE;
			b_NoScaleMode = COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_NoScale);
			b_Specific = COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_Specific);
			uc_ENG_Index = COL_uc_Instance_GetEngineIndexWithZone(pst_Instance, pst_ZDx);
			b_Active = (_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active) && COL_b_Instance_IsActive(pst_Instance, uc_ENG_Index);
		}
		break;
#ifdef ODE_INSIDE
	case GEO_Cul_ODE:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			DYN_tdst_ODE			*pst_ODE;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_ODE = ((DYN_tdst_ODE *) _pst_Data);

			if(b_Plane)
			{
				MATH_tdst_Vector	*pst_Min, *pst_Max;
				MATH_tdst_Vector	st_Diag;

				pst_Min = OBJ_pst_BV_GetGMin(_pst_GO->pst_BV);
				pst_Max = OBJ_pst_BV_GetGMax(_pst_GO->pst_BV);

				MATH_SubVector(&st_Diag, pst_Max, pst_Min);

				st_ODE_Box.st_Max.x = pst_ODE->st_Offset.x + (st_Diag.x / 2.0f);
				st_ODE_Box.st_Max.y = pst_ODE->st_Offset.y + (st_Diag.y / 2.0f);
				st_ODE_Box.st_Max.z = 0.01f;


				st_ODE_Box.st_Min.x = pst_ODE->st_Offset.x - (st_Diag.x / 2.0f);
				st_ODE_Box.st_Min.y = pst_ODE->st_Offset.y - (st_Diag.y / 2.0f);
				st_ODE_Box.st_Min.z = -0.01f;
			}
			else
			{
				st_ODE_Box.st_Max.x = pst_ODE->f_X / 2.0f;
				st_ODE_Box.st_Max.y = pst_ODE->f_Y / 2.0f;
				st_ODE_Box.st_Max.z = pst_ODE->f_Z / 2.0f;

				st_ODE_Box.st_Min.x = -(pst_ODE->f_X / 2.0f);
				st_ODE_Box.st_Min.y = -(pst_ODE->f_Y / 2.0f);
				st_ODE_Box.st_Min.z = -(pst_ODE->f_Z / 2.0f);
				
				MATH_GetRotationMatrix(&st_ODEMatrix, &pst_ODE->st_RotMatrix);
				MATH_SetTranslation(&st_ODEMatrix, &pst_ODE->st_Offset);

			}

			pst_Box = (COL_tdst_Box *) &st_ODE_Box;

			b_ObjectMode = TRUE;
			b_NoScaleMode = 1;
			b_Specific = 1;
			b_Active = 1;

		}
		break;
#endif
	case GEO_Cul_Cob:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_Cob	*pst_Cob;
			COL_tdst_ColMap *pst_ColMap;
			UCHAR			uc_ENG_Index;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Cob = (COL_tdst_Cob *) _pst_Data;
			pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;
			pst_Box = (COL_tdst_Box *) pst_Cob->pst_MathCob->p_Shape;
			b_ObjectMode = TRUE;
			b_NoScaleMode = FALSE;
			b_Specific = FALSE;
			uc_ENG_Index = COL_uc_ColMap_GetEngineIndexWithCob(pst_ColMap, pst_Cob);
			b_Active = (_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active) && COL_b_ColMap_IsActive(pst_ColMap, uc_ENG_Index);
			pst_GMat = COL_pst_GMat_Get(pst_Cob, NULL);

			if
			(
				pst_GMat
			&&	(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)
			&&	(((_pst_DD->uc_ColMapDisplayMode & 0x7F) == 2) || ((_pst_DD->uc_ColMapDisplayMode & 0x7F) == 3))
			) return;


			if
			(
				((_pst_DD->uc_ColMapDisplayMode & 0x7F) == 1)
			&&	((!pst_GMat) || (pst_GMat && !(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)))
			) return;
		}
		break;
	}

	b_Selected = FALSE;

	/* The zone is in the Object coordinate system. We push the Object matrix. */
	if(b_ObjectMode)
	{
		MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
		MATH_ClearScale(&st_NoScaleMatrix, 1);
		SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_NoScaleMatrix);
		if(!b_Plane && _uc_Type == GEO_Cul_ODE)
			SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_ODEMatrix);
	}

	if(b_NoScaleMode)
	{
		st_Min = *COL_pst_Shape_GetMin(pst_Box);
		st_Max = *COL_pst_Shape_GetMax(pst_Box);
	}
	else
	{
		/* Gets the scale of the object. */
		MATH_GetScale(&st_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
		pst_Min = COL_pst_Shape_GetMin(pst_Box);
		pst_Max = COL_pst_Shape_GetMax(pst_Box);

		/*$F----------- Min -------------- */
		st_Min.x = pst_Min->x * st_Scale.x;
		st_Min.y = pst_Min->y * st_Scale.y;
		st_Min.z = pst_Min->z * st_Scale.z;

		/*$F----------- Max -------------- */
		st_Max.x = pst_Max->x * st_Scale.x;
		st_Max.y = pst_Max->y * st_Scale.y;
		st_Max.z = pst_Max->z * st_Scale.z;
	}

	GEO_DebugObject_PushBoxMatrix(&_pst_DD->st_MatrixStack, &st_Min, &st_Max);

	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

	b_Selected = SEL_RetrieveItem(_pst_DD->pst_World->pst_Selection, _pst_Data) ? TRUE : FALSE;

	switch(_uc_Type)
	{
	case GEO_Cul_ZDx:
		_pst_DD->ul_ColorConstant = b_Active ? b_Selected ? b_Specific ? GEO_Cul_WhiteRed : GEO_Cul_WhiteBlue : b_Specific ? GEO_Cul_Red : GEO_Cul_Blue : b_Selected ? GEO_Cul_WhiteBlack : GEO_Cul_Black;
    	GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Box, GEO_Cul_Zone_DrawMask, 0, NULL);
		break;

#ifdef ODE_INSIDE
	case GEO_Cul_ODE:
		if(SEL_RetrieveItem(_pst_DD->pst_World->pst_Selection, _pst_GO) && _pst_DD->uc_EditODE)
		{
			_pst_DD->ul_ColorConstant = GEO_Cul_White;
		}
		else
		{
			const dBodyID id = _pst_GO->pst_Base->pst_ODE->ode_id_body;

			if( id != 0 && dBodyIsEnabled( id ) )
				_pst_DD->ul_ColorConstant = GEO_Cul_Yellow;
			else
				_pst_DD->ul_ColorConstant = GEO_Cul_Orange3;
		}

		_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_Fogged;
        GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Box, GEO_Cul_Zone_DrawMask, 0, NULL);
		break;
#endif
	case GEO_Cul_Cob:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			LIGHT_tdst_List st_SaveLightList;
			ULONG			ul_SaveDrawMask;
			ULONG			ul_SaveCurrentDrawMask;
			ULONG			ul_LightFlags;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			L_memcpy(&st_SaveLightList, &_pst_DD->st_LightList, sizeof(LIGHT_tdst_List));

			GEO_DebugObject_SetLight(0xFFFFFFFF);

			ul_LightFlags = ((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags;
			if(!(_pst_DD->ul_DrawMask & GDI_Cul_DM_DontForceColor))
			{
				((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags |= LIGHT_Cul_LF_Paint;
			}

			L_memcpy(&_pst_DD->st_LightList, &GEO_gst_DebugObject_LightList, sizeof(LIGHT_tdst_List));

			ul_SaveDrawMask = _pst_DD->ul_DrawMask;
			ul_SaveCurrentDrawMask = _pst_DD->ul_CurrentDrawMask;
			_pst_DD->ul_DrawMask |= GDI_Cul_DM_Draw + GDI_Cul_DM_DontForceColor;
			_pst_DD->ul_CurrentDrawMask = (_pst_DD->ul_DrawMask - GDI_Cul_DM_Fogged);
			_pst_DD->ul_CurrentDrawMask |= GDI_Cul_DM_MaterialColor;
			_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_UseAmbient;

			if(b_Active)
			{
				if(b_Selected)
				{
					if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX))
						MAT_gst_DefaultSingleMaterial.ul_Diffuse = GEO_Cul_WhiteBlue;
					else
						MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFF0000FF;
				}
				else
				{
					if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX))
						MAT_gst_DefaultSingleMaterial.ul_Diffuse = GEO_Cul_Blue;
					else
						MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFF00FF00;
				}
			}
			else
			{
				_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontForceColor;
				if(b_Selected)
					_pst_DD->ul_ColorConstant = 0x00333333;
				else
					_pst_DD->ul_ColorConstant = 0x00000000;
			}

			if(!(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_WiredCob))
			{
		    	GEO_DebugObject_Draw(_pst_DD,GEO_DebugObject_Box,_pst_DD->ul_CurrentDrawMask,0,OBJ_pst_GetAbsoluteMatrix(_pst_GO));
			}

			_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_NotWired;
			_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontForceColor;
			_pst_DD->ul_ColorConstant = 0xFFFF0000;

			if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_WiredCob)
			{
				if(b_Active)
				{
					if(b_Selected)
						_pst_DD->ul_ColorConstant = GEO_Cul_WhiteGreen;
					else
						_pst_DD->ul_ColorConstant = GEO_Cul_Green;

					if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX))
					{
						if(b_Selected)
							_pst_DD->ul_ColorConstant = GEO_Cul_WhiteBlue;
						else
							_pst_DD->ul_ColorConstant = GEO_Cul_Blue;
					}
				}
				else
				{
					if(b_Selected)
						_pst_DD->ul_ColorConstant = GEO_Cul_WhiteBlack;
					else
						_pst_DD->ul_ColorConstant = GEO_Cul_Black;
				}
			}

            GEO_DebugObject_Draw
                (
                _pst_DD,
                GEO_DebugObject_Box,
                _pst_DD->ul_CurrentDrawMask,
                0,
                OBJ_pst_GetAbsoluteMatrix(_pst_GO)
                );

			L_memcpy(&_pst_DD->st_LightList, &st_SaveLightList, sizeof(LIGHT_tdst_List));

			_pst_DD->ul_DrawMask = ul_SaveDrawMask;
			_pst_DD->ul_CurrentDrawMask = ul_SaveCurrentDrawMask;
			MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFFFFFFFF;
			((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags = ul_LightFlags;
		}
		break;
	}

	if
	(
		(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer)
	&&	((GetAsyncKeyState('Z') < 0) || b_Selected || (_uc_Type == GEO_Cul_Cob))
	)
	{
		pst_GraphicZone = GEO_Zone_GetNextZone(&_pst_DD->st_DisplayedZones);
		pst_GraphicZone->pv_Data = _pst_Data;
		pst_GraphicZone->pst_GO = _pst_GO;
		pst_GraphicZone->uc_Type = (_uc_Type == GEO_Cul_ZDx) ? GEO_Cul_GraphicZDx : GEO_Cul_GraphicCob;
		GRO_RenderPickableObject
		(
			NULL,
			GEO_pst_DebugObject_Get(GEO_DebugObject_Box),
			(ULONG) (pst_GraphicZone - _pst_DD->st_DisplayedZones.dpst_GraphicZone),
			SOFT_Cuc_PBQF_Zone
		);
	}

	GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);

	/* The zone is in the Object coordinate system. */
	if(b_ObjectMode && _uc_Type == GEO_Cul_ODE && !b_Plane)
		SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);	
	if(b_ObjectMode) SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);

	_pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_DisplaySphere
(
	GDI_tdst_DisplayData	*_pst_DD,
	void					*_pst_Data,
	OBJ_tdst_GameObject		*_pst_GO,
	UCHAR					_uc_Type
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Sphere			*pst_Sphere;
	GEO_tdst_GraphicZone	*pst_GraphicZone;
	MATH_tdst_Vector		st_Scale, st_Center, *pst_Center;
	float					f_Radius;
	ULONG					ul_SaveColor;
	MATH_tdst_Matrix		st_NoScaleMatrix;
	BOOL					b_Selected, b_Specific, b_Active;
	BOOL					b_ObjectMode, b_NoScaleMode;
	COL_tdst_GameMat		*pst_GMat;
#ifdef ODE_INSIDE
	COL_tdst_Sphere			st_ODE_Sphere;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_SaveColor = _pst_DD->ul_ColorConstant;
	switch(_uc_Type)
	{
	case GEO_Cul_ZDx:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_ZDx		*pst_ZDx;
			COL_tdst_Instance	*pst_Instance;
			UCHAR				uc_ENG_Index;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
			pst_ZDx = (COL_tdst_ZDx *) _pst_Data;
			pst_Sphere = (COL_tdst_Sphere *) pst_ZDx->p_Shape;
			b_ObjectMode = TRUE;
			b_NoScaleMode = COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_NoScale);
			b_Specific = COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_Specific);
			uc_ENG_Index = COL_uc_Instance_GetEngineIndexWithZone(pst_Instance, pst_ZDx);
			b_Active = (_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active) && COL_b_Instance_IsActive(pst_Instance, uc_ENG_Index);
		}
		break;
	case GEO_Cul_Cob:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_Cob	*pst_Cob;
			COL_tdst_ColMap *pst_ColMap;
			UCHAR			uc_ENG_Index;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Cob = (COL_tdst_Cob *) _pst_Data;
			pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;
			pst_Sphere = (COL_tdst_Sphere *) pst_Cob->pst_MathCob->p_Shape;
			b_ObjectMode = TRUE;
			b_NoScaleMode = FALSE;
			b_Specific = FALSE;
			uc_ENG_Index = COL_uc_ColMap_GetEngineIndexWithCob(pst_ColMap, pst_Cob);
			b_Active = (_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active) &&  COL_b_ColMap_IsActive(pst_ColMap, uc_ENG_Index);

			pst_GMat = COL_pst_GMat_Get(pst_Cob, NULL);

			if
			(
				pst_GMat
			&&	(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)
			&&	(((_pst_DD->uc_ColMapDisplayMode & 0x7F) == 2) || ((_pst_DD->uc_ColMapDisplayMode & 0x7F) == 3))
			) return;

			if
			(
				((_pst_DD->uc_ColMapDisplayMode & 0x7F) == 1)
			&&	((!pst_GMat) || (pst_GMat && !(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)))
			) return;
		}
		break;

#ifdef ODE_INSIDE
	case GEO_Cul_ODE:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			DYN_tdst_ODE	*pst_ODE;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_ODE = (DYN_tdst_ODE *) _pst_Data;

			st_ODE_Sphere.f_Radius = pst_ODE->f_X;
			MATH_CopyVector(&st_ODE_Sphere.st_Center, &pst_ODE->st_Offset);

			pst_Sphere = (COL_tdst_Sphere *) &st_ODE_Sphere;
			b_ObjectMode = TRUE;
			b_NoScaleMode = 0;
			b_Specific = 1;
			b_Active = 1;
		}
		break;
#endif
	}

	b_Selected = FALSE;

	/* The zone is in the Object coordinate system. We push the Object matrix. */
	if(b_ObjectMode)
	{
		MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
		MATH_ClearScale(&st_NoScaleMatrix, 1);
		SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_NoScaleMatrix);
	}

	if(b_NoScaleMode)
	{
		st_Center = *COL_pst_Shape_GetCenter(pst_Sphere);
		f_Radius = COL_f_Shape_GetRadius(pst_Sphere);
	}
	else
	{
		/* Gets the scale of the object. */
		MATH_GetScale(&st_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
		pst_Center = COL_pst_Shape_GetCenter(pst_Sphere);

		st_Center.x = pst_Center->x * st_Scale.x;
		st_Center.y = pst_Center->y * st_Scale.y;
		st_Center.z = pst_Center->z * st_Scale.z;

		f_Radius = COL_f_Shape_GetRadius(pst_Sphere) * fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
	}

	GEO_DebugObject_PushSphereMatrix(&_pst_DD->st_MatrixStack, &st_Center, f_Radius);
	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

#ifdef ODE_INSIDE
	if(_uc_Type == GEO_Cul_ODE)
		b_Selected = FALSE;
	else
		b_Selected = SEL_RetrieveItem(_pst_DD->pst_World->pst_Selection, _pst_Data) ? TRUE : FALSE;
#else
	b_Selected = SEL_RetrieveItem(_pst_DD->pst_World->pst_Selection, _pst_Data) ? TRUE : FALSE;
#endif

	switch(_uc_Type)
	{
	case GEO_Cul_ZDx:
		{
			_pst_DD->ul_ColorConstant = b_Active ? b_Selected ? b_Specific ? GEO_Cul_WhiteRed : GEO_Cul_WhiteBlue : b_Specific ? GEO_Cul_Red : GEO_Cul_Blue : b_Selected ? GEO_Cul_WhiteBlack : GEO_Cul_Black;
            GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Sphere, GEO_Cul_Zone_DrawMask, 0, NULL);
		}
		break;

#ifdef ODE_INSIDE
	case GEO_Cul_ODE:
		{			
			const dBodyID id = _pst_GO->pst_Base->pst_ODE->ode_id_body;

			if( id != 0 && dBodyIsEnabled( id ) )
				_pst_DD->ul_ColorConstant = GEO_Cul_Yellow;
			else
				_pst_DD->ul_ColorConstant = GEO_Cul_Orange3;

			_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_Fogged;
            GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Sphere, GEO_Cul_Zone_DrawMask, 0, NULL);
		}
		break;
#endif
	case GEO_Cul_Cob:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			LIGHT_tdst_List st_SaveLightList;
			ULONG			ul_SaveDrawMask;
			ULONG			ul_SaveCurrentDrawMask;
			ULONG			ul_LightFlags;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			L_memcpy(&st_SaveLightList, &_pst_DD->st_LightList, sizeof(LIGHT_tdst_List));

			GEO_DebugObject_SetLight(0xFFFFFFFF);

			ul_LightFlags = ((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags;
			if(!(_pst_DD->ul_DrawMask & GDI_Cul_DM_DontForceColor))
			{
				((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags |= LIGHT_Cul_LF_Paint;
			}

			L_memcpy(&_pst_DD->st_LightList, &GEO_gst_DebugObject_LightList, sizeof(LIGHT_tdst_List));

			ul_SaveDrawMask = _pst_DD->ul_DrawMask;
			ul_SaveCurrentDrawMask = _pst_DD->ul_CurrentDrawMask;
			_pst_DD->ul_DrawMask |= GDI_Cul_DM_Draw + GDI_Cul_DM_DontForceColor;
			_pst_DD->ul_CurrentDrawMask = (_pst_DD->ul_DrawMask - GDI_Cul_DM_Fogged);
			_pst_DD->ul_CurrentDrawMask |= GDI_Cul_DM_MaterialColor;
			_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_UseAmbient;

			if(b_Active)
			{
				if(b_Selected)
				{
					if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX))
						MAT_gst_DefaultSingleMaterial.ul_Diffuse = GEO_Cul_WhiteBlue;
					else
						MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFF0000FF;
				}
				else
				{
					if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX))
						MAT_gst_DefaultSingleMaterial.ul_Diffuse = GEO_Cul_Blue;
					else
						MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFF00FF00;
				}
			}
			else
			{
				_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontForceColor;
				if(b_Selected)
					_pst_DD->ul_ColorConstant = 0x00333333;
				else
					_pst_DD->ul_ColorConstant = 0x00000000;
			}

			if(!(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_WiredCob))
			{
                GEO_DebugObject_Draw
                    (
                    _pst_DD,
                    GEO_DebugObject_Sphere,
                    _pst_DD->ul_CurrentDrawMask,
                    0,
                    OBJ_pst_GetAbsoluteMatrix(_pst_GO)
                    );
			}

			_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_NotWired;
			_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontForceColor;
			_pst_DD->ul_ColorConstant = 0xFFFF0000;

			if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_WiredCob)
			{
				if(b_Active)
				{
					if(b_Selected)
						_pst_DD->ul_ColorConstant = GEO_Cul_WhiteGreen;
					else
						_pst_DD->ul_ColorConstant = GEO_Cul_Green;

					if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX))
					{
						if(b_Selected)
							_pst_DD->ul_ColorConstant = GEO_Cul_WhiteBlue;
						else
							_pst_DD->ul_ColorConstant = GEO_Cul_Blue;
					}
				}
				else
				{
					if(b_Selected)
						_pst_DD->ul_ColorConstant = GEO_Cul_WhiteBlack;
					else
						_pst_DD->ul_ColorConstant = GEO_Cul_Black;
				}
			}

            GEO_DebugObject_Draw
                (
                _pst_DD,
                GEO_DebugObject_Sphere,
                _pst_DD->ul_CurrentDrawMask,
                0,
                OBJ_pst_GetAbsoluteMatrix(_pst_GO)
                );

			L_memcpy(&_pst_DD->st_LightList, &st_SaveLightList, sizeof(LIGHT_tdst_List));

			_pst_DD->ul_DrawMask = ul_SaveDrawMask;
			_pst_DD->ul_CurrentDrawMask = ul_SaveCurrentDrawMask;
			MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFFFFFFFF;
			((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags = ul_LightFlags;
		}
		break;
	}

	if
	(
		(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer)
	&&	((GetAsyncKeyState('Z') < 0) || b_Selected || (_uc_Type == GEO_Cul_Cob))
	)
	{
		pst_GraphicZone = GEO_Zone_GetNextZone(&_pst_DD->st_DisplayedZones);
		pst_GraphicZone->pv_Data = _pst_Data;
		pst_GraphicZone->pst_GO = _pst_GO;
		pst_GraphicZone->uc_Type = (_uc_Type == GEO_Cul_ZDx) ? GEO_Cul_GraphicZDx : GEO_Cul_GraphicCob;
		GRO_RenderPickableObject
		(
			NULL,
			GEO_pst_DebugObject_Get(GEO_DebugObject_Sphere),
			(ULONG) (pst_GraphicZone - _pst_DD->st_DisplayedZones.dpst_GraphicZone),
			SOFT_Cuc_PBQF_Zone
		);
	}

	GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);

	/* The zone is in the Object coordinate system. */
	if(b_ObjectMode) SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);

	_pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_DisplayCylinder
(
	GDI_tdst_DisplayData	*_pst_DD, 
	void					*_p_data, 
	OBJ_tdst_GameObject		*_pst_GO, 
	UCHAR					_uc_Type
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap			*pst_ColMap;
	COL_tdst_Cylinder		*pst_Cyl;
#ifdef ODE_INSIDE
	COL_tdst_Cylinder		st_ODE_Cyl;
	MATH_tdst_Matrix		st_ZToY;
#endif
	GEO_tdst_GraphicZone	*pst_GraphicZone;
	ULONG					ul_SaveColor;
	MATH_tdst_Matrix		st_NoScaleMatrix;
	MATH_tdst_Vector		*pst_Center;
	MATH_tdst_Vector		st_Center, st_Scale;
	UCHAR					uc_ENG_Index;
	float					f_Radius, f_Height;
	BOOL					b_Selected, b_Active;
	LIGHT_tdst_List			st_SaveLightList;
	ULONG					ul_SaveDrawMask;
	ULONG					ul_SaveCurrentDrawMask;
	ULONG					ul_LightFlags;
	COL_tdst_GameMat		*pst_GMat;
	COL_tdst_Cob			*_pst_Cob;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_SaveColor = _pst_DD->ul_ColorConstant;

	switch(_uc_Type)
	{
#ifdef ODE_INSIDE
	case GEO_Cul_ODE:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			DYN_tdst_ODE	*pst_ODE;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/


			st_ZToY.Ix = 1.0f; st_ZToY.Iy = 0.0f; st_ZToY.Iz = 0.0f;
			st_ZToY.Jx = 0.0f; st_ZToY.Jy = 0.0f; st_ZToY.Jz = 1.0f;
			st_ZToY.Kx = 0.0f; st_ZToY.Ky = -1.0f; st_ZToY.Kz = 0.0f;

			st_ZToY.T.x = 0.0f; st_ZToY.T.y = 0.0f; st_ZToY.T.z = 0.0f;

			st_ZToY.lType = 4;

			pst_ODE = (DYN_tdst_ODE *) _p_data;

			pst_GMat = NULL;

			st_ODE_Cyl.f_Radius = pst_ODE->f_X;
			st_ODE_Cyl.f_Height = pst_ODE->f_Y;
			MATH_CopyVector(&st_ODE_Cyl.st_Center, &pst_ODE->st_Offset);

			pst_Cyl = (COL_tdst_Cylinder *) &st_ODE_Cyl;

			b_Selected = 1;

			SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix(_pst_GO));

			SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_ZToY);

			pst_Center = &pst_Cyl->st_Center;

			MATH_CopyVector(&st_Center, pst_Center);

			f_Radius = COL_f_Shape_GetRadius(pst_Cyl);
			f_Height = COL_f_Shape_GetHeight(pst_Cyl) / 2.0f;

			b_Active = TRUE;
		}
		break;
#endif

	case GEO_Cul_Cob:
		{
			_pst_Cob = (COL_tdst_Cob *) _p_data;

			pst_GMat = COL_pst_GMat_Get(_pst_Cob, NULL);			


			if
			(
				pst_GMat
			&&	(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)
			&&	(((_pst_DD->uc_ColMapDisplayMode & 0x7F) == 2) || ((_pst_DD->uc_ColMapDisplayMode & 0x7F) == 3))
			) return;

			if
			(
				((_pst_DD->uc_ColMapDisplayMode & 0x7F) == 1)
			&&	((!pst_GMat) || (pst_GMat && !(pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera)))
			) return;


			pst_Cyl = (COL_tdst_Cylinder *) _pst_Cob->pst_MathCob->p_Shape;
			pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;
			uc_ENG_Index = COL_uc_ColMap_GetEngineIndexWithCob(pst_ColMap, _pst_Cob);
			b_Active = (_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active) &&  COL_b_ColMap_IsActive(pst_ColMap, uc_ENG_Index);

			MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
			MATH_ClearScale(&st_NoScaleMatrix, 1);
			SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_NoScaleMatrix);

			MATH_GetScale(&st_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
			pst_Center = COL_pst_Shape_GetCenter(pst_Cyl);

			st_Center.x = pst_Center->x * st_Scale.x;
			st_Center.y = pst_Center->y * st_Scale.y;
			st_Center.z = pst_Center->z * st_Scale.z;

			f_Radius = COL_f_Shape_GetRadius(pst_Cyl) * fMax(st_Scale.x, st_Scale.y);
			f_Height = COL_f_Shape_GetHeight(pst_Cyl) * st_Scale.z;

			b_Selected = SEL_RetrieveItem(_pst_DD->pst_World->pst_Selection, _pst_Cob) ? TRUE : FALSE;

		}
		break;

	}

	GEO_DebugObject_PushCylinderMatrix(&_pst_DD->st_MatrixStack, &st_Center, f_Height, f_Radius);

	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

	L_memcpy(&st_SaveLightList, &_pst_DD->st_LightList, sizeof(LIGHT_tdst_List));

	GEO_DebugObject_SetLight(0xFFFFFFFF);

	ul_LightFlags = ((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags;
	if(!(_pst_DD->ul_DrawMask & GDI_Cul_DM_DontForceColor))
	{
		((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags |= LIGHT_Cul_LF_Paint;
	}

	L_memcpy(&_pst_DD->st_LightList, &GEO_gst_DebugObject_LightList, sizeof(LIGHT_tdst_List));

	ul_SaveDrawMask = _pst_DD->ul_DrawMask;
	ul_SaveCurrentDrawMask = _pst_DD->ul_CurrentDrawMask;
	_pst_DD->ul_DrawMask |= GDI_Cul_DM_Draw + GDI_Cul_DM_DontForceColor;
	_pst_DD->ul_CurrentDrawMask = (_pst_DD->ul_DrawMask - GDI_Cul_DM_Fogged);
	_pst_DD->ul_CurrentDrawMask |= GDI_Cul_DM_MaterialColor;
	_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_UseAmbient;

	if(b_Active)
	{
#ifdef ODE_INSIDE
		if(_uc_Type == GEO_Cul_ODE)
		{
			MAT_gst_DefaultSingleMaterial.ul_Diffuse = GEO_Cul_Orange3;
		}
#endif
		if(b_Selected)
		{
			if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX))
				MAT_gst_DefaultSingleMaterial.ul_Diffuse = GEO_Cul_WhiteBlue;
			else
				MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFF0000FF;
		}
		else
		{
			if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX))
				MAT_gst_DefaultSingleMaterial.ul_Diffuse = GEO_Cul_Blue;
			else
				MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFF00FF00;
		}
	}
	else
	{
		_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontForceColor;
		if(b_Selected)
			_pst_DD->ul_ColorConstant = 0x00333333;
		else
			_pst_DD->ul_ColorConstant = 0x00000000;
	}

#ifdef ODE_INSIDE
	if(_uc_Type != GEO_Cul_ODE)
#endif
		if(!(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_WiredCob))
		{
            GEO_DebugObject_Draw
                (
                _pst_DD,
                GEO_DebugObject_Cylinder,
                _pst_DD->ul_CurrentDrawMask,
                0,
                OBJ_pst_GetAbsoluteMatrix(_pst_GO)
                );
		}

	_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_NotWired;
	_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontForceColor;
	_pst_DD->ul_ColorConstant = 0xFFFF0000;

	if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_WiredCob)
	{
		if(b_Active)
		{
			if(b_Selected)
				_pst_DD->ul_ColorConstant = GEO_Cul_WhiteGreen;
			else
				_pst_DD->ul_ColorConstant = GEO_Cul_Green;

			if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX))
			{
				if(b_Selected)
					_pst_DD->ul_ColorConstant = GEO_Cul_WhiteBlue;
				else
					_pst_DD->ul_ColorConstant = GEO_Cul_Blue;
			}
		}
		else
		{
			if(b_Selected)
				_pst_DD->ul_ColorConstant = GEO_Cul_WhiteBlack;
			else
				_pst_DD->ul_ColorConstant = GEO_Cul_Black;
		}
	}
#ifdef ODE_INSIDE
	if(_uc_Type == GEO_Cul_ODE)
	{
		const dBodyID id = _pst_GO->pst_Base->pst_ODE->ode_id_body;

		if( id != 0 && dBodyIsEnabled( id ) )
			_pst_DD->ul_ColorConstant = GEO_Cul_Yellow;
		else
			_pst_DD->ul_ColorConstant = GEO_Cul_Orange3;
	}
#endif

    GEO_DebugObject_Draw
        (
        _pst_DD,
        GEO_DebugObject_Cylinder,
        _pst_DD->ul_CurrentDrawMask,
        0,
        OBJ_pst_GetAbsoluteMatrix(_pst_GO)
        );

	L_memcpy(&_pst_DD->st_LightList, &st_SaveLightList, sizeof(LIGHT_tdst_List));

	_pst_DD->ul_DrawMask = ul_SaveDrawMask;
	_pst_DD->ul_CurrentDrawMask = ul_SaveCurrentDrawMask;
	MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFFFFFFFF;

#ifdef ODE_INSIDE
	if(_uc_Type != GEO_Cul_ODE)
#endif
		if((_pst_DD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer) || b_Selected)
		{
			pst_GraphicZone = GEO_Zone_GetNextZone(&_pst_DD->st_DisplayedZones);
			pst_GraphicZone->pv_Data = _pst_Cob;
			pst_GraphicZone->pst_GO = _pst_GO;
			pst_GraphicZone->uc_Type = GEO_Cul_GraphicCob;
			GRO_RenderPickableObject
			(
				NULL,
				GEO_pst_DebugObject_Get(GEO_DebugObject_Cylinder),
				(ULONG) (pst_GraphicZone - _pst_DD->st_DisplayedZones.dpst_GraphicZone),
				SOFT_Cuc_PBQF_Zone
			);
		}

#ifdef ODE_INSIDE
	if(_uc_Type == GEO_Cul_ODE)
		GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);
#endif
	GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);

	SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);
	_pst_DD->ul_ColorConstant = ul_SaveColor;
	((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags = ul_LightFlags;
}

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_CreateGeoFromCob(OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *_pst_Geo, COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_IndexedTriangles			*pst_CobObj;
	COL_tdst_ElementIndexedTriangles	*pst_CobElement, *pst_LastElement;
	GEO_tdst_ElementIndexedTriangles	*pst_GeoElement;
	COL_tdst_IndexedTriangle			*pst_CobTriangle, *pst_LastTriangle;
	GEO_tdst_IndexedTriangle			*pst_GeoTriangle;
	ULONG								ul_Element, ul_Triangle;
	MATH_tdst_Vector					st_Scale, *pst_CobPoint, *pst_LastPoint;
	GEO_Vertex							*pst_GeoPoint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Cob->uc_Type != COL_C_Zone_Triangles) return;

	pst_CobObj = _pst_Cob->pst_TriangleCob;

	_pst_Geo->l_NbPoints = pst_CobObj->l_NbPoints;
	_pst_Geo->l_NbElements = pst_CobObj->l_NbElements;

	_pst_Geo->dst_UV = NULL;
	_pst_Geo->l_NbUVs = 0;

	if(0 && _pst_GO && MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_GO)))
	{
		_pst_Geo->dst_Point = (GEO_Vertex *) MEM_p_Alloc(sizeof(GEO_Vertex) * pst_CobObj->l_NbPoints);

		MATH_GetScale(&st_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
		pst_GeoPoint = _pst_Geo->dst_Point;
		pst_CobPoint = pst_CobObj->dst_Point;
		pst_LastPoint = pst_CobPoint + pst_CobObj->l_NbPoints;
		for(; pst_CobPoint < pst_LastPoint; pst_GeoPoint++, pst_CobPoint++)
		{
			pst_GeoPoint->x = pst_CobPoint->x  / st_Scale.x;
			pst_GeoPoint->y = pst_CobPoint->y  / st_Scale.y;
			pst_GeoPoint->z = pst_CobPoint->z  / st_Scale.z;
		}
	}
	else
	{
		_pst_Geo->dst_Point = (GEO_Vertex *) MEM_p_Alloc(sizeof(GEO_Vertex) * pst_CobObj->l_NbPoints);
		pst_GeoPoint = _pst_Geo->dst_Point;
		pst_CobPoint = pst_CobObj->dst_Point;
		pst_LastPoint = pst_CobPoint + pst_CobObj->l_NbPoints;
		for(; pst_CobPoint < pst_LastPoint; pst_GeoPoint++, pst_CobPoint++)
		{
			pst_GeoPoint->x = pst_CobPoint->x;
			pst_GeoPoint->y = pst_CobPoint->y;
			pst_GeoPoint->z = pst_CobPoint->z;
		}
	}

	_pst_Geo->dst_Element = (GEO_tdst_ElementIndexedTriangles *) MEM_p_Alloc(sizeof(GEO_tdst_ElementIndexedTriangles) * pst_CobObj->l_NbElements);
	L_memset(_pst_Geo->dst_Element, 0, sizeof(GEO_tdst_ElementIndexedTriangles) * pst_CobObj->l_NbElements);

	pst_CobElement = pst_CobObj->dst_Element;
	pst_LastElement = pst_CobElement + pst_CobObj->l_NbElements;
	pst_GeoElement = _pst_Geo->dst_Element;
	for(ul_Element = 0; pst_CobElement < pst_LastElement; pst_CobElement++, pst_GeoElement++, ul_Element++)
	{
		pst_GeoElement->l_NbTriangles = (LONG) pst_CobElement->uw_NbTriangles;
		pst_GeoElement->l_MaterialId = pst_CobElement->l_MaterialId;
		if(pst_GeoElement->l_NbTriangles)
		{
			pst_GeoElement->dst_Triangle = (GEO_tdst_IndexedTriangle *) MEM_p_Alloc(pst_GeoElement->l_NbTriangles * sizeof(GEO_tdst_IndexedTriangle));
			L_memset(pst_GeoElement->dst_Triangle, 0, pst_GeoElement->l_NbTriangles * sizeof(GEO_tdst_IndexedTriangle));

			pst_CobTriangle = pst_CobElement->dst_Triangle;
			pst_GeoTriangle = pst_GeoElement->dst_Triangle;
			pst_LastTriangle = pst_CobTriangle + pst_CobElement->uw_NbTriangles;

			/* We go thru all the triangles of the current object. */
			for
			(
				ul_Triangle = 0;
				pst_CobTriangle < pst_LastTriangle;
				pst_CobTriangle++, pst_GeoTriangle++, ul_Triangle++
			)
			{
				L_memcpy(pst_GeoTriangle->auw_Index, pst_CobTriangle->auw_Index, 3 * sizeof(USHORT));
				pst_GeoTriangle->auw_UV[0] = 0;
				pst_GeoTriangle->auw_UV[1] = 0;
				pst_GeoTriangle->auw_UV[2] = 0;
			}
		}
	}

	_pst_Geo->dst_PointNormal = (MATH_tdst_Vector *) MEM_p_Alloc(sizeof(MATH_tdst_Vector) * pst_CobObj->l_NbPoints);
	GEO_ComputeNormals(_pst_Geo);
}

void	GEO_OK3_Display(GDI_tdst_DisplayData *, OBJ_tdst_GameObject *, BOOL);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_Display(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ColMap		*pst_ColMap;
	COL_tdst_Cob		**dpst_Cob, **dpst_LastCob;
	COL_tdst_ZDx		**dpst_ZDx, **dpst_LastZDx;
	UCHAR				uc_Index;
	ULONG				ul_SaveEditorFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_SaveEditorFlags = _pst_GO->ul_EditorFlags;

	if(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_DisplaySnd)
	{
		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ExtendedObject))
		{
			if(_pst_GO->pst_Extended->pst_Modifiers)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MDF_tdst_Modifier	*pst_MdF;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_MdF = _pst_GO->pst_Extended->pst_Modifiers;
				while(pst_MdF)
				{
					switch(pst_MdF->i->ul_Type)
					{
					case MDF_C_Modifier_Sound:
                        if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMdFplayer) 
                            GEO_Zone_DisplaySoundModifier(_pst_DD, pst_MdF);
						break;

                    case MDF_C_Modifier_SoundVolume:
                        if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMdFVol) 
                            GEO_Zone_DisplaySoundVolModifier(_pst_DD, pst_MdF);
						break;

					case MDF_C_Modifier_SoundFx:
                        if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMdFFx) 
						    GEO_Zone_DisplaySoundFxModifier(_pst_DD, pst_MdF);
						break;

					case MDF_C_Modifier_SoundLoading:
                        if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMdFWac) 
						    GEO_Zone_DisplayLoadingSoundModifier(_pst_DD, pst_MdF);
						break;

					default: break;
					}

					pst_MdF = pst_MdF->pst_Next;
				}
			}
		}

		if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayInstance)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			int						i;
			SND_tdst_SoundInstance	*pst_SI;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
			{
				pst_SI = SND_gst_Params.dst_Instance + i;
				if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;
				if(!(pst_SI->ul_Flags & SND_Cul_SF_MaskDynVol)) continue;

				if((unsigned int) pst_SI->p_GameObject == (unsigned int) _pst_GO)
				{
					GEO_Zone_DisplaySoundInstance(_pst_DD, pst_SI);
				}
			}
		}

		if((SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMicro) && SND_gst_Params.pst_EdiMicroGao)
		{
			if(SND_gst_Params.pst_EdiMicroGao == _pst_GO)
			{
				GEO_Zone_DisplaySoundMicro(_pst_DD);
			}
		}
	}
#ifdef JADEFUSION
    if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ExtendedObject))
    {
        if(_pst_GO->pst_Extended->pst_Modifiers)
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
            MDF_tdst_Modifier	*pst_MdF;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

            pst_MdF = _pst_GO->pst_Extended->pst_Modifiers;
            while(pst_MdF)
            {
                switch(pst_MdF->i->ul_Type)
                {
                case MDF_C_Modifier_Wind:
                    {
                        GEO_MDFWind_Display(_pst_DD, _pst_GO, pst_MdF);
                    }
                    break;

                case MDF_C_Modifier_SoftBody:
                    {
                        GEO_ModifierSoftBody_Display(_pst_DD, _pst_GO, pst_MdF);
                    }
                    break;

                default:
                    break;
                }

                pst_MdF = pst_MdF->pst_Next;
            }
        }
	}
#endif

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
#ifdef ACTIVE_EDITORS
		if(_pst_GO->ul_EditorFlags &  OBJ_C_EditFlags_Hidden) return;
#endif
		pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
		if(!pst_Instance) return;

		dpst_ZDx = pst_Instance->dpst_ZDx;

		if(!dpst_ZDx) return;

		dpst_LastZDx = dpst_ZDx + pst_Instance->uc_NbOfZDx;

		for(uc_Index = 0; dpst_ZDx < dpst_LastZDx; dpst_ZDx++, uc_Index++)
		{
			if(COL_b_Zone_TestFlag(*dpst_ZDx, COL_C_Zone_ZDM | COL_C_Zone_ZDE))
			{
				if
				(
					!(
						(
							(COL_b_Zone_TestFlag(*dpst_ZDx, COL_C_Zone_ZDM))
						&&	(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowZDM)
						)
					||	(
								(COL_b_Zone_TestFlag(*dpst_ZDx, COL_C_Zone_ZDE))
							&&	(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowZDE)
							)
					)
				) continue;
			}
			else
			{
				if(!(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowZDM) && !(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowZDE))
					continue;
			}

			switch((*dpst_ZDx)->uc_Type)
			{
			case COL_C_Zone_Sphere:
				GEO_Zone_DisplaySphere(_pst_DD, *dpst_ZDx, _pst_GO, GEO_Cul_ZDx);
				break;

			case COL_C_Zone_Box:
				GEO_Zone_DisplayBox(_pst_DD, *dpst_ZDx, _pst_GO, GEO_Cul_ZDx);
				break;
			}
		}
	}


	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap) && !(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden))
	{
		pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;
		if(!pst_ColMap) return;

		if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_HiddenCob) return;

		dpst_Cob = pst_ColMap->dpst_Cob;
		dpst_LastCob = dpst_Cob + pst_ColMap->uc_NbOfCob;
		for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
		{
			if(!(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCOB)) continue;

			switch((*dpst_Cob)->uc_Type)
			{
			case COL_C_Zone_Cylinder:
				if(!(_pst_DD->ul_DrawMask & GDI_Cul_DM_NotWired)) _pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_WiredCob;

				GEO_Zone_DisplayCylinder(_pst_DD, *dpst_Cob, _pst_GO, GEO_Cul_Cob);
				break;

			case COL_C_Zone_Sphere:
				if(!(_pst_DD->ul_DrawMask & GDI_Cul_DM_NotWired)) _pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_WiredCob;
				GEO_Zone_DisplaySphere(_pst_DD, *dpst_Cob, _pst_GO, GEO_Cul_Cob);
				break;

			case COL_C_Zone_Box:
				if(!(_pst_DD->ul_DrawMask & GDI_Cul_DM_NotWired)) _pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_WiredCob;
				GEO_Zone_DisplayBox(_pst_DD, *dpst_Cob, _pst_GO, GEO_Cul_Cob);
				break;

			case COL_C_Zone_Triangles:
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					ULONG					ul_SaveCurrentDrawMask;
					ULONG					ul_SaveDrawMask;
					ULONG					ul_SaveColor;
					ULONG					ul_SaveInfo;
					GEO_tdst_GraphicZone	*pst_GraphicZone;
					GEO_tdst_Object			*pst_GeoCob;
					OBJ_tdst_Base			st_Base = { 0, 0, 0, 0 };
					OBJ_tdst_Base			*pst_SaveBase;
					GRO_tdst_Visu			st_Visu;
					BOOL					b_Selected, b_Active, b_ForceColor;
					MATH_tdst_Matrix		st_Unscaled;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					if(_pst_DD->ul_WiredMode & 3) _pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_WiredCob;

					ul_SaveCurrentDrawMask = _pst_DD->ul_CurrentDrawMask;
					ul_SaveDrawMask = _pst_DD->ul_DrawMask;
					ul_SaveColor = _pst_DD->ul_ColorConstant;
					ul_SaveInfo = _pst_DD->ul_DisplayInfo;

					b_ForceColor = !(_pst_DD->ul_DrawMask & GDI_Cul_DM_DontForceColor);

					_pst_DD->ul_DrawMask |= GDI_Cul_DM_Draw;
					_pst_DD->ul_DrawMask |= GDI_Cul_DM_DontForceColor;
					_pst_DD->ul_DrawMask &= ~GDI_Cul_DM_Fogged;
					_pst_DD->ul_DrawMask &= ~GDI_Cul_DM_UseAmbient;

					_pst_DD->ul_CurrentDrawMask = _pst_DD->ul_DrawMask;
					_pst_DD->ul_CurrentDrawMask |= GDI_Cul_DM_MaterialColor;

					pst_SaveBase = _pst_GO->pst_Base;

					if((*dpst_Cob)->p_GeoCob)
					{
						pst_GeoCob = (GEO_tdst_Object *) (*dpst_Cob)->p_GeoCob;
					}
					else
					{
						(*dpst_Cob)->p_GeoCob = (GEO_tdst_Object*)MEM_p_Alloc(sizeof(GEO_tdst_Object));
						pst_GeoCob = (GEO_tdst_Object*)(*dpst_Cob)->p_GeoCob;

						/* Fill the st_GeoCob structure with info needed to see the Cob. */
						L_memset(pst_GeoCob, 0, sizeof(GEO_tdst_Object));

						pst_GeoCob->st_Id.i = &GRO_gast_Interface[GRO_Geometric];
						GEO_CreateGeoFromCob(_pst_GO, pst_GeoCob, *dpst_Cob);
					}

					b_Selected = SEL_RetrieveItem(_pst_GO->pst_World->pst_Selection, *dpst_Cob) ? 1 : 0;
					b_Active = (_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active) && COL_b_ColMap_IsActive(pst_ColMap, 0);

#if defined(_XENON_RENDER)
                    L_memset(&st_Visu, 0, sizeof(GRO_tdst_Visu));
#endif

					st_Visu.ul_DrawMask = _pst_DD->ul_DrawMask;
					st_Visu.pst_Material = NULL;
					st_Visu.dul_VertexColors = NULL;
					st_Visu.pst_Object = (GRO_tdst_Struct *) pst_GeoCob;
					st_Base.pst_Visu = &st_Visu;

					_pst_GO->pst_Base = &st_Base;

					if(_pst_DD->pst_EditOptions->ul_Flags & GRO_Cul_EOF_SubObject)
					{
						if(_pst_DD->uc_EditBounding)
						{
							MessageBox
							(
								NULL,
								"SubObject Mode -> BV Edition Mode canceled",
								TEXT("Warning"),
								MB_OK | MB_ICONWARNING | MB_TASKMODAL | MB_SETFOREGROUND
							);
							_pst_DD->uc_EditBounding = 0;
						}

						if(!((GEO_tdst_Object *) st_Visu.pst_Object)->pst_SubObject)
							GEO_SubObject_Create((GEO_tdst_Object *) st_Visu.pst_Object);
					}
					else
					{
						if(((GEO_tdst_Object *) st_Visu.pst_Object)->pst_SubObject)
							GEO_SubObject_Free((GEO_tdst_Object *) st_Visu.pst_Object);
					}

					MATH_CopyMatrix(&st_Unscaled, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
					st_Unscaled.Sx = 0.0f;
					st_Unscaled.Sy = 0.0f;
					st_Unscaled.Sz = 0.0f;
					MATH_ClearScaleType(&st_Unscaled);

					SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_Unscaled);
					_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
					GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

					if(st_Visu.pst_Object)
					{
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
						LIGHT_tdst_List st_SaveLightList;
						UCHAR			uc_ENG_Index;
						ULONG			ul_LightFlags;
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

						GEO_DebugObject_SetLight(0xFFFFFFFF);

						ul_LightFlags = ((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags;
						if(b_ForceColor)
						{
							((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags |= LIGHT_Cul_LF_Paint;
						}

						L_memcpy(&st_SaveLightList, &_pst_DD->st_LightList, sizeof(LIGHT_tdst_List));
						L_memcpy(&_pst_DD->st_LightList, &GEO_gst_DebugObject_LightList, sizeof(LIGHT_tdst_List));

						uc_ENG_Index = COL_uc_ColMap_GetEngineIndexWithCob(pst_ColMap, *dpst_Cob);
						if(COL_b_ColMap_IsActive(pst_ColMap, uc_ENG_Index))
						{
							MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFF00FF00;
						}
						else
						{
							MAT_gst_DefaultSingleMaterial.ul_Diffuse = GEO_Cul_Black;
						}

						_pst_DD->ul_DisplayFlags |= GDI_Cul_DF_ShowCurrentCOB;

						if(!(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_WiredCob))
						{
                            extern void GEO_RenderZone(OBJ_tdst_GameObject *_pst_GO);
							GDI_gpst_CurDD_SPR.pst_CurrentMat = &MAT_gst_DefaultSingleMaterial;
							GDI_gpst_CurDD_SPR.ul_CurrentDrawMask |= GDI_Cul_DM_DontAttenuateLight;
#if defined(_XENON_RENDER)
                            GDI_gpst_CurDD_SPR.ul_CurrentDrawMask &= ~GDI_Cul_DM_UseTexture;
#endif
							GEO_RenderZone(_pst_GO);
						}

						_pst_DD->ul_CurrentDrawMask = _pst_DD->ul_DrawMask;
						_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_NotWired;
						_pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_DontForceColor;
#if defined(_XENON_RENDER)
                        _pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_UseTexture;
#endif
						if(!(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_WiredCob))
						{
							_pst_DD->ul_ColorConstant = 0xFFFF0000;
						}
						else
						{
							if(b_Active)
							{
								if(b_Selected)
									_pst_DD->ul_ColorConstant = GEO_Cul_WhiteGreen;
								else
									_pst_DD->ul_ColorConstant = GEO_Cul_Green;
							}
							else
							{
								if(b_Selected)
									_pst_DD->ul_ColorConstant = GEO_Cul_WhiteBlack;
								else
									_pst_DD->ul_ColorConstant = GEO_Cul_Black;
							}
						}

						pst_GeoCob->st_Id.i->pfn_Render(_pst_GO);

						MAT_gst_DefaultSingleMaterial.ul_Diffuse = 0xFFFFFFFF;
						((LIGHT_tdst_Light *) GEO_gst_DebugObject_LightList.dpst_Light[0]->pst_Extended->pst_Light)->ul_Flags = ul_LightFlags;

						L_memcpy(&_pst_DD->st_LightList, &st_SaveLightList, sizeof(LIGHT_tdst_List));
#if defined(_XENON_RENDER)
                        if (GDI_b_IsXenonGraphics())
                        {
                            // Delete the dynamic mesh if one was created while rendering
                            if (st_Visu.p_XeElements != NULL)
                            {
                                MEM_Free(st_Visu.p_XeElements);
                                st_Visu.p_XeElements = NULL;
                            }
                        }
#endif
					}

					if(st_Visu.pst_Object && (_pst_DD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer))
					{
						pst_GraphicZone = GEO_Zone_GetNextZone(&_pst_DD->st_DisplayedZones);
						pst_GraphicZone->pv_Data = *dpst_Cob;
						pst_GraphicZone->pst_GO = _pst_GO;
						pst_GraphicZone->uc_Type = GEO_Cul_GraphicCob;

						GRO_RenderPickableObject
						(
							_pst_GO,
							(GEO_tdst_Object *) st_Visu.pst_Object,
							(ULONG) (pst_GraphicZone - _pst_DD->st_DisplayedZones.dpst_GraphicZone),
							SOFT_Cuc_PBQF_Zone
						);
					}

					_pst_DD->ul_DisplayFlags &= ~GDI_Cul_DF_ShowCurrentCOB;

					_pst_GO->pst_Base = pst_SaveBase;

					_pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;

					SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);

					_pst_DD->ul_CurrentDrawMask = ul_SaveCurrentDrawMask;
					_pst_DD->ul_ColorConstant = ul_SaveColor;
					_pst_DD->ul_DrawMask = ul_SaveDrawMask;
					_pst_DD->ul_DisplayInfo = ul_SaveInfo;

					if(_pst_DD->ul_DisplayFlags & GDI_cul_DF_DisplayOK3) GEO_OK3_Display(_pst_DD, _pst_GO, TRUE);
				}
				break;
			}
		}
	}

#ifdef ODE_INSIDE
	if((_pst_DD->uc_ColMapDisplayMode & 0x7F) == 4)
		return;

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ODE))
	{
#ifdef ACTIVE_EDITORS
		if(_pst_GO->ul_EditorFlags &  OBJ_C_EditFlags_Hidden) return;
#endif
		if(!(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCOB)) return;

		if(!(_pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_MATHCOLMAP) && !(_pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)) return;

		if(_pst_GO->pst_Base->pst_ODE->uc_Type == ODE_TYPE_SPHERE)
			GEO_Zone_DisplaySphere(_pst_DD, _pst_GO->pst_Base->pst_ODE, _pst_GO, GEO_Cul_ODE);

		if
		(
			(_pst_GO->pst_Base->pst_ODE->uc_Type == ODE_TYPE_BOX)
		||	(_pst_GO->pst_Base->pst_ODE->uc_Type == ODE_TYPE_PLAN)
		)
			GEO_Zone_DisplayBox(_pst_DD, _pst_GO->pst_Base->pst_ODE, _pst_GO, GEO_Cul_ODE + _pst_GO->pst_Base->pst_ODE->uc_Type);

		if(_pst_GO->pst_Base->pst_ODE->uc_Type == ODE_TYPE_CYLINDER)
			GEO_Zone_DisplayCylinder(_pst_DD, (void *)_pst_GO->pst_Base->pst_ODE, _pst_GO, GEO_Cul_ODE);


	}
#endif


	_pst_GO->ul_EditorFlags = ul_SaveEditorFlags;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_OK3_Display(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO, BOOL _b_ColMap)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_OK3		*pst_OK3;
	COL_tdst_OK3_Box	*pst_OK3_Box;
	MATH_tdst_Vector	*pst_LMin, *pst_LMax;
	MATH_tdst_Vector	st_Scale, st_Min, st_Max;
	ULONG				ul_SaveColor;
	int					i, Total;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_ColMap)
	{
		if
		(
			!_pst_GO
		||	!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
		||	!(_pst_GO->pst_Extended)
		||	!(_pst_GO->pst_Extended->pst_Col)
		||	!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap
		||	!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob
		||	(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles)
		||	!(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->pst_TriangleCob->pst_OK3)
		) return;
	}
	else
	{
		if
		(
			!_pst_GO
		||	!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
		||	!(_pst_GO->pst_Base)
		||	!(_pst_GO->pst_Base->pst_Visu)
		||	!(_pst_GO->pst_Base->pst_Visu->pst_Object)
		|| !(OBJ_p_GetCurrentGeo(_pst_GO))
		) return;
	}

	if(_b_ColMap)
	{
		pst_OK3 = (((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->pst_TriangleCob->pst_OK3);
	}
	else
	{
		pst_OK3 = ((GEO_tdst_Object *) OBJ_p_GetCurrentGeo(_pst_GO))->pst_OK3;
		if(!pst_OK3) return;
	}

	Total = 0;
	for(i = 0; i < (int) pst_OK3->ul_NumBox; i++)
	{
		pst_OK3_Box = &pst_OK3->pst_OK3_Boxes[i];

		ul_SaveColor = _pst_DD->ul_ColorConstant;

		if(!pst_OK3_Box->ul_NumElement && !pst_OK3_Box->ul_OK3_Flag)
		{
			_pst_DD->ul_ColorConstant = 0x00FF0000;
		}
		else if(pst_OK3_Box->ul_OK3_Flag & 1)
		{
			_pst_DD->ul_ColorConstant = 0x000000FF;
		}
		else if(pst_OK3_Box->ul_OK3_Flag & 2)
		{
			_pst_DD->ul_ColorConstant = 0x0000FF00;
		}
		else
		{
			_pst_DD->ul_ColorConstant = 0x00FFFFFF;
		}

		SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, _pst_GO->pst_GlobalMatrix);

		if(_b_ColMap && MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
		{
			MATH_GetScale(&st_Scale, _pst_GO->pst_GlobalMatrix);

			MATH_InvEqualVector(&st_Scale);

			pst_LMin = &pst_OK3_Box->st_Min;;
			pst_LMax = &pst_OK3_Box->st_Max;

			st_Min.x = fMul(st_Scale.x, pst_LMin->x);
			st_Min.y = fMul(st_Scale.y, pst_LMin->y);
			st_Min.z = fMul(st_Scale.z, pst_LMin->z);

			st_Max.x = fMul(st_Scale.x, pst_LMax->x);
			st_Max.y = fMul(st_Scale.y, pst_LMax->y);
			st_Max.z = fMul(st_Scale.z, pst_LMax->z);
		}
		else
		{
			MATH_CopyVector(&st_Min, &pst_OK3_Box->st_Min);
			MATH_CopyVector(&st_Max, &pst_OK3_Box->st_Max);
		}

		GEO_DebugObject_PushBoxMatrix(&_pst_DD->st_MatrixStack, &st_Min, &st_Max);
		GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

        GEO_DebugObject_Draw(
            _pst_DD,
            GEO_DebugObject_Box,
            (GDI_Cul_DM_All -GDI_Cul_DM_NotWired -GDI_Cul_DM_DontForceColor -GDI_Cul_DM_Lighted -GDI_Cul_DM_Fogged),
            0,
            NULL
            );

		GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);
		SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);

		_pst_DD->ul_ColorConstant = ul_SaveColor;
	}
}

#define C_infinite	0.003f
ULONG	ul_NearColor;
ULONG	ul_FarColor;
ULONG	ul_PlayColor;
ULONG	ul_StopColor;
ULONG	ul_MiddleColor;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_DisplaySoundModifier(GDI_tdst_DisplayData *_pst_DD, MDF_tdst_Modifier *pst_MdF)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_SaveColor;
	MATH_tdst_Matrix		st_NoScaleMatrix;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	BOOL					b_Selected;
    ULONG                   ulPos;
    SND_tdst_SModifier      *pSMD;
    SND_tdst_SModifierExtPlayer*pExtPlay;
    ULONG                   ulFlags;
    SND_tdst_OneSound       *pst_Sound;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
    if(pst_SndMdF->ui_FileKey == BIG_C_InvalidKey) return;

    if(pst_SndMdF->i_SndIdx == -1)
    {
        ulPos = BIG_ul_SearchKeyToPos(pst_SndMdF->ui_FileKey);
        if(ulPos == -1) return;

        pSMD = (SND_tdst_SModifier*)LOA_ul_SearchAddress(ulPos);
        if((int)pSMD == -1) return;
    }
    else
    {
        pst_Sound = SND_gst_Params.dst_Sound + pst_SndMdF->i_SndIdx;
        if(pst_SndMdF->ui_FileKey != pst_Sound->ul_FileKey) return;
        pSMD = pst_Sound->pst_SModifier;
    }
    pExtPlay = (SND_tdst_SModifierExtPlayer*)pSMD->pv_Data;

    /**/
    b_Selected = SEL_RetrieveItem(pst_MdF->pst_GO->pst_World->pst_Selection, pst_MdF->pst_GO) ? 1 : 0;
    if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayOnlySelection)
	{
		if(!b_Selected && ((pst_SndMdF->ui_EdiFlags & MDF_Cte_SndEdiForceDisplay) == 0)) return;
		if(pst_SndMdF->ui_EdiFlags & MDF_Cte_SndEdiDontDisplay) return;
	}
	else
	{
		pst_SndMdF->ui_EdiFlags &= ~(MDF_Cte_SndEdiDontDisplay | MDF_Cte_SndEdiForceDisplay);
	}

    /**/
	ul_SaveColor = _pst_DD->ul_ColorConstant;
	if(b_Selected)
	{
		ul_NearColor = GEO_Cul_PinkS;
		ul_MiddleColor = GEO_Cul_PurpleS;
		ul_FarColor = GEO_Cul_OrangeS;
		ul_PlayColor = GEO_Cul_SpecialBlue1S;
		ul_StopColor = GEO_Cul_SpecialBlue2S;
	}
	else
	{
		ul_NearColor = GEO_Cul_Pink;
		ul_MiddleColor = GEO_Cul_Purple;
		ul_FarColor = GEO_Cul_Orange;
		ul_PlayColor = GEO_Cul_SpecialBlue1;
		ul_StopColor = GEO_Cul_SpecialBlue2;
	}

    /**/
    ulFlags = pExtPlay->ul_SndFlags & (SND_Cul_SF_DynVolSpheric | SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis);
 	MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(pst_MdF->pst_GO));
	MATH_ClearScale(&st_NoScaleMatrix, 1);

    if(ulFlags & SND_Cul_SF_DynVolSpheric)
	{
        if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayNear)
            GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, pExtPlay->ul_SndFlags, pExtPlay->f_CylinderHeight, pExtPlay->af_Near[0], ul_NearColor);
            
        if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMiddle)
            GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, pExtPlay->ul_SndFlags, pExtPlay->f_CylinderHeight, MATH_f_FloatBlend(pExtPlay->af_Near[0], pExtPlay->af_Far[0], pExtPlay->af_MiddleBlend[0]), ul_MiddleColor);
            
        if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayFar) 
            GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, pExtPlay->ul_SndFlags, pExtPlay->f_CylinderHeight, pExtPlay->af_Far[0], ul_FarColor);
            
        if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayStop)
            GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, pExtPlay->ul_SndFlags, pExtPlay->f_CylinderHeight, pExtPlay->af_Far[0] + 2 * pst_SndMdF->f_DeltaFar, ul_PlayColor);
            
        if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayStart)
            GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, pExtPlay->ul_SndFlags, pExtPlay->f_CylinderHeight, pExtPlay->af_Far[0] +  pst_SndMdF->f_DeltaFar, ul_StopColor);
	}
	else if	(ulFlags == (SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis))
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,
            pst_SndMdF->f_DeltaFar,
			pExtPlay->af_Near[0],
            MATH_f_FloatBlend(pExtPlay->af_Near[0], pExtPlay->af_Far[0], pExtPlay->af_MiddleBlend[0]),
			pExtPlay->af_Far[0],
			pExtPlay->af_Near[1],
			MATH_f_FloatBlend(pExtPlay->af_Near[1], pExtPlay->af_Far[1], pExtPlay->af_MiddleBlend[1]),
			pExtPlay->af_Far[1],
			pExtPlay->af_Near[2],
			MATH_f_FloatBlend(pExtPlay->af_Near[2], pExtPlay->af_Far[2], pExtPlay->af_MiddleBlend[2]),
			pExtPlay->af_Far[2]
		);
	}
	else if(ulFlags == (SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis))
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,pst_SndMdF->f_DeltaFar,
			pExtPlay->af_Near[0],
			MATH_f_FloatBlend(pExtPlay->af_Near[0], pExtPlay->af_Far[0], pExtPlay->af_MiddleBlend[0]),
			pExtPlay->af_Far[0],
			pExtPlay->af_Near[1],
			MATH_f_FloatBlend(pExtPlay->af_Near[1], pExtPlay->af_Far[1], pExtPlay->af_MiddleBlend[1]),
			pExtPlay->af_Far[1],
			C_infinite,
			C_infinite,
			C_infinite
		);
	}
	else if(ulFlags == (SND_Cul_SF_DynVolZaxis | SND_Cul_SF_DynVolYaxis))
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,pst_SndMdF->f_DeltaFar,
			C_infinite,
			C_infinite,
			C_infinite,
			pExtPlay->af_Near[1],
			MATH_f_FloatBlend(pExtPlay->af_Near[1], pExtPlay->af_Far[1], pExtPlay->af_MiddleBlend[1]),
			pExtPlay->af_Far[1],
			pExtPlay->af_Near[2],
			MATH_f_FloatBlend(pExtPlay->af_Near[2], pExtPlay->af_Far[2], pExtPlay->af_MiddleBlend[2]),
			pExtPlay->af_Far[2]
		);
	}
	else if(ulFlags == (SND_Cul_SF_DynVolZaxis | SND_Cul_SF_DynVolXaxis))
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,pst_SndMdF->f_DeltaFar,
			pExtPlay->af_Near[0],
			MATH_f_FloatBlend(pExtPlay->af_Near[0], pExtPlay->af_Far[0], pExtPlay->af_MiddleBlend[0]),
			pExtPlay->af_Far[0],
			C_infinite,
			C_infinite,
			C_infinite,
			pExtPlay->af_Near[2],
			MATH_f_FloatBlend(pExtPlay->af_Near[2], pExtPlay->af_Far[2], pExtPlay->af_MiddleBlend[2]),
			pExtPlay->af_Far[2]
		);
	}
    else if(ulFlags == SND_Cul_SF_DynVolXaxis)
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,pst_SndMdF->f_DeltaFar,
			pExtPlay->af_Near[0],
			MATH_f_FloatBlend(pExtPlay->af_Near[0], pExtPlay->af_Far[0], pExtPlay->af_MiddleBlend[0]),
			pExtPlay->af_Far[0],
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite
		);
	}
    else if(ulFlags == SND_Cul_SF_DynVolYaxis)
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,pst_SndMdF->f_DeltaFar,
			C_infinite,
			C_infinite,
			C_infinite,
			pExtPlay->af_Near[1],
			MATH_f_FloatBlend(pExtPlay->af_Near[1], pExtPlay->af_Far[1], pExtPlay->af_MiddleBlend[1]),
			pExtPlay->af_Far[1],
			C_infinite,
			C_infinite,
			C_infinite
		);
	}
    else if(ulFlags == SND_Cul_SF_DynVolZaxis)
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,pst_SndMdF->f_DeltaFar,
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite,
			pExtPlay->af_Near[2],
			MATH_f_FloatBlend(pExtPlay->af_Near[2], pExtPlay->af_Far[2], pExtPlay->af_MiddleBlend[2]),
			pExtPlay->af_Far[2]
		);
	}

	_pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DrawSoundBox
(
	GDI_tdst_DisplayData	*_pst_DD,
    MATH_tdst_Matrix        *pCenter,
    float                  f_DeltaFar,
	float					f_NearX,
	float					f_MiddleX,
	float					f_FarX,
	float					f_NearY,
	float					f_MiddleY,
	float					f_FarY,
	float					f_NearZ,
	float					f_MiddleZ,
	float					f_FarZ
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		st_Max, st_Min;
    GDI_tdst_Request_DrawBox st_Box;
	float	f_X, f_Y, f_Z;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    
    _pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	_pst_DD->st_GDI.pfnv_SetViewMatrix(_pst_DD->st_Camera.pst_ObjectToCameraMatrix);


    st_Box.f_Width = 2;
    st_Box.pst_Center = MATH_pst_GetTranslation(pCenter);
    st_Box.pst_M   = pCenter;
    st_Box.pst_Max = &st_Max;
    st_Box.pst_Min = &st_Min;
    st_Box.ul_Flags = GDI_Request_DrawBox_NoZWrite;

    if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayStop)
	{
		if(f_FarX != C_infinite)
			f_X = f_FarX + 2 * f_DeltaFar;
		else
			f_X = C_infinite;

		if(f_FarY != C_infinite)
			f_Y = f_FarY + 2 * f_DeltaFar;
		else
			f_Y = C_infinite;

		if(f_FarZ != C_infinite)
			f_Z = f_FarZ + 2 * f_DeltaFar;
		else
			f_Z = C_infinite;

		MATH_InitVector(&st_Max, f_X, f_Y, f_Z);
		MATH_NegVector(&st_Min, &st_Max);
        
        st_Box.ul_EdgeColor = 0xFF000000 | ul_PlayColor;
        st_Box.ul_SolidColor= 0x3F000000 | (0x00FFFFFF & ul_PlayColor);
	    _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawBox, (ULONG) &st_Box);    
	}

	if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayStart)
	{
		if(f_FarX != C_infinite)
			f_X = f_FarX + f_DeltaFar;
		else
			f_X = 2 * C_infinite;

		if(f_FarY != C_infinite)
			f_Y = f_FarY + f_DeltaFar;
		else
			f_Y = 2 * C_infinite;

		if(f_FarZ != C_infinite)
			f_Z = f_FarZ + f_DeltaFar;
		else
			f_Z = 2 * C_infinite;

		MATH_InitVector(&st_Max, f_X, f_Y, f_Z);
		MATH_NegVector(&st_Min, &st_Max);

        st_Box.ul_EdgeColor = 0xFF000000 | ul_StopColor;
        st_Box.ul_SolidColor= 0x3F000000 | (0x00FFFFFF & ul_StopColor);
	    _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawBox, (ULONG) &st_Box);    
	}

    if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayFar)
	{
		/* draw far box */
		MATH_InitVector(&st_Max, f_FarX + 2 * C_infinite, f_FarY + 2 * C_infinite, f_FarZ + 2 * C_infinite);
		MATH_NegVector(&st_Min, &st_Max);


        st_Box.ul_EdgeColor = 0xFF000000 | ul_FarColor;
        st_Box.ul_SolidColor= 0x3F000000 | (0x00FFFFFF & ul_FarColor);
	    _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawBox, (ULONG) &st_Box);    
	}

	if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMiddle)
	{
		/* draw middle box */
		MATH_InitVector(&st_Max, f_MiddleX + 3 * C_infinite, f_MiddleY + 3 * C_infinite, f_MiddleZ + 3 * C_infinite);
		MATH_NegVector(&st_Min, &st_Max);

        st_Box.ul_EdgeColor = 0xFF000000 | ul_MiddleColor;
        st_Box.ul_SolidColor= 0x3F000000 | (0x00FFFFFF & ul_MiddleColor);
	    _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawBox, (ULONG) &st_Box);    
    }

	if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayNear)
	{
		/* draw near box */
		MATH_InitVector(&st_Max, f_NearX + 3 * C_infinite, f_NearY + 3 * C_infinite, f_NearZ + 3 * C_infinite);
		MATH_NegVector(&st_Min, &st_Max);

        st_Box.ul_EdgeColor = 0xFF000000 | ul_NearColor;
        st_Box.ul_SolidColor= 0x3F000000 | (0x00FFFFFF & ul_NearColor);
	    _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawBox, (ULONG) &st_Box);    
    }

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DrawOneSoundBox
(
	GDI_tdst_DisplayData	*_pst_DD,
    MATH_tdst_Matrix        *pCenter,
    float					f_NearX,
	float					f_NearY,
	float					f_NearZ, 
    ULONG                   ulColor
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		st_Max, st_Min;
    GDI_tdst_Request_DrawBox st_Box;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    
    _pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	_pst_DD->st_GDI.pfnv_SetViewMatrix(_pst_DD->st_Camera.pst_ObjectToCameraMatrix);


    st_Box.f_Width = 2;
    st_Box.pst_Center = MATH_pst_GetTranslation(pCenter);
    st_Box.pst_M   = pCenter;
    st_Box.pst_Max = &st_Max;
    st_Box.pst_Min = &st_Min;
    st_Box.ul_Flags = GDI_Request_DrawBox_NoZWrite;

	MATH_InitVector(&st_Max, f_NearX + 3 * C_infinite, f_NearY + 3 * C_infinite, f_NearZ + 3 * C_infinite);
	MATH_NegVector(&st_Min, &st_Max);

    st_Box.ul_EdgeColor = 0xFF000000 | ulColor;
    st_Box.ul_SolidColor= 0x3F000000 | (0x00FFFFFF & ulColor);
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawBox, (ULONG) &st_Box);    
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_DisplaySoundFxModifier(GDI_tdst_DisplayData *_pst_DD, MDF_tdst_Modifier *pst_MdF)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    extern GEN_tdst_ModifierSoundFx *SND_p_FxNetworkGetMaster(int id);
	ULONG						ul_SaveColor;
	MATH_tdst_Matrix			st_NoScaleMatrix;
	GEN_tdst_ModifierSoundFx	*pst_SndMdF;
	GEN_tdst_ModifierSoundFx	*pMaster;
	BOOL						b_Selected;
	ULONG						ul_DistanceColor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_SaveColor = _pst_DD->ul_ColorConstant;
	pst_SndMdF = (GEN_tdst_ModifierSoundFx *) pst_MdF->p_Data;

	b_Selected = SEL_RetrieveItem(pst_MdF->pst_GO->pst_World->pst_Selection, pst_MdF->pst_GO) ? 1 : 0;

	if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayOnlySelection)
	{
		if(!b_Selected) return;
		if(pst_SndMdF->ui_MdfFlag & MDF_Cte_SndFx_EdiDontDisplay) return;
	}
	else
	{
		pst_SndMdF->ui_MdfFlag &= ~(MDF_Cte_SndFx_EdiDontDisplay);
	}

    if(pst_SndMdF->i_NetIdx == -1)
        pMaster = pst_SndMdF;
    else
    {
        pMaster = SND_p_FxNetworkGetMaster(pst_SndMdF->i_NetIdx);
        pMaster = pMaster ? pMaster : pst_SndMdF;
    }
    
    if(pMaster->i_CoreId)
        ul_DistanceColor = b_Selected ? GEO_Cul_FxRedS : GEO_Cul_FxRed;		
    else
        ul_DistanceColor = b_Selected ? GEO_Cul_FxYellowS : GEO_Cul_FxYellow;		

 	MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(pst_MdF->pst_GO));
	MATH_ClearScale(&st_NoScaleMatrix, 1);

    if(pst_SndMdF->ui_MdfFlag & MDF_Cte_SndFx_Sphere)
    {
        GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, SND_Cul_SF_DynVolSpheric | SND_Cul_SF_DynVolXaxis, 0, pst_SndMdF->af_Distance[0], ul_DistanceColor);
        
        if(pMaster->i_CoreId)
            ul_DistanceColor = b_Selected ? GEO_Cul_FxRed2S : GEO_Cul_FxRed2;		
        else
            ul_DistanceColor = b_Selected ? GEO_Cul_FxYellow2S : GEO_Cul_FxYellow2;		

        GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, SND_Cul_SF_DynVolSpheric | SND_Cul_SF_DynVolXaxis, 0, pst_SndMdF->af_Far[0], ul_DistanceColor);
    }
    else
    {
        GEO_DrawOneSoundBox(_pst_DD, &st_NoScaleMatrix, pst_SndMdF->af_Distance[0],pst_SndMdF->af_Distance[1], pst_SndMdF->af_Distance[2], ul_DistanceColor);
        
        if(pMaster->i_CoreId)
            ul_DistanceColor = b_Selected ? GEO_Cul_FxRed2S : GEO_Cul_FxRed2;		
        else
            ul_DistanceColor = b_Selected ? GEO_Cul_FxYellow2S : GEO_Cul_FxYellow2;		
        
        GEO_DrawOneSoundBox(_pst_DD, &st_NoScaleMatrix, pst_SndMdF->af_Far[0],pst_SndMdF->af_Far[1], pst_SndMdF->af_Far[2], ul_DistanceColor);
    }

    _pst_DD->ul_ColorConstant = ul_SaveColor;
}

void GEO_Zone_DisplaySoundVolModifier(GDI_tdst_DisplayData *_pst_DD, MDF_tdst_Modifier *pst_MdF)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG						ul_SaveColor;
	MATH_tdst_Matrix			st_NoScaleMatrix;
	SND_tdst_ModifierSoundVol	*pst_SndMdF;
	BOOL						b_Selected;
	ULONG						ul_DistanceColor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	ul_SaveColor = _pst_DD->ul_ColorConstant;
	pst_SndMdF = (SND_tdst_ModifierSoundVol *) pst_MdF->p_Data;

	b_Selected = SEL_RetrieveItem(pst_MdF->pst_GO->pst_World->pst_Selection, pst_MdF->pst_GO) ? 1 : 0;

	if( (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayOnlySelection)  && !b_Selected ) 
        return;

    ul_DistanceColor = b_Selected ? GEO_Cul_FxRedS : GEO_Cul_FxRed;		

 	MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(pst_MdF->pst_GO));
	MATH_ClearScale(&st_NoScaleMatrix, 1);

    if(pst_SndMdF->ul_Flags & SND_Cte_MdFSoundVol_Spheric)
    {
        GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, SND_Cul_SF_DynVolSpheric | SND_Cul_SF_DynVolXaxis, 0, pst_SndMdF->af_Near[0], ul_DistanceColor);
        
        ul_DistanceColor = b_Selected ? GEO_Cul_FxRed2S : GEO_Cul_FxRed2;		
        GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, SND_Cul_SF_DynVolSpheric | SND_Cul_SF_DynVolXaxis, 0, pst_SndMdF->af_Far[0], ul_DistanceColor);
    }
    else
    {
        GEO_DrawOneSoundBox(_pst_DD, &st_NoScaleMatrix, pst_SndMdF->af_Near[0],pst_SndMdF->af_Near[1], pst_SndMdF->af_Near[2], ul_DistanceColor);
        
        ul_DistanceColor = b_Selected ? GEO_Cul_FxRed2S : GEO_Cul_FxRed2;		
        GEO_DrawOneSoundBox(_pst_DD, &st_NoScaleMatrix, pst_SndMdF->af_Far[0],pst_SndMdF->af_Far[1], pst_SndMdF->af_Far[2], ul_DistanceColor);
    }

    _pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_DisplaySoundMicro(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_SaveColor;
	MATH_tdst_Matrix	st_NoScaleMatrix;
	MATH_tdst_Vector	st_ConeTrans;
	float				f_ConeRadius;
	float				f_ConeHeight;
	MATH_tdst_Vector	st_SphereTrans;
	float				f_SphereRadius;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* if(SND_gst_Params.ul_Flags & SND_Cte_EdiHideMdF) return; */
	if(!SND_gst_Params.pst_EdiMicroGao) return;

	ul_SaveColor = _pst_DD->ul_ColorConstant;

	/* local position */
	MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(SND_gst_Params.pst_EdiMicroGao));
	MATH_ClearScale(&st_NoScaleMatrix, 1);
	SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_NoScaleMatrix);

	MATH_InitVector(&st_ConeTrans, 0.0f, 0.7f, 0.0f);
	f_ConeRadius = 0.14f;
	f_ConeHeight = -1.0f;
	GEO_DebugObject_PushConeMatrix(&_pst_DD->st_MatrixStack, &st_ConeTrans, f_ConeHeight, f_ConeRadius);
	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

	_pst_DD->ul_ColorConstant = GEO_Cul_PinkS;
    GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Cone, GEO_Cul_MdF_DrawMask, 0, NULL);
	GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);

	MATH_InitVector(&st_SphereTrans, 0.0f, -0.5f, 0.0f);
	f_SphereRadius = 0.25f;
	GEO_DebugObject_PushSphereMatrix(&_pst_DD->st_MatrixStack, &st_SphereTrans, f_SphereRadius);
	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

	_pst_DD->ul_ColorConstant = GEO_Cul_PinkS;
    GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Sphere, GEO_Cul_MdF_DrawMask, 0, NULL);
	GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);

	/* The zone is in the Object coordinate system. */
	SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);

	_pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_DisplayLoadingSoundModifier(GDI_tdst_DisplayData *_pst_DD, MDF_tdst_Modifier *pst_MdF)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_SaveColor;
	MATH_tdst_Matrix		st_NoScaleMatrix;
	MDF_tdst_LoadingSound	*pst_SndMdF;
	BOOL					b_Selected;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_SaveColor = _pst_DD->ul_ColorConstant;
	pst_SndMdF = (MDF_tdst_LoadingSound *) pst_MdF->p_Data;

	b_Selected = SEL_RetrieveItem(pst_MdF->pst_GO->pst_World->pst_Selection, pst_MdF->pst_GO) ? 1 : 0;
	if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayOnlySelection)
	{
        if(!b_Selected && ((pst_SndMdF->ui_MdfFlag & MDF_Cte_SndEdiForceDisplay) == 0)) return;
		if(pst_SndMdF->ui_MdfFlag & MDF_Cte_SndEdiDontDisplay) return;
	}
	else
	{
        pst_SndMdF->ui_MdfFlag &= ~(MDF_Cte_SndEdiDontDisplay | MDF_Cte_SndEdiForceDisplay);
	}

    MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(pst_MdF->pst_GO));
	MATH_ClearScale(&st_NoScaleMatrix, 1);

    GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, SND_Cul_SF_DynVolSpheric | SND_Cul_SF_DynVolXaxis, 0, pst_SndMdF->f_LoadingDistance, GEO_Cul_Red);
	_pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Zone_DisplaySoundInstance(GDI_tdst_DisplayData *_pst_DD, SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_SaveColor;
	MATH_tdst_Matrix	st_NoScaleMatrix;
	BOOL				b_Selected;
    ULONG ulFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Selected = SEL_RetrieveItem
		(
			((OBJ_tdst_GameObject *) pst_SI->p_GameObject)->pst_World->pst_Selection,
			((OBJ_tdst_GameObject *) pst_SI->p_GameObject)
		) ? 1 : 0;

    if((SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayOnlySelection) && !b_Selected) return;

    ul_SaveColor = _pst_DD->ul_ColorConstant;
	if(b_Selected)
	{
		ul_NearColor = GEO_Cul_PinkS;
		ul_MiddleColor = GEO_Cul_PurpleS;
		ul_FarColor = GEO_Cul_OrangeS;
		ul_PlayColor = GEO_Cul_SpecialBlue1S;
		ul_StopColor = GEO_Cul_SpecialBlue2S;
	}
	else
	{
		ul_NearColor = GEO_Cul_Pink;
		ul_MiddleColor = GEO_Cul_Purple;
		ul_FarColor = GEO_Cul_Orange;
		ul_PlayColor = GEO_Cul_SpecialBlue1;
		ul_StopColor = GEO_Cul_SpecialBlue2;
	}

	/* local position */
    ulFlags = pst_SI->ul_Flags & (SND_Cul_SF_DynVolSpheric | SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis);
	MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(((OBJ_tdst_GameObject *) pst_SI->p_GameObject)));
	MATH_ClearScale(&st_NoScaleMatrix, 1);

	if(ulFlags & SND_Cul_SF_DynVolSpheric)
	{
        if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayNear)
            GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, ulFlags, pst_SI->f_CylinderHeight, pst_SI->af_Near[0], ul_NearColor);
            
		if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMiddle)
            GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, ulFlags, pst_SI->f_CylinderHeight, pst_SI->af_Middle[0], ul_MiddleColor);
            
		if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayFar) 
            GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, ulFlags, pst_SI->f_CylinderHeight, pst_SI->af_Far[0], ul_FarColor);
            
		if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayStop)
            GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, ulFlags, pst_SI->f_CylinderHeight, pst_SI->af_Far[0] + 2 * pst_SI->f_DeltaFar, ul_PlayColor);
            
		if(SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayStart)
            GEO_DrawSoundCapsule(_pst_DD, &st_NoScaleMatrix, ulFlags, pst_SI->f_CylinderHeight, pst_SI->af_Far[0] +  pst_SI->f_DeltaFar, ul_StopColor);
	   
	}
	else if	(ulFlags == (SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis))
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,
            pst_SI->f_DeltaFar,
			pst_SI->af_Near[0],
            pst_SI->af_Middle[0],
			pst_SI->af_Far[0],
			pst_SI->af_Near[1],
			pst_SI->af_Middle[1],
			pst_SI->af_Far[1],
			pst_SI->af_Near[2],
			pst_SI->af_Middle[2],
			pst_SI->af_Far[2]
		);
	}
	else if(ulFlags == (SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis))
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,
            pst_SI->f_DeltaFar,
			pst_SI->af_Near[0],
			pst_SI->af_Middle[0],
			pst_SI->af_Far[0],
			pst_SI->af_Near[1],
			pst_SI->af_Middle[1],
			pst_SI->af_Far[1],
			C_infinite,
			C_infinite,
			C_infinite
		);
	}
	else if(ulFlags == (SND_Cul_SF_DynVolZaxis | SND_Cul_SF_DynVolYaxis))
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,
            pst_SI->f_DeltaFar,
			C_infinite,
			C_infinite,
			C_infinite,
			pst_SI->af_Near[1],
			pst_SI->af_Middle[1],
			pst_SI->af_Far[1],
			pst_SI->af_Near[2],
			pst_SI->af_Middle[2],
			pst_SI->af_Far[2]
		);
	}
	else if(ulFlags == (SND_Cul_SF_DynVolZaxis | SND_Cul_SF_DynVolXaxis))
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,
            pst_SI->f_DeltaFar,
			pst_SI->af_Near[0],
			pst_SI->af_Middle[0],
			pst_SI->af_Far[0],
			C_infinite,
			C_infinite,
			C_infinite,
			pst_SI->af_Near[2],
			pst_SI->af_Middle[2],
			pst_SI->af_Far[2]
		);
	}
    else if(ulFlags == SND_Cul_SF_DynVolXaxis)
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,
            pst_SI->f_DeltaFar,
			pst_SI->af_Near[0],
			pst_SI->af_Middle[0],
			pst_SI->af_Far[0],
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite
		);
	}
    else if(ulFlags == SND_Cul_SF_DynVolYaxis)
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,
            pst_SI->f_DeltaFar,
			C_infinite,
			C_infinite,
			C_infinite,
			pst_SI->af_Near[1],
            pst_SI->af_Middle[1],
			pst_SI->af_Far[1],
			C_infinite,
			C_infinite,
			C_infinite
		);
	}
    else if(ulFlags == SND_Cul_SF_DynVolZaxis)
	{
		GEO_DrawSoundBox
		(
			_pst_DD,
			&st_NoScaleMatrix,
            pst_SI->f_DeltaFar,
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite,
			C_infinite,
			pst_SI->af_Near[2],
			pst_SI->af_Middle[2],
			pst_SI->af_Far[2]
		);
	}

	_pst_DD->ul_ColorConstant = ul_SaveColor;
}


static void GEO_DrawSoundCapsule(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Matrix*pMatrix, ULONG ulAxis, float fScale, float fRadius, ULONG ulColor)
{
	GDI_tdst_Request_DrawCylinder	st_Cyl;
    MATH_tdst_Vector stNormAxis;
    MATH_tdst_Vector stPos;
    
    if(!fScale) fScale = 0.0000001f;

    if(ulAxis & SND_Cul_SF_DynVolXaxis)
        MATH_CopyVector(&stPos, MATH_pst_GetXAxis(pMatrix));
    else if(ulAxis & SND_Cul_SF_DynVolYaxis)
        MATH_CopyVector(&stPos, MATH_pst_GetYAxis(pMatrix));
    else if(ulAxis & SND_Cul_SF_DynVolZaxis)
        MATH_CopyVector(&stPos, MATH_pst_GetZAxis(pMatrix));
    else
    {
        fScale = 0.0000001f;
        MATH_CopyVector(&stPos, MATH_pst_GetXAxis(pMatrix));
    }

    MATH_NormalizeAnyVector(&stNormAxis, &stPos);

    MATH_ScaleEqualVector(&stNormAxis, fScale);
    MATH_SubVector(&stPos, MATH_pst_GetTranslation(pMatrix), &stNormAxis);
    MATH_ScaleEqualVector(&stNormAxis, 2.0f);
    
    _pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	_pst_DD->st_GDI.pfnv_SetViewMatrix(_pst_DD->st_Camera.pst_ObjectToCameraMatrix);

    st_Cyl.pst_Pos = &stPos;
	st_Cyl.pst_Axe = &stNormAxis;
	st_Cyl.ul_SolidColor = 0x3F000000 | (0x00FFFFFF & ulColor);
	st_Cyl.f_Radius = fRadius;
	st_Cyl.ul_EdgeColor =  0xFF000000 | ulColor;	
	st_Cyl.f_LineWidth = 2;							// grosseur des edge
	st_Cyl.ul_Flags = GDI_Request_DrawCylinder_Line|GDI_Request_DrawCylinder_Capsule|GDI_Request_DrawCylinder_NoZWrite;
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawCylinder, (ULONG) &st_Cyl );    
}
#ifdef JADEFUSION
void GEO_ModifierSoftBody_Display(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO,
                                  MDF_tdst_Modifier *pst_MdF)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG					  ul_SaveColor;
    ULONG                     ul_PlaneColor;
    ULONG                     ul_NormalColor;
    MATH_tdst_Matrix		  st_NoScaleMatrix;
    MATH_tdst_Matrix		  st_BoxMatrix;
    GAO_tdst_ModifierSoftBody * pst_SoftBody;
    BOOL                      b_Selected;
    MATH_tdst_Vector          st_Min;
    MATH_tdst_Vector          st_Max;
    CPlane                  * p_Plane;
    MATH_tdst_Vector          st_Normal;
    MATH_tdst_Vector          st_Point;
    MATH_tdst_Vector          st_PointNormal;	
    GDI_tdst_Request_DrawLineEx	st_DrawNormal;

    //MATH_tdst_Vector          *st_SphereCenter;
    //MATH_tdst_Matrix			st_SphereMatrix;
    //GDI_tdst_Request_DrawLineEx	st_DrawForce,st_DrawOffset;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Selected = (SEL_RetrieveItem(_pst_GO->pst_World->pst_Selection, _pst_GO) != NULL);

    if (!SoftBody_gb_DisplayPlaneCollision)
        return;

    ul_SaveColor = _pst_DD->ul_ColorConstant;

    pst_SoftBody = (GAO_tdst_ModifierSoftBody *) pst_MdF->p_Data;

    if(b_Selected) 
    {
        ul_PlaneColor = GEO_Cul_Blue;
        ul_NormalColor = GEO_Cul_Red;
    } 
    else
    {
        //// Not Selected
        //if (MDF_bg_DisplayOnlySelected)
        //    return;

        ul_PlaneColor = GEO_Cul_WhiteBlue;
        ul_NormalColor = GEO_Cul_WhiteRed;
    }

    MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
    MATH_ClearScale(&st_NoScaleMatrix, 1);
    SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_NoScaleMatrix);	

    MATH_InitVector(&st_Min, -4.0f, -0.01f, -4.0f);
    MATH_InitVector(&st_Max,  4.0f,  0.01f,  4.0f);

    for(ULONG i=0; i<pst_SoftBody->ul_NbCollPlane; i++)
    { 
        p_Plane = &(pst_SoftBody->a_CollPlane[i].m_LocalPlane);

        MATH_InitVector(&st_Normal, p_Plane->X, p_Plane->Y, p_Plane->Z);
        MATH_ScaleVector(&st_Point, &st_Normal , p_Plane->W);

        MATH_AddVector(&st_PointNormal, &st_Point, &st_Normal);

        MATH_SetIdentityMatrix(&st_BoxMatrix);
        MATH_CopyVector(&st_BoxMatrix.T, &st_Point);

        if (fAbs(p_Plane->X) > fAbs(p_Plane->Y))
        {
            if (fAbs(p_Plane->Y) > fAbs(p_Plane->Z))
                MATH_OrientMatrix_UsingSight(&st_BoxMatrix, &st_Normal, &MATH_gst_BaseVectorK);
            else
                MATH_OrientMatrix_UsingSight(&st_BoxMatrix, &st_Normal, &MATH_gst_BaseVectorJ);
        }
        else if (fAbs(p_Plane->X) > fAbs(p_Plane->Z))
            MATH_OrientMatrix_UsingSight(&st_BoxMatrix, &st_Normal, &MATH_gst_BaseVectorK);
        else
            MATH_OrientMatrix_UsingSight(&st_BoxMatrix, &st_Normal, &MATH_gst_BaseVectorI);

        SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_BoxMatrix);
        GEO_DebugObject_PushBoxMatrix(&_pst_DD->st_MatrixStack, &st_Min, &st_Max);

        _pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
        GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

        _pst_DD->ul_ColorConstant = ul_PlaneColor;
        GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Box, GEO_Cul_MdF_DrawMask | GDI_Cul_DM_NotWired, 0, NULL);

        GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);
        GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);

        GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

        st_DrawNormal.A = &st_Point;
        st_DrawNormal.B = &st_PointNormal;
        st_DrawNormal.ul_Color = ul_NormalColor;
        st_DrawNormal.f_Width = 1.0f;

        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&st_DrawNormal);
    }

    SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);

    // debug pendulum

    // draw anchor	
    for(int i=0;i<3+pst_SoftBody->abovePoints.size()+pst_SoftBody->tearingVertices.size();i++)
    {
        CSoftBodyVertex *v;
        MATH_tdst_Vector *vertex;
        if(i==0)
        {
            v = pst_SoftBody->pst_AnchorPoint;
            if(v == NULL)
                break;

            vertex = v->GetCurrPos();
            _pst_DD->ul_ColorConstant = GEO_Cul_Yellow;
        }
        else if(i==1)
        {
            v = pst_SoftBody->pst_StartPoint;
            if(v == NULL)
                break;

            vertex = v->GetCurrPos();

            _pst_DD->ul_ColorConstant = GEO_Cul_White;
        }
        else if(i==2)
        {
            vertex = &pst_SoftBody->st_TearingPosition;
            _pst_DD->ul_ColorConstant = GEO_Cul_Red;
        }
        else if(i>2 && i <(3+pst_SoftBody->abovePoints.size()))
        {
            vertex = pst_SoftBody->abovePoints[i-3]->GetCurrPos();
            _pst_DD->ul_ColorConstant = GEO_Cul_Blue;
        }
        else if(i>2+pst_SoftBody->abovePoints.size())
        {
            vertex = &pst_SoftBody->tearingVertices[i-(pst_SoftBody->abovePoints.size()+3)];
            _pst_DD->ul_ColorConstant = GEO_Cul_Red;
        }


        // local position 
        MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
        MATH_ClearScale(&st_NoScaleMatrix, 1);
        SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_NoScaleMatrix);

        // draw active sphere 
        GEO_DebugObject_PushSphereMatrix(&_pst_DD->st_MatrixStack, vertex, 0.02f);
        _pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
        GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);


        GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Sphere, GEO_Cul_MdF_DrawMask, 0, NULL);
        GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);

        SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);
    }

    // draw tearing position


    // debug spring code
    /*
    // draw spheres around the springs vertex
    for(ULONG i=0; i<10; i++)
    {
    CSoftBodySpring *s = &pst_SoftBody->springs[i];
    ULONG vertexID = s->m_vertexID;
    if(vertexID != -1)
    {
    // get position of vertex
    CSoftBodyVertex *v = &pst_SoftBody->a_Vertices[vertexID];  
    OBJ_tdst_GameObject *bone = s->GetBoneGao(pst_SoftBody);

    if(v != NULL && bone != NULL)
    {
    st_SphereCenter = v->GetCurrPos();

    // local position 
    MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
    MATH_ClearScale(&st_NoScaleMatrix, 1);
    SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_NoScaleMatrix);

    // draw active sphere 
    GEO_DebugObject_PushSphereMatrix(&_pst_DD->st_MatrixStack, st_SphereCenter, 0.02f);
    _pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
    GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

    _pst_DD->ul_ColorConstant = GEO_Cul_Blue;
    GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Sphere, GEO_Cul_MdF_DrawMask, 0, NULL);
    GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);

    SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);


    _pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
    GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

    // draw force applied to the vertex
    MATH_tdst_Vector vs,vt;				
    MATH_TransformVertex(&vs,OBJ_pst_GetAbsoluteMatrix(_pst_GO),st_SphereCenter);
    st_DrawForce.A = &vs;				
    MATH_CopyVector(&vt,st_DrawForce.A);
    st_DrawForce.B = &vt;
    MATH_AddEqualVector(st_DrawForce.B,&v->m_Accel);

    st_DrawForce.ul_Color = GEO_Cul_Blue;
    st_DrawForce.f_Width = 2.0f;

    GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&st_DrawForce);	


    // draw bone offsets
    MATH_tdst_Vector vu;
    st_DrawOffset.A = &OBJ_pst_GetAbsoluteMatrix(bone)->T;
    st_DrawOffset.B = &vu;
    MATH_TransformVertex(st_DrawOffset.B,OBJ_pst_GetAbsoluteMatrix(bone),&s->m_boneOffset);				

    st_DrawOffset.ul_Color = ul_NormalColor;
    st_DrawOffset.f_Width = 2.0f;

    GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&st_DrawOffset);				
    }
    }
    }*/



    _pst_DD->ul_ColorConstant = ul_SaveColor;
}

void GEO_MDFWind_Display(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *pst_MdF)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG					  ul_SaveColor;
    ULONG                     ul_PlaneColor;
    MATH_tdst_Matrix          st_NoScaleMatrix;
    MATH_tdst_Matrix		  st_PlaneMat;
    GAO_tdst_ModifierWind *   pst_Wind;
    BOOL                      b_Selected;
    FLOAT                     f_Dx, f_Dz;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Selected = (SEL_RetrieveItem(_pst_GO->pst_World->pst_Selection, _pst_GO) != NULL);

    if (!Wind_gb_Display)
        return;

    ul_SaveColor = _pst_DD->ul_ColorConstant;

    pst_Wind = (GAO_tdst_ModifierWind *) pst_MdF->p_Data;

    if (pst_Wind->po_Source == NULL)
        return;

    if(b_Selected) 
    {
        ul_PlaneColor = GEO_Cul_FxRedS;
    } 
    else
    {
        //// Not Selected
        //if (MDF_bg_DisplayOnlySelected)
        //    return;

        ul_PlaneColor = GEO_Cul_SpecialFxRedS;
    }

    pst_Wind->po_Source->GetSizePlane(f_Dx, f_Dz);

    MATH_CopyMatrix(&st_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(pst_MdF->pst_GO));
    MATH_ClearScale(&st_NoScaleMatrix, 1);

    MATH_CopyMatrix(&st_PlaneMat, &MATH_gst_IdentityMatrix);
    MATH_SetScaleType(&st_PlaneMat);
    MATH_SetTranslationType(&st_PlaneMat);
    MATH_InitVector(&st_PlaneMat.T, 0.0f, -0.25, 0.0f);

    st_PlaneMat.Sx = 0.5f * f_Dx;
    st_PlaneMat.Sy = 1.0f;
    st_PlaneMat.Sz = 0.5f * f_Dz;

    SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_NoScaleMatrix);
    SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, &st_PlaneMat);

    _pst_DD->ul_ColorConstant = ul_PlaneColor;
    _pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
    GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

    GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Portal, GEO_Cul_MdF_DrawMask | GDI_Cul_DM_NotWired, 0, NULL);

    SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);
    SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);

    GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

    _pst_DD->ul_ColorConstant = ul_SaveColor;
}
#endif

#undef C_infinite
#endif
