/*$T EVEnt_interpolationkey.c GC! 1.081 02/13/02 14:44:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "EVEstruct.h"
#include "EVEconst.h"
#include "EVEplay.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GAO.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BAse/BENch/BENch.h"

#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#define _ReadLong(_a)	ReadLong((char *) _a)
#define _ReadShort(_a)	ReadShort((char *) _a)
#define _ReadFloat(_a)	ReadFloat((char *) _a)

#else
#define _ReadLong(_a)	*(ULONG *) _a
#define _ReadShort(_a)	*(SHORT *) _a
#define _ReadFloat(_a)	*(float *) _a
#endif
#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
MATH_tdst_Matrix		EVE_ast_Gizmo[64];
MATH_tdst_Matrix		EVE_st_MagicBox;
LONG					EVE_gl_SpecialPlay = 0;
#endif
extern MATH_tdst_Matrix 	*EVE_gpst_SavedMatrix;
extern EVE_tdst_ListTracks 	*EVE_gpst_CurrentListTracks;

#ifdef ACTIVE_EDITORS
extern void				AI_Debug_Move(OBJ_tdst_GameObject *, char *);
#endif

#ifdef _GAMECUBE
#include "GXI_GC/GXI_Bench.h"

#endif


ULONG				Event_SizeOf_Ram;
extern ULONG		LOA_ul_FileTypeSize[40];


#ifdef PSX2_TARGET

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 _inline_ void MATH_CopyCompressedQuaternionUnaligned(MATH_tdst_CompressedQuaternion *CQDst, MATH_tdst_CompressedQuaternion *C)
{
	CQDst->x = (_ReadShort((char *) &C->x));
	CQDst->y = (_ReadShort((char *) &C->y));
	CQDst->z = (_ReadShort((char *) &C->z));
}

_inline_ void MATH_CopyQuaternionUnaligned(MATH_tdst_Quaternion *QDst, MATH_tdst_Quaternion *Q)
{
	QDst->x = (_ReadFloat((char *) &Q->x));
	QDst->y = (_ReadFloat((char *) &Q->y));
	QDst->z = (_ReadFloat((char *) &Q->z));
	QDst->w = (_ReadFloat((char *) &Q->w));
	
}

_inline_ void MATH_UncompressUnitQuaternionUnaligned(MATH_tdst_Quaternion *QDst, MATH_tdst_CompressedQuaternion *C)
{
	/*~~~~~~~~~*/
	float	fSum;
	/*~~~~~~~~~*/

	QDst->x = (_ReadShort((char *) &C->x) / 32767.0f);
	QDst->y = (_ReadShort((char *) &C->y) / 32767.0f);
	QDst->z = (_ReadShort((char *) &C->z) / 32767.0f);

	fSum = fSqr(QDst->x) + fSqr(QDst->y) + fSqr(QDst->z);
	QDst->w = fSum < 1.0f ? fSqrt(1.0f - fSum) : 0.0f;
}

_inline_ void MATH_CompressUnitQuaternionUnaligned(MATH_tdst_CompressedQuaternion *CQDst, MATH_tdst_Quaternion *Q)
{
    MATH_tdst_Quaternion st_Q;

    /* As Q and -Q represent the same rotation, we can have Q->w >= 0 */
    MATH_CopyQuaternionUnaligned(&st_Q, Q);
    if(st_Q.w < 0.0f)
        MATH_NegQuaternion(&st_Q);

	CQDst->x = (short) (st_Q.x * 32767.0f + 0.5f);
	CQDst->y = (short) (st_Q.y * 32767.0f + 0.5f);
	CQDst->z = (short) (st_Q.z * 32767.0f + 0.5f);
}


#endif


/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ int i_ConvertTranslationCoord(float _c)
{
	return (int) (_c * 1024.0f + 0.5f);
}

/*$4
 ***********************************************************************************************************************
    Access functions
 ***********************************************************************************************************************
 */

#ifndef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *EVE_p_GetPdata(EVE_tdst_Event *_pst_Event)
{
	/* Ugly Optimsation */
	if(_pst_Event->uw_NumFrames & 0x8000)
	{
		return (char *) _pst_Event + 2;		
	}

	/* t.size with 0x0001 set => Size and Type are Valid in p_Data union */
	if(_pst_Event->t.size & 0x0001)
	{
		return EVE_gpst_CurrentTrack->pc_AllData + ((_pst_Event - EVE_gpst_CurrentTrack->pst_AllEvents) * ((_pst_Event->t.size >> 1) - 4));			
	}
	else
		return (char *) _pst_Event->p_Data + 4;		
}

#else
#define EVE_p_GetPdata(p)	((char *) p->p_Data + 4)
#endif

/*
 =======================================================================================================================
    La valeur de retour pour une clé qui n'aurait pas de données est 0)
 =======================================================================================================================
 */
short EVE_w_Event_InterpolationKey_GetFlags(EVE_tdst_Event *_pst_Event)
{
	/* Ugly Optimisation */
	if(_pst_Event->uw_NumFrames & 0x8000) return EVE_C_EventFlag_InterpolationKey;
	
	return _pst_Event->w_Flags;
}

/*
 =======================================================================================================================
    La valeur de retour pour une clé qui n'aurait pas de données est 0)
 =======================================================================================================================
 */
short EVE_w_Event_InterpolationKey_GetType(EVE_tdst_Event *_pst_Event)
{
	/* Ugly Optimisation */
	if(_pst_Event->uw_NumFrames & 0x8000) return (EVE_InterKeyType_Rotation_Quaternion + EVE_InterKey_CompressedQuaternion);

	/* t.size with 0x0001 set => size and type are valid in p_Data union */
	if(_pst_Event->t.size & 0x0001) return _pst_Event->t.type;
	
	if(!_pst_Event->p_Data) return 0;
	
	return(*((short *) _pst_Event->p_Data + 1));
}


/*
 =======================================================================================================================
    Aim:    Gets Translation of the given Event or NULL if not a Translation Track.
 =======================================================================================================================
 */
MATH_tdst_Vector *EVE_pst_Event_InterpolationKey_GetPos(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~*/
	int w_Type;
	/*~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
	if((w_Type == -1) || ((w_Type & EVE_InterKeyType_Translation_Mask) == 0)) return NULL;

	return (MATH_tdst_Vector *) EVE_p_GetPdata(_pst_Event);
}

/*
 =======================================================================================================================
    Aim:    Gets Rotation of the given Event or NULL if not a Rotation Track.
 =======================================================================================================================
 */
MATH_tdst_Matrix *EVE_pst_Event_InterpolationKey_GetRotation(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~*/
	char	*pc_Data;
	int		w_Type;
	/*~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
	if((w_Type == -1) || (!(w_Type & EVE_InterKeyType_Rotation_Matrix))) return NULL;

	pc_Data = EVE_p_GetPdata(_pst_Event);
	if(w_Type & EVE_InterKeyType_Translation_Mask)
	{
		pc_Data += sizeof(MATH_tdst_Vector);
		if((w_Type & EVE_InterKeyType_Translation_Mask) > 1)
		{
			pc_Data += sizeof(MATH_tdst_Vector);
			if((w_Type & EVE_InterKeyType_Translation_Mask) > 2) pc_Data += sizeof(MATH_tdst_Vector);
		}
	}

	return((MATH_tdst_Matrix *) pc_Data);
}

/*
 =======================================================================================================================
    Aim:
 =======================================================================================================================
 */
void EVE_pst_Event_InterpolationKey_CopyQuaternion(EVE_tdst_Event *_pst_Event, MATH_tdst_Quaternion *_pst_Q)
{
	EVE_Event_InterpolationKey_GetQuaternion(_pst_Event, _pst_Q);

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_GetCompressedQuaternion(EVE_tdst_Event *_pst_Event, MATH_tdst_CompressedQuaternion *_pst_CQ)
{
	/*~~~~~~~~~~~~~*/
	char	*pc_Data;
	int		w_Type;
	/*~~~~~~~~~~~~~*/

	if(!_pst_CQ) 
		return;

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
	if((w_Type == -1) || (!(w_Type & EVE_InterKeyType_Rotation_Quaternion))) 
		return;

	pc_Data = EVE_p_GetPdata(_pst_Event);
	if(w_Type & EVE_InterKeyType_Translation_Mask)
	{
		pc_Data += sizeof(MATH_tdst_Vector);
		if((w_Type & EVE_InterKeyType_Translation_Mask) > 1)
		{
			pc_Data += sizeof(MATH_tdst_Vector);
			if((w_Type & EVE_InterKeyType_Translation_Mask) > 2) pc_Data += sizeof(MATH_tdst_Vector);
		}
	}

#if defined (ACTIVE_EDITORS)
		MATH_CompressUnitQuaternion(_pst_CQ, (MATH_tdst_Quaternion *) pc_Data);
#else
	if(w_Type & EVE_InterKey_CompressedQuaternion)
	{
		_pst_CQ->x = ((MATH_tdst_CompressedQuaternion *) pc_Data)->x;
		_pst_CQ->y = ((MATH_tdst_CompressedQuaternion *) pc_Data)->y;
		_pst_CQ->z = ((MATH_tdst_CompressedQuaternion *) pc_Data)->z;

	}
	else
	{
		MATH_CompressUnitQuaternion(_pst_CQ, (MATH_tdst_Quaternion *) pc_Data);
	}
#endif
}
/*
 =======================================================================================================================
    Aim:    Gets Quaternion of the given Event or NULL if not a Rotation Track.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_GetQuaternion(EVE_tdst_Event *_pst_Event, MATH_tdst_Quaternion *_pst_Q)
{
	/*~~~~~~~~~~~~~*/
	char	*pc_Data;
	int		w_Type;
	/*~~~~~~~~~~~~~*/

	if(!_pst_Q) 
		return;


	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
	if((w_Type == -1) || (!(w_Type & EVE_InterKeyType_Rotation_Quaternion))) 
		return;

	pc_Data = EVE_p_GetPdata(_pst_Event);
	if(w_Type & EVE_InterKeyType_Translation_Mask)
	{
		pc_Data += sizeof(MATH_tdst_Vector);
		if((w_Type & EVE_InterKeyType_Translation_Mask) > 1)
		{
			pc_Data += sizeof(MATH_tdst_Vector);
			if((w_Type & EVE_InterKeyType_Translation_Mask) > 2) pc_Data += sizeof(MATH_tdst_Vector);
		}
	}

#if defined (ACTIVE_EDITORS)
	MATH_CopyQuaternion(_pst_Q, (MATH_tdst_Quaternion *) pc_Data);
#else
	if(w_Type & EVE_InterKey_CompressedQuaternion)
	{
		MATH_UncompressUnitQuaternion(_pst_Q, (MATH_tdst_CompressedQuaternion *) pc_Data);
	}
	else
	{
		MATH_CopyQuaternion(_pst_Q, (MATH_tdst_Quaternion *) pc_Data);
	}
#endif
}

/*
 =======================================================================================================================
    Aim:    (Event IK/Next Event No IK) or (Event No IK/Next Event IK) have this next value. Event IK are in the Anim
            Reference Coordinate System and No IK Event are in their father Coordinate System. We stop another Matrix
            to avoid massive Matrix Multiplication during Real Time IK.
 =======================================================================================================================
 */
void *EVE_pst_Event_InterpolationKey_GetNextValue(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~*/
	char	*pc_Data;
	int		w_Type;
	/*~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
	if((w_Type == -1) || (!(w_Type & EVE_InterKeyType_HasNextValue))) return NULL;

	pc_Data = EVE_p_GetPdata(_pst_Event);
	if(w_Type & EVE_InterKeyType_Translation_Mask)
	{
		pc_Data += sizeof(MATH_tdst_Vector);
		if((w_Type & EVE_InterKeyType_Translation_Mask) > 1)
		{
			pc_Data += sizeof(MATH_tdst_Vector);
			if((w_Type & EVE_InterKeyType_Translation_Mask) > 2) pc_Data += sizeof(MATH_tdst_Vector);
		}
	}
	else if(w_Type & EVE_InterKeyType_Rotation_Matrix)
	{
		pc_Data += sizeof(MATH_tdst_Matrix);
	}
	else if(w_Type & EVE_InterKeyType_Rotation_Quaternion)
	{
		pc_Data += sizeof(MATH_tdst_Quaternion);
	}

	return((void *) pc_Data);
}

/*
 =======================================================================================================================
    Aim:    Get Time Data of the current event or NULL if not a Time Track.
 =======================================================================================================================
 */
float *EVE_pf_Event_InterpolationKey_GetTime(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~*/
	char	*pc_Data;
	int		w_Type;
	/*~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
	if((w_Type == -1) || (!(w_Type & EVE_InterKeyType_Time))) return NULL;

	pc_Data = EVE_p_GetPdata(_pst_Event);
	if(w_Type & EVE_InterKeyType_Translation_Mask)
	{
		pc_Data += sizeof(MATH_tdst_Vector);
		if((w_Type & EVE_InterKeyType_Translation_Mask) > 1)
		{
			pc_Data += sizeof(MATH_tdst_Vector);
			if((w_Type & EVE_InterKeyType_Translation_Mask) > 2) pc_Data += sizeof(MATH_tdst_Vector);
		}
	}

	if(w_Type & EVE_InterKeyType_Rotation_Matrix) pc_Data += sizeof(MATH_tdst_Matrix);
	if(w_Type & EVE_InterKeyType_Rotation_Quaternion) pc_Data += sizeof(MATH_tdst_Quaternion);

	return((float *) pc_Data);
}

/*
 =======================================================================================================================
    Aim:    Allocation depending on the Type. (May also desallocate if Event already exist)

    Note:   This function is done so that we can have many types in the same Event, which is not very well used.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_AllocData(EVE_tdst_Event *_pst_Event, short _w_Type, int _i_TimeInter)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	short	w_Size, w_Offset;
	char	*pc_OldData;
	LONG	l_Offset[10];
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pc_OldData = (char *) _pst_Event->p_Data;
	L_memset(l_Offset, 0, 10 * sizeof(LONG));

	/* get old data */
	if(pc_OldData)
	{
		w_Size = *((short *) _pst_Event->p_Data + 1);
		w_Offset = 4;

		if(_w_Type == -1)
			_w_Type = w_Size;
		else
			_w_Type |= w_Size & EVE_InterKeyType_BlockedForIK;

		if((w_Size & EVE_InterKeyType_Translation_Mask) > 0)
		{
			l_Offset[0] = w_Offset;
			w_Offset += sizeof(MATH_tdst_Vector);
		}

		if((w_Size & EVE_InterKeyType_Translation_Mask) > 1)
		{
			l_Offset[1] = w_Offset;
			w_Offset += sizeof(MATH_tdst_Vector);
		}

		if((w_Size & EVE_InterKeyType_Translation_Mask) > 2)
		{
			l_Offset[2] = w_Offset;
			w_Offset += sizeof(MATH_tdst_Vector);
		}

		if(w_Size & EVE_InterKeyType_Rotation_Matrix)
		{
			l_Offset[3] = w_Offset;
			w_Offset += sizeof(MATH_tdst_Matrix);
		}

		if(w_Size & EVE_InterKeyType_Rotation_Quaternion)
		{
			l_Offset[4] = w_Offset;
			w_Offset += sizeof(MATH_tdst_Quaternion);
		}

		if(w_Size & EVE_InterKeyType_HasNextValue)
		{
			if((w_Size & EVE_InterKeyType_Translation_Mask) > 0)
			{
				l_Offset[7] = w_Offset;
				w_Offset += sizeof(MATH_tdst_Vector);
			}

			if(w_Size & EVE_InterKeyType_Rotation_Matrix)
			{
				l_Offset[8] = w_Offset;
				w_Offset += sizeof(MATH_tdst_Matrix);
			}

			if(w_Size & EVE_InterKeyType_Rotation_Quaternion)
			{
				l_Offset[9] = w_Offset;
				w_Offset += sizeof(MATH_tdst_Quaternion);
			}
		}
		else
		{
			l_Offset[7] = l_Offset[0];
			l_Offset[8] = l_Offset[3];
			l_Offset[9] = l_Offset[4];
		}

		if(w_Size & EVE_InterKeyType_Time)
		{
			if(_i_TimeInter == -1) _i_TimeInter = (*(int *) (pc_OldData + w_Offset));

			l_Offset[5] = w_Offset;
			w_Offset += 8 * (*(int *) (pc_OldData + w_Offset));
			l_Offset[6] = w_Offset;
		}
	}

	/* create new buffer */
	w_Size = 4;
	if((_w_Type & EVE_InterKeyType_Translation_Mask) > 0) w_Size += sizeof(MATH_tdst_Vector);
	if((_w_Type & EVE_InterKeyType_Translation_Mask) > 1) w_Size += sizeof(MATH_tdst_Vector);
	if((_w_Type & EVE_InterKeyType_Translation_Mask) > 2) w_Size += sizeof(MATH_tdst_Vector);
	if(_w_Type & EVE_InterKeyType_Rotation_Matrix) w_Size += sizeof(MATH_tdst_Matrix);
	if(_w_Type & EVE_InterKeyType_Rotation_Quaternion) w_Size += sizeof(MATH_tdst_Quaternion);

	if(_w_Type & EVE_InterKeyType_HasNextValue)
	{
		if((_w_Type & EVE_InterKeyType_Translation_Mask) > 0) w_Size += sizeof(MATH_tdst_Vector);
		if(_w_Type & EVE_InterKeyType_Rotation_Matrix) w_Size += sizeof(MATH_tdst_Matrix);
		if(_w_Type & EVE_InterKeyType_Rotation_Quaternion) w_Size += sizeof(MATH_tdst_Quaternion);
	}

	if(_w_Type & EVE_InterKeyType_Time)
	{
		w_Offset = w_Size;
		w_Size += 8 * _i_TimeInter;
	}

	_pst_Event->p_Data = MEM_p_Alloc(w_Size);
	L_memset(_pst_Event->p_Data, 0, w_Size);

	*(short *) _pst_Event->p_Data = (short) w_Size;
	*((short *) _pst_Event->p_Data + 1) = _w_Type;

	/* restore old data */
	if(pc_OldData)
	{
		w_Size = 4;
		if((_w_Type & EVE_InterKeyType_Translation_Mask) > 0)
		{
			if(l_Offset[0])
				L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[0], sizeof(MATH_tdst_Vector));
			w_Size += sizeof(MATH_tdst_Vector);
		}

		if((_w_Type & EVE_InterKeyType_Translation_Mask) > 1)
		{
			if(l_Offset[1])
				L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[1], sizeof(MATH_tdst_Vector));
			else if(l_Offset[0])
				L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[0], sizeof(MATH_tdst_Vector));
			w_Size += sizeof(MATH_tdst_Vector);
		}

		if((_w_Type & EVE_InterKeyType_Translation_Mask) > 2)
		{
			if(l_Offset[2])
				L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[2], sizeof(MATH_tdst_Vector));
			else if(l_Offset[1])
				L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[1], sizeof(MATH_tdst_Vector));
			else if(l_Offset[0])
				L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[0], sizeof(MATH_tdst_Vector));
			w_Size += sizeof(MATH_tdst_Vector);
		}

		if(_w_Type & EVE_InterKeyType_Rotation_Matrix)
		{
			if(l_Offset[3])
				L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[3], sizeof(MATH_tdst_Matrix));
			w_Size += sizeof(MATH_tdst_Matrix);
		}

		if(_w_Type & EVE_InterKeyType_Rotation_Quaternion)
		{
			if(l_Offset[4])
				L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[4], sizeof(MATH_tdst_Quaternion));
			w_Size += sizeof(MATH_tdst_Quaternion);
		}

		if(_w_Type & EVE_InterKeyType_HasNextValue)
		{
			if((_w_Type & EVE_InterKeyType_Translation_Mask) > 0)
			{
				if(l_Offset[7])
					L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[0], sizeof(MATH_tdst_Vector));
				w_Size += sizeof(MATH_tdst_Vector);
			}

			if(_w_Type & EVE_InterKeyType_Rotation_Matrix)
			{
				if(l_Offset[8])
					L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[3], sizeof(MATH_tdst_Matrix));
				w_Size += sizeof(MATH_tdst_Matrix);
			}

			if(_w_Type & EVE_InterKeyType_Rotation_Quaternion)
			{
				if(l_Offset[9])
					L_memcpy
					(
						(char *) _pst_Event->p_Data + w_Size,
						pc_OldData + l_Offset[4],
						sizeof(MATH_tdst_Quaternion)
					);
				w_Size += sizeof(MATH_tdst_Quaternion);
			}
		}

		if(_w_Type & EVE_InterKeyType_Time)
		{
			if(l_Offset[5])
			{
				if(*(int *) pc_OldData + l_Offset[5] > _i_TimeInter)
					L_memcpy((char *) _pst_Event->p_Data + w_Size, pc_OldData + l_Offset[5], _i_TimeInter * 8);
				else
					L_memcpy
					(
						(char *) _pst_Event->p_Data + w_Size,
						pc_OldData + l_Offset[5],
						(*(int *) pc_OldData + l_Offset[5]) * 8
					);
			}

			w_Size += _i_TimeInter * 8;
		}
	}

	if(_w_Type & EVE_InterKeyType_Time)
	{
		*(int *) ((char *) _pst_Event->p_Data + w_Offset) = _i_TimeInter;
	}

	if(pc_OldData) MEM_Free(pc_OldData);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Changes the Type of Data.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_ChangeTypeWithoutChangingData(EVE_tdst_Event *_pst_Event, short _w_Type)
{
	if(!_pst_Event->p_Data) return;
	*((short *) _pst_Event->p_Data + 1) = _w_Type;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
USHORT EVE_w_Event_ComputeCompressType(EVE_tdst_Event *_pst_Event, EVE_tdst_Event *_pst_PreviousEvent)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT	uw_Type, uw_PreviousType;
	USHORT	uw_CompressType;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_CompressType = 0;

	uw_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
	uw_PreviousType = (_pst_PreviousEvent) ? EVE_w_Event_InterpolationKey_GetType(_pst_PreviousEvent) : -1;

	/* Remove Compress Flags. */
	uw_Type &= ~EVE_InterKey_CompressedQuaternionMask;

	if(uw_Type == EVE_InterKeyType_Rotation_Quaternion)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Quaternion			st_Q, Quat, QuatOrig;
		MATH_tdst_CompressedQuaternion	st_CompressedQ ;
		float							f_Error;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		EVE_Event_InterpolationKey_GetQuaternion(_pst_Event, &st_Q);
		MATH_CompressUnitQuaternion(&st_CompressedQ, &st_Q);

		MATH_CopyQuaternion(&QuatOrig, &st_Q);
		if(st_Q.w < 0.0f)
		{
			MATH_NegQuaternion(&QuatOrig);
		}

		MATH_UncompressUnitQuaternion(&Quat, &st_CompressedQ);
		f_Error = (float) fabs(Quat.w - QuatOrig.w);

		if(f_Error < 1E-4)
		{
			uw_CompressType |= EVE_InterKey_CompressedQuaternion;
		}
	}

	return uw_CompressType;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_Save
