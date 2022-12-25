/*$T NIMtoTracks.c GC! 1.081 11/08/00 10:43:23 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/BASsys.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "MATHs/MATH.h"
#include "ANImation/ANIstruct.h"
#include "ANImation/ANIaccess.h"
#include "ANImation/ANIinit.h"
#include "ANImation/ANIplay.h"
#include "WORld/WORstruct.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "AIinterp/Sources/Events/EVEsave.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "EDIpaths.h"

#include "NIMtoTracks.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
extern WOR_tdst_Universe	WOR_gst_Universe;

/*$F
 -----------------------------------------------------------------------------------------------------------------------




			
									  (NIM ---> TRACKS)
									
									FUNCTIONS/STRUCTURES

									  DELETE THEM ASAP




 -----------------------------------------------------------------------------------------------------------------------
 */

/*$F
 ---------------------------------------------------------------------------------------------------
								ANI_st_Canal
 ---------------------------------------------------------------------------------------------------
 */

struct ANI_st_Canal
{
	ULONG					*pst_CurrentEvent;				/* Current event for the bone */
	ULONG					*pst_LastEvent;

	USHORT					uwCurrentEventFrameNumber;
	UCHAR					uc_RealTimeIK;
	UCHAR					uc_NewFather;

	MATH_tdst_Vector		st_BeforePreviousTranslation;	/* | We need */
	MATH_tdst_Vector		st_PreviousTranslation;			/* | 4 translations */
	MATH_tdst_Vector		st_NextTranslation;				/* | for a cubic */
	MATH_tdst_Vector		st_AfterNextTranslation;		/* | interpolation */

	USHORT					uwFrameNumberBeforePreviousKey;
	USHORT					uwFrameNumberPreviousKey;		/* Time of the previous key */
	USHORT					uwFrameNumberNextKey;			/* Time of the next key */
	USHORT					uwFrameNumberAfterNextKey;

	MATH_tdst_Quaternion	st_PreviousQuaternion;			/* Quaternion of the previous key */
	MATH_tdst_Quaternion	st_NextQuaternion;				/* Quaternion of the next key */
	MATH_tdst_Quaternion	st_AfterNextQuaternion;
	float					fTheta;							/* Angle between the 2 quaternions */
	float					fInvSinTheta;					/* Inverse of the sine of the angle */
};

/*$F
 ---------------------------------------------------------------------------------------------------
								ANI_st_Data
 ---------------------------------------------------------------------------------------------------
 */

