/*$T AIframe_act.h GC! 1.100 03/07/01 15:26:38 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS

/* Define constants. */
#define EAI_ACTION_SAVE					1

#define EAI_ACTION_COMPILE				10
#define EAI_ACTION_COMPILE2LOG			11

#define EAI_ACTION_ADDFUNCTION			14
#define EAI_ACTION_CREATEINSTANCE		17
#define EAI_ACTION_REINITINSTANCE		18

#define EAI_ACTION_COMPILEALL			19
#define EAI_ACTION_NEWMODEL				20
#define EAI_ACTION_NEWFUNCTION			21
#define EAI_ACTION_NEWVARS				22
#define EAI_ACTION_MODELCLOSE			23
#define EAI_ACTION_MODELCOMPILE			24
#define EAI_ACTION_MODELCOMPILEDEP		25

#define EAI_ACTION_GLOBFCT				26
#define EAI_ACTION_RENAMEFUNC			27

#define EAI_ACTION_GOTONEXTCPT			28
#define EAI_ACTION_AI2C					29

#define EAI_ACTION_BREAKPOINT			30
#define EAI_ACTION_ONESTEPOUT			31
#define EAI_ACTION_ONESTEPIN			32
#define EAI_ACTION_SYSBREAK				33
#define EAI_ACTION_DELETE				34
#define EAI_ACTION_SYSBREAKON			35
#define EAI_ACTION_BREAKON				36
#define EAI_ACTION_WARNINGHIGH			37
#define EAI_ACTION_DELBKP				38
#define EAI_ACTION_CALLSTACK			39

#define EAI_ACTION_CHECKOUTMODEL		40
#define EAI_ACTION_CHECKINMODEL			41
#define EAI_ACTION_CHECKINOUTMODEL		42
#define EAI_ACTION_GETLATESTMODEL		43
#define EAI_ACTION_UNDOCHECKOUT			44
#define EAI_ACTION_UPDATEOWNER			45

#define EAI_ACTION_LOOKDATE				46
#define EAI_ACTION_SHOWINBROWSER		47
#define EAI_ACTION_NEWPROCLIST			48
#define EAI_ACTION_OUTPROC				49
#define EAI_ACTION_DELALLWATCH			50
#define EAI_ACTION_AIRASTERS			51

#define EAI_ACTION_FIND					60
#define EAI_ACTION_FINDNEXT				62
#define EAI_ACTION_MATCH				63
#define EAI_ACTION_TOGGLEMARK			64
#define EAI_ACTION_NEXTMARK				65
#define EAI_ACTION_FINDFILES			66

#define EAI_ACTION_SPEEDSELECTMDL		100
#define EAI_ACTION_SPEEDSELECTFCT		101
#define EAI_ACTION_SPEEDSELECTHISFCT	102
#define EAI_ACTION_SPEEDSELECTGLOB		103

#define EAI_ACTION_UNIVERSE				110

#define EAI_ACTION_CHECKIN				120
#define EAI_ACTION_CHECKOUT				121
#define EAI_ACTION_CHECKINOUT			122
#define EAI_ACTION_UNDOCHECKOUTFILE		123
#define EAI_ACTION_GETLATESTFILE		124

#define EAI_ACTION_P4_ADD				129
#define EAI_ACTION_P4_DELETE			130
#define EAI_ACTION_P4_EDIT				131
#define EAI_ACTION_P4_REVERT			132
#define EAI_ACTION_P4_SYNC				133
#define EAI_ACTION_P4_EDITMODEL			134
#define EAI_ACTION_P4_DIFF				135
#define EAI_ACTION_P4_SHOWHISTORY		136
#define EAI_ACTION_P4_SUBMIT			137
#define EAI_ACTION_P4_CHECKINMODEL		138
#define EAI_ACTION_P4_REVERTMODEL		139
#define EAI_ACTION_P4_SYNCMODEL			140
#define EAI_ACTION_P4_SUBMITEDIT		141
#define EAI_ACTION_P4_CHECKINOUTMODEL	142
#define EAI_ACTION_P4_FORCESYNC			143

