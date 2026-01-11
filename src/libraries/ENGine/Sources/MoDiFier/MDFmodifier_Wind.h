/*$T MDFmodifier_XMEC.h GC! 1.100 08/29/01 14:29:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_WIND_H__
#define __MDFMODIFIER_WIND_H__

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/Wind/WindSource.h"

// ***********************************************************************************************************************
//    Modifier desc
// ***********************************************************************************************************************

#define Wind_C_VersionNumber 0

#define AI_Modifier_WindNear		0
#define AI_Modifier_WindFar			1
#define AI_Modifier_WindBehindPlane 2
#define AI_Modifier_WindAmplitude	3
#define AI_Modifier_WindMinForce	4
#define AI_Modifier_WindMaxForce	5

#ifdef ACTIVE_EDITORS
extern BOOL Wind_gb_Display;
#endif // #ifdef ACTIVE_EDITORS


struct GEO_tdst_Object_;

typedef struct	GAO_tdst_ModifierWind_
{
    CWindSource * po_Source;

} GAO_tdst_ModifierWind;

// ***********************************************************************************************************************
//    Modifier functions
// ***********************************************************************************************************************

void  GAO_ModifierWind_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data);
void  GAO_ModifierWind_Destroy(MDF_tdst_Modifier *_pst_Mod);
void  GAO_ModifierWind_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object_ *_pst_Obj);
void  GAO_ModifierWind_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object_ *_pst_Obj);
void  GAO_ModifierWind_Reinit(MDF_tdst_Modifier *_pst_Mod);
ULONG GAO_ModifierWind_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);

#ifdef ACTIVE_EDITORS
void  GAO_ModifierWind_Save(MDF_tdst_Modifier *_pst_Mod);
void  GAO_ModifierWind_Refresh(MDF_tdst_Modifier *_pst_Mod);
void  GAO_ModifierWind_Helper_ComputeMatrix(MDF_tdst_Modifier *_pst_Mod, MATH_tdst_Matrix * _pst_Matrix);
void  GAO_ModifierWind_Helper_Move(MDF_tdst_Modifier *_pst_Mod, MATH_tdst_Vector * _pst_Move);
void  GAO_ModifierWind_Helper_Scale(MDF_tdst_Modifier *_pst_Mod, MATH_tdst_Vector * _pst_Scale);
void  GAO_ModifierWind_Helper_Rotate(MDF_tdst_Modifier *_pst_Mod, MATH_tdst_Vector * _pst_Axis, FLOAT _f_Angle);
#endif // #ifdef ACTIVE_EDITORS

#endif // #ifndef __MDFMODIFIER_WIND_H__