struct ANI_st_Data
{
	UCHAR				uc_Flags;					/* Flags with version number */
	UCHAR				uc_NumberOfCanal;			/* Number of canals played by the anim (canal 0 is Magic Box) */
	USHORT				uw_NumberOfFrame;			/* Number of frames of the anim */
	MATH_tdst_Vector	st_TotalDeplacement;		/* UNUSED but present in the NIM files */
	USHORT				uw_NumStandardBones;		/* Number of standart bones */
	USHORT				uw_MaxAddBonesIndex;		/* Number of additionnal bones in this Anim */
	ULONG				aul_BonesPresentFlags[2];	/* BitField of Bones presence in this Anim */
	USHORT				auw_FirstEventForCanal[ANI_C_MaxNumberOfCanal + 1];
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ANI_b_CanalUsed(struct ANI_st_Data *_pst_Data, ULONG i)
{
	if(_pst_Data->uc_Flags != 2) return TRUE;

	if(i < 32)
		return(_pst_Data->aul_BonesPresentFlags[0] & (1 << i));
	else
		return(_pst_Data->aul_BonesPresentFlags[1] & (1 << (i - 32)));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ANI_InitCanal
(
	struct ANI_st_Canal *_pst_Canal,
	struct ANI_st_Data	*_pst_Data,
	int					_iCanalIndex,
	char				*_psz_Name,
	BOOL				*_pb_WithMagic
)
{
	/*~~~~~~~~~~~~~~~~*/
	int		iDelta;
	BOOL	b_ValidAnim;
	/*~~~~~~~~~~~~~~~~*/

	b_ValidAnim = TRUE;

	iDelta = ((int) _pst_Data->auw_FirstEventForCanal[_iCanalIndex]) * 4 + 8;

	if(_pst_Data->uc_Flags == 0)
	{
		/* There, we have an Old NIM format without Magic box */
		iDelta += 4;
		*_pb_WithMagic = FALSE;
	}
	else
		*_pb_WithMagic = TRUE;

	if(ANI_i_GetEventType(*(ULONG *) (((char *) _pst_Data) + iDelta)) != ANI_C_EventFlag_KeyEvent)
		b_ValidAnim = FALSE;

	_pst_Canal->pst_CurrentEvent = (ULONG *) (((char *) _pst_Data) + iDelta);
	iDelta = ((int) _pst_Data->auw_FirstEventForCanal[_iCanalIndex + 1]) * 4 + 8;
	if(!_pst_Data->uc_Flags) iDelta += 4;
	_pst_Canal->pst_LastEvent = (ULONG *) (((char *) _pst_Data) + iDelta);

	_pst_Canal->uwCurrentEventFrameNumber = 0;
	_pst_Canal->uc_RealTimeIK = FALSE;
	_pst_Canal->uc_NewFather = 0xFF;
	_pst_Canal->uwFrameNumberBeforePreviousKey = _pst_Canal->uwFrameNumberPreviousKey = _pst_Canal->uwFrameNumberNextKey = _pst_Canal->uwFrameNumberAfterNextKey = 0xFFFF;
	_pst_Canal->fInvSinTheta = 0.0f;

	return b_ValidAnim;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
struct ANI_st_Canal *ANI_FillCanals(struct ANI_st_Data *_pst_Data, char *_psz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct ANI_st_Canal *apst_Canal, *pst_Canal;
	BOOL				b_ValidAnim, b_WithMagic;
	ULONG				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	apst_Canal = (struct ANI_st_Canal *) MEM_p_Alloc(_pst_Data->uc_NumberOfCanal * sizeof(struct ANI_st_Canal));
	b_ValidAnim = TRUE;

	for(pst_Canal = apst_Canal, i = 0; i < _pst_Data->uc_NumberOfCanal; i++, pst_Canal++)
	{
		if(!ANI_InitCanal(pst_Canal, _pst_Data, i, _psz_Name, &b_WithMagic)) b_ValidAnim = FALSE;
	}

	if(!b_ValidAnim) ERR_X_Warning(0, "Invalid Anim Event. All events should be Keys!", _psz_Name);
	if(!b_WithMagic) ERR_X_Warning(0, "Anim without Magic Box detected", _psz_Name);

	return apst_Canal;
}

/*
 =======================================================================================================================
    Aim:    Gets the conjugate of the quaternion because Q * Q bar = Identity
 =======================================================================================================================
 */
void ANI_InvertQuaternion(MATH_tdst_Quaternion *_pst_Q)
{
	/*~~~~~~~~*/
	LONG	*pl;
	/*~~~~~~~~*/

	pl = (LONG *) _pst_Q;

	pl[0] += 0x80000000;
	pl[1] += 0x80000000;
	pl[2] += 0x80000000;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_SkipNonZRotation(MATH_tdst_Matrix *_pst_Matrix)
{
	_pst_Matrix->Kx = 0.0f;
	_pst_Matrix->Ky = 0.0f;
	_pst_Matrix->Kz = 1.0f;
	_pst_Matrix->Iz = 0.0f;
	_pst_Matrix->Jz = 0.0f;
	MATH_Orthonormalize(_pst_Matrix);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_ConvertAnimDataToListTracks
(
	EVE_tdst_ListTracks *_pst_ListTracks,
	struct ANI_st_Canal *_past_Canal,
	int					_iNumberOfCanal,
	int					_iNumberOfFrame
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct ANI_st_Canal		*pst_Canal;
	MATH_tdst_Vector		st_Translation;
	MATH_tdst_Quaternion	st_Quaternion;
	int						i, iFrameNumber;
	MATH_tdst_Matrix		st_Matrix;
	MATH_tdst_Matrix		ast_MBoxMatrix[6000];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(_iNumberOfFrame <= 6000);

	if(!(_pst_ListTracks && _past_Canal)) return;

	/* We loop thru all the canals. */
	pst_Canal = _past_Canal;
	for(i = 0; i < _iNumberOfCanal; i++, pst_Canal++)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EVE_tdst_Track	*pst_TranslTrack, *pst_RotTrack;
		EVE_tdst_Event	*pst_RotEvent, *pst_TransEvent;
		ULONG			ul_NumberOfTranslationKeys, ul_NumberOfRotationKeys;
		BOOL			b_SameTrans, b_SameRot, b_Optimize;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_RotTrack = &_pst_ListTracks->pst_AllTracks[i * 2];
		pst_TranslTrack = &_pst_ListTracks->pst_AllTracks[i * 2 + 1];
		ul_NumberOfTranslationKeys = ul_NumberOfRotationKeys = 0;

		if(GetAsyncKeyState( VK_SHIFT ) < 0)
		{
			pst_RotTrack->uw_Flags |= EVE_C_Track_Optimized;
			pst_TranslTrack->uw_Flags |= EVE_C_Track_Optimized;
		}

		for(iFrameNumber = 0; iFrameNumber < _iNumberOfFrame; iFrameNumber++)
		{
			/*~~~~~~~~~~~~~~~~~*/
			ULONG	*pEvent;
			ULONG	ul_EventFlag;
			char	*pc;
			int		iEventType;
			/*~~~~~~~~~~~~~~~~~*/

			pst_RotTrack = &_pst_ListTracks->pst_AllTracks[i * 2];
			pst_TranslTrack = &_pst_ListTracks->pst_AllTracks[i * 2 + 1];

			b_SameTrans = FALSE;
			b_SameRot = FALSE;
			b_Optimize = TRUE;
			pEvent = pst_Canal->pst_CurrentEvent;

			ul_EventFlag = *pEvent;
			pc = ((char *) pEvent) + 4;
			iEventType = ANI_i_GetEventType(ul_EventFlag);

			pst_Canal->uwCurrentEventFrameNumber += ANI_i_GetEventRelativeTime(ul_EventFlag);

			switch(iEventType)
			{
			/* Quaternion Interpretation */
			case ANI_C_EventFlag_KeyEvent:
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Quaternion	*Q;
					MATH_tdst_Vector		*T;
					char					**p;
					BOOL					b_LogRot, b_LogTrans;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					Q = &pst_Canal->st_AfterNextQuaternion;
					T = &pst_Canal->st_AfterNextTranslation;
					p = &pc;

					/* Log variables ON/OFF */
					b_LogRot = FALSE;
					b_LogTrans = FALSE;

					switch(ANI_i_GetKeyQuaternionType(ul_EventFlag))
					{
					case ANI_C_EventFlag_SameQuaternion:
						if(b_LogRot) LINK_PrintStatusMsg("Same Quaternion");
						b_SameRot = TRUE;
						break;

					case ANI_C_EventFlag_CompleteQuaternion:
						{
							/*~~~~~~~~~*/
							float	fSum;
							/*~~~~~~~~~*/

							if(b_LogRot) LINK_PrintStatusMsg("Complete Quaternion");
							Q->x = *(float *) * p;
							*p += sizeof(float);
							Q->y = *(float *) * p;
							*p += sizeof(float);
							Q->z = *(float *) * p;
							*p += sizeof(float);
							fSum = fSqr(Q->x) + fSqr(Q->y) + fSqr(Q->z);
							Q->w = (fSum < 1.0f) ? fSqrt(1.0f - fSum) : 0.0f;
						}
						break;

					case ANI_C_EventFlag_CompressedQuaternion:
						if(b_LogRot) LINK_PrintStatusMsg("Compressed Quaternion");
						MATH_UncompressUnitQuaternion(Q, (MATH_tdst_CompressedQuaternion *) * p);
						*p += sizeof(MATH_tdst_CompressedQuaternion);
						break;

					case ANI_C_EventFlag_UltraCompressedQuaternionX:
						if(b_LogRot) LINK_PrintStatusMsg("Ultra Compressed X Quaternion");
						MATH_UltraUncompressXUnitQuaternion(Q, (MATH_tdst_UltraCompressedQuaternion *) * p);
						*p += sizeof(MATH_tdst_UltraCompressedQuaternion);
						break;

					case ANI_C_EventFlag_UltraCompressedQuaternionY:
						if(b_LogRot) LINK_PrintStatusMsg("Ultra Compressed Y Quaternion");
						MATH_UltraUncompressYUnitQuaternion(Q, (MATH_tdst_UltraCompressedQuaternion *) * p);
						*p += sizeof(MATH_tdst_UltraCompressedQuaternion);
						break;

					case ANI_C_EventFlag_UltraCompressedQuaternionZ:
						if(b_LogRot) LINK_PrintStatusMsg("Ultra Compressed Z Quaternion");
						MATH_UltraUncompressZUnitQuaternion(Q, (MATH_tdst_UltraCompressedQuaternion *) * p);
						*p += sizeof(MATH_tdst_UltraCompressedQuaternion);
						break;

					case ANI_C_EventFlag_IdentityQuaternion:
						if(b_LogRot) LINK_PrintStatusMsg("Identity Quaternion");
						MATH_InitQuaternion(Q, 0.0f, 0.0f, 0.0f, 1.0f);
						break;
					}

					/* Translation Interpretation */
					switch(ANI_i_GetKeyTranslationType(ul_EventFlag))
					{
					case ANI_C_EventFlag_SameTranslation:
						if(b_LogTrans) LINK_PrintStatusMsg("Same Translation");
						b_SameTrans = TRUE;
						if((int) * p & 3) *p = (char *) ((((int) * p) | 3) + 1);
						break;

					case ANI_C_EventFlag_CompleteTranslation:
						if(b_LogTrans) LINK_PrintStatusMsg("Complete Translation");
						if((int) * p & 3) *p = (char *) ((((int) * p) | 3) + 1);
						MATH_CopyVector(T, (MATH_tdst_Vector *) * p);
						*p += sizeof(MATH_tdst_Vector);
						break;

					case ANI_C_EventFlag_CompressedAbsoluteTranslation:
						if(b_LogTrans) LINK_PrintStatusMsg("Compressed Absolute Translation");
						MATH_UncompressAbsoluteVector(T, (MATH_tdst_CompressedVector *) * p);
						*p += sizeof(MATH_tdst_CompressedVector);
						if((int) * p & 3) *p = (char *) ((((int) * p) | 3) + 1);
						break;

					case ANI_C_EventFlag_CompressedRelativeTranslation:
						if(b_LogTrans) LINK_PrintStatusMsg("Compressed Relative Translation");
						MATH_UncompressRelativeVector(T, (MATH_tdst_CompressedVector *) * p);
						*p += sizeof(MATH_tdst_CompressedVector);
						if((int) * p & 3) *p = (char *) ((((int) * p) | 3) + 1);
						break;

					case ANI_C_EventFlag_UltraCompressedTranslationX:
						if(b_LogTrans) LINK_PrintStatusMsg("Ultra Compressed X Translation");
						if((int) * p & 3) *p = (char *) ((((int) * p) | 3) + 1);
						MATH_UltraUncompressXVector(T, (MATH_tdst_UltraCompressedVector *) * p);
						*p += sizeof(MATH_tdst_UltraCompressedVector);
						break;

					case ANI_C_EventFlag_UltraCompressedTranslationY:
						if(b_LogTrans) LINK_PrintStatusMsg("Ultra Compressed Y Translation");
						if((int) * p & 3) *p = (char *) ((((int) * p) | 3) + 1);
						MATH_UltraUncompressYVector(T, (MATH_tdst_UltraCompressedVector *) * p);
						*p += sizeof(MATH_tdst_UltraCompressedVector);
						break;

					case ANI_C_EventFlag_UltraCompressedTranslationZ:
						if(b_LogTrans) LINK_PrintStatusMsg("Ultra Compressed Z Translation");
						if((int) * p & 3) *p = (char *) ((((int) * p) | 3) + 1);
						MATH_UltraUncompressZVector(T, (MATH_tdst_UltraCompressedVector *) * p);
						*p += sizeof(MATH_tdst_UltraCompressedVector);
						break;

					case ANI_C_EventFlag_NulTranslation:
						if(b_LogTrans) LINK_PrintStatusMsg("Nul Translation");
						if((int) * p & 3) *p = (char *) ((((int) * p) | 3) + 1);
						MATH_InitVectorToZero(T);
						break;
					}

					MATH_CopyQuaternion(&st_Quaternion, Q);
					MATH_CopyVector(&st_Translation, T);

					MATH_CheckVector(&st_Translation, NULL);
					MATH_CheckQuaternion(&st_Quaternion, NULL);
				}
				break;

			case ANI_C_EventFlag_LongJumpEvent:
				break;

			case ANI_C_EventFlag_IKEvent:
				break;

			case ANI_C_EventFlag_NewFatherEvent:
				break;
			}

			pst_Canal->pst_CurrentEvent = (ULONG *) pc;

			if(i == 0)
			{
				MATH_ConvertQuaternionToMatrix(&ast_MBoxMatrix[iFrameNumber], &st_Quaternion);
				ANI_SkipNonZRotation(&ast_MBoxMatrix[iFrameNumber]);
				MATH_ConvertMatrixToQuaternion(&st_Quaternion, &ast_MBoxMatrix[iFrameNumber]);
				MATH_CopyVector(&ast_MBoxMatrix[iFrameNumber].T, &st_Translation);
				MATH_SetCorrectType(&ast_MBoxMatrix[iFrameNumber]);
			}

			if(i == 1)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix /* st_InvMagicBoxPosition, */ st_MainBonePosition;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				b_SameRot = FALSE;
				b_SameTrans = FALSE;
				MATH_ConvertQuaternionToMatrix(&st_Matrix, &st_Quaternion);
				MATH_CopyVector(&st_Matrix.T, &st_Translation);
				MATH_SetCorrectType(&st_Matrix);

				/*
				 * MATH_InvertMatrix(&st_InvMagicBoxPosition, &ast_MBoxMatrix[0]);
				 * MATH_MulMatrixMatrix(&st_MainBonePosition, &st_Matrix,
				 * &st_InvMagicBoxPosition);
				 */
				MATH_CopyMatrix(&st_MainBonePosition, &st_Matrix);
				MATH_ConvertMatrixToQuaternion(&st_Quaternion, &st_MainBonePosition);
				MATH_CopyVector(&st_Translation, &st_MainBonePosition.T);

				MATH_CheckVector(&st_Translation, NULL);
				MATH_CheckQuaternion(&st_Quaternion, NULL);
			}

			/* Copy rotation in its track */
			if(b_SameRot && b_Optimize && (iFrameNumber != _iNumberOfFrame - 1))
				pst_RotEvent->uw_NumFrames += 1;
			else
			{
				pst_RotEvent = EVE_pst_NewEvent(pst_RotTrack, -1, EVE_C_EventFlag_InterpolationKey);
				pst_RotEvent->uw_NumFrames = 1;
				EVE_Event_InterpolationKey_AllocData(pst_RotEvent, EVE_InterKeyType_Rotation_Quaternion, 0);
				EVE_Event_InterpolationKey_GetQuaternion(pst_RotEvent, &st_Quaternion);
			}

			/* Copy translation in its track */
			if(b_SameTrans && b_Optimize && (iFrameNumber != _iNumberOfFrame - 1))
				pst_TransEvent->uw_NumFrames += 1;
			else
			{
				pst_TransEvent = EVE_pst_NewEvent(pst_TranslTrack, -1, EVE_C_EventFlag_InterpolationKey);
				pst_TransEvent->uw_NumFrames = 1;
				EVE_Event_InterpolationKey_AllocData(pst_TransEvent, EVE_InterKeyType_Translation_0, 0);
				MATH_CopyVector(EVE_pst_Event_InterpolationKey_GetPos(pst_TransEvent), &st_Translation);
			}

			/* We dont want an interpolation between the last frame and the first one. */
			if(iFrameNumber == _iNumberOfFrame - 1)
			{
				pst_RotEvent->uw_NumFrames = 0;
				pst_TransEvent->uw_NumFrames = 0;
			}
		}
	}
}

/*$F
 =======================================================================================================================
 Note: Info contained in the Nim.
		
		  - uc_Flags					(1)
		  - uc_NumberOfCanal			(1)
		  - uw_NumberOfFrame			(2)
		  - Total Deplacement			(12)	
		  - uw_NumStandardBones			(2)
		  - uw_MaxAddBonesIndex			(2)
		  - aul_BonesPresentFlags[2]	(8)
		  - All the Canals				(...)
 =======================================================================================================================
 */

struct ANI_st_Data *ANI_pst_NIMLoad(BIG_INDEX ul_DataIndex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct ANI_st_Data	*pst_Data;
	char				*pc_buf;
	ULONG				ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	pc_buf = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_DataIndex), &ul_Size);

