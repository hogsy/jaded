/*$T EVEframe_act.h GC! 1.081 06/26/00 18:18:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS

/* Define constants. */
#define EEVE_ACTION_NEWTRACK			1
#define EEVE_ACTION_RUNALL				2
#define EEVE_ACTION_NORUNALL			3
#define EEVE_ACTION_CLOSEALL			4
#define EEVE_ACTION_SAVEALL				5
#define EEVE_ACTION_RUNALLI				6
#define EEVE_ACTION_NORUNALLI			7
#define EEVE_ACTION_UNDO				8
#define EEVE_ACTION_LOCK				9

#define EEVE_ACTION_UNIT100MS			10
#define EEVE_ACTION_UNIT1S				11
#define EEVE_ACTION_UNIT5S				12
#define EEVE_ACTION_UNIT6S				13
#define EEVE_ACTION_UNIT10S				14
#define EEVE_ACTION_UNIT15S				15
#define EEVE_ACTION_UNIT20S				16
#define EEVE_ACTION_UNIT30S				17
#define EEVE_ACTION_UNIT60S				18

#define EEVE_ACTION_INTER5				20
#define EEVE_ACTION_INTER6				21
#define EEVE_ACTION_INTER10				22
#define EEVE_ACTION_INTER50				23
#define EEVE_ACTION_INTER60				24

#define EEVE_ACTION_UNIT2S				25
#define EEVE_ACTION_UNIT3S				26

#define EEVE_ACTION_DISPLSEC			30
#define EEVE_ACTION_DISPLCNT			31
#define EEVE_ACTION_DISPLHIDDEN			32
#define EEVE_ACTION_ALLEVENTS			33
#define EEVE_ACTION_DISPNAMES			34
#define EEVE_ACTION_FORCE0				35
#define EEVE_ACTION_FORCE1				36
#define EEVE_ACTION_FORCE01				37
#define EEVE_ACTION_SPLITSEL			38

#define EEVE_ACTION_FORCESNAP			40

#define EEVE_ACTION_COPY				50

#define EEVE_ACTION_RUNALLIS			51
#define EEVE_ACTION_NORUNALLIS			52

#define EEVE_ACTION_GENTRE				60

#define EEVE_ACTION_SIMPLECHECK			61

#define EEVE_ACTION_GOTOORIGIN			100
#define EEVE_ACTION_DELSELECTED			101
#define EEVE_ACTION_GOTOEND				102
#define EEVE_ACTION_DELSELECTED2		111

#define EEVE_ACTION_BIGZOOMIN			122
#define EEVE_ACTION_BIGZOOMOUT			123
#define EEVE_ACTION_CENTERREAL			124
#define EEVE_ACTION_BIGZOOMINP			125
#define EEVE_ACTION_BIGZOOMOUTP			126

#define EEVE_ACTION_ALIGN				152

#define EEVE_ACTION_SETREALUNIT			153
#define EEVE_ACTION_REALUNIT1			154
#define EEVE_ACTION_REALUNITI			155
#define EEVE_ACTION_REALUNITU			156

#define EEVE_ACTION_SETSNAPUNIT			160
#define EEVE_ACTION_SNAPUNIT1			161
#define EEVE_ACTION_SNAPUNITI			162
#define EEVE_ACTION_SNAPUNITU			163
#define EEVE_ACTION_SNAPUNITT			164

#define EEVE_ACTION_MOVETRACKUP			170
#define EEVE_ACTION_MOVETRACKDOWN		171

#define EEVE_ACTION_INSERTTIME			180
#define EEVE_ACTION_REMOVETIME			181

#define EEVE_ACTION_OPTANIM				200
#define EEVE_ACTION_ROUND				201

#define EEVE_ACTION_AUTOALIGN			210

/* PLAY */
#define EEVE_ACTION_PLAY				211
#define EEVE_ACTION_PLAY2CYCLES			212
#define EEVE_ACTION_PLAY60				213
#define EEVE_ACTION_PLAY30				214
#define EEVE_ACTION_PLAY25				215
#define EEVE_ACTION_PLAY15				216
#define EEVE_ACTION_PLAYCONTRACT		217

#define EEVE_ACTION_LINKACTION			218
#define EEVE_ACTION_RUNSEL				219

/* ANIM */
#define EEVE_ACTION_ADDFRAMEALL			300
#define EEVE_ACTION_ADDXFRAMEALL		301
#define EEVE_ACTION_DUPLICATEFRAMEALL	302
#define EEVE_ACTION_DELKEYFRAME			310
#define EEVE_ACTION_ADDKEY				311
#define EEVE_ACTION_DELKEY				312
#define EEVE_ACTION_SHOWALLROTCURVE		320
#define EEVE_ACTION_DELALLCURVE			321
#define EEVE_ACTION_EXPAND2				322
#define EEVE_ACTION_EXPAND3				323
#define EEVE_ACTION_ANIMMODE			324

