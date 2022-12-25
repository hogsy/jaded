/*$T VAVview_bounding.cpp GC!1.5 10/11/99 16:08:20 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_BV_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_SingleBV   *pst_BV;
    OBJ_tdst_GameObject *pst_GO;
    OBJ_tdst_GameObject st_GO;
    USHORT              us_Offset;  /* Offset to get the pst_BV from the beginning of the GO
                                     * structure. */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* WARNING, here the _p_Value represents the address of the pointer on OBJ_tdst_BV */
    pst_BV = (OBJ_tdst_SingleBV *) * (long *) _p_Value;

    /* We want to obtain the pointer on the Game object. */
    us_Offset = (char *) &st_GO.pst_BV - (char *) &st_GO;
    pst_GO = (OBJ_tdst_GameObject *) ((char *) _p_Value - us_Offset);

    if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_OBBox))
    {
        if(OBJ_BV_IsSphere(pst_BV))
            return "Sphere + OBBox";
        else
            return "AABBox + OBBox";
    }
    else
    {
        if(OBJ_BV_IsSphere(pst_BV))
            return "Sphere";
        else
            return "AABBox";
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_BV_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_SingleBV   *pst_BV;
    OBJ_tdst_GameObject *pst_GO;
    OBJ_tdst_GameObject st_GO;
    USHORT              us_Offset;  /* Offset to get the pst_BV from the beginning of the GO
                                     * structure. */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_BV = (OBJ_tdst_SingleBV *) * (long *) _p_Value;

    /* We want to obtain the pointer on the Game object. */
    us_Offset = (char *) &st_GO.pst_BV - (char *) &st_GO;
    pst_GO = (OBJ_tdst_GameObject *) ((char *) _p_Value - us_Offset);

	/*$F
    // If the BV is a sphere, we change it to a AABBox 
    if(OBJ_BV_IsAABBox(pst_BV))
        OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_Sphere);
    else
        OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_AABBox);
	*/

	OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_AABBox);

    /* We update the GameObject Pointer */
    LINK_UpdatePointer(pst_GO);
    LINK_UpdatePointers();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_ODE_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
#ifdef ODE_INSIDE
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE		*pst_ODE;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_ODE = (DYN_tdst_ODE *) (long *) _p_Value;

	switch(pst_ODE->uc_Type)
	{
	case ODE_TYPE_SPHERE:
		if(pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
			return "Sphere+RigidBody";
		else
			return "Sphere+Immovable";

	case ODE_TYPE_BOX:
		if(pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
			return "Box+RigidBody";
		else
			return "Box+Immovable";

	case ODE_TYPE_CYLINDER:
		if(pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
			return "Cylinder+RigidBody";
		else
			return "Cylinder+Immovable";

	case ODE_TYPE_PLAN:
			return "Plan+Immovable";

	}

	return "Immovable";
#else
	return "Immovable";

#endif
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_ODE_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
#ifdef ODE_INSIDE
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE		*pst_ODE;
    OBJ_tdst_GameObject *pst_GO;
	dMass				Mass;
	float				mass;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_ODE = (DYN_tdst_ODE *) (long *) _p_Value;
	pst_GO = pst_ODE->pst_GO;

	mass = 10.0f;

	switch(pst_ODE->uc_Type)
	{
	case ODE_TYPE_SPHERE:
		pst_ODE->uc_Flags |= ODE_FLAGS_MATHCOLMAP;
		pst_ODE->uc_Type = ODE_TYPE_BOX;
		pst_ODE->f_X = 1.0;
		pst_ODE->f_Y = 1.0;
		pst_ODE->f_Z = 1.0;
		dMassSetBoxTotal(&Mass, mass, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z); 

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateBox(pst_GO->pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_BOX:
		{
			MATH_tdst_Vector	st_Norm;

			pst_ODE->uc_Flags |= ODE_FLAGS_MATHCOLMAP;
			pst_ODE->uc_Type = ODE_TYPE_PLAN;
			MATH_InitVector(&st_Norm, pst_ODE->pst_GO->pst_GlobalMatrix->Kx, pst_ODE->pst_GO->pst_GlobalMatrix->Ky, pst_ODE->pst_GO->pst_GlobalMatrix->Kz);

			pst_ODE->f_X = MATH_f_DotProduct(&pst_GO->pst_GlobalMatrix->T, &st_Norm);
			pst_ODE->f_Y = 0.0;
			pst_ODE->f_Z = 0.0;

			dGeomDestroy(pst_ODE->ode_id_geom);
			pst_ODE->ode_id_geom = dCreatePlane(pst_GO->pst_World->ode_id_space, pst_ODE->pst_GO->pst_GlobalMatrix->Kx, pst_ODE->pst_GO->pst_GlobalMatrix->Ky, pst_ODE->pst_GO->pst_GlobalMatrix->Kz, pst_ODE->f_X);
			dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		}
		break;

	case ODE_TYPE_PLAN:
		pst_ODE->uc_Flags |= ODE_FLAGS_MATHCOLMAP;
		pst_ODE->uc_Type = ODE_TYPE_CYLINDER;
		pst_ODE->f_X = 1.0;
		pst_ODE->f_Y = 1.0;
		pst_ODE->f_Z = 1.0;
		dMassSetCylinderTotal(&Mass, mass, (int) pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z); 

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateCylinder(pst_GO->pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);

		break;


	case ODE_TYPE_CYLINDER:
	case 0:
		pst_ODE->uc_Flags |= ODE_FLAGS_MATHCOLMAP;

		pst_ODE->uc_Flags |= ODE_FLAGS_RIGIDBODY;
		pst_ODE->ode_id_body = dBodyCreate(pst_GO->pst_World->ode_id_world);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);

		dBodySetData(pst_ODE->ode_id_body, pst_GO);

		pst_ODE->uc_Type = ODE_TYPE_SPHERE;

		pst_ODE->f_X = 1.0;
		pst_ODE->f_AngularThres = 0.2f;
		pst_ODE->f_LinearThres = 0.2f;
		pst_ODE->mass_init = 10.0f;

		dMassSetSphereTotal(&Mass, mass, pst_ODE->f_X); 

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateSphere(pst_GO->pst_World->ode_id_space, pst_ODE->f_X);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);

		break;

	}


	if(pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
	{
		if(pst_ODE->uc_Flags & ODE_FLAGS_AUTODISABLE)
		{
			dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 1);
			dBodySetAutoDisableLinearThreshold(pst_ODE->ode_id_body, pst_ODE->f_LinearThres);
			dBodySetAutoDisableAngularThreshold(pst_ODE->ode_id_body, pst_ODE->f_AngularThres);
			dBodySetAutoDisableSteps(pst_ODE->ode_id_body, 10);
		}
		else 
		{
			dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 0);
		}
	}


    /* We update the GameObject Pointer */
    LINK_UpdatePointer(pst_GO);
    LINK_UpdatePointers();
#endif
}




#endif /* ACTIVE_EDITORS */
