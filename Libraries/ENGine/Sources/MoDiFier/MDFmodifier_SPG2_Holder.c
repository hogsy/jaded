/*$T MDFModifier_SPG2Holder.c GC! 1.081 06/09/00 09:00:51 */


/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"

#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_SPG2.h"
#include "MoDiFier/MDFmodifier_SPG2_Holder.h"
#include "GEOmetric/GEOobject.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#include "XenonGraphics/XeRenderer.h"

#include "BASe/BENch/BENch.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "GEOmetric/GEO_SKIN.h"

SPG2Holder_tdst_Modifier *pst_FirstSPG2HolderModifier = NULL;

void SPG2_Modifier_ApplyForInstances(SPG2_tdst_Modifier *pst_SPG2);
void SPG2_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
void GEO_Render_InitRLI(GDI_tdst_DisplayData *_pst_DD, GEO_tdst_Object *pst_Obj, GRO_tdst_Visu *pst_Visu);

void SPG2Holder_Modifier_AddInstances(SPG2_tdst_Modifier *pst_SPG2)
{
    MDF_tdst_Modifier *pst_Mod = MDF_pst_GetByType(pst_SPG2->pSPG2Holder, MDF_C_Modifier_SPG2Holder);

    if(!pst_Mod)
        return;

    SPG2Holder_tdst_Modifier *pHolder = (SPG2Holder_tdst_Modifier *)pst_Mod->p_Data;
    pHolder->st_SPG2List.push_back(pst_SPG2);
}

