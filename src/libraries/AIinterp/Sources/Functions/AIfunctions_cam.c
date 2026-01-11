/*$T AIfunctions_cam.c GC!1.41 07/27/99 14:12:28 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "CAMera/CAMera.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "ENGine/Sources/INTersection/INTmain.h"

#include "AIinterp/Sources/Functions/AIfunctions_cam.h"

// ************************************************************************************************************
//
//										C A M E R A   O U T P U T 
//
// ************************************************************************************************************

/*
DEFINE_FUNCTION(17001,	FUNCTION_CAM_TRACEINT,				"CAM_TraceInt",					TYPE_VOID,			INT,															AI_EvalFunc_CamTraceInt,						AI_C_NotUltra,				"AI_EvalFunc_CamTraceInt_C",						EVE_C_All,										void AI_EvalFunc_CamTraceInt_C(int))
DEFINE_FUNCTION(17002,	FUNCTION_CAM_TRACEFLOAT,			"CAM_TraceFloat",				TYPE_VOID,			FLOAT,															AI_EvalFunc_CamTraceFloat,						AI_C_NotUltra,				"AI_EvalFunc_CamTraceFloat_C",						EVE_C_All,										void AI_EvalFunc_CamTraceFloat_C(float))
DEFINE_FUNCTION(17003,	FUNCTION_CAM_TRACEVECTOR,			"CAM_TraceVector",				TYPE_VOID,			VECTOR,															AI_EvalFunc_CamTraceVector,						AI_C_NotUltra,				"AI_EvalFunc_CamTraceVector_C",						EVE_C_All,										void AI_EvalFunc_CamTraceVector_C(MATH_tdst_Vector *))
DEFINE_FUNCTION(17004,	FUNCTION_CAM_TRACESTRING,			"CAM_TraceString",				TYPE_VOID,			STRING,															AI_EvalFunc_CamTraceString,						AI_C_NotUltra,				"AI_EvalFunc_CamTraceString_C",						EVE_C_All,										void AI_EvalFunc_CamTraceString_C(char *))
DEFINE_FUNCTION(17005,	FUNCTION_CAM_TRACEGAO,				"CAM_TraceObject",				TYPE_VOID,			OBJECT,															AI_EvalFunc_CamTraceObject,						AI_C_NotUltra,				"AI_EvalFunc_CamTraceObject_C",						EVE_C_All,										void AI_EvalFunc_CamTraceObject_C(OBJ_tdst_GameObject *))
DEFINE_FUNCTION(17006,	FUNCTION_CAM_TRACEEOL,				"CAM_TraceEOL",					TYPE_VOID,			VOID,															AI_EvalFunc_CamTraceEOL,						AI_C_NotUltra,				"AI_EvalFunc_CamTraceEOL_C",						EVE_C_None,										void AI_EvalFunc_CamTraceEOL_C(void))
*/

#ifdef		ACTIVE_EDITORS
COLORREF	CAM_gul_OldColorTxt		= 0;
#define		CAM_SET_COLORTXT()		{CAM_gul_OldColorTxt = LINK_gul_ColorTxt; LINK_gul_ColorTxt = 0x00AA00AA; }
#define		CAM_RESET_COLORTXT()	{LINK_gul_ColorTxt = CAM_gul_OldColorTxt;}
#else
#define		CAM_SET_COLORTXT()
#define		CAM_RESET_COLORTXT()
#endif


// =======================================================================================================================
// FUNCTION_CAM_TRACEINT
// =======================================================================================================================
void AI_EvalFunc_CamTraceInt_C(int _i_Val)
{
	CAM_SET_COLORTXT()
	AI_EvalFunc_DbgTraceInt_C(_i_Val);
	CAM_RESET_COLORTXT()
}
//
AI_tdst_Node *AI_EvalFunc_CamTraceInt(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_CamTraceInt_C(AI_PopInt());
	return ++_pst_Node;
}

// =======================================================================================================================
// FUNCTION_CAM_TRACEFLOAT
// =======================================================================================================================
void AI_EvalFunc_CamTraceFloat_C(float _f_Val)
{
	CAM_SET_COLORTXT()
	AI_EvalFunc_DbgTraceFloat_C(_f_Val);
	CAM_RESET_COLORTXT()
}
//
AI_tdst_Node *AI_EvalFunc_CamTraceFloat(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_CamTraceFloat_C(AI_PopFloat());
	return ++_pst_Node;
}

// =======================================================================================================================
// FUNCTION_CAM_TRACEVECTOR
// =======================================================================================================================
void AI_EvalFunc_CamTraceVector_C(MATH_tdst_Vector *_pst_Vec)
{
	CAM_SET_COLORTXT()
	AI_EvalFunc_DbgTraceVector_C(_pst_Vec);
	CAM_RESET_COLORTXT()
}

//
AI_tdst_Node *AI_EvalFunc_CamTraceVector(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&v);
	AI_EvalFunc_CamTraceVector_C(&v);
	return ++_pst_Node;
}

