/*$T AIfunctions_save.c GC 1.138 04/08/05 14:14:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/Functions/AIfunctions_savephotos.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGio.h"
#include <time.h>
#include "INOut/INOsaving.h"

#ifdef _GAMECUBE
#include "SDK/Sources/GameCube/GC_arammng.h"
#include "SDK/Sources/GameCube/GC_aramheap.h"
#endif
extern SCR_tt_SFDyn AI_gst_SFDyn[SFDYN_MAX];

#define MAX_CODE 25

char g_SAV_InternetCode[MAX_CODE + 10] = {0};

/*$4
 ***********************************************************************************************************************
    save managements struct
 ***********************************************************************************************************************
 */
char* AI_EvalFunc_SAVEGetInternetCode_C(int in_iSlot);

#ifdef _DEBUG
BOOL SAV_ConvertCode(char* in_pCode,uint* in_iCode);
BOOL SAV_ValidateCode( uint* io_iCode );
BOOL SAV_UnitTest_ValidateCodes(int in_iSlot);
#endif // #ifdef _DEBUG

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SAVEValidateAl_C(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_VarDes	*pst_VarDes;
	ULONG			i, ul_Size, ul_Act;
	int				i_Offset;
	ULONG			ul_SizeArr, i_NumElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg( "SAVE :         CP <- Cur (AL)" );

	pst_VarDes = WOR_gst_Universe.pst_AI->pst_Model->pst_VarDes;
	ul_Size = 0;
	for(i = 0; i < pst_VarDes->ul_NbVars; i++)
	{
		ul_Act = AI_gast_Types[pst_VarDes->pst_VarsInfos[i].w_Type].w_Size;
		i_Offset = pst_VarDes->pst_VarsInfos[i].i_Offset;
		i_NumElem = pst_VarDes->pst_VarsInfos[i].i_NumElem;
		ul_SizeArr = (ULONG) i_NumElem >> 30;

		if(pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_SaveAl)
		{
			if(ul_SizeArr)
			{
				L_memcpy
				(
					INO_gst_SavManager.ac_CurrSlotBuffer + ul_Size,
					WOR_gst_Universe.pst_AI->pc_VarsBuffer + i_Offset,
					ul_SizeArr * 4
				);
				ul_Size += ul_SizeArr * 4;
			}

			L_memcpy
			(
				INO_gst_SavManager.ac_CurrSlotBuffer + ul_Size,
				WOR_gst_Universe.pst_AI->pc_VarsBuffer + i_Offset + (ul_SizeArr * 4),
				(i_NumElem & 0x3FFFFFFF) * ul_Act
			);

			ul_Size += (i_NumElem & 0x3FFFFFFF) * ul_Act;
		}
		else if(pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Save)
		{
			if(ul_SizeArr) ul_Size += ul_SizeArr * 4;
			ul_Size += (i_NumElem & 0x3FFFFFFF) * ul_Act;
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVEValidateAl(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_SAVEValidateAl_C();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SAVEValidate_C(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_VarDes	*pst_VarDes;
	ULONG			i, ul_Size, ul_Act;
	int				i_Offset;
	ULONG			ul_SizeArr, i_NumElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg( "SAVE :         CP <- Cur" );

	pst_VarDes = WOR_gst_Universe.pst_AI->pst_Model->pst_VarDes;
	ul_Size = 0;
	for(i = 0; i < pst_VarDes->ul_NbVars; i++)
	{
		ul_Act = AI_gast_Types[pst_VarDes->pst_VarsInfos[i].w_Type].w_Size;
		i_Offset = pst_VarDes->pst_VarsInfos[i].i_Offset;
		i_NumElem = pst_VarDes->pst_VarsInfos[i].i_NumElem;
		ul_SizeArr = (ULONG) i_NumElem >> 30;

		if(pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Save)
		{
			if(ul_SizeArr)
			{
				L_memcpy
				(
					INO_gst_SavManager.ac_CurrSlotBuffer + ul_Size,
					WOR_gst_Universe.pst_AI->pc_VarsBuffer + i_Offset,
					ul_SizeArr * 4
				);
				ul_Size += ul_SizeArr * 4;
			}

			L_memcpy
			(
				INO_gst_SavManager.ac_CurrSlotBuffer + ul_Size,
				WOR_gst_Universe.pst_AI->pc_VarsBuffer + i_Offset + (ul_SizeArr * 4),
				(i_NumElem & 0x3FFFFFFF) * ul_Act
			);

			ul_Size += (i_NumElem & 0x3FFFFFFF) * ul_Act;
		}
	}

	/* SF dyn */
	L_memcpy
	(
		INO_gst_SavManager.ac_CurrSlotBuffer + INO_Cte_SavUniverseMaxSize,
		&AI_gst_SFDyn,
		(SFDYN_MAX * sizeof(SCR_tt_SFDyn))
	);
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVEValidate(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_SAVEValidate_C();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SAVEUnvalidate_C(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_VarDes	*pst_VarDes;
	ULONG			i, ul_Size, ul_Act;
	int				i_Offset;
	ULONG			ul_SizeArr, i_NumElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg( "SAVE :         CP -> Cur" );

	pst_VarDes = WOR_gst_Universe.pst_AI->pst_Model->pst_VarDes;
	ul_Size = 0;
	for(i = 0; i < pst_VarDes->ul_NbVars; i++)
	{
		ul_Act = AI_gast_Types[pst_VarDes->pst_VarsInfos[i].w_Type].w_Size;
		i_Offset = pst_VarDes->pst_VarsInfos[i].i_Offset;
		i_NumElem = pst_VarDes->pst_VarsInfos[i].i_NumElem;
		ul_SizeArr = (ULONG) i_NumElem >> 30;

		if(pst_VarDes->pst_VarsInfos[i].w_Flags & AI_CVF_Save)
		{
			if(ul_SizeArr)
			{
				L_memcpy
				(
					WOR_gst_Universe.pst_AI->pc_VarsBuffer + i_Offset,
					INO_gst_SavManager.ac_CurrSlotBuffer + ul_Size,
					ul_SizeArr * 4
				);
				ul_Size += ul_SizeArr * 4;
			}

			L_memcpy
			(
				WOR_gst_Universe.pst_AI->pc_VarsBuffer + i_Offset + (ul_SizeArr * 4),
				INO_gst_SavManager.ac_CurrSlotBuffer + ul_Size,
				(i_NumElem & 0x3FFFFFFF) * ul_Act
			);

			ul_Size += (i_NumElem & 0x3FFFFFFF) * ul_Act;
		}
	}

	/* SF dyn */
	L_memcpy
	(
		&AI_gst_SFDyn,
		INO_gst_SavManager.ac_CurrSlotBuffer + INO_Cte_SavUniverseMaxSize,
		(SFDYN_MAX * sizeof(SCR_tt_SFDyn))
	);
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVEUnvalidate(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_SAVEUnvalidate_C();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SAVESaveSlotExt_C(int _i_Slot, int _i_Map, int _i_WP, int _i_Time)
{
	/* TODO a virer */
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVESaveSlotExt(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int i_Slot, i_Map, i_WP, i_Time;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Time = AI_PopInt();
	i_WP = AI_PopInt();
	i_Map = AI_PopInt();
	i_Slot = AI_PopInt();
	AI_EvalFunc_SAVESaveSlotExt_C(i_Slot, i_Map, i_WP, i_Time);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SAVELoadSlot_C(int _i_Slot)
{
    //TODO a virer
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVELoadSlot(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	int i_Slot;
	/*~~~~~~~*/

	i_Slot = AI_PopInt();
	AI_EvalFunc_SAVELoadSlot_C(i_Slot);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SAVERequest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~*/
	int i_Rq, i_Val;
	/*~~~~~~~~~~~~*/

	i_Val = AI_PopInt();
	i_Rq = AI_PopInt();
	AI_PushInt(INO_i_SavRequest(i_Rq, i_Val));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SAVEDbgRequest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~*/
	int i_Rq, i_Val;
	/*~~~~~~~~~~~~*/

	i_Val = AI_PopInt();
	i_Rq = AI_PopInt();
	AI_PushInt(INO_i_SavDbgRequest(i_Rq, i_Val));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

#ifdef PSX2_TARGET 
int i_PS2Time = 0;
#endif
#ifdef _XBOX 
int i_XboxTime = 0;
#endif

void AI_EvalFunc_SAVEHeaderSet_C
(
	int		_i_Slot,
	int	    _i_Time,
	int     _ul_GameTime,
	int		_i_Map,
	int		_i_WP,
	int		_i_Progress
)
{
	char		sz_SaveName[ 32 ];
	L_time_t    x_Time;
	int			i_Time;
	int			i_ID, i_Tick;
	int			i_CreateTime, i_GameTime;
	int			i_Data[ 8 ];

    if(_i_Slot >= INO_Cte_SavSlotNbMax) return; 
    if(_i_Slot < 0) return; 

	// cas spéciaux pour sauvegarde paramètres servant à générer code internet
	// param JACK
	if (_i_Time == -1)
	{
		INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Death = _ul_GameTime;
		INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Bullet = _i_Map;
		INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Kill = _i_WP;
		INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Tick = TIM_ul_GetLowPartTimerInternalCounter() & 0x3FF;
		return;
	}
	// param KONG
	else if (_i_Time == -2)
	{
		INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Death = _ul_GameTime;
		INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Time = _i_Map;
		INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Kill = _i_WP;
		return;
	}
	// other param
	else if (_i_Time == -3)
	{
		if ( INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Progress != _i_Progress )
		{
			INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Progress = _i_Progress;
			INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Tick = TIM_ul_GetLowPartTimerInternalCounter() & 0x3FF;
		}
		return;
	}

	// keep name
	L_memcpy( sz_SaveName, INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].asz_Name, 32 );
	// keep creation time
	i_CreateTime = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Time;
	// keep ID
	i_ID = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_ID;
	i_Tick = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Tick;
	// keep other data
	i_Data[ 0 ] = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Death;
	i_Data[ 1 ] = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Bullet;
	i_Data[ 2 ] = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Kill; 
	i_Data[ 3 ] = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Death;
	i_Data[ 4 ] = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Time;
	i_Data[ 5 ] = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Kill; 

	// clear structure
    L_memset(&INO_gst_SavManager.ast_SlotDesc[_i_Slot], 0, sizeof(INO_tdst_SavSlotDesc));
	// restore name
	L_memcpy( INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].asz_Name, sz_SaveName, 32 );
	// restore ID
	INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_ID = i_ID;
	INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Tick = i_Tick;
	// restore data
	INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Death	= i_Data[ 0 ]; 
	INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Bullet= i_Data[ 1 ]; 
	INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Kill	= i_Data[ 2 ]; 
	INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Death	= i_Data[ 3 ]; 
	INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Time	= i_Data[ 4 ]; 
	INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Kill	= i_Data[ 5 ];

	// get save time
#ifdef PSX2_TARGET
	i_Time = i_PS2Time++;
#elif defined(_GAMECUBE)
	i_Time = (int) (OSGetTime() / (OS_TIMER_CLOCK * 16)); // Toutes les 16 secondes, i_Time augmente
#elif defined(_XBOX)
	i_Time = i_XboxTime++;
#else
	L_time(&x_Time);
	i_Time = (int) x_Time;
#endif

	switch ( _i_Time )
	{
		/* effacement du slot */
		case 0: i_CreateTime = 0; i_GameTime = 0; break;
		/* sauvegarde du slot */
		case 1: if (!i_CreateTime)
					i_CreateTime = 1; 
				i_GameTime = i_Time ? i_Time : 1;
				break;
		/* création du slot */
		case 2: 
				i_CreateTime = i_GameTime = i_Time ? i_Time : 1; 
				srand( (unsigned) i_Time );
				INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_ID = rand() & 0x3FFF;
				INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Death	= 0; 
				INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Bullet= 0; 
				INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Kill	= 0; 
				INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Death	= 0; 
				INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Time	= 0; 
				INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Kill	= 0;
				break;
	}

	INO_gst_SavManager.ast_SlotDesc[_i_Slot].i_Time = i_CreateTime;
	INO_gst_SavManager.ast_SlotDesc[_i_Slot].ul_GameTime = i_GameTime;
	INO_gst_SavManager.ast_SlotDesc[_i_Slot].i_Slot = _i_Slot;
	INO_gst_SavManager.ast_SlotDesc[_i_Slot].i_Map = _i_Map;
	INO_gst_SavManager.ast_SlotDesc[_i_Slot].i_WP = _i_WP;
	INO_gst_SavManager.ast_SlotDesc[_i_Slot].i_Progress = _i_Progress;
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVEHeaderSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int			i_Time;
	ULONG		ul_GameTime;
	int			i_Profile;
	int			i_Map;
	int			i_WP;
	int			i_DeathNumber;
	/*~~~~~~~~~~~~~~~~~~*/

	i_DeathNumber = AI_PopInt();
	i_WP = AI_PopInt();
	i_Map = AI_PopInt();
	ul_GameTime = AI_PopInt();
	i_Time = AI_PopInt(); 
	i_Profile = AI_PopInt();

	AI_EvalFunc_SAVEHeaderSet_C( i_Profile, i_Time, ul_GameTime, i_Map, i_WP, i_DeathNumber);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SAVEHeaderSetNameChar_C
(
	int		_i_Slot,
	int	    _i_Char,
	int		_i_Val
)
{
    if(_i_Slot >= INO_Cte_SavSlotNbMax) return; 
    if(_i_Slot < 0) return; 
    if (_i_Char <0) return;
    if (_i_Char >= 31) return;
    
    INO_gst_SavManager.ast_SlotDesc[_i_Slot].asz_Name[_i_Char] = _i_Val & 0xFF;
    INO_gst_SavManager.ast_SlotDesc[_i_Slot].asz_Name[31] = 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVEHeaderSetNameChar(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int		i_Char;
	int		i_Val;
	/*~~~~~~~~~~~~~~~~~~*/

	i_Val = AI_PopInt();
	i_Char = AI_PopInt();
	
	AI_EvalFunc_SAVEHeaderSetNameChar_C(AI_PopInt(), i_Char, i_Val);
	return ++_pst_Node;
}






/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SAVEHeaderGet_C
(
	int		_i_Slot,
	int	    *_pi_Time,
	int	    *_pul_GameTime,
	int		*_pi_Map,
	int		*_pi_WP,
	int		*_pi_Progress
)
{
    if(_i_Slot >= INO_Cte_SavSlotNbMax) return; 
    if(_i_Slot < 0) return; 

	if (*_pi_Time == -1)
	{
		*_pul_GameTime = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Death;
		*_pi_Map = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Bullet;
		*_pi_WP = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Jack_Kill;
		return;
	}
	
	if (*_pi_Time == -2)
	{
		*_pul_GameTime = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Death ;
		*_pi_Map = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Time;
		*_pi_WP = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Kong_Kill;
		return;
	}
	
	if (*_pi_Time == -3)
	{
		int i;
		*_pul_GameTime = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_Tick ;
		for (i = 0, *_pi_Map = 0; i < 40; i++)
			*_pi_Map += INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].ai_Score[ i ];
		*_pi_WP = INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_ID;
		*_pi_Progress = INO_gst_SavManager.ast_SlotDesc[_i_Slot].i_Progress ;
		return;
	}

	*_pi_Time = INO_gst_SavManager.ast_SlotDesc[_i_Slot].i_Time;
	*_pul_GameTime = INO_gst_SavManager.ast_SlotDesc[_i_Slot].ul_GameTime ;
	*_pi_Map = INO_gst_SavManager.ast_SlotDesc[_i_Slot].i_Map;
	*_pi_WP = INO_gst_SavManager.ast_SlotDesc[_i_Slot].i_WP ;
	*_pi_Progress = INO_gst_SavManager.ast_SlotDesc[_i_Slot].i_Progress ;

}
/**/
AI_tdst_Node *AI_EvalFunc_SAVEHeaderGet(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~*/
    AI_tdst_PushVar		st_iTimeVar;
	AI_tdst_UnionVar	iTimeVal;
    
    AI_tdst_PushVar		st_ulGameTimeVar;
	AI_tdst_UnionVar	ulGameTimeVal;
    int             iSlot;
    
    AI_tdst_PushVar		st_iMapVar;
	AI_tdst_UnionVar	iMapVal;
    
    AI_tdst_PushVar		st_iWPVar;
	AI_tdst_UnionVar	iWPVal;
    AI_tdst_PushVar		st_iDeathNumberVar;
	AI_tdst_UnionVar	iDeathNumberVal;
	/*~~~~~~~~~~~~~~~~*/


    AI_PopVar(&iDeathNumberVal, &st_iDeathNumberVar);
    AI_PopVar(&iWPVal, &st_iWPVar);
    AI_PopVar(&iMapVal, &st_iMapVar);
    AI_PopVar(&ulGameTimeVal, &st_ulGameTimeVar);
    AI_PopVar(&iTimeVal, &st_iTimeVar);
    iSlot = AI_PopInt();

    AI_EvalFunc_SAVEHeaderGet_C
        (
        iSlot,
        (int*)st_iTimeVar.pv_Addr, 
        (int*)st_ulGameTimeVar.pv_Addr, 
        (int*)st_iMapVar.pv_Addr, 
        (int*)st_iWPVar.pv_Addr, 
        (int*)st_iDeathNumberVar.pv_Addr
        );

	
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *AI_EvalFunc_SAVEHeaderGetName_C( int _i_Slot )
{
    if(_i_Slot >= INO_Cte_SavSlotNbMax) return NULL; 
    if(_i_Slot < 0) return NULL; 

    return (INO_gst_SavManager.ast_SlotDesc[_i_Slot].asz_Name);
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVEHeaderGetName(AI_tdst_Node *_pst_Node)
{
    AI_PushString( AI_EvalFunc_SAVEHeaderGetName_C( AI_PopInt() ) );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SAVEHeaderSetScore_C( int _i_Slot, int _i_Map, int _i_Score )
{
    if(_i_Slot >= INO_Cte_SavSlotNbMax) return; 
    if(_i_Slot < 0) return; 
	if (_i_Map >= 40) return;
	if (_i_Map < 0) return;
	INO_gst_SavManager.ast_SlotDesc[_i_Slot].ai_Score[ _i_Map ] = _i_Score;
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVEHeaderSetScore(AI_tdst_Node *_pst_Node)
{
	int	i_Map, i_Score;

	i_Score = AI_PopInt();
	i_Map = AI_PopInt();
    AI_EvalFunc_SAVEHeaderSetScore_C( AI_PopInt(), i_Map, i_Score );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SAVEHeaderGetScore_C( int _i_Slot, int _i_Map )
{
    if(_i_Slot >= INO_Cte_SavSlotNbMax) return 0; 
    if(_i_Slot < 0) return 0; 
	if (_i_Map >= 40) return 0;
	if (_i_Map < 0) return 0;
	return INO_gst_SavManager.ast_SlotDesc[_i_Slot].ai_Score[ _i_Map ];
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVEHeaderGetScore(AI_tdst_Node *_pst_Node)
{
	int	i_Map;

	i_Map = AI_PopInt();
    AI_PushInt( AI_EvalFunc_SAVEHeaderGetScore_C( AI_PopInt(), i_Map) );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
	FONCTION STATS DEBUG
 =======================================================================================================================
 */

#if defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL ) || defined( _PC_RETAIL )

typedef struct STATS_Data_
{
	time_t	tt_time;
	int		i_Type;
	int		i_Time;
	int		i_GameTime;
	int		i_Map; 
	int		i_ES;
	int		i_JackWeapon[ 3 ];
	int		i_JackMunition[ 3 ];
	int		i_HayesWeapon[ 3 ];
	int		i_HayesMunition[ 3 ];
	int		i_Death;
	int		i_JackDeath;
} STATS_Data;

STATS_Data	STATS_gst_Data[ 4096 ];
int			STATS_i_NbData = 0;


#endif

void AI_EvalFunc_STATSSet_C( int _i_Command, int _i_Value )
{
#if defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL ) || defined( _PC_RETAIL )
	switch( _i_Command )
	{
	case 0:
		{
			L_FILE		h_File;
			int			i;
			struct tm	when;

			// enregistement
			h_File = L_fopen( "DbgStats.txt", "at" );
			if (h_File)
			{
				for (i = 0; i < STATS_i_NbData ; i++)
				{
					when = *localtime(&STATS_gst_Data[ i ].tt_time );
					fprintf( h_File, "%d/%02d/%02d - %02d:%02d:%02d ; ", 1900 + when.tm_year, when.tm_mon, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec );
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_Type );
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_Time);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_GameTime);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_Map);  
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_ES);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_JackWeapon[0]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_JackWeapon[1]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_JackWeapon[2]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_JackMunition[0]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_JackMunition[1]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_JackMunition[2]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_HayesWeapon[0]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_HayesWeapon[1]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_HayesWeapon[2]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_HayesMunition[0]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_HayesMunition[1]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_HayesMunition[2]);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_Death);
					fprintf( h_File, "%4d ; ", STATS_gst_Data[ i ].i_JackDeath);
					fprintf( h_File, "\n" );
				}
				fclose( h_File );
			}

			// remise à zéro
			STATS_i_NbData = 0;
			break;
		}
	case 1:	time(  &STATS_gst_Data[ STATS_i_NbData++ ].tt_time ); break;
	case 10:	STATS_gst_Data[ STATS_i_NbData ].i_Type = _i_Value; break;
	case 11:	STATS_gst_Data[ STATS_i_NbData ].i_Time = _i_Value; break;
	case 12:	STATS_gst_Data[ STATS_i_NbData ].i_GameTime = _i_Value; break;
	case 13:	STATS_gst_Data[ STATS_i_NbData ].i_Map = _i_Value; break;
	case 14:	STATS_gst_Data[ STATS_i_NbData ].i_ES = _i_Value; break;
	case 15:	STATS_gst_Data[ STATS_i_NbData ].i_JackWeapon[0] = _i_Value; break;
	case 16:	STATS_gst_Data[ STATS_i_NbData ].i_JackWeapon[1] = _i_Value; break;
	case 17:	STATS_gst_Data[ STATS_i_NbData ].i_JackWeapon[2] = _i_Value; break;
	case 18:	STATS_gst_Data[ STATS_i_NbData ].i_JackMunition[0] = _i_Value; break;
	case 19:	STATS_gst_Data[ STATS_i_NbData ].i_JackMunition[1] = _i_Value; break;
	case 20:	STATS_gst_Data[ STATS_i_NbData ].i_JackMunition[2] = _i_Value; break;
	case 21:	STATS_gst_Data[ STATS_i_NbData ].i_HayesWeapon[0] = _i_Value; break;
	case 22:	STATS_gst_Data[ STATS_i_NbData ].i_HayesWeapon[1] = _i_Value; break;
	case 23:	STATS_gst_Data[ STATS_i_NbData ].i_HayesWeapon[2] = _i_Value; break;
	case 24:	STATS_gst_Data[ STATS_i_NbData ].i_HayesMunition[0] = _i_Value; break;
	case 25:	STATS_gst_Data[ STATS_i_NbData ].i_HayesMunition[1] = _i_Value; break;
	case 26:	STATS_gst_Data[ STATS_i_NbData ].i_HayesMunition[2] = _i_Value; break;
	case 27:	STATS_gst_Data[ STATS_i_NbData ].i_Death = _i_Value; break;
	case 28:	STATS_gst_Data[ STATS_i_NbData ].i_JackDeath = _i_Value; break;
	}
#endif
}

AI_tdst_Node *AI_EvalFunc_STATSSet( AI_tdst_Node *_pst_Node )
{
	int	value;

	value = AI_PopInt();
	AI_EvalFunc_STATSSet_C( AI_PopInt(), value );
	return ++_pst_Node;
}

//------------------------------------------------------------
//   int SAV_ConvertScoreToCode(	int* io_iCode,
/// \author    YCharbonneau
/// \date      19/09/2005
/// \par       Description: 
///            Converts score parameters into int code with encryption and checksums 
///			   160 bits are used as storage
///					150 bits are really used for string
///					10 bits are padding ( 2 each for each int ) 
///					35 checksum bits 
///					115 for the data itself
/// \param     score params, tick count from the save game to randomize
/// \see 
//------------------------------------------------------------
typedef struct SAV_InternetCode_
{
	uint				iInternetCode[5];
} SAV_InternetCode;


int SAV_ConvertScoreToCode(	uint* io_iCode,
						   uint in_iID,
						   uint in_iJack_Bullet,
						   uint in_iJack_Death,
						   uint in_iJack_Kill,
						   uint in_iKong_Death,
						   uint in_iKong_Kill,
						   uint in_iKong_Time,
						   uint in_iProgress,
						   uint in_uiTotalScore,
						   int in_iTick
						   )
{
	uint iChecksum;

	{
		// 14 bits 0x3FFF
		io_iCode[0]		= ((in_iID			& 0x3FFF)	<< (32 - 14));

		// 10 bits 0x3FF
		io_iCode[0]		|= ((in_iJack_Kill	& 0x3FF)	<< (32 - 14 - 6 - 10));

		// 6 checksum bits 0x3F 
		iChecksum	 = (in_iID & 0x3FFF);
		iChecksum	+= (in_iJack_Kill	& 0x3FF);
		iChecksum   = (iChecksum & 0x3F);
		io_iCode[0]	|= 	(iChecksum	<< (32 - 14 - 6)) ;
		io_iCode[0]  = io_iCode[0] ^ ( ((in_iTick & 0x7FF) << ((in_iTick % 5)+ 4)) | ((in_iTick & 0x7FF) << 21));
		ERR_X_Assert((io_iCode[0] & 0x3) < 1);
	}
	// 2 unused padding bits

	{
		// 19 bits 0x7FFFF
		io_iCode[1] = (in_uiTotalScore & 0x7FFFF) << (32 - 19);

		// 11 bits for tick
		io_iCode[1] |= (in_iTick & 0x7FF) << (32 - 19 - 11);
		io_iCode[1] = io_iCode[1] ^ (io_iCode[0] & 0xFFFFE003);

		// 8 checksum bits
		iChecksum	 = (in_uiTotalScore & 0x7FFFF);
		iChecksum	+= (in_iTick & 0x7FF);
		iChecksum   = (iChecksum & 0xFF);
		io_iCode[2]	= (iChecksum	<< ( 32 - 8 )) ;
		ERR_X_Assert((io_iCode[1] & 0x3) < 1);
	}
	// 2 unused padding bits

	{
		// 6 bits 0x3F
		io_iCode[2] |= ((in_iProgress & 0x3F) << (32 - 8 - 6));

		// 14 bits 0x3FFF
		io_iCode[2] |= ((in_iJack_Bullet & 0x3FFF) << (32 - 8 - 6 - 14));

		// 2 bits 0x3
		io_iCode[2] |= ((in_iTick & 0x3) << (32 - 8 - 6 - 14 - 2));
		io_iCode[2] = io_iCode[2] ^ ( (in_iTick & 0x7FF) << ((in_iTick % 11)+ 4 ) ) ^ 0x0F00C30C;

		ERR_X_Assert((io_iCode[2] & 0x3) < 1); 

		// 8 checksum bits
		iChecksum	 = (in_iProgress & 0x3F);
		iChecksum	+= (in_iJack_Bullet & 0x3FFF);
		iChecksum	+= in_iTick & 0x3;
		iChecksum   = (iChecksum & 0xFF);
		io_iCode[3]	= (iChecksum << ( 32 - 8 )) ;
	}

	{
		// 7 bits 0x7F 
		io_iCode[3]		|= ((in_iJack_Death	& 0x7F)		<< (32 - 8 - 7 ));

		// 15 bits 0x7FFF
		io_iCode[3] |= ((in_iKong_Time & 0x7FFF) << (32 - 8 - 7 - 15));
		io_iCode[3] = io_iCode[3] ^ ( (in_iTick & 0x7FF) << ((in_iTick % 17)+ 4 ) ) ^ 0x757558D0 ;

		// 5 checksum bits
		iChecksum	 = (in_iJack_Death & 0x7F);
		iChecksum	+= (in_iKong_Time & 0x7FFF);
		iChecksum   = (iChecksum & 0x1F);
		ERR_X_Assert((io_iCode[3] & 0x3) < 1);
		io_iCode[4]	= (iChecksum << ( 32 - 5 )) ;
	}
	// 2 unused padding bits

	{
		// 7 bits 0x7F
		io_iCode[4] |= ((in_iKong_Death & 0x7F) << (32 - 5 - 7));

		// 10 bits 0x3FF
		io_iCode[4] |= ((in_iKong_Kill & 0x3FF) << (32 - 5 - 7 - 10));

		// 8 checksum bits
		iChecksum	= (in_iKong_Death & 0x7F);
		iChecksum	+= (in_iKong_Kill & 0x3FF);
		iChecksum   = (iChecksum & 0xFF);

		io_iCode[4]	|= (iChecksum << ( 32 - 5 - 7 - 10 - 8 )) ;
		io_iCode[4] = io_iCode[4] ^ io_iCode[2] ;
		ERR_X_Assert((io_iCode[4] & 0x3) < 1);
	}
	// 2 unused padding bits

	return TRUE;
}

//------------------------------------------------------------
//   void SAV_GetCharacterCode(	char* out_pCode, 
/// \author    YCharbonneau
/// \date      19/09/2005
/// \par       Description: 
///            First converts the score to code, then converts the code to string
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SAV_ConvertCodeToString(	char* out_pCode, 
								const SAV_InternetCode in_Code)
{
	const uint iShiftDefault = 0xFC000000;
	const uint iShiftDefaultCount = 0x06;
	uint iStrCursor = 0 ;
	uint iLineCount;	
	int iCount;	

	// recovering first 160 bits
	for(iLineCount = 0 ; iLineCount < 5; iLineCount ++ )
	{
		for(iCount = 0; iCount < 5; iCount++ )
		{
			char c;
			int iCurrentCount = (0x06 * (iCount + 1));
			int iCurrentShift = (0xFC000000 >>  (0x06 * iCount));
			unsigned int iChar = (in_Code.iInternetCode[iLineCount]) & iCurrentShift;
			iChar = iChar >> (32 - (0x06 * (iCount + 1) )) ;


			if ( iChar == 61 ) 
			{
				c = '?';
			}
			else if ( iChar == 62 ) 
			{
				c = '=';
			}
			else if ( iChar == 63 ) 
			{
				c = '-';
			}
			// special case for the O to differenciate from 0
			else if ( iChar == 14 ) 
			{
				c = '+';
			}
			else if ( iChar < 26 ) 
			{
				// A-Z
				c = iChar + 65 ;
			}
			else if (iChar >= 26 && iChar < 52) 
			{
				// a-z
				c = (iChar - 26) + 97 ;
			}
			else if ( iChar >= 52 && iChar < 61 ) 
			{
				// 0-9
				c = (iChar - 52) + 49 ;
			}
			
			out_pCode[iStrCursor++] = c;
		}
	}
	out_pCode[iStrCursor] = 0;
}

//------------------------------------------------------------
//   char* AI_EvalFunc_SAVEGetInternetCode_C(int in_iSlot)
/// \author    YCharbonneau
/// \date      19/09/2005
/// \par       Description: 
///				AI 2 C entry point, this will send saved game data to the function that converts the 
///				score information to character code
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
char* AI_EvalFunc_SAVEGetInternetCode_C(int in_iSlot)
{
	uint uiTotalScore = 0;
	uint iCode[5] = {0};
	int i;
	BOOL bValid = TRUE;
	SAV_InternetCode Code;

	for (i = 0; i < 40; i++)
	{
		uiTotalScore +=	INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].ai_Score[ i ];
	}

	
	SAV_ConvertScoreToCode(	&(Code.iInternetCode[0]),
							INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_ID,
							INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Jack_Bullet,
							INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Jack_Death,
							INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Jack_Kill,
							INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Kong_Death,
							INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Kong_Kill,
							INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Kong_Time,
							INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Progress,
							uiTotalScore,
							INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Tick);
							
	SAV_ConvertCodeToString(g_SAV_InternetCode,Code);	

#ifdef  _DEBUG
	// if code is correctly made 
	if ( bValid = SAV_ConvertCode(g_SAV_InternetCode,&iCode[0]) ) 
	{
		 bValid = SAV_ValidateCode(iCode);
	}

	// use this to stress test 
	/*
	{
		static BOOL sb_UnitTesting = FALSE;
		{
			sb_UnitTesting = FALSE;
			SAV_UnitTest_ValidateCodes(in_iSlot);
		}
	}
	*/
#endif // _DEBUG

	/* add some space between group of five characters for more readability */
	L_memmove( &g_SAV_InternetCode[24], &g_SAV_InternetCode[20], 5 );
	g_SAV_InternetCode[23] = ' ';
	L_memmove( &g_SAV_InternetCode[18], &g_SAV_InternetCode[15], 5 );
	g_SAV_InternetCode[17] = ' ';
    L_memmove( &g_SAV_InternetCode[12], &g_SAV_InternetCode[10], 5 );
	g_SAV_InternetCode[11] = ' ';
	L_memmove( &g_SAV_InternetCode[6], &g_SAV_InternetCode[5], 5 );
	g_SAV_InternetCode[5] = ' ';

	return bValid ? &g_SAV_InternetCode[0]: NULL;
}

//------------------------------------------------------------
//   AI_tdst_Node *AI_EvalFunc_SAVEGetInternetCode(AI_tdst_Node *_pst_Node)
/// \author    YCharbonneau
/// \date      19/09/2005
/// \par       Description: 
///            interprated AI entry point
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
AI_tdst_Node *AI_EvalFunc_SAVEGetInternetCode(AI_tdst_Node *_pst_Node)
{
	AI_PushString(AI_EvalFunc_SAVEGetInternetCode_C(AI_PopInt()));
	return ++_pst_Node;
}



#ifdef _DEBUG
//------------------------------------------------------------
//   BOOL SAV_ConvertInternetCode(char* in_pCode,int* in_iCode)
/// \author    YCharbonneau
/// \date      19/09/2005
/// \par       Description: 
///            this function converts back the char code to into code
/// \param     char code from the game to int array
/// \see	   
//------------------------------------------------------------
BOOL SAV_ConvertCode(char* in_pCode,uint* in_iCode)
{
	SAV_InternetCode Code = {0}; // 160 bits | 25 characters | 6 bits per character | 10 unused bits 
	char aszCode[255] = {0};
	const uint iShiftDefault = 0xFC000000;
	const uint iShiftDefaultCount = 0x06;
	uint iStrCursor = 0 ;
	uint iLineCount;	
	int iCount;	

	// recovering first 126 bits
	for(iLineCount = 0 ; iLineCount < 5; iLineCount ++ )
	{
		for(iCount = 0; iCount < 5 ; iCount++ )
		{
			char c = in_pCode[ (iLineCount * 5) +  iCount ];
			uint iChar;

			uint iCurrentCount = (0x06 * (iCount + 1));
			uint iCurrentShift = (0xFC000000 >>  (0x06 * iCount));

			if ( c ==  '?' ) 
			{
				iChar = 61;
			}
			else if ( c == '=' ) 
			{
				iChar = 62;
			}
			else if ( c == '-' ) 
			{
				iChar = 63;
			}
			// special case for the O to differenciate from 0
			else if ( c == '+' ) 
			{
				iChar = 79 - 65;
			}
			// A - Z 
			else if ( c >= 65 && c <= 90 ) 
			{
				iChar = c - 65 ;
			}
			// a-z
			else if (c >= 97 && c <= 122) 
			{
				iChar = (c - 97) + 26;
			}
			// 1-9
			else if ( c >= 49 && c <= 57 ) 
			{
				iChar = (c - 49) + 52;
			}
			else 
			{
				// code is invalid
				ERR_X_Assert(FALSE && "Code is invalid");
				return FALSE;
			}
			in_iCode[iLineCount] |=  ( (iChar & 0x0000003F) << (32 - iCurrentCount)) ;
		}
	}
	return TRUE;
}
#endif // #ifdef _DEBUG

//------------------------------------------------------------
//   BOOL SAV_ValidateInternetCode( int* io_iCode )
/// \author    YCharbonneau
/// \date      19/09/2005
/// \par       Description: 
///				validate the received code to make sure it is valid
///				with saved checksum and xor's
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
#ifdef _DEBUG
BOOL SAV_ValidateCode( uint* io_iCode )
{

	// retreiving un crypted tick on 11 bits (0x7FF)
	int iTick		= (io_iCode[1] & 0x00000FFC) >> (32 - 19 - 11  );

	// checking back the data 
	{ 
		uint i_ID;				// 14 bits 0x3FFF
		uint i_Jack_Kill;		// 10 bits 0x3FF
		uint iChecksumRecovery;	// 6 checksum bits 0x3F
		uint iCode;

		iCode		= io_iCode[0] ^ ( ((iTick & 0x7FF) << ((iTick % 5) + 4)) | ((iTick & 0x7FF) << 21));
		i_ID		= (iCode & 0xFFFC0000) >> (32 - 14);
		i_Jack_Kill = (iCode & 0x00000FFC) >> (32 - 14 - 6 - 10  );
		iChecksumRecovery = (iCode & 0x0003F000) >> (32 - 14 - 6 );
		// validating checksum
		if ( ((i_ID + i_Jack_Kill) & 0x3F) != iChecksumRecovery ) 
		{
			// checksum is invalid 
			ERR_X_Assert(FALSE && "Internet code checksum does not math" ) ;
			return FALSE; // code is invalid
		}	
	}
	// 2 unused bits 

	// checking back the data 
	{ 
		uint r_iTotalScore; // 19 bits 0x7FFFF
		uint iChecksumRecovery; // 8 checksum bits
		uint iCode;

		iCode			= io_iCode[1] ^ io_iCode[0];
		r_iTotalScore	= (iCode & 0xFFFFE000) >> (32 - 19);
		iChecksumRecovery = ((io_iCode[2] ^( (iTick & 0x7FF) << ((iTick % 11)+ 4 ) ) ^ 0x0F00C30C) & 0xFF000000) >> (32 - 8);
		// validating checksum
		if ( ((r_iTotalScore + iTick) & 0xFF) != iChecksumRecovery ) 
		{
			// checksum is invalid 
			ERR_X_Assert(FALSE && "Internet code checksum does not math" ) ;
			return FALSE; // code is invalid
		}	
	}

	// checking back the data 
	{ 
		uint i_Progress; // 6 bits 0x3F
		uint i_Jack_Bullet; // 14 bits 0x3FFF
		uint i_Tick; // 2 bits 0x3
		uint iChecksumRecovery; // 8 checksum bits
		uint iCode;

		iCode			= io_iCode[2] ^ ( (iTick & 0x7FF) << ((iTick % 11)+ 4 ) ) ^ 0x0F00C30C;
		i_Progress		= (iCode & 0x00FC0000) >> (32 - 8 - 6);
		i_Jack_Bullet	= (iCode & 0x0003FFF0) >> (32 - 8 - 6 - 14);
		i_Tick			= (iCode & 0x0000000C) >> (32 - 8 - 6 - 14 - 2);
		iChecksumRecovery = ((io_iCode[3] ^ ( (iTick & 0x7FF) << ((iTick % 17)+ 4 ) ) ^ 0x757558D0) & 0xFF000000) >> (32 - 8);
		// validating checksum
		if ( ((i_Progress + i_Jack_Bullet + i_Tick) & 0xFF) != iChecksumRecovery ) 
		{
			// checksum is invalid 
			ERR_X_Assert(FALSE && "Internet code checksum does not math" ) ;
			return FALSE; // code is invalid
		}	
	}
	// checking back the data 
	{ 
		uint i_Jack_Death; // 7 bits 0x7F 
		uint i_Kong_Time; // 15 bits 0x7FFF
		uint iChecksumRecovery; // 5 checksum bits
		uint iCode;

		iCode		= io_iCode[3] ^ ( (iTick & 0x7FF) << ((iTick % 17)+ 4 ) ) ^ 0x757558D0;
		i_Jack_Death= (iCode & 0x00FE0000) >> (32 - 8 - 7);
		i_Kong_Time	= (iCode & 0x0001FFFF) >> (32 - 8 - 7 - 15);
		iChecksumRecovery = ((io_iCode[4] ^ io_iCode[2])& 0xF8000000) >> (32 - 5);
		// validating checksum
		if ( ((i_Jack_Death + i_Kong_Time ) & 0x1F) != iChecksumRecovery ) 
		{
			// checksum is invalid 
			ERR_X_Assert(FALSE && "Internet code checksum does not math" ) ;
			return FALSE; // code is invalid
		}	
	}
	// 2 unused padding bits


	// checking back the data 
	{ 
		uint i_Kong_Death;		// 7 bits 0x7F
		uint i_Kong_Kill;		// 10 bits 0x3FF
		uint iChecksumRecovery;	// 8 checksum bits
		uint iCode;

		iCode		= io_iCode[4] ^ io_iCode[2];
		i_Kong_Death= (iCode & 0x07F00000) >> (32 - 5 - 7);
		i_Kong_Kill	= (iCode & 0x000FFC00) >> (32 - 5 - 7 - 10);
		iChecksumRecovery = (iCode & 0x000003FC) >> (32 - 5 - 7 - 10 - 8);
		// validating checksum
		if ( ((i_Kong_Death + i_Kong_Kill ) & 0xFF) != iChecksumRecovery ) 
		{
			// checksum is invalid 
			ERR_X_Assert(FALSE && "Internet code checksum does not math" ) ;
			return FALSE; // code is invalid
		}	
	}
	return TRUE;
}
#endif // #ifdef _DEBUG

#ifdef _DEBUG
BOOL SAV_UnitTest_ValidateCodes(int in_iSlot)
{
	char aszCode[25] = {0};
	uint i,j,k,l,m,n,o,p;
	uint uiTotalScore = 0;
	FILE* file = fopen("c:\\code.txt","w"); 

	//total score for all maps; < 500000
	//unit testing: testing all cases
	//this may take a while
	for ( i = 0 ; i < 16384; i ++ )
	{	
		if ( !(i < 1 || i == 11739 || i == 16383 )) continue;
		INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_ID = i;
		for(j = 0 ; j < 16384; j++ ) 
		{
			if ( !(j < 1 || j == 7256 || j == 16383 )) continue;

			INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Jack_Bullet = j;
			for ( k = 0 ; k < 128; k++ ) 
			{
				if ( !(k < 1 || k == 68 || k == 127 )) continue;
				INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Jack_Death = k;
				for ( l = 0; l < 1024; l ++ )
				{
					if ( !(l < 1 || l == 325 || l == 1023 )) continue;
					INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Jack_Kill = l;
					for ( m = 0; m < 128; m ++ ) 
					{
						if ( !(m < 1 || m == 65 || m == 127 )) continue;
						INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Kong_Death = m;
						for ( n = 0 ; n < 1024; n++ ) 
						{
							if ( !(n < 1 || n == 523 || n == 1023 )) continue;

							INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Kong_Kill = n;
							for (o = 0; o < 64; o++ ) 
							{
								if ( !(o < 1 || o == 23 || o == 63 )) continue;

								INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Progress = o;
								for ( p = 0; p < 524288; p++) 
								{
									uint iCode[5] = {0};
									uint iScore = 0;
									char *pCode = NULL;
									INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].ai_Score[ 0 ] = p;
									
									if ( !(p < 1 || p == 235645 || p == 524287 )) continue;
									uiTotalScore = 0;

									for (iScore = 0; iScore < 40; iScore++)
									{
										uiTotalScore +=	INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].ai_Score[ iScore ];
									}

									INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Tick = 798;//rand() & 0x3FF ;
									pCode = AI_EvalFunc_SAVEGetInternetCode_C(in_iSlot);


									if ( file ) 
									{
										fprintf(file,"%s [%d;%d;%d;%d;%d;%d;%d;%d;%d;%d]\n",pCode,
										INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_ID,
										INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Jack_Bullet,
										INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Jack_Death,
										INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Jack_Kill,
										INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Kong_Death,
										INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Kong_Kill,
										INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Kong_Time,
										INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Progress,
										uiTotalScore,
										INO_gst_SavManager.ast_SlotDesc[ in_iSlot ].i_Tick);
									}
		
									
								}
							}
						}
					}
				}
			}
		}
	}
	fclose(file);
	return TRUE;
}
#endif

/*$4
 ***********************************************************************************************************************
    FUNCTION to GENERATE ONLINE CODE FROM SLOT ID
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
	generate one of the four reentering internet code ( generate 0-based int value )
	in :	i_ID	=> ID of the save (0..16283)
			i_Num	=> num of the code (0..3)
			pi_Code => pointer on an int array (assume that size of array is more or equal to 4)
	out :	pi_Code is filled with code value
 =======================================================================================================================
 */
void SAV_GenCode( int i_Id, int i_Num, int *pi_Code )
{
	int		*pGP, GP[32] = {2,97,1,37,5,71,11,37,3,73,5,37,11,89,7,37,2,73,3,37,7,97,5,37,1,83,2,79,3,97,11,79};
	int		GD[ 4] = {0,997,1994, 2991 };
	int		GA[ 4] = {2, 79, 59, 83};
	int		id;

	id  = (i_Id + GD[ i_Num ]) % 16384;
	pGP = GP + (8 * (i_Num & 0x3) );
	pi_Code[ 0 ] = ( ((id * pGP[0]) % pGP[1]) + GA[0] ) % 50;
	pi_Code[ 1 ] = ( ((id * pGP[2]) % pGP[3]) + GA[1] ) % 8;
	pi_Code[ 2 ] = ( ((id * pGP[4]) % pGP[5]) + GA[2] ) % 50;
	pi_Code[ 3 ] = ( ((id * pGP[6]) % pGP[7]) + GA[3] ) % 8;
}

/*
 =======================================================================================================================
	generate one of the four reentering internet code ( generate character value )
	in :	i_ID	=> ID of the save (0..16283)
			i_Num	=> num of the code (0..3)
			pc_Code => pointer on an char array (assume that size of array is more or equal to 4)
	out :	pc_Code is filled with the code string
 =======================================================================================================================
 */
void SAV_GenCodeChar( int i_Id, int i_Num, char *pc_Code )
{
	int i, ai_Code[ 4 ];
	
	SAV_GenCode( i_Id, i_Num, ai_Code );
	
	i = ai_Code[0];
	if (i <= 13)		pc_Code[0] = 'A' + i;
	else if (i <= 24)	pc_Code[0] = 'A' + i + 1;
	else if (i <= 38)	pc_Code[0] = 'a' + i - 25;
	else				pc_Code[0] = 'a' + i - 24;
	pc_Code[ 1 ] = ai_Code[1] + '2';
	i = ai_Code[2];
	if (i <= 13)		pc_Code[2] = 'A' + i;
	else if (i <= 24)	pc_Code[2] = 'A' + i + 1;
	else if (i <= 38)	pc_Code[2] = 'a' + i - 25;
	else				pc_Code[2] = 'a' + i - 24;
	pc_Code[ 3 ] =  ai_Code[3] + '2';
}

/*
 =======================================================================================================================
	same as above but shorter
	note :	not used because a little complicated, but it's fun isn't it ?
 =======================================================================================================================
 */
/*
void SAV_GenCodeCharShort( int i_Id, int i_Num, char *pc_Code )
{
	int j, code[4], *c = code;
	SAV_GenCode( i_Id, i_Num, code );
	pc_Code[0] = ((j = (*c < 25) ? 'A' : (*c-=25, 'a')), ((*c <= 13) ? j + *c : j + *c + 1)), c++;
	pc_Code[1] = *c++ + '2';
	pc_Code[2] = ((j = (*c < 25) ? 'A' : (*c-=25, 'a')), ((*c <= 13) ? j + *c : j + *c + 1)), c++;
	pc_Code[3] = *c + '2';
}
*/

/*
 =======================================================================================================================
	fonction to generate code string
	to be used in the magnificient internet code utility of the great Yan
 =======================================================================================================================
 */
/*
char sz_ReenteringCode[ 5 ];
char *GenReenteringCodeString( int i_Id, int i_Num )
{
	SAV_GenCodeChar( INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_ID, _i_Num, sz_ReenteringCode );
	sz_ReenteringCode[ 4 ] = 0;
	return sz_ReenteringCode;
}
*/

/*
 =======================================================================================================================
	IA function to compare internet code with the 4 character given by user
 =======================================================================================================================
 */
int AI_EvalFunc_SAVECompareOnlineCode_C( int _i_Slot, int _i_Num, int c0, int c1, int c2, int c3 )
{
	char ac_Code[ 4 ];

	if ((_i_Slot < 0) || (_i_Slot >= INO_Cte_SavSlotNbMax)) return 0; 
	if ((_i_Num < 0) || (_i_Num >= 4)) return 0; 
	
	//SAV_GenCodeCharShort( INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_ID, _i_Num, ac_Code );
	SAV_GenCodeChar( INO_gst_SavManager.ast_SlotDesc[ _i_Slot ].i_ID, _i_Num, ac_Code );
	if (c0 != ac_Code[ 0 ]) return 0;
	if (c1 != ac_Code[ 1 ]) return 0;
	if (c2 != ac_Code[ 2 ]) return 0;
	if (c3 != ac_Code[ 3 ]) return 0;
	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_SAVECompareOnlineCode(AI_tdst_Node *_pst_Node)
{
	int c3, c2, c1, c0, num;

	c3 = AI_PopInt();
	c2 = AI_PopInt();
	c1 = AI_PopInt();
	c0 = AI_PopInt();
	num = AI_PopInt();
	AI_PushInt(AI_EvalFunc_SAVECompareOnlineCode_C(AI_PopInt(), num, c0, c1, c2, c3));
	return ++_pst_Node;
}


/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
