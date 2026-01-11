/*$T AItools.h GC!1.41 08/17/99 17:34:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __AITOOLS_H__
#define __AITOOLS_H__

#include "AIstack.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ===================================================================================================
    Aim:    Attempt to have two parameters with the same time. If one of the parameters is a float,
            convert the other to a float too.

    Out:    The type of the result (int, or float).
 ===================================================================================================
 */
static _inline_ char IntOrFloat2
(
    AI_tdst_UnionVar    *_pst_Val1,
    AI_tdst_UnionVar    *_pst_Val2,
    AI_tdst_PushVar     *_pst_Var1,
    AI_tdst_PushVar     *_pst_Var2
)
{
    if(_pst_Var1->w_Type != _pst_Var2->w_Type)
    {
        if(_pst_Var1->w_Type == ENUTYPE_FLOAT)
        {
            _pst_Val2->f = fLongToFloat(_pst_Val2->i);
            return 1;
        }

        if(_pst_Var1->w_Type == ENUTYPE_INT)
        {
            _pst_Val2->i = lFloatToLong(_pst_Val2->f);
            return 0;
        }
    }

    return _pst_Var1->w_Type == ENUTYPE_FLOAT;
}

/*
 ===================================================================================================
    Aim:    Attempt to have two parameters with the same time. If one of the parameters is a float,
            convert the other to a float too.

    Out:    The type of the result (int, or float).
 ===================================================================================================
 */
static _inline_ char IntOrFloat2FFloat
(
    AI_tdst_UnionVar    *_pst_Val1,
    AI_tdst_UnionVar    *_pst_Val2,
    AI_tdst_PushVar     *_pst_Var1,
    AI_tdst_PushVar     *_pst_Var2
)
{
    if(_pst_Var1->w_Type != _pst_Var2->w_Type)
    {
        if(_pst_Var1->w_Type == ENUTYPE_FLOAT)
		{
            _pst_Val2->f = fLongToFloat(_pst_Val2->i);
			return 1;
		}

        if(_pst_Var2->w_Type == ENUTYPE_FLOAT)
		{
            _pst_Val1->f = fLongToFloat(_pst_Val1->i);
			return 1;
		}
    }

    return _pst_Var1->w_Type == ENUTYPE_FLOAT;
}

/*
 ===================================================================================================
    Aim:    If a parameter is a float, convert it to an int.
 ===================================================================================================
 */
static _inline_ void IntToFloat1(AI_tdst_UnionVar *_pst_Val1, AI_tdst_PushVar *_pst_Var1)
{
    if(_pst_Var1->w_Type != ENUTYPE_FLOAT)
        _pst_Val1->f = fLongToFloat(_pst_Val1->i);
}

/*
 ===================================================================================================
    Aim:    If a parameter is an int, convert it to a float.
 ===================================================================================================
 */
static _inline_ void FloatToInt1(AI_tdst_UnionVar *_pst_Val1, AI_tdst_PushVar *_pst_Var1)
{
    if(_pst_Var1->w_Type == ENUTYPE_FLOAT)
        _pst_Val1->i = lFloatToLong(_pst_Val1->f);
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __AITOOLS_H__ */