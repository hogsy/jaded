 /*$T MDFmodifier_GEO.h GC! 1.081 11/14/00 12:12:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_PROTEX_H__
#define __MDFMODIFIER_PROTEX_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Modifier const
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */

#define PRTX_ClipDynamicsOutsideSquare	1
#define PRTX_ClipWithNotDynamic			2
#define PRTX_PerturbWithDynamics		4
#define PRTX_HasBeenUsedInFrame			8
#define PRTX_LineHasBeenUsed			16
#define PRTX_IsInDrawList				32
#define	PRTX_TextureIDValid				64
typedef struct	PROTEX_tdst_Modifier_
{
#ifdef ACTIVE_EDITORS
	ULONG				ulCodeKey;
	ULONG				ulMaxNumberOfLines;
	ULONG				ulNumberOfLines;
	ULONG				*p_ulLines;
#endif
	/* Loaded datas */
	ULONG						ulSubMatSourceNum;
	ULONG						ulSubMatMuTexSourceNum;
	ULONG						ulNumberOfHLines;
	ULONG						ulNumberOfDots;
	UCHAR						*P_AllDotNumbers;	// Size = Texture Height (128 or 256)
	UCHAR						*p_AllDot;			// Size = Sum(*P_AllDotNumbers[Texture Height])

	
	/* Retreived datas */
	ULONG				ulFlags;
		MATH_tdst_Vector	stTextureCenter;
	MATH_tdst_Vector	stUVector;
	MATH_tdst_Vector	stVVector;
	ULONG				W,H,BPP;
	ULONG				TextureID;
	ULONG				TextureRawPalID;
	void				*p_Surface0;
	void				*p_Surface1;
	void				*p_SurfaceTexture;
	struct OBJ_tdst_GameObject_ *p_GO;
	struct	PROTEX_tdst_Modifier_ *p_NextAcitveProtex;
#ifndef PSX2_TARGET
	ULONG				*p_ColorBuffer;
#endif


} PROTEX_tdst_Modifier;

extern PROTEX_tdst_Modifier *p_FirstActiveProtex;
/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void							PROTEX_Modifier_Create
									(
										struct OBJ_tdst_GameObject_ *,
										MDF_tdst_Modifier *,
										void *
									);
extern void							PROTEX_Modifier_Destroy(MDF_tdst_Modifier *);
extern void							PROTEX_Modifier_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							PROTEX_Modifier_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						PROTEX_ul_Modifier_Load(MDF_tdst_Modifier *, char *);
extern void							PROTEX_Modifier_Save(MDF_tdst_Modifier *);
extern void							PROTEX_Modifier_Reinit(MDF_tdst_Modifier *);

extern void							PROTEX_Disturb(struct OBJ_tdst_GameObject_ *pCurrentGameObject , struct GEO_tdst_Object_ *p_CurrentGEOObject);
_inline_ void PROTEX_UpdateEarthWindAndFire(struct OBJ_tdst_GameObject_ *pCurrentGameObject , struct GEO_tdst_Object_ *p_CurrentGEOObject)
{
	if (!p_FirstActiveProtex) return;
	PROTEX_Disturb(pCurrentGameObject,p_CurrentGEOObject);
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEOMODIFIER_H */
