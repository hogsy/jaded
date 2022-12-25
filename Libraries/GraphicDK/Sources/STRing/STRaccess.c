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
#include "GDInterface/GDInterface.h"
#include "STRing/STRstruct.h"
//#include "SPriteList/SPLstruct.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "TEXture/TEXfile.h"
#include "GFX/GFX.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "INOut/INO.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"

#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#endif
#include "TIMer/TIMdefs.h"

#ifdef PSX2_TARGET
// mamagouille
#include "PSX2debug.h"
#endif

extern int                          TEXT_i_GetLang(void);

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
LONG STR_l_GetLength(STR_tdst_Struct *_pst_STR, ULONG i)
{
    if(i >= _pst_STR->uc_NbMaxStrings) return -1;
    return (LONG) _pst_STR->dst_String[i].uw_Number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_GetMarkIndex(STR_tdst_Struct *_pst_STR, STR_tdst_String *_pst_String, int _i_Mark)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG            l_Index;
    STR_tdst_Letter *pst_L;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_L = _pst_STR->dst_Letter + _pst_String->uw_First;

    for(l_Index = 0; l_Index < (LONG) _pst_String->uw_Number; l_Index++, pst_L++)
    {
        if(((pst_L->ul_Flags & STR_Cul_LF_MarkMask) >> STR_Cul_LF_MarkShift) == (ULONG) _i_Mark)
            break;
    }

    return l_Index;
}
   
