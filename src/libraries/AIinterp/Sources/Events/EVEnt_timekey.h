/*$T EVEnt_timekey.h GC! 1.081 04/04/00 10:17:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EVENT_TimeKey_H__
#define __EVENT_TimeKey_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    constants
 ***********************************************************************************************************************
 */

#define EVE_KeyType_Linear					0
#define EVE_KeyType_Square					1
#define EVE_KeyType_Speed					2
#define EVE_KeyType_SpeedPos				3
#define EVE_KeyType_Acceleration			4
#define EVE_KeyType_AccelerationSpeed		5
#define EVE_KeyType_AccelerationPos			6
#define EVE_KeyType_AccelerationSpeedPos	7

#define EVE_KeyType_TimeFactorStart         8
#define EVE_KeyType_TimeFactor              9

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
extern void				EVE_Event_TimeKey_Save(EVE_tdst_Event *);
extern void             EVE_Event_TimeKey_ComputeSP(struct EVE_tdst_Track_ *_pst_Track);

#endif /* ACTIVE_EDITORS */

extern short			EVE_w_Event_TimeKey_GetType(EVE_tdst_Event *);
extern void				EVE_Event_TimeKey_AllocData(EVE_tdst_Event *, short);
extern int				EVE_Event_TimeKey_Load(EVE_tdst_Event *, char *, char *);
extern void				EVE_Event_TimeKey_Resolve(EVE_tdst_Event *);
extern EVE_tdst_Event	*EVE_Event_TimeKey_Play(EVE_tdst_Event *);

#ifdef __cplusplus
}
#endif
#endif /* __EVENT_TimeKey_H__ */
