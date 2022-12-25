#include "Precomp.h"

#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/ENGInit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJgizmo.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "MoDiFier/MDFstruct.h"
#include "MDFmodifier_Sfx.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "GRObject/GROrender.h"
#include "TIMer/TIMdefs.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/OBJects/OBJ.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GEOmetric/GeoObjectAccess.h"
#include "TEXture/TEXfile.h"
#include "BASe/BENch/BENch.h"

#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

// Current version number for this modifier
#define MDF_SFX_CASSERT(exp) 		ERR_X_Assert(exp)
#define MDF_SFX_VERSION 2

BOOL    IsRimLightHeightAttenuationEnabled      = FALSE;
float	RimLightHeightWorldMin                  = 0.0f;
float	RimLightHeightWorldMax                  = 0.0f;
float	RimLightHeightAttenuationMin            = 0.0f;
float	RimLightHeightAttenuationMax            = 1.0f;


//--------------------------------------------------------
// GetHotAirModifier
//--------------------------------------------------------
MDF_tdst_HotAir *
MDF_SfxGetHotAirModifier( OBJ_tdst_GameObject * _pst_GO )
{
    ERR_X_Assert( _pst_GO );
   
    if( OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ExtendedObject ) )
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
                    return &pMdfSfx->hotAirParams;
                }
            }

            pst_Modifier = pst_Modifier->pst_Next;
        }
    }

    return NULL;
}

//--------------------------------------------------------
// GAO_ModifierSfx_Create
//--------------------------------------------------------
void    GAO_ModifierSfx_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	GAO_tdst_ModifierSfx *p_Sfx;

	// allocate space for the struct 
	_pst_Mod->p_Data = MEM_p_Alloc( sizeof(GAO_tdst_ModifierSfx) );
	p_Sfx = (GAO_tdst_ModifierSfx *)_pst_Mod->p_Data;

	if (p_Data == NULL)
	{	
		// First init
		L_memset(p_Sfx, 0 , sizeof(GAO_tdst_ModifierSfx));

#ifdef ACTIVE_EDITORS
        p_Sfx->type = MDF_SFX_HotAir;
#endif
	} 
	else
	{ 
		// Duplication				
		L_memcpy( p_Sfx, p_Data, sizeof (GAO_tdst_ModifierSfx) ); 

		p_Sfx->flags |= MDF_SFX_Flags_WasDuplicated;
	}

    _pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;
}

//--------------------------------------------------------
// GAO_ModifierSfx_Destroy
//--------------------------------------------------------
void GAO_ModifierSfx_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierSfx *p_Sfx;
	p_Sfx = (GAO_tdst_ModifierSfx *)_pst_Mod->p_Data;

	if (_pst_Mod->p_Data)
	{
		MEM_Free(_pst_Mod->p_Data);		
	}
}

