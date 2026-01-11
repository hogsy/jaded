/*$T VERsion_Check.h GC!1.52 11/03/99 11:49:47 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
extern bool VERsion_CheckCurrent(void);
#else
#define VERsion_CheckCurrent()
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif 
 