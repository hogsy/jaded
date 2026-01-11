/*$T AIfunctions_GFX.c GC! 1.100 05/31/01 10:18:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "GFX/GFX.h"
#include "GFX/GFXlightning.h"
#include "GFX/GFXflare.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDconst.h"
#include "ENGine/Sources/MODifier/MDFstruct.h"
#include "ENGine/Sources/MODifier/MDFmodifier_GPG.h"
#ifdef JADEFUSION
#include "ENGine/Sources/MoDiFier/MDFmodifier_SFX.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#endif
#include "GDInterface/GDIrequest.h"
#include "INOut/INO.h"
#ifdef PSX2_TARGET
#   include <eeregs.h>
#   include <eestruct.h>
#   include <libgraph.h>
#   include <libdma.h>
#   include <libvu0.h>
#   include <sifdev.h>
#   include <libpc.h>
#   include "GS_PS2/Gsp.h"
#   define _PSX2_DSPLS
#endif

#ifdef _XENON_RENDER
#include "XenonGraphics/XeAfterEffectManager.h"
#include "XenonGraphics/XeWeatherManager.h"
#endif

#ifdef _XENON
#include "Xenon/VideoManager/VideoManager.h"
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    General GFX functions
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET
void	Gsp_DepthBlur(int OnOff, float ZStart, float ZEnd);
#elif defined(_GAMECUBE)
void	GXI_DepthBlur(int OnOff, float ZStart, float ZEnd);
#endif

int GFX_i_GlobalID = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_GFXDepthBlur(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	int		OnOff;
	float	ZStart, ZEnd;
	/*~~~~~~~~~~~~~~~~~*/

	ZEnd = AI_PopFloat();
	ZStart = AI_PopFloat();
	OnOff = AI_PopInt();
#ifdef PSX2_TARGET
	Gsp_DepthBlur(OnOff, ZStart, ZEnd);
#elif defined(_GAMECUBE)
	GXI_DepthBlur(OnOff, ZStart, ZEnd);
