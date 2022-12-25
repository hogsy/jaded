/*$T STRstruct.h GC!1.71 02/08/00 10:57:20 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __STRSTRUCT_H__
#define __STRSTRUCT_H__

#include "GRObject/GROstruct.h"
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/* Letter Flags : include letter value, page (sub material) used, mark, and other flags */
#define STR_Cul_LF_ValueMask            0x00000FFF

#define STR_Cul_LF_PageMask				0x0003F000
#define STR_Cul_LF_PageShift			12

#define STR_Cul_LF_MarkMask             0x00F00000
#define STR_Cul_LF_MarkShift            20

#define STR_Cul_LF_CenterX              0x01000000
#define STR_Cul_LF_CenterY              0x02000000
#define STR_Cul_LF_RightX               0x04000000
#define STR_Cul_LF_BottomY              0x08000000
#define STR_Cul_LF_Retrait              0x10000000
#define STR_Cul_LF_NewLine              0x20000000


/*$F Effect Flags  : 
    0-7  : type of effect
    8-15 : reserved for generic effect flags
    16-23: effect flag (can be set by IA)
    24-31: effect internal flag
    */
//================== type of effect
#define STR_Cul_EF_Fade2Black           0x00000001
#define STR_Cul_EF_Blink                0x00000002
#define STR_Cul_EF_Gouraud              0x00000003
#define STR_Cul_EF_ChangeColor          0x00000004
#define STR_Cul_EF_Bounce               0x00000005
#define STR_Cul_EF_Perturb              0x00000006
#define STR_Cul_EF_Shrink               0x00000007
#define STR_Cul_EF_Unshrink             0x00000008
#define STR_Cul_EF_ShrinkCenter         0x00000009
#define STR_Cul_EF_UnshrinkCenter       0x0000000A
#define STR_Cul_EF_Enlarge              0x0000000B
#define STR_Cul_EF_Unenlarge            0x0000000C    
#define STR_Cul_EF_Door                 0x0000000D
#define STR_Cul_EF_Undoor               0x0000000E
#define STR_Cul_EF_Rotate               0x0000000F
#define STR_Cul_EF_Flow                 0x00000010
#define STR_Cul_EF_Cut                  0x00000011
#define STR_Cul_EF_Italic				0x00000012
#define STR_Cul_EF_Joy					0x00000013
#define STR_Cul_EF_Blur					0x00000014
#define	STR_Cul_EF_Scroll				0x00000015

#define STR_Cul_EF_Type                 0x000000FF

//================== generic effect flag
#define STR_Cul_EF_ApplyToFrame			0x00000100
#define STR_Cul_EF_ApplyJustToFrame		0x00000200


//================== effect flag
#define STR_Cul_EF_Fade2Black_Invert	0x00010000

#define STR_Cul_EF_Blink_Smooth         0x00010000
#define STR_Cul_EF_Blink_Up             0x01000000
#define STR_Cul_EF_Blink_Change			0x02000000

#define STR_Cul_EF_ChangeColor_Apply	0x00010000

#define STR_Cul_EF_PerturbFadeIn        0x00010000
#define STR_Cul_EF_PerturbFadeOut       0x00020000

#define STR_Cul_EF_DoorUp               0x00000000
#define STR_Cul_EF_DoorDown             0x00010000
#define STR_Cul_EF_DoorLeft             0x00020000
#define STR_Cul_EF_DoorRight            0x00030000

/* Effect return value */
#define STR_Cul_ER_Terminated           0x00000001

