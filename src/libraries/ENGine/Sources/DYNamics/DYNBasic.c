/*$T DYNBasic.c GC!1.52 01/19/00 14:31:41 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    This file countains most of the functions called directly by DYNMain.c
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "BASe/BASsys.h"/* Pour la definition du _inline_ */
#include "BASe/ERRors/ERRasser.h"
#include "MATHs/MATH.h"

#ifdef PSX2_TARGET
#ifdef __cplusplus
extern "C" {
#endif
#else
#include "TIMer/TIMdefs.h"
#endif
/*
 ===================================================================================================
    Aim:    Artificially lim*it the Speed (in norm)
 ===================================================================================================
 */
void DYN_ApplySpeedLimit(DYN_tdst_Dyna *_pst_Dyna)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_Speed;
    MATH_tdst_Vector    st_SpeedHoriz;
    MATH_tdst_Vector    st_SpeedVert;
    float               f_SpeedHoriz;
    float               f_SpeedVert;
    float               f_SpeedLimitVert;
    float               f_SpeedLimitHoriz;
    MATH_tdst_Vector    st_UnitG;   /* Gravity direction */
    char                b_Apply;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    f_SpeedLimitHoriz = DYN_f_GetSpeedLimitHoriz(_pst_Dyna);
    f_SpeedLimitVert = DYN_f_GetSpeedLimitVert(_pst_Dyna);
    if(f_SpeedLimitHoriz != f_SpeedLimitVert)
    {
        b_Apply = 0;

        /* Limit differently if it's vertical or horizontal */
        DYN_GetSpeedVector(_pst_Dyna, &st_Speed);

        /* Calculate gravity direction */
        MATH_CopyVector(&st_UnitG, DYN_pst_GetGravity(_pst_Dyna));
        MATH_NormalizeEqualVector(&st_UnitG);

        /* Calculate the vertical component */
        f_SpeedVert = MATH_f_DotProduct(&st_UnitG, &st_Speed);
        MATH_ScaleVector(&st_SpeedVert, &st_UnitG, f_SpeedVert);
        f_SpeedVert = fAbs(f_SpeedVert);

        /* Calculate the horizontal component */
        MATH_SubVector(&st_SpeedHoriz, &st_Speed, &st_SpeedVert);
        f_SpeedHoriz = MATH_f_NormVector(&st_SpeedHoriz);

        /* Limit */
        if(fSup(f_SpeedHoriz, f_SpeedLimitHoriz))
        {
            MATH_SetNormVector(&st_SpeedHoriz, &st_SpeedHoriz, f_SpeedLimitHoriz);
            b_Apply = 1;
        }

        if(fSup(f_SpeedVert, f_SpeedLimitVert))
        {
            MATH_SetNormVector(&st_SpeedVert, &st_SpeedVert, f_SpeedLimitVert);
            b_Apply = 1;
        }

        /* Apply new speed */
        if(b_Apply)
        {
            MATH_AddVector(&st_Speed, &st_SpeedVert, &st_SpeedHoriz);
            DYN_SetSpeedVector(_pst_Dyna, &st_Speed);
        }
    }
    else
    {
        /* Limit the speed in norm */
        f_SpeedHoriz = DYN_f_GetNormSpeed(_pst_Dyna);
        if(fSup(f_SpeedHoriz, f_SpeedLimitHoriz))
            f_SpeedHoriz = f_SpeedLimitHoriz;
        DYN_SetNormSpeed(_pst_Dyna, f_SpeedHoriz);
    }
}

/*
 ===================================================================================================
    Aim:    Calculates the friction to set to have a maximal speed of _f_MaxSpeed, with the current
            traction vector
 ===================================================================================================
 */
float DYN_f_FindFrictionForMaxSpeed(DYN_tdst_Dyna *_pst_Dyna, float _f_MaxSpeed)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    *pst_Traction;
    float               f_K;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* K = T/V */
    pst_Traction = DYN_pst_GetTraction(_pst_Dyna);
    f_K = fDiv(MATH_f_NormVector(pst_Traction), _f_MaxSpeed);

    return(f_K);
}

