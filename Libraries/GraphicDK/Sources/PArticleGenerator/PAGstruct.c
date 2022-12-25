/*$T PAGstruct.c GC! 1.081 06/28/00 15:17:14 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "PArticleGenerator/PAGstruct.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTzlist.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "GFX/GFX.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_MPAG.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"

#ifdef PSX2_TARGET
/* mamagouille */
#include "PSX2debug.h"
#endif

#include "BASe/BENch/BENch.h"

#ifdef _GAMECUBE
#ifndef _FINAL_
#include "GXI_GC/GXI_dbg.h"
#endif
#endif


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
PAG_tdst_Struct *PAG_pst_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_P;
	/*~~~~~~~~~~~~~~~~~~~*/

	pst_P = (PAG_tdst_Struct *) MEM_p_Alloc(sizeof(PAG_tdst_Struct));
	L_memset(pst_P, 0, sizeof(PAG_tdst_Struct));
	GRO_Struct_Init(&pst_P->st_Id, GRO_ParticleGenerator);
    pst_P->w_NbGenerator = 1;
    pst_P->ast_Gen[0].i_Id = 1;
    pst_P->ast_Gen[0].p_GO = NULL;
	return pst_P;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_Free(PAG_tdst_Struct *_pst_P)
{
	if(_pst_P == NULL) return;
	if(_pst_P->st_Id.i->ul_Type != GRO_ParticleGenerator) return;
	if(_pst_P->st_Id.l_Ref > 0) return;

	GRO_Struct_Free(&_pst_P->st_Id);
	if(_pst_P->l_NbMaxP) MEM_Free(_pst_P->dst_P);

	LOA_DeleteAddress(_pst_P);
	MEM_Free(_pst_P);
}
#if defined(XML_CONV_TOOL)
ULONG gPAGVersion = 0;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *PAG_p_CreateFromBuffer(GRO_tdst_Struct *_pst_Id, char **ppc_Buffer, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_P;
	LONG			l_Version;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_P = PAG_pst_Create();
	pst_P->l_NbMaxP = LOA_ReadLong(ppc_Buffer);

	/* swap version */
	l_Version = LOA_ReadLong(ppc_Buffer);
#if defined(XML_CONV_TOOL)
	gPAGVersion = l_Version;
#endif

	if(l_Version == 0)
	{
		pst_P->ul_Flags = LOA_ReadUChar(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		LOA_ReadChar_Ed(ppc_Buffer, NULL); //skip
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecondInit = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_GenTime = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadLong_Ed(ppc_Buffer, NULL); //skip 12
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		LOA_ReadLong_Ed(ppc_Buffer, NULL); //skip 12
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
	}
	else if(l_Version == 1)
	{
		pst_P->ul_Flags = LOA_ReadUChar(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		LOA_ReadChar_Ed(ppc_Buffer, NULL); //skip
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_GenTime = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadLong_Ed(ppc_Buffer, NULL); //skip 12
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		LOA_ReadLong_Ed(ppc_Buffer, NULL); //skip 12
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
	}
	else if(l_Version == 2)
	{
		pst_P->ul_Flags = LOA_ReadShort(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_GenTime = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadLong_Ed(ppc_Buffer, NULL); //skip 12
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		LOA_ReadLong_Ed(ppc_Buffer, NULL); //skip 12
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		LOA_ReadLong_Ed(ppc_Buffer, NULL);
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
	}
	else if(l_Version == 3)
	{
		pst_P->ul_Flags = LOA_ReadShort(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_GenTime = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
	}
	else if(l_Version == 4)
	{
		pst_P->ul_Flags = LOA_ReadShort(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadLong_Ed(ppc_Buffer, NULL); //skip
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->ul_Color = LOA_ReadULong(ppc_Buffer);
	}
	else if(l_Version == 5)
	{
		pst_P->ul_Flags = LOA_ReadShort(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMax = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->ul_Color = LOA_ReadULong(ppc_Buffer);
	}
	else if(l_Version == 6)
	{
		pst_P->ul_Flags = LOA_ReadShort(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMax = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->ul_Color = LOA_ReadULong(ppc_Buffer);
		pst_P->f_ZMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMinStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMax = LOA_ReadFloat(ppc_Buffer);
	}
	else if(l_Version == 7)
	{
		pst_P->ul_Flags = LOA_ReadShort(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMax = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_Friction = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->ul_Color = LOA_ReadULong(ppc_Buffer);
		pst_P->f_ZMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMinStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMaxStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationSpeedMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationSpeedMax = LOA_ReadFloat(ppc_Buffer);
	}
	else if(l_Version == 8)
	{
		pst_P->ul_Flags = LOA_ReadShort(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		pst_P->f_GenOffset = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMax = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_Friction = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->ul_Color = LOA_ReadULong(ppc_Buffer);
		pst_P->f_ZMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMinStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMaxStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationSpeedMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationSpeedMax = LOA_ReadFloat(ppc_Buffer);
	}
	else if(l_Version == 9)
	{
		pst_P->ul_Flags = LOA_ReadShort(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		pst_P->f_GenOffset = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecondInit = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = pst_P->ast_Gen[0].f_NbPerSecondInit;
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMax = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_Friction = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->ul_Color = LOA_ReadULong(ppc_Buffer);
		pst_P->f_ZMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMinStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMaxStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationSpeedMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationSpeedMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SinXFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SinYFactor = LOA_ReadFloat(ppc_Buffer);
	}
	else if(l_Version == 10)
	{
		pst_P->ul_Flags = LOA_ReadUShort(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		pst_P->f_GenOffset = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecondInit = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = pst_P->ast_Gen[0].f_NbPerSecondInit;
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMax = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_Friction = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->ul_Color = LOA_ReadULong(ppc_Buffer);
		pst_P->f_ZMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMinStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMaxStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationSpeedMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationSpeedMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SinXFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SinYFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_DistConstraint = LOA_ReadFloat(ppc_Buffer);
	}
	else if (l_Version == 11)
	{
		pst_P->ul_Flags = LOA_ReadULong(ppc_Buffer);
		pst_P->c_GenType = LOA_ReadChar(ppc_Buffer);
		pst_P->c_SpeedType = LOA_ReadChar(ppc_Buffer);
		pst_P->f_GenOffset = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[0] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[1] = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_GenParam[2] = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecondInit = LOA_ReadFloat(ppc_Buffer);
		pst_P->ast_Gen[0].f_NbPerSecond = pst_P->ast_Gen[0].f_NbPerSecondInit;
		pst_P->f_Speed0 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Speed1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle1 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_Angle2 = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeXMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeYMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeDeathMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_TimeBirthMax = LOA_ReadFloat(ppc_Buffer);
		LOA_ReadVector(ppc_Buffer, &pst_P->st_Acc);
		pst_P->f_Friction = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SizeDeathFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->ul_Color = LOA_ReadULong(ppc_Buffer);
		pst_P->f_ZMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMinStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_ZMaxStrength = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationSpeedMin = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_RotationSpeedMax = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SinXFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_SinYFactor = LOA_ReadFloat(ppc_Buffer);
		pst_P->f_DistConstraint = LOA_ReadFloat(ppc_Buffer);
	}

	if(pst_P->l_NbMaxP)
		pst_P->dst_P = (PAG_tdst_P *) MEM_p_Alloc(sizeof(PAG_tdst_P) * pst_P->l_NbMaxP);
	else
		pst_P->dst_P = NULL;

    pst_P->w_NbGenerator = 1;
    pst_P->ast_Gen[0].i_Id = 1;
    pst_P->ast_Gen[0].p_GO = NULL;

	return pst_P;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *PAG_p_Duplicate(PAG_tdst_Struct *_pst_P, char *_asz_Path, char * _sz_Name, ULONG _ul_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_P;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_P = PAG_pst_Create();
	L_memcpy(pst_P, _pst_P, sizeof(PAG_tdst_Struct));

#ifdef ACTIVE_EDITORS
	((GRO_tdst_Struct *) pst_P)->sz_Name = NULL;
#endif

	if(pst_P->l_NbMaxP)
	{
		pst_P->dst_P = (PAG_tdst_P *) MEM_p_Alloc(sizeof(PAG_tdst_P) * pst_P->l_NbMaxP);
		L_memcpy(pst_P->dst_P, _pst_P->dst_P, sizeof(PAG_tdst_P) * pst_P->l_NbMaxP);
	}

#ifdef ACTIVE_EDITORS
	if(_asz_Path) GRO_ul_Struct_FullSave((GRO_tdst_Struct *) pst_P, _asz_Path, NULL, NULL);
#endif
	return pst_P;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG PAG_l_SaveInBuffer(PAG_tdst_Struct *_pst_P, void *p_Unused)
{
	GRO_Struct_Save(&_pst_P->st_Id);
	return PAG_l_SaveInBufferCommon(_pst_P, p_Unused);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG PAG_l_SaveInBufferCommon(PAG_tdst_Struct *_pst_P, void *p_Unused)
{
	/*~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	LONG	l_Version;
	/*~~~~~~~~~~~~~~*/

	SAV_Buffer(&_pst_P->l_NbMaxP, 4);
#if defined(XML_CONV_TOOL)
	l_Version = gPAGVersion;
#else
	l_Version = 11;
#endif
	SAV_Buffer(&l_Version, 4);
#ifdef JADEFUSION
	if (l_Version == 9)
		SAV_Buffer(&_pst_P->ul_Flags, 2);
	else
#endif
	SAV_Buffer(&_pst_P->ul_Flags, 4);
	SAV_Buffer(&_pst_P->c_GenType, 1);
	SAV_Buffer(&_pst_P->c_SpeedType, 1);
	SAV_Buffer(&_pst_P->f_GenOffset, 4);
	SAV_Buffer(_pst_P->f_GenParam, 12);
	SAV_Buffer(&_pst_P->ast_Gen[0].f_NbPerSecondInit, 4);
	SAV_Buffer(&_pst_P->f_Speed0, 4);
	SAV_Buffer(&_pst_P->f_Speed1, 4);
	SAV_Buffer(&_pst_P->f_Angle1, 4);
	SAV_Buffer(&_pst_P->f_Angle2, 4);
	SAV_Buffer(&_pst_P->f_SizeXMin, 4);
	SAV_Buffer(&_pst_P->f_SizeXMax, 4);
	SAV_Buffer(&_pst_P->f_SizeYMin, 4);
	SAV_Buffer(&_pst_P->f_SizeYMax, 4);
	SAV_Buffer(&_pst_P->f_TimeMin, 4);
	SAV_Buffer(&_pst_P->f_TimeMax, 4);
	SAV_Buffer(&_pst_P->f_TimeDeathMin, 4);
	SAV_Buffer(&_pst_P->f_TimeDeathMax, 4);
	SAV_Buffer(&_pst_P->f_TimeBirthMin, 4);
	SAV_Buffer(&_pst_P->f_TimeBirthMax, 4);
	SAV_Buffer(&_pst_P->st_Acc, 12);
	SAV_Buffer(&_pst_P->f_Friction, 4);
	SAV_Buffer(&_pst_P->f_SizeDeathFactor, 4);
	SAV_Buffer(&_pst_P->ul_Color, 4);
	SAV_Buffer(&_pst_P->f_ZMin, 4);
	SAV_Buffer(&_pst_P->f_ZMinStrength, 4);
	SAV_Buffer(&_pst_P->f_ZMax, 4);
	SAV_Buffer(&_pst_P->f_ZMaxStrength, 4);
	SAV_Buffer(&_pst_P->f_RotationMin, 4);
	SAV_Buffer(&_pst_P->f_RotationMax, 4);
	SAV_Buffer(&_pst_P->f_RotationSpeedMin, 4);
	SAV_Buffer(&_pst_P->f_RotationSpeedMax, 4);
	SAV_Buffer(&_pst_P->f_SinXFactor, 4);
	SAV_Buffer(&_pst_P->f_SinYFactor, 4);
#ifdef JADEFUSION
	if (l_Version != 9)
#endif
		SAV_Buffer(&_pst_P->f_DistConstraint, 4);
#endif
	return 0;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_ChangeNumberOfParticles(PAG_tdst_Struct *_pst_P, LONG _l_Number)
{
	if(_l_Number == _pst_P->l_NbMaxP) return;

	if(_pst_P->l_NbMaxP)
	{
		MEM_Free(_pst_P->dst_P);
		_pst_P->dst_P = NULL;
	}

	_pst_P->l_NbMaxP = _l_Number;
	_pst_P->l_NbP = 0;

	if(_l_Number)
	{
		_pst_P->dst_P = (PAG_tdst_P*)MEM_p_Alloc(_l_Number * sizeof(PAG_tdst_P));
	}
}

#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG PAG_l_HasSomethingToRender
(
	GRO_tdst_Visu			*_pst_Visu,
	GEO_tdst_Object			**ppst_PickableObject
)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	i_Index;
	/*~~~~~~~~~~~~~~~~~~~~*/

	*ppst_PickableObject = NULL;
	if(!(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible)) return TRUE;

#ifdef ACTIVE_EDITORS
	i_Index = GDI_gpst_CurDD->pst_CurrentGameObject->ul_InvisibleObjectIndex;
	if(i_Index > GEO_DebugObject_LastInvisible - GEO_DebugObject_Invisible) i_Index = 0;

	if( !GDI_WPShowStatusOn( GDI_gpst_CurDD, i_Index ) ) 
		return FALSE;
		
#else
	i_Index = 0;
#endif

#ifdef ACTIVE_EDITORS
	*ppst_PickableObject = GEO_pst_DebugObject_Get(GEO_DebugObject_Invisible + i_Index);
#endif

	return TRUE;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_Render_DataCommon(OBJ_tdst_GameObject *_pst_Node, PAG_tdst_Struct *pst_P)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Request_DrawPointEx	st_PtEx;
	GDI_tdst_Request_DrawLineEx		st_LineEx[3];
	SOFT_tdst_Ellipse				st_Ellipse;
	MATH_tdst_Vector				A, B, C, D, Offset;
	int								i;
	float							f_Angle, f_DAngle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* display generator */
    /*
    if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_Proportionnal)
    {
        GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix - 1;
		GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
    }
    */

	MATH_ScaleVector(&Offset, &MATH_gst_BaseVectorJ, -pst_P->f_GenOffset);
	switch(pst_P->c_GenType)
	{
	case PAG_GenType_Point:
		st_PtEx.A = &Offset;
		st_PtEx.f_Size = 5.0f;
		st_PtEx.ul_Color = 0xFFFF0000;
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointEx, (ULONG) & st_PtEx);
		break;
	case PAG_GenType_Rectangle:
		st_LineEx[0].A = &A;
		st_LineEx[0].B = &B;
		st_LineEx[0].f_Width = 3;
		st_LineEx[0].ul_Flags = 0;
		st_LineEx[0].ul_Color = 0xFFFF0000;
		MATH_ScaleVector(&A, &MATH_gst_BaseVectorI, pst_P->f_GenParam[0]);
		MATH_CopyVector(&B, &A);
		MATH_AddScaleVector(&A, &A, &MATH_gst_BaseVectorK, pst_P->f_GenParam[1]);
		MATH_AddScaleVector(&B, &B, &MATH_gst_BaseVectorK, -pst_P->f_GenParam[1]);
		MATH_AddEqualVector(&A, &Offset);
		MATH_AddEqualVector(&B, &Offset);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
		MATH_SubEqualVector(&A, &Offset);
		MATH_NegEqualVector(&A);
		MATH_AddEqualVector(&A, &Offset);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
		MATH_SubEqualVector(&B, &Offset);
		MATH_NegEqualVector(&B);
		MATH_AddEqualVector(&B, &Offset);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
		MATH_SubEqualVector(&A, &Offset);
		MATH_NegEqualVector(&A);
		MATH_AddEqualVector(&A, &Offset);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
		break;
	case PAG_GenType_Circle:
		MATH_CopyVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &Offset);
		st_Ellipse.st_Center.color = 0xFFFF0000;
		st_Ellipse.l_Flag = 5;
		MATH_ScaleVector(&st_Ellipse.st_A, &MATH_gst_BaseVectorI, pst_P->f_GenParam[0]);
		MATH_ScaleVector(&st_Ellipse.st_B, &MATH_gst_BaseVectorK, pst_P->f_GenParam[0]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		break;
	case PAG_GenType_Cylindre:
		MATH_CopyVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &Offset);
		st_Ellipse.st_Center.color = 0xFFFF0000;
		st_Ellipse.l_Flag = 5;
		MATH_ScaleVector(&st_Ellipse.st_A, &MATH_gst_BaseVectorI, pst_P->f_GenParam[0]);
		MATH_ScaleVector(&st_Ellipse.st_B, &MATH_gst_BaseVectorK, pst_P->f_GenParam[0]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		MATH_ScaleVector(&C, &MATH_gst_BaseVectorJ, -pst_P->f_GenParam[1] * .5f);
		MATH_AddEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		MATH_AddEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		st_LineEx[0].B = &B;
		st_LineEx[0].A = &st_Ellipse.st_A;
		st_LineEx[0].f_Width = 3;
		st_LineEx[0].ul_Flags = 0;
		st_LineEx[0].ul_Color = 0xFFFF0000;
		MATH_ScaleEqualVector(&C, 2);
		MATH_AddEqualVector(st_LineEx[0].A, &Offset);
		MATH_AddVector(&B, st_LineEx[0].A, &C);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
		MATH_SubEqualVector(st_LineEx[0].A, &Offset);
		MATH_NegEqualVector(st_LineEx[0].A);
		MATH_AddEqualVector(st_LineEx[0].A, &Offset);
		MATH_AddVector(&B, st_LineEx[0].A, &C);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
		MATH_CopyVector(st_LineEx[0].A, &st_Ellipse.st_B);
		MATH_AddEqualVector(st_LineEx[0].A, &Offset);
		MATH_AddVector(&B, st_LineEx[0].A, &C);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
		MATH_SubEqualVector(st_LineEx[0].A, &Offset);
		MATH_NegEqualVector(st_LineEx[0].A);
		MATH_AddEqualVector(st_LineEx[0].A, &Offset);
		MATH_AddVector(&B, st_LineEx[0].A, &C);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
		break;
	case PAG_GenType_Sphere:
		MATH_CopyVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &Offset);
		st_Ellipse.st_Center.color = 0xFFFF0000;
		st_Ellipse.l_Flag = 5;
		MATH_ScaleVector(&st_Ellipse.st_A, &MATH_gst_BaseVectorI, pst_P->f_GenParam[0]);
		MATH_ScaleVector(&st_Ellipse.st_B, &MATH_gst_BaseVectorK, pst_P->f_GenParam[0]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		MATH_ScaleVector(&C, &MATH_gst_BaseVectorJ, -pst_P->f_GenParam[0] * .5f);
		MATH_AddEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		MATH_ScaleEqualVector(&st_Ellipse.st_A, fCos(Cf_PiBy6));
		MATH_ScaleEqualVector(&st_Ellipse.st_B, fCos(Cf_PiBy6));
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		MATH_SubEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		MATH_SubEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);

		MATH_CopyVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &Offset);
		MATH_ScaleVector(&st_Ellipse.st_A, &MATH_gst_BaseVectorI, pst_P->f_GenParam[0]);
		MATH_ScaleVector(&st_Ellipse.st_B, &MATH_gst_BaseVectorJ, pst_P->f_GenParam[0]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		MATH_ScaleVector(&C, &MATH_gst_BaseVectorK, -pst_P->f_GenParam[0] * .5f);
		MATH_AddEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		MATH_ScaleEqualVector(&st_Ellipse.st_A, fCos(Cf_PiBy6));
		MATH_ScaleEqualVector(&st_Ellipse.st_B, fCos(Cf_PiBy6));
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		MATH_SubEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		MATH_SubEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);

		MATH_CopyVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &Offset);
		MATH_ScaleVector(&st_Ellipse.st_A, &MATH_gst_BaseVectorJ, pst_P->f_GenParam[0]);
		MATH_ScaleVector(&st_Ellipse.st_B, &MATH_gst_BaseVectorK, pst_P->f_GenParam[0]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		MATH_ScaleVector(&C, &MATH_gst_BaseVectorI, -pst_P->f_GenParam[0] * .5f);
		MATH_AddEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		MATH_ScaleEqualVector(&st_Ellipse.st_A, fCos(Cf_PiBy6));
		MATH_ScaleEqualVector(&st_Ellipse.st_B, fCos(Cf_PiBy6));
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		MATH_SubEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		MATH_SubEqualVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &C);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		break;
	case PAG_GenType_Box:
		st_LineEx[0].A = st_LineEx[1].A = &A;
		st_LineEx[0].B = &B;
		st_LineEx[1].B = st_LineEx[2].A = &C;
		st_LineEx[2].B = &D;
		st_LineEx[0].f_Width = st_LineEx[1].f_Width = st_LineEx[2].f_Width = 3;
		st_LineEx[0].ul_Color = st_LineEx[1].ul_Color = st_LineEx[2].ul_Color = 0xFFFF0000;
		st_LineEx[0].ul_Flags = st_LineEx[1].ul_Flags = st_LineEx[2].ul_Flags = 0xFFFF0000;
		MATH_ScaleVector(&A, &MATH_gst_BaseVectorI, pst_P->f_GenParam[0]);
		MATH_AddEqualVector(&A, &Offset);
		MATH_CopyVector(&B, &A);
		MATH_AddScaleVector(&A, &A, &MATH_gst_BaseVectorK, pst_P->f_GenParam[1]);
		MATH_AddScaleVector(&B, &B, &MATH_gst_BaseVectorK, -pst_P->f_GenParam[1]);
		MATH_AddScaleVector(&C, &A, &MATH_gst_BaseVectorJ, -pst_P->f_GenParam[2]);
		MATH_AddScaleVector(&D, &B, &MATH_gst_BaseVectorJ, -pst_P->f_GenParam[2]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[0]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[1]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[2]);
		MATH_SubEqualVector(&A, &Offset);
		MATH_NegEqualVector(&A);
		MATH_AddEqualVector(&A, &Offset);
		MATH_AddScaleVector(&C, &A, &MATH_gst_BaseVectorJ, -pst_P->f_GenParam[2]);
		st_LineEx[1].A = &B;
		st_LineEx[1].B = &D;
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[0]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[1]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[2]);
		MATH_SubEqualVector(&B, &Offset);
		MATH_NegEqualVector(&B);
		MATH_AddEqualVector(&B, &Offset);
		MATH_AddScaleVector(&D, &B, &MATH_gst_BaseVectorJ, -pst_P->f_GenParam[2]);
		st_LineEx[1].A = &A;
		st_LineEx[1].B = &C;
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[0]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[1]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[2]);
		MATH_SubEqualVector(&A, &Offset);
		MATH_NegEqualVector(&A);
		MATH_AddEqualVector(&A, &Offset);
		MATH_AddScaleVector(&C, &A, &MATH_gst_BaseVectorJ, -pst_P->f_GenParam[2]);
		st_LineEx[1].A = &B;
		st_LineEx[1].B = &D;
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[0]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[1]);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[2]);
		break;
	}

	switch(pst_P->c_SpeedType)
	{
	case PAG_SpeedType_1D:
		st_LineEx[0].A = &A;
		st_LineEx[0].B = &B;
		st_LineEx[0].f_Width = 3;
		st_LineEx[0].ul_Color = 0xFF00FF00;
		st_LineEx[0].ul_Flags = 0;
		MATH_ScaleVector(&A, &MATH_gst_BaseVectorJ, -pst_P->f_Speed0);
		MATH_ScaleVector(&B, &MATH_gst_BaseVectorJ, -pst_P->f_Speed1);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[0]);
		break;
	case PAG_SpeedType_2D:
		st_LineEx[0].f_Width = st_LineEx[1].f_Width = st_LineEx[2].f_Width = 3;
		st_LineEx[0].ul_Color = st_LineEx[1].ul_Color = st_LineEx[2].ul_Color = 0xFF00FF00;
		st_LineEx[0].ul_Flags = st_LineEx[1].ul_Flags = st_LineEx[2].ul_Flags = 0; 
		st_LineEx[0].A = st_LineEx[2].A = &A;
		st_LineEx[0].B = &B;
		st_LineEx[1].A = st_LineEx[2].B = &C;
		st_LineEx[1].B = &D;
		f_DAngle = (pst_P->f_Angle1 * 2) / 10;
		f_Angle = -pst_P->f_Angle1;

		for(i = 0; i < 11; i++, f_Angle += f_DAngle)
		{
			MATH_ScaleVector(&A, &MATH_gst_BaseVectorJ, -fOptCos(f_Angle));
			MATH_AddScaleVector(&A, &A, &MATH_gst_BaseVectorI, fOptSin(f_Angle));
			MATH_ScaleVector(&C, &A, pst_P->f_Speed1);
			MATH_ScaleEqualVector(&A, pst_P->f_Speed0);

			if(i == 0)
			{
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[2]);
			}
			else
			{
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[0]);
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[1]);
			}

			MATH_CopyVector(&B, &A);
			MATH_CopyVector(&D, &C);
		}

		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[2]);
		break;
	case PAG_SpeedType_3D:
		st_LineEx[0].f_Width = st_LineEx[1].f_Width = st_LineEx[2].f_Width = 3;
		st_LineEx[0].ul_Color = st_LineEx[1].ul_Color = st_LineEx[2].ul_Color = 0xFF00FF00;
		st_LineEx[0].ul_Flags = st_LineEx[1].ul_Flags = st_LineEx[2].ul_Flags = 0xFF00FF00;
		st_LineEx[0].A = st_LineEx[2].A = &A;
		st_LineEx[0].B = &B;
		st_LineEx[1].A = st_LineEx[2].B = &C;
		st_LineEx[1].B = &D;
		f_DAngle = (pst_P->f_Angle1 * 2) / 10;
		f_Angle = -pst_P->f_Angle1;

		for(i = 0; i < 11; i++, f_Angle += f_DAngle)
		{
			MATH_ScaleVector(&A, &MATH_gst_BaseVectorJ, -fOptCos(f_Angle));
			MATH_AddScaleVector(&A, &A, &MATH_gst_BaseVectorI, fOptSin(f_Angle));
			MATH_ScaleVector(&C, &A, pst_P->f_Speed1);
			MATH_ScaleEqualVector(&A, pst_P->f_Speed0);

			if(i == 0)
			{
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[2]);
			}
			else
			{
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[0]);
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[1]);
			}

			MATH_CopyVector(&B, &A);
			MATH_CopyVector(&D, &C);
		}

		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[2]);

		f_DAngle = (pst_P->f_Angle2 * 2) / 10;
		f_Angle = -pst_P->f_Angle2;

		for(i = 0; i < 11; i++, f_Angle += f_DAngle)
		{
			MATH_ScaleVector(&A, &MATH_gst_BaseVectorJ, -fOptCos(f_Angle));
			MATH_AddScaleVector(&A, &A, &MATH_gst_BaseVectorK, fOptSin(f_Angle));
			MATH_ScaleVector(&C, &A, pst_P->f_Speed1);
			MATH_ScaleEqualVector(&A, pst_P->f_Speed0);

			if(i == 0)
			{
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[2]);
			}
			else
			{
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[0]);
				GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[1]);
			}

			MATH_CopyVector(&B, &A);
			MATH_CopyVector(&D, &C);
		}

		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_LineEx[2]);
	}

    /*
    if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_Proportionnal)
    {
        GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
		GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
    }
    */
}