	pst_Data = (struct ANI_st_Data *) MEM_p_Alloc(ul_Size);
	L_memset(pst_Data, 0, ul_Size);
	L_memcpy(pst_Data, pc_buf, 4);
	L_memcpy(&pst_Data->uw_NumStandardBones, pc_buf + 16, 12);
	L_memcpy(&pst_Data->auw_FirstEventForCanal, pc_buf + 28, ul_Size - 28);

	L_free(pc_buf);
#endif
	return pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ANI_b_GizmoAlreadyHere(EVE_tdst_ListTracks *_pst_ListTracks, USHORT _uw_GizmoToTest)
{
	/*~~~*/
	int	i;
	/*~~~*/

	for(i = 0; i < (int) _pst_ListTracks->uw_NumTracks; i++)
	{
		if(_pst_ListTracks->pst_AllTracks[i].uw_Gizmo == _uw_GizmoToTest) 
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_MergeListTracks(EVE_tdst_Data *_pst_NewData, EVE_tdst_ListTracks *_pst_OldList, BOOL _b_MergeMode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			i, j;
	ULONG			Start, End, Step;
	BOOL			b_TestInf;
	void			*p;
	EVE_tdst_Track	*pst_Track, *pst_ExtraTrack;
	EVE_tdst_Event	*pst_Event, *pst_ExtraEvent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_MergeMode == 0)
	{
		Start = _pst_NewData->pst_ListTracks->uw_NumTracks;
		End = _pst_OldList->uw_NumTracks;
		Step = 1;
		b_TestInf = TRUE;
	}

	if(_b_MergeMode == 1)
	{
		Start = _pst_OldList->uw_NumTracks - 1;
		End = 0;
		Step = -1;
		b_TestInf = FALSE;
	}

	for(i = Start; (b_TestInf ? (i < End) : (i > End)); i += Step)
	{
		pst_ExtraTrack = &_pst_OldList->pst_AllTracks[i];

		/* Merge From the Start */
		if(_b_MergeMode == 0)
		{
			if(pst_ExtraTrack->uw_Flags & EVE_C_Track_Anims)
			{
				if(ANI_b_GizmoAlreadyHere(_pst_NewData->pst_ListTracks, pst_ExtraTrack->uw_Gizmo)) continue;			
			}		
		}


		/* Merge From the End */
		if(_b_MergeMode == 1)
		{
			/* First Anim Track, we return */
			if(pst_ExtraTrack->uw_Flags & EVE_C_Track_Anims) return;
		}

		pst_Track = EVE_pst_NewTrack(_pst_NewData);
		pst_Track->pst_GO = pst_ExtraTrack->pst_GO;
		pst_Track->uw_Flags = pst_ExtraTrack->uw_Flags;
		pst_Track->uw_Gizmo = pst_ExtraTrack->uw_Gizmo;
		pst_Track->uw_NumEvents = pst_ExtraTrack->uw_NumEvents;

		if(pst_ExtraTrack->pst_AllEvents)
		{
			pst_Track->pst_AllEvents = (EVE_tdst_Event *) MEM_p_Alloc(sizeof(EVE_tdst_Event) * pst_ExtraTrack->uw_NumEvents);
		}
		else
			pst_Track->pst_AllEvents = NULL;

		for(j = 0; j < pst_ExtraTrack->uw_NumEvents; j++)
		{
			pst_ExtraEvent = &pst_ExtraTrack->pst_AllEvents[j];
			pst_Event = &pst_Track->pst_AllEvents[j];

			L_memcpy(pst_Event, pst_ExtraEvent, sizeof(EVE_tdst_Event));
			pst_Event->w_Flags &= ~EVE_C_EventFlag_Selected;

			if(pst_ExtraEvent->p_Data)
			{
				p = MEM_p_Alloc(*(short *) pst_ExtraEvent->p_Data);
				pst_Event->p_Data = p;
				L_memcpy(p, pst_ExtraEvent->p_Data, *(short *) pst_ExtraEvent->p_Data);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ANI_b_PlayedTrackList(EVE_tdst_ListTracks *_pst_TrackList, EVE_tdst_Data **_apst_Data, int *_pi_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_CurrentElemW;
	TAB_tdst_PFelem			*pst_EndElemW;
	WOR_tdst_World			*pst_World;
	TAB_tdst_PFtable		*pst_AnimEOT;
	TAB_tdst_PFelem			*pst_CurrentElem;
	TAB_tdst_PFelem			*pst_EndElem;
	OBJ_tdst_GameObject		*pst_GO;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	BOOL					b_Played;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Played = FALSE;
	if(_pi_Num) *_pi_Num = 0;

	/* If No World, return; */
	if(WOR_gst_Universe.st_WorldsTable.ul_NbElems == 0) return FALSE;

	pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
	{
		pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
		if(TAB_b_IsAHole(pst_World)) continue;

		/* Loop thru the EOT Anim Table. */
		pst_AnimEOT = &pst_World->st_EOT.st_Anims;

		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AnimEOT);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AnimEOT);

		if(!pst_CurrentElem) return FALSE;

		for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO) || !pst_GO->pst_Base || !pst_GO->pst_Base->pst_GameObjectAnim) continue;
			pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;

			for(i = 0; i < 4; i++)
			{
				if
				(
					pst_GOAnim->apst_Anim[i]
				&&	((ULONG) pst_GOAnim->apst_Anim[i] != (ULONG) - 1)
				&&	(pst_GOAnim->apst_Anim[i]->pst_Data->pst_ListTracks == _pst_TrackList)
				)
				{
					b_Played = TRUE;
					if(_apst_Data && _pi_Num)
						_apst_Data[(*_pi_Num)++] = pst_GOAnim->apst_Anim[i]->pst_Data;
					else
					{
						if(_pi_Num) (*_pi_Num)++;
					}
				}
			}
		}
	}

	return b_Played;
}

/*
 =======================================================================================================================
    Convert an animation .nim file (generated by Sioux) to a list of tracks a nim file contains keys for every frame
    and bone
 =======================================================================================================================
 */
BIG_KEY ANI_ConvertNIMfileToTracks(BIG_INDEX _ul_DataIndex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_ListTracks *pst_ListTracks, *pst_OldListTracks;
	struct ANI_st_Data	*pst_Data;
	EVE_tdst_Data		*pst_EVEData, *pst_SaveData;
	struct ANI_st_Canal *pst_Canal;
	EVE_tdst_Track		*pst_Track;
	BIG_INDEX			ul_Fat;
	int					i, EndLoop;
	USHORT				uw_Gizmo;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenPath];
	BOOL				b_Merge, b_OldExists, b_OldLoaded;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = ANI_pst_NIMLoad(_ul_DataIndex);
	if(!pst_Data) return NULL;

