/*$T STRstruct.c GC!1.71 03/03/00 11:18:24 */

/*
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
#include "STRing/STRstruct.h"
#include "STRing/STRdata.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "TEXture/TEXfile.h"
#include "GFX/GFX.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"

#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#endif
#include "TIMer/TIMdefs.h"

#ifdef PSX2_TARGET
#include "PSX2debug.h"
#include "GS_PS2/Gsp.h"
#endif

#ifdef _XBOX
#include "Gx8\Gx8buffer.h"
#endif // _XBOX

#if defined(_PC_RETAIL)
#include "Dx9\Dx9buffer.h"
#endif // defined(_PC_RETAIL)

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    external
 ***********************************************************************************************************************
 */
#ifdef PSX2_TARGET
extern BOOL	ps2MAI_gb_VideoModeNTSC;
#endif /*PSX2_TARGET*/

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

OBJ_tdst_GameObject  *STR_sgpst_GO;

STR_tdst_Format      STR_sgst_Format;
short				 STR_w_XFramePivot, STR_w_YFramePivot;
ULONG                STR_ul_Color;

short                STR_w_XPivot, STR_w_YPivot;
short                STR_w_LetterDX, STR_w_LetterDY;

float                STR_f_DefaultLetterDepth = 0.01f;
float                STR_f_FrameDepthOffset = 0.0001f;

LONG                 STR_sgl_NumberOfFont;
LONG                 STR_sgl_MaxNumberOfFont;
STR_tdst_FontDesc    *STR_sgpst_Font;

int                  STR_sgac_CharTable[256];

#define              STR_C_3DString_Max 64
OBJ_tdst_GameObject  *STR_3DString_Obj[ STR_C_3DString_Max ];
int                  STR_3DString_Number;

char                STR_sgaac_Global[ STR_Cuc_NbGlobal ][ STR_Cuc_GlobalLength];

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
STR_tdst_Struct *STR_pst_Create(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Struct *pst_STR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = (STR_tdst_Struct *) MEM_p_Alloc(sizeof(STR_tdst_Struct));
    L_memset(pst_STR, 0, sizeof(STR_tdst_Struct));
    GRO_Struct_Init(&pst_STR->st_Id, GRO_2DText);

    pst_STR->uc_NbMaxStrings = 80;
    pst_STR->dst_String = (STR_tdst_String *) MEM_p_Alloc(sizeof(STR_tdst_String) * pst_STR->uc_NbMaxStrings);
    L_memset(pst_STR->dst_String, 0, sizeof(STR_tdst_String) * 64);

    pst_STR->uc_NbMaxEffects = 96;
    pst_STR->uc_NbCurEffects = 0;
    pst_STR->uw_SW = 640;
    pst_STR->uw_SH = 480;
    L_memset( &pst_STR->st_Effect, 0, pst_STR->uc_NbMaxEffects * sizeof( STR_tdst_Effect ) );

    pst_STR->w_FontPage = -1;

    return pst_STR;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Free(STR_tdst_Struct *_pst_STR)
{
    if(_pst_STR == NULL) return;
    if(_pst_STR->st_Id.i->ul_Type != GRO_2DText)
        return;
    if(_pst_STR->st_Id.l_Ref > 0) return;

    GRO_Struct_Free(&_pst_STR->st_Id);
    if(_pst_STR->ul_NbMaxLetters) MEM_Free(_pst_STR->dst_Letter);

    if(_pst_STR->uc_NbMaxStrings) MEM_Free(_pst_STR->dst_String);

    LOA_DeleteAddress(_pst_STR);
    MEM_Free(_pst_STR);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_ChangeNumberMaxOfLetters(STR_tdst_Struct *_pst_STR, ULONG _ul_NewMax)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String, *pst_Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_ul_NewMax == _pst_STR->ul_NbMaxLetters)
        return;

    if(_pst_STR->ul_NbMaxLetters == 0)
    {
        _pst_STR->dst_Letter = (STR_tdst_Letter *) MEM_p_Alloc(sizeof(STR_tdst_Letter) * _ul_NewMax);
        L_memset(_pst_STR->dst_Letter, 0, sizeof(STR_tdst_Letter) * _ul_NewMax);
    }
    else
    {
        if(_ul_NewMax == 0)
            MEM_Free(_pst_STR->dst_Letter);
        else
        {
            _pst_STR->dst_Letter = (STR_tdst_Letter *) MEM_p_Realloc
                (
                    _pst_STR->dst_Letter,
                    sizeof(STR_tdst_Letter) * _ul_NewMax
                );
            if(_pst_STR->ul_NbMaxLetters < _ul_NewMax)
            {
                L_memset
                (
                    _pst_STR->dst_Letter + _pst_STR->ul_NbMaxLetters,
                    0,
                    sizeof(STR_tdst_Letter) * (_ul_NewMax - _pst_STR->ul_NbMaxLetters)
                );
            }
        }
    }

    _pst_STR->ul_NbMaxLetters = _ul_NewMax;

    /*
     * Check string to be sure that a string don't reference a character out of array
     */
    pst_String = _pst_STR->dst_String;
    pst_Last = pst_String + _pst_STR->uc_NbMaxStrings;
    for(; pst_String < pst_Last; pst_String++)
    {
        if(pst_String->uw_First >= _ul_NewMax)
        {
            pst_String->uw_First = (USHORT) _ul_NewMax - 1;
            pst_String->uw_Number = 0;
        }
        else if(pst_String->uw_First + pst_String->uw_Number > (USHORT) _ul_NewMax)
            pst_String->uw_Number = (USHORT) (_ul_NewMax - pst_String->uw_First);
    }

    /* delete all effects to avoid bug */
    _pst_STR->uc_NbCurEffects = 0;
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Output(STR_tdst_Struct *_pst_STR)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char            sz_Text[256], *pc_Cur;
    STR_tdst_Letter *pst_L;
    STR_tdst_String *pst_S;
    int             i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*$off*/
    sprintf( sz_Text, "String (char : %d, string : %d, effect : %d", _pst_STR->ul_NbMaxLetters, _pst_STR->uc_NbMaxStrings, _pst_STR->uc_NbMaxEffects );
    LINK_PrintStatusMsg( sz_Text );

    sprintf( sz_Text, "Char Buffer" );
    LINK_PrintStatusMsg( sz_Text );

    pst_L = _pst_STR->dst_Letter;
    pc_Cur = sz_Text;
    for ( i = 0; i < (int) _pst_STR->ul_NbMaxLetters; i++, pst_L++)
    {

        *pc_Cur++ = (char) (pst_L->ul_Flags & STR_Cul_LF_ValueMask);
        if ((i & 0x1F) == 0x1F)
        {
            *pc_Cur = 0;
            LINK_PrintStatusMsg( sz_Text );
            pc_Cur = sz_Text;
        }
    }
    if (pc_Cur != sz_Text)
    {
        *pc_Cur = 0;
        LINK_PrintStatusMsg( sz_Text );
    }

    sprintf( sz_Text, "String" );
    LINK_PrintStatusMsg( sz_Text );
    pst_S = _pst_STR->dst_String;
    for (i = 0; i < (int) _pst_STR->uc_NbMaxStrings; i++, pst_S++ )
    {
        sprintf( sz_Text, "%3d : %3d / %3d", i, pst_S->uw_First, pst_S->uw_Number );
        LINK_PrintStatusMsg( sz_Text );
    }
    /*$on*/
}