/* parsing return value */
#define STR_Cul_Format_Unknown          0
#define STR_Cul_Format_EOL              1
#define STR_Cul_Format_Mark             2
#define STR_Cul_Format_Color            3
#define STR_Cul_Format_X                4
#define STR_Cul_Format_Y                5
#define STR_Cul_Format_W                6
#define STR_Cul_Format_H                7
#define	STR_Cul_Format_PageInst			8
#define	STR_Cul_Format_Page				9
#define	STR_Cul_Format_Ascii			10
#define STR_Cul_Format_Retrait          11
#define STR_Cul_Format_GlobalString     12
#define STR_Cul_Format_LetterEffect		13
#define STR_Cul_Format_Justify			14
#define STR_Cul_Format_Tab				15
#define STR_Cul_Format_TabGlobal		16
#define STR_Cul_Format_Frame			17
#define STR_Cul_Format_FrameColor		18
#define STR_Cul_Format_FrameIcon		19
#define STR_Cul_Format_FrameBorder		20
#define STR_Cul_Format_Align			21
#define STR_Cul_Format_Unicode	        22

/* string flags */
#define STR_Cuw_SF_Left                 0
#define STR_Cuw_SF_CenterWidth          1
#define STR_Cuw_SF_Right                2
#define STR_Cuw_SF_XJustifyMask         0x0003
#define STR_Cuw_SF_Top                  0
#define STR_Cuw_SF_CenterHeight         4
#define STR_Cuw_SF_Bottom               8
#define STR_Cuw_SF_YJustifyMask         0x000C
#define STR_Cuw_SF_Proportionnal        0x0010
#define STR_Cuw_SF_AdaptFrameToText     0x0020
#define STR_Cuw_SF_ShowFrame            0x0100
#define STR_Cuw_SF_IndepentLetter       0x0200
#define STR_Cuw_SF_Hide                 0x0400

#define STR_Cul_SFExt_SetText			0x0001
#define STR_Cul_SFExt_SetSize			0x0002
#define STR_Cul_SFExt_SetPivot			0x0004
#define STR_Cul_SFExt_SetAlign			0x0008
#define STR_Cul_SFExt_SetFlags			0x0010
#define STR_Cul_SFExt_AlignXCenter		0x0000
#define STR_Cul_SFExt_AlignYCenter		0x0100		
#define STR_Cul_SFExt_AlignYTop			0x0200
#define STR_Cul_SFExt_AlignShift		8
#define STR_Cul_SFExt_AlignMask			0xF

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

/* effects */    
typedef struct  STR_tdst_Fade_
{
    float   f_Time;
    float   f_CurTime;
    ULONG   ul_ColorSrc;
    ULONG   ul_ColorTgt;
    float   f_Delay;
} STR_tdst_Fade;

typedef struct STR_tdst_Blink_
{
    float f_Time;
    float f_CurTime;
    ULONG ul_Color1, ul_Color2;
} STR_tdst_Blink;

typedef struct STR_tdst_Gouraud_
{
    ULONG aul_Color[4];
} STR_tdst_Gouraud;

typedef struct STR_tdst_Bounce_
{
    float f_Amplitude;
    float f_TimeFactor;
    float f_YFactor;
    float f_Time;
    float f_CurTime;
} STR_tdst_Bounce;

typedef struct STR_tdst_Perturb_
{
    float fX, fY;
    float f_Time;
    float f_CurTime;
} STR_tdst_Perturb;

typedef struct STR_tdst_Shrink_
{
    float f_Time;
    float f_LetterTime;
    float f_CurTime;
    float f_Delay;
} STR_tdst_Shrink;

typedef struct STR_tdst_Enlarge_
{
    float f_Time;
    float f_CurTime;
    float f_Delay;
    float f_Factor;
} STR_tdst_Enlarge;

typedef struct STR_tdst_Door_
{
    float f_Time;
    float f_CurTime;
    float f_Ref;
} STR_tdst_Door;

typedef struct STR_tdst_Rotate_
{
    float f_Angle;
    float f_Speed;
} STR_tdst_Rotate;

typedef struct STR_tdst_Flow_
{
    float   f_Time;
    float   f_CurTime;
    float   f_Delta;
    int     i_Dir;
} STR_tdst_Flow;