#define EAI_SEPACTION_OPTIONS			- 2

/* Base buffer. */

#ifndef EXTERN
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#define EXTERN	extern
#endif
#endif

EXTERN char *EAI_asz_ActionBase
#ifdef ACTION_GLOBAL
=
	"\
File=-1=--=0;\
New Global Function=26=--=0;\
Sep=0=--=0;\
Compile To File=10=--=0;\
Compile To Log=11=--=0;\
Sep=0=--=0;\
Save=1=^S=0;\
Sep=0=--=0;\
CheckOut File=121=--=0;\
Undo CheckOut File=123=--=0;\
CheckIn File=120=--=0;\
CheckInOut File=122=--=0;\
GetLatest Version File=124=--=0;\
Sep=0=--=0;\
Perforce - CheckIn File=137=--=0;\
Perforce - CheckOut File=131=--=0;\
Perforce - CheckInOut File=141=--=0;\
Perforce - Undo Checkout File=132=--=0;\
Perforce - GetLatest Version=133=--=0;\
Perforce - Add to server =129=--=0;\
Perforce - Delete from server=130=--=0;\
Sep=0=--=0;\
Perforce - Show difference=135=--=0;\
Perforce - Show history=136=--=0;\
Model=-1=--=0;\
Speed Select Fct=101=--=0;\
Speed Select Model=100=^#SPACE=0;\
Speed Select History=102=^NUMPAD0=0;\
Speed Select Global Library=103=^#L=0;\
Go To Next History File=28=^F3=0;\
Sep=0=--=0;\
New Model=20=^#M=0;\
New Function=21=^#F=0;\
New ProcList=48=--=0;\
New Vars=22=^#V=0;\
Sep=0=--=0;\
Add Function/Var=14=--=0;\
Rename=27=F2=0;\
Delete=34=--=0;\
Show in browser=47=--=0;\
Sep=0=--=0;\
LookAt Model Dates=46=--=0;\
Compile All Models=19=^#F7=0;\
Compile/Save=24=^F7=0;\
Compile/Save Dependencies=25=F7=0;\
Close=23=^#F4=0;\
Sep=0=--=0;"
"Generate C files=29=--=0;\
Warning Level=37=--=0;\
Instance=-3=--=0;\
Load Universe=110=-=0;\
Sep=0=--=0;\
Create Dummy Instance=17=^F9=0;\
Sep=0=--=0;\
Reinit Instance=18=--=0;\
Sep=0=--=0;\
Edition=-4=--=0;\
Find & Replace=60=^F=0;\
Find Next=62=F3=0;\
Find In Files=66=--=0;\
Sep=0=--=0;\
Match ({[]})=63=^U=0;\
Sep=0=--=0;\
Toggle Mark=64=^F2=0;\
Goto Next Mark=65=#F2=0;\
Debug=-1=--=0;\
One Step Out=31=F10=0;\
One Step In=32=F11=0;\
Out Procedure/Track=49=#F11=0;\
Sep=0=--=0;\
System BreakPoints=33=--=0;\
System BreakPoints ON=35=--=0;\
BreakPoints ON=36=--=0;\
Sep=0=--=0;\
Toggle BreakPoint=30=F9=0;\
Delete All BreakPoints=38=--=0;\
Delete All Watch=50=--=0;\
Sep=0=--=0;\
AI Rasters=51=--=0;\
Data-Control=-3=--=0;\
Get Latest Version Model=43=--=0;\
CheckOut Model=40=--=0;\
CheckIn Model=41=--=0;\
CheckInOut Model=42=--=0;\
Undo CheckOut Model=44=--=0;\
Sep=0=--=0;\
Perforce - Get Latest Model Version=140=--=0;\
Perforce - CheckOut Model=134=--=0;\
Perforce - CheckIn Model=138=--=0;\
Perforce - CheckInOut Model=142=--=0;\
Perforce - Undo CheckOut Model=139=--=0;\
Refresh=45=F4=0;\
Options=-2=--=0;\
;\
"
#endif
;
#undef EXTERN
#undef ACTION_GLOBAL
#endif
