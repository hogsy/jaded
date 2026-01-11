/*$T DYNcol.h GC! 1.100 01/09/01 17:19:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Initialisation and close of the module */
#ifndef PSX2_TARGET
#pragma once
#endif
#include "BASe/BASsys.h"	/* Pour la definition du _inline_ */
#include "MATHs/MATH.h"
#include "ENGine/Sources/DYNamics/DYNstruct.h"
#include "ENGine/Sources/DYNamics/DYNconst.h"
#include "BASe/MEMory/MEM.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$3
 =======================================================================================================================
    Init functions
 =======================================================================================================================
 */

DYN_tdst_Col	*DYN_pst_ColCreate(void);
void			DYN_ColInit
				(
					DYN_tdst_Col	*_pst_Col,
					float			_f_Rebound,
					float			_f_Slide,
					float			_f_ReboundHorizCosAngle,
					float			_f_SlideHorizCosAngle
				);
void			DYN_ColReinit(DYN_tdst_Col *_pst_Col);
void			DYN_ColClose(DYN_tdst_Col *_pst_Col);

/*$3
 =======================================================================================================================
    Access functions
 =======================================================================================================================
 */

/*
 =======================================================================================================================
    Set/Get the slide coeff
 =======================================================================================================================
 */
_inline_ void DYN_SlideSet(DYN_tdst_Col *_pst_Col, float _f_Slide)
{
	if(MEM_b_CheckPointer(_pst_Col))
	{
		_pst_Col->f_Slide = _f_Slide;
	}
}
/**/
_inline_ float DYN_f_SlideGet(DYN_tdst_Col *_pst_Col)
{
	if(MEM_b_CheckPointer(_pst_Col))
	{
		return(_pst_Col->f_Slide);
	}
	else
		return Cf_One;
}

/*
 =======================================================================================================================
    Set/Get the rebound coeff
 =======================================================================================================================
 */
_inline_ void DYN_ReboundSet(DYN_tdst_Col *_pst_Col, float _f_Rebound)
{
	if(MEM_b_CheckPointer(_pst_Col))
	{
		_pst_Col->f_Rebound = _f_Rebound;
	}
}
/**/
_inline_ float DYN_f_ReboundGet(DYN_tdst_Col *_pst_Col)
{
	if(MEM_b_CheckPointer(_pst_Col))
	{
		return(_pst_Col->f_Rebound);
	}
	else
		return Cf_Zero;
}

/*
 =======================================================================================================================
    Set/Get the f_SlideHorizCosAngle coeff
 =======================================================================================================================
 */
_inline_ void DYN_SlideHorizCosAngleSet(DYN_tdst_Col *_pst_Col, float _f_SlideHorizCosAngle)
{
	if(MEM_b_CheckPointer(_pst_Col))
	{
		_pst_Col->f_SlideHorizCosAngle = _f_SlideHorizCosAngle;
	}
}
/**/
_inline_ float DYN_f_SlideHorizCosAngleGet(DYN_tdst_Col *_pst_Col)
{
	if(MEM_b_CheckPointer(_pst_Col))
	{
		return(_pst_Col->f_SlideHorizCosAngle);
	}
	else
		return Cf_Zero;
}

/*
 =======================================================================================================================
    Set/Get the f_ReboundHorizCosAngle coeff
 =======================================================================================================================
 */
_inline_ void DYN_ReboundHorizCosAngleSet(DYN_tdst_Col *_pst_Col, float _f_ReboundHorizCosAngle)
{
	if(MEM_b_CheckPointer(_pst_Col))
	{
		_pst_Col->f_ReboundHorizCosAngle = _f_ReboundHorizCosAngle;
	}
}
/**/
_inline_ float DYN_f_ReboundHorizCosAngleGet(DYN_tdst_Col *_pst_Col)
{
	if(MEM_b_CheckPointer(_pst_Col))
	{
		return(_pst_Col->f_ReboundHorizCosAngle);
	}
	else
		return Cf_Zero;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
