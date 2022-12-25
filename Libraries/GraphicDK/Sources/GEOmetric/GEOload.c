/*$T GEOload.c GC!1.40 07/13/99 15:04:07 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORload.h"

#include "GRObject/GROstruct.h"
#include "LIGHT/LIGHTstruct.h"
#include "GEOmetric/GEOobjectcomputing.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

#ifndef KER_IS_NOT_A_CHEVRE
#define KER_IS_NOT_A_CHEVRE
#endif

#ifdef JADEFUSION
void COL_OK3_Build(void *_p_Data, BOOL _b_ColMap, BOOL _b_ReBuild);
#else
void	COL_OK3_Build(void  *, BOOL, BOOL);
#endif

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

BOOL GEO_b_IsInFix = FALSE;

#ifdef ACTIVE_EDITORS

void GEO_WarningIfObjectIsTooBig(GEO_tdst_Object *pst_Object)
{
    char    *sz_FileName, *sz_Ext;
    char    sz_Msg[256];
    ULONG   ul_Index;
    LONG    l_Nb;
#if !defined(XML_CONV_TOOL)
    ul_Index = BIG_ul_SearchKeyToFat( LOA_ul_GetCurrentKey() );
    sz_FileName = BIG_NameFile( ul_Index );
    sz_Ext = strrchr( sz_FileName, '.' );
    if (sz_Ext) *sz_Ext = 0;
    pst_Object->st_Id.sz_Name = (char *) MEM_p_Alloc(L_strlen(sz_FileName) + 1);
    L_strcpy(pst_Object->st_Id.sz_Name, sz_FileName);
    if (sz_Ext) *sz_Ext = '.';
#else
		if (st_Id.sz_Name)
		{
	        pst_Object->st_Id.sz_Name = (char *) MEM_p_Alloc(L_strlen(st_Id.sz_Name) + 1);
		    L_strcpy(pst_Object->st_Id.sz_Name, st_Id.sz_Name);
		}
#endif
    if(pst_Object->st_Id.i->ul_Type == GRO_Geometric)
    {
#if !defined(XML_CONV_TOOL)//popoverif
		if ( pst_Object->l_NbPoints > 5000 )
        {
            sprintf( sz_Msg, "[%08X] has more than 5000 vertex (%d)", LOA_ul_GetCurrentKey(), pst_Object->l_NbPoints );
            ERR_X_Warning(0,sz_Msg,sz_FileName);
        }

        l_Nb = GEO_l_GetNumberOfTriangles( pst_Object );
        if ( l_Nb > 3000 )
        {
            sprintf( sz_Msg, "[%08X] has more than 3000 triangles (%d)", LOA_ul_GetCurrentKey(), l_Nb );
            ERR_X_Warning(0,sz_Msg,sz_FileName);
        }
#endif // !defined(XML_CONV_TOOL)
    }
}
#endif //ACTIVE_EDITORS


/*
 ===================================================================================================
 ===================================================================================================
 */
ULONG GEO_ul_Load_ObjectCallback(ULONG _ul_PosFile)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Object *pst_Object;
    char            *pc_Buffer;
    ULONG           l_Length;
    GRO_tdst_Struct st_Id;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEMpro_StartMemRaster();

	GEO_b_IsInFix = FALSE;

    pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &l_Length);

    pc_Buffer += GRO_l_Struct_Load(&st_Id, pc_Buffer);
    pst_Object = (GEO_tdst_Object*)st_Id.i->pfnp_CreateFromBuffer(&st_Id, &pc_Buffer, WOR_gpst_WorldToLoadIn);


#ifdef KER_IS_NOT_A_CHEVRE
	if((!(LOA_IsBinaryData())) && (st_Id.i->ul_Type == GRO_Geometric))
	{
		GEO_I_Need_The_Triangles_Begin(pst_Object,1);
		COL_OK3_Build(pst_Object, FALSE, FALSE);
		GEO_I_Need_The_Triangles_End(pst_Object,1);
	}
#else
	if(st_Id.i->ul_Type == GRO_Geometric)  COL_OK3_Build(pst_Object, FALSE, FALSE);
#endif

#if !defined(XML_CONV_TOOL)
    GRO_Struct_Free(&st_Id);
#endif

#ifdef ACTIVE_EDITORS
    GEO_WarningIfObjectIsTooBig(pst_Object);
#endif

    /* Reading is completed */
	MEMpro_StopMemRaster(MEMpro_Id_GDK_ObjectCallback);


    return (ULONG) pst_Object;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
GRO_tdst_Struct *GEO_pst_Object_Load(BIG_INDEX _ul_Index, WOR_tdst_World *_pst_World)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_KEY         ul_Key;
    GEO_tdst_Object *pst_Object;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    WOR_gpst_WorldToLoadIn = _pst_World;

    if(_ul_Index == BIG_C_InvalidIndex) return NULL;
    ul_Key = BIG_FileKey(_ul_Index);
    if(ul_Key == BIG_C_InvalidKey) return NULL;

    LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Object, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
    LOA_Resolve();

    return &pst_Object->st_Id;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
