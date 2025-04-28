/*$T STRdata.c GC! 1.081 02/26/04 11:17:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifndef PSX2_TARGET
#pragma once
#endif
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "LINKs/LINKmsg.h"
#include "STRdata.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

STR_tdst_AllData	STR_gst_Data;
BOOL				STR_gb_DataIsInit = 0;

#ifdef STR_DEBUG0
char				szDBG0[256];


#endif

/*$4
 ***********************************************************************************************************************
    Functions - Test
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STRDATA_Log( void )
{
	int		i, nbstring, nbletter;
	char	sz_Text[ 512 ];
	
	nbstring = nbletter = 0;
	for (i = 0; i < STR_gst_Data.uw_MaxString; i++ )
	{
		if ( STR_gst_Data.pst_String[i].ul_Flags & STR_String_Used )
		{
			nbstring++;
			nbletter += L_strlen( STR_gst_Data.pc_Letter + STR_gst_Data.pst_String[i].uw_First );
		}
	}
	sprintf( sz_Text, "STR : %d strings, %d letters", nbstring, nbletter );
	LINK_PrintStatusMsg( sz_Text );
	
	for (i = 0; i < STR_gst_Data.uw_MaxString; i++ )
	{
		if ( STR_gst_Data.pst_String[i].ul_Flags & STR_String_Used )
		{
			sprintf( sz_Text, "%2d - \n", i );
			LINK_PrintStatusMsg( sz_Text );
			LINK_PrintStatusMsg( STR_gst_Data.pc_Letter + STR_gst_Data.pst_String[i].uw_First );
		}
	}
}


/*$4
 ***********************************************************************************************************************
    Functions - Init
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STRDATA_Init(void)
{
	if (STR_gb_DataIsInit) 
	{
		STRDATA_ReinitAll();
		return;
	}
	STR_gb_DataIsInit = TRUE;
	
	L_memset(&STR_gst_Data, 0, sizeof(STR_tdst_AllData));

	STR_gst_Data.uw_MaxLetter = STR_Alloc_NbLetter;
	STR_gst_Data.pc_Letter = (char *) MEM_p_Alloc(STR_Alloc_NbLetter);
	L_memset(STR_gst_Data.pc_Letter, 0, STR_Alloc_NbLetter);

	STR_gst_Data.uw_MaxString = STR_Alloc_NbString;
	STR_gst_Data.pst_String = (STR_tdst_OneString *) MEM_p_Alloc(STR_Alloc_NbString * sizeof(STR_tdst_OneString));
	L_memset(STR_gst_Data.pst_String, 0, STR_Alloc_NbString * sizeof(STR_tdst_OneString));
	
	STR_gst_Data.f_SizeX = STR_Default_SizeX;
	STR_gst_Data.f_SizeY = STR_Default_SizeY;
	STR_gst_Data.f_XMul = 1.0f;
	STR_gst_Data.f_XEcart = 0.0f;
	STR_gst_Data.f_YEcart = 0.0f;
	STR_gst_Data.ul_Group_Display = 0xFFFFFFFF;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STRDATA_Close(void)
{
	if (!STR_gb_DataIsInit) return;
	STR_gb_DataIsInit = FALSE;
	MEM_Free(STR_gst_Data.pc_Letter);
	MEM_Free(STR_gst_Data.pst_String);
	L_memset(&STR_gst_Data, 0, sizeof(STR_tdst_AllData));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STRDATA_ReinitAll(void)
{
	L_memset(STR_gst_Data.pst_String, 0, STR_gst_Data.uw_MaxString * sizeof(STR_tdst_OneString));
	L_memset(STR_gst_Data.pc_Letter, 0, STR_gst_Data.uw_MaxLetter);
	STR_gst_Data.uw_NbLetter = 0;
	STR_gst_Data.uw_NbString = 0;
	STR_gst_Data.f_SizeX = STR_Default_SizeX;
	STR_gst_Data.f_SizeY = STR_Default_SizeY;
	STR_gst_Data.ul_Group_Display = 0xFFFFFFFF;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STRDATA_Reset(void)
{
	STRDATA_ReinitAll();
}

/*$4
 ***********************************************************************************************************************
    Functions - Buffer
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL STRDATA_IncreaseLetterBuffer(void)
{
	if(STR_gst_Data.uw_MaxLetter + STR_Alloc_LetterGran > STR_Alloc_LetterMax) return FALSE;

	STR_gst_Data.uw_MaxLetter += STR_Alloc_LetterGran;
	STR_gst_Data.pc_Letter = (char *) MEM_p_Realloc(STR_gst_Data.pc_Letter, STR_gst_Data.uw_MaxLetter);
	L_memset(STR_gst_Data.pc_Letter + (STR_gst_Data.uw_MaxLetter - STR_Alloc_LetterGran), 0, STR_Alloc_LetterGran);

#ifdef STR_DEBUG0
	sprintf(szDBG0, "STRDATA_IncreaseLetterBuffer : increase to %d char", STR_gst_Data.uw_MaxLetter);
	LINK_PrintStatusMsg(szDBG0);
#endif
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL STRDATA_IncreaseStringBuffer(void)
{
	if(STR_gst_Data.uw_MaxString + STR_Alloc_StringGran > STR_Alloc_StringMax) return FALSE;

	STR_gst_Data.uw_MaxString += STR_Alloc_StringGran;
	STR_gst_Data.pst_String = (STR_tdst_OneString *) MEM_p_Realloc
		(
			STR_gst_Data.pst_String,
			STR_gst_Data.uw_MaxString * sizeof(STR_tdst_OneString)
		);
	L_memset(STR_gst_Data.pst_String + (STR_gst_Data.uw_MaxString - STR_Alloc_StringGran), 0, STR_Alloc_StringGran * sizeof(STR_tdst_OneString));

#ifdef STR_DEBUG0
	sprintf(szDBG0, "STRDATA_IncreaseStringBuffer : increase to %d string", STR_gst_Data.uw_MaxString);
	LINK_PrintStatusMsg(szDBG0);
#endif
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STRDATA_DefragBuffer(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	STR_tdst_OneString	*S;
	ULONG				*pul_Src, *pul_Tgt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	STR_gst_Data.uw_NbLetter = 0;
	S = STR_gst_Data.pst_String;

	for(i = 0; i < STR_gst_Data.uw_NbString; i++, S++)
	{
		if(S->ul_Flags & STR_String_Used)
		{
			if(S->uw_First != STR_gst_Data.uw_NbLetter)
			{
				pul_Tgt = (ULONG *) (STR_gst_Data.pc_Letter + STR_gst_Data.uw_NbLetter);
				pul_Src = (ULONG *) (STR_gst_Data.pc_Letter + S->uw_First);
				L_memcpy(pul_Tgt, pul_Src, S->uw_Nb);
				S->uw_First = STR_gst_Data.uw_NbLetter;
			}

			STR_gst_Data.uw_NbLetter += S->uw_Nb;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL STRDATA_SetStringSize(int _i_String, int _i_Length)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					k, i_L32, i_ToAdd;
	STR_tdst_OneString	*S, *SLoop;
	ULONG				*pul_Src, *pul_Tgt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* get size with gran */
	if(_i_Length <= 0)
		i_L32 = 0;
	else
		i_L32 = (((_i_Length - 1) >> 5) + 1) << 5;

	/* insert a string ? */
	if(_i_String >= STR_gst_Data.uw_MaxString)
	{
		if(!i_L32) return TRUE;
		if(!STRDATA_IncreaseStringBuffer()) return FALSE;
	}

	/* get current string */
	S = STR_gst_Data.pst_String + _i_String;

	/* string no more used */
	if(!i_L32)
	{
		S->ul_Flags = 0;
		return TRUE;
	}

	/* current string was unused, compute possible first and number of letter */
	if(!(S->ul_Flags & STR_String_Used))
	{
		S->ul_Flags = STR_String_Used;

		S->uw_First = 0;
		SLoop = S - 1;
		while(SLoop >= STR_gst_Data.pst_String)
		{
			if(SLoop->ul_Flags & STR_String_Used)
			{
				S->uw_First = SLoop->uw_First + SLoop->uw_Nb;
				break;
			}

			SLoop--;
		}

		S->uw_Nb = 0;
		SLoop = S + 1;
		while(SLoop < STR_gst_Data.pst_String + STR_gst_Data.uw_NbString)
		{
			if(SLoop->ul_Flags & STR_String_Used)
			{
				S->uw_Nb = SLoop->uw_First - S->uw_First;
				break;
			}

			SLoop++;
		}
	}

	/* current size greater than required size */
	if(i_L32 <= S->uw_Nb)
	{
		S->uw_Nb = i_L32;
		return TRUE;
	}

	i_ToAdd = i_L32 - S->uw_Nb;

	/* add letter ? */
	while(STR_gst_Data.uw_NbLetter + i_ToAdd > STR_gst_Data.uw_MaxLetter)
	{
		if(!STRDATA_IncreaseLetterBuffer()) 
			return FALSE;
	}

	/* decal string above current one */
	SLoop = STR_gst_Data.pst_String + (STR_gst_Data.uw_NbString - 1);
	while(SLoop > S)
	{
		if ( SLoop->ul_Flags & STR_String_Used )
		{
			pul_Src = (ULONG *) (STR_gst_Data.pc_Letter + (SLoop->uw_First + SLoop->uw_Nb));
			pul_Tgt = pul_Src + (i_ToAdd >> 2);
			k = SLoop->uw_Nb >> 2;
			while(k--) L_memcpy(--pul_Tgt, --pul_Src, sizeof(ULONG)); 
			SLoop->uw_First += i_ToAdd;
		}
		SLoop--;
	}

	S->uw_Nb = i_L32;
	STR_gst_Data.uw_NbLetter += i_ToAdd;
	
	{
		int ii;
		ii = (int)(STR_gst_Data.pc_Letter + STR_gst_Data.pst_String[ _i_String ].uw_First + _i_Length - 8);
		
		if( (ii & 3) == 0)
		{
			if(*(int*)(ii) == 0xA0110270)
			{
				printf("ddd\n");
			}
		}
	}
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 int STRDATA_i_GetUnusedString( void )
 {
 int i;
 
	if ( STR_gst_Data.uw_NbString < STR_gst_Data.uw_MaxString )
		return STR_gst_Data.uw_NbString;
	
	for (i = 0; i < STR_gst_Data.uw_NbString; i++ )
	{
		if ( !(STR_gst_Data.pst_String[i].ul_Flags & STR_String_Used) )
			return i;
	}
	
	if(!STRDATA_IncreaseStringBuffer()) return -1;
	
	return STR_gst_Data.uw_NbString;
 }
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
 int STRDATA_i_CreateTextGroup( char *_sz_String, MATH_tdst_Vector *_st_Pos, float _f_Timer, int _i_Group )
 {
	int	i_String, i_Length;
	
	i_String = STRDATA_i_GetUnusedString();
	if (i_String == -1) return -1; 
	STR_gst_Data.pst_String[ i_String ].c_Group = (char) _i_Group;
	
	i_Length = L_strlen( _sz_String ) + 1;
	
	if ( !STRDATA_SetStringSize( i_String, i_Length ) )
		return -1;
	
		
	L_strcpy( STR_gst_Data.pc_Letter + STR_gst_Data.pst_String[ i_String ].uw_First, _sz_String );
	STR_gst_Data.pst_String[ i_String ].f_Timer = _f_Timer;
	MATH_CopyVector( &STR_gst_Data.pst_String[ i_String ].st_Pivot, _st_Pos );
	
	
	if ( i_String >= STR_gst_Data.uw_NbString )
		STR_gst_Data.uw_NbString = i_String + 1;
		
	return i_String;
 }

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 int STRDATA_i_CreateText( char *_sz_String, MATH_tdst_Vector *_st_Pos, float _f_Timer )
 {
	return STRDATA_i_CreateTextGroup( _sz_String, _st_Pos, _f_Timer, 0 );
 }
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void STRDATA_DeleteText( int _i_String )
 {
	STR_tdst_OneString *S;
	
	if ( (_i_String < 0) || (_i_String >= STR_gst_Data.uw_NbString) ) return;
	S = STR_gst_Data.pst_String + _i_String;
	S->ul_Flags &= ~STR_String_Used;
	STR_gst_Data.uw_NbLetter -= S->uw_Nb;
 }
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void STRDATA_AppendText( int _i_String, char *_sz_String )
 {
	STR_tdst_OneString *S;
	int					i_Length;
	
	if ( (_i_String < 0) || (_i_String >= STR_gst_Data.uw_NbString) ) return;
	S = STR_gst_Data.pst_String + _i_String;
	if ( !(S->ul_Flags & STR_String_Used)) return;
	
	i_Length = L_strlen( STR_gst_Data.pc_Letter + S->uw_First ) + L_strlen( _sz_String ) + 1;
	
	STRDATA_SetStringSize( _i_String, i_Length );
	L_strcat( STR_gst_Data.pc_Letter + S->uw_First, _sz_String );
 }
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void STRDATA_AppendInt( int _i_String, int _i_Val )
 {
	char	sz_Text[ 64 ];
	sprintf( sz_Text, "%d", _i_Val );
	STRDATA_AppendText( _i_String, sz_Text );
 }
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void STRDATA_AppendHexa( int _i_String, int _i_Val )
 {
	char	sz_Text[ 64 ];
	sprintf( sz_Text, "%X", _i_Val );
	STRDATA_AppendText( _i_String, sz_Text );
 }
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void STRDATA_AppendFloat( int _i_String, float _f_Val, int _i_Dec )
 {
	char	sz_Format[ 16 ], sz_Text[ 64 ];
	if (_i_Dec == -1)
		L_strcpy( sz_Format, "%f" );
	else
		sprintf( sz_Format, "%%.%df", _i_Dec );
	sprintf( sz_Text, sz_Format, _f_Val );
	STRDATA_AppendText( _i_String, sz_Text );
 }
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void STRDATA_AppendVector( int _i_String, MATH_tdst_Vector *V )
 {
	char	sz_Text[ 64 ];
	sprintf( sz_Text, "(%.3f, %.3f, %.3f)", V->x, V->y, V->z );
	STRDATA_AppendText( _i_String, sz_Text );
 }
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void STRDATA_AppendGao( int _i_String, OBJ_tdst_GameObject *_pst_GO )
 {
 #ifdef _DEBUG
	if (!_pst_GO)
		STRDATA_AppendText( _i_String, "<obj null>");
	else if (!_pst_GO->sz_Name)
		STRDATA_AppendText( _i_String, "<name null>");
	else
		STRDATA_AppendText( _i_String, _pst_GO->sz_Name );
#endif
 }
 
/*$4
 ***********************************************************************************************************************
    Functions - Group
 ***********************************************************************************************************************
 */
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STRDATA_SetGroup( int _i_String, int _i_Group )
{
	STR_tdst_OneString *S;
	
	if ( (_i_String < 0) || (_i_String >= STR_gst_Data.uw_NbString) ) return;
	S = STR_gst_Data.pst_String + _i_String;
	if ( !(S->ul_Flags & STR_String_Used)) return;
	
	S->c_Group = (char) _i_Group;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STRDATA_i_Group_SetDisplay( int _i_On, int _i_Off )
{
	STR_gst_Data.ul_Group_Display |= _i_On;
	STR_gst_Data.ul_Group_Display &= ~_i_Off;
	return STR_gst_Data.ul_Group_Display;
}

 
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
