/*$T IMPbase.c GC!1.38 05/27/99 11:24:02 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"
#include "IMPbase.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"

#ifdef ACTIVE_EDITORS
IMP_tdst_ImportDialog IMP_gst_ImportDialog_Interface;


/*$2
 ---------------------------------------------------------------------------------------------------
    Array for import callback
 ---------------------------------------------------------------------------------------------------
 */

#define IMP_C_NumberOfImportData    30

typedef struct  IMP_tdst_ImportData_
{
    char                sz_Ext[4];
    IMP_tdpfn_Import    pfnb_Callback;
} IMP_tdst_ImportData;

static IMP_tdst_ImportData  IMP_sgast_ImportData[IMP_C_NumberOfImportData];

/*
 ===================================================================================================
 ===================================================================================================
 */
void IMP_Init(void)
{
    L_memset(IMP_sgast_ImportData, 0, sizeof(IMP_tdst_ImportData) * IMP_C_NumberOfImportData);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
IMP_tdst_ImportData *IMP_pst_FindImportData(char *_sz_Ext)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    IMP_tdst_ImportData *pst_ID, *pst_LastID;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    L_strlwr(_sz_Ext);

    pst_ID = IMP_sgast_ImportData;
    pst_LastID = pst_ID + IMP_C_NumberOfImportData;

    for(; pst_ID < pst_LastID; pst_ID++)
    {
        if(strcmp(_sz_Ext, pst_ID->sz_Ext) == 0) return pst_ID;
    }

    return NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
IMP_tdst_ImportData *IMP_pst_FindEmptyImportData(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    IMP_tdst_ImportData *pst_ID, *pst_LastID;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_ID = IMP_sgast_ImportData;
    pst_LastID = pst_ID + IMP_C_NumberOfImportData;

    for(; pst_ID < pst_LastID; pst_ID++)
    {
        if(*pst_ID->sz_Ext == 0) return pst_ID;
    }

    return NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL IMP_b_AddImportCallback(char *_sz_Ext, IMP_tdpfn_Import pfn_ImportCallback)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    IMP_tdst_ImportData *pst_ID;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_ID = IMP_pst_FindImportData(_sz_Ext);
    if(pst_ID == NULL)
    {
        pst_ID = IMP_pst_FindEmptyImportData();
        if(pst_ID == NULL) return FALSE;
        L_strcpy(pst_ID->sz_Ext, _sz_Ext);
    }

    pst_ID->pfnb_Callback = pfn_ImportCallback;
    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL IMP_b_SpecialImportFile(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                *sz_Ext;
    IMP_tdst_ImportData *pst_ID;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sz_Ext = strrchr(_psz_ExternName, '.');
    if(sz_Ext)
    {
        pst_ID = IMP_pst_FindImportData(sz_Ext + 1);
        if(pst_ID && pst_ID->pfnb_Callback)
		{
			GetAsyncKeyState(VK_SHIFT);
			if(GetAsyncKeyState(VK_SHIFT) == 0)
			{
				if(IMP_gst_ImportDialog_Interface.pfnb_Request)
				{
					if(IMP_gst_ImportDialog_Interface.pfnb_Request(sz_Ext) != -1)
						return pst_ID->pfnb_Callback(_psz_BigPathName, _psz_BigFileName, _psz_ExternName);
					else
						return TRUE;
				}
				else
					return pst_ID->pfnb_Callback(_psz_BigPathName, _psz_BigFileName, _psz_ExternName);
			}
		}
    }

    return TRUE;
}

#endif