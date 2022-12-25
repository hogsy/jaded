/*$T EACTframe_act.h GC! 1.098 11/09/00 12:15:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS

/* Define constants. */
#define EACT_ACTION_CLOSEALL		1
#define EACT_ACTION_SAVEKIT			2
#define EACT_ACTION_RENAMEACTION	10
#define EACT_ACTION_FIND			11

#define EACT_ACTION_RESETTRANSITION 51
#define EACT_ACTION_RESTOREINIT		52

#define EACT_ACTION_DUPLICATEKIT	60

#define EACT_ACTION_LOGKIT			100

/* Base buffer. */
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#undef EXTERN
#define EXTERN	extern
#endif
EXTERN char *EACT_asz_ActionBase
#ifdef ACTION_GLOBAL
=
	"\
General=-1=--=0;\
Close=1=^F4=0;\
Save Action Kit=2=^S=0;\
Sep=0=--=0;\
Log Kit=100=--=0;\
Sep=0=--=0;\
Duplicate Kit=60=--=0;\
Sep=0=--=0;\
Rename Action=10=F2=0;\
Find=11=F3=0;\
Special=-2=--=0;\
Reset Transition Blending Data=51=--=0;\
Restore Initial Kit=52=--=0;\
;"
#endif
;
#undef EXTERN
#undef ACTION_GLOBAL
#endif
