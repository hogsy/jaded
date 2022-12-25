/*$T BROframe_act.h GC! 1.081 06/21/00 16:35:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS

/* Define constants. */
#define EBRO_ACTION_IMPORT				1
#define EBRO_ACTION_EXPORT				3
#define EBRO_ACTION_IMPORTMERGE			4
#define EBRO_ACTION_EXPORTMERGE			5
#define EBRO_ACTION_IMPORTMERGEFILT		6
#define EBRO_ACTION_EXPORTMERGEFILT		7
#define EBRO_ACTION_IMPORTMERGEEDITFILT		8
#define EBRO_ACTION_EXPORTMERGEEDITFILT		9

#define EBRO_ACTION_ADDFAV				10
#define EBRO_ACTION_ORGFAV				11
#define EBRO_ACTION_CREATEDIR			20
#define EBRO_ACTION_RENAME				21
#define EBRO_ACTION_DELETE				22
#define EBRO_ACTION_CREATEFILE			23
#define EBRO_ACTION_CREATESPECIALFILE	24
#define EBRO_ACTION_ORDERGRP			25
#define EBRO_ACTION_CHGKEY				26
#define EBRO_ACTION_CLEANGRP			27
#define EBRO_ACTION_LOGSIZE				28
#define EBRO_ACTION_RENAMEMULTI			29

#define EBRO_ACTION_REFRESH				30
#define EBRO_ACTION_FIND				31
#define EBRO_ACTION_FINDNEXT			32
#define EBRO_ACTION_ENGINEMODE			34
#define EBRO_ACTION_FINDKEY				35
#define EBRO_ACTION_COLLAPSE			36
#define EBRO_ACTION_TRACEKEY			37

#define EBRO_ACTION_STRINGFILTER		38

#define EBRO_ACTION_SCANKEY		        39
#define EBRO_ACTION_SCANTRIGGER		    40

#define EBRO_ACTION_CLEARUNLOADED		41
#define EBRO_ACTION_GROUPCHECK          42
#define EBRO_ACTION_GROUPOWNEROF        43
#define EBRO_ACTION_FILETYPEREFINGROUP  44

#define EBRO_ACTION_LISTUNUSEDGAO       45
#define EBRO_ACTION_LISTEXTERNREF       46

#define EBRO_ACTION_DELDIR				50
#define EBRO_ACTION_CHECKIN				51
#define EBRO_ACTION_CHECKOUT			52
#define EBRO_ACTION_UNDOCHECKOUT		53
#define EBRO_ACTION_GETLVERSION			54
#define EBRO_ACTION_CHECKINNOOUT		56
#define EBRO_ACTION_LOCALCHECKOUT		57
#define EBRO_ACTION_CHECKINMIRROR		58
#define EBRO_ACTION_IGNORERECENT		59

#define EBRO_ACTION_LINKTOEXT			60
#define EBRO_ACTION_IMPFROMEXT			61
#define EBRO_ACTION_EXPTOEXT			62
#define EBRO_ACTION_REFLINKED			63
#define EBRO_ACTION_UNLINK				64
#define EBRO_ACTION_SETASMIRROR			65
#define EBRO_ACTION_ORDERPATHS			66
#define EBRO_ACTION_SETASINVMIRROR		67
#define EBRO_ACTION_IMPORTONLYLOADED    68
#define EBRO_ACTION_IMPORTONLYTRUNCATED 69

#define EBRO_ACTION_ZOOM3D1				80
#define EBRO_ACTION_FORCELOAD1			84
#define EBRO_ACTION_SELECTGROUP1		88

#define EBRO_ACTION_FINDCHECKWORLD		89

#define EBRO_ACTION_CREATECOB			126
#define EBRO_ACTION_DISPLAYINFO			127

#define EBRO_ACTION_CREATEASSSMD        128
#define EBRO_ACTION_CREATEASSMTX        129

#define EBRO_SEPACTION_OPTIONS			- 4

// Added MENU OPTION TO IMPORT/EXPORT KEYS FROM/TO BF/FILESYSTEM (used to compare with perforce data)
#define EBRO_ACTION_IMPORTKEYS			140
#define EBRO_ACTION_EXPORTKEYS			141

// Added MENU OPTION to use PERFORCE as Source Control
#define EBRO_ACTION_PERFORCE_FORCESYNC	142
#define EBRO_ACTION_PERFORCE_EDIT		143
#define EBRO_ACTION_PERFORCE_SYNC		144