#else
#endif
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GFXAddWorld_C(int c_Type, int i_Param)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_DisplayData	*pst_DD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(MAI_gst_MainHandles.pst_DisplayData)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		SOFT_tdst_Vertex	st_Vertex[3];
		ULONG				ul_Color;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_DD = GDI_gpst_CurDD;
		GDI_gpst_CurDD = MAI_gst_MainHandles.pst_DisplayData;

		pst_DD->st_GDI.pfnv_SetProjectionMatrix(&CAM_gst_IsoNoClip);
		pst_DD->st_GDI.pfnv_SetViewMatrix(&MATH_gst_IdentityMatrix);

		GDI_gpst_CurDD = pst_DD;

		/* ul_Color = (i_Param << 16) + (i_Param << 8) + i_Param; */
		ul_Color = (i_Param << 24);

		st_Vertex[0].color = ul_Color;
		st_Vertex[0].x = -1;
		st_Vertex[0].y = -1;
		st_Vertex[0].ooz = 0;
		st_Vertex[1].color = ul_Color;
		st_Vertex[1].x = 1;
		st_Vertex[1].y = -1;
		st_Vertex[1].ooz = 1;
		st_Vertex[2].color = ul_Color;
		st_Vertex[2].x = -1;
		st_Vertex[2].y = 1;
		st_Vertex[2].ooz = 0;
		pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_Draw2DTriangle, (ULONG) & st_Vertex);

		st_Vertex[0].color = ul_Color;
		st_Vertex[0].x = 1;
		st_Vertex[0].y = -1;
		st_Vertex[0].ooz = 0;
		st_Vertex[1].color = ul_Color;
		st_Vertex[1].x = 1;
		st_Vertex[1].y = 1;
		st_Vertex[1].ooz = 1;
		st_Vertex[2].color = ul_Color;
		st_Vertex[2].x = -1;
		st_Vertex[2].y = 1;
		st_Vertex[2].ooz = 0;
		pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_Draw2DTriangle, (ULONG) & st_Vertex);

		pst_DD->ul_DisplayFlags |= GDI_Cul_DF_DoNotClear;
	}

	return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXAddWorld(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~*/
	char	c_Type;
	int		i_Param;
	/*~~~~~~~~~~~~*/

	i_Param = AI_PopInt();
	c_Type = (char) AI_PopInt();

	AI_PushInt(AI_EvalFunc_GFXAddWorld_C(c_Type, i_Param));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GFXAdd_C(OBJ_tdst_GameObject *pst_GO, char c_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	return GFX_i_Add(&pst_World->pst_GFX, c_Type, (void *) pst_GO);
}
/**/
int AI_EvalFunc_GFXAdd_C_CURRENT(char c_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(AI_gpst_CurrentGameObject);
	return GFX_i_Add(&pst_World->pst_GFX, c_Type, (void *) AI_gpst_CurrentGameObject);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXAdd(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				c_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	c_Type = (char) AI_PopInt();
	AI_PushInt(AI_EvalFunc_GFXAdd_C(pst_GO, c_Type));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GFXAddSorted_C(OBJ_tdst_GameObject *pst_GO, char c_Type, int i_Order)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	return GFX_i_AddSorted(&pst_World->pst_GFXInterface, c_Type, (USHORT) i_Order, (void *) pst_GO);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXAddSorted(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				c_Type;
	int					i_Order;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Order = AI_PopInt();
	c_Type = (char) AI_PopInt();
	AI_PushInt(AI_EvalFunc_GFXAddSorted_C(pst_GO, c_Type, i_Order));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GFXDel_C(OBJ_tdst_GameObject *pst_GO, int val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	**ppst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	ppst_GFX = GFX_M_GetListPtr(pst_World, val);
	GFX_i_SetDeath(ppst_GFX, GFX_pst_FindById(ppst_GFX, (USHORT) val));
}
/**/
void AI_EvalFunc_GFXDel_C_CURRENT(int val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	**ppst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(AI_gpst_CurrentGameObject);
	ppst_GFX = GFX_M_GetListPtr(pst_World, val);
	GFX_i_SetDeath(ppst_GFX, GFX_pst_FindById(ppst_GFX, (USHORT) val));
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXDel(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	val = AI_PopInt();
	AI_EvalFunc_GFXDel_C(pst_GO, val);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GFXLifeTimeSet_C(OBJ_tdst_GameObject *pst_GO, int id, float time)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	**ppst_GFX, *pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	ppst_GFX = GFX_M_GetListPtr(pst_World, id);
	pst_GFX = GFX_pst_FindById(ppst_GFX, (USHORT) id);
	AI_Check(pst_GFX, "Unknown GFX id");
	pst_GFX->f_LifeTime = time;
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXLifeTimeSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id;
	float				time;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	time = AI_PopFloat();
	id = AI_PopInt();
	AI_EvalFunc_GFXLifeTimeSet_C(pst_GO, id, time);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_GFXRequest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~*/
	int i1, i2, i3;
	/*~~~~~~~~~~~*/

	i3 = AI_PopInt();
	i2 = AI_PopInt();
	i1 = AI_PopInt();

	AI_PushInt(GFX_i_Request(AI_PopInt(), i1, i2, i3));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_GFXGetf_C(OBJ_tdst_GameObject *pst_GO, int Id, int Param)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	return GFX_f_Getf(pst_GFX, (USHORT) Id, Param);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXGetf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Id, Param;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Param = AI_PopInt();
	Id = AI_PopInt();
	AI_PushFloat(AI_EvalFunc_GFXGetf_C(pst_GO, Id, Param));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFXSetCurGlobal(int id)
{
	GFX_i_GlobalID = id;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GFXSetf_C(OBJ_tdst_GameObject *pst_GO, int Id, int Param, float f_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	GFX_Setf(pst_GFX, (USHORT) Id, Param, f_Value);
}
/**/
void AI_EvalFunc_GFXSetf_C_CURGLOB(int Param, float f_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(AI_gpst_CurrentGameObject);
	pst_GFX = GFX_M_GetList(pst_World, GFX_i_GlobalID);
	GFX_Setf(pst_GFX, (USHORT) GFX_i_GlobalID, Param, f_Value);
}
/**/
void AI_EvalFunc_GFXSetf_C_CURRENT(int Id, int Param, float f_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(AI_gpst_CurrentGameObject);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	GFX_Setf(pst_GFX, (USHORT) Id, Param, f_Value);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXSetf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Param, Id;
	float				f_Value;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Value = AI_PopFloat();
	Param = AI_PopInt();
	Id = AI_PopInt();
	AI_EvalFunc_GFXSetf_C(pst_GO, Id, Param, f_Value);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GFXGeti_C(OBJ_tdst_GameObject *pst_GO, int Id, int Param)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	return GFX_i_Geti(pst_GFX, (USHORT) Id, Param);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXGeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Id, Param;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Param = AI_PopInt();
	Id = AI_PopInt();
	AI_PushInt(AI_EvalFunc_GFXGeti_C(pst_GO, Id, Param));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GFXSeti_C(OBJ_tdst_GameObject *pst_GO, int Id, int Param, int Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	GFX_Seti(pst_GFX, (USHORT) Id, Param, Value);
}
/**/
void AI_EvalFunc_GFXSeti_C_CURGLOB(int Param, int Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(AI_gpst_CurrentGameObject);
	pst_GFX = GFX_M_GetList(pst_World, GFX_i_GlobalID);
	GFX_Seti(pst_GFX, (USHORT) GFX_i_GlobalID, Param, Value);
}
/**/
void AI_EvalFunc_GFXSeti_C_CURRENT(int Id, int Param, int Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(AI_gpst_CurrentGameObject);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	GFX_Seti(pst_GFX, (USHORT) Id, Param, Value);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXSeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Id, Param, Value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Value = AI_PopInt();
	Param = AI_PopInt();
	Id = AI_PopInt();
	AI_EvalFunc_GFXSeti_C(pst_GO, Id, Param, Value);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GFXGetv_C(OBJ_tdst_GameObject *pst_GO, int Id, int Param, MATH_tdst_Vector *Result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	MATH_CopyVector(Result, GFX_pst_Getv(pst_GFX, (USHORT) Id, Param));
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXGetv(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Id, Param;
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Param = AI_PopInt();
	Id = AI_PopInt();

	AI_EvalFunc_GFXGetv_C(pst_GO, Id, Param, &v);
	AI_PushVector(&v);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GFXSetv_C(OBJ_tdst_GameObject *pst_GO, int Id, int Param, MATH_tdst_Vector *V)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	GFX_Setv(pst_GFX, (USHORT) Id, Param, V);
}
/**/
void AI_EvalFunc_GFXSetv_C_CURRENT(int Id, int Param, MATH_tdst_Vector *V)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(AI_gpst_CurrentGameObject);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	GFX_Setv(pst_GFX, (USHORT) Id, Param, V);
}
/**/
void AI_EvalFunc_GFXSetv_C_CURGLOB(int Param, MATH_tdst_Vector *V)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(AI_gpst_CurrentGameObject);
	pst_GFX = GFX_M_GetList(pst_World, GFX_i_GlobalID);
	GFX_Setv(pst_GFX, (USHORT) GFX_i_GlobalID, Param, V);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXSetv(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Id, Param;
	MATH_tdst_Vector	*v;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	v = AI_PopVectorPtr();
	Param = AI_PopInt();
	Id = AI_PopInt();
	AI_EvalFunc_GFXSetv_C(pst_GO, Id, Param, v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GFXFlagSet_C(OBJ_tdst_GameObject *pst_GO, int Id, int i_Flag, int i_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	GFX_FlagSet(pst_GFX, (USHORT) Id, i_Flag, i_Value);
}
/**/
void AI_EvalFunc_GFXFlagSet_C_CURRENT(int Id, int i_Flag, int i_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(AI_gpst_CurrentGameObject);
	pst_GFX = GFX_M_GetList(pst_World, Id);
	GFX_FlagSet(pst_GFX, (USHORT) Id, i_Flag, i_Value);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXFlagSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Value, i_Flag;
	int					Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Value = AI_PopInt();
	i_Flag = AI_PopInt();
	Id = AI_PopInt();
	AI_EvalFunc_GFXFlagSet_C(pst_GO, Id, i_Flag, i_Value);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GFXMaterialSet_C(OBJ_tdst_GameObject *pst_GOU, int Id, OBJ_tdst_GameObject *pst_GO, int _l_Id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	MAT_tdst_Multi	*pst_Mat;
	GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return;

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_GFX = GFX_M_GetList(pst_World, Id);

	pst_Mat = (MAT_tdst_Multi *) pst_GO->pst_Base->pst_Visu->pst_Material;
	if(pst_Mat && (_l_Id >= 0) && (pst_Mat->st_Id.i->ul_Type == GRO_MaterialMulti))
		pst_Mat = (MAT_tdst_Multi *) pst_Mat->dpst_SubMaterial[_l_Id % pst_Mat->l_NumberOfSubMaterials];

	GFX_SetMaterial(pst_GFX, (USHORT) Id, pst_Mat);
}
/**/
void AI_EvalFunc_GFXMaterialSet_C_CURRENT(int Id, OBJ_tdst_GameObject *pst_GO, int _l_Id)
{
	AI_EvalFunc_GFXMaterialSet_C(AI_gpst_CurrentGameObject, Id, pst_GO, _l_Id);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXMaterialSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_GO1;
	LONG				_l_Id;
	int					Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	_l_Id = AI_PopInt();
	pst_GO1 = AI_PopGameObject();
	Id = AI_PopInt();
	AI_EvalFunc_GFXMaterialSet_C(pst_GO, Id, pst_GO1, _l_Id);

	return ++_pst_Node;
}

extern void GFX_CreateExplosion
			(
				float				Size,
				float				Speed,
				MATH_tdst_Vector	*Position,
				OBJ_tdst_GameObject *pst_GO,
				void				*p_Material
			);
/**/
AI_tdst_Node *AI_EvalFunc_CreateExplosion(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*Position;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_GOMATERIAL;
	float				Speed, Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	pst_GOMATERIAL = AI_PopGameObject();
	Position = AI_PopVectorPtr();
	Speed = AI_PopFloat();
	Size = AI_PopFloat();

	if
	(
		(pst_GOMATERIAL->pst_Base)
	&&	(pst_GOMATERIAL->pst_Base->pst_Visu)
	&&	(pst_GOMATERIAL->pst_Base->pst_Visu->pst_Material)
	)
	{
		GFX_CreateExplosion(Size, Speed, Position, pst_GO, (void *) pst_GOMATERIAL->pst_Base->pst_Visu->pst_Material);
	}

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GFXCarteObject_C
(
	OBJ_tdst_GameObject *pst_GO,
	int					i1,
	OBJ_tdst_GameObject *pst_GOParam,
	int					i_Oper,
	int					i_Type
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
    GFX_tdst_List	*pst_GFX;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
    pst_GFX = GFX_M_GetList(pst_World, i1);
	GFX_Carte_Object(pst_GFX, (USHORT) i1, pst_GOParam, i_Oper, i_Type);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFXCarteObject(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Oper, i_Type, i1;
	OBJ_tdst_GameObject *pst_GOParam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Type = AI_PopInt();
	i_Oper = AI_PopInt();
	pst_GOParam = AI_PopGameObject();
	i1 = AI_PopInt();
	AI_EvalFunc_GFXCarteObject_C(pst_GO, i1, pst_GOParam, i_Oper, i_Type);
	return ++_pst_Node;
}
#ifdef _XENON_RENDER
// cant use define because some AI2C functions call Gsp_AE_MASTER...
void Gsp_AE_MASTER(ULONG AENum, ULONG OnOff, ULONG P1, float Pf1)
{
	g_oAfterEffectManager.SetParams(AENum, OnOff, P1, Pf1);
}
#define Gsp_AE_MASTER_GET_ONOFF(a)  g_oAfterEffectManager.IsAfterEffectEnabled(a, g_oAfterEffectManager.GetWriteContext())
#define Gsp_AE_MASTER_GET_P1(a, b)  g_oAfterEffectManager.GetParam(a, b)
#else
extern void		Gsp_AE_MASTER(ULONG AENum, ULONG OnOff, ULONG P1, float Pf1);
extern ULONG	Gsp_AE_MASTER_GET_ONOFF(ULONG AENum);
extern float	Gsp_AE_MASTER_GET_P1(ULONG AENum, ULONG P1);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef PSX2_TARGET	
extern __declspec(scratchpad) Gsp_BigStruct gs_st_Globals 	;
extern void Gsp_InitFB(u_int H_Resolution, u_int VRES, u_int ColorBitNum , u_int ZBufferBitNum);
#endif
extern int	IO_SpecialE3_JoyMoveModif;
int			IO_gi_SubtitleOn;

ULONG GetProgressiveMode() 
{
#ifdef PSX2_TARGET	
	return gs_st_Globals.ModePEnable;
#endif
	// Return -1 = no progressive mode
	// Return 0  = progressive mode is desactivated
	// Return 1  = progressive mode is activated
	return -1;
};

ULONG SetProgressiveMode(ULONG Request) 
{
#ifdef PSX2_TARGET	
	if (gs_st_Globals.ModePEnable != Request)
	{
		gs_st_Globals.ModePEnable = Request;
		Gsp_InitFB(gs_st_Globals.Xsize, GSP_VRES_x2 , 32 , 32);
	}
	return gs_st_Globals.ModePEnable;
#endif

	// Return 0 = desactivate progressive mode 
	// Return 1 = activate progressive mode 
	return -1;
};


ULONG ConsoleSpecificOption(ULONG Request)
{
	
	void ps2INO_NoPad2(void);
	
	switch(Request)
	{
#ifdef _XBOX
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
            return -1;
#else
		case 0x01: 
		    switch(SND_i_ChangeRenderMode( -1 ))
		    {
		    case SND_Cte_RenderMono:
		        return 0;
		    case SND_Cte_RenderStereo:
		    case SND_Cte_RenderHeadPhone:
		        return 1;
		    case SND_Cte_RenderDolbyPrologic:
		    case SND_Cte_RenderDolbyPrologicII:
    		    return 2;
    		default:
    		    return 0;		    
		    }
		
		case 0x02: 
			if(SND_i_ChangeRenderMode( SND_Cte_RenderMono ))
				return ConsoleSpecificOption(0x01);
			else
				return 0;
		
		case 0x03: 
			if(SND_i_ChangeRenderMode( SND_Cte_RenderStereo ))
				return ConsoleSpecificOption(0x01);
			else
				return 1;
		
		case 0x04: 
			if(SND_i_ChangeRenderMode( SND_Cte_RenderDolbyPrologicII ))
				return ConsoleSpecificOption(0x01);
			else
				return 2;
		
		case 0x05: return 0;

		case 0x06:	IO_gi_SubtitleOn = 0; return 0;
		case 0x07:	IO_gi_SubtitleOn = 1; return 1;
#endif		
		
		case 0x10:return GetProgressiveMode(); // Menu progressivemode
		case 0x11:return SetProgressiveMode(0); // 0 desactivate , 1 activate
		case 0x12:return SetProgressiveMode(1); // 0 desactivate , 1 activate
		
		
		/**/
		case 20:	IO_SpecialE3_JoyMoveModif += 1;
					if (IO_SpecialE3_JoyMoveModif > 3)
						IO_SpecialE3_JoyMoveModif = 0;
		/**/
		case 21:	return IO_SpecialE3_JoyMoveModif;
					break;

		/* LANGAGE : DOES CONSOLE SUPPORT LANGAGE SELECTION MENU, return 1 = yes, 0 = no */
		case 30:
			#if defined (_XBOX) || (_XENON)
				return 0;
			#else
				return 1;
			#endif			 
		/* BOOTUP : does console need a boot up test, return 1 = yes, 0 = no */
		case 31:
			#if (defined _XBOX) || (defined _XENON) || (defined _GAMECUBE)
					return 0;
			#else
					return 1;
			#endif		
		
		/* does console needs a loading text between starting maps */
		case 32:
			#ifdef _XBOX
				return 1;
			#else
				return 0;
			#endif			 

		/* does console support menu Audio Type (mono, stereo, .... ) */
		case 33:
			#if defined(_XBOX) || defined(_XENON) || defined(ACTIVE_EDITORS)
				return 0;
			#else
				return 1;
			#endif			 

		/* additional langage */
		case 34 :
			// langue additionnelle : apparaitront dans le menu langue additionnelle
			// retourne 0 si aucune langue additionnelle
			// constant langue définie dans INO.h
			// Yoan : faut que tu te renseignes si dans la liste des langues il faut aussi mettre la langue par défaut (celle du dashboard)
			// et sinon si tu dois sauver quelquepart cette langue ou si le joueur doit la resélectionner à chaque fois qu'il démarre
			#if defined(_XBOX) || defined(_XENON)
			{
				int	lang;
				lang = 0;
				if (INO_b_LanguageIsPresent( INO_e_Dutch ))
					lang |= 1 << INO_e_Dutch;
				if (INO_b_LanguageIsPresent( INO_e_Finnish ))
					lang |= 1 << INO_e_Finnish;
				if (INO_b_LanguageIsPresent( INO_e_Swedish ))
					lang |= 1 << INO_e_Swedish;
				if (INO_b_LanguageIsPresent( INO_e_Danish ))
					lang |= 1 << INO_e_Danish;
				if (INO_b_LanguageIsPresent( INO_e_Norwegian ))
					lang |= 1 << INO_e_Norwegian;
				return lang;
			}
			#else
				// pour tester menu en éditeur return (1 << INO_e_Dutch) | (1 << INO_e_Finnish);
				return 0;
			#endif
		/**/
		case 0x100:
			#ifdef PSX2_TARGET
				ps2INO_NoPad2();
			#endif
			break;
	}
	return -1;
}
AI_tdst_Node *AI_EvalFunc_ConsoleSpecificOption(AI_tdst_Node *_pst_Node)
{
	int		Request;
	Request = AI_PopInt();
	AI_PushInt(ConsoleSpecificOption(Request));
	return ++_pst_Node;
}
void CIN_OnOff(ULONG OnOff)
{
#ifdef USE_DOUBLE_RENDERING
	extern ULONG ENG_gp_DoubleRenderingLocker;
	if (!OnOff)
		ENG_gp_DoubleRenderingLocker = 0x4;
#endif
};

AI_tdst_Node *AI_EvalFunc_CIN_OnOff(AI_tdst_Node *_pst_Node)
{
	int		ONNoff;
	ONNoff = AI_PopInt();
	CIN_OnOff(ONNoff);
	return ++_pst_Node;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DBR_SetN(ULONG OnOff)
{
#ifdef USE_DOUBLE_RENDERING
	extern ULONG ENG_gp_DoubleRendering;
	ENG_gp_DoubleRendering = OnOff;
#endif
};

AI_tdst_Node *AI_EvalFunc_DBR_SetN(AI_tdst_Node *_pst_Node)
{
	int		Value;
	Value = AI_PopInt();
	DBR_SetN(Value);
	return ++_pst_Node;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG DBR_GetN()
{
#ifdef USE_DOUBLE_RENDERING
	extern ULONG ENG_gp_DoubleRendering;
	return ENG_gp_DoubleRendering;
#else
	return 0;
#endif
};

AI_tdst_Node *AI_EvalFunc_DBR_GetN(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(DBR_GetN());
	return ++_pst_Node;
};


/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_Gsp_AE_MASTER(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int		AENum, AEOn, AEP1;
	float	AEPf1;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AEPf1 = AI_PopFloat();
	AEP1 = AI_PopInt();
	AEOn = AI_PopInt();
	AENum = AI_PopInt();
	Gsp_AE_MASTER(AENum, AEOn, AEP1, AEPf1);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_Gsp_AE_MASTEREVT_C(ULONG AENum, ULONG OnOff, ULONG P1, OBJ_tdst_GameObject *gao)
{
	Gsp_AE_MASTER(AENum, OnOff, P1, gao->pst_GlobalMatrix->T.x);
}
/**/
AI_tdst_Node *AI_EvalFunc_Gsp_AE_MASTEREVT(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int		AENum, AEOn, AEP1;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = AI_PopGameObject();
	AEP1 = AI_PopInt();
	AEOn = AI_PopInt();
	AENum = AI_PopInt();
	Gsp_AE_MASTER(AENum, AEOn, AEP1, pst_GO->pst_GlobalMatrix->T.x);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_Gsp_AE_MASTER_GETP1(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	ULONG	P1;
	/*~~~~~~~*/

	P1 = AI_PopInt();
	AI_PushFloat(Gsp_AE_MASTER_GET_P1(AI_PopInt(), P1));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_Gsp_AE_MASTER_GETONOFF(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(Gsp_AE_MASTER_GET_ONOFF(AI_PopInt()));
	return ++_pst_Node;
}

/*
FULL SCREEN VIDEO 
*/
#ifdef PSX2_TARGET
extern void GSP_VideoFullScreenCreate(ULONG ulBigKey );
extern void GSP_VideoFullScreenKill();
#endif 
#ifdef _GAMECUBE
extern void GC_VideoFullScreenCreate(ULONG ulBigKey );
extern void GC_VideoFullScreenKill();
extern ULONG GC_VideoFullScreenIsFinish();
#endif

extern void MTX_Kill(void);
extern ULONG MTX_ul_CallbackLoadFile(ULONG _ul_Key);
extern void MTX_Start(void);

unsigned int    ui_PAL=0x610127be;
unsigned int    ui_NTSC=0x61012b4b;

unsigned int    ui_TRAILERmtxPAL=0x610127be;
unsigned int    ui_TRAILERmtxNTSC=0x61012b4b;
unsigned int    ui_LastVideoKey=-1;

unsigned int    ui_MOFipuKey=0x490294c0;
unsigned int    ui_MOFmtxNTSC=0x61015fbf;
unsigned int    ui_MOFmtxPAL=0x61015fc0;

void GFX_FS_VideoStart(ULONG ulBK)
{
#ifdef PSX2_TARGET
    extern BOOL	ps2MAI_gb_VideoModeNTSC;
    ULONG ul;
    
    // record last key
    ui_LastVideoKey = ulBK;

    // exchange key when pal/ntsc/progressive
    if(ulBK == ui_TRAILERmtxPAL)
    {
        // trailer ?
        if(gs_st_Globals.ModePEnable || ps2MAI_gb_VideoModeNTSC) 
            ulBK = ui_TRAILERmtxNTSC; // force NTSC key
    }
    else if(ulBK == ui_MOFipuKey) 
    {
        // MOF ?
        if(gs_st_Globals.ModePEnable || ps2MAI_gb_VideoModeNTSC) 
            ulBK = ui_MOFmtxNTSC; // force NTSC key
        else
            ulBK = ui_MOFmtxPAL; // force PAL key
    }
    
    // start
    ul=MTX_ul_CallbackLoadFile(ulBK);
    if(ul) 
        MTX_Start();
    else
        GSP_VideoFullScreenCreate(ulBK);
#endif
#ifdef _GAMECUBE
	GC_VideoFullScreenCreate(ulBK);
#endif
#ifdef _XENON
	//g_pVideoManager->Play( ?? );
#endif
}
void GFX_FS_VideoStop()
{
#ifdef PSX2_TARGET
    GSP_VideoFullScreenKill();
    MTX_Kill();
#endif
#ifdef _GAMECUBE
	GC_VideoFullScreenKill();
#endif
#ifdef _XENON
	g_pVideoManager->Stop();
#endif
}
ULONG GFX_FS_VideoFinish()
{
#ifdef PSX2_TARGET
	extern ULONG ulVideoScreentValid;
	return (ulVideoScreentValid & 2) >> 1;
#else
#ifdef _GAMECUBE
	return GC_VideoFullScreenIsFinish();
#endif
#ifdef _XENON
	return g_pVideoManager->IsPlaying() ? 0 : 1;
#endif
	return 1;
#endif
}
AI_tdst_Node *AI_EvalFunc_GFX_FS_VideoStart(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	ULONG	P1;
	/*~~~~~~~*/

	P1 = AI_PopInt();
	GFX_FS_VideoStart(P1);
	return ++_pst_Node;
}
AI_tdst_Node *AI_EvalFunc_GFX_FS_VideoStop(AI_tdst_Node *_pst_Node)
{
	GFX_FS_VideoStop();
	return ++_pst_Node;
}

AI_tdst_Node *AI_EvalFunc_GFX_FS_VideoFinish(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(GFX_FS_VideoFinish());
	return ++_pst_Node;
}

void AE_Remanece(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(7, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(7, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_Remanece(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_Remanece(Factor);
	return ++_pst_Node;
}

void AE_BlackAndWhite(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(11, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(11, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_BlackAndWhite(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_BlackAndWhite(Factor);
	return ++_pst_Node;
}

void AE_ColorBalance(float Factor,float SFactor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(12, 1, 0, Factor);
		Gsp_AE_MASTER(12, 1, 1, SFactor);
	} else
	{
		Gsp_AE_MASTER(12, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_ColorBalance(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor,SFactor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	SFactor = AI_PopFloat();
	Factor = AI_PopFloat();
	AE_ColorBalance(Factor,SFactor);
	return ++_pst_Node;
}

void AE_Contraste(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(9, 1, 0, Factor * 0.5f + 0.5f);
	} else
	{
		Gsp_AE_MASTER(9, 0, 0, 0.5f);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_Contraste(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_Contraste(Factor);
	return ++_pst_Node;
}
#ifdef JADEFUSION
void AE_Contraste_Xe(float Factor)
{
    if (Factor != 0.0f)
    {
        Gsp_AE_MASTER(35, 1, 0, Factor * 0.5f + 0.5f);
    } else
    {
        Gsp_AE_MASTER(35, 0, 0, 0.5f);
    }
}
AI_tdst_Node *AI_EvalFunc_AE_Contraste_Xe(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    float				Factor;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    Factor = AI_PopFloat();
    AE_Contraste_Xe(Factor);
    return ++_pst_Node;
}
#endif
void AE_Brightness(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(8, 1, 0, Factor * 0.5f + 0.5f);
	} else
	{
		Gsp_AE_MASTER(8, 0, 0, 0.5f);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_Brightness(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_Brightness(Factor);
	return ++_pst_Node;
}
#ifdef JADEFUSION
void AE_Brightness_Xe(float Factor)
{
    if (Factor != 0.0f)
    {
        Gsp_AE_MASTER(34, 1, 0, Factor * 0.5f + 0.5f);
    } else
    {
        Gsp_AE_MASTER(34, 0, 0, 0.5f);
    }
}
AI_tdst_Node *AI_EvalFunc_AE_Brightness_Xe(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    float				Factor;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    Factor = AI_PopFloat();
    AE_Brightness_Xe(Factor);
    return ++_pst_Node;
}
#endif
void AE_Wrap(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(17, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(17, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_Wrap(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_Wrap(Factor);
	return ++_pst_Node;
}

void AE_Blur(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(2, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(2, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_Blur(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_Blur(Factor);
	return ++_pst_Node;
}

void AE_ZoomSmooth(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(5, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(5, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_ZoomSmooth(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_ZoomSmooth(Factor);
	return ++_pst_Node;
}

void AE_RotationSmooth(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(6, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(6, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_RotationSmooth(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_RotationSmooth(Factor);
	return ++_pst_Node;
}

void AE_MotionBlur(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(1, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(1, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_MotionBlur(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_MotionBlur(Factor);
	return ++_pst_Node;
}
void AE_MotionSmooth(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(4, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(4, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_MotionSmooth(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_MotionSmooth(Factor);
	return ++_pst_Node;
}
void AE_GodRays(float Factor,MATH_tdst_Vector *LightDir)
{
	Gsp_AE_MASTER(18, 1, 0, Factor);
	Gsp_AE_MASTER(18, 1, 1, LightDir->x);
	Gsp_AE_MASTER(18, 1, 2, LightDir->y);
	Gsp_AE_MASTER(18, 1, 3, LightDir->z);
}
AI_tdst_Node *AI_EvalFunc_AE_GodRays(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	MATH_tdst_Vector *LightDir;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	LightDir = AI_PopVectorPtr();
	Factor = AI_PopFloat();
	AE_GodRays(Factor,LightDir);
	return ++_pst_Node;
}
void AE_ZoomSmoothCenter(float Factor,MATH_tdst_Vector *LightDir)
{
	Gsp_AE_MASTER(5, 1, 0, Factor);
	Gsp_AE_MASTER(5, 1, 1, LightDir->x);
	Gsp_AE_MASTER(5, 1, 2, LightDir->y);
	Gsp_AE_MASTER(5, 1, 3, LightDir->z);
}
AI_tdst_Node *AI_EvalFunc_AE_ZoomSmoothCenter(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	MATH_tdst_Vector *LightDir;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	LightDir = AI_PopVectorPtr();
	Factor = AI_PopFloat();
	AE_ZoomSmoothCenter(Factor,LightDir);
	return ++_pst_Node;
}
void AE_Sharpen(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(19, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(19, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_Sharpen(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_Sharpen(Factor);
	return ++_pst_Node;
}
void AE_BorderBrightness(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(20, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(20, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_BorderBrightness(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_BorderBrightness(Factor);
	return ++_pst_Node;
}
#ifdef JADEFUSION
void AE_BorderColor(int Color)
#else
void AE_BorderColor(u32 Color)
#endif
{
	Gsp_AE_MASTER(23, 1, Color, 1.0f);
}
AI_tdst_Node *AI_EvalFunc_AE_BorderColor(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	u32					Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Color = AI_PopInt();
	AE_BorderColor(Color);
	return ++_pst_Node;
}

void AE_FoggyBlur(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(21, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(21, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_FoggyBlur(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_FoggyBlur(Factor);
	return ++_pst_Node;
}

void AE_PerfectGlow(float Factor , float Threshold, float Radius )
{
	Gsp_AE_MASTER(24, 1, 0, Factor);
	Gsp_AE_MASTER(24, 1, 1, Threshold);
	Gsp_AE_MASTER(24, 1, 2, Radius);
}


AI_tdst_Node *AI_EvalFunc_AE_PerfectGlow(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor,Radius,Thresh;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Radius = AI_PopFloat();
	Thresh = AI_PopFloat();
	Factor = AI_PopFloat();
	AE_PerfectGlow(Factor,Thresh,Radius);
	return ++_pst_Node;
}
void AE_Glow(float Factor)
{
	if (Factor != 0.0f)
	{
		Gsp_AE_MASTER(22, 1, 0, Factor);
	} else
	{
		Gsp_AE_MASTER(22, 0, 0, Factor);
	}
}
AI_tdst_Node *AI_EvalFunc_AE_Glow(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Factor = AI_PopFloat();
	AE_Glow(Factor);
	return ++_pst_Node;
}

AI_tdst_Node *AI_EvalFunc_AE_Splash_AddOne(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern void AE_Splash_AddOne(MATH_tdst_Vector *p2DPosPlusSize , u32 Color );
	MATH_tdst_Vector	*v;
	u32					color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	color = AI_PopInt();
	v = AI_PopVectorPtr();
	//AE_Splash_AddOne(v, color);
	return ++_pst_Node;
}

void AE_Xinvert(int Enable)
{
	GDI_gpst_CurDD->GlobalXInvert = Enable;
}

AI_tdst_Node *AI_EvalFunc_AE_Xinvert(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	u32					color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	color = AI_PopInt();
	AE_Xinvert(color);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GPG_SetI_C(OBJ_tdst_GameObject *pst_GO, ULONG what, float val)
{
	switch(what)
	{
	/* Add perturbator */
	case 0:
		if(GPG_gi_NumPerturbators == GPG_MaxPerturbators) return -1;
		GPG_gpt_Perturbators[GPG_gi_NumPerturbators].pt_GAO = pst_GO;
		GPG_gpt_Perturbators[GPG_gi_NumPerturbators].f_Size = val;
		return GPG_gi_NumPerturbators++;

	/* Del perturbator */
	case 1:
		if(val == -1) return -1;
		if(val >= (ULONG) GPG_gi_NumPerturbators) return -1;
		L_memcpy(&GPG_gpt_Perturbators[(int) val], &GPG_gpt_Perturbators[GPG_gi_NumPerturbators - 1], sizeof(GPG_tdst_Perturbator));
		GPG_gi_NumPerturbators--;
		break;
	}

	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_GPG_SetI(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				what;
	float				val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	val = AI_PopFloat();
	what = AI_PopInt();
	AI_PushInt(AI_EvalFunc_GPG_SetI_C(pst_GO, what, val));
	return ++_pst_Node;
}


void AI_EvalFunc_GFX_DynFogActiveGet_C(OBJ_tdst_GameObject **_ast_GO, int _i_Size, int _i_Id)
{
	extern OBJ_tdst_GameObject*	FOGDYN_gap_ActiveGao[];
	extern int	FOGDYN_gi_ActiveGaoNb;

	L_memset(_ast_GO, 0, sizeof(OBJ_tdst_GameObject**)*_i_Size);
	_i_Size = _i_Size < FOGDYN_gi_ActiveGaoNb ? _i_Size : FOGDYN_gi_ActiveGaoNb ;
	L_memcpy(_ast_GO, FOGDYN_gap_ActiveGao, sizeof(OBJ_tdst_GameObject**)*_i_Size);
}
/**/
AI_tdst_Node *AI_EvalFunc_GFX_DynFogActiveGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Size, i_Id;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Id = AI_PopInt();
	i_Size = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_GFX_DynFogActiveGet_C((OBJ_tdst_GameObject **) st_Var.pv_Addr, i_Size, i_Id );
	return ++_pst_Node;
}
#ifdef JADEFUSION
void AI_EvalFunc_GFX_XeUpdateRain_C(FLOAT _fRainIntensity)
{
#if defined(_XENON_RENDER)
    // Make sure there is a WeatherFX modifier on the object before updating the rain intensity
    if (MDF_pst_GetByType(AI_gpst_CurrentGameObject, MDF_C_Modifier_Weather) == NULL)
        return;

    g_oXeWeatherManager.SetRainIntensity(_fRainIntensity);
#endif
}

AI_tdst_Node* AI_EvalFunc_GFX_XeUpdateRain(AI_tdst_Node* _pst_Node)
{
    FLOAT fValue;

    fValue = AI_PopFloat();
    AI_EvalFunc_GFX_XeUpdateRain_C(fValue);

    return ++_pst_Node;
}

void AI_EvalFunc_GFX_XeUpdateLightning_C(FLOAT _fLightningIntensity)
{
#if defined(_XENON_RENDER)
    g_oXeWeatherManager.SetLightningIntensity(_fLightningIntensity);
#endif
}

AI_tdst_Node* AI_EvalFunc_GFX_XeUpdateLightning(AI_tdst_Node* _pst_Node)
{
    FLOAT fValue;

    fValue = AI_PopFloat();
    AI_EvalFunc_GFX_XeUpdateLightning_C(fValue);

    return ++_pst_Node;
}

void AI_EvalFunc_GFX_XeUpdateRainMode_C(INT _iNoRainZone)
{
#if defined(_XENON_RENDER)
    g_oXeWeatherManager.SetRainMode(_iNoRainZone == 0);
#endif
}

AI_tdst_Node* AI_EvalFunc_GFX_XeUpdateRainMode(AI_tdst_Node* _pst_Node)
{
    INT iValue;

    iValue = AI_PopInt();
    AI_EvalFunc_GFX_XeUpdateRainMode_C(iValue);

    return ++_pst_Node;
}

//---------------------------------------
// AI_EvalFunc_MDF_HeatShimmerCreate_C
//---------------------------------------
void AI_EvalFunc_MDF_HeatShimmerCreate_C( OBJ_tdst_GameObject *   _pst_GO,
                                          MATH_tdst_Vector *      _pst_Offset,
                                          float                   _fWidth,
                                          float                   _fHeight,
                                          float                   _fRange,
                                          float                   _fScrollSpeed,
                                          float                   _fNoisePixelSize )
{
    // Create heat shimmer modifier for the game object
    MDF_tdst_HotAir * pst_HotAir = MDF_SfxGetHotAirModifier( _pst_GO );
    if( pst_HotAir == NULL )
    {
        // Make sure we have extended flag in object
        OBJ_ChangeIdentityFlags( _pst_GO, _pst_GO->ul_IdentityFlags | OBJ_C_IdentityFlag_ExtendedObject, _pst_GO->ul_IdentityFlags);

        // Create Modifier SFX
        MDF_tdst_Modifier * pst_Modifier = MDF_pst_Modifier_Create( _pst_GO, MDF_C_Modifier_Sfx, NULL );
        ERR_X_Assert( pst_Modifier != NULL );

        MDF_Modifier_AddToGameObject( _pst_GO, pst_Modifier );

        pst_HotAir = MDF_SfxGetHotAirModifier( _pst_GO );
    }

    ERR_X_Assert( pst_HotAir != NULL );

    // Init attributes
    pst_HotAir->stOffset                        = *_pst_Offset;
    pst_HotAir->stHotAirObject.Width            = _fWidth;
    pst_HotAir->stHotAirObject.Height           = _fHeight;
    pst_HotAir->stHotAirObject.Range            = _fRange;
    pst_HotAir->stHotAirObject.ScrollSpeed      = _fScrollSpeed;
    pst_HotAir->stHotAirObject.NoisePixelSize   = _fNoisePixelSize;
    pst_HotAir->stHotAirObject.Scroll           = 0.0f;
}

//---------------------------------------
// AI_EvalFunc_MDF_HeatShimmerCreate
//---------------------------------------
AI_tdst_Node * AI_EvalFunc_MDF_HeatShimmerCreate( AI_tdst_Node *_pst_Node )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject * pst_GO;
    MATH_tdst_Vector *    pst_Offset;
    float                 fWidth;
    float                 fHeight;
    float                 fRange;
    float                 fScrollSpeed;
    float                 fNoisePixelSize;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_M_GetCurrentObject(pst_GO);
    fNoisePixelSize = AI_PopFloat();
    fScrollSpeed    = AI_PopFloat();
    fRange          = AI_PopFloat();
    fHeight         = AI_PopFloat();
    fWidth          = AI_PopFloat();
    pst_Offset      = AI_PopVectorPtr();

    AI_EvalFunc_MDF_HeatShimmerCreate_C( pst_GO,
                                         pst_Offset, 
                                         fWidth, 
                                         fHeight,   
                                         fRange,   
                                         fScrollSpeed,   
                                         fNoisePixelSize );
    return ++_pst_Node;
}

//---------------------------------------
// AI_EvalFunc_MDF_HeatShimmerSetRange_C
//---------------------------------------
void AI_EvalFunc_MDF_HeatShimmerSetRange_C( OBJ_tdst_GameObject *   _pst_GO, 
                                            float                   _fRange )
{

    MDF_tdst_HotAir * pst_HotAir = MDF_SfxGetHotAirModifier( _pst_GO );
    ERR_X_Assert( pst_HotAir != NULL );

    pst_HotAir->stHotAirObject.Range = _fRange;
}

//---------------------------------------
// AI_EvalFunc_MDF_HeatShimmerSetRange
//---------------------------------------
AI_tdst_Node * AI_EvalFunc_MDF_HeatShimmerSetRange( AI_tdst_Node * _pst_Node )
{
    OBJ_tdst_GameObject * pst_GO = NULL;

    AI_M_GetCurrentObject(pst_GO);
    float fNewRange = AI_PopFloat();

    AI_EvalFunc_MDF_HeatShimmerSetRange_C( pst_GO, fNewRange );
    
    return ++_pst_Node;
}


//---------------------------------------
// AI_EvalFunc_MDF_HeatShimmerDestroy_C
//---------------------------------------
void AI_EvalFunc_MDF_HeatShimmerDestroy_C( OBJ_tdst_GameObject * _pst_GO )
{
    ERR_X_Assert( _pst_GO );

    if( OBJ_b_TestIdentityFlag( _pst_GO, OBJ_C_IdentityFlag_ExtendedObject) )
    {
        ERR_X_Assert( _pst_GO->pst_Extended );

        // Loop all modifiers to find the hot air 
        MDF_tdst_Modifier * pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
        while( pst_Modifier )
        {
            if( pst_Modifier->i->ul_Type == MDF_C_Modifier_Sfx )
            {
                GAO_tdst_ModifierSfx * pMdfSfx = (GAO_tdst_ModifierSfx *) pst_Modifier->p_Data;
                if( pMdfSfx->type == MDF_SFX_HotAir )
                {
                    // We found the modifier, so destroy it and remove it from list of modifiers
                    MDF_Modifier_DelInGameObject( _pst_GO, pst_Modifier );
                    MDF_Modifier_Destroy( pst_Modifier );
                    return;
                }
            }

            pst_Modifier = pst_Modifier->pst_Next;
        }
    }
}

//---------------------------------------
// AI_EvalFunc_MDF_HeatShimmerDestroy
//---------------------------------------
AI_tdst_Node * AI_EvalFunc_MDF_HeatShimmerDestroy( AI_tdst_Node * _pst_Node )
{
    OBJ_tdst_GameObject * pst_GO = NULL;

    AI_M_GetCurrentObject( pst_GO );
    AI_EvalFunc_MDF_HeatShimmerDestroy_C( pst_GO );

    return ++_pst_Node;
}
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
