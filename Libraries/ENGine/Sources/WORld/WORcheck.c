/*$T WORcheck.c GC! 1.081 09/10/02 10:42:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "ENGine/Sources/WORld/WORcheck.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINKs/LINKmsg.h"
#include "MATerial/MATstruct.h"
#include "GEOmetric/GEOstaticlod.h"
#include "EDIpaths.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GAO.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "MoDiFier/MDFmodifier_XMEN.h"

static char					sgsz_Message[512];

/* Parse all file on bigfile to find some data */
static int					si_Total[3];

extern	MDF_tdst_Modifier	*GAO_ModifierPhoto_Get(OBJ_tdst_GameObject *, BOOL);
extern	BAS_tdst_barray      WOR_ListAllKeys;
		BAS_tdst_barray      WOR_AllRef;
		int					 WOR_AllRefInit = 0;
		int					 WOR_AllRef_DoIt = 1;

extern void                 COL_ResolveRefs(OBJ_tdst_GameObject *_pst_GO);
/*$4
 ***********************************************************************************************************************
    Table des refs
 ***********************************************************************************************************************
 */

void WORCheck_AllRef_Close( void )
{
	if ( !WOR_AllRefInit ) return;
	WOR_AllRefInit = 0;
	BAS_bfree( &WOR_AllRef );
}

void WORCheck_AllRef_Init( void )
{
	if (!WOR_AllRef_DoIt) return;

	if (WOR_AllRefInit)
		WORCheck_AllRef_Close();
	BAS_binit( &WOR_AllRef, 1000 );
	WOR_AllRefInit = 1;
}

void WORCheck_AllRef_AddRef( ULONG ul_Father, ULONG ul_Child )
{
	if (!WOR_AllRefInit)
		WORCheck_AllRef_Init();

	BAS_bsortmode = FALSE;
	BAS_binsert( ul_Father, ul_Child, &WOR_AllRef);
	BAS_bsortmode = TRUE;
}

ULONG WORCheck_AllRef_FatherOf( ULONG ul_Child, ULONG *List, ULONG Max )
{
	BAS_tdst_Key	*base, *last;
	ULONG			nb;

	nb = 0;
	if (WOR_AllRefInit) 
	{
		base = WOR_AllRef.base;
		last = WOR_AllRef.base + WOR_AllRef.num;
		for ( ; base < last; base++)
		{
			if ( base->ul_Val == ul_Child )
			{
				List[ nb++ ] = base->ul_Key;
				if (nb >= Max) return nb;

			}
		}
	}
	return nb;
}

