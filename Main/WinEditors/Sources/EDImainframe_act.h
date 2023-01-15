/*$T EDImainframe_act.h GC!1.39 06/30/99 12:34:24 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS

/* Define constants. */
#define EDI_ACTION_TOGGLEMENU       1
#define EDI_ACTION_TOGGLEENGINE     2
#define EDI_ACTION_ENGINESTEP		3
#define EDI_ACTION_EXIT             4
#define EDI_ACTION_TOGGLEORIENT     6
#define EDI_ACTION_TOGGLEENGINES    8

#define EDI_ACTION_OPENPROJECT      10
#define EDI_ACTION_NEWPROJECT       11
#define EDI_ACTION_CLOSEPROJECT     12
#define EDI_ACTION_CHECKBIGFILE     13
#define EDI_ACTION_CLEANBIGFILE     14
#define EDI_ACTION_RESTOREDEL		15
#define EDI_ACTION_CHECKBIGFILEC    16
#define EDI_ACTION_GETLASTKEY		17
#define EDI_ACTION_CHECKBIGFILESTAT	18
#define EDI_ACTION_CLEANFINAL		19

#define EDI_ACTION_DESKEQUALIZE     20
#define EDI_ACTION_DESKFILL         21
#define EDI_ACTION_DESKMAX          22
#define EDI_ACTION_DESKFULLMAX      23
#define EDI_ACTION_DESKFULLMAXRES   24

#define EDI_ACTION_CLEANFAT			25
#define EDI_ACTION_CLEANLOADED		26
#define EDI_ACTION_GETLASTKEYUSER	27

#define EDI_ACTION_DESKSAVE         30
#define EDI_ACTION_DESKNEW          31

#define EDI_ACTION_SET0             40
#define EDI_ACTION_SET1             41

#define EDI_ACTION_LINKCONTROL      50

#define EDI_ACTION_GETALLIDKEY     60

#define EDI_ACTION_CLOSEUNIVERSE    70
#define EDI_ACTION_SETUNIVERSEAIKEY 71
#define EDI_ACTION_REINITENGINE		72
#define EDI_ACTION_ACTIVATEUNIV		73
#define EDI_ACTION_SPEEDDRAW		74

#define EDI_ACTION_CHECKMEMORY      76
#define EDI_ACTION_CHECKMEMONTRAME  77
#define EDI_ACTION_DUMPMEMORY       78
#define EDI_ACTION_MEMSTATS			79

#define EDI_ACTION_JOYSTICK         90
#define EDI_ACTION_JOYSTICKPC       91

#ifdef JADEFUSION
#define EDI_ACTION_JOYSTICKXENON    92
#define EDI_ACTION_JOYSTICKPS2      93

#define EDI_ACTION_SETLANG			94

#else
#define EDI_ACTION_JOYSTICKPS2      92

#define EDI_ACTION_SETLANG			93
#endif

#define EDI_SEPACTION_DESKTOP       - 3
#define EDI_SEPACTION_EDITORS       - 5
#define EDI_SEPACTION_OPTIONS       - 6

#define EDI_ACTION_DESK1            100
#define EDI_ACTION_DESK2            101
#define EDI_ACTION_DESK3            102
#define EDI_ACTION_DESK4            103
#define EDI_ACTION_DESK5            104
#define EDI_ACTION_DESK6            105
#define EDI_ACTION_DESK7            106
#define EDI_ACTION_DESK8            107
#define EDI_ACTION_DESK9            108
#define EDI_ACTION_DESK10           109

#define EDI_ACTION_HELPABOUT		200
#define EDI_ACTION_HELP				201
#define EDI_ACTION_HELPPATH			202

#define EDI_ACTION_TEXTSTATS		300
#define EDI_ACTION_REFSTATS			301
#define EDI_ACTION_UNREFSTATS		302