typedef struct STR_tdst_Cut_
{
    float   min;
    float   max;
} STR_tdst_Cut;

typedef struct STR_tdst_Italic_
{
    float   value;
} STR_tdst_Italic;

typedef struct STR_tdst_Joy_
{
	int		i_Dir;
	char	c_Value;
	char	c_CurDir;
	short	w_Dummy;
	float	f_Time;
	float	f_CurTime;
} STR_tdst_Joy;

typedef struct STR_tdst_Blur_
{
	float	f_Time;
	float	f_CurTime;
	float	f_Dist;
} STR_tdst_Blur;

typedef struct STR_tdst_Scroll_
{
	float	f_Time;
	float	f_CurTime;
	int		i_NbLine; 
} STR_tdst_Scroll;

typedef struct  STR_tdst_Effect_
{
    ULONG                   ul_Flags;
    int                     i_Id;
    int                     i_String;
    int                     i_From;
    int                     i_Number;
    
    union
    {
        STR_tdst_Fade       st_Fade;
        STR_tdst_Blink      st_Blink;
        STR_tdst_Gouraud    st_Gouraud;
        STR_tdst_Bounce     st_Bounce;
        STR_tdst_Perturb    st_Perturb;
        STR_tdst_Shrink     st_Shrink;
        STR_tdst_Enlarge    st_Enlarge;
        STR_tdst_Door       st_Door;
        STR_tdst_Rotate     st_Rotate;
        STR_tdst_Flow       st_Flow;
        STR_tdst_Cut        st_Cut;
		STR_tdst_Italic		st_Italic;
		STR_tdst_Joy		st_Joy;
		STR_tdst_Blur		st_Blur;
		STR_tdst_Scroll		st_Scroll;
    };
} STR_tdst_Effect;

/* one letter */
typedef struct  STR_tdst_Letter_
{
    ULONG           ul_Flags;
    short           x, y, w, h;
    ULONG           ul_Color;
} STR_tdst_Letter;

/* string frame */
typedef struct STR_tdst_Frame
{
    ULONG               ul_Flags;
    short               x[2],y[2];
	short				borderx, bordery;
    ULONG               ul_Color;
	ULONG				ul_Sprite;
} STR_tdst_Frame;

/* one string */
typedef struct  STR_tdst_String_
{
    USHORT              uw_Flags;
    UCHAR               uc_NbPages;
    CHAR                c_Page;

    USHORT              uw_First;
    USHORT              uw_Number;

    USHORT              uw_SubFirst;
    USHORT              uw_SubNumber;

    MATH_tdst_Vector    st_Pivot;
    STR_tdst_Frame      st_Frame;
} STR_tdst_String;

/* All data : strings, letters, effect */
typedef struct  STR_tdst_Struct_
{
    GRO_tdst_Struct             st_Id;
    ULONG                       ul_NbMaxLetters;
    
    UCHAR                       uc_NbMaxStrings;
    UCHAR                       uc_NbMaxEffects;
    UCHAR                       uc_NbCurEffects;
    UCHAR                       uc_Dummy; 

    short                       w_FontPage;
    short                       w_Dummy; 

    ULONG                       ul_RenderingCounter;
    USHORT                      uw_SW;                  /* screen width */
    USHORT                      uw_SH;                  /* screen height */
	
    STR_tdst_Letter             *dst_Letter;
    STR_tdst_String             *dst_String;
    STR_tdst_Effect             st_Effect[96];
    struct STR_tdst_FontDesc_   *pst_Font;
    ULONG                       *pul_Sprite;
    ULONG                       *pul_SpriteFont;
    ULONG                       ul_NbSprite;

	SHORT						aw_Interligne[ 24 ];
	SHORT						aw_LetterSpacing[ 24 ];
	float						f_MaxWidth;
} STR_tdst_Struct;

