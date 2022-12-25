/*$T TEXTstruct.h GC 1.138 07/01/05 15:09:49 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __TEXTSTRUCT_H__
#define __TEXTSTRUCT_H__

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#include "BIGfiles/BIGkey.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define TEXT_MaxLenId				64

#define TEXT_SearchResult_Name		1
#define TEXT_SearchResult_Content	2
#define TEXT_SearchResult_EntryKey	3

#define TEXT_Cte_Version1			0x0001
#define TEXT_Cte_CurrVersion		TEXT_Cte_Version1

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */


typedef enum	TEXT_tden_ExpFacial_
{
	TEXT_e_ExpNormal	= 1,
	TEXT_e_ExpAngry		= 2,
	TEXT_e_ExpFighting	= 3,
	TEXT_e_ExpWorried	= 4,
	TEXT_e_ExpHappy		= 5,
	TEXT_e_ExpSurprised = 6,
	TEXT_e_ExpRelieved	= 7
} TEXT_tden_ExpFacial;

typedef enum	TEXT_tden_Lips_ 
{ 
	TEXT_e_LipsWhispered = 1, 
	TEXT_e_LipsNormal = 2, 
	TEXT_e_LipsShouted = 3,
	TEXT_e_LipsScreamed = 4
} TEXT_tden_Lips;

typedef enum	TEXT_tden_Priority_
{
	TEXT_e_PrioInvalid	= -1,
	TEXT_e_PrioUltraHigh= 10,
	TEXT_e_PrioVeryHigh = 20,
	TEXT_e_PrioHigh		= 30,
	TEXT_e_PrioNormal	= 40,
	TEXT_e_PrioLow		= 50,
	TEXT_e_PrioVeryLow	= 60,
	TEXT_e_PrioUltraLow = 70
} TEXT_tden_Priority;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Value for a text
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TEXT_tdst_Eval_
{
	int i_FileKey;
	int i_Id;
} TEXT_tdst_Eval;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Structure to define an id
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TEXT_tdst_Id_
{
	ULONG			ul_IdKey;
	ULONG			ul_SoundKey;
	ULONG			ul_ObjKey;
	int				i_Offset;
	unsigned short	us_Version;
	unsigned short	us_Priority;
	char			c_FacialIdx;
	char			c_LipsIdx;
	char			c_AnimIdx;
	char			c_DumIdx;
#ifdef ACTIVE_EDITORS
	char			asz_Name[TEXT_MaxLenId];
	char			*asz_Comment;
	char			asz_LabelAuto[TEXT_MaxLenId];
	int				top, bottom, order;
	char			sel;
	char			searchres;
	char			*asz_4Sort;
	int				i_CommentOffset[100];
#endif
	float			f_LastTime;
	void			*pv_Obj;
} TEXT_tdst_Id;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Structure for one text
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TEXT_tdst_OneText_
{
	BIG_KEY			ul_Key;			/* Key (txl) */
	ULONG			ul_Num;			/* Number of texts */
	TEXT_tdst_Id	*pst_Ids;		/* Array of all strings depending on ids */
	char			*psz_Text;		/* Buffer that contains all texts */
	ULONG			ul_Inst;		/* Number of references */
	BIG_KEY			ul_FatherKey;	/* Key (txg) */
} TEXT_tdst_OneText;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Main structure for texts
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TEXT_tdst_AllText_
{
	int					i_CurrentLanguage;		/* Current language */
	int					i_CurrentSndLanguage;	/* Current language */
	int					i_Num;					/* Number of texts */
	TEXT_tdst_OneText	**pst_AllTexts;			/* All loaded texts (language file) */
} TEXT_tdst_AllText;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
