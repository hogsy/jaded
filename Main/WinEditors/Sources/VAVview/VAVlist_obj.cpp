/*$T VAVlist_obj.cpp GC! 1.097 04/10/02 17:01:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "BASe/MEMory/Mem.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJgizmo.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/GRP/GRPload.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GAO.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SDW.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_XMEN.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_XMEC.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GPG.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_ROTR.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SNAKE.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SOUNDFX.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SOUNDLOADING.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_PROTEX.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_MPAG.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_LAZY.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_FUR.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_ODE.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_MatrixBore.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GRID.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SNDVOL.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_BrumeDynamique.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_Vine.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_FCLONE.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_UVTexWave.h"

#include "ENGine/Sources/MoDiFier/MDFmodifier_WATER3D.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_Disturber.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SFX.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_Weather.h"
#ifdef JADEFUSION
#include "ENGine/Sources/MoDiFier/MDFmodifier_SoftBody.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_Wind.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2_Holder.h"
#endif

#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEOload.h"
#include "MATerial/MATstruct.h"
#include "Material/Matsprite.h"
#include "CAMera/CAMstruct.h"

/* #include "SPriteList/SPLstruct.h" */
#include "STRing/STRstruct.h"
#include "PArticleGenerator/PAGstruct.h"
#include "LIGHT/LIGHTstruct.h"
#include "SELection/SELection.h"
#include "TEXture/TEXprocedural.h"
#include "TEXture/TEXanimated.h"
#include "SDK/Sources/BIGfiles/BIGfat.h"
#include "SDK/Sources/BIGfiles/LOADing/LOAdefs.h"

#include "EDImsg.h"
#include "EDImainframe.h"

#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDfx.h"
#include "EDItors/Sources/SOuNd/SONframe.h"
#include "EDItors/Sources/SOuNd/SONmsg.h"

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeUtils.h"
#include "XenonGraphics/XeLightShaftManager.h"
#endif

#ifdef ODE_INSIDE
#include "objects.h"
#endif

#ifdef JADEFUSION
extern ULONG SPG2_PrimitivLimit;
extern ULONG SPG2_WHOLESCENENUM;
#else
extern "C" ULONG SPG2_PrimitivLimit;
extern "C" ULONG SPG2_WHOLESCENENUM;
#endif

extern EVAV_cl_ListBox	*gpo_CurVavListBox;
extern void				LINK_CallBack_ChangeCobID(void *, void *, void *, LONG);
extern void				LINK_CallBack_ChangeElementID(void *, void *, void *, LONG);

#ifdef JADEFUSION
extern void SND_FxNetworkDeleteNode(GEN_tdst_ModifierSoundFx *pOldNode);
extern GEN_tdst_ModifierSoundFx *SND_p_FxNetworkGetMaster(int id);
#else
extern "C" void SND_FxNetworkDeleteNode(GEN_tdst_ModifierSoundFx *pOldNode);
extern "C" GEN_tdst_ModifierSoundFx *SND_p_FxNetworkGetMaster(int id);
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define M_InsertItemColor(a, b, c, d, e) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, 0, 0, 0, d, e, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItem(a, b, c, d) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, 0, 0, 0, d, po_Item->mx_Color, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#ifdef JADEFUSION
#define M_InsertItemPrivate(a, b, c, d) \
    pos = mpo_ListItems->InsertAfter \
    ( \
    pos, \
    po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, 0, 0, 0, d, po_Item->mx_Color, 1, 0, 0, 0, EVAV_Filter_Private) \
    ); \
    po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;
#endif

#define M_InsertItem4(a, b, c, f) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, 0, 0, 0, NULL, po_Item->mx_Color, f) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItem4clbk(a, b, c, e, f) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, 0, 0, 0, e, po_Item->mx_Color, f) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItemRO(a, b, c, d) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_ReadOnly, 0, 0, 0, d, po_Item->mx_Color, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItem1(a, b, c, d, e) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, d, 0, 0, e, po_Item->mx_Color, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#ifdef JADEFUSION
#define M_InsertItem1Private(a, b, c, d, e) \
    pos = mpo_ListItems->InsertAfter \
    ( \
    pos, \
    po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, d, 0, 0, e, po_Item->mx_Color, 1, 0, 0, 0, EVAV_Filter_Private) \
    ); \
    po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;
#endif

#define M_InsertItem1COLOR(a, b, c, d, e, f) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, d, 0, 0, e, f, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;


#define M_InsertItem1RO(a, b, c, d, e) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_ReadOnly, d, 0, 0, e, po_Item->mx_Color, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItem12(a, b, c, d, e, f) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, d, e, 0, f, po_Item->mx_Color, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItem12RO(a, b, c, d, e, f) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_ReadOnly, d, e, 0, f, po_Item->mx_Color, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;


#define M_InsertItem123(a, b, c, d, e, f, g) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, d, e, f, g, po_Item->mx_Color, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItem123RO(a, b, c, d, e, f, g) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_ReadOnly, d, e, f, g, po_Item->mx_Color, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItem1234(a, b, c, d, e, f, g, h) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, d, e, f, g, po_Item->mx_Color, h) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItem1234RO(a, b, c, d, e, f, g, h) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_ReadOnly, d, e, f, g, po_Item->mx_Color, h) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItem3(a, b, c, d, e) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, 0, 0, d, e, po_Item->mx_Color, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItemROColor(a, c, d) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem \
				( \
					a, \
					EVAV_EVVIT_CustSep, \
					c, \
					EVAV_ReadOnly, \
					0, \
					0, \
					0, \
					d, \
					po_Item->mx_Color /* 0x00E6E6E6 */ , \
					1 \
				) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#define M_InsertItemROColorSpecial(a, c, d, e) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem \
				( \
					a, \
					EVAV_EVVIT_CustSep, \
					c, \
					EVAV_ReadOnly, \
					0, \
					e, \
					0, \
					d, \
					po_Item->mx_Color /* 0x00E6E6E6 */ , \
					1 \
				) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

