/*$T SONframe_mdf.cpp GC 1.138 04/01/04 09:39:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "Res/Res.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAname_dlg.h"

/*$2- engine ---------------------------------------------------------------------------------------------------------*/

#include "WORld/WORstruct.h"
#include "WORld/WORvars.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_GEN.h"
#include "MoDiFier/MDFmodifier_SOUNDFX.h"
#include "MoDiFier/MDFmodifier_SOUNDLOADING.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "BIGfiles/LOAding/LOAdefs.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDload.h"
#include "SouND/sources/SNDconst.h"
#include "SouND/sources/SNDdialog.h"
#include "SouND/sources/SNDambience.h"
#include "SouND/sources/SNDmusic.h"
#include "SouND/sources/SNDmodifier.h"
#include "SouND/sources/SNDvolume.h"
#include "SouND/sources/SNDstream.h"

/*$2- sound editor ---------------------------------------------------------------------------------------------------*/

#include "SONframe.h"
#include "SONview.h"
#include "SONutil.h"

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::MdF_GaoHasChanged(unsigned int lgao)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO;
	MDF_tdst_Modifier		*pst_MdF;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	ULONG					ul_fat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) lgao;

	if(!pst_GO) return;
	if(!pst_GO->pst_Extended) return;
	if(!pst_GO->pst_Extended->pst_Modifiers) return;

	pst_MdF = pst_GO->pst_Extended->pst_Modifiers;
	while(pst_MdF)
	{
		if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
		{
			pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;

			ul_fat = BIG_ul_SearchKeyToFat(pst_SndMdF->ui_FileKey);
			if(ul_fat == BIG_C_InvalidIndex)
			{
				/*~~~~~~~~~~~~~*/
				char	log[512];
				/*~~~~~~~~~~~~~*/

				sprintf
				(
					log,
					"One Sound MdF (gao %s index %d) is referencing a bad key : %x",
					pst_GO->sz_Name,
					pst_SndMdF->ui_Id,
					pst_SndMdF->ui_FileKey
				);
				ERR_X_Warning(0, log, NULL);
				pst_SndMdF->ui_FileKey = BIG_C_InvalidKey;
			}

			if(pst_SndMdF->i_SndIdx != -1)
			{
				SND_StreamFlushOneSound(pst_SndMdF->i_SndIdx, TRUE);
				SND_DeleteByIndex(pst_SndMdF->i_SndIdx);
			}

			if(pst_SndMdF->ui_FileKey != BIG_C_InvalidKey)
			{
				pst_SndMdF->i_SndIdx = SND_l_AddSModifier(pst_SndMdF->ui_FileKey);
				LOA_Resolve();
                ESON_LoadAllSounds();
			}
		}

		pst_MdF = pst_MdF->pst_Next;
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
