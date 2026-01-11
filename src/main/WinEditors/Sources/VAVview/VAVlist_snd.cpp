/*$T VAVlist_snd.cpp GC! 1.097 06/21/01 12:27:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDmodifier.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"
#include "EDItors/Sources/SOuNd/SONframe.h" 

/*
 =======================================================================================================================
    Aim:    Macro to add a new item in the list
 =======================================================================================================================
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
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::SNDAddBank(POSITION pos, SND_tdun_Main *_pst_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item, *po_NewItem;
	int					i_NumFields;
	int					i;
	SND_tdst_Bank*pBank;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get the pointer of instance */
	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);

	if(_pst_Instance->st_Bank.ul_Flags & SND_Cte_Bank)
		pBank = & _pst_Instance->st_Bank;
	else
		pBank = _pst_Instance->st_Metabank.pst_Curr;

	/* Init all variables */
	i_NumFields = 0;

    if(pBank)
    {
	    for(i = 0; i < pBank->i_SoundNb; i++)
	    {
		    /*~~~~~~~~~~~~*/
		    char	tmp[128];
		    /*~~~~~~~~~~~~*/

		    if(pBank->pi_Bank[i] != -1)
		    {
                ULONG ul_Fat;
                ul_Fat = BIG_ul_SearchKeyToFat(SND_gst_Params.dst_Sound[pBank->pi_Bank[i]].ul_FileKey);
                if(ul_Fat ==BIG_C_InvalidIndex)
					snprintf( tmp, sizeof(tmp), "%d - Invalid", i );
                else
					snprintf( tmp, sizeof(tmp), "%d - %s", i, BIG_NameFile( ul_Fat ) );
    			
                A(
				    tmp,
				    EVAV_EVVIT_SubStruct,
				    &SND_gst_Params.dst_Sound[pBank->pi_Bank[i]],
				    EVAV_ReadOnly,
				    LINK_C_SND_Sound,
				    0,
				    sizeof(SND_tdst_OneSound),
				    NULL,
				    po_Item->mx_Color,
				    0,
				    0,
				    0,
				    0,
				    0,
				    0
			    );
		    }
	    }
    }
	/* Set number of fields of the initial pointer */
	po_Item->mi_NumFields = i_NumFields;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::SNDAddSound(POSITION pos, SND_tdst_OneSound *_pst_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item, *po_NewItem;
	int					i_NumFields;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get the pointer of instance */
	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);

	/* Init all variables */
	i_NumFields = 0;

	/* Set number of fields of the initial pointer */
	A(
		"Edit",
		EVAV_EVVIT_SndKey,
		&_pst_Instance->ul_FileKey,
		EVAV_None,
		0,
		0,
		4,
		NULL,
		po_Item->mx_Color,
		0,
		0,
		0,
		0,
		0,
		0
	);

	A(
		"Find",
		EVAV_EVVIT_SndKey,
		&_pst_Instance->ul_FileKey,
		EVAV_None,
		0,
		0,
		4,
		NULL,
		po_Item->mx_Color,
		0,
		0,
		0,
		0,
		0,
		0
	);
	po_Item->mi_NumFields = i_NumFields;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

