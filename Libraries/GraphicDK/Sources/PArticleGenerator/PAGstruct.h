/*$T PAGstruct.h GC! 1.081 06/28/00 15:52:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __PAGSTRUCT_H__
#define __PAGSTRUCT_H__

#include "BASe/BAStypes.h"

#include "GRObject/GROstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define PAG_GenType_Point			0
#define PAG_GenType_Rectangle		1
#define PAG_GenType_Circle			2
#define PAG_GenType_Cylindre		3
#define PAG_GenType_Sphere			4
#define PAG_GenType_Box				5
#define PAG_GenType_ObjectVertex    6

#define PAG_SpeedType_1D			0
#define PAG_SpeedType_2D			1
#define PAG_SpeedType_3D			2
#define PAG_SpeedType_FromCenter	3
#define PAG_SpeedType_ObjectNormal  4
#define PAG_SpeedType_Target		5

#define PAG_Flags_Transparent		        0x00000001
#define PAG_Flags_RGBEqualA			        0x00000002
#define PAG_Flags_YEqualX			        0x00000004
#define PAG_Flags_AlphaDeath		        0x00000008
#define PAG_Flags_DecreaseDeath		        0x00000010
#define PAG_Flags_IncreaseDeath		        0x00000020
#define PAG_Flags_Active			        0x00000040
#define PAG_Flags_Freeze			        0x00000080
#define PAG_Flags_AlphaBirth		        0x00000100
#define PAG_Flags_IncreaseBirth		        0x00000200
#define PAG_Flags_UseZMin			        0x00000400
#define PAG_Flags_UseZMax			        0x00000800
#define PAG_Flags_UseRotation		        0x00001000
#define PAG_Flags_UseFriction		        0x00002000
#define PAG_Flags_NoParticleDesctruction    0x00004000
#define PAG_Flags_AccSpeed					0x00008000
#define PAG_Flags_DontSortParticles			0x00010000

/* params constant for ia fonction that set or get particle generator parameter */
#define PAG_Paramf_Offset               0
#define PAG_Paramf_Gen0                 1
#define PAG_Paramf_Gen1                 2
#define PAG_Paramf_Gen2                 3
#define PAG_Paramf_Speed0               4
#define PAG_Paramf_Speed1               5
#define PAG_Paramf_Angle1               6
#define PAG_Paramf_Angle2               7
#define PAG_Paramf_SizeXMin             8
#define PAG_Paramf_SizeXMax             9
#define PAG_Paramf_SizeYMin             10
#define PAG_Paramf_SizeYMax             11
#define PAG_Paramf_TimeMin              12
#define PAG_Paramf_TimeMax              13
#define PAG_Paramf_TimeDeathMin         14
#define PAG_Paramf_TimeDeathMax         15
#define PAG_Paramf_TimeBirthMin         16
#define PAG_Paramf_TimeBirthMax         17
#define PAG_Paramf_SizeDeathFactor      18
#define PAG_Paramf_AccX                 19
#define PAG_Paramf_AccY                 20
#define PAG_Paramf_AccZ                 21
#define PAG_Paramf_Friction             22
#define PAG_Paramf_ZMin                 23
#define PAG_Paramf_ZMinStrength         24
#define PAG_Paramf_ZMax                 25
#define PAG_Paramf_ZMaxStrength         26
#define PAG_Paramf_RotationMin          27
#define PAG_Paramf_RotationMax          28
#define PAG_Paramf_RotationSpeedMin     29
#define PAG_Paramf_RotationSpeedMax     30
#define PAG_Paramf_SinX					32
#define PAG_Paramf_SinY					33
#define PAG_Paramf_DistConstraint		34

#define PAG_Paramf_Number               35

/* flags for particles */
#define PAG_PFlags_Birth                0x01
#define PAG_PFlags_Death                0x02

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	PAG_tdst_P_
{
    ULONG               ul_Flags;
	MATH_tdst_Vector	st_Pos;
	MATH_tdst_Vector	st_Speed;
	float				f_TimeFactor, f_TimeLeft, f_TimeCur;
	float				f_SizeX, f_SizeY;
	ULONG				ul_Rotation;
} PAG_tdst_P;

