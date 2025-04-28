/*$T GEOstaticLOD.c GC!1.40 09/08/99 17:52:36 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"

#include "ENGine/Sources/WORld/WORstruct.h"

#include "GDInterface/GDInterface.h"
#include "SELection/SELection.h"
#include "GRObject/GROstruct.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOobjectaccess.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTlinear.h"

#ifdef PSX2_TARGET
#include "PSX2debug.h"
#endif

#ifdef ACTIVE_EDITORS
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#include "MAD_mem/Sources/MAD_mem.h"
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

#if defined(XML_CONV_TOOL)
UCHAR ucGeoStaticLODDummy;
#endif // defined(XML_CONV_TOOL)
/*
 ===================================================================================================
 ===================================================================================================
 */
LONG GEO_l_StaticLOD_GetIndex(GEO_tdst_StaticLOD *_pst_LOD, UCHAR _uc_Distance)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    i = 0;
    while(i < _pst_LOD->uc_NbLOD - 1)
    {
        if(_pst_LOD->auc_EndDistance[i] < _uc_Distance) break;
        i++;
    }

    return i;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
GRO_tdst_Struct *GEO_pst_StaticLOD_GetLOD(GEO_tdst_StaticLOD *_pst_LOD, UCHAR _uc_Distance)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef _XENON
    // minimum last level of LOD that assurely has the right mesh
    // this fixes spg2 spawning on non visible mesh
    if(_uc_Distance<5)
        _uc_Distance = 5;
#endif

    i = 0;
    while(i < _pst_LOD->uc_NbLOD - 1)
    {
        if (_pst_LOD->auc_EndDistance[i] < _uc_Distance) break;
        i++;
    }

    return _pst_LOD->dpst_Id[i];
}

/*
 ===================================================================================================
    Create an object
 ===================================================================================================
 */
GEO_tdst_StaticLOD *GEO_pst_StaticLOD_Create(UCHAR _uc_NbLOD)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_StaticLOD  *pst_LOD;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_LOD = (GEO_tdst_StaticLOD *) MEM_p_Alloc(sizeof(GEO_tdst_StaticLOD));
    L_memset(pst_LOD, 0, sizeof(GEO_tdst_StaticLOD));

    GRO_Struct_Init(&pst_LOD->st_Id, GRO_GeoStaticLOD);

    pst_LOD->uc_NbLOD = _uc_NbLOD;
    if (pst_LOD->uc_NbLOD > 6)
        pst_LOD->uc_NbLOD = 6;

    return pst_LOD;
}

/*
 ===================================================================================================
    Free content of an object
 ===================================================================================================
 */
void GEO_StaticLOD_FreeContent(GEO_tdst_StaticLOD *_pst_LOD)
{
    int i;
    GRO_tdst_Struct *pst_Gro;

    if(_pst_LOD == NULL) return;

    if(_pst_LOD->uc_NbLOD)
    {
        for (i = 0; i < _pst_LOD->uc_NbLOD; i++)
        {
            pst_Gro = _pst_LOD->dpst_Id[i];
            pst_Gro->i->pfn_AddRef(pst_Gro, -1);
            pst_Gro->i->pfn_Destroy(pst_Gro);
        }
    }

    GRO_Struct_Free(&_pst_LOD->st_Id);
}

/*
 ===================================================================================================
    Add a LOD
 ===================================================================================================
 */