ULONG WORCheck_AllRef_ChildOf( ULONG ul_Father, ULONG *List, ULONG Max )
{
	BAS_tdst_Key	*base, *last;
	ULONG nb;
	
	nb = 0;
	if (WOR_AllRefInit) 
	{
		base = WOR_AllRef.base;
		last = WOR_AllRef.base + WOR_AllRef.num;
		for ( ; base < last; base++)
		{
			if ( base->ul_Key == ul_Father )
			{
				List[ nb++ ] = base->ul_Val;
				if (nb >= Max) return nb;

			}
		}
	}
	return nb;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_FileFatherOfFile( ULONG _ul_File )
{
	ULONG				ul_Ref[ 5000 ];
    ULONG               ul_Index;
    int                 i, number;
    char                sz_Text[ 512 ];

	if (!WOR_AllRefInit || !WOR_AllRef_DoIt)
	{
		LINK_PrintStatusMsg( "All Ref tables not initialized" );
		return;
	}

	number = WORCheck_AllRef_FatherOf( BIG_FileKey( _ul_File ), ul_Ref, 5000 );

	sprintf( sz_Text, "[%08X] %s is referenced by %d other files", BIG_FileKey( _ul_File ), BIG_NameFile( _ul_File ), number );
	LINK_PrintStatusMsg( sz_Text );

	for (i = 0; i < number; i++)
	{
		ul_Index = BIG_ul_SearchKeyToFat( ul_Ref[ i ] );
		if (ul_Index == BIG_C_InvalidIndex )
			sprintf( sz_Text, "....[%08X] ????", ul_Ref[ i ] );
		else
			sprintf( sz_Text, "....[%08X] %s", ul_Ref[ i ], BIG_NameFile( ul_Index ) );
		LINK_PrintStatusMsg( sz_Text );
	}
}


/*$4
 ***********************************************************************************************************************
    Table des Gameobjects
 ***********************************************************************************************************************
 */

BAS_tdst_barray				WORCheck_AllGao;
int							WORCheck_AllGaoLoaded = 0;
int							WORCheck_AllGaoNb;
typedef struct				WORCheck_tdst_Gao_
{
	ULONG	ul_Gro;
	ULONG	ul_Mat;
	ULONG	ul_Light;
	ULONG	ul_AIModel;
	ULONG	ul_AIInstance;
} WORCheck_tdst_Gao;
OBJ_tdst_GameObject         *WORCheck_Gao[ 1024 ];
int                         WORCheck_NbGao;
void                        *WORCheck_Gro[ 1024 ];
int                         WORCheck_NbGro;
void                        *WORCheck_Network[ 256 ];
int                         WORCheck_NbNetwork;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_LoadAllGao_Free( )
{
    OBJ_tdst_GameObject     *pst_Gao;
    int                     i, j, k;
    WAY_tdst_Struct         *pst_Way;

    for (i = 0; i< WORCheck_NbGao; i++)
    {
        pst_Gao = WORCheck_Gao[ i ];
         
        if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (pst_Gao->pst_Extended))
        {
            if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Links) && (pst_Gao->pst_Extended->pst_Links) )
            {
                pst_Way = (WAY_tdst_Struct*)pst_Gao->pst_Extended->pst_Links;
                if (pst_Way->pst_AllLinks)
                {
                    for(j = 0; j < (int) pst_Way->ul_Num; j++)
                    {
                        if (pst_Way->pst_AllLinks[j].pst_Network)
                        {
                            for (k = 0; k < WORCheck_NbNetwork; k++)
                            {
                                if (WORCheck_Network[k] == pst_Way->pst_AllLinks[j].pst_Network)
                                    break;
                            }
                            if (k == WORCheck_NbNetwork)
                            {
                                WORCheck_Network[WORCheck_NbNetwork++] = pst_Way->pst_AllLinks[j].pst_Network;
                                LOA_DeleteAddress(pst_Way->pst_AllLinks[j].pst_Network);
			                    MEM_Free(pst_Way->pst_AllLinks[j].pst_Network);
                            }
                        }
                    }
                }
            }
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int WORCheck_LoadAllGao_UpdateRef( OBJ_tdst_GameObject *pst_Gao )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject             *pst_Father;
    MAT_tdst_Multi		            *pst_MultiMat;
    int                             i, j, res;
    TAB_tdst_PFelem		            *pst_CurrentElem, *pst_EndElem;
    EVE_tdst_ListTracks             *pst_LT;
    MDF_tdst_Modifier               *pst_Mod;
    GAO_tdst_ModifierXMEN           *p_XMEN;
    GAO_tdst_ModifierSpecialLookAt  *pst_LookAt;
    GAO_tdst_ModifierLegLink        *pst_LegLink;
    GEO_tdst_StaticLOD              *pst_Lod;
    WAY_tdst_Struct                 *pst_Way;
    LIGHT_tdst_Light	            *pst_Light;
    COL_tdst_ColMap                 *pst_ColMap;
    ULONG                           ul_Pos;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ( ((ULONG) pst_Gao == -1) || ((ULONG) pst_Gao == 0) ) 
        return 0;


    res = 0;
    for (i = 0; i < WORCheck_NbGao; i++)
        if (WORCheck_Gao[i] == pst_Gao) 
            return (i == 0) ? 1 : 0;
    WORCheck_Gao[WORCheck_NbGao++] = pst_Gao;

    if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_BaseObject) && (pst_Gao->pst_Base))
    {
        if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu) && (pst_Gao->pst_Base->pst_Visu))
	    {
		    if(pst_Gao->pst_Base->pst_Visu->pst_Object)
            {
                pst_Gao->pst_Base->pst_Visu->pst_Object->l_Ref++;

                if (pst_Gao->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
                {
                    pst_Lod = (GEO_tdst_StaticLOD *) pst_Gao->pst_Base->pst_Visu->pst_Object;
                    for (i = 0; i < WORCheck_NbGro; i++)
                    {
                        if (WORCheck_Gro[ i ] == pst_Lod)
                            break;
                    }
                    if (i == WORCheck_NbGro)
                    {
                        WORCheck_Gro[ WORCheck_NbGro++ ] = pst_Lod;
                        for (i = 0; i < pst_Lod->uc_NbLOD; i++)
                        {
                            if ( !pst_Lod->dpst_Id[i] ) continue;

                            if ( pst_Lod->dpst_Id[i]->i->ul_Type != GRO_Geometric )
                            {
                                ERR_X_Warning(0, "LOD contains something that is not a geometry.", pst_Lod->st_Id.sz_Name );
                                pst_Lod->dpst_Id[i] = NULL;
                                continue;
                            }
                            pst_Lod->dpst_Id[i]->l_Ref++;
                        }
                    }
                    
                }
            }
                
		    if(pst_Gao->pst_Base->pst_Visu->pst_Material)
            {
                pst_Gao->pst_Base->pst_Visu->pst_Material->l_Ref++;
                if (pst_Gao->pst_Base->pst_Visu->pst_Material->i->ul_Type == GRO_MaterialMulti)
                {
                    pst_MultiMat = (MAT_tdst_Multi *) pst_Gao->pst_Base->pst_Visu->pst_Material;
                    for (i = 0; i < WORCheck_NbGro; i++)
                    {
                        if (WORCheck_Gro[ i ] == pst_MultiMat)
                            break;
                    }
                    if (i == WORCheck_NbGro)
                    {
                        WORCheck_Gro[ WORCheck_NbGro++ ] = pst_MultiMat;

                        for (i = 0; i < pst_MultiMat->l_NumberOfSubMaterials; i++)
                        {
                            if (!pst_MultiMat->dpst_SubMaterial[i]) continue;

                            if ( (pst_MultiMat->dpst_SubMaterial[i]->st_Id.i->ul_Type != GRO_MaterialSingle) && (pst_MultiMat->dpst_SubMaterial[i]->st_Id.i->ul_Type != GRO_MaterialMultiTexture) )
                            {
                                ERR_X_Warning(0, "There's a multimaterial into a multimaterial.", pst_MultiMat->st_Id.sz_Name );
                                pst_MultiMat->dpst_SubMaterial[i] = NULL;
                                continue;
                            }
                            pst_MultiMat->dpst_SubMaterial[i]->st_Id.l_Ref++;
                        }
                    }
                }
            }
	    }

        if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims) && (pst_Gao->pst_Base->pst_GameObjectAnim) )
        {
			//pst_Gao->pst_Base->pst_GameObjectAnim->pst_ActionKit->uw_Counter++;
			pst_Gao->pst_Base->pst_GameObjectAnim->pst_ActionKit = NULL;
            pst_Gao->pst_Base->pst_GameObjectAnim->pst_SkeletonModel = NULL;
            pst_Gao->pst_Base->pst_GameObjectAnim->apst_Anim[0] = NULL;
            pst_Gao->pst_Base->pst_GameObjectAnim->apst_Anim[1] = NULL;
            pst_Gao->pst_Base->pst_GameObjectAnim->apst_Anim[2] = NULL;
            pst_Gao->pst_Base->pst_GameObjectAnim->apst_Anim[3] = NULL;
        }

        if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && (pst_Gao->pst_Base->pst_Hierarchy->pst_FatherInit) )
        {
            res |= WORCheck_LoadAllGao_UpdateRef( pst_Gao->pst_Base->pst_Hierarchy->pst_FatherInit );
        }
        
        if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix) && (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer) && (pst_Gao->pst_Base->pst_AddMatrix) )
        {
            for(i = 0; i < pst_Gao->pst_Base->pst_AddMatrix->l_Number; i++)
            {
                res |=WORCheck_LoadAllGao_UpdateRef( pst_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[ i ].pst_GO );
            }
        }
    }
		
    if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (pst_Gao->pst_Extended))
    {
        if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) &&	(pst_Gao->pst_Extended->pst_Ai)	)
	    {
           	if(((AI_tdst_Instance *) pst_Gao->pst_Extended->pst_Ai)->pst_Model)
                ((AI_tdst_Instance *) pst_Gao->pst_Extended->pst_Ai)->pst_Model->uw_NbInstances++;
        }
        
        if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights) && (pst_Gao->pst_Extended->pst_Light ) )
        {
            pst_Gao->pst_Extended->pst_Light->i->pfn_AddRef( pst_Gao->pst_Extended->pst_Light, 1);
            pst_Light = (LIGHT_tdst_Light *) pst_Gao->pst_Extended->pst_Light;
            res |= WORCheck_LoadAllGao_UpdateRef( pst_Light->pst_GO );
        }

        if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Group) &&	(pst_Gao->pst_Extended->pst_Group) )
        {
			pst_Gao->pst_Extended->pst_Group->ul_NbObjectsUsingMe++;

            pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Gao->pst_Extended->pst_Group->pst_AllObjects);
            pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Gao->pst_Extended->pst_Group->pst_AllObjects);
			for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
			    pst_Father = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
				if(TAB_b_IsAHole(pst_Father)) continue;
                res|= WORCheck_LoadAllGao_UpdateRef (pst_Father);
            }
        }

        if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Events) && (pst_Gao->pst_Extended->pst_Events ) )
        {
            pst_LT = (EVE_tdst_ListTracks *) pst_Gao->pst_Extended->pst_Events->pst_ListTracks;
            if (pst_LT)
            {
                pst_LT->ul_NbOfInstances++;
                for(i = 0; i < pst_LT->uw_NumTracks; i++)
	            {
		            if( !( pst_LT->pst_AllTracks[i].uw_Flags & EVE_C_Track_Anims ) )
		            {
                        res|= WORCheck_LoadAllGao_UpdateRef (pst_LT->pst_AllTracks[i].pst_GO);
			        }
                }
            }
        }

        if (pst_Gao->pst_Extended->pst_Modifiers)
        {
            pst_Mod = pst_Gao->pst_Extended->pst_Modifiers;
            while ( pst_Mod )
            {
                if (pst_Mod->i->ul_Type == MDF_C_Modifier_XMEN)
                {
                    p_XMEN = (GAO_tdst_ModifierXMEN *)pst_Mod->p_Data;
                    if (p_XMEN->p_MaterialUsed)
                        p_XMEN->p_MaterialUsed->st_Id.i->pfn_AddRef(p_XMEN->p_MaterialUsed, 1);
                }
                else if (pst_Mod->i->ul_Type == MDF_C_Modifier_SpecialLookAt)
                {
                    pst_LookAt = (GAO_tdst_ModifierSpecialLookAt *) pst_Mod->p_Data;
                    res|= WORCheck_LoadAllGao_UpdateRef( (OBJ_tdst_GameObject*)pst_LookAt->p_GAO );
                }
                else if (pst_Mod->i->ul_Type == MDF_C_Modifier_LegLink)
                {
                    pst_LegLink = (GAO_tdst_ModifierLegLink *) pst_Mod->p_Data;
                    
                    ul_Pos = LOA_ul_SearchIndexWithAddress( (ULONG) pst_LegLink->p_GAOA );
                    if(ul_Pos != BIG_C_InvalidIndex)
                        res|= WORCheck_LoadAllGao_UpdateRef( (OBJ_tdst_GameObject*)pst_LegLink->p_GAOA );
                    
                    ul_Pos = LOA_ul_SearchIndexWithAddress( (ULONG) pst_LegLink->p_GAOC);
                    if ( ul_Pos != BIG_C_InvalidIndex )
                         res|= WORCheck_LoadAllGao_UpdateRef( (OBJ_tdst_GameObject*)pst_LegLink->p_GAOC );

                    ul_Pos = LOA_ul_SearchIndexWithAddress( (ULONG) pst_LegLink->p_GaoOrient );
                    if(ul_Pos != BIG_C_InvalidIndex)
                         res|= WORCheck_LoadAllGao_UpdateRef( (OBJ_tdst_GameObject*)pst_LegLink->p_GaoOrient );
               }
                pst_Mod = pst_Mod->pst_Next;
            }
        }

        if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Links) && (pst_Gao->pst_Extended->pst_Links) )
        {
            pst_Way = (WAY_tdst_Struct*)pst_Gao->pst_Extended->pst_Links;
            if (pst_Way->pst_AllLinks)
            {
                for(j = 0; j < (int) pst_Way->ul_Num; j++)
                {
                    if (pst_Way->pst_AllLinks[j].pst_Network)
                        res |= WORCheck_LoadAllGao_UpdateRef( pst_Way->pst_AllLinks[j].pst_Network->pst_Root );
                    for(i = 0; i < (int) pst_Way->pst_AllLinks[j].ul_Num; i++)
                        res |= WORCheck_LoadAllGao_UpdateRef( pst_Way->pst_AllLinks[j].pst_Links[i].pst_Next );
                }
            }
        }

        
        if(OBJ_b_TestIdentityFlag(pst_Gao, (OBJ_C_IdentityFlag_ColMap | OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)))
        {
            pst_ColMap = ((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_ColMap;
            if ( ( pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap ) && (!pst_ColMap) )
            {
                COL_tdst_ColMap st_ColMap;
                COL_tdst_Cob	st_Cob;
                COL_tdst_Cob	*pst_Cob;

                st_ColMap.uc_NbOfCob = 1;
                st_ColMap.dpst_Cob = &pst_Cob;
                pst_Cob = &st_Cob;
                st_Cob.uc_Type = COL_C_Zone_Sphere;
                st_Cob.pst_GMatList = NULL;
                st_Cob.sz_GMatName = NULL;

                ((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_ColMap = &st_ColMap;
                COL_ResolveRefs( pst_Gao);
                ((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_ColMap = NULL;

                MEM_Free( st_Cob.sz_GMatName );
            }
            else
            {
                COL_ResolveRefs( pst_Gao);
            }
        }
	}
    return res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_LoadAllGao_Load(ULONG ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Gao, *pst_InMem;
	WORCheck_tdst_Gao	*pst_CGao;
    int                 i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CGao = (WORCheck_tdst_Gao	*) BAS_bsearch( ul_File, &WORCheck_AllGao );
	if ((ULONG) pst_CGao != 0xFFFFFFFF ) return;

	pst_InMem = pst_Gao = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_File));
	if( (ULONG) pst_Gao == BIG_C_InvalidKey )
	{
		LOA_MakeFileRef(BIG_FileKey(ul_File), (ULONG *) &pst_Gao, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
		LOA_Resolve();
		if(!pst_Gao) return;
	}

	WORCheck_NbGao = 0;
    WORCheck_NbGro = 0;
    WORCheck_NbNetwork = 0;
#ifdef JADEFUSION
	if ( WORCheck_LoadAllGao_UpdateRef( pst_Gao ) ) pst_InMem = (OBJ_tdst_GameObject*)1;
#else
	if ( WORCheck_LoadAllGao_UpdateRef( pst_Gao ) ) (ULONG) pst_InMem = 1;
#endif
	WORCheck_LoadAllGao_Free();

	for (i = 0; i < WORCheck_NbGao; i++)
	{
		pst_Gao = WORCheck_Gao[i];
		if (i)
		{
			ul_File = LOA_ul_SearchIndexWithAddress( (ULONG ) pst_Gao );
			if (ul_File == BIG_C_InvalidIndex ) continue;
			pst_CGao = (WORCheck_tdst_Gao	*) BAS_bsearch( ul_File, &WORCheck_AllGao );
			if ((ULONG) pst_CGao != 0xFFFFFFFF ) continue;
		}
		
		pst_CGao = (WORCheck_tdst_Gao*)L_malloc(sizeof(WORCheck_tdst_Gao));
		L_memset(pst_CGao, 0, sizeof(WORCheck_tdst_Gao));
		BAS_binsert(ul_File, (ULONG) pst_CGao, &WORCheck_AllGao);

		if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_BaseObject) && (pst_Gao->pst_Base))
		{
			if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu) && (pst_Gao->pst_Base->pst_Visu))
			{
				if(pst_Gao->pst_Base->pst_Visu->pst_Object)
					pst_CGao->ul_Gro = LOA_ul_SearchIndexWithAddress((ULONG) pst_Gao->pst_Base->pst_Visu->pst_Object);
            
			    if(pst_Gao->pst_Base->pst_Visu->pst_Material)
		            pst_CGao->ul_Mat = LOA_ul_SearchIndexWithAddress((ULONG) pst_Gao->pst_Base->pst_Visu->pst_Material);
		    }
		}

		if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (pst_Gao->pst_Extended))
		{
			if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights) && (pst_Gao->pst_Extended->pst_Light))
				pst_CGao->ul_Light = LOA_ul_SearchIndexWithAddress((ULONG) pst_Gao->pst_Extended->pst_Light);

			if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) &&	(pst_Gao->pst_Extended->pst_Ai)	)
			{
    			pst_CGao->ul_AIInstance = LOA_ul_SearchIndexWithAddress((ULONG) pst_Gao->pst_Extended->pst_Ai);
	    		if(((AI_tdst_Instance *) pst_Gao->pst_Extended->pst_Ai)->pst_Model)
    			{
	    			pst_CGao->ul_AIModel = LOA_ul_SearchIndexWithAddress((ULONG) ((AI_tdst_Instance *) pst_Gao->pst_Extended->pst_Ai)->pst_Model);
				}
			}
		}
	}

    for (i = 0; i < WORCheck_NbGao; i++)
        OBJ_GameObject_Remove(WORCheck_Gao[i], 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_LoadAllGao_ParseDir(ULONG ul_Dir)
{
	/*~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	/*~~~~~~~~~~~~~*/
    
	/* sub dir */
	ul_Index = BIG_SubDir(ul_Dir);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		WORCheck_LoadAllGao_ParseDir(ul_Index);
		ul_Index = BIG_NextDir(ul_Index);
	}

	/* file */
	ul_Index = BIG_FirstFile(ul_Dir);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		if(BIG_b_IsFileExtensionIn(ul_Index, ".gao")) 
        {
            if ( WORCheck_AllGao.num >= WORCheck_AllGaoNb )
            {
                char sz_Value[10];
                LINK_PrintStatusMsg( _itoa( WORCheck_AllGao.num, sz_Value, 10 ) );
				WORCheck_AllGaoNb += 100;
            }
            //if (WORCheck_AllGao.num >= 33000 )
				WORCheck_LoadAllGao_Load(ul_Index);
        }
		ul_Index = BIG_NextFile(ul_Index);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_LoadAllGao(void)
{
	if(WORCheck_AllGaoLoaded) return;

	WORCheck_AllGaoLoaded = 1;
    WORCheck_AllGaoNb = 100;
	BAS_binit(&WORCheck_AllGao, 256);

    BAS_binit(&WOR_ListAllKeys, 100);
	WORCheck_LoadAllGao_ParseDir(BIG_Root());
    BAS_bfree(&WOR_ListAllKeys);
	BAS_bsort(&WORCheck_AllGao);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_FreeAllGao(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BAS_tdst_Key	*pst_Key, *pst_L;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_L = (pst_Key = WORCheck_AllGao.base) + WORCheck_AllGao.num;
	while(pst_Key < pst_L)
		L_free((WORCheck_tdst_Gao *) pst_Key++->ul_Val);
	BAS_bfree(&WORCheck_AllGao);
	WORCheck_AllGaoLoaded = 0;
}

/*$4
 ***********************************************************************************************************************
    Check des groupes
 ***********************************************************************************************************************
 */
BAS_tdst_barray				WORCheck_AllGroup;
int							WORCheck_AllGroupLoaded = 0;
typedef struct WORCheck_tdst_Group_
{
    int     i_NbRefs;
    ULONG   *dul_Ref;    
} WORCheck_tdst_Group;

BAS_tdst_barray				WORCheck_Files;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_LoadAllGroup_Load( ULONG _ul_Group )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WORCheck_tdst_Group	*pst_CGrp;
    BIG_tdst_GroupElem  *pst_Buf;
    ULONG               ul_Size;
    int                 i, j;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* pour zapper les sauvegardes textes de Jacques */
    if (BIG_gst.dst_FileTableExt[ _ul_Group ].st_ToSave.ul_LengthOnDisk > 200000 )
        return;
    
    pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Group), &ul_Size);
    pst_CGrp = (WORCheck_tdst_Group*)L_malloc(sizeof(WORCheck_tdst_Group));
    pst_CGrp->i_NbRefs = ul_Size / sizeof( BIG_tdst_GroupElem );
    pst_CGrp->dul_Ref = (ULONG*)L_malloc( 4 * pst_CGrp->i_NbRefs );

    for( i=0, j=0; i < pst_CGrp->i_NbRefs; i++)
    {
        if (pst_Buf[i].ul_Type != 0)
            pst_CGrp->dul_Ref[ j++ ] = pst_Buf[i].ul_Key;
    }
    pst_CGrp->i_NbRefs = j;

    BAS_binsert( _ul_Group, (ULONG) pst_CGrp, &WORCheck_AllGroup);
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_LoadAllGroup_ParseDir(ULONG ul_Dir)
{
	/*~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	/*~~~~~~~~~~~~~*/
    
	/* sub dir */
	ul_Index = BIG_SubDir(ul_Dir);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		WORCheck_LoadAllGroup_ParseDir(ul_Index);
		ul_Index = BIG_NextDir(ul_Index);
	}

	/* file */
	ul_Index = BIG_FirstFile(ul_Dir);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		if( BIG_b_IsGrpFile(ul_Index) )
            WORCheck_LoadAllGroup_Load(ul_Index);
		ul_Index = BIG_NextFile(ul_Index);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_LoadAllGroup( void )
{
    if(WORCheck_AllGroupLoaded) return;

	WORCheck_AllGroupLoaded = 1;
	BAS_binit(&WORCheck_AllGroup, 256);
	WORCheck_LoadAllGroup_ParseDir(BIG_Root());
	BAS_bsort(&WORCheck_AllGroup);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_GroupStat( void )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                sz_Text[ 512 ];
	BAS_tdst_Key	    *pst_Key, *pst_L;
    WORCheck_tdst_Group	*GRP;
    int                 i, j, type, ai_GroupTypeNb[ 16 ];
    int                 i_NbRefs;
    char                *psz_Temp, sz_UnknowExt[] = ""; 
    char                *sz_Ext[ 15 ] = {   
                                        EDI_Csz_ExtAIEditorModel, EDI_Csz_ExtAIEngineModel, EDI_Csz_ExtAIEditorDepend,
                                        EDI_Csz_ExtGameObjects, EDI_Csz_ExtAnimTbl, EDI_Csz_ExtActionKit, EDI_Csz_ExtObjModels,
                                        EDI_Csz_ExtObjGolGroups, EDI_Csz_ExtGrpWorld, EDI_Csz_ExtWorldList, 
                                        EDI_Csz_ExtTextLang, EDI_Csz_ExtSoundBank, EDI_Csz_ExtSoundMetaBank, EDI_Csz_ExtWorldText,
                                        };
    typedef struct tdst_RefType_
    {
        char    ext[8];
        int     nb;
    } tdst_RefType;

    tdst_RefType        ast_RefType[ 16 ][ 32 ];
    int                 ai_RefTypeNb[16];
    int                 ai_BadRefNb[ 16 ];
    int                 ai_RefTypeCur[ 16 ];
    ULONG               ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    LINK_PrintStatusMsg( "==< Group Information >==" );
    sprintf( sz_Text, "%d group found", WORCheck_AllGroup.num );
    LINK_PrintStatusMsg( sz_Text );

    /* init stats */
    i_NbRefs = 0;
    L_memset( ai_GroupTypeNb, 0, sizeof( ai_GroupTypeNb ) );
    L_memset( ast_RefType, 0, sizeof( ast_RefType ) );
    L_memset( ai_RefTypeNb, 0, sizeof( ai_RefTypeNb ) );
    L_memset( ai_BadRefNb, 0, sizeof( ai_BadRefNb ) );
    L_memset( ai_RefTypeCur, 0, sizeof( ai_RefTypeCur ) );

    /* get stats */
    pst_Key = WORCheck_AllGroup.base;
	pst_L = WORCheck_AllGroup.base + WORCheck_AllGroup.num;
	while(pst_Key < pst_L)
	{
        psz_Temp = L_strrchr(BIG_NameFile( pst_Key->ul_Key ), '.');
        if(!psz_Temp) i = 15;
        else
        {
            for (i = 0; i < 15; i++)
                if (!L_strnicmp(psz_Temp, sz_Ext[i], 4)) break;
        }
        type = i;
        ai_GroupTypeNb[type]++;

        GRP = (WORCheck_tdst_Group *) pst_Key->ul_Val;
        i_NbRefs += GRP->i_NbRefs;

        for (i = 0; i < GRP->i_NbRefs; i++)
        {
            ul_Index = BIG_ul_SearchKeyToFat( GRP->dul_Ref[ i ] );
            if (ul_Index == BIG_C_InvalidIndex )
            {
                sprintf( sz_Text, "[%08X] %s has a bad key [%08X - rank %d]", BIG_FileKey( pst_Key->ul_Key), BIG_NameFile( pst_Key->ul_Key ), GRP->dul_Ref[ i ], i );
                LINK_PrintStatusMsg( sz_Text );
                ai_BadRefNb[ type ] ++;
            }
            else 
            {
                BIG_FileChanged( ul_Index ) |= EDI_FHC_Loaded;

                psz_Temp = L_strrchr(BIG_NameFile( ul_Index ), '.');
                if (!psz_Temp) psz_Temp = sz_UnknowExt;

                if (ai_RefTypeNb[type] == 0)
                {
                    L_strcpy( ast_RefType[ type ][0].ext, psz_Temp );
                    ast_RefType[ type ][0].nb = 1;
                    ai_RefTypeNb[type]++;
                }
                else if ( !L_strnicmp( psz_Temp, ast_RefType[type][ai_RefTypeCur[type] ].ext, 4) )
                    ast_RefType[ type ][ ai_RefTypeCur[ type ]].nb++;
                else
                {
                    for (j = 0; j < ai_RefTypeNb[type]; j++)
                    {
                        if (!L_strnicmp( psz_Temp, ast_RefType[type][ j ].ext, 4) )
                        {
                            ast_RefType[type][j].nb++;
                            ai_RefTypeCur[type] = j;
                            break;
                        }
                    }
                    if (j == ai_RefTypeNb[type])
                    {
                        L_strcpy( ast_RefType[ type ][ai_RefTypeNb[type]].ext, psz_Temp );
                        ast_RefType[ type ][ai_RefTypeNb[type]].nb = 1;
                        ai_RefTypeCur[type] = ai_RefTypeNb[type]++;
                    }
                }
            }
        }
        pst_Key++;
    }

    LINK_PrintStatusMsg( "==< Group Number by Type >===" );
    for (i = 0; i < 15; i++)
    {
        sprintf( sz_Text, "%s   : %4d", sz_Ext[i], ai_GroupTypeNb[ i ] );
        LINK_PrintStatusMsg( sz_Text );
    }
    sprintf( sz_Text, "Unknow : %4d", ai_GroupTypeNb[ 15 ] );
    LINK_PrintStatusMsg( sz_Text );

    LINK_PrintStatusMsg( "==< Refs >==" );
    sprintf( sz_Text, "Total  : %4d", i_NbRefs );
    LINK_PrintStatusMsg( sz_Text );

    for (i = 0; i < 16; i++)
    {
        sprintf(sz_Text, "..[%s]", (i == 15) ? "unknow" : sz_Ext[i] );
        LINK_PrintStatusMsg( sz_Text );

        sprintf( sz_Text, "....Bad refs = %d", ai_BadRefNb[ i ] );
        LINK_PrintStatusMsg( sz_Text );

        sprintf( sz_Text, "....Nb Type  = %d", ai_RefTypeNb[ i ] );
        LINK_PrintStatusMsg( sz_Text );

        for (j = 0; j < ai_RefTypeNb[i]; j++)
        {
            sprintf( sz_Text, "....Type %2d  = %d (%s)", j, ast_RefType[i][j].nb, ast_RefType[i][j].ext );
            LINK_PrintStatusMsg( sz_Text );
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_GroupOwnerOfFile( ULONG _ul_File )
{
    BAS_tdst_Key	    *pst_Key, *pst_L;
    WORCheck_tdst_Group	*GRP;
    ULONG               ul_Key;
    int                 i, number;
    char                sz_Text[ 512 ];

	//WORCheck_FileFatherOfFile( _ul_File );
	//return;

    WORCheck_LoadAllGroup();

    ul_Key = BIG_FileKey( _ul_File );
    number = 0;

    sprintf( sz_Text, "%s is in", BIG_NameFile( _ul_File ) );
    LINK_PrintStatusMsg( sz_Text );

    pst_Key = WORCheck_AllGroup.base;
	pst_L = WORCheck_AllGroup.base + WORCheck_AllGroup.num;
	while(pst_Key < pst_L)
    {
        GRP = (WORCheck_tdst_Group *) pst_Key->ul_Val;
        for (i = 0; i < GRP->i_NbRefs; i++)
        {
            if ( GRP->dul_Ref[i] == ul_Key )
            {
                sprintf(sz_Text, "[%08X] %s", BIG_FileKey( pst_Key->ul_Key ), BIG_NameFile( pst_Key->ul_Key ) );
                LINK_PrintStatusMsg( sz_Text );
                number++;
            }
        }

        pst_Key++;
    }

    if (number == 0)
        LINK_PrintStatusMsg( "No group" );
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_GroupOwnerOfDir( ULONG _ul_Dir )
{
    /*~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	/*~~~~~~~~~~~~~*/
    
	ul_Index = BIG_FirstFile(_ul_Dir);
	while(ul_Index != BIG_C_InvalidIndex)
	{
        WORCheck_GroupOwnerOfFile( ul_Index );
		ul_Index = BIG_NextFile(ul_Index);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_Files_ParseDir(ULONG ul_Dir, char *_sz_Ext )
{
	/*~~~~~~~~~~~~~*/
	ULONG	ul_Index;
    ULONG   *pul_Data;
	/*~~~~~~~~~~~~~*/

	/* sub dir */
	ul_Index = BIG_SubDir(ul_Dir);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		WORCheck_Files_ParseDir(ul_Index, _sz_Ext );
		ul_Index = BIG_NextDir(ul_Index);
	}

	/* file */
	ul_Index = BIG_FirstFile(ul_Dir);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		if ( BIG_b_IsFileExtension( ul_Index, _sz_Ext ) )
        {
            pul_Data = (ULONG *) L_malloc( 8 );
            pul_Data[0] = ul_Index;
            pul_Data[1] = 0;
            BAS_binsert( BIG_FileKey( ul_Index ), (ULONG) pul_Data, &WORCheck_Files );
            if ( (WORCheck_Files.num % 1000) == 0)
                LINK_PrintStatusMsg( ".\n" );
        }
		ul_Index = BIG_NextFile(ul_Index);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WORCheck_GaoNotInGroup( char *_sz_Ext, ULONG ul_Dir )
{
    BAS_tdst_Key	    *pst_Key, *pst_L;
    WORCheck_tdst_Group	*GRP;
    char                sz_Text[ 256 ];
    int                 i, unrefsize, unref;
    ULONG               *pul_Data;
    
    if ( _sz_Ext[0] != '.') return;

    /* find all file key of type given */
    sprintf( sz_Text, "==> Load all %s key", _sz_Ext + 1 );
    LINK_PrintStatusMsg( sz_Text );
    BAS_binit(&WORCheck_Files, 256);
    WORCheck_Files_ParseDir(ul_Dir, _sz_Ext );
	BAS_bsort(&WORCheck_Files);
    sprintf( sz_Text, "%d %s key found", WORCheck_Files.num, _sz_Ext + 1 );
    LINK_PrintStatusMsg( sz_Text );
    
    if ( WORCheck_Files.num == 0)
    {
        sprintf( sz_Text, "no %s file found in BF", _sz_Ext + 1 );
        LINK_PrintStatusMsg( sz_Text );
        return;
    }

    LINK_PrintStatusMsg( "==> Load all group" );
    WORCheck_LoadAllGroup();

    /* mark gao referenced in group */
    sprintf( sz_Text, "==> Mark all %s found in group", _sz_Ext+1 );
    LINK_PrintStatusMsg( sz_Text );
    
    pst_Key = WORCheck_AllGroup.base;
	pst_L = WORCheck_AllGroup.base + WORCheck_AllGroup.num;
	while(pst_Key < pst_L)
    {
        GRP = (WORCheck_tdst_Group *) pst_Key->ul_Val;
        for (i = 0; i < GRP->i_NbRefs; i++)
        {
            pul_Data = (ULONG *) BAS_bsearch( GRP->dul_Ref[i], &WORCheck_Files );
            if ( (pul_Data) && ( (ULONG) pul_Data != (ULONG) -1) )
                pul_Data[1]++;
        }
        pst_Key++;
    }

    /* display all non referenced gao */
    sprintf( sz_Text, "==> List of all unreferenced %s file", _sz_Ext+1);
    LINK_PrintStatusMsg( sz_Text );
    pst_Key = WORCheck_Files.base;
	pst_L = WORCheck_Files.base + WORCheck_Files.num;
    unref = 0;
    unrefsize = 0;
	while(pst_Key < pst_L)
    {
        pul_Data = (ULONG *) pst_Key->ul_Val;
        if (pul_Data[1] == 0)
        {
            sprintf( sz_Text, "[%08X] %s", pst_Key->ul_Key, BIG_NameFile( pul_Data[0] ) );
            LINK_PrintStatusMsg( sz_Text );
            unref++;
            unrefsize += BIG_LengthDiskFile( pul_Data[0] );
        }
        L_free( pul_Data );
        pst_Key++;
    }

    sprintf( sz_Text, "==> Result : %d unreferenced %s (size = %d)", unref, _sz_Ext+1, unrefsize );
    LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, "==>        : %d %s", WORCheck_Files.num, _sz_Ext+1 );
    LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, "==>        : %.2f percent of %s unreferenced", ((float) unref * 100.0f) / ((float) WORCheck_Files.num), _sz_Ext+1 );
    LINK_PrintStatusMsg( sz_Text );

    BAS_bfree(&WORCheck_Files);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG WORCheck_ul_SearchMaterial(ULONG *pul_SearchIndex, ULONG _ul_MatIndex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BAS_tdst_Key	*pst_Key, *pst_L;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Key = WORCheck_AllGao.base + *pul_SearchIndex;
	pst_L = WORCheck_AllGao.base + WORCheck_AllGao.num;
	while(pst_Key < pst_L)
	{
		if(((WORCheck_tdst_Gao *) pst_Key->ul_Val)->ul_Mat == _ul_MatIndex)
		{
			*pul_SearchIndex = pst_Key - WORCheck_AllGao.base + 1;
			return(pst_Key->ul_Key);
		}
        pst_Key++;
	}

	*pul_SearchIndex = WORCheck_AllGao.num;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_CheckFile(ULONG ul_File)
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Type;
	char	sz_Text[256];
	/*~~~~~~~~~~~~~~~~~*/

	si_Total[0]++;
	if(!BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtGraphicObject)) return;

	si_Total[1]++;
	BIG_Read(BIG_PosFile(ul_File) + 4, &ul_Type, 4);

	/* if ( ul_Type != GRO_2DSpriteList) */
	return;

	si_Total[2]++;
	sprintf(sz_Text, "%0.8X : %s", BIG_FileKey(ul_File), BIG_NameFile(ul_File));
	LINK_PrintStatusMsg(sz_Text);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_ParseDir(ULONG ul_Dir)
{
	/*~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	/*~~~~~~~~~~~~~*/

	/* sub dir */
	ul_Index = BIG_SubDir(ul_Dir);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		WOR_ParseDir(ul_Index);
		ul_Index = BIG_NextDir(ul_Index);
	}

	/* file */
	ul_Index = BIG_FirstFile(ul_Dir);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		WOR_CheckFile(ul_Index);
		ul_Index = BIG_NextFile(ul_Index);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_ParseBigfile(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Text[256];
	/*~~~~~~~~~~~~~~~~~*/

	si_Total[0] = 0;
	si_Total[1] = 0;
	si_Total[2] = 0;

	WOR_ParseDir(BIG_Root());

	sprintf(sz_Text, "Files : %d / %d / %d", si_Total[0], si_Total[1], si_Total[2]);
	LINK_PrintStatusMsg(sz_Text);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Check_Out(char b, char i, ULONG color)
{
	/*~~~~~~~~~*/
	char	*msg;
	/*~~~~~~~~~*/

	msg = sgsz_Message + 4;

	if(b)
	{
		*(--msg) = 'b';
		*(--msg) = '%';
	}

	if(i)
	{
		*(--msg) = 'i';
		*(--msg) = '%';
	}

	if(color) LINK_gul_ColorTxt = color;

	LINK_PrintStatusMsg(msg);
	LINK_gul_ColorTxt = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Check_BuildGameObjectDesc(OBJ_tdst_GameObject *_pst_Gao, char *msg)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ul_Value;
	ULONG	ul_Value2;
	/*~~~~~~~~~~~~~~*/

	ul_Value = _pst_Gao->ul_IdentityFlags;
	ul_Value2 = 0x80000000;
	while(ul_Value2)
	{
		*msg++ = (ul_Value & ul_Value2) ? 'x' : '-';
		ul_Value2 >>= 1;
	}

	*msg++ = ' ';

	ul_Value = _pst_Gao->ul_StatusAndControlFlags;
	ul_Value2 = 0x00000004;
	while(ul_Value2)
	{
		*msg++ = (ul_Value & ul_Value2) ? 'x' : '-';
		ul_Value2 >>= 1;
	}

	*msg++ = ' ';

	ul_Value2 = 0x01000000;
	while(ul_Value2 != 0x8000)
	{
		*msg++ = (ul_Value & ul_Value2) ? 'x' : '-';
		ul_Value2 >>= 1;
	}

	*msg++ = ' ';

	L_strcpy(msg, _pst_Gao->sz_Name);
	L_strcat(msg, "\n");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Check_EOT(TAB_tdst_PFtable *_pst_EOT, ULONG _ul_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_Gao;
	LONG				l_Index;
	ULONG				ul_Color;
	char				*msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	msg = sgsz_Message + 4;

	sprintf(msg, "Number %d", TAB_ul_PFtable_GetNbElems(_pst_EOT));
	WOR_Check_Out(1, 0, 0);
	sprintf(msg, "Hole : %d", TAB_ul_PFtable_GetNbHoles(_pst_EOT));
	WOR_Check_Out(1, 0, 0);

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(_pst_EOT);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(_pst_EOT);
	for(l_Index = 0; pst_PFElem <= pst_PFLastElem; pst_PFElem++, l_Index++)
	{
		pst_Gao = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_Gao)) continue;

		sprintf(msg, ".%3d %s", l_Index, pst_Gao->sz_Name);
		ul_Color = _ul_Flag * (OBJ_b_TestIdentityFlag(pst_Gao, _ul_Flag) ? 0 : 0xFF);
		WOR_Check_Out(0, 0, ul_Color);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Check(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_Ptable		*Ptable;
	TAB_tdst_PFtable	*PFtable;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	GRO_tdst_Struct		*pst_Gro2, *pst_Gro, **ppst_Gro, **ppst_LastGro;
	OBJ_tdst_GameObject *pst_Gao;
	char				*msg, *pmsg;
	LONG				l_Index, i;
	ULONG				ul_Value;
	WOR_tdst_View		*pst_View;
	void				*p_Last;
	ULONG				ul_NbObjects, ul_NbMaterials;
	LONG				*dl_ObjRef, *dl_MatRef, *dl_AnimRef, *dl_AnimMatRef;
	MAT_tdst_Multi		*pst_MM;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	msg = sgsz_Message + 4;

	/* Init data */
	Ptable = &_pst_World->st_GraphicObjectsTable;
	ul_NbObjects = Ptable->ul_NbElems + Ptable->ul_NbHoles;
	dl_ObjRef = (LONG *) L_malloc(ul_NbObjects * 4);
	dl_AnimRef = (LONG *) L_malloc(ul_NbObjects * 4);
	L_memset(dl_ObjRef, 0, ul_NbObjects * 4);
	L_memset(dl_AnimRef, 0, ul_NbObjects * 4);

	Ptable = &_pst_World->st_GraphicMaterialsTable;
	ul_NbMaterials = Ptable->ul_NbElems + Ptable->ul_NbHoles;
	dl_MatRef = (LONG *) L_malloc(ul_NbMaterials * 4);
	dl_AnimMatRef = (LONG *) L_malloc(ul_NbMaterials * 4);
	L_memset(dl_MatRef, 0, ul_NbMaterials * 4);
	L_memset(dl_AnimMatRef, 0, ul_NbMaterials * 4);

	sprintf(msg, "--< Check world \n");
	WOR_Check_Out(1, 0, 0x00800080);
	sprintf(msg, "%s \n", _pst_World->sz_Name);
	WOR_Check_Out(1, 1, 0x00800080);
	sprintf(msg, ">--", _pst_World->sz_Name);
	WOR_Check_Out(1, 0, 0x00800080);

	sprintf(msg, "--< Game object table >--");
	WOR_Check_Out(0, 0, 0x00800080);
	PFtable = &_pst_World->st_AllWorldObjects;
	sprintf(msg, "Number %d (%d)", TAB_ul_PFtable_GetNbElems(PFtable), _pst_World->ul_NbTotalGameObjects);
	WOR_Check_Out(1, 0, 0);
	sprintf(msg, "Hole : %d", TAB_ul_PFtable_GetNbHoles(PFtable));
	WOR_Check_Out(1, 0, 0);
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(PFtable);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(PFtable);
	for(l_Index = 0; pst_PFElem <= pst_PFLastElem; pst_PFElem++, l_Index++)
	{
		pst_Gao = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_Gao)) continue;

		pmsg = msg + sprintf(msg, ".%3d ", l_Index);
		WOR_Check_BuildGameObjectDesc(pst_Gao, pmsg);
		WOR_Check_Out(0, 0, 0);

		/* Check existence of reference object and add ref */
		if(pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)
		{
			pst_Gro = pst_Gao->pst_Extended->pst_Light;
			if(pst_Gro)
			{
				ul_Value = TAB_ul_Ptable_GetElemIndexWithPointer(&_pst_World->st_GraphicObjectsTable, pst_Gro);
				if(ul_Value == TAB_Cul_BadIndex)
				{
					sprintf(msg, "light not in graphic object list (%s)\n", GRO_sz_Struct_GetName(pst_Gro));
					WOR_Check_Out(1, 0, 0xFF0000);
				}
				else
					dl_ObjRef[ul_Value]++;
			}
			else
			{
				sprintf(msg, "light without light\n");
				WOR_Check_Out(1, 0, 0xFF0000);
			}
		}

		if(pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
		{
			pst_Gro = pst_Gao->pst_Base->pst_Visu->pst_Object;
			if(pst_Gro)
			{
				ul_Value = TAB_ul_Ptable_GetElemIndexWithPointer(&_pst_World->st_GraphicObjectsTable, pst_Gro);
				if(ul_Value != TAB_Cul_BadIndex) dl_ObjRef[ul_Value]++;

				/* LOD */
				if(pst_Gro->i->ul_Type == GRO_GeoStaticLOD)
				{
					for(i = 0; i < ((GEO_tdst_StaticLOD *) pst_Gro)->uc_NbLOD; i++)
					{
						pst_Gro2 = ((GEO_tdst_StaticLOD *) pst_Gro)->dpst_Id[i];
						if(pst_Gro2)
						{
							ul_Value = TAB_ul_Ptable_GetElemIndexWithPointer
								(
									&_pst_World->st_GraphicObjectsTable,
									pst_Gro2
								);
							if(ul_Value != TAB_Cul_BadIndex) dl_ObjRef[ul_Value]++;
						}
					}
				}
			}
			else
			{
				sprintf(msg, "visuel without object\n");
				WOR_Check_Out(1, 0, 0xFF0000);
			}

			pst_Gro = pst_Gao->pst_Base->pst_Visu->pst_Material;
			if(pst_Gro)
			{
				ul_Value = TAB_ul_Ptable_GetElemIndexWithPointer(&_pst_World->st_GraphicMaterialsTable, pst_Gro);
				if(ul_Value != TAB_Cul_BadIndex) dl_MatRef[ul_Value]++;
			}
		}

		strcpy(msg, "");
		WOR_Check_Out(0, 0, 0);
	}

	sprintf(msg, "--< Anims EOT >--");
	WOR_Check_Out(0, 0, 0x00800080);
	WOR_Check_EOT(&_pst_World->st_EOT.st_Anims, OBJ_C_IdentityFlag_Anims);
	sprintf(msg, "--< Dyna EOT >--");
	WOR_Check_Out(0, 0, 0x00800080);
	WOR_Check_EOT(&_pst_World->st_EOT.st_Dyna, OBJ_C_IdentityFlag_Dyna);
	sprintf(msg, "--< AI EOT >--");
	WOR_Check_Out(0, 0, 0x00800080);
	WOR_Check_EOT(&_pst_World->st_EOT.st_AI, OBJ_C_IdentityFlag_AI);
	sprintf(msg, "--< ColMap EOT >--");
	WOR_Check_Out(0, 0, 0x00800080);
	WOR_Check_EOT(&_pst_World->st_EOT.st_ColMap, OBJ_C_IdentityFlag_ColMap);
	sprintf(msg, "--< ZDM EOT >--");
	WOR_Check_Out(0, 0, 0x00800080);
	WOR_Check_EOT(&_pst_World->st_EOT.st_ZDM, OBJ_C_IdentityFlag_ZDM);
	sprintf(msg, "--< ZDE EOT >--");
	WOR_Check_Out(0, 0, 0x00800080);
	WOR_Check_EOT(&_pst_World->st_EOT.st_ZDE, OBJ_C_IdentityFlag_ZDE);
	sprintf(msg, "--< Events EOT >--");
	WOR_Check_Out(0, 0, 0x00800080);
	WOR_Check_EOT(&_pst_World->st_EOT.st_Events, OBJ_C_IdentityFlag_Events);
	sprintf(msg, "--< Hierarchy EOT >--");
	WOR_Check_Out(0, 0, 0x00800080);
	WOR_Check_EOT(&_pst_World->st_EOT.st_Hierarchy, OBJ_C_IdentityFlag_Hierarchy);

	sprintf(msg, "--< Activ object table >--");
	WOR_Check_Out(0, 0, 0x00800080);
	WOR_Check_EOT(&_pst_World->st_ActivObjects, 0);

	sprintf(msg, "--< Views >--");
	WOR_Check_Out(0, 0, 0x00800080);
	sprintf(msg, "Number : %d ", _pst_World->ul_NbViews);
	WOR_Check_Out(1, 0, 0);

	pst_View = _pst_World->pst_View;
	p_Last = (void *) (pst_View + _pst_World->ul_NbViews);
	for(l_Index = 0; (void *) pst_View < p_Last; pst_View++, l_Index++)
	{
		if(pst_View->pfnv_ViewPointModificator == NULL) continue;
		sprintf(msg, "--< View %d >--", l_Index);
		WOR_Check_Out(0, 0, 0x00800080);
	}

	sprintf(msg, "--< Graphic object table >--");
	WOR_Check_Out(0, 0, 0x00800080);
	Ptable = &_pst_World->st_GraphicObjectsTable;
	sprintf(msg, "Number : %d", TAB_ul_Ptable_GetNbElems(Ptable));
	WOR_Check_Out(1, 0, 0);
	sprintf(msg, "Hole   : %d", TAB_ul_Ptable_GetNbHoles(Ptable));
	WOR_Check_Out(1, 0, 0);

	sprintf(msg, "Num -> Ref (WorldRef/AnimRef) Name Key Fat Type");
	WOR_Check_Out(0, 1, 0);

	ppst_Gro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(Ptable);
	ppst_LastGro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(Ptable);

	for(l_Index = 0; ppst_Gro <= ppst_LastGro; ppst_Gro++, l_Index++)
	{
		if(TAB_b_IsAHole(*ppst_Gro)) continue;

		ul_Value = LOA_ul_SearchKeyWithAddress((ULONG) * ppst_Gro);
		if(ul_Value != BIG_C_InvalidKey) ul_Value = BIG_ul_SearchKeyToFat(ul_Value);

		pmsg = msg +
			sprintf
			(
				msg,
				".%3d -> %3d (%3d/%3d) %s ",
				l_Index,
				(*ppst_Gro)->l_Ref,
				dl_ObjRef[l_Index],
				dl_AnimRef[l_Index],
				GRO_sz_Struct_GetName(*ppst_Gro)
			);
		if(ul_Value != BIG_C_InvalidKey)
		{
			sprintf
			(
				pmsg,
				"0x%08X %d %s",
				BIG_FileKey(ul_Value),
				ul_Value,
				GRO_gasz_InterfaceName[(*ppst_Gro)->i->ul_Type]
			);
			WOR_Check_Out(0, 0, 0);
		}
		else
		{
			sprintf(pmsg, "0x%08X", ul_Value);
			WOR_Check_Out(0, 0, 0x000000FF);
		}
	}

	sprintf(msg, "--< Graphic material table >--");
	WOR_Check_Out(0, 0, 0x00800080);
	Ptable = &_pst_World->st_GraphicMaterialsTable;
	sprintf(msg, "Number : %d", TAB_ul_Ptable_GetNbElems(Ptable));
	WOR_Check_Out(1, 0, 0);
	sprintf(msg, "Hole   : %d", TAB_ul_Ptable_GetNbHoles(Ptable));
	WOR_Check_Out(1, 0, 0);

	sprintf(msg, "Num -> Ref (WorldRef) Name Key Fat Type");
	WOR_Check_Out(0, 1, 0);

	ppst_Gro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(Ptable);
	ppst_LastGro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(Ptable);
	for(; ppst_Gro <= ppst_LastGro; ppst_Gro++)
	{
		if(TAB_b_IsAHole(*ppst_Gro)) continue;

		if((*ppst_Gro)->i->ul_Type == GRO_MaterialMulti)
		{
			pst_MM = (MAT_tdst_Multi *) (*ppst_Gro);
			for(l_Index = 0; l_Index < pst_MM->l_NumberOfSubMaterials; l_Index++)
			{
				ul_Value = TAB_ul_Ptable_GetElemIndexWithPointer(Ptable, pst_MM->dpst_SubMaterial[l_Index]);
				if(ul_Value == TAB_Cul_BadIndex)
				{
					sprintf(msg, "Multi Mat with bad material (%s %d)", GRO_sz_Struct_GetName(*ppst_Gro), l_Index);
					WOR_Check_Out(1, 0, 0xFF0000);
				}
				else
					dl_MatRef[ul_Value]++;
			}
		}
	}

	ppst_Gro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(Ptable);
	ppst_LastGro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(Ptable);

	for(l_Index = 0; ppst_Gro <= ppst_LastGro; ppst_Gro++, l_Index++)
	{
		if(TAB_b_IsAHole(*ppst_Gro)) continue;

		ul_Value = LOA_ul_SearchKeyWithAddress((ULONG) * ppst_Gro);
		if(ul_Value != BIG_C_InvalidKey) ul_Value = BIG_ul_SearchKeyToFat(ul_Value);

		pmsg = msg +
			sprintf
			(
				msg,
				".%3d -> %3d (%3d/%3d) %s ",
				l_Index,
				(*ppst_Gro)->l_Ref,
				dl_MatRef[l_Index],
				dl_AnimMatRef[l_Index],
				GRO_sz_Struct_GetName(*ppst_Gro)
			);

		if((*ppst_Gro)->l_Ref != dl_MatRef[l_Index] + 1)
		{
			pmsg += sprintf(pmsg, " [Bad Ref] ");
		}

		if(ul_Value != BIG_C_InvalidKey)
		{
			sprintf
			(
				pmsg,
				"0x%08X %d %s",
				BIG_FileKey(ul_Value),
				ul_Value,
				GRO_gasz_InterfaceName[(*ppst_Gro)->i->ul_Type]
			);
			WOR_Check_Out(0, 0, 0);
		}
		else
		{
			sprintf(pmsg, "0x%08X", ul_Value);
			WOR_Check_Out(0, 0, 0x000000FF);
		}
	}

	L_free(dl_ObjRef);
	L_free(dl_AnimRef);
	L_free(dl_MatRef);
	L_free(dl_AnimMatRef);

	/* WOR_ParseBigfile(); */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Check_InfoPhoto(WOR_tdst_World *_pst_World, int _typeout)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	FILE					*hp_File;
	int						i_Res, i_MissionId, i_MissionType;
	LONG					i, j, l_Index, desanimal, desreport, descode, descarte, modanimal, modreport, modcode, modcarte;
	TAB_tdst_PFtable		*PFtable;
	TAB_tdst_PFelem			*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject		*pst_Gao;
	char					*cur, sz_Text[2048];
	MDF_tdst_Modifier		*pst_Modifier;
	GAO_tdst_ModifierPhoto	*pst_Mod;
	MATH_tdst_Vector		*T;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_World) return;

	PFtable = &_pst_World->st_AllWorldObjects;
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(PFtable);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(PFtable);
	for(l_Index = 0, i_Res = 0; pst_PFElem <= pst_PFLastElem; pst_PFElem++, l_Index++)
	{
		pst_Gao = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_Gao)) continue;
		if(!pst_Gao || !pst_Gao->pst_Extended) continue;
		if((pst_Gao->pst_Extended->pst_Design) || (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_DesignStruct))
		{
			if(pst_Gao->pst_Extended->pst_Design->i3 & 0xFF000000)
			{
				i_Res++;
				continue;
			}
		}

		pst_Modifier = GAO_ModifierPhoto_Get(pst_Gao, TRUE);
		if(pst_Modifier) i_Res++;
	}

	if(_typeout == 0)
	{
		sprintf(sz_Text, "====< %d GAO with info photo found >=====", i_Res);
		LINK_PrintStatusMsg(sz_Text);
	}
	else
	{
		hp_File = fopen("x:\\infophoto.txt", "at");
		if(!hp_File) return;
		sprintf(sz_Text, "%s [%d]\n", _pst_World->sz_Name, i_Res);
		fprintf(hp_File, sz_Text);
	}

	if(i_Res == 0) return;

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(PFtable);
	for(l_Index = 0, i_Res = 0; pst_PFElem <= pst_PFLastElem; pst_PFElem++, l_Index++)
	{
		pst_Gao = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_Gao)) continue;
		if(!pst_Gao || !pst_Gao->pst_Extended) continue;
		i_MissionId = 0;
		if((pst_Gao->pst_Extended->pst_Design) || (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_DesignStruct))
		{
			if(pst_Gao->pst_Extended->pst_Design->i3 & 0xFF000000)
			{
				i_MissionId = pst_Gao->pst_Extended->pst_Design->i3 >> 24;
				i_MissionType = pst_Gao->pst_Extended->pst_Design->i3 & 0xFF;
			}
		}

		pst_Modifier = GAO_ModifierPhoto_Get(pst_Gao, TRUE);
		if(i_MissionId || pst_Modifier)
		{
			if(_typeout == 0)
			{
				sprintf(sz_Text, "[%08X] (%.2f, %.2f, %.2f) %s", LOA_ul_SearchKeyWithAddress((ULONG) pst_Gao), pst_Gao->pst_GlobalMatrix->T.x,pst_Gao->pst_GlobalMatrix->T.y, pst_Gao->pst_GlobalMatrix->T.z, pst_Gao->sz_Name );
				LINK_PrintStatusMsg(sz_Text);
				if(i_MissionId)
				{
					switch(i_MissionId)
					{
					case 4:
						sprintf(sz_Text, ". [des] animals (%d - %d)", i_MissionId, i_MissionType);
						break;
					case 100:
						sprintf(sz_Text, ". [des] carte   (%d - %d)", i_MissionId, i_MissionType);
						break;
					case 101:
						sprintf(sz_Text, ". [des] code    (%d - %d)", i_MissionId, i_MissionType);
						break;
					default:
						sprintf(sz_Text, ". [des] report  (%d - %d)", i_MissionId, i_MissionType);
						break;
					}

					LINK_PrintStatusMsg(sz_Text);
				}

				if(pst_Modifier)
				{
					pst_Mod = (GAO_tdst_ModifierPhoto *) pst_Modifier->p_Data;
					switch(pst_Mod->i_Mission)
					{
					case 4:
						sprintf(sz_Text, ". [mod] animals (%d - %d)", pst_Mod->i_Mission, pst_Mod->i_Info);
						break;
					case 100:
						sprintf(sz_Text, ". [mod] carte   (%d - %d)", pst_Mod->i_Mission, pst_Mod->i_Info);
						break;
					case 101:
						sprintf(sz_Text, ". [mod] code    (%d - %d)", pst_Mod->i_Mission, pst_Mod->i_Info);
						break;
					default:
						sprintf(sz_Text, ". [mod] report  (%d - %d)", pst_Mod->i_Mission, pst_Mod->i_Info);
						break;
					}

					LINK_PrintStatusMsg(sz_Text);
				}
			}
			else
			{
				desanimal = desreport = descode = descarte = -1;
				modanimal = modreport = modcode = modcarte = -1;
				j = 0;
				if(i_MissionId)
				{
					if(i_MissionId == 101)
						descode = i_MissionType;
					else if(i_MissionId == 100)
						descarte = i_MissionType;
					else if(i_MissionId == 4)
						desanimal = i_MissionType;
					else
						desreport = i_MissionType;
					j++;
				}

				if(pst_Modifier)
				{
					pst_Mod = (GAO_tdst_ModifierPhoto *) pst_Modifier->p_Data;
					if(pst_Mod->i_Mission == 101)
						modcode = pst_Mod->i_Info;
					else if(pst_Mod->i_Mission == 100)
						modcarte = pst_Mod->i_Info;
					else if(pst_Mod->i_Mission == 4)
						modanimal = pst_Mod->i_Info;
					else
						modreport = pst_Mod->i_Info;
					j++;
				}

				cur = sz_Text;
				for(i = 0; i < 80; i++) *cur++ = ' ';
				if((desanimal != -1) || (modanimal != -1))
				{
					// name
					strcpy(cur, pst_Gao->sz_Name);
					i = strlen(pst_Gao->sz_Name);
					while(i < 60) cur[i++] = ' ';
					cur += 60;
					// coord
					T = &pst_Gao->pst_GlobalMatrix->T;
					sprintf( cur, "(%.2f, %.2f, %.2f)", T->x, T->y, T->z );
					i = strlen( cur );
					while(i < 30) cur[i++] = ' ';
					cur += 30;
					// data
					cur += sprintf(cur, (desanimal != -1) ? "%5d" : "     ", desanimal);
					cur += sprintf(cur, (modanimal != -1) ? "%5d" : "     ", modanimal);
					j -= ((desanimal != -1) ? 1 : 0) + ((modanimal != -1) ? 1 : 0);
				}
				else
				{
					for(i = 0; i < 100; i++) *cur++ = ' ';
				}

				if(j > 0)
				{
					if((desreport != -1) || (modreport != -1))
					{
						// name
						strcpy(cur, pst_Gao->sz_Name);
						i = strlen(pst_Gao->sz_Name);
						while(i < 60) cur[i++] = ' ';
						cur += 60;
						// data
						cur += sprintf(cur, (desreport != -1) ? "%5d" : "     ", desreport);
						cur += sprintf(cur, (modreport != -1) ? "%5d" : "     ", modreport);
						j -= ((desreport != -1) ? 1 : 0) + ((modreport != -1) ? 1 : 0);
					}
					else
					{
						for(i = 0; i < 70; i++) *cur++ = ' ';
					}
				}

				if(j > 0)
				{
					if((descode != -1) || (modcode != -1)) strcpy(cur, "[code] ");
					if((descarte != -1) || (modcarte != -1)) strcpy(cur, "[carte] ");
					strcat(cur, pst_Gao->sz_Name);
					i = strlen(cur);
					while(i < 60) cur[i++] = ' ';
					cur += 60;
					if(descode != -1)
						cur += sprintf(cur, "%5d", descode);
					else if(descarte != -1)
						cur += sprintf(cur, "%5d", descarte);
					else
						cur += sprintf(cur, "     ");

					if(modcode != -1)
						cur += sprintf(cur, "%5d", modcode);
					else if(modcarte != -1)
						cur += sprintf(cur, "%5d", modcarte);
					else
						cur += sprintf(cur, "     ");
				}

				*cur++ = '\n';
				*cur = 0;
				fprintf(hp_File, sz_Text);
			}
		}
	}

	if(_typeout != 0) fclose(hp_File);
}

#endif /* ACTIVE_EDITORS */
