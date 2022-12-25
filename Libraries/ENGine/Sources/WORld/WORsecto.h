/*$T WORsecto.h GC 1.134 04/22/04 13:39:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __WORSECTO_H__
#define __WORSECTO_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define SCT_MODE_Cam	0
#define SCT_MODE_Main0	1
#define SCT_MODE_Main1	2
extern ULONG SCT_gul_Flags;

/*$4-******************************************************************************************************************/

#define WOR_C_MaxLenNameSecto		64
#define WOR_C_MaxLenNamePortal		64
#define WOR_C_MaxSecto				64
#define WOR_C_MaxSectoRef			16
#define WOR_C_MaxSectoPortals		16

#define WOR_CF_PortalValid			0x0001
#define WOR_CF_PortalShare			0x0002
#define WOR_CF_PortalRender			0x0010
#define WOR_CF_PortalPickable		0x0020
#define WOR_CF_PortalPicked			0x0040

#define WOR_CF_SectorValid			0x00000001
#define WOR_CF_SectorMerged			0x00000002
#define WOR_CF_SectorColor1			0x00000100
#define WOR_CF_SectorColor2			0x00000200
#define WOR_CF_SectorColor3			0x00000400
#define WOR_CF_SectorActive			0x00010000
#define WOR_CF_SectorVisible		0x00020000
#define WOR_CF_SectorAlwaysVisible	0x01000000
#define WOR_CF_SectorAlwaysActive	0x02000000
#define WOR_CF_SectorNeverVisible	0x04000000
#define WOR_CF_SectorNeverActive	0x08000000

/*$4-******************************************************************************************************************/

typedef struct	WOR_tdst_Portal_
{
#ifdef ACTIVE_EDITORS
	char				az_Name[WOR_C_MaxLenNamePortal];
#endif
	USHORT				uw_Flags;
	UCHAR				uc_ShareSect;
	UCHAR				uc_SharePortal;
	MATH_tdst_Vector	vA;
	MATH_tdst_Vector	vB;
	MATH_tdst_Vector	vC;
	MATH_tdst_Vector	vD;
} WOR_tdst_Portal;

/*$4-******************************************************************************************************************/

typedef struct	WOR_tdst_Secto_
{
#ifdef ACTIVE_EDITORS
	char			az_Name[WOR_C_MaxLenNameSecto];
#endif
	ULONG			ul_Flags;
	UCHAR			auc_RefVis[WOR_C_MaxSectoRef];		/* List of vis sectors */
	UCHAR			auc_RefAct[WOR_C_MaxSectoRef];		/* List of act sectors */
	WOR_tdst_Portal ast_Portals[WOR_C_MaxSectoPortals]; /* List of portals */
} WOR_tdst_Secto;

/*$4-******************************************************************************************************************/


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$4-******************************************************************************************************************/

#define PIT_C_NoRead		0x00000001	/* Lu par utilisateur courant */
#define PIT_C_ForMe			0x00000002	/* Destiné a moi */
#define PIT_C_Draw			0x00000004	/* Affichage 3D */
#define PIT_C_NoReadOnly	0x00000008	/* N'importe qui peut le modifier */

/*$4-******************************************************************************************************************/

typedef struct	F3D_tdst_PostIt_
{
	ULONG				ul_Flags;
	MATH_tdst_Vector	st_Pos;
	char				az_Owner[64];
	char				az_To[1024];
	char				az_Read[1024];
	char				az_Note[1024];
	char				az_Name[128];
	ULONG				aul_DateCreate[2];
} F3D_tdst_PostIt;

/*$4-******************************************************************************************************************/

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WORINIT_H__ */
