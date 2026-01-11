/*$T MATHconst.h GC!1.5 10/06/99 17:50:26 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Définition des constantes mathématiques usuelles
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _MATHCONST_H_
#define _MATHCONST_H_
#include "BASe/BAStypes.h"

#define Cf_Zero         0.0f
#define Cf_EpsilonLow   0.000000001f
#define Cf_Epsilon      0.0000001f
#define Cf_EpsilonBig   0.0001f
#define Cf_Inv1000      0.001f
#define Cf_Inv100       0.01f
#define Cf_PiBy180      0.01745329251994329f
#define Cf_Inv16		0.0625f
#define Cf_Inv10        0.1f
#define Cf_1By2Pi       0.1591549430919f
#define Cf_Inv5         0.2f
#define Cf_Inv4         0.25f
#define Cf_Inv3         0.3333333333333333f
#define Cf_PiBy8        0.3926990816987241f
#define Cf_Inv2         0.5f
#define Cf_PiBy6        0.5235987755983f
#define Cf_InvSqrt3     0.5773502691896259f
#define Cf_InvSqrt2     0.7071067811865475f
#define Cf_PiBy4        0.7853981633974482f
#define Cf_Sqrt3By2     0.866025403784f
#define Cf_One          1.0f
#define Cf_PiBy3        1.047197551197f
#define Cf_Sqrt2        1.414213562373095f
#define Cf_PiBy2        1.570796326794896f
#define Cf_Sqrt3        1.732050807568877f
#define Cf_Two          2.0f
#define Cf_Sqrt5        2.236067977499790f
#define Cf_E            2.718281828459045f
#define Cf_Pi           3.141592653589793f
#define Cf_3PiBy2       4.712388980385f
#define Cf_2Pi          6.283185307179586f
#define Cf_10           10.0f
#define Cf_180ByPi      57.29577951308233f
#define Cf_1024by2Pi    162.9746617261f
#define Cf_1K           1000.0f
#define Cf_10K          10000.0f
#define Cf_2exp16       65536.0f
#define Cf_1M           1000000.0f
#define Cf_2exp24       16777216.0f
#define Cf_2exp32       4294967296.0f
#define Cf_Infinit      3.402822E38f

#define Cf_Sin30        Cf_Inv2
#define Cf_Cos30        Cf_Sqrt3By2
#define Cf_Sin45        Cf_InvSqrt2
#define Cf_Cos45        Cf_InvSqrt2
#define Cf_Sin60        Cf_Sqrt3By2
#define Cf_Cos60        Cf_Inv2

#define Cl_MaxLong      ((LONG) 2147483647)
#define Cl_0f           0x00000000  /* 0.0 f in LONG format */
#define Cl_1f           0x3f800000  /* 1.0 f in LONG format */

#endif

