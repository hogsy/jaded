/*$T STRdata.h GC! 1.081 02/26/04 09:55:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __STRDATA_H__
#define __STRDATA_H__

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

#define STR_DEBUG0

/* ALL data first allocation */
#define STR_Alloc_NbLetter		2048
#define STR_Alloc_LetterGran	1024
#define STR_Alloc_LetterMax		16384

#define STR_Alloc_NbString		64
#define STR_Alloc_StringGran	32
#define STR_Alloc_StringMax		512

#define STR_Alloc_StringLength	32

/* default value for data */
#define STR_Default_SizeX		0.02f
#define STR_Default_SizeY		0.04f

/* string flags */
#define STR_String_Used					0x00000001
#define STR_String_Hide					0x00000002
#define STR_String_FixedWidth			0x00000004

#define STR_String_JustPivotXCenter		0x00000100
#define STR_String_JustPivotXRight		0x00000200
#define STR_String_JustLetterXCenter	0x00000400
#define STR_String_JustLetterXRight		0x00000800
#define STR_String_JustPivotYCenter		0x00001000
#define STR_String_JustPivotYBottom		0x00002000
#define STR_String_JustLetterYCenter	0x00004000
#define STR_String_JustLetterYBottom	0x00008000
#define STR_String_JustFlags			0x0000FF00
#define STR_String_AlignXCenter			0x00010000
#define STR_String_AlignXRight			0x00020000
#define STR_String_AlignYCenter			0x00040000
#define STR_String_AlignYTop			0x00080000

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    one string
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	STR_tdst_OneString_
{
	MATH_tdst_Vector	st_Pivot;
	ULONG				ul_Flags;
	USHORT				uw_First;
	USHORT				uw_Nb;
	float				f_Timer;
	char				c_Group;
	char				c_Dummy;
	short				w_Dummy;
} STR_tdst_OneString;

/*
 -----------------------------------------------------------------------------------------------------------------------
    All data : strings, letters, effect
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	STR_tdst_AllData_
{
	USHORT						uw_MaxLetter;
	USHORT						uw_NbLetter;
	char						*pc_Letter;

	USHORT						uw_MaxString;
	USHORT						uw_NbString;
	STR_tdst_OneString			*pst_String;

	USHORT						uw_SW;	/* screen width */
	USHORT						uw_SH;	/* screen height */

	struct STR_tdst_FontDesc_	*pst_Font;
	struct MAT_tdst_Material_	*pst_Material;
	
	ULONG						*pul_Sprite;
	
	float						f_SizeX;
	float						f_SizeY;
	float						f_XMul;
	float						f_XEcart;
	float						f_YEcart;
	
	ULONG						ul_Group_Display;
} STR_tdst_AllData;

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

extern STR_tdst_AllData STR_gst_Data;
extern BOOL				STR_gb_DataIsInit;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void		STRDATA_Init(void);
void		STRDATA_Close(void);
void		STRDATA_ReinitAll(void);
void		STRDATA_Reset(void);

void		STRDATA_DefragBuffer( void );

int			STRDATA_i_CreateTextGroup( char *, MATH_tdst_Vector *, float, int );
int			STRDATA_i_CreateText( char *, MATH_tdst_Vector *, float );
void		STRDATA_DeleteText( int );
void		STRDATA_AppendText( int, char * );
void		STRDATA_AppendInt( int, int );
void		STRDATA_AppendHexa( int, int );
void		STRDATA_AppendFloat( int, float _f_Val, int );
void		STRDATA_AppendVector( int, MATH_tdst_Vector * );
void		STRDATA_AppendGao( int, OBJ_tdst_GameObject * );

int			STRDATA_i_Group_SetDisplay( int, int );

void					STRDATA_Render( void );
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __STRDATA_H */
