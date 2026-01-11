/*$T RASframe_act.h GC!1.41 08/04/99 10:22:02 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS

/* Define constantsfor action. */
#define ERAS_ACTION_ADDWATCH			1


/* Base buffer. */
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#define EXTERN  extern
#endif
EXTERN char *ERAS_asz_ActionBase
#ifdef ACTION_GLOBAL
=
    "\
Watch=-1=--=0;\
Add watch on message=1==0;\
;\
"

#endif
;
#undef EXTERN
#undef ACTION_GLOBAL
#endif