(
	EVE_tdst_Event	*_pst_Event,
	EVE_tdst_Event	*_pst_PreviousEvent,
	USHORT			_uw_OptimFlags
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Size, uw_Size;
	USHORT	uw_CompressType, uw_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Event->p_Data == NULL)
	{
		ERR_X_Warning(0, "Interpolation Key with no Data found.", NULL);
		ul_Size = 0;
		SAV_Buffer(&ul_Size, 4);
		return;
	}

	uw_Size = *(short *) _pst_Event->p_Data;
	uw_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);

	/* Remove Previous Compress Flags. */
	uw_Type &= ~EVE_InterKey_CompressedQuaternionMask;

	uw_CompressType = EVE_w_Event_ComputeCompressType(_pst_Event, _pst_PreviousEvent);

	uw_Type += uw_CompressType;

	if(uw_Type & EVE_InterKeyType_Rotation_Quaternion)
	{
		if(uw_CompressType & EVE_InterKey_CompressedQuaternion)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_CompressedQuaternion	st_CompressedQ ;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			EVE_Event_InterpolationKey_GetCompressedQuaternion(_pst_Event, &st_CompressedQ);

			uw_Type |= EVE_InterKey_CompressedQuaternion;
			uw_Size = sizeof(MATH_tdst_CompressedQuaternion) + 4;
			if(_uw_OptimFlags & EVE_C_Track_SameSize)
			{
				if(!_pst_PreviousEvent) SAV_Buffer(&uw_Size, 2);
			}
			else
				SAV_Buffer(&uw_Size, 2);

			if(_uw_OptimFlags & EVE_C_Track_SameType)
			{
				if(!_pst_PreviousEvent) SAV_Buffer(&uw_Type, 2);
			}
			else
				SAV_Buffer(&uw_Type, 2);

			SAV_Buffer(&st_CompressedQ, sizeof(MATH_tdst_CompressedQuaternion));
			return;
		}


		if(_uw_OptimFlags & EVE_C_Track_SameSize)
		{
			if(!_pst_PreviousEvent) SAV_Buffer(&uw_Size, 2);
		}
		else
			SAV_Buffer(&uw_Size, 2);

		if(_uw_OptimFlags & EVE_C_Track_SameType)
		{
			if(!_pst_PreviousEvent) SAV_Buffer(&uw_Type, 2);
		}
		else
			SAV_Buffer(&uw_Type, 2);
		SAV_Buffer(((char *) _pst_Event->p_Data) + 4, uw_Size - 4);
		return;
	}

	if(uw_Type == EVE_InterKeyType_Translation_0)
	{
		if(_uw_OptimFlags & EVE_C_Track_SameSize)
		{
			if(!_pst_PreviousEvent) SAV_Buffer(&uw_Size, 2);
		}
		else
			SAV_Buffer(&uw_Size, 2);

		if(_uw_OptimFlags & EVE_C_Track_SameType)
		{
			if(!_pst_PreviousEvent) SAV_Buffer(&uw_Type, 2);
		}
		else
			SAV_Buffer(&uw_Type, 2);
		SAV_Buffer(((char *) _pst_Event->p_Data) + 4, uw_Size - 4);
		return;
	}

	if(_uw_OptimFlags & EVE_C_Track_SameSize)
	{
		if(!_pst_PreviousEvent) SAV_Buffer(&uw_Size, 2);
	}
	else
		SAV_Buffer(&uw_Size, 2);

	if(_uw_OptimFlags & EVE_C_Track_SameType)
	{
		if(!_pst_PreviousEvent) SAV_Buffer(&uw_Type, 2);
	}
	else
		SAV_Buffer(&uw_Type, 2);
	SAV_Buffer(((char *) _pst_Event->p_Data) + 4, uw_Size - 4);
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */

int EVE_Event_InterpolationKey_Load
(
	EVE_tdst_Event	*_pst_Event,
	EVE_tdst_Event	*_pst_PreviousEvent,
	char			*_pc_Buffer,
	char			*_pc_DestBuffer,
	EVE_tdst_Track	*_pst_Track,
	USHORT			_uw_OptimFlags
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			*pc_Buf;
	short			w_Size, w_Type;
	char			*p_Data;
	BOOL			b_SameSize, b_SameType, b_SameFlags;
	BOOL			b_SameAll;
	BOOL			b_UglyOptim;
	BOOL			b_FirstEvent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Buf = _pc_Buffer;

	b_SameSize = (_uw_OptimFlags & EVE_C_Track_SameSize);
	b_SameType = (_uw_OptimFlags & EVE_C_Track_SameType);
	b_SameFlags = (_uw_OptimFlags & EVE_C_Track_SameFlags);

	b_SameAll = b_SameSize && b_SameType && b_SameFlags;

	b_FirstEvent = (_pst_Track->pst_AllEvents == _pst_Event);

	if(b_SameSize && !(_uw_OptimFlags & EVE_C_Track_FirstEvent))
	{
		if(!(_pst_Track->pst_AllEvents[0].uw_NumFrames & 0x8000)) // Not ugly optim
		{
#ifdef ACTIVE_EDITORS
			w_Size = *((short *) _pst_Track->pst_AllEvents[0].p_Data);
#else
			w_Size = _pst_Track->pst_AllEvents[0].t.size >> 1;
#endif
		}
	}
	else
	{
		w_Size = LOA_ReadUShort(&pc_Buf);		
	}

	if(b_SameType  && !(_uw_OptimFlags & EVE_C_Track_FirstEvent))
	{
#ifndef ACTIVE_EDITORS
		if(b_SameSize)
		{
			if(_pst_Track->pst_AllEvents[0].uw_NumFrames & 0x8000)
				w_Type = EVE_InterKeyType_Rotation_Quaternion | EVE_InterKey_CompressedQuaternion;
			else
				w_Type = _pst_Track->pst_AllEvents[0].t.type;
		}
		else
#endif	// ACTIVE_EDITORS
			w_Type = EVE_w_Event_InterpolationKey_GetType(&_pst_Track->pst_AllEvents[0]);
	}
	else
	{
		w_Type = LOA_ReadUShort(&pc_Buf);				
	}

	b_UglyOptim = b_SameAll && (w_Type & EVE_InterKey_CompressedQuaternion);


#ifndef ACTIVE_EDITORS
	_pst_Event->p_Data = _pc_DestBuffer;
#endif



	//Anti Bug ... SameSize but not SameType. That means track with compressed AND not compressed Q. We have to save them uncompressed.
	if(w_Type & EVE_InterKeyType_Rotation_Mask)
	{
		if((w_Type & (EVE_InterKeyType_Rotation_Quaternion | EVE_InterKey_CompressedQuaternion)) == EVE_InterKeyType_Rotation_Quaternion)
		{
			if(w_Size != (sizeof(MATH_tdst_Quaternion) + 4))
				w_Size = (sizeof(MATH_tdst_Quaternion) + 4);
		}
	}

	if(w_Type & (EVE_InterKey_CompressedQuaternionMask))
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		MATH_tdst_Quaternion				Q = { 0.0f, 0.0f, 0.0f, 0.0f };
		MATH_tdst_CompressedQuaternion		st_CompressedQ  ONLY_PSX2_ALIGNED(2);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if(w_Type & EVE_InterKey_CompressedQuaternion)
		{
			LOA_ReadCompressedQuaternion(&pc_Buf, &st_CompressedQ);
		}
		else
		{
			LINK_PrintStatusMsg("[Bug] Ultra compression found.");
		}

		MATH_UncompressUnitQuaternion(&Q, &st_CompressedQ);

#ifdef ACTIVE_EDITORS
			w_Size = sizeof(MATH_tdst_Quaternion) + 4;
#else
			if(b_SameType)
				w_Size = sizeof(MATH_tdst_CompressedQuaternion) + 4;
			else
				w_Size = sizeof(MATH_tdst_Quaternion) + 4;
#endif	// ACTIVE_EDITORS

#ifndef ACTIVE_EDITORS
		if(b_UglyOptim)
		{
			_pst_Event->uw_NumFrames |= 0x8000;	// Flag to indicate the ugly optimisation 
			L_memcpy((char *)_pst_Event + 2, &st_CompressedQ, sizeof(MATH_tdst_CompressedQuaternion));

			return(pc_Buf - _pc_Buffer);

		}
		else
		{
			if(_uw_OptimFlags & EVE_C_Track_SameSize)
			{
				_pst_Event->t.size = (w_Size << 1) | 0x0001;
				_pst_Event->t.type = b_SameType ? w_Type : w_Type & ~EVE_InterKey_CompressedQuaternion;
				if(b_SameType)
					L_memcpy(_pc_DestBuffer, &st_CompressedQ, sizeof(MATH_tdst_CompressedQuaternion));
				else
					L_memcpy(_pc_DestBuffer, &Q, sizeof(MATH_tdst_Quaternion));
				return(pc_Buf - _pc_Buffer);
			}
		}
#else // ACTIVE_EDITORS
		_pst_Event->p_Data = MEM_p_Alloc(w_Size);

		LOA_ul_FileTypeSize[31] += w_Size + 8;		//8 = Header of each allocation chunck
#endif // ACTIVE_EDITORS
		p_Data = (char *) _pst_Event->p_Data;

		*(short *) p_Data = w_Size;
		p_Data += 2;

		*(short *) p_Data = b_SameType ? w_Type : w_Type & ~EVE_InterKey_CompressedQuaternion;
		p_Data += 2;

#ifdef ACTIVE_EDITORS
		L_memcpy(p_Data, &Q, sizeof(MATH_tdst_Quaternion));
#else
		if(b_SameType)
			L_memcpy(p_Data, &st_CompressedQ, sizeof(MATH_tdst_CompressedQuaternion));
		else
			L_memcpy(p_Data, &Q, sizeof(MATH_tdst_Quaternion));
#endif	// ACTIVE_EDITORS

		return(pc_Buf - _pc_Buffer);
	} // if(w_Type & (EVE_InterKey_CompressedQuaternionMask))


	/*
	 * Process "Standard" Interpolation Keys (e.g. translation & rotation) as well as
	 * the time sub type.
	 */
	if(w_Type & (EVE_InterKeyType_Translation_Mask | EVE_InterKeyType_Rotation_Mask))
	{
		/* Configure the output buffer */
#ifndef ACTIVE_EDITORS
		if(_uw_OptimFlags & EVE_C_Track_SameSize)
		{
			_pst_Event->t.size = (w_Size << 1) | 0x0001;
			_pst_Event->t.type = w_Type;
			p_Data = _pc_DestBuffer;
		}
		else
#else
			_pst_Event->p_Data = MEM_p_Alloc(w_Size);

			LOA_ul_FileTypeSize[31] += w_Size + 8;	//8 = Header of each allocation chunck
#endif
		{
			p_Data = (char *) _pst_Event->p_Data;
			*(short *) p_Data = w_Size;
			p_Data += 2;
			*(short *) p_Data = w_Type;
			p_Data += 2;
		}

		/*
		 * Decrease the specified size if the Size or Type is implicitely known (i.e. not
		 * read from disk)
		 */
		if(b_SameSize && !(_uw_OptimFlags & EVE_C_Track_FirstEvent)) w_Size -= 2;
		if(b_SameType && !(_uw_OptimFlags & EVE_C_Track_FirstEvent)) w_Size -= 2;

		if(w_Type & EVE_InterKeyType_Translation_Mask)
		{
			/*
			 * Read the vectors from the buffer Yes, it is normal that there are no break;
			 * after each case depending on the type there are more vectors (_2=3, _1=2, _0
			 * =1)
			 */
			switch(w_Type & EVE_InterKeyType_Translation_Mask)
			{
#ifdef JADEFUSION //& en +
			case EVE_InterKeyType_Translation_2:
				LOA_ReadVector(&pc_Buf, ((MATH_tdst_Vector *&) p_Data)++);
			case EVE_InterKeyType_Translation_1:
				LOA_ReadVector(&pc_Buf, ((MATH_tdst_Vector *&) p_Data)++);
			case EVE_InterKeyType_Translation_0:
				LOA_ReadVector(&pc_Buf, ((MATH_tdst_Vector *&) p_Data)++);
#else
			case EVE_InterKeyType_Translation_2:
				LOA_ReadVector(&pc_Buf, ((MATH_tdst_Vector *) p_Data)++);
			case EVE_InterKeyType_Translation_1:
				LOA_ReadVector(&pc_Buf, ((MATH_tdst_Vector *) p_Data)++);
			case EVE_InterKeyType_Translation_0:
				LOA_ReadVector(&pc_Buf, ((MATH_tdst_Vector *) p_Data)++);
#endif
			}
		}
		else if(w_Type & EVE_InterKeyType_Rotation_Mask)
		{
			switch(w_Type & EVE_InterKeyType_Rotation_Mask)
			{
			case EVE_InterKeyType_Rotation_Matrix:
#ifdef JADEFUSION
				LOA_ReadMatrix(&pc_Buf, ((MATH_tdst_Matrix *&) p_Data)++);
#else
				LOA_ReadMatrix(&pc_Buf, ((MATH_tdst_Matrix *) p_Data)++);
#endif
				break;
			case EVE_InterKeyType_Rotation_Quaternion:
#ifdef JADEFUSION
				LOA_ReadQuaternion(&pc_Buf, ((MATH_tdst_Quaternion *&) p_Data)++);
#else
				LOA_ReadQuaternion(&pc_Buf, ((MATH_tdst_Quaternion *) p_Data)++);
#endif
				break;
			}
		}

		if((w_Type & EVE_InterKeyType_Time) && ((pc_Buf - _pc_Buffer) < w_Size))
		{
			/*
			 * EVE_InterKeyType_Time Format Is found as a subtype of an interpolation key, it
			 * edits the time key for the event... in the editor, it is possible to draw a
			 * curve that describes the evolution of time for this event The format for the
			 * control points is as follows:
			 */

			/*
			 * INT: number of keys N float: factor at time 0 for( 1..N) float Time index N
			 * float Factor for time index N Thus, a EVE_InterKeyType_Time data is read with 1
			 * INT & N*2 -1 floats
			 */

			/*~~~~~~~~~~*/
			int i, iCount;
			/*~~~~~~~~~~*/

			/* Read the number of keys in iCount AND in the data buffer */
#ifdef JADEFUSION
			iCount = _LOA_ReadInt(&pc_Buf, ((INT *&) p_Data)++, LOA_eBinEngineData);
#else
			iCount = _LOA_ReadInt(&pc_Buf, ((INT *) p_Data)++, LOA_eBinEngineData);
#endif
			/* Read all the keys in the data buffer */
			for(i = 0; i < (iCount * 2 - 1); ++i)
			{
#ifdef JADEFUSION
				_LOA_ReadFloat(&pc_Buf, ((float *&) p_Data)++, LOA_eBinEngineData);
#else
				_LOA_ReadFloat(&pc_Buf, ((float *) p_Data)++, LOA_eBinEngineData);
#endif
			}
		}

		ERR_X_ErrorAssert
		(
			((pc_Buf - _pc_Buffer) == w_Size),
			"EVE_Event_InterpolationKey_Load size mismatch while loading.",
			""
		);
		
			
		return(pc_Buf - _pc_Buffer);
	}

	ERR_X_ErrorAssert(0, "EVE_Event_InterpolationKey_Load Unsupported data type", "");
	return(pc_Buf - _pc_Buffer);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_Resolve(EVE_tdst_Event *_pst_Event)
{
}
/*
 =======================================================================================================================
 =======================================================================================================================
*/
void EVE_Anim_InterpolationKey_CopyMatrix(MATH_tdst_Matrix *_pst_Matrix, EVE_tdst_Event *_pst_Event, BOOL _b_Next)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Quaternion			st_Q;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EVE_Event_InterpolationKey_GetQuaternion(_pst_Event, &st_Q);
	MATH_ConvertQuaternionToMatrix(_pst_Matrix, &st_Q);
	MATH_SetRotationType(_pst_Matrix);
}

/*
 =======================================================================================================================
    Aim:    Copy Matrix (or Compute it if Quternion) of the current Rotation/Quaternion Event.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_CopyMatrix(MATH_tdst_Matrix *_pst_Matrix, EVE_tdst_Event *_pst_Event, BOOL _b_Next)
{
	/*~~~~~~~~~~~~~~*/
	short	w_Type;
	BOOL	b_Symetry;
	/*~~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);

	if(!(w_Type & EVE_InterKeyType_Rotation_Mask)) return;

	b_Symetry = EVE_w_Event_InterpolationKey_GetFlags(_pst_Event) & EVE_C_EventFlag_Symetric;

	if(w_Type & EVE_InterKeyType_Rotation_Matrix)
	{
		if(_b_Next)
		{
			MATH_CopyMatrix(_pst_Matrix, (MATH_tdst_Matrix *) EVE_pst_Event_InterpolationKey_GetNextValue(_pst_Event));
		}
		else
			MATH_CopyMatrix(_pst_Matrix, EVE_pst_Event_InterpolationKey_GetRotation(_pst_Event));

		if(b_Symetry)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Quaternion	st_Q;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_ConvertMatrixToQuaternion(&st_Q, _pst_Matrix);
			st_Q.y = -st_Q.y;
			st_Q.z = -st_Q.z;
			MATH_ConvertQuaternionToMatrix(_pst_Matrix, &st_Q);
		}

		return;
	}

	if(w_Type & EVE_InterKeyType_Rotation_Quaternion)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Quaternion	*pst_Q;
		MATH_tdst_Quaternion	st_Q;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_b_Next)
			pst_Q = (MATH_tdst_Quaternion *) EVE_pst_Event_InterpolationKey_GetNextValue(_pst_Event);
		else
		{
			EVE_Event_InterpolationKey_GetQuaternion(_pst_Event, &st_Q);
			pst_Q = &st_Q;
		}

		if(b_Symetry)
		{
			pst_Q->y = -pst_Q->y;
			pst_Q->z = -pst_Q->z;
		}

		MATH_ConvertQuaternionToMatrix(_pst_Matrix, pst_Q);

		if(b_Symetry)
		{
			pst_Q->y = -pst_Q->y;
			pst_Q->z = -pst_Q->z;
		}

		MATH_SetCorrectType(_pst_Matrix);
	}
}

/*
 =======================================================================================================================
    Aim:    We have computed the matrix of one event. We copy it into the Event structure.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_SetMatrix(MATH_tdst_Matrix *_pst_Matrix, EVE_tdst_Event *_pst_Event)
{
	if(!(EVE_w_Event_InterpolationKey_GetType(_pst_Event) & EVE_InterKeyType_Rotation_Mask)) return;

	switch(EVE_w_Event_InterpolationKey_GetType(_pst_Event))
	{
	case EVE_InterKeyType_Rotation_Matrix:
		MATH_CopyMatrix(EVE_pst_Event_InterpolationKey_GetRotation(_pst_Event), _pst_Matrix);
		return;

	case EVE_InterKeyType_Rotation_Quaternion:
		{
			MATH_tdst_Quaternion			st_Q;
			int								w_Type;
			char							*pc_Data;

			w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
			pc_Data = EVE_p_GetPdata(_pst_Event);

			MATH_ConvertMatrixToQuaternion(&st_Q, _pst_Matrix);

#ifdef ACTIVE_EDITORS
			MATH_CopyQuaternion((MATH_tdst_Quaternion *) pc_Data, &st_Q);
#else
			if(w_Type & EVE_InterKey_CompressedQuaternion)
				MATH_CompressUnitQuaternion((MATH_tdst_CompressedQuaternion *) pc_Data, &st_Q);
			else
				MATH_CopyQuaternion((MATH_tdst_Quaternion *) pc_Data, &st_Q);
#endif
		}

		return;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Anim_ComputeInterpoledMatrix
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Matrix	*_pst_Matrix,
	EVE_tdst_Event		*_pst_EventA,
	EVE_tdst_Event		*_pst_EventB,
	float				_f_Time
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Quaternion	st_Q1, st_Q2;
	float					t;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(EVE_NumFrames(_pst_EventA))
	{

		if(_f_Time >= EVE_FrameToTime(EVE_NumFrames(_pst_EventA)))
		{
			EVE_Anim_InterpolationKey_CopyMatrix(_pst_Matrix, _pst_EventB, 0);
		}
		else
		{
			EVE_Event_InterpolationKey_GetQuaternion(_pst_EventA, &st_Q1);
			EVE_Event_InterpolationKey_GetQuaternion(_pst_EventB, &st_Q2);
							
			t = _f_Time / EVE_FrameToTime(EVE_NumFrames(_pst_EventA));

			MATH_QuaternionBlend(_pst_Matrix, &st_Q1, &st_Q2, t, 1);
		}
	}
	else
		EVE_Anim_InterpolationKey_CopyMatrix(_pst_Matrix, _pst_EventA, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_ComputeInterpoledMatrix
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Matrix	*_pst_Matrix,
	EVE_tdst_Event		*_pst_EventA,
	EVE_tdst_Event		*_pst_EventB,
	float				_f_Time
)	
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Quaternion	*pst_Q1, *pst_Q2;
	MATH_tdst_Quaternion	st_Q1, st_Q2, st_SymQ1, st_SymQ2;
	float					t;
	short					w_TypeA, w_TypeB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	w_TypeA = EVE_w_Event_InterpolationKey_GetType(_pst_EventA);
	w_TypeB = EVE_w_Event_InterpolationKey_GetType(_pst_EventB);

	if((w_TypeA == -1) || !(w_TypeA & EVE_InterKeyType_Rotation_Mask)) return;

	if
	(
		(EVE_NumFrames(_pst_EventA))
	&&	(w_TypeB != -1)
	&&	(w_TypeB & EVE_InterKeyType_Rotation_Mask)
	&&	!(EVE_w_Event_InterpolationKey_GetFlags(_pst_EventA) & EVE_C_EventFlag_NoInterpolation)
	)
	{
		if(_f_Time >= EVE_FrameToTime(EVE_NumFrames(_pst_EventA)))
		{
			if(w_TypeA & EVE_InterKeyType_HasNextValue)
				EVE_Event_InterpolationKey_CopyMatrix(_pst_Matrix, _pst_EventA, 1);
			else
				EVE_Event_InterpolationKey_CopyMatrix(_pst_Matrix, _pst_EventB, 0);
		}
		else
		{
			/* get quaternion of current event ( starting orientation ) */
			if(w_TypeA & EVE_InterKeyType_Rotation_Matrix)
			{
				MATH_ConvertMatrixToQuaternion(&st_Q1, EVE_pst_Event_InterpolationKey_GetRotation(_pst_EventA));
				pst_Q1 = &st_Q1;
			}
			else
			{
				EVE_Event_InterpolationKey_GetQuaternion(_pst_EventA, &st_Q1);
				pst_Q1 = &st_Q1;
			}

			/* symetrize starting orientation if needed */
			if(EVE_w_Event_InterpolationKey_GetFlags(_pst_EventA) & EVE_C_EventFlag_Symetric)
			{
				st_SymQ1.w = pst_Q1->w;
				st_SymQ1.x = pst_Q1->x;
				st_SymQ1.y = -pst_Q1->y;
				st_SymQ1.z = -pst_Q1->z;

				pst_Q1 = &st_SymQ1;
			}

			if(w_TypeA & EVE_InterKeyType_HasNextValue)
			{
				if(w_TypeA & EVE_InterKeyType_Rotation_Matrix)
				{
					MATH_ConvertMatrixToQuaternion
					(
						&st_Q2,
						(MATH_tdst_Matrix *) EVE_pst_Event_InterpolationKey_GetNextValue(_pst_EventA)
					);
					pst_Q2 = &st_Q2;
				}
				else
				{
					pst_Q2 = (MATH_tdst_Quaternion *) EVE_pst_Event_InterpolationKey_GetNextValue(_pst_EventA);
				}
			}
			else
			{
				if(w_TypeB & EVE_InterKeyType_Rotation_Matrix)
				{
					MATH_ConvertMatrixToQuaternion(&st_Q2, EVE_pst_Event_InterpolationKey_GetRotation(_pst_EventB));
					pst_Q2 = &st_Q2;
				}
				else
				{
					EVE_Event_InterpolationKey_GetQuaternion(_pst_EventB, &st_Q2);
					pst_Q2 = &st_Q2;
				}
			}

			/* symetrize ending orientation if needed */
			if(EVE_w_Event_InterpolationKey_GetFlags(_pst_EventB) & EVE_C_EventFlag_Symetric)
			{
				st_SymQ2.w = pst_Q2->w;
				st_SymQ2.x = pst_Q2->x;
				st_SymQ2.y = -pst_Q2->y;
				st_SymQ2.z = -pst_Q2->z;
				pst_Q2 = &st_SymQ2;
			}

			t = _f_Time / EVE_FrameToTime(EVE_NumFrames(_pst_EventA));

			MATH_QuaternionBlend(_pst_Matrix, pst_Q1, pst_Q2, t, 1);
		}
	}
	else
		EVE_Event_InterpolationKey_CopyMatrix(_pst_Matrix, _pst_EventA, 0);
}

extern EVE_tdst_ListTracks	*EVE_gpst_CurrentListTracks;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVE_Event_InterpolationKey_IsChild(OBJ_tdst_GameObject *_pst_GAO)
{
	if(!OBJ_b_TestIdentityFlag(_pst_GAO, OBJ_C_IdentityFlag_Hierarchy)) return FALSE;
	if(OBJ_b_TestIdentityFlag(_pst_GAO, OBJ_C_IdentityFlag_Bone)) return TRUE;
#ifdef ACTIVE_EDITORS
	if(_pst_GAO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject) return TRUE;
#endif
	if(!OBJ_b_TestIdentityFlag(_pst_GAO, OBJ_C_IdentityFlag_Anims)) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Determines if a Given Track uses the Flash Matrix of the CurrentGAO object or Not.

    Note:   We must be careful because many Tracks that uses Flash do not have their first Event with a Flash Flag. We
            have to look at all the Interpolation Tracks to try to find one with a Flash.
 =======================================================================================================================
 */
BOOL EVE_b_Track_UseFlash
(
	EVE_tdst_Track		*_pst_Track,
	OBJ_tdst_GameObject *_pst_CurrentGO,
	OBJ_tdst_GameObject *_pst_OwnerGO,
	EVE_tdst_ListTracks *_pst_ListTrack
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track		*pst_CurrentTrack;
	EVE_tdst_ListTracks *pst_ListTrack;
	OBJ_tdst_GameObject *pst_CurrentGO, *pst_OwnerGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentGO = _pst_CurrentGO ? _pst_CurrentGO : EVE_gpst_CurrentGAO;
	pst_OwnerGO = _pst_OwnerGO ? _pst_OwnerGO : EVE_gpst_OwnerGAO;
	pst_ListTrack = _pst_ListTrack ? _pst_ListTrack : EVE_gpst_CurrentListTracks;

	/* In Animation cases, return TRUE; */

	/*$F	
		(Meduse et Cinematique: On ne peut pas mettre une valeur absolue sur une clef puisque tout est par rapport au Flash)
	if
	(
		pst_CurrentGO
	&&	(pst_CurrentGO == pst_OwnerGO)
	&&	(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_Anims))
	) return TRUE;
	*/
	pst_CurrentTrack = &pst_ListTrack->pst_AllTracks[0];

	while(pst_CurrentTrack <= _pst_Track)
	{
		if
		(
			((!pst_CurrentTrack->pst_GO) && (pst_CurrentGO != pst_OwnerGO))
		||	((pst_CurrentTrack->pst_GO) && (pst_CurrentTrack->pst_GO != pst_CurrentGO))
		||	!(pst_CurrentTrack->pst_AllEvents)
		||	((EVE_w_Event_InterpolationKey_GetFlags(&pst_CurrentTrack->pst_AllEvents[0]) & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey)
		)
		{
			pst_CurrentTrack++;
			continue;
		}

		/* Ugly Optim */
		if(pst_CurrentTrack->pst_AllEvents[0].uw_NumFrames & 0x8000)
			return TRUE;

		if(EVE_w_Event_InterpolationKey_GetFlags(&pst_CurrentTrack->pst_AllEvents[0]) & EVE_C_EventFlag_Flash) 
			return TRUE;

		pst_CurrentTrack++;
	}

	return FALSE;
}

/*$F
 =======================================================================================================================
 =======================================================================================================================
 */

void EVE_Anim_QuaternionKey_Play(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	EVE_tdst_Event		*pst_NextKey;
	float				f_Time;
	MATH_tdst_Matrix	st_M, st_Matrix;
	MATH_tdst_Matrix	*pst_Matrix2Update;
	OBJ_tdst_GameObject *pst_GO;
	BOOL				b_GizmoTrack;
	BOOL				b_MagicBoxChild;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	/* Get Next Key */
	i = EVE_gpst_CurrentParam->uw_CurrentEvent + 1;
	if(i == EVE_gpst_CurrentTrack->uw_NumEvents) i = 0;
	pst_NextKey = EVE_gpst_CurrentTrack->pst_AllEvents + i;
	

	f_Time = EVE_gpst_CurrentParam->f_Time;

	b_GizmoTrack = (EVE_gpst_CurrentTrack->uw_Gizmo != (unsigned short) - 1);

	/* Get current Bone GO */
	if(b_GizmoTrack)
	{
		if(EVE_gpst_CurrentTrack->uw_Gizmo >= EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects->ul_NbElems)
			return;

		pst_GO = (OBJ_tdst_GameObject *)
			(
				EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects->p_Table +
				EVE_gpst_CurrentTrack->uw_Gizmo
			)->p_Pointer;
		if(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AnimDontTouch) return;

		/* LOD */
		if
		(
			pst_GO->pst_Extended 
		&& (EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->c_LOD_Dynamic & EVE_LOD_SkipExtremityBones) 
		&& (pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_EndBone)
		) 
		{
			return;
		}

		/* LOD */
		if
		(
			pst_GO->pst_Extended 
		&& (pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_SkipROnBone)
		) 
		{
			return;
		}


	}
	else
		pst_GO = EVE_gpst_CurrentGAO;

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
	{
		pst_Matrix2Update = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix;
		b_MagicBoxChild = (OBJ_pst_GetFather(pst_GO) == EVE_gpst_CurrentGAO) ? TRUE : FALSE;
	}
	else
	{
		pst_Matrix2Update = OBJ_pst_GetAbsoluteMatrix(pst_GO);
		b_MagicBoxChild = FALSE;
	}

	EVE_Anim_ComputeInterpoledMatrix(pst_GO, &st_M, _pst_Event, pst_NextKey, f_Time);

	if(!b_GizmoTrack)
	{
		MATH_Mul33MatrixMatrix(&st_Matrix, &st_M, EVE_gpst_CurrentRefMatrix, 1);
		MATH_Copy33Matrix(pst_Matrix2Update, &st_Matrix);
	}
	else
	{
		if(b_MagicBoxChild && EVE_gpst_CurrentRefMatrix)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix				st_Delta;
			MATH_tdst_Matrix st_InvMatrix	ONLY_PSX2_ALIGNED(16);
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/*
			 * The Anim information is in the Flash Matrix Coordinate System. We want to
			 * compute the Local Matrix of the Bone in the MagicBox Coordinate System.
			 */
			if(OBJ_b_TestIdentityFlag(EVE_gpst_CurrentGAO, OBJ_C_IdentityFlag_Hierarchy))
				MATH_InvertMatrix(&st_InvMatrix, OBJ_pst_GetLocalMatrix(EVE_gpst_CurrentGAO));
			else
				MATH_InvertMatrix(&st_InvMatrix, OBJ_pst_GetAbsoluteMatrix(EVE_gpst_CurrentGAO));

			MATH_MulMatrixMatrix(&st_Delta, EVE_gpst_CurrentRefMatrix, &st_InvMatrix);
			MATH_Mul33MatrixMatrix(&st_Matrix, &st_M, &st_Delta, 1);
		}
		else
			MATH_Copy33Matrix(&st_Matrix, &st_M);

		MATH_Copy33Matrix(pst_Matrix2Update, &st_Matrix);
	}

	MATH_SetRotationType(pst_Matrix2Update);
}

/*$F
 =======================================================================================================================
 =======================================================================================================================
 */

void EVE_Anim_TranslationKey_Play(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	MATH_tdst_Vector	*pst_Pos, *pst_DestPos, st_Pos;
	EVE_tdst_Event		*pst_NextKey;
	float				t, f_Time;
	MATH_tdst_Matrix	*pst_Matrix2Update;
	OBJ_tdst_GameObject *pst_GO;
	BOOL				b_GizmoTrack;
	BOOL				b_MagicBoxChild;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get Next Key */
	i = EVE_gpst_CurrentParam->uw_CurrentEvent + 1;
	if(i == EVE_gpst_CurrentTrack->uw_NumEvents) i = 0;
	pst_NextKey = EVE_gpst_CurrentTrack->pst_AllEvents + i;
		

	f_Time = EVE_gpst_CurrentParam->f_Time;

	b_GizmoTrack = (EVE_gpst_CurrentTrack->uw_Gizmo != (unsigned short) - 1);

	/* Get current Bone GO */
	if(b_GizmoTrack)
	{
		if(EVE_gpst_CurrentTrack->uw_Gizmo >= EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects->ul_NbElems)
			return;

		pst_GO = (OBJ_tdst_GameObject *)
			(
				EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects->p_Table +
				EVE_gpst_CurrentTrack->uw_Gizmo
			)->p_Pointer;
		if(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AnimDontTouch) 
			return;

		/* LOD */
		if
		(
			pst_GO->pst_Extended 
		&& (EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->c_LOD_Dynamic & EVE_LOD_SkipExtremityBones) 
		&& (pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_EndBone)
		) 
		{
			return;
		}
	}
	else
		pst_GO = EVE_gpst_CurrentGAO;

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
	{
		pst_Matrix2Update = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix;
		b_MagicBoxChild = (OBJ_pst_GetFather(pst_GO) == EVE_gpst_CurrentGAO) ? TRUE : FALSE;
	}
	else
	{
		pst_Matrix2Update = OBJ_pst_GetAbsoluteMatrix(pst_GO);
		b_MagicBoxChild = FALSE;
	}

	/* Skip Translations ? */
	if
	(
		pst_GO->pst_Extended 
	&& (pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_SkipTOnBone)
	) 
	{
		if
		(
			(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		&& 	(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_HasInitialPos))
		)
		{
			if (!b_MagicBoxChild)
			{
				MATH_CopyVector(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &(pst_GO->pst_GlobalMatrix + 1)->T);
				return;
			}
		}
	}

	pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(_pst_Event);
	if(EVE_NumFrames(_pst_Event))
	{
		pst_DestPos = EVE_pst_Event_InterpolationKey_GetPos(pst_NextKey);

		t = f_Time / EVE_FrameToTime(EVE_NumFrames(_pst_Event));
		MATH_ScaleVector(&st_Pos, pst_Pos, 1 - t);
		MATH_AddScaleVector(&st_Pos, &st_Pos, pst_DestPos, t);
	}
	else
	{
		MATH_CopyVector(&st_Pos, pst_Pos);
	}

	if(!b_GizmoTrack)
	{
		MATH_TransformVertex(&st_Pos, EVE_gpst_CurrentRefMatrix, &st_Pos);
		MATH_CopyVector(&pst_Matrix2Update->T, &st_Pos);
		MATH_SetTranslationType(pst_Matrix2Update);
	}
	else
	{
		if(b_MagicBoxChild && EVE_gpst_CurrentRefMatrix)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix				st_Delta4;
			MATH_tdst_Matrix st_InvMatrix4	ONLY_PSX2_ALIGNED(16);
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/*
			 * The Anim information is in the Flash Matrix Coordinate System. We want to
			 * compute the Local Matrix of the Bone in the MagicBox Coordinate System.
			 */
			if(OBJ_b_TestIdentityFlag(EVE_gpst_CurrentGAO, OBJ_C_IdentityFlag_Hierarchy))
				MATH_InvertMatrix(&st_InvMatrix4, OBJ_pst_GetLocalMatrix(EVE_gpst_CurrentGAO));
			else
				MATH_InvertMatrix(&st_InvMatrix4, OBJ_pst_GetAbsoluteMatrix(EVE_gpst_CurrentGAO));

			MATH_MulMatrixMatrix(&st_Delta4, EVE_gpst_CurrentRefMatrix, &st_InvMatrix4);
			MATH_TransformVertex(&st_Pos, &st_Delta4, &st_Pos);

			if(OBJ_b_TestIdentityFlag(EVE_gpst_CurrentGAO, OBJ_C_IdentityFlag_Anims))
			{
				st_Pos.z += EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->f_Z_Offset;
			}
		}

		/*************************************
			TODO : Faire ça proprement
		 *************************************/
		// Scale translation on pelvis ?
		if
		(
			pst_GO->pst_Extended 
		&&	(pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_SkipTOnBone)
		&&	(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		&& 	(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_HasInitialPos))
		&&	b_MagicBoxChild
		)
		{
			//MATH_tdst_Vector	st_TempVect = {-9.4833369e-5f, -0.034114454f, 0.45344737f};
			float				f_Ratio =
				MATH_f_NormVector(&(pst_GO->pst_GlobalMatrix + 1)->T) / 0.45472884f; //MATH_f_NormVector(&st_TempVect);
			MATH_ScaleEqualVector( &st_Pos, f_Ratio );
		}

		MATH_SetTranslation(pst_Matrix2Update, &st_Pos);
		MATH_SetTranslationType(pst_Matrix2Update);
	}
}

/*$F
 =======================================================================================================================
    Aim:    Update Local or Global Matrix of the Track's GO depending on hierarchy or not.

    Note:   This function can be called from EVE_Event_InterpolationKey_SpecialPlay . 
			If it is the case, All Global Matrix of Bones/Magic Box are NOT CORRECT (Temp Matrix) !!!
 =======================================================================================================================
 */

EVE_tdst_Event *EVE_Event_InterpolationKey_Play(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	short					w_Type, w_NextType;
	int						i;
	MATH_tdst_Vector		*pst_Pos, *pst_DestPos, st_Pos, st_SymPos, st_SymDestPos;
	EVE_tdst_Event			*pst_NextKey;
	float					t, f_Time;
	MATH_tdst_Matrix		st_M, st_Matrix;
	MATH_tdst_Matrix		*pst_Matrix2Update;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	TAB_tdst_PFelem			*pst_CurrentBone;
	OBJ_tdst_GameObject		*pst_GO;
	BOOL					b_GizmoTrack;
	BOOL					b_HierarchyGO;
	BOOL					b_IK;
	BOOL					b_MagicBoxChild;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!EVE_gpst_CurrentTrack) return _pst_Event;
	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
	if(w_Type == -1) return _pst_Event;
		
	b_GizmoTrack = (EVE_gpst_CurrentTrack->uw_Gizmo != (unsigned short) - 1);

	/* Get current Bone GO */
	if(b_GizmoTrack)
	{
		if
		(
			EVE_gpst_CurrentGAO->pst_Base
		&&	EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim
		&&	EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->pst_Skeleton
		)
		{
			pst_GOAnim = EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim;
			if(EVE_gpst_CurrentTrack->uw_Gizmo >= pst_GOAnim->pst_Skeleton->pst_AllObjects->ul_NbElems)
				return _pst_Event;
			pst_CurrentBone = pst_GOAnim->pst_Skeleton->pst_AllObjects->p_Table + EVE_gpst_CurrentTrack->uw_Gizmo;
			pst_GO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
			if(!ENG_gb_EVERunning && (pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AnimDontTouch))
				return _pst_Event;
		}
		else
			return _pst_Event;
	}
	else
		pst_GO = EVE_gpst_CurrentGAO;

	b_HierarchyGO = OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	b_IK = w_Type & EVE_InterKeyType_BlockedForIK;

	if(b_HierarchyGO)
	{
		pst_Matrix2Update = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix;
		b_MagicBoxChild = (OBJ_pst_GetFather(pst_GO) == EVE_gpst_CurrentGAO) ? TRUE : FALSE;
	}
	else
	{
		pst_Matrix2Update = OBJ_pst_GetAbsoluteMatrix(pst_GO);
		b_MagicBoxChild = FALSE;
	}

#ifdef ACTIVE_EDITORS

	/*
	 * In Special Play case, we DO NOT want to update anything. The AbsoluteMatrix of
	 * EVE_gpst_CurrentGAO is here a Temp matrix used outside this function.
	 */
	if(EVE_gl_SpecialPlay)
	{
		pst_Matrix2Update = EVE_gpst_CurrentGAO->pst_GlobalMatrix;
		if(b_IK || b_MagicBoxChild) EVE_gpst_CurrentGAO->pst_GlobalMatrix = EVE_gpst_SavedMatrix;
	}