//--------------------------------------------------------
// GAO_ModifierSfx_UnApplyHotAir
//--------------------------------------------------------
void GAO_ModifierSfx_UnApplyHotAir(MDF_tdst_Modifier *_pst_Mod)
{
#if !defined(XML_CONV_TOOL)
	GAO_tdst_ModifierSfx* pSFX = (GAO_tdst_ModifierSfx*)_pst_Mod->p_Data;
	OBJ_tdst_GameObject* pGAO = _pst_Mod->pst_GO;

    // HotAir plane is local to its GAO, so add GAO position to plane's offset
#if defined(_XENON_RENDER)
    MATH_AddVector( &pSFX->hotAirParams.stHotAirObject.Position , &pGAO->pst_GlobalMatrix->T, &pSFX->hotAirParams.stOffset);

#ifdef ACTIVE_EDITORS
    // display the size square
    if (pGAO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
    {
        GDI_tdst_Request_DrawLineEx line;

        line.f_Width = 2.0f;
        line.ul_Color = 0xFF00FF00;   
        
        MATH_tdst_Vector corners[4];
        MATH_tdst_Vector halfI, halfJ;

        MATH_tdst_Vector oCenter;

        // Transform hotAirPos in world space to camera space
        MATH_TransformVertex( &oCenter, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix, &pSFX->hotAirParams.stHotAirObject.Position );

        // Build corners in camera space
        for( int i=0; i<4; ++i )
        {
            corners[i] = oCenter;
        }

        halfI.x = pSFX->hotAirParams.stHotAirObject.Width/2.0f;
        halfI.y = 0.0f;
        halfI.z = 0.0f;

        halfJ.x = 0.0f;
        halfJ.y = pSFX->hotAirParams.stHotAirObject.Height/2.0f;
        halfJ.z = 0.0f;

        MATH_SubEqualVector( &corners[0], &halfI );
        MATH_SubEqualVector( &corners[0], &halfJ );

        MATH_SubEqualVector( &corners[1], &halfI );
        MATH_AddEqualVector( &corners[1], &halfJ );

        MATH_AddEqualVector( &corners[2], &halfI );
        MATH_AddEqualVector( &corners[2], &halfJ );

        MATH_AddEqualVector( &corners[3], &halfI );
        MATH_SubEqualVector( &corners[3], &halfJ );

        MATH_tdst_Matrix worldToObject;
        MATH_InvertMatrix( &worldToObject, pGAO->pst_GlobalMatrix );

        MATH_tdst_Matrix cameraToObject;
        MATH_MulMatrixMatrix( &cameraToObject, &GDI_gpst_CurDD->st_Camera.st_Matrix, &worldToObject );

        for( int i = 0; i < 4; i++ )
        {
            // Back to object space
            MATH_TransformVertex( &corners[i], &cameraToObject, &corners[i] );
        }

        // Draw the 4 lines that make up the plane
        for (int i = 0; i < 4; i++)
        {
            line.A = corners + i;
            line.B = corners + ((i + 1) % 4);

            GDI_gpst_CurDD->st_GDI.pfnl_Request( GDI_Cul_Request_DrawLineEx, (ULONG) &line );
        }
    }
#endif

    if (GDI_b_IsXenonGraphics())
    {
        pSFX->hotAirParams.stHotAirObject.Intensity = 1.0f;
        pSFX->hotAirParams.stHotAirObject.BottomScale = 1.0f;
        g_oHeatManager.AddHotAirPoint( &pSFX->hotAirParams.stHotAirObject );
    }
#endif
#endif // XML_CONV_TOOL
}

#if !defined(XML_CONV_TOOL)
//--------------------------------------------------------
// GAO_ModifierSfx_RimLightGetMaxHeight
//--------------------------------------------------------
float GAO_ModifierSfx_RimLightGetMaxBoneHeight( OBJ_tdst_GameObject * _pst_GO )
{
    MATH_tdst_Matrix *p_UsedMatrix(NULL), st_Matrix2;
    MATH_tdst_Matrix st_Matrix3;
  
    ERR_X_Assert( _pst_GO != NULL );

    float fMaxBoneHeight = 100.0f;

    p_UsedMatrix = NULL;
    if( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix && _pst_GO->pst_Base->pst_AddMatrix )
    {
        for( INT i=0; i<_pst_GO->pst_Base->pst_AddMatrix->l_Number; ++i )
        {
            if (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
            {
                p_UsedMatrix = _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_Matrix;
                if( (_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].l_MatrixId == (LONG)0xffffffff) || 
                    (_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO && 
                        (_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)))
                {
                    if (_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO )
                    {
                        p_UsedMatrix = _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO->pst_GlobalMatrix;
                        MATH_InvertMatrix(&st_Matrix3 , _pst_GO->pst_GlobalMatrix);
                        MATH_MulMatrixMatrix(&st_Matrix2 , p_UsedMatrix, &st_Matrix3 );
                    }
                    else
                    {
                        MATH_SetIdentityMatrix(&st_Matrix2);
                    }
                    p_UsedMatrix = &st_Matrix2;
                }
            }
            else
            {
                p_UsedMatrix = &_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[i].st_Matrix;
            }

            if (!p_UsedMatrix) p_UsedMatrix = &MATH_gst_IdentityMatrix;

            MATH_tdst_Vector *p_stBonePos = &p_UsedMatrix->T;

            MATH_TransformVertex( p_stBonePos, _pst_GO->pst_GlobalMatrix, p_stBonePos );

            if( i == 0 )
            {
                fMaxBoneHeight = p_stBonePos->z;
            }
            else if( p_stBonePos->z > fMaxBoneHeight )
            {
                fMaxBoneHeight = p_stBonePos->z;
            }
        } 
    }
   
    return fMaxBoneHeight;
}
#endif

//--------------------------------------------------------
// GAO_ModifierSfx_ApplyRimLight
//--------------------------------------------------------
void GAO_ModifierSfx_ApplyRimLight(GAO_tdst_ModifierSfx * p_Sfx, GEO_tdst_Object *_pst_Obj, MDF_tdst_Modifier * _pst_Mod )
{
#if !defined(XML_CONV_TOOL)
    ERR_X_Assert( p_Sfx != NULL && _pst_Mod != NULL );

    // Set global rim light values
    IsRimLightHeightAttenuationEnabled      = TRUE;
    RimLightHeightAttenuationMin            = p_Sfx->rimLightParams.fHeightAttenuationMin;
    RimLightHeightAttenuationMax            = p_Sfx->rimLightParams.fHeightAttenuationMax;

    // Sanity check for values
#define M_Clamp( V, Min, Max ) { V = min(V, (Max) ); V = max(V, (Min) ); }
    M_Clamp( RimLightHeightAttenuationMin, 0.0f, 1.0f );
    M_Clamp( RimLightHeightAttenuationMax, 0.0f, 1.0f );
  
    if( RimLightHeightAttenuationMin >= RimLightHeightAttenuationMax )
    {
         IsRimLightHeightAttenuationEnabled = FALSE;
         return;
    }

    // Compute the height min and max for height attenuation
    RimLightHeightWorldMin = _pst_Mod->pst_GO->pst_GlobalMatrix->T.z;
    RimLightHeightWorldMax = RimLightHeightWorldMin;

    // If the gao is an animation, find the highest bone
    if( _pst_Mod->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims )
    {
        RimLightHeightWorldMax = GAO_ModifierSfx_RimLightGetMaxBoneHeight( _pst_Mod->pst_GO );
    }
    else
    {
        // Find the coordinates of bounding box points in world space
        void * pst_BV = NULL;
        if( _pst_Mod->pst_GO != NULL && _pst_Mod->pst_GO->pst_BV != NULL && OBJ_BV_IsAABBox(_pst_Mod->pst_GO->pst_BV ) )
        {
            pst_BV = _pst_Mod->pst_GO->pst_BV;
        }

        if( pst_BV != NULL )
        {
            MATH_tdst_Vector & vMin = *OBJ_pst_BV_GetGMin( pst_BV );
            MATH_tdst_Vector & vMax = *OBJ_pst_BV_GetGMax( pst_BV );
      
            RimLightHeightWorldMin = vMin.z + _pst_Mod->pst_GO->pst_GlobalMatrix->T.z;
            RimLightHeightWorldMax = vMax.z + _pst_Mod->pst_GO->pst_GlobalMatrix->T.z;
        }
        else
        {
            IsRimLightHeightAttenuationEnabled = FALSE;
        }
    }
#endif
}

//--------------------------------------------------------
// GAO_ModifierSfx_Apply
//--------------------------------------------------------
void GAO_ModifierSfx_Apply( MDF_tdst_Modifier * _pst_Mod, GEO_tdst_Object * _pst_Obj )
{
    GAO_tdst_ModifierSfx *p_Sfx;
    p_Sfx = (GAO_tdst_ModifierSfx *)_pst_Mod->p_Data;

    switch (p_Sfx->type)
    {
    case MDF_SFX_HotAir:
        break;

    case MDF_SFX_RimLight:
        GAO_ModifierSfx_ApplyRimLight( p_Sfx, _pst_Obj, _pst_Mod );
        break;
    }
}

//--------------------------------------------------------
// GAO_ModifierSfx_Unapply
//--------------------------------------------------------
void GAO_ModifierSfx_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	GAO_tdst_ModifierSfx *p_Sfx;
	p_Sfx = (GAO_tdst_ModifierSfx *)_pst_Mod->p_Data;

	switch (p_Sfx->type)
	{
		case MDF_SFX_HotAir:
			GAO_ModifierSfx_UnApplyHotAir(_pst_Mod);
			break;
        case MDF_SFX_RimLight:
            IsRimLightHeightAttenuationEnabled = FALSE;
            break;
	}
}

