/*$T VAVview_go.cpp GC!1.41 08/30/99 11:32:37 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "VAVview/VAVview.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LIGHT/LIGHTstruct.h"
#include "Dialogs/DIAfile_dlg.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "GRObject/GROstruct.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_GroType_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul = *(ULONG *) _p_Value;
    if (ul >= GRO_Cl_NumberOfInterfaces)
        ul = 0;
    
    return GRO_gasz_InterfaceName[ul];
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_GroType_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CComboBox           *po_Combo;
    int                 iSel, iIndex;
    GRO_tdst_Struct     *pst_Gro;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_Combo = (CComboBox *) _po_Wnd;
    pst_Gro = (GRO_tdst_Struct *) _po_Data->mi_Param1;

    /* Fill the combo box with the list of all GRaphic Object types */
    if(_b_Fill)
    {
        po_Combo->ResetContent();

        for (iSel = 0; iSel < GRO_Cl_NumberOfInterfaces; iSel++)
        {
            iIndex = po_Combo->InsertString(iSel, GRO_gasz_InterfaceName[iSel] );
            po_Combo->SetItemData(iIndex, iSel);
        }

        /* Select current gro type  */
        po_Combo->SetTopIndex(0);
        po_Combo->SetCurSel( *(ULONG *) _p_Value );
        return;
    }

    /* Search the selected gro type , and change it */
    iSel = po_Combo->GetCurSel();
    if(iSel == -1) return;
    *(ULONG *) _p_Value = po_Combo->GetItemData(iSel);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_GroLightType_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul = (*(ULONG *) _p_Value) & LIGHT_Cul_LF_Type;
    if (ul >= LIGHT_Cul_LF_NumberOfType)
        ul = 0;
    
    return LIGHT_gasz_TypeName[ul];
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_GroLightType_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CComboBox           *po_Combo;
    int                 iSel, iIndex;
    GRO_tdst_Struct     *pst_Gro;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_Combo = (CComboBox *) _po_Wnd;
    pst_Gro = (GRO_tdst_Struct *) _po_Data->mi_Param1;

    /* Fill the combo box with the list of all Graphic Object light types */
    if(_b_Fill)
    {
        po_Combo->ResetContent();

        for (iSel = 0; iSel < LIGHT_Cul_LF_NumberOfType; iSel++)
        {
            iIndex = po_Combo->InsertString(iSel, LIGHT_gasz_TypeName[iSel] );
            po_Combo->SetItemData(iIndex, iSel);
        }

        /* Select current light type  */
        po_Combo->SetTopIndex(0);
        po_Combo->SetCurSel( (*(ULONG *) _p_Value) & LIGHT_Cul_LF_Type);
        return;
    }

    /* Search the selected light type , and change it */
    iSel = po_Combo->GetCurSel();
    if(iSel == -1) return;
    *(ULONG *) _p_Value = ((*(ULONG *) _p_Value) & ~LIGHT_Cul_LF_Type ) | po_Combo->GetItemData(iSel);
}

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_Geom_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Object	*pst_Geom;
	ULONG			ul_Key;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Geom = *(GEO_tdst_Object **) _p_Value;
	if(!pst_Geom)
	{
		return "";
	}

	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Geom);
	ul_Key = BIG_ul_SearchKeyToFat(ul_Key);
    return BIG_NameFile(ul_Key);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_Geom_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Choose a geometry", 0, 0, 1, NULL, "*.gro");
	CString				o_Temp;
	BIG_INDEX			ul_Index;
    GEO_tdst_Object		*pst_Geom;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_b_Fill) return;

	if(o_File.DoModal() == IDOK)
	{
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			if(!BIG_b_IsFileExtension(ul_Index, ".gro"))
			{
				ERR_X_ForceError("This is not a geometry file", NULL);
			}
			else
			{
			    pst_Geom = *(GEO_tdst_Object **) _p_Value;
				if(pst_Geom)
				{
					((GRO_tdst_Struct *) pst_Geom)->i->pfn_AddRef(pst_Geom, -1);
					((GRO_tdst_Struct *) pst_Geom)->i->pfn_Destroy(pst_Geom);
				}

				LOA_MakeFileRef
				(
					BIG_FileKey(ul_Index),
					(ULONG *) _p_Value,
					GEO_ul_Load_ObjectCallback,
					LOA_C_MustExists
				);
				LOA_Resolve();

			    pst_Geom = *(GEO_tdst_Object **) _p_Value;
				((GRO_tdst_Struct *) pst_Geom)->i->pfn_AddRef(pst_Geom, 1);
				LINK_Refresh();
			}
		}
	}
}

#endif /* ACTIVE_EDITORS */