// =======================================================================================================================
// FUNCTION_CAM_TRACESTRING
// =======================================================================================================================
void AI_EvalFunc_CamTraceString_C(char *_psz_Str)
{
	CAM_SET_COLORTXT()
	AI_EvalFunc_DbgTraceString_C(_psz_Str);
	CAM_RESET_COLORTXT()
}

//
AI_tdst_Node *AI_EvalFunc_CamTraceString(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	char	*p;
	/*~~~~~~~*/

	p = (char *) AI_PopStringPtr();
	AI_EvalFunc_CamTraceString_C(p);
	return ++_pst_Node;
}

// =======================================================================================================================
// FUNCTION_CAM_TRACEGAO
// =======================================================================================================================
void AI_EvalFunc_CamTraceObject_C(OBJ_tdst_GameObject *_pst_GO)
{
	CAM_SET_COLORTXT()
	AI_EvalFunc_DbgTraceObject_C(_pst_GO);
	CAM_RESET_COLORTXT()
}
//
AI_tdst_Node *AI_EvalFunc_CamTraceObject(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = AI_PopGameObject();
	AI_EvalFunc_CamTraceObject_C(pst_GO);
	return ++_pst_Node;
}

// =======================================================================================================================
// FUNCTION_CAM_TRACEEOL
// =======================================================================================================================
void AI_EvalFunc_CamTraceEOL_C(void)
{
	CAM_SET_COLORTXT()
	AI_EvalFunc_DbgTraceEOL_C();
	CAM_RESET_COLORTXT()
}
//
AI_tdst_Node *AI_EvalFunc_CamTraceEOL(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_CamTraceEOL_C();
	return ++_pst_Node;
}


// ************************************************************************************************************
//
//										C A M E R A   I N P U T
//
// ************************************************************************************************************

// =======================================================================================================================
// Mouse Was Used ?
// =======================================================================================================================
INT AI_EvalFunc_CamMouseWasUsed_C()
{
	return FALSE;
}
/**/
AI_tdst_Node *AI_EvalFunc_CamMouseWasUsed(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_CamMouseWasUsed_C());
	return ++_pst_Node;
}

// =======================================================================================================================
// Get Input Max X
// =======================================================================================================================
FLOAT AI_EvalFunc_CamGetInputMaxX_C()
{
	return 1.0f;
}
/**/
AI_tdst_Node *AI_EvalFunc_CamGetInputMaxX(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_CamGetInputMaxX_C());
	return ++_pst_Node;
}

// =======================================================================================================================
// Get Mouse Max Y
// =======================================================================================================================
FLOAT AI_EvalFunc_CamGetInputMaxY_C()
{
	return 1.0f;
}
/**/
AI_tdst_Node *AI_EvalFunc_CamGetInputMaxY(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_CamGetInputMaxY_C());
	return ++_pst_Node;
}

// ************************************************************************************************************
//
//										C A M E R A   I N I T   C A S T   S P H E R E S
//
// ************************************************************************************************************

OBJ_tdst_GameObject		*	CAM_gapst_ObjToBeParsed[CAM_OBJ_MAX];		// Array of obj to be parsed
UCHAR						CAM_guc_NumberOfObjToBeParsed = 0;			// Arrays size

COL_tdst_Cob			*	CAM_gapst_CobToBeParsed[CAM_OBJ_MAX][CAM_COB_MAX];	// Array of Cob to be Parsed after all flag and rejection tests
UCHAR						CAM_gauc_NumberOfCobToBeParsed[CAM_OBJ_MAX];		// Array of cob number for each obj
UCHAR						CAM_guc_TotalNumberOfCobToBeParsed = 0;				// Total number of Cobs

// =======================================================================================================================
//	Process All Objects in the World
// =======================================================================================================================
BOOL	AI_CamCastProcessAllObjects(WOR_tdst_World	* _pst_World,INT _i_Options)
{
	TAB_tdst_PFelem			*pst_CurrentElem	=	NULL;		// CurrentElem Parsed 
	TAB_tdst_PFelem			*pst_EndElem		=	NULL;		// End of List
	OBJ_tdst_GameObject		*pst_CurrentObject	=	NULL;		// Current Object Parsed
	INT						i_ReturnValue		=	TRUE;	

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ColMap);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ColMap);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_CurrentObject = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if (!AI_CamCastProcessObject(_pst_World,pst_CurrentObject,_i_Options))
		{
			i_ReturnValue = FALSE;
		}
	}

	return i_ReturnValue;
}

// =======================================================================================================================
//	Process All Objects overlapping one object
// =======================================================================================================================
BOOL	AI_CamCastProcessOverlappingObjects(WOR_tdst_World	* _pst_World, OBJ_tdst_GameObject * _pst_GO,INT _i_Options)
{
	OBJ_tdst_GameObject		**ppst_Object			=	NULL;	// Current Gao Processed
	OBJ_tdst_GameObject		**ppst_LastObject		=	NULL;	// Last Gao to be Processed
	OBJ_tdst_GameObject		*pst_CurrentObject		=	NULL;	// Current Gao Processed
	COL_tdst_DetectionList	*pst_List				=	NULL;	// List Of Gaos to be Processed
	INT						i_ReturnValue			=	TRUE;	

	ERR_X_Assert(_pst_GO)
	ERR_X_Assert(_pst_GO->pst_Extended)
	ERR_X_Assert(_pst_GO->pst_Extended->pst_Col)

	pst_List = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_List;

	if (pst_List)
	{
		ppst_Object = pst_List->dpst_CollidedObject;
		ppst_LastObject = ppst_Object + pst_List->ul_NbCollidedObjects - 1;

		for(; ppst_Object <= ppst_LastObject; ppst_Object++)
		{
			pst_CurrentObject = (*ppst_Object);

			// Coll Map ?
			if(!OBJ_b_TestIdentityFlag(pst_CurrentObject, OBJ_C_IdentityFlag_ColMap)) 
			{
				continue;
			}

			if (!AI_CamCastProcessObject(_pst_World,pst_CurrentObject,_i_Options))
			{
				i_ReturnValue = FALSE;
			}
		}
	}
	return i_ReturnValue;
}

// =======================================================================================================================
//	Process One Object
// =======================================================================================================================
_inline_ BOOL	AI_CamCastProcessObject(WOR_tdst_World	* _pst_World, OBJ_tdst_GameObject * _pst_CurrentObject,INT _i_Options)
{
	COL_tdst_Base			*pst_CurrentColBase					=	NULL;	// Current Gao Col Basis
	COL_tdst_ColMap			*pst_CurrentColMap					=	NULL;	// Current Gao Col Map
	UCHAR					uc_CurrentNumberOfCobToBeParsed		=	0;		// Current Cob to be parder for _pst_CurrentObject
	UCHAR					uc_CurrentCobIndex					=	0;		// Index of Cob in Col
	COL_tdst_Cob			**ppst_CurrentCob					=	NULL;	// Current Cob of Gao'Col
	COL_tdst_Cob			*pst_CurrentCob						=	NULL;	// Current Cob of Gao'Col
	COL_tdst_Cob			**ppst_LastCob						=	NULL;	// Last Cob of Current Gao'Col
	INT						i_ReturnValue						=	TRUE;	


	// Hole ?
	if(TAB_b_IsAHole(_pst_CurrentObject))
	{
		return i_ReturnValue;
	}

	// CollMap ?
	ERR_X_Assert(OBJ_b_TestIdentityFlag(_pst_CurrentObject, OBJ_C_IdentityFlag_ColMap)) 

	// Active ?
	if(!OBJ_b_TestStatusFlag(_pst_CurrentObject, OBJ_C_StatusFlag_Active)) 
	{
		return i_ReturnValue;
	}
	// Active ?
	if(OBJ_b_TestControlFlag(_pst_CurrentObject, OBJ_C_ControlFlag_ForceInactive))
	{
		return i_ReturnValue;
	}

	pst_CurrentColBase = (COL_tdst_Base *) _pst_CurrentObject->pst_Extended->pst_Col;
	ERR_X_Assert(pst_CurrentColBase)
	if (!pst_CurrentColBase)
		return i_ReturnValue;

	pst_CurrentColMap = pst_CurrentColBase->pst_ColMap;
	ERR_X_Assert(pst_CurrentColMap)
	if (!pst_CurrentColMap)
		return i_ReturnValue;

	// Loop thru all the Current's ColMap 
	uc_CurrentNumberOfCobToBeParsed = 0;

	ppst_CurrentCob = pst_CurrentColMap->dpst_Cob;
	ppst_LastCob	= ppst_CurrentCob + pst_CurrentColMap->uc_NbOfCob;

	// Loop thru all the B's ZDRs 
	for(uc_CurrentCobIndex = 0; ppst_CurrentCob < ppst_LastCob; ppst_CurrentCob++, uc_CurrentCobIndex++)
	{

		if(!COL_b_ColMap_IsActive(pst_CurrentColMap, uc_CurrentCobIndex))
		{
			continue;
		}

		pst_CurrentCob = *ppst_CurrentCob;

		if(pst_CurrentCob->uc_Type == COL_C_Zone_Triangles)
		{
			if ((uc_CurrentNumberOfCobToBeParsed < CAM_COB_MAX) && (CAM_guc_NumberOfObjToBeParsed < CAM_OBJ_MAX))
			{
				CAM_gapst_CobToBeParsed[CAM_guc_NumberOfObjToBeParsed][uc_CurrentNumberOfCobToBeParsed] = pst_CurrentCob;
				uc_CurrentNumberOfCobToBeParsed++;
			}
			else
			{
				ERR_X_ErrorAssert(0,"[Warning] Assertion failed: '(uc_TmpNumberOfCobToBeParsed < CAM_COB_MAX) && (uc_NumberOfObjToBeParsed < CAM_OBJ_MAX)'\n","WTF ?");	
				i_ReturnValue = FALSE;
			}
		}
	}

	if (uc_CurrentNumberOfCobToBeParsed > 0)
	{
		if (CAM_guc_NumberOfObjToBeParsed < CAM_OBJ_MAX)
		{
			CAM_gapst_ObjToBeParsed[CAM_guc_NumberOfObjToBeParsed] = _pst_CurrentObject;
			CAM_gauc_NumberOfCobToBeParsed[CAM_guc_NumberOfObjToBeParsed] = uc_CurrentNumberOfCobToBeParsed;
			CAM_guc_NumberOfObjToBeParsed++;
		}
		else
		{
			ERR_X_ErrorAssert(0,"[Warning] Assertion failed: 'uc_NumberOfObjToBeParsed < CAM_OBJ_MAX'\n","WTF ?");
			i_ReturnValue = FALSE;
		}
		CAM_guc_TotalNumberOfCobToBeParsed += uc_CurrentNumberOfCobToBeParsed;
	}

	return i_ReturnValue;
}

// =======================================================================================================================
//	Init the Cast Sphere Process
// =======================================================================================================================
INT		AI_EvalFunc_CamCastInitSpheres_C(OBJ_tdst_GameObject * _pst_RootGO, OBJ_tdst_GameObject * _pst_ReferencedGO , 
										 INT _i_Options)
{
	WOR_tdst_World	*	pst_World;
	INT					i_ReturnValue = FALSE;

	// Init Process
	{	
		UINT dw;

		CAM_guc_NumberOfObjToBeParsed		= 0;
		CAM_guc_TotalNumberOfCobToBeParsed	= 0;

		for (dw = 0; dw < CAM_OBJ_MAX; ++dw)
		{
			CAM_gapst_ObjToBeParsed[dw]        = NULL;
			CAM_gauc_NumberOfCobToBeParsed[dw] = 0;
		}
	}

	pst_World = WOR_World_GetWorldOfObject(_pst_RootGO);

	if(!pst_World) 
		return i_ReturnValue;

	if (!_pst_ReferencedGO)
	{
		return AI_CamCastProcessAllObjects(pst_World,_i_Options);
	}

	i_ReturnValue = AI_CamCastProcessOverlappingObjects(pst_World,_pst_ReferencedGO,_i_Options);

	return i_ReturnValue;
}

//
AI_tdst_Node * AI_EvalFunc_CamCastInitSpheres(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *	pst_RootGO;
	OBJ_tdst_GameObject *	pst_ReferencedGO;
	INT						i_Options;

	AI_M_GetCurrentObject(pst_RootGO);

	i_Options = AI_PopInt();

	pst_ReferencedGO = AI_PopGameObject();

	AI_PushInt(AI_EvalFunc_CamCastInitSpheres_C(pst_RootGO,pst_ReferencedGO,i_Options));
	return ++_pst_Node;
}

// =======================================================================================================================
// Getter
// =======================================================================================================================
INT AI_EvalFunc_CamCastInitGetNumObj_C()
{
	return CAM_guc_NumberOfObjToBeParsed;
}
/**/
AI_tdst_Node *AI_EvalFunc_CamCastInitGetNumObj(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_CamCastInitGetNumObj_C());
	return ++_pst_Node;
}


// =======================================================================================================================
// Getter
// =======================================================================================================================
INT AI_EvalFunc_CamCastInitGetNumCob_C()
{
	return CAM_guc_TotalNumberOfCobToBeParsed;
}
/**/
AI_tdst_Node *AI_EvalFunc_CamCastInitGetNumCob(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_CamCastInitGetNumCob_C());
	return ++_pst_Node;
}

// ************************************************************************************************************
//
//										C A M E R A   C A S T   S P H E R E S
//
// ************************************************************************************************************

// Init Global Stuff
UINT	CAM_gui_NumberOfIterationDone	= 0;
ULONG	CAM_gul_NumberOfTriParsed		= 0;
ULONG	CAM_gul_NumberOfObjParsed		= 0;

// =======================================================================================================================
// Sphere Sphere Overlap Test
// =======================================================================================================================
#define AreSpheresOverlapped(_pst_CenterA, f_RadiusA, _pst_CenterB, f_RadiusB) INT_SphereSphere(_pst_CenterA, f_RadiusA, _pst_CenterB, f_RadiusB)

// =======================================================================================================================
// Axe Oriented BBox Versus Sphere Overlap Test
// =======================================================================================================================
#define AreAABBoxAndSphereOverlapped(BoxMin, BoxMax, Center, Radius) INT_SphereAABBox(Center, Radius, BoxMin, BoxMax)

