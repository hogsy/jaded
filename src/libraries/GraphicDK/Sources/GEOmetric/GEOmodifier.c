/*$T GEOmodifier.c GC!1.71 02/04/00 14:53:07 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATHstruct.h"
#include "MATHs/MATHasm.h"

#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"

#include "GDInterface/GDInterface.h"
#include "GEOmetric/GEOmodifier.h"
#include "GEOmetric/GEOobject.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/*$4
 ***********************************************************************************************************************
    Modifier globals
 ***********************************************************************************************************************
 */

GEO_tdst_ModifierInterface  GEO_gast_ModifierInterface[GEO_C_Modifier_Number];

#ifdef ACTIVE_EDITORS
char                        *GEO_gasz_ModifierName[GEO_C_Modifier_Number] = { "Snap", "WaVe YoUr BoDy", "Shadow0", "LegLink" };
#endif /* ACTIVE_EDITORS */

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

/*$3
 =======================================================================================================================
    Snap
 =======================================================================================================================
 */

void GEO_ModifierSnap_Create(GEO_tdst_Modifier *_pst_Mod, void *p_Data)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierSnap   *pst_Data;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierSnap));
    if(!p_Data)
    {
        L_memset(_pst_Mod->p_Data, 0, sizeof(GEO_tdst_ModifierSnap));
        return;
    }

    L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GEO_tdst_ModifierSnap));
    pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;

    if(pst_Data->ul_NbPoints)
        pst_Data->pst_Point = (GEO_tdst_ModifierSnap_OnePoint*)MEM_p_Alloc(sizeof(GEO_tdst_ModifierSnap_OnePoint) * pst_Data->ul_NbPoints);
    else
        pst_Data->pst_Point = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_Destroy(GEO_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierSnap   *pst_Data;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;
    if(pst_Data->ul_NbPoints) MEM_Free(pst_Data->pst_Point);
    MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_Apply(GEO_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj, GDI_tdst_DisplayData *_pst_DD)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierSnap           *pst_Data;
    MATH_tdst_Matrix                st_Inv, *pst_Mat;
    GEO_tdst_ModifierSnap_OnePoint  *pst_Point, *pst_LastPoint;
    MATH_tdst_Vector                v;
    GEO_tdst_Object                 *pst_ObjDest;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;

    pst_Mat = OBJ_pst_GetAbsoluteMatrix(_pst_DD->pst_CurrentGameObject);
    MATH_InvertMatrix(&st_Inv, pst_Mat);
    pst_Mat = OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject *) pst_Data->p_GameObject);
    pst_ObjDest = (GEO_tdst_Object*)OBJ_p_GetGro((OBJ_tdst_GameObject *) pst_Data->p_GameObject);

    pst_Point = pst_Data->pst_Point;
    pst_LastPoint = pst_Point + pst_Data->ul_NbPoints;

    for(; pst_Point < pst_LastPoint; pst_Point++)
    {
        MATH_CopyVector(&pst_Point->st_Point, _pst_Obj->dst_Point + pst_Point->ul_IndexSrc);
        MATH_TransformVertex(&v, pst_Mat, pst_ObjDest->dst_Point + pst_Point->ul_IndexTgt);
        MATH_TransformVertex(_pst_Obj->dst_Point + pst_Point->ul_IndexSrc, &st_Inv, &v);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_Unapply(GEO_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj, GDI_tdst_DisplayData *_pst_DD)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierSnap           *pst_Data;
    GEO_tdst_ModifierSnap_OnePoint  *pst_Point, *pst_LastPoint;
    MATH_tdst_Vector                *pst_Vertex;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;
    pst_Point = pst_Data->pst_Point;
    pst_LastPoint = pst_Point + pst_Data->ul_NbPoints;
    pst_Vertex = _pst_Obj->dst_Point;

    for(; pst_Point < pst_LastPoint; pst_Point++)
        MATH_CopyVector(pst_Vertex + pst_Point->ul_IndexSrc, &pst_Point->st_Point);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_ModifierSnap_Load(GEO_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                            *pc_Cur;
    ULONG                           ul_Size, ul_Key;
    GEO_tdst_ModifierSnap           *pst_Data;
    GEO_tdst_ModifierSnap_OnePoint  *pst_Point, *pst_LastPoint;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;
    pc_Cur = _pc_Buffer;

    /* Size */
    ul_Size = *(ULONG *) pc_Cur;
    pc_Cur += 4;

    /* Game object */
    ul_Key = *(ULONG *) pc_Cur;
    pc_Cur += 4;
    if(ul_Key != BIG_C_InvalidIndex)
    {
        if((int) BIG_ul_SearchKeyToPos(ul_Key) != -1)
            LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Data->p_GameObject, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
        else
            pst_Data->p_GameObject = NULL;
    }

    /* Number of points */
    pst_Data->ul_NbPoints = *(ULONG *) pc_Cur;
    pc_Cur += 4;

    /* Alloc buffer for points */
    if(pst_Data->ul_NbPoints)
        pst_Data->pst_Point = (GEO_tdst_ModifierSnap_OnePoint*)MEM_p_Alloc(sizeof(GEO_tdst_ModifierSnap_OnePoint) * pst_Data->ul_NbPoints);
    else
        pst_Data->pst_Point = NULL;

    /* Points data */
    pst_Point = pst_Data->pst_Point;
    pst_LastPoint = pst_Point + pst_Data->ul_NbPoints;

    for(; pst_Point < pst_LastPoint; pst_Point++)
    {
        pst_Point->ul_IndexSrc = *(ULONG *) pc_Cur;
        pc_Cur += 4;
        pst_Point->ul_IndexTgt = *(ULONG *) pc_Cur;
        pc_Cur += 4;
    }

    /* Test size */
    if((ul_Size + 4) != (ULONG) (pc_Cur - _pc_Buffer))
    {
        ul_Size = 0;
    }

    return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_Save(GEO_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierSnap           *pst_Data;
    GEO_tdst_ModifierSnap_OnePoint  *pst_Point, *pst_LastPoint;
    ULONG                           ul_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;

    /* Save size */
    ul_Size = 8 + 8 * pst_Data->ul_NbPoints;
    SAV_Buffer(&ul_Size, 4);

    /* Save game object */
    ul_Size = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GameObject);
    SAV_Buffer(&ul_Size, 4);

    /* Save number of points */
    SAV_Buffer(&pst_Data->ul_NbPoints, 4);

    /* Save points */
    pst_Point = pst_Data->pst_Point;
    pst_LastPoint = pst_Point + pst_Data->ul_NbPoints;
    for(; pst_Point < pst_LastPoint; pst_Point++)
    {
        SAV_Buffer(&pst_Point->ul_IndexSrc, 4);
        SAV_Buffer(&pst_Point->ul_IndexTgt, 4);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_ComputeWithTresh
(
    OBJ_tdst_GameObject *_pst_ObjSrc,
    OBJ_tdst_GameObject *_pst_ObjTgt,
    float               _f_Tresh,
    GEO_tdst_Modifier   *_pst_Mod
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector        *pst_PtSrc, *pst_PtTgt;
    MATH_tdst_Vector        st_MinSrc, st_MaxSrc, st_MinTgt, st_MaxTgt;
    MATH_tdst_Vector        *A, *B, *C, v;
    GEO_tdst_Object         *pst_GeoSrc, *pst_GeoTgt;
    MATH_tdst_Matrix        *M;
    int                     i, j, i_TgtInSrc, i_SrcInTgt;
    ULONG                   *pul_SrcIndex, *pul_TgtIndex, *pul_SrcTgt, *pi, *pj;
    float                   f_Dist, f_DistMax;
    GEO_tdst_ModifierSnap   st_SnapData, *pst_SnapData;
    GEO_tdst_Modifier       *pst_NewMod;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Test data */
    if(!(_pst_ObjSrc->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu))
        return;
    pst_GeoSrc = (GEO_tdst_Object *) _pst_ObjSrc->pst_Base->pst_Visu->pst_Object;
    if(pst_GeoSrc == NULL) return;
    if(pst_GeoSrc->l_NbPoints == 0) return;
    if(!(_pst_ObjTgt->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu))
        return;
    pst_GeoTgt = (GEO_tdst_Object *) _pst_ObjTgt->pst_Base->pst_Visu->pst_Object;
    if(pst_GeoTgt == NULL) return;
    if(pst_GeoTgt->l_NbPoints == 0) return;

    /* Alloc temp data */
    pst_PtSrc = (MATH_tdst_Vector *) L_malloc(sizeof(MATH_tdst_Vector) * pst_GeoSrc->l_NbPoints);
    pst_PtTgt = (MATH_tdst_Vector *) L_malloc(sizeof(MATH_tdst_Vector) * pst_GeoTgt->l_NbPoints);

    /* Compute Src BV */
    A = pst_PtSrc;
    B = pst_PtSrc + pst_GeoSrc->l_NbPoints;
    C = pst_GeoSrc->dst_Point;
    M = OBJ_pst_GetAbsoluteMatrix(_pst_ObjSrc);
    MATH_InitVector(&st_MinSrc, Cf_Infinit, Cf_Infinit, Cf_Infinit);
    MATH_NegVector(&st_MaxSrc, &st_MinSrc);
    for(; A < B; A++, C++)
    {
        MATH_TransformVertex(A, M, C);

        if(A->x < st_MinSrc.x) st_MinSrc.x = A->x;
        if(A->x > st_MaxSrc.x) st_MaxSrc.x = A->x;
        if(A->y < st_MinSrc.y) st_MinSrc.y = A->y;
        if(A->y > st_MaxSrc.y) st_MaxSrc.y = A->y;
        if(A->z < st_MinSrc.z) st_MinSrc.z = A->z;
        if(A->z > st_MaxSrc.z) st_MaxSrc.z = A->z;
    }

    /* Include treshold in BV */
    MATH_InitVector(&v, _f_Tresh, _f_Tresh, _f_Tresh);
    MATH_AddEqualVector(&st_MaxSrc, &v);
    MATH_SubEqualVector(&st_MinSrc, &v);

    /* Compute Tgt BV */
    A = pst_PtTgt;
    B = pst_PtTgt + pst_GeoTgt->l_NbPoints;
    C = pst_GeoTgt->dst_Point;
    M = OBJ_pst_GetAbsoluteMatrix(_pst_ObjTgt);
    MATH_InitVector(&st_MinTgt, Cf_Infinit, Cf_Infinit, Cf_Infinit);
    MATH_NegVector(&st_MaxTgt, &st_MinTgt);
    i_TgtInSrc = 0;
    for(; A < B; A++, C++)
    {
        MATH_TransformVertex(A, M, C);

        if(A->x < st_MinTgt.x) st_MinTgt.x = A->x;
        if(A->x > st_MaxTgt.x) st_MaxTgt.x = A->x;
        if(A->y < st_MinTgt.y) st_MinTgt.y = A->y;
        if(A->y > st_MaxTgt.y) st_MaxTgt.y = A->y;
        if(A->z < st_MinTgt.z) st_MinTgt.z = A->z;
        if(A->z > st_MaxTgt.z) st_MaxTgt.z = A->z;

        if((A->x < st_MinSrc.x) || (A->x > st_MaxSrc.x))
            *(LONG *) A = -1;
        else if((A->y < st_MinSrc.y) || (A->y > st_MaxSrc.y))
            *(LONG *) A = -1;
        else if((A->z < st_MinSrc.z) || (A->z > st_MaxSrc.z))
            *(LONG *) A = -1;
        else
            i_TgtInSrc++;
    }

    /* Include treshold in BV */
    MATH_AddEqualVector(&st_MaxTgt, &v);
    MATH_SubEqualVector(&st_MinTgt, &v);

    /* Find all point of src in tgt BV */
    A = pst_PtSrc;
    B = pst_PtSrc + pst_GeoSrc->l_NbPoints;
    i_SrcInTgt = 0;
    for(; A < B; A++)
    {
        if((A->x < st_MinTgt.x) || (A->x > st_MaxTgt.x))
            *(LONG *) A = -1;
        else if((A->y < st_MinTgt.y) || (A->y > st_MaxTgt.y))
            *(LONG *) A = -1;
        else if((A->z < st_MinTgt.z) || (A->z > st_MaxTgt.z))
            *(LONG *) A = -1;
        else
            i_SrcInTgt++;
    }

    pul_SrcIndex = (ULONG *) L_malloc(4 * i_SrcInTgt);
    pul_SrcTgt = (ULONG *) L_malloc(4 * i_SrcInTgt);
    pul_TgtIndex = (ULONG *) L_malloc(4 * i_TgtInSrc);

    pi = pul_TgtIndex;
    for(i = 0; i < pst_GeoTgt->l_NbPoints; i++)
    {
        if(*(LONG *) (pst_PtTgt + i) != -1) *pi++ = i;
    }

    _f_Tresh *= _f_Tresh;
    pi = pul_SrcIndex;
    pj = pul_SrcTgt;
    for(i = 0; i < pst_GeoSrc->l_NbPoints; i++)
    {
        A = pst_PtSrc + i;
        if(*(LONG *) A == -1) continue;

        f_DistMax = _f_Tresh;
        *pj = 0xFFFFFFFF;
        for(j = 0; j < i_TgtInSrc; j++)
        {
            B = pst_PtTgt + pul_TgtIndex[j];
            MATH_SubVector(&v, A, B);
            f_Dist = MATH_f_SqrVector(&v);
            if(f_Dist <= f_DistMax)
            {
                *pj = pul_TgtIndex[j];
                f_DistMax = f_Dist;
            }
        }

        if(*pj != 0xFFFFFFFF)
        {
            *pi++ = i;
            pj++;
        }
        else
            i_SrcInTgt--;
    }

    if(i_SrcInTgt)
    {
        st_SnapData.p_GameObject = (void *) _pst_ObjTgt;
        st_SnapData.ul_NbPoints = i_SrcInTgt;
        pst_NewMod = GEO_pst_Modifier_Create(GEO_C_Modifier_Snap, &st_SnapData);
        (void *) pst_SnapData = pst_NewMod->p_Data;

        for(i = 0; i < i_SrcInTgt; i++)
        {
            pst_SnapData->pst_Point[i].ul_IndexSrc = pul_SrcIndex[i];
            pst_SnapData->pst_Point[i].ul_IndexTgt = pul_SrcTgt[i];
        }

        if(_pst_Mod)
        {
            GEO_Modifier_ReplaceInGameObject(_pst_ObjSrc, _pst_Mod, pst_NewMod);
            GEO_Modifier_Destroy(_pst_Mod);
        }
        else
            GEO_Modifier_AddToGameObject(_pst_ObjSrc, pst_NewMod);
    }
}

#endif ACTIVE_EDITORS

/*$3
 =======================================================================================================================
    Ondule Ton Corps
 =======================================================================================================================
 */

void GEO_ModifierOnduleTonCorps_Create(GEO_tdst_Modifier *_pst_Mod, void *p_Data)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierOnduleTonCorps *pst_Data;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierOnduleTonCorps));

    if(!p_Data)
    {
        L_memset(_pst_Mod->p_Data, 0, sizeof(GEO_tdst_ModifierOnduleTonCorps));
        return;
    }

    pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;
    L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GEO_tdst_ModifierOnduleTonCorps));
    pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;
    pst_Data->ul_Flags = 0;

    if(pst_Data->ul_NbPoints)
        pst_Data->pst_Point = (MATH_tdst_Vector*)MEM_p_Alloc(sizeof(MATH_tdst_Vector) * pst_Data->ul_NbPoints);
    else
        pst_Data->pst_Point = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierOnduleTonCorps_Destroy(GEO_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierOnduleTonCorps *pst_Data;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;
    if(pst_Data->ul_NbPoints) MEM_Free(pst_Data->pst_Point);
    MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierOnduleTonCorps_Apply
(
    GEO_tdst_Modifier       *_pst_Mod,
    GEO_tdst_Object         *_pst_Obj,
    GDI_tdst_DisplayData    *_pst_DD
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierOnduleTonCorps *pst_Data;
    MATH_tdst_Vector                *pst_Save, *pst_Point, *pst_Last;
    ULONG                           *pul_RLI;
    float                           f_Amplitude;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;

    pst_Point = _pst_Obj->dst_Point;
    pst_Last = pst_Point + ((_pst_Obj->l_NbPoints > (LONG) pst_Data->ul_NbPoints) ? pst_Data->ul_NbPoints : _pst_Obj->l_NbPoints);
    pst_Save = pst_Data->pst_Point;
    pul_RLI = _pst_DD->pst_CurrentGameObject->pst_Base->pst_Visu->dul_VertexColors;

    if ( ( pst_Data->ul_Flags & GEO_C_OTCF_UseAlphaOfRLI ) && pul_RLI )
    {
        if (pst_Data->ul_Flags & GEO_C_OTCF_InvertAlpha)
        {
            if(pst_Data->ul_Flags & GEO_C_OTCF_X)
            {
                if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
                        pst_Point->x += f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->x += f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
                else
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
                        pst_Point->y += f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->z += f_Amplitude * fOptCos((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
            }
            else if(pst_Data->ul_Flags & GEO_C_OTCF_Y)
            {
                if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
                        pst_Point->y += f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->y += f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
                else
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
                        pst_Point->x += f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->z += f_Amplitude * fOptCos((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
            }
            else if(pst_Data->ul_Flags & GEO_C_OTCF_Z)
            {
                if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
                        pst_Point->z += f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->z += f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
                else
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
                        pst_Point->x += f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->y += f_Amplitude * fOptCos((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
            }
            else
            {
                pst_Data->ul_Flags |= GEO_C_OTCF_NothingComputed;
            }
        }
        else
        {
            if(pst_Data->ul_Flags & GEO_C_OTCF_X)
            {
                if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
                        pst_Point->x += f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->x += f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
                else
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
                        pst_Point->y += f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->z += f_Amplitude * fOptCos((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
            }
            else if(pst_Data->ul_Flags & GEO_C_OTCF_Y)
            {
                if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
                        pst_Point->y += f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->y += f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
                else
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
                        pst_Point->x += f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->z += f_Amplitude * fOptCos((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
            }
            else if(pst_Data->ul_Flags & GEO_C_OTCF_Z)
            {
                if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
                        pst_Point->z += f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->z += f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
                else
                {
                    for(; pst_Point < pst_Last; pst_Point++, pst_Save++, pul_RLI++)
                    {
                        MATH_CopyVector(pst_Save, pst_Point);
                        f_Amplitude = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
                        pst_Point->x += f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                        pst_Point->y += f_Amplitude * fOptCos((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                    }
                }
            }
            else
            {
                pst_Data->ul_Flags |= GEO_C_OTCF_NothingComputed;
            }
        }
    }
    else
    {
        if(pst_Data->ul_Flags & GEO_C_OTCF_X)
        {
            if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
            {
                for(; pst_Point < pst_Last; pst_Point++, pst_Save++)
                {
                    MATH_CopyVector(pst_Save, pst_Point);
                    pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                    pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                }
            }
            else
            {
                for(; pst_Point < pst_Last; pst_Point++, pst_Save++)
                {
                    MATH_CopyVector(pst_Save, pst_Point);
                    pst_Point->y += pst_Data->f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                    pst_Point->z += pst_Data->f_Amplitude * fOptCos((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                }
            }
        }
        else if(pst_Data->ul_Flags & GEO_C_OTCF_Y)
        {
            if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
            {
                for(; pst_Point < pst_Last; pst_Point++, pst_Save++)
                {
                    MATH_CopyVector(pst_Save, pst_Point);
                    pst_Point->y += pst_Data->f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                    pst_Point->y += pst_Data->f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                }
            }
            else
            {
                for(; pst_Point < pst_Last; pst_Point++, pst_Save++)
                {
                    MATH_CopyVector(pst_Save, pst_Point);
                    pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                    pst_Point->z += pst_Data->f_Amplitude * fOptCos((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                }
            }
        }
        else if(pst_Data->ul_Flags & GEO_C_OTCF_Z)
        {
            if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
            {
                for(; pst_Point < pst_Last; pst_Point++, pst_Save++)
                {
                    MATH_CopyVector(pst_Save, pst_Point);
                    pst_Point->z += pst_Data->f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
                    pst_Point->z += pst_Data->f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
                }
            }
            else
            {
                for(; pst_Point < pst_Last; pst_Point++, pst_Save++)
                {
                    MATH_CopyVector(pst_Save, pst_Point);
                    pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                    pst_Point->y += pst_Data->f_Amplitude * fOptCos((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
                }
            }
        }
        else
        {
            pst_Data->ul_Flags |= GEO_C_OTCF_NothingComputed;
        }
    }

    pst_Data->f_Angle += pst_Data->f_Delta;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierOnduleTonCorps_Unapply
(
    GEO_tdst_Modifier       *_pst_Mod,
    GEO_tdst_Object         *_pst_Obj,
    GDI_tdst_DisplayData    *_pst_DD
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierOnduleTonCorps *pst_Data;
    MATH_tdst_Vector                *pst_Save, *pst_Point, *pst_Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;

    if(pst_Data->ul_Flags & GEO_C_OTCF_NothingComputed)
    {
        pst_Data->ul_Flags &= ~GEO_C_OTCF_NothingComputed;
        return;
    }

    pst_Point = _pst_Obj->dst_Point;
    pst_Last = pst_Point + ((_pst_Obj->l_NbPoints > (LONG) pst_Data->ul_NbPoints) ? pst_Data->ul_NbPoints : _pst_Obj->l_NbPoints);
    pst_Save = pst_Data->pst_Point;

    for(; pst_Point < pst_Last; pst_Point++, pst_Save++)
        MATH_CopyVector(pst_Point, pst_Save);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_ModifierOnduleTonCorps_Load(GEO_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                            *pc_Cur;
    GEO_tdst_ModifierOnduleTonCorps *pst_Data;
    ULONG                           ul_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;
    pc_Cur = _pc_Buffer;

    /* Size */
    ul_Size = *(ULONG *) pc_Cur;
    pc_Cur += 4;

    pst_Data->ul_Flags = *(ULONG *) pc_Cur;
    pc_Cur += 4;
    pst_Data->f_Angle = *(float *) pc_Cur;
    pc_Cur += 4;
    pst_Data->f_Amplitude = *(float *) pc_Cur;
    pc_Cur += 4;
    pst_Data->f_Factor = *(float *) pc_Cur;
    pc_Cur += 4;
    pst_Data->f_Delta = *(float *) pc_Cur;
    pc_Cur += 4;
    pst_Data->ul_NbPoints = *(ULONG *) pc_Cur;
    pc_Cur += 4;

    if(pst_Data->ul_NbPoints)
        pst_Data->pst_Point = (MATH_tdst_Vector*)MEM_p_Alloc(sizeof(MATH_tdst_Vector) * pst_Data->ul_NbPoints);
    else
        pst_Data->pst_Point = NULL;

    /* Test size */
    if((ul_Size + 4) != (ULONG) (pc_Cur - _pc_Buffer))
    {
        ul_Size = 0;
    }

    return(pc_Cur - _pc_Buffer);
}


#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierOnduleTonCorps_Save(GEO_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierOnduleTonCorps *pst_Data;
    ULONG                           ul_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;

    ul_Size = 24;
    SAV_Buffer(&ul_Size, 4);

    SAV_Buffer(&pst_Data->ul_Flags, 4);
    SAV_Buffer(&pst_Data->f_Angle, 4);
    SAV_Buffer(&pst_Data->f_Amplitude, 4);
    SAV_Buffer(&pst_Data->f_Factor, 4);
    SAV_Buffer(&pst_Data->f_Delta, 4);
    SAV_Buffer(&pst_Data->ul_NbPoints, 4);
}

#endif

/*$3
 =======================================================================================================================
    Shadow 0
 =======================================================================================================================
 */

void GEO_ModifierShadow0_Create(GEO_tdst_Modifier *_pst_Mod, void *p_Data)
{
    _pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierShadow0));

    if(!p_Data)
        L_memset(_pst_Mod->p_Data, 0, sizeof(GEO_tdst_ModifierOnduleTonCorps));
    else
        L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GEO_tdst_ModifierOnduleTonCorps));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierShadow0_Destroy(GEO_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierShadow0   *pst_Data;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierShadow0 *) _pst_Mod->p_Data;
    MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierShadow0_Apply(GEO_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj, GDI_tdst_DisplayData *_pst_DD)
{
    WOR_tdst_World *pst_World;
    OBJ_tdst_GameObject *pst_GO;
    MATH_tdst_Vector st_Pos, st_Dir, *N, X, Y, C;
    MATH_tdst_Matrix *M;
    float            oonz, fTemp;
    GEO_tdst_ModifierShadow0   *pst_Data;
    
    pst_Data = (GEO_tdst_ModifierShadow0 *) _pst_Mod->p_Data;

    pst_GO = _pst_DD->pst_CurrentGameObject;
    pst_World = WOR_World_GetWorldOfObject( pst_GO );
    MATH_CopyVector( &st_Pos, OBJ_pst_GetAbsolutePosition( pst_GO ) );
    M = OBJ_pst_GetAbsoluteMatrix(pst_GO);
    MATH_NegVector( &st_Dir, MATH_pst_GetZAxis( M ));

    if( COL_RayCast( pst_World, pst_GO, &st_Pos, &st_Dir, pst_Data->f_Dist, OBJ_C_IdentityFlag_Visu, OBJ_C_IdentityFlag_Anims, NULL, FALSE ) )
    {
        MATH_ScaleVector( &X, &st_Dir, 0.05f );
        MATH_SubVector( &st_Dir, &pst_World->st_RayInfo.st_CollidedPoint, &st_Pos );
        MATH_CopyVector( &C, &pst_World->st_RayInfo.st_CollidedPoint );
        
        oonz = MATH_f_NormVector( &st_Dir );
        fTemp = pst_Data->f_Size * (( pst_Data->f_Dist - oonz) / pst_Data->f_Dist);

        MATH_SubEqualVector( &st_Dir, &X );

        MATH_ScaleVector( &X, MATH_pst_GetXAxis(M), fTemp );
        MATH_ScaleVector( &Y, MATH_pst_GetYAxis(M), fTemp );

        MATH_AddVector( _pst_Obj->dst_Point, &st_Dir, &X );
        MATH_AddVector( _pst_Obj->dst_Point + 1, &st_Dir, &Y );
        MATH_SubVector( _pst_Obj->dst_Point + 2, &st_Dir, &Y );
        MATH_SubVector( _pst_Obj->dst_Point + 3, &st_Dir, &X );

        N = &pst_World->st_RayInfo.st_Normal;
        if (N->z != 0)
        {
            oonz = fOptInv( N->z );
            fTemp = MATH_f_DotProduct( &X, N) * oonz;
            _pst_Obj->dst_Point[0].z -= fTemp;
            _pst_Obj->dst_Point[3].z += fTemp;
            fTemp = MATH_f_DotProduct( &Y, N) * oonz;
            _pst_Obj->dst_Point[1].z -= fTemp;
            _pst_Obj->dst_Point[2].z += fTemp;
        }
    }   
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierShadow0_Unapply(GEO_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj, GDI_tdst_DisplayData *_pst_DD)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_ModifierShadow0_Load(GEO_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                            *pc_Cur;
    GEO_tdst_ModifierShadow0        *pst_Data;
    ULONG                           ul_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    pst_Data = (GEO_tdst_ModifierShadow0 *) _pst_Mod->p_Data;
    pc_Cur = _pc_Buffer;

    /* Size */
    ul_Size = *(ULONG *) pc_Cur;
    pc_Cur += 4;

    pst_Data->f_Dist = *(float *) pc_Cur;
    pc_Cur += 4;
    pst_Data->f_Size = *(float *) pc_Cur;
    pc_Cur += 4;
    
    /* Test size */
    if((ul_Size + 4) != (ULONG) (pc_Cur - _pc_Buffer))
    {
        ul_Size = 0;
    }

    return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierShadow0_Save(GEO_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierShadow0 *pst_Data;
    ULONG                    ul_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierShadow0 *) _pst_Mod->p_Data;

    ul_Size = 8;
    SAV_Buffer(&ul_Size, 4);

    SAV_Buffer(&pst_Data->f_Dist , 4);
    SAV_Buffer(&pst_Data->f_Size , 4);
}


#endif ACTIVE_EDITORS

/*$3
 =======================================================================================================================
    Leg link
 =======================================================================================================================
 */

void GEO_ModifierLegLink_Create(GEO_tdst_Modifier *_pst_Mod, void *p_Data)
{
    _pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierLegLink));

    if(!p_Data)
        L_memset(_pst_Mod->p_Data, 0, sizeof(GEO_tdst_ModifierLegLink));
    else
        L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GEO_tdst_ModifierLegLink));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierLegLink_Destroy(GEO_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierLegLink   *pst_Data;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierLegLink *) _pst_Mod->p_Data;
    MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierLegLink_Apply(GEO_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj, GDI_tdst_DisplayData *_pst_DD)
{
    MATH_tdst_Vector *A, *B, *C, *D, U, V, W;
    float            l0, l1, H, h, n;
    GEO_tdst_ModifierLegLink *pst_Data;
	MATH_tdst_Matrix	*M, st_M;
	OBJ_tdst_GameObject *pst_GO;
    
    pst_Data = (GEO_tdst_ModifierLegLink *) _pst_Mod->p_Data;

    if ( !pst_Data ) return;
    if ( !pst_Data->p_GAOA) return;
    if ( !pst_Data->p_GAOB) return;
    if ( !pst_Data->p_GAOC) return;

    A = OBJ_pst_GetAbsolutePosition( pst_Data->p_GAOA );
    B = OBJ_pst_GetAbsolutePosition( pst_Data->p_GAOB );
    C = OBJ_pst_GetAbsolutePosition( pst_Data->p_GAOC );
    D = OBJ_pst_GetAbsolutePosition( _pst_DD->pst_CurrentGameObject );

    H = MATH_f_Distance( A, C );
	l0 = pst_Data->f_AB;
	l1 = pst_Data->f_BC;
    h = ((l0 * l0) - (l1 * l1) + (H * H)) / (2 * H);
    n = MATH_f_OptSqrtAsm( (l0*l0) - (h*h) );

    MATH_SubVector( &V, C, A );
    MATH_NormalizeEqualVector( &V );
	MATH_ScaleEqualVector( &V, h );
    
	MATH_SubVector( &U, D, A );
	MATH_CrossProduct( &W, &U, &V );
	MATH_CrossProduct( &U, &V, &W );
	MATH_NormalizeEqualVector( &U );
	MATH_ScaleEqualVector( &U, n );

	MATH_AddVector( B, &V, A );
	MATH_AddEqualVector( B, &U );

	pst_GO = (OBJ_tdst_GameObject *) pst_Data->p_GAOA;
	M = OBJ_pst_GetAbsoluteMatrix( pst_GO );
	MATH_NormalizeEqualVector( &W );
	MATH_NegEqualVector( &W );
	MATH_CopyVector( MATH_pst_GetXAxis( M ), &W );
	MATH_SubVector( &U, B, A );
	MATH_NormalizeEqualVector( &U );
	MATH_CopyVector( MATH_pst_GetZAxis( M ), &U );
	MATH_CrossProduct( &V, &U, &W );
	MATH_CopyVector( MATH_pst_GetYAxis( M ), &V );

	if ( OBJ_b_IsChild( pst_GO ) )
	{
		MATH_InvertMatrix( &st_M, OBJ_pst_GetAbsoluteMatrix(pst_GO->pst_Base->pst_Hierarchy->pst_Father) );
        MATH_MulMatrixMatrix( &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, M, &st_M );
	}


	pst_GO = (OBJ_tdst_GameObject *) pst_Data->p_GAOB;
	M = OBJ_pst_GetAbsoluteMatrix( pst_GO );
	MATH_CopyVector( MATH_pst_GetXAxis( M ), &W );
	MATH_SubVector( &U, C, B );
	MATH_NormalizeEqualVector( &U );
	MATH_CopyVector( MATH_pst_GetZAxis( M ), &U );
	MATH_CrossProduct( &V, &U, &W );
	MATH_CopyVector( MATH_pst_GetYAxis( M ), &V );

	if ( OBJ_b_IsChild( pst_GO ) )
	{
		MATH_InvertMatrix( &st_M, OBJ_pst_GetAbsoluteMatrix(pst_GO->pst_Base->pst_Hierarchy->pst_Father) );
        MATH_MulMatrixMatrix( &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, M, &st_M );
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierLegLink_Unapply(GEO_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj, GDI_tdst_DisplayData *_pst_DD)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ModifierLegLink_Load(GEO_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                            *pc_Cur;
    GEO_tdst_ModifierLegLink        *pst_Data;
    ULONG                           ul_Size;
	ULONG                           ul_Key;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    pst_Data = (GEO_tdst_ModifierLegLink *) _pst_Mod->p_Data;
    pc_Cur = _pc_Buffer;

    /* Size */
    ul_Size = *(ULONG *) pc_Cur;
    pc_Cur += 4;

	ul_Key = *(ULONG *) pc_Cur;
    pc_Cur += 4;
	if ((ul_Key != 0) && (ul_Key != BIG_C_InvalidKey) )
		LOA_MakeFileRef( ul_Key, (ULONG *) &pst_Data->p_GAOA, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
	else
		pst_Data->p_GAOA = NULL;
	ul_Key = *(ULONG *) pc_Cur;
    pc_Cur += 4;
	if ((ul_Key != 0) && (ul_Key != BIG_C_InvalidKey) )
		LOA_MakeFileRef( ul_Key, (ULONG *) &pst_Data->p_GAOB, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
	else
		pst_Data->p_GAOA = NULL;
	ul_Key = *(ULONG *) pc_Cur;
    pc_Cur += 4;
	if ((ul_Key != 0) && (ul_Key != BIG_C_InvalidKey) )
		LOA_MakeFileRef( ul_Key, (ULONG *) &pst_Data->p_GAOC, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
	else
		pst_Data->p_GAOA = NULL;
	pst_Data->f_AB = *(float *) pc_Cur;
	pc_Cur += 4;
	pst_Data->f_BC = *(float *) pc_Cur;
	pc_Cur += 4;
	
    return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierLegLink_Save(GEO_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ModifierLegLink *pst_Data;
    ULONG                    ul_Size;
	ULONG                    ul_Key;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GEO_tdst_ModifierLegLink *) _pst_Mod->p_Data;

	ul_Size = 20;
    SAV_Buffer(&ul_Size, 4);

	if (pst_Data->p_GAOA)
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GAOA);
	else
		ul_Key = BIG_C_InvalidKey;
    SAV_Buffer(&ul_Key, 4);

	if (pst_Data->p_GAOB)
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GAOB);
	else
		ul_Key = BIG_C_InvalidKey;
	SAV_Buffer(&ul_Key, 4);

	if (pst_Data->p_GAOC)
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GAOC);
	else
		ul_Key = BIG_C_InvalidKey;
	SAV_Buffer(&ul_Key, 4);

	SAV_Buffer( &pst_Data->f_AB, 4 );
	SAV_Buffer( &pst_Data->f_BC, 4 );
}


#endif ACTIVE_EDITORS


/*$3
 =======================================================================================================================
    General
 =======================================================================================================================
 */

void GEO_Modifier_Init(void)
{
    GEO_gast_ModifierInterface[GEO_C_Modifier_Snap].ul_Type = GEO_C_Modifier_Snap;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Snap].pfnv_Create = GEO_ModifierSnap_Create;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Snap].pfnv_Destroy = GEO_ModifierSnap_Destroy;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Snap].pfnv_Apply = GEO_ModifierSnap_Apply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Snap].pfnv_Unapply = GEO_ModifierSnap_Unapply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Snap].pfnv_ApplyInGeom = GEO_ModifierSnap_Apply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Snap].pfnul_Load = GEO_ul_ModifierSnap_Load;

    GEO_gast_ModifierInterface[GEO_C_Modifier_OnduleTonCorps].ul_Type = GEO_C_Modifier_OnduleTonCorps;
    GEO_gast_ModifierInterface[GEO_C_Modifier_OnduleTonCorps].pfnv_Create = GEO_ModifierOnduleTonCorps_Create;
    GEO_gast_ModifierInterface[GEO_C_Modifier_OnduleTonCorps].pfnv_Destroy = GEO_ModifierOnduleTonCorps_Destroy;
    GEO_gast_ModifierInterface[GEO_C_Modifier_OnduleTonCorps].pfnv_Apply = GEO_ModifierOnduleTonCorps_Apply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_OnduleTonCorps].pfnv_Unapply = GEO_ModifierOnduleTonCorps_Unapply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_OnduleTonCorps].pfnv_ApplyInGeom = GEO_ModifierOnduleTonCorps_Apply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_OnduleTonCorps].pfnul_Load = GEO_ul_ModifierOnduleTonCorps_Load;

    GEO_gast_ModifierInterface[GEO_C_Modifier_Shadow0].ul_Type = GEO_C_Modifier_Shadow0;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Shadow0].pfnv_Create = GEO_ModifierShadow0_Create;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Shadow0].pfnv_Destroy = GEO_ModifierShadow0_Destroy;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Shadow0].pfnv_Apply = GEO_ModifierShadow0_Apply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Shadow0].pfnv_Unapply = GEO_ModifierShadow0_Unapply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Shadow0].pfnv_ApplyInGeom = GEO_ModifierShadow0_Apply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Shadow0].pfnul_Load = GEO_ul_ModifierShadow0_Load;

    GEO_gast_ModifierInterface[GEO_C_Modifier_LegLink].ul_Type = GEO_C_Modifier_LegLink;
    GEO_gast_ModifierInterface[GEO_C_Modifier_LegLink].pfnv_Create = GEO_ModifierLegLink_Create;
    GEO_gast_ModifierInterface[GEO_C_Modifier_LegLink].pfnv_Destroy = GEO_ModifierLegLink_Destroy;
    GEO_gast_ModifierInterface[GEO_C_Modifier_LegLink].pfnv_Apply = GEO_ModifierLegLink_Apply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_LegLink].pfnv_Unapply = GEO_ModifierLegLink_Unapply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_LegLink].pfnv_ApplyInGeom = GEO_ModifierLegLink_Apply;
    GEO_gast_ModifierInterface[GEO_C_Modifier_LegLink].pfnul_Load = GEO_ModifierLegLink_Load;



#ifdef ACTIVE_EDITORS
    GEO_gast_ModifierInterface[GEO_C_Modifier_Snap].pfnv_Save = GEO_ModifierSnap_Save;
    GEO_gast_ModifierInterface[GEO_C_Modifier_OnduleTonCorps].pfnv_Save = GEO_ModifierOnduleTonCorps_Save;
    GEO_gast_ModifierInterface[GEO_C_Modifier_Shadow0].pfnv_Save = GEO_ModifierShadow0_Save;
    GEO_gast_ModifierInterface[GEO_C_Modifier_LegLink].pfnv_Save = GEO_ModifierLegLink_Save;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Modifier *GEO_pst_Modifier_Create(int i, void *p_Data)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Modifier   *pst_Mod;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Mod = (GEO_tdst_Modifier *) MEM_p_Alloc(sizeof(GEO_tdst_Modifier));
    pst_Mod->i = &GEO_gast_ModifierInterface[i];
    pst_Mod->i->pfnv_Create(pst_Mod, p_Data);
    pst_Mod->pst_Next = NULL;
    pst_Mod->pst_Prev = NULL;
    pst_Mod->ul_Flags = 0;
    return pst_Mod;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Modifier_Destroy(GEO_tdst_Modifier *_pst_Mod)
{
    _pst_Mod->i->pfnv_Destroy(_pst_Mod);
    MEM_Free(_pst_Mod);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_Modifier_Load(GEO_tdst_Modifier **_ppst_Mod, char *_pc_Buffer, int _b_Recurs, void *_p_GO )
{
    /*~~~~~~~~~~~~~~~~*/
    char    *pc_Cur;
    /*~~~~~~~~~~~~~~~~*/

    pc_Cur = _pc_Buffer;
    (*_ppst_Mod) = GEO_pst_Modifier_Create(*(ULONG *) pc_Cur, NULL);
#ifdef ACTIVE_EDITORS
	(*_ppst_Mod)->pst_GO = _p_GO;
#endif
    pc_Cur += 4;
    (*_ppst_Mod)->ul_Flags = *(ULONG *) pc_Cur;
    pc_Cur += 4;

    pc_Cur += (*_ppst_Mod)->i->pfnul_Load((*_ppst_Mod), pc_Cur);

    if((_b_Recurs) && (*(ULONG *) pc_Cur != 0xFFFFFFFF))
    {
        pc_Cur += GEO_ul_Modifier_Load(&(*_ppst_Mod)->pst_Next, pc_Cur, 1, _p_GO);
        if((*_ppst_Mod)->pst_Next) (*_ppst_Mod)->pst_Next->pst_Prev = (*_ppst_Mod);
    }

    return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Modifier_AddToGameObject(OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Modifier   *pst_Mod;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
        return;

#ifdef ACTIVE_EDITORS
	_pst_Mod->pst_GO = _pst_GO;
#endif

    if(_pst_GO->pst_Base->pst_Visu->pst_Modifier == NULL)
    {
        _pst_GO->pst_Base->pst_Visu->pst_Modifier = _pst_Mod;
        return;
    }

    pst_Mod = _pst_GO->pst_Base->pst_Visu->pst_Modifier;
    while(pst_Mod->pst_Next) pst_Mod = pst_Mod->pst_Next;

    _pst_Mod->pst_Prev = pst_Mod;
    pst_Mod->pst_Next = _pst_Mod;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Modifier_DelInGameObject(OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Modifier   *pst_Mod;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
        return;

    pst_Mod = _pst_GO->pst_Base->pst_Visu->pst_Modifier;

    while(pst_Mod)
    {
        if(pst_Mod == _pst_Mod)
        {
            if(pst_Mod == _pst_GO->pst_Base->pst_Visu->pst_Modifier)
            {
                if(pst_Mod->pst_Next) pst_Mod->pst_Next->pst_Prev = NULL;
                _pst_GO->pst_Base->pst_Visu->pst_Modifier = pst_Mod->pst_Next;
            }
            else
            {
                if(pst_Mod->pst_Next) pst_Mod->pst_Next->pst_Prev = pst_Mod->pst_Prev;
                pst_Mod->pst_Prev->pst_Next = pst_Mod->pst_Next;
            }

            return;
        }

        pst_Mod = pst_Mod->pst_Next;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Modifier_ReplaceInGameObject
(
    struct OBJ_tdst_GameObject_ *_pst_GO,
    GEO_tdst_Modifier           *_pst_OldMod,
    GEO_tdst_Modifier           *_pst_NewMod
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Modifier   *pst_Mod;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
        return;

    pst_Mod = _pst_GO->pst_Base->pst_Visu->pst_Modifier;
    while(pst_Mod)
    {
        if(pst_Mod == _pst_OldMod)
        {
            _pst_NewMod->pst_Next = _pst_OldMod->pst_Next;
            _pst_NewMod->pst_Prev = _pst_OldMod->pst_Prev;

            if(_pst_NewMod->pst_Next) _pst_NewMod->pst_Next->pst_Prev = _pst_NewMod;
            if(_pst_NewMod->pst_Prev) _pst_NewMod->pst_Prev->pst_Next = _pst_NewMod;

            if(pst_Mod == _pst_GO->pst_Base->pst_Visu->pst_Modifier)
                _pst_GO->pst_Base->pst_Visu->pst_Modifier = _pst_NewMod;
            return;
        }

        pst_Mod = pst_Mod->pst_Next;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Modifier_Save(GEO_tdst_Modifier *_pst_Mod, int _i_Recurs)
{
    SAV_Buffer(&_pst_Mod->i->ul_Type, 4);
    SAV_Buffer(&_pst_Mod->ul_Flags, 4);

    _pst_Mod->i->pfnv_Save(_pst_Mod);

    if(_pst_Mod->pst_Next && _i_Recurs) GEO_Modifier_Save(_pst_Mod->pst_Next, _i_Recurs);
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