void SPG2Holder_Modifier_Build(SPG2Holder_tdst_Modifier *_pst_SPG2HolderModifier)
{
    std::list<SPG2_tdst_Modifier*>::iterator itCur = _pst_SPG2HolderModifier->st_SPG2List.begin();
    while(itCur!=_pst_SPG2HolderModifier->st_SPG2List.end())
    {
        GEO_tdst_Object* pst_Obj = (GEO_tdst_Object*)OBJ_p_GetCurrentGeo(((SPG2_tdst_Modifier*)*itCur)->pParentModifier->pst_GO);
        if(!pst_Obj)
            continue;

        // prepare global vertex buffer and color array
        GDI_gpst_CurDD->p_Current_Vertex_List = pst_Obj->dst_Point;
        GDI_gpst_CurDD->pst_CurrentGameObject = ((SPG2_tdst_Modifier*)*itCur)->pParentModifier->pst_GO;
        GEO_Render_InitRLI(GDI_gpst_CurDD , pst_Obj, GDI_gpst_CurDD->pst_CurrentGameObject->pst_Base->pst_Visu);

        // compute spg2
        SPG2_Modifier_ApplyForInstances(*itCur);
        itCur++;
    }

    // compute total number of SPG2 to generate
    SPG2_Instance *p_CacheSPG2;
    _pst_SPG2HolderModifier->NumberOfSPG2Entries = 0;
    itCur = _pst_SPG2HolderModifier->st_SPG2List.begin();
    while(itCur!=_pst_SPG2HolderModifier->st_SPG2List.end())
    {
        /*
        p_CacheSPG2 = &(*itCur)->SPG2_FirstCachedPrimitivs;
        _pst_SPG2HolderModifier->NumberOfSPG2Entries += p_CacheSPG2->a_PtrLA2>>2;

        while(p_CacheSPG2->p_NextCacheLine)
        {
            _pst_SPG2HolderModifier->NumberOfSPG2Entries += p_CacheSPG2->a_PtrLA2>>2;
            p_CacheSPG2 = p_CacheSPG2->p_NextCacheLine;
        }
        */
        _pst_SPG2HolderModifier->NumberOfSPG2Entries += (*itCur)->stInstance.a_PtrLA2>>2;

        itCur++;
    }

    // transform to holder local space and build streams
    MATH_tdst_Matrix stTransform;
    MATH_InvertMatrix(&stTransform, _pst_SPG2HolderModifier->pParentModifier->pst_GO->pst_GlobalMatrix);

    if(_pst_SPG2HolderModifier->pSPG2Instances)
        delete _pst_SPG2HolderModifier->pSPG2Instances;

    if(!_pst_SPG2HolderModifier->NumberOfSPG2Entries)
        return;

    _pst_SPG2HolderModifier->pSPG2Instances = new XeSPG2Instances[_pst_SPG2HolderModifier->NumberOfSPG2Entries];
    XeSPG2Instances * pCurInstance = _pst_SPG2HolderModifier->pSPG2Instances;

    itCur = _pst_SPG2HolderModifier->st_SPG2List.begin();
    while(itCur!=_pst_SPG2HolderModifier->st_SPG2List.end())
    {
        p_CacheSPG2 = &(*itCur)->stInstance;//(*itCur)->SPG2_FirstCachedPrimitivs;

        p_CacheSPG2->fMaxSpriteSize = 0.0f;

        //while(p_CacheSPG2)
        {
            SOFT_tdst_AVertex *Coordinates = p_CacheSPG2->a_PointLA2;
            SOFT_tdst_AVertex stPos,stRotX,stRotY,stRotZ;
            for(unsigned int i=0; i<p_CacheSPG2->a_PtrLA2>>2; i++)
            {
                stPos = *(Coordinates ++);// Pos
                stRotY = *(Coordinates ++); // Xa 
                stRotX = *(Coordinates ++); // Ya
                stRotZ = *(Coordinates ++); // Za

                p_CacheSPG2->fMaxSpriteSize = max( stRotX.w, p_CacheSPG2->fMaxSpriteSize );

                MATH_NormalizeVector((MATH_tdst_Vector*)&stRotX, (MATH_tdst_Vector*)&stRotX);
                MATH_NormalizeVector((MATH_tdst_Vector*)&stRotY, (MATH_tdst_Vector*)&stRotY);
                MATH_NormalizeVector((MATH_tdst_Vector*)&stRotZ, (MATH_tdst_Vector*)&stRotZ);

                // stRotX.w is local scale
                MATH_ScaleVector((MATH_tdst_Vector*)&stRotX, (MATH_tdst_Vector*)&stRotX, stRotX.w);
                MATH_ScaleVector((MATH_tdst_Vector*)&stRotY, (MATH_tdst_Vector*)&stRotY, -stRotX.w);
                MATH_ScaleVector((MATH_tdst_Vector*)&stRotZ, (MATH_tdst_Vector*)&stRotZ, stRotX.w);

                MATH_TransformVertex(&pCurInstance->vPos, (*itCur)->pParentModifier->pst_GO->pst_GlobalMatrix, (MATH_tdst_Vector*)&stPos);
                MATH_TransformVertex(&pCurInstance->vPos, &stTransform, &pCurInstance->vPos);

                MATH_TransformVector(&pCurInstance->vX, (*itCur)->pParentModifier->pst_GO->pst_GlobalMatrix, (MATH_tdst_Vector*)&stRotX);
                MATH_TransformVector(&pCurInstance->vX, &stTransform, &pCurInstance->vX);

                MATH_TransformVector(&pCurInstance->vY, (*itCur)->pParentModifier->pst_GO->pst_GlobalMatrix, (MATH_tdst_Vector*)&stRotY);
                MATH_TransformVector(&pCurInstance->vY, &stTransform, &pCurInstance->vY);

                MATH_TransformVector(&pCurInstance->vZ, (*itCur)->pParentModifier->pst_GO->pst_GlobalMatrix, (MATH_tdst_Vector*)&stRotZ);
                MATH_TransformVector(&pCurInstance->vZ, &stTransform, &pCurInstance->vZ);

                pCurInstance++;
            }

            //p_CacheSPG2 = p_CacheSPG2->p_NextCacheLine;
        }
        itCur++;
    }
    

    // create geometry stream and mesh
    //...
}

void SPG2Holder_Modifier_Prepare()
{
    SPG2Holder_tdst_Modifier *pst_SPG2HolderModifier = pst_FirstSPG2HolderModifier;

    // clear all holders
    while(pst_SPG2HolderModifier)
    {
        if(pst_SPG2HolderModifier->pSPG2Instances)
        {
            delete pst_SPG2HolderModifier->pSPG2Instances;
            pst_SPG2HolderModifier->pSPG2Instances = NULL;
        }
        pst_SPG2HolderModifier->st_SPG2List.clear();
        pst_SPG2HolderModifier = pst_SPG2HolderModifier->p_NextSPG2Holder;
    }

    // affect all SPG2 to appropriate holders
    extern SPG2_tdst_Modifier *pst_FirstSPG2Modifier;
    SPG2_tdst_Modifier *pst_Parser = pst_FirstSPG2Modifier ;
    while (pst_Parser)
    {
        if(pst_Parser->pSPG2Holder)
        {
            SPG2Holder_Modifier_AddInstances(pst_Parser);
        }

        pst_Parser = pst_Parser->p_NextSPG2;
    }

    // setup holder with necessary data from spg2
    pst_SPG2HolderModifier = pst_FirstSPG2HolderModifier;
    while(pst_SPG2HolderModifier)
    {
        SPG2Holder_Modifier_Build(pst_SPG2HolderModifier);
        pst_SPG2HolderModifier = pst_SPG2HolderModifier->p_NextSPG2Holder;
    }
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void SPG2Holder_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	SPG2Holder_tdst_Modifier *pst_Data;
	
	_pst_Mod->p_Data = new SPG2Holder_tdst_Modifier;
	pst_Data = (SPG2Holder_tdst_Modifier *) _pst_Mod->p_Data;
	
    pst_Data->st_SPG2List.clear();

    pst_Data->pParentModifier = _pst_Mod;
    pst_Data->pSPG2Instances = NULL;

	_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyRender;

    // update global spg2holder list
    pst_Data->p_NextSPG2Holder = pst_FirstSPG2HolderModifier;
    pst_FirstSPG2HolderModifier = pst_Data;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void SPG2Holder_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	SPG2Holder_tdst_Modifier *pst_Data;
	
	pst_Data = (SPG2Holder_tdst_Modifier *) _pst_Mod->p_Data;	

    SPG2Holder_tdst_Modifier *p_Finder = pst_FirstSPG2HolderModifier;

    // update global spg2holder list
    if (pst_Data == p_Finder) 
        pst_FirstSPG2HolderModifier = p_Finder->p_NextSPG2Holder;
    else
    {
        while (p_Finder->p_NextSPG2Holder != pst_Data)
            p_Finder = p_Finder->p_NextSPG2Holder;
        p_Finder->p_NextSPG2Holder = p_Finder->p_NextSPG2Holder->p_NextSPG2Holder;
    }

	if(!pst_Data) 
		return;

    if(pst_Data->pSPG2Instances)
        delete pst_Data->pSPG2Instances;

	delete pst_Data;
	_pst_Mod->p_Data = NULL;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void SPG2Holder_Modifier_Apply( MDF_tdst_Modifier *_pst_Mod,	GEO_tdst_Object	*_pst_Obj )
{
	SPG2Holder_tdst_Modifier *pst_Data = (SPG2Holder_tdst_Modifier *) _pst_Mod->p_Data;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void SPG2Holder_Modifier_Unapply( MDF_tdst_Modifier *_pst_Mod,	GEO_tdst_Object	*_pst_Obj )
{ 
	SPG2Holder_tdst_Modifier *_pst_SPG2HolderModifier = (SPG2Holder_tdst_Modifier *) _pst_Mod->p_Data;
    g_oXeRenderer.PostDrawPrimitiveCallback(DPCB_SPG2HOLDER, (void*)_pst_SPG2HolderModifier);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void SPG2Holder_Modifier_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	SPG2Holder_tdst_Modifier *pst_Data = (SPG2Holder_tdst_Modifier *) _pst_Mod->p_Data;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG SPG2Holder_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	SPG2Holder_tdst_Modifier		*pst_Data;
	ULONG							ul_Version;
	unsigned char					*pc_Cur;

	pc_Cur = (UCHAR*)_pc_Buffer;
	pst_Data = (SPG2Holder_tdst_Modifier *) _pst_Mod->p_Data;
	
	ul_Version = LOA_ReadLong((CHAR**)&pc_Cur);

    _pst_Mod-> ul_Flags |= MDF_C_Modifier_ApplyRender;
		
	return pc_Cur - (UCHAR*)_pc_Buffer;
}

#ifdef ACTIVE_EDITORS

/*
=======================================================================================================================
=======================================================================================================================
*/
void SPG2Holder_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	SPG2Holder_tdst_Modifier		*pst_Data;
	ULONG							ul_Version;

	pst_Data = (SPG2Holder_tdst_Modifier *) _pst_Mod->p_Data;

	// Save version
	ul_Version = 1;
	SAV_Buffer(&ul_Version, 4);
}

#endif
