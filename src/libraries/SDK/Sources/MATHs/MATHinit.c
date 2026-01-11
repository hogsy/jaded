/*$T MATHinit.c GC!1.32 05/18/99 15:34:59 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Optimisation tables creation
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#include "MATHs/MATHfloat.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/LOAding/LOAread.h"

/*
 ===================================================================================================
    Aim:    Square root table creation
 ===================================================================================================
 */
void MATH_CreateSqrtTable(void)
{

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    i;
    float   f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < 1024; i++)
    {
        f = (float) L_sqrt( 1.0f + ((float) i / 1024));
        MATH_gl_TableRacine[i + 1024] = lGetFloatMantisse(f);

        f = (float) (L_sqrt(2.0f) * L_sqrt( 1.0f + ((float) i / 1024)));
        MATH_gl_TableRacine[i] = lGetFloatMantisse(f);;
    }

}

/*
 ===================================================================================================
    Aim:    divisions table creation
 ===================================================================================================
 */
void MATH_CreateInvTable(void)
{
#ifndef _GAMECUBE
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    i;
    float   f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_gl_TableInverse[0] = (1 << 23);

    for(i = 1; i < 1024; i++)
    {
        f = 1.0F / (1.0F + ((float) i / 1024));
        MATH_gl_TableInverse[i] = lGetFloatMantisse(f);
    }
#endif    
}

/*
 ===================================================================================================
    Aim:    1 divided by squareroot table creation
 ===================================================================================================
 */
void MATH_CreateInvSqrtTable(void)
{
#ifndef _GAMECUBE
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG    i;
    float   f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < 1024; i++)
    {
        f = (float) (2.0 / L_sqrt( (1.0f + ((float) i / 1024))));
        MATH_gl_TableInverseRacine[i + 1024] = lGetFloatMantisse(f);

        f = (float) (L_sqrt(2.0) / L_sqrt( (1.0f + ((float) i / 1024))));
        MATH_gl_TableInverseRacine[i] = lGetFloatMantisse(f) + 0x800000;
    }

    MATH_gl_TableInverseRacine[1024] = (1 << 23);
#endif
}

/*
 ===================================================================================================
    Aim:    Sine table creation
 ===================================================================================================
 */
void MATH_CreateTrigTables(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < 1024; i++)
    {
        MATH_gf_TableSin[i] = (float) L_sin(((float) i) * (Cf_2Pi / 1024.0f));
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
#undef rand
#define rand rand
extern ULONG INO_n_FrameCounter;
extern int INO_b_RecordInput;
extern int INO_b_PlayInput;
extern BOOL LOA_gb_SwapperActive;
int JADErand(void)
{
	float ff;
	unsigned short ush;
	extern float TIM_f_Clock_TrueRead(void);

	ff = TIM_f_Clock_TrueRead();
	
#ifdef JOYRECORDER
    if (INO_b_RecordInput || INO_b_PlayInput)
    {
        ff = (float)INO_n_FrameCounter/30.0f;
        srand(INO_n_FrameCounter);
    }
#endif

	if(LOA_gb_SwapperActive)
		SwapDWord((int *) &ff);
		
	ush = *(unsigned short*)&ff;
	
	ff = fLongToFloat(ush) / (float)0xFFFF;

	return (rand() ^ (lFloatToLong(ff*(float)RAND_MAX)));
}
