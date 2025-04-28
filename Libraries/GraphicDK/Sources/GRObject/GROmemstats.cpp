/*$T GROmemstats.c GC! 1.081 10/02/01 09:59:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "TABles/TABles.h"
#include "LINks/LINKmsg.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOstaticlod.h"
#include "GEOmetric/GEO_STRIP.h"
#include "LIGHT/LIGHTstruct.h"
#include "MATerial/MATstruct.h"
#include "CAMera/CAMstruct.h"
#include "STRing/STRstruct.h"
#include "PArticleGenerator/PAGstruct.h"

typedef struct	GRO_tdst_MemStat_
{
	ULONG	ul_Geo_Nb;
    ULONG	ul_Geo_TotalMem;
	ULONG	ul_Geo_Mem;
	ULONG	ul_GeoPoints_Nb;
	ULONG	ul_GeoPoints_Mem;
	ULONG	ul_GeoNormals_Mem;
    ULONG	ul_GeoRLI_NbGeo;
	ULONG	ul_GeoRLI_Nb;
	ULONG	ul_GeoRLI_Mem;
	ULONG	ul_GeoUV_NbGeo;
    ULONG	ul_GeoUV_Nb;
    ULONG	ul_GeoUV_Mem;
	ULONG	ul_GeoBV_NbGeo;
	ULONG	ul_GeoMRM_Nb;
    ULONG   ul_GeoMRM_Mem;
	ULONG	ul_GeoSkin_Nb;
    ULONG	ul_GeoSkin_Mem;
    ULONG	ul_GeoStrip_Nb;
	ULONG	ul_GeoElem_Nb;
    ULONG	ul_GeoElem_Mem;
    ULONG   ul_GeoElem_TotalMem;
    ULONG   ul_GeoElem_Zero;
    ULONG   ul_GeoElemUsedIndex_Nb;
    ULONG   ul_GeoElemUsedIndex_Mem;
    ULONG   ul_GeoElemMRM_Nb;
    ULONG   ul_GeoElemMRM_Mem;
    ULONG   ul_GeoElemStrip_Nb;
    ULONG   ul_GeoElemStrip_Mem;
    ULONG   ul_GeoElemTri_Nb;
    ULONG   ul_GeoElemTri_Mem;

    ULONG   ul_Light_Nb;
    ULONG   ul_Light_Active;
    ULONG   ul_Light_Specular;
    ULONG   ul_Light_Shadows;
    ULONG   ul_Light_RTD;
    ULONG   ul_Light_RTND;
    ULONG   ul_Light_RLID;
    ULONG   ul_Light_RLIND;
    ULONG   ul_Light_Mem;
    ULONG   ul_Light_Omni;
    ULONG   ul_Light_Direct;
    ULONG   ul_Light_Spot;
    ULONG   ul_Light_Fog;
    ULONG   ul_Light_AddMaterial;

    ULONG   ul_Mat_Nb;
    ULONG   ul_Mat_Mem;
    ULONG   ul_MatSingle_Nb;
    ULONG   ul_MatSingle_Mem;
    ULONG   ul_MatMulti_Nb;
    ULONG   ul_MatMulti_Mem;
    ULONG   ul_MatMulti_NbSub;
    ULONG   ul_MatMTex_Nb;
    ULONG   ul_MatMTex_Mem;
    ULONG   ul_MatMTex_NbLevel;

    ULONG   ul_Gro_Nb;
    ULONG   ul_Gro_Mem;
    ULONG   ul_Camera_Nb;
    ULONG   ul_Camera_Mem;
    ULONG   ul_Waypoint_Nb;
    ULONG   ul_Waypoint_Mem;
    ULONG   ul_LOD_Nb;
    ULONG   ul_LOD_Mem;
    ULONG   ul_Unused_Nb;
    ULONG   ul_Unused_Mem;
    ULONG   ul_2DText_Nb;
    ULONG   ul_2DText_Mem;
    ULONG   ul_Particle_Nb;
    ULONG   ul_Particle_Mem;
} GRO_tdst_MemStat;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_MemStat_AddGeo(GRO_tdst_MemStat *Mem, GEO_tdst_Object *Geo)
{
	/*~~~~~~*/
	ULONG	i, j ,l;
	/*~~~~~~*/

	if(!Geo || (Geo->st_Id.i->ul_Type != GRO_Geometric)) return;

    /* struct */
	Mem->ul_Geo_Nb++;
	Mem->ul_Geo_Mem += sizeof(GEO_tdst_Object);
    Mem->ul_Geo_TotalMem += sizeof(GEO_tdst_Object);
	
    /* points & normals */
    Mem->ul_GeoPoints_Nb += Geo->l_NbPoints;
	l = Geo->l_NbPoints * sizeof(GEO_Vertex);
	Mem->ul_GeoPoints_Mem += l;
	Mem->ul_GeoNormals_Mem += l;
	Mem->ul_Geo_TotalMem += 2 * l;

    /* RLI */
	if(Geo->dul_PointColors)
	{
		Mem->ul_GeoRLI_NbGeo++;
        l = Geo->dul_PointColors[0];
        Mem->ul_GeoRLI_Nb += l;
        l = (l + 1) * 4;
        Mem->ul_GeoRLI_Mem += l;
        Mem->ul_Geo_TotalMem += l;
	}

    /* UV */
    if (Geo->dst_UV)
    {
        Mem->ul_GeoUV_NbGeo++;
        Mem->ul_GeoUV_Nb += Geo->l_NbUVs;
        l = Geo->l_NbUVs * sizeof( GEO_tdst_UV );
        Mem->ul_GeoUV_Mem += l;
        Mem->ul_Geo_TotalMem += l;
    }

    /* BV */
    if (Geo->pst_BV)
    {
        Mem->ul_GeoBV_NbGeo++;
    }

    /* strip */
    if (Geo->ulStripFlag & GEO_C_Strip_DataValid)
    {
        Mem->ul_GeoStrip_Nb++;
    }

    /* MRM */
    if (Geo->p_MRM_ObjectAdditionalInfo)
    {
        Mem->ul_GeoMRM_Nb++;
        l = Geo->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints * 4 + sizeof( GEO_tdst_MRM_Object );
        Mem->ul_GeoMRM_Mem += l;
        Mem->ul_Geo_TotalMem += l;
    }

    /* skin */
    if (Geo->p_SKN_Objectponderation)
    {
        Mem->ul_GeoSkin_Nb++;
        l = sizeof( GEO_tdst_ObjectPonderation ) + Geo->p_SKN_Objectponderation->NumberPdrtLists * (4 + sizeof( GEO_tdst_VertexPonderationList ) );
        for (i = 0; i < Geo->p_SKN_Objectponderation->NumberPdrtLists; i++)
        {
            l += Geo->p_SKN_Objectponderation->pp_PdrtLst[i]->us_NumberOfPonderatedVertices * sizeof( GEO_tdst_CompressedVertexPonderation );
        }
        Mem->ul_GeoSkin_Mem += l;
        Mem->ul_Geo_TotalMem += l;
    }

    /* elements */
    if (Geo->l_NbElements)
    {
        Mem->ul_GeoElem_Nb += Geo->l_NbElements;
        l = Geo->l_NbElements * sizeof( GEO_tdst_ElementIndexedTriangles );
        Mem->ul_GeoElem_Mem += l;
        Mem->ul_GeoElem_TotalMem += l;
        Mem->ul_Geo_TotalMem += l;

        for (i = 0; i < (ULONG) Geo->l_NbElements; i++)
        {
            /* triangles */
            l = Geo->dst_Element[i].l_NbTriangles;
            if ( l )
            {
                Mem->ul_GeoElemTri_Nb += l;
                l *= sizeof( GEO_tdst_IndexedTriangle );
                Mem->ul_GeoElemTri_Mem += l;
                Mem->ul_GeoElem_TotalMem += l;
                Mem->ul_Geo_TotalMem += l;
            }
            else
                Mem->ul_GeoElem_Zero++;

            /* MRM */
            if ( Geo->dst_Element[i].p_MrmElementAdditionalInfo )
            {
                Mem->ul_GeoElemMRM_Nb++;
                l = sizeof( GEO_tdst_MRM_Element ) + Geo->dst_Element[i].p_MrmElementAdditionalInfo->ul_RealNumberOfTriangle * 2;
                Mem->ul_GeoElemMRM_Mem += l;
                Mem->ul_GeoElem_TotalMem += l;
                Mem->ul_Geo_TotalMem += l;
            }

            /* strip */
            if (Geo->dst_Element[i].pst_StripData)
            {
                Mem->ul_GeoElemStrip_Nb++;
                l = sizeof( GEO_tdst_StripData );
                l+= Geo->dst_Element[i].pst_StripData->ulStripNumber;
                for (j = 0; j < Geo->dst_Element[i].pst_StripData->ulStripNumber; j++)
                {
                    l += Geo->dst_Element[i].pst_StripData->pStripList[j].ulVertexNumber * (sizeof(ULONG) + sizeof( GEO_tdst_MinVertexData) );
                }

                Mem->ul_GeoElemStrip_Mem += l;
                Mem->ul_GeoElem_TotalMem += l;
                Mem->ul_Geo_TotalMem += l;
            }

            /* used index */
            if (Geo->dst_Element[i].pus_ListOfUsedIndex)
            {
                Mem->ul_GeoElemUsedIndex_Nb++;
                l = Geo->dst_Element[i].ul_NumberOfUsedIndex * sizeof( USHORT );
                Mem->ul_GeoElemUsedIndex_Mem+= l;
                Mem->ul_GeoElem_TotalMem += l;
                Mem->ul_Geo_TotalMem += l;
            }
        }
    }
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_MemStat_Geo(TAB_tdst_Ptable *_pst_Table, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct		**ppst_First, **ppst_Last;
	GRO_tdst_MemStat	*M, st_Stat;
    int                 nodisp;
    char                sz_Text[ 256 ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    M = (GRO_tdst_MemStat *) p;
    nodisp = (M != NULL);

    if (!M)
    {
        L_memset( &st_Stat, 0, sizeof(GRO_tdst_MemStat));
        M = &st_Stat;
    }

	ppst_First = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(_pst_Table);
	ppst_Last = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(_pst_Table);

	for(; ppst_First <= ppst_Last; ppst_First++)
	{
		if(TAB_b_IsAHole(*ppst_First)) continue;

		GRO_MemStat_AddGeo(M, (GEO_tdst_Object *) (*ppst_First));
	}

    if (nodisp)
        return;

    /*$off*/
    LINK_PrintStatusMsg( "<<< Statistiques mémoires - géométrie du monde >>>" );
    LINK_PrintStatusMsg( ".           nbgeo    nb       mem      total " );
    sprintf( sz_Text, "Géométrie   %4d          %8d  %8d"               , M->ul_Geo_Nb, M->ul_Geo_Mem, M->ul_Geo_TotalMem );                                  LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Point          %6d  %8d"                      , M->ul_GeoPoints_Nb, M->ul_GeoPoints_Mem );                                          LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Normal         %6d  %8d"                      , M->ul_GeoPoints_Nb, M->ul_GeoNormals_Mem );                                         LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  UV       %4d  %6d  %8d"                       , M->ul_GeoUV_NbGeo, M->ul_GeoUV_Nb, M->ul_GeoUV_Mem );                               LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  RLI      %4d  %6d  %8d"                       , M->ul_GeoRLI_NbGeo, M->ul_GeoRLI_Nb, M->ul_GeoRLI_Mem );                            LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  BV       %4d"                                 , M->ul_GeoBV_NbGeo );                                                                LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  MRM      %4d          %8d"                    , M->ul_GeoMRM_Nb, M->ul_GeoMRM_Mem );                                                LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Skin     %4d          %8d"                    , M->ul_GeoSkin_Nb, M->ul_GeoSkin_Mem );                                              LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Strip    %4d"                                 , M->ul_GeoStrip_Nb );                                                                LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Elements       %6d  %8d  %8d  [zero T : %d]"  , M->ul_GeoElem_Nb, M->ul_GeoElem_Mem, M->ul_GeoElem_TotalMem, M->ul_GeoElem_Zero );  LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".     triangles   %6d  %8d"                      , M->ul_GeoElemTri_Nb, M->ul_GeoElemTri_Mem );                                        LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".     used index  %6d  %8d"                      , M->ul_GeoElemUsedIndex_Nb, M->ul_GeoElemUsedIndex_Mem );                            LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".     MRM         %6d  %8d"                      , M->ul_GeoElemMRM_Nb, M->ul_GeoElemMRM_Mem );                                        LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".     Strip       %6d  %8d"                      , M->ul_GeoElemStrip_Nb, M->ul_GeoElemStrip_Mem );                                    LINK_PrintStatusMsg( sz_Text );
    /*$on*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_MemStat_Light(TAB_tdst_Ptable *_pst_Table, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct		**ppst_First, **ppst_Last;
	GRO_tdst_MemStat	*M, st_Stat;
    int                 nodisp;
    char                sz_Text[ 256 ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    M = (GRO_tdst_MemStat *) p;
    nodisp = (M != NULL);
    if (!M)
    {
        L_memset( &st_Stat, 0, sizeof(GRO_tdst_MemStat));
        M = &st_Stat;
    }

	ppst_First = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(_pst_Table);
	ppst_Last = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(_pst_Table);

	for(; ppst_First <= ppst_Last; ppst_First++)
	{
		if(TAB_b_IsAHole(*ppst_First)) continue;

        if ( *ppst_First == NULL ) continue;
        if ( (*ppst_First)->i->ul_Type != GRO_Light) continue;

        M->ul_Light_Nb++;
        M->ul_Light_Mem += sizeof( LIGHT_tdst_Light );

        if ( ((LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & LIGHT_Cul_LF_Active)
            M->ul_Light_Active++;
        if ( ((LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & LIGHT_Cul_LF_Specular)
            M->ul_Light_Specular++;
        if ( ((LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & LIGHT_Cul_LF_EmitRTShadows)
            M->ul_Light_Shadows++;
        if ( ((LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & LIGHT_Cul_LF_RealTimeOnDynam)
            M->ul_Light_RTD++;
        if ( ((LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & LIGHT_Cul_LF_RealTimeOnNonDynam)
            M->ul_Light_RTND++;
        if ( ((LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & LIGHT_Cul_LF_RLIOnDynam)
            M->ul_Light_RLID++;
        if ( ((LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & LIGHT_Cul_LF_RLIOnNonDynam)
            M->ul_Light_RLIND++;

        switch( ((LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & LIGHT_Cul_LF_Type)
        {
        case LIGHT_Cul_LF_Omni:
            M->ul_Light_Omni++;
            break;
        case LIGHT_Cul_LF_Direct:
            M->ul_Light_Direct++;
            break;
        case LIGHT_Cul_LF_Spot:
            M->ul_Light_Spot++;
            break;
        case LIGHT_Cul_LF_Fog:
            M->ul_Light_Fog++;
            break;
        case LIGHT_Cul_LF_AddMaterial:
            M->ul_Light_AddMaterial++;
            break;
        }
	}

    if (nodisp) return;

    /*$off*/
    LINK_PrintStatusMsg( "<<< Statistiques mémoires - lumières du monde >>>" );
    sprintf( sz_Text, "Light (nb/mem)    %4d  %8d" , M->ul_Light_Nb, M->ul_Light_Mem );     LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Omni           %4d"      , M->ul_Light_Omni );                   LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Direct         %4d"      , M->ul_Light_Direct);                  LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Spot           %4d"      , M->ul_Light_Spot );                   LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Fog            %4d"      , M->ul_Light_Fog );                    LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  AddMat         %4d"      , M->ul_Light_AddMaterial );            LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Active         %4d"      , M->ul_Light_Active );                 LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Specular       %4d"      , M->ul_Light_Specular );               LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Shadows        %4d"      , M->ul_Light_Shadows );                LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  RT dynam       %4d"      , M->ul_Light_RTD );                    LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  RT not dynam   %4d"      , M->ul_Light_RTND );                   LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  RLI dynam      %4d"      , M->ul_Light_RLID );                   LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  RLI not dynam  %4d"      , M->ul_Light_RLIND );                  LINK_PrintStatusMsg( sz_Text );
    /*$on*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_MemStat_AllGro(TAB_tdst_Ptable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct		**ppst_First, **ppst_Last;
	GRO_tdst_MemStat	M;
    char                sz_Text[ 256 ];
    ULONG               l;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&M, 0, sizeof(GRO_tdst_MemStat));

    GRO_MemStat_Geo( _pst_Table, &M );
    M.ul_Gro_Nb += M.ul_Geo_Nb;
    M.ul_Gro_Mem += M.ul_Geo_TotalMem;
    
    GRO_MemStat_Light( _pst_Table, &M );
    M.ul_Gro_Nb += M.ul_Light_Nb;
    M.ul_Gro_Mem += M.ul_Light_Mem;

	ppst_First = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(_pst_Table);
	ppst_Last = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(_pst_Table);

	for(; ppst_First <= ppst_Last; ppst_First++)
	{
		if(TAB_b_IsAHole(*ppst_First)) continue;

        if ( *ppst_First == NULL ) continue;
        
        if ( (*ppst_First)->i->ul_Type == GRO_Camera) 
        {
            l = sizeof( CAM_tdst_CameraObject );
            M.ul_Camera_Nb++;
            M.ul_Camera_Mem += l;
            M.ul_Gro_Nb ++;
            M.ul_Gro_Mem += l;
        }
        else if ( (*ppst_First)->i->ul_Type == GRO_Waypoint) 
        {
            l = sizeof( GRO_tdst_Struct );
            M.ul_Waypoint_Nb++;
            M.ul_Waypoint_Mem += l;
            M.ul_Gro_Nb ++;
            M.ul_Gro_Mem += l;
        }
        else if ( (*ppst_First)->i->ul_Type == GRO_GeoStaticLOD) 
        {
            l = sizeof( GEO_tdst_StaticLOD );
            l += ((GEO_tdst_StaticLOD *) (*ppst_First))->uc_NbLOD * 4;
            M.ul_LOD_Nb++;
            M.ul_LOD_Mem += l;
            M.ul_Gro_Nb ++;
            M.ul_Gro_Mem += l;
        }
        else if ( (*ppst_First)->i->ul_Type == GRO_Unused) 
        {
            l = sizeof( GRO_tdst_Struct );
            M.ul_Unused_Nb++;
            M.ul_Unused_Mem += l;
            M.ul_Gro_Nb ++;
            M.ul_Gro_Mem += l;
        }
        else if ( (*ppst_First)->i->ul_Type == GRO_2DText) 
        {
            l = sizeof( STR_tdst_Struct );
            l += ((STR_tdst_Struct *)(*ppst_First))->uc_NbMaxStrings * sizeof( STR_tdst_String );
            l += ((STR_tdst_Struct *)(*ppst_First))->ul_NbMaxLetters * sizeof( STR_tdst_Letter );
            M.ul_2DText_Nb++;
            M.ul_2DText_Mem += l;
            M.ul_Gro_Nb ++;
            M.ul_Gro_Mem += l;
        }
        else if ( (*ppst_First)->i->ul_Type == GRO_ParticleGenerator) 
        {
            l = sizeof( PAG_tdst_Struct );
            l += ((PAG_tdst_Struct *)(*ppst_First))->l_NbMaxP * sizeof( PAG_tdst_P );
            M.ul_Particle_Nb++;
            M.ul_Particle_Mem += l;
            M.ul_Gro_Nb ++;
            M.ul_Gro_Mem += l;
        }
	}

    /*$off*/
    LINK_PrintStatusMsg( "<<< Statistiques mémoires - objets graphiques du monde >>>" );
    sprintf( sz_Text, "Graphic object (nb/mem)  %4d  %8d" , M.ul_Gro_Nb, M.ul_Gro_Mem);             LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Geometrie             %4d  %8d" , M.ul_Geo_Nb, M.ul_Geo_TotalMem);        LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Light                 %4d  %8d" , M.ul_Light_Nb, M.ul_Light_Mem);         LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Camera                %4d  %8d" , M.ul_Camera_Nb, M.ul_Camera_Mem);       LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Waypoint              %4d  %8d" , M.ul_Waypoint_Nb, M.ul_Waypoint_Mem);   LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Static LOD            %4d  %8d" , M.ul_LOD_Nb, M.ul_LOD_Mem);             LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  (Unused)              %4d  %8d" , M.ul_Unused_Nb, M.ul_Unused_Mem);       LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  2D Text               %4d  %8d" , M.ul_2DText_Nb, M.ul_2DText_Mem);       LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Particle generator    %4d  %8d" , M.ul_Particle_Nb, M.ul_Particle_Mem);   LINK_PrintStatusMsg( sz_Text );
    
    /*$on*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_MemStat_AllMat(TAB_tdst_Ptable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct		**ppst_First, **ppst_Last;
	GRO_tdst_MemStat	M;
    char                sz_Text[ 256 ];
    ULONG               l;
    MAT_tdst_MTLevel    *pst_Level;
    float               f1, f2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&M, 0, sizeof(GRO_tdst_MemStat));

	ppst_First = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(_pst_Table);
	ppst_Last = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(_pst_Table);

	for(; ppst_First <= ppst_Last; ppst_First++)
	{
		if(TAB_b_IsAHole(*ppst_First)) continue;

        if ( *ppst_First == NULL ) continue;

        if ( (*ppst_First)->i->ul_Type == GRO_MaterialMulti)
        {
            l = sizeof( MAT_tdst_Multi );
            l+= ((MAT_tdst_Multi *) (*ppst_First))->l_NumberOfSubMaterials * 4;
            M.ul_Mat_Nb++;
            M.ul_Mat_Mem += l;
            M.ul_MatMulti_Nb++;
            M.ul_MatMulti_Mem += l;
            M.ul_MatMulti_NbSub += ((MAT_tdst_Multi *) (*ppst_First))->l_NumberOfSubMaterials;
        }
        else if ( (*ppst_First)->i->ul_Type == GRO_MaterialMultiTexture)
        {
            l = sizeof( MAT_tdst_MultiTexture );
            pst_Level = ((MAT_tdst_MultiTexture *) (*ppst_First))->pst_FirstLevel;
            while (pst_Level)
            {
                l+= sizeof( MAT_tdst_MTLevel );
                M.ul_MatMTex_NbLevel++;
                pst_Level = pst_Level->pst_NextLevel;
            }
            M.ul_Mat_Nb++;
            M.ul_Mat_Mem += l;
            M.ul_MatMTex_Nb++;
            M.ul_MatMTex_Mem += l;
        }
        else if ( (*ppst_First)->i->ul_Type == GRO_MaterialSingle)
        {
            l = sizeof( MAT_tdst_Single );
            M.ul_Mat_Nb++;
            M.ul_Mat_Mem += l;
            M.ul_MatSingle_Nb++;
            M.ul_MatSingle_Mem += l;
        }
	}

    if (M.ul_MatMulti_Nb)
        f1 = ((float) M.ul_MatMulti_NbSub) / ((float) M.ul_MatMulti_Nb);
    if (M.ul_MatMTex_Nb)
        f2 = ((float) M.ul_MatMTex_NbLevel) / ((float) M.ul_MatMTex_Nb);

    /*$off*/
    LINK_PrintStatusMsg( "<<< Statistiques mémoires - matériaux du monde >>>" );
    sprintf( sz_Text, "Materiaux (nb/mem)    %4d  %8d"                                      , M.ul_Mat_Nb, M.ul_Mat_Mem);                   LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Multi              %4d  %8d (nb moyen de sous matériaux : %.2f)"  , M.ul_MatMulti_Nb, M.ul_MatMulti_Mem, f1);     LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Single             %4d  %8d "                                     , M.ul_MatSingle_Nb, M.ul_MatSingle_Mem);       LINK_PrintStatusMsg( sz_Text );
    sprintf( sz_Text, ".  Multi texture      %4d  %8d (nb moyen de niveaus de tex : %.2f)"  , M.ul_MatMTex_Nb, M.ul_MatMTex_Mem, f2);       LINK_PrintStatusMsg( sz_Text );
    /*$on*/
}

#endif /* ACTIVE_EDITORS */

