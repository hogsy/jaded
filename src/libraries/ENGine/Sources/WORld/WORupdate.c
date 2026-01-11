/*$T WORupdate.c GC! 1.081 05/23/01 18:51:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Main functions of the world module */
#include "Precomp.h"
#include "ENGine/Sources/WORld/WORupdate.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "MATerial/MATstruct.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "SOFT/SOFTcolor.h"
#include "GEOmetric/GEOobjectcomputing.h"
#ifdef JADEFUSION
#include "GEOmetric/GEOstaticLOD.h"
#endif
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_GEO.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_World_Update_GroPointerChange(WOR_tdst_World *_pst_World, void *_p_New, void *_p_Old)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				l_Index;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	GRO_tdst_Struct		*pst_Gro, *pst_NewGro;
	MAT_tdst_Multi		**ppst_MultiMat, **ppst_LastMultiMat;
	GRO_tdst_Struct		**ppst_Mat, **ppst_LastMat;
    ULONG               *pul_OldRLI, *pul_NewRLI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_NewGro = (GRO_tdst_Struct *) _p_New;
	l_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&_pst_World->st_GraphicObjectsTable, _p_Old);
	if(l_Index != (LONG) TAB_Cul_BadIndex)
	{
		pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
		pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
		while(pst_PFElem <= pst_PFLastElem)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
			pst_PFElem++;
			if(TAB_b_IsAHole(pst_GO)) continue;

			if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
			{
				pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
#ifdef JADEFUSION
				if (!pst_Gro) continue;

				// SC: We are looking for a geometric object, not a LOD.. duh
				if (pst_Gro->i->ul_Type == GRO_GeoStaticLOD)
				{
					GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Gro;

					if ((pst_LOD->uc_NbLOD > 0) && (pst_LOD->dpst_Id[0] != NULL))
					{
						pst_Gro = pst_LOD->dpst_Id[0];
					}
				}
#endif

				if((void *) pst_Gro == _p_Old)
				{
                    /* try to keep RLI good */
                    pul_OldRLI = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
                    if ( ( pul_OldRLI ) && ( pst_NewGro->i->ul_Type == GRO_Geometric) && (pst_Gro->i->ul_Type == GRO_Geometric) )
                    {
                        pul_NewRLI = NULL;
                        GEO_PickRLI( (GEO_tdst_Object *) pst_NewGro, &pul_NewRLI, (GEO_tdst_Object *) pst_Gro, pul_OldRLI, 0 );
                        OBJ_VertexColor_ChangePointer( pst_GO, pul_NewRLI, 1 );
                    }
                    
					pst_Gro->i->pfn_AddRef(pst_Gro, -1);
					pst_Gro->i->pfn_Destroy(pst_Gro);
					pst_NewGro->i->pfn_AddRef(pst_NewGro, 1);
					pst_GO->pst_Base->pst_Visu->pst_Object = pst_NewGro;
				}
			}

			if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Lights))
			{
				pst_Gro = (GRO_tdst_Struct *) pst_GO->pst_Extended->pst_Light;
				if((void *) pst_Gro == _p_Old)
				{
					pst_Gro->i->pfn_AddRef(pst_Gro, -1);
					pst_Gro->i->pfn_Destroy(pst_Gro);
					pst_NewGro->i->pfn_AddRef(pst_NewGro, 1);
					pst_GO->pst_Extended->pst_Light = pst_NewGro;
				}
			}
		}
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~*/
			TAB_tdst_Ptable *pst_Table;
			/*~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Table = &_pst_World->st_GraphicObjectsTable;

			pst_Gro = (GRO_tdst_Struct *) _pst_World->st_GraphicObjectsTable.p_Table[l_Index];
			pst_Gro->i->pfn_AddRef(pst_Gro, -1);
			pst_Gro->i->pfn_Destroy(pst_Gro);
			pst_NewGro->i->pfn_AddRef(pst_NewGro, 1);
			_pst_World->st_GraphicObjectsTable.p_Table[l_Index] = _p_New;
		}

		return;
	}

	/* try in material table */
	l_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&_pst_World->st_GraphicMaterialsTable, _p_Old);
	if(l_Index != (LONG) TAB_Cul_BadIndex)
	{
		pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
		pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
		while(pst_PFElem <= pst_PFLastElem)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
			pst_PFElem++;
			if(TAB_b_IsAHole(pst_GO)) continue;

			if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) continue;

			pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Material;
			if((void *) pst_Gro != _p_Old) continue;

			pst_Gro->i->pfn_AddRef(pst_Gro, -1);
			pst_Gro->i->pfn_Destroy(pst_Gro);
			pst_NewGro->i->pfn_AddRef(pst_NewGro, 1);
			pst_GO->pst_Base->pst_Visu->pst_Material = pst_NewGro;
		}

		/* look into material table also */
		ppst_MultiMat = (MAT_tdst_Multi **) TAB_ppv_Ptable_GetFirstElem(&_pst_World->st_GraphicMaterialsTable);
		ppst_LastMultiMat = (MAT_tdst_Multi **) TAB_ppv_Ptable_GetLastElem(&_pst_World->st_GraphicMaterialsTable);
		for(; ppst_MultiMat <= ppst_LastMultiMat; ppst_MultiMat++)
		{
			if((*ppst_MultiMat)->st_Id.i->ul_Type == GRO_MaterialMulti)
			{
				ppst_Mat = (GRO_tdst_Struct **) (*ppst_MultiMat)->dpst_SubMaterial;
				ppst_LastMat = ppst_Mat + (*ppst_MultiMat)->l_NumberOfSubMaterials;
				for(; ppst_Mat < ppst_LastMat; ppst_Mat++)
				{
					if((void *) (*ppst_Mat) == _p_Old)
					{
						(*ppst_Mat)->i->pfn_AddRef(*ppst_Mat, -1);
						(*ppst_Mat)->i->pfn_Destroy(*ppst_Mat);
						pst_NewGro->i->pfn_AddRef(pst_NewGro, 1);
						(*ppst_Mat) = pst_NewGro;
					}
				}
			}
		}

		pst_Gro = (GRO_tdst_Struct *) _pst_World->st_GraphicMaterialsTable.p_Table[l_Index];
		pst_Gro->i->pfn_AddRef(pst_Gro, -1);
		pst_Gro->i->pfn_Destroy(pst_Gro);
		_pst_World->st_GraphicMaterialsTable.p_Table[l_Index] = _p_New;
		pst_NewGro->i->pfn_AddRef(pst_NewGro, 1);
		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_World_Update_RLI(WOR_tdst_World *_pst_World, WOR_tdst_Update_RLI *_pst_Data)
{
#ifdef ACTIVE_EDITORS
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				        l_Op, l_Index, *dl_NewIndex;
	TAB_tdst_PFelem		        *pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject         *pst_GO;
	GRO_tdst_Struct		        *pst_Gro;
	ULONG				        *pul_RLI, ul_C0, ul_C1;
	GEO_tdst_Object		        *pst_Geo;
    LONG                        *pl_I0, *pl_I1;
    float                       *pf_Ratio;
    GEO_tdst_ModifierMorphing	*pst_Morph;
	MDF_tdst_Modifier			*pst_RLICarte; 
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Geo = (GEO_tdst_Object *) _pst_Data->p_Geo;
	l_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&_pst_World->st_GraphicObjectsTable, pst_Geo);
	if(l_Index == (LONG) TAB_Cul_BadIndex) return;

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	while(pst_PFElem <= pst_PFLastElem)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		pst_PFElem++;
		if(TAB_b_IsAHole(pst_GO)) continue;

		if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) continue;

        pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
		if((void *) pst_Gro != pst_Geo) continue;

        if ( pst_Morph = GEO_pst_ModifierMorphing_Get( pst_GO ) )
            GEO_ModifierMorphing_UpdateWhenGeoChange( pst_Morph, _pst_Data, pst_Geo->l_NbPoints );
		if ( pst_RLICarte = MDF_pst_GetByType( pst_GO, MDF_C_Modifier_RLICarte ) )
			GEO_ModifierRLICarte_UpdateWhenGeoChange( pst_RLICarte, _pst_Data, pst_Geo->l_NbPoints );

		pul_RLI = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
		if(!pul_RLI) continue;

		l_Op = _pst_Data->l_Op & WOR_Update_RLI_OpMask;

		/* GameObject use geo and has RLI : do the update */
		if(l_Op == WOR_Update_RLI_Add)
		{
			pul_RLI = OBJ_VertexColor_Realloc( pst_GO, pst_Geo->l_NbPoints );
			/*
			pul_RLI = (ULONG *) MEM_p_Realloc(pul_RLI, (pst_Geo->l_NbPoints + 1) * sizeof(ULONG));
			pul_RLI[0] = pst_Geo->l_NbPoints;
			pst_GO->pst_Base->pst_Visu->dul_VertexColors = pul_RLI;
			*/

			if(_pst_Data->l_Op & WOR_Update_RLI_Barycentre)
			{
				pul_RLI[pst_Geo->l_NbPoints] = COLOR_ul_Blend3
					(
						pul_RLI[1 + _pst_Data->l_Ind0],
						pul_RLI[1 + _pst_Data->l_Ind1],
						pul_RLI[1 + _pst_Data->l_Ind2],
						_pst_Data->f0,
						_pst_Data->f1,
						_pst_Data->f2
					);
			}
			else
			{
				ul_C0 = pul_RLI[1 + _pst_Data->l_Ind0];
				ul_C1 = pul_RLI[1 + _pst_Data->l_Ind1];
				pul_RLI[pst_Geo->l_NbPoints] = LIGHT_ul_Interpol2Colors(ul_C0, ul_C1, _pst_Data->f_Blend);
			}
		}
		else if(l_Op == WOR_Update_RLI_AddSome)
		{
			pul_RLI = OBJ_VertexColor_Realloc( pst_GO, pst_Geo->l_NbPoints );
			//pul_RLI = (ULONG *) MEM_p_Realloc(pul_RLI, (pst_Geo->l_NbPoints + 1) * sizeof(ULONG));
            dl_NewIndex = (LONG *) _pst_Data->l_Ind1;
            if (dl_NewIndex)
            {
    		    for(l_Index = 0; l_Index < _pst_Data->l_OldNbPoints; l_Index++)
	    		{
		    		if(dl_NewIndex[l_Index] == -1) break;
			    	pul_RLI[1 + dl_NewIndex[l_Index]] = pul_RLI[1 + l_Index];
			    }
            }

            if (_pst_Data->l_NbAdded && _pst_Data->aul_AddedRLI)
            {
                ERR_X_Assert(_pst_Data->l_NbAdded == pul_RLI[0]);
                for (l_Index = _pst_Data->l_OldNbPoints; l_Index < _pst_Data->l_NbAdded; l_Index++)
                {
                    pul_RLI[1 + l_Index ] = _pst_Data->aul_AddedRLI[1+l_Index-_pst_Data->l_OldNbPoints];
                }
            }

			pul_RLI[0] = pst_Geo->l_NbPoints;
			pst_GO->pst_Base->pst_Visu->dul_VertexColors = pul_RLI;
		}
        else if (l_Op == WOR_Update_RLI_AddSomeCenter)
        {
			pul_RLI = OBJ_VertexColor_Realloc( pst_GO, pst_Geo->l_NbPoints );
            //pul_RLI = (ULONG *) MEM_p_Realloc(pul_RLI, (pst_Geo->l_NbPoints + 1) * sizeof(ULONG));
            pl_I0 = (LONG *) _pst_Data->l_Ind0;
            pl_I1 = (LONG *) _pst_Data->l_Ind1;
            pf_Ratio = (float *) _pst_Data->l_Ind2;
            for (l_Index = 0; l_Index < _pst_Data->l_NbAdded; l_Index++)
            {
                pul_RLI[ 1 + _pst_Data->l_OldNbPoints + l_Index ] = COLOR_ul_Blend( pul_RLI[ 1 + pl_I0[ l_Index ] ], pul_RLI[ 1 + pl_I1[ l_Index ] ] , pf_Ratio[ l_Index ] );
            }
            //pul_RLI[0] = pst_Geo->l_NbPoints;
			//pst_GO->pst_Base->pst_Visu->dul_VertexColors = pul_RLI;
        }
		else if(l_Op == WOR_Update_RLI_Del)
		{
			pul_RLI[_pst_Data->l_Ind0 + 1] = pul_RLI[_pst_Data->l_Ind1 + 1];
			pul_RLI[0]--;
		}
		else if(l_Op == WOR_Update_RLI_DelSome)
		{
			dl_NewIndex = (LONG *) _pst_Data->l_Ind1;
			for(l_Index = 0; l_Index < pst_Geo->l_NbPoints; l_Index++)
			{
                if(dl_NewIndex[l_Index] == -1) continue;
				pul_RLI[1 + dl_NewIndex[l_Index]] = pul_RLI[1 + l_Index];
			}

			pul_RLI[0] = _pst_Data->l_Ind0;
		}
	}
#endif
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