#else
#define PAG_Render_Data(a, b)
#endif
#ifdef GSP_PS2_BENCH
extern unsigned int NoPAG;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_Render(OBJ_tdst_GameObject *_pst_Node)
{
	PAG_RenderCommon(_pst_Node, (PAG_tdst_Struct *) _pst_Node->pst_Base->pst_Visu->pst_Object, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_RenderCommon(OBJ_tdst_GameObject *_pst_Node, PAG_tdst_Struct *pst_P, char _c_SortParticles )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				DM, DI;
	PAG_tdst_P			*P, *LastP;
	MATH_tdst_Matrix	*M;
	MATH_tdst_Vector	st_X, st_Y, *pst_X, *pst_Y, st_Temp;
    MATH_tdst_Vector	st_ScaleX, st_ScaleY;
	unsigned short		uw_SaveCurMatrix;
	ULONG				ul_Color;
	BOOL				b_SizeOk;
	float				f_Sin, f_Cos, sizex, sizey, blend;
    GEO_Vertex          *V;
    GEO_tdst_IndexedTriangle *T;
    GEO_tdst_IndexedSprite  *S;
    ULONG               *pul_Color;
    int                 i_Vertex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    static test = 1;

	if (pst_P->ul_Flags & PAG_Flags_DontSortParticles)
		_c_SortParticles = 0;
	else 
		_c_SortParticles = 1;

	//if ( pst_P->c_GenType != PAG_GenType_ObjectVertex )
	//	_c_SortParticles = 1;

    /*
    if (GetAsyncKeyState( VK_SHIFT ) < 0)
    {
        test = 1 - test;
        if (test)
            LINK_PrintStatusMsg( "Poly" );
        else
            LINK_PrintStatusMsg( "Sprites" );
        while( GetAsyncKeyState( VK_SHIFT ) < 0);
    }
    */

#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if (NoPAG) return;
#endif

#ifdef ACTIVE_EDITORS
    if ( !(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency) )
    {
	    if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible)
	    {
		    /*~~~~~~~~~~~~~~~~~~~*/
		    ULONG				C;
		    MATH_tdst_Matrix	*M;
		    int					i;
		    /*~~~~~~~~~~~~~~~~~~~*/

		    i = _pst_Node->ul_InvisibleObjectIndex;
		    if((i < 0) || (i > GEO_DebugObject_LastInvisible - GEO_DebugObject_Invisible)) i = 0;

		    if( GDI_WPShowStatusOn( GDI_gpst_CurDD, i) ) 
			{
			    C = (_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected) ? 1 : 0;
			    C = GEO_ul_DebugObject_GetColor(GEO_DebugObject_Invisible, C);
			    GEO_DebugObject_SetLight(C);
			    M = OBJ_pst_GetAbsoluteMatrix(_pst_Node);
			    DM = GDI_Cul_DM_All - GDI_Cul_DM_UseAmbient - GDI_Cul_DM_Fogged - GDI_Cul_DM_MaterialColor;
			    GEO_DebugObject_Draw(GDI_gpst_CurDD, GEO_DebugObject_Invisible + i, DM, _pst_Node->ul_EditorFlags, M);
		    }
	    }

	    if(_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected) 
            PAG_Render_DataCommon(_pst_Node, pst_P);
    }
#endif

	if(!(pst_P->ul_Flags & PAG_Flags_Active)) return;
    if(_pst_Node->uc_LOD_Vis == 0) return;
    
    if ( !_c_SortParticles && (pst_P->ul_Flags & PAG_Flags_Transparent) && (!(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency)) )
    {
        SOFT_AddCurrentObjectInZList(GDI_gpst_CurDD, _pst_Node);
        return;
    }

	/* generate new particles and update old one */
	if(pst_P->ul_RenderingCounter != GDI_gpst_CurDD->ul_RenderingCounter)
	{
		if((ENG_gb_EngineRunning) && !(pst_P->ul_Flags & PAG_Flags_Freeze))
		{
			PAG_Update(_pst_Node, pst_P, TIM_gf_dt);
			PAG_Generate(_pst_Node, pst_P, TIM_gf_dt);
		}

		pst_P->ul_RenderingCounter = GDI_gpst_CurDD->ul_RenderingCounter;
	}

    /* y'a t'il des particules à afficher ?? */
	if(!pst_P->l_NbP) return;

	DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
	DI = GDI_gpst_CurDD->ul_DisplayInfo;

    /* code pour afficher les particle generator avec les sprites triés */
    if (_c_SortParticles)
    {
	    GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_DontSortObject;
	    if (pst_P->ul_Flags & PAG_Flags_Transparent)
        	GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_ForceSortTriangle;
    }

	uw_SaveCurMatrix = GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix;
	//GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix = 0;
	//GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix = GDI_gpst_CurDD->st_MatrixStack.dst_Matrix;
    GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix = &GDI_gpst_CurDD->st_Camera.st_InverseMatrix;

	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);

	M = &GDI_gpst_CurDD->st_Camera.st_Matrix;
	if
	(
		(pst_P->f_SizeXMax == pst_P->f_SizeXMin)
	&&	(pst_P->f_SizeYMax == pst_P->f_SizeYMin)
	&&	!(pst_P->ul_Flags & (PAG_Flags_DecreaseDeath | PAG_Flags_IncreaseDeath))
	)
	{
		MATH_ScaleVector(&st_ScaleX, MATH_pst_GetXAxis(M), pst_P->f_SizeXMin);
		if(pst_P->ul_Flags & PAG_Flags_YEqualX)
			MATH_ScaleVector(&st_ScaleY, MATH_pst_GetYAxis(M), pst_P->f_SizeXMin);
		else
			MATH_ScaleVector(&st_ScaleY, MATH_pst_GetYAxis(M), pst_P->f_SizeYMin);
        MATH_CopyVector( &st_X, &st_ScaleX );
        MATH_CopyVector( &st_Y, &st_ScaleY );
		pst_X = &st_ScaleX;
		pst_Y = &st_ScaleY;
		b_SizeOk = 1;
	}
	else
	{
		pst_X = MATH_pst_GetXAxis(M);
		pst_Y = MATH_pst_GetYAxis(M);
		b_SizeOk = 0;
	}

    /* set material */
	if(!_pst_Node->pst_Base || !_pst_Node->pst_Base->pst_Visu) return;
    GFX_gpst_GO->pst_Base->pst_Visu->pst_Material = _pst_Node->pst_Base->pst_Visu->pst_Material;

    if (test)
    {
 #ifdef _XENON_RENDER
        GFX_gpst_Geo->b_Particles = TRUE;
#endif
        GFX_NeedGeom(4 * pst_P->l_NbP, 4, 2 * pst_P->l_NbP, 1);
        GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
        V = GFX_gpst_Geo->dst_Point;
        T = GFX_gpst_Geo->dst_Element->dst_Triangle;
        pul_Color = GFX_gpst_Geo->dul_PointColors + 1;

        /* set UV */
        GFX_gpst_Geo->dst_UV[0].fU = 0;
        GFX_gpst_Geo->dst_UV[0].fV = 0;
        GFX_gpst_Geo->dst_UV[1].fU = 1;
        GFX_gpst_Geo->dst_UV[1].fV = 0;
        GFX_gpst_Geo->dst_UV[2].fU = 1;
        GFX_gpst_Geo->dst_UV[2].fV = 1;
        GFX_gpst_Geo->dst_UV[3].fU = 0;
        GFX_gpst_Geo->dst_UV[3].fV = 1;

        P = pst_P->dst_P;
	    LastP = P + pst_P->l_NbP;
        for(; P < LastP; P++)
	    {
		    ul_Color = pst_P->ul_Color;
            sizex = P->f_SizeX;
            sizey = P->f_SizeY;

            if (P->ul_Flags & PAG_PFlags_Birth)
            {
                blend = 1.0f - (P->f_TimeLeft * P->f_TimeFactor);
			    if(pst_P->ul_Flags & PAG_Flags_AlphaBirth)
			    {
				    ul_Color = COLOR_ul_Mul(ul_Color, blend);
				    if(!(pst_P->ul_Flags & PAG_Flags_RGBEqualA))
				    {
					    ul_Color &= 0xFF000000;
					    ul_Color |= pst_P->ul_Color & 0xFFFFFF;
				    }
			    }

			    if(pst_P->ul_Flags & PAG_Flags_IncreaseBirth)
			    {
				    sizex *= blend;
				    sizey *= blend;
			    }
		    }
		    else if (P->ul_Flags & PAG_PFlags_Death )
		    {
                blend = P->f_TimeLeft * P->f_TimeFactor;
			    
                if(pst_P->ul_Flags & PAG_Flags_AlphaDeath)
			    {
				    ul_Color = COLOR_ul_Mul(ul_Color, blend);
				    if(!(pst_P->ul_Flags & PAG_Flags_RGBEqualA))
				    {
					    ul_Color &= 0xFF000000;
					    ul_Color |= pst_P->ul_Color & 0xFFFFFF;
				    }
			    }

			    if(pst_P->ul_Flags & PAG_Flags_DecreaseDeath)
			    {
				    sizex *= blend;
				    sizey *= blend;
			    }
			    else if(pst_P->ul_Flags & PAG_Flags_IncreaseDeath)
			    {
                    blend = (1 + (1 - blend) * pst_P->f_SizeDeathFactor);
                    sizex *= blend;
				    sizey *= blend;
			    }
		    }

		    if(!b_SizeOk)
		    {
			    MATH_ScaleVector(&st_X, pst_X, sizex);
			    MATH_ScaleVector(&st_Y, pst_Y, sizey);
		    }

		    if(P->ul_Rotation)
		    {
			    if(b_SizeOk)
			    {
				    MATH_CopyVector(&st_X, pst_X);
				    MATH_CopyVector(&st_Y, pst_Y);
			    }

			    f_Sin = MATH_gf_TableSin[P->ul_Rotation >> 22];
			    f_Cos = MATH_gf_TableSin[((P->ul_Rotation >> 22) + 256) % 1023];
			    MATH_ScaleVector(&st_Temp, &st_X, f_Cos);
			    MATH_AddScaleVector(&st_Temp, &st_Temp, &st_Y, f_Sin);
			    MATH_ScaleEqualVector(&st_Y, f_Cos);
			    MATH_AddScaleVector(&st_Y, &st_Y, &st_X, -f_Sin);
			    MATH_CopyVector(&st_X, &st_Temp);
		    }

            *pul_Color++ = ul_Color;
            *pul_Color++ = ul_Color;
            *pul_Color++ = ul_Color;
            *pul_Color++ = ul_Color;

            i_Vertex = V - GFX_gpst_Geo->dst_Point;
            T->auw_Index[0] = i_Vertex;
            T->auw_Index[1] = i_Vertex + 1;
            T->auw_Index[2] = i_Vertex + 2;
            T->auw_UV[0] = 0;
	        T->auw_UV[1] = 1;
	        T->auw_UV[2] = 2;
            T++;

            T->auw_Index[0] = i_Vertex + 2;
	        T->auw_Index[1] = i_Vertex + 3;
            T->auw_Index[2] = i_Vertex;
            T->auw_UV[0] = 2;
            T->auw_UV[1] = 3;
            T->auw_UV[2] = 0;
            T++;
		    
		    MATH_SubVector(VCast(V), &P->st_Pos, &st_X);
		    MATH_SubEqualVector(VCast(V), &st_Y);
            V++;
		    MATH_SubVector(VCast(V), &P->st_Pos, &st_X);
		    MATH_AddEqualVector(VCast(V), &st_Y);
            V++;
            MATH_AddVector(VCast(V), &P->st_Pos, &st_X);
		    MATH_AddEqualVector(VCast(V), &st_Y);
            V++;
            MATH_AddVector(VCast(V), &P->st_Pos, &st_X);
		    MATH_SubEqualVector(VCast(V), &st_Y);
            V++;
	    }
    }
    else
    {
        GFX_NeedGeomEx( pst_P->l_NbP, 4, 0, 0, 1);
        GFX_NeedGeomNbElemSprites( 1 );
        GFX_NeedGeomElemNbSprites( 0, pst_P->l_NbP );
        GFX_gpst_Geo->dst_SpritesElements->l_MaterialId = 0;
        GFX_gpst_Geo->dst_SpritesElements->fGlobalSize = 1.0f;
        GFX_gpst_Geo->dst_SpritesElements->fGlobalRatio = 1.0f;
        V = GFX_gpst_Geo->dst_Point;
        S = GFX_gpst_Geo->dst_SpritesElements->dst_Sprite;
        pul_Color = GFX_gpst_Geo->dul_PointColors + 1;

        /* set UV */
        GFX_gpst_Geo->dst_UV[0].fU = 0;
        GFX_gpst_Geo->dst_UV[0].fV = 0;
        GFX_gpst_Geo->dst_UV[1].fU = 1;
        GFX_gpst_Geo->dst_UV[1].fV = 0;
        GFX_gpst_Geo->dst_UV[2].fU = 1;
        GFX_gpst_Geo->dst_UV[2].fV = 1;
        GFX_gpst_Geo->dst_UV[3].fU = 0;
        GFX_gpst_Geo->dst_UV[3].fV = 1;

        P = pst_P->dst_P;
	    LastP = P + pst_P->l_NbP;
        for(; P < LastP; P++)
	    {
		    ul_Color = pst_P->ul_Color;
            sizex = P->f_SizeX;
            sizey = P->f_SizeY;

            if (P->ul_Flags & PAG_PFlags_Birth)
            {
                blend = 1.0f - (P->f_TimeLeft * P->f_TimeFactor);
			    if(pst_P->ul_Flags & PAG_Flags_AlphaBirth)
			    {
				    ul_Color = COLOR_ul_Mul(ul_Color, blend);
				    if(!(pst_P->ul_Flags & PAG_Flags_RGBEqualA))
				    {
					    ul_Color &= 0xFF000000;
					    ul_Color |= pst_P->ul_Color & 0xFFFFFF;
				    }
			    }

			    if(pst_P->ul_Flags & PAG_Flags_IncreaseBirth)
			    {
				    sizex *= blend;
				    sizey *= blend;
			    }
		    }
		    else if (P->ul_Flags & PAG_PFlags_Death )
		    {
                blend = P->f_TimeLeft * P->f_TimeFactor;
			    
                if(pst_P->ul_Flags & PAG_Flags_AlphaDeath)
			    {
				    ul_Color = COLOR_ul_Mul(ul_Color, blend);
				    if(!(pst_P->ul_Flags & PAG_Flags_RGBEqualA))
				    {
					    ul_Color &= 0xFF000000;
					    ul_Color |= pst_P->ul_Color & 0xFFFFFF;
				    }
			    }

			    if(pst_P->ul_Flags & PAG_Flags_DecreaseDeath)
			    {
				    sizex *= blend;
				    sizey *= blend;
			    }
			    else if(pst_P->ul_Flags & PAG_Flags_IncreaseDeath)
			    {
                    blend = (1 + (1 - blend) * pst_P->f_SizeDeathFactor);
                    sizex *= blend;
				    sizey *= blend;
			    }
		    }

		    if(!b_SizeOk)
		    {
			    MATH_ScaleVector(&st_X, pst_X, sizex);
			    MATH_ScaleVector(&st_Y, pst_Y, sizey);
		    }


            *pul_Color++ = ul_Color;
            *(float *) S = sizex;
            S->auw_Index = V - GFX_gpst_Geo->dst_Point;
            S++;
		    MATH_CopyVector( VCast(V), &P->st_Pos);
            V++;
	    }
    }

    M_GFX_CheckGeom();
    
    /* render */
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_Lighted | GDI_Cul_DM_UseAmbient);
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);

