// MDFmodifier_Wind.c


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#include "Precomp.h"
#include "MDFmodifier_Wind.h"
#include "ENGine/Sources/Wind/WindManager.h"
#include "BASe/BENch/BENch.h"
#include "GEOmetric/GEOobject.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "ENGine/Sources/OBJects/OBJorient.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif //#ifdef ACTIVE_EDITORS


#ifdef ACTIVE_EDITORS
BOOL Wind_gb_Display = TRUE;
#endif // #ifdef ACTIVE_EDITORS


// ***********************************************************************************************************************
//    Wind modifier:: Modifier apply Wind on world
// ***********************************************************************************************************************

// =======================================================================================================================
// =======================================================================================================================

void GAO_ModifierWind_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	GAO_tdst_ModifierWind * p_Wind;

	_pst_Mod->p_Data = MEM_p_Alloc( sizeof(GAO_tdst_ModifierWind) );
	p_Wind = (GAO_tdst_ModifierWind *) _pst_Mod->p_Data;

	if(p_Data == NULL)
	{
		/* First init */
		//L_memset(_pst_Mod->p_Data, 0, sizeof(GAO_tdst_ModifierWind));
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;

        p_Wind->po_Source = new CWindSource;

        #ifdef ACTIVE_EDITORS
        p_Wind->po_Source->SetSizePlane(1.0f, 1.0f);
        p_Wind->po_Source->SetAmplitude(0.0f);
        p_Wind->po_Source->SetShape(0.0f);
        p_Wind->po_Source->SetFrequency(0.0f);
        p_Wind->po_Source->SetSpeed(2.0f);
        p_Wind->po_Source->SetMinForce(0.0f);
        p_Wind->po_Source->SetMaxForce(1.0f);
		p_Wind->po_Source->SetDynamic(FALSE);
		p_Wind->po_Source->SetRadial(FALSE);
		p_Wind->po_Source->SetNear(5.0f);
		p_Wind->po_Source->SetFar(10.0f);
        #endif // #ifdef ACTIVE_EDITORS
    }
	else
	{
        // Copy modifier Wind
        GAO_tdst_ModifierWind * p_SrcWind = (GAO_tdst_ModifierWind *)p_Data;

		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
		//L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierWind));

        p_Wind->po_Source = new CWindSource;

        *p_Wind->po_Source = *p_SrcWind->po_Source;
	}

  	//_pst_Mod->pst_GO->us_Runtime_IdentityFlag |= OBJ_C_RTIdentityFlag_IsWind;
}

// =======================================================================================================================
// =======================================================================================================================
void GAO_ModifierWind_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
    GAO_tdst_ModifierWind * p_Wind;

    p_Wind = (GAO_tdst_ModifierWind *) _pst_Mod->p_Data;

    CWindManager::GetInstance()->RemoveWindSource(p_Wind->po_Source);
    p_Wind->po_Source->DeleteRef();
	MEM_Free(_pst_Mod->p_Data);

  	//_pst_Mod->pst_GO->us_Runtime_IdentityFlag &= ~OBJ_C_RTIdentityFlag_IsWind;
}

// =======================================================================================================================
// =======================================================================================================================
void GAO_ModifierWind_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	GAO_tdst_ModifierWind * p_Wind;

	p_Wind = (GAO_tdst_ModifierWind *) _pst_Mod->p_Data;

    p_Wind->po_Source->Update();
}

// =======================================================================================================================
// =======================================================================================================================
void GAO_ModifierWind_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

// =======================================================================================================================
// =======================================================================================================================
void GAO_ModifierWind_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierWind * p_Wind;

	p_Wind = (GAO_tdst_ModifierWind *) _pst_Mod->p_Data;

    p_Wind->po_Source->Init(_pst_Mod->pst_GO);

    CWindManager::GetInstance()->AddWindSource(p_Wind->po_Source);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GAO_ModifierWind_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	GAO_tdst_ModifierWind * p_Wind;
	char *                  pc_BufferSave;
	ULONG					ulVersion;

	pc_BufferSave = _pc_Buffer;
	p_Wind = (GAO_tdst_ModifierWind *) _pst_Mod->p_Data;

	// version
	ulVersion = LOA_ReadULong(&_pc_Buffer);

    if (ulVersion == Wind_C_VersionNumber)
    {
        _pc_Buffer += p_Wind->po_Source->Load(_pc_Buffer);
    }

   	//_pst_Mod->pst_GO->us_Runtime_IdentityFlag |= OBJ_C_RTIdentityFlag_IsWind;

	return (_pc_Buffer - pc_BufferSave);
}

#ifdef ACTIVE_EDITORS

// =======================================================================================================================
// =======================================================================================================================
void GAO_ModifierWind_Save(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierWind	*p_Wind;
    ULONG                   ulVersion;

	p_Wind = (GAO_tdst_ModifierWind *) _pst_Mod->p_Data;

	// Save version 
	ulVersion = Wind_C_VersionNumber;
	SAV_Buffer(&ulVersion, sizeof(ULONG));

    p_Wind->po_Source->Save();
}

void GAO_ModifierWind_Refresh(MDF_tdst_Modifier *_pst_Mod)
{
    GAO_tdst_ModifierWind	*p_Wind;

    p_Wind = (GAO_tdst_ModifierWind *) _pst_Mod->p_Data;

    p_Wind->po_Source->UpdateErrorDirection();
	p_Wind->po_Source->UpdateDynamic();
}

void GAO_ModifierWind_Helper_ComputeMatrix(MDF_tdst_Modifier *_pst_Mod, MATH_tdst_Matrix * _pst_Matrix)
{
    MATH_CopyMatrix(_pst_Matrix, OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO));
}

void GAO_ModifierWind_Helper_Move(MDF_tdst_Modifier *_pst_Mod, MATH_tdst_Vector * _pst_Move)
{
    MATH_AddEqualVector(OBJ_pst_GetAbsolutePosition(_pst_Mod->pst_GO), _pst_Move);
}

void GAO_ModifierWind_Helper_Scale(MDF_tdst_Modifier *_pst_Mod, MATH_tdst_Vector * _pst_Scale)
{
    GAO_tdst_ModifierWind * p_Wind;

    p_Wind = (GAO_tdst_ModifierWind *) _pst_Mod->p_Data;

    p_Wind->po_Source->ScalePlane(_pst_Scale->x, _pst_Scale->z);
}

void GAO_ModifierWind_Helper_Rotate(MDF_tdst_Modifier *_pst_Mod, MATH_tdst_Vector * _pst_Axis, FLOAT _f_Angle)
{
    MATH_tdst_Matrix st_TempMatrix;
    MATH_tdst_Matrix st_Rotation;

    MATH_MakeRotationMatrix_PointAxisAngle(&st_Rotation, OBJ_pst_GetAbsolutePosition(_pst_Mod->pst_GO), _pst_Axis, _f_Angle, NULL, 1);

    MATH_MulMatrixMatrix(&st_TempMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO), &st_Rotation);
    MATH_CopyMatrix(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO), &st_TempMatrix);
    MATH_Orthonormalize(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO));
}


#endif // #ifdef ACTIVE_EDITORS