/* one font letter description */
typedef struct  STR_tdst_FontLetterDesc_
{
    float   f_U[2], f_V[2];
} STR_tdst_FontLetterDesc;

/* font description */
typedef struct  STR_tdst_FontDesc_
{
    ULONG                   ul_Key;
    ULONG                   ul_AltKey;
    void                    *pst_Material;
    //STR_tdst_FontLetterDesc ast_Letter[224];
    ULONG					ul_NbChar;
	STR_tdst_FontLetterDesc *pst_Letter;
    USHORT                  uw_Width, uw_Height;
    float					fWoH;
} STR_tdst_FontDesc;

/* format description */
typedef struct  STR_tdst_Format
{
    int i_Format;
    union
    {
        int     i_Param;
        float   f_Param;
        ULONG   ul_Param;
    };
} STR_tdst_Format;

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */
extern ULONG                        STR_ul_Color;
extern struct OBJ_tdst_GameObject_ *STR_sgpst_GO;
extern short                        STR_w_XFramePivot, STR_w_YFramePivot;
extern short                        STR_w_XPivot, STR_w_YPivot;
extern short                        STR_w_LetterDX, STR_w_LetterDY;

extern LONG                         STR_sgl_NumberOfFont;
extern STR_tdst_FontDesc           *STR_sgpst_Font;

extern STR_tdst_Format              STR_sgst_Format;

extern float                        STR_f_DefaultLetterDepth;
extern float                        STR_f_FrameDepthOffset;

extern int                          STR_sgac_CharTable[256];

#define                             STR_Cuc_NbGlobal        8
#define                             STR_Cuc_GlobalLength    16
extern char                         STR_sgaac_Global[ STR_Cuc_NbGlobal ][ STR_Cuc_GlobalLength];

extern	STR_tdst_Effect				*STR_spst_CurEffect;

/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

#define STR_M_A2I(value)    ( (((value) & STR_Cul_LF_ValueMask) >= 32) ? (((value) & STR_Cul_LF_ValueMask) - 32) : 0)

#define STR_M_GetLetterOrReturn0(p, i, s) \
    { \
        if((ULONG) i >= (ULONG) p->ul_NbMaxLetters) \
            return 0; \
        s = p->dst_Letter + i; \
    }

#define STR_M_GetLetterOrReturn(p, i, s) \
    { \
        if((ULONG) i >= (ULONG) p->ul_NbMaxLetters) \
            return; \
        s = p->dst_Letter + i; \
    }

#define STR_M_SetMaterial(go)   STR_sgpst_GO->pst_Base->pst_Visu->pst_Material = go->pst_Base->pst_Visu->pst_Material;

#define STR_M_SetFont(s, i) \
    { \
        if(((SHORT) i) != s->w_FontPage) \
        { \
            s->w_FontPage = (SHORT) i; \
            s->pst_Font = STR_pst_GetFontDesc(STR_sgpst_GO->pst_Base->pst_Visu->pst_Material, s->w_FontPage); \
        } \
    }

#define STR_M_ResetFont(s) \
    { \
        s->w_FontPage = -1; \
        s->pst_Font = NULL; \
    }

/*$4
 ***********************************************************************************************************************
    Macros pour le recalage (PS2)
 ***********************************************************************************************************************
 */
#if defined( PSX2_TARGET ) || defined( _XBOX ) || defined(_XENON)
extern float    STR_f_XOffset;
extern float    STR_f_XFactor;
extern float    STR_f_YOffset;
extern float    STR_f_YFactor;
#endif

#if defined( PSX2_TARGET ) || defined( _XBOX ) || defined(_XENON)

#define STR_M_Recalage( _x, _y )\
{\
    _x = (short)((_x * STR_f_XFactor) + STR_f_XOffset);\
    _y = (short)((_y * STR_f_YFactor) + STR_f_YOffset);\
}

#define STR_M_RecalageSprite( _x1, _y1, _x2, _y2 )\
{\
    STR_M_Recalage( _x1, _y1 );\
    STR_M_Recalage( _x2, _y2 );\
}