#define EDI_ACTION_SPYSFON			303
#define EDI_ACTION_SPYSFRESET		304
#define EDI_ACTION_SPYSFTRACE		305
#define EDI_ACTION_SPYSFSTATE		306
#define EDI_ACTION_SPYCONTENTSF		307

#define EDI_ACTION_SPEEDACT			400

#define EDI_ACTION_CHECKVSSBASE		450
#define EDI_ACTION_CLEANVSSBASE		451
#define EDI_ACTION_CHECKVSSBASEBF	452

#define EDI_ACTION_GENMAPLIST       500

/* Base buffer. */
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#define EXTERN  extern
#endif
EXTERN char *EDI_asz_ActionBase

#ifdef ACTION_GLOBAL
#ifdef JADEFUSION
=
    "\
Project=-2=--=0;\
Open Project=10=^O=0;\
New Project=11=^N=0;\
Close Project=12=^#F4=0;\
BigFile=0=--=0;\
Check BigFile=13=--=0;\
Check BigFile & Stats=18=--=0;\
Check BigFile & Correct=16=--=0;\
Clean BigFile=14=--=0;\
Clean BigFile (Final !)=19=--=0;\
Clean BigFile (Fat Mode !)=25=--=0;\
Clean BigFile (Loaded Mode !)=26=--=0;\
Sep=0=--=0;\
Restore Deleted Files=15=--=0;\
Clean VSS Rights Base=451=--=0;\
Sep=0=--=0;\
Check VSS Rights Base=450=--=0;\
Check VSS Base=452=--=0;\
Sep=0=--=0;\
Scan Files=400=--=0;\
Generate maplist=500=--=0;\
Sep=0=--=0;\
Data Control=50=^#C=0;\
Retreive Last Key=17=--=0;\
Retreive Last Key For User=27=--=0;\
Retreive All Id Key=60=--=0;\
Sep=0=--=0;\
Exit Application=4=@F4=206;\
Desktop=-3=--=0;\
Equalize=20=^PAGEUP=0;\
Fill Holes=21=^PAGEDOWN=0;\
Toggle Maximised=22=^RETURN=0;\
Toggle Full Maximised=23=^#RETURN=0;\
Toggle Left Pane=1=^TAB=0;\
Horizontal Orientation=6=^BACK=0;\
Sep=0=--=0;\
Desktop 1=100=^1=0;\
Desktop 2=101=^2=0;\
Desktop 3=102=^3=0;\
Desktop 4=103=^4=0;\
Desktop 5=104=^5=0;\
Desktop 6=105=^6=0;\
Desktop 7=106=^7=0;\
Desktop 8=107=^8=0;\
Desktop 9=108=^9=0;\
Desktop 10=109=^0=0;\
Sep=0=--=0;\
New Desktop=31=^#D=218;\
Save Workspace=30=^W=219;\
Editors=-5=--=0;\
Engine=-7=--=0;\
Toggle Engine=2=F5=202;\
Run Engine (Game Mode)=8=^F5=0;\
Engine Step=3=PAUSE=0;\
Sep=0=--=0;\
Activate Universe=73=--=0;\
Set Universe AI Key=71=--=0;\
Sep=0=--=0;\
Reinit Engine=72=F6=0;\
Speed Mode=74=#TAB=--=0;\
Destroy All Engine Datas=70=--=0;\
Sep=0=--=0;\
Memory Stats=79=--=0;\
Texture Stats=300=--=0;\
Key -> References=301=--=0;\
Reference -> Keys=302=--=0;\
Sep=0=--=0;\
Check Memory Each Trame=77=--=0;\
Dump Memory=80=--=0;\
Check Memory=76=^M=0;\
Sep=0=--=0;\
SpySF On=303=--=0;\
SpySF Reset=304=--=0;\
SpySF Trace=305=--=0;\
SpySF State=306=--=0;\
SpySF (Check AI Var Content) =307=--=0;\
Sep=0=--=0;\
Joystick Calibration=90=--=0;\
Joystick PC=91=--=0;\
Joystick Xenon=92=--=0;\
Joystick PS2=93=--=0;\
Sep=0=--=0;\
Set Language=94=--=0;\
Options=-6=--=0;\
Help=-7=--=0;\
Help Path=202=--=0;\
Content=201=--=0;\
Sep=0=--=0;\
About=200=--=0;\
;\
"
#else
=
    "\
Project=-2=--=0;\
Open Project=10=^O=0;\
New Project=11=^N=0;\
Close Project=12=^#F4=0;\
BigFile=0=--=0;\
Check BigFile=13=--=0;\
Check BigFile & Stats=18=--=0;\
Check BigFile & Correct=16=--=0;\
Clean BigFile=14=--=0;\
Clean BigFile (Final !)=19=--=0;\
Clean BigFile (Fat Mode !)=25=--=0;\
Clean BigFile (Loaded Mode !)=26=--=0;\
Sep=0=--=0;\
Restore Deleted Files=15=--=0;\
Clean VSS Rights Base=451=--=0;\
Sep=0=--=0;\
Check VSS Rights Base=450=--=0;\
Check VSS Base=452=--=0;\
Sep=0=--=0;\
Scan Files=400=--=0;\
Generate maplist=500=--=0;\
Sep=0=--=0;\
Data Control=50=^#C=0;\
Retreive Last Key=17=--=0;\
Retreive Last Key For User=27=--=0;\
Retreive All Id Key=60=--=0;\
Sep=0=--=0;\
Exit Application=4=@F4=206;\
Desktop=-3=--=0;\
Equalize=20=^PAGEUP=0;\
Fill Holes=21=^PAGEDOWN=0;\
Toggle Maximised=22=^RETURN=0;\
Toggle Full Maximised=23=^#RETURN=0;\
Toggle Left Pane=1=^TAB=0;\
Horizontal Orientation=6=^BACK=0;\
Sep=0=--=0;\
Desktop 1=100=--=0;\
Desktop 2=101=--=0;\
Desktop 3=102=--=0;\
Desktop 4=103=--=0;\
Desktop 5=104=--=0;\
Desktop 6=105=--=0;\
Desktop 7=106=--=0;\
Desktop 8=107=--=0;\
Desktop 9=108=--=0;\
Desktop 10=109=^0=0;\
Sep=0=--=0;\
New Desktop=31=^#D=218;\
Save Workspace=30=^W=219;\
Editors=-5=--=0;\
Engine=-7=--=0;\
Toggle Engine=2=F5=202;\
Run Engine (Game Mode)=8=^F5=0;\
Engine Step=3=PAUSE=0;\
Sep=0=--=0;\
Activate Universe=73=--=0;\
Set Universe AI Key=71=--=0;\
Sep=0=--=0;\
Reinit Engine=72=F6=0;\
Speed Mode=74=#TAB=--=0;\
Destroy All Engine Datas=70=--=0;\
Sep=0=--=0;\
Memory Stats=79=--=0;\
Texture Stats=300=--=0;\
Key -> References=301=--=0;\
Reference -> Keys=302=--=0;\
Sep=0=--=0;\
Check Memory Each Trame=77=--=0;\
Dump Memory=80=--=0;\
Check Memory=76=^M=0;\
Sep=0=--=0;\
SpySF On=303=--=0;\
SpySF Reset=304=--=0;\
SpySF Trace=305=--=0;\
SpySF State=306=--=0;\
SpySF (Check AI Var Content) =307=--=0;\
Sep=0=--=0;\
Joystick Calibration=90=--=0;\
Joystick PC=91=--=0;\
Joystick PS2=92=--=0;\
Sep=0=--=0;\
Set Language=93=--=0;\
Options=-6=--=0;\
Help=-7=--=0;\
Help Path=202=--=0;\
Content=201=--=0;\
Sep=0=--=0;\
About=200=--=0;\
;\
"
#endif//JADEFUSION
#endif 
;
#undef EXTERN
#undef ACTION_GLOBAL
#endif