void GEO_StaticLOD_Add(GEO_tdst_StaticLOD *_pst_LOD, GRO_tdst_Struct *_pst_Gro, UCHAR _uc_Distance )
{
    /*~~~~~~~~~~~~~~~*/
    LONG    l_Index;
    int     i;
    /*~~~~~~~~~~~~~~~*/

    if(_pst_LOD == NULL) return;
    if(_pst_Gro == NULL) return;
    if(_pst_Gro->i->ul_Type != GRO_Geometric) return;

    if (_pst_LOD->uc_NbLOD >= 6) return;

    l_Index = GEO_l_StaticLOD_GetIndex( _pst_LOD, _uc_Distance ) + 1;
    for (i = _pst_LOD->uc_NbLOD; i > l_Index; i--)
    {
        _pst_LOD->auc_EndDistance[ i + 1 ] = _pst_LOD->auc_EndDistance[ i ];
        _pst_LOD->dpst_Id[ i + 1 ] = _pst_LOD->dpst_Id[ i ];
    }

    _pst_LOD->dpst_Id[ l_Index ] = _pst_Gro;
    _pst_LOD->auc_EndDistance[ l_Index ] = _pst_LOD->auc_EndDistance[ l_Index - 1 ];
    _pst_LOD->auc_EndDistance[ l_Index - 1 ] = _uc_Distance;

    _pst_LOD->uc_NbLOD++;
    _pst_Gro->i->pfn_AddRef( _pst_Gro, 1);
}


/*
 ===================================================================================================
    replace a LOD
 ===================================================================================================
 */
void GEO_StaticLOD_Replace(GEO_tdst_StaticLOD *_pst_LOD, GRO_tdst_Struct *_pst_Gro, LONG _l_Index )
{
    if(_pst_LOD == NULL) return;
    if(_pst_Gro == NULL) return;
    if(_pst_Gro->i->ul_Type != GRO_Geometric) return;
    
    _l_Index %= _pst_LOD->uc_NbLOD;
    _pst_LOD->dpst_Id[ _l_Index ]->i->pfn_AddRef( _pst_LOD->dpst_Id[ _l_Index ], -1);
    _pst_LOD->dpst_Id[ _l_Index ] = _pst_Gro;
    _pst_Gro->i->pfn_AddRef( _pst_Gro, 1);
}

/*
 ===================================================================================================
    delete a LOD
 ===================================================================================================
 */
void GEO_StaticLOD_Delete(GEO_tdst_StaticLOD *_pst_LOD, LONG _l_Index )
{
    ULONG ul_Size;

    if(_pst_LOD == NULL) return;
    if (_pst_LOD->uc_NbLOD <= 1) return;
        
    _l_Index %= _pst_LOD->uc_NbLOD;

    if (_pst_LOD->dpst_Id[ _l_Index ])
        _pst_LOD->dpst_Id[ _l_Index ]->i->pfn_AddRef(_pst_LOD->dpst_Id[ _l_Index ], -1);

    if (_l_Index < _pst_LOD->uc_NbLOD - 1)
    {
        ul_Size = (_pst_LOD->uc_NbLOD - 1 - _l_Index);
        L_memmove( _pst_LOD->auc_EndDistance + _l_Index, _pst_LOD->auc_EndDistance + _l_Index + 1, ul_Size );
        L_memmove( _pst_LOD->dpst_Id + _l_Index, _pst_LOD->dpst_Id + _l_Index + 1, ul_Size * 4);
    }
    _pst_LOD->uc_NbLOD--;
}


/*
 ===================================================================================================
    Free an object
 ===================================================================================================
 */
void GEO_StaticLOD_Free(GEO_tdst_StaticLOD *_pst_LOD)
{
    if(_pst_LOD == NULL) return;
    if(_pst_LOD->st_Id.i->ul_Type != GRO_GeoStaticLOD) return;
	if(_pst_LOD->st_Id.l_Ref > 0) return;

    GEO_StaticLOD_FreeContent(_pst_LOD);

    /* Remove address from loading tables */
    LOA_DeleteAddress(_pst_LOD);

    MEM_Free(_pst_LOD);
}

/*
 ===================================================================================================
    Aim:    Load object from .gob file
 ===================================================================================================
 */