/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_GetValidIndex(STR_tdst_Struct *_pst_STR, STR_tdst_String *_pst_String, int f)
{
    if(f <= -50) f = STR_l_GetMarkIndex(_pst_STR, _pst_String, (-50 - f) & 0xF);
    if(f <= -10) f = STR_l_GetMarkIndex(_pst_STR, _pst_String, (-10 - f) & 0xF) + 1;
    if ( f == -2 ) f = _pst_String->uw_SubFirst;
    if((f < 0) || (f >= (int) _pst_String->uw_Number))
        f = _pst_String->uw_Number;
    return f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG STR_l_GetValidNumber(STR_tdst_Struct *_pst_STR, STR_tdst_String *_pst_String, int f, int n)
{
    if(n <= -10)
    {
        n = STR_l_GetMarkIndex(_pst_STR, _pst_String, (-10 - n) & 0xF);
        if(n < f)
            n = -1;
        else
            return(n - f);
    }
    if ( n == -2 ) 
        n = _pst_String->uw_SubFirst + _pst_String->uw_SubNumber - f;

    if((n <= 0) || (f + n > (int) _pst_String->uw_Number))
        n = _pst_String->uw_Number - f;
    return n;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
STR_tdst_Struct *STR_pst_Get(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Struct *pst_STR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_GO && (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu) )
    {
        pst_STR = (STR_tdst_Struct *) _pst_GO->pst_Base->pst_Visu->pst_Object;
        if(pst_STR)
        {
            if(pst_STR->st_Id.i->ul_Type == GRO_2DText)
                return pst_STR;
        }
    }

    return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
STR_tdst_String *STR_pst_GetString(STR_tdst_Struct *_pst_STR, int i)
{
    if ( !_pst_STR->dst_Letter ) return NULL;
    if (i < 0) 
        return NULL;
    if (i >= (int) _pst_STR->uc_NbMaxStrings)
        return NULL;
    return _pst_STR->dst_String + i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetPivot(STR_tdst_Struct *_pst_STR, int i, MATH_tdst_Vector *v)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_String = STR_pst_GetString(_pst_STR, i);
    if(pst_String == NULL) return;
    MATH_CopyVector(&pst_String->st_Pivot, v);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Vector *STR_pst_GetPivot(STR_tdst_Struct *_pst_STR, int i)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_String = STR_pst_GetString(_pst_STR, i);
    if(pst_String == NULL) return NULL;
    return &pst_String->st_Pivot;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetFlag(STR_tdst_Struct *_pst_STR, int i, ULONG _l_Flags)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_String = STR_pst_GetString(_pst_STR, i);
    if(pst_String == NULL) return;
    pst_String->uw_Flags = (USHORT) _l_Flags;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG STR_ul_GetFlag(STR_tdst_Struct *_pst_STR, int i)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_String = STR_pst_GetString(_pst_STR, i);
    if(pst_String == NULL) return 0;
    return (ULONG) pst_String->uw_Flags;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetLetterFlag( OBJ_tdst_GameObject *_pst_GO, int i, int f, int n, int flags)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Letter			*pst_L, *pst_Last;
	STR_tdst_String			*pst_String;
	STR_tdst_Struct			*pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return;
	if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL) return;

    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);

    pst_L = pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_L + n;
   
    for ( ; pst_L < pst_Last; pst_L++)
    {
        pst_L->ul_Flags &= 0xFFFFFF;
        pst_L->ul_Flags |= flags << 24;
    }
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG STR_GetLetterFlag( OBJ_tdst_GameObject *_pst_GO, int i, int _i_Char )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Struct			*pst_STR;
    STR_tdst_String *pst_String;
    STR_tdst_Letter *pst_Letter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GO);
    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL)
        return 0;

    _i_Char = STR_l_GetValidIndex(pst_STR, pst_String, _i_Char);
    if(_i_Char >= (int) pst_String->uw_Number)
        return 0;

    pst_Letter = pst_STR->dst_Letter + pst_String->uw_First + _i_Char;

    return (pst_Letter->ul_Flags >> 24);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetChar(STR_tdst_Struct *_pst_STR, int i, int _i_Char, int c)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String;
    STR_tdst_Letter *pst_Letter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_String = STR_pst_GetString(_pst_STR, i)) == NULL)
        return;

    _i_Char = STR_l_GetValidIndex(_pst_STR, pst_String, _i_Char);
    if(_i_Char >= (int) pst_String->uw_Number)
        return;

    pst_Letter = _pst_STR->dst_Letter + pst_String->uw_First + _i_Char;

    pst_Letter->ul_Flags &= ~(STR_Cul_LF_ValueMask | STR_Cul_LF_PageMask);
    pst_Letter->ul_Flags += c & (STR_Cul_LF_ValueMask | STR_Cul_LF_PageMask);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_i_GetChar(STR_tdst_Struct *_pst_STR, int i, int _i_Char )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String *pst_String;
    STR_tdst_Letter *pst_Letter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_String = STR_pst_GetString(_pst_STR, i)) == NULL)
        return 0;

    _i_Char = STR_l_GetValidIndex(_pst_STR, pst_String, _i_Char);
    if(_i_Char >= (int) pst_String->uw_Number)
        return 0;

    pst_Letter = _pst_STR->dst_Letter + pst_String->uw_First + _i_Char;

    return (pst_Letter->ul_Flags & (STR_Cul_LF_ValueMask | STR_Cul_LF_PageMask) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetColor(STR_tdst_Struct *_pst_STR, int i, int f, int n, ULONG c)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter *pst_Letter, *pst_Last;
    STR_tdst_String *pst_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_String = STR_pst_GetString(_pst_STR, i)) == NULL)
        return;

    f = STR_l_GetValidIndex(_pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(_pst_STR, pst_String, f, n);

    pst_Letter = _pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_Letter + n;

    for(; pst_Letter < pst_Last; pst_Letter++)
        pst_Letter->ul_Color = c;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetAlpha(STR_tdst_Struct *_pst_STR, int i, int f, int n, ULONG c)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter *pst_Letter, *pst_Last;
    STR_tdst_String *pst_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_String = STR_pst_GetString(_pst_STR, i)) == NULL)
        return;

    f = STR_l_GetValidIndex(_pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(_pst_STR, pst_String, f, n);
    c <<= 24;

    pst_Letter = _pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_Letter + n;

    for(; pst_Letter < pst_Last; pst_Letter++)
    {
        pst_Letter->ul_Color &= 0x00FFFFFF;
        pst_Letter->ul_Color |= c;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetColorExt(STR_tdst_Struct *_pst_STR, int i, int f, int n, ULONG c, ULONG mask)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter *pst_Letter, *pst_Last;
    STR_tdst_String *pst_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_String = STR_pst_GetString(_pst_STR, i)) == NULL)
        return;

    f = STR_l_GetValidIndex(_pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(_pst_STR, pst_String, f, n);
    c &= mask;
    mask = ~mask;

    pst_Letter = _pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_Letter + n;

    for(; pst_Letter < pst_Last; pst_Letter++)
    {
        pst_Letter->ul_Color &= mask;
        pst_Letter->ul_Color |= c;
    }
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int STR_GetColor(STR_tdst_Struct *_pst_STR, int i, int f, int n)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter *pst_Letter, *pst_Last;
    STR_tdst_String *pst_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_String = STR_pst_GetString(_pst_STR, i)) == NULL)
        return 0;

    f = STR_l_GetValidIndex(_pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(_pst_STR, pst_String, f, n);

    pst_Letter = _pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_Letter + n;

	return pst_Letter->ul_Color;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_ChangePage(STR_tdst_Struct *_pst_STR, int i, int f, int n, int newP, int oldP )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter *pst_Letter, *pst_Last;
    STR_tdst_String *pst_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_String = STR_pst_GetString(_pst_STR, i)) == NULL)
        return;

    f = STR_l_GetValidIndex(_pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(_pst_STR, pst_String, f, n);

	pst_Letter = _pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_Letter + n;
    newP <<= STR_Cul_LF_PageShift;

    if ( oldP < 0 )
    {
        for(; pst_Letter < pst_Last; pst_Letter++)
        {
            pst_Letter->ul_Flags &= ~STR_Cul_LF_PageMask;
            pst_Letter->ul_Flags |= newP;
        }
    }
    else if (oldP < 256 )
    {
		oldP <<= STR_Cul_LF_PageShift;
        for(; pst_Letter < pst_Last; pst_Letter++)
        {
            if ( (pst_Letter->ul_Flags & STR_Cul_LF_PageMask) == (ULONG) oldP)
            {
                pst_Letter->ul_Flags &= ~STR_Cul_LF_PageMask;
        		pst_Letter->ul_Flags |= newP;
            }
        }
    }
    else if (oldP < 512)
    {
        oldP -= 256;
		oldP <<= STR_Cul_LF_PageShift;
        for(; pst_Letter < pst_Last; pst_Letter++)
        {
            if ( (pst_Letter->ul_Flags & STR_Cul_LF_PageMask) != (ULONG) oldP)
            {
                pst_Letter->ul_Flags &= ~STR_Cul_LF_PageMask;
                pst_Letter->ul_Flags |= newP;
            }
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetPos(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n, MATH_tdst_Vector *v)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter *pst_Letter;
    STR_tdst_String *pst_String;
    STR_tdst_Struct *pst_STR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GO);
    if(!pst_STR) return;

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL)
        return;

    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);
    if (n == 0) return;

    if (v->z == 0) v->z = STR_f_DefaultLetterDepth; 

	if (pst_String->uw_First + f < (int) pst_STR->ul_NbMaxLetters)
	{
		pst_Letter = pst_STR->dst_Letter + pst_String->uw_First + f;
		//pst_Letter->fX = STR_w_CompressFloat( v->x );
		//pst_Letter->fY = STR_w_CompressFloat( v->y );

        pst_Letter->x = (short) (v->x * pst_STR->uw_SW);
        pst_Letter->y = (short) (v->y * pst_STR->uw_SH);
	}

    if ( !(pst_String->uw_Flags & STR_Cuw_SF_IndepentLetter) )
    {
        STR_M_SetMaterial(_pst_GO);
        STR_M_ResetFont(pst_STR);
        STR_RecomputePos(pst_STR, pst_String, f, n);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetSize(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n, MATH_tdst_Vector *v)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter             *pst_Letter, *pst_Last;
    STR_tdst_String             *pst_String;
    STR_tdst_Struct             *pst_STR;
	STR_tdst_FontLetterDesc     *pst_FontLetter;
    float                       x;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GO);
    if(!pst_STR) return;

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL)
        return;

	STR_M_SetMaterial(_pst_GO);
    STR_M_ResetFont(pst_STR);

    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);

    pst_Letter = pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_Letter + n;

    if ( ( v->x < 0) || (v->y <= 0) )
	{
		for(; pst_Letter < pst_Last; pst_Letter++)
		{
			STR_M_SetFont(pst_STR, ((pst_Letter->ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift));

            if (pst_STR->pst_Font && (v->x < 0) )
            {
                pst_FontLetter = pst_STR->pst_Font->pst_Letter + STR_M_A2I(pst_Letter->ul_Flags);

                x = (float) fabs( pst_FontLetter->f_U[1] - pst_FontLetter->f_U[0] );
                x *= pst_STR->pst_Font->uw_Width;
				if (v->y) x *= v->y;
                pst_Letter->w = (short) x;

                x = (float) fabs( pst_FontLetter->f_V[1] - pst_FontLetter->f_V[0] );
                x *= pst_STR->pst_Font->uw_Height;
				if (v->y) x *= v->y;
                pst_Letter->h = (short) x;
            }
            else
            { 
                pst_Letter->w = (short) (v->x * pst_STR->uw_SW);
                if (pst_STR->pst_Font)
                {
                    pst_FontLetter = pst_STR->pst_Font->pst_Letter + STR_M_A2I(pst_Letter->ul_Flags);
                    x = v->x * ( ((float) fabs(pst_FontLetter->f_V[1] - pst_FontLetter->f_V[0])) / (float) fabs(pst_FontLetter->f_U[1] - pst_FontLetter->f_U[0]));
                    pst_Letter->h = (short) (x * pst_STR->uw_SH);
                }
                else
                {
                    pst_Letter->h = pst_Letter->w;
                }
            }
		}
	}
	else
	{
		for(; pst_Letter < pst_Last; pst_Letter++)
		{
            pst_Letter->w = (short) (v->x * pst_STR->uw_SW);
            pst_Letter->h = (short) (v->y * pst_STR->uw_SH);
		}
    }

    if ( !(pst_String->uw_Flags & STR_Cuw_SF_IndepentLetter) )
    {
        STR_RecomputePos(pst_STR, pst_String, f, -1);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_GetRect(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n, int command, MATH_tdst_Vector *V )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    MIN, MAX;
	MATH_tdst_Vector	min, max;
    STR_tdst_Struct		*pst_STR;
	float				dx, dy;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !V ) return;

	pst_STR = STR_pst_Get(_pst_GO);
	if( !pst_STR )
	{
		MATH_InitVectorToZero( V );
		return;
	}

	if ( command == 10 )
	{
		V->x = 1.0f / ((float) pst_STR->uw_SW);
		V->y = 1.0f / ((float) pst_STR->uw_SH);
		return;
	}
	if ( command == 11 )
	{
		V->x = ((float) pst_STR->uw_SW);
		V->y = ((float) pst_STR->uw_SH);
		return;
	}

	STR_M_SetMaterial(_pst_GO);
	STR_M_ResetFont(pst_STR);
    STR_GetRectShort(pst_STR, i, f, n, &min, &max);
	STR_GetRectShort(pst_STR, i, 0, -1, &MIN, &MAX);

	switch( pst_STR->dst_String[ i ].uw_Flags & STR_Cuw_SF_XJustifyMask )
	{
	case STR_Cuw_SF_Left: dx = 0; break;
	case STR_Cuw_SF_CenterWidth: dx = (MAX.x + MIN.x) / 2; break;
	case STR_Cuw_SF_Right: dx = MAX.x - MIN.x; break;
	}
	switch( pst_STR->dst_String[ i ].uw_Flags & STR_Cuw_SF_YJustifyMask )
	{
	case STR_Cuw_SF_Top: dy = 0; break;
	case STR_Cuw_SF_CenterHeight: dy = (MAX.y + MIN.y) / 2; break;
	case STR_Cuw_SF_Bottom: dy = MAX.y - MIN.y; break;
	}

	min.x = pst_STR->dst_String[ i ].st_Pivot.x + ((min.x - dx) / pst_STR->uw_SW);
	min.y = pst_STR->dst_String[ i ].st_Pivot.y + ((min.y - dy) / pst_STR->uw_SH);
	max.x = pst_STR->dst_String[ i ].st_Pivot.x + ((max.x - dx) / pst_STR->uw_SW);
	max.y = pst_STR->dst_String[ i ].st_Pivot.y + ((max.y - dy) / pst_STR->uw_SH);

	switch( command )
	{
	case 0:	MATH_CopyVector( V, &min ); break;
	case 1: MATH_CopyVector( V, &max ); break;
	default: MATH_InitVectorToZero( V );
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Vector *STR_pst_GetSize(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n, MATH_tdst_Vector *v)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    min, max;
    STR_tdst_Struct		*pst_STR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (!v) return NULL;

	pst_STR = STR_pst_Get(_pst_GO);
    if(pst_STR)
	{
		STR_M_SetMaterial(_pst_GO);
		STR_M_ResetFont(pst_STR);
        STR_GetRectShort(pst_STR, i, f, n, &min, &max);
   
		MATH_SubVector(v, &max, &min);
		if(v->x < 0) v->x *= -1;
		if(v->y < 0) v->y *= -1;
        v->x /= pst_STR->uw_SW;
        v->y /= pst_STR->uw_SH;
	}
	return v;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_ToUpper(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter             *pst_Letter, *pst_Last;
    STR_tdst_String             *pst_String;
    STR_tdst_Struct             *pst_STR;
    int                         val;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GO);
    if(!pst_STR) return;

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL)
        return;

	STR_M_SetMaterial(_pst_GO);
    STR_M_ResetFont(pst_STR);

    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);

    pst_Letter = pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_Letter + n;

    for(; pst_Letter < pst_Last; pst_Letter++)
	{
        val = pst_Letter->ul_Flags & STR_Cul_LF_ValueMask;
        if ((val >= 'a') && (val <= 'z') )
        {
            val += 'A' - 'a';
            pst_Letter->ul_Flags &= ~STR_Cul_LF_ValueMask;
            pst_Letter->ul_Flags |= val;
        }
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_ToLower(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Letter             *pst_Letter, *pst_Last;
    STR_tdst_String             *pst_String;
    STR_tdst_Struct             *pst_STR;
    int                         val;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GO);
    if(!pst_STR) return;

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL)
        return;

	STR_M_SetMaterial(_pst_GO);
    STR_M_ResetFont(pst_STR);

    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);

    pst_Letter = pst_STR->dst_Letter + pst_String->uw_First + f;
    pst_Last = pst_Letter + n;

    for(; pst_Letter < pst_Last; pst_Letter++)
	{
        val = pst_Letter->ul_Flags & STR_Cul_LF_ValueMask;
        if ((val >= 'A') && (val <= 'Z') )
        {
            val += 'a' - 'A';
        }
		else
		{
			if (TEXT_i_GetLang() == INO_e_French)
			{
				if (val == 212) val = 244;
			}
			else if (TEXT_i_GetLang() == INO_e_Polish )
			{
				if (val == 202) val = 234;
				else if (val == 165) val = 185;
				else if (val == 140) val = 156;
				else if (val == 163) val = 179;
				else if (val == 175) val = 191;
				else if (val == 143) val = 159;
				else if (val == 198) val = 230;
				else if (val == 209) val = 241;
			}
			else if (TEXT_i_GetLang() == INO_e_Russian)
			{
				if ((val >= 190) && (val <= 221))
					val = (val - 190) + 223;
			}
		}
		pst_Letter->ul_Flags &= ~STR_Cul_LF_ValueMask;
        pst_Letter->ul_Flags |= val;
	}
}


