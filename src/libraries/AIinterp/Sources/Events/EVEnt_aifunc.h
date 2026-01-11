/*$T EVEnt_aifunc.h GC! 1.078 03/17/00 10:07:52 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EVENT_AIFUNC_H__
#define __EVENT_AIFUNC_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
extern void				EVE_Event_AIFunction_Save(EVE_tdst_Event *);
#endif
extern int				EVE_Event_AIFunction_Load(EVE_tdst_Event *, char *, char *);
extern void				EVE_Event_AIFunction_Resolve(EVE_tdst_Event *);
extern EVE_tdst_Event	*EVE_Event_AIFunction_Play(EVE_tdst_Event *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EVENT_AIFUNC_H__ */