#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *STR_p_CreateFromBuffer(GRO_tdst_Struct *_pst_Id, char **ppc_Buffer, WOR_tdst_World *_pst_World)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Struct *pst_STR;
    STR_tdst_Letter *pst_Letter, *pst_Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Create();

    pst_STR->ul_NbMaxLetters = LOA_ReadLong(ppc_Buffer);

    if(pst_STR->ul_NbMaxLetters)
        pst_STR->dst_Letter = (STR_tdst_Letter *) MEM_p_Alloc(sizeof(STR_tdst_Letter) * pst_STR->ul_NbMaxLetters);
    else
        pst_STR->dst_Letter = NULL;

    if(pst_STR->dst_Letter == NULL)
    {
        pst_STR->ul_NbMaxLetters = 0;
    }
    else
    {
        pst_Letter = pst_STR->dst_Letter;
        pst_Last = pst_Letter + pst_STR->ul_NbMaxLetters;
        for(; pst_Letter < pst_Last; pst_Letter++)
            L_memset(pst_Letter, 0, sizeof(STR_tdst_Letter));
    }

    return pst_STR;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_SaveInBuffer(STR_tdst_Struct *_pst_STR, void *p_Unused)
{
#ifdef ACTIVE_EDITORS
    GRO_Struct_Save(&_pst_STR->st_Id);
    SAV_Buffer(&_pst_STR->ul_NbMaxLetters, 4);
#endif
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Reinit(GRO_tdst_Struct *pst_Gro)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Struct *pst_STR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = (STR_tdst_Struct *) pst_Gro;
    L_memset(pst_STR->dst_String, 0, sizeof(STR_tdst_String) * pst_STR->uc_NbMaxStrings);

    pst_STR->uc_NbCurEffects = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Reset( OBJ_tdst_GameObject *_pst_GO )
{
    STR_tdst_Struct			*pst_STR;
    
    STRDATA_Reset();

    pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return;

    STR_Reinit( (GRO_tdst_Struct *) pst_STR );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_InitGlobal( void )
{
	int j;

	/* global string */
    for (j = 0; j < STR_Cuc_NbGlobal; j++)
        STR_sgaac_Global[ j ][ 0 ] = 0;

#ifdef PSX2_TARGET
	if (!ps2MAI_gb_VideoModeNTSC)
	{
		STR_sgaac_Global[ 0 ][ 0 ] = ' ';
		STR_sgaac_Global[ 0 ][ 1 ] = 0;
		STR_sgaac_Global[ 1 ][ 0 ] = 'm';
		STR_sgaac_Global[ 1 ][ 1 ] = 'a';
		STR_sgaac_Global[ 1 ][ 2 ] = 'n';
		STR_sgaac_Global[ 1 ][ 3 ] = 'd';
		STR_sgaac_Global[ 1 ][ 4 ] = 'o';
		STR_sgaac_Global[ 1 ][ 5 ] = 0;
	}
	else
	{
		STR_sgaac_Global[ 1 ][ 0 ] = 'c';
		STR_sgaac_Global[ 1 ][ 1 ] = 'o';
		STR_sgaac_Global[ 1 ][ 2 ] = 'n';
		STR_sgaac_Global[ 1 ][ 3 ] = 't';
		STR_sgaac_Global[ 1 ][ 4 ] = 'r';
		STR_sgaac_Global[ 1 ][ 5 ] = 'o';
		STR_sgaac_Global[ 1 ][ 6 ] = 'l';
		STR_sgaac_Global[ 1 ][ 7 ] = 0;
	}
	if(WOR_gi_CurrentConsole == 2)
	{
		STR_sgaac_Global[ 2 ][ 0 ] = 'b';
		STR_sgaac_Global[ 2 ][ 1 ] = 'o';
		STR_sgaac_Global[ 2 ][ 2 ] = 'u';
		STR_sgaac_Global[ 2 ][ 3 ] = 't';
		STR_sgaac_Global[ 2 ][ 4 ] = 'o';
		STR_sgaac_Global[ 2 ][ 5 ] = 'n';
		STR_sgaac_Global[ 2 ][ 6 ] = 0;
	}
	else
	{
		STR_sgaac_Global[ 2 ][ 0 ] = 't';
		STR_sgaac_Global[ 2 ][ 1 ] = 'o';
		STR_sgaac_Global[ 2 ][ 2 ] = 'u';
		STR_sgaac_Global[ 2 ][ 3 ] = 'c';
		STR_sgaac_Global[ 2 ][ 4 ] = 'h';
		STR_sgaac_Global[ 2 ][ 5 ] = 'e';
		STR_sgaac_Global[ 2 ][ 6 ] = 0;
	}
#endif /*PSX2_TARGET*/
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Init(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GRO_tdst_Interface                  *i;
	int									j;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    STRDATA_Init();

	/* Convert ascii table. Thanks bill */
	for(j = 0; j < 256; j++)
		STR_sgac_CharTable[j] = j;
	/*
	STR_sgac_CharTable[161] = 168;
	STR_sgac_CharTable[224] = 133;
	STR_sgac_CharTable[225] = 160;
	STR_sgac_CharTable[226] = 131;
	STR_sgac_CharTable[228] = 132;
	STR_sgac_CharTable[231] = 135;
	STR_sgac_CharTable[232] = 138;
	STR_sgac_CharTable[233] = 130;
	STR_sgac_CharTable[234] = 136;
	STR_sgac_CharTable[235] = 137;
	STR_sgac_CharTable[236] = 141;
	STR_sgac_CharTable[237] = 161;
	STR_sgac_CharTable[238] = 140;
	STR_sgac_CharTable[239] = 139;
	STR_sgac_CharTable[241] = 164;
	STR_sgac_CharTable[242] = 149;
	STR_sgac_CharTable[243] = 162;
	STR_sgac_CharTable[244] = 147;
	STR_sgac_CharTable[245] = 167;
	STR_sgac_CharTable[246] = 148;
	STR_sgac_CharTable[249] = 151;
	STR_sgac_CharTable[250] = 163;
	STR_sgac_CharTable[251] = 150;
	STR_sgac_CharTable[252] = 129;
	*/

    /* Init string object interface */
    i = &GRO_gast_Interface[GRO_2DText];
#if defined (__cplusplus)
	i->pfnp_CreateFromBuffer = (void *(__cdecl *)(GRO_tdst_Struct *,char ** ,void *))STR_p_CreateFromBuffer;
    i->pfn_Destroy = (void (__cdecl *)(void *))STR_Free;
    i->pfnl_HasSomethingToRender = (LONG (__cdecl *)(void *,void *))STR_l_HasSomethingToRender;
    i->pfn_Render = (void (__cdecl *)(void *))STR_Render;
#else
	i->pfnp_CreateFromBuffer = STR_p_CreateFromBuffer;
    i->pfn_Destroy = STR_Free;
    i->pfnl_HasSomethingToRender = STR_l_HasSomethingToRender;
    i->pfn_Render = STR_Render;
#endif
	i->pfn_Reinit = STR_Reinit;

#ifdef ACTIVE_EDITORS
#if defined (__cplusplus)
    i->pfnl_SaveInBuffer = (LONG (__cdecl *)(void *,void *))STR_l_SaveInBuffer;
    i->pfnl_PushSpecialMatrix = (LONG (__cdecl *)(void *))GRO_PushSpecialMatrixForProportionnal;
#else
	i->pfnl_SaveInBuffer = STR_l_SaveInBuffer;
    i->pfnl_PushSpecialMatrix = GRO_PushSpecialMatrixForProportionnal;
#endif
#endif

    STR_sgpst_GO = OBJ_GameObject_Create(OBJ_C_IdentityFlag_BaseObject | OBJ_C_IdentityFlag_Visu);
    STR_sgpst_GO->pst_Base->pst_Visu->ul_DrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_ReceiveDynSdw);

    STR_sgl_NumberOfFont = 0;
    STR_sgpst_Font = NULL;
	STR_sgl_MaxNumberOfFont = 30;
	STR_sgl_NumberOfFont = 0;
	STR_sgpst_Font = (STR_tdst_FontDesc *) MEM_p_Alloc(STR_sgl_MaxNumberOfFont * sizeof(STR_tdst_FontDesc));

	STR_InitGlobal();
	STRDATA_Init();
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if defined(_XENON_RENDER)
void STR_XenonShutdown(void)
{
	if (!STR_sgpst_GO)
		return;

	GEO_ClearXenonMesh(STR_sgpst_GO->pst_Base->pst_Visu,
		               (GEO_tdst_Object*)STR_sgpst_GO->pst_Base->pst_Visu->pst_Object);
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Close(void)
{
	int	i;
	
	STRDATA_Close();
	
	if(!STR_sgpst_GO) return;
    STR_sgpst_GO->pst_Base->pst_Visu->pst_Material = NULL;
    STR_sgpst_GO->pst_Base->pst_Visu->pst_Object =NULL;
    OBJ_GameObject_Remove(STR_sgpst_GO, 1);
    if(STR_sgpst_Font) 
	{
		for (i = 0; i < STR_sgl_NumberOfFont; i++)
		{
			if (STR_sgpst_Font[i].pst_Letter)
			{
				MEM_Free( STR_sgpst_Font[i].pst_Letter );
				STR_sgpst_Font[i].pst_Letter = NULL;
			}
		}
		MEM_Free(STR_sgpst_Font);
	}
	STR_sgl_NumberOfFont = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_ReinitAll(void)
{
	int	i;
	
	STRDATA_ReinitAll();	
	for (i = 0; i < STR_sgl_NumberOfFont; i++)
	{
		if (STR_sgpst_Font[i].pst_Letter)
		{
			MEM_Free( STR_sgpst_Font[i].pst_Letter );
			STR_sgpst_Font[i].pst_Letter = NULL;
		}
	}
    STR_sgl_NumberOfFont = 0;
}

/*$4
 ***********************************************************************************************************************
    global string
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_GlobalSet( int _i_Index, char *_sz_Text )
{
    int i;

    if ( ((UCHAR) _i_Index) >= STR_Cuc_NbGlobal ) return;

    for (i = 0; i < STR_Cuc_GlobalLength - 1; i++)
    {
        if (*_sz_Text == 0) break;
        STR_sgaac_Global[ _i_Index ][ i ] = *_sz_Text++;
    }
    STR_sgaac_Global[ _i_Index ][ i ] = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_GlobalSetChar( int _i_Index, int _i_Char, int _i_Value )
{
    if ( ((UCHAR) _i_Index) >= STR_Cuc_NbGlobal ) return;
    
    STR_sgaac_Global[ _i_Index ][ STR_Cuc_GlobalLength - 1 ] = 0;
    if ( ((UCHAR) _i_Char) >= STR_Cuc_GlobalLength - 1 ) return;

    STR_sgaac_Global[ _i_Index ][ _i_Char ] = (CHAR) _i_Value;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_i_GlobalLength( int _i_Index )
{
    if ( ((UCHAR) _i_Index) >= STR_Cuc_NbGlobal ) return 0;
    return L_strlen( STR_sgaac_Global[ _i_Index ] );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *STR_sz_GlobalGet( int _i_Index )
{
    if ( ((UCHAR) _i_Index) >= STR_Cuc_NbGlobal ) return NULL;
    return STR_sgaac_Global[ _i_Index ];
}



/*$4
 ***********************************************************************************************************************
    sub string
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetSubStringWithLine(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                     i_Cur;
	STR_tdst_Letter			*pst_L;
	STR_tdst_String			*pst_String;
	STR_tdst_Struct			*pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return;
	if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL) return;
    
    pst_L = pst_STR->dst_Letter + pst_String->uw_First;

    i_Cur = 0;
    while(f > 0)
	{
		if(pst_L->ul_Flags & STR_Cul_LF_NewLine) f--;
		pst_L++;
        i_Cur++;
		if(i_Cur >= pst_String->uw_Number) return;
	}

    pst_String->uw_SubFirst = i_Cur;

    while (n > 0 )
    {
		if(pst_L->ul_Flags & STR_Cul_LF_NewLine) n--;
		pst_L++;
        i_Cur++;
		if(i_Cur >= pst_String->uw_Number) break;
    }
    pst_String->uw_SubNumber = i_Cur - pst_String->uw_SubFirst;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_i_CountLine( OBJ_tdst_GameObject *_pst_GO, int i, int f, int n)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                     i_Count;
	STR_tdst_Letter			*pst_L, *pst_Last;
	STR_tdst_String			*pst_String;
	STR_tdst_Struct			*pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return 0;
	if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL) return 0;
    
    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);

    pst_L = pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_L + n;
    for ( i_Count = 0; pst_L < pst_Last; pst_L++)
    {
        if(pst_L->ul_Flags & STR_Cul_LF_NewLine) i_Count++;
    }
    return i_Count+1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetSubStringWithMark(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                     i_Cur;
	STR_tdst_Letter			*pst_L;
	STR_tdst_String			*pst_String;
	STR_tdst_Struct			*pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return;
	if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL) return;
    
    pst_L = pst_STR->dst_Letter + pst_String->uw_First;

    i_Cur = 0;
    while(f > 0)
	{
		if( ((pst_L->ul_Flags & STR_Cul_LF_MarkMask) >> STR_Cul_LF_MarkShift) == 15) f--;
        if (!f) 
            pst_String->uw_SubFirst = i_Cur;
		pst_L++;
        i_Cur++;
		if(i_Cur >= pst_String->uw_Number) return;
	}

    while (n > 0 )
    {
		if( ((pst_L->ul_Flags & STR_Cul_LF_MarkMask) >> STR_Cul_LF_MarkShift) == 15) n--;
		pst_L++;
        i_Cur++;
		if(i_Cur >= pst_String->uw_Number) break;
    }
    pst_String->uw_SubNumber = i_Cur - pst_String->uw_SubFirst;
    if ( (i_Cur < pst_String->uw_Number) && (pst_String->uw_SubNumber > 0) )
        pst_String->uw_SubNumber--;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_i_CountMark( OBJ_tdst_GameObject *_pst_GO, int i, int f, int n, int m)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                     i_Count;
	STR_tdst_Letter			*pst_L, *pst_Last;
	STR_tdst_String			*pst_String;
	STR_tdst_Struct			*pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return 0;
	if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL) return 0;

    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);

    pst_L = pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_L + n;
   
    for ( i_Count = 0; pst_L < pst_Last; pst_L++)
    {
        if( ((pst_L->ul_Flags & STR_Cul_LF_MarkMask) >> STR_Cul_LF_MarkShift) == (ULONG) m) i_Count++;
    }
    return i_Count+1;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetSubString(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_String			*pst_String;
	STR_tdst_Struct			*pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return;
	if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL) return;

    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    if(f >= (int) pst_String->uw_Number) return;
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);
    
    pst_String->uw_SubFirst = f;
    pst_String->uw_SubNumber = n;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_GetSubLength(STR_tdst_Struct *_pst_STR, ULONG i, int _i_Sub)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String;
    STR_tdst_Letter *pst_L, *pst_Last;
    LONG            l_Length, l_LengthMax;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_String = STR_pst_GetString(_pst_STR, i);
    if(pst_String == NULL) return 0;

    pst_L = _pst_STR->dst_Letter + pst_String->uw_First;
    pst_Last = pst_L + pst_String->uw_Number;

    if(_i_Sub < 0)
    {
        l_LengthMax = l_Length = 0;
        for(; pst_L < pst_Last; pst_L++)
        {
            l_Length++;

            if(pst_L->ul_Flags & STR_Cul_LF_NewLine)
            {
                if(l_Length > l_LengthMax) l_LengthMax = l_Length;
                l_Length = 0;
            }
        }

        if(l_Length > l_LengthMax) l_LengthMax = l_Length;
        return l_LengthMax;
    }

    while(_i_Sub != 0)
    {
        if(pst_L->ul_Flags & STR_Cul_LF_NewLine)
            _i_Sub--;
        pst_L++;
        if(pst_L >= pst_Last) return 0;
    }

    l_Length = 0;
    while(pst_L < pst_Last)
    {
        l_Length++;
        if(pst_L->ul_Flags & STR_Cul_LF_NewLine)
            break;
        pst_L++;
    }

    return l_Length;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_ChangeStringLength(STR_tdst_Struct *_pst_STR, ULONG i, ULONG _ul_NewLength)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String, *pst_Next, *pst_Last;
    STR_tdst_Letter *pst_LOld, *pst_LNew;
    LONG            l_Number, l_MaxLetters;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_String = STR_pst_GetString(_pst_STR, i)) == NULL)
        return;
    if(_ul_NewLength == pst_String->uw_Number)
        return;
    if(pst_String->uw_First + _ul_NewLength > _pst_STR->ul_NbMaxLetters)
	{
		ERR_X_Warning(0, "A text is too long for max number of letters", NULL);
        _ul_NewLength = _pst_STR->ul_NbMaxLetters - pst_String->uw_First;
	}
    if(_ul_NewLength == pst_String->uw_Number)
        return;

    pst_Next = pst_String + 1;
    pst_Last = _pst_STR->dst_String + _pst_STR->uc_NbMaxStrings;

    l_MaxLetters = (pst_Last - 1)->uw_First + (pst_Last - 1)->uw_Number;

    if(pst_Next < pst_Last)
    {
        pst_LNew = _pst_STR->dst_Letter + pst_String->uw_First + _ul_NewLength;
        pst_LOld = _pst_STR->dst_Letter + pst_Next->uw_First;

        l_Number = l_MaxLetters - (pst_Next->uw_First);
        if ( pst_String->uw_First + _ul_NewLength + l_Number > _pst_STR->ul_NbMaxLetters )
            l_Number = _pst_STR->ul_NbMaxLetters - (pst_String->uw_First + _ul_NewLength);

        if(l_Number > 0)
            L_memmove(pst_LNew, pst_LOld, l_Number * sizeof(STR_tdst_Letter));

        if(pst_LNew > pst_LOld)
            L_memset( pst_LOld, 0, (char *) pst_LNew - (char *) pst_LOld );

        pst_String->uw_Number = (USHORT) _ul_NewLength;
        pst_String->uw_SubNumber = (USHORT) _ul_NewLength;
        while(pst_Next < pst_Last)
        {
            pst_Next->uw_First = (pst_Next - 1)->uw_First + (pst_Next - 1)->uw_Number;
            if(pst_Next->uw_First + pst_Next->uw_Number > (USHORT) _pst_STR->ul_NbMaxLetters)
            {
                pst_Next->uw_Number = (USHORT) _pst_STR->ul_NbMaxLetters - pst_Next->uw_First;
				pst_String->uw_SubNumber = pst_Next->uw_Number;
                pst_Next++;
                break;
            }

            pst_Next++;
        }

        while(pst_Next < pst_Last)
        {
            pst_Next->uw_First = (USHORT) _pst_STR->ul_NbMaxLetters;
            pst_Next->uw_Number = 0;
            pst_Next->uw_SubNumber = 0;
            pst_Next++;
        }
    }
    else
    {
        pst_String->uw_Number = (USHORT) _ul_NewLength;
        pst_String->uw_SubNumber = (USHORT) _ul_NewLength;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_CutLineToFitWithMaxWidth(STR_tdst_Letter *_pst_First, STR_tdst_Letter *_pst_Last, short _w_XStart, int _i_MaxWidth )
{
	STR_tdst_Letter *pst_L, *pst_StartLine, *pst_EndLine, *pst_Space;
	int				i_Max, i_Width, i_CurWidth, i_WidthSpace;
	int				i, i_NbLine, i_BestNbLine, i_LineWidth[ 32 ];
	int				i_Test, i_BestTest, i_TestW[ 8 ];
	float			f_BestVar, f_Moy, f_Var, f_Val;

	if ( !_i_MaxWidth ) return;

	pst_L = _pst_First;
	pst_StartLine = pst_L;

	while ( pst_L < _pst_Last )
	{
		pst_StartLine = pst_L;
		i_Width = pst_StartLine->x;
		while ( (pst_L < _pst_Last ) && !(pst_L->ul_Flags & STR_Cul_LF_NewLine) )
		{
			i_Width += pst_L->w;
			pst_L++;
		}
		pst_EndLine = pst_L;
		if (i_Width <= _i_MaxWidth )
		{
			pst_L++;
			continue;
		}

		i_TestW[ 0 ] = _i_MaxWidth;
		for ( i_Test = 0; i_Test < 8; i_Test++)
		{
			pst_L = pst_StartLine;
			pst_Space = NULL;
			i_NbLine = 0;
			i_CurWidth = _w_XStart;
			i_Max = i_TestW[ i_Test ];
			while ( pst_L <= pst_EndLine )
			{
				i_CurWidth += pst_L->w;
				if ( (pst_L->ul_Flags & STR_Cul_LF_ValueMask) == 32 )
				{
					pst_Space = pst_L;
					i_WidthSpace = i_CurWidth;
				}
				if ( (i_CurWidth > i_Max) && pst_Space)
				{
					i_LineWidth[ i_NbLine ] = i_WidthSpace;
					i_NbLine++;
					i_CurWidth = _w_XStart;
					pst_L = pst_Space + 1;
					pst_Space = NULL;
				}
				else
					pst_L++;
			}
			i_LineWidth[ i_NbLine ] = i_CurWidth;
			i_NbLine++;

			if ( !i_Test )
			{
				i_BestNbLine = i_NbLine;
				f_BestVar = Cf_Infinit;
				f_Moy = (float) i_Width / (float) i_NbLine;
				i_TestW[ 7 ] = (int) f_Moy;
				f_Val = (float) (i_TestW[ 0 ] - i_TestW[ 7 ]);
				f_Val /= 7;
				for (i = 1; i < 7; i++)
					i_TestW[ i ] = i_Max - (int)(f_Val * i);
			}
			else 
			{
				if (i_NbLine != i_BestNbLine )
					continue;
			}

			f_Var = 0;
			for (i = 0; i < i_NbLine; i++)
			{
				f_Val = i_LineWidth[ i ] - f_Moy;
				f_Var += f_Val * f_Val;
			}
			
			if ( f_Var < f_BestVar )
			{
				f_BestVar = f_Var;
				i_BestTest = i_Test;
			}
		}

		pst_L = pst_StartLine;
		pst_Space = NULL;
		i_CurWidth = _w_XStart;
		i_Max = i_TestW[ i_BestTest ];
		while ( pst_L <= pst_EndLine )
		{
			if ( (pst_L->ul_Flags & STR_Cul_LF_ValueMask) == 32 )
				pst_Space = pst_L;
			i_CurWidth += pst_L->w;
			if ( (i_CurWidth > i_Max) && pst_Space)
			{
				pst_Space->ul_Flags |= STR_Cul_LF_NewLine;
				i_CurWidth = _w_XStart;
				pst_L = pst_Space + 1;
				pst_Space = NULL;
			}
			pst_L++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_RecomputePos(STR_tdst_Struct *_pst_STR, STR_tdst_String *_pst_String, int f, int n)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter *pst_First, *pst_L, *pst_Last, *pst_Space;
    short           XSTART, YSTART, X, Y, YMAX;
	BOOL			b_Retrait;
	int				i_MaxWidth, i_DoCutLine;
	static int		si_CutLine = 0;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_L = pst_First = _pst_STR->dst_Letter + _pst_String->uw_First + f;
    if(n <= 0)
        pst_Last = _pst_STR->dst_Letter + _pst_String->uw_First + _pst_String->uw_Number;
    else
        pst_Last = pst_L + n;

    if ( (f == 0) && (pst_L->ul_Flags & STR_Cul_LF_Retrait) && (pst_L + 1 < pst_Last ) )
    {
		b_Retrait = 1;
		pst_L++;
    }
    else
		b_Retrait = 0;

	X = XSTART = pst_L->x;
    Y = YSTART = YMAX = pst_L->y;
	i_MaxWidth = (int) (_pst_STR->f_MaxWidth * _pst_STR->uw_SW);
	//pst_Space = NULL;
	i_DoCutLine = 0;

    if( _pst_String->uw_Flags & STR_Cuw_SF_Proportionnal )
    {
		for(; pst_L < pst_Last; pst_L++)
        {
			/*
			if ( (pst_L->ul_Flags & STR_Cul_LF_ValueMask) == 32 )
				pst_Space = pst_L;
			*/
			
            STR_M_SetFont(_pst_STR, ((pst_L->ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift));
            if ( !_pst_STR->pst_Font ) continue;

            pst_L->x = X;
            pst_L->y = Y;
        
            X += pst_L->w;
			X += _pst_STR->aw_LetterSpacing[ _pst_STR->w_FontPage ];
			if (Y + pst_L->h > YMAX) YMAX = Y + pst_L->h;

			if (i_MaxWidth && (X > i_MaxWidth) )
			{
				i_DoCutLine = 1;
				/*
				if (pst_Space)
					pst_L = pst_Space;
				pst_L->ul_Flags |= STR_Cul_LF_NewLine;
				*/
			}
            
            if(pst_L->ul_Flags & STR_Cul_LF_NewLine)
            {
				pst_Space = NULL;
                X = XSTART;
                Y = YMAX;
				if ((_pst_STR->w_FontPage >= 0 ) && (_pst_STR->w_FontPage < 24 ))
					Y += _pst_STR->aw_Interligne[ _pst_STR->w_FontPage ];
            }
        }
    }
    else
    {
        for(; pst_L < pst_Last; pst_L++)
        {
            pst_L->x = X;
            pst_L->y = Y;
            
            X += pst_L->w;
            if (Y + pst_L->h > YMAX) YMAX = Y + pst_L->h;

            if(pst_L->ul_Flags & STR_Cul_LF_NewLine)
            {
                X = XSTART;
                Y = YMAX;
            }
        }
    }

	if ( b_Retrait )
	{
		pst_L = _pst_STR->dst_Letter + _pst_String->uw_First + f;
		pst_L->x = XSTART - 4 - pst_L->w;
		pst_L->y = YSTART;
		if (YSTART + pst_L->h > YMAX )
			pst_L->y -= ( (YSTART + pst_L->h) - YMAX ) >> 1;
	}

	if (i_DoCutLine && !si_CutLine )
	{
		si_CutLine = 1;
		STR_CutLineToFitWithMaxWidth( pst_First, pst_Last, XSTART, i_MaxWidth );
		STR_RecomputePos( _pst_STR, _pst_String, f, n);
	}
	si_CutLine = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetString(OBJ_tdst_GameObject *_pst_GO, int i, char *_sz_String)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String;
    STR_tdst_Letter *pst_Letter;
    STR_tdst_Struct *pst_STR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_sz_String == NULL) return;

    pst_STR = STR_pst_Get(_pst_GO);
    if(!pst_STR) return;

    STR_M_SetMaterial(_pst_GO);
    STR_M_ResetFont(pst_STR);

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL)
        return;
	STR_EffectDelAllForAString(pst_STR, i);
    STR_ChangeStringLength(pst_STR, i, STR_l_GetStringLength(_sz_String));

    pst_Letter = pst_STR->dst_Letter + pst_String->uw_First;
    STR_ParseString( pst_STR, i, pst_Letter, (unsigned char *) _sz_String, pst_String->uw_Number, 0.05f, 0.05f, 0xFFFFFFFF, 0 );
    
	if ( (pst_STR->f_MaxWidth == 0) && !(pst_String->uw_Flags & STR_Cuw_SF_IndepentLetter) )
	    STR_RecomputePos(pst_STR, pst_String, 0, -1);

    pst_String->uw_SubFirst = 0;
    pst_String->uw_SubNumber = pst_String->uw_Number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetStringExt(OBJ_tdst_GameObject *_pst_GO, int i, int flags, char *_sz_String, MATH_tdst_Vector *_pst_Pivot )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String;
    STR_tdst_Letter *pst_Letter;
    STR_tdst_Struct *pst_STR;
	int				FlagsExt;
	MATH_tdst_Vector size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_sz_String == NULL) return;
	pst_STR = STR_pst_Get(_pst_GO);
    if(!pst_STR) return;

	FlagsExt = (flags >> 16) & 0xFFFF;
	flags &= 0xFFFF;

    STR_M_SetMaterial(_pst_GO);
    STR_M_ResetFont(pst_STR);
    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL)
        return;

	if ( FlagsExt & STR_Cul_SFExt_SetText )
	{
		STR_ChangeStringLength(pst_STR, i, STR_l_GetStringLength(_sz_String));

		pst_Letter = pst_STR->dst_Letter + pst_String->uw_First;
		STR_ParseString( pst_STR, i, pst_Letter, (unsigned char *) _sz_String, pst_String->uw_Number, 0.05f, 0.05f, 0xFFFFFFFF, 0 );
		pst_String->uw_SubFirst = 0;
		pst_String->uw_SubNumber = pst_String->uw_Number;
	}

	if ( FlagsExt & STR_Cul_SFExt_SetFlags )
		pst_String->uw_Flags = (USHORT) flags;

	if ( FlagsExt & STR_Cul_SFExt_SetPivot )
		MATH_CopyVector(&pst_String->st_Pivot, _pst_Pivot);

	if ( FlagsExt & STR_Cul_SFExt_SetSize )
	{
		MATH_InitVector( &size, -1, 0, 0 );
		STR_SetSize( _pst_GO, i, 0, -1, &size );
	}

	if ( FlagsExt & STR_Cul_SFExt_SetAlign )
		STR_Align( _pst_GO, i, 0, -1, (FlagsExt >> STR_Cul_SFExt_AlignShift) & STR_Cul_SFExt_AlignMask );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_InsertString(OBJ_tdst_GameObject *_pst_GO, int i, int _i_From, char *_sz_String)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
	STR_tdst_Struct *pst_STR(NULL);
    STR_tdst_String *pst_String(NULL);
    LONG            l_Length(0);
    STR_tdst_Letter *pst_Letter(NULL), *pst_Next(NULL);
    ULONG           ul_OldNumber(0), ul_Color(0);
    float           w(0), h(0);
