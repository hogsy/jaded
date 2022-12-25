/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#ifdef ACTIVE_EDITORS

/* Define constantsfor action. */
#define EPERCHECK_ACTION_REFRESH_BRANCH			1
#define EPERCHECK_ACTION_EXPAND_BRANCH			2
#define EPERCHECK_ACTION_REFRESH_NOTINP4		3
#define EPERCHECK_ACTION_EXPAND_NOTINP4			4
#define EPERCHECK_ACTION_REFRESH_NOTINBF		5
#define EPERCHECK_ACTION_EXPAND_NOTINBF			6
#define EPERCHECK_ACTION_REFRESH_DELETED		7
#define EPERCHECK_ACTION_EXPAND_DELETED			8
#define EPERCHECK_ACTION_PERFORCE_EDITTOP		50
#define EPERCHECK_ACTION_PERFORCE_SYNCTOP		52
#define EPERCHECK_ACTION_PERFORCE_FORCESYNCTOP	53
#define EPERCHECK_ACTION_PERFORCE_ADDTOP		56
#define EPERCHECK_ACTION_PERFORCE_DELETETOP		57
#define EPERCHECK_ACTION_PERFORCE_DIFFTOP		54
#define EPERCHECK_ACTION_PERFORCE_HISTORYTOP	55
#define EPERCHECK_ACTION_PERFORCE_SUBMITTOP		58
#define EPERCHECK_ACTION_PERFORCE_REVERTTOP		59
#define EPERCHECK_ACTION_PERFORCE_SYNCBOTTOM	60
#define EPERCHECK_ACTION_PERFORCE_ADDBOTTOM		61
#define EPERCHECK_ACTION_PERFORCE_DELETEBOTTOM	62
#define EPERCHECK_ACTION_PERFORCE_SUBMITEDITTOP	63

/* Base buffer. */
#ifdef ACTION_GLOBAL

#define EXTERN
#else

#define EXTERN  extern
#endif

EXTERN char * EPERCheck_asz_ActionBase

#ifdef ACTION_GLOBAL
=
"\
General=-1=--=0;\
Refresh (filters only)=1==0;\
Expand/Refresh=2==0;\
Sep=0=--=0;\
Refresh=3==0;\
Expand (All)=4==0;\
Sep=0=--=0;\
Refresh=5==0;\
Expand (All)=6==0;\
Sep=0=--=0;\
Refresh=7==0;\
Expand (All)=8==0;\
Sep=0=--=0;\
Perforce - Check In=58==0;\
Perforce - Check Out=50==0;\
Perforce - Check InOut=63==0;\
Perforce - Undo Check Out=59==0;\
Perforce - Get Latest Version=52==0;\
Perforce - Force Get Latest Version=53==0;\
Perforce - Add to server=56==0;\
Perforce - Delete from server=57==0;\
Perforce - Show difference=54==0;\
Perforce - Show history=55==0;\
Perforce - Get Latest Version=60==0;\
Perforce - Add to server=61==0;\
Perforce - Delete from server=62==0;\
;\
"
#endif
;

#undef EXTERN
#undef ACTION_GLOBAL
#endif

