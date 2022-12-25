/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS

/* Define constantsfor action. */
#define EPENC_ACTION_REFRESH				1
#define EPENC_ACTION_UNDOCHECKOUT_SELECTED	2
#define EPENC_ACTION_CHECKIN_SELECTED		3
#define EPENC_ACTION_CHECKINOUT_SELECTED	4
#define EPENC_ACTION_UNDOCHECKOUT_ALL	    5
#define EPENC_ACTION_CHECKIN_ALL		    6
#define EPENC_ACTION_SHOWINBROWSER         7
#define EPENC_ACTION_DATACONTROL           8

/* Base buffer. */
#ifdef ACTION_GLOBAL

#define EXTERN
#else

#define EXTERN  extern
#endif

EXTERN char * EPENC_asz_ActionBase

#ifdef ACTION_GLOBAL
=
    "\
General=-1=--=0;\
Refresh=1=F4=0;\
Data Control=-2=--=0;\
On / Off=8=--=0;\
Show in browser=7=--=0;\
=0=--=0;\
Undo CheckOut=2=--=0;\
CheckIn=3=--=0;\
CheckInOut=4=--=0;\
=0=--=0;\
Undo CheckOut All=5=--=0;\
CheckIn All=6=--=0;\
;\
"
#endif
;

#undef EXTERN
#undef ACTION_GLOBAL
#endif