void *GEO_p_StaticLOD_CreateFromBuffer
(
    GRO_tdst_Struct *_pst_Id,
    char            **ppc_Buffer,
    WOR_tdst_World  *_pst_World
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_StaticLOD  *pst_LOD;
    LONG                i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Create object structure and init structure Id */
    pst_LOD = GEO_pst_StaticLOD_Create(0);

    /* Get number of load */
    pst_LOD->uc_NbLOD = LOA_ReadUChar(ppc_Buffer);
    if (pst_LOD->uc_NbLOD > 6 )
        pst_LOD->uc_NbLOD = 6;

    /* dummy */
#if !defined(XML_CONV_TOOL)
	LOA_ReadUChar_Ed(ppc_Buffer,NULL);
#else
	LOA_ReadUChar_Ed(ppc_Buffer, &ucGeoStaticLODDummy);
#endif // !defined(XML_CONV_TOOL)

    /* Get end of distance values */
    for (i = 0; i < 6; i++)
        pst_LOD->auc_EndDistance[ i ] = LOA_ReadUChar(ppc_Buffer);

    /* Get LOD object key */
    for(i = 0; i < pst_LOD->uc_NbLOD; i++)
    {
        LOA_MakeFileRef
        (
            LOA_ReadLong(ppc_Buffer),
            (ULONG *) &pst_LOD->dpst_Id[i],
            GEO_ul_Load_ObjectCallback,
            LOA_C_MustExists
        );
    }
    return pst_LOD;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
LONG GEO_l_StaticLOD_HasSomethingToRender
(
    GRO_tdst_Visu           *_pst_Visu,
    GEO_tdst_Object         **ppst_PickableObject
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_StaticLOD  *pst_LOD;
    LONG                l_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GDI_gpst_CurDD->ul_CurrentDrawMask = GDI_gpst_CurDD->ul_DrawMask & _pst_Visu->ul_DrawMask;
    pst_LOD = (GEO_tdst_StaticLOD *) _pst_Visu->pst_Object;

    pst_LOD->uc_Distance = ((OBJ_tdst_GameObject*) GDI_gpst_CurDD->pst_CurrentGameObject)->uc_LOD_Vis;

    l_Index = GEO_l_StaticLOD_GetIndex( pst_LOD, pst_LOD->uc_Distance );
    *ppst_PickableObject = (GEO_tdst_Object *) pst_LOD->dpst_Id[ l_Index ];

    if (!(*ppst_PickableObject))
        return FALSE;
    
    return(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Draw);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void GEO_StaticLOD_Render(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GRO_tdst_Visu       *pst_Visu;
    GEO_tdst_Object     *pst_Obj;
    GEO_tdst_StaticLOD  *pst_LOD;
#ifdef ACTIVE_EDITORS
    ULONG ul_Index;
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Visu = _pst_GO->pst_Base->pst_Visu;
    pst_LOD = (GEO_tdst_StaticLOD *) pst_Visu->pst_Object;
    

#ifdef ACTIVE_EDITORS
    if (_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ForceLOD)
        ul_Index = _pst_GO->ul_ForceLODIndex % pst_LOD->uc_NbLOD;
    else
        ul_Index = _pst_GO->ul_ForceLODIndex = GEO_l_StaticLOD_GetIndex(pst_LOD, pst_LOD->uc_Distance);
    pst_Obj = (GEO_tdst_Object *) pst_LOD->dpst_Id[ ul_Index ];
#else
    pst_Obj = (GEO_tdst_Object *) GEO_pst_StaticLOD_GetLOD(pst_LOD, pst_LOD->uc_Distance);
#endif

#ifndef NO_GC_FUR
#if (defined _GAMECUBE) // && (defined _GAMECUBE_DSPLS)
	// Render all LODs the first time to call GEO_Optimize_GAMECUBE 
	if (pst_Obj->dst_Element && pst_Obj->dst_Element->dl_size == 0xFFFFFFFF)
	{
		int ul_Index;
		for (ul_Index = 0; ul_Index<pst_LOD->uc_NbLOD; ul_Index++)
		{
			GEO_tdst_Object     *pst_CurObj = (GEO_tdst_Object *) pst_LOD->dpst_Id[ ul_Index ];
		    pst_Visu->pst_Object = (GRO_tdst_Struct *) pst_CurObj;
    		pst_CurObj->st_Id.i->pfn_Render(_pst_GO);		
		}
	}
#endif
#endif
#if defined(_XENON_RENDER)
    // Use most detailed LOD the first time an object is rendered, this will ensure that the color
    // vertex buffer is large enough for all other levels
// SC: HACK: Force the LOD level to 0 at all times to prevent a queued mesh from being deleted when
//           multiple levels are used in the same frame because of the validation object...
//    if (pst_Visu->pElementsMeshes == NULL)
    {
        pst_Obj = (GEO_tdst_Object*)pst_LOD->dpst_Id[0];
   }
#endif

    pst_Visu->pst_Object = (GRO_tdst_Struct *) pst_Obj;
    pst_Obj->st_Id.i->pfn_Render(_pst_GO);
    pst_Visu->pst_Object = (GRO_tdst_Struct *) pst_LOD;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
LONG GEO_l_StaticLOD_SaveInBuffer(GEO_tdst_StaticLOD *_pst_LOD, void *p_Unused)
{
#ifdef ACTIVE_EDITORS

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul_Value;
    LONG    i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GRO_Struct_Save(&_pst_LOD->st_Id);

    /* nombre de niveau de détail*/
    SAV_Buffer((char *) &_pst_LOD->uc_NbLOD, 1);

    /* dummy */
    i = 0;
    SAV_Buffer((char *) &i, 1);
    
    /* distances */
    SAV_Buffer(_pst_LOD->auc_EndDistance, 6 );

    /* objets */
    for(i = 0; i < _pst_LOD->uc_NbLOD; i++)
    {
        ul_Value = LOA_ul_SearchKeyWithAddress((ULONG) _pst_LOD->dpst_Id[i]);
        SAV_Buffer(&ul_Value, 4);
    }

#endif
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
MAD_GeometricObject *GEO_p_StaticLOD_ToMad(GEO_tdst_StaticLOD *_pst_LOD, struct WOR_tdst_World_ *_pst_World)
{
    GEO_tdst_Object *pst_Obj;

    pst_Obj = (GEO_tdst_Object *) _pst_LOD->dpst_Id[0];
    if (!pst_Obj) return NULL;
    if (pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return NULL;
    return (MAD_GeometricObject*)pst_Obj->st_Id.i->pfnp_ToMad( pst_Obj, _pst_World );
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
BOOL GEO_StaticLOD_HasRLI(GEO_tdst_StaticLOD *_pst_LOD) {
	int i;
	for (i=0; i<_pst_LOD->uc_NbLOD; i++) {
		if (_pst_LOD->dpst_Id[i]->i->ul_Type == GRO_Geometric &&
			((GEO_tdst_Object *)_pst_LOD->dpst_Id[i])->dul_PointColors)
			return TRUE;
	}
	return FALSE;
}
#endif

/*
 ===================================================================================================
 ===================================================================================================
 */
void GEO_StaticLOD_Init(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GRO_tdst_Interface  *i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    i = &GRO_gast_Interface[GRO_GeoStaticLOD];
    i->pfnp_CreateFromBuffer = (void *(__cdecl *)(GRO_tdst_Struct *,char ** ,void *))GEO_p_StaticLOD_CreateFromBuffer;
    i->pfn_Destroy = (void (__cdecl *)(void *))GEO_StaticLOD_Free;
    i->pfnl_HasSomethingToRender = (LONG (__cdecl *)(void *,void *))GEO_l_StaticLOD_HasSomethingToRender;
    i->pfn_Render = (void (__cdecl *)(void *))GEO_StaticLOD_Render;

#ifdef ACTIVE_EDITORS

	i->pfnl_SaveInBuffer = (LONG (__cdecl *)(void *,void *))GEO_l_StaticLOD_SaveInBuffer;
    i->pfnp_ToMad = (void* (__cdecl *)(void *,void *))GEO_p_StaticLOD_ToMad;

#endif
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
