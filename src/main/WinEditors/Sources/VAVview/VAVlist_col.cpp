/*$T VAVlist_col.cpp GC!1.5 11/23/99 12:05:40 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "LINKs/LINKstruct.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"

/*
 ===================================================================================================
    Aim:    Macro to add a new item in the list
 ===================================================================================================
 */
#define A(a, b, c, d, e, f, g, h, i, p4, p5, p6, p7, fil, help) \
    if(c) \
    { \
        pos = mpo_ListItems->InsertAfter \
            ( \
                pos, \
                po_NewItem = new EVAV_cl_ViewItem(a, b, c, d, e, f, g, h, i, p4, p5, p6, p7, fil) \
            ); \
        i_NumFields++; \
        po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data; \
        po_NewItem->psz_Help = help; \
    }

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_cl_ListBox::COLAddInstance(POSITION pos, COL_tdst_Instance *_pst_Instance)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EVAV_cl_ViewItem    *po_Item, *po_NewItem;
    EVAV_tdpfnv_Change  pfn_CB;
    int                 i_NumFields;
    COLORREF            x_Col;
    ULONG               ulF;
	COL_tdst_ZDx		*pst_ZDx, *pst_LastZDx;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Colors */
    x_Col = 0x008EB48B; /* Init field */

    /* Get the pointer of instance */
    if(!pos)
        po_Item = mpo_ListItems->GetTail();
    else
        po_Item = mpo_ListItems->GetAt(pos);

    /* Init all variables */
    pfn_CB = LINK_gast_StructTypes[LINK_C_COL_Instance].pfn_CB;
    i_NumFields = 0;
    ulF = 0;
    /*$off*/
    A("Priority" ,				EVAV_EVVIT_Int, &_pst_Instance->c_Priority, EVAV_ReadOnly, 0, 0, 1, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
    A("Nb Zones" ,				EVAV_EVVIT_Int, &_pst_Instance->uc_NbOfZDx, EVAV_ReadOnly, 0, 0, 1, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
	A("Nb Shared" ,				EVAV_EVVIT_Int, &_pst_Instance->uc_NbOfShared, EVAV_ReadOnly, 0, 0, 1, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
	A("Nb Specific" ,			EVAV_EVVIT_Int, &_pst_Instance->uc_NbOfSpecific, EVAV_ReadOnly, 0, 0, 1, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
	A("Activation" ,			EVAV_EVVIT_Int, &_pst_Instance->uw_Activation, EVAV_ReadOnly, 0, 0, 2, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
	A("Max LOD" ,				EVAV_EVVIT_Int, &_pst_Instance->uc_MaxLOD, 0, 0, 0, 1, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
	A("Min LOD" ,				EVAV_EVVIT_Int, &_pst_Instance->uc_MinLOD, 0, 0, 0, 1, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
	A("Current LOD" ,			EVAV_EVVIT_Int, &_pst_Instance->uc_LOD, EVAV_ReadOnly, 0, 0, 1, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
	A("Recal in the Wall" ,		EVAV_EVVIT_Bool, &_pst_Instance->b_InTheWall, EVAV_ReadOnly, 0, 0, 1, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
	A("Crossable" ,				EVAV_EVVIT_SubStruct, &_pst_Instance->uw_Crossable, 0, LINK_C_ENG_COL_Crossable, 0, 2, pfn_CB, po_Item->mx_Color, 0, 0, 0, 0, 0, 0);
	if(_pst_Instance->uc_NbOfSpecific)
	{
		pst_ZDx = _pst_Instance->past_Specific;
		pst_LastZDx = pst_ZDx + _pst_Instance->uc_NbOfSpecific;

		for(; pst_ZDx < pst_LastZDx; pst_ZDx ++)
		{
			A("Zone" ,	EVAV_EVVIT_Pointer, pst_ZDx, 0, LINK_C_Zone, 0, 0, pfn_CB, x_Col, 0, 0, 0, 0, 0, 0);
		}
	}



/*$on*/

    /* Set number of fields of the initial pointer */
    po_Item->mi_NumFields = i_NumFields;
}