// =======================================================================================================================
// Project X on Segment IJ
// =======================================================================================================================
_inline_ void NearestPointOnSegment (MATH_tdst_Vector * _pst_PointResult, MATH_tdst_Vector * _pst_PointX,
									 MATH_tdst_Vector * _pst_SegIJ, MATH_tdst_Vector * _pst_PointI)
{
	FLOAT	f_SegLength, f_SegRatio;

	MATH_SubVector(_pst_PointResult, _pst_PointX, _pst_PointI);

	f_SegRatio = MATH_f_DotProduct(_pst_SegIJ,_pst_PointResult);

	if ( f_SegRatio <= 0.0f )
	{
		f_SegRatio = 0.0f;    
	}
	else    
	{
		f_SegLength	= MATH_f_SqrNormVector(_pst_SegIJ);

		if ( f_SegRatio >= f_SegLength )        
		{
			f_SegRatio = 1.0f;
		}
		else
		{
			f_SegRatio /= f_SegLength;
		}
	}

	MATH_ScaleVector(_pst_PointResult,_pst_SegIJ,f_SegRatio);
	MATH_AddVector(_pst_PointResult,_pst_PointI,_pst_PointResult);

}

// =======================================================================================================================
// Axe Oriented BBox Versus Sphere Overlap Test
// =======================================================================================================================