#else
	STR_tdst_Struct *pst_STR;
    STR_tdst_String *pst_String;
    LONG            l_Length;
    STR_tdst_Letter *pst_Letter, *pst_Next;
    ULONG           ul_OldNumber, ul_Color;
    float           w, h;
#endif 
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_sz_String == NULL) return;

    pst_STR = STR_pst_Get(_pst_GO);
    if(!pst_STR) return;
    
    STR_M_SetMaterial(_pst_GO);
    STR_M_ResetFont(pst_STR);

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL)
        return;

    l_Length = STR_l_GetStringLength(_sz_String);
    if(l_Length == 0) return;

    _i_From = STR_l_GetValidIndex(pst_STR, pst_String, _i_From);

    ul_OldNumber = pst_String->uw_Number;
    STR_ChangeStringLength(pst_STR, i, pst_String->uw_Number + l_Length);

    pst_Letter = pst_STR->dst_Letter + pst_String->uw_First + _i_From;
    if ( (_i_From != (int) ul_OldNumber) && (pst_String->uw_Number > l_Length) )
    {
        ul_Color = pst_Letter->ul_Color;
        pst_Next = pst_Letter + l_Length;
        L_memmove(pst_Next, pst_Letter, (pst_String->uw_Number - _i_From - l_Length) * sizeof(STR_tdst_Letter));
    }
    else if(_i_From != 0)
    {
        ul_Color = (pst_Letter - 1)->ul_Color;
    }
    else
    {
        w = 0.05f;
        h = 0.05f;
        ul_Color = 0xFFFFFFFF;
    }

    STR_ParseString( pst_STR, i, pst_Letter, (unsigned char *) _sz_String, l_Length, w, h, ul_Color, _i_From );
    if ( !(pst_String->uw_Flags & STR_Cuw_SF_IndepentLetter) )
        STR_RecomputePos(pst_STR, pst_String, 0, -1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetFloat(OBJ_tdst_GameObject *_pst_GO, int i, float _f_Value, int _i_Dec )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char            sz_Format[5] = "%.0f", sz_Value[32];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sz_Format[2] = '0' + _i_Dec;
    sprintf( sz_Value, sz_Format, _f_Value );
    STR_SetString( _pst_GO, i, sz_Value );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_InsertFloat(OBJ_tdst_GameObject *_pst_GO, int i, int f, float _f_Value, int _i_Dec )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char            sz_Format[5] = "%.0f", sz_Value[32];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sz_Format[2] = '0' + _i_Dec;
    sprintf( sz_Value, sz_Format, _f_Value );
    STR_InsertString( _pst_GO, i, f, sz_Value );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_InsertInt(OBJ_tdst_GameObject *_pst_GO, int i, int f, int _i_Value )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char            sz_Value[32];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sprintf( sz_Value, "%d", _i_Value );
    STR_InsertString( _pst_GO, i, f, sz_Value );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_DelChar(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter *pst_Letter, *pst_Last;
    STR_tdst_String *pst_String;
    STR_tdst_Struct *pst_STR;
	int				mark;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GO);
    if(!pst_STR) return;

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL)
        return;

	// store the mark 
	mark = (n <= -10) ? (-10 - n) : -1;


    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    if(f >= (int) pst_String->uw_Number) return;
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);

	STR_EffectCheck( pst_STR, i, f, n );

    if(f + n != (int) pst_String->uw_Number)
    {
		ERR_X_Assert(f + n < pst_String->uw_Number);
        pst_Letter = pst_STR->dst_Letter + pst_String->uw_First + f;
        pst_Last = pst_Letter + n;

        L_memmove(pst_Letter, pst_Last, (pst_String->uw_Number - f - n) * sizeof(STR_tdst_Letter));

		if (mark != -1)
			pst_Letter->ul_Flags &= ~((mark << STR_Cul_LF_MarkShift) & STR_Cul_LF_MarkMask);
    }

    STR_ChangeStringLength(pst_STR, i, pst_String->uw_Number - n);


    STR_M_SetMaterial(_pst_GO);
    STR_M_ResetFont(pst_STR);
	if ( !(pst_String->uw_Flags & STR_Cuw_SF_IndepentLetter) )
		STR_RecomputePos(pst_STR, pst_String, 0, -1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_ToString( OBJ_tdst_GameObject *_pst_GO, int i, int f, int n, char *sz_Text )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter *pst_Letter, *pst_Last;
    STR_tdst_String *pst_String;
    STR_tdst_Struct *pst_STR;
	int				page;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GO);
    if(!pst_STR) return;


    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL)
        return;

    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);

    pst_Letter = pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_Letter + n;

    for(; pst_Letter < pst_Last; pst_Letter++)
	{
		if (pst_Letter->ul_Flags & STR_Cul_LF_PageMask )
		{
			page = (pst_Letter->ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift;
			*sz_Text++ = '\\';
			*sz_Text++ = 'p';
			if (page >= 10)
			{
				*sz_Text++ = (page / 10) + '0';
				page = page % 10;
			}
			*sz_Text++ = page + '0';
			*sz_Text++ = '\\';
		}
        *sz_Text++ = (char) (pst_Letter->ul_Flags & STR_Cul_LF_ValueMask);
	}
	*sz_Text++ = 0;
}