#ifdef _XENON_RENDER
    GFX_gpst_Geo->b_Particles = FALSE;
#endif

    /* restore data*/
	GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
	GDI_gpst_CurDD->ul_DisplayInfo = DI;
	GDI_gpst_CurDD->st_MatrixStack.uw_IndexOfCurrentMatrix = uw_SaveCurMatrix;
	GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix = GDI_gpst_CurDD->st_MatrixStack.dst_Matrix + uw_SaveCurMatrix;
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_Reinit(GRO_tdst_Struct *pst_Gro)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_P;
	/*~~~~~~~~~~~~~~~~~~~*/

	pst_P = (PAG_tdst_Struct *) pst_Gro;
	pst_P->l_NbP = 0;
    pst_P->w_NbGenerator = 1;
    pst_P->ast_Gen->p_GO = NULL;
    pst_P->ast_Gen->i_Id = 1;
	pst_P->ast_Gen->f_NbPerSecond = pst_P->ast_Gen->f_NbPerSecondInit;

	pst_P->pst_GO = NULL;
	pst_P->pst_GOMatrixRef = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Interface					*i;
	//GEO_tdst_ElementIndexedTriangles	*pst_Element;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Init light object interface */
	i = &GRO_gast_Interface[GRO_ParticleGenerator];
