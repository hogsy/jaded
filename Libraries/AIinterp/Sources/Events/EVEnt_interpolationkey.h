/*$T EVEnt_interpolationkey.h GC! 1.100 09/04/01 10:34:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EVENT_INTERPOLATIONKEY_H__
#define __EVENT_INTERPOLATIONKEY_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#ifndef _GAMECUBE
extern ULONG		Event_SizeOf_Ram;
#endif

/*$4
 ***********************************************************************************************************************
    constants
 ***********************************************************************************************************************
 */

#define EVE_InterKeyType_Translation_0				0x0001
#define EVE_InterKeyType_Translation_1				0x0002
#define EVE_InterKeyType_Translation_2				0x0003
#define EVE_InterKeyType_Translation_Mask			0x0003	/* Mask */

#define EVE_InterKeyType_Rotation_Matrix			0x0004
#define EVE_InterKeyType_Rotation_Quaternion		0x0010
#define EVE_InterKeyType_Rotation_Mask				0x0014	/* Mask */

#define EVE_InterKeyType_Time						0x0008

#define EVE_InterKeyType_BlockedForIK				0x0020
#define EVE_InterKeyType_HasNextValue				0x0040

#define EVE_InterKey_CompressedQuaternion			0x0080		/* Quaternion with 3 short */
#define EVE_InterKey_CompressedQuaternionMask		0x0080		/* Idem */
//#define EVE_InterKey_UltraCompressedQuaternionX		0x0100	/* Quaternion with 1 short & 2 char */
//#define EVE_InterKey_UltraCompressedQuaternionY		0x0200	/* Idem */
//#define EVE_InterKey_UltraCompressedQuaternionZ		0x0400	/* Idem */


//#define EVE_InterKey_CompressedAbsoluteTranslation	0x0800	/* Thanslation with 3 short */
//#define EVE_InterKey_CompressedRelativeTranslation	0x1000	/* Thanslation with 3 short */
//#define EVE_InterKey_UltraCompressedTranslationX		0x2000	/* Translation 1 short & 2 char */
//#define EVE_InterKey_UltraCompressedTranslationY		0x4000	/* Idem */
//#define EVE_InterKey_UltraCompressedTranslationZ		0x8000	/* Idem */
//#define EVE_InterKey_CompressedTranslationMask		0xF800	/* Idem */

/*$4
 ***********************************************************************************************************************
    globalsd
 ***********************************************************************************************************************
 */

#define EVE_NumFrames(a)			((a)->uw_NumFrames & 0x7FFF)



#ifdef ACTIVE_EDITORS
extern MATH_tdst_Matrix				EVE_ast_Gizmo[64];
extern MATH_tdst_Matrix				EVE_st_MagicBox;
#endif /* ACTIVE_EDITORS */

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
extern void							EVE_Event_InterpolationKey_Save(EVE_tdst_Event *, EVE_tdst_Event *, USHORT);

extern void							EVE_Event_InterpolationKey_SpecialPlay
									(
										struct OBJ_tdst_GameObject_ *,
										EVE_tdst_Data *,
										float,
										int,
										int,
										MATH_tdst_Matrix *,
										MATH_tdst_Matrix *
									);

extern void							EVE_Event_InterpolationKey_UpdateGizmos
									(
										OBJ_tdst_GameObject *,
										OBJ_tdst_Group *,
										MATH_tdst_Matrix *,
										MATH_tdst_Matrix *
									);

extern void							EVE_Event_InterpolationKey_GetAbsoluteMatrix
									(
										EVE_tdst_Data *,
										EVE_tdst_Track *,
										struct OBJ_tdst_GameObject_ *,
										BOOL,
										unsigned short,
										struct MATH_tdst_Matrix_ *
									);

extern void							EVE_Event_InterpolationKey_GetPathInitMatrix
									(
										EVE_tdst_Track *,
										struct OBJ_tdst_GameObject_ *,
										struct MATH_tdst_Matrix_ *
									);

extern void							EVE_Event_InterpolationKey_Compute
									(
										OBJ_tdst_GameObject *,
										OBJ_tdst_GameObject *,
										EVE_tdst_ListTracks *,
										EVE_tdst_Track *,
										EVE_tdst_Event *,
										int,
                                        int
									);
extern void							EVE_Event_InterpolationKey_WhenInsert
									(
										struct OBJ_tdst_GameObject_ *,
										struct EVE_tdst_ListTracks_ *,
										struct EVE_tdst_Track_ *,
										EVE_tdst_Event *
									);

extern void							EVE_Event_InterpolationKey_ZoomTimeInterpolation(EVE_tdst_Event *, float);

extern void							EVE_Event_InterpolationKey_CopyMatrix(MATH_tdst_Matrix *, EVE_tdst_Event *, BOOL);
extern void							EVE_Event_InterpolationKey_SetMatrix(MATH_tdst_Matrix *, EVE_tdst_Event *);

extern void							EVE_Event_InterpolationKey_ChangeTypeWithoutChangingData(EVE_tdst_Event *, short);

extern void							EVE_Event_InterpolationKey_UpdateNextValue
									(
										OBJ_tdst_GameObject *,
										EVE_tdst_ListTracks *,
										EVE_tdst_Track *
									);

#endif /* ACTIVE_EDITORS */

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern short						EVE_w_Event_InterpolationKey_GetType(EVE_tdst_Event *);
extern short						EVE_w_Event_InterpolationKey_GetFlags(EVE_tdst_Event *);
extern struct MATH_tdst_Vector_		*EVE_pst_Event_InterpolationKey_GetPos(EVE_tdst_Event *);
extern struct MATH_tdst_Matrix_		*EVE_pst_Event_InterpolationKey_GetRotation(EVE_tdst_Event *);
extern void							EVE_Event_InterpolationKey_GetQuaternion(EVE_tdst_Event *, struct MATH_tdst_Quaternion_ *);
extern void							EVE_Event_InterpolationKey_GetCompressedQuaternion(EVE_tdst_Event *, struct MATH_tdst_CompressedQuaternion_ *);
extern float						*EVE_pf_Event_InterpolationKey_GetTime(EVE_tdst_Event *);

extern void							EVE_Event_InterpolationKey_AllocData(EVE_tdst_Event *, short, int);

extern int							EVE_Event_InterpolationKey_Load
									(
										EVE_tdst_Event *,
										EVE_tdst_Event *,
										char *,
										char *,
										EVE_tdst_Track *,
										USHORT
									);
extern void							EVE_Event_InterpolationKey_Resolve(EVE_tdst_Event *);
extern EVE_tdst_Event				*EVE_Event_InterpolationKey_Play(EVE_tdst_Event *);
extern USHORT						EVE_w_Event_ComputeCompressType(EVE_tdst_Event *, EVE_tdst_Event *);

extern float						EVE_f_Event_InterpolationKey_InterpolateTime(EVE_tdst_Event *, float);

extern void							EVE_Event_InterpolationKey_BlendTranslation
									(
										EVE_tdst_Event *,
										EVE_tdst_Event *,
										float,
										struct MATH_tdst_Vector_ *
									);
extern void							EVE_Event_InterpolationKey_GetTranslation
									(
										EVE_tdst_Track *,
										float,
										MATH_tdst_Vector *
									);
extern void							EVE_Event_InterpolationKey_GetRotationPos
									(
										EVE_tdst_Track *,
										EVE_tdst_Track *,
										int,
										MATH_tdst_Vector *
									);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EVENT_INTERPOLATIONKEY_H__ */
