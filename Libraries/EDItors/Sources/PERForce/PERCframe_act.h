/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS

/* Define constantsfor action. */
#define EPERC_ACTION_REFRESH													1
#define EPERC_ACTION_REVERT														2
#define EPERC_ACTION_SUBMIT														3
#define EPERC_ACTION_DELETEEMPTY												4
#define EPERC_ACTION_EDITCHANGELIST												5
#define EPERC_ACTION_SETWORKINGCHANGELIST										6
#define EPERC_ACTION_ADDCHANGELIST												7
#define EPERC_ACTION_SHOWHISTORY												8
#define EPERC_ACTION_REVERT_UNCHANGED											9
#define EPERC_ACTION_RESOLVE													10
#define EPERC_ACTION_SYNC_BF_TO_SELECTEDCHANGELIST								11
#define EPERC_ACTION_SYNC_FILES_IN_SELECTEDCHANGELIST							12
#define EPERC_ACTION_SYNC_FILES_IN_SELECTEDCHANGELIST_TO_PREVIOUSREVISION		13
#define EPERC_ACTION_OPEN_FILES_IN_SELECTEDCHANGELIST							14
#define EPERC_ACTION_SHOWDIFF													16
#define EPERC_ACTION_CLEARLOG													17
#define EPER_ACTION_REFRESH_CLIENTVIEW											18
#define EPERC_ACTION_SUBMIT_REOPEN												19
#define EPERC_ACTION_SYNC														20
#define EPERC_ACTION_INTEG														21


/* Base buffer. */
#ifdef ACTION_GLOBAL

#define EXTERN
#else
#undef EXTERN
#define EXTERN  extern
#endif

EXTERN char * EPERC_asz_ActionBase

#ifdef ACTION_GLOBAL
=
"\
General=-1=--=0;\
Refresh server info=18=--=0;\
Revert=2=--=0;\
Revert unchanged files=9=--=0;\
Sep=0=--=0;\
Clear log window=17=--=0;\
Sep=0=--=0;\
Refresh=1=F4=0;\
Changelist=-2=--=0;\
Submit changelist=3=--=0;\
Submit changelist (keep files)=19=--=0;\
Sep=0=--=0;\
Set as default changelist=6=--=0;\
Add new changelist=7=--=0;\
Edit changelist=5=--=0;\
Delete empty changelist=4=--=0;\
Sep=0=--=0;\
Sync BF to changelist=11=--=0;\
Sync files in changelist=12=--=0;\
Sync files in changelist to previous revision=13=--=0;\
Checkout files in selected changelist=14=--=0;\
Sep=0=--=0;\
Files=-2=--=0;\
Integrate=21=--=0;\
Sync=20=--=0;\
Resolve=10=--=0;\
Show history=8=--=0;\
Show difference=16=--=0;\
;\
"
#endif
;

#undef EXTERN
#undef ACTION_GLOBAL
#endif