/*$4
 =======================================================================================================================
    Frame
 =======================================================================================================================
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetFrame( OBJ_tdst_GameObject *_pst_GO, int i, int p, int c)
{
    STR_tdst_String *pst_String;
    STR_tdst_Struct *pst_STR;

    pst_STR = STR_pst_Get(_pst_GO);
    if( !pst_STR) return;

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL) return;
    pst_String->st_Frame.ul_Flags = (p << STR_Cul_LF_PageShift) & STR_Cul_LF_PageMask;
    pst_String->st_Frame.ul_Flags |= (c & STR_Cul_LF_ValueMask);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void  STR_SetFramePos( OBJ_tdst_GameObject *_pst_GO, int i, MATH_tdst_Vector *v1, MATH_tdst_Vector *v2)
{
    STR_tdst_String *pst_String;
    STR_tdst_Struct *pst_STR;

    pst_STR = STR_pst_Get(_pst_GO);
    if( !pst_STR ) return;

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL) return;
    pst_String->st_Frame.x[0] = (SHORT) (v1->x * pst_STR->uw_SW);
    pst_String->st_Frame.y[0] = (SHORT) (v1->y * pst_STR->uw_SH);
    pst_String->st_Frame.x[1] = (SHORT) (v2->x * pst_STR->uw_SW);
    pst_String->st_Frame.y[1] = (SHORT) (v2->y * pst_STR->uw_SH);
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetFrameBorderSize( OBJ_tdst_GameObject *_pst_GO, int i, float f)
{
    STR_tdst_String *pst_String;
    STR_tdst_Struct *pst_STR;

    pst_STR = STR_pst_Get(_pst_GO);
    if( !pst_STR) return;

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL) return;
   	pst_String->st_Frame.borderx = (SHORT) (f * pst_STR->uw_SW);
	pst_String->st_Frame.bordery = (SHORT) (f * pst_STR->uw_SH);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_SetFrameColor( OBJ_tdst_GameObject *_pst_GO, int i, ULONG c)
{
    STR_tdst_String *pst_String;
    STR_tdst_Struct *pst_STR;

    pst_STR = STR_pst_Get(_pst_GO);
    if( !pst_STR) return;

    if((pst_String = STR_pst_GetString(pst_STR, i)) == NULL) return;
    pst_String->st_Frame.ul_Color = c;
}

/*$4
 =======================================================================================================================
    Rect 
 =======================================================================================================================
 */
