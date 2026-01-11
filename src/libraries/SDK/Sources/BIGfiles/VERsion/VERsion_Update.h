/*$T VERsion_Check.h GC!1.52 11/03/99 11:10:26 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
extern void VERsion_UpdateOneFile(ULONG, char *, ULONG, char *, char *);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

extern void VERsion_UpdateVersion36 ();
