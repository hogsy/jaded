/*$T PROframe_act.h GC!1.34 05/10/99 14:44:56 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS

/* Define constants. */
#define EPRO_ACTION_REFRESH		1
#define EPRO_ACTION_FIND		2
#define EPRO_ACTION_CLEAR		10
#define EPRO_ACTION_UNIVERSE	20

/* Base buffer. */
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#define EXTERN  extern
#endif
EXTERN char *EPRO_asz_ActionBase
#ifdef ACTION_GLOBAL
= "\
General=-1=--=0;\
Refresh=1=F4=198;\
Find=2=F3=0;\
Sep=0=--=0;\
Clear All=10=^F4=0;\
Sep=0=--=0;\
Load Universe=20=--=0;\
;\
"
#endif
;
#undef EXTERN
#undef ACTION_GLOBAL
#endif
