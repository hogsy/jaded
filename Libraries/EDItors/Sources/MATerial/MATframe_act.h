/*$T MATframe_act.h GC!1.5 10/05/99 15:14:11 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS

/* Define constants. */
#define EMAT_ACTION_SAVEMATERIAL        100
#define EMAT_ACTION_UNDO                101
#define EMAT_ACTION_REDO                102
#define EMAT_ACTION_COPY_ALL		    105
#define EMAT_ACTION_PASTE_ALL		    106
#define EMAT_ACTION_SHMLTSUBMAT		    104
#define EMAT_ACTION_SHCLPBRDMAT			103
#define EMAT_ACTION_CHECKMULTIMAT       107
#define EMAT_ACTION_CHECKTEXTURE        108
#define EMAT_ACTION_CHECKMATOWNER       109
#define EMAT_ACTION_CHECKMULTIMATOWNER  110
#define EMAT_ACTION_CHECKSOUNDID		111
#define EMAT_ACTION_CLOSE				120

#define EMAT_ACTION_NEWMULTI			200
#define EMAT_ACTION_NEWSINGLE			201

#define EMAT_ACTION_GENERATEHTML		300

#define EMAT_ACTION_OPTIONS			-3
/* Base buffer. */
#ifdef ACTION_GLOBAL
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN  extern
#endif
EXTERN char *EMAT_asz_ActionBase
#ifdef ACTION_GLOBAL
=
    "\
General=-2=--=0;\
Show Left Pane=104=^M=0;\
Show Clipboard=103=--=0;\
Sep=0=--=0;\
Check Multi Material=107==0;\
Check Texture Ref=108==0;\
Check Material Owner=109==0;\
Check Multi-Material owner=110==0;\
Check Sound ID=111==0;\
Sep=0=--=0;\
Close=120=^F4=0;\
File=-5=--=0;\
Save=100=^S=0;\
Sep=0=--=0;\
Create Multi=200=--=0;\
Create Single=201=--=0;\
Edition=-4=--=0;\
Undo=101=^Z=0;\
Redo=102=^A=0;\
Copy All=105=^C=0;\
Paste All=106=^V=0;\
Sep=0=--=0;\
Generate html=300==0;\
Options=-3=--=0;\
;\
"
#endif
;
#undef EXTERN
#undef ACTION_GLOBAL
#endif