//--------------------------------------------------------
// GAO_ModifierSfx_Reinit
//--------------------------------------------------------
void GAO_ModifierSfx_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierSfx *p_Sfx;
	p_Sfx = (GAO_tdst_ModifierSfx *)_pst_Mod->p_Data;

	switch (p_Sfx->type)
	{
    case MDF_SFX_HotAir:
        break;
    case MDF_SFX_RimLight:
        break;
	}
}

//--------------------------------------------------------
// GAO_ModifierSfx_Load
//--------------------------------------------------------
ULONG GAO_ModifierSfx_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	GAO_tdst_ModifierSfx* p_Sfx;
	ULONG	ulVersion;
	char *_pc_BufferSave;
	_pc_BufferSave = _pc_Buffer;

	p_Sfx = (GAO_tdst_ModifierSfx*)_pst_Mod->p_Data;

	// load version
	ulVersion = LOA_ReadULong(&_pc_Buffer);

	// load id
	p_Sfx->ulUserID = LOA_ReadULong(&_pc_Buffer);

	// load type
	p_Sfx->type = (MDF_SFX_Type) LOA_ReadULong(&_pc_Buffer);

	// switch on type
	switch(p_Sfx->type)
	{
		// Hot Air
		case MDF_SFX_HotAir:
		{
			LOA_ReadVector(&_pc_Buffer, &p_Sfx->hotAirParams.stOffset);
#if defined(_XENON_RENDER)
			p_Sfx->hotAirParams.stHotAirObject.Width = LOA_ReadFloat(&_pc_Buffer);
            p_Sfx->hotAirParams.stHotAirObject.Height = LOA_ReadFloat(&_pc_Buffer);
            p_Sfx->hotAirParams.stHotAirObject.Range = LOA_ReadFloat(&_pc_Buffer);
            p_Sfx->hotAirParams.stHotAirObject.ScrollSpeed = LOA_ReadFloat(&_pc_Buffer);
            p_Sfx->hotAirParams.stHotAirObject.NoisePixelSize = LOA_ReadFloat(&_pc_Buffer);
            p_Sfx->hotAirParams.stHotAirObject.Scroll = 0.0f;
#if !defined(XML_CONV_TOOL)
            _pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;
#endif
#endif
		}
		break;

        case MDF_SFX_RimLight:
        {
            if( ulVersion < 2 )
            {
                LOA_ReadFloat(&_pc_Buffer); //p_Sfx->rimLightParams.fWidthMin  
                LOA_ReadFloat(&_pc_Buffer); //p_Sfx->rimLightParams.fWidthMax  
                LOA_ReadULong(&_pc_Buffer); //p_Sfx->rimLightParams.ulColor    
                LOA_ReadFloat(&_pc_Buffer); //p_Sfx->rimLightParams.fIntensity 
            }
            p_Sfx->rimLightParams.fHeightAttenuationMin     = LOA_ReadFloat(&_pc_Buffer);
            p_Sfx->rimLightParams.fHeightAttenuationMax     = LOA_ReadFloat(&_pc_Buffer);

            if( ulVersion < 2 )
            {
                LOA_ReadULong(&_pc_Buffer); //p_Sfx->rimLightParams.bSpecularMapAttenuation
            }
#if !defined(XML_CONV_TOOL)
            _pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;
#endif
        }
        break;
	}

	return _pc_Buffer - _pc_BufferSave ;
}

