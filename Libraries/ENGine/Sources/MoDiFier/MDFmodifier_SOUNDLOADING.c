/*$T MDFmodifier_SOUNDLOADING.c GC 1.138 12/15/04 11:32:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "Precomp.h"

#include "GEOmetric/GEOobject.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_SOUNDLOADING.h"
#include "BASe/MEMory/MEM.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDmacros.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDinterface.h"
#include "SouND/Sources/SNDload.h"

#include "ENGine/Sources/OBJects/OBJorient.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAdefs.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAread.h"
#include "SDK/Sources/BIGfiles/BIGfat.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_LoadingSound_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_LoadingSound	*pst_SndMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SndMdF = (MDF_tdst_LoadingSound *) MEM_p_Alloc(sizeof(MDF_tdst_LoadingSound));

	if(pst_SndMdF)
	{
		L_memset(pst_SndMdF, 0, sizeof(MDF_tdst_LoadingSound));
		pst_SndMdF->ui_MdFVersion = SND_Cte_MdFLSVersion;
		pst_SndMdF->ui_MdfFlag = 0;

		pst_SndMdF->ul_FileKey = -1;
		pst_SndMdF->ui_SndFlags = -1;
		pst_SndMdF->i_SndIdx = -1;
		pst_SndMdF->f_LoadingDistance = 50;

		_pst_Mod->p_Data = pst_SndMdF;

		/* duplicate operation ? */
		if(p_Data) _pst_Mod->i->pfnul_Load(_pst_Mod, (char *) p_Data);
	}
	else
		_pst_Mod->p_Data = 0;

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGen;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_LoadingSound_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_LoadingSound	*pst_SndMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SndMdF = (MDF_tdst_LoadingSound *) _pst_Mod->p_Data;
	
    SND_WacUnregisterMdF(_pst_Mod);
    SND_DeleteByIndex(pst_SndMdF->i_SndIdx);
	
    if(pst_SndMdF) MEM_Free(pst_SndMdF);

	_pst_Mod->p_Data = NULL;
	_pst_Mod->ul_Flags = MDF_C_Modifier_Inactive;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_LoadingSound_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
    //SND_WacRegisterMdF(_pst_Mod);
	
    _pst_Mod->ul_Flags &= ~MDF_C_Modifier_ApplyGen;
	_pst_Mod->ul_Flags |= MDF_C_Modifier_Inactive;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_LoadingSound_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_LoadingSound_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
    _pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGen;
	_pst_Mod->ul_Flags &= ~MDF_C_Modifier_Inactive;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG MDF_LoadingSound_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_LoadingSound	*pst_Dst;
	char					*pc_Buf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Buf = _pc_Buffer;
	pst_Dst = (MDF_tdst_LoadingSound *) _pst_Mod->p_Data;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    load all struct fields
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(pst_Dst)
	{
		pst_Dst->ui_MdFVersion = LOA_ReadUInt(&pc_Buf);
		pst_Dst->ui_MdfFlag = LOA_ReadUInt(&pc_Buf);
		pst_Dst->ul_FileKey = LOA_ReadUInt(&pc_Buf);
		pst_Dst->ui_SndFlags = LOA_ReadUInt(&pc_Buf);
		pst_Dst->i_SndIdx = LOA_ReadInt(&pc_Buf);
		pst_Dst->f_LoadingDistance = LOA_ReadFloat(&pc_Buf);
		LOA_ReadCharArray_Ed(&pc_Buf, NULL, MDF_Cte_SndLS_ReserveSize); /* skip asz_Reserve */
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    (re)set dynamic fields
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_Dst->i_SndIdx = -1;

	if(pst_Dst->ui_SndFlags & SND_Cul_SF_LoadingSound)
		pst_Dst->i_SndIdx = SND_l_AddLoadingSound(pst_Dst->ul_FileKey);
	else if(pst_Dst->ui_SndFlags & SND_Cul_SF_SModifier)
		pst_Dst->i_SndIdx = SND_l_AddSModifier(pst_Dst->ul_FileKey);

    

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGen;

	return(pc_Buf - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_LoadingSound_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_LoadingSound	st_Tmp;
	MDF_tdst_LoadingSound	*pst_Src;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Src = (MDF_tdst_LoadingSound *) _pst_Mod->p_Data;

	if(pst_Src)
	{
		L_memcpy(&st_Tmp, pst_Src, sizeof(MDF_tdst_LoadingSound));

		/* clear dyn fields */
		st_Tmp.ui_MdfFlag = 0;
		st_Tmp.i_SndIdx = -1;

		/* save data */
		SAV_Buffer(&st_Tmp, sizeof(MDF_tdst_LoadingSound));
	}
}

int MDF_LoadingSound_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src )
{
    MDF_LoadingSound_Load(_pst_Dst, (char* )_pst_Src->p_Data);
	return sizeof( MDF_tdst_LoadingSound );
}

#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
