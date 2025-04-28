/*$T SNDinsert.h GC 1.138 11/10/04 11:38:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDinsert_h__
#define __SNDinsert_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined(PSX2_TARGET) || defined(_GAMECUBE)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef enum	SND_tden_InsRefType
{
	en_InsXaxisDistance = 0,
	en_InsXaxisTime		= 1,
	en_InsXaxisVar		= 2,
	en_InsYaxisVol		= 3,
	en_InsYaxisPan		= 4,
	en_InsYaxisFreq		= 5,
	en_InsKeyArray		= 6,
	en_InsDummy			= 0xFFFFFFFF
} SND_tden_InsRefType;

typedef struct	SND_tdst_InsKey_
{
	float	tx, ty;
} SND_tdst_InsKey;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    file
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	SND_tdst_InsertFileHeader_
{
	unsigned int	ui_FileId;
	unsigned int	ui_Version;
	unsigned int	ui_DataSize;
} SND_tdst_InsertFileHeader;

typedef struct	SND_tdst_InsertChunkHeader_
{
	SND_tden_InsRefType e_ChunkId;
	unsigned int		ui_ChunkSize;
} SND_tdst_InsertChunkHeader;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    X axis
 -----------------------------------------------------------------------------------------------------------------------
 */

/* ui_Axis */
#define SND_Cte_InsAxisX	0x00000001
#define SND_Cte_InsAxisY	0x00000002
#define SND_Cte_InsAxisZ	0x00000004
#define SND_Cte_InsAxisXYZ	0x00000008

typedef struct	SND_tdst_InsXDistanceDef_
{
	SND_tdst_InsertChunkHeader	st_Header;
	unsigned int				ui_GaoKey;
	unsigned int				ui_Axis;
	float						f_Min;
	float						f_Max;
} SND_tdst_InsXDistanceDef;

typedef struct	SND_tdst_InsXTimeDef_
{
	SND_tdst_InsertChunkHeader	st_Header;
	float						f_Min;
	float						f_Max;
} SND_tdst_InsXTimeDef;

typedef struct	SND_tdst_InsXVarDef_
{
	SND_tdst_InsertChunkHeader	st_Header;
	unsigned int				ui_VarId;
} SND_tdst_InsXVarDef;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Y axis
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	SND_tdst_InsYVolDef_
{
	SND_tdst_InsertChunkHeader	st_Header;
	float						f_Min;
	float						f_Max;
	BOOL						b_Wet;
} SND_tdst_InsYVolDef;

typedef struct	SND_tdst_InsYPanDef_
{
	SND_tdst_InsertChunkHeader	st_Header;
	float						f_Min;
	float						f_Max;
	BOOL						b_Surround;
} SND_tdst_InsYPanDef;

typedef struct	SND_tdst_InsYFreqDef_
{
	SND_tdst_InsertChunkHeader	st_Header;
	float						f_Min;
	float						f_Max;
} SND_tdst_InsYFreqDef;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	SND_tdst_InsKeyArray_
{
	SND_tdst_InsertChunkHeader	st_Header;
	unsigned int				ui_Size;
	SND_tdst_InsKey				*dst_Key;
} SND_tdst_InsKeyArray;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

/* ul_Flags */
#define SND_Cte_InsFade			0x00000001
#define SND_Cte_InsCycling		0x00000002
#define SND_Cte_InsActiveOnPlay 0x00000004
#define SND_Cte_InsReserved0	0x00000400	/* SND_Cul_DSF_Used */

typedef struct	SND_tdst_Insert_
{
	ULONG	ul_Flags;
	ULONG	ul_UserCount;
	ULONG	ul_FileKey;

	union insYdef
	{
		SND_tdst_InsYVolDef			*pst_VolDef;
		SND_tdst_InsYPanDef			*pst_PanDef;
		SND_tdst_InsYFreqDef		*pst_FreqDef;
		SND_tdst_InsertChunkHeader	*pst_InsVoid;
	} Yaxis;
	union insXdef
	{
		SND_tdst_InsXDistanceDef	*pst_DistDef;
		SND_tdst_InsXTimeDef		*pst_TimeDef;
		SND_tdst_InsXVarDef			*pst_VarDef;
		SND_tdst_InsertChunkHeader	*pst_InsVoid;
	} Xaxis;
	SND_tdst_InsKeyArray	*pst_KeyArray;
} SND_tdst_Insert;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined(PSX2_TARGET) || defined(_GAMECUBE)
#pragma pack(0)
#else
#pragma pack(pop)
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

void			SND_InsertInitModule(void);
void			SND_InsertCloseModule(void);

void			SND_InsertVarSet(int p_Gao, int i_Idx, float f_Value);
float			SND_f_InsertVarGet(int p_Gao, int i_Idx);
void			SND_InsertDestroyInstanceCallback(int i_Instance);
void			SND_InsertVarFreeAll(void);

float			SND_f_InsertGetYvsX(SND_tdst_Insert *pInsert, float fX, int iInstance);
float			SND_f_InsertGetXvsY(SND_tdst_Insert *pInsert, float fX);
float			SND_f_InsertGetYmin(SND_tdst_Insert *pInsert);
float			SND_f_InsertGetYmax(SND_tdst_Insert *pInsert);
float			SND_f_InsertGetXmin(SND_tdst_Insert *pInsert);
float			SND_f_InsertGetXmax(SND_tdst_Insert *pInsert);

ULONG			SND_ul_CallbackInsertLoad(ULONG _ul_FilePos);
void			SND_InsertUnload(int l_Index);
int				SND_i_InsertGetIndex(ULONG);
void			SND_InsertChangeType(SND_tdst_Insert *pInsert, SND_tden_InsRefType eType);

#ifdef ACTIVE_EDITORS
void			SND_InsertSave(SND_tdst_Insert *pst_Insert, BIG_KEY _ul_Key);
SND_tdst_Insert *SND_p_InsertCreate
				(
					ULONG				ulFileKey,
					SND_tden_InsRefType eY,
					SND_tden_InsRefType eX,
					unsigned int		uiKeyNb
				);
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
