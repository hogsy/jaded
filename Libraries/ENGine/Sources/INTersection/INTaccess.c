/*$T INTaccess.c GC! 1.081 06/19/00 14:11:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "INTConst.h"
#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_ResetFlags(ULONG *_pul_Flag)
{
	/*~~~~~~~~~~~~~~~~~~*/
	ULONG	*pul_LastFlag;
	/*~~~~~~~~~~~~~~~~~~*/

	pul_LastFlag = _pul_Flag + INT_Cul_MaxFlags;

	for(; _pul_Flag <= pul_LastFlag; _pul_Flag++) *_pul_Flag = 0x00000000;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INT_SnP_GetFlag(ULONG *_pul_FlagAddress, ULONG _ul_FlagIndexInLong, ULONG _ul_Offset)
{
	return(*(_pul_FlagAddress + _ul_Offset) & (0x00000001 << _ul_FlagIndexInLong));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INT_SnP_FullGetFlag
(
	ULONG	*_pul_FlagAddress,
	ULONG	l_XObj1Ref,
	ULONG	l_XObj2Ref,
	ULONG	_ul_NbElems,
	ULONG	*_ul_FlagIndexInLong,
	ULONG	*_ul_Offset
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	l_XMinRef, l_XMaxRef, l_FlagIndex, l_FlagIndexInLong;
	ULONG	ul_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_XMaxRef = lMax(l_XObj1Ref, l_XObj2Ref);
	l_XMinRef = lMin(l_XObj1Ref, l_XObj2Ref);

	l_FlagIndex = l_XMinRef * _ul_NbElems - lHalf(lSqr(l_XMinRef + 1) - (l_XMinRef + 1)) + (l_XMaxRef - l_XMinRef - 1);
	ul_Offset = l_FlagIndex >> 5;
	*_ul_Offset = ul_Offset;
	l_FlagIndexInLong = l_FlagIndex - (ul_Offset << 5);
	*_ul_FlagIndexInLong = l_FlagIndexInLong;

	return(*(_pul_FlagAddress + ul_Offset) & (0x00000001 << l_FlagIndexInLong));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INT_SnP_FullSetFlag
(
	ULONG	*_pul_FlagAddress,
	ULONG	l_XObj1Ref,
	ULONG	l_XObj2Ref,
	ULONG	_ul_NbElems,
	ULONG	*_ul_FlagIndexInLong,
	ULONG	*_ul_Offset,
	BOOL	_b_Set
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	l_XMinRef, l_XMaxRef, l_FlagIndex, l_FlagIndexInLong;
	ULONG	ul_Offset;
	BOOL	b_IsAlreadySet;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_XMaxRef = lMax(l_XObj1Ref, l_XObj2Ref);
	l_XMinRef = lMin(l_XObj1Ref, l_XObj2Ref);

	l_FlagIndex = l_XMinRef * _ul_NbElems - lHalf(lSqr(l_XMinRef + 1) - (l_XMinRef + 1)) + (l_XMaxRef - l_XMinRef - 1);
	ul_Offset = l_FlagIndex >> 5;
	*_ul_Offset = ul_Offset;
	l_FlagIndexInLong = l_FlagIndex - (ul_Offset << 5);
	*_ul_FlagIndexInLong = l_FlagIndexInLong;

	b_IsAlreadySet = *(_pul_FlagAddress + ul_Offset) & (0x00000001 << l_FlagIndexInLong);

	if(_b_Set)
		*(_pul_FlagAddress + ul_Offset) |= (0x00000001 << l_FlagIndexInLong);
	else
		*(_pul_FlagAddress + ul_Offset) &= ~(0x00000001 << l_FlagIndexInLong);

	return b_IsAlreadySet;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INT_SnP_SetFlag(ULONG *_pul_FlagAddress, ULONG _ul_FlagIndexInLong, ULONG _ul_Offset, BOOL _b_Set)
{
	if(_b_Set)
		*(_pul_FlagAddress + _ul_Offset) |= (0x00000001 << _ul_FlagIndexInLong);
	else
		*(_pul_FlagAddress + _ul_Offset) &= ~(0x00000001 << _ul_FlagIndexInLong);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