/*$4
 ***********************************************************************************************************************
    Letter into geometry (change UV of a geometry to fit with font texture and a string)
 ***********************************************************************************************************************
 */
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_Init3DStringExtExt( OBJ_tdst_GameObject *_pst_GO, char *_sz_String, int _i_Max, char _c_Center )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_FontLetterDesc     *pst_FontLetter;
    LONG                        i, j, k, l_Length, l_Sprite;
    STR_tdst_Letter             st_Letter[256];
    STR_tdst_Letter             *pst_Letter;
    STR_tdst_Struct             st_STR;
    GEO_tdst_Object             *pst_Geo;
    GEO_tdst_UV                 *pst_UV, *pst_LastUV;
	static char                 ac_UV[4][2] = { {0,1}, {0,0}, {1,1}, {1,0} };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Geo = (GEO_tdst_Object *) OBJ_p_GetGro( _pst_GO );
    if ( (!pst_Geo) || (pst_Geo->st_Id.i->ul_Type != GRO_Geometric)) return;

    l_Length = STR_l_GetStringLength(_sz_String);
	l_Sprite = pst_Geo->l_NbUVs >> 2;

/*
#ifdef _GAMECUBE
	if(pst_Geo->dst_Element[ 0 ].dl)
	{
		u16     	p1, p2, p3;
		u8		*Stream,*StreamLast;
		u32 		Pitch ;
		l_Sprite = 0;		
		Stream = pst_Geo->dst_Element[ 0 ].dl;
		StreamLast = Stream + (pst_Geo->dst_Element[ 0 ].dl_size & 0x0fffffff);
		Pitch = 8;
		if (pst_Geo->dst_Element[ 0 ].dl_size & 0x80000000) {Pitch -= 2;};
		if (pst_Geo->dst_Element[ 0 ].dl_size & 0x40000000) Pitch -= 1;
		if (pst_Geo->dst_Element[ 0 ].dl_size & 0x20000000) Pitch -= 1;
		while (Stream < StreamLast)
		{
			{
				u16 lNbVertex;
				u8		*StreamStripLast;
				Stream++; // Flags
				lNbVertex = *(u16 *)Stream;	// Number Of Vertexes
				Stream += 2 + lNbVertex * Pitch; 				// Number Of Vertexes
			    l_Sprite += (lNbVertex - 2);
			}
			while ((*Stream == 0) && (Stream < StreamLast)) Stream++; // Skip DX_NOP
		}
		l_Sprite >>= 1;
	}
	else
		l_Sprite = pst_Geo->dst_Element[ 0 ].l_NbTriangles >> 1;
#elif defined(PSX2_TARGET)
	if (pst_Geo->dst_Element[ 0 ].pst_StripDataPS2)
	{
		USHORT	*p0,*pL;
		l_Sprite = 0;
		p0 = pst_Geo->dst_Element[ 0 ].pst_StripDataPS2->pVertexIndexes;
		pL = p0 + pst_Geo->dst_Element[ 0 ].pst_StripDataPS2->ulNumberOfAtoms;
		while (p0 < pL) l_Sprite += (*(p0++) >> 15) ^ 1;
		l_Sprite >>= 1;
	} else
		l_Sprite = pst_Geo->dst_Element[ 0 ].l_NbTriangles >> 1;
#else
		l_Sprite = pst_Geo->dst_Element[ 0 ].l_NbTriangles >> 1;
#endif
	l_Sprite = lMin(pst_Geo->l_NbUVs >> 2,l_Sprite);
*/
 
    j = (l_Length < l_Sprite) ? l_Length : l_Sprite;
    j = ((_i_Max < 0) || (j <= _i_Max)) ? j : _i_Max;

    l_Length = STR_l_GetStringLength(_sz_String);
    st_STR.uw_SH = (unsigned short) GDI_gpst_CurDD->st_Camera.f_Width;
    st_STR.uw_SW = (unsigned short) GDI_gpst_CurDD->st_Camera.f_Height;
    STR_ParseString( &st_STR, 0, st_Letter, (unsigned char *) _sz_String, l_Length, 0.05f, 0.05f, 0xFFFFFFFF, 0 );

    STR_M_SetMaterial(_pst_GO);
    STR_M_ResetFont( (&st_STR) );
    STR_M_SetFont( (&st_STR), 0);
    if ( !st_STR.pst_Font ) return;
    
    pst_Letter = st_Letter;
	pst_UV = pst_Geo->dst_UV;
	pst_LastUV = pst_UV + (l_Sprite << 2); 

    pst_Geo->dst_Element->l_MaterialId = ((pst_Letter->ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift);
    st_STR.w_FontPage = -1;

    for ( i = 0; i < j; i++, pst_Letter++)
    {
        STR_M_SetFont( (&st_STR), ((pst_Letter->ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift));
        pst_FontLetter = st_STR.pst_Font->pst_Letter + STR_M_A2I(pst_Letter->ul_Flags);

		for (k = 0; k < 4; k++)
		{
			pst_UV->fU = pst_FontLetter->f_U[ac_UV[k][0]];
			pst_UV->fV = pst_FontLetter->f_V[ac_UV[k][1]];
			pst_UV++;
		}
    }
    while (pst_UV < pst_LastUV)
    {
		pst_UV->fU = 0;
        pst_UV->fV = 0;
        pst_UV++;
    }
#ifdef PSX2_TARGET	
	GSP_FreeABackCache(pst_Geo->dst_Element->p_ElementCache);
#endif	

#if defined(_XBOX)
	if ( pst_Geo->pIB )
		Gx8_ReleaseVBForObject( pst_Geo );
#endif // defined(_XBOX)

#if defined(_PC_RETAIL)
	if ( pst_Geo->pIB )
		Dx9_ReleaseVBForObject( pst_Geo );
#endif // defined(_PC_RETAIL)

}

/**/
void STR_Init3DStringCenter( OBJ_tdst_GameObject *_pst_GO, char *_sz_String, int _i_Max )
{
    STR_Init3DStringExtExt( _pst_GO, _sz_String, _i_Max, 1);
}
/**/
void STR_Init3DStringExt( OBJ_tdst_GameObject *_pst_GO, char *_sz_String, int _i_Max )
{
    STR_Init3DStringExtExt( _pst_GO, _sz_String, _i_Max, 0);
}
/**/
void STR_Init3DString( OBJ_tdst_GameObject *_pst_GO, char *_sz_String )
{
    STR_Init3DStringExtExt( _pst_GO, _sz_String, -1, 0 );
}

/**/
void STR_Init3DStringFromSTR( OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_GOSTR, int _i_Str, int _i_Max, int _i_Center )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter *pst_Letter, *pst_Last;
    STR_tdst_String *pst_String;
    STR_tdst_Struct *pst_STR;
    char            sz_Text[ 256 ];
    int             i, f, n;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GOSTR);
    if(!pst_STR) return;

    if((pst_String = STR_pst_GetString(pst_STR, _i_Str)) == NULL)
        return;

    f = STR_l_GetValidIndex(pst_STR, pst_String, 0);
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, -1);

    pst_Letter = pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_Letter + n;

    for( i = 0; pst_Letter < pst_Last; i++, pst_Letter++)
        sz_Text[i] = (char) (pst_Letter->ul_Flags & STR_Cul_LF_ValueMask);
    sz_Text[i] = 0;

    STR_Init3DStringExtExt( _pst_GO, sz_Text, _i_Max, (char) _i_Center );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_3DStringList_Clear( void )
{
    STR_3DString_Number = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_3DStringList_Reinit( void )
{
    int i;
    OBJ_tdst_GameObject *o;

    for (i = 0; i < STR_3DString_Number; i++)
    {
        o = STR_3DString_Obj[ i ];
        if (o->pst_Extended && o->pst_Extended->pst_Ai )
            AI_ReinitInstance( o->pst_Extended->pst_Ai );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_3DStringList_Add( struct OBJ_tdst_GameObject_ *p_Obj )
{
    int i;

    if (STR_3DString_Number == STR_C_3DString_Max) return;
    for (i =0; i < STR_3DString_Number; i++)
        if ( STR_3DString_Obj[ i ] == p_Obj ) return;
    STR_3DString_Obj[ STR_3DString_Number++ ] = p_Obj;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
