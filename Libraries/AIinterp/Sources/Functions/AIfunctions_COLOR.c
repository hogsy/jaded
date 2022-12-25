/*$T AIfunctions_COLOR.c GC! 1.100 03/19/01 17:42:33 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "LINks/LINKmsg.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "BIGfiles/LOAding/LOAdefs.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLORSetRGBA_C(int i1, int i2, int i3, int i4)
{
	/*~~~~~~*/
	int Color;
	/*~~~~~~*/

	Color = (i4 & 0xff) << 24;	/* A */
	Color |= (i3 & 0xff) << 16; /* B */
	Color |= (i2 & 0xff) << 8;	/* G */
	Color |= (i1 & 0xff) << 0;	/* R */
	return Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLORSetRGBA(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~*/
	int i1, i2, i3, i4;
	/*~~~~~~~~~~~~~~~*/

	i4 = AI_PopInt();
	i3 = AI_PopInt();
	i2 = AI_PopInt();
	i1 = AI_PopInt();
	AI_PushInt(AI_EvalFunc_COLORSetRGBA_C(i1, i2, i3, i4));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLORGetRed_C(int col)
{
	/*~~~~~~*/
	int Color;
	/*~~~~~~*/

	Color = (col & 0xff) >> 0;	/* A */
	return Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLORGetRed(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_COLORGetRed_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLORGetGreen_C(int col)
{
	/*~~~~~~*/
	int Color;
	/*~~~~~~*/

	Color = (col & 0xff00) >> 8;	/* G */
	return Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLORGetGreen(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_COLORGetGreen_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLORGetBlue_C(int col)
{
	/*~~~~~~*/
	int Color;
	/*~~~~~~*/

	Color = (col & 0xff0000) >> 16; /* B */
	return Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLORGetBlue(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_COLORGetBlue_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLORGetAlpha_C(int col)
{
	/*~~~~~~*/
	int Color;
	/*~~~~~~*/

	Color = (col & 0xff000000) >> 24; /* A */
	return Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLORGetAlpha(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_COLORGetAlpha_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ul_Interpol2PackedColor(ULONG ulP1, ULONG ulP2, float fZClipLocalCoef)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	RetValue, Interpoler;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(fZClipLocalCoef >= 0.98f) return ulP2;
	if(fZClipLocalCoef <= 0.02f) return ulP1;
	*((float *) &Interpoler) = fZClipLocalCoef + 32768.0f + 16384.0f;
	RetValue = (Interpoler & 128) ? (ulP2 & 0xfefefefe) >> 1 : (ulP1 & 0xfefefefe) >> 1;
	RetValue += (Interpoler & 64) ? (ulP2 & 0xfcfcfcfc) >> 2 : (ulP1 & 0xfcfcfcfc) >> 2;
	RetValue += (Interpoler & 32) ? (ulP2 & 0xf8f8f8f8) >> 3 : (ulP1 & 0xf8f8f8f8) >> 3;
	RetValue += (Interpoler & 16) ? (ulP2 & 0xf0f0f0f0) >> 4 : (ulP1 & 0xf0f0f0f0) >> 4;
	RetValue += (Interpoler & 8) ? (ulP2 & 0xe0e0e0e0) >> 5 : (ulP1 & 0xe0e0e0e0) >> 5;
	RetValue += (Interpoler & 4) ? (ulP2 & 0xc0c0c0c0) >> 6 : (ulP1 & 0xc0c0c0c0) >> 6;
	RetValue += (Interpoler & 2) ? (ulP2 & 0x80808080) >> 7 : (ulP1 & 0x80808080) >> 7;
	return RetValue;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLORBlend_C(int C1, int C2, float Interp)
{
	float red, green, blue, alpha;
	float red1, green1, blue1, alpha1;

	if(C1 == C2) return C1;
	if(Interp < 0.0f) Interp = 0.0f;
	if(Interp > 1.0f) Interp = 1.0f;

	red = (float) (C1 & 0xFF) / 255.0f;
	green = (float) ((C1 >> 8) & 0xFF) / 255.0f;
	blue = (float) ((C1 >>16) & 0xFF) / 255.0f;
	alpha = (float) ((C1 >> 24) & 0xFF) / 255.0f;

	red1 = (float) (C2 & 0xFF) / 255.0f;
	green1 = (float) ((C2 >> 8) & 0xFF) / 255.0f;
	blue1 = (float) ((C2 >>16) & 0xFF) / 255.0f;
	alpha1 = (float) ((C2 >> 24) & 0xFF) / 255.0f;

	red = ((1 - Interp) * red) + (Interp * red1);
	green = ((1 - Interp) * green) + (Interp * green1);
	blue = ((1 - Interp) * blue) + (Interp * blue1);
	alpha = ((1 - Interp) * alpha) + (Interp * alpha1);

	C1 = ((int) (alpha * 255)) << 24;
	C1 |= ((int) (blue * 255)) << 16;
	C1 |= ((int) (green * 255)) << 8;
	C1 |= ((int) (red * 255)) << 0;

	return C1;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLORBlend(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~*/
	int		C1, C2;
	float	Interp;
	/*~~~~~~~~~~~*/

	Interp = AI_PopFloat();
	C2 = AI_PopInt();
	C1 = AI_PopInt();
	AI_PushInt(AI_EvalFunc_COLORBlend_C(C1, C2, Interp));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLORWhite_C(void)
{
	return 0xffffffff;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLORWhite(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(0xffffffff);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLORBlack_C(void)
{
	return 0xff000000;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLORBlack(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(0xff000000);
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