	b_Merge = FALSE;
	b_OldLoaded = FALSE;
	pst_SaveData = NULL;

	/* Create temp data */
	pst_EVEData = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
	L_memset(pst_EVEData, 0, sizeof(EVE_tdst_Data));

	/* Associate a file */
	BIG_ComputeFullName(BIG_ParentFile(_ul_DataIndex), asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(_ul_DataIndex));
	if(L_strrchr(asz_Name, '.')) *L_strrchr(asz_Name, '.') = 0;
	L_strcat(asz_Name, EDI_Csz_ExtEventAllsTracks);

	ul_Fat = BIG_ul_SearchFileExt(asz_Path, asz_Name);

	/* Is the TrackList already loaded ? */
	pst_OldListTracks = (EVE_tdst_ListTracks *) LOA_ul_SearchAddress(BIG_PosFile(ul_Fat));
	if(pst_OldListTracks && (pst_OldListTracks != (EVE_tdst_ListTracks *) BIG_C_InvalidKey))	/* The TrackList is
																								 * already loaded ... */
	{
		b_OldLoaded = TRUE;
		b_OldExists = TRUE;

		if(ANI_b_PlayedTrackList(pst_OldListTracks, NULL, NULL))
		{
			ERR_X_Warning(0, "Anim cannot be imported when currently played.", asz_Name);
			MEM_Free(pst_EVEData);
			MEM_Free(pst_Data);
			return BIG_C_InvalidKey;
		}
		else
		{
			/* If there are more tracks in the Old List of Tracks, we set the b_Merge BOOL; */
			b_Merge = (pst_OldListTracks->uw_NumTracks != (pst_Data->uc_NumberOfCanal << 1));

			if(b_Merge)
			{
				pst_SaveData = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
				L_memset(pst_EVEData, 0, sizeof(EVE_tdst_Data));
				pst_SaveData->pst_ListTracks = (EVE_tdst_ListTracks *) MEM_p_Alloc(sizeof(EVE_tdst_ListTracks));
				L_memset(pst_SaveData->pst_ListTracks, 0, sizeof(EVE_tdst_ListTracks));
				ANI_MergeListTracks(pst_SaveData, pst_OldListTracks, 0);
				b_Merge = TRUE;
			}

			for(i = 0; i < pst_OldListTracks->uw_NumTracks; i++) EVE_FreeTrack(&pst_OldListTracks->pst_AllTracks[i]);
			MEM_Free(pst_OldListTracks->pst_AllTracks);

			pst_OldListTracks->uw_Flags = 0;
			pst_OldListTracks->uw_NumTracks = 0;

			pst_EVEData->pst_ListTracks = pst_OldListTracks;
		}
	}
	else													/* The TrackList is NOT already loaded */
	{
		/* Create list of tracks */
		pst_ListTracks = (EVE_tdst_ListTracks *) MEM_p_Alloc(sizeof(EVE_tdst_ListTracks));
		L_memset(pst_ListTracks, 0, sizeof(EVE_tdst_ListTracks));
		pst_EVEData->pst_ListTracks = pst_ListTracks;
		pst_EVEData->pst_ListTracks->ul_NbOfInstances++;	/* Useful for end desallocation */

		if(ul_Fat == BIG_C_InvalidIndex)
		{
			ul_Fat = BIG_ul_CreateFile(asz_Path, asz_Name);
			LOA_AddAddress(ul_Fat, pst_ListTracks);
			b_OldExists = FALSE;
		}
		else
		{
			b_OldExists = TRUE;
			LOA_MakeFileRef
			(
				BIG_FileKey(ul_Fat),
				(ULONG *) &pst_OldListTracks,
				EVE_ul_CallbackLoadListTracks,
				LOA_C_MustExists
			);
			LOA_Resolve();
			EVE_ResolveListTrackRef(pst_OldListTracks);

			pst_OldListTracks->ul_NbOfInstances = 1;

			/* If there are more tracks in the Old List of Tracks, we set the b_Merge BOOL; */
			b_Merge = (pst_OldListTracks->uw_NumTracks != (pst_Data->uc_NumberOfCanal << 1));
		}
	}