#define EBRO_ACTION_PERFORCE_ADD		145
#define EBRO_ACTION_PERFORCE_DELETE		146
#define EBRO_ACTION_PERFORCE_DIFF		147
#define EBRO_ACTION_PERFORCE_HISTORY	148
#define EBRO_ACTION_PERFORCE_SERVERSYNC	149
#define EBRO_ACTION_PERFORCE_SUBMIT		150
#define EBRO_ACTION_PERFORCE_REVERT     151
#define EBRO_ACTION_PERFORCE_SUBMITEDIT	152

/* Base buffer. */
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#undef EXTERN
#define EXTERN	extern
#endif
EXTERN char *EBRO_asz_ActionBase
#ifdef ACTION_GLOBAL
=
	"\
Import/Export=-1=--=0;\
Import=1=#I=210;\
Export=3=#E=211;\
Import from Keys=140=--=0;\
Export to Keys=141=--=0;\
Sep=0=--=0;\
Imp/Merge=4=^M=212;\
Exp/Merge=5=^X=213;\
Sep=0=--=0;\
Imp/Merge Filtered=6=--=0;\
Exp/Merge Filtered=7=--=0;\
Edit Imp/Merge Filter=8=--=0;\
Edit Exp/Merge Filter=9=--=0;\
Sep=0=--=0;\
Unlink=64=^U=0;\
Link To Path=60=^L=0;\
Sep=0=--=0;\
Set As Mirror=65=--=0;\
Set As Invert Mirror=67=--=0;\
Import Only Loaded Files=68=--=0;\
Import Only Truncated Files=69=--=0;\
Order Paths=66=--=0;\
Sep=0=--=0;\
Import From Path=61=^I=0;\
Export To Path=62=^E=0;\
Refresh Linked Paths=63=#F4=0;\
Favorites=-2=--=0;\
Add To Favorites=10=#1=203;\
Organize Favorites=11=#2=204;\
Data Control=-5=--=0;"
"Check In=51=^#I=221;\
Check In Mirror=58=--=0;\
Check InOut=56=^#Q=0;\
Check Out=52=^#O=222;\
Local Check Out=57=--=225;\
Undo Check Out=53=^#U=223;\
Get Latest Version=54=^#L=0;\
Sep=0=--=0;\
Perforce - CheckIn=150=--=0;\
Perforce - CheckOut =143=--=0;\
Perforce - CheckInOut=152=--=0;\
Perforce - Undo CheckOut=151=--=0;\
Perforce - GetLatest version=144=--=0;\
Perforce - Force GetLatest version=142=--=0;\
Perforce - Synchronize with server=149=--=0;\
Perforce - Add to server=145=--=0;\
Perforce - Delete from server=146=--=0;\
Perforce - Show Difference=147=--=0;\
Perforce - Show History=148=--=0;"   // string "break" required here to bypass compilation error related to length
"Sep=0=--=0;\
Delete Dir On Base=50=--=0;\
Sep=0=--=0;\
Ignore More Recent Files=59=--=0;\
Modify=-3=--=0;\
Rename=21=F2=207;\
Rename Multi Files=29=--=0;\
Delete=22=DEL=209;\
Log Dir Size=28=--=0;\
Change Key=26=--=0;\
Sep=0=--=0;\
Modify Group=25=--=0;\
Clean Group=27=--=0;\
Sep=0=--=0;\
Create Dir=20=^F2=208;\
Create File=23=^#F2=0;\
Create Special File=24==0;\
General=-1=--=0;\
String Filter=38=--=0;\
Sep=0=--=0;\
Refresh=30=F4=198;\
Collapse All=36=#ESCAPE=0;\
Sep=0=--=0;\
Engine Mode Display=34=F8=0;\
Sep=0=--=0;\
Find Key=35=#F3=0;\
Find File/Dir=31=F3=0;\
Find Next=32=^F3=0;\
Trace Key=37==0;\
Scan Key=39==0;\
Scan Trigger=40==0;\
List Unused GAOs=45==0;\
List Extern Refs=46==0;\
Sep=0=--=0;\
Zoom (3D View 1)=80=--=0;\
Force Load In (3D View 1) =84=--=0;\
Select Group (3D View 1)=88=--=0;\
Sep=0=--=0;\
Find in CheckWorld Dialog=89=--=0;\
Sep=0=--=0;\
Clear Loaded Mark=41=--=0;\
Sort/Display Info=127=--=0;\
Group check=42=--=0;\
Group owner of..=43=--=0;\
Unreferenced (by group) GAO =44=--=0;\
Options=-4=--=0;\
Sep=0=--=0;\
Create Associated COB=126=--=0;\
Create Associated SMD=128=--=0;\
Create MTX=129=--=0;\
;"
#endif
;
#undef EXTERN
#undef ACTION_GLOBAL
#endif