void STR_GetRectShort(STR_tdst_Struct *_pst_STR, int i, int f, int n, MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_String     *pst_String;
    STR_tdst_Letter     *pst_Letter, *pst_Last;
    short               x, y, xmin, xmax, ymin, ymax;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    xmin = ymin = 10000;
    xmax = ymax = -10000;

    if( (pst_String = STR_pst_GetString(_pst_STR, i)) )
    {
        f = STR_l_GetValidIndex(_pst_STR, pst_String, f);
        n = STR_l_GetValidNumber(_pst_STR, pst_String, f, n);

        pst_Letter = _pst_STR->dst_Letter + pst_String->uw_First + f;
        pst_Last = pst_Letter + n;

        for(; pst_Letter < pst_Last; pst_Letter++)
        {
            if (pst_Letter->x < xmin) xmin = pst_Letter->x;
            if (pst_Letter->y < ymin) ymin = pst_Letter->y;
            x = pst_Letter->x + pst_Letter->w;
            y = pst_Letter->y + pst_Letter->h;
            if (x > xmax) xmax = x;
            if (y > ymax) ymax = y;
        }
    }

    if (_pst_Min )
    {
        if (xmin == 10000)
            MATH_InitVectorToZero( _pst_Min );
        else
            MATH_InitVector( _pst_Min, (float) xmin, (float) ymin, 0 );
    }
    if (_pst_Max )
    {
        if (xmin == 10000)
            MATH_InitVectorToZero( _pst_Max );
        else
            MATH_InitVector( _pst_Max, (float) xmax, (float) ymax, 0 );
    }
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Vector *STR_pst_GetMin(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n, MATH_tdst_Vector *_pst_Min)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Struct *pst_STR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_Min) return NULL;

    pst_STR = STR_pst_Get(_pst_GO);
    if(pst_STR)
	{
		STR_M_SetMaterial(_pst_GO);
		STR_M_ResetFont(pst_STR);
        STR_GetRectShort(pst_STR, i, f, n, _pst_Min, NULL);
        _pst_Min->x /= pst_STR->uw_SW;
        _pst_Min->y /= pst_STR->uw_SH;
	}
    return _pst_Min;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Vector *STR_pst_GetMax(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n, MATH_tdst_Vector *_pst_Max)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Struct *pst_STR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_Max) return NULL;

    pst_STR = STR_pst_Get(_pst_GO);
    if(pst_STR) 
	{
		STR_M_SetMaterial(_pst_GO);
		STR_M_ResetFont(pst_STR);
        STR_GetRectShort(pst_STR, i, f, n, NULL, _pst_Max);
        _pst_Max->x /= pst_STR->uw_SW;
        _pst_Max->y /= pst_STR->uw_SH;
	}
    return _pst_Max;
}