/*
 ===================================================================================================
    Aim:    Calculate the traction vector and the friction to reach the given speed vector in a
            given time

    In:     _pst_V2     Speed vector to reach
            _f_dt       Time to reach the speed vector
            _pf_K       Output parameter: Calculated friction to apply
            _pst_T      Output parameter: Calculated Traction vector to apply Out TRUE if
                        everything went OK, FALSE if not (for example, basic forces were not
                        allocated) 

    Note:   V2 is the limit speed. In fact, The coeffs are chosen so that 95 % of the limit speed
            is reached in dt seconds)
 ===================================================================================================
 */
int DYN_b_SpeedReachInTime
(
    DYN_tdst_Dyna       *_pst_Dyna,
    float               *_pf_K,
    MATH_tdst_Vector    *_pst_T,
    MATH_tdst_Vector    *_pst_V2,
    float               _f_dt
)
{
    /* This value is (1-alpha/alpha for alpha = 1.05) */

    /* #define f_OneMinusAlphaUnderAlpha - 0.0476190476190f */
#define f_OneMinusAlphaUnderAlpha   - 0.090909090909f

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float                   f_Tmp;
    float                   f_Start;
    float                   f_Const1;
    float                   f_NormV2;
    float                   f_InvMass;
    MATH_tdst_Vector        st_V0;
#if (0)
    MATH_tdst_Vector        CheckSpeed;
    static MATH_tdst_Vector CheckPosition;
#endif
    int                     iRet;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Return value */
    iRet = FALSE;

    /* Get the current speed of the object */
    DYN_GetSpeedVector(_pst_Dyna, &st_V0);
    f_InvMass = DYN_f_GetInvMass(_pst_Dyna);

    f_Start = f_OneMinusAlphaUnderAlpha;
    f_Const1 = fInv(_f_dt * f_InvMass); /* M/dt */
    f_NormV2 = MATH_f_NormVector(_pst_V2);

    f_Tmp = fAbs(f_Start * fDiv(f_NormV2, MATH_f_NormVector(&st_V0) - f_NormV2));

    /* Return the value of K */
    *_pf_K = -fLn(f_Tmp) * f_Const1;

    /* And the value of the Traction vector (T) */
    MATH_NormalizeVector(_pst_T, _pst_V2);
    MATH_MulEqualVector(_pst_T, fMul3(*_pf_K, f_NormV2, f_InvMass));

    if(DYN_ul_GetDynFlags(_pst_Dyna) & DYN_C_BasicForces)
    {
        /* Set the new parameters */
        DYN_FrictionSet(_pst_Dyna, *_pf_K);
        DYN_SetTraction(_pst_Dyna, _pst_T);
        iRet = TRUE;
    }

#if (0)

    /* Test, to remove */
    DYN_SpeedAfterNseconds(_pst_Dyna, &CheckSpeed, _f_dt);
    DYN_PositionAfterNseconds(_pst_Dyna, &CheckPosition, _f_dt);

    /* Gf_StartTime = TIM_f_Clock_TrueRead(); gf_WaitTime = _f_dt; */
#endif
    return(iRet);
}

/*
 ===================================================================================================
    Aim:    Calculates the speed vector that the object will have after dt seconds, (if no new
            forces are applied between these dt seconds ) Tested: OK

    In:     _pst_FinalSpeed     Pointer to the returned vector
 ===================================================================================================
 */
void DYN_SpeedAfterNseconds
(
    DYN_tdst_Dyna       *_pst_Dyna,
    MATH_tdst_Vector    *_pst_FinalSpeed,
    float               _f_dt
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    V2;
    MATH_tdst_Vector    V0;
    MATH_tdst_Vector    Tmp;
    MATH_tdst_Vector    *pT;
    float               k, InvK;
    float               InvM;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_Dyna->pst_Forces)
    {
        /* Get known values */
        k = DYN_f_FrictionGet(_pst_Dyna);
        DYN_GetSpeedVector(_pst_Dyna, &V0);
        InvK = fInv(k);
        pT = DYN_pst_GetTraction(_pst_Dyna);
        InvM = DYN_f_GetInvMass(_pst_Dyna);

        /* Calculate Speed */
        MATH_MulVector(&V2, pT, InvK);                      /* V2 = T/k */
        MATH_SubVector(&Tmp, &V2, &V0);                     /* Tmp = T/k-V0 */
        MATH_MulEqualVector(&Tmp, fExp(-k * _f_dt * InvM)); /* (T/k-V0)*Exp(-(k/m)*dt) */
        MATH_SubVector(_pst_FinalSpeed, &V2, &Tmp);
    }
}