typedef struct	PAG_tdst_Generator_
{
	int		i_Id;
	void	*p_GO;
	float	f_NbPerSecond;
	float	f_NbPerSecondInit;
	float	f_GenTime;
} PAG_tdst_Generator;

typedef struct	PAG_tdst_Struct_
{
	GRO_tdst_Struct		st_Id;
	ULONG				ul_RenderingCounter;
    float               f_TimeLeft;

	ULONG				ul_Flags;
	char				c_GenType;
	char				c_SpeedType;
	short				w_NbGenerator;

    struct OBJ_tdst_GameObject_ *pst_GO;
	struct OBJ_tdst_GameObject_ *pst_GOMatrixRef;

	PAG_tdst_Generator	ast_Gen[16];
	//LONG				l_NbGenerator;

    /* keep all float together  and in order of PAG_Paramf_XXX const defined behind */
    float				f_GenOffset;
	float				f_GenParam[3];

	float				f_Speed0;
	float				f_Speed1;
	float				f_Angle1;
	float				f_Angle2;

	float				f_SizeXMin;
	float				f_SizeXMax;
	float				f_SizeYMin;
	float				f_SizeYMax;

	float				f_TimeMin;
	float				f_TimeMax;
	float				f_TimeDeathMin;
	float				f_TimeDeathMax;
	float				f_TimeBirthMin;
	float				f_TimeBirthMax;
	float				f_SizeDeathFactor;

	MATH_tdst_Vector	st_Acc;
	float				f_Friction;

	float				f_ZMin;
	float				f_ZMinStrength;
	float				f_ZMax;
	float				f_ZMaxStrength;

	float				f_RotationMin;
	float				f_RotationMax;
	float				f_RotationSpeedMin;
	float				f_RotationSpeedMax;

	float				f_SinXFactor;
	float				f_SinYFactor;
	float				f_DistConstraint;

	ULONG				ul_Color;

	LONG				l_NbMaxP;
	LONG				l_NbP;
	PAG_tdst_P			*dst_P;

    int                 i_NbAttractor;
    MATH_tdst_Vector    st_ACenter[4];
    float               f_ADist1[4];
    float               f_ADist2[4];
    float               f_AStrength[4];
    float               f_AFriction[4];

} PAG_tdst_Struct;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void			PAG_Init(void);
void			PAG_Close(void);
PAG_tdst_Struct *PAG_pst_Create(void);
void			PAG_Free(PAG_tdst_Struct *);

void			PAG_Update(OBJ_tdst_GameObject *, PAG_tdst_Struct *, float);
void			PAG_Generate(OBJ_tdst_GameObject *, PAG_tdst_Struct *, float);
PAG_tdst_P      *PAG_AddOneParticle( OBJ_tdst_GameObject * );

PAG_tdst_Struct *PAG_pst_GetGameObjectParticleGenerator(struct OBJ_tdst_GameObject_ *);
int				PAG_i_AddGenerator(PAG_tdst_Struct *, void *, float);
void			PAG_DelGenerator(PAG_tdst_Struct *, int);
void			PAG_SetGeneratorFrequency(PAG_tdst_Struct *, int, float);
float			PAG_GetGeneratorFrequency(PAG_tdst_Struct *, int);
void			PAG_RenderCommon(OBJ_tdst_GameObject *, PAG_tdst_Struct *, char);
void			PAG_Reinit(GRO_tdst_Struct *);
void			*PAG_p_CreateFromBuffer(struct GRO_tdst_Struct_ *, char **, struct WOR_tdst_World_ *);
LONG			PAG_l_SaveInBuffer(PAG_tdst_Struct *, void *);
LONG			PAG_l_SaveInBufferCommon(PAG_tdst_Struct *, void *);
void			*PAG_p_Duplicate(PAG_tdst_Struct *, char *, char *, ULONG);

#ifdef ACTIVE_EDITORS
void			PAG_ChangeNumberOfParticles(PAG_tdst_Struct *, LONG);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SPLSTRUCT_H */