/* Base buffer. */
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#define EXTERN	extern
#endif
EXTERN char *EEVE_asz_ActionBase
#ifdef ACTION_GLOBAL
=
	"\
General=-1=--=0;\
Close All=4=^F4=0;\
Save All=5=^S=0;\
Simple Check=61=--=0;\
Sep=0=--=0;\
Generate Tre=60=--=0;\
Sep=0=--=0;\
New Track=1=^N=0;\
Sep=0=--=0;\
Undo=8=^Z=0;\
Sep=0=--=0;\
Lock Selection=9=--=0;\
Display=-1=--=0;\
100 ms=10=--=0;\
1 s=11=--=0;\
2 s=25=--=0;\
3 s=26=--=0;\
5 s=12=--=0;\
6 s=13=--=0;\
10 s=14=--=0;\
15 s=15=--=0;\
20 s=16=--=0;\
30 s=17=--=0;\
60 s=18=--=0;\
Inters=0=--=0;\
5=20=--=0;\
6=21=--=0;\
10=22=--=0;\
50=23=--=0;\
60=24=--=0;\
Sep=0=--=0;\
Zoom (keep left)=122=NUMPAD+=0;\
UnZoom (keep left)=123=NUMPAD-=0;\
Zoom (keep right)=125=#NUMPAD+=0;\
UnZoom (keep right)=126=#NUMPAD-=0;\
Center To Real Time=124=^SPACE=0;\
Sep=0=--=0;\
Time In Seconds=30=--=0;\
Time In Frames=31=--=0;\
Sep=0=--=0;\
Hidden Tracks=32=I=0;\
User Tracks Names=34=U=0;\
Fix Width=33=A=0;\
Edit=-1=--=0;\
Move Track Up=170=^UP=0;\
Move Track Down=171=^DOWN=0;\
Sep=0=--=0;\
Copy=50=^C=0;\
Delete Events (Shift Right)=111=DEL=0;\
Delete Events (Keep Right)=101^=^DEL=0;\
Force 0 Delay (Shift Right)=35=^NUMPAD0=0;\
Force 0 Delay (Keep Right)=37=^NUMPAD1=0;\
Force To 1 Inter=36=^I=0;\
Split Selected=38=NUMPAD1=0;\
Sep=0=--=0;\
Force Snap=40=--=0;\
Set Snap Time=160=--=0;\
Snap Time User=163=--=0;\
Snap Time To 1/60=161=--=0;\
Snap Time To Inter=162=--=0;\
Snap To Tracks=164=--=0;\
Sep=0=--=0;\
Insert Time=180=--=0;"

"Remove Time=181=--=0;\
Runtime=-1=--=0;\
Set Unit Time=153=--=0;\
Unit Time User=156=--=0;\
Unit Time To 1/60=154=--=0;\
Unit Time To Inter=155=--=0;\
Sep=0=--=0;\
Set Running On All Tracks=2=--=0;\
Set Running Off All Tracks=3=--=0;\
Set Running Init On All Tracks=6=--=0;\
Set Running Init Off All Tracks=7=--=0;\
Sep=0=--=0;\
Set Running Init On Sel Tracks=51=--=0;\
Set Running Init Off Sel Tracks=52=--=0;\
Sep=0=--=0;\
Align=152=BACK=0;\
Auto Align=210=--=0;\
Run Selected Tracks=219=--=0;\
Link To Action=218=--=0;\
Compute=-1=--=0;\
Optimize (Anim)=200=--=0;\
Round To Snap Unit=201=--=0;\
Play=-1=--=0;\
Play/Stop=211=P=--;\
Play 2 Cycles=212=--=0;\
Play Contract=217=--=0;\
Sep=0=--=0;\
Play 60 Hz=213=^#NUMPAD6=0;\
Play 30 Hz=214=^#NUMPAD3=0;\
Play 25 Hz=215=^#NUMPAD2=0;\
Play 15 Hz=216=^#NUMPAD1=0;\
Animation=-1=--=0;\
Animation Mode=324=--=0;\
Sep=0=--=0;\
Add Frame=300=INS=0;\
Add Several Frames=301=^INS=0;\
Duplicate Frame=302=^#INS=0;\
Remove Keys And Frames=310=#DEL=0;\
Add Keys=311=--=0;\
Remove Keys=312=--=0;\
Sep=0=--=0;\
Show All Trajectories (Rot)=320=^J=0;\
Hide All Trajectories =321=^#J=0;\
Sep=0=--=0;\
Goto Origin=100=HOME=0;\
Goto End=102=END=0;\
Sep=0=--=0;\
Expand Anim 2x=322=--=0;\
Expand Anim 3x=323=--=0;\
;\
"
#endif
;
#undef EXTERN
#undef ACTION_GLOBAL
#endif
