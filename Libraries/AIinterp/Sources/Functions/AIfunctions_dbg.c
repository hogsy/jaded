/*$T AIfunctions_dbg.c GC! 1.081 07/23/02 14:54:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

extern void LINK_PrintStatusMsgEOLCanal(char *,int);

#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "LINks/LINKmsg.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIdebug.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "ENGine/Sources/TEXT/TEXT.h"

#ifdef ACTIVE_EDITORS
#include "EDItors/Sources/LOGfile/LOGmsg.h"
#include "LINKs/LINKtoed.h"
#endif

#include "BASe/BENch/BENch.h"
#ifdef PSX2_TARGET
#ifndef _FINAL_
#define DEFINE_RASTER(a,b,c,d)  
#include "BAse/BENch/BENch_RasterDef.h"
#undef DEFINE_RASTER
#endif
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$2- undef switch and functions -------------------------------------------------------------------------------------*/

#ifdef ENABLE_AI_DBG_MESSAGE
#undef ENABLE_AI_DBG_MESSAGE
#endif
#ifdef M_Print
#undef M_Print
#endif
#ifdef M_Print3
#undef M_Print3
#endif

/*$2- define among the config ----------------------------------------------------------------------------------------*/

#ifdef ACTIVE_EDITORS
#define ENABLE_AI_DBG_MESSAGE
#endif

/*$2- define display fonction ----------------------------------------------------------------------------------------*/

#ifdef ENABLE_AI_DBG_MESSAGE
/**/
#ifdef PSX2_TARGET
#define M_Print(__format, __val,__Canal)					printf("[DBG] "__format "\n", __val)
#define M_Print3(__format, __val1, __val2, __val3,__Canal)	printf("[DBG] "__format "\n", __val1, __val2, __val3)
/**/
#elif defined(_GAMECUBE)
#define M_Print(__format, __val,__Canal)					OSReport("[DBG] "__format "\n", __val)
#define M_Print3(__format, __val1, __val2, __val3,__Canal)	OSReport("[DBG] "__format "\n", __val1, __val2, __val3)
/**/
#elif defined(ACTIVE_EDITORS)
#define M_Print(__format, __val,__canal) \
	do \
	{ \
		char	asz_Temp[500]; \
		sprintf(asz_Temp, __format, __val); \
		LINK_gb_CanAbort = FALSE; \
		LINK_PrintStatusMsgEOLCanal(asz_Temp,__canal); \
	} while(0);
#define M_Print3(__format, __val1, __val2, __val3,__canal) \
	do \
	{ \
		char	asz_Temp[500]; \
		sprintf(asz_Temp, __format, __val1, __val2, __val3); \
		LINK_gb_CanAbort = FALSE; \
		LINK_PrintStatusMsgEOLCanal(asz_Temp,__canal); \
	} while(0);