#endif
	/* Get next event type */
	i = EVE_gpst_CurrentParam->uw_CurrentEvent + 1;
	if(i == EVE_gpst_CurrentTrack->uw_NumEvents) i = 0;

	pst_NextKey = EVE_gpst_CurrentTrack->pst_AllEvents + i;
	if((EVE_w_Event_InterpolationKey_GetFlags(pst_NextKey) & EVE_C_EventFlag_Type) != EVE_C_EventFlag_Empty)
	{
		if((EVE_w_Event_InterpolationKey_GetFlags(pst_NextKey) & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) return _pst_Event;
		w_NextType = EVE_w_Event_InterpolationKey_GetType(pst_NextKey);
		if(w_NextType == -1) return _pst_Event;
	}
	else
		w_NextType = 0;

	if(w_Type & EVE_InterKeyType_Time)
		f_Time = EVE_f_Event_InterpolationKey_InterpolateTime(_pst_Event, EVE_gpst_CurrentParam->f_Time);
	else
		f_Time = EVE_gpst_CurrentParam->f_Time;

	/* Update father (IK or Non IK Event have not the same father) */
	/*
	if
	(
		b_GizmoTrack
	&&	b_HierarchyGO
	&&	(
			(b_IK && (pst_GO->pst_Base->pst_Hierarchy->pst_Father != EVE_gpst_CurrentGAO))
		||	((!b_IK) && (pst_GO->pst_Base->pst_Hierarchy->pst_Father != pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit))
		)
	)
	{
		pst_GO->pst_Base->pst_Hierarchy->pst_Father = b_IK ? EVE_gpst_CurrentGAO : pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit;
		OBJ_ComputeLocalWhenHie(pst_GO);
	}
	*/

	if(w_Type & EVE_InterKeyType_Translation_Mask)
	{
		pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(_pst_Event);
		if(EVE_w_Event_InterpolationKey_GetFlags(_pst_Event) & EVE_C_EventFlag_Symetric)
		{
			st_SymPos.x = -pst_Pos->x;
			st_SymPos.y = pst_Pos->y;
			st_SymPos.z = pst_Pos->z;

			pst_Pos = &st_SymPos;
		}

		if
		(
			(w_NextType & EVE_InterKeyType_Translation_Mask)
		&&	(EVE_NumFrames(_pst_Event))
		&&	!(EVE_w_Event_InterpolationKey_GetFlags(_pst_Event) & EVE_C_EventFlag_NoInterpolation)
		)
		{
			if(w_Type & EVE_InterKeyType_HasNextValue)
				pst_DestPos = (MATH_tdst_Vector *) EVE_pst_Event_InterpolationKey_GetNextValue(_pst_Event);
			else
				pst_DestPos = EVE_pst_Event_InterpolationKey_GetPos(pst_NextKey);

			if(EVE_w_Event_InterpolationKey_GetFlags(pst_NextKey) & EVE_C_EventFlag_Symetric)
			{
				st_SymDestPos.x = -pst_DestPos->x;
				st_SymDestPos.y = pst_DestPos->y;
				st_SymDestPos.z = pst_DestPos->z;

				pst_DestPos = &st_SymDestPos;
			}

#ifdef ACTIVE_EDITORS
			AI_Debug_Move(pst_GO, "EVE_InterpolationKey_Play");
#endif
			if(f_Time >= EVE_FrameToTime(EVE_NumFrames(_pst_Event)))
				MATH_CopyVector(&st_Pos, pst_DestPos);
			else
			{
				t = f_Time / EVE_FrameToTime(EVE_NumFrames(_pst_Event));
				if((w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_2)
				{
					MATH_ScaleVector(&st_Pos, pst_Pos, (1 - t) * (1 - t) * (1 - t));
					MATH_AddScaleVector(&st_Pos, &st_Pos, pst_Pos + 1, 3 * t * (1 - t) * (1 - t));
					MATH_AddScaleVector(&st_Pos, &st_Pos, pst_Pos + 2, 3 * t * t * (1 - t));
					MATH_AddScaleVector(&st_Pos, &st_Pos, pst_DestPos, t * t * t);
				}
				else if((w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_1)
				{
					MATH_ScaleVector(&st_Pos, pst_Pos, (1 - t) * (1 - t));
					MATH_AddScaleVector(&st_Pos, &st_Pos, pst_Pos + 1, 2 * t * (1 - t));
					MATH_AddScaleVector(&st_Pos, &st_Pos, pst_DestPos, t * t);
				}
				else
				{
					MATH_ScaleVector(&st_Pos, pst_Pos, 1 - t);
					MATH_AddScaleVector(&st_Pos, &st_Pos, pst_DestPos, t);
				}
			}
		}
		else
		{
#ifdef ACTIVE_EDITORS
			AI_Debug_Move(pst_GO, "EVE_InterpolationKey_Play");
#endif
			MATH_CopyVector(&st_Pos, pst_Pos);
		}

		if(!b_GizmoTrack)
		{
			if(EVE_gpst_CurrentRefMatrix && EVE_b_Track_UseFlash(EVE_gpst_CurrentTrack, NULL, NULL, NULL))
				MATH_TransformVertex(&st_Pos, EVE_gpst_CurrentRefMatrix, &st_Pos);
			MATH_CopyVector(&pst_Matrix2Update->T, &st_Pos);
			MATH_SetTranslationType(pst_Matrix2Update);

			if
			(
				(EVE_w_Event_InterpolationKey_GetFlags(_pst_Event) & EVE_C_EventFlag_NoInterpolation)
			&&	OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ZDM)
			&&	pst_GO->pst_Extended->pst_Col
			&&	((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance
			&&	((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix
			)
			{
				if(b_HierarchyGO)
				{
					MATH_CopyVector
					(
						&(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix->T),
						&st_Pos
					);
				}
				else
				{
					MATH_CopyVector
					(
						&(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix->T),
						&st_Pos
					);
				}
			}
		}
		else
		{
			/* Skip Translations ? */
			if
			(
				pst_GO->pst_Extended 
			&& (pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_SkipTOnBone)
			) 
			{
				if
				(
					(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
				&& 	(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_HasInitialPos))
				)
				{
					if (!b_MagicBoxChild)
					{
						// Skip translations on other bones
						MATH_CopyVector(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &(pst_GO->pst_GlobalMatrix + 1)->T);
						return _pst_Event;
					}
				}
			}

			if(b_MagicBoxChild && EVE_gpst_CurrentRefMatrix)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix				st_Delta4;
				MATH_tdst_Matrix st_InvMatrix4	ONLY_PSX2_ALIGNED(16);
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				/*
				 * The Anim information is in the Flash Matrix Coordinate System. We want to
				 * compute the Local Matrix of the Bone in the MagicBox Coordinate System.
				 */
				if(OBJ_b_TestIdentityFlag(EVE_gpst_CurrentGAO, OBJ_C_IdentityFlag_Hierarchy))
					MATH_InvertMatrix(&st_InvMatrix4, OBJ_pst_GetLocalMatrix(EVE_gpst_CurrentGAO));
				else
					MATH_InvertMatrix(&st_InvMatrix4, OBJ_pst_GetAbsoluteMatrix(EVE_gpst_CurrentGAO));

				MATH_MulMatrixMatrix(&st_Delta4, EVE_gpst_CurrentRefMatrix, &st_InvMatrix4);
				MATH_TransformVertex(&st_Pos, &st_Delta4, &st_Pos);

				if(OBJ_b_TestIdentityFlag(EVE_gpst_CurrentGAO, OBJ_C_IdentityFlag_Anims))
				{
					st_Pos.z += EVE_gpst_CurrentGAO->pst_Base->pst_GameObjectAnim->f_Z_Offset;
				}

			}

			/*************************************
				TODO : Faire ça proprement
			*************************************/
			// Scale translation on pelvis ?
			if
			(
				pst_GO->pst_Extended 
			&&	(pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_SkipTOnBone)
			&&	(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
			&& 	(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_HasInitialPos))
			&&	b_MagicBoxChild
			)
			{
				//MATH_tdst_Vector	st_TempVect = {-9.4833369e-5f, -0.034114454f, 0.45344737f};
				float				f_Ratio =
					MATH_f_NormVector(&(pst_GO->pst_GlobalMatrix + 1)->T) / 0.45472884f; //MATH_f_NormVector(&st_TempVect);
				MATH_ScaleEqualVector( &st_Pos, f_Ratio );
			}


			MATH_SetTranslation(pst_Matrix2Update, &st_Pos);
			MATH_SetTranslationType(pst_Matrix2Update);
		}
	}

	/* Rotation */
	if(w_Type & EVE_InterKeyType_Rotation_Mask)
	{
		EVE_Event_ComputeInterpoledMatrix(pst_GO, &st_M, _pst_Event, pst_NextKey, f_Time);

		/*
		 * Anti-Bug £
		 * MATH_ClearScale(pst_Matrix2Update, 1);
		 */
		if(!b_GizmoTrack)
		{
			if(EVE_gpst_CurrentRefMatrix && EVE_b_Track_UseFlash(EVE_gpst_CurrentTrack, NULL, NULL, NULL))
				MATH_Mul33MatrixMatrix(&st_Matrix, &st_M, EVE_gpst_CurrentRefMatrix, 1);
			else
				MATH_Copy33Matrix(&st_Matrix, &st_M);

			MATH_Copy33Matrix(pst_Matrix2Update, &st_Matrix);
		}
		else
		{
			/* LOD */
			if
			(
				pst_GO->pst_Extended 
			&& (pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_SkipROnBone)
			) 
			{
				return _pst_Event;
			}

			if(b_MagicBoxChild && EVE_gpst_CurrentRefMatrix)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix				st_Delta;
				MATH_tdst_Matrix st_InvMatrix	ONLY_PSX2_ALIGNED(16);
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				/*
				 * The Anim information is in the Flash Matrix Coordinate System. We want to
				 * compute the Local Matrix of the Bone in the MagicBox Coordinate System.
				 */
				if(OBJ_b_TestIdentityFlag(EVE_gpst_CurrentGAO, OBJ_C_IdentityFlag_Hierarchy))
					MATH_InvertMatrix(&st_InvMatrix, OBJ_pst_GetLocalMatrix(EVE_gpst_CurrentGAO));
				else
					MATH_InvertMatrix(&st_InvMatrix, OBJ_pst_GetAbsoluteMatrix(EVE_gpst_CurrentGAO));

				MATH_MulMatrixMatrix(&st_Delta, EVE_gpst_CurrentRefMatrix, &st_InvMatrix);

				MATH_Mul33MatrixMatrix(&st_Matrix, &st_M, &st_Delta, 1);
			}
			else
				MATH_Copy33Matrix(&st_Matrix, &st_M);

			MATH_Copy33Matrix(pst_Matrix2Update, &st_Matrix);
		}

		/* MATH_ClearScaleType(pst_Matrix2Update); */
		MATH_SetRotationType(pst_Matrix2Update);
	}

#ifdef ACTIVE_EDITORS
	if(EVE_gl_SpecialPlay)
	{
		if(b_IK || b_MagicBoxChild) EVE_gpst_CurrentGAO->pst_GlobalMatrix = pst_Matrix2Update;
		return _pst_Event;
	}

#endif
	if((b_GizmoTrack) && b_IK && (w_Type & EVE_InterKeyType_Translation_Mask))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_Father, *pst_GOA, *pst_GOB;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(OBJ_b_IsChild(pst_GO)) pst_GOB = OBJ_pst_GetFatherInit(pst_GO);
		if(pst_GOB && OBJ_b_IsChild(pst_GOB))
		{
			pst_GOA = OBJ_pst_GetFatherInit(pst_GOB);
			if(pst_GOA)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	M, MA, MB, MC;
				MATH_tdst_Matrix I	ONLY_PSX2_ALIGNED(16);
				float				AB, BC;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				OBJ_ComputeGlobalWithLocal(pst_GOA, &MA, TRUE);
				OBJ_ComputeGlobalWithLocal(pst_GOB, &MB, TRUE);
				OBJ_ComputeGlobalWithLocal(pst_GO, &MC, TRUE);
				AB = MATH_f_NormVector(&OBJ_pst_GetInitialAbsoluteMatrix(pst_GOB)->T);
				BC = MATH_f_NormVector(&OBJ_pst_GetInitialAbsoluteMatrix(pst_GO)->T);
				GAO_ModifierLegLink_Compute(&MA, &MB, &MC, AB, BC, NULL );

				if(OBJ_b_IsChild(pst_GOA) && (pst_Father = OBJ_pst_GetFather(pst_GOA)))
				{
					OBJ_ComputeGlobalWithLocal(pst_Father, &M, TRUE);
					MATH_InvertMatrix(&I, &M);
					MATH_MulMatrixMatrix(&pst_GOA->pst_Base->pst_Hierarchy->st_LocalMatrix, &MA, &I);
					MATH_Orthonormalize(&pst_GOA->pst_Base->pst_Hierarchy->st_LocalMatrix);
				}

				MATH_InvertMatrix(&I, &MA);
				MATH_MulMatrixMatrix(&pst_GOB->pst_Base->pst_Hierarchy->st_LocalMatrix, &MB, &I);
				MATH_Orthonormalize(&pst_GOB->pst_Base->pst_Hierarchy->st_LocalMatrix);

				if(OBJ_b_IsChild(pst_GO) && (pst_Father = OBJ_pst_GetFather(pst_GO)))
				{
					OBJ_ComputeGlobalWithLocal(pst_Father, &M, TRUE);
					MATH_InvertMatrix(&I, &M);
					MATH_MulMatrixMatrix(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, &MC, &I);
					MATH_Orthonormalize(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
				}
			}
		}
	}

	return _pst_Event;
}

/*
 =======================================================================================================================
    Aim:    Interpolate time if interpolation key has time.

    Note:   Assume that type and sub type of key are good (interpolation key with time interpolation)
 =======================================================================================================================
 */
float EVE_f_Event_InterpolationKey_InterpolateTime(EVE_tdst_Event *_pst_Evt, float _f_Time)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		i_Number;
	float	*pf_Start, *pf_TimeKey, *pf_Last;
	float	f_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pf_Start = pf_TimeKey = EVE_pf_Event_InterpolationKey_GetTime(_pst_Evt);
	i_Number = *(int *) pf_TimeKey;
	*pf_TimeKey = 0;
	pf_Last = pf_TimeKey + (i_Number - 1) * 2;

	f_Result = 0;
	while(pf_TimeKey < pf_Last)
	{
		if(pf_TimeKey[2] > _f_Time) break;
		f_Result += (pf_TimeKey[2] - pf_TimeKey[0]) * pf_TimeKey[1];
		pf_TimeKey += 2;
	}

	f_Result += (_f_Time - pf_TimeKey[0]) * pf_TimeKey[1];
	*(int *) pf_Start = i_Number;
	return f_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_BlendTranslation
(
	EVE_tdst_Event		*_pst_Evt,
	EVE_tdst_Event		*_pst_NextEvt,
	float				_f_Time,
	MATH_tdst_Vector	*_pst_Pos
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	short				w_Type;
	MATH_tdst_Vector	*pst_Pos, *pst_DestPos;
	float				t, omt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(_pst_Evt);

	if(_f_Time <= 0.0f)
	{
		MATH_CopyVector(_pst_Pos, pst_Pos);
		return;
	}

	pst_DestPos = EVE_pst_Event_InterpolationKey_GetPos(_pst_NextEvt);

	if(_f_Time >= EVE_FrameToTime(EVE_NumFrames(_pst_Evt)))
		MATH_CopyVector(_pst_Pos, pst_DestPos);
	else
	{
		w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Evt);

		if(w_Type & EVE_InterKeyType_Time)
			t = EVE_f_Event_InterpolationKey_InterpolateTime(_pst_Evt, _f_Time);
		else
			t = _f_Time;
		t /= EVE_FrameToTime(EVE_NumFrames(_pst_Evt));
		omt = 1.0f - t;

		if((w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_2)
		{
			MATH_ScaleVector(_pst_Pos, pst_Pos, omt * omt * omt);
			MATH_AddScaleVector(_pst_Pos, _pst_Pos, pst_Pos + 1, 3 * t * omt * omt);
			MATH_AddScaleVector(_pst_Pos, _pst_Pos, pst_Pos + 2, 3 * t * t * omt);
			MATH_AddScaleVector(_pst_Pos, _pst_Pos, pst_DestPos, t * t * t);
		}
		else if((w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_1)
		{
			MATH_ScaleVector(_pst_Pos, pst_Pos, omt * omt);
			MATH_AddScaleVector(_pst_Pos, _pst_Pos, pst_Pos + 1, 2 * t * omt);
			MATH_AddScaleVector(_pst_Pos, _pst_Pos, pst_DestPos, t * t);
		}
		else
		{
			MATH_ScaleVector(_pst_Pos, pst_Pos, omt);
			MATH_AddScaleVector(_pst_Pos, _pst_Pos, pst_DestPos, t);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_GetTranslation(EVE_tdst_Track *_pst_Track, float _f_Time, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt, *pst_Next, *pst_Prev;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = _pst_Track->pst_AllEvents;
	pst_Prev = pst_Next = NULL;
	for(i = 0; i < _pst_Track->uw_NumEvents; i++, pst_Evt++)
	{
		if(_f_Time <= EVE_FrameToTime(EVE_NumFrames(pst_Evt)))
		{
			if((EVE_w_Event_InterpolationKey_GetFlags(pst_Evt) & EVE_C_EventFlag_Type) == EVE_C_EventFlag_Empty)
			{
				if(pst_Prev && pst_Next) EVE_Event_InterpolationKey_BlendTranslation(pst_Prev, pst_Next, 0, _pst_Pos);
				return;
			}

			pst_Prev = pst_Evt;
			pst_Next = (i + 1 == _pst_Track->uw_NumEvents) ? _pst_Track->pst_AllEvents : (pst_Evt + 1);
			if((EVE_w_Event_InterpolationKey_GetFlags(pst_Next) & EVE_C_EventFlag_Type) == EVE_C_EventFlag_Empty) _f_Time = 0;
			EVE_Event_InterpolationKey_BlendTranslation(pst_Evt, pst_Next, _f_Time, _pst_Pos);
			return;
		}

		_f_Time -= EVE_FrameToTime(EVE_NumFrames(pst_Evt));
	}

	pst_Evt = _pst_Track->pst_AllEvents + (_pst_Track->uw_NumEvents - 1);
	pst_Next = _pst_Track->pst_AllEvents;
	EVE_Event_InterpolationKey_BlendTranslation(pst_Evt, pst_Next, EVE_FrameToTime(EVE_NumFrames(pst_Evt)), _pst_Pos);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_GetRotationPos
(
	EVE_tdst_Track		*_pst_RT,
	EVE_tdst_Track		*_pst_TT,
	int					i_Key,
	MATH_tdst_Vector	*_pst_Pos
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	float			f_Time;
	EVE_tdst_Event	*pst_Evt;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(i_Key >= _pst_RT->uw_NumEvents)
	{
		EVE_Event_InterpolationKey_GetTranslation(_pst_TT, Cf_Infinit, _pst_Pos);
		return;
	}

	f_Time = 0;
	pst_Evt = _pst_RT->pst_AllEvents;
	while(i_Key--) f_Time += EVE_FrameToTime(EVE_NumFrames(pst_Evt));
	if(_pst_TT) EVE_Event_InterpolationKey_GetTranslation(_pst_TT, f_Time, _pst_Pos);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    We want all the Bones/Magic Box position information at a given time in an Anim without changing anything
            in the different Matrix (Local/Global) of the Bones/MagicBox. So, we store the Play results in arrays that
            we will use after this function call ...
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_SpecialPlay
(
	OBJ_tdst_GameObject *_pst_GO,
	EVE_tdst_Data		*_pst_Data,
	float				_f_Time,
	int					_i_Track,
	int					_i_Evt,
	MATH_tdst_Matrix	*_pst_MagicBox,
	MATH_tdst_Matrix	*_pst_Gizmo
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	OBJ_tdst_Group			*pst_Skel;
	LONG					l_NbGizmo, i, j;
	TAB_tdst_PFelem			*pst_CurrentBone;
	OBJ_tdst_GameObject		*pst_BoneObj;
	EVE_tdst_Track			*pst_Track;
	EVE_tdst_Event			*pst_Event, *pst_Last;
	float					time;
	OBJ_tdst_GameObject		*pst_SaveCurGao;
	MATH_tdst_Matrix		*pst_SaveM, *pst_SaveRefM, *pst_SaveM2;
	EVE_tdst_ListTracks		*pst_SaveLT;
	EVE_tdst_Track			*pst_SaveTrack;
	EVE_tdst_Params			*pst_SaveParams;
	unsigned short			uw_SaveCurEvent;
	float					f_SaveTime;
	OBJ_tdst_GameObject		*pst_BoneGO, *pst_SaveFather;
	MATH_tdst_Matrix		st_SaveLocal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) return;

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	if(!pst_GOAnim) return;

	pst_Skel = pst_GOAnim->pst_Skeleton;
	if(!pst_Skel) return;

	l_NbGizmo = pst_Skel->pst_AllObjects->ul_NbElems;

	/* init matrix to current value */
	for(i = 0; i < l_NbGizmo; i++)
	{
		pst_CurrentBone = pst_Skel->pst_AllObjects->p_Table + i;
		pst_BoneObj = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		MATH_CopyMatrix(&_pst_Gizmo[i], OBJ_pst_GetLocalMatrix(pst_BoneObj));
	}

	/* save events global parameters */
	pst_SaveParams = EVE_gpst_CurrentParam;
	pst_SaveLT = EVE_gpst_CurrentListTracks;
	pst_SaveTrack = EVE_gpst_CurrentTrack;
	pst_SaveM = _pst_GO->pst_GlobalMatrix;
	pst_SaveCurGao = EVE_gpst_CurrentGAO;
	pst_SaveRefM = EVE_gpst_CurrentRefMatrix;
	pst_SaveM2 = EVE_gpst_SavedMatrix;

	/* set new data */
	EVE_gpst_CurrentGAO = _pst_GO;
	EVE_gpst_CurrentParam = _pst_Data->pst_ListParam;
	EVE_gpst_CurrentListTracks = _pst_Data->pst_ListTracks;
	EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(_pst_GO);
	EVE_gpst_SavedMatrix = EVE_gpst_OwnerGAO->pst_GlobalMatrix;

	/* save current params data */
	uw_SaveCurEvent = EVE_gpst_CurrentParam->uw_CurrentEvent;
	f_SaveTime = EVE_gpst_CurrentParam->f_Time;

	/* now play all track */
	EVE_gl_SpecialPlay = 1;
	for(i = 0; i < EVE_gpst_CurrentListTracks->uw_NumTracks; i++)
	{
		pst_Track = EVE_gpst_CurrentListTracks->pst_AllTracks + i;
		if(pst_Track->pst_GO != NULL) continue;
		if(pst_Track->uw_Gizmo == (unsigned short) - 1)
			_pst_GO->pst_GlobalMatrix = _pst_MagicBox;
		else
		{
			_pst_GO->pst_GlobalMatrix = _pst_Gizmo + pst_Track->uw_Gizmo;
			pst_BoneGO = (OBJ_tdst_GameObject *)
				(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects->p_Table +
					pst_Track->uw_Gizmo)->p_Pointer;

			/*
			 * We are about to "play" a BONE event at an unknown time. Father of the Bone can
			 * be changed by the Player (IK / Non IK Events). Save Father and Local Matrix.
			 */
			pst_SaveFather = pst_BoneGO->pst_Base->pst_Hierarchy->pst_Father;
			MATH_CopyMatrix(&st_SaveLocal, &pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix);
		}

		/* compute current event and time into this event */
		time = _f_Time;
		j = 0;
		pst_Event = pst_Track->pst_AllEvents;
		if(!pst_Event) continue;
		pst_Last = pst_Event + pst_Track->uw_NumEvents;

		if(_i_Track == i)
		{

			while(1)
			{

				if((time < EVE_FrameToTime(EVE_NumFrames(pst_Event))) || ((time == 0) && (j == _i_Evt))) break;
				time -= EVE_FrameToTime(EVE_NumFrames(pst_Event));
				pst_Event++;
				j++;
				if(pst_Event == pst_Last)
				{
					if(time == _f_Time) break;
					pst_Event = pst_Track->pst_AllEvents;
					j = 0;
				}

			}

		}
		else
		{

			while(1)
			{

				/* if(time <= pst_Event->f_Delay) break; */
				if
				(
					(time < EVE_FrameToTime(EVE_NumFrames(pst_Event)))
				||	((time == 0) && (EVE_FrameToTime(EVE_NumFrames(pst_Event)) == 0))
				) break;
				time -= EVE_FrameToTime(EVE_NumFrames(pst_Event));
				pst_Event++;
				j++;
				if(pst_Event == pst_Last)
				{
					if(time == _f_Time) break;
					pst_Event = pst_Track->pst_AllEvents;
					j = 0;
				}

			}
;
		}

		if(time > EVE_FrameToTime(EVE_NumFrames(pst_Event))) continue;

		EVE_gpst_CurrentTrack = pst_Track;
		EVE_gpst_CurrentParam->uw_CurrentEvent = (unsigned short) j;
		EVE_gpst_CurrentParam->f_Time = time;

		EVE_Event_InterpolationKey_Play(pst_Event);

		if(pst_Track->uw_Gizmo != (unsigned short) - 1)
		{
			/*
			 * Restore Bone Father and Local Matrix that can have been changed by the Player
			 * (IK/Non IK Events)
			 */
			pst_BoneGO->pst_Base->pst_Hierarchy->pst_Father = pst_SaveFather;
			MATH_CopyMatrix(&pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix, &st_SaveLocal);
		}
	}

	EVE_gl_SpecialPlay = 0;

	EVE_gpst_CurrentParam = EVE_gpst_CurrentParam;
	EVE_gpst_CurrentListTracks = pst_SaveLT;
	EVE_gpst_CurrentTrack = pst_SaveTrack;
	_pst_GO->pst_GlobalMatrix = pst_SaveM;
	EVE_gpst_CurrentGAO = pst_SaveCurGao;
	EVE_gpst_CurrentRefMatrix = pst_SaveRefM;
	EVE_gpst_CurrentParam->uw_CurrentEvent = uw_SaveCurEvent;
	EVE_gpst_CurrentParam->f_Time = f_SaveTime;
	EVE_gpst_SavedMatrix = pst_SaveM2;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_UpdateGizmos
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_Group		*_pst_Skeleton,
	MATH_tdst_Matrix	*_pst_MagicBox,
	MATH_tdst_Matrix	*_pst_Gizmo
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	TAB_tdst_PFelem		*pst_Bone;
	OBJ_tdst_GameObject *pst_BoneGO;
	OBJ_tdst_GameObject *pst_Father;
	ULONG				ul_ID;
	MATH_tdst_Matrix	st_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* now update all matrix depending on skeloton hierarchy */
	for(i = 0; i < (int) _pst_Skeleton->pst_AllObjects->ul_NbElems; i++)
	{
		pst_Bone = _pst_Skeleton->pst_AllObjects->p_Table + i;
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_Bone->p_Pointer;
		pst_Father = OBJ_pst_GetFather(pst_BoneGO);

		if((pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && (pst_Father != _pst_GO))
		{
			ul_ID = TAB_ul_PFtable_GetElemIndexWithPointer(_pst_Skeleton->pst_AllObjects, pst_Father);
			MATH_CopyMatrix(&st_Temp, _pst_Gizmo + i);
			MATH_MulMatrixMatrix(_pst_Gizmo + i, &st_Temp, _pst_Gizmo + ul_ID);
		}
	}

	if(_pst_MagicBox)
	{
		for(i = 0; i < (int) _pst_Skeleton->pst_AllObjects->ul_NbElems; i++)
		{
			MATH_CopyMatrix(&st_Temp, _pst_Gizmo + i);
			MATH_MulMatrixMatrix(_pst_Gizmo + i, &st_Temp, _pst_MagicBox);
		}
	}

	/* Anim is played contracted */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EVE_tdst_Params		*pst_Params;
		MATH_tdst_Vector	st;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0])
		{
			pst_Params = (_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListParam) + 1;
			if(!(pst_Params->uw_Flags & EVE_C_Track_Running))
			{
				MATH_SubVector(&st, &_pst_MagicBox->T, OBJ_pst_GetAbsolutePosition(_pst_GO));
				for(i = 0; i < (int) _pst_Skeleton->pst_AllObjects->ul_NbElems; i++)
				{
					MATH_SubVector(&(_pst_Gizmo + i)->T, &(_pst_Gizmo + i)->T, &st);
				}

				MATH_CopyVector(&_pst_MagicBox->T, OBJ_pst_GetAbsolutePosition(_pst_GO));
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_GetAbsoluteMatrix
(
	EVE_tdst_Data		*_pst_Data,
	EVE_tdst_Track		*_pst_Track,
	OBJ_tdst_GameObject *_pst_GO,
	BOOL				_b_Anim,
	unsigned short		_uw_Gizmo,
	MATH_tdst_Matrix	*_pst_Matrix
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event			*pst_Evt;
	MATH_tdst_Matrix		M, *Gizmo;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	TAB_tdst_PFelem			*pst_CurrentBone;
	OBJ_tdst_GameObject		*pst_GO, *pst_Father;
	OBJ_tdst_Group			*pst_Skel;
	ULONG					ul_FatherID;
	BOOL					b_Flash;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((!_pst_GO) || (!_pst_Matrix) || (!_pst_Data)) return;

	b_Flash = FALSE;
	if(_pst_Track)
	{
		_b_Anim = (_pst_Track->uw_Gizmo != (unsigned short) - 1)
	&&	(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims);
		_uw_Gizmo = _pst_Track->uw_Gizmo;
		pst_Evt = _pst_Track->pst_AllEvents;
		if((pst_Evt != NULL) && ((ULONG) pst_Evt != 0xFFFFFFFF) && (_pst_Track->uw_NumEvents > 0)) b_Flash = EVE_w_Event_InterpolationKey_GetFlags(pst_Evt) & EVE_C_EventFlag_Flash;
	}

	b_Flash &= (!(_pst_Data->pst_ListTracks->uw_Flags & EVE_C_Tracks_NoFlash));

	MATH_CopyMatrix(_pst_Matrix, &MATH_gst_IdentityMatrix);

	if(_b_Anim)
	{
		pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
		pst_Skel = pst_GOAnim->pst_Skeleton;
		if(pst_Skel)
		{
			pst_CurrentBone = pst_Skel->pst_AllObjects->p_Table + _uw_Gizmo;
			pst_GO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

			pst_Father = OBJ_pst_GetFather(pst_GO);
			ul_FatherID = TAB_ul_PFtable_GetElemIndexWithPointer(pst_Skel->pst_AllObjects, pst_Father);
			if(EVE_Event_InterpolationKey_IsChild(pst_GO))
			{
				EVE_Event_InterpolationKey_SpecialPlay(_pst_GO, _pst_Data, 0, -1, 0, &EVE_st_MagicBox, EVE_ast_Gizmo);
				EVE_Event_InterpolationKey_UpdateGizmos(_pst_GO, pst_Skel, &EVE_st_MagicBox, EVE_ast_Gizmo);
				Gizmo = EVE_ast_Gizmo + ul_FatherID;
				MATH_CopyMatrix(_pst_Matrix, Gizmo);
			}
		}
	}

	if((_uw_Gizmo != (unsigned short) - 1) || b_Flash)
	{
		MATH_CopyMatrix(&M, _pst_Matrix);
		if(EVE_gpst_CurrentRefMatrix)
			MATH_MulMatrixMatrix(_pst_Matrix, &M, EVE_gpst_CurrentRefMatrix);
		else
			MATH_MulMatrixMatrix(_pst_Matrix, &M, OBJ_pst_GetInitialAbsoluteMatrix(_pst_GO));
	}

	if(EVE_Event_InterpolationKey_IsChild(_pst_GO))
	{
		MATH_CopyMatrix(&M, _pst_Matrix);
		MATH_MulMatrixMatrix(_pst_Matrix, &M, OBJ_pst_GetAbsoluteMatrix(_pst_GO->pst_Base->pst_Hierarchy->pst_Father));
	}
}

/*
 =======================================================================================================================
	Aim : Get the transformation which should be applied to a path so that the curve can be drawn
	      at the right place.
	+ _pst_Track is the track which has the translation keys of the path
	+ _pst_GO is the GO moving along the path
	+ _pst_Matrix is a reference to the matrix where the result will be stored
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_GetPathInitMatrix
(
	EVE_tdst_Track		*_pst_Track,
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Matrix	*_pst_Matrix
)
{
	if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
	{
		if (
			_pst_Track->uw_NumEvents && _pst_Track->pst_AllEvents &&
			(_pst_Track->pst_AllEvents[0].w_Flags & EVE_C_EventFlag_Flash) &&
			OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		{
			// If there is an initial pos, we use it. If no, we use the flash matrix.
			// So we always use the first matrix after the global pos.
			MATH_MulMatrixMatrix(
				_pst_Matrix,
				_pst_GO->pst_GlobalMatrix + 1,
				OBJ_pst_GetFather(_pst_GO)->pst_GlobalMatrix);
		}
		else
		{
			// The path will move as the father moves.
			// Moreover, the object may not be on the curve at the beginning
			// (but it will be at the first frame).
			MATH_CopyMatrix(
				_pst_Matrix,
				OBJ_pst_GetFather(_pst_GO)->pst_GlobalMatrix);
		}
	}
	else
	{
		if (
			_pst_Track->uw_NumEvents && _pst_Track->pst_AllEvents &&
			(_pst_Track->pst_AllEvents[0].w_Flags & EVE_C_EventFlag_Flash) &&
			OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		{
			// If there is an initial pos, we use it. If no, we use the flash matrix.
			// So we always use the first matrix after the global pos.
			MATH_CopyMatrix(_pst_Matrix, _pst_GO->pst_GlobalMatrix + 1);
		}
		else
		{
			// The translations of the keys are relative to the origin.
			// The object may not be on the curve at the beginning
			// (but it will be at the first frame).
			MATH_CopyMatrix(_pst_Matrix, &MATH_gst_IdentityMatrix);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    We have inserted a new Interpolation key in the given track, and computed its delay. Now, we want to
            compute the data of this new Event based on the Global Matrix of the bone which is considered as good.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_WhenInsert
(
	OBJ_tdst_GameObject *_pst_GO,
	EVE_tdst_ListTracks *_pst_List,
	EVE_tdst_Track		*_pst_Track,
	EVE_tdst_Event		*_pst_Event
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Prev;
	short			w_Type;
	float			f;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
	if(w_Type & (EVE_InterKeyType_Translation_Mask | EVE_InterKeyType_Rotation_Mask))
	{
		EVE_Event_InterpolationKey_Compute(_pst_GO, _pst_GO, _pst_List, _pst_Track, _pst_Event, w_Type, 1);
	}

	if(w_Type & EVE_InterKeyType_Time)
	{
		pst_Prev = _pst_Event - 1;
		f = 1.0f / (EVE_FrameToTime(EVE_NumFrames(pst_Prev)) + EVE_FrameToTime(EVE_NumFrames(_pst_Event)));
		EVE_Event_InterpolationKey_ZoomTimeInterpolation(pst_Prev, EVE_FrameToTime(EVE_NumFrames(pst_Prev)) * f);
		EVE_Event_InterpolationKey_ZoomTimeInterpolation(_pst_Event, EVE_FrameToTime(EVE_NumFrames(_pst_Event)) * f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_ZoomTimeInterpolation(EVE_tdst_Event *_pst_Event, float _f_Zoom)
{
	/*~~~~~~~~~~~~~~~~*/
	short	w_Type;
	float	*pf_TimeKey;
	/*~~~~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Event);
	if(!(w_Type & EVE_InterKeyType_Time)) return;
	pf_TimeKey = EVE_pf_Event_InterpolationKey_GetTime(_pst_Event);

	for(w_Type = 1; w_Type < (*(short *) pf_TimeKey); w_Type++) pf_TimeKey[w_Type * 2] *= _f_Zoom;
}

/*
 =======================================================================================================================
    Aim:    Determines if the given Event is a NEW Transition IK. That means that this Event is a IK Event and the Next
            one is not or the exact opposite BUT it has no next value.

    Note:   This function is usually called to find the new Transition IK Events and to compute the next values.
            Sometimes, we want to update all those values without taking care of new. (Like in the UpdateAllValues for
            ex heheh)
 =======================================================================================================================
 */
BOOL EVE_Event_InterpolationKey_TransitionIK(EVE_tdst_Track *_pst_Track, EVE_tdst_Event *_pst_Evt, BOOL _b_All)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_NextEvt;
	short			w_Type1, w_Type2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((EVE_w_Event_InterpolationKey_GetFlags(_pst_Evt) & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) return FALSE;
	if(!_b_All && (EVE_w_Event_InterpolationKey_GetType(_pst_Evt) & EVE_InterKeyType_HasNextValue)) return FALSE;

	if(_pst_Evt == (_pst_Track->pst_AllEvents + (_pst_Track->uw_NumEvents - 1)))
		pst_NextEvt = _pst_Track->pst_AllEvents;
	else
		pst_NextEvt = _pst_Evt + 1;

	if((EVE_w_Event_InterpolationKey_GetFlags(pst_NextEvt) & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) return FALSE;

	w_Type1 = EVE_w_Event_InterpolationKey_GetType(_pst_Evt);
	w_Type2 = EVE_w_Event_InterpolationKey_GetType(pst_NextEvt);

	return((w_Type1 & EVE_InterKeyType_BlockedForIK) != (w_Type2 & EVE_InterKeyType_BlockedForIK));
}

/*
 =======================================================================================================================
    Aim:    Trace the content of the given Event.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_Trace(EVE_tdst_Event *_pst_Evt, char *_psz_Zlabog)
{
	/*~~~~~~~~~~~~~~~~~*/
	short	w_Type;
	char	asz_Log[500];
	/*~~~~~~~~~~~~~~~~~*/

	if((EVE_w_Event_InterpolationKey_GetFlags(_pst_Evt) & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) return;

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Evt);

	if(_psz_Zlabog) LINK_PrintStatusMsg(_psz_Zlabog);

	if(w_Type & EVE_InterKeyType_Translation_Mask)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	*pst_T;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_T = EVE_pst_Event_InterpolationKey_GetPos(_pst_Evt);
		sprintf(asz_Log, "[ x= %0.2f, y= %0.2f, z= %0.2f ] (Pos) ", pst_T->x, pst_T->y, pst_T->z);
		if(w_Type & EVE_InterKeyType_BlockedForIK) L_strcat(asz_Log, "(IK)");
		LINK_PrintStatusMsg(asz_Log);

		if(w_Type & EVE_InterKeyType_HasNextValue)
		{
			pst_T = (MATH_tdst_Vector *) EVE_pst_Event_InterpolationKey_GetNextValue(_pst_Evt);
			sprintf(asz_Log, "[ x= %0.2f, y= %0.2f, z= %0.2f ] (Pos) (Next Value) ", pst_T->x, pst_T->y, pst_T->z);
			if(w_Type & EVE_InterKeyType_BlockedForIK) L_strcat(asz_Log, "(IK)");
			LINK_PrintStatusMsg(asz_Log);
		}
	}

	if(w_Type & EVE_InterKeyType_Rotation_Quaternion)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Quaternion	st_Q;
		MATH_tdst_Quaternion	*pst_Q;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		EVE_Event_InterpolationKey_GetQuaternion(_pst_Evt, &st_Q);

		sprintf(asz_Log, "[ x= %0.2f, y= %0.2f, z= %0.2f ] (Q) ", st_Q.x, st_Q.y, st_Q.z);
		if(w_Type & EVE_InterKeyType_BlockedForIK) L_strcat(asz_Log, "(IK)");

		LINK_PrintStatusMsg(asz_Log);
		if(w_Type & EVE_InterKeyType_HasNextValue)
		{
			pst_Q = (MATH_tdst_Quaternion *) EVE_pst_Event_InterpolationKey_GetNextValue(_pst_Evt);
			sprintf(asz_Log, "[ x= %0.2f, y= %0.2f, z= %0.2f ] (Q) (Next Value) ", pst_Q->x, pst_Q->y, pst_Q->z);
			if(w_Type & EVE_InterKeyType_BlockedForIK) L_strcat(asz_Log, "(IK)");
			LINK_PrintStatusMsg(asz_Log);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    The EVE_Event_InterpolationKey_Compute function is called (in editors) when u move a bone. If u move the
            bassin and if the foot has IK. u have to recompute all the Knee matrixes (Local/Global) when processing the
            foot.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_RecomputeFatherMatrixWhenIK(OBJ_tdst_GameObject *_pst_BoneGO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_GOA, *pst_GOB;
	GAO_tdst_ModifierLegLink	st_IK;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(OBJ_b_IsChild(_pst_BoneGO)) pst_GOB = OBJ_pst_GetFatherInit(_pst_BoneGO);
	if(pst_GOB && OBJ_b_IsChild(pst_GOB))
	{
		pst_GOA = OBJ_pst_GetFatherInit(pst_GOB);
		if(pst_GOA)
		{
			st_IK.f_AB = MATH_f_NormVector(&OBJ_pst_GetInitialAbsoluteMatrix(pst_GOB)->T);
			st_IK.f_BC = MATH_f_NormVector(&OBJ_pst_GetInitialAbsoluteMatrix(_pst_BoneGO)->T);
			st_IK.p_GAOA = pst_GOA;
			st_IK.p_GAOC = _pst_BoneGO;
			st_IK.p_GaoOrient = NULL;
			st_IK.ul_Flags = 0;
			GAO_ModifierLegLink_DoIt(&st_IK, pst_GOB);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    We found a new TransitionIK Event and we want to create/update the next value of this Event.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_ComputeNextValue
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject *_pst_BoneGO,
	EVE_tdst_Track		*_pst_Track,
	EVE_tdst_Event		*_pst_Evt,
	int					_w_Type,
	BOOL				_b_IKEvent
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix		Temp, MIK, I;
	EVE_tdst_Event			*pst_NextEvt;
	ULONG					ul_ID;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	OBJ_tdst_GameObject		*pst_Father;
	short					w_NextType;
	float					f_Time;
	void					*p_NextValue;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_IKEvent)
	{
		/*$F------------------------------------------------------------------------


									IK Event - Non IK Event


		    ------------------------------------------------------------------------*/
		if(_pst_Evt == _pst_Track->pst_AllEvents + (_pst_Track->uw_NumEvents - 1))
			pst_NextEvt = _pst_Track->pst_AllEvents;
		else
			pst_NextEvt = _pst_Evt + 1;

		/*
		 * Test next key; if next key is not an IK key we have to store in current IK key
		 * the ending position/rotation to avoid complex computing into event player
		 */
		w_NextType = EVE_w_Event_InterpolationKey_GetType(pst_NextEvt);

		if(w_NextType & EVE_InterKeyType_BlockedForIK) return;

		f_Time = EVE_f_GetTimeFromStartToEvent(_pst_Track, pst_NextEvt);
		pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

		EVE_Event_InterpolationKey_SpecialPlay
		(
			_pst_GO,
			pst_GOAnim->apst_Anim[0]->pst_Data,
			f_Time,
			-1,
			-1,
			&EVE_st_MagicBox,
			EVE_ast_Gizmo
		);

		ul_ID = TAB_ul_PFtable_GetElemIndexWithPointer(pst_GOAnim->pst_Skeleton->pst_AllObjects, _pst_BoneGO);
		MATH_CopyMatrix(&MIK, &EVE_ast_Gizmo[ul_ID]);

		/*
		 * We want to compute the Local Matrix of this bone at this next event into the
		 * Magic Box coordinate system to stock it at the end of the previous Event.
		 */
		pst_Father = _pst_BoneGO;

		while(1)
		{

			pst_Father = OBJ_pst_GetFatherInit(pst_Father);
			ul_ID = TAB_ul_PFtable_GetElemIndexWithPointer(pst_GOAnim->pst_Skeleton->pst_AllObjects, pst_Father);
			if(ul_ID == -1) break;
			MATH_CopyMatrix(&Temp, &MIK);
			MATH_MulMatrixMatrix(&MIK, &Temp, &EVE_ast_Gizmo[ul_ID]);

		}

	}
	else
	{
		/*$F------------------------------------------------------------------------


									Non IK Event - IK Event


		    ------------------------------------------------------------------------*/
		if(_pst_Evt == _pst_Track->pst_AllEvents + (_pst_Track->uw_NumEvents - 1))
			pst_NextEvt = _pst_Track->pst_AllEvents;
		else
			pst_NextEvt = _pst_Evt + 1;
		w_NextType = EVE_w_Event_InterpolationKey_GetType(pst_NextEvt);

		/*
		 * Test next key; if next key is an IK key we have to store in current non-IK key
		 * the ending position/rotation to avoid complex computing into event player
		 */
		if(!(w_NextType & EVE_InterKeyType_BlockedForIK)) return;

		f_Time = EVE_f_GetTimeFromStartToEvent(_pst_Track, pst_NextEvt);
		pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

		EVE_Event_InterpolationKey_SpecialPlay
		(
			_pst_GO,
			pst_GOAnim->apst_Anim[0]->pst_Data,
			f_Time,
			-1,
			-1,
			&EVE_st_MagicBox,
			EVE_ast_Gizmo
		);

		ul_ID = TAB_ul_PFtable_GetElemIndexWithPointer(pst_GOAnim->pst_Skeleton->pst_AllObjects, _pst_BoneGO);

		/*
		 * Get what Bone Local Matrix would be if we were at the start of the next Event.
		 * Dont forget that this next event is an IK one an so, the Local matrix of this
		 * bone is in the MagicBox coordinate system !!
		 */
		MATH_CopyMatrix(&MIK, &EVE_ast_Gizmo[ul_ID]);

		/*
		 * We want to compute the Local Matrix of this bone at this next event into its
		 * Father coordinate system to stock it at the end of the previous Event.
		 */
		pst_Father = _pst_BoneGO;
		MATH_SetIdentityMatrix(&I);

		while(1)
		{

			pst_Father = OBJ_pst_GetFatherInit(pst_Father);
			ul_ID = TAB_ul_PFtable_GetElemIndexWithPointer(pst_GOAnim->pst_Skeleton->pst_AllObjects, pst_Father);
			if(ul_ID == -1) break;
			MATH_CopyMatrix(&Temp, &I);
			MATH_MulMatrixMatrix(&I, &Temp, &EVE_ast_Gizmo[ul_ID]);

		}


		MATH_CopyMatrix(&Temp, &I);
		MATH_InvertMatrix(&I, &Temp);
		MATH_CopyMatrix(&Temp, &MIK);
		MATH_MulMatrixMatrix(&MIK, &Temp, &I);
	}

	/* Store ending position into key */
	if(!(_w_Type & EVE_InterKeyType_HasNextValue))
		EVE_Event_InterpolationKey_AllocData(_pst_Evt, (short) (_w_Type | EVE_InterKeyType_HasNextValue), -1);
	p_NextValue = EVE_pst_Event_InterpolationKey_GetNextValue(_pst_Evt);
	if(_w_Type & EVE_InterKeyType_Translation_Mask)
	{
		MATH_CopyVector((MATH_tdst_Vector *) p_NextValue, &MIK.T);
	}
	else if(_w_Type & EVE_InterKeyType_Rotation_Matrix)
	{
		MATH_CopyMatrix((MATH_tdst_Matrix *) p_NextValue, &MIK);
		MATH_ClearTranslation((MATH_tdst_Matrix *) p_NextValue);
	}
	else if(_w_Type & EVE_InterKeyType_Rotation_Quaternion)
	{
		MATH_ClearTranslation(&MIK);
		MATH_ConvertMatrixToQuaternion((MATH_tdst_Quaternion *) p_NextValue, &MIK);
	}

	/* EVE_Event_InterpolationKey_Trace(_pst_Evt, "Compute Next Value ..."); */
}

/*
 =======================================================================================================================
    Aim:    Recompute the Event Data depending on Global/Local matrix.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_Compute
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject *_pst_OwnerGO,
	EVE_tdst_ListTracks *_pst_ListTrack,
	EVE_tdst_Track		*_pst_Track,
	EVE_tdst_Event		*_pst_Evt,
	int					_w_Type,
    int                 _i_New
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix		*M, MR;
	MATH_tdst_Matrix I		ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Vector		*V, *W, Pos;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	TAB_tdst_PFelem			*pst_CurrentBone;
	OBJ_tdst_GameObject		*pst_BoneGO;
	MATH_tdst_Quaternion	Q;
	short					w_NextType;
	BOOL					b_Gizmo, b_Previous;
	BOOL					b_IK;
	int						i_Index;
	BOOL					b_SkipTranslations, b_MagicBoxChild;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Test validity of parameters */
	if((EVE_w_Event_InterpolationKey_GetFlags(_pst_Evt) & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) return;
	if((!_pst_GO)) return;

	b_Gizmo = (_pst_Track->uw_Gizmo != (USHORT) - 1);
	b_Previous = FALSE;
	b_IK = (_w_Type & EVE_InterKeyType_BlockedForIK);

	if(!b_Gizmo)
	{
		b_SkipTranslations = FALSE;
		b_MagicBoxChild = FALSE;

		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
			M = OBJ_pst_GetLocalMatrix(_pst_GO);
		else
			M = OBJ_pst_GetAbsoluteMatrix(_pst_GO);

		//if(EVE_Event_InterpolationKey_IsChild(_pst_GO))
		//	M = OBJ_pst_GetLocalMatrix(_pst_GO);
		//else
		//	M = OBJ_pst_GetAbsoluteMatrix(_pst_GO);

		if(EVE_b_Track_UseFlash(_pst_Track, _pst_GO, _pst_OwnerGO, _pst_ListTrack))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix	*pst_Flash;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Flash = OBJ_pst_GetFlashMatrix(_pst_GO);

			if(!pst_Flash)
			{
				OBJ_SetFlashMatrix(_pst_GO, &MATH_gst_IdentityMatrix);
				pst_Flash = OBJ_pst_GetFlashMatrix(_pst_GO);
			}

			/*
			 * The Flash Matrix may be the init one.(Identity) If it is the case, we copy the
			 * current OwnerGO Global matrix into the Flash Matrix.
			 */
			if((pst_Flash->T.x == 0.0f) && (pst_Flash->T.x == 0.0f) && (pst_Flash->T.x == 0.0f))
			{
				MATH_CopyMatrix(pst_Flash, _pst_GO->pst_GlobalMatrix);
			}

			MATH_InvertMatrix(&I, pst_Flash);
			MATH_MulMatrixMatrix(&MR, M, &I);
		}
		else
		{
			MATH_CopyMatrix(&MR, M);
		}
	}
	else
	{
		if(!_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims) return;
		if(!_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton) return;

		pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

		pst_CurrentBone = pst_GOAnim->pst_Skeleton->pst_AllObjects->p_Table + _pst_Track->uw_Gizmo;
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

		MATH_CopyMatrix(&MR, OBJ_pst_GetLocalMatrix(pst_BoneGO));

		/*************************************
			TODO : Faire ça proprement
		 *************************************/
		/* Skip Translations ? */
		b_SkipTranslations = pst_BoneGO->pst_Extended &&
							(pst_BoneGO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_SkipTOnBone) &&
							(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Hierarchy)) &&
							(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_HasInitialPos));
		b_MagicBoxChild = (OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Hierarchy)) &&
						  (OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_HasInitialPos)) &&
						  OBJ_pst_GetFather(pst_BoneGO)== _pst_GO;
		if (b_SkipTranslations && b_MagicBoxChild)
		{
			float f_Ratio =
				0.45472884f / MATH_f_NormVector(&(pst_BoneGO->pst_GlobalMatrix + 1)->T);
			V = MATH_pst_GetTranslation(&MR);
			MATH_ScaleEqualVector( V, f_Ratio );
		}

		if(b_IK) EVE_Event_InterpolationKey_RecomputeFatherMatrixWhenIK(pst_BoneGO);

		if(EVE_Event_InterpolationKey_TransitionIK(_pst_Track, _pst_Evt, FALSE))
		{
			EVE_Event_InterpolationKey_ComputeNextValue
			(
				_pst_GO,
				pst_BoneGO,
				_pst_Track,
				_pst_Evt,
				_w_Type,
				(_w_Type & EVE_InterKeyType_BlockedForIK)
			);
			b_Previous = TRUE;
		}
		else
		{
			if(_w_Type & EVE_InterKeyType_HasNextValue)
			{
				_w_Type &= ~EVE_InterKeyType_HasNextValue;
				EVE_Event_InterpolationKey_AllocData(_pst_Evt, (short) _w_Type, -1);
				b_Previous = TRUE;
			}
		}
	}

	/*$F ----- Store position into key data ----- */

	/* Translation */
	V = EVE_pst_Event_InterpolationKey_GetPos(_pst_Evt);
	if(V && (!b_SkipTranslations || b_MagicBoxChild))
	{
		MATH_CopyVector(V, MATH_pst_GetTranslation(&MR));
		if(_i_New && ((_w_Type & EVE_InterKeyType_Translation_Mask) != EVE_InterKeyType_Translation_0) )
		{
			i_Index = (_pst_Evt - _pst_Track->pst_AllEvents) + 1;
			if(i_Index == _pst_Track->uw_NumEvents) i_Index = 0;
			W = EVE_pst_Event_InterpolationKey_GetPos(_pst_Track->pst_AllEvents + i_Index);
			if(W)
			{
				MATH_AddVector(&Pos, V, W);
				MATH_ScaleEqualVector(&Pos, 0.5f);
			}
			else
				MATH_CopyVector(&Pos, V);
			MATH_CopyVector(V + 1, &Pos);
			if((_w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_2)
				MATH_CopyVector(V + 2, &Pos);
		}
	}

	/* Matrix rotation */
	M = EVE_pst_Event_InterpolationKey_GetRotation(_pst_Evt);
	if(M)
	{
		MATH_ClearTranslation(&MR);
		MATH_CopyMatrix(M, &MR);
	}

	/* Matrix quaternion */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		char					*pc_Data;
		int						w_Type;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Evt);
		if(w_Type & EVE_InterKeyType_Rotation_Quaternion)
		{
			MATH_ClearTranslation(&MR);
			MATH_ConvertMatrixToQuaternion(&Q, &MR);

			pc_Data = (char *) _pst_Evt->p_Data + 4;
#ifdef ACTIVE_EDITORS
			MATH_CopyQuaternion(((MATH_tdst_Quaternion *) pc_Data), &Q);
#else
			if(w_Type & EVE_InterKey_CompressedQuaternion)
			{
				MATH_CompressUnitQuaternion(((MATH_tdst_CompressedQuaternion *) pc_Data), &Q);
			}
			else
			{
				MATH_CopyQuaternion(((MATH_tdst_Quaternion *) pc_Data), &Q);
			}
#endif
		}
	}

	/*
	 * If the current Event was particular (NewTransitionIK or (Not NewTransitionIK
	 * and Next-Value Flag)), we have to check for previous Event hypothetic Next-
	 * value.
	 */
	if(b_Previous)
	{
		/* Check if we are not in the first event. */
		if(_pst_Evt != _pst_Track->pst_AllEvents)
		{
			_pst_Evt--;
			w_NextType = EVE_w_Event_InterpolationKey_GetType(_pst_Evt);

			if(EVE_Event_InterpolationKey_TransitionIK(_pst_Track, _pst_Evt, FALSE))
			{
				EVE_Event_InterpolationKey_ComputeNextValue
				(
					_pst_GO,
					pst_BoneGO,
					_pst_Track,
					_pst_Evt,
					w_NextType,
					(w_NextType & EVE_InterKeyType_BlockedForIK)
				);
			}
			else if(w_NextType & EVE_InterKeyType_HasNextValue)
			{
				w_NextType &= ~EVE_InterKeyType_HasNextValue;
				EVE_Event_InterpolationKey_AllocData(_pst_Evt, (short) w_NextType, -1);
			}
		}
	}
}

/*
 =======================================================================================================================
    Aim:    We parse all Events of given Track and update all next values.
 =======================================================================================================================
 */
void EVE_Event_InterpolationKey_UpdateNextValue
(
	OBJ_tdst_GameObject *_pst_GO,
	EVE_tdst_ListTracks *_pst_ListTrack,
	EVE_tdst_Track		*_pst_Track
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event			*pst_Evt;
	short					w_Type;
	int						i;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	TAB_tdst_PFelem			*pst_CurrentBone;
	OBJ_tdst_GameObject		*pst_BoneGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	if(pst_GOAnim->pst_Skeleton)
	{
		pst_CurrentBone = pst_GOAnim->pst_Skeleton->pst_AllObjects->p_Table + _pst_Track->uw_Gizmo;
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
	}

	for(i = 0; i < _pst_Track->uw_NumEvents; i++)
	{
		pst_Evt = _pst_Track->pst_AllEvents + i;
		if((EVE_w_Event_InterpolationKey_GetFlags(pst_Evt) & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) continue;
		w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Evt);

		if(EVE_Event_InterpolationKey_TransitionIK(_pst_Track, pst_Evt, TRUE))
		{
			EVE_Event_InterpolationKey_ComputeNextValue
			(
				_pst_GO,
				pst_BoneGO,
				_pst_Track,
				pst_Evt,
				w_Type,
				(w_Type & EVE_InterKeyType_BlockedForIK)
			);
		}
		else
		{
			if(w_Type & EVE_InterKeyType_HasNextValue)
			{
				w_Type &= ~EVE_InterKeyType_HasNextValue;
				EVE_Event_InterpolationKey_AllocData(pst_Evt, w_Type, -1);
			}
		}
	}
}

#endif /* ACTIVE_EDITORS */

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