	if(1)
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[100];
		/*~~~~~~~~~~~~~~~~~*/

		if(b_Merge)
			sprintf(asz_Log, "Track List file merged: %s", asz_Name);
		else
		{
			if(b_OldExists)
				sprintf(asz_Log, "Track List file re-created: %s", asz_Name);
			else
				sprintf(asz_Log, "Track List file created: %s", asz_Name);
		}

		LINK_PrintStatusMsg(asz_Log);
	}

	/* Version 2 of .nim (Extra objects tagged from Sioux) */
	if(pst_Data->uc_Flags == 2)
		EndLoop = pst_Data->uw_NumStandardBones + pst_Data->uw_MaxAddBonesIndex;
	else
		EndLoop = pst_Data->uc_NumberOfCanal;

	LOA_DeleteAddress(pst_EVEData->pst_ListTracks);

	/* For now: 2 tracks for each canal = rotation followed by translation (separate) */
	for(i = 0; i < EndLoop * 2; i++)
	{
		uw_Gizmo = i / 2;
		if(!uw_Gizmo)
		{
			pst_Track = EVE_pst_NewTrack(pst_EVEData);
			pst_Track->uw_Gizmo = (unsigned short) - 1;
		}
		else
		{
			if(ANI_b_CanalUsed(pst_Data, uw_Gizmo))
			{
				pst_Track = EVE_pst_NewTrack(pst_EVEData);
				pst_Track->uw_Gizmo = uw_Gizmo - 1;
			}
		}
	}

	LOA_AddAddress(ul_Fat, pst_EVEData->pst_ListTracks);

	/* Check that this Anim has no more than 64 canals (BitField width) */
	ERR_X_Assert(pst_Data->uc_NumberOfCanal <= 64);

	pst_Canal = ANI_FillCanals(pst_Data, asz_Name);
	ANI_ConvertAnimDataToListTracks
	(
		pst_EVEData->pst_ListTracks,
		pst_Canal,
		pst_Data->uc_NumberOfCanal,
		pst_Data->uw_NumberOfFrame
	);
	MEM_Free(pst_Canal);

	if(b_Merge)
	{
		LOA_DeleteAddress(pst_EVEData->pst_ListTracks);
		if(b_OldLoaded)
			ANI_MergeListTracks(pst_EVEData, pst_SaveData->pst_ListTracks, 1);
		else
			ANI_MergeListTracks(pst_EVEData, pst_OldListTracks, 1);
		LOA_AddAddress(ul_Fat, pst_EVEData->pst_ListTracks);
	}

	/* Sets the Flash Information on the First Track, first Event. */
	pst_EVEData->pst_ListTracks->pst_AllTracks->pst_AllEvents->w_Flags |= EVE_C_EventFlag_Flash;

	/* Save Track list */
	EVE_SaveListTracks(NULL, pst_EVEData->pst_ListTracks);

	MEM_Free(pst_EVEData->pst_ListParam);

	/* Old List Track desallocation if not already loaded */
	if(!b_OldLoaded)
	{
		EVE_DeleteListTracks(pst_EVEData->pst_ListTracks);
		if(b_OldExists) EVE_DeleteListTracks(pst_OldListTracks);
	}

	/* Free Temp Events Data. */
	MEM_Free(pst_EVEData);
	MEM_Free(pst_Data);

	if(pst_SaveData)
	{
		if(pst_SaveData->pst_ListParam) MEM_Free(pst_SaveData->pst_ListParam);
		if(pst_SaveData->pst_ListTracks)
		{
			for(i = 0; i < pst_SaveData->pst_ListTracks->uw_NumTracks; i++)
				EVE_FreeTrack(&pst_SaveData->pst_ListTracks->pst_AllTracks[i]);
			MEM_Free(pst_SaveData->pst_ListTracks->pst_AllTracks);
			MEM_Free(pst_SaveData->pst_ListTracks);
		}

		MEM_Free(pst_SaveData);
	}

	BIG_DeleteFile(BIG_ParentFile(_ul_DataIndex), _ul_DataIndex);
	return(BIG_FileKey(ul_Fat));
}

#endif /* ACTIVE_EDITORS */

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