/**/
#else
#define M_Print(__format, __val,__canal)					(0)
#define M_Print3(__format, __val1, __val2, __val3,__canal)	(0)
#endif
/**/
#else
#define M_Print(__format, __val,__canal)					(0)
#define M_Print3(__format, __val1, __val2, __val3,__canal)	(0)
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgTraceIntCanal_C(int _i_Val,int _iCanal)
{
	M_Print("%d", _i_Val,_iCanal);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceIntCanal(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_DbgTraceIntCanal_C(AI_PopInt(),AI_PopInt());
	return ++_pst_Node;
}
/**/
void AI_EvalFunc_DbgTraceInt_C(int _i_Val)
{
    AI_EvalFunc_DbgTraceIntCanal_C(_i_Val,0);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceInt(AI_tdst_Node *_pst_Node)
{
    AI_PushInt(0);
    return AI_EvalFunc_DbgTraceIntCanal(_pst_Node);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgTraceEOLCanal_C(int _iCanal)
{
	M_Print("\\n", 0,_iCanal);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceEOLCanal(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_DbgTraceEOLCanal_C(AI_PopInt());
	return ++_pst_Node;
}
/**/
void AI_EvalFunc_DbgTraceEOL_C(void)
{
    AI_EvalFunc_DbgTraceEOLCanal_C(0);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceEOL(AI_tdst_Node *_pst_Node)
{
    AI_PushInt(0);
    return AI_EvalFunc_DbgTraceEOLCanal(_pst_Node);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgTraceCharCanal_C(int _c_Val,int _iCanal)
{
	M_Print("%d", _c_Val,_iCanal);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceCharCanal(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_DbgTraceCharCanal_C((int) AI_PopChar(),AI_PopInt());
	return ++_pst_Node;
}
/**/
void AI_EvalFunc_DbgTraceChar_C(int _c_Val)
{
    AI_EvalFunc_DbgTraceCharCanal_C(_c_Val,0);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceChar(AI_tdst_Node *_pst_Node)
{
    AI_PushInt(0);
    return AI_EvalFunc_DbgTraceCharCanal(_pst_Node);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgTraceFloatCanal_C(float _f_Val,int _iCanal)
{
	M_Print("%f", _f_Val,_iCanal);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceFloatCanal(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_DbgTraceFloatCanal_C(AI_PopFloat(),AI_PopInt());
	return ++_pst_Node;
}
/**/
void AI_EvalFunc_DbgTraceFloat_C(float _f_Val)
{
    AI_EvalFunc_DbgTraceFloatCanal_C(_f_Val,0);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceFloat(AI_tdst_Node *_pst_Node)
{
    AI_PushInt(0);
    return AI_EvalFunc_DbgTraceFloatCanal(_pst_Node);
}
/**/
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgTraceVectorCanal_C(MATH_tdst_Vector *_pst_Vec,int _iCanal)
{
	M_Print3("(%.5f, %.5f, %.5f)", _pst_Vec->x, _pst_Vec->y, _pst_Vec->z,_iCanal);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceVectorCanal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
    int iCanal = AI_PopInt();
	/*~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&v);
	AI_EvalFunc_DbgTraceVectorCanal_C(&v,iCanal);
	return ++_pst_Node;
}
/**/
void AI_EvalFunc_DbgTraceVector_C(MATH_tdst_Vector *_pst_Vec)
{
    AI_EvalFunc_DbgTraceVectorCanal_C(_pst_Vec,0);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceVector(AI_tdst_Node *_pst_Node)
{
    AI_PushInt(0);
    return AI_EvalFunc_DbgTraceVectorCanal(_pst_Node);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgTraceStringCanal_C(char *_psz_Str,int _iCanal)
{
	M_Print("%s", _psz_Str,_iCanal);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceStringCanal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
    int iCanal = AI_PopInt();
	char *p = (char *) AI_PopStringPtr();
	/*~~~~~~~*/

	AI_EvalFunc_DbgTraceStringCanal_C(p,iCanal);
	return ++_pst_Node;
}
/**/
void AI_EvalFunc_DbgTraceString_C(char *_psz_Str)
{
	AI_EvalFunc_DbgTraceStringCanal_C(_psz_Str,0);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceString(AI_tdst_Node *_pst_Node)
{
    AI_PushInt(0);
    return AI_EvalFunc_DbgTraceStringCanal(_pst_Node);
}
/**/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgTraceTextCanal_C(TEXT_tdst_Eval *_pst_Text,int _iCanal)
{
    TEXT_tdst_OneText	*pst_Txt;
    int id;
    char logg[1024];

	if(!_pst_Text) return ;
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return ;
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return ;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return ;

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return ;

#ifdef ACTIVE_EDITORS
    sprintf(logg, "%s(%x) :%s->{%s}", 
        pst_Txt->pst_Ids[id].asz_Name, 
        pst_Txt->pst_Ids[id].ul_IdKey, 
        pst_Txt->pst_Ids[id].pv_Obj ? ((OBJ_tdst_GameObject*)pst_Txt->pst_Ids[id].pv_Obj)->sz_Name : "none",
        pst_Txt->pst_Ids[id].i_Offset + pst_Txt->psz_Text);

    M_Print("%s", logg,_iCanal);
#else    
    sprintf(logg, "%x->{%s}", 
        pst_Txt->pst_Ids[id].ul_IdKey, 
        pst_Txt->pst_Ids[id].i_Offset + pst_Txt->psz_Text);

    M_Print("%s", logg,_iCanal);
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceTextCanal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
    int iCanal = AI_PopInt();
	/*~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_EvalFunc_DbgTraceTextCanal_C(ptr,iCanal);
	return ++_pst_Node;
}
/**/
void AI_EvalFunc_DbgTraceText_C(TEXT_tdst_Eval *_pst_Text)
{
    AI_EvalFunc_DbgTraceTextCanal_C(_pst_Text,0);
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceText(AI_tdst_Node *_pst_Node)
{
    AI_PushInt(0);
    return AI_EvalFunc_DbgTraceTextCanal(_pst_Node);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgError_C(char *_psz_Str)
{
#ifdef ACTIVE_EDITORS
	AI_Check(0, _psz_Str);
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgError(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	char	*p;
	/*~~~~~~~*/

	p = (char *) AI_PopStringPtr();
	AI_EvalFunc_DbgError_C(p);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgWarning_C(char *_psz_Str)
{
#ifdef ACTIVE_EDITORS
	ERR_X_Warning(0, _psz_Str, NULL);
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgWarning(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	char	*p;
	/*~~~~~~~*/

	p = (char *) AI_PopStringPtr();
	AI_EvalFunc_DbgWarning_C(p);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgTraceObjectCanal_C(OBJ_tdst_GameObject *_pst_GO,int _iCanal)
{
#ifdef ACTIVE_EDITORS
	LINK_gb_CanAbort = FALSE;
	if(!_pst_GO)
	{
		M_Print("%s", "None",_iCanal);
	}
	else
	{
		M_Print("%s", _pst_GO->sz_Name,_iCanal);
	}
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceObjectCanal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
    int iCanal = AI_PopInt();
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = AI_PopGameObject();
	AI_EvalFunc_DbgTraceObjectCanal_C(pst_GO,iCanal);
	return ++_pst_Node;
}
/**/
void AI_EvalFunc_DbgTraceObject_C(OBJ_tdst_GameObject *_pst_GO)
{
#ifdef ACTIVE_EDITORS
    AI_EvalFunc_DbgTraceObjectCanal_C(_pst_GO,0);
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgTraceObject(AI_tdst_Node *_pst_Node)
{
    AI_PushInt(0);
    return AI_EvalFunc_DbgTraceObjectCanal(_pst_Node);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgBreakPoint_C(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DbgBreakPoint(AI_tdst_Node *_pst_Node)
{
	_pst_Node[1].c_Flags |= AI_Cflag_ForceBreakPoint;
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgRenderVector_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*pst_Origin,
	MATH_tdst_Vector	*pst_Vector,
	ULONG				ul_Color
)
{
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Get world of the object */
		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		if(!pst_World) return;

		MATH_CopyVector(&pst_World->st_Origin[pst_World->uc_Vector], pst_Origin);
		MATH_CopyVector(&pst_World->st_Vector[pst_World->uc_Vector], pst_Vector);
		pst_World->aul_Color[pst_World->uc_Vector] = ul_Color;
        pst_World->ap_VectorGAO[pst_World->uc_Vector] = pst_GO;
		pst_World->uc_Vector = (pst_World->uc_Vector == (WOR_Cte_DbgVectorRender-1)) ? 0 : pst_World->uc_Vector + 1;
	}

#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgRenderVector(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Vector, st_Origin;
	ULONG				ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Color = AI_PopInt();
	AI_PopVector(&st_Vector);
	AI_PopVector(&st_Origin);

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DbgRenderVector_C(pst_GO, &st_Origin, &st_Vector, ul_Color);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgEraseAllVectors_C(OBJ_tdst_GameObject *pst_GO)
{
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Get world of the object */
		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		if(!pst_World) return;

		pst_World->uc_Vector = 0;
		pst_World->uc_Circle = 0;
		pst_World->uc_GeoForm = 0;
	}

#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgEraseAllVectors(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DbgEraseAllVectors_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgRenderCircle_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*pst_Center,
	float				_f_Radius,
	MATH_tdst_Vector	*_pst_Normal,
	ULONG				ul_Color
)
{
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

        /* Get world of the object */
		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		if(!pst_World) return;

		MATH_CopyVector(&pst_World->st_Center[pst_World->uc_Circle], pst_Center);
		MATH_CopyVector(&pst_World->st_Normal[pst_World->uc_Circle], _pst_Normal);
		pst_World->af_Radius[pst_World->uc_Circle] = _f_Radius;
		pst_World->aul_CircleColor[pst_World->uc_Circle] = ul_Color;
        pst_World->ap_CircleGAO[pst_World->uc_Circle] = pst_GO;
		pst_World->uc_Circle = (pst_World->uc_Circle == (WOR_Cte_DbgCircleRender-1)) ? 0 : pst_World->uc_Circle + 1;
	}

#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgRenderCircle(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Center, st_Normal;
	float				f_Radius;
	ULONG				ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Color = AI_PopInt();
	AI_PopVector(&st_Normal);
	f_Radius = AI_PopFloat();
	AI_PopVector(&st_Center);

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DbgRenderCircle_C(pst_GO, &st_Center, f_Radius, &st_Normal, ul_Color);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgRenderSphere_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*pst_Origin,
	float				f_Radius,
	ULONG				ul_Color
)
{
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
        ULONG ulIndex; 
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Get world of the object */
		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		if(!pst_World) return;
		
        ulIndex = pst_World->uc_GeoForm;
		pst_World->ai_GFType[ ulIndex ] = WOR_Cte_DbgGFType_Sphere;
		MATH_CopyVector(&pst_World->ast_GFPos[ ulIndex ], pst_Origin);
		pst_World->af_GFVal1[ ulIndex ] = f_Radius;
		pst_World->aul_GFColor[ ulIndex ] = ul_Color;
        pst_World->ap_GeoFormGAO[ ulIndex ] = pst_GO;
		
		pst_World->uc_GeoForm = (ulIndex + 1) % WOR_Cte_DbgGeoForm;
	}

#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgRenderSphere(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Origin;
	float				f_Radius;
	ULONG				ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Color = AI_PopInt();
	f_Radius = AI_PopFloat();
	AI_PopVector(&st_Origin);

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DbgRenderSphere_C(pst_GO, &st_Origin, f_Radius, ul_Color);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgRenderCone_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*pst_Origin,
	MATH_tdst_Vector	*pst_Axis,
	float				f_Alpha,
	ULONG				ul_Color
)
{
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Get world of the object */
		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		if(!pst_World) return;
		
		pst_World->ai_GFType[ pst_World->uc_GeoForm ] = WOR_Cte_DbgGFType_Cone3D;
		MATH_CopyVector(&pst_World->ast_GFPos[ pst_World->uc_GeoForm ], pst_Origin);
		MATH_CopyVector(&pst_World->ast_GFAxe1[ pst_World->uc_GeoForm ], pst_Axis );
		pst_World->af_GFVal1[ pst_World->uc_GeoForm ] = f_Alpha;
		pst_World->aul_GFColor[ pst_World->uc_GeoForm ] = ul_Color;
        pst_World->ap_GeoFormGAO[ pst_World->uc_GeoForm ] = pst_GO;

		pst_World->uc_GeoForm = (pst_World->uc_GeoForm + 1) % WOR_Cte_DbgGeoForm;
	}

#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgRenderCone(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Origin, st_Axis;
	float				f_Alpha;
	ULONG				ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Color = AI_PopInt();
	f_Alpha = AI_PopFloat();
	AI_PopVector(&st_Axis);
	AI_PopVector(&st_Origin);

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DbgRenderCone_C(pst_GO, &st_Origin, &st_Axis, f_Alpha, ul_Color);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgRenderCylinder_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*pst_Origin,
	MATH_tdst_Vector	*pst_Axis,
	float				f_Radius,
	ULONG				ul_Color
)
{
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Get world of the object */
		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		if(!pst_World) return;
		
		pst_World->ai_GFType[ pst_World->uc_GeoForm ] = WOR_Cte_DbgGFType_Cylinder;
		MATH_CopyVector(&pst_World->ast_GFPos[ pst_World->uc_GeoForm ], pst_Origin);
		MATH_CopyVector(&pst_World->ast_GFAxe1[ pst_World->uc_GeoForm ], pst_Axis );
		pst_World->af_GFVal1[ pst_World->uc_GeoForm ] = f_Radius;
		pst_World->aul_GFColor[ pst_World->uc_GeoForm ] = ul_Color;
        pst_World->ap_GeoFormGAO[ pst_World->uc_GeoForm ] = pst_GO;

		pst_World->uc_GeoForm = (pst_World->uc_GeoForm + 1) % WOR_Cte_DbgGeoForm;
	}

#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgRenderCylinder(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Origin, st_Axis;
	float				f_Radius;
	ULONG				ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Color = AI_PopInt();
	f_Radius = AI_PopFloat();
	AI_PopVector(&st_Axis);
	AI_PopVector(&st_Origin);

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DbgRenderCylinder_C(pst_GO, &st_Origin, &st_Axis, f_Radius, ul_Color);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgRender2DRect_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*pst_Min,
	MATH_tdst_Vector	*pst_Max,
	ULONG				ul_Color
)
{
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Get world of the object */
		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		if(!pst_World) return;
		
		pst_World->ai_GFType[ pst_World->uc_GeoForm ] = WOR_Cte_DbgGFType_2DRect;
		MATH_CopyVector(&pst_World->ast_GFAxe1[ pst_World->uc_GeoForm ], pst_Min );
		MATH_CopyVector(&pst_World->ast_GFAxe2[ pst_World->uc_GeoForm ], pst_Max );
		pst_World->aul_GFColor[ pst_World->uc_GeoForm ] = ul_Color;
        pst_World->ap_GeoFormGAO[ pst_World->uc_GeoForm ] = pst_GO;

		pst_World->uc_GeoForm = (pst_World->uc_GeoForm + 1) % WOR_Cte_DbgGeoForm;
	}

#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgRender2DRect(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Min, st_Max;
	ULONG				ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Color = AI_PopInt();
	AI_PopVector(&st_Max);
	AI_PopVector(&st_Min);

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DbgRender2DRect_C(pst_GO, &st_Min, &st_Max, ul_Color);
	return ++_pst_Node;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgLastRayGet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Dest)
{
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Get world of the object */
		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		if(!pst_World) return;

		MATH_SubVector(pst_Dest, &pst_World->st_RayInfo.st_CollidedPoint, &pst_World->st_RayInfo.st_Origin);
	}

#else
	MATH_CopyVector(pst_Dest, &MATH_gst_NulVector);
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgLastRayGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DbgLastRayGet_C(pst_GO, &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgClearLog_C(void)
{
#ifdef ACTIVE_EDITORS
	LINK_SendMessageToEditors(ELOG_MESSAGE_CLEARLOG, 0, 0);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DbgClearLog(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_DbgClearLog_C();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgStartRaster_C(int a, char *b)
{
#ifdef PSX2_TARGET
#ifndef _FINAL_
	{
		extern void GSP_SetRasterName(u32 Number , char *String);
		GSP_SetRasterName(RASTER_IA_USR_BASE + a , b);
		_GSP_BeginRaster(RASTER_IA_USR_BASE + a);
	}
#endif
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgStartRaster(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	uc_Raster;
	char	*pc_String;
	/*~~~~~~~~~~~~~~~*/

	pc_String = AI_PopStringPtr();
	uc_Raster = AI_PopInt();
#ifdef PSX2_TARGET
#ifndef _FINAL_
{
extern void GSP_SetRasterName(u32 Number , char *String);
	GSP_SetRasterName(RASTER_IA_USR_BASE + uc_Raster , pc_String);
	_GSP_BeginRaster(RASTER_IA_USR_BASE + uc_Raster);
	}
#endif
#endif

#ifdef RASTERS_ON 
	if(uc_Raster < ENG_C_NbUserRasters)
	{
		ENG_gapst_RasterEng_User[uc_Raster].uw_StartCount = 0;
		ENG_gapst_RasterEng_User[uc_Raster].psz_Name = pc_String;
		PRO_StartTrameRaster(&ENG_gapst_RasterEng_User[uc_Raster]);
	}

#endif
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgStartRaster2_C(int a, char *b)
{
}

/**/
AI_tdst_Node *AI_EvalFunc_DbgStartRaster2(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	uc_Raster;
	/*~~~~~~~~~~~~~~~*/

	uc_Raster = AI_PopInt();

#ifdef RASTERS_ON
	if(uc_Raster < ENG_C_NbUserRasters)
	{
		ENG_gapst_RasterEng_User[uc_Raster].uw_StartCount = 0;
		ENG_gapst_RasterEng_User[uc_Raster].psz_Name = "noname";
		PRO_StartTrameRaster(&ENG_gapst_RasterEng_User[uc_Raster]);
	}
#endif
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgStopRaster_C(int a)
{
#ifdef PSX2_TARGET
#ifndef _FINAL_
	_GSP_EndRaster(RASTER_IA_USR_BASE + a);
#endif
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgStopRaster(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	uc_Raster;
	/*~~~~~~~~~~~~~~*/

	uc_Raster = AI_PopInt();
	
#ifdef PSX2_TARGET
#ifndef _FINAL_
	_GSP_EndRaster(RASTER_IA_USR_BASE + uc_Raster);
#endif
#endif
	

#ifdef RASTERS_ON
	if(uc_Raster < ENG_C_NbUserRasters)
	{
		PRO_StopTrameRaster(&ENG_gapst_RasterEng_User[uc_Raster]);
	}

#endif
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DbgVarsCurToInit_C(OBJ_tdst_GameObject *p)
{
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgVarsCurToInit(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_Instance *pst_Ai;

	AI_M_GetCurrentObject(pst_GO);
#if defined ACTIVE_EDITORS || !defined(AI_OPTIM)
	if(!pst_GO->pst_Extended) return ++_pst_Node;
	if(!pst_GO->pst_Extended->pst_Ai) return ++_pst_Node;
	pst_Ai = (AI_tdst_Instance *) pst_GO->pst_Extended->pst_Ai;

	if(!pst_Ai->pc_VarsBufferInit) return ++_pst_Node;
	if(!pst_Ai->pst_Model) return ++_pst_Node;
	if(!pst_Ai->pst_Model->pst_VarDes) return ++_pst_Node;
	if(!pst_Ai->pst_Model->pst_VarDes->ul_SizeBufferInit) return ++_pst_Node;

	L_memcpy(pst_Ai->pc_VarsBufferInit, pst_Ai->pc_VarsBuffer, pst_Ai->pst_Model->pst_VarDes->ul_SizeBufferInit);
#endif
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_DbgIsObjSel_C(OBJ_tdst_GameObject *p)
{
#ifdef ACTIVE_EDITORS
	return ( p->ul_EditorFlags & OBJ_C_EditFlags_Selected ) ? 1 : 0;
#else
	return FALSE;
#endif;
}
/**/
AI_tdst_Node *AI_EvalFunc_DbgIsObjSel(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *pst_GO;
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt( AI_EvalFunc_DbgIsObjSel_C( pst_GO ) );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_DbgStartRecord_C()
{
    if (!INO_b_RecordInput && !INO_b_PlayInput)
    {
        INO_b_RecordInput = TRUE;
        MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 2;
    }
}

AI_tdst_Node *AI_EvalFunc_DbgStartRecord(AI_tdst_Node *_pst_Node)
{    
    AI_EvalFunc_DbgStartRecord_C();
    return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_DbgStopRecord_C()
{
    if (INO_b_RecordInput)
    {
        INO_b_RecordInput = FALSE;   
        MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 0;
    }
}

AI_tdst_Node *AI_EvalFunc_DbgStopRecord(AI_tdst_Node *_pst_Node)
{
    AI_EvalFunc_DbgStopRecord_C();
    return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_DbgStartPlay_C()
{
    if (!INO_b_RecordInput && !INO_b_PlayInput)
    {
        INO_b_PlayInput = TRUE;
        MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 2;
    }
}

AI_tdst_Node *AI_EvalFunc_DbgStartPlay(AI_tdst_Node *_pst_Node)
{
    AI_EvalFunc_DbgStartPlay_C();
    return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_DbgStopPlay_C()
{
    if (INO_b_PlayInput)
    {
        INO_b_PlayInput = FALSE;
        MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 0;
    }
}

AI_tdst_Node *AI_EvalFunc_DbgStopPlay(AI_tdst_Node *_pst_Node)
{
    AI_EvalFunc_DbgStopPlay_C();
    return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_DbgIsPlay_C()
{
    if (INO_b_PlayInput)
        return TRUE;
    return FALSE;
}

AI_tdst_Node *AI_EvalFunc_DbgIsPlay(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( AI_EvalFunc_DbgIsPlay_C() );
    return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_DbgIsRecord_C()
{
    if (INO_b_RecordInput)
        return TRUE;
    return FALSE;
}

AI_tdst_Node *AI_EvalFunc_DbgIsRecord(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( AI_EvalFunc_DbgIsRecord_C() );        
    return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_DbgDisplaySet_C(OBJ_tdst_GameObject *pst_GO, BOOL _bDisplayDebug)
{
#ifdef ACTIVE_EDITORS
    pst_GO->b_DontDisplayDebug = !_bDisplayDebug;
#endif //ACTIVE_EDITORS
}

AI_tdst_Node * AI_EvalFunc_DbgDisplaySet(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *pst_GO;
	AI_M_GetCurrentObject(pst_GO);
    AI_EvalFunc_DbgDisplaySet_C(pst_GO,AI_PopBool());
    return ++_pst_Node;
}

BOOL AI_EvalFunc_DbgDisplayGet_C(OBJ_tdst_GameObject *pst_GO)
{
#ifdef ACTIVE_EDITORS
    return !pst_GO->b_DontDisplayDebug;
#else//ACTIVE_EDITORS
    return FALSE;
#endif //ACTIVE_EDITORS
}

AI_tdst_Node * AI_EvalFunc_DbgDisplayGet(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *pst_GO;
	AI_M_GetCurrentObject(pst_GO);
    AI_PushBool(AI_EvalFunc_DbgDisplayGet_C( pst_GO ));
    return ++_pst_Node;
}


#undef ENABLE_AI_DBG_MESSAGE
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
