/*$T VAVview_zone.cpp GC!1.67 01/05/00 15:34:33 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLedit.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#ifdef ODE_INSIDE
#include "ENGine/Sources/WORld/WORstruct.h"
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EVAV_Zone_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_ZDx    *pst_ZDx;
    ULONG           ul_Type;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_ZDx = (COL_tdst_ZDx *) _p_Value;
    ul_Type = COL_Zone_GetType(pst_ZDx);

    switch(ul_Type)
    {
    case COL_C_Zone_Box: return "Box";

    case COL_C_Zone_Sphere: return "Sphere";
    }

    return "Error";
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_Zone_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_ZDx			*pst_ZDx;
	OBJ_tdst_GameObject		*pst_GO;
    ULONG					ul_Type;
#ifdef ODE_INSIDE
	BOOL					b_ODE;
	DYN_tdst_ODE			*pst_ODE;
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_ZDx = (COL_tdst_ZDx *) _p_Value;
    ul_Type = COL_Zone_GetType(pst_ZDx);

	pst_GO = pst_ZDx->pst_GO;
#ifdef ODE_INSIDE
	b_ODE = FALSE;
	if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && (pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY))
	{
		b_ODE = TRUE;
		pst_ODE = pst_GO->pst_Base->pst_ODE;
	}
#endif

    switch(ul_Type)
    {
    case COL_C_Zone_Box:
        COL_Zone_BoxToSphere(pst_ZDx);
		pst_ZDx->uc_Type = COL_C_Zone_Sphere;

#ifdef ODE_INSIDE
		if(b_ODE)
		{
			dMass		Mass;
			pst_ODE->uc_Type = ODE_TYPE_SPHERE;
			pst_ODE->f_X = COL_f_Shape_GetRadius(pst_ZDx->p_Shape);
			dMassSetSphereTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X); 

			dGeomDestroy(pst_ODE->ode_id_geom);
			pst_ODE->ode_id_geom = dCreateSphere(pst_GO->pst_World->ode_id_space, pst_ODE->f_X);
			dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
			dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		}
#endif
        break;

    case COL_C_Zone_Sphere:
        COL_Zone_SphereToBox(pst_ZDx);
		pst_ZDx->uc_Type = COL_C_Zone_Box;

#ifdef ODE_INSIDE
		if(b_ODE)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Diag;
			dMass				Mass;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_SubVector(&st_Diag, COL_pst_Shape_GetMax(pst_ZDx->p_Shape), COL_pst_Shape_GetMin(pst_ZDx->p_Shape));
			pst_ODE->uc_Type = ODE_TYPE_BOX;
			pst_ODE->f_X = st_Diag.x;
			pst_ODE->f_Y = st_Diag.y;
			pst_ODE->f_Z = st_Diag.z;
			dMassSetBoxTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z); 

			dGeomDestroy(pst_ODE->ode_id_geom);
			pst_ODE->ode_id_geom = dCreateSphere(pst_GO->pst_World->ode_id_space, pst_ODE->f_X);
			dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
			dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		}
#endif
        break;
    }

    /* We update the GameObject Pointer */
    LINK_UpdatePointer(pst_ZDx);

    LINK_UpdatePointers();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EVAV_Cob_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_Cob    *pst_Cob;
    ULONG           ul_Type;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Cob = (COL_tdst_Cob *) _p_Value;
    ul_Type = COL_Cob_GetType(pst_Cob);

    switch(ul_Type)
    {
    case COL_C_Zone_Box: return "Box";

    case COL_C_Zone_Sphere: return "Sphere";

    case COL_C_Zone_Cylinder: return "Cylinder";

    case COL_C_Zone_Triangles: return "Triangles";
    }

    return "Error";
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_Cob_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_Cob    *pst_Cob;
    ULONG           ul_Type;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Cob = (COL_tdst_Cob *) _p_Value;
    ul_Type = COL_Cob_GetType(pst_Cob);

    switch(ul_Type)
    {
    case COL_C_Zone_Sphere:
        COL_Cob_SphereToBox(pst_Cob);
		pst_Cob->uc_Type = COL_C_Zone_Box;
        break;

    case COL_C_Zone_Box:
        COL_Cob_BoxToCylinder(pst_Cob);
		pst_Cob->uc_Type = COL_C_Zone_Cylinder;
        break;

    case COL_C_Zone_Cylinder:
        COL_Cob_CylinderToSphere(pst_Cob);
		pst_Cob->uc_Type = COL_C_Zone_Sphere;
        break;


    }

    /* We update the GameObject Pointer */
    LINK_UpdatePointer(pst_Cob);

    LINK_UpdatePointers();
}


#endif /* ACTIVE_EDITORS */