#define STR_M_RecalageFrame( _x0, _y0, _x1, _y1, _x2, _y2, _x3, _y3 )\
{\
    STR_M_Recalage( _x0, _y0 );\
    STR_M_Recalage( _x1, _y1 );\
    STR_M_Recalage( _x2, _y2 );\
    STR_M_Recalage( _x3, _y3 );\
}

#define STR_M_RecalageFloat( _x, _y )           \
{                                               \
    _x = (_x * STR_f_XFactor) + STR_f_XOffset;  \
    _y = (_y * STR_f_YFactor) + STR_f_YOffset;  \
}

#define STR_M_RecalageSpriteFloat( _x1, _y1, _x2, _y2 )\
{\
    _x1 *= GDI_gpst_CurDD->st_Camera.f_Width;\
    _x2 *= GDI_gpst_CurDD->st_Camera.f_Width;\
    _y1 *= GDI_gpst_CurDD->st_Camera.f_Height;\
    _y2 *= GDI_gpst_CurDD->st_Camera.f_Height;\
    _x2 -= _x1;\
    _y2 -= _y1;\
    STR_M_RecalageFloat( _x1, _y1 );\
    _x2 += _x1;\
    _y2 += _y1;\
    _x1 /= GDI_gpst_CurDD->st_Camera.f_Width;\
    _x2 /= GDI_gpst_CurDD->st_Camera.f_Width;\
    _y1 /= GDI_gpst_CurDD->st_Camera.f_Height;\
    _y2 /= GDI_gpst_CurDD->st_Camera.f_Height;\
}

#else
#define STR_M_Recalage( x, y )
#define STR_M_RecalageSprite( _x1, _y1, _x2, _y2 )
#define STR_M_RecalageSpriteFloat( _x1, _y1, _x2, _y2 )
#define STR_M_RecalageFrame( _x0, _y0, _x1, _y1, _x2, _y2, _x3, _y3 )
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void                STR_Init(void);
void                STR_Close(void);
void                STR_ReinitAll(void);
STR_tdst_Struct     *STR_pst_Create(void);
void                STR_Free(STR_tdst_Struct *);

STR_tdst_Struct     *STR_pst_Get(struct OBJ_tdst_GameObject_ *);

void                STR_Reset( struct OBJ_tdst_GameObject_ *);

#if defined(_XENON_RENDER)
void STR_XenonShutdown(void);
#endif
/*
 =======================================================================================================================
    Font
 =======================================================================================================================
 */

void                STR_AddFont( ULONG, struct TEX_tdst_File_Desc_ *);
void                STR_FontAddAlternativeKey( ULONG , ULONG );
void                STR_FontResolveAlternativeKey(void);
STR_tdst_FontDesc   *STR_pst_GetFont( ULONG );

#ifdef ACTIVE_EDITORS
ULONG               STR_ul_SaveFontDescriptor(STR_tdst_FontDesc *, char *, char * );
ULONG				STR_ul_CreateFontDescriptor( ULONG, char *, ULONG);
#endif

/*
 =======================================================================================================================
    Content
 =======================================================================================================================
 */
void                STR_SetString(struct OBJ_tdst_GameObject_ *, int, char *);
void                STR_SetStringExt(struct OBJ_tdst_GameObject_ *, int, int, char *, MATH_tdst_Vector *);
void                STR_InsertString(struct OBJ_tdst_GameObject_ *, int, int, char *);
void                STR_SetFloat(struct OBJ_tdst_GameObject_ *, int, float, int );
void                STR_InsertFloat(struct OBJ_tdst_GameObject_ *, int, int, float, int );
void                STR_InsertInt(struct OBJ_tdst_GameObject_ *, int, int, int );
void                STR_DelChar(struct OBJ_tdst_GameObject_ *, int, int, int);
void                STR_SetChar(STR_tdst_Struct *, int, int, int);
int                 STR_i_GetChar(STR_tdst_Struct *, int, int );
LONG                STR_l_GetLength(STR_tdst_Struct *, ULONG);
LONG                STR_l_GetSubLength(STR_tdst_Struct *, ULONG, int);
void                STR_SetPivot(STR_tdst_Struct *, int, MATH_tdst_Vector *);
MATH_tdst_Vector    *STR_pst_GetPivot(STR_tdst_Struct *, int);
void                STR_SetFlag(STR_tdst_Struct *, int, ULONG);
ULONG               STR_ul_GetFlag(STR_tdst_Struct *, int);