#if defined (JADEFUSION)
	i->pfnp_CreateFromBuffer = (void *(__cdecl *)(GRO_tdst_Struct *,char ** ,void *))PAG_p_CreateFromBuffer;
	i->pfn_Destroy = (void (__cdecl *)(void *))PAG_Free;
	i->pfnl_HasSomethingToRender = (LONG (__cdecl *)(void *,void *))PAG_l_HasSomethingToRender;
	i->pfn_Render = (void (__cdecl *)(void *))PAG_Render;
#else
	i->pfnp_CreateFromBuffer = PAG_p_CreateFromBuffer;
	i->pfn_Destroy = PAG_Free;
	i->pfnl_HasSomethingToRender = PAG_l_HasSomethingToRender;
	i->pfn_Render = PAG_Render;
#endif
	i->pfn_Reinit = PAG_Reinit;
#if defined (JADEFUSION)
    i->pfnp_Duplicate = (void *(__cdecl *)(void *,char *, char*, ULONG))PAG_p_Duplicate;
#else
	i->pfnp_Duplicate = PAG_p_Duplicate;
#endif
#ifdef ACTIVE_EDITORS
#if defined (JADEFUSION)
	i->pfnl_SaveInBuffer = (LONG (__cdecl *)(void *,void *))PAG_l_SaveInBuffer;
    i->pfnl_PushSpecialMatrix = (LONG (__cdecl *)(void *))GRO_PushSpecialMatrixForProportionnal;
