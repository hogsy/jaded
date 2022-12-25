/*$T textframe_act.h GC 1.138 03/25/04 15:34:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS

/* Define constants. */
#define ETEXT_ACTION_BANK_NEW				10
#define ETEXT_ACTION_BANK_CLOSE				12
#define ETEXT_ACTION_BANK_OPEN				13
#define ETEXT_ACTION_CLOSEALL				14
#define ETEXT_ACTION_BANK_NEWNOLANG			16

#define ETEXT_ACTION_FILE_NEW				20
#define ETEXT_ACTION_FILE_SAVE				21
#define ETEXT_ACTION_FILE_DEL				22
#define ETEXT_ACTION_FILE_AUTOSNDFILE		24
#define ETEXT_ACTION_SYNC					23

#define ETEXT_ACTION_ENTRY_NEW				30
#define ETEXT_ACTION_ENTRY_NEWMULTI			31
#define ETEXT_ACTION_ENTRY_DEL				32
#define ETEXT_ACTION_ENTRY_COPY				33
#define ETEXT_ACTION_ENTRY_CUT				34
#define ETEXT_ACTION_ENTRY_PASTE			35

#define ETEXT_ACTION_OPTION_STANDARD_LANG	40
#define ETEXT_ACTION_OPTION_SYNC_AUTO		41
#define ETEXT_ACTION_OPTION_KEEP_EMPTY_TXT	42
#define ETEXT_ACTION_UNICODE				43
#define ETEXT_ACTION_UPDATECHARTABLE		44
#define ETEXT_ACTION_OPTION_KEEP_ALL_TXT	45

#define ETEXT_ACTION_FIND					50
#define ETEXT_ACTION_NEXTFIND				51
#define ETEXT_ACTION_SYNCALL				52
#define ETEXT_ACTION_EXPORT_FILE			53
#define ETEXT_ACTION_EXPORT_ALL				55
#define ETEXT_ACTION_IMPORT					56
#define ETEXT_ACTION_EXPORTNOFORMAT			57
#define ETEXT_ACTION_FINDPREV				58

#define ETEXT_ACTION_FILE_AUTOPREFIX		59

#define ETEXT_ACTION_CHECKANDFIX			60
#define ETEXT_ACTION_FILE_AUTOPREFIXSEL		61
#define ETEXT_ACTION_REPORT_SOUND_FILE		62

#define ETEXT_ACTION_TOOL_DISP_LOADED		63
#define ETEXT_ACTION_TOOL_REPORT_NOSOUND	64

#define ETEXT_ACTION_FINDSALECHAR			70
#define ETEXT_ACTION_REPLACESALECHAR		71


/* Base buffer. */
#ifdef ACTION_GLOBAL
#define EXTERN
#else
#define EXTERN	extern
#endif
EXTERN char *ETEXT_asz_ActionBase
#ifdef ACTION_GLOBAL
= "\
Bank=-2=--=0;\
Open=13==0;\
Create=10==0;\
Create Language Independant=16==0;\
Close=12=^F4=0;\
Close All=14==0;\
\
File=-3=--=0;\
Create New Language File=20==0;\
Save=21=^S=0;\
Delete=22==0;\
Get Auto Sound=24==0;\
Sep=0=--=0;\
Get Auto Prefix=59==0;\
Get Auto Prefix (only on selected entries)=61==0;\
\
Entry=-4=--=0;\
Create=30=^E=0;\
Create Multi=31=^#E=0;\
Delete=32=^DEL=0;\
Sep=0=--=0;\
Copy=33=--=0;\
Cut=34=--=0;\
Paste=35=--=0;\
\
Tools=-5=--=0;\
Find=50=^F=0;\
Find Next=51=F3=0;\
Find Previous=58=#F3=0;\
Sep=0=--=0;\
Find sale à=70=#^F=0;\
Replace sale à (and save)=71=#^R=0;\
Sep=0=--=0;\
Synchronize=23==0;\
Synchronize All=52==0;\
Sep=0=--=0;\
No Format Code=57=--=0;\
Export File=53=--=0;\
Export All=55=--=0;\
Import=56=--=0;\
Sep=0=--=0;\
Check and fix=60=--=0;\
Report Sound Files=62=--=0;\
Report NoSound Text=64=--=0;\
Display All Files Loaded=63=--=0;\
\
Options=-6=--=0;\
Manage Standard Languages=40==0;\
Synchronize=0=--=0;\
Auto=41==0;\
Keep Empty Text=42==0;\
Keep All Text=45==0;\
Sep=0=--=0;\
Unicode=43==0;\
Set Character Tables=44==0;\
;\
"
#endif
;

EXTERN char *ETEXT_export_head
#ifdef ACTION_GLOBAL
= 
"<html xmlns:o=""urn:schemas-microsoft-com:office:office"" xmlns:x=""urn:schemas-microsoft-com:office:excel"">\
    <head>\
        <style>\
            <!--\
                tr {mso-height-source:auto;}\
                col	{mso-width-source:auto;}\
                br {mso-data-placement:same-cell;}\
            -->\
        </style>\
        <!--[if gte mso 9]><xml>\
        <x:ExcelWorkbook>\
        <x:ExcelWorksheets>\
        <x:ExcelWorksheet>\
            <x:Name>xls</x:Name>\
            <x:WorksheetOptions>\
            <x:DefaultColWidth>10</x:DefaultColWidth>\
            <x:Selected/>\
            <x:ProtectContents>False</x:ProtectContents>\
            <x:ProtectObjects>False</x:ProtectObjects>\
            <x:ProtectScenarios>False</x:ProtectScenarios>\
            </x:WorksheetOptions>\
        </x:ExcelWorksheet>\
        </x:ExcelWorksheets>\
        <x:WindowHeight>12870</x:WindowHeight>\
        <x:WindowWidth>17535</x:WindowWidth>\
        <x:WindowTopX>315</x:WindowTopX>\
        <x:WindowTopY>2730</x:WindowTopY>\
        <x:ProtectStructure>False</x:ProtectStructure>\
        <x:ProtectWindows>False</x:ProtectWindows>\
        </x:ExcelWorkbook>\
        </xml><![endif]-->\
    </head>\
    <body link=""blue"" vlink=""purple"">\
    <table u1=str>"
#endif
;

EXTERN char *ETEXT_export_tail
#ifdef ACTION_GLOBAL
= "</table></body></html>"
#endif
;

#undef EXTERN
#undef ACTION_GLOBAL
#endif
