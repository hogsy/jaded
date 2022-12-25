/*$T EVEnt_timekey.c GC! 1.081 08/07/00 14:45:59 */


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
#include "EVEstruct.h"
#include "EVEconst.h"
#include "EVEplay.h"
#include "AIinterp/Sources/Events/EVEnt_timekey.h"
#include "BIGfiles/LOAding/LOAread.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Access functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
short EVE_w_Event_TimeKey_GetType(EVE_tdst_Event *_pst_Event)
{
	if(!_pst_Event->p_Data) return -1;
	return(*((short *) _pst_Event->p_Data + 1));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_TimeKey_AllocData(EVE_tdst_Event *_pst_Event, short _w_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	short	w_Size;
	short	w_OldType;
	float	pos, speed, acc;
	float	*pf_Params, params[3];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pos = speed = acc = 0;
	if(_pst_Event->p_Data)
	{
		w_OldType = EVE_w_Event_TimeKey_GetType(_pst_Event);
		pf_Params = ((float *) _pst_Event->p_Data) + 1;

		switch(w_OldType)
		{
		case EVE_KeyType_Speed:
			speed = pf_Params[0];
			break;
		case EVE_KeyType_SpeedPos:
			speed = pf_Params[0];
			pos = pf_Params[1];
			break;
		case EVE_KeyType_Acceleration:
			acc = pf_Params[0];
			break;
		case EVE_KeyType_AccelerationSpeed:
			acc = pf_Params[0];
			speed = pf_Params[1];
			break;
		case EVE_KeyType_AccelerationPos:
			acc = pf_Params[0];
			pos = pf_Params[1];
			break;
		case EVE_KeyType_AccelerationSpeedPos:
			acc = pf_Params[0];
			speed = pf_Params[1];
			pos = pf_Params[2];
			break;
		case EVE_KeyType_TimeFactorStart:
		case EVE_KeyType_TimeFactor:
			acc = pf_Params[0];
			break;
		}

		MEM_Free(_pst_Event->p_Data);
	}

	w_Size = 4;

	switch(_w_Type)
	{
	case EVE_KeyType_Speed:
		w_Size += 4;
		params[0] = speed;
		break;
	case EVE_KeyType_SpeedPos:
		w_Size += 8;
		params[0] = speed;
		params[1] = pos;
		break;
	case EVE_KeyType_Acceleration:
		w_Size += 4;
		params[0] = acc;
		break;
	case EVE_KeyType_AccelerationSpeed:
		w_Size += 8;
		params[0] = acc;
		params[1] = speed;
		break;
	case EVE_KeyType_AccelerationPos:
		w_Size += 8;
		params[0] = acc;
		params[1] = pos;
		break;

	case EVE_KeyType_AccelerationSpeedPos:
		w_Size += 12;
		params[0] = acc;
		params[1] = speed;
		params[2] = pos;
		break;
	case EVE_KeyType_TimeFactorStart:
	case EVE_KeyType_TimeFactor:
		params[0] = acc;
		params[1] = 0;
		w_Size += 8;
		break;
	}

	_pst_Event->p_Data = MEM_p_Alloc(w_Size);
	if(w_Size > 4) L_memcpy(((char *) _pst_Event->p_Data) + 4, params, w_Size - 4);

	*(short *) _pst_Event->p_Data = (short) w_Size;
	*((short *) _pst_Event->p_Data + 1) = _w_Type;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_TimeKey_Save(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~*/
	ULONG	ul_Size;
	/*~~~~~~~~~~~~*/

	if(_pst_Event->p_Data == NULL)
	{
		ul_Size = 0;
		SAV_Buffer(&ul_Size, 4);
		return;
	}

	SAV_Buffer(_pst_Event->p_Data, *(short *) _pst_Event->p_Data);
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVE_Event_TimeKey_Load(EVE_tdst_Event *_pst_Event, char *_pc_Buffer, char *_pc_DestBuffer)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	short	w_Size, w_Type;
	char * pc_Buf = _pc_Buffer;
	float * pf_Data;
	/*~~~~~~~~~~~~~~~~~~~*/
	
	w_Size = LOA_ReadShort(&pc_Buf);
	w_Type = LOA_ReadShort(&pc_Buf);

#ifdef ACTIVE_EDITORS
	EVE_Event_TimeKey_AllocData(_pst_Event, w_Type);
#else
	_pst_Event->p_Data = _pc_DestBuffer;
#endif // #ifdef ACTIVE_EDITORS

	pf_Data = (float *) _pst_Event->p_Data;
	*((SHORT *)pf_Data)++ = w_Size;
	*((SHORT *)pf_Data)++ = w_Type;
	
	// Yes, its normal not to have break;'s because
	// the number of floats to load depends on the type
	switch(w_Type)
	{
// Loads 3 floats
	case EVE_KeyType_AccelerationSpeedPos:
		_LOA_ReadFloat(&pc_Buf, pf_Data++, LOA_eBinEngineData);
// Loads 2 floats
	case EVE_KeyType_AccelerationSpeed:
	case EVE_KeyType_SpeedPos:
	case EVE_KeyType_AccelerationPos:
	case EVE_KeyType_TimeFactorStart:
	case EVE_KeyType_TimeFactor:
		_LOA_ReadFloat(&pc_Buf, pf_Data++, LOA_eBinEngineData);
// Loads 1 float
	case EVE_KeyType_Speed:
	case EVE_KeyType_Acceleration:
		_LOA_ReadFloat(&pc_Buf, pf_Data++, LOA_eBinEngineData);
	}

	ERR_X_ErrorAssert(((pc_Buf - _pc_Buffer) == w_Size), "EVE_Event_TimeKey_Load: size mismatch while loading.","");
	return (pc_Buf - _pc_Buffer);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_TimeKey_Resolve(EVE_tdst_Event *_pst_Event)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVE_tdst_Event *EVE_Event_TimeKey_Play(EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt, *pst_OtherEvt;
	short			w_Type;
	float			t;
	float			*f_Params;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_TimeKey_GetType(_pst_Event);
	if(w_Type == -1) return _pst_Event;

	f_Params = ((float *) _pst_Event->p_Data) + 1;

	switch(w_Type)
	{
	case EVE_KeyType_Linear:
		t = EVE_gpst_CurrentParam->f_Time;
		pst_Evt = EVE_gpst_CurrentTrack->pst_AllEvents;
		while(pst_Evt != _pst_Event) t += EVE_FrameToTime((pst_Evt++)->uw_NumFrames);
		break;
	case EVE_KeyType_Square:
		t = EVE_gpst_CurrentParam->f_Time / EVE_FrameToTime(_pst_Event->uw_NumFrames);
		t *= t;
		t *= EVE_FrameToTime(_pst_Event->uw_NumFrames);
		pst_Evt = EVE_gpst_CurrentTrack->pst_AllEvents;
		while(pst_Evt != _pst_Event) t += EVE_FrameToTime((pst_Evt++)->uw_NumFrames);
		break;
	case EVE_KeyType_Speed:
		t = EVE_gpst_CurrentParam->f_Time * f_Params[0];
		break;
	case EVE_KeyType_SpeedPos:
		t = EVE_gpst_CurrentParam->f_Time * f_Params[0] + f_Params[1];
		break;
	case EVE_KeyType_Acceleration:
		t = EVE_gpst_CurrentParam->f_Time * EVE_gpst_CurrentParam->f_Time * f_Params[0] * 0.5f;
		break;
	case EVE_KeyType_AccelerationSpeed:
		t = EVE_gpst_CurrentParam->f_Time * (EVE_gpst_CurrentParam->f_Time * f_Params[0] * 0.5f + f_Params[1]);
		break;
	case EVE_KeyType_AccelerationPos:
		t = EVE_gpst_CurrentParam->f_Time * EVE_gpst_CurrentParam->f_Time * f_Params[0] * 0.5f + f_Params[1];
		break;
	case EVE_KeyType_AccelerationSpeedPos:
		t = EVE_gpst_CurrentParam->f_Time *
			(EVE_gpst_CurrentParam->f_Time * f_Params[0] * 0.5f + f_Params[1]) +
			f_Params[2];
		break;

	case EVE_KeyType_TimeFactorStart:
		t = f_Params[0] * EVE_gpst_CurrentParam->f_Time;
		pst_Evt = EVE_gpst_CurrentTrack->pst_AllEvents;
		while(pst_Evt != _pst_Event) t += EVE_FrameToTime((pst_Evt++)->uw_NumFrames);
		break;
	case EVE_KeyType_TimeFactor:
		t = f_Params[0] * EVE_gpst_CurrentParam->f_Time;

		/* t+= f_Params[1]; */
		pst_OtherEvt = _pst_Event - 1;
		while(1)
		{
			t += *(((float *) pst_OtherEvt->p_Data) + 1) * EVE_FrameToTime(pst_OtherEvt->uw_NumFrames);
			if(EVE_w_Event_TimeKey_GetType(pst_OtherEvt) == EVE_KeyType_TimeFactorStart) break;
			pst_OtherEvt--;
			if(pst_OtherEvt < EVE_gpst_CurrentTrack->pst_AllEvents) return _pst_Event;
		};
		pst_Evt = EVE_gpst_CurrentTrack->pst_AllEvents;
		while(pst_Evt != pst_OtherEvt) t += EVE_FrameToTime((pst_Evt++)->uw_NumFrames);

		break;
	}

	*(float *) &EVE_gpst_CurrentParam->i_Param1 = t;

	return _pst_Event;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_TimeKey_ComputeSP(EVE_tdst_Track *_pst_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt, *pst_Evt1, *pst_Evt2;
	float			acc, speed, pos, *params;
	float			t, acc0, speed0, pos0;
	int				i, j, i_First;
	float			L, l0, l1;
	short			w_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	acc = 0;
	speed = 0;
	pos = 0;
	i_First = 1;

	for(i = 0, pst_Evt = _pst_Track->pst_AllEvents; i < _pst_Track->uw_NumEvents; i++, pst_Evt++)
	{
		switch(pst_Evt->w_Flags & EVE_C_EventFlag_Type)
		{
		case EVE_C_EventFlag_TimeKey:
			w_Type = EVE_w_Event_TimeKey_GetType(pst_Evt);
			params = ((float *) pst_Evt->p_Data) + 1;

			acc0 = 0;
			speed0 = 0;
			pos0 = 0;

			switch(w_Type)
			{
			case EVE_KeyType_Speed:
				if(!i_First) params[0] = speed;
				speed0 = params[0];
				break;
			case EVE_KeyType_SpeedPos:
				if(!i_First) params[0] = speed, params[1] = pos;
				speed0 = params[0];
				pos0 = params[1];
				break;
			case EVE_KeyType_Acceleration:
				acc0 = params[0];
				break;
			case EVE_KeyType_AccelerationSpeed:
				if(!i_First) params[1] = speed;
				acc0 = params[0];
				speed0 = params[1];
				break;
			case EVE_KeyType_AccelerationPos:
				if(!i_First) params[1] = pos;
				acc0 = params[0];
				pos0 = params[1];
				break;
			case EVE_KeyType_AccelerationSpeedPos:
				if(!i_First) params[1] = speed, params[2] = pos;
				acc0 = params[0];
				speed0 = params[1];
				pos0 = params[2];
				break;

			case EVE_KeyType_TimeFactorStart:
				L = EVE_FrameToTime(pst_Evt->uw_NumFrames);
				pst_Evt1 = pst_Evt + 1;
				for(j = i + 1, pst_Evt1 = pst_Evt + 1; j < _pst_Track->uw_NumEvents; j++, pst_Evt1++)
				{
					if(EVE_w_Event_TimeKey_GetType(pst_Evt1) == EVE_KeyType_TimeFactorStart) break;
					L += EVE_FrameToTime(pst_Evt1->uw_NumFrames);
				}

				l0 = 0;
				for(pst_Evt2 = pst_Evt; pst_Evt2 < pst_Evt1; pst_Evt2++)
				{
					l1 = EVE_FrameToTime(pst_Evt2->uw_NumFrames) **(((float *) pst_Evt2->p_Data) + 1);
					if(((pst_Evt2 == pst_Evt1 - 1) && (l0 + l1 < L)) || (l0 + l1 > L))
					{
						l1 = L - l0;
						t = l1 ? (l1 / EVE_FrameToTime(pst_Evt2->uw_NumFrames)) : 0;
						*(((float *) pst_Evt2->p_Data) + 1) = t;
						l0 = L;
					}
					else
						l0 += l1;
				}

			default: pos0 = pos + EVE_FrameToTime(pst_Evt->uw_NumFrames);
			}

			i_First = 0;

			t = EVE_FrameToTime(pst_Evt->uw_NumFrames);
			speed = acc0 * t + speed0;
			pos = acc0 * 0.5f * t * t + speed0 * t + pos0;
			break;

		default:
			pos += EVE_FrameToTime(pst_Evt->uw_NumFrames);
			break;
		}
	}
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