#else
	i->pfnl_SaveInBuffer = PAG_l_SaveInBuffer;
    i->pfnl_PushSpecialMatrix = GRO_PushSpecialMatrixForProportionnal;
#endif
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_Close(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_Update(OBJ_tdst_GameObject *_pst_GO, PAG_tdst_Struct *_pst_P, float _f_Dt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define Cf_1o60 0.016667f
#define Cf_1o120 0.008333f

	int					i_NbP, n ,i;
	PAG_tdst_P			*P;
	MATH_tdst_Vector	st_Temp;
	float				t;
	short				w_Rotation;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_NbP = _pst_P->l_NbP;

	P = _pst_P->dst_P;

    _f_Dt += _pst_P->f_TimeLeft;
    n = (int) (_f_Dt * 60.0f);
    t = n * Cf_1o60;
    _pst_P->f_TimeLeft = _f_Dt - t;
    _f_Dt = t;
    
	while(i_NbP--)
	{
        t = _f_Dt;

PAG_Update_State:
        if (P->f_TimeLeft < _f_Dt)
        {
            t -= P->f_TimeLeft;
            if (P->ul_Flags & PAG_PFlags_Birth)
            {
                P->f_TimeLeft = fRand(_pst_P->f_TimeMin, _pst_P->f_TimeMax);
#ifdef JADEFUSION
                P->f_TimeFactor = (P->f_TimeLeft>0.0f) ? (1.0f / P->f_TimeLeft) : 0.0f;
#else
				P->f_TimeFactor = 1.0f / P->f_TimeLeft;
#endif
				P->ul_Flags = 0;
                goto PAG_Update_State;
            }
            else if (P->ul_Flags & PAG_PFlags_Death)
            {
                _pst_P->l_NbP--;
				if(i_NbP) 
                    L_memcpy(P, &_pst_P->dst_P[_pst_P->l_NbP], sizeof(PAG_tdst_P));
				continue;
            }
            else
            {
                P->f_TimeLeft = fRand(_pst_P->f_TimeDeathMin, _pst_P->f_TimeDeathMax);
#ifdef JADEFUSION
                P->f_TimeFactor = (P->f_TimeLeft>0.0f) ? (1.0f / P->f_TimeLeft) : 0.0f;
#else
				P->f_TimeFactor = 1.0f / P->f_TimeLeft;
#endif
				P->ul_Flags = PAG_PFlags_Death;
                goto PAG_Update_State;
            }
        }

        P->f_TimeLeft -= _f_Dt;
		P->f_TimeCur += _f_Dt;

		if(_pst_P->ul_Flags & PAG_Flags_UseRotation)
		{
			if(P->ul_Rotation & 0x8000)
			{
				w_Rotation = (short) (P->ul_Rotation >> 16);
				w_Rotation -= (short) (P->ul_Rotation & 0x7FFF);
                P->ul_Rotation &= 0x0000FFFF;
				P->ul_Rotation |= ((LONG) w_Rotation) << 16;
			}
			else
				P->ul_Rotation += (P->ul_Rotation & 0xFFFF) << 16;
		}

        /* Attractor */
        /*$F
        {
            MATH_tdst_Vector    V;
            int i;
            float f;

            MATH_CopyVector( &st_Temp, &_pst_P->st_Acc );
            for ( i = 0; i < _pst_P->i_NbAttractor; i++)
            {
                MATH_SubVector( &V, _pst_P->st_ACenter + i, &P->st_Pos );
                f = MATH_f_SqrVector( &V );
                if (f < _pst_P->f_ADist2[i]) 
                {
                    MATH_ScaleEqualVector( &P->st_Speed, _pst_P->f_AFriction[ i ] );
                    continue;
                }
                f = _pst_P->f_AStrength[ i ] / (f - _pst_P->f_ADist1[i] );
                MATH_AddScaleVector( &st_Temp, &st_Temp, &V, f );
            }
            MATH_ScaleEqualVector(&st_Temp, _f_Dt);
        }
        */

        i = n;
        while (i--)
        {
		    MATH_ScaleVector(&st_Temp, &_pst_P->st_Acc, Cf_1o60);
		    MATH_AddScaleVector(&P->st_Pos, &P->st_Pos, &st_Temp, Cf_1o120);
		    MATH_AddScaleVector(&P->st_Pos, &P->st_Pos, &P->st_Speed, Cf_1o60);
		    if(_pst_P->ul_Flags & PAG_Flags_AccSpeed)
			{
				MATH_AddEqualVector(&P->st_Speed, &st_Temp);
			}

		    if((_pst_P->ul_Flags & PAG_Flags_UseZMin) && (P->st_Pos.z - P->f_SizeY < _pst_P->f_ZMin))
		    {
				float				norm;
				float				dist;
				MATH_tdst_Vector	vv;

				dist = _pst_P->f_DistConstraint;
				MATH_SubVector(&vv, &P->st_Pos, &_pst_GO->pst_GlobalMatrix->T);
				vv.z = 0;
				norm = MATH_f_SqrNormVector(&vv);
				if(!dist || norm <= dist)
				{
					P->st_Pos.z = _pst_P->f_ZMin + P->f_SizeY;
					P->st_Speed.z = -P->st_Speed.z * _pst_P->f_ZMinStrength;
				}
		    }
		    else if((_pst_P->ul_Flags & PAG_Flags_UseZMax) && (P->st_Pos.z + P->f_SizeY > _pst_P->f_ZMax))
		    {
			    P->st_Pos.z = _pst_P->f_ZMax - P->f_SizeY;
			    P->st_Speed.z = -P->st_Speed.z * _pst_P->f_ZMaxStrength;
		    }
		    else
			    MATH_AddEqualVector(&P->st_Speed, &st_Temp);

    		if(_pst_P->ul_Flags & PAG_Flags_UseFriction) 
                MATH_ScaleEqualVector(&P->st_Speed, _pst_P->f_Friction);
        }

		/* Changement trajectoire */
		if(_pst_P->f_SinXFactor) P->st_Pos.x += fOptSin(P->f_TimeCur * _pst_P->f_SinXFactor) * _f_Dt;
		if(_pst_P->f_SinYFactor) P->st_Pos.y += fOptSin(P->f_TimeCur * _pst_P->f_SinYFactor) * _f_Dt;

		P++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_Generate(OBJ_tdst_GameObject *_pst_GO, PAG_tdst_Struct *_pst_P, float _f_Dt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_NbGen, i_NbP, i_Pt;
	PAG_tdst_P			*P;
	MATH_tdst_Matrix	*M, *MSpeed;
	MATH_tdst_Vector	*V;
	float				f_Radius, f_Angle, f_Angle2, f_Scale;
	PAG_tdst_Generator	*pst_Gen;
    GEO_tdst_Object     *pst_Obj;
    GEO_Vertex          *pst_VGen;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_NbGen = _pst_P->w_NbGenerator;
	pst_Gen = _pst_P->ast_Gen;

    pst_Obj = (GEO_tdst_Object*)OBJ_p_GetGro( _pst_P->pst_GO );
    if (pst_Obj && pst_Obj->st_Id.i->ul_Type != GRO_Geometric )
        pst_Obj = NULL;

	if ( _pst_P->c_GenType == PAG_GenType_ObjectVertex )
    {
        MDF_tdst_Modifier *pst_Mod;
        GEO_tdst_ModifierStoreTransformedPoint *pst_ModData;

        if ( !pst_Obj ) return;

        pst_VGen = pst_Obj->dst_Point;
        pst_Mod = MDF_pst_GetByType( _pst_P->pst_GO, MDF_C_Modifier_StoreTransformedPoints );
        if (pst_Mod)
        {
            pst_ModData = (GEO_tdst_ModifierStoreTransformedPoint *) pst_Mod->p_Data;
            if (pst_ModData->i_PointsAreValid )
                pst_VGen = pst_ModData->dst_Points;
        }

		if (_pst_P->pst_GOMatrixRef)
			M = OBJ_pst_GetAbsoluteMatrix( _pst_P->pst_GOMatrixRef );
		else
			M = OBJ_pst_GetAbsoluteMatrix( _pst_P->pst_GO );
    }



	while(i_NbGen--)
	{
        if (!pst_Gen->f_NbPerSecond) 
        {
            pst_Gen++;
            continue;
        }

		pst_Gen->f_GenTime += _f_Dt;
		i_NbP = (int) (pst_Gen->f_NbPerSecond * pst_Gen->f_GenTime);
		pst_Gen->f_GenTime -= i_NbP / pst_Gen->f_NbPerSecond;

		if(i_NbP + _pst_P->l_NbP >= _pst_P->l_NbMaxP) i_NbP = _pst_P->l_NbMaxP - _pst_P->l_NbP - 1;
		if(i_NbP <= 0) 
        {
            pst_Gen++;
            continue;
        }

		if(pst_Gen->p_GO)
			MSpeed = OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject *) pst_Gen->p_GO);
		else
			MSpeed = OBJ_pst_GetAbsoluteMatrix(_pst_GO);
		if ( _pst_P->c_GenType != PAG_GenType_ObjectVertex )
			M = MSpeed;

		while(i_NbP--)
		{
			P = &_pst_P->dst_P[_pst_P->l_NbP++];
            P->f_TimeLeft = 0;
			P->f_TimeCur = 0;

            if(_pst_P->f_TimeBirthMax)
            {
                P->f_TimeLeft = fRand(_pst_P->f_TimeBirthMin, _pst_P->f_TimeBirthMax);
                P->ul_Flags = PAG_PFlags_Birth;
            }

            if (P->f_TimeLeft == 0)
            {
                P->f_TimeLeft = fRand(_pst_P->f_TimeMin, _pst_P->f_TimeMax);
                P->ul_Flags = 0;

                if (P->f_TimeLeft == 0)
                {
                    if (_pst_P->f_TimeDeathMax)
                    {
                        P->f_TimeLeft = fRand(_pst_P->f_TimeDeathMin, _pst_P->f_TimeDeathMax);
                        P->ul_Flags = PAG_PFlags_Death;
                    }
                }
            }
#ifdef JADEFUSION
			P->f_TimeFactor = (P->f_TimeLeft>0.0f) ? (1.0f / P->f_TimeLeft) : 0.0f;
#else
			P->f_TimeFactor = 1.0f / P->f_TimeLeft;
#endif

			P->f_SizeX = fRand(_pst_P->f_SizeXMin, _pst_P->f_SizeXMax);
			if(_pst_P->ul_Flags & PAG_Flags_YEqualX)
				P->f_SizeY = P->f_SizeX;
			else
				P->f_SizeY = fRand(_pst_P->f_SizeYMin, _pst_P->f_SizeYMax);

			switch(_pst_P->c_GenType)
			{
			case PAG_GenType_Point:
				MATH_CopyVector(&P->st_Pos, &M->T);
				break;
			case PAG_GenType_Rectangle:
				MATH_AddScaleVector
				(
					&P->st_Pos,
					&M->T,
					(MATH_tdst_Vector *) &M->Ix,
					fRand(-1, 1) * _pst_P->f_GenParam[0]
				);
				MATH_AddScaleVector
				(
					&P->st_Pos,
					&P->st_Pos,
					(MATH_tdst_Vector *) &M->Kx,
					fRand(-1, 1) * _pst_P->f_GenParam[1]
				);
				break;
			case PAG_GenType_Circle:
				f_Angle = fRand(0, 2 * Cf_2Pi);
				f_Radius = fRand(0, _pst_P->f_GenParam[0]);
				MATH_AddScaleVector(&P->st_Pos, &M->T, (MATH_tdst_Vector *) &M->Ix, f_Radius * fOptCos(f_Angle));
				MATH_AddScaleVector(&P->st_Pos, &P->st_Pos, (MATH_tdst_Vector *) &M->Kx, f_Radius * fOptSin(f_Angle));
				break;
			case PAG_GenType_Cylindre:
				f_Angle = fRand(0, 2 * Cf_2Pi);
				f_Radius = fRand(0, _pst_P->f_GenParam[0]);
				MATH_AddScaleVector(&P->st_Pos, &M->T, (MATH_tdst_Vector *) &M->Ix, f_Radius * fOptCos(f_Angle));
				MATH_AddScaleVector(&P->st_Pos, &P->st_Pos, (MATH_tdst_Vector *) &M->Kx, f_Radius * fOptSin(f_Angle));
				MATH_AddScaleVector
				(
					&P->st_Pos,
					&P->st_Pos,
					(MATH_tdst_Vector *) &M->Jx,
					-fRand(0, 1) * _pst_P->f_GenParam[1]
				);
				break;
			case PAG_GenType_Sphere:
				f_Angle = fRand(0, Cf_2Pi);
				f_Angle2 = fRand(-Cf_PiBy2, Cf_PiBy2);
				f_Radius = fRand(0, _pst_P->f_GenParam[0]);
				MATH_AddScaleVector
				(
					&P->st_Pos,
					&M->T,
					(MATH_tdst_Vector *) &M->Ix,
					f_Radius * fOptCos(f_Angle) * fOptCos(f_Angle2)
				);
				MATH_AddScaleVector
				(
					&P->st_Pos,
					&P->st_Pos,
					(MATH_tdst_Vector *) &M->Jx,
					f_Radius * fOptSin(f_Angle) * fOptCos(f_Angle2)
				);
				MATH_AddScaleVector(&P->st_Pos, &P->st_Pos, (MATH_tdst_Vector *) &M->Kx, f_Radius * fOptSin(f_Angle2));
				break;
			case PAG_GenType_Box:
				MATH_AddScaleVector
				(
					&P->st_Pos,
					&M->T,
					(MATH_tdst_Vector *) &M->Ix,
					fRand(-1, 1) * _pst_P->f_GenParam[0]
				);
				MATH_AddScaleVector
				(
					&P->st_Pos,
					&P->st_Pos,
					(MATH_tdst_Vector *) &M->Kx,
					fRand(-1, 1) * _pst_P->f_GenParam[1]
				);
				MATH_AddScaleVector
				(
					&P->st_Pos,
					&P->st_Pos,
					(MATH_tdst_Vector *) &M->Jx,
					-fRand(0, _pst_P->f_GenParam[2])
				);
				break;
            case PAG_GenType_ObjectVertex:
                i_Pt = (rand() * pst_Obj->l_NbPoints) / RAND_MAX;
                V = VCast( &pst_VGen[i_Pt] );
				if ((V->z < _pst_P->f_ZMin) || (V->z > _pst_P->f_ZMax) )
				{
					_pst_P->l_NbP--;
					continue;
				}
				MATH_TransformVertex( &P->st_Pos, M, VCast( &pst_VGen[i_Pt] ) );
                break;
			}

			MATH_AddScaleVector(&P->st_Pos, &P->st_Pos, (MATH_tdst_Vector *) &M->Jx, -_pst_P->f_GenOffset);

			switch(_pst_P->c_SpeedType)
			{
			case PAG_SpeedType_1D:
				MATH_ScaleVector(&P->st_Speed, MATH_pst_GetYAxis(MSpeed), -fRand(_pst_P->f_Speed0, _pst_P->f_Speed1));
				break;
			case PAG_SpeedType_2D:
				f_Angle = fRand(-_pst_P->f_Angle1, _pst_P->f_Angle1);
				f_Scale = fRand(_pst_P->f_Speed0, _pst_P->f_Speed1);
				MATH_ScaleVector(&P->st_Speed, MATH_pst_GetYAxis(MSpeed), -f_Scale * fOptCos(f_Angle));
				MATH_AddScaleVector(&P->st_Speed, &P->st_Speed, MATH_pst_GetXAxis(MSpeed), f_Scale * fOptSin(f_Angle));
				break;
			case PAG_SpeedType_3D:
				f_Angle = fRand(-_pst_P->f_Angle1, _pst_P->f_Angle1);
				f_Angle2 = fRand(-_pst_P->f_Angle1, _pst_P->f_Angle2);
				f_Scale = fRand(_pst_P->f_Speed0, _pst_P->f_Speed1);
				MATH_ScaleVector(&P->st_Speed, MATH_pst_GetYAxis(MSpeed), -f_Scale * fOptCos(f_Angle) * fOptCos(f_Angle2));
				MATH_AddScaleVector
				(
					&P->st_Speed,
					&P->st_Speed,
					MATH_pst_GetXAxis(MSpeed),
					f_Scale * fOptSin(f_Angle) * fOptCos(f_Angle2)
				);
				MATH_AddScaleVector(&P->st_Speed, &P->st_Speed, MATH_pst_GetZAxis(MSpeed), f_Scale * fOptSin(f_Angle2));
				break;
			case PAG_SpeedType_FromCenter:
				MATH_SubVector(&P->st_Speed, &P->st_Pos, &M->T);
				f_Scale = MATH_f_NormVector(&P->st_Speed);
				if(f_Scale) f_Scale = fRand(_pst_P->f_Speed0, _pst_P->f_Speed1) / f_Scale;
				MATH_ScaleEqualVector(&P->st_Speed, f_Scale);
				break;
            case PAG_SpeedType_ObjectNormal:
				GEO_UseNormals(pst_Obj);
                MATH_ScaleVector( &P->st_Speed, pst_Obj->dst_PointNormal + i_Pt, -fRand(_pst_P->f_Speed0, _pst_P->f_Speed1) );
                break;
			case PAG_SpeedType_Target:
				MATH_SubVector(&P->st_Speed, (MATH_tdst_Vector *) &_pst_P->f_RotationMin, &P->st_Pos);
				f_Scale = MATH_f_NormVector(&P->st_Speed);
				if(f_Scale) f_Scale = fRand(_pst_P->f_Speed0, _pst_P->f_Speed1) / f_Scale;
				MATH_ScaleEqualVector(&P->st_Speed, f_Scale);
                break;
			}

			if(_pst_P->ul_Flags & PAG_Flags_UseRotation)
			{
				f_Angle = fRand(_pst_P->f_RotationMin, _pst_P->f_RotationMax);
				MATH_f_FloatModulo(f_Angle, Cf_2Pi);
				f_Angle *= 1024.0f / Cf_2Pi;
				P->ul_Rotation = ((ULONG) f_Angle) << 22;

				f_Angle = fRand(_pst_P->f_RotationSpeedMin, _pst_P->f_RotationSpeedMax);
				f_Angle *= 65536.0f / Cf_2Pi;
				if(f_Angle < 0)
				{
					P->ul_Rotation |= 0x8000;
					f_Angle = -f_Angle;
				}

				P->ul_Rotation |= ((ULONG) f_Angle) & 0x7FFF;
			}
			else
				P->ul_Rotation = 0;
		}

		pst_Gen++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
PAG_tdst_P *PAG_AddOneParticle( OBJ_tdst_GameObject *_pst_GO )
{
    PAG_tdst_Struct		*pst_P;

#ifdef JADEFUSION
    if (!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return NULL;
#else
    if (!_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu) return NULL;
#endif
	pst_P = (PAG_tdst_Struct *) _pst_GO->pst_Base->pst_Visu->pst_Object;
    if ( pst_P->st_Id.i->ul_Type != GRO_ParticleGenerator) return NULL;
    if ( pst_P->l_NbP == pst_P->l_NbMaxP) return NULL;

    return pst_P->dst_P + pst_P->l_NbP++;
}

/*$4
 ***********************************************************************************************************************
    function for IA access
 ***********************************************************************************************************************
 */

static int	PAG_sgi_GenId = 2;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
PAG_tdst_Struct *PAG_pst_GetGameObjectParticleGenerator(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    PAG_tdst_Struct *pst_PAG;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
    {
        pst_PAG = (PAG_tdst_Struct *) _pst_GO->pst_Base->pst_Visu->pst_Object;
        if(pst_PAG)
        {
            if(pst_PAG->st_Id.i->ul_Type == GRO_ParticleGenerator)
                return pst_PAG;
        }
    }

	/* Modifier ? */
	if ((_pst_GO->pst_Extended ) && (_pst_GO->pst_Extended->pst_Modifiers))
	{
		MDF_tdst_Modifier *p_Mod;
		p_Mod = _pst_GO->pst_Extended->pst_Modifiers;
		while (p_Mod)
		{
			if (p_Mod ->i ->ul_Type == MDF_C_Modifier_PAG)
			{
				return((MPAG_tdst_Modifier *) p_Mod->p_Data)->pst_P;
			}
			p_Mod = p_Mod->pst_Next;
		}
	}

    return NULL;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int PAG_i_AddGenerator(PAG_tdst_Struct *_pst_P, void *_p_GO, float _f_NbPerSecond)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Generator	*pst_Gen;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_P->w_NbGenerator == 16) return -1;

	pst_Gen = &_pst_P->ast_Gen[_pst_P->w_NbGenerator++];
	pst_Gen->i_Id = PAG_sgi_GenId++;
	if(PAG_sgi_GenId & 0x8000000) PAG_sgi_GenId = 2;
	pst_Gen->p_GO = _p_GO;
	pst_Gen->f_NbPerSecond = _f_NbPerSecond;
	pst_Gen->f_GenTime = 0;
	return pst_Gen->i_Id;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_DelGenerator(PAG_tdst_Struct *_pst_P, int _i_Id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Generator	*pst_Gen;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Gen = _pst_P->ast_Gen;
	for(i = 0; i < _pst_P->w_NbGenerator; i++, pst_Gen++)
	{
		if(pst_Gen->i_Id == _i_Id)
		{
			if(i < _pst_P->w_NbGenerator - 1)
				L_memcpy(pst_Gen, &_pst_P->ast_Gen[_pst_P->w_NbGenerator - 1], sizeof(PAG_tdst_Generator));
			_pst_P->w_NbGenerator--;
			return;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PAG_SetGeneratorFrequency(PAG_tdst_Struct *_pst_P, int _i_Id, float _f_NbPerSecond)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Generator	*pst_Gen;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Gen = _pst_P->ast_Gen;
	for(i = 0; i < _pst_P->w_NbGenerator; i++, pst_Gen++)
	{
		if(pst_Gen->i_Id == _i_Id)
		{
			pst_Gen->f_NbPerSecond = _f_NbPerSecond;
			return;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float PAG_GetGeneratorFrequency(PAG_tdst_Struct *_pst_P, int _i_Id )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Generator	*pst_Gen;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Gen = _pst_P->ast_Gen;
	for(i = 0; i < _pst_P->w_NbGenerator; i++, pst_Gen++)
	{
		if(pst_Gen->i_Id == _i_Id)
		{
			return pst_Gen->f_NbPerSecond;
		}
	}
	return 0;
}
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
