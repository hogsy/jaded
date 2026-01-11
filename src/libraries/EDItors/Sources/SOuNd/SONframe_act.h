/*$T SONframe_act.h GC! 1.097 12/11/01 08:49:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS

/* Define constants. */
#define ESON_ACTION_AUTOPLAY					1
#define ESON_ACTION_EDITOR						2
#define ESON_ACTION_TEMPDIR						3
#define ESON_ACTION_EDIT						4
#define ESON_ACTION_RELOAD						5
#define ESON_ACTION_CHECKALLFILES				6

#define ESON_ACTION_SMODIFIER_NEW				8
#define ESON_ACTION_SMODIFIER_OPEN				9
#define ESON_ACTION_SMODIFIER_CLOSE				10
#define ESON_ACTION_SMODIFIER_SAVE				11
#define ESON_ACTION_DISPLAY_EQUALSPLIT			12
//13
#define ESON_ACTION_AUTOSAVE					14
#define ESON_ACTION_CLOSEBANK					15
#define ESON_ACTION_CLOSESND					16
//18
#define ESON_ACTION_CLOSEALL					17
#define ESON_ACTION_VOLUMEOFF					19
#define ESON_ACTION_SMODIFIERAUTOOPEN			22
#define ESON_ACTION_SETSOLO						23
#define ESON_ACTION_MODIFYBANK					25
#define ESON_ACTION_CLOSEALLWHENDESTROYWORLD	27

#define ESON_ACTION_SOUNDSPY					34
#define ESON_ACTION_INST_SET					39
#define ESON_ACTION_INST_SPY					40
#define ESON_ACTION_SOUNDSETSPY					41
#define ESON_ACTION_INST_EDITSOUND				44

#define ESON_ACTION_BANK_REPORT                 49
#define ESON_ACTION_SOUND_REPORT                50
#define ESON_ACTION_BANK_USER_REPORT            51
#define ESON_ACTION_SOUND_USER_REPORT           52
#define ESON_ACTION_BANK_CONTENTS_REPORT        53
#define ESON_ACTION_ENABLE_DEBUGLOG             54
#define ESON_ACTION_SOUNDUNSPY                  55
#define ESON_ACTION_BANK_FINDFILE               56
#define ESON_ACTION_SOUND_FINDFILE              57
#define ESON_ACTION_SMODIFIER_FINDFILE          58
#define ESON_ACTION_NOFREQTRACKMODULATION		59
#define ESON_ACTION_SMODIFIER_USER_REPORT		60
#define ESON_ACTION_SOUND_PLAY					61
#define ESON_ACTION_SOUND_PAUSE					62
#define ESON_ACTION_SOUND_STOP					63
#define ESON_ACTION_SMODIFIER_PLAY				64
#define ESON_ACTION_SMODIFIER_PAUSE				65
#define ESON_ACTION_SMODIFIER_STOP				66
#define ESON_ACTION_INST_SOLOGAO				67
#define ESON_ACTION_INST_MUTEGAO				68
#define ESON_ACTION_INST_SOLOINST				69
#define ESON_ACTION_INST_MUTEINST				70
#define ESON_ACTION_INST_SOLOOFF				71
#define ESON_ACTION_INST_MUTEOFF				72
#define ESON_ACTION_INST_EDITSMD				73
#define ESON_ACTION_INST_FINDSOUND				76
#define ESON_ACTION_INST_FINDSMD				77
#define ESON_ACTION_INST_FINDGAO				78
#define ESON_ACTION_DISPLAY_EFFVOL				79
#define ESON_ACTION_INST_ADDAIWATCH				80
#define ESON_ACTION_NO_SYNCHRO					81
#define ESON_ACTION_INS_OPEN					89
#define ESON_ACTION_INS_NEW						90
#define ESON_ACTION_SOUND_FINDREF				91

/* Base buffer. */
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#undef EXTERN
#define EXTERN	extern
#endif
EXTERN char *ESON_asz_ActionBase
#ifdef ACTION_GLOBAL
="\
Bank=-1=--=0;\
Report=49==0;\
Edit=0=--=0;\
Find=56==0;\
Modify=25==0;\
User Report=51==0;\
Contents Report=53==0;\
Close=15==0;\
Language=0=--=0;\
\
Sound=-2=--=0;\
Report=50==0;\
Edit=0=--=0;\
Spy This Sound=41==0;\
Find=57==0;\
User Report=52==0;\
Player=0=--=0;\
Play=61==0;\
Pause=62=^SPACE=0;\
Stop=63==0;\
Close=16==0;\
\
SModifier=-3=--=0;\
New=8=^N=0;\
Find=58==0;\
Open=9=^O=0;\
Save=11=^S=0;\
User Report=60==0;\
Player=0=--=0;\
Play=64==0;\
Pause=65=SPACE=0;\
Stop=66==0;\
Close=10=^F4=0;\
Insert=0=--=0;\
New=90==0;\
Open=89==0;\
\
Instance=-4=--=0;\
Find Smd=77==0;\
Edit Smd=73==0;\
Find Sound=76==0;\
Edit Sound=44==0;\
Solo/Mute=0=--=0;\
Solo Gao=67==0;\
Mute Gao=68==0;\
Solo Inst=69==0;\
Mute Inst=70==0;\
Solo Off=71==0;\
Mute Off=72==0;\
Instance=0=--=0;\
Modify=39==0;\
Spy=40==0;\
Add AI Watch=80==0;\
Gao=0=--=0;\
Find Gao=78==0;\
\
Tools=-5=--=0;\
Spy One Sound=34==0;\
Reset Spy=55==0;\
Files=0=--=0;\
Check All=6==0;\
Find Reference=91==0;\
\
Display=-6=--=0;\
Close all=17==0;\
Splitter=0=--=0;\
Equalize=12=^0=0;\
\
Options=-7=--=0;\
Disable DT Freq Modulation=59==0;\
Disable Synchro=81==0;\
Display Efficient Volume=79==0;\
Autoplay=1==0;\
Volume Off=19==0;\
Close All At World Destruction=27==0;\
SModifier=0=--=0;\
Auto Save=14==0;\
Auto Open=22==0;\
External=0=--=0;\
Sound Editor=2==0;\
Temp Dir=3==0;\
Edit=4==0;\
Reload=5==0;\
Debug=0=--=0;\
Enable debug log=54==0;\
\
;\
"
#endif
;

#undef EXTERN
#undef ACTION_GLOBAL
#endif
