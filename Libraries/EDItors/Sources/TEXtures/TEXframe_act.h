/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS

/* Define constants. */
#define ETEX_ACTION_ALPHA					1
#define ETEX_ACTION_SHOWNAME				2
#define ETEX_ACTION_CLOSE					8
#define ETEX_ACTION_RESFREE					10
#define ETEX_ACTION_RES32_32				11
#define ETEX_ACTION_RES64_64				12
#define ETEX_ACTION_RES128_128				13
#define ETEX_ACTION_RES256_256				14
#define ETEX_ACTION_FORCERES				15

#define ETEX_ACTION_TEXSHOWALL				20
#define ETEX_ACTION_TEXUSETMORDER			21
#define ETEX_ACTION_TEXSHOWTC				22
#define ETEX_ACTION_TEXSHOWRAWPAL			23
#define ETEX_ACTION_TEXSLOTPS2				24
#define ETEX_ACTION_TEXSLOTGC				25
#define ETEX_ACTION_TEXSLOTXBOX				26
#define ETEX_ACTION_TEXSLOTPC				27

#define ETEX_ACTION_CHECKUNIQUENAME			100
#define ETEX_ACTION_CHECKLOADED				101
#define ETEX_ACTION_CHECKTEXFILE			102
#define ETEX_ACTION_CHECKPALETTE			103
#define ETEX_ACTION_CHECKBADPARAMS			104
#define ETEX_ACTION_CHECKFONT				105
#define ETEX_ACTION_CHECKFORUSE				106
#define ETEX_ACTION_CHECKFORDUPLICATERAW	107

#define ETEx_ACTION_EXPORTTEX2TGA			200

#ifdef JADEFUSION
#define ETEx_ACTION_CREATEALLDDS			201
#endif

/* Base buffer. */
#ifdef ACTION_GLOBAL

#define EXTERN
#else

#define EXTERN  extern
#endif

EXTERN char * ETEX_asz_ActionBase

#ifdef ACTION_GLOBAL
=
    "\
File=-1=--=0;\
Create and Export TGA from tex=200==0;\
Display=-2=--=0;\
Toggle Alpha Mode=1=a=201;\
Show Name=2=n=201;\
Sep=0=--=0;\
Tex - Show all=20==201;\
Tex - Show TrueColor=22==201;\
Tex - Show RawPal=23==201;\
Tex - Show PS2=24==201;\
Tex - Show GC=25==201;\
Tex - Show XBOX=26==201;\
Tex - Show PC=27==201;\
Tex - Show slot in texture manager order=21==201;\
Sep=0=--=0;\
Close Display=8=^F4=0;\
Resolution=-3=--=0;\
Free=10=NUMPAD0=205;\
32 x 32=11=NUMPAD1=205;\
64 x 64=12=NUMPAD2=205;\
128 x 128=13=NUMPAD3=205;\
256 x 256=14=NUMPAD4=205;\
Sep=0=--=0;\
Force Res=15=n=201;\
Check=-4=--=0;\
Unique Name=100==0;\
Loaded=101==0;\
Tex file=102==0;\
Palette=103==0;\
Bad params=104==0;\
Font=105==0;\
User=106=^U=0;\
Duplicate Raw=107==0;\
;\
"
#endif

;
#undef EXTERN
#undef ACTION_GLOBAL
#endif