_inline_ BOOL AreFaceAndSphereOverlapped( COL_tdst_IndexedTriangles *  _pst_TrianglesGeo,
										 COL_tdst_IndexedTriangle *  _pst_Triangle, MATH_tdst_Vector * _pst_Normal,
										 MATH_tdst_Vector *  _pst_Center, FLOAT _f_Radius)
{
	MATH_tdst_Vector			*A, *B, *C,*X;
	MATH_tdst_Vector			TMP,CA,AB,BC,U,PtAB,PtBC,PtCA;
	USHORT						a, b, c;
	FLOAT						distPlane,distEdge,radius2,dAB,dBC,dCA,dist;
	BOOL						inTriangleZone = TRUE;

	// Degenerated Faces : Ignored
	if (MATH_f_SqrNormVector(_pst_Normal) < 0.1)
	{
		return FALSE;
	}

	// We get the triangles index
	a = _pst_Triangle->auw_Index[0];
	b = _pst_Triangle->auw_Index[1];
	c = _pst_Triangle->auw_Index[2];

	// We get the triangles points
	A = &_pst_TrianglesGeo->dst_Point[a];
	B = &_pst_TrianglesGeo->dst_Point[b];
	C = &_pst_TrianglesGeo->dst_Point[c];

	// X the center of Sphere
	X	= _pst_Center;

	// TMP = AX
	MATH_SubVector(&TMP, X, A);

	// Distance from X to Face Plane
	distPlane = MATH_f_DotProduct(&TMP, _pst_Normal);

	if((distPlane < -_f_Radius) || (distPlane > _f_Radius))
	{
		// Sphere is outside plane and far enough  
		return FALSE;	
	} 

	radius2 = _f_Radius * _f_Radius;


	// Distance from X to AC -----------
	MATH_SubVector(&CA, A, C);
	MATH_CrossProduct(&U,&CA,_pst_Normal);
	distEdge = MATH_f_DotProduct(&TMP, &U);	

	// We are toward outside of triangle ?
	if (distEdge > 0.0f)
	{
		// Yes
		inTriangleZone = FALSE;
		// But Far enough ?
		if (distEdge*distEdge > _f_Radius*MATH_f_SqrNormVector(&U))
		{ 
			// Outside Triangle and far enough
			return FALSE;	
		}
	} 

	// Distance from X to AB -----------
	MATH_SubVector(&AB, B, A);
	MATH_CrossProduct(&U,&AB,_pst_Normal);
	distEdge = MATH_f_DotProduct(&TMP, &U);	

	// We are toward outside of triangle ?
	if (distEdge > 0.0f)
	{
		// Yes
		inTriangleZone = FALSE;
		// But Far enough ?
		if (distEdge*distEdge > _f_Radius*MATH_f_SqrNormVector(&U))
		{ 
			// Outside Triangle and far enough
			return FALSE;	
		}
	} 

	// Distance from X to BC -----------	
	// We need to compute BX : TMP = BX
	MATH_SubVector(&TMP, X, B);
	MATH_SubVector(&BC, C, B);
	MATH_CrossProduct(&U,&BC,_pst_Normal);
	distEdge = MATH_f_DotProduct(&TMP, &U);	

	// We are toward outside of triangle ?
	if (distEdge > 0.0f)
	{
		// Yes
		inTriangleZone = FALSE;
		// But Far enough ?
		if (distEdge*distEdge > _f_Radius*MATH_f_SqrNormVector(&U))
		{ 
			// Outside Triangle and far enough
			return FALSE;	
		}
	} 

	// We are in triangle zone and to near from plane
	if(inTriangleZone)
	{
		return TRUE;
	}
	else
	{
		NearestPointOnSegment(&PtAB,X,&AB,A);
		NearestPointOnSegment(&PtBC,X,&BC,B);
		NearestPointOnSegment(&PtCA,X,&CA,C);

		MATH_SubVector(&AB,X,&PtAB);
		MATH_SubVector(&BC,X,&PtBC);
		MATH_SubVector(&CA,X,&PtCA);

		dAB=MATH_f_SqrNormVector(&AB);
		dBC=MATH_f_SqrNormVector(&BC);
		dCA=MATH_f_SqrNormVector(&CA);

		if (dAB<dBC)
		{
			if (dAB<dCA)
			{
				// AB
				if (dAB <= radius2) 
				{
					return TRUE;
				}
			}
			else
			{
				// CA
				if (dCA <= radius2) 
				{
					return TRUE;
				}
			}
		}
		else
		{
			if (dBC<dCA) 
			{
				//BC
				if (dBC <= radius2) 
				{
					return TRUE;
				}
			}
			else 
			{
				// CA
				dist = fSqrt(dCA);
				if (dist <= radius2) 
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

// =======================================================================================================================
// Cast one Sphere
// =======================================================================================================================
_inline_	BOOL	AI_CamCastTestCrossable(COL_tdst_GameMat * _pst_Cob_ElementGMat, INT _i_Options)
{
	if (!_pst_Cob_ElementGMat)
		return FALSE;

	if (_i_Options & CAM_SCAST_OPTIONS_USE_SPECIFICCROSSABLE)
	{
		if(((USHORT) _pst_Cob_ElementGMat->ul_CustomBits) & COL_guw_UserCrossable)
		{
			return TRUE;
		}
	}
	return FALSE;
}

// =======================================================================================================================
// Cast one Sphere
// =======================================================================================================================
INT AI_EvalFunc_CamCastSphere_C(OBJ_tdst_GameObject * _pst_RootGO, MATH_tdst_Vector *  _pst_CastInitPos, FLOAT _f_Radius, FLOAT _f_CastDistInc, MATH_tdst_Vector * _pst_CastDir, MATH_tdst_Vector * _pst_ResultPoint, INT _i_Options)
{
	OBJ_tdst_GameObject					*pst_CurrentObject		= NULL;		// Current Gao Processed
	COL_tdst_Cob						*pst_Current_Cob		= NULL;		// Current Gao Col Cob

	COL_tdst_IndexedTriangles	*		pst_CurrentCob_Geo		= NULL;		// Geometry of Cob
	ULONG								ul_Triangle				= 0;		// Index of Parsed Triangle in ALL Parsing
	USHORT								uw_Triangle				= 0;		// Index of Parsed Triangle in OK3 Parsing

	ULONG								ul_Element				= 0;		// Index of Element in Geo in ALL Parsing
	ULONG								ul_ElementTriangle		= 0;		// Index of Triangle in Element in ALL Parsing

	COL_tdst_IndexedTriangle			*pst_Triangle			= NULL;		// Current Triangle in ALL Parsing
	COL_tdst_IndexedTriangle			*pst_LastTriangle		= NULL;		// Last Triangle in ALL Parsing
	COL_tdst_ElementIndexedTriangles	*pst_Element			= NULL;		// Current Geo Element in ALL Parsing
	COL_tdst_ElementIndexedTriangles	*pst_LastElement		= NULL;		// Last Geo Element in ALL Parsing

	COL_tdst_OK3						*pst_OK3				= NULL;		// OK3 of Geom
	COL_tdst_OK3_Box					*pst_Box				= NULL;		// OK3 Current Box
	COL_tdst_OK3_Box					*pst_LastBox			= NULL;		// OK3 Last Box
	COL_tdst_OK3_Element				*pst_OK3_Element		= NULL;		// OK3 Current Box Element
	COL_tdst_OK3_Element				*pst_OK3_LastElement	= NULL;		// OK3 Last Box Element
	COL_tdst_ElementIndexedTriangles	*pst_Cob_Element		= NULL;		// OK3 Current Element 
	COL_tdst_GameMat					*pst_Cob_ElementGMat	= NULL;		// Material of Element

	MATH_tdst_Vector					*pst_Normal				= NULL;		// Normal of Triangle



	ULONG								ul_Save					= 0;		// Number of triangle for all precedent OK3 element
	int									i = 0, obj = 0, cob		= 0;		// Indexes for some stuff

	BOOL								b_CollisionOccured		= FALSE;	// We touched something ?

	MATH_tdst_Vector					st_SphereCenter;					// Center of the current Sphere
	MATH_tdst_Vector					st_LastSphereCenter;				// Last Processed Sphere Pos
	MATH_tdst_Vector					st_TmpVector;						// Tmp Vector
	MATH_tdst_Vector					st_SphereCenterInB;					// Center of the current Sphere in Current Object Space
	MATH_tdst_Vector					st_SphereCenterInBTrans;			// Center of the current Sphere in Current Object Space (Just Translation)
	MATH_tdst_Matrix					st_BInvMatrix;						// Current Object Inv Matrix
	MATH_tdst_Vector					st_BInvTranslation;					// Current Object Inv Translation

	MATH_tdst_Vector					st_CastDirNormalized;				// Cast Dir normalized
	FLOAT								f_CastDistToGo			= 0;		// Cast Distance
	FLOAT								f_CastDistInc			= 0;		// Cast Distance Inc between 2 spheres
	FLOAT								f_CastCurrentDist		= 0;		// Current Distance gone thru


	void								*pst_CurrentObjectBV		= NULL;		// Current Parsed Object BV
	BOOL								b_BVOverlapp				= FALSE;	// Sphere and current parsed object BV Overlapped ?

	INT									i_Sphere					= -1;		// Current Sphere launched index
	INT									i_ReturnValue				= FALSE;	// return Value

	// Init Global Stuff
	CAM_gui_NumberOfIterationDone	= 0;
	CAM_gul_NumberOfTriParsed		= 0;
	CAM_gul_NumberOfObjParsed		= 0;

	// Init Cast Stuff 
	MATH_CopyVector(&st_CastDirNormalized,_pst_CastDir);
	f_CastDistToGo = MATH_f_NormVector(&st_CastDirNormalized); 
	MATH_DivVector(&st_CastDirNormalized,&st_CastDirNormalized,f_CastDistToGo);
	f_CastCurrentDist	= 0;
	f_CastDistInc		= _f_CastDistInc;
	st_SphereCenter 	= *_pst_CastInitPos;
	b_CollisionOccured	= FALSE;

	// Going thru casting
	while (f_CastCurrentDist < f_CastDistToGo && !b_CollisionOccured)
	{		
		// Next Sphere
		CAM_gui_NumberOfIterationDone++;
		i_Sphere++; 
		f_CastCurrentDist += f_CastDistInc;
		if (f_CastCurrentDist >= f_CastDistToGo)
		{
			f_CastCurrentDist = f_CastDistToGo;
		}
		st_LastSphereCenter = st_SphereCenter;
		MATH_MulVector(&st_TmpVector,&st_CastDirNormalized,f_CastCurrentDist);
		MATH_AddVector(&st_SphereCenter,_pst_CastInitPos,&st_TmpVector);

		// Going Thru Object 
		for (obj = 0 ; obj < CAM_guc_NumberOfObjToBeParsed ; obj++)
		{
			pst_CurrentObject = CAM_gapst_ObjToBeParsed[obj];

			pst_CurrentObjectBV = pst_CurrentObject->pst_BV;

			b_BVOverlapp = TRUE;

			MATH_NegVector(&st_BInvTranslation, MATH_pst_GetTranslation(pst_CurrentObject->pst_GlobalMatrix));
			MATH_AddVector(&st_SphereCenterInBTrans, &st_BInvTranslation,&st_SphereCenter);

			if(OBJ_BV_IsAABBox(pst_CurrentObjectBV))
			{
				b_BVOverlapp = AreAABBoxAndSphereOverlapped(OBJ_pst_BV_GetGMin(pst_CurrentObjectBV),OBJ_pst_BV_GetGMax(pst_CurrentObjectBV),
					&st_SphereCenterInBTrans,_f_Radius);
			}
			else if (OBJ_BV_IsSphere(pst_CurrentObjectBV))
			{
				b_BVOverlapp = AreSpheresOverlapped(OBJ_pst_BV_GetCenter(pst_CurrentObjectBV), OBJ_f_BV_GetRadius(pst_CurrentObjectBV),
					&st_SphereCenterInBTrans,_f_Radius);

			}
			if (!b_BVOverlapp)
			{
				continue;
			}

			CAM_gul_NumberOfObjParsed++;

			MATH_InvertMatrix(&st_BInvMatrix, pst_CurrentObject->pst_GlobalMatrix);
			MATH_TransformVertexNoScale(&st_SphereCenterInB,&st_BInvMatrix,&st_SphereCenter);

			for (cob = 0 ; cob < CAM_gauc_NumberOfCobToBeParsed[obj] ; cob++)
			{
				pst_Current_Cob = CAM_gapst_CobToBeParsed[obj][cob];

				pst_CurrentCob_Geo = pst_Current_Cob->pst_TriangleCob;

				// Octree ?
				if(pst_CurrentCob_Geo->pst_OK3)
				{
					pst_OK3 = pst_CurrentCob_Geo->pst_OK3;
					pst_Box = pst_OK3->pst_OK3_Boxes;
					pst_LastBox = pst_Box +	pst_OK3->ul_NumBox;

					for(; pst_Box < pst_LastBox; pst_Box++)
					{
						if (!AreAABBoxAndSphereOverlapped(&pst_Box->st_Min,&pst_Box->st_Max,&st_SphereCenterInB,_f_Radius))
						{
							continue;
						}

						pst_OK3_Element = pst_Box->pst_OK3_Element;
						pst_OK3_LastElement = pst_OK3_Element +	pst_Box->ul_NumElement;

						for(; pst_OK3_Element < pst_OK3_LastElement; pst_OK3_Element++)
						{
							pst_Cob_Element = &pst_CurrentCob_Geo->dst_Element[pst_OK3_Element->uw_Element];

							pst_Cob_ElementGMat = COL_pst_GMat_Get(pst_Current_Cob, pst_Cob_Element);
							if (AI_CamCastTestCrossable(pst_Cob_ElementGMat,_i_Options))
							{
								continue;
							}

							for(ul_Save = 0, i = 0; i < pst_OK3_Element->uw_Element; i++)
							{
								ul_Save += pst_CurrentCob_Geo->dst_Element[i].uw_NbTriangles;;
							}

							for(i = 0; i < pst_OK3_Element->uw_NumTriangle; i++)
							{
								uw_Triangle = pst_OK3_Element->puw_OK3_Triangle[i];
								pst_Triangle = &pst_Cob_Element->dst_Triangle[uw_Triangle];
								pst_Normal = (pst_CurrentCob_Geo->dst_FaceNormal + ul_Save + uw_Triangle);

								CAM_gul_NumberOfTriParsed++;

								if (AreFaceAndSphereOverlapped(pst_CurrentCob_Geo,pst_Triangle,pst_Normal,&st_SphereCenterInB,_f_Radius))
								{
									b_CollisionOccured = TRUE;
									goto Collision;
								}
							}
						}
					}
				}
				else
				{
					pst_Element = pst_CurrentCob_Geo->dst_Element;
					pst_LastElement = (pst_Element + pst_CurrentCob_Geo->l_NbElements);

					// We go thru all the Elements of the Geometric object.
					for(ul_Element = 0, ul_Triangle = 0; pst_Element < pst_LastElement; pst_Element++, ul_Element++)
					{
						pst_Cob_ElementGMat = COL_pst_GMat_Get(pst_Current_Cob, pst_Element);
						if (AI_CamCastTestCrossable(pst_Cob_ElementGMat,_i_Options))
						{
							continue;
						}

						pst_Triangle = pst_Element->dst_Triangle;
						pst_LastTriangle = (pst_Triangle + pst_Element->uw_NbTriangles);
						for (ul_ElementTriangle = 0;pst_Triangle < pst_LastTriangle;pst_Triangle++, ul_Triangle++, ul_ElementTriangle++)
						{
							pst_Normal = (pst_CurrentCob_Geo->dst_FaceNormal + ul_Triangle);

							CAM_gul_NumberOfTriParsed++;

							if (AreFaceAndSphereOverlapped(pst_CurrentCob_Geo,pst_Triangle,pst_Normal,&st_SphereCenterInB,_f_Radius))
							{							
								b_CollisionOccured = TRUE;
								goto Collision;
							}
						}
					}
				}
			}
		}
Collision:
		if (b_CollisionOccured)
		{
			MATH_CopyVector(_pst_ResultPoint,&st_LastSphereCenter);
			i_ReturnValue = TRUE;
		}
		else if (f_CastCurrentDist >= f_CastDistToGo)
		{
			MATH_CopyVector(_pst_ResultPoint,&st_SphereCenter);
			i_ReturnValue = FALSE;
		}
	}

	return i_ReturnValue;
}
//
AI_tdst_Node * AI_EvalFunc_CamCastSphere(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *	pst_RootGO;
	MATH_tdst_Vector		st_TargetPos;
	MATH_tdst_Vector		st_CastDir;
	FLOAT					f_Radius;
	FLOAT					f_DistInc;
	AI_tdst_PushVar 		st_ResultPos;
	AI_tdst_UnionVar		st_Val;
	INT						i_Options;


	AI_M_GetCurrentObject(pst_RootGO);
	i_Options = AI_PopInt();

	AI_PopVar(&st_Val, &st_ResultPos);
	AI_PopVector(&st_CastDir);
	f_DistInc	= AI_PopFloat();
	f_Radius	= AI_PopFloat();
	AI_PopVector(&st_TargetPos);
	AI_PushInt(AI_EvalFunc_CamCastSphere_C(pst_RootGO,&st_TargetPos,f_Radius,f_DistInc,&st_CastDir,(MATH_tdst_Vector *) st_ResultPos.pv_Addr,i_Options));

	return ++_pst_Node;
}

// =======================================================================================================================
// Getter
// =======================================================================================================================
INT AI_EvalFunc_CamCastGetNumTri_C()
{
	return CAM_gul_NumberOfTriParsed;
}
/**/
AI_tdst_Node *AI_EvalFunc_CamCastGetNumTri(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_CamCastGetNumTri_C());
	return ++_pst_Node;
}

// =======================================================================================================================
// Getter
// =======================================================================================================================
INT AI_EvalFunc_CamCastGetNumIter_C()
{
	return CAM_gui_NumberOfIterationDone;
}
/**/
AI_tdst_Node *AI_EvalFunc_CamCastGetNumIter(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_CamCastGetNumIter_C());
	return ++_pst_Node;
}

// =======================================================================================================================
// Getter
// =======================================================================================================================
INT AI_EvalFunc_CamCastGetNumObj_C()
{
	return CAM_gul_NumberOfObjParsed;
}
/**/
AI_tdst_Node *AI_EvalFunc_CamCastGetNumObj(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_CamCastGetNumObj_C());
	return ++_pst_Node;
}