#ifdef ACTIVE_EDITORS
void GAO_ModifierSfx_Save(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierSfx* p_Sfx;
	ULONG	ulData;
	
	p_Sfx = (GAO_tdst_ModifierSfx*)_pst_Mod->p_Data;

    // Reminder not to forget to update load/save when the version number changes
    MDF_SFX_CASSERT( MDF_SFX_VERSION == 2 );

    ulData = MDF_SFX_VERSION; // version
	SAV_Buffer(&ulData, 4);  
	
	//save user id
	SAV_Buffer(&p_Sfx->ulUserID, 4);

	// save the type
	SAV_Buffer(&p_Sfx->type, 4);

	// save custom settings
	switch(p_Sfx->type)
	{
		case MDF_SFX_HotAir:
		{
			SAV_Buffer(&p_Sfx->hotAirParams.stOffset, sizeof(MATH_tdst_Vector));
#if defined(_XENON_RENDER)
            SAV_Buffer(&p_Sfx->hotAirParams.stHotAirObject.Width,           sizeof(float));	
            SAV_Buffer(&p_Sfx->hotAirParams.stHotAirObject.Height,          sizeof(float));
            SAV_Buffer(&p_Sfx->hotAirParams.stHotAirObject.Range,           sizeof(float));
            SAV_Buffer(&p_Sfx->hotAirParams.stHotAirObject.ScrollSpeed,     sizeof(float));
            SAV_Buffer(&p_Sfx->hotAirParams.stHotAirObject.NoisePixelSize,  sizeof(float));
#endif
		}		
		break;

        case MDF_SFX_RimLight:
        {
            SAV_Buffer(&p_Sfx->rimLightParams.fHeightAttenuationMin,    sizeof(float));
            SAV_Buffer(&p_Sfx->rimLightParams.fHeightAttenuationMax,    sizeof(float));
        }
        break;
	}
}
#endif
