/*$T LOGframe_act.h GC!1.26 04/21/99 14:22:43 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS

/* Define constants. */
#define ELOG_ACTION_CLEARLOG    1
#define ELOG_ACTION_FREEZE      2
#define ELOG_ACTION_WRITETOFILE	3
#define ELOG_ACTION_FILTER      4
#define ELOG_ACTION_LOGLOAD     5
#define ELOG_ACTION_MEMLOAD		10
#define ELOG_ACTION_RASTERS		11

#define ELOG_ACTION_CANAL0      20 // Default
#define ELOG_ACTION_CANAL1      21 // Error
#define ELOG_ACTION_CANAL2      22 // AI 1
#define ELOG_ACTION_CANAL3      23 // AI 2
#define ELOG_ACTION_CANAL4      24 // AI 3
#define ELOG_ACTION_CANAL5      25 // AI 4

/* Base buffer. */
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#define EXTERN  extern
#endif
EXTERN char *ELOG_asz_ActionBase
#ifdef ACTION_GLOBAL
= "\
General=-1=--=0;\
Freeze=2=F=0;\
Filter=4==0;\
Write To \"Jade.log\"=3=--=0;\
Log loading=5==0;\
Sep=0=--=0;\
Clear Log=1=^F4=0;\
Sep=0=--=0;\
Log Mem Load=10=--=0;\
Log Rasters=11=--=0;\
\
Canal=-2=--=0;\
Canal 0 (default)=20==0;\
Canal 1 (error)=21==0;\
Canal 2 (AI 1)=22==0;\
Canal 3 (AI 2)=23==0;\
Canal 4 (AI 3)=24==0;\
Canal 5 (AI 4)=25==0;\
;\
"
#endif
;
#undef EXTERN
#undef ACTION_GLOBAL
#endif