/*
 ===================================================================================================
    Aim:    Calculates the position that the object will have after dt seconds, (if no new forces
            are applied between these dt seconds )

    Note:   NOT optimized ! coded like this for tests
 ===================================================================================================
 */
void DYN_PositionAfterNseconds
(
    DYN_tdst_Dyna       *_pst_Dyna,
    MATH_tdst_Vector    *_pst_FinalPos,
    float               _f_dt
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    V0;
    MATH_tdst_Vector    *pX0;
    MATH_tdst_Vector    Tmp, Tmp1, Tmp2, Tmp3;
    MATH_tdst_Vector    T;
    float               f_k, InvK;
    float               InvM, m, E;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_Dyna->pst_Forces)
    {
        /* Read Known Parameters */
        DYN_GetSpeedVector(_pst_Dyna, &V0);
        pX0 = DYN_pst_GetPosition(_pst_Dyna);
        MATH_CopyVector(&T, DYN_pst_GetTraction(_pst_Dyna));
        f_k = DYN_f_FrictionGet(_pst_Dyna);
        InvM = DYN_f_GetInvMass(_pst_Dyna);
        m = fInv(InvM);

        /* Test f_k to know how to integrate the equation */
        if(fNulWithEpsilon(f_k, Cf_EpsilonBig))
        {
            MATH_MulVector(&Tmp3, &T, fHalf(InvM) * fSqr(_f_dt));
            MATH_MulVector(&Tmp2, &V0, _f_dt);
            MATH_AddVector(_pst_FinalPos, &Tmp3, &Tmp2);
            MATH_AddEqualVector(_pst_FinalPos, pX0);
        }
        else
        {
            InvK = fInv(f_k);

            /* Calc Intermediate values */

            /* Calculate E */
            E = fExp(-f_k * _f_dt * InvM);

            /* Calculate T2 */
            MATH_MulEqualVector(&T, InvK);
            MATH_MulVector(&Tmp2, &T, _f_dt);

            /* Calculate T1 */
            MATH_CopyVector(&Tmp1, pX0);
            MATH_SubVector(&Tmp, &T, &V0);
            MATH_MulEqualVector(&Tmp, InvK * m);    /* Tmp=(t/f_k-v0)*(m/f_k) */
            MATH_SubEqualVector(&Tmp1, &Tmp);

            /* Calculate T3 */
            MATH_MulVector(&Tmp3, &Tmp, E);

            /* Add the 3 tmp vectors to get final position vector */
            MATH_AddVector(_pst_FinalPos, &Tmp1, &Tmp2);
            MATH_AddEqualVector(_pst_FinalPos, &Tmp3);
        }
    }
}

/*
 ===================================================================================================
    Aim:    Reach a position in a given time

    Note:   To be finished...
 ===================================================================================================
 */
void DYN_Position_ReachInTime
(
    DYN_tdst_Dyna       *_pst_Dyna,
    MATH_tdst_Vector    *_pst_FinalPos,
    float               _f_dt
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float               _f_d;                       /* Distance between the two positions */
    MATH_tdst_Vector    *pst_Pos1;                  /* Current position */
    float               f_Speed1, f_Speed2;         /* Norm of current and wanted speeds */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Pos1 = DYN_pst_GetPosition(_pst_Dyna);
    f_Speed1 = DYN_f_GetNormSpeed(_pst_Dyna);
    _f_d = MATH_f_Distance(pst_Pos1, _pst_FinalPos);

    /* Find the aproximate speed we want after _f_dt seconds */
    f_Speed2 = fSub(fDiv(fTwice(_f_d), _f_dt), f_Speed1);
}

/*
 ===================================================================================================
    Aim:    Calculates the speed in the case there is a friction vector
 ===================================================================================================
 */
void DYN_CalculateSpeedWhenFrictionVector(DYN_tdst_Dyna *_pst_Dyna, float _f_Dt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float               f_Tmp, f_InvMass, f_A;
    int                 i_Calc;
    float               f_KoverM;
    float               f_MoverK;
    MATH_tdst_Vector    st_TLoc, st_PLoc;
    MATH_tdst_Vector    *pst_k;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    i_Calc = 0;

    /* Convert sum of forces to local coodinates */
	if(_pst_Dyna->ul_DynFlags & DYN_C_GlobalFriction)
	{
		MATH_CopyVector(&st_TLoc, DYN_pst_GetSumOfForces(_pst_Dyna));
		MATH_CopyVector(&st_PLoc, DYN_pst_GetP(_pst_Dyna));
	}
	else
	{
		MATH_VectorGlobalToLocal
		(
			&st_TLoc,
			DYN_pst_GetGlobalMatrix(_pst_Dyna),
			DYN_pst_GetSumOfForces(_pst_Dyna)
		);

	    /* Convert linear momentum to local coodinates */
		MATH_VectorGlobalToLocal(&st_PLoc, DYN_pst_GetGlobalMatrix(_pst_Dyna), DYN_pst_GetP(_pst_Dyna));

	}

    pst_k = DYN_pst_FrictionVectorGet(_pst_Dyna);

    /* If no friction in x,y or z,the x,y or z term has no exponential: dP=F*dt */
    if(fAlmostNul(pst_k->x))
        st_PLoc.x += st_TLoc.x * _f_Dt;
    else
        i_Calc += 1;
    if(fAlmostNul(pst_k->y))
        st_PLoc.y += st_TLoc.y * _f_Dt;
    else
        i_Calc += 2;
    if(fAlmostNul(pst_k->z))
        st_PLoc.z += st_TLoc.z * _f_Dt;
    else
        i_Calc += 4;

    if(i_Calc != 0)
    {
        f_InvMass = DYN_f_GetInvMass(_pst_Dyna);
        if(i_Calc & 1)
        {
            /* Needs to calculate x */
            f_KoverM = fMul(pst_k->x, DYN_f_GetInvMass(_pst_Dyna));
            f_MoverK = fInv(f_KoverM);

            /* F_Tmp = (m/k)*T -P0 */
            f_A = f_MoverK * st_TLoc.x;
            f_Tmp = f_A - st_PLoc.x;
            f_Tmp *= fExp(-_f_Dt * f_KoverM);
            st_PLoc.x = f_A - f_Tmp;
        }

        if(i_Calc & 2)
        {
            /* Needs to calculate y */
            f_KoverM = fMul(pst_k->y, DYN_f_GetInvMass(_pst_Dyna));
            f_MoverK = fInv(f_KoverM);

            /* F_Tmp = (m/k)*T -P0 */
            f_A = f_MoverK * st_TLoc.y;
            f_Tmp = f_A - st_PLoc.y;
            f_Tmp *= fExp(-_f_Dt * f_KoverM);
            st_PLoc.y = f_A - f_Tmp;
        }

        if(i_Calc & 4)
        {
            /* Needs to calculate z */
            f_KoverM = fMul(pst_k->z, DYN_f_GetInvMass(_pst_Dyna));
            f_MoverK = fInv(f_KoverM);

            /* F_Tmp = (m/k)*T -P0 */
            f_A = f_MoverK * st_TLoc.z;
            f_Tmp = f_A - st_PLoc.z;
            f_Tmp *= fExp(-_f_Dt * f_KoverM);
            st_PLoc.z = f_A - f_Tmp;
        }
    }

	if(_pst_Dyna->ul_DynFlags & DYN_C_GlobalFriction)
		MATH_CopyVector(DYN_pst_GetP(_pst_Dyna), &st_PLoc);
	else
		MATH_VectorLocalToGlobal(DYN_pst_GetP(_pst_Dyna), DYN_pst_GetGlobalMatrix(_pst_Dyna), &st_PLoc);
}



#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