#ifndef JADEFUSION
extern "C"
{
#endif
	extern BOOL		OBJ_gb_DebugPhotoMode;
	extern ULONG	EDI_OUT_gl_ForceSetMode;
#ifndef JADEFUSION
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */

void MDF_ODE_ChangeValues(GAO_tdst_ModifierODE *pst_Mod_ODE, UCHAR _uc_Type, OBJ_tdst_GameObject *_pst_GO1, OBJ_tdst_GameObject *_pst_GO2)
{
	if(!pst_Mod_ODE) return;

	switch(pst_Mod_ODE->uc_Type)
	{
		case ODE_JointTypeBall:
		{
			break;
		}

		case ODE_JointTypeHinge:
		{
			if(pst_Mod_ODE->f_LoLimit)
				dJointSetHingeParam(pst_Mod_ODE->i_ode_joint_id,dParamLoStop, pst_Mod_ODE->f_LoLimit);

			if(pst_Mod_ODE->f_HiLimit)
				dJointSetHingeParam(pst_Mod_ODE->i_ode_joint_id,dParamHiStop, pst_Mod_ODE->f_HiLimit);

			if(pst_Mod_ODE->f_BounceStop)
			{
				dJointSetHingeParam (pst_Mod_ODE->i_ode_joint_id, dParamBounce, pst_Mod_ODE->f_BounceStop);
			}

			break;
		}

		case ODE_JointTypeAMotor:
		{
			if(pst_Mod_ODE->f_LoLimit)
				dJointSetAMotorParam(pst_Mod_ODE->i_ode_joint_id,dParamLoStop, pst_Mod_ODE->f_LoLimit);

			if(pst_Mod_ODE->f_HiLimit)
				dJointSetAMotorParam(pst_Mod_ODE->i_ode_joint_id,dParamHiStop, pst_Mod_ODE->f_HiLimit);

			if(pst_Mod_ODE->f_Friction)
			{
				dJointSetAMotorParam (pst_Mod_ODE->i_ode_joint_id, dParamVel, 0);
				dJointSetAMotorParam(pst_Mod_ODE->i_ode_joint_id, dParamFMax, pst_Mod_ODE->f_Friction);
			}

			if(pst_Mod_ODE->f_BounceStop)
			{
				dJointSetAMotorParam (pst_Mod_ODE->i_ode_joint_id, dParamBounce, pst_Mod_ODE->f_BounceStop);
			}


			break;
		}

	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ODE_ChangeParam(GAO_tdst_ModifierODE *pst_Mod_ODE, UCHAR _uc_Type, OBJ_tdst_GameObject *_pst_GO1, OBJ_tdst_GameObject *_pst_GO2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	dBodyID			ode_body_1, ode_body_2;
	DYN_tdst_ODE		*pst_ODE;
	float				rot[12];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_Mod_ODE) return;

	if(!_pst_GO1 && !_pst_GO2) return;

	/* We cannot "destroy" a joint. We would have to destroy or empty its joint group. So, we let it and reattach it to nothing */
	if(pst_Mod_ODE->i_ode_joint_id)
		dJointAttach(pst_Mod_ODE->i_ode_joint_id, 0, 0);

	pst_Mod_ODE->i_ode_joint_id = 0;

	pst_World = _pst_GO1 ? WOR_World_GetWorldOfObject(_pst_GO1) : WOR_World_GetWorldOfObject(_pst_GO2);

	ode_body_1 = (pst_Mod_ODE->pst_GO1 && pst_Mod_ODE->pst_GO1->pst_Base && pst_Mod_ODE->pst_GO1->pst_Base->pst_ODE) ? pst_Mod_ODE->pst_GO1->pst_Base->pst_ODE->ode_id_body : 0;
	ode_body_2 = (pst_Mod_ODE->pst_GO2 && pst_Mod_ODE->pst_GO2->pst_Base && pst_Mod_ODE->pst_GO2->pst_Base->pst_ODE) ? pst_Mod_ODE->pst_GO2->pst_Base->pst_ODE->ode_id_body : 0;

	if(ode_body_1 == ode_body_2)
		return;

	if(ode_body_1)
	{
		pst_ODE = pst_Mod_ODE->pst_GO1->pst_Base->pst_ODE;
		ODE_ComputeODEMatrixFromJade(pst_Mod_ODE->pst_GO1->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot); 
		dBodySetPosition(ode_body_1, pst_Mod_ODE->pst_GO1->pst_GlobalMatrix->T.x + pst_ODE->st_Offset.x, pst_Mod_ODE->pst_GO1->pst_GlobalMatrix->T.y + pst_ODE->st_Offset.y, pst_Mod_ODE->pst_GO1->pst_GlobalMatrix->T.z + pst_ODE->st_Offset.z);
		dBodySetRotation(ode_body_1, &rot[0]);
	}

	if(ode_body_2)
	{
		pst_ODE = pst_Mod_ODE->pst_GO2->pst_Base->pst_ODE;
		ODE_ComputeODEMatrixFromJade(pst_Mod_ODE->pst_GO2->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot); 
		dBodySetPosition(ode_body_2, pst_Mod_ODE->pst_GO2->pst_GlobalMatrix->T.x + pst_ODE->st_Offset.x, pst_Mod_ODE->pst_GO2->pst_GlobalMatrix->T.y + pst_ODE->st_Offset.y, pst_Mod_ODE->pst_GO2->pst_GlobalMatrix->T.z + pst_ODE->st_Offset.z);
		dBodySetRotation(ode_body_2, &rot[0]);
	}


	switch(pst_Mod_ODE->uc_Type)
	{
		case ODE_JointTypeBall:
		{
			pst_Mod_ODE->i_ode_joint_id = dJointCreateBall(pst_World->ode_id_world, pst_World->ode_joint_ode);
			dJointAttach(pst_Mod_ODE->i_ode_joint_id, ode_body_1, ode_body_2);
			break;
		}

		case ODE_JointTypeAMotor:
		{
			pst_Mod_ODE->i_ode_joint_id = dJointCreateAMotor(pst_World->ode_id_world, pst_World->ode_joint_ode);
			dJointAttach(pst_Mod_ODE->i_ode_joint_id, ode_body_1, ode_body_2);
			break;
		}

		case ODE_JointTypeHinge:
		{
			pst_Mod_ODE->i_ode_joint_id = dJointCreateHinge(pst_World->ode_id_world, pst_World->ode_joint_ode);
			dJointAttach(pst_Mod_ODE->i_ode_joint_id, ode_body_1, ode_body_2);
			break;
		}

		case ODE_JointTypeFixed:
		{
			pst_Mod_ODE->i_ode_joint_id = dJointCreateFixed(pst_World->ode_id_world, pst_World->ode_joint_ode);
			dJointAttach(pst_Mod_ODE->i_ode_joint_id, ode_body_1, ode_body_2);
#ifdef JADEFUSION
			dJointSetFixed((dxJointFixed*)pst_Mod_ODE->i_ode_joint_id);
#else
			dJointSetFixed(pst_Mod_ODE->i_ode_joint_id);
#endif
			break;
		}

	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ODE_ChangeType(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierODE st_Mod_ODE, *pst_Mod_ODE;
	USHORT				uw_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_Offset = (char *) &st_Mod_ODE.uc_Type - (char *) &st_Mod_ODE;
	pst_Mod_ODE = (GAO_tdst_ModifierODE *) ((char *) p_Data - uw_Offset);

	MDF_ODE_ChangeParam(pst_Mod_ODE, (UCHAR) (*(ULONG *) p_Data), pst_Mod_ODE->pst_GO1, pst_Mod_ODE->pst_GO2);

	if(pst_Mod_ODE->pst_GO)
	{
		LINK_UpdatePointer(pst_Mod_ODE->pst_GO);
		LINK_UpdatePointers();
	}

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ODE_ChangeHiLimit(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierODE st_Mod_ODE, *pst_Mod_ODE;
	USHORT				uw_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_Offset = (char *) &st_Mod_ODE.f_HiLimit - (char *) &st_Mod_ODE;
	pst_Mod_ODE = (GAO_tdst_ModifierODE *) ((char *) p_Data - uw_Offset);

	MDF_ODE_ChangeValues(pst_Mod_ODE, pst_Mod_ODE->uc_Type, pst_Mod_ODE->pst_GO1, pst_Mod_ODE->pst_GO1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ODE_ChangeLoLimit(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierODE st_Mod_ODE, *pst_Mod_ODE;
	USHORT				uw_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_Offset = (char *) &st_Mod_ODE.f_LoLimit - (char *) &st_Mod_ODE;
	pst_Mod_ODE = (GAO_tdst_ModifierODE *) ((char *) p_Data - uw_Offset);

	MDF_ODE_ChangeValues(pst_Mod_ODE, pst_Mod_ODE->uc_Type, pst_Mod_ODE->pst_GO1, pst_Mod_ODE->pst_GO1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ODE_ChangeBounce(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierODE st_Mod_ODE, *pst_Mod_ODE;
	USHORT				uw_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_Offset = (char *) &st_Mod_ODE.f_BounceStop - (char *) &st_Mod_ODE;
	pst_Mod_ODE = (GAO_tdst_ModifierODE *) ((char *) p_Data - uw_Offset);

	MDF_ODE_ChangeValues(pst_Mod_ODE, pst_Mod_ODE->uc_Type, pst_Mod_ODE->pst_GO1, pst_Mod_ODE->pst_GO1);

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ODE_ChangeFriction(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierODE st_Mod_ODE, *pst_Mod_ODE;
	USHORT				uw_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_Offset = (char *) &st_Mod_ODE.f_Friction - (char *) &st_Mod_ODE;
	pst_Mod_ODE = (GAO_tdst_ModifierODE *) ((char *) p_Data - uw_Offset);

	MDF_ODE_ChangeValues(pst_Mod_ODE, pst_Mod_ODE->uc_Type, pst_Mod_ODE->pst_GO1, pst_Mod_ODE->pst_GO1);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ODE_ChangeGO1(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierODE st_Mod_ODE, *pst_Mod_ODE;
	USHORT				uw_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_Offset = (char *) &st_Mod_ODE.pst_GO1 - (char *) &st_Mod_ODE;
	pst_Mod_ODE = (GAO_tdst_ModifierODE *) ((char *) p_Data - uw_Offset);

	MDF_ODE_ChangeParam(pst_Mod_ODE, pst_Mod_ODE->uc_Type, (OBJ_tdst_GameObject *) (* (ULONG *) p_Data), pst_Mod_ODE->pst_GO2);

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ODE_ChangeGO2(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierODE st_Mod_ODE, *pst_Mod_ODE;
	USHORT				uw_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_Offset = (char *) &st_Mod_ODE.pst_GO2 - (char *) &st_Mod_ODE;
	pst_Mod_ODE = (GAO_tdst_ModifierODE *) ((char *) p_Data - uw_Offset);

	MDF_ODE_ChangeParam(pst_Mod_ODE, pst_Mod_ODE->uc_Type, pst_Mod_ODE->pst_GO1, (OBJ_tdst_GameObject *) (* (ULONG *) p_Data));

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_SDW_CallBack_Refresh3DEngineAndReloadTExture(void *, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierSDW	*p_SDW;
	TEX_tdst_Data			*pst_Texture;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_SDW = (GAO_tdst_ModifierSDW *) p_Data;
	p_SDW = (GAO_tdst_ModifierSDW *) ((char *) p_Data - ((char *) &p_SDW->TextureUsed - (char *) p_SDW));

	if(p_SDW->TextureUsed != 0)
	{
		if(p_SDW->TextureUsed != 0xffffffff)
		{
			pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, p_SDW->TextureUsed);
			if(pst_Texture)
				p_SDW->TextureIndex = (LONG) pst_Texture->w_Index;
			else
			{
				p_SDW->TextureIndex = TEX_w_List_AddTexture(&TEX_gst_GlobalList, p_SDW->TextureUsed, 1);
			}
		}
	}

	EDI_OUT_gl_ForceSetMode = 1;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_WATER3D_Modifier_Reset(void *, void *, void *p_Data, LONG)
{
	WATER3D_tdst_Modifier	*p_WATER3D;
	ULONG					*pDetector;

	pDetector = (ULONG *) p_Data;
	while(*pDetector != 0xC0DE2001) pDetector--;

	p_WATER3D = (WATER3D_tdst_Modifier *) pDetector;
	WATER3D_Modifier_Reset(p_WATER3D);

	LINK_UpdatePointers();
	EDI_OUT_gl_ForceSetMode = 1;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}

void MDF_Disturber_CallBack_Refresh(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{	
    EVAV_cl_ViewItem	*po_Item;
    EVAV_cl_ViewItem	*po_Parent;
    MDF_tdst_Modifier   * pst_Mdf;

    po_Item = (EVAV_cl_ViewItem *) _po_Item;

    if(po_Item->mp_Data == p_Data)
    {
        //The display item is the one modified
        po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_MDF_Modifier);
        if (po_Parent)
            pst_Mdf = *(MDF_tdst_Modifier **)(po_Parent->mp_Data);
        else
            return;
    }	 
    else
    {
        return;
    }

    Disturber_tdst_Modifier* p_Disturber = (Disturber_tdst_Modifier*) pst_Mdf->p_Data;
    p_Disturber->bActive = FALSE; // force reset
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Xmen_CallBack_Refresh(void *, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierXMEN	*p_XMEN;
	ULONG					*pDetector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDetector = (ULONG *) p_Data;
	while(*pDetector != 0xC0DE2001) pDetector--;

	p_XMEN = (GAO_tdst_ModifierXMEN *) pDetector;

	if(p_XMEN->ulNumber_Of_Chhlaahhh > p_XMEN->ulOLD_Number_Of_Chhlaahhh)
	{
		if(p_XMEN->p_st_Chhlaahhh)
		{
			p_XMEN->p_st_Chhlaahhh = (GAO_tdst_XMEN_Chhlaahhh *) MEM_p_Realloc
				(
					p_XMEN->p_st_Chhlaahhh,
					sizeof(GAO_tdst_XMEN_Chhlaahhh) * p_XMEN->ulNumber_Of_Chhlaahhh
				);
			p_XMEN->p_stXMEN_MEM = (GAO_tdst_XMEN_Chhlaahhh_Memory *) MEM_p_Realloc
				(
					p_XMEN->p_stXMEN_MEM,
					sizeof(GAO_tdst_XMEN_Chhlaahhh_Memory) * (p_XMEN->ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2)
				);
		}
		else
		{
			p_XMEN->p_st_Chhlaahhh = (GAO_tdst_XMEN_Chhlaahhh *) MEM_p_Alloc(sizeof(GAO_tdst_XMEN_Chhlaahhh) * p_XMEN->ulNumber_Of_Chhlaahhh);
			p_XMEN->p_stXMEN_MEM = (GAO_tdst_XMEN_Chhlaahhh_Memory *) MEM_p_Alloc(sizeof(GAO_tdst_XMEN_Chhlaahhh_Memory) * (p_XMEN->ulNumber_Of_Chhlaahhh << XMEN_NumberOfSegs_PO2));
		}

		memset
		(
			(p_XMEN->p_st_Chhlaahhh + p_XMEN->ulOLD_Number_Of_Chhlaahhh),
			0,
			(p_XMEN->ulNumber_Of_Chhlaahhh - p_XMEN->ulOLD_Number_Of_Chhlaahhh) * sizeof(GAO_tdst_XMEN_Chhlaahhh)
		);
		memset
		(
			(p_XMEN->p_stXMEN_MEM + p_XMEN->ulOLD_Number_Of_Chhlaahhh),
			0,
			(
				(p_XMEN->ulNumber_Of_Chhlaahhh - p_XMEN->ulOLD_Number_Of_Chhlaahhh) * sizeof
					(GAO_tdst_XMEN_Chhlaahhh_Memory)
			) << XMEN_NumberOfSegs_PO2
		);
		p_XMEN->ulOLD_Number_Of_Chhlaahhh = p_XMEN->ulNumber_Of_Chhlaahhh;
	}

	p_XMEN->p_MaterialUsed = NULL;
	if(p_XMEN->bk_MaterialUsed != 0)
	{
		if(p_XMEN->bk_MaterialUsed != 0xffffffff)
		{
			LOA_MakeFileRef
			(
				p_XMEN->bk_MaterialUsed,
				(ULONG *) &p_XMEN->p_MaterialUsed,
				GEO_ul_Load_ObjectCallback,
				LOA_C_MustExists
			);
			LOA_Resolve();
		}
	}

	p_XMEN->ulFlags = 0;
	if(p_XMEN->bGlobalMatrix) p_XMEN->ulFlags |= MOD_XMEN_GlobalMatrix;
	if(p_XMEN->bSmoothed) p_XMEN->ulFlags |= MOD_XMEN_Smooth;
	if(p_XMEN->bMaterialIsTransparent) p_XMEN->ulFlags |= MOD_XMEN_TransMat;
	if(p_XMEN->bCentered) p_XMEN->ulFlags |= MOD_XMEN_Centered;
	if(p_XMEN->bTurn90) p_XMEN->ulFlags |= MOD_XMEN_TurnText90;

	LINK_UpdatePointer(p_XMEN->pst_GO);
	LINK_UpdatePointers();
	EDI_OUT_gl_ForceSetMode = 1;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Xmec_CallBack_Refresh(void *, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierXMEC	*p_XMEC;
	ULONG					*pDetector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDetector = (ULONG *) p_Data;
	while(*pDetector != 0xC0DE2001) pDetector--;

	p_XMEC = (GAO_tdst_ModifierXMEC *) pDetector;

	MDF_Reinit(p_XMEC->pst_GO);
	LINK_UpdatePointer(p_XMEC->pst_GO);
	LINK_UpdatePointers();
	EDI_OUT_gl_ForceSetMode = 1;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}
#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
void MDF_SPG2Holder_CallBack_Reset(void *, void *, void *p_Data, LONG)
{
    void SPG2Holder_Modifier_Prepare();
    SPG2Holder_Modifier_Prepare();
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Lazy_CallBack_Refresh(void *, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierLazy	*p_LAZY;
	ULONG					*pDetector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDetector = (ULONG *) p_Data;
	while(*pDetector != 0xC0DE2001) pDetector--;

	p_LAZY = (GAO_tdst_ModifierLazy *) pDetector;

	if (p_LAZY->LazyFactor < 0.001f) p_LAZY->LazyFactor = 0.001f;
//	if (p_LAZY->LazyFactor > 1.00f) p_LAZY->LazyFactor = 1.00f;

	LINK_UpdatePointers();

	EDI_OUT_gl_ForceSetMode = 1;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_PROTEX_CallBack_Refresh(void *, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PROTEX_tdst_Modifier	*p_PROTEX;
	ULONG					*pDetector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDetector = (ULONG *) p_Data;
	while(*pDetector != 0xC0DE2001) pDetector--;

	p_PROTEX = (PROTEX_tdst_Modifier *) pDetector;

	LINK_UpdatePointers();
	EDI_OUT_gl_ForceSetMode = 1;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_SPG_CallBack_Refresh(void *, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SPG_tdst_Modifier	*p_SPG;
	ULONG				*pDetector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDetector = (ULONG *) p_Data;
	while(*pDetector != 0xC0DE2002) pDetector--;

	p_SPG = (SPG_tdst_Modifier *) pDetector;
	*(ULONG *) &p_SPG->p_Material = (ULONG) - 1;
	if(p_SPG->bk_MaterialID != (ULONG) - 1)
	{
		LOA_MakeFileRef
		(
			p_SPG->bk_MaterialID,
			(ULONG *) &p_SPG->p_Material,
			GEO_ul_Load_ObjectCallback,
			LOA_C_MustExists
		);
		LOA_Resolve();
	}

	EDI_OUT_gl_ForceSetMode = 1;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}

ULONG MDF_SPG2_CallBack_GetColorOfThisOne(int pst_GO,int pmod)
{
	static int Counter = 0;
	static int pst_GOCurrent = 0;
	static int pmodCurrent = 0;
	ULONG Color;
	if (pst_GOCurrent == pst_GO)
	{
		Counter ++;
		if (pmodCurrent == pmod)
			Counter = 0;
	}
	else
	{
		pmodCurrent = pmod;
		Counter = 0;
	}

	pst_GOCurrent = pst_GO;

	Color = 0xc0c0c0;
	if (Counter & 1) Color |= 0xf0;
	if (Counter & 2) Color |= 0xf000;
	if (Counter & 4) Color |= 0xf00000;

	return Color;
}

void MDF_SPG2_CallBack_Refresh(void *, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SPG2_tdst_Modifier	*p_SPG;
	ULONG				*pDetector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pDetector = (ULONG *) p_Data;
	while(*pDetector != 0xC0DE2002) pDetector--;
	p_SPG = (SPG2_tdst_Modifier *) pDetector;
	/* Retreive Big key from GO Ptr */
	if (p_SPG->GameObjectToDuplicate)
		p_SPG->GameObjectKey = LOA_ul_SearchKeyWithAddress((ULONG)p_SPG->GameObjectToDuplicate);
	p_SPG->ulMustbeRecompute = 1;
	EDI_OUT_gl_ForceSetMode = 1;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}

void MDF_Sfx_CallBack_Refresh(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
    EVAV_cl_ViewItem	*po_Item;
    EVAV_cl_ViewItem	*po_Parent;
    MDF_tdst_Modifier   * pst_Mdf;

    po_Item = (EVAV_cl_ViewItem *) _po_Item;

    if(po_Item->mp_Data == p_Data)
    {
        //The display item is the one modified
        po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_MDF_Modifier);
        if (po_Parent)
            pst_Mdf = *(MDF_tdst_Modifier **)(po_Parent->mp_Data);
        else
            return;
    }	 
    else
    {
        return;
    }

    GAO_tdst_ModifierSfx* p_Sfx = (GAO_tdst_ModifierSfx*) pst_Mdf->p_Data;

    if (p_Sfx)
    {		
        switch (p_Sfx->type)
        {
        case MDF_SFX_HotAir:
        case MDF_SFX_RimLight:
            pst_Mdf->ul_Flags |= MDF_C_Modifier_ApplyGao;
            break;
        }

        LINK_UpdatePointers();
        EDI_OUT_gl_ForceSetMode = 1;
        LINK_Refresh();
        EDI_OUT_gl_ForceSetMode = 0;

        LINK_UpdatePointer( pst_Mdf->pst_GO );
        LINK_UpdatePointers();
    }
}

#ifdef JADEFUSION
void MDF_SoftBody_CallBack_NbPlanes(void *p_Owner, void *_po_Item, void *p_Data, long l_OldData)
{
    GAO_tdst_ModifierSoftBody * p_SoftBody;
    ULONG					  * pDetector;

    pDetector = (ULONG *) p_Data;
    while(*pDetector != 0xC0DE2001) pDetector--;

    p_SoftBody = (GAO_tdst_ModifierSoftBody *) pDetector;

    GAO_ModifierSoftBody_ResizePlanes(p_SoftBody, (ULONG)l_OldData);

    LINK_UpdatePointer(p_SoftBody->pst_Hook);
    LINK_UpdatePointers();
}

void EVAV_cl_ListBox::OBJAddSoftBodyListRods(POSITION pos, void *_pst_Data)
{
    EVAV_cl_ViewItem	      * po_Item, *po_NewItem;
    GAO_tdst_ModifierSoftBody * p_SoftBody;
    CHAR                        az[64];

    /* Get the pointer of instance */
    if(!pos)
        po_Item = mpo_ListItems->GetTail();
    else
        po_Item = mpo_ListItems->GetAt(pos);

    p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Data;

    for (ULONG i=0; i<p_SoftBody->ul_NbRods; i++)
    {
        sprintf(az, "Rod %d", i);
        M_InsertItemROColor(az, 0, NULL );
        M_InsertItem("Bone 1", EVAV_EVVIT_Int, &p_SoftBody->a_Rods[i].m_v1, NULL);
        M_InsertItem("Bone 2", EVAV_EVVIT_Int, &p_SoftBody->a_Rods[i].m_v2, NULL);
        M_InsertItem1("Length", EVAV_EVVIT_Float, &p_SoftBody->a_Rods[i].m_fLength, 0, NULL);
        po_Item->mi_NumFields += 4;
    }
}

void EVAV_cl_ListBox::OBJAddSoftBodyListVertices(POSITION pos, void *_pst_Data)
{
    EVAV_cl_ViewItem	      * po_Item, *po_NewItem;
    GAO_tdst_ModifierSoftBody * p_SoftBody;
    CHAR                        az[64];

    // Get the pointer of instance 
    if(!pos)
        po_Item = mpo_ListItems->GetTail();
    else
        po_Item = mpo_ListItems->GetAt(pos);

    p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Data;

    for (ULONG i=0; i<p_SoftBody->ul_NbVertices; i++)
    {
        sprintf(az, "Vertex %d", i);
        M_InsertItemROColor(az, 0, NULL);
        M_InsertItem12("Fixe", EVAV_EVVIT_Flags, &p_SoftBody->a_Vertices[i].m_ulFlags, 0, 4, NULL);
        M_InsertItem("Bone Id", EVAV_EVVIT_Int, &p_SoftBody->a_Vertices[i].m_ulBoneId, NULL);
        M_InsertItem1("Coord u", EVAV_EVVIT_Float, &p_SoftBody->a_Vertices[i].m_u, 0, NULL);
        M_InsertItem1("Coord v", EVAV_EVVIT_Float, &p_SoftBody->a_Vertices[i].m_v, 0, NULL);
        po_Item->mi_NumFields += 5;
    }	
}

void EVAV_cl_ListBox::OBJAddSoftBodyListPlanes(POSITION pos, void *_pst_Data)
{
    EVAV_cl_ViewItem	      * po_Item, *po_NewItem;
    GAO_tdst_ModifierSoftBody * p_SoftBody;
    CHAR                        az[64];

    // Get the pointer of instance 
    if(!pos)
        po_Item = mpo_ListItems->GetTail();
    else
        po_Item = mpo_ListItems->GetAt(pos);

    p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Data;

    for (ULONG i=0; i<p_SoftBody->ul_NbCollPlane; i++)
    {
        sprintf(az, "Plane %d", i);
        M_InsertItemROColor(az, 0, NULL);
        M_InsertItem1("Plane", EVAV_EVVIT_SubStruct, &p_SoftBody->a_CollPlane[i], LINK_C_MDF_SoftBodyColPlane, NULL);
        po_Item->mi_NumFields += 2;
    }
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_CallBack_ChangeType(void* _p_Owner, void* _p_Item, void* _p_Data, LONG _l_OldData)
{
    ULONG ul_NewFlags = *(ULONG*)_p_Data;

    if ((ul_NewFlags & LIGHT_Cul_LF_Type) != ((ULONG)_l_OldData & LIGHT_Cul_LF_Type))
    {
        LIGHT_tdst_Light  stTempLight;
        ULONG             ulOffset  = (ULONG)&stTempLight.ul_Flags - (ULONG)&stTempLight;
        LIGHT_tdst_Light* pst_Light = (LIGHT_tdst_Light*)(((UCHAR*)_p_Data) - ulOffset);

        if ((ul_NewFlags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_LightShaft)
        {
            LIGHT_InitLightShaft(&pst_Light->st_LightShaft);

#if defined(_XENON_RENDER)
            g_oXeLightShaftManager.RegisterLightShaft(pst_Light);
        }
        else
        {
            g_oXeLightShaftManager.UnregisterLightShaft(pst_Light);
#endif
        }
    }

    LINK_CallBack_Refresh3DEngineAndPointers(_p_Owner, _p_Item, _p_Data, _l_OldData);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void LIGHT_CallBack_ChangeLightCookieTexture(void* _p_Owner, void* _p_Item, void* _p_Data, LONG _l_OldData)
{
    LIGHT_tdst_Light  stTempLight;
    LIGHT_tdst_Light* pst_Light;
    ULONG ulOffset = (ULONG)&stTempLight.ul_CookieTextureKey - (ULONG)&stTempLight;

    pst_Light = (LIGHT_tdst_Light*)((UCHAR*)_p_Data - ulOffset);

    pst_Light->us_CookieTexture = -1;

    if (pst_Light->ul_CookieTextureKey != BIG_C_InvalidKey)
    {
        TEX_tdst_Data* pst_Texture;

        pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_Light->ul_CookieTextureKey);
        if (pst_Texture != NULL)
        {
            pst_Light->us_CookieTexture = pst_Texture->w_Index;
        }
        else
        {
            pst_Light->us_CookieTexture = TEX_w_List_AddTexture(&TEX_gst_GlobalList, 
                pst_Light->ul_CookieTextureKey, 1);
        }

        if (pst_Light->us_CookieTexture == -1)
        {
            pst_Light->ul_CookieTextureKey = BIG_C_InvalidKey;

            LINK_CallBack_Refresh3DEngineAndPointers(_p_Owner, _p_Item, _p_Data, _l_OldData);
        }
        else
        {
#if defined(_XENON_RENDER)
            if (GDI_b_IsXenonGraphics())
            {
                if (pst_Texture == NULL)
                {
                    GDI_Xe_ForceLoadLastTexture();
                }
            }
            else
            {
                EDI_OUT_gl_ForceSetMode = 1;
            }
#else
            EDI_OUT_gl_ForceSetMode = 1;
#endif
            LINK_Refresh();
            EDI_OUT_gl_ForceSetMode = 0;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_CallBack_ChangeLightShaftCookieTexture(void* _p_Owner, void* _p_Item, void* _p_Data, LONG _l_OldData)
{
    LIGHT_tdst_Light  stTempLight;
    LIGHT_tdst_Light* pst_Light;
    ULONG ulOffset = (ULONG)&stTempLight.st_LightShaft.ul_CookieTextureKey - (ULONG)&stTempLight;

    pst_Light = (LIGHT_tdst_Light*)((UCHAR*)_p_Data - ulOffset);

    pst_Light->st_LightShaft.us_CookieTexture = -1;

    if (pst_Light->st_LightShaft.ul_CookieTextureKey != BIG_C_InvalidKey)
    {
        TEX_tdst_Data* pst_Texture;

        pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_Light->st_LightShaft.ul_CookieTextureKey);
        if (pst_Texture != NULL)
        {
            pst_Light->st_LightShaft.us_CookieTexture = pst_Texture->w_Index;
        }
        else
        {
            pst_Light->st_LightShaft.us_CookieTexture = TEX_w_List_AddTexture(&TEX_gst_GlobalList, 
                                                                              pst_Light->st_LightShaft.ul_CookieTextureKey, 1);
        }

        if (pst_Light->st_LightShaft.us_CookieTexture == -1)
        {
            pst_Light->st_LightShaft.ul_CookieTextureKey = BIG_C_InvalidKey;

            LINK_CallBack_Refresh3DEngineAndPointers(_p_Owner, _p_Item, _p_Data, _l_OldData);
        }
        else
        {
#if defined(_XENON_RENDER)
            if (GDI_b_IsXenonGraphics())
            {
                if (pst_Texture == NULL)
                {
                    GDI_Xe_ForceLoadLastTexture();
                }
            }
            else
            {
                EDI_OUT_gl_ForceSetMode = 1;
            }
#else
            EDI_OUT_gl_ForceSetMode = 1;
#endif
            LINK_Refresh();
            EDI_OUT_gl_ForceSetMode = 0;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LIGHT_CallBack_ChangeLightShaftNoiseTexture(void* _p_Owner, void* _p_Item, void* _p_Data, LONG _l_OldData)
{
    LIGHT_tdst_Light  stTempLight;
    LIGHT_tdst_Light* pst_Light;
    ULONG ulOffset = (ULONG)&stTempLight.st_LightShaft.ul_NoiseTextureKey - (ULONG)&stTempLight;

    pst_Light = (LIGHT_tdst_Light*)((UCHAR*)_p_Data - ulOffset);

    pst_Light->st_LightShaft.us_NoiseTexture = -1;

    if (pst_Light->st_LightShaft.ul_NoiseTextureKey != BIG_C_InvalidKey)
    {
        TEX_tdst_Data* pst_Texture;

        pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_Light->st_LightShaft.ul_NoiseTextureKey);
        if (pst_Texture != NULL)
        {
            pst_Light->st_LightShaft.us_NoiseTexture = pst_Texture->w_Index;
        }
        else
        {
            pst_Light->st_LightShaft.us_NoiseTexture = TEX_w_List_AddTexture(&TEX_gst_GlobalList, 
                                                                             pst_Light->st_LightShaft.ul_NoiseTextureKey, 1);
        }

        if (pst_Light->st_LightShaft.us_NoiseTexture == -1)
        {
            pst_Light->st_LightShaft.ul_NoiseTextureKey = BIG_C_InvalidKey;

            LINK_CallBack_Refresh3DEngineAndPointers(_p_Owner, _p_Item, _p_Data, _l_OldData);
        }
        else
        {
#if defined(_XENON_RENDER)
            if (GDI_b_IsXenonGraphics())
            {
                if (pst_Texture == NULL)
                {
                    GDI_Xe_ForceLoadLastTexture();
                }
            }
            else
            {
                EDI_OUT_gl_ForceSetMode = 1;
            }
#else
            EDI_OUT_gl_ForceSetMode = 1;
#endif
            LINK_Refresh();
            EDI_OUT_gl_ForceSetMode = 0;
        }
    }
}

void LIGHT_CallBack_ValidateNear(void* _p_Owner, void* _p_Item, void* _p_Data, LONG _l_OldData)
{
	LIGHT_tdst_Light  stTempLight;
	LIGHT_tdst_Light* pst_Light;
	ULONG ulOffset = (ULONG)&stTempLight.st_Spot.f_Near - (ULONG)&stTempLight;

	pst_Light = (LIGHT_tdst_Light*)((UCHAR*)_p_Data - ulOffset);

    FLOAT fBigRadius    = pst_Light->st_Spot.f_Far * fTan(pst_Light->st_Spot.f_BigAlpha);
    FLOAT fLittleRadius = *((FLOAT*)&_l_OldData) * fTan(pst_Light->st_Spot.f_LittleAlpha);
	FLOAT fShadowRadius = pst_Light->st_Spot.f_Far * fTan(pst_Light->f_HiResFOV);

    FLOAT* pf_NewNear  = (FLOAT*)_p_Data;

	if(*pf_NewNear > pst_Light->st_Spot.f_Far)
		pst_Light->st_Spot.f_Far = *pf_NewNear;

    if (pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
    {
        // adjust angles to keep same radius
        pst_Light->st_Spot.f_BigAlpha = fAtan(fBigRadius / pst_Light->st_Spot.f_Far);
        pst_Light->st_Spot.f_LittleAlpha = fAtan(fLittleRadius / pst_Light->st_Spot.f_Near);
		pst_Light->f_HiResFOV = fAtan(fShadowRadius / pst_Light->st_Spot.f_Far);
    }

    LINK_CallBack_Refresh3DEngineAndPointers(_p_Owner, _p_Item, _p_Data, _l_OldData);
}

void LIGHT_CallBack_ValidateFar(void* _p_Owner, void* _p_Item, void* _p_Data, LONG _l_OldData)
{
	LIGHT_tdst_Light  stTempLight;
	LIGHT_tdst_Light* pst_Light;
	ULONG ulOffset = (ULONG)&stTempLight.st_Spot.f_Far - (ULONG)&stTempLight;

	pst_Light = (LIGHT_tdst_Light*)((UCHAR*)_p_Data - ulOffset);

    FLOAT fBigRadius    = *((FLOAT*)&_l_OldData) * fTan(pst_Light->st_Spot.f_BigAlpha);
    FLOAT fLittleRadius = pst_Light->st_Spot.f_Near * fTan(pst_Light->st_Spot.f_LittleAlpha);
	FLOAT fShadowRadius = *((FLOAT*)&_l_OldData) * fTan(pst_Light->f_HiResFOV);

    FLOAT* pf_NewFar  = (FLOAT*)_p_Data;

	if(*pf_NewFar < pst_Light->st_Spot.f_Near)
		pst_Light->st_Spot.f_Near = *pf_NewFar;

	if(*pf_NewFar < pst_Light->f_ShadowNear)
		pst_Light->f_ShadowNear = *pf_NewFar - Cf_Epsilon;

    if (pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
    {
        // adjust angles to keep same radius
        pst_Light->st_Spot.f_BigAlpha = fAtan(fBigRadius / pst_Light->st_Spot.f_Far);
        pst_Light->st_Spot.f_LittleAlpha = fAtan(fLittleRadius / pst_Light->st_Spot.f_Near);
		pst_Light->f_HiResFOV = fAtan(fShadowRadius / pst_Light->st_Spot.f_Far);
    }

    LINK_CallBack_Refresh3DEngineAndPointers(_p_Owner, _p_Item, _p_Data, _l_OldData);
}

void LIGHT_CallBack_ValidateLittleAlpha(void* _p_Owner, void* _p_Item, void* _p_Data, LONG _l_OldData)
{
	LIGHT_tdst_Light  stTempLight;
	LIGHT_tdst_Light* pst_Light;
	ULONG ulOffset = (ULONG)&stTempLight.st_Spot.f_LittleAlpha - (ULONG)&stTempLight;

	pst_Light = (LIGHT_tdst_Light*)((UCHAR*)_p_Data - ulOffset);

	FLOAT *pf_NewLittleAlpha = (FLOAT*)_p_Data;

    if(*pf_NewLittleAlpha <= 0.000001f)
        *pf_NewLittleAlpha = 0.000001f;

    if(*pf_NewLittleAlpha > Cf_PiBy2 - Cf_EpsilonBig)
        *pf_NewLittleAlpha = Cf_PiBy2 - Cf_EpsilonBig;

    if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
	{
        FLOAT fBigRadius = pst_Light->st_Spot.f_Far * fTan(pst_Light->st_Spot.f_BigAlpha);
		FLOAT fLittleRadius = pst_Light->st_Spot.f_Near * fTan(*pf_NewLittleAlpha);

		// clamp big radius to little radius
		if(fLittleRadius > fBigRadius)
			pst_Light->st_Spot.f_BigAlpha = fAtan((fLittleRadius + Cf_EpsilonBig) / pst_Light->st_Spot.f_Far);

	}
	else
	{
		if(*pf_NewLittleAlpha > pst_Light->st_Spot.f_BigAlpha)
			pst_Light->st_Spot.f_BigAlpha = *pf_NewLittleAlpha;
	}
	
	LINK_CallBack_Refresh3DEngineAndPointers(_p_Owner, _p_Item, _p_Data, _l_OldData);
}

void LIGHT_CallBack_ValidateBigAlpha(void* _p_Owner, void* _p_Item, void* _p_Data, LONG _l_OldData)
{
	LIGHT_tdst_Light  stTempLight;
	LIGHT_tdst_Light* pst_Light;
	ULONG ulOffset = (ULONG)&stTempLight.st_Spot.f_BigAlpha - (ULONG)&stTempLight;

	pst_Light = (LIGHT_tdst_Light*)((UCHAR*)_p_Data - ulOffset);

	FLOAT* pf_NewBigAlpha  = (FLOAT*)_p_Data;

    if(*pf_NewBigAlpha <= 0.000001f)
        *pf_NewBigAlpha = 0.000001f;

    if(*pf_NewBigAlpha > Cf_PiBy2 - Cf_EpsilonBig)
        *pf_NewBigAlpha = Cf_PiBy2 - Cf_EpsilonBig;

    if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
	{
        FLOAT fBigRadius = pst_Light->st_Spot.f_Far * fTan(*pf_NewBigAlpha);
		FLOAT fLittleRadius = pst_Light->st_Spot.f_Near * fTan(pst_Light->st_Spot.f_LittleAlpha);
		FLOAT fShadowRadius = pst_Light->st_Spot.f_Far * fTan(pst_Light->f_HiResFOV);

		// clamp little radius to big radius
		if(fBigRadius < fLittleRadius)
		{
			pst_Light->st_Spot.f_LittleAlpha = fAtan((fBigRadius - Cf_EpsilonBig) / pst_Light->st_Spot.f_Near);
		}

		if(fBigRadius < fShadowRadius)
		{
			pst_Light->f_HiResFOV = *pf_NewBigAlpha;
		}
	}
	else
	{
		if(*pf_NewBigAlpha < pst_Light->st_Spot.f_LittleAlpha)
		{
			pst_Light->st_Spot.f_LittleAlpha = *pf_NewBigAlpha;
		}

		if(*pf_NewBigAlpha < pst_Light->f_HiResFOV)
		{
			pst_Light->f_HiResFOV = *pf_NewBigAlpha;
		}	
	}

    LINK_CallBack_Refresh3DEngineAndPointers(_p_Owner, _p_Item, _p_Data, _l_OldData);
}

void LIGHT_CallBack_ValidateShadowAlpha(void* _p_Owner, void* _p_Item, void* _p_Data, LONG _l_OldData)
{	
	LIGHT_tdst_Light  stTempLight;
	LIGHT_tdst_Light* pst_Light;
	ULONG ulOffset = (ULONG)&stTempLight.f_HiResFOV - (ULONG)&stTempLight;

	pst_Light = (LIGHT_tdst_Light*)((UCHAR*)_p_Data - ulOffset);

	//update the Max value for this variable with the Big Alpha
	//the max value specified with the InsertItem fct is static
	*(float*)(&((EVAV_cl_ViewItem* )_p_Item)->mi_Param2) = pst_Light->st_Spot.f_BigAlpha;

	FLOAT* pf_NewShadowAlpha  = (FLOAT*)_p_Data;

	if(*pf_NewShadowAlpha <= 0.000001f)
		*pf_NewShadowAlpha = 0.000001f;

	if(*pf_NewShadowAlpha > Cf_PiBy2 - Cf_EpsilonBig)
		*pf_NewShadowAlpha = Cf_PiBy2 - Cf_EpsilonBig;

	if(pst_Light->ul_Flags & LIGHT_Cul_LF_SpotIsCylindrical)
	{
		FLOAT fShadowRadius = pst_Light->st_Spot.f_Far * fTan(*pf_NewShadowAlpha);
		FLOAT fBigRadius = pst_Light->st_Spot.f_Far * fTan(pst_Light->st_Spot.f_BigAlpha);

		// clamp shadow radius to big radius
		if(fShadowRadius > fBigRadius)
			pst_Light->f_HiResFOV = fAtan((fBigRadius - Cf_EpsilonBig) / pst_Light->st_Spot.f_Far);
	}
	else
	{
		if(*pf_NewShadowAlpha > pst_Light->st_Spot.f_BigAlpha)
			pst_Light->f_HiResFOV = pst_Light->st_Spot.f_BigAlpha;
	}

	LINK_CallBack_Refresh3DEngineAndPointers(_p_Owner, _p_Item, _p_Data, _l_OldData);
}

void LIGHT_CallBack_ValidateShadowNear(void* _p_Owner, void* _p_Item, void* _p_Data, LONG _l_OldData)
{
	LIGHT_tdst_Light  stTempLight;
	LIGHT_tdst_Light* pst_Light;
	ULONG ulOffset = (ULONG)&stTempLight.f_ShadowNear - (ULONG)&stTempLight;

	pst_Light = (LIGHT_tdst_Light*)((UCHAR*)_p_Data - ulOffset);

	//update the Max value for this variable with the Far value
	//the max value specified with the InsertItem fct is static
	*(float*)(&((EVAV_cl_ViewItem* )_p_Item)->mi_Param2) = pst_Light->st_Spot.f_Far;

	FLOAT* pf_NewShadowNear  = (FLOAT*)_p_Data;

	if(*pf_NewShadowNear > pst_Light->st_Spot.f_Far)
		pst_Light->f_ShadowNear = pst_Light->st_Spot.f_Far;

	LINK_CallBack_Refresh3DEngineAndPointers(_p_Owner, _p_Item, _p_Data, _l_OldData);
}
#endif
void MDF_FOGDY_CallBack_Refresh(void *, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierFOGDY	*p_FOGDY;
	ULONG				*pDetector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pDetector = (ULONG *) p_Data;
	while(*pDetector != 0xC0DE2002) pDetector--;
	p_FOGDY = (GAO_tdst_ModifierFOGDY *) pDetector;
	/* Retreive Big key from GO Ptr */
	p_FOGDY->IsInit = 12;
	EDI_OUT_gl_ForceSetMode = 1;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}

void MDF_FOGDY_2_CallBack_Refresh(void *p_Owner, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierFOGDY	*p_FOGDY;
	ULONG				*pDetector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pDetector = (ULONG *) p_Data;
	while(*pDetector != 0xC0DE2002) pDetector--;
	p_FOGDY = (GAO_tdst_ModifierFOGDY *) pDetector;
	LINK_UpdatePointer(p_FOGDY->p_GO);
	LINK_UpdatePointers();
}

void MDF_FOGDY_EMTR_CallBack_Refresh(void *, void *, void *p_Data, LONG)
{
	LINK_Refresh();
}

void MDF_BONEMECA_EMTR_CallBack_Refresh(void *, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierBoneMeca	*p_BoneMec;
	ULONG						*pDetector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pDetector = (ULONG *) p_Data;
	while(*pDetector != 0xC0DE2002) pDetector--;
	p_BoneMec = (GAO_tdst_ModifierBoneMeca *) pDetector;
	p_BoneMec ->ulNumberOfExcluders = lMin(p_BoneMec ->ulNumberOfExcluders,MAX_BONEMECA_EXCLUDERS);
	p_BoneMec ->ulNumberOfMovers	= lMin(p_BoneMec ->ulNumberOfMovers,MAX_BONEMECA_MOVERS);
	LINK_UpdatePointer(p_BoneMec->_pst_GO);
	LINK_UpdatePointers();
}
#ifdef JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Wind_CallBack_Refresh(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
    EVAV_cl_ViewItem	*po_Item;
    EVAV_cl_ViewItem	*po_Parent;
    MDF_tdst_Modifier   *pst_Mdf;

    po_Item = (EVAV_cl_ViewItem *) _po_Item;

    if(po_Item->mp_Data == p_Data)
    {
        //The display item is the one modified
        po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_MDF_Modifier);
        pst_Mdf = *(MDF_tdst_Modifier **)(po_Parent->mp_Data);
    }
    else
        return;

    if (pst_Mdf->i->ul_Type == MDF_C_Modifier_Wind)
    {
        GAO_ModifierWind_Refresh(pst_Mdf);
    }

    LINK_UpdatePointers();
    EDI_OUT_gl_ForceSetMode = 1;
    LINK_Refresh();
    EDI_OUT_gl_ForceSetMode = 0;

    LINK_UpdatePointer( pst_Mdf->pst_GO );
    LINK_UpdatePointers();	
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OBJAddBoundingVolume(POSITION pos, void *_pst_BV)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item, *po_NewItem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get the pointer of instance */
	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);

	pst_GO = ((OBJ_tdst_SingleBV *) _pst_BV)->pst_GO;

	if(OBJ_BV_IsAABBox(_pst_BV))
	{
		if(OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_EditableBV))
		{
			M_InsertItem
			(
				"AABBox Min",
				EVAV_EVVIT_Vector,
				OBJ_pst_BV_GetGMinInit(_pst_BV),
				LINK_CallBack_Refresh3DEngine
			);
			M_InsertItem
			(
				"AABBox Max",
				EVAV_EVVIT_Vector,
				OBJ_pst_BV_GetGMaxInit(_pst_BV),
				LINK_CallBack_Refresh3DEngine
			);
		}
		else
		{
			M_InsertItemRO
			(
				"AABBox Min",
				EVAV_EVVIT_Vector,
				OBJ_pst_BV_GetGMinInit(_pst_BV),
				LINK_CallBack_Refresh3DEngine
			);
			M_InsertItemRO
			(
				"AABBox Max",
				EVAV_EVVIT_Vector,
				OBJ_pst_BV_GetGMaxInit(_pst_BV),
				LINK_CallBack_Refresh3DEngine
			);
		}
	}
	else
	{
		if(OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_EditableBV))
		{
			M_InsertItem("Center", EVAV_EVVIT_Vector, OBJ_pst_BV_GetCenter(_pst_BV), LINK_CallBack_Refresh3DEngine);
			M_InsertItem
			(
				"Radius",
				EVAV_EVVIT_Float,
				&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax.x,
				LINK_CallBack_Refresh3DEngine
			);
		}
		else
		{
			M_InsertItemRO("Center", EVAV_EVVIT_Vector, OBJ_pst_BV_GetCenter(_pst_BV), LINK_CallBack_Refresh3DEngine);
			M_InsertItemRO
			(
				"Radius",
				EVAV_EVVIT_Float,
				&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax.x,
				LINK_CallBack_Refresh3DEngine
			);
		}
	}

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_OBBox))
	{
		if(OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_EditableBV))
		{
			M_InsertItem("OBBox Min", EVAV_EVVIT_Vector, OBJ_pst_BV_GetLMin(_pst_BV), LINK_CallBack_Refresh3DEngine);
			M_InsertItem("OBBox Max", EVAV_EVVIT_Vector, OBJ_pst_BV_GetLMax(_pst_BV), LINK_CallBack_Refresh3DEngine);
		}
		else
		{
			M_InsertItemRO("OBBox Min", EVAV_EVVIT_Vector, OBJ_pst_BV_GetLMin(_pst_BV), LINK_CallBack_Refresh3DEngine);
			M_InsertItemRO("OBBox Max", EVAV_EVVIT_Vector, OBJ_pst_BV_GetLMax(_pst_BV), LINK_CallBack_Refresh3DEngine);
		}

		/* Set number of fields of the initial pointer */
		po_Item->mi_NumFields = 4;
	}
	else
		/* Set number of fields of the initial pointer */
		po_Item->mi_NumFields = 2;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OBJAddZone(POSITION pos, void *_pst_ZDx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item, *po_NewItem;
	COL_tdst_ZDx		*pst_ZDx;
	COL_tdst_Sphere		*pst_Sphere;
	COL_tdst_Box		*pst_Box;
	ULONG				ul_Flag;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get the pointer of instance */
	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);

	pst_ZDx = (COL_tdst_ZDx *) _pst_ZDx;
	ul_Flag = pst_ZDx->pst_GO->pst_World->pst_Selection->l_Flag;

	switch(pst_ZDx->uc_Type)
	{
	case COL_C_Zone_Sphere:
		pst_Sphere = (COL_tdst_Sphere *) (pst_ZDx->p_Shape);
		M_InsertItem("Center", EVAV_EVVIT_Vector, COL_pst_Shape_GetCenter(pst_Sphere), LINK_CallBack_Refresh3DEngine);
		M_InsertItem("Radius", EVAV_EVVIT_Float, &pst_Sphere->f_Radius, LINK_CallBack_Refresh3DEngine);
		break;

	case COL_C_Zone_Box:
		pst_Box = (COL_tdst_Box *) (pst_ZDx->p_Shape);
		M_InsertItem("Min", EVAV_EVVIT_Vector, COL_pst_Shape_GetMin(pst_Box), LINK_CallBack_Refresh3DEngine);
		M_InsertItem("Max", EVAV_EVVIT_Vector, COL_pst_Shape_GetMax(pst_Box), LINK_CallBack_Refresh3DEngine);
		break;
	}

	/* Set number of fields of the initial pointer */
	po_Item->mi_NumFields = 2;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OBJAddCob(POSITION pos, void *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem					*po_Item, *po_NewItem;
	COL_tdst_GameMat					*pst_GMat;
	COL_tdst_Cob						*pst_Cob;
	COL_tdst_ElementIndexedTriangles	*pst_Element;
	COL_tdst_Sphere						*pst_Sphere;
	COL_tdst_Cylinder					*pst_Cyl;
	COL_tdst_Box						*pst_Box;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);

	pst_Cob = (COL_tdst_Cob *) _pst_Cob;

	po_Item->mi_NumFields = 0;

	switch(pst_Cob->uc_Type)
	{
	case COL_C_Zone_Sphere:
		if(pst_Cob->pst_GMatList)
		{
			M_InsertItem1COLOR
			(
				"Game Material ID",
				EVAV_EVVIT_Int,
				&pst_Cob->pst_MathCob->l_MaterialId,
				0,
				LINK_CallBack_ChangeCobID,
				0x00E6E6E6
			);
			po_Item->mi_NumFields += 1;
		}
		pst_Sphere = (COL_tdst_Sphere *) (pst_Cob->pst_MathCob->p_Shape);
		M_InsertItem("Center", EVAV_EVVIT_Vector, COL_pst_Shape_GetCenter(pst_Sphere), LINK_CallBack_Refresh3DEngine);
		M_InsertItem("Radius", EVAV_EVVIT_Float, &pst_Sphere->f_Radius, LINK_CallBack_Refresh3DEngine);

		/* Set number of fields of the initial pointer */
		po_Item->mi_NumFields += 2;

		break;

	case COL_C_Zone_Box:
		if(pst_Cob->pst_GMatList)
		{
			M_InsertItem1COLOR
			(
				"Game Material ID",
				EVAV_EVVIT_Int,
				&pst_Cob->pst_MathCob->l_MaterialId,
				0,
				LINK_CallBack_ChangeCobID,
				0x00E6E6E6
			);
			po_Item->mi_NumFields += 1;
		}
		pst_Box = (COL_tdst_Box *) (pst_Cob->pst_MathCob->p_Shape);
		M_InsertItem("Min", EVAV_EVVIT_Vector, COL_pst_Shape_GetMin(pst_Box), LINK_CallBack_Refresh3DEngine);
		M_InsertItem("Max", EVAV_EVVIT_Vector, COL_pst_Shape_GetMax(pst_Box), LINK_CallBack_Refresh3DEngine);

		/* Set number of fields of the initial pointer */
		po_Item->mi_NumFields += 2;

		break;

	case COL_C_Zone_Cylinder:
		if(pst_Cob->pst_GMatList)
		{
			M_InsertItem1COLOR
			(
				"Game Material ID",
				EVAV_EVVIT_Int,
				&pst_Cob->pst_MathCob->l_MaterialId,
				0,
				LINK_CallBack_ChangeCobID,
				0x00E6E6E6
			);
			po_Item->mi_NumFields += 1;
		}
		pst_Cyl = (COL_tdst_Cylinder *) (pst_Cob->pst_MathCob->p_Shape);
		M_InsertItem("Center", EVAV_EVVIT_Vector, COL_pst_Shape_GetCenter(pst_Cyl), LINK_CallBack_Refresh3DEngine);
		M_InsertItem("Radius", EVAV_EVVIT_Float, &pst_Cyl->f_Radius, LINK_CallBack_Refresh3DEngine);
		M_InsertItem("Height", EVAV_EVVIT_Float, &pst_Cyl->f_Height, LINK_CallBack_Refresh3DEngine);

		/* Set number of fields of the initial pointer */
		po_Item->mi_NumFields += 3;

		break;
	}

	switch(pst_Cob->uc_Type)
	{
	case COL_C_Zone_Sphere:
	case COL_C_Zone_Box:
	case COL_C_Zone_Cylinder:
		pst_GMat = COL_pst_GMat_Get(pst_Cob, NULL);
		M_InsertItem1("Cob Flags", EVAV_EVVIT_SubStruct, &pst_Cob->uc_Flag, LINK_C_CobFlags, NULL);
		M_InsertItem1("Game Material", EVAV_EVVIT_Pointer, pst_GMat, LINK_C_GMStruct, NULL);
		po_Item->mi_NumFields += 2;
		break;
	case COL_C_Zone_Triangles:
		if(pst_Cob->uc_Flag & COL_C_Cob_Updated) COL_SynchronizeCob(pst_Cob, FALSE,FALSE);

		pst_Element = &pst_Cob->pst_TriangleCob->dst_Element[pst_Cob->ul_EditedElement];
		pst_GMat = COL_pst_GMat_Get(pst_Cob, pst_Element);

		M_InsertItemRO("Total Triangles", EVAV_EVVIT_Int, &pst_Cob->pst_TriangleCob->l_NbFaces, NULL);
		M_InsertItem123("Camera Triangles", EVAV_EVVIT_Int, &pst_Cob->pst_TriangleCob->uw_NumCameraFaces, 0, 0, 2, NULL);
		M_InsertItem1COLOR("Game Material ID", EVAV_EVVIT_Int, &pst_Element->l_MaterialId, 0, LINK_CallBack_ChangeElementID, 0x00E6E6E6);
		M_InsertItem1("Selected Element", EVAV_EVVIT_Int, &pst_Cob->ul_EditedElement, 0, NULL);
		M_InsertItemRO("Total Elements", EVAV_EVVIT_Int, &pst_Cob->pst_TriangleCob->l_NbElements, NULL);
		M_InsertItem1("Cob Flags", EVAV_EVVIT_SubStruct, &pst_Cob->uc_Flag, LINK_C_CobFlags, NULL);

		if(pst_GMat)
			pst_Element->uc_Flag |= COL_C_Cob_GameMat;
		else
			pst_Element->uc_Flag &= ~COL_C_Cob_GameMat;

		M_InsertItem123("Design", EVAV_EVVIT_Int, &pst_Element->uc_Design, 0, 0, 1, NULL);
		M_InsertItem1("Element Flags", EVAV_EVVIT_SubStruct, &pst_Element->uc_Flag, LINK_C_ElementFlags, NULL);
		M_InsertItem1("Element Material", EVAV_EVVIT_Pointer, pst_GMat, LINK_C_GMStruct, NULL);
		po_Item->mi_NumFields += 10;
		break;
	}
}

#ifdef ODE_INSIDE
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ODE_X(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE						st_ODE;
	DYN_tdst_ODE						*pst_ODE;
	OBJ_tdst_GameObject					*pst_GO;
	USHORT								uw_Offset;
	dMass								Mass;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	uw_Offset = (char *) &st_ODE.f_X - (char *) &st_ODE;
	pst_ODE = (DYN_tdst_ODE *) ((char *) p_Data - uw_Offset);

	pst_ODE->f_X = (*(float *) p_Data);
	pst_GO = pst_ODE->pst_GO;

	switch(pst_ODE->uc_Type)
	{
	case ODE_TYPE_SPHERE:
		dMassSetSphereTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateSphere(pst_GO->pst_World->ode_id_space, pst_ODE->f_X);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_BOX:
		dMassSetBoxTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateBox(pst_GO->pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_CYLINDER:
		dMassSetCylinderTotal(&Mass, pst_ODE->mass_init, 2, pst_ODE->f_X, pst_ODE->f_Y);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateCylinder(pst_GO->pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_PLAN:
		pst_ODE->ode_id_geom = dCreatePlane(pst_GO->pst_World->ode_id_space, pst_GO->pst_GlobalMatrix->Kx, pst_GO->pst_GlobalMatrix->Ky, pst_GO->pst_GlobalMatrix->Kz, pst_GO->pst_Base->pst_ODE->f_X);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_GO);
		break;
	}

	LINK_UpdatePointer(pst_ODE->pst_GO);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ODE_Y(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE						st_ODE;
	DYN_tdst_ODE						*pst_ODE;
	OBJ_tdst_GameObject					*pst_GO;
	USHORT								uw_Offset;
	dMass								Mass;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	uw_Offset = (char *) &st_ODE.f_Y - (char *) &st_ODE;
	pst_ODE = (DYN_tdst_ODE *) ((char *) p_Data - uw_Offset);

	pst_ODE->f_Y = (*(float *) p_Data);
	pst_GO = pst_ODE->pst_GO;

	switch(pst_ODE->uc_Type)
	{
	case ODE_TYPE_SPHERE:
		dMassSetSphereTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateSphere(pst_GO->pst_World->ode_id_space, pst_ODE->f_X);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_BOX:
		dMassSetBoxTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateBox(pst_GO->pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_CYLINDER:
		dMassSetCylinderTotal(&Mass, pst_ODE->mass_init, 2, pst_ODE->f_X, pst_ODE->f_Y);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateCylinder(pst_GO->pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_PLAN:
		pst_ODE->ode_id_geom = dCreatePlane(pst_GO->pst_World->ode_id_space, pst_GO->pst_GlobalMatrix->Kx, pst_GO->pst_GlobalMatrix->Ky, pst_GO->pst_GlobalMatrix->Kz, pst_GO->pst_Base->pst_ODE->f_X);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_GO);
		break;
	}

	LINK_UpdatePointer(pst_ODE->pst_GO);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ODE_Z(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE						st_ODE;
	DYN_tdst_ODE						*pst_ODE;
	OBJ_tdst_GameObject					*pst_GO;
	USHORT								uw_Offset;
	dMass								Mass;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	uw_Offset = (char *) &st_ODE.f_Z - (char *) &st_ODE;
	pst_ODE = (DYN_tdst_ODE *) ((char *) p_Data - uw_Offset);

	pst_ODE->f_Z = (*(float *) p_Data);
	pst_GO = pst_ODE->pst_GO;

	switch(pst_ODE->uc_Type)
	{
	case ODE_TYPE_SPHERE:
		dMassSetSphereTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateSphere(pst_GO->pst_World->ode_id_space, pst_ODE->f_X);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_BOX:
		dMassSetBoxTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateBox(pst_GO->pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_CYLINDER:
		dMassSetCylinderTotal(&Mass, pst_ODE->mass_init, 2, pst_ODE->f_X, pst_ODE->f_Y);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateCylinder(pst_GO->pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_PLAN:
		pst_ODE->ode_id_geom = dCreatePlane(pst_GO->pst_World->ode_id_space, pst_GO->pst_GlobalMatrix->Kx, pst_GO->pst_GlobalMatrix->Ky, pst_GO->pst_GlobalMatrix->Kz, pst_GO->pst_Base->pst_ODE->f_X);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_GO);
		break;

	}

	LINK_UpdatePointer(pst_ODE->pst_GO);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ODE_Mass(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	dMass								st_Mass;
	DYN_tdst_ODE						st_ODE;
	DYN_tdst_ODE						*pst_ODE;
	OBJ_tdst_GameObject					*pst_GO;
	USHORT								uw_Offset;
	dMass								Mass;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	uw_Offset = (char *) &st_ODE.mass_init- (char *) &st_ODE;

	pst_ODE = (DYN_tdst_ODE *) ((char *) p_Data - uw_Offset);

	pst_GO = pst_ODE->pst_GO;

	pst_ODE->mass_init = *(float *)p_Data;

	switch(pst_ODE->uc_Type)
	{
	case ODE_TYPE_SPHERE:
		dMassSetSphereTotal(&Mass, *(float *)p_Data, pst_ODE->f_X);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateSphere(pst_GO->pst_World->ode_id_space, pst_ODE->f_X);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_BOX:
		dMassSetBoxTotal(&Mass, *(float *)p_Data, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateBox(pst_GO->pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	case ODE_TYPE_CYLINDER:
		dMassSetCylinderTotal(&Mass, *(float *)p_Data, 2, pst_ODE->f_X, pst_ODE->f_Y);

		dGeomDestroy(pst_ODE->ode_id_geom);
		pst_ODE->ode_id_geom = dCreateCylinder(pst_GO->pst_World->ode_id_space, pst_ODE->f_X, pst_ODE->f_Y);
		dGeomSetData(pst_ODE->ode_id_geom, (void *) pst_ODE->pst_GO);
		dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
		if(pst_ODE->ode_id_body)
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		break;

	}

	LINK_UpdatePointer(pst_ODE->pst_GO);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OBJAddODE(POSITION pos, void *_pst_ODE)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem					*po_Item, *po_NewItem;
	DYN_tdst_ODE						*pst_ODE;
	OBJ_tdst_GameObject					*pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);

	pst_ODE = (DYN_tdst_ODE *) _pst_ODE;
	pst_GO = pst_ODE->pst_GO;

	if(pst_ODE->ode_id_body)
	{
		M_InsertItem1("Internal Body", EVAV_EVVIT_SubStruct, &(((struct dxBody *)pst_ODE->ode_id_body)->flags), LINK_C_InternalBodyFlags, NULL);
		po_Item->mi_NumFields += 1;
	}

	M_InsertItem1("Control Flags", EVAV_EVVIT_SubStruct, &pst_ODE->uc_Flags, LINK_C_ODEFlags, NULL);
	po_Item->mi_NumFields += 1;

	M_InsertItem1("Type of Geometric ColMap", EVAV_EVVIT_ODE, pst_ODE, NULL, NULL);
	po_Item->mi_NumFields += 1;


	M_InsertItem1234
	(
		"Sound ID",
		EVAV_EVVIT_ConstInt,
//		EVAV_EVVIT_Int,
		&pst_ODE->uc_Sound,
		0,
		0,
		1,
		NULL,
		// hogsy: originals
		//(int) "0-No Sound\n0\n1-Terre battue\n1\n2-Végétation\n2\n3-Pierre\n3\n4-Eau/Marécage\n4\n5-Chair\n5\n6-Bois\n6\n7-Metal\n7\n"
	    (int) "0-No Sound\n0\n1-Dirt\n1\n2-Vegetation\n2\n3-Stone\n3\n4-Water/Marsh\n4\n5-Chair\n5\n6-Wood\n6\n7-Metal\n7\n"
	);

//	M_InsertItem123("Sound ID", EVAV_EVVIT_Int, &pst_ODE->uc_Sound, 0, 0, 1, NULL);
	po_Item->mi_NumFields += 1;

	if(pst_ODE->uc_Type)
	{
		if(pst_ODE->ode_id_body)
		{
			M_InsertItem("Mass Init", EVAV_EVVIT_Float, &pst_ODE->mass_init, LINK_CallBack_ODE_Mass);
			M_InsertItemRO("Mass", EVAV_EVVIT_Float, &pst_ODE->ode_id_body->mass.mass, NULL);
			po_Item->mi_NumFields += 2;
		}

		switch(pst_ODE->uc_Type)
		{
		case ODE_TYPE_SPHERE:
			M_InsertItem("Radius", EVAV_EVVIT_Float, &pst_ODE->f_X, LINK_CallBack_ODE_X);
			po_Item->mi_NumFields += 1;
			break;

		case ODE_TYPE_BOX:
			M_InsertItem("dX", EVAV_EVVIT_Float, &pst_ODE->f_X, LINK_CallBack_ODE_X);
			M_InsertItem("dY", EVAV_EVVIT_Float, &pst_ODE->f_Y, LINK_CallBack_ODE_Y);
			M_InsertItem("dZ", EVAV_EVVIT_Float, &pst_ODE->f_Z, LINK_CallBack_ODE_Z);
			po_Item->mi_NumFields += 3;
			break;

		case ODE_TYPE_CYLINDER:
			M_InsertItem("Radius", EVAV_EVVIT_Float, &pst_ODE->f_X, LINK_CallBack_ODE_X);
			M_InsertItem("Length", EVAV_EVVIT_Float, &pst_ODE->f_Y, LINK_CallBack_ODE_Y);
			po_Item->mi_NumFields += 2;
			break;
		}

		M_InsertItem("OffSet", EVAV_EVVIT_Vector, &pst_ODE->st_Offset, LINK_CallBack_Refresh3DEngine);
		po_Item->mi_NumFields += 1;

		M_InsertItem("Orientation I", EVAV_EVVIT_Vector, &pst_ODE->st_RotMatrix, LINK_CallBack_Refresh3DEngine);
		M_InsertItem("Orientation J", EVAV_EVVIT_Vector, &pst_ODE->st_RotMatrix.Jx, LINK_CallBack_Refresh3DEngine);
		M_InsertItem("Orientation K", EVAV_EVVIT_Vector, &pst_ODE->st_RotMatrix.Kx, LINK_CallBack_Refresh3DEngine);
		po_Item->mi_NumFields += 3;


		M_InsertItem("Linear Threshold", EVAV_EVVIT_Float, &pst_ODE->f_LinearThres, NULL);
		M_InsertItem("Angular Threshold", EVAV_EVVIT_Float, &pst_ODE->f_AngularThres, NULL);
		po_Item->mi_NumFields += 2;

		if(pst_ODE->ode_id_body)
		{
			M_InsertItemRO("I0", EVAV_EVVIT_Vector, &pst_ODE->ode_id_body->mass.I[0], NULL);
			M_InsertItemRO("I1", EVAV_EVVIT_Vector, &pst_ODE->ode_id_body->mass.I[4], NULL);
			M_InsertItemRO("I2", EVAV_EVVIT_Vector, &pst_ODE->ode_id_body->mass.I[8], NULL);
			po_Item->mi_NumFields += 3;
		}

		if(pst_ODE->ode_id_body)
		{
			M_InsertItemRO("Position", EVAV_EVVIT_Vector, (void *) dBodyGetPosition(pst_ODE->ode_id_body), NULL);
			M_InsertItemRO("Linear Speed", EVAV_EVVIT_Vector, (void *) dBodyGetLinearVel(pst_ODE->ode_id_body), NULL);
			M_InsertItemRO("Angular Speed", EVAV_EVVIT_Vector, (void *) dBodyGetAngularVel(pst_ODE->ode_id_body), NULL);
			po_Item->mi_NumFields += 3;
		}
	}

	/* Anti Bug */
	if(pst_ODE->SurfaceMode == 0)
		pst_ODE->SurfaceMode = 4;	/* bounce per default */


	/*
	M_InsertItem1234
	(
		"ODE - Surface",
		EVAV_EVVIT_ConstInt,
		&pst_ODE->SurfaceMode,
		0,
		0,
		4,
		NULL,
		(int) "0-dContactMu2\n1\n1-dContactFDir1\n2\n2-dContactBounce\n4\n3-dContactSoftERP\n8\n4-dContactSoftCFM\n16\n5-dContactMotion1\n32\n6-dContactMotion2\n64\n7-dContactSlip1\n128\n8-dContactSlip2\n256\n9-dContactApprox1\n2048\n9-dContactSoftERP + dContactSoftCFM\n24\n"
	);
	*/
	M_InsertItem1("Surface Flags", EVAV_EVVIT_SubStruct, &pst_ODE->SurfaceMode, LINK_C_SurfaceFlags, NULL);

	M_InsertItem("mu", EVAV_EVVIT_Float, &pst_ODE->mu, NULL);
	M_InsertItem("mu2", EVAV_EVVIT_Float, &pst_ODE->mu2, NULL);
	M_InsertItem("bounce", EVAV_EVVIT_Float, &pst_ODE->bounce, NULL);
	M_InsertItem("bounce vel", EVAV_EVVIT_Float, &pst_ODE->bounce_vel, NULL);
	M_InsertItem("soft erp", EVAV_EVVIT_Float, &pst_ODE->soft_erp, NULL);
	M_InsertItem("soft cfm", EVAV_EVVIT_Float, &pst_ODE->soft_cfm, NULL);
	M_InsertItem("motion 1", EVAV_EVVIT_Float, &pst_ODE->motion1, NULL);
	M_InsertItem("motion 2", EVAV_EVVIT_Float, &pst_ODE->motion2, NULL);
	M_InsertItem("slip 1", EVAV_EVVIT_Float, &pst_ODE->slip1, NULL);
	M_InsertItem("slip 2", EVAV_EVVIT_Float, &pst_ODE->slip2, NULL);
	po_Item->mi_NumFields += 11;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OBJAddGraphicObject(POSITION pos, void *_pst_Gro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item, *po_NewItem;
	GRO_tdst_Struct		*pst_Gro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Gro = (GRO_tdst_Struct *) _pst_Gro;

	/* Get the pointer of instance */
	if(!pos)
		po_Item = mpo_ListItems->GetTail();	
	else
		po_Item = mpo_ListItems->GetAt(pos);

	M_InsertItemRO("Type", EVAV_EVVIT_GroType, &pst_Gro->i->ul_Type, NULL);
	M_InsertItem("Name", EVAV_EVVIT_String, pst_Gro->sz_Name, NULL);
	po_Item->mi_NumFields = 2;

	switch(pst_Gro->i->ul_Type)
	{
	case GRO_Geometric:
		po_Item->mi_NumFields += OBJ_i_AddGraphicObject_Geometric(pos, po_Item, pst_Gro);
		break;
	case GRO_Light:
		po_Item->mi_NumFields += OBJ_i_AddGraphicObject_Light(pos, po_Item, pst_Gro);
		break;
	case GRO_MaterialSingle:
		po_Item->mi_NumFields += OBJ_i_AddGraphicObject_MaterialSingle(pos, po_Item, pst_Gro);
		break;
	case GRO_MaterialMulti:
		po_Item->mi_NumFields += OBJ_i_AddGraphicObject_MaterialMulti(pos, po_Item, pst_Gro);
		break;
	case GRO_MaterialMultiTexture:
		M_InsertItem123("Sound ID", EVAV_EVVIT_Int, &((MAT_tdst_MultiTexture *)pst_Gro)->uc_Sound, 0, 0, 1, NULL);
		po_Item->mi_NumFields ++;
		break;
	case GRO_Camera:
		po_Item->mi_NumFields += OBJ_i_AddGraphicObject_Camera(pos, po_Item, pst_Gro);
		break;
	case GRO_GeoStaticLOD:
		po_Item->mi_NumFields += OBJ_i_AddGraphicObject_StaticLOD(pos, po_Item, pst_Gro);
		break;
	case GRO_2DText:
		po_Item->mi_NumFields += OBJ_i_AddGraphicObject_Text(pos, po_Item, pst_Gro);
		break;
	case GRO_ParticleGenerator:
		po_Item->mi_NumFields += OBJ_i_AddGraphicObject_ParticleGenerator(pos, po_Item, pst_Gro);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVAV_cl_ListBox::OBJ_i_AddGraphicObject_Geometric(POSITION pos, EVAV_cl_ViewItem *po_Item, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_NewItem;
	GEO_tdst_Object		*pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Geo = (GEO_tdst_Object *) p;

	M_InsertItemRO("Nb Points", EVAV_EVVIT_Int, &pst_Geo->l_NbPoints, NULL);
	M_InsertItemRO("Nb UVs", EVAV_EVVIT_Int, &pst_Geo->l_NbUVs, NULL);
	M_InsertItemRO("Nb Elements", EVAV_EVVIT_Int, &pst_Geo->l_NbElements, NULL);
	M_InsertItemRO("Has RLI", EVAV_EVVIT_Bool, &pst_Geo->dul_PointColors, NULL);
	M_InsertItem1("Editor Flags", EVAV_EVVIT_SubStruct, &pst_Geo->ul_EditorFlags, LINK_C_GEO_EditorFlags, NULL);
	return 5;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVAV_cl_ListBox::OBJ_i_AddGraphicObject_Light(POSITION pos, EVAV_cl_ViewItem *po_Item, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_NewItem;
	LIGHT_tdst_Light	*pst_Light;
	int					num;
#ifdef JADEFUSION
	float				f_Min, f_Max;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Light = (LIGHT_tdst_Light *) p;

#ifdef JADEFUSION
	M_InsertItem("Type", EVAV_EVVIT_GroLightType, &pst_Light->ul_Flags, LIGHT_CallBack_ChangeType);
#else
	M_InsertItem("Type", EVAV_EVVIT_GroLightType, &pst_Light->ul_Flags, LINK_CallBack_Refresh3DEngineAndPointers);
#endif

	M_InsertItem1
	(
		"Flags",
		EVAV_EVVIT_SubStruct,
		&pst_Light->ul_Flags,
		LINK_C_GDK_LightFlag,
		LINK_CallBack_Refresh3DEngine
	);

	M_InsertItem("Color", EVAV_EVVIT_Color, &pst_Light->ul_Color, LINK_CallBack_Refresh3DEngine);
	
	M_InsertItem("Object", EVAV_EVVIT_GO, &pst_Light->pst_GO, LINK_CallBack_Refresh3DEngine);
	num = 4;

	switch(pst_Light->ul_Flags & LIGHT_Cul_LF_Type)
	{
	case LIGHT_Cul_LF_Omni:
		M_InsertItem("Near", EVAV_EVVIT_Float, &pst_Light->st_Omni.f_Near, LINK_CallBack_Refresh3DEngine);
		M_InsertItem("Far", EVAV_EVVIT_Float, &pst_Light->st_Omni.f_Far, LINK_CallBack_Refresh3DEngine);
		num += 2;
		break;

	case LIGHT_Cul_LF_Direct:
		break;

	case LIGHT_Cul_LF_Spot:
#ifdef JADEFUSION
		f_Min = Cf_EpsilonBig;
		f_Max = Cf_Infinit;
		M_InsertItem12("Near", EVAV_EVVIT_Float, &pst_Light->st_Spot.f_Near, *(int*)&f_Min, *(int*)&f_Max, LIGHT_CallBack_ValidateNear);
		M_InsertItem12("Far", EVAV_EVVIT_Float, &pst_Light->st_Spot.f_Far, *(int*)&f_Min, *(int*)&f_Max, LIGHT_CallBack_ValidateFar);
		M_InsertItem("alpha", EVAV_EVVIT_Float, &pst_Light->st_Spot.f_LittleAlpha, LIGHT_CallBack_ValidateLittleAlpha);
		M_InsertItem("ALPHA", EVAV_EVVIT_Float, &pst_Light->st_Spot.f_BigAlpha, LIGHT_CallBack_ValidateBigAlpha);
#else
		M_InsertItem("Near", EVAV_EVVIT_Float, &pst_Light->st_Spot.f_Near, LINK_CallBack_Refresh3DEngine);
		M_InsertItem("Far", EVAV_EVVIT_Float, &pst_Light->st_Spot.f_Far, LINK_CallBack_Refresh3DEngine);
		M_InsertItem("alpha", EVAV_EVVIT_Float, &pst_Light->st_Spot.f_LittleAlpha, LINK_CallBack_Refresh3DEngine);
		M_InsertItem("ALPHA", EVAV_EVVIT_Float, &pst_Light->st_Spot.f_BigAlpha, LINK_CallBack_Refresh3DEngine);
#endif
		num += 4;
		break;
	case LIGHT_Cul_LF_Fog:
		M_InsertItem("Start", EVAV_EVVIT_Float, &pst_Light->st_Fog.f_Start, LINK_CallBack_Refresh3DEngine);
		M_InsertItem("End", EVAV_EVVIT_Float, &pst_Light->st_Fog.f_End, LINK_CallBack_Refresh3DEngine);
		num += 2;
		if(!(pst_Light->ul_Flags & LIGHT_Cul_LF_FogLinear))
		{
			M_InsertItem("Density", EVAV_EVVIT_Float, &pst_Light->st_Fog.f_Density, LINK_CallBack_Refresh3DEngine);
			num++;
		}
#ifdef JADEFUSION
        M_InsertItem("Pitch Attenuation Min (Degrees)", EVAV_EVVIT_Float, &pst_Light->st_Fog.f_PitchAttenuationMin, LINK_CallBack_Refresh3DEngine);
        M_InsertItem("Pitch Attenuation Max (Degrees)", EVAV_EVVIT_Float, &pst_Light->st_Fog.f_PitchAttenuationMax, LINK_CallBack_Refresh3DEngine);
        M_InsertItem("Pitch Attenuation Intensity", EVAV_EVVIT_Float, &pst_Light->st_Fog.f_PitchAttenuationIntensity, LINK_CallBack_Refresh3DEngine);
        num += 3;
#endif			
		M_InsertItem("Fog as number 1", EVAV_EVVIT_Bool, &pst_Light->st_Fog.b_FogNumber1, LINK_CallBack_Refresh3DEngine);
		num++;
		break;

#ifdef JADEFUSION
    case LIGHT_Cul_LF_LightShaft:
        M_InsertItem("Start",              EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_Start,             LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Length",             EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_Length,            LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("FOV X",              EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_FOVX,              LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("FOV Y",              EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_FOVY,              LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Spot Inner Angle",   EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_SpotInnerAngle,    LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Spot Outer Angle",   EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_SpotOuterAngle,    LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Cookie Texture",     EVAV_EVVIT_Key,       &pst_Light->st_LightShaft.ul_CookieTextureKey, LIGHT_CallBack_ChangeLightShaftCookieTexture); ++num;
        M_InsertItem("Noise Texture",      EVAV_EVVIT_Key,       &pst_Light->st_LightShaft.ul_NoiseTextureKey,  LIGHT_CallBack_ChangeLightShaftNoiseTexture); ++num;
        M_InsertItem("Noise 1 Scroll U",   EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_Noise1ScrollU,     LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Noise 1 Scroll V",   EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_Noise1ScrollV,     LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Noise 2 Scroll U",   EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_Noise2ScrollU,     LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Noise 2 Scroll V",   EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_Noise2ScrollV,     LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Plane Density",      EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_PlaneDensity,      LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Attenuation Start",  EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_AttenuationStart,  LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Attenuation Factor", EVAV_EVVIT_Float,     &pst_Light->st_LightShaft.f_AttenuationFactor, LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem("Beam Color",         EVAV_EVVIT_Color,     &pst_Light->st_LightShaft.ul_Color,            LINK_CallBack_Refresh3DEngine); ++num;
        M_InsertItem1("Light Shaft Flags", EVAV_EVVIT_SubStruct, &pst_Light->st_LightShaft.ul_Flags,            LINK_C_LightShaftFlags, NULL); ++num;
        break;
#endif

	case LIGHT_Cul_LF_AddMaterial:
		M_InsertItem123
		(
			"SubId",
			EVAV_EVVIT_Int,
			&pst_Light->st_AddMaterial.w_Id,
			0,
			0,
			2,
			LINK_CallBack_Refresh3DEngine
		);

		/*
		 * M_InsertItem123 ( "Add material flags", EVAV_EVVIT_SubStruct,
		 * &pst_Light->st_AddMaterial.c_Flags, LINK_C_GDK_LightAddMaterialFlag, 0, 0,
		 * LINK_CallBack_Refresh3DEngine ); num += 2;
		 */
		num++;
		break;
	}
#ifdef JADEFUSION
    M_InsertItem("Diffuse Multiplier", EVAV_EVVIT_Float, &pst_Light->f_DiffuseMultiplier, LINK_CallBack_Refresh3DEngine);
    M_InsertItem("Specular Multiplier", EVAV_EVVIT_Float, &pst_Light->f_SpecularMultiplier, LINK_CallBack_Refresh3DEngine);
    M_InsertItem("Actor Diffuse Ponderator", EVAV_EVVIT_Float, &pst_Light->f_ActorDiffusePonderator, LINK_CallBack_Refresh3DEngine);
	M_InsertItem("Actor Specular Ponderator", EVAV_EVVIT_Float, &pst_Light->f_ActorSpecularPonderator, LINK_CallBack_Refresh3DEngine);
	M_InsertItem("RLI Blending Scale", EVAV_EVVIT_Float, &pst_Light->f_RLIBlendingScale, LINK_CallBack_Refresh3DEngine);
    M_InsertItem("RLI Blending Offset", EVAV_EVVIT_Float, &pst_Light->f_RLIBlendingOffset, LINK_CallBack_Refresh3DEngine);

	// Light Rejection
	M_InsertItem("LRL: Light Actors", EVAV_EVVIT_Bool, &pst_Light->b_LightActor, LINK_CallBack_Refresh3DEngine);
	M_InsertItem("LRL: Light Scene", EVAV_EVVIT_Bool, &pst_Light->b_LightScene, LINK_CallBack_Refresh3DEngine);

    // Xenon shadows
	M_InsertItem("Shadow: Color (Xenon)", EVAV_EVVIT_Color, &pst_Light->ul_ShadowColor, LINK_CallBack_Refresh3DEngine);
	M_InsertItem("Shadow: Use ambient color (Xenon)", EVAV_EVVIT_Bool, &pst_Light->b_UseAmbientAsColor, LINK_CallBack_Refresh3DEngine);

	f_Min = Cf_EpsilonBig;
	f_Max = Cf_Infinit;
    M_InsertItem12("Shadow: Near", EVAV_EVVIT_Float, &pst_Light->f_ShadowNear, *(int*)&f_Min, *(int*)&f_Max, LIGHT_CallBack_ValidateShadowNear);
    
    // Artists asked to use the light's far instead of having a separate far for shadows
    //M_InsertItem("Shadow: Far", EVAV_EVVIT_Float, &pst_Light->f_ShadowFar, LINK_CallBack_Refresh3DEngine);

    // Don't display # iterations anymore as it is now hardcoded in the shader
    //M_InsertItem12("Shadow: # Iterations (Xenon)", EVAV_EVVIT_Int, &pst_Light->ul_NumIterations, 2, 8, LINK_CallBack_Refresh3DEngine);

    f_Min = 0.0f;
    f_Max = 16.0f; 
    M_InsertItem12("Shadow: Filter size (Xenon)", EVAV_EVVIT_Float, &pst_Light->f_FilterSize, *(int*)&f_Min, *(int*)&f_Max, LINK_CallBack_Refresh3DEngine);

    f_Min = -0.1f;
    f_Max = 0.1f;
    M_InsertItem12("Shadow: Z offset (Xenon)", EVAV_EVVIT_Float, &pst_Light->f_ZOffset, *(int*)&f_Min, *(int*)&f_Max, LINK_CallBack_Refresh3DEngine);

    M_InsertItem("Shadow: Cookie Texture (Xenon)", EVAV_EVVIT_Key,  &pst_Light->ul_CookieTextureKey, LIGHT_CallBack_ChangeLightCookieTexture);

    f_Min = 0.1f;
    f_Max = M_PI / 2.0f;
    M_InsertItem12("Shadow: Dynamic casters FOV (Xenon)", EVAV_EVVIT_Float, &pst_Light->f_HiResFOV, *(int*)&f_Min, *(int*)&f_Max, LIGHT_CallBack_ValidateShadowAlpha);

    M_InsertItem("Shadow: Use dynamic casters FOV (Xenon)", EVAV_EVVIT_Bool, &pst_Light->b_UseHiResFOV, LINK_CallBack_Refresh3DEngine);
    M_InsertItem("Shadow: Force static receivers (Xenon)", EVAV_EVVIT_Bool, &pst_Light->b_ForceStaticReceiver, LINK_CallBack_Refresh3DEngine);

    num += 15;

	M_InsertItem("Rain Effect (Dynamic Specular) (Xenon)", EVAV_EVVIT_Bool, &pst_Light->b_RainEffect, LINK_CallBack_Refresh3DEngine);
	num++;
#endif

	return num;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVAV_cl_ListBox::OBJ_i_AddGraphicObject_MaterialSingle(POSITION pos, EVAV_cl_ViewItem *po_Item, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_NewItem;
	MAT_tdst_Single		*pst_Mat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mat = (MAT_tdst_Single *) p;

	M_InsertItemRO("TextureId", EVAV_EVVIT_Int, &pst_Mat->l_TextureId, NULL);
	M_InsertItem("Ambient", EVAV_EVVIT_Color, &pst_Mat->ul_Ambiant, LINK_CallBack_Refresh3DEngine);
	M_InsertItem("Diffuse", EVAV_EVVIT_Color, &pst_Mat->ul_Diffuse, LINK_CallBack_Refresh3DEngine);
	M_InsertItem("Opacity", EVAV_EVVIT_Float, &pst_Mat->f_Opacity, LINK_CallBack_Refresh3DEngine);

	/*
	 * M_InsertItem1 ( "DrawMask", EVAV_EVVIT_SubStruct, &pst_Mat->ul_DrawMask,
	 * LINK_C_FlagsDrawMask, LINK_CallBack_Refresh3DEngine );
	 */
	return 4;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVAV_cl_ListBox::OBJ_i_AddGraphicObject_MaterialMulti(POSITION pos, EVAV_cl_ViewItem *po_Item, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_NewItem;
	char				sz_Text[50];
	MAT_tdst_Multi		*pst_Mat;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mat = (MAT_tdst_Multi *) p;

	for(i = 0; i < pst_Mat->l_NumberOfSubMaterials; i++)
	{
		sprintf(sz_Text, "SubMat %d", i);
		M_InsertItem1(sz_Text, EVAV_EVVIT_Pointer, pst_Mat->dpst_SubMaterial[i], LINK_C_GDK_StructId, NULL);
	}

	return i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVAV_cl_ListBox::OBJ_i_AddGraphicObject_Camera(POSITION pos, EVAV_cl_ViewItem *po_Item, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem		*po_NewItem;
	CAM_tdst_CameraObject	*pst_Cam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cam = (CAM_tdst_CameraObject *) p;

	M_InsertItem("Near plane", EVAV_EVVIT_Float, &pst_Cam->f_NearPlane, LINK_CallBack_Refresh3DEngine);
	M_InsertItem("Far plane ", EVAV_EVVIT_Float, &pst_Cam->f_FarPlane, LINK_CallBack_Refresh3DEngine);
	M_InsertItem("FOV", EVAV_EVVIT_Float, &pst_Cam->f_FieldOfVision, LINK_CallBack_Refresh3DEngine);
	return 3;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVAV_cl_ListBox::OBJ_i_AddGraphicObject_StaticLOD(POSITION pos, EVAV_cl_ViewItem *po_Item, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_NewItem;
	GEO_tdst_StaticLOD	*pst_LOD;
	int					i;
	char				sz_Text[50];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_LOD = (GEO_tdst_StaticLOD *) p;

	for(i = 0; i < pst_LOD->uc_NbLOD; i++)
	{
		sprintf(sz_Text, "End distance %d", i);
		M_InsertItem123(sz_Text, EVAV_EVVIT_Int, pst_LOD->auc_EndDistance + i, 0, 0, 1, LINK_CallBack_Refresh3DEngine);
		sprintf(sz_Text, "LOD %d", i);
		M_InsertItem1(sz_Text, EVAV_EVVIT_Pointer, pst_LOD->dpst_Id[i], LINK_C_GDK_StructId, NULL);
	}

	return i * 2;
}

/*
 =======================================================================================================================
    Aim:    Function called when the number of sprites in SpriteList visuel change
 =======================================================================================================================
 */
void VAV_OBJ_CallBack_ChangeSpriteListNumberMax(void *p_Owner, void *_po_Item, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if 0
	SPL_tdst_Struct spl, *pst_SPL;
	long			l_NewNumber;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SPL = (SPL_tdst_Struct *) ((char *) p_Data - ((char *) &spl.l_NbMaxSprites - (char *) &spl));
	l_NewNumber = pst_SPL->l_NbMaxSprites;
	pst_SPL->l_NbMaxSprites = l_OldData;

	SPL_ChangeNumberMaxOfSprites(pst_SPL, l_NewNumber);
#endif
}

#if 0

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVAV_cl_ListBox::OBJ_i_AddGraphicObject_SpriteList(POSITION pos, EVAV_cl_ViewItem *po_Item, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SPL_tdst_Struct		*pst_SPL;
	EVAV_cl_ViewItem	*po_NewItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SPL = (SPL_tdst_Struct *) p;
	M_InsertItem
	(
		"Max number of sprites",
		EVAV_EVVIT_Int,
		&pst_SPL->l_NbMaxSprites,
		VAV_OBJ_CallBack_ChangeSpriteListNumberMax
	);
	return 1;
}

#endif

/*
 =======================================================================================================================
    Aim:    Function called when the number of sprites in SpriteList visuel change
 =======================================================================================================================
 */
void VAV_OBJ_CallBack_ChangeTextLetterNumberMax(void *p_Owner, void *_po_Item, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct str, *pst_STR;
	long			l_NewNumber;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = (STR_tdst_Struct *) ((char *) p_Data - ((char *) &str.ul_NbMaxLetters - (char *) &str));
	l_NewNumber = pst_STR->ul_NbMaxLetters;
	pst_STR->ul_NbMaxLetters = l_OldData;

	STR_ChangeNumberMaxOfLetters(pst_STR, l_NewNumber);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVAV_cl_ListBox::OBJ_i_AddGraphicObject_Text(POSITION pos, EVAV_cl_ViewItem *po_Item, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct		*pst_STR;
	EVAV_cl_ViewItem	*po_NewItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = (STR_tdst_Struct *) p;
	M_InsertItem
	(
		"Max number of letters",
		EVAV_EVVIT_Int,
		&pst_STR->ul_NbMaxLetters,
		VAV_OBJ_CallBack_ChangeTextLetterNumberMax
	);
	return 1;
}

/*
 =======================================================================================================================
    Aim:    Function called when the number of sprites in SpriteList visuel change
 =======================================================================================================================
 */
void VAV_OBJ_CallBack_ChangeNbParticle(void *p_Owner, void *_po_Item, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct P, *pst_P;
	long			l_NewNumber;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_P = (PAG_tdst_Struct *) ((char *) p_Data - ((char *) &P.l_NbMaxP - (char *) &P));
	l_NewNumber = pst_P->l_NbMaxP;
	pst_P->l_NbMaxP = l_OldData;

	PAG_ChangeNumberOfParticles(pst_P, l_NewNumber);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VAV_OBJ_Callback_ParticleUpdate(void *p_Owner, void *_po_Item, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	EVAV_cl_ViewItem	*po_Parent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Item = (EVAV_cl_ViewItem *) _po_Item;
	po_Parent = gpo_CurVavListBox->po_GetParentGAO(po_Item);
	if(po_Parent == NULL) return;
	LINK_UpdatePointer(po_Parent->mp_Data);
	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVAV_cl_ListBox::OBJ_i_AddGraphicObject_ParticleGenerator(POSITION pos, EVAV_cl_ViewItem *po_Item, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct		*pst_P;
	EVAV_cl_ViewItem	*po_NewItem;
	int					n;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_P = (PAG_tdst_Struct *) p;
	M_InsertItem("Max number of particles", EVAV_EVVIT_Int, &pst_P->l_NbMaxP, VAV_OBJ_CallBack_ChangeNbParticle);
	M_InsertItem1("Flags", EVAV_EVVIT_SubStruct, &pst_P->ul_Flags, LINK_C_ENG_ParticuleGeneratorFlags, NULL);
	M_InsertItem("Particle Color", EVAV_EVVIT_Color, &pst_P->ul_Color, NULL);
	M_InsertItem("Particle size X Min", EVAV_EVVIT_Float, &pst_P->f_SizeXMin, NULL);
	M_InsertItem("Particle size X Max", EVAV_EVVIT_Float, &pst_P->f_SizeXMax, NULL);
	M_InsertItem("Particle size Y Min", EVAV_EVVIT_Float, &pst_P->f_SizeYMin, NULL);
	M_InsertItem("Particle size Y Max", EVAV_EVVIT_Float, &pst_P->f_SizeYMax, NULL);
	M_InsertItem("P per second (Init)", EVAV_EVVIT_Float, &pst_P->ast_Gen->f_NbPerSecondInit, NULL);
	M_InsertItem("P per second (Cur)", EVAV_EVVIT_Float, &pst_P->ast_Gen->f_NbPerSecond, NULL);

	M_InsertItem("Object", EVAV_EVVIT_GO, &pst_P->pst_GO, NULL);

	M_InsertItem("Generator offset", EVAV_EVVIT_Float, &pst_P->f_GenOffset, NULL);

	M_InsertItem1234
	(
		"Generator type",
		EVAV_EVVIT_ConstInt,
		&pst_P->c_GenType,
		0,
		0,
		1,
		VAV_OBJ_Callback_ParticleUpdate,
		(int) "Point\n0\nRectangle\n1\nCircle\n2\nCylindre\n3\nSphere\n4\nBox\n5\nObject point\n6\n"
	);
	n = 12;

	if(pst_P->c_GenType == PAG_GenType_Rectangle)
	{
		M_InsertItem("X size", EVAV_EVVIT_Float, &pst_P->f_GenParam[0], NULL);
		M_InsertItem("Y size", EVAV_EVVIT_Float, &pst_P->f_GenParam[1], NULL);
		n += 2;
	}
	else if(pst_P->c_GenType == PAG_GenType_Circle)
	{
		M_InsertItem("Radius", EVAV_EVVIT_Float, &pst_P->f_GenParam[0], NULL);
		n++;
	}
	else if(pst_P->c_GenType == PAG_GenType_Cylindre)
	{
		M_InsertItem("Radius", EVAV_EVVIT_Float, &pst_P->f_GenParam[0], NULL);
		M_InsertItem("Height", EVAV_EVVIT_Float, &pst_P->f_GenParam[1], NULL);
		n += 2;
	}
	else if(pst_P->c_GenType == PAG_GenType_Sphere)
	{
		M_InsertItem("Radius", EVAV_EVVIT_Float, &pst_P->f_GenParam[0], NULL);
		n++;
	}
	else if(pst_P->c_GenType == PAG_GenType_Box)
	{
		M_InsertItem("X size", EVAV_EVVIT_Float, &pst_P->f_GenParam[0], NULL);
		M_InsertItem("Y size", EVAV_EVVIT_Float, &pst_P->f_GenParam[1], NULL);
		M_InsertItem("Z size", EVAV_EVVIT_Float, &pst_P->f_GenParam[2], NULL);
		n += 3;
	}

	M_InsertItem1234
	(
		"Speed type",
		EVAV_EVVIT_ConstInt,
		&pst_P->c_SpeedType,
		0,
		0,
		1,
		VAV_OBJ_Callback_ParticleUpdate,
		(int) "Axis\n0\nFan\n1\nCone\n2\nFar from center\n3\nObject normal\n4\nTarget\n5\n"
	);
	M_InsertItem("Angle 1", EVAV_EVVIT_Float, &pst_P->f_Angle1, NULL);
	M_InsertItem("Angle 2", EVAV_EVVIT_Float, &pst_P->f_Angle2, NULL);
	n += 3;

	M_InsertItem("Speed factor min", EVAV_EVVIT_Float, &pst_P->f_Speed0, NULL);
	M_InsertItem("Speed factor max", EVAV_EVVIT_Float, &pst_P->f_Speed1, NULL);
	n += 2;

	M_InsertItem("Birth time min", EVAV_EVVIT_Float, &pst_P->f_TimeBirthMin, NULL);
	M_InsertItem("Birth time max", EVAV_EVVIT_Float, &pst_P->f_TimeBirthMax, NULL);
	M_InsertItem("Life time min", EVAV_EVVIT_Float, &pst_P->f_TimeMin, NULL);
	M_InsertItem("Life time max", EVAV_EVVIT_Float, &pst_P->f_TimeMax, NULL);
	M_InsertItem("Death time min", EVAV_EVVIT_Float, &pst_P->f_TimeDeathMin, NULL);
	M_InsertItem("Death time max", EVAV_EVVIT_Float, &pst_P->f_TimeDeathMax, NULL);
	M_InsertItem("Death size factor", EVAV_EVVIT_Float, &pst_P->f_SizeDeathFactor, NULL);
	n += 7;

	M_InsertItem("Acceleration", EVAV_EVVIT_Vector, &pst_P->st_Acc, NULL);
	M_InsertItem("Friction", EVAV_EVVIT_Float, &pst_P->f_Friction, NULL);
	M_InsertItem("Z min", EVAV_EVVIT_Float, &pst_P->f_ZMin, NULL);
	M_InsertItem("Z min strength", EVAV_EVVIT_Float, &pst_P->f_ZMinStrength, NULL);
	M_InsertItem("Z max", EVAV_EVVIT_Float, &pst_P->f_ZMax, NULL);
	M_InsertItem("Z max strength", EVAV_EVVIT_Float, &pst_P->f_ZMaxStrength, NULL);

	M_InsertItem("Rotation min", EVAV_EVVIT_Float, &pst_P->f_RotationMin, NULL);
	M_InsertItem("Rotation max", EVAV_EVVIT_Float, &pst_P->f_RotationMax, NULL);
	M_InsertItem("Rotation speed min", EVAV_EVVIT_Float, &pst_P->f_RotationSpeedMin, NULL);
	M_InsertItem("Rotation speed max", EVAV_EVVIT_Float, &pst_P->f_RotationSpeedMax, NULL);
	n += 10;

	M_InsertItem("Sinus Factor X", EVAV_EVVIT_Float, &pst_P->f_SinXFactor, NULL);
	M_InsertItem("Sinus Factor Y", EVAV_EVVIT_Float, &pst_P->f_SinYFactor, NULL);
	n += 2;

	M_InsertItem("Dist Constraint", EVAV_EVVIT_Float, &pst_P->f_DistConstraint, NULL);
	n += 1;

	/*$F
    M_InsertItem("Number of attractor", EVAV_EVVIT_Int, &pst_P->i_NbAttractor, NULL);
    M_InsertItem("Center", EVAV_EVVIT_Vector, &pst_P->st_ACenter[0], NULL);
    M_InsertItem("Internal dist²", EVAV_EVVIT_Float, &pst_P->f_ADist1[0], NULL);
    M_InsertItem("External dist²", EVAV_EVVIT_Float, &pst_P->f_ADist2[0], NULL);
    M_InsertItem("Strength", EVAV_EVVIT_Float, &pst_P->f_AStrength[0], NULL);
    M_InsertItem("Friction", EVAV_EVVIT_Float, &pst_P->f_AFriction[0], NULL);
    M_InsertItem("Center", EVAV_EVVIT_Vector, &pst_P->st_ACenter[1], NULL);
    M_InsertItem("Internal dist²", EVAV_EVVIT_Float, &pst_P->f_ADist1[1], NULL);
    M_InsertItem("External dist²", EVAV_EVVIT_Float, &pst_P->f_ADist2[1], NULL);
    M_InsertItem("Strength", EVAV_EVVIT_Float, &pst_P->f_AStrength[1], NULL);
    M_InsertItem("Friction", EVAV_EVVIT_Float, &pst_P->f_AFriction[1], NULL);
    n += 11;
    */
	return n;
}

/*
 =======================================================================================================================
    Aim:    Function called when the number of sprites in SpriteList visuel change
 =======================================================================================================================
 */
void VAV_OBJ_CallBack_ChangeNumberOfAdditionalMatrix(void *p_Owner, void *_po_Item, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_AdditionalMatrix	st_AM, *pst_AM;
	long						l_Decal, l_NewNumber, i;
	OBJ_tdst_Gizmo				*pst_Gizmo;
	OBJ_tdst_GizmoPtr			*pst_GizmoPtr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Decal = (char *) &st_AM.l_Number - (char *) &st_AM;
	pst_AM = (OBJ_tdst_AdditionalMatrix *) (((char *) p_Data) - l_Decal);
	l_NewNumber = pst_AM->l_Number;

	if(l_NewNumber == l_OldData) return;

	if(l_NewNumber == 0)
	{
		MEM_Free(pst_AM->dst_Gizmo);
	}
	else
	{
		if(pst_AM->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
		{
			l_Decal = sizeof(OBJ_tdst_GizmoPtr) * l_NewNumber;
			if(l_OldData == 0)
				pst_AM->dst_GizmoPtr = (OBJ_tdst_GizmoPtr *) MEM_p_Alloc(l_Decal);
			else
				pst_AM->dst_GizmoPtr = (OBJ_tdst_GizmoPtr *) MEM_p_Realloc(pst_AM->dst_GizmoPtr, l_Decal);

			for(i = l_OldData; i < l_NewNumber; i++)
			{
				pst_GizmoPtr = pst_AM->dst_GizmoPtr + i;
				pst_GizmoPtr->pst_Matrix = NULL;
				pst_GizmoPtr->pst_GO = NULL;
				pst_GizmoPtr->l_MatrixId = -1;
			}
		}
		else
		{
			l_Decal = sizeof(OBJ_tdst_Gizmo) * l_NewNumber;
			if(l_OldData == 0)
				pst_AM->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_Alloc(l_Decal);
			else
				pst_AM->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_Realloc(pst_AM->dst_Gizmo, l_Decal);

			for(i = l_OldData; i < l_NewNumber; i++)
			{
				pst_Gizmo = pst_AM->dst_Gizmo + i;
				MATH_SetIdentityMatrix(&pst_Gizmo->st_Matrix);
				sprintf(pst_Gizmo->sz_Name, "Gizmo %d", i);
			}
		}
	}

	LINK_UpdatePointer(pst_AM->pst_GO);
	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
    Aim:    Function called when pointer ref change
 =======================================================================================================================
 */
void VAV_OBJ_CallBack_AMUpdate(void *p_Owner, void *_po_Item, void *_p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_ComboGO;
	EVAV_cl_ViewItem	*po_Parent;
	EVAV_cl_ViewItem	*_po_Data;
	TAB_tdst_PFtable	*pst_AllObjects;
	OBJ_tdst_GizmoPtr	*pst_GizmoPtr;
	BOOL				b_MustDel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_po_Data = (EVAV_cl_ViewItem *) _po_Item;

	if(_po_Data->me_Type == EVAV_EVVIT_GO)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GizmoPtr	st_GizmoPtr;
		ULONG				ul_Offset;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_Offset = (char *) &st_GizmoPtr.pst_GO - (char *) &st_GizmoPtr;
		pst_GizmoPtr = (OBJ_tdst_GizmoPtr *) ((char *) _po_Data->mp_Data - ul_Offset);
		pst_ComboGO = pst_GizmoPtr->pst_GO;
	}
	else
		pst_ComboGO = NULL;
	po_Parent = _po_Data;
	while(1)
	{
		po_Parent = gpo_CurVavListBox->GetItemParent(po_Parent);
		if(po_Parent == NULL) return;
		if((po_Parent->me_Type == EVAV_EVVIT_Pointer) && (po_Parent->mi_Param1 == LINK_C_ENG_GameObjectOriented))
			break;
	}

	if(!po_Parent) return;
	pst_GO = (OBJ_tdst_GameObject *) po_Parent->mp_Data;

	if
	(
		pst_ComboGO
	&&	pst_GO
	&&	pst_GO->pst_Base
	&&	pst_GO->pst_Base->pst_AddMatrix
	&&	pst_GO->pst_Base->pst_AddMatrix->ul_GrpIndex
	&&	(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
	)
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	ul_BoneIndex;
		ULONG	ul_NumBones;
		ULONG	ul_NumAM;
		ULONG	i, Min, GizmoNum;
		/*~~~~~~~~~~~~~~~~~~~~~*/

		b_MustDel = FALSE;
		pst_AllObjects = (TAB_tdst_PFtable *) LOA_ul_SearchAddress(BIG_PosFile(pst_GO->pst_Base->pst_AddMatrix->ul_GrpIndex));
		if(!pst_AllObjects || ((int) pst_AllObjects == -1))
		{
			LOA_MakeFileRef
			(
				BIG_FileKey(pst_GO->pst_Base->pst_AddMatrix->ul_GrpIndex),
				(ULONG *) &pst_AllObjects,
				OBJ_ul_World_LoadGolCallback,
				LOA_C_MustExists
			);
			LOA_Resolve();
			b_MustDel = TRUE;
		}

		ul_BoneIndex = TAB_ul_PFtable_GetElemIndexWithPointer(pst_AllObjects, pst_ComboGO);

		if(ul_BoneIndex != TAB_Cul_BadIndex)
		{
			ul_NumBones = pst_AllObjects->ul_NbElems;
			ul_NumAM = pst_GO->pst_Base->pst_AddMatrix->l_Number;

			GizmoNum = pst_GizmoPtr - pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr;

			/* We have set a new Bone to ponderate the Skin. Sets the gyzmo number. */
			pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].l_MatrixId = ul_BoneIndex;

			Min = (ul_NumAM - GizmoNum < ul_NumBones - ul_BoneIndex) ? ul_NumAM - GizmoNum : ul_NumBones - ul_BoneIndex;
			for(i = 0; i < Min - 1; i++)
			{
				GizmoNum++;
				pst_ComboGO = (OBJ_tdst_GameObject *) pst_AllObjects->p_Table[++ul_BoneIndex].p_Pointer;
				pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].l_MatrixId = ul_BoneIndex;
				pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO = pst_ComboGO;
			}
		}

		/* Delete group */
		if(b_MustDel) OBJ_FreeGroupGrl(pst_AllObjects);
	}

	OBJ_Gizmo_UpdateGizmoPtr(pst_GO);
	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OBJAddBaseAdditionalMatrix(POSITION pos, void *_pst_AM)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem			*po_Item, *po_NewItem;
	OBJ_tdst_AdditionalMatrix	*pst_AM;
	OBJ_tdst_Gizmo				*pst_Gizmo;
	OBJ_tdst_GizmoPtr			*pst_GizmoPtr;
	char						sz_Text[50];
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);

	pst_AM = (OBJ_tdst_AdditionalMatrix *) _pst_AM;

	po_Item->mi_NumFields = 0;
	if
	(
		pst_AM
	&&	!(pst_AM->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)
	&&	(pst_AM->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
	)
	{
		M_InsertItemColor("Skeleton", EVAV_EVVIT_Group, &pst_AM->ul_GrpIndex, NULL, 0x008EB48B);
		po_Item->mi_NumFields++;
	}

	M_InsertItem("Number", EVAV_EVVIT_Int, &pst_AM->l_Number, VAV_OBJ_CallBack_ChangeNumberOfAdditionalMatrix);
	po_Item->mi_NumFields++;

	strcpy(sz_Text, "Gizmo ");
	if(pst_AM->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
	{
		pst_GizmoPtr = pst_AM->dst_GizmoPtr;
		for(i = 0; i < pst_AM->l_Number; i++, pst_GizmoPtr++)
		{
			_itoa(i, sz_Text + 6, 10);
			M_InsertItem1
			(
				sz_Text,
				EVAV_EVVIT_GO,
				&pst_GizmoPtr->pst_GO,
				(int) pst_AM->pst_GO,
				VAV_OBJ_CallBack_AMUpdate
			);
			M_InsertItem(sz_Text, EVAV_EVVIT_Int, &pst_GizmoPtr->l_MatrixId, VAV_OBJ_CallBack_AMUpdate);
			M_InsertItem1(sz_Text, EVAV_EVVIT_Pointer, pst_GizmoPtr->pst_Matrix, LINK_C_Matrix, NULL);
			po_Item->mi_NumFields += 3;
		}
	}
	else
	{
		pst_Gizmo = pst_AM->dst_Gizmo;
		for(i = 0; i < pst_AM->l_Number; i++, pst_Gizmo++)
		{
			_itoa(i, sz_Text + 6, 10);
			M_InsertItem(sz_Text, EVAV_EVVIT_String, pst_Gizmo->sz_Name, NULL);
			M_InsertItem1(sz_Text, EVAV_EVVIT_Pointer, &pst_Gizmo->st_Matrix, LINK_C_Matrix, NULL);
			po_Item->mi_NumFields += 2;
		}
	}
}

/*$4
 ***********************************************************************************************************************
    Aim:    Modifier functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Function called when the number of sprites in SpriteList visuel change
 =======================================================================================================================
 */
void VAV_MDF_UpdateGameObject(void *p_Owner, void *_po_Data, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	EVAV_cl_ViewItem	*po_Parent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Item = (EVAV_cl_ViewItem *) _po_Data;
	po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_ENG_GameObjectOriented);
	LINK_UpdatePointer(po_Parent->mp_Data);
	LINK_UpdatePointers();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_OnChangeSndVolFlag(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
    if((*(int*)p_NewValue) & SND_Cte_MdFSoundVol_Spheric)
        (*(int*)p_NewValue) &= ~(SND_Cte_MdFSoundVol_Xaxis|SND_Cte_MdFSoundVol_Yaxis|SND_Cte_MdFSoundVol_Zaxis);
    else
        (*(int*)p_NewValue) |= (SND_Cte_MdFSoundVol_Xaxis|SND_Cte_MdFSoundVol_Yaxis|SND_Cte_MdFSoundVol_Zaxis);
    
    VAV_MDF_UpdateGameObject(p1, p2, p_NewValue, l_OldValue);
}
/**/
void SND_OnChangeSndFxNetIdx(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
    VAV_MDF_UpdateGameObject(p1, p2, p_NewValue, l_OldValue);
}
/**/
void SND_OnSelectSndFxNetSlave(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
	if((*(int *) p_NewValue & MDF_Cte_SndFx_NetSlave))
        (*(int *) p_NewValue) &= ~MDF_Cte_SndFx_NetMaster;
    else
        (*(int *) p_NewValue) |= MDF_Cte_SndFx_NetMaster;
    VAV_MDF_UpdateGameObject(p1, p2, p_NewValue, l_OldValue);
}
/**/
void SND_OnSelectSndFxNetMaster(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
	if((*(int *) p_NewValue & MDF_Cte_SndFx_NetMaster))
        (*(int *) p_NewValue) &= ~MDF_Cte_SndFx_NetSlave;
    else
        (*(int *) p_NewValue) |= MDF_Cte_SndFx_NetSlave;
    VAV_MDF_UpdateGameObject(p1, p2, p_NewValue, l_OldValue);
}

void SND_OnSelectSndStaticPrefetch(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
	if((*(int *) p_NewValue & MDF_Cte_SndPrefetchStatic))
    {
		(*(int *) p_NewValue) &= ~(MDF_Cte_SndPrefetchAuto|MDF_Cte_SndPrefetchManual);
    }
    VAV_MDF_UpdateGameObject(p1, p2, p_NewValue, l_OldValue);
}

void SND_OnSelectSndDynamicPrefetch(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
	if((*(int *) p_NewValue & MDF_Cte_SndPrefetchAuto))
		(*(int *) p_NewValue) &= ~(MDF_Cte_SndPrefetchStatic|MDF_Cte_SndPrefetchManual);
	VAV_MDF_UpdateGameObject(p1, p2, p_NewValue, l_OldValue);
}

void SND_OnSelectSndManualPrefetch(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
	if((*(int *) p_NewValue & MDF_Cte_SndPrefetchManual))
		(*(int *) p_NewValue) &= ~(MDF_Cte_SndPrefetchAuto|MDF_Cte_SndPrefetchStatic);
	VAV_MDF_UpdateGameObject(p1, p2, p_NewValue, l_OldValue);
}

void SND_OnSelectSndFile(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	            ul_fat;
	EVAV_cl_ViewItem	*po_Item;
	EVAV_cl_ViewItem	*po_Parent;
    ESON_cl_Frame       *po_SoundEdi;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	if(*(ULONG *) p_NewValue != BIG_C_InvalidKey)
	{
		ul_fat = BIG_ul_SearchKeyToFat(*(ULONG *) p_NewValue);
		if(ul_fat != BIG_C_InvalidIndex)
		{
            if(!BIG_b_IsFileExtension(ul_fat, EDI_Csz_ExtSModifier))
			{
				MessageBox
				(
					NULL,
					"file type must be SModifier file (.smd), Please choose an other file",
					"Caution",
					MB_OK | MB_ICONSTOP
				);
                *(ULONG *) p_NewValue = BIG_C_InvalidKey;			
			}
		}

        if(l_OldValue && (*(LONG *) p_NewValue != l_OldValue))
        {
	        po_Item = (EVAV_cl_ViewItem *) p2;
	        po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_ENG_GameObjectOriented);

	        po_SoundEdi = (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
	        po_SoundEdi->i_OnMessage(ESON_MESSAGE_GAO_HAS_CHANGED, (ULONG)po_Parent->mp_Data, 0);
        }

    }

	VAV_MDF_UpdateGameObject(p1, p2, p_NewValue, l_OldValue);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_OnSelectWacFile(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_fat;
	char	*pz_Temp, *psz_Name;
	char	asz_Path[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(*(ULONG *) p_NewValue != BIG_C_InvalidKey)
	{
		ul_fat = BIG_ul_SearchKeyToFat(*(ULONG *) p_NewValue);
		if(ul_fat != BIG_C_InvalidIndex)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_fat), asz_Path);
			psz_Name = BIG_NameFile(ul_fat);

			pz_Temp = L_strrchr(psz_Name, '.');
			if(pz_Temp)
			{
                if(L_strcmpi(pz_Temp, EDI_Csz_ExtLoadingSound) && L_strcmpi(pz_Temp, EDI_Csz_ExtSModifier))
				{
					MessageBox(NULL, "You have to choose a WAC or a SMD file, please retry", "Error", MB_OK | MB_ICONSTOP);
					*(ULONG *) p_NewValue = BIG_C_InvalidKey;
				}
			}
		}
	}

	VAV_MDF_UpdateGameObject(p1, p2, p_NewValue, l_OldValue);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ComputeModifierName(MDF_tdst_Modifier *pst_Mod, char *_psz_Name)
{
	if ((!pst_Mod) || (pst_Mod->i->ul_Type >= MDF_C_Modifier_Number ))
	{
		*_psz_Name = 0;
		return;
	}
	L_strcpy( _psz_Name, MDF_gasz_ModifierName[ pst_Mod->i->ul_Type ] );
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EVAV_cl_ListBox::AddModifierAnimatedGAOProperties(POSITION &pos, EVAV_cl_ViewItem	*po_Item, int _AnimType, void *_pParams)
{
    MDF_AnimatedGAO_Interpolation_Types eAnimationTypes = (MDF_AnimatedGAO_Interpolation_Types)_AnimType;
    EVAV_cl_ViewItem *po_NewItem;

    switch(eAnimationTypes)	
    {
    case MDF_AnimatedGAO_Linear:
        {
            GAO_tdst_ModifierAnimatedGAOLinearParams *pParam = (GAO_tdst_ModifierAnimatedGAOLinearParams *)_pParams;
            M_InsertItem("Minimum value", EVAV_EVVIT_Float, &pParam->f_Min, NULL); 
            po_Item->mi_NumFields++;
            M_InsertItem("Maximum value", EVAV_EVVIT_Float, &pParam->f_Max, NULL); 
            po_Item->mi_NumFields++;
            M_InsertItem("Start time", EVAV_EVVIT_Float, &pParam->f_StartTime, NULL); 
            po_Item->mi_NumFields++;
            M_InsertItem("Stop time", EVAV_EVVIT_Float, &pParam->f_StopTime, NULL); 
            po_Item->mi_NumFields++;
            M_InsertItem("Total time", EVAV_EVVIT_Float, &pParam->f_TotalTime, NULL); 
            po_Item->mi_NumFields++;
            M_InsertItem("Go back and forth", EVAV_EVVIT_Bool, &pParam->b_BackAndForth, NULL);
            po_Item->mi_NumFields++;
        }
        break;

    case MDF_AnimatedGAO_Noise:
        {
            GAO_tdst_ModifierAnimatedGAONoiseParams *pParam = (GAO_tdst_ModifierAnimatedGAONoiseParams *)_pParams;
            M_InsertItem("Minimum value", EVAV_EVVIT_Float, &pParam->f_Min, NULL); 
            po_Item->mi_NumFields++;
            M_InsertItem("Maximum value", EVAV_EVVIT_Float, &pParam->f_Max, NULL); 
            po_Item->mi_NumFields++;			
            M_InsertItem("Variation", EVAV_EVVIT_Float, &pParam->f_Var, NULL); 
            po_Item->mi_NumFields++;
            M_InsertItem("Variation speed", EVAV_EVVIT_Float, &pParam->f_VarSpeed, NULL); 
            po_Item->mi_NumFields++;
        }
        break;

    case MDF_AnimatedGAO_Sinus:
        {
            GAO_tdst_ModifierAnimatedGAOSinusParams *pParam = (GAO_tdst_ModifierAnimatedGAOSinusParams *)_pParams;
            M_InsertItem("Angle", EVAV_EVVIT_Float, &pParam->f_Angle, NULL); 
            po_Item->mi_NumFields++;
            M_InsertItem("Total time", EVAV_EVVIT_Float, &pParam->f_TotalTime, NULL); 
            po_Item->mi_NumFields++;
            M_InsertItem("Time bias", EVAV_EVVIT_Float, &pParam->f_TimeBias, NULL); 
            po_Item->mi_NumFields++;
        }
        break;
    }	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OBJAddModifier(POSITION pos, void *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier				**ppst_Mod;
	SPG_tdst_Modifier				*pst_SPG;
	SPG2_tdst_Modifier				*pst_SPG2;
	GPG_tdst_Modifier				*pst_GPG;
	GEO_tdst_ModifierOnduleTonCorps *pst_OTC;
	GEO_tdst_ModifierSnap			*pst_Snap;
	GAO_tdst_ModifierLegLink		*pst_LegLink;
	GEO_tdst_ModifierMorphing		*pst_Morph;
	GAO_tdst_ModifierSDW			*pst_Shadow;
	GAO_tdst_ModifierExplode		*pst_Explode;
	GAO_tdst_ModifierPhoto			*pst_Photo;
	GAO_tdst_ModifierODE			*pst_Mod_ODE;
	GEN_tdst_ModifierSound			*pst_Sound;
	GEN_tdst_ModifierSoundFx		*pst_SoundFx;
    SND_tdst_ModifierSoundVol       *pst_SoundVol;
	GAO_tdst_ModifierSpecialLookAt	*pst_LookAt;
	GAO_tdst_ModifierFOGDY			*pst_FogDy;
	GAO_tdst_ModifierFOGDY_Emiter	*pst_FogDy_Emtr;
	MatrixBore_tdst_Modifier		*pst_MB;
	GAO_tdst_ModifierSaveAddMatrix	*pst_SaveAddMatrix;
	GEO_tdst_ModifierCrush			*pst_Crush;
	FUR_tdst_Modifier				*p_FUR;
#ifdef JADEFUSION
	DYNFUR_tdst_Modifier			*p_DYNFUR;
#endif
	Grid_tdst_Modifier				*pst_GridMdF;
	EVAV_cl_ViewItem				*po_Item, *po_NewItem;
	MDF_tdst_LoadingSound			*pst_LoadingSound;
	GAO_tdst_ModifierBoneRefine		*p_BR;
	GAO_tdst_ModifierBoneMeca		*p_BM;
	FCLONE_tdst_Modifier			*p_FCLONE;
	UVTexWave_tdst_Modifier			*p_UVTexWave;
	char							asz_Name[100];
	int								i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);
	po_Item->mi_NumFields = 0;

	ppst_Mod = (MDF_tdst_Modifier **) _pst_Mod;
	if((*ppst_Mod) == NULL)
	{
		M_InsertItem("Create", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
		po_Item->mi_NumFields++;
		return;
	}
	else
	{
		POSITION InitPos,EndPos;
		InitPos = pos;

		ComputeModifierName(*ppst_Mod, asz_Name);
		//M_InsertItemROColor(asz_Name, NULL, NULL);

		po_NewItem = new EVAV_cl_ViewItem( asz_Name, EVAV_EVVIT_CustSep, *ppst_Mod, EVAV_ReadOnly, LINK_C_MDF_Modifier, 0, 0, 0, po_Item->mx_Color, 1 );
		po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;
		pos = mpo_ListItems->InsertAfter( pos, po_NewItem );
		
		po_Item->mi_NumFields++;
		M_InsertItemColor("Destroy", EVAV_EVVIT_Modifier, _pst_Mod, NULL, 0x00000000);
		po_Item->mi_NumFields++;
		if(!((*ppst_Mod)->ul_Flags & MDF_C_Modifier_NoApply))
		{
			if
			(
				((*ppst_Mod)->i->ul_Type != MDF_C_Modifier_Sound)
			&&	((*ppst_Mod)->i->ul_Type != MDF_C_Modifier_SoundFx)
			&&	((*ppst_Mod)->i->ul_Type != MDF_C_Modifier_SoundVolume)
			&&	((*ppst_Mod)->i->ul_Type != MDF_C_Modifier_SoundLoading)
			&&  ((*ppst_Mod)->i->ul_Type != MDF_C_Modifier_WATER3D)
			&&  ((*ppst_Mod)->i->ul_Type != MDF_C_Modifier_Disturber)
            &&  ((*ppst_Mod)->i->ul_Type != MDF_C_Modifier_SPG2Holder)
			)
			{
				M_InsertItem("Apply in geometry", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
				po_Item->mi_NumFields++;
#ifdef JADEFUSION
//			}
#endif
			    M_InsertItem12("Freeze", EVAV_EVVIT_Flags, &(*ppst_Mod)->ul_Flags, 25, 4, NULL);
			    po_Item->mi_NumFields++;
			}

		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Modifiers parameters edition
		 ---------------------------------------------------------------------------------------------------------------
		 */

		switch((*ppst_Mod)->i->ul_Type)
		{
		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		case MDF_C_Modifier_WATER3D:
			{
				WATER3D_tdst_Modifier	*p_WATER3D;

				p_WATER3D = (WATER3D_tdst_Modifier *) (*ppst_Mod)->p_Data;
				M_InsertItem("Damping", EVAV_EVVIT_Float, &p_WATER3D->fDamping, NULL);
				M_InsertItem("Propagation speed", EVAV_EVVIT_Float, &p_WATER3D->fPropagationSpeed, NULL);
				M_InsertItem("Perturbation boost factor", EVAV_EVVIT_Float, &p_WATER3D->fPerturbanceAmplitudeModifier, NULL);
				M_InsertItem("Impact force attenuation", EVAV_EVVIT_Float, &p_WATER3D->fImpactForceAttenuation, NULL);
				M_InsertItem("Turbulance Factor", EVAV_EVVIT_Int, &p_WATER3D->lTurbulanceFactor, NULL);
				M_InsertItem("Turbulance Amplitude", EVAV_EVVIT_Float, &p_WATER3D->fTurbulanceAmplitude, NULL);
				M_InsertItem("Mesh Density", EVAV_EVVIT_Int, &p_WATER3D->Density, MDF_WATER3D_Modifier_Reset);
				M_InsertItem("Radius Falloff", EVAV_EVVIT_Float, &p_WATER3D->fRadius, NULL);
				M_InsertItem("Radius Damping", EVAV_EVVIT_Float, &p_WATER3D->fDampingOutsideRadius, NULL);
				M_InsertItem("Turbulance Off when outise Radius", EVAV_EVVIT_Bool, &p_WATER3D->bTurbulanceOffIfOutsideRadius, NULL);
				M_InsertItem("Radius Undisturbed", EVAV_EVVIT_Float, &p_WATER3D->fRadiusCut, NULL);
				M_InsertItem("Use Refraction-like Effect", EVAV_EVVIT_Bool, &p_WATER3D->bWaterChrome, NULL);
				M_InsertItem("Water Density", EVAV_EVVIT_Float, &p_WATER3D->fWaterDensity, NULL);
                M_InsertItem("Refraction Intensity", EVAV_EVVIT_Float, &p_WATER3D->fRefractionIntensity, NULL);
                M_InsertItem("Reflection Intensity", EVAV_EVVIT_Float, &p_WATER3D->fReflectionIntensity, NULL);
				M_InsertItem("Fog Intensity", EVAV_EVVIT_Float, &p_WATER3D->fFogIntensity, NULL);
                M_InsertItem("Basemap Opacity", EVAV_EVVIT_Float, &p_WATER3D->fBaseMapOpacity, NULL);
                M_InsertItemRO("Total Polygons", EVAV_EVVIT_Int, &p_WATER3D->iNbPoly, NULL);

				po_NewItem->psz_Help = "...";
				po_Item->mi_NumFields += 18;
			}
			break;

        case MDF_C_Modifier_Disturber:
            {
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                Disturber_tdst_Modifier	*p_Disturber;
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                p_Disturber = (Disturber_tdst_Modifier *) (*ppst_Mod)->p_Data;

                {
                    char asz_temp[128];

                    /* TITLE */
                    sprintf(asz_temp, " WATER 3D");
                    M_InsertItemROColorSpecial(asz_temp, 0, NULL, (int) p_Disturber);
                    po_Item->mi_NumFields++;
                }

                M_InsertItem("Disturber Boost Factor", EVAV_EVVIT_Float, &p_Disturber->fDisturbBoost, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Disturber is static", EVAV_EVVIT_Bool, &p_Disturber->bStaticDisturber, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Static disturber force", EVAV_EVVIT_Float, &p_Disturber->fStaticDisturberForce, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Static disturber delay", EVAV_EVVIT_Float, &p_Disturber->fStaticDisturberDelay, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Disturber Z Offset", EVAV_EVVIT_Float, &p_Disturber->fZOffset, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Float on Water [FOW]", EVAV_EVVIT_Bool, &p_Disturber->bFloatOnWater, MDF_Disturber_CallBack_Refresh);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] velocity", EVAV_EVVIT_Vector, &p_Disturber->vFloatOnWaterInitialVelocity, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] velocity back to initial strength", EVAV_EVVIT_Float, &p_Disturber->fFloatOnWaterVelocityBackToInitialStrength, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] velocity damping", EVAV_EVVIT_Float, &p_Disturber->vFloatOnWaterVelocityDamping, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] water waves strength", EVAV_EVVIT_Float, &p_Disturber->fFloatOnWaterWaveStrength, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] water waves influence", EVAV_EVVIT_Float, &p_Disturber->fFloatOnWaterWaveInfluence, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] Z rotation speed", EVAV_EVVIT_Float, &p_Disturber->fFloatOnWaterInitialZRotationSpeed, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] Z offset", EVAV_EVVIT_Float, &p_Disturber->fFloatOnWaterZOffset, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] Z stability", EVAV_EVVIT_Float, &p_Disturber->fFloatOnWaterZStability, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] banking damping", EVAV_EVVIT_Float, &p_Disturber->fFloatOnWaterBankingDamping, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] evaluation plane in meters", EVAV_EVVIT_Float, &p_Disturber->fFloatOnWaterEvaluationPlaneDelta, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[FOW] use water boundingbox for collision", EVAV_EVVIT_Bool, &p_Disturber->bUseWaterBoundingBoxForCollision, MDF_Disturber_CallBack_Refresh);
                po_Item->mi_NumFields++;

                {
                    char asz_temp[128];

                    /* TITLE */
                    sprintf(asz_temp, " PAG Dispersion");
                    M_InsertItemROColorSpecial(asz_temp, 0, NULL, (int) p_Disturber);
                    po_Item->mi_NumFields++;
                }

                M_InsertItem("PAG Dispersion is Active", EVAV_EVVIT_Bool, &p_Disturber->bPAGDispersionActive, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Radius", EVAV_EVVIT_Float, &p_Disturber->fRadius, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Front Force Translation", EVAV_EVVIT_Float, &p_Disturber->fFrontForceTrans, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Front Force Rotation", EVAV_EVVIT_Float, &p_Disturber->fFrontForceRot, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Rear Force Translation", EVAV_EVVIT_Float, &p_Disturber->fRearForceTrans, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Rear Force Rotation", EVAV_EVVIT_Float, &p_Disturber->fRearForceRot, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("BackDraft Near", EVAV_EVVIT_Float, &p_Disturber->fDraftNear, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("BackDraft Far", EVAV_EVVIT_Float, &p_Disturber->fDraftFar, NULL);
                po_Item->mi_NumFields++;

                /*M_InsertItem("Disturbances generate particules", EVAV_EVVIT_Bool, &p_Disturber->bGenParticles, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[1] Number of particules", EVAV_EVVIT_Int, &p_Disturber->nbParticules1, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[1] Modulate number of particules with perturbation strength", EVAV_EVVIT_Bool, &p_Disturber->bPonderateParticulesWithForce1, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[1] Particule generator", EVAV_EVVIT_ParticleGen, &p_Disturber->pParticuleGenerator1, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[2] Number of particules", EVAV_EVVIT_Int, &p_Disturber->nbParticules2, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[2] Modulate number of particules with perturbation strength", EVAV_EVVIT_Bool, &p_Disturber->bPonderateParticulesWithForce2, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[2] Particule generator", EVAV_EVVIT_ParticleGen, &p_Disturber->pParticuleGenerator2, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[3] Number of particules", EVAV_EVVIT_Int, &p_Disturber->nbParticules3, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[3] Modulate number of particules with perturbation strength", EVAV_EVVIT_Bool, &p_Disturber->bPonderateParticulesWithForce3, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("[3] Particule generator", EVAV_EVVIT_ParticleGen, &p_Disturber->pParticuleGenerator3, NULL);
                po_Item->mi_NumFields++;*/

                po_NewItem->psz_Help = "...";
            }
            break;
#ifdef JADEFUSION
        case MDF_C_Modifier_SPG2Holder:
            {
                SPG2Holder_tdst_Modifier *p_SPG2Holder = (SPG2Holder_tdst_Modifier *) (*ppst_Mod)->p_Data;

                M_InsertItem("Rebuild", EVAV_EVVIT_Modifier, _pst_Mod, MDF_SPG2Holder_CallBack_Reset);
                po_Item->mi_NumFields++;

                M_InsertItemRO("Number of instances", EVAV_EVVIT_Int, &p_SPG2Holder->NumberOfSPG2Entries, NULL);
                po_Item->mi_NumFields++;

                po_NewItem->psz_Help = "...";
            }
            break;
#endif
        
        /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		case MDF_C_Modifier_GRID:
			pst_GridMdF = (Grid_tdst_Modifier *) (*ppst_Mod)->p_Data;
			M_InsertItem("Copy Grid", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
			po_Item->mi_NumFields++;
			M_InsertItem("Paste Grid", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
			po_Item->mi_NumFields++;
			M_InsertItem("Rotate Pattern", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
			po_Item->mi_NumFields++;
			break;


		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_Snap:
			pst_Snap = (GEO_tdst_ModifierSnap *) (*ppst_Mod)->p_Data;
			M_InsertItem("Re-Snap", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
			po_Item->mi_NumFields++;
			M_InsertItemRO("Number Of Snapped Points", EVAV_EVVIT_Int, &pst_Snap->ul_NbPoints, NULL);
			po_Item->mi_NumFields++;
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_MatrixBore:
			pst_MB = (MatrixBore_tdst_Modifier *) (*ppst_Mod)->p_Data;
			M_InsertItem("Strenght", EVAV_EVVIT_Float, &pst_MB->Strenght, NULL);
			po_Item->mi_NumFields++;
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_OnduleTonCorps:
			pst_OTC = (GEO_tdst_ModifierOnduleTonCorps *) (*ppst_Mod)->p_Data;
			M_InsertItem1("Flags", EVAV_EVVIT_SubStruct, &pst_OTC->ul_Flags, LINK_C_GEO_ModifierOTCFlag, NULL);
			M_InsertItem("Amplitude", EVAV_EVVIT_Float, &pst_OTC->f_Amplitude, NULL);
			M_InsertItem("Angle", EVAV_EVVIT_Float, &pst_OTC->f_Angle, NULL);
			M_InsertItem("Delta", EVAV_EVVIT_Float, &pst_OTC->f_Delta, NULL);
			M_InsertItem("Factor", EVAV_EVVIT_Float, &pst_OTC->f_Factor, NULL);
			po_Item->mi_NumFields += 5;
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_SPG:
			pst_SPG = (SPG_tdst_Modifier *) (*ppst_Mod)->p_Data;
			M_InsertItem1("FLags", EVAV_EVVIT_SubStruct, &pst_SPG->ulFlags, LINK_C_SPGFlags, NULL);
			M_InsertItemRO("Generated sprites", EVAV_EVVIT_Int, &pst_SPG->ulNumberofgeneratedSprites, NULL);
			M_InsertItemRO("Used triangles", EVAV_EVVIT_Int, &pst_SPG->ulNumberofUsedTriangles, NULL);
			M_InsertItem("Global Size", EVAV_EVVIT_Float, &pst_SPG->f_GlobalSize, NULL);
			M_InsertItem("Global Ratio", EVAV_EVVIT_Float, &pst_SPG->f_GlobalRatio, NULL);

			M_InsertItem("Extraction", EVAV_EVVIT_Float, &pst_SPG->f_Extraction, NULL);
			M_InsertItem("Noise", EVAV_EVVIT_Float, &pst_SPG->f_Noise, NULL);
			M_InsertItem("Threshold", EVAV_EVVIT_Float, &pst_SPG->f_ThresholdMin, NULL);
			M_InsertItem("Depth Max", EVAV_EVVIT_Int, &pst_SPG->ulMaxDepth, NULL);
			M_InsertItem("LOD Correction", EVAV_EVVIT_Float, &pst_SPG->fLODCorrectionFactor, NULL);

			M_InsertItem("Special fog near", EVAV_EVVIT_Float, &pst_SPG->fSpecialFogNear, NULL);
			M_InsertItem("Special fog far", EVAV_EVVIT_Float, &pst_SPG->fSpecialFogFar, NULL);
			M_InsertItem("Special fog Color", EVAV_EVVIT_Color, &pst_SPG->ulSpecialFogColor, NULL);
			M_InsertItem1("Sub material mask", EVAV_EVVIT_SubStruct, &pst_SPG->ulSubMaterialMask, LINK_C_FlagsSelect32, NULL);
			M_InsertItem1("Sprite Mapper 0", EVAV_EVVIT_SubStruct, &pst_SPG->stSED[0], LINK_C_SPG_SpriteMapper, NULL);

			M_InsertItem1("Sprite Mapper 1", EVAV_EVVIT_SubStruct, &pst_SPG->stSED[1], LINK_C_SPG_SpriteMapper, NULL);
			M_InsertItem1("Sprite Mapper 2", EVAV_EVVIT_SubStruct, &pst_SPG->stSED[2], LINK_C_SPG_SpriteMapper, NULL);
			M_InsertItem1("Sprite Mapper 3", EVAV_EVVIT_SubStruct, &pst_SPG->stSED[3], LINK_C_SPG_SpriteMapper, NULL);
			po_Item->mi_NumFields += 15 + 3;
			break;


		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		case MDF_C_Modifier_FogDyn:
			pst_FogDy = (GAO_tdst_ModifierFOGDY *) (*ppst_Mod)->p_Data;

			M_InsertItem("Number Of Sprites", EVAV_EVVIT_Int, &pst_FogDy->ulNumberOfActiveSprites, MDF_FOGDY_CallBack_Refresh);
			M_InsertItem("Generation rate", EVAV_EVVIT_Float, &pst_FogDy->GenerationRate, NULL);
			M_InsertItem("Height", EVAV_EVVIT_Float, &pst_FogDy->ZSize, NULL);
			M_InsertItem("sub-material num 1", EVAV_EVVIT_Int, &pst_FogDy->SubMaterialNum1, NULL);
			M_InsertItem("sub-material num 2", EVAV_EVVIT_Int, &pst_FogDy->SubMaterialNum2, NULL);
			M_InsertItem("sub-material num 3", EVAV_EVVIT_Int, &pst_FogDy->SubMaterialNum3, NULL);
			M_InsertItem("sub-material num 4", EVAV_EVVIT_Int, &pst_FogDy->SubMaterialNum4, NULL);
			po_Item->mi_NumFields += 3;
			M_InsertItem1("Active channels", EVAV_EVVIT_SubStruct, &pst_FogDy->ActiveChannel, LINK_C_ChannelFlag, NULL);

			M_InsertItem("trigger Speed", EVAV_EVVIT_Float, &pst_FogDy->SpeedStart, NULL);
			M_InsertItem("max speed", EVAV_EVVIT_Float, &pst_FogDy->MaxSpeed, NULL);
			po_Item->mi_NumFields += 2;

			

			M_InsertItem1234
			(
				"Collide model",
				EVAV_EVVIT_ConstInt,
				&pst_FogDy->CollisionMode,
				0,
				0,
				1,
				MDF_FOGDY_2_CallBack_Refresh,
				(int) "Classical\n0\nDust\n1\nVortex\n2\nSpark\n3\n"
			);
			if (pst_FogDy->CollisionMode == 1)
			{
				M_InsertItem("Generation lenght", EVAV_EVVIT_Float, &pst_FogDy->FrictionLenght, NULL);
				po_Item->mi_NumFields ++;
			}
			if (pst_FogDy->CollisionMode == 3) /* Spark Mode */
			{
				M_InsertItem("Colors", EVAV_EVVIT_Color, &pst_FogDy->Colors1, NULL);
				M_InsertItem("Time Phase 1", EVAV_EVVIT_Float, &pst_FogDy->TimePhase1, NULL);
				M_InsertItem("Time Phase 2", EVAV_EVVIT_Float, &pst_FogDy->TimePhase2, NULL);
				M_InsertItem("Speed factor Min", EVAV_EVVIT_Float, &pst_FogDy->SpeedMin, NULL);
				M_InsertItem("Speed factor Max", EVAV_EVVIT_Float, &pst_FogDy->SpeedMax, NULL);
				M_InsertItem("With", EVAV_EVVIT_Float, &pst_FogDy->SizeMin, NULL);
				M_InsertItem("Lenght multiplier", EVAV_EVVIT_Float, &pst_FogDy->SizeMax, NULL);
				M_InsertItem("Friction", EVAV_EVVIT_Float, &pst_FogDy->FrictionGrow, NULL);
				M_InsertItem("Gravity", EVAV_EVVIT_Float, &pst_FogDy->Gravitiy, NULL);
				M_InsertItem("Time Variance", EVAV_EVVIT_Float, &pst_FogDy->TimeVariance, NULL);
/*
				M_InsertItem("Push Power", EVAV_EVVIT_Float, &pst_FogDy->PushPower, NULL);
				M_InsertItem("Extraction speed", EVAV_EVVIT_Float, &pst_FogDy->ExtractionSpeed, NULL);
				M_InsertItem("Colors2", EVAV_EVVIT_Color, &pst_FogDy->Colors2, NULL);
				M_InsertItem("Colors3", EVAV_EVVIT_Color, &pst_FogDy->Colors3, NULL);
				M_InsertItem("Growing Min", EVAV_EVVIT_Float, &pst_FogDy->GrowingMin, NULL);
				M_InsertItem("Growing Max", EVAV_EVVIT_Float, &pst_FogDy->GrowingMax, NULL);
				M_InsertItem("Friction Speed", EVAV_EVVIT_Vector,&pst_FogDy->FrictionSpeed, NULL);
*/
				po_Item->mi_NumFields += 23 - 7;
			} else
			{
				M_InsertItem("Push Power", EVAV_EVVIT_Float, &pst_FogDy->PushPower, NULL);
				M_InsertItem("Extraction speed", EVAV_EVVIT_Float, &pst_FogDy->ExtractionSpeed, NULL);
				M_InsertItem("Colors1", EVAV_EVVIT_Color, &pst_FogDy->Colors1, NULL);
				M_InsertItem("Colors2", EVAV_EVVIT_Color, &pst_FogDy->Colors2, NULL);
				M_InsertItem("Colors3", EVAV_EVVIT_Color, &pst_FogDy->Colors3, NULL);
				M_InsertItem("Time Phase 1", EVAV_EVVIT_Float, &pst_FogDy->TimePhase1, NULL);
				M_InsertItem("Time Phase 2", EVAV_EVVIT_Float, &pst_FogDy->TimePhase2, NULL);
				M_InsertItem("Speed Min", EVAV_EVVIT_Float, &pst_FogDy->SpeedMin, NULL);
				M_InsertItem("Speed Max", EVAV_EVVIT_Float, &pst_FogDy->SpeedMax, NULL);
				M_InsertItem("Size Min", EVAV_EVVIT_Float, &pst_FogDy->SizeMin, NULL);
				M_InsertItem("Size Max", EVAV_EVVIT_Float, &pst_FogDy->SizeMax, NULL);
				M_InsertItem("Growing Min", EVAV_EVVIT_Float, &pst_FogDy->GrowingMin, NULL);
				M_InsertItem("Growing Max", EVAV_EVVIT_Float, &pst_FogDy->GrowingMax, NULL);
				M_InsertItem("Friction Grow", EVAV_EVVIT_Float, &pst_FogDy->FrictionGrow, NULL);
				M_InsertItem("Friction Speed", EVAV_EVVIT_Vector,&pst_FogDy->FrictionSpeed, NULL);
				M_InsertItem("Gravity", EVAV_EVVIT_Float, &pst_FogDy->Gravitiy, NULL);
				M_InsertItem("Time Variance", EVAV_EVVIT_Float, &pst_FogDy->TimeVariance, NULL);
				po_Item->mi_NumFields += 23;
			}
			

			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		case MDF_C_Modifier_FogDyn_Emiter:
			{
				u32 CounterL;
				pst_FogDy_Emtr = (GAO_tdst_ModifierFOGDY_Emiter *) (*ppst_Mod)->p_Data;
				
				M_InsertItem("Draw spheres", EVAV_EVVIT_Bool, &pst_FogDy_Emtr->ulShowInfo, NULL);
				po_Item->mi_NumFields ++;
				M_InsertItem1("Active channels", EVAV_EVVIT_SubStruct, &pst_FogDy_Emtr->ActiveChannel, LINK_C_ChannelFlag, NULL);
				po_Item->mi_NumFields ++;
				M_InsertItem("Number Of Bones (max 8)", EVAV_EVVIT_Int, &pst_FogDy_Emtr->ActiveBoneNumber, NULL);
				po_Item->mi_NumFields ++;
				for (CounterL = 0;CounterL < 8; CounterL ++)
				{
					M_InsertItem("Bones ID", EVAV_EVVIT_Int, &pst_FogDy_Emtr->BoneID[CounterL], MDF_FOGDY_EMTR_CallBack_Refresh);
					M_InsertItem("Bones Radius", EVAV_EVVIT_Float, &pst_FogDy_Emtr->BoneRadius[CounterL], MDF_FOGDY_EMTR_CallBack_Refresh);
					M_InsertItem("Bone sub-pos", EVAV_EVVIT_Vector, &pst_FogDy_Emtr->BoneDelta[CounterL], MDF_FOGDY_EMTR_CallBack_Refresh);
					po_Item->mi_NumFields += 3;
				}
			}
			break;
		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		case MDF_C_Modifier_Half_Angle:
			{
				p_BR = (GAO_tdst_ModifierBoneRefine	 *) (*ppst_Mod)->p_Data;
				M_InsertItem("Torsion only", EVAV_EVVIT_Bool, &p_BR ->ulMode, NULL);
				M_InsertItem("Interpolation value", EVAV_EVVIT_Float, &p_BR ->fInterpolValue, NULL);
				po_Item->mi_NumFields += 2;
			}
			break;
		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		case MDF_C_Modifier_BoneMeca:
			{
				u32 CounterL;
				p_BM = (GAO_tdst_ModifierBoneMeca *) (*ppst_Mod)->p_Data;
				M_InsertItem("Draw excluders", EVAV_EVVIT_Bool, &p_BM->ulDrawBones, NULL);
				M_InsertItem("Number Of Excluders", EVAV_EVVIT_Int, &p_BM->ulNumberOfExcluders, MDF_BONEMECA_EMTR_CallBack_Refresh);
				po_Item->mi_NumFields += 2;
				for (CounterL = 0;CounterL < p_BM->ulNumberOfExcluders; CounterL ++)
				{
					M_InsertItem("Bone ID", EVAV_EVVIT_Int, &p_BM->stBM[CounterL].ulBoneRef, NULL);
					M_InsertItem("Bone Radius", EVAV_EVVIT_Vector, &p_BM->stBM[CounterL].SphereRadius, NULL);
					M_InsertItem("Bone Pos", EVAV_EVVIT_Vector, &p_BM->stBM[CounterL].DeltaPos, NULL);
					po_Item->mi_NumFields += 3;
				}
				M_InsertItem("Number Of Bones", EVAV_EVVIT_Int, &p_BM->ulNumberOfMovers, MDF_BONEMECA_EMTR_CallBack_Refresh);
				po_Item->mi_NumFields += 1;
				for (CounterL = 0;CounterL < p_BM->ulNumberOfMovers; CounterL ++)
				{
					M_InsertItem("Bone ID", EVAV_EVVIT_Int, &p_BM->stBR[CounterL], NULL);
					po_Item->mi_NumFields += 1;
				}
				M_InsertItem("Gravity", EVAV_EVVIT_Float, &p_BM->fGravity, NULL);
				M_InsertItem("Friction", EVAV_EVVIT_Float, &p_BM->fFriction, NULL);
				M_InsertItem("Masse repartition", EVAV_EVVIT_Float, &p_BM->fRenormToFatherFactor, NULL);
				po_Item->mi_NumFields += 3;
			}
			break;
		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_SpriteMapper2:
				pst_SPG2 = (SPG2_tdst_Modifier *) (*ppst_Mod)->p_Data;
				M_InsertItemRO("Generated sprites", EVAV_EVVIT_Int, &pst_SPG2->ulNumberofgeneratedSprites, MDF_SPG2_CallBack_Refresh);
				M_InsertItemRO("Used triangles", EVAV_EVVIT_Int, &pst_SPG2->ulNumberofUsedTriangles, MDF_SPG2_CallBack_Refresh);
				M_InsertItemRO("Total number", EVAV_EVVIT_Int, &SPG2_WHOLESCENENUM, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Gloabal Limit", EVAV_EVVIT_Int, &SPG2_PrimitivLimit, NULL);
				M_InsertItem1("Flags", EVAV_EVVIT_SubStruct, &pst_SPG2->ulFlags, LINK_C_SPG2Flags, MDF_SPG2_CallBack_Refresh);
				M_InsertItem1("Flags (bis)", EVAV_EVVIT_SubStruct, &pst_SPG2->ulFlags1, LINK_C_SPG2Flags1, MDF_SPG2_CallBack_Refresh);

				M_InsertItem("Global Size", EVAV_EVVIT_Float, &pst_SPG2->f_GlobalSize, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Sprite Size", EVAV_EVVIT_Float, &pst_SPG2->f_SpriteSize, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Sprite Generator Radius", EVAV_EVVIT_Float, &pst_SPG2->f_SpriteGeneratorRadius, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("V shift", EVAV_EVVIT_Float, &pst_SPG2->VShift, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Trapèze", EVAV_EVVIT_Float, &pst_SPG2->fTrapeze, MDF_SPG2_CallBack_Refresh);

				M_InsertItem("Global Ratio", EVAV_EVVIT_Float, &pst_SPG2->f_GlobalRatio, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Density", EVAV_EVVIT_Float, &pst_SPG2->f_Density, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Noise", EVAV_EVVIT_Float, &pst_SPG2->f_Noise, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Alpha Thres hold", EVAV_EVVIT_Int, &pst_SPG2->AlphaThreshold, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Extraction", EVAV_EVVIT_Float, &pst_SPG2->fExtraction, MDF_SPG2_CallBack_Refresh);

				M_InsertItem("Extraction Of Horizontal Plane", EVAV_EVVIT_Float, &pst_SPG2->fExtractionOfHorizontalPlane, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Number Of Segments", EVAV_EVVIT_Int, &pst_SPG2->NumberOfSegments, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Number Of Sprites", EVAV_EVVIT_Int, &pst_SPG2->NumberOfSprites, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Texture tile", EVAV_EVVIT_Int, &pst_SPG2->TileNumber, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Anim Texture tile X", EVAV_EVVIT_Int, &pst_SPG2->AnimTextureTileUPo2, MDF_SPG2_CallBack_Refresh);

				M_InsertItem("Anim Texture tile Y", EVAV_EVVIT_Int, &pst_SPG2->AnimTextureTileVPo2, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Anim Texture Speed", EVAV_EVVIT_Float, &pst_SPG2->AnimTextureSpeed, MDF_SPG2_CallBack_Refresh);

				M_InsertItem("Unic ID", EVAV_EVVIT_Int, &pst_SPG2->UniqID, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Base for noise", EVAV_EVVIT_Int, &pst_SPG2->BaseNoise, MDF_SPG2_CallBack_Refresh);

				M_InsertItemROColor("*** Inertia ***",NULL, MDF_SPG2_CallBack_Refresh);
				M_InsertItem1234
				(
					"Inertia : Preset",
					EVAV_EVVIT_ConstInt,
					&pst_SPG2->Preset,
					0,
					0,
					1,
					MDF_SPG2_CallBack_Refresh,
					(int) "Default (following values)\n0\nfern (Fougère)\n1\nLong leaves\n2\nSmall leaves\n3\nGrass\n4\n"
				);
				M_InsertItem("Inertia : Gravity", EVAV_EVVIT_Float, &pst_SPG2->Gravity, MDF_SPG2_CallBack_Refresh);

				M_InsertItem("Inertia : Spring strenght", EVAV_EVVIT_Float, &pst_SPG2->SringStrenght, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Inertia : SpeedAbsorbtion ", EVAV_EVVIT_Float, &pst_SPG2->SpeedAbsorbtion , MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Inertia : Freedom", EVAV_EVVIT_Float, &pst_SPG2->Freedom, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Inertia : Wind sensitivity", EVAV_EVVIT_Float, &pst_SPG2->WindSensibility, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Inertia : Spherecollide radius factor", EVAV_EVVIT_Float, &pst_SPG2->SphereCollideRadius, MDF_SPG2_CallBack_Refresh);


				M_InsertItemROColor("*** LOD ***",NULL, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("LOD Near factor", EVAV_EVVIT_Float, &pst_SPG2->f_Near, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("LOD Far factor", EVAV_EVVIT_Float, &pst_SPG2->f_Far, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("LOD2 Near factor", EVAV_EVVIT_Float, &pst_SPG2->f_Near2, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("LOD2 Far factor", EVAV_EVVIT_Float, &pst_SPG2->f_Far2, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Sub Material", EVAV_EVVIT_Int, &pst_SPG2->ulSubMaterialNum, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Ponderation selctor for Alpha", EVAV_EVVIT_Int, &pst_SPG2 -> ulAlphaFromPondSelector, MDF_SPG2_CallBack_Refresh);

				M_InsertItem("Grid filter", EVAV_EVVIT_Int, &pst_SPG2 ->GridFilter, MDF_SPG2_CallBack_Refresh);
				M_InsertItem("Grid noise", EVAV_EVVIT_Float, &pst_SPG2 ->GridNoise, MDF_SPG2_CallBack_Refresh);
				po_NewItem->psz_Help = "Choose from where the texture is taken\n First multitexture level will define vertical faces,\n Second multitexture level will define horizontal face";
				M_InsertItem1("Sub material mask", EVAV_EVVIT_SubStruct, &pst_SPG2->ulSubMaterialMask, LINK_C_FlagsSelect32, MDF_SPG2_CallBack_Refresh);
#ifdef JADEFUSION
                M_InsertItemROColor("*** Heat Shimmering ***",NULL, NULL);
                M_InsertItem("Noise Pixel Size",    EVAV_EVVIT_Float, &pst_SPG2->f_HeatShimmerNoisePixelSize, NULL );
                M_InsertItem("Scroll Speed",        EVAV_EVVIT_Float, &pst_SPG2->f_HeatShimmerScrollSpeed, NULL);
                M_InsertItem("Range",               EVAV_EVVIT_Float, &pst_SPG2->f_HeatShimmerRange, NULL);
                M_InsertItem("Height",              EVAV_EVVIT_Float, &pst_SPG2->f_HeatShimmerHeight, NULL);
                M_InsertItem("Opacity",             EVAV_EVVIT_Float, &pst_SPG2->f_HeatShimmerOpacity, NULL);
#endif
				po_Item->mi_NumFields += 43;

#ifdef JADEFUSION			
                M_InsertItem("SPG2 Holder", EVAV_EVVIT_GO, &pst_SPG2->pSPG2Holder, NULL); 
                po_Item->mi_NumFields++;
#endif
				break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_GPG:
			pst_GPG = (GPG_tdst_Modifier *) (*ppst_Mod)->p_Data;
			for(i = 0; i < GPG_MaxGeom; i++)
			{
				sprintf(asz_Name, "Geom %d", i);
				M_InsertItem1(asz_Name, EVAV_EVVIT_SubStruct, &pst_GPG->apt_Geoms[i], LINK_C_GPG_Geom, NULL);
				po_Item->mi_NumFields++;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_Shadow:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				char	uc_ZobiLaMouch[] = "*.tga,*.jpg,*.bmp";
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_Shadow = (GAO_tdst_ModifierSDW *) (*ppst_Mod)->p_Data;
				M_InsertItem1
				(
					"Shadow Generation Options",
					EVAV_EVVIT_SubStruct,
					&pst_Shadow->ulFlags,
					LINK_C_GAO_ModifierSDWFlag,
					0
				);
				M_InsertItem4
				(
					"Projection Method",
					EVAV_EVVIT_ConstInt,
					&pst_Shadow->ulProjectionMethod,
					(int)
						"Plane XY\n0\nPlane XZ\n1\nPlane YZ\n2\nCylinder X\n3\nCylinder Y\n4\nCylinder Z\n5\nSpherical\n6\n"
				);
				M_InsertItem
				(
					"Texture Used",
					EVAV_EVVIT_Key,
					&pst_Shadow->TextureUsed,
					MDF_SDW_CallBack_Refresh3DEngineAndReloadTExture
				);
				po_NewItem->psz_Help = "Texture fo shadow. Clic for change";
				M_InsertItem("X Factor", EVAV_EVVIT_Float, &pst_Shadow->XSizeFactor, NULL);
				po_NewItem->psz_Help = "Lenght of the shadow";
				M_InsertItem("Y Factor", EVAV_EVVIT_Float, &pst_Shadow->YSizeFactor, NULL);
				po_NewItem->psz_Help = "Width of the shadow.\nThis field is ignored if projection method is spherical";
				M_InsertItem("Center", EVAV_EVVIT_Vector, &pst_Shadow->stCenter, LINK_CallBack_Refresh3DEngine);
				po_NewItem->psz_Help = "Define the center of the shadow.\nDefined in the local coordinate";
				M_InsertItem("Z attenuation Factor", EVAV_EVVIT_Float, &pst_Shadow->ZAttenuationFactor, NULL);
				po_NewItem->psz_Help = "Shadow Deep in metters";
				M_InsertItem("Z start", EVAV_EVVIT_Float, &pst_Shadow->ZStart, NULL);
				po_NewItem->psz_Help = "Shadow Deep in metters";
				M_InsertItem("Shadow Color", EVAV_EVVIT_Color, &pst_Shadow->ulShadowColor, NULL);
				M_InsertItem("Exclusive Light", EVAV_EVVIT_GO, &pst_Shadow->pst_LightGO, NULL);
				po_Item->mi_NumFields += 10;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_XMEN:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				char					uc_ZobiLaMouch[] = "*.tga,*.jpg,*.bmp";
				GAO_tdst_ModifierXMEN	*p_XMEN;
				ULONG					XBONECOUNTER;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				p_XMEN = (GAO_tdst_ModifierXMEN *) (*ppst_Mod)->p_Data;
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Global moves", EVAV_EVVIT_Bool, &p_XMEN->bGlobalMatrix, 0, MDF_Xmen_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Smooth", EVAV_EVVIT_Bool, &p_XMEN->bSmoothed, 0, MDF_Xmen_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem1
				(
					"Material is transparent",
					EVAV_EVVIT_Bool,
					&p_XMEN->bMaterialIsTransparent,
					0,
					MDF_Xmen_CallBack_Refresh
				);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("XMEN is centered", EVAV_EVVIT_Bool, &p_XMEN->bCentered, 0, MDF_Xmen_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Turn texture 90", EVAV_EVVIT_Bool, &p_XMEN->bTurn90, 0, MDF_Xmen_CallBack_Refresh);
				po_NewItem->psz_Help = "Minimal time of creation.";
				M_InsertItem1("DT min", EVAV_EVVIT_Float, &p_XMEN->fDTMin, 0, MDF_Xmen_CallBack_Refresh);
				po_NewItem->psz_Help = "ID Used for IA detection";
				M_InsertItem1("User ID", EVAV_EVVIT_Int, &p_XMEN->ulUserID, 0, MDF_Xmen_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem1
				(
					"XBONES Material",
					EVAV_EVVIT_Key,
					&p_XMEN->bk_MaterialUsed,
					0,
					MDF_Xmen_CallBack_Refresh
				);
				po_NewItem->psz_Help = "...";
				M_InsertItem1
				(
					"Number of XBONES",
					EVAV_EVVIT_Int,
					&p_XMEN->ulNumber_Of_Chhlaahhh,
					0,
					MDF_Xmen_CallBack_Refresh
				);

				M_InsertItem4clbk
				(
					"Projection Method",
					EVAV_EVVIT_ConstInt,
					&p_XMEN->ulProjectionMethod,
					MDF_Xmen_CallBack_Refresh,
					(int)
						"X axis\n2\nY axis\n1\nZ axis\n0\nSpeed axis - LookAt\n3\nSpeed axis - Perpendicular to X\n4\nSpeed axis - Perpendicular to Y\n5\nSpeed axis - Perpendicular to Z\n6\n"
				);

				for(XBONECOUNTER = 0; XBONECOUNTER < p_XMEN->ulNumber_Of_Chhlaahhh; XBONECOUNTER++)
				{
					/*~~~~~~~~~~~~~~~~~~*/
					char	StringLoc[64];
					/*~~~~~~~~~~~~~~~~~~*/

					po_NewItem->psz_Help = "...";
					sprintf(StringLoc, "XBONE %d reference", XBONECOUNTER);
					M_InsertItem1(StringLoc, EVAV_EVVIT_Int, &p_XMEN->p_st_Chhlaahhh[XBONECOUNTER].ulBonesNum, 0, NULL);
					po_NewItem->psz_Help = "...";
					sprintf(StringLoc, "XBONE %d Lenght", XBONECOUNTER);
					M_InsertItem1(StringLoc, EVAV_EVVIT_Float, &p_XMEN->p_st_Chhlaahhh[XBONECOUNTER].fLenght, 0, NULL);
				}

				po_Item->mi_NumFields += 10;
				po_Item->mi_NumFields += p_XMEN->ulNumber_Of_Chhlaahhh * 2;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_ROTR:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				GAO_tdst_ModifierROTR	*p_ROTR;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				p_ROTR = (GAO_tdst_ModifierROTR *) (*ppst_Mod)->p_Data;
				M_InsertItem1("Retard X", EVAV_EVVIT_Float, &p_ROTR->f_RetardX, 0, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Retard Y", EVAV_EVVIT_Float, &p_ROTR->f_RetardY, 0, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Retard Z", EVAV_EVVIT_Float, &p_ROTR->f_RetardZ, 0, NULL);
				po_NewItem->psz_Help = "...";
			}

			po_Item->mi_NumFields += 3;
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_SNAKE:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				GAO_tdst_ModifierSNAKE	*p_ROTR;
				char					az[512];
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				p_ROTR = (GAO_tdst_ModifierSNAKE *) (*ppst_Mod)->p_Data;
				M_InsertItem12("Take Y", EVAV_EVVIT_Flags, &(*ppst_Mod)->ul_Flags, 0, 4, NULL);
				M_InsertItem1("Num Bones", EVAV_EVVIT_Int, &p_ROTR->i_NumBones, 0, NULL);
				po_NewItem->psz_Help = "...";
				for(int i = 0; i < SNAKE_MXBONES; i++)
				{
					sprintf(az, "Bone %d", i);
					M_InsertItem1(az, EVAV_EVVIT_Int, &p_ROTR->ai_Bones[i], 0, NULL);
					po_NewItem->psz_Help = "...";
				}

				M_InsertItem1("Inertie", EVAV_EVVIT_Float, &p_ROTR->f_Inertie, 0, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Blend Dist", EVAV_EVVIT_Float, &p_ROTR->f_BlendDist, 0, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Attenuation", EVAV_EVVIT_Float, &p_ROTR->f_Attenuation, 0, NULL);
				po_NewItem->psz_Help = "...";

				M_InsertItem1("Use Model 2", EVAV_EVVIT_Bool, &p_ROTR->ul_Flags, 0, NULL);
				po_NewItem->psz_Help = "...";


			}

			po_Item->mi_NumFields += 6 + SNAKE_MXBONES;
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_Lazy:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				GAO_tdst_ModifierLazy	*p_LAZY;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				p_LAZY = (GAO_tdst_ModifierLazy	*) (*ppst_Mod)->p_Data;

				M_InsertItem12("Trans", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 0, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem12("Rotation", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 1, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem12("Scale", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 2, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem12("Lazy in parent space", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 3, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";

				M_InsertItem12("Inherit Trans", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 4, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "inherit if FATHER is a son";
				M_InsertItem12("Inherit Rotation", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 5, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "inherit if FATHER is a son";
				M_InsertItem12("Inherit Scale", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 6, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "inherit if FATHER is a son";

				M_InsertItem12("Inherit Flags", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 7, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "inherit if FATHER have the same modifier";
				M_InsertItem12("Inherit Factor", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 8, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "inherit if FATHER have the same modifier";

				M_InsertItem12("Draw info", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 9, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "..";
				M_InsertItem12("Use constraint BV", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 10, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "..";
				M_InsertItem12("Inherit BV", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 11, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "..";

				M_InsertItem("BV min X", EVAV_EVVIT_Float, &p_LAZY->stMinBV.x, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem("BV max X", EVAV_EVVIT_Float, &p_LAZY->stMaxBV.x, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem("BV min Y", EVAV_EVVIT_Float, &p_LAZY->stMinBV.y, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem("BV max Y", EVAV_EVVIT_Float, &p_LAZY->stMaxBV.y, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem("BV min Z", EVAV_EVVIT_Float, &p_LAZY->stMinBV.z, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem("BV max Z", EVAV_EVVIT_Float, &p_LAZY->stMaxBV.z, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";

				M_InsertItem12("Semi mecanical model", EVAV_EVVIT_Flags, &p_LAZY->ulFlags, 12, 4, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "..";

				M_InsertItem("Lazy Factor", EVAV_EVVIT_Float, &p_LAZY->LazyFactor, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";

				M_InsertItem("Gravity effect", EVAV_EVVIT_Float, &p_LAZY->Gravity, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "only in Lazy in parent space mode";

				M_InsertItem("Friction effect", EVAV_EVVIT_Float, &p_LAZY->Friction, MDF_Lazy_CallBack_Refresh);
				po_NewItem->psz_Help = "...";


				po_Item->mi_NumFields += 21;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_XMEC:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				GAO_tdst_ModifierXMEC	*p_XMEC;
				int						i;
				char					az[100];
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				p_XMEC = (GAO_tdst_ModifierXMEC *) (*ppst_Mod)->p_Data;
				M_InsertItem12("Take Y", EVAV_EVVIT_Flags, &p_XMEC->ulFlags, 0, 4, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem12("Auto Compute Dist", EVAV_EVVIT_Flags, &p_XMEC->ulFlags, 1, 4, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem("Reference Bone", EVAV_EVVIT_Int, &p_XMEC->i_RefBone, MDF_Xmec_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Dist", EVAV_EVVIT_Float, &p_XMEC->fDistance, 0, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Gravity", EVAV_EVVIT_Float, &p_XMEC->fGravity, 0, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Ground", EVAV_EVVIT_Float, &p_XMEC->fGround, 0, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Ground friction", EVAV_EVVIT_Float, &p_XMEC->fGroundFriction, 0, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Tens", EVAV_EVVIT_Float, &p_XMEC->fTension, 0, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem1("Elasticity", EVAV_EVVIT_Float, &p_XMEC->fElasticity, 0, NULL);
				po_NewItem->psz_Help = "...";
				M_InsertItem("Num Bones", EVAV_EVVIT_Int, &p_XMEC->i_NumBones, MDF_Xmec_CallBack_Refresh);
				po_NewItem->psz_Help = "...";
				po_Item->mi_NumFields += 10;

				for(i = 0; i < 32; i++)
				{
					sprintf(az, "Bone %d", i);
					M_InsertItem(az, EVAV_EVVIT_Int, &p_XMEC->ai_Bones[i], MDF_Xmec_CallBack_Refresh);
					po_NewItem->psz_Help = "...";
					po_Item->mi_NumFields++;
				}
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_PROTEX:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				PROTEX_tdst_Modifier	*p_PROTEX;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				p_PROTEX = (PROTEX_tdst_Modifier *) (*ppst_Mod)->p_Data;
				M_InsertItem
				(
					"Sub Mat Source Num",
					EVAV_EVVIT_Int,
					&p_PROTEX->ulSubMatSourceNum,
					MDF_PROTEX_CallBack_Refresh
				);
				po_NewItem->psz_Help = "...";
				M_InsertItem
				(
					"MultiTex Source Num",
					EVAV_EVVIT_Int,
					&p_PROTEX->ulSubMatMuTexSourceNum,
					MDF_PROTEX_CallBack_Refresh
				);
				po_NewItem->psz_Help = "...";
				po_Item->mi_NumFields += 2;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_LegLink:
			pst_LegLink = (GAO_tdst_ModifierLegLink *) (*ppst_Mod)->p_Data;
			M_InsertItem1("Segment 1", EVAV_EVVIT_GO, &pst_LegLink->p_GAOA, (int) (*ppst_Mod)->pst_GO, NULL);
			M_InsertItem1("Segment 3", EVAV_EVVIT_GO, &pst_LegLink->p_GAOC, (int) (*ppst_Mod)->pst_GO, NULL);
			M_InsertItem("Segment 1 length", EVAV_EVVIT_Float, &pst_LegLink->f_AB, NULL);
			M_InsertItem("Segment 2 length", EVAV_EVVIT_Float, &pst_LegLink->f_BC, NULL);
			M_InsertItem1("Orient. GAO", EVAV_EVVIT_GO, &pst_LegLink->p_GaoOrient, (int) (*ppst_Mod)->pst_GO, NULL);
			M_InsertItem1("Orient. Flags", EVAV_EVVIT_SubStruct, &pst_LegLink->ul_Flags, LINK_C_MDF_LegLinkFlag, NULL);
			M_InsertItem1("Segment 1 (Rank)", EVAV_EVVIT_Int, &pst_LegLink->p_GAOA, 0, NULL);
			M_InsertItem1("Segment 3 (Rank)", EVAV_EVVIT_Int, &pst_LegLink->p_GAOC, 0, NULL);
			M_InsertItem1("Orient. GAO (Rank)", EVAV_EVVIT_Int, &pst_LegLink->p_GaoOrient, 0, NULL);
			po_Item->mi_NumFields += 9;
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_Morphing:
			pst_Morph = (GEO_tdst_ModifierMorphing *) (*ppst_Mod)->p_Data;
			M_InsertItem("Edit morph", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
			po_Item->mi_NumFields++;
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_Explode:
			pst_Explode = (GAO_tdst_ModifierExplode *) (*ppst_Mod)->p_Data;
			if(pst_Explode)
			{
				M_InsertItem("Gravity", EVAV_EVVIT_Float, &pst_Explode->f_Grav, NULL);
				M_InsertItem("Multiplier", EVAV_EVVIT_Float, &pst_Explode->f_Mul, NULL);
				po_Item->mi_NumFields += 2;
			}
			break;


		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_InfoPhoto:
			pst_Photo = (GAO_tdst_ModifierPhoto *) (*ppst_Mod)->p_Data;
			if(pst_Photo)
			{
				M_InsertItem("Mission ID", EVAV_EVVIT_Int, &pst_Photo->i_Mission, NULL);
				M_InsertItem("Mission Info", EVAV_EVVIT_Int, &pst_Photo->i_Info, NULL);
				M_InsertItem("Snap Sphere on Canal", EVAV_EVVIT_Int, &pst_Photo->i_BoneForSpherePivot, NULL);
				M_InsertItem("Sphere Offset", EVAV_EVVIT_Vector, &pst_Photo->st_SphereOffset, NULL);
				M_InsertItem("Snap Info on Canal", EVAV_EVVIT_Int, &pst_Photo->i_BoneForInfoPivot, NULL);
				M_InsertItem("Info Offset", EVAV_EVVIT_Vector, &pst_Photo->st_InfoOffset, NULL);
				M_InsertItem("LOD", EVAV_EVVIT_Float, &pst_Photo->f_CurrentLOD, NULL);
				M_InsertItem("LOD Min", EVAV_EVVIT_Float, &pst_Photo->f_LODMin, NULL);
				M_InsertItem("LOD Max", EVAV_EVVIT_Float, &pst_Photo->f_LODMax, NULL);
				M_InsertItem("Frame", EVAV_EVVIT_Float, &pst_Photo->f_CurrentFrame, NULL);
				M_InsertItem("Frame Min", EVAV_EVVIT_Float, &pst_Photo->f_FrameMin, NULL);
				M_InsertItem("Sphere Radius", EVAV_EVVIT_Float, &pst_Photo->f_Radius, NULL);
				po_Item->mi_NumFields += 12;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		case MDF_C_Modifier_ODE:
			pst_Mod_ODE = (GAO_tdst_ModifierODE *) (*ppst_Mod)->p_Data;
			if(pst_Mod_ODE)
			{
				M_InsertItem1234
				(
					"Joint Type",
					EVAV_EVVIT_ConstInt,
					&pst_Mod_ODE->uc_Type,
					0,
					0,
					1,
					MDF_ODE_ChangeType,
					(int)
						"Ball and Socket\n1\nHinge\n2\nFixed\n7\nAngular Motor\n9\n"
				);


				M_InsertItem("GO1", EVAV_EVVIT_GO, &pst_Mod_ODE->pst_GO1, MDF_ODE_ChangeGO1);
				M_InsertItem("GO2", EVAV_EVVIT_GO, &pst_Mod_ODE->pst_GO2, MDF_ODE_ChangeGO2);

				po_Item->mi_NumFields += 3;

				if((pst_Mod_ODE->uc_Type == ODE_JointTypeHinge) ) // || (pst_Mod_ODE->uc_Type == ODE_JointTypeAMotor) )
				{
					M_InsertItem("High Limit", EVAV_EVVIT_Float, &pst_Mod_ODE->f_HiLimit, MDF_ODE_ChangeHiLimit);
					M_InsertItem("Low Limit", EVAV_EVVIT_Float, &pst_Mod_ODE->f_LoLimit, MDF_ODE_ChangeLoLimit);
					po_Item->mi_NumFields += 2;
				}

				/*
				if((pst_Mod_ODE->uc_Type == ODE_JointTypeHinge) || (pst_Mod_ODE->uc_Type == ODE_JointTypeAMotor) )
				{
					M_InsertItem("Stop bounce", EVAV_EVVIT_Float, &pst_Mod_ODE->f_BounceStop, MDF_ODE_ChangeBounce);
					po_Item->mi_NumFields += 1;
				}
				*/

				if( (pst_Mod_ODE->uc_Type == ODE_JointTypeAMotor) )
				{
					M_InsertItem("Friction", EVAV_EVVIT_Float, &pst_Mod_ODE->f_Friction, MDF_ODE_ChangeFriction);
					po_Item->mi_NumFields += 1;
				}
			}
			break;


		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_PAG:
			{
				/*~~~~~~~~~~~~~~~~~~~*/
				MPAG_tdst_Modifier	*p;
				/*~~~~~~~~~~~~~~~~~~~*/

				p = ((MPAG_tdst_Modifier *) (*ppst_Mod)->p_Data);
				if(!p->pst_P) p->pst_P = PAG_pst_Create();
				po_Item->mi_NumFields += OBJ_i_AddGraphicObject_ParticleGenerator(pos, po_Item, p->pst_P);
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_SpecialLookAt:
			pst_LookAt = (GAO_tdst_ModifierSpecialLookAt *) (*ppst_Mod)->p_Data;
			M_InsertItem1234
			(
				"Type",
				EVAV_EVVIT_ConstInt,
				&pst_LookAt->c_Type,
				0,
				0,
				1,
				VAV_MDF_UpdateGameObject,
				(int) "Full Screen\n0\nPlanet aura\n1\nLook at object\n2\n"
			);
			po_Item->mi_NumFields++;
			switch(pst_LookAt->c_Type)
			{
			case MDF_SpecialLookAt_FullScreen:
				M_InsertItem("Distance", EVAV_EVVIT_Float, &pst_LookAt->f_Z, NULL);
				po_Item->mi_NumFields++;
				break;
			case MDF_SpecialLookAt_PlanetAura:
				M_InsertItem("Compute normals", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
				M_InsertItem("compute radius", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
				M_InsertItem("Planet rank", EVAV_EVVIT_Int, &pst_LookAt->i_GaoRank, NULL);

				if(pst_LookAt->i_GaoRank == -1)
				{
					M_InsertItem1("Planet", EVAV_EVVIT_GO, &pst_LookAt->p_GAO, (int) (*ppst_Mod)->pst_GO, NULL);
				}
				else
				{
					M_InsertItem1RO("Planet", EVAV_EVVIT_GO, &pst_LookAt->p_GAO, (int) (*ppst_Mod)->pst_GO, NULL);
				}

				M_InsertItem("Radius", EVAV_EVVIT_Float, &pst_LookAt->f_Z, NULL);
				M_InsertItem("Thickness", EVAV_EVVIT_Float, &pst_LookAt->f_Value1, NULL);
				M_InsertItem("Slope", EVAV_EVVIT_Float, &pst_LookAt->f_Value2, NULL);
				po_Item->mi_NumFields += 7;
				break;
			case MDF_SpecialLookAt_Object:
				M_InsertItem("Object rank", EVAV_EVVIT_Int, &pst_LookAt->i_GaoRank, NULL);
				if(pst_LookAt->i_GaoRank == -1)
				{
					M_InsertItem1("Object", EVAV_EVVIT_GO, &pst_LookAt->p_GAO, (int) (*ppst_Mod)->pst_GO, NULL);
				}
				else
				{
					M_InsertItem1RO("Object", EVAV_EVVIT_GO, &pst_LookAt->p_GAO, (int) (*ppst_Mod)->pst_GO, NULL);
				}

				po_Item->mi_NumFields += 2;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_Sound:
			M_InsertItemROColor("", 0, NULL);
			po_Item->mi_NumFields++;
			pst_Sound = (GEN_tdst_ModifierSound *) (*ppst_Mod)->p_Data;
			if(pst_Sound)
			{
				/*~~~~~~~~~~~~~~~~~~*/
				float	f_Min, f_Max;
				/*~~~~~~~~~~~~~~~~~~*/

				/* ORDER */
				M_InsertItemROColor("Order", 0, NULL);
				po_Item->mi_NumFields++;

				M_InsertItem
				(
					"Order Sound MdF",
					EVAV_EVVIT_Modifier,
					(*ppst_Mod)->pst_GO->pst_Extended->pst_Modifiers,
					NULL
				);
				po_Item->mi_NumFields++;

                M_InsertItemRO("Index", EVAV_EVVIT_Int, &pst_Sound->ui_Id, NULL);
				po_Item->mi_NumFields++;
				
                /* FILE */
				M_InsertItemROColor("File", 0, NULL);
				po_Item->mi_NumFields++;
                M_InsertItem12
				(
					"Smd File Key",
					EVAV_EVVIT_SndKey,
					&pst_Sound->ui_FileKey,
					NULL,
					0,
					SND_OnSelectSndFile
				);
				po_Item->mi_NumFields++;

                /* PREFETCH */
				M_InsertItemROColor("Prefetch", 0, NULL);
				po_Item->mi_NumFields++;

                M_InsertItem12
				(
					"Static",
					EVAV_EVVIT_Flags,
					&pst_Sound->ui_ConfigFlags,
					6,
					4,
                    SND_OnSelectSndStaticPrefetch
				);
				po_Item->mi_NumFields++;
                
                M_InsertItem12
				(
					"Manual",
					EVAV_EVVIT_Flags,
					&pst_Sound->ui_ConfigFlags,
					8,
					4,
                    SND_OnSelectSndManualPrefetch
				);
				po_Item->mi_NumFields++;
				
                M_InsertItem12
				(
					"Automatic",
					EVAV_EVVIT_Flags,
					&pst_Sound->ui_ConfigFlags,
					7,
					4,
                    SND_OnSelectSndDynamicPrefetch
				);
                po_Item->mi_NumFields++;

                if(pst_Sound->ui_ConfigFlags & MDF_Cte_SndPrefetchAuto)
                {
				    f_Min = 0.0f;
				    f_Max = 100000000.0f;
				    M_InsertItem12
				    (
					    "  Prefetch Distance",
					    EVAV_EVVIT_Float,
					    &pst_Sound->f_PrefetchDistance,
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
                    po_Item->mi_NumFields++;
                }


				/* PLAYING */
				M_InsertItemROColor("Playing", 0, NULL);
				po_Item->mi_NumFields++;

				M_InsertItem12("On Init", EVAV_EVVIT_Flags, &pst_Sound->ui_ConfigFlags, 1, 4, NULL);
				po_Item->mi_NumFields++;

				M_InsertItem12("In Volume", EVAV_EVVIT_Flags, &pst_Sound->ui_ConfigFlags, 0, 4, NULL);
				po_Item->mi_NumFields++;

				if(pst_Sound->ui_ConfigFlags & MDF_Cte_SndPlayInVolume)
				{
					f_Min = 0.0f;
					f_Max = 500.0f;
					M_InsertItem12
					(
						"  DeltaFar",
						EVAV_EVVIT_Float,
						&pst_Sound->f_DeltaFar,
						*(int *) &f_Min,
						*(int *) &f_Max,
						NULL
					);
					po_Item->mi_NumFields++;
				}

				M_InsertItem12("On Track", EVAV_EVVIT_Flags, &pst_Sound->ui_ConfigFlags, 3, 4, VAV_MDF_UpdateGameObject);
				po_Item->mi_NumFields++;

				if(pst_Sound->ui_ConfigFlags & MDF_Cte_SndPlayOnTrack)
				{
				    M_InsertItem1234
				    (
					    "  Track Id",
					    EVAV_EVVIT_ConstInt,
					    &pst_Sound->i_SndTrack,
					    0,
					    0,
					    4,
					    VAV_MDF_UpdateGameObject,
					    (int)"A\n0\nB\n1\nC\n2\nD (dialog)\n3\n"
				    );

					po_Item->mi_NumFields++;

					M_InsertItem12
					(
						"  Only When Free",
						EVAV_EVVIT_Flags,
						&pst_Sound->ui_ConfigFlags,
						4,
						4,
						VAV_MDF_UpdateGameObject
					);
					po_Item->mi_NumFields++;
				}

				M_InsertItem12("Loop", EVAV_EVVIT_Flags, &pst_Sound->ui_ConfigFlags, 2, 4, NULL);
				po_Item->mi_NumFields++;

                /* delay */
				f_Min = 0.0f;
				f_Max = 1000000.0f;
				M_InsertItem12
				(
					"Delay",
					EVAV_EVVIT_Float,
					&pst_Sound->f_Delay,
					*(int *) &f_Min,
					*(int *) &f_Max,
					NULL
				);
				po_Item->mi_NumFields++;
				
				M_InsertItem12("Random Start Pos.", EVAV_EVVIT_Flags, &pst_Sound->ui_ConfigFlags, 5, 4, NULL);
				po_Item->mi_NumFields++;

		
				/* EDITOR */
				M_InsertItemROColor("Editor", 0, NULL);
				po_Item->mi_NumFields++;

				M_InsertItem12
				(
					"Force no display",
					EVAV_EVVIT_Flags,
					&pst_Sound->ui_EdiFlags,
					1,
					4,
					VAV_MDF_UpdateGameObject
				);
				po_Item->mi_NumFields++;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_SoundLoading:
			M_InsertItemROColor("", 0, NULL);
			po_Item->mi_NumFields++;
			pst_LoadingSound = (MDF_tdst_LoadingSound *) (*ppst_Mod)->p_Data;
			if(pst_LoadingSound)
			{
				/*~~~~~~~~~~~~~~~~~~*/
				float	f_Min, f_Max;
				/*~~~~~~~~~~~~~~~~~~*/


				/*$1
				 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				    FILE
				 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				 */

				M_InsertItem12
				(
					"File Key",
					EVAV_EVVIT_SndKey,
					&pst_LoadingSound->ul_FileKey,
					(int) & pst_LoadingSound->ui_SndFlags,
					0,
					SND_OnSelectWacFile
				);
				po_Item->mi_NumFields++;

				/*$1
				 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				    DISTANCE
				 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				 */

				f_Min = 0.0f;
				f_Max = 100000000.0f;
				M_InsertItem12
				(
					"Loading Distance",
					EVAV_EVVIT_Float,
					&pst_LoadingSound->f_LoadingDistance,
					*(int *) &f_Min,
					*(int *) &f_Max,
					NULL
				);
				po_Item->mi_NumFields++;

				/*$1
				 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				    EDITOR
				 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				 */

				M_InsertItemROColor("Editor", 0, NULL);
				po_Item->mi_NumFields++;

				M_InsertItem12
				(
					"Force no display",
					EVAV_EVVIT_Flags,
					&pst_LoadingSound->ui_MdfFlag,
					6,
					4,
					VAV_MDF_UpdateGameObject
				);
				po_Item->mi_NumFields++;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_SoundVolume:
            pst_SoundVol = (SND_tdst_ModifierSoundVol*) (*ppst_Mod)->p_Data;
			if(pst_SoundVol)
			{
				/*~~~~~~~~~~~~~~~~~~*/
				float	f_Min, f_Max;
				/*~~~~~~~~~~~~~~~~~~*/

				M_InsertItem1234
				(
					"Group Id",
					EVAV_EVVIT_ConstInt,
                    &pst_SoundVol->i_GroupId,
					0,
					0,
					4,
					VAV_MDF_UpdateGameObject,
					(int)
						"sfx\n0\nmusic\n1\nambience\n2\ndialog\n3\ncut-scene\n4\nInterface\n5\nGroupA\n6\nGroupB\n7\nMaster\n8\n"
				);
				po_Item->mi_NumFields++;

                f_Min = 0.0f;
				f_Max = 1.0f;
				M_InsertItem12
				(
					"Near factor",
					EVAV_EVVIT_Float,
					&pst_SoundVol->f_NearVol,
					*(int *) &f_Min,
					*(int *) &f_Max,
					NULL
				);
				po_Item->mi_NumFields++;
				
                f_Min = 0.0f;
				f_Max = 1.0f;
				M_InsertItem12
				(
					"Far factor",
					EVAV_EVVIT_Float,
					&pst_SoundVol->f_FarVol,
					*(int *) &f_Min,
					*(int *) &f_Max,
					NULL
				);
				po_Item->mi_NumFields++;


				M_InsertItem12
				(
					"Spherical Zone",
					EVAV_EVVIT_Flags,
                    &pst_SoundVol->ul_Flags,
					0,
					4,
					SND_OnChangeSndVolFlag
				);
				po_Item->mi_NumFields++;

                if(pst_SoundVol->ul_Flags & SND_Cte_MdFSoundVol_Spheric)
                {
				    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "Radius - near",
					    EVAV_EVVIT_Float,
					    &pst_SoundVol->af_Near[0],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;

				    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "      far",
					    EVAV_EVVIT_Float,
					    &pst_SoundVol->af_Far[0],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;
                }
                else
                {
				    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "X - near",
					    EVAV_EVVIT_Float,
					    &pst_SoundVol->af_Near[0],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;

                    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "      far",
					    EVAV_EVVIT_Float,
					    &pst_SoundVol->af_Far[0],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;


				    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "Y - near",
					    EVAV_EVVIT_Float,
					    &pst_SoundVol->af_Near[1],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;

				    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "      far",
					    EVAV_EVVIT_Float,
					    &pst_SoundVol->af_Far[1],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;


                    /**/    
                    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "Z - near",
					    EVAV_EVVIT_Float,
					    &pst_SoundVol->af_Near[2],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;

                    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "      far",
					    EVAV_EVVIT_Float,
					    &pst_SoundVol->af_Far[2],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;  
                }
            }
            break;
		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_SoundFx:
			M_InsertItemROColor("", 0, NULL);
			po_Item->mi_NumFields++;
			pst_SoundFx = (GEN_tdst_ModifierSoundFx *) (*ppst_Mod)->p_Data;
			if(pst_SoundFx)
			{
				/*~~~~~~~~~~~~~~~~~~*/
				float	f_Min, f_Max;
				/*~~~~~~~~~~~~~~~~~~*/

				/* ACTIVATION */
				M_InsertItemROColor("Zone", 0, NULL);
				po_Item->mi_NumFields++;

				M_InsertItem12
				(
					"Sphere",
					EVAV_EVVIT_Flags,
                    &pst_SoundFx->ui_MdfFlag,
					2,
					4,
					VAV_MDF_UpdateGameObject
				);
				po_Item->mi_NumFields++;

                if(pst_SoundFx->ui_MdfFlag & MDF_Cte_SndFx_Sphere)
                {
				    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "Radius - near",
					    EVAV_EVVIT_Float,
					    &pst_SoundFx->af_Distance[0],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;

				    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "               far",
					    EVAV_EVVIT_Float,
					    &pst_SoundFx->af_Far[0],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;
                }
                else
                {
				    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "X - near",
					    EVAV_EVVIT_Float,
					    &pst_SoundFx->af_Distance[0],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;
                    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "      far",
					    EVAV_EVVIT_Float,
					    &pst_SoundFx->af_Far[0],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;


				    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "Y - near",
					    EVAV_EVVIT_Float,
					    &pst_SoundFx->af_Distance[1],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;

				    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "      far",
					    EVAV_EVVIT_Float,
					    &pst_SoundFx->af_Far[1],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;


                    /**/    
                    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "Z - near",
					    EVAV_EVVIT_Float,
					    &pst_SoundFx->af_Distance[2],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;
                    f_Min = 0.0f;
				    f_Max = 1000000.0f;
				    M_InsertItem12
				    (
					    "      far",
					    EVAV_EVVIT_Float,
					    &pst_SoundFx->af_Far[2],
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;
                    /**/    
                }
                
                /* NETWORK */
				M_InsertItemROColor("Network", 0, NULL);
				po_Item->mi_NumFields++;

				M_InsertItem1234
				(
					"NetIdx",
					EVAV_EVVIT_ConstInt,
                    &pst_SoundFx->i_NetIdx,
					0,
					0,
					4,
					SND_OnChangeSndFxNetIdx,
					(int)
						"None\n-1\nNet-00\n0\nNet-01\n1\nNet-02\n2\nNet-03\n3\nNet-04\n4\nNet-05\n5\nNet-06\n6\nNet-07\n7\nNet-08\n8\nNet-09\n9\nNet-10\n10\nNet-11\n11\nNet-12\n12\nNet-13\n13\nNet-14\n14\nNet-15\n15\nNet-16\n16\n"
				);
				po_Item->mi_NumFields++;

                if(pst_SoundFx->i_NetIdx == -1)
                {
                    pst_SoundFx->ui_MdfFlag &= ~(MDF_Cte_SndFx_NetSlave|MDF_Cte_SndFx_NetMaster);
                    SND_FxNetworkDeleteNode(pst_SoundFx);
                }
                else
                {
                    if((pst_SoundFx->ui_MdfFlag & (MDF_Cte_SndFx_NetSlave|MDF_Cte_SndFx_NetMaster)) == 0)
                        pst_SoundFx->ui_MdfFlag |= MDF_Cte_SndFx_NetSlave;




                    if(pst_SoundFx->ui_MdfFlag & MDF_Cte_SndFx_NetMaster)
                    {
				        M_InsertItem12
				        (
					        "MASTER",
					        EVAV_EVVIT_Flags,
					        &pst_SoundFx->ui_MdfFlag,
					        3,
					        4,
					        SND_OnSelectSndFxNetMaster
				        );
				        po_Item->mi_NumFields++;
                    }
                    else
                    {
				        M_InsertItem12
				        (
					        "SLAVE",
					        EVAV_EVVIT_Flags,
					        &pst_SoundFx->ui_MdfFlag,
					        4,
					        4,
					        SND_OnSelectSndFxNetSlave
				        );
				        po_Item->mi_NumFields++;
                    }
                }

                /* SETTINGS */
                if(pst_SoundFx->ui_MdfFlag & MDF_Cte_SndFx_NetSlave)
                {
				    M_InsertItemROColor("Settings (from master)", 0, NULL);
				    po_Item->mi_NumFields++;

                    GEN_tdst_ModifierSoundFx *pFx;
                    pFx = SND_p_FxNetworkGetMaster(pst_SoundFx->i_NetIdx);

                    if(pFx)
                    {
				        M_InsertItem1234RO
				        (
					        "Core Id",
					        EVAV_EVVIT_ConstInt,
                            &pFx->i_CoreId,
					        0,
					        0,
					        4,
					        NULL,
					        (int)"A\n0\nB\n1\n"
				        );
				        po_Item->mi_NumFields++;

                        f_Min = 0.0f;
				        f_Max = 1.0f;
				        M_InsertItem12RO
				        (
					        "Fx Vol",
					        EVAV_EVVIT_Float,
					        &pFx->f_WetVol,
					        *(int *) &f_Min,
					        *(int *) &f_Max,
					        NULL
				        );
				        po_Item->mi_NumFields++;

				        M_InsertItem1234RO
				        (
					        "Mode",
					        EVAV_EVVIT_ConstInt,
					        &pFx->i_Mode,
					        0,
					        0,
					        4,
					        VAV_MDF_UpdateGameObject,
					        (int)
						        "Off\n0\nRoom\n1\nStudioA\n2\nStudioB\n3\nStudioC\n4\nHall\n5\nSpace\n6\nEcho\n7\nDelay\n8\nPipe\n9\nMountains\n10\nCity\n11\n"
				        );
				        po_Item->mi_NumFields++;

                        if((pFx->i_Mode == SND_Cte_FxMode_Echo) ||(pFx->i_Mode == SND_Cte_FxMode_Delay))
                        {
				            M_InsertItem12RO("Delay   (0-1000ms)", EVAV_EVVIT_Int, &pFx->i_Delay, 0, 1000, NULL);
				            po_Item->mi_NumFields++;

				            M_InsertItem12RO("Feedback  (0-100%)", EVAV_EVVIT_Int, &pFx->i_Feedback, 0, 100, NULL);
				            po_Item->mi_NumFields++;
                        }
                    }
                    else
                    {
                       	pos = mpo_ListItems->InsertAfter 
		                ( 
			                pos, 
			                po_NewItem = new EVAV_cl_ViewItem 
				                ( 
					                "no master found", 
					                EVAV_EVVIT_String, 
					                0, 
					                EVAV_ReadOnly, 
					                0, 
					                0, 
					                0, 
					                NULL, 
					                po_Item->mx_Color /* 0x00E6E6E6 */ , 
					                1 
				                ) 
		                );  
	                    po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;

				        po_Item->mi_NumFields++;
                    }
                }
                else
                {
				    M_InsertItemROColor("Settings", 0, NULL);
				    po_Item->mi_NumFields++;

				    M_InsertItem1234
				    (
					    "Core Id",
					    EVAV_EVVIT_ConstInt,
                        &pst_SoundFx->i_CoreId,
					    0,
					    0,
					    4,
					    NULL,
					    (int)"A\n0\nB\n1\n"
				    );
				    po_Item->mi_NumFields++;

                    f_Min = 0.0f;
				    f_Max = 1.0f;
				    M_InsertItem12
				    (
					    "Fx Vol",
					    EVAV_EVVIT_Float,
					    &pst_SoundFx->f_WetVol,
					    *(int *) &f_Min,
					    *(int *) &f_Max,
					    NULL
				    );
				    po_Item->mi_NumFields++;

				    M_InsertItem1234
				    (
					    "Mode",
					    EVAV_EVVIT_ConstInt,
					    &pst_SoundFx->i_Mode,
					    0,
					    0,
					    4,
					    VAV_MDF_UpdateGameObject,
					    (int)
						    "Off\n0\nRoom\n1\nStudioA\n2\nStudioB\n3\nStudioC\n4\nHall\n5\nSpace\n6\nEcho\n7\nDelay\n8\nPipe\n9\nMountains\n10\nCity\n11\n"
				    );
				    po_Item->mi_NumFields++;

                    if((pst_SoundFx->i_Mode == SND_Cte_FxMode_Echo) ||(pst_SoundFx->i_Mode == SND_Cte_FxMode_Delay))
                    {
				        M_InsertItem12("Delay   (0-1000ms)", EVAV_EVVIT_Int, &pst_SoundFx->i_Delay, 0, 1000, NULL);
				        po_Item->mi_NumFields++;

				        M_InsertItem12("Feedback  (0-100%)", EVAV_EVVIT_Int, &pst_SoundFx->i_Feedback, 0, 100, NULL);
				        po_Item->mi_NumFields++;
                    }
                }

				/* EDITOR */
				M_InsertItemROColor("Editor", 0, NULL);
				po_Item->mi_NumFields++;

				M_InsertItem12
				(
					"Force no display",
					EVAV_EVVIT_Flags,
					&pst_SoundFx->ui_MdfFlag,
					1,
					4,
					VAV_MDF_UpdateGameObject
				);
				po_Item->mi_NumFields++;
			}
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_SaveAddMatrix:
			pst_SaveAddMatrix = (GAO_tdst_ModifierSaveAddMatrix *) (*ppst_Mod)->p_Data;
			M_InsertItem("Edit data", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
			po_Item->mi_NumFields++;
			break;
			
		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		case MDF_C_Modifier_FUR:
			p_FUR = (FUR_tdst_Modifier *) (*ppst_Mod)->p_Data;
			M_InsertItem("Number layers", EVAV_EVVIT_Int, &p_FUR->NBR_Layers, NULL);
			M_InsertItem("offset normal", EVAV_EVVIT_Float, &p_FUR->f_NormalOffset, NULL);
			M_InsertItem("offset U", EVAV_EVVIT_Float, &p_FUR->f_UOffset, NULL);
			M_InsertItem("offset V", EVAV_EVVIT_Float, &p_FUR->f_VOffset, NULL);
			M_InsertItem("LOD Near", EVAV_EVVIT_Float, &p_FUR->f_Near, LINK_CallBack_Refresh3DEngine);
			M_InsertItem("LOD Far", EVAV_EVVIT_Float, &p_FUR->f_Far, LINK_CallBack_Refresh3DEngine);
			M_InsertItem("Near LOD", EVAV_EVVIT_Int, &p_FUR->uc_NearLod, NULL);
			M_InsertItem1("Flags", EVAV_EVVIT_SubStruct, &p_FUR->ul_Flags, LINK_C_ModifierFurFlags, NULL);			
			M_InsertItem("Color Outline", EVAV_EVVIT_Color, &p_FUR->LineColor, LINK_CallBack_Refresh3DEngine);
			po_Item->mi_NumFields+=9;
			break;

#ifdef JADEFUSION
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        case MDF_C_Modifier_DYNFUR:
            p_DYNFUR = (DYNFUR_tdst_Modifier *) (*ppst_Mod)->p_Data;
            M_InsertItem("Fur Length", EVAV_EVVIT_Float, &p_DYNFUR->f_FurLength, NULL);
            M_InsertItem("Number layers", EVAV_EVVIT_Int, &p_DYNFUR->NBR_Layers, NULL);
            M_InsertItem("offset U", EVAV_EVVIT_Float, &p_DYNFUR->f_UOffset, NULL);
            M_InsertItem("offset V", EVAV_EVVIT_Float, &p_DYNFUR->f_VOffset, NULL);
            M_InsertItem("Gravity", EVAV_EVVIT_Float, &p_DYNFUR->f_Gravity, NULL);
            M_InsertItem("Global linear velocity", EVAV_EVVIT_Float, &p_DYNFUR->f_GlobalLinVelBoost, NULL);
            M_InsertItem("Global angular velocity", EVAV_EVVIT_Float, &p_DYNFUR->f_GlobalAngVelBoost, NULL);
            M_InsertItem("Local angular velocity", EVAV_EVVIT_Float, &p_DYNFUR->f_LocalAngVelBoost, NULL);
            po_Item->mi_NumFields+=8;
            break;
#endif

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_Crush:
			pst_Crush = (GEO_tdst_ModifierCrush *) (*ppst_Mod)->p_Data;
			M_InsertItem1("Flags", EVAV_EVVIT_SubStruct, &pst_Crush->ul_Flags, LINK_C_ModifierCrushFlags, NULL);
			M_InsertItem1("Min", EVAV_EVVIT_Float, &pst_Crush->f_Min, 0, NULL);
			M_InsertItem1("Max", EVAV_EVVIT_Float, &pst_Crush->f_Max, 0, NULL);
			M_InsertItem1("Level Init", EVAV_EVVIT_Float, &pst_Crush->f_LevelInit, 0, NULL);
			M_InsertItem1("Level", EVAV_EVVIT_Float, &pst_Crush->f_Level, 0, NULL);
			po_Item->mi_NumFields += 5;
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_C_Modifier_RLICarte:
			M_InsertItem("Edit RLI group and color", EVAV_EVVIT_Modifier, _pst_Mod, NULL);
			po_Item->mi_NumFields++;
			break;

        /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        case MDF_C_Modifier_Sfx:
            {
                GAO_tdst_ModifierSfx* pst_Sfx;
                pst_Sfx= (GAO_tdst_ModifierSfx *) (*ppst_Mod)->p_Data;

                if (pst_Sfx)
                {
                    // insert type dropdown
                    M_InsertItem1234
                        (
                        "Special Effect Type",
                        EVAV_EVVIT_ConstInt,
                        &pst_Sfx->type,
                        0,
                        0,
                        1,
                        MDF_Sfx_CallBack_Refresh,
                        (int) "Hot Air\n0\nRim Light Height Attenuation\n1\n"
                        );
                    po_Item->mi_NumFields++;
 
                    // switch on the type 
                    switch (pst_Sfx->type)
                    {
#ifdef JADEFUSION
                    case MDF_SFX_HotAir:
                        {
                            M_InsertItem("Offset", EVAV_EVVIT_Vector, &pst_Sfx->hotAirParams.stOffset, NULL);
                            po_Item->mi_NumFields++;

                            M_InsertItem("SizeX", EVAV_EVVIT_Float, &pst_Sfx->hotAirParams.stHotAirObject.Width, NULL);
                            po_Item->mi_NumFields++;

                            M_InsertItem("SizeY", EVAV_EVVIT_Float, &pst_Sfx->hotAirParams.stHotAirObject.Height, NULL);
                            po_Item->mi_NumFields++;

                            M_InsertItem("Range", EVAV_EVVIT_Float, &pst_Sfx->hotAirParams.stHotAirObject.Range, NULL);
                            po_Item->mi_NumFields++;

                            M_InsertItem("Scroll Speed", EVAV_EVVIT_Float, &pst_Sfx->hotAirParams.stHotAirObject.ScrollSpeed, NULL);
                            po_Item->mi_NumFields++;

                            M_InsertItem("Noise Pixel Size", EVAV_EVVIT_Float, &pst_Sfx->hotAirParams.stHotAirObject.NoisePixelSize, NULL);
                            po_Item->mi_NumFields++;
                            break;
                        }
#endif
                    case MDF_SFX_RimLight:
                        {
                            M_InsertItem("Height Attenuation Min", EVAV_EVVIT_Float, &pst_Sfx->rimLightParams.fHeightAttenuationMin, NULL);
                            po_Item->mi_NumFields++;

                            M_InsertItem("Height Attenuation Max", EVAV_EVVIT_Float, &pst_Sfx->rimLightParams.fHeightAttenuationMax, NULL);
                            po_Item->mi_NumFields++;
                            break;
                        }
		            }
                }
		    }
            break;

        case MDF_C_Modifier_RotationPaste:
            {
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                GAO_tdst_ModifierRotationPaste	*p_RotationPaste;
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                p_RotationPaste = (GAO_tdst_ModifierRotationPaste *) (*ppst_Mod)->p_Data;

                M_InsertItem("GAO to paste from", EVAV_EVVIT_GO, &p_RotationPaste->pObjectToPasteFrom, NULL); 
                po_Item->mi_NumFields++;
                po_NewItem->psz_Help = "...";
            }
            break;

        case MDF_C_Modifier_TranslationPaste:
            {
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                GAO_tdst_ModifierTranslationPaste	*p_TranslationPaste;
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                p_TranslationPaste = (GAO_tdst_ModifierTranslationPaste *) (*ppst_Mod)->p_Data;

                M_InsertItem("GAO to paste from", EVAV_EVVIT_GO, &p_TranslationPaste->pObjectToPasteFrom, NULL); 
                po_Item->mi_NumFields++;
                po_NewItem->psz_Help = "...";
            }
            break;

        case MDF_C_Modifier_AnimatedGAO:
            {
                GAO_tdst_ModifierAnimatedGAO	*p_AnimatedGAO;

                p_AnimatedGAO = (GAO_tdst_ModifierAnimatedGAO *) (*ppst_Mod)->p_Data;

                //M_InsertItem12("Sync with game time", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 12, 4, LINK_CallBack_Modifier_Refresh);
                //po_Item->mi_NumFields++;

                M_InsertItem12("Use reference GAO", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 13, 4, LINK_CallBack_Modifier_Refresh);
                po_Item->mi_NumFields++;
                if(p_AnimatedGAO->ulFlags.bUseReferenceGAO)
                {
                    M_InsertItem("Reference GAO", EVAV_EVVIT_GO, &p_AnimatedGAO->p_GAOReference, NULL); 
                    po_Item->mi_NumFields++;
                }

                M_InsertItem12("Offset the initial position", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 14, 4, LINK_CallBack_Modifier_Refresh);
                po_Item->mi_NumFields++;
                if(p_AnimatedGAO->ulFlags.bUsePositionOffset)
                {
                    M_InsertItem("Offset", EVAV_EVVIT_Vector, &p_AnimatedGAO->vOffset, NULL);
                    po_Item->mi_NumFields++;
                }

                M_InsertItem12("Apply to rotation", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 0, 4, LINK_CallBack_Modifier_Refresh);
                po_Item->mi_NumFields++;

                if(p_AnimatedGAO->ulFlags.bApplyToRotation)
                {
                    M_InsertItem12("Apply to rotation X", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 1, 4, LINK_CallBack_Modifier_Refresh);
                    po_Item->mi_NumFields++;
                    M_InsertItem12("Apply to rotation Y", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 2, 4, LINK_CallBack_Modifier_Refresh);
                    po_Item->mi_NumFields++;
                    M_InsertItem12("Apply to rotation Z", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 3, 4, LINK_CallBack_Modifier_Refresh);
                    po_Item->mi_NumFields++;
                }

                M_InsertItem12("Apply to translation", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 4, 4, LINK_CallBack_Modifier_Refresh);
                po_Item->mi_NumFields++;

                if(p_AnimatedGAO->ulFlags.bApplyToTranslation)
                {
                    M_InsertItem12("Apply to translation X", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 5, 4, LINK_CallBack_Modifier_Refresh);
                    po_Item->mi_NumFields++;
                    M_InsertItem12("Apply to translation Y", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 6, 4, LINK_CallBack_Modifier_Refresh);
                    po_Item->mi_NumFields++;
                    M_InsertItem12("Apply to translation Z", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 7, 4, LINK_CallBack_Modifier_Refresh);
                    po_Item->mi_NumFields++;
		}

                M_InsertItem12("Apply to scale", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 8, 4, LINK_CallBack_Modifier_Refresh);
                po_Item->mi_NumFields++;

                if(p_AnimatedGAO->ulFlags.bApplyToScale)
                {					
                    M_InsertItem12("Apply to scale X", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 9, 4, LINK_CallBack_Modifier_Refresh);
                    po_Item->mi_NumFields++;
                    M_InsertItem12("Apply to scale Y", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 10, 4, LINK_CallBack_Modifier_Refresh);
                    po_Item->mi_NumFields++;
                    M_InsertItem12("Apply to scale Z", EVAV_EVVIT_Flags, &p_AnimatedGAO->ulRawFlags, 11, 4, LINK_CallBack_Modifier_Refresh);
                    po_Item->mi_NumFields++;
                }

                if(p_AnimatedGAO->ulFlags.bApplyToRotation && p_AnimatedGAO->ulFlags.bApplyToRotationX)
                {
                    M_InsertItemROColor("Apply to rotation X properties", 0, NULL);
                    po_Item->mi_NumFields++;

                    M_InsertItem1234
                        (
                        "Animation type",
                        EVAV_EVVIT_ConstInt,
                        &p_AnimatedGAO->eAnimationTypes[0],
                        0,
                        0,
                        4,
                        LINK_CallBack_Modifier_Refresh,
                        (int) "Linear\n0\nNoise\n1\nSinus\n2\n"
                        );
                    po_Item->mi_NumFields++;

                    AddModifierAnimatedGAOProperties(pos, po_Item, (int)p_AnimatedGAO->eAnimationTypes[0], p_AnimatedGAO->pParams[0]);
                }

                if(p_AnimatedGAO->ulFlags.bApplyToRotation && p_AnimatedGAO->ulFlags.bApplyToRotationY)
                {
                    M_InsertItemROColor("Apply to rotation Y properties", 0, NULL);
                    po_Item->mi_NumFields++;

                    M_InsertItem1234
                        (
                        "Animation type",
                        EVAV_EVVIT_ConstInt,
                        &p_AnimatedGAO->eAnimationTypes[1],
                        0,
                        0,
                        4,
                        LINK_CallBack_Modifier_Refresh,
                        (int) "Linear\n0\nNoise\n1\nSinus\n2\n"
                        );
                    po_Item->mi_NumFields++;

                    AddModifierAnimatedGAOProperties(pos, po_Item, (int)p_AnimatedGAO->eAnimationTypes[1], p_AnimatedGAO->pParams[1]);
                }

                if(p_AnimatedGAO->ulFlags.bApplyToRotation && p_AnimatedGAO->ulFlags.bApplyToRotationZ)
                {
                    M_InsertItemROColor("Apply to rotation Z properties", 0, NULL);
                    po_Item->mi_NumFields++;

                    M_InsertItem1234
                        (
                        "Animation type",
                        EVAV_EVVIT_ConstInt,
                        &p_AnimatedGAO->eAnimationTypes[2],
                        0,
                        0,
                        4,
                        LINK_CallBack_Modifier_Refresh,
                        (int) "Linear\n0\nNoise\n1\nSinus\n2\n"
                        );
                    po_Item->mi_NumFields++;

                    AddModifierAnimatedGAOProperties(pos, po_Item, (int)p_AnimatedGAO->eAnimationTypes[2], p_AnimatedGAO->pParams[2]);
                }

                if(p_AnimatedGAO->ulFlags.bApplyToTranslation && p_AnimatedGAO->ulFlags.bApplyToTranslationX)
                {
                    M_InsertItemROColor("Apply to translation X properties", 0, NULL);
                    po_Item->mi_NumFields++;

                    M_InsertItem1234
                        (
                        "Animation type",
                        EVAV_EVVIT_ConstInt,
                        &p_AnimatedGAO->eAnimationTypes[3],
                        0,
                        0,
                        4,
                        LINK_CallBack_Modifier_Refresh,
                        (int) "Linear\n0\nNoise\n1\nSinus\n2\n"
                        );
                    po_Item->mi_NumFields++;

                    AddModifierAnimatedGAOProperties(pos, po_Item, (int)p_AnimatedGAO->eAnimationTypes[3], p_AnimatedGAO->pParams[3]);
                }

                if(p_AnimatedGAO->ulFlags.bApplyToTranslation && p_AnimatedGAO->ulFlags.bApplyToTranslationY)
                {
                    M_InsertItemROColor("Apply to translation Y properties", 0, NULL);
                    po_Item->mi_NumFields++;

                    M_InsertItem1234
                        (
                        "Animation type",
                        EVAV_EVVIT_ConstInt,
                        &p_AnimatedGAO->eAnimationTypes[4],
                        0,
                        0,
                        4,
                        LINK_CallBack_Modifier_Refresh,
                        (int) "Linear\n0\nNoise\n1\nSinus\n2\n"
                        );
                    po_Item->mi_NumFields++;

                    AddModifierAnimatedGAOProperties(pos, po_Item, (int)p_AnimatedGAO->eAnimationTypes[4], p_AnimatedGAO->pParams[4]);
                }

                if(p_AnimatedGAO->ulFlags.bApplyToTranslation && p_AnimatedGAO->ulFlags.bApplyToTranslationZ)
                {
                    M_InsertItemROColor("Apply to translation Z properties", 0, NULL);
                    po_Item->mi_NumFields++;

                    M_InsertItem1234
                        (
                        "Animation type",
                        EVAV_EVVIT_ConstInt,
                        &p_AnimatedGAO->eAnimationTypes[5],
                        0,
                        0,
                        4,
                        LINK_CallBack_Modifier_Refresh,
                        (int) "Linear\n0\nNoise\n1\nSinus\n2\n"
                        );
                    po_Item->mi_NumFields++;

                    AddModifierAnimatedGAOProperties(pos, po_Item, (int)p_AnimatedGAO->eAnimationTypes[5], p_AnimatedGAO->pParams[5]);
                }

                if(p_AnimatedGAO->ulFlags.bApplyToScale && p_AnimatedGAO->ulFlags.bApplyToScaleX)
                {
                    M_InsertItemROColor("Apply to scale X properties", 0, NULL);
                    po_Item->mi_NumFields++;

                    M_InsertItem1234
                        (
                        "Animation type",
                        EVAV_EVVIT_ConstInt,
                        &p_AnimatedGAO->eAnimationTypes[6],
                        0,
                        0,
                        4,
                        LINK_CallBack_Modifier_Refresh,
                        (int) "Linear\n0\nNoise\n1\nSinus\n2\n"
                        );
                    po_Item->mi_NumFields++;

                    AddModifierAnimatedGAOProperties(pos, po_Item, (int)p_AnimatedGAO->eAnimationTypes[6], p_AnimatedGAO->pParams[6]);
                }

                if(p_AnimatedGAO->ulFlags.bApplyToScale && p_AnimatedGAO->ulFlags.bApplyToScaleY)
                {
                    M_InsertItemROColor("Apply to scale Y properties", 0, NULL);
                    po_Item->mi_NumFields++;

                    M_InsertItem1234
                        (
                        "Animation type",
                        EVAV_EVVIT_ConstInt,
                        &p_AnimatedGAO->eAnimationTypes[7],
                        0,
                        0,
                        4,
                        LINK_CallBack_Modifier_Refresh,
                        (int) "Linear\n0\nNoise\n1\nSinus\n2\n"
                        );
                    po_Item->mi_NumFields++;

                    AddModifierAnimatedGAOProperties(pos, po_Item, (int)p_AnimatedGAO->eAnimationTypes[7], p_AnimatedGAO->pParams[7]);
                }

                if(p_AnimatedGAO->ulFlags.bApplyToScale && p_AnimatedGAO->ulFlags.bApplyToScaleZ)
                {
                    M_InsertItemROColor("Apply to scale Z properties", 0, NULL);
                    po_Item->mi_NumFields++;

                    M_InsertItem1234
                        (
                        "Animation type",
                        EVAV_EVVIT_ConstInt,
                        &p_AnimatedGAO->eAnimationTypes[8],
                        0,
                        0,
                        4,
                        LINK_CallBack_Modifier_Refresh,
                        (int) "Linear\n0\nNoise\n1\nSinus\n2\n"
                        );
                    po_Item->mi_NumFields++;

                    AddModifierAnimatedGAOProperties(pos, po_Item, (int)p_AnimatedGAO->eAnimationTypes[8], p_AnimatedGAO->pParams[8]);
                }

                po_NewItem->psz_Help = "...";
            }
            break;
        case MDF_C_Modifier_Weather:
            {
                MDF_tdst_Weather* pWeather;
                pWeather = (MDF_tdst_Weather*)(*ppst_Mod)->p_Data;

                // Type
                M_InsertItem1234("Weather Effect", EVAV_EVVIT_ConstInt, &pWeather->e_Type,
                                 0, 0, 4, LINK_CallBack_Modifier_Refresh, 
                                 (int)"RainFX\n0\n");
                po_Item->mi_NumFields++;

                switch (pWeather->e_Type)
                {
                    case MDF_Weather_RainFX:
                        // Flags
                        {
                            M_InsertItem12("Dynamic", EVAV_EVVIT_Flags, &pWeather->st_RainFX.ul_Flags, 0, 4, LINK_CallBack_Modifier_Refresh);
                            po_Item->mi_NumFields++;

                            if (pWeather->st_RainFX.ul_Flags & MDF_Weather_RainFX_Flag_Dynamic)
                            {
                                M_InsertItem12("Enable Wind", EVAV_EVVIT_Flags, &pWeather->st_RainFX.ul_Flags, 1, 4, LINK_CallBack_Modifier_Refresh);
                                po_Item->mi_NumFields++;
                            }
                        }

                        if (pWeather->st_RainFX.ul_Flags & MDF_Weather_RainFX_Flag_Dynamic)
                        {
                            M_InsertItem("U Scale", EVAV_EVVIT_Float, &pWeather->st_RainFX.f_RainScaleU, NULL);
                            po_Item->mi_NumFields++;

                            M_InsertItem("V Scale", EVAV_EVVIT_Float, &pWeather->st_RainFX.f_RainScaleV, NULL);
                            po_Item->mi_NumFields++;
                        }

                        M_InsertItem("Alpha Boost (Global)", EVAV_EVVIT_Float, &pWeather->st_RainFX.f_AlphaBoost, NULL);
                        po_Item->mi_NumFields++;

                        M_InsertItem("Alpha Intensity (Global)", EVAV_EVVIT_Float, &pWeather->st_RainFX.f_AlphaIntensity, NULL);
                        po_Item->mi_NumFields++;
                        break;
                }
            }
            break;
        /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        case MDF_C_Modifier_Vine: 
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            VINE_tdst_Modifier	*p_Vine;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

            p_Vine = (VINE_tdst_Modifier *) (*ppst_Mod)->p_Data;
            M_InsertItem1("Dampening Factor", EVAV_EVVIT_Float, &p_Vine->fDampeningFactor, 0, NULL);
            po_NewItem->psz_Help = "...";
            M_InsertItem1("Gravity", EVAV_EVVIT_Float, &p_Vine->fGravity, 0, NULL);
            po_NewItem->psz_Help = "...";
#ifdef JADEFUSION
            M_InsertItem1("Maximum Force", EVAV_EVVIT_Float, &p_Vine->fMaxForce, 0, NULL);
            po_NewItem->psz_Help = "...";
#endif
			M_InsertItem1("Applied force when Collide", EVAV_EVVIT_Float, &p_Vine->fForceApplied, 0, NULL);
            po_NewItem->psz_Help = "...";
            M_InsertItem1("Child is also hook", EVAV_EVVIT_Bool, &p_Vine->ul_ChildIsAlsoHook, 0, NULL);
            po_NewItem->psz_Help = "...";
            M_InsertItem1("Second hook break on contact", EVAV_EVVIT_Bool, &p_Vine->ul_ChildHookBrokeOnContact, 0, NULL);
            po_NewItem->psz_Help = "...";
            

            po_Item->mi_NumFields += 5;
            break;

#ifdef JADEFUSION
        case MDF_C_Modifier_SoftBody:
            {
                CHAR  az[64];
                GAO_tdst_ModifierSoftBody *p_SoftBody;

                p_SoftBody = (GAO_tdst_ModifierSoftBody *) (*ppst_Mod)->p_Data;

                sprintf(az, "Soft Body Modifier");
                M_InsertItemROColor("", 0, NULL);
                po_Item->mi_NumFields++;
                M_InsertItemROColorSpecial(az, 0, NULL, (int) p_SoftBody);
                po_Item->mi_NumFields++;

                M_InsertItemPrivate("Nb Iter", EVAV_EVVIT_Int, &p_SoftBody->ul_NbIter, NULL);
                po_Item->mi_NumFields++;
                M_InsertItemPrivate("Nb Normalize Iter", EVAV_EVVIT_Int, &p_SoftBody->ul_NbNormalizeIter, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem12("Full hierarchy", EVAV_EVVIT_Flags, &p_SoftBody->ul_Flags, 1, 4, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem12("Non collidable", EVAV_EVVIT_Flags, &p_SoftBody->ul_Flags, 2, 4, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem1("Gravity", EVAV_EVVIT_Vector, &p_SoftBody->st_Gravity, 0, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem1("DT Damping", EVAV_EVVIT_Float, &p_SoftBody->f_DTDamping, 0, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem1("Wind Min", EVAV_EVVIT_Vector, &p_SoftBody->st_WindMin, 0, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem1("Wind Max", EVAV_EVVIT_Vector, &p_SoftBody->st_WindMax, 0, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem1("Wind U Scale", EVAV_EVVIT_Float, &p_SoftBody->f_WindUScale, 0, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem1("Wind V Scale", EVAV_EVVIT_Float, &p_SoftBody->f_WindVScale, 0, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem1("Wind U Pan", EVAV_EVVIT_Float, &p_SoftBody->f_WindUPan, 0, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem1("Wind V Pan", EVAV_EVVIT_Float, &p_SoftBody->f_WindVPan, 0, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem1Private("Static Friction", EVAV_EVVIT_Float, &p_SoftBody->f_StaticFriction, 0, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem1Private("Kinetic Friction", EVAV_EVVIT_Float, &p_SoftBody->f_KineticFriction, 0, NULL);
                po_Item->mi_NumFields++;

                M_InsertItemRO("Nb Vertices", EVAV_EVVIT_Int, &p_SoftBody->ul_NbVertices, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem1("List Vertices", EVAV_EVVIT_Pointer, p_SoftBody, LINK_C_MDF_SoftBodyListVertices, NULL);
                po_Item->mi_NumFields++;
                M_InsertItemRO("Nb Rods", EVAV_EVVIT_Int, &p_SoftBody->ul_NbRods, NULL);
                po_Item->mi_NumFields++;
                M_InsertItem1("List Rods", EVAV_EVVIT_Pointer, p_SoftBody, LINK_C_MDF_SoftBodyListRods, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Nb Col Plane", EVAV_EVVIT_Int, &p_SoftBody->ul_NbCollPlane, MDF_SoftBody_CallBack_NbPlanes);
                po_Item->mi_NumFields++;

                M_InsertItem1("List Planes", EVAV_EVVIT_Pointer, p_SoftBody, LINK_C_MDF_SoftBodyListPlanes, NULL);
                po_Item->mi_NumFields++;

                //////////////////////////////////////////////////////////////////////////
                // Hide these fields to integrators. Values have been tweeked and should not be
                // changed. The values are hardcoded into soft body.
                //////////////////////////////////////////////////////////////////////////
                /*
                // tearing code
                M_InsertItem1("Damping Length", EVAV_EVVIT_Float, &p_SoftBody->f_DampLength, 0, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem1("Initial Speed", EVAV_EVVIT_Float, &p_SoftBody->f_InitialSpeed, 0, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem1("Acceleration", EVAV_EVVIT_Float, &p_SoftBody->f_Acceleration, 0, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem1("First Angle", EVAV_EVVIT_Float, &p_SoftBody->f_Angles[0], 0, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem1("Second Angle", EVAV_EVVIT_Float, &p_SoftBody->f_Angles[1], 0, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem1("Max Swing", EVAV_EVVIT_Float, &p_SoftBody->f_MaxSwing, 0, NULL);
                po_Item->mi_NumFields++;
                */

                // dress code
                M_InsertItem1("Actor", EVAV_EVVIT_GO, &p_SoftBody->actor, (int) (*ppst_Mod)->pst_GO, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Father Channel", EVAV_EVVIT_Int, &p_SoftBody->fatherChannel, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Max Distance Between 2 Frames", EVAV_EVVIT_Float, &p_SoftBody->f_DampingDistance, NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Blend Ratio", EVAV_EVVIT_Float, &p_SoftBody->f_BlendRatio, NULL);
                po_Item->mi_NumFields++;

                // spring code
                /*
                M_InsertItem("Collider Channel 0", EVAV_EVVIT_Int, &p_SoftBody->channels[0], NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Collider Channel 1", EVAV_EVVIT_Int, &p_SoftBody->channels[1], NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Collider Channel 2", EVAV_EVVIT_Int, &p_SoftBody->channels[2], NULL);
                po_Item->mi_NumFields++;

                M_InsertItem("Collider Channel 3", EVAV_EVVIT_Int, &p_SoftBody->channels[3], NULL);				
                po_Item->mi_NumFields++;

                char boneLabel[40],vertexLabel[40];
                for(int i=0;i<10;i++)
                {					
                sprintf(boneLabel,"Bone Channel %i",i);
                sprintf(vertexLabel,"Vertex ID %i",i);

                M_InsertItem(boneLabel, EVAV_EVVIT_Int, &p_SoftBody->springs[i].m_boneChannel, MDF_SoftBody_CallBack_InitSpringLength);
                po_Item->mi_NumFields++;
                M_InsertItem(vertexLabel, EVAV_EVVIT_Int, &p_SoftBody->springs[i].m_vertexID, MDF_SoftBody_CallBack_InitSpringLength);
                po_Item->mi_NumFields++;
                }
                */
            }
            break;
        case MDF_C_Modifier_Wind:
            {
                GAO_tdst_ModifierWind * pst_Wind;
                pst_Wind = (GAO_tdst_ModifierWind *) (*ppst_Mod)->p_Data;

                if (pst_Wind)
                {
                    char asz_temp[128];
                    CWindSource * p_Source = pst_Wind->po_Source;

                    // Title
                    sprintf(asz_temp, "Wind Source Modifier");
                    M_InsertItemROColorSpecial(asz_temp, 0, NULL, (int) pst_Wind);
                    po_Item->mi_NumFields++;

                    M_InsertItem("Edit Wind Curve", EVAV_EVVIT_WindCurve, pst_Wind, NULL);
                    po_Item->mi_NumFields++;

                    if(!p_Source->m_bIsRadial)
                    {
                        M_InsertItem1("Plane Length", EVAV_EVVIT_Float, &p_Source->m_fDx, 0, NULL);						
                        M_InsertItem1("Plane Height", EVAV_EVVIT_Float, &p_Source->m_fDz, 0, NULL);						
                        M_InsertItem1("Error Direction (degree)", EVAV_EVVIT_Float, &p_Source->m_fErrorAngleDirection, 0, MDF_Wind_CallBack_Refresh);						
                        M_InsertItem1("Variation Direction", EVAV_EVVIT_Float, &p_Source->m_fVariationDirection, 0, NULL);
                        po_Item->mi_NumFields+=4;
                    }

                    M_InsertItem1("Dynamic Source", EVAV_EVVIT_Bool, &p_Source->m_bIsDynamic, 0, MDF_Wind_CallBack_Refresh);
                    po_Item->mi_NumFields++;

                    if(p_Source->m_bIsDynamic)
                    {						
                        M_InsertItem1("Radial", EVAV_EVVIT_Bool, &p_Source->m_bIsRadial, 0, MDF_Wind_CallBack_Refresh);						
                        M_InsertItem("Near", EVAV_EVVIT_Float, &p_Source->m_fNear, LINK_CallBack_Refresh3DEngine);
                        M_InsertItem("Far", EVAV_EVVIT_Float, &p_Source->m_fFar, LINK_CallBack_Refresh3DEngine);
                        M_InsertItem1("Wind Behind Plane", EVAV_EVVIT_Bool, &p_Source->m_bWindBehindPlane, 0, NULL);
                        po_Item->mi_NumFields+=4;

                        M_InsertItem1("Speed Modulated", EVAV_EVVIT_Bool, &p_Source->m_bIsSpeedModulated, 0, MDF_Wind_CallBack_Refresh);
                        po_Item->mi_NumFields++;

                        if(p_Source->m_bIsSpeedModulated)
                        {												
                            M_InsertItem("Speed Modulation [0,infinity]", EVAV_EVVIT_Float, &p_Source->m_fSpeedModulation, LINK_CallBack_Refresh3DEngine);						
                            po_Item->mi_NumFields+=1;
                        }
                    }					
                }
            }
            break;
#endif
		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef JADEFUSION //POPOWARNING
		case MDF_C_Modifier_FClone:
			p_FCLONE = (FCLONE_tdst_Modifier *) (*ppst_Mod)->p_Data;
			M_InsertItemRO("Number clones", EVAV_EVVIT_Int, &p_FCLONE->NBR_Instances, NULL);
			//po_NewItem->psz_Help = "Number of clones";
			M_InsertItem1("Flags", EVAV_EVVIT_SubStruct, &p_FCLONE->ulFlags, LINK_C_ModifierCloneFlags, NULL);
			/*M_InsertItem("offset normal", EVAV_EVVIT_Float, &p_FUR->f_NormalOffset, NULL);
			M_InsertItem("offset U", EVAV_EVVIT_Float, &p_FUR->f_UOffset, NULL);
			M_InsertItem("offset V", EVAV_EVVIT_Float, &p_FUR->f_VOffset, NULL);*/
			po_Item->mi_NumFields+=1;
			break;
#endif		
		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		case MDF_C_Modifier_UVTexWave:
			{
				//int i;
				p_UVTexWave = (UVTexWave_tdst_Modifier *) (*ppst_Mod)->p_Data;
				//for (i=0;i<5;i++)
				{
					M_InsertItem("Mat Level", EVAV_EVVIT_Int, &p_UVTexWave->MatLevel, NULL);
					M_InsertItem("Speed Coef U", EVAV_EVVIT_Float, &p_UVTexWave->f_SpeedCoefU, NULL);
					M_InsertItem("Speed Coef V", EVAV_EVVIT_Float, &p_UVTexWave->f_SpeedCoefV, NULL);
					M_InsertItem("Rayon U", EVAV_EVVIT_Float, &p_UVTexWave->f_AngleU, NULL);
					M_InsertItem("Rayon V", EVAV_EVVIT_Float, &p_UVTexWave->f_AngleV, NULL);
				}
				po_Item->mi_NumFields+=5;
			}
			break;
	}


		M_InsertItem1("Next modifier", EVAV_EVVIT_Pointer, &(*ppst_Mod)->pst_Next, LINK_C_MDF_Modifier, NULL);
		po_Item->mi_NumFields++;

		EndPos = pos;
		{
			EVAV_cl_ViewItem *pParser;
			ULONG ColorOTO;
			pParser = mpo_ListItems ->GetAt(InitPos);
			ColorOTO = MDF_SPG2_CallBack_GetColorOfThisOne((int) (*ppst_Mod)->pst_GO , (int) *ppst_Mod);
			while (InitPos != EndPos)
			{
				pParser->mx_Color = ColorOTO ;
				pParser = mpo_ListItems ->GetNext(InitPos);
			}
		}
	}
}

/*$4
 ***********************************************************************************************************************
    Animated texture
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VAV_TEXANI_OnNumberChange(void *p_Owner, void *_po_Data, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	EVAV_cl_ViewItem	*po_Parent;
	UCHAR				uc_NewValue;
	TEX_tdst_Animated	*pst_Ani;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uc_NewValue = *(UCHAR *) p_Data;
	if((UCHAR) (l_OldData & 0xFF) == uc_NewValue) return;

	po_Item = (EVAV_cl_ViewItem *) _po_Data;
	po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_TEX_Animated);
	if(po_Parent == NULL) return;

	pst_Ani = (TEX_tdst_Animated *) po_Parent->mp_Data;

	if(pst_Ani->dst_Tex)
	{
		if(uc_NewValue)
		{
			pst_Ani->dst_Tex = (TEX_tdst_AnimTex *) MEM_p_Realloc
				(
					pst_Ani->dst_Tex,
					((int) uc_NewValue) * sizeof(TEX_tdst_AnimTex)
				);
		}
		else
		{
			MEM_Free(pst_Ani->dst_Tex);
			pst_Ani->dst_Tex = NULL;
		}
	}
	else
		pst_Ani->dst_Tex = (TEX_tdst_AnimTex *) MEM_p_Alloc(((int) uc_NewValue) * sizeof(TEX_tdst_AnimTex));

	if(uc_NewValue > (UCHAR) (l_OldData & 0xFF))
	{
		L_memset
		(
			pst_Ani->dst_Tex + (UCHAR) (l_OldData & 0xFF),
			0,
			sizeof(TEX_tdst_AnimTex) * (uc_NewValue - (UCHAR) (l_OldData & 0xFF))
		);
	}

	TEX_Anim_Save();

	EDI_OUT_gl_ForceSetMode = TRUE;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = FALSE;

	LINK_UpdatePointer(pst_Ani);
	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VAV_TEXANI_OnTexKeyChange(void *p_Owner, void *_po_Data, void *p_Data, long l_OldData)
{
	TEX_Anim_Save();
	EDI_OUT_gl_ForceSetMode = TRUE;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::TEXAddAnimated(POSITION pos, void *_pst_Ani)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				sz_Text[64];
	int					i;
	EVAV_cl_ViewItem	*po_Item, *po_NewItem;
	TEX_tdst_Animated	*pst_Ani;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Ani = (TEX_tdst_Animated *) _pst_Ani;

	/* Get the pointer of instance */
	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);

	M_InsertItem123("Number Of Textures", EVAV_EVVIT_Int, &pst_Ani->uc_Number, 0, 0, 1, VAV_TEXANI_OnNumberChange);
	M_InsertItem123RO("Current Texture", EVAV_EVVIT_Int, &pst_Ani->uc_Current, 0, 0, 1, NULL);
	M_InsertItem123RO("Counter", EVAV_EVVIT_Int, &pst_Ani->w_Counter, 0, 0, 2, NULL);

	po_Item->mi_NumFields = 3;

	for(i = 0; i < pst_Ani->uc_Number; i++)
	{
		sprintf(sz_Text, "Texture %d", i);
		M_InsertItem(sz_Text, EVAV_EVVIT_Key, &pst_Ani->dst_Tex[i].ul_Key, VAV_TEXANI_OnTexKeyChange);
		sprintf(sz_Text, "Delay %d", i);
		M_InsertItem123(sz_Text, EVAV_EVVIT_Int, &pst_Ani->dst_Tex[i].w_Time, 0, 0, 2, NULL);
		po_Item->mi_NumFields += 2;
	}
}

/*$4
 ***********************************************************************************************************************
    Procedural texture
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Function called when the number of sprites in SpriteList visuel change
 =======================================================================================================================
 */
void VAV_TEXPRO_OnTypeChange(void *p_Owner, void *_po_Data, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	EVAV_cl_ViewItem	*po_Parent;
	TEX_tdst_Procedural *pst_Pro;
	LONG				l_NewType;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_NewType = *(long *) p_Data;
	if(l_OldData == l_NewType) return;

	po_Item = (EVAV_cl_ViewItem *) _po_Data;
	po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_TEX_Procedural);
	if(po_Parent == NULL) return;

	pst_Pro = (TEX_tdst_Procedural *) po_Parent->mp_Data;

	pst_Pro->i->pfnv_Free(pst_Pro);
	pst_Pro->p_Data = NULL;
	pst_Pro->i = &TEX_gast_ProceduralInterface[l_NewType];
	pst_Pro->p_Data = pst_Pro->i->pfnpv_Load(NULL, 0, pst_Pro->uw_Width, pst_Pro->uw_Height);

	LINK_UpdatePointer(pst_Pro);
	LINK_UpdatePointers();
}

/* Aim: Function called when the number of sprites in SpriteList visuel change */
#ifndef JADEFUSION 
extern "C"
{
extern ULONG	EDI_OUT_gl_ForceSetMode;
};
#else
extern ULONG	EDI_OUT_gl_ForceSetMode;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VAV_TEXPRO_OnSizeChange(void *p_Owner, void *_po_Data, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	EVAV_cl_ViewItem	*po_Parent;
	USHORT				uw_NewValue;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_NewValue = *(USHORT *) p_Data;
	if((USHORT) l_OldData == uw_NewValue) return;

	po_Item = (EVAV_cl_ViewItem *) _po_Data;
	po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_TEX_Procedural);
	if(po_Parent == NULL) return;

	TEX_Procedural_Save();

	EDI_OUT_gl_ForceSetMode = TRUE;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = FALSE;
}

/*
 =======================================================================================================================
    Aim:    Function called when the number of sprites in SpriteList visuel change
 =======================================================================================================================
 */
void VAV_TEXPRO_OnMpegChange(void *p_Owner, void *_po_Data, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	EVAV_cl_ViewItem	*po_Parent;
	TEX_tdst_Procedural *pst_Pro;
	ULONG				ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Key = *(ULONG *) p_Data;
	if((ULONG) l_OldData == ul_Key) return;

	po_Item = (EVAV_cl_ViewItem *) _po_Data;
	po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_TEX_Procedural);
	if(po_Parent == NULL) return;

	pst_Pro = (TEX_tdst_Procedural *) po_Parent->mp_Data;
	TEXPRO_Mpeg_SetKey(pst_Pro, ul_Key);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VAV_TEXPRO_OnMpegIPUChange(void *p_Owner, void *_po_Data, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	EVAV_cl_ViewItem	*po_Parent;
	TEX_tdst_Procedural *pst_Pro;
	ULONG				ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Key = *(ULONG *) p_Data;
	if((ULONG) l_OldData == ul_Key) return;

	po_Item = (EVAV_cl_ViewItem *) _po_Data;
	po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_TEX_Procedural);
	if(po_Parent == NULL) return;

	pst_Pro = (TEX_tdst_Procedural *) po_Parent->mp_Data;
	TEXPRO_Mpeg_SetIPUKey(pst_Pro, ul_Key);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VAV_TEXPRO_OnMpegBinkChange(void *p_Owner, void *_po_Data, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	EVAV_cl_ViewItem	*po_Parent;
	TEX_tdst_Procedural *pst_Pro;
	ULONG				ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Key = *(ULONG *) p_Data;
	if((ULONG) l_OldData == ul_Key) return;

	po_Item = (EVAV_cl_ViewItem *) _po_Data;
	po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_TEX_Procedural);
	if(po_Parent == NULL) return;

	pst_Pro = (TEX_tdst_Procedural *) po_Parent->mp_Data;
	TEXPRO_Mpeg_SetBinkKey(pst_Pro, ul_Key);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::TEXAddProcedural(POSITION pos, void *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item, *po_NewItem;
	TEX_tdst_Procedural *pst_Pro;
	TEXPRO_tdst_Mpeg	*pst_Mpeg;
	TEXPRO_tdst_Photo	*pst_Photo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Pro = (TEX_tdst_Procedural *) _pst_Pro;

	/* Get the pointer of instance */
	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);

	M_InsertItem4clbk
	(
		"Type",
		EVAV_EVVIT_ConstInt,
		&pst_Pro->ul_Type,
		VAV_TEXPRO_OnTypeChange,
		(int) "Unknow\n0\nWater\n1\nFire\n2\nMpeg\n3\nPhoto\n4\nPlasma\n5\n"
	);
	M_InsertItem1("Flags", EVAV_EVVIT_SubStruct, &pst_Pro->uw_Flags, LINK_C_TEX_ProceduralFlag, NULL);
	M_InsertItem123("Width", EVAV_EVVIT_Int, &pst_Pro->uw_Width, 0, 0, 2, VAV_TEXPRO_OnSizeChange);
	M_InsertItem123("Height", EVAV_EVVIT_Int, &pst_Pro->uw_Height, 0, 0, 2, VAV_TEXPRO_OnSizeChange);

	po_Item->mi_NumFields = 4;

	switch(pst_Pro->i->ul_Type)
	{
	case TEXPRO_Unknow:
		break;
	case TEXPRO_Water:
		break;
	case TEXPRO_Fire:
		break;
	case TEXPRO_Mpeg:
		pst_Mpeg = (TEXPRO_tdst_Mpeg *) pst_Pro->p_Data;
		M_InsertItem("Mpeg file", EVAV_EVVIT_Key, &pst_Mpeg->ul_Key, VAV_TEXPRO_OnMpegChange);
		M_InsertItem("IPU Psx2 file", EVAV_EVVIT_Key, &pst_Mpeg->ul_PSX2_IPUKey, VAV_TEXPRO_OnMpegIPUChange);

		/* if (pst_Mpeg->ul_PSX2_IPUKey != -1) */
		{
			M_InsertItemRO("IPU Width", EVAV_EVVIT_Int, &pst_Mpeg->ul_SizeX, NULL);
			M_InsertItemRO("IPU Height", EVAV_EVVIT_Int, &pst_Mpeg->ul_SizeY, NULL);
			M_InsertItemRO("IPU Size", EVAV_EVVIT_Int, &pst_Mpeg->ul_Size, NULL);
			M_InsertItemRO("IPU number of image", EVAV_EVVIT_Int, &pst_Mpeg->ul_NbImages, NULL);
			M_InsertItemRO("IPU image max size", EVAV_EVVIT_Int, &pst_Mpeg->ul_MaxBufSize, NULL);
			po_Item->mi_NumFields += 5;
		}
		M_InsertItem("BINK GameCube file", EVAV_EVVIT_Key, &pst_Mpeg->ul_BINK_Key, VAV_TEXPRO_OnMpegBinkChange);
		M_InsertItemRO("Bink Width", EVAV_EVVIT_Int, &pst_Mpeg->ul_BinkSizeX, NULL);
		M_InsertItemRO("Bink Height", EVAV_EVVIT_Int, &pst_Mpeg->ul_BinkSizeX, NULL);
		po_Item->mi_NumFields += 3;
		M_InsertItem("XMV xbox file", EVAV_EVVIT_Key, &pst_Mpeg->ul_XMV_Key, NULL);
		po_Item->mi_NumFields += 1;

		M_InsertItem123("Flags", EVAV_EVVIT_Int, &pst_Mpeg->c_Flags, 0, 0, 1, NULL);
		M_InsertItem123("Request", EVAV_EVVIT_Int, &pst_Mpeg->c_Request, 0, 0, 1, NULL);
		po_Item->mi_NumFields += 4;
		break;
	case TEXPRO_Photo:
		pst_Photo = (TEXPRO_tdst_Photo *) pst_Pro->p_Data;
		M_InsertItem123("Current photo index", EVAV_EVVIT_Int, &pst_Photo->c_Photo, 0, 0, 1, NULL);
		M_InsertItem123("Asked photo index", EVAV_EVVIT_Int, &pst_Photo->c_AskedPhoto, 0, 0, 1, NULL);
		M_InsertItem123("Update current photo", EVAV_EVVIT_Int, &pst_Photo->c_Update, 0, 0, 1, NULL);
		po_Item->mi_NumFields += 3;
		break;
	}
}

/*$4
 ***********************************************************************************************************************
    Sprite generator
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Function called when the number of sprites in SpriteList visuel change
 =======================================================================================================================
 */
void VAV_SPRITEGEN_OnTextureChange(void *p_Owner, void *_po_Data, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_SpriteGen	*pst_SprGen;
	TEX_tdst_Data		*pst_Texture;
	EVAV_cl_ViewItem	*po_Parent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!l_OldData || (*(LONG *) p_Data == l_OldData)) return;

	po_Parent = gpo_CurVavListBox->po_GetTypedParent((EVAV_cl_ViewItem *) _po_Data, LINK_C_ENG_SpriteGen);
	pst_SprGen = (MAT_tdst_SpriteGen *) po_Parent->mp_Data;

	pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_SprGen->TEXTURE_BIGKEY);
	if(pst_Texture)
		pst_SprGen->s_TextureIndex = pst_Texture->w_Index;
	else
		pst_SprGen->s_TextureIndex = TEX_w_List_AddTexture(&TEX_gst_GlobalList, pst_SprGen->TEXTURE_BIGKEY, 1);
	EDI_OUT_gl_ForceSetMode = 1;
	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}

/*
 =======================================================================================================================
    Aim:    Function called when the number of sprites in SpriteList visuel change
 =======================================================================================================================
 */
void VAV_SPRITEGEN_OnBumpMapChange(void *p_Owner, void *_po_Data, void *p_Data, long l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_SpriteGen		*pst_SprGen;
	EVAV_cl_ViewItem		*po_Parent;
	MAT_SPR_tdst_BumpMap	*pst_OldBM;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!l_OldData || (*(LONG *) p_Data == l_OldData)) return;

	po_Parent = gpo_CurVavListBox->po_GetTypedParent((EVAV_cl_ViewItem *) _po_Data, LINK_C_ENG_SpriteGen);
	pst_SprGen = (MAT_tdst_SpriteGen *) po_Parent->mp_Data;

	pst_OldBM = pst_SprGen->p_BMap;
	pst_SprGen->p_BMap = MAT_pst_BumpMap_Add(pst_SprGen->XYZSMap_BIGKEY);
	if((pst_OldBM != NULL) && (pst_SprGen->p_BMap != pst_OldBM)) MAT_BumpMap_Del(pst_OldBM);

	LINK_Refresh();
}

/*
 =======================================================================================================================
    sprite generator
 =======================================================================================================================
 */
void EVAV_cl_ListBox::TEXAddSpriteGen(POSITION pos, void *_pst_SpriteGen)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item, *po_NewItem;
	MAT_tdst_SpriteGen	*pst_SG;
	float				f_Min, f_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get the pointer of instance */
	if(!pos)
		po_Item = mpo_ListItems->GetTail();
	else
		po_Item = mpo_ListItems->GetAt(pos);

	pst_SG = (MAT_tdst_SpriteGen *) _pst_SpriteGen;

	M_InsertItem123
	(
		"Flags",
		EVAV_EVVIT_SubStruct,
		&pst_SG->flags,
		LINK_C_ENG_SpriteGenFlags,
		0,
		0,
		LINK_CallBack_Refresh3DEngine
	);
	M_InsertItem12
	(
		"Texture",
		EVAV_EVVIT_Key,
		&pst_SG->TEXTURE_BIGKEY,
		(int) EDI_Csz_Path_Textures,
		(int) "*.tga,*.jpg,*.bmp",
		VAV_SPRITEGEN_OnTextureChange
	);

	f_Min = 0;
	f_Max = Cf_Infinit;
	M_InsertItem12
	(
		"Size factor",
		EVAV_EVVIT_Float,
		&pst_SG->Size,
		*(int *) &f_Min,
		*(int *) &f_Max,
		LINK_CallBack_Refresh3DEngine
	);
	M_InsertItem("Z Extraction", EVAV_EVVIT_Float, &pst_SG->ZExtraction, LINK_CallBack_Refresh3DEngine);
	f_Max = 20.0f;
	M_InsertItem12
	(
		"Noise",
		EVAV_EVVIT_Float,
		&pst_SG->Noise,
		*(int *) &f_Min,
		*(int *) &f_Max,
		LINK_CallBack_Refresh3DEngine
	);
	f_Max = 2.0f;
	M_InsertItem12
	(
		"Noise Size",
		EVAV_EVVIT_Float,
		&pst_SG->SizeNoise,
		*(int *) &f_Min,
		*(int *) &f_Max,
		LINK_CallBack_Refresh3DEngine
	);
	f_Min = 0.01f;
	f_Max = Cf_Infinit;
	M_InsertItem12
	(
		"Mipmap correction",
		EVAV_EVVIT_Float,
		&pst_SG->MipMapCoef,
		*(int *) &f_Min,
		*(int *) &f_Max,
		LINK_CallBack_Refresh3DEngine
	);
	f_Min = 0;
	M_InsertItem12
	(
		"Distortion max",
		EVAV_EVVIT_Float,
		&pst_SG->DistortionMax,
		*(int *) &f_Min,
		*(int *) &f_Max,
		LINK_CallBack_Refresh3DEngine
	);
	M_InsertItem12
	(
		"Bump MAP (Square RAW 32 bit!)",
		EVAV_EVVIT_Key,
		&pst_SG->XYZSMap_BIGKEY,
		(int) EDI_Csz_Path_Textures,
		(int) "*.raw",
		VAV_SPRITEGEN_OnBumpMapChange
	);
	M_InsertItem("Bump Factor", EVAV_EVVIT_Float, &pst_SG->fBumpFactor, LINK_CallBack_Refresh3DEngine);

	po_Item->mi_NumFields = 10;
}

