/*$T GFXsmoke.h GC! 1.081 09/21/00 11:59:44 */
/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXExplode_H__
#define __GFXExplode_H__
#include "BASe/BAStypes.h"
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	GFX_tdst_Explode_
{
	SOFT_tdst_AVertex		Wind_0 				ONLY_PSX2_ALIGNED(16); // W is frictionG
	SOFT_tdst_AVertex		FrictionSpeed_FG 	ONLY_PSX2_ALIGNED(16); // W is frictionG
	ULONG					ulNumberOfSprites;
	
	SOFT_tdst_AVertex		*p_Pos___SZ;
	SOFT_tdst_AVertex		*p_Speed_GS;
	ULONG					*p_Colors;
	float					*p_TimeElapsed;
	

	MATH_tdst_Vector		CreationPos;
	MATH_tdst_Vector		MainPosSpeed;
	MATH_tdst_Vector		MainPosFriction;

	MATH_tdst_Vector		CreationPosVolumeX;
	MATH_tdst_Vector		CreationPosVolumeY;
	MATH_tdst_Vector		CreationPosVolumeZ;

	
	float					Gravity;
	float					fGround;

	float					GrowingSpeedMin;
	float					GrowingSpeedMax;

	float					NormSpeedMin;
	float					NormSpeedMax;

	MATH_tdst_Vector		SpeedMin;
	MATH_tdst_Vector		SpeedMax;

	float					CreationSizeMin;
	float					CreationSizeMax;

//	float					FrictionGrow;

	float					TimeInFase1;
	float					TimeInFase2;
	float					OoTimeInFase1;
	float					OoTimeInFase2;
	float					TimeRandomFactor;


	ULONG					SubMaterial1;
	ULONG					SubMaterial2;
	ULONG					SubMaterial3;
	ULONG					SubMaterial4;

	ULONG					ColorFase1;
	ULONG					ColorFase2;
	ULONG					ColorFase3;

	ULONG					ulNumberToGenerate;
	ULONG					ulNumberOfValidSprites;
	float					fGenerationRate;

	float					fLastTimeCreation;
	float					fTimeElapsed;

	ULONG					ulSortEnable;
	
	
	ULONG 					ulColorTable_012[512];
	
	
	GEO_tdst_Object					stObjectForEffects;
	OBJ_tdst_GameObject				*p_Owner;
	GEO_tdst_ElementIndexedSprite	stSpritesElements[4];
	GEO_tdst_IndexedSprite			stISpr[8];

	ULONG                   ulRotationEnable;
    USHORT                  *p_usRotation;
    float                   *p_fAngularSpeed;

    float                   fMinRotation;
    float                   fMaxRotation;
    float                   fAngularSpeedMin;
    float                   fAngularSpeedMax; 


	ULONG					ZOrder;

} GFX_tdst_Explode;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Explode_Create(void);
void    GFX_Explode_Destroy( void *);
int     GFX_i_Explode_Render(void *,GRO_tdst_Struct *);

void    GFX_Explode_Seti( void *, int, int );
int     GFX_i_Explode_Geti( void *, int );

void    GFX_Explode_Setf( void *, int, float );

void    GFX_Explode_Setv( void *, int, MATH_tdst_Vector * );


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXExplode_H__ */