void                STR_SetLetterFlag( struct OBJ_tdst_GameObject_ *, int, int, int, int );
ULONG               STR_GetLetterFlag( struct OBJ_tdst_GameObject_ *, int , int );
void                STR_ToUpper( struct OBJ_tdst_GameObject_ *, int, int, int );
void                STR_ToLower( struct OBJ_tdst_GameObject_ *, int, int, int );

void                STR_ToString( struct OBJ_tdst_GameObject_ *, int, int, int, char * );

/*
 =======================================================================================================================
    Global string
 =======================================================================================================================
 */
void                STR_GlobalSet( int , char * );
void                STR_GlobalSetChar( int , int , int );
int                 STR_i_GlobalLength( int );
char               *STR_sz_GlobalGet( int );

/*
 =======================================================================================================================
    Sub string
 =======================================================================================================================
 */
void                STR_SetSubStringWithLine(struct OBJ_tdst_GameObject_ *, int, int, int );
int                 STR_i_CountLine( struct OBJ_tdst_GameObject_ *, int, int, int );
void                STR_SetSubStringWithMark(struct OBJ_tdst_GameObject_ *, int, int, int );
int                 STR_i_CountMark( struct OBJ_tdst_GameObject_ *, int, int, int, int );
void                STR_SetSubString(struct OBJ_tdst_GameObject_ *, int, int, int );

/*
 =======================================================================================================================
    Pos
 =======================================================================================================================
 */
void                STR_SetPos(struct OBJ_tdst_GameObject_ *, int, int, int, MATH_tdst_Vector *);
MATH_tdst_Vector	*STR_pst_GetMin( struct OBJ_tdst_GameObject_ *, int, int, int, MATH_tdst_Vector *);
MATH_tdst_Vector	*STR_pst_GetMax( struct OBJ_tdst_GameObject_ *, int, int, int, MATH_tdst_Vector *);
void                STR_Align( struct OBJ_tdst_GameObject_ *, int, int, int, int );
/*
 =======================================================================================================================
    Size
 =======================================================================================================================
 */
void                STR_SetSize(struct OBJ_tdst_GameObject_ *, int, int, int, MATH_tdst_Vector *);
MATH_tdst_Vector    *STR_pst_GetSize(struct OBJ_tdst_GameObject_ *, int, int, int, MATH_tdst_Vector *);
void				STR_GetRect( struct OBJ_tdst_GameObject_ *, int , int , int , int,  MATH_tdst_Vector * );

/*
 =======================================================================================================================
    Color
 =======================================================================================================================
 */
void                STR_SetColor(STR_tdst_Struct *, int, int, int, ULONG);
void                STR_SetAlpha(STR_tdst_Struct *, int, int, int, ULONG);
void                STR_SetColorExt(STR_tdst_Struct *, int, int, int, ULONG, ULONG);
int					STR_GetColor(STR_tdst_Struct *, int, int, int);

/*
 =======================================================================================================================
    Font page
 =======================================================================================================================
 */
void                STR_ChangePage(STR_tdst_Struct *, int, int, int, int, int);

/*
 =======================================================================================================================
    Frame
 =======================================================================================================================
 */
