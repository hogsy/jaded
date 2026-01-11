/*$T MDFmodifier_SDW.c GC! */


/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_SDW.h"
#include "GEOmetric/GEOobject.h"
#include "MATerial/MATShadow.h"
#include "GraphicDK/Sources/MATerial/MATShadow.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

static GAO_tdst_ModifierSDW *pst_FirstShadowMdf = NULL;
/*$4
***********************************************************************************************************************
Shadow modifier :
Modifier apply shadow on world
***********************************************************************************************************************
*/

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierShadow_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierSDW));

   if (p_Data == NULL)
   {
	    L_memset(_pst_Mod->p_Data , 0 , sizeof(GAO_tdst_ModifierSDW));

		((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->uc_Version = 1;

#if !defined(XML_CONV_TOOL)
	    ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->TextureUsed  = (ULONG)-1;
	    ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->TextureIndex = (LONG)-1;
	    ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->ulFlags	 = MDF_SDW_DefaulFlag;
	    ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->XSizeFactor = 1.0f;
	    ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->YSizeFactor = 1.0f;
	    ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->ZSizeFactor = 1.0f;
	    ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->ZAttenuationFactor = 2.0f;
	    ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->ulShadowColor = 0;
#endif
   }
   else
   {
       L_memcpy( _pst_Mod->p_Data, p_Data, sizeof (GAO_tdst_ModifierSDW) );
   }
   ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->pst_GO = _pst_GO;
   ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->p_NextSdwMdf = pst_FirstShadowMdf;
	pst_FirstShadowMdf = (GAO_tdst_ModifierSDW *)_pst_Mod->p_Data;
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierShadow_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierSDW **ppst_ParseShadowMdf,*p_SDW;
	ppst_ParseShadowMdf = &pst_FirstShadowMdf;
	p_SDW = (GAO_tdst_ModifierSDW *)_pst_Mod->p_Data;
	if (*ppst_ParseShadowMdf)
	{
		while ((*ppst_ParseShadowMdf) != p_SDW)
		{
			ppst_ParseShadowMdf = &(*ppst_ParseShadowMdf) -> p_NextSdwMdf;
		}
		(*ppst_ParseShadowMdf) = (*ppst_ParseShadowMdf) -> p_NextSdwMdf;
	}
	MEM_Free(_pst_Mod->p_Data);
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierShadow_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/* nothing on s'en fout */
}

/*
=======================================================================================================================
=======================================================================================================================
*/
extern ULONG LIGHT_ul_Interpol2Colors(ULONG ulP1, ULONG ulP2, float fZClipLocalCoef);
void GAO_ModifierShadow_Apply_All()
{
	GAO_tdst_ModifierSDW *p_SDW;
	ULONG				ulLightCounter;
	SDW_Clear();

	p_SDW = pst_FirstShadowMdf;
	while (p_SDW)
	{
		DD_tdst_ShadowStackNode stNewShadow;
		if (OBJ_b_TestStatusFlag(p_SDW->pst_GO, OBJ_C_StatusFlag_Active))
		{
			if (OBJ_b_TestStatusFlag(p_SDW->pst_GO, OBJ_C_StatusFlag_Visible))
			{
				if (p_SDW -> ulFlags & MDF_SDW_IsActivate)
				{
					/* Add A shadow in the list */
					/* Init It */
					stNewShadow.fStartFactor = 1.f;
					if (p_SDW -> ulFlags & MDF_SDW_1Vertical)
					{
						stNewShadow.p_FatherModifier = p_SDW;
						stNewShadow.fZDepth = stNewShadow.p_FatherModifier->ZAttenuationFactor;
						MATH_InitVector(&stNewShadow.stWorldVectShadow,0.0f,0.0f,-1.0f);
						stNewShadow.ulColor = p_SDW -> ulShadowColor;
						SDW_AddAShadow(&stNewShadow);
					}
					if (p_SDW -> ulFlags & MDF_SDW_1ObjectX)
					{
						stNewShadow.p_FatherModifier = p_SDW;
						stNewShadow.fZDepth = stNewShadow.p_FatherModifier->ZAttenuationFactor;
						MATH_CopyVector(&stNewShadow.stWorldVectShadow , MATH_pst_GetXAxis(p_SDW->pst_GO->pst_GlobalMatrix));
						stNewShadow.ulColor = p_SDW -> ulShadowColor;
						if (p_SDW -> ulFlags & MDF_SDW_InvertDirection)
							MATH_NegEqualVector(&stNewShadow.stWorldVectShadow );
						SDW_AddAShadow(&stNewShadow);
					}
					if (p_SDW -> ulFlags & MDF_SDW_1ObjectY)
					{
						stNewShadow.p_FatherModifier = p_SDW;
						stNewShadow.fZDepth = stNewShadow.p_FatherModifier->ZAttenuationFactor;
						MATH_CopyVector(&stNewShadow.stWorldVectShadow , MATH_pst_GetYAxis(p_SDW->pst_GO->pst_GlobalMatrix));
						stNewShadow.ulColor = p_SDW -> ulShadowColor;
						if (p_SDW -> ulFlags & MDF_SDW_InvertDirection)
							MATH_NegEqualVector(&stNewShadow.stWorldVectShadow );
						SDW_AddAShadow(&stNewShadow);
					}
					if (p_SDW -> ulFlags & MDF_SDW_1ObjectZ)
					{
						stNewShadow.p_FatherModifier = p_SDW;
						stNewShadow.fZDepth = stNewShadow.p_FatherModifier->ZAttenuationFactor;
						MATH_CopyVector(&stNewShadow.stWorldVectShadow , MATH_pst_GetZAxis(p_SDW->pst_GO->pst_GlobalMatrix));
						stNewShadow.ulColor = p_SDW -> ulShadowColor;
						if (p_SDW -> ulFlags & MDF_SDW_InvertDirection)
							MATH_NegEqualVector(&stNewShadow.stWorldVectShadow );
						SDW_AddAShadow(&stNewShadow);
					}
					if (p_SDW -> ulFlags & (MDF_SDW_1PerLight | MDF_SDW_1PerDirectional)) /* one for each non - directional */
					{
						ulLightCounter = GDI_gpst_CurDD ->st_LightList.ul_Current;
						stNewShadow.p_FatherModifier = p_SDW;
						while (ulLightCounter--)
						{
							LIGHT_tdst_Light	*st_CurLight;
							OBJ_tdst_GameObject	*st_CurObject;
							st_CurObject = GDI_gpst_CurDD ->st_LightList.dpst_Light[ulLightCounter];


							if(p_SDW->pst_LightGO && (p_SDW->pst_LightGO != st_CurObject))
								continue;


							st_CurLight = (LIGHT_tdst_Light *) st_CurObject->pst_Extended->pst_Light;
							if (p_SDW -> ulFlags & MDF_SDW_AlwaysUseShadowColor)
								stNewShadow.ulColor = p_SDW -> ulShadowColor;
							else
								stNewShadow.ulColor = st_CurLight ->ul_Color;//p_SDW -> ulShadowColor;
							stNewShadow.fZDepth = stNewShadow.p_FatherModifier->ZAttenuationFactor;
							if (stNewShadow.ulColor & 0xf0f0f0)
							{
								if ((st_CurLight->ul_Flags & LIGHT_Cul_LF_EmitRTShadows) && (st_CurLight->ul_Flags & LIGHT_Cul_LF_Active))
								{
									switch(st_CurLight->ul_Flags & LIGHT_Cul_LF_Type)
									{
									case LIGHT_Cul_LF_Omni:
										if (p_SDW -> ulFlags & MDF_SDW_1PerLight)
										{
											float fDistance;
											/* Light culling is done Here */
											MATH_SubVector(&stNewShadow.stWorldVectShadow ,&st_CurObject->pst_GlobalMatrix->T , &p_SDW->pst_GO->pst_GlobalMatrix->T);
											fDistance = MATH_f_NormVector(&stNewShadow.stWorldVectShadow);
											if (fDistance < st_CurLight->st_Omni.f_Far)
											{
												if (fDistance > st_CurLight->st_Omni.f_Near)
												{
													stNewShadow.fStartFactor = (fDistance - st_CurLight->st_Omni.f_Near) / (st_CurLight->st_Omni.f_Far - st_CurLight->st_Omni.f_Near);
													stNewShadow.ulColor = LIGHT_ul_Interpol2Colors(st_CurLight ->ul_Color, 0, stNewShadow.fStartFactor);
													stNewShadow.fStartFactor *= stNewShadow.fStartFactor;
													stNewShadow.fStartFactor = 1.0f - stNewShadow.fStartFactor;
												}
												if (stNewShadow.fZDepth > st_CurLight->st_Omni.f_Far - fDistance )
													stNewShadow.fZDepth = st_CurLight->st_Omni.f_Far - fDistance;
												stNewShadow.ulColor ^= 0xffffff;
												MATH_NegVector(&stNewShadow.stWorldVectShadow, &stNewShadow.stWorldVectShadow);
												MATH_NormalizeVector(&stNewShadow.stWorldVectShadow, &stNewShadow.stWorldVectShadow);
												if (p_SDW -> ulFlags & MDF_SDW_InvertDirection)
													MATH_NegEqualVector(&stNewShadow.stWorldVectShadow );
												SDW_AddAShadow(&stNewShadow);
												stNewShadow.fStartFactor = 1.f;
											}
										}
										break;
									case LIGHT_Cul_LF_Spot:
										if (p_SDW -> ulFlags & MDF_SDW_1PerLight)
										{
											float fDistance;
											/* Light culling is done Here */
											MATH_SubVector(&stNewShadow.stWorldVectShadow ,&st_CurObject->pst_GlobalMatrix->T , &p_SDW->pst_GO->pst_GlobalMatrix->T);
											fDistance = MATH_f_NormVector(&stNewShadow.stWorldVectShadow);
											if (fDistance < st_CurLight->st_Omni.f_Far)
											{
												if (fDistance > st_CurLight->st_Omni.f_Near)
												{
													stNewShadow.ulColor = LIGHT_ul_Interpol2Colors(st_CurLight ->ul_Color, 0, (fDistance - st_CurLight->st_Omni.f_Near) / (st_CurLight->st_Omni.f_Far - st_CurLight->st_Omni.f_Near));
												}
												if (stNewShadow.fZDepth > st_CurLight->st_Omni.f_Far - fDistance )
													stNewShadow.fZDepth = st_CurLight->st_Omni.f_Far - fDistance;
												stNewShadow.ulColor ^= 0xffffff;
												MATH_SubVector(&stNewShadow.stWorldVectShadow ,&st_CurObject->pst_GlobalMatrix->T , &p_SDW->pst_GO->pst_GlobalMatrix->T);
												MATH_NegVector(&stNewShadow.stWorldVectShadow, &stNewShadow.stWorldVectShadow);
												MATH_NormalizeVector(&stNewShadow.stWorldVectShadow, &stNewShadow.stWorldVectShadow);
												if (p_SDW -> ulFlags & MDF_SDW_InvertDirection)
													MATH_NegEqualVector(&stNewShadow.stWorldVectShadow );
												SDW_AddAShadow(&stNewShadow);
											}
										}
										break;
									case LIGHT_Cul_LF_Direct:
										if (p_SDW -> ulFlags & MDF_SDW_1PerDirectional)
										{
											stNewShadow.ulColor ^= 0xffffff;
											MATH_CopyVector(&stNewShadow.stWorldVectShadow , MATH_pst_GetYAxis(st_CurObject->pst_GlobalMatrix));
											MATH_NegVector(&stNewShadow.stWorldVectShadow, &stNewShadow.stWorldVectShadow);
											MATH_NormalizeVector(&stNewShadow.stWorldVectShadow, &stNewShadow.stWorldVectShadow);
											if (p_SDW -> ulFlags & MDF_SDW_InvertDirection)
												MATH_NegEqualVector(&stNewShadow.stWorldVectShadow );
											SDW_AddAShadow(&stNewShadow);
										}
										break;
									}
								}
							}
						}
					}
			}
		}
		}
		p_SDW = p_SDW ->p_NextSdwMdf;
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierShadow_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierShadow_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierShadow_DetectUsedTextures(char *pc_UsedIndex)
{
	GAO_tdst_ModifierSDW *ppst_ParseShadowMdf;
	ppst_ParseShadowMdf = pst_FirstShadowMdf;
	while (ppst_ParseShadowMdf)
	{
		if (ppst_ParseShadowMdf->TextureIndex != (LONG)0xffffffff)
		{
			pc_UsedIndex[ppst_ParseShadowMdf->TextureIndex] = 1;
		}
		ppst_ParseShadowMdf = ppst_ParseShadowMdf -> p_NextSdwMdf;
	}
	
}
/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG GAO_ModifierShadow_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	GAO_tdst_ModifierSDW *p_SDW,*p_NextOne;
	TEX_tdst_Data        *pst_Texture;
	OBJ_tdst_GameObject *_pst_GO;
	char * pc_Buf;
	
	pc_Buf = _pc_Buffer;
	LOA_ReadLong_Ed(&pc_Buf, NULL); // skip the size
	p_SDW = (GAO_tdst_ModifierSDW*)_pst_Mod->p_Data;
	p_NextOne = p_SDW->p_NextSdwMdf;
	_pst_GO = p_SDW->pst_GO;
	((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data) ->pst_GO = _pst_GO;
	p_SDW->ulFlags = LOA_ReadULong(&pc_Buf);
	p_SDW->TextureUsed = LOA_ReadULong(&pc_Buf);
	p_SDW->XSizeFactor = LOA_ReadFloat(&pc_Buf);
	p_SDW->YSizeFactor = LOA_ReadFloat(&pc_Buf);
	p_SDW->TextureIndex = LOA_ReadULong(&pc_Buf);
	LOA_ReadULong_Ed(&pc_Buf, (ULONG *) &p_SDW->p_NextSdwMdf);
	LOA_ReadULong_Ed(&pc_Buf, (ULONG *) &p_SDW->pst_GO);
	p_SDW->ZAttenuationFactor = LOA_ReadFloat(&pc_Buf);
	p_SDW->ulShadowColor = LOA_ReadULong(&pc_Buf);
	p_SDW->ZStart = LOA_ReadFloat(&pc_Buf);
	LOA_ReadVector(&pc_Buf, &p_SDW->stCenter);
	p_SDW->ZSizeFactor = LOA_ReadFloat(&pc_Buf);
	p_SDW->ulProjectionMethod = LOA_ReadULong(&pc_Buf);
	p_SDW->TextureTiling = LOA_ReadChar(&pc_Buf);

	p_SDW->uc_Version = LOA_ReadChar(&pc_Buf);

	if(p_SDW->uc_Version == 0)
	{
		p_SDW->uw_Dummy = LOA_ReadUShort(&pc_Buf);
	}
	else
	{
		p_SDW->uw_Dummy = LOA_ReadUShort(&pc_Buf);

		p_SDW->pst_LightGO = (OBJ_tdst_GameObject *) LOA_ReadULong(&pc_Buf);


		if((ULONG) p_SDW->pst_LightGO && (ULONG) p_SDW->pst_LightGO != BIG_C_InvalidKey)
		{
			LOA_MakeFileRef
			(
				(ULONG) p_SDW->pst_LightGO,
				(ULONG *) &p_SDW->pst_LightGO,
				OBJ_ul_GameObjectCallback,
				LOA_C_MustExists
			);
		}
		else
			p_SDW->pst_LightGO = NULL;

	}

#if !defined(XML_CONV_TOOL)
	p_SDW->p_NextSdwMdf = p_NextOne;
	p_SDW->pst_GO = _pst_GO;
	p_SDW -> ulFlags &= ~MDF_SDW_IsUpdatedOnThisDisplayData;
#if defined(PSX2_TARGET) || defined(_GAMECUBE)
	if (p_SDW -> ulFlags & MDF_SDW_1RealTime) 
	{
		p_SDW -> stCenter = (MATH_tdst_Vector) {0.0f,0.0f,0.0f};
		p_SDW -> XSizeFactor = p_SDW -> YSizeFactor = 1.0f;
		
	}
#endif	
	
	/* Load each texture */
	p_SDW->TextureIndex = -1;
	if (p_SDW->TextureUsed != 0)
		if (p_SDW->TextureUsed != 0xffffffff)
		{
			pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, p_SDW->TextureUsed);
			if(pst_Texture)
				p_SDW->TextureIndex = (LONG)pst_Texture->w_Index;
			else
			{
				p_SDW->TextureIndex = TEX_w_List_AddTexture
					(
					&TEX_gst_GlobalList,
					p_SDW->TextureUsed, 1
					);
			}
		}
#endif
		//*/
		return (pc_Buf - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS
/*
=======================================================================================================================
=======================================================================================================================
	*/
#ifdef JADEFUSION
void GAO_ModifierShadow_Save(MDF_tdst_Modifier *_pst_Mod)
{
	ULONG		ulSize, ul_Mem;
	BIG_KEY		ul_Key;
	GAO_tdst_ModifierSDW* pData = (GAO_tdst_ModifierSDW *)_pst_Mod->p_Data;
	ulSize = sizeof(GAO_tdst_ModifierSDW);

#if defined(XML_CONV_TOOL)
	if (pData->uc_Version == 0)
		ulSize -= 4; // do not save LightGO
#else
	pData->uc_Version = 1;
#endif

	SAV_Buffer(&ulSize, 4); 
	ul_Mem = (ULONG) pData->pst_LightGO;
	if(ul_Mem)
		ul_Key = LOA_ul_SearchKeyWithAddress(ul_Mem);
	else
		ul_Key = BIG_C_InvalidKey;
	pData->pst_LightGO = (OBJ_tdst_GameObject *)ul_Key;
	SAV_Buffer(pData, ulSize); 
	pData->pst_LightGO = (OBJ_tdst_GameObject *)ul_Mem;
}
#else
	void GAO_ModifierShadow_Save(MDF_tdst_Modifier *_pst_Mod)
	{
		ULONG		ulSize, ul_Mem;
		BIG_KEY		ul_Key;
		ulSize = sizeof(GAO_tdst_ModifierSDW);
		SAV_Buffer(&ulSize, 4); 

		((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data)->uc_Version = 1;

		ul_Mem = (ULONG) ((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data)->pst_LightGO;

		if(ul_Mem)
			ul_Key = LOA_ul_SearchKeyWithAddress(ul_Mem);
		else
			ul_Key = BIG_C_InvalidKey;

		((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data)->pst_LightGO = (OBJ_tdst_GameObject *)ul_Key;

		SAV_Buffer(_pst_Mod->p_Data, sizeof(GAO_tdst_ModifierSDW)); 

		((GAO_tdst_ModifierSDW *)_pst_Mod->p_Data)->pst_LightGO = (OBJ_tdst_GameObject *)ul_Mem;

	}
#endif	
#endif
	
	
	
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