/*
 =======================================================================================================================
 aligne le texte, align donne le type d'alignement £
    0 : centré horizontalement
    1 : chaque centré verticalement
    2 : chaque ligne aligné par le bas
	3 : aligné sur la droite
 =======================================================================================================================
 */
void STR_Align( struct OBJ_tdst_GameObject_ *_pst_GO, int i, int f, int n, int align )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    STR_tdst_Struct     *pst_STR;
    STR_tdst_String     *pst_String;
    STR_tdst_Letter     *pst_Letter, *pst_Last, *pst_Line;
    MATH_tdst_Vector    v;
    short               x, y, xmin, xmax, ymin, ymax;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return;
    if( !(pst_String = STR_pst_GetString(pst_STR, i)) ) return;

    f = STR_l_GetValidIndex(pst_STR, pst_String, f);
    n = STR_l_GetValidNumber(pst_STR, pst_String, f, n);

    if ( (align == 0) || (align == 3) )
	{
        STR_pst_GetSize( _pst_GO, i, f, n, &v );
		v.x *= pst_STR->uw_SW;
        v.y *= pst_STR->uw_SH;
	}

    pst_Letter = pst_STR->dst_Letter + pst_String->uw_First + f;
	pst_Last = pst_Letter + n;
	if (pst_Letter->ul_Flags & STR_Cul_LF_Retrait) 
		pst_Letter++;
    pst_Line = pst_Letter;

    while (pst_Letter < pst_Last )
    {
        xmin = ymin = 10000;
        xmax = ymax = -10000;

        for(; pst_Letter < pst_Last; pst_Letter++)
        {
			if ( (align == 3) && (pst_Letter->ul_Flags & STR_Cul_LF_NewLine) )
			{
				if ( (pst_Letter->ul_Flags & STR_Cul_LF_ValueMask) == 32 )
					break;
			}

            if (pst_Letter->x < xmin) xmin = pst_Letter->x;
            if (pst_Letter->y < ymin) ymin = pst_Letter->y;
            x = pst_Letter->x + pst_Letter->w;
            y = pst_Letter->y + pst_Letter->h;
            if (x > xmax) xmax = x;
            if (y > ymax) ymax = y;

            if (pst_Letter->ul_Flags & STR_Cul_LF_NewLine)
                break;
        }
        if (pst_Letter < pst_Last)
            pst_Letter++;

        switch (align)
        {
        case 0:
            x = (((short) v.x) - (xmax - xmin)) / 2;
            for ( ; pst_Line < pst_Letter; pst_Line++)
                pst_Line->x += x;
            break;
        case 1:
            y = ymax - ymin;
            for ( ; pst_Line < pst_Letter; pst_Line++)
                pst_Line->y += (y - pst_Line->h) / 2;
            break;
        case 2:
            for ( ; pst_Line < pst_Letter; pst_Line++)
                pst_Line->y = ymax - pst_Line->h;
            break;
		case 3:
			x = ((short) v.x) - xmax;
            for ( ; pst_Line < pst_Letter; pst_Line++)
                pst_Line->x += x;
            break;
        }
    }
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