void                STR_SetFrame( struct OBJ_tdst_GameObject_ *, int, int, int );
void                STR_SetFramePos( struct OBJ_tdst_GameObject_ *, int, MATH_tdst_Vector *, MATH_tdst_Vector *);
void                STR_SetFrameBorderSize( struct OBJ_tdst_GameObject_ *, int, float );
void                STR_SetFrameColor( struct OBJ_tdst_GameObject_ *, int, ULONG );

/*
 =======================================================================================================================
    Effect
 =======================================================================================================================
 */

int                 STR_i_EffectAdd(struct OBJ_tdst_GameObject_ *, int , int , int , int );
void                STR_EffectDel(struct OBJ_tdst_GameObject_ *, int );
void                STR_EffectSeti(struct OBJ_tdst_GameObject_ *, int , int , int );
int					STR_EffectGeti(struct OBJ_tdst_GameObject_ *, int , int );
void                STR_EffectSetf(struct OBJ_tdst_GameObject_ *, int , int , float );
void				STR_SetCurEffect(STR_tdst_Struct *, int );
void				STR_EffectCheck( STR_tdst_Struct *, int, int, int );
void				STR_EffectDelAllForAString(STR_tdst_Struct *, int );

/*
 =======================================================================================================================
    Parse string 
 =======================================================================================================================
 */
LONG                STR_l_GetStringLength(char *);
void                STR_ParseString( STR_tdst_Struct *, int, STR_tdst_Letter *, unsigned char *, LONG , float , float , ULONG, int );
LONG				STR_l_ReadFormat(char *);

/*
 =======================================================================================================================
    3D Text
 =======================================================================================================================
 */
void                STR_Init3DString( struct OBJ_tdst_GameObject_ *, char * );
void                STR_Init3DStringExt( struct OBJ_tdst_GameObject_ *, char *, int );
void                STR_Init3DStringCenter( struct OBJ_tdst_GameObject_ *, char *, int );
void                STR_Init3DStringFromSTR( struct OBJ_tdst_GameObject_ *, struct OBJ_tdst_GameObject_ *, int , int , int );

void                STR_3DStringList_Clear( void );
void                STR_3DStringList_Reinit( void );
void                STR_3DStringList_Add( struct OBJ_tdst_GameObject_ * );

/*
 =======================================================================================================================
    Private functions 
 =======================================================================================================================
 */

LONG                STR_l_GetMarkIndex(STR_tdst_Struct *, STR_tdst_String *, int );
LONG                STR_l_GetValidIndex(STR_tdst_Struct *, STR_tdst_String *, int );
LONG                STR_l_GetValidNumber(STR_tdst_Struct *, STR_tdst_String *, int , int );
STR_tdst_String     *STR_pst_GetString(STR_tdst_Struct *, int);
void                STR_GetRectShort(STR_tdst_Struct *, int, int, int, MATH_tdst_Vector *, MATH_tdst_Vector *);

void                STR_RecomputePos(STR_tdst_Struct *, STR_tdst_String *, int, int);

LONG                STR_l_Effect_PreTreatAll( STR_tdst_Struct * );
LONG                STR_l_Effect_CountAdditionalSprite(STR_tdst_Struct * );
void                STR_Effect_TreatAll( STR_tdst_Struct * );
void                STR_Effect_TreatAll2( STR_tdst_Struct * );

STR_tdst_FontDesc   *STR_pst_GetFontDesc(void *, ULONG);

LONG                STR_l_HasSomethingToRender( struct GRO_tdst_Visu_ *, struct GEO_tdst_Object_ **);
void                STR_Render(struct OBJ_tdst_GameObject_ *);



/*
 =======================================================================================================================
    Editors
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
void                STR_ChangeNumberMaxOfLetters(STR_tdst_Struct *, ULONG);
void                STR_DelFormatString( char * );
#else
#define STR_ChangeNumberMaxOfLetters(a, b)
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __STRSTRUCT_H */
