/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#ifdef ACTIVE_EDITORS

/* Define constantsfor action. */
#define ESELECTION_ACTION_INSERT					1
#define ESELECTION_ACTION_SHOW_STANDARD				3
#define ESELECTION_ACTION_SHOW_SHADING				4
#define ESELECTION_ACTION_SHOW_NORMALMAP			5
#define ESELECTION_ACTION_SHOW_DETAILNORMALMAP		6
#define ESELECTION_ACTION_SHOW_SPECULARMAP			7
#define ESELECTION_ACTION_SHOW_ENVIRONMENTMAP		8
#define ESELECTION_ACTION_SHOW_MOSSMAP				9
#define ESELECTION_ACTION_SHOW_RIMLIGHT				10
#define ESELECTION_ACTION_SHOW_MESHPROCESSING		11

#define ESELECTION_ACTION_SAVE						20

#define ESELECTION_ACTION_PERFORCE_EDIT				50
#define ESELECTION_ACTION_PERFORCE_DIFF				54
#define ESELECTION_ACTION_PERFORCE_HISTORY			55
#define ESELECTION_ACTION_PERFORCE_REVERT			59

#define ESELECTION_ACTION_SHOW_INMATERIAL			60
#define ESELECTION_ACTION_SHOW_SELECTED				61
#define ESELECTION_ACTION_SHOW_ALL					62

/* Base buffer. */
#ifdef ACTION_GLOBAL

#define EXTERN
#else

#define EXTERN  extern
#endif

EXTERN char * ESELection_asz_ActionBase

#ifdef ACTION_GLOBAL
=
"\
General=-1=--=0;\
Save=20=^S=0;\
Sep=0=--=0;\
Insert Objects Selected in 3DView=1=INS=0;\
Columns=-1=--=0;\
Show Standard Param. (in light blue)=3==0;\
Sep=0=--=0;\
Show Shading Param. (in yellow)=4==0;\
Show NormalMap Param. (in orange)=5==0;\
Show Detail NormalMap Param. (in green)=6==0;\
Show SpecularMap Param. (in light orange)=7==0;\
Show EnvironmentMap Param. (in light yellow)=8==0;\
Show MossMap Param. (in light green)=9==0;\
Show Rim Light Param. (in light red)=10==0;\
Show Mesh Processing Param. (in purple)=11==0;\
Actions=-1=--=0;\
Show in Material Editor=60=--=0;\
Sep=0=--=0;\
Show Only Selected Rows (in Grid)=61=--=0;\
Show All Rows (in Grid)=62=--=0;\
Data Control=-1=--=0;\
Perforce - Check Out=50=--=0;\
Perforce - Undo Check Out=59=--=0;\
Sep=0=--=0;\
Perforce - Show Difference=54=--=0;\
Perforce - Show History=55=--=0;\
;\
"
#endif
;

#undef EXTERN
#undef ACTION_GLOBAL
#endif

