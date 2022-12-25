

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "MDFmodifier_GEO.h"
#include "MDFmodifier_GAO.h"
#include "MDFmodifier_SDW.h"
#include "MDFmodifier_GEN.h"
#include "MDFmodifier_SOUNDFX.h"
#include "MDFmodifier_XMEN.h"
#include "MDFmodifier_XMEC.h"
#include "MDFmodifier_ROTR.h"
#include "MDFmodifier_SPG.h"
#include "MDFmodifier_SNAKE.h"
#include "MDFmodifier_PROTEX.h"
#include "MDFmodifier_MPAG.h"
#include "MDFmodifier_SOUNDLOADING.h"
#include "MDFmodifier_XMEC.h"
#include "MDFmodifier_LAZY.h"
#include "MDFmodifier_GPG.h"
#include "MDFmodifier_FUR.h"
#include "MDFmodifier_SPG2.h"
#include "MDFmodifier_ODE.h"
#include "MDFmodifier_MatrixBore.h"
#include "MDFmodifier_GRID.h"
#include "MDFmodifier_SNDVOL.h"
#include "MDFmodifier_BrumeDynamique.h"
#include "MDFmodifier_Vine.h"
#include "MDFmodifier_FCLONE.h"
#include "MDFmodifier_UVTexWave.h"

#include "GraphicDK/Sources/GEOmetric/GEOobject.h"
#include "MoDiFier/MDFstruct.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"

#include "BASe/BENch/BENch.h"

#ifdef JADEFUSION
#include "MDFmodifier_Water3D.h"
#include "MDFmodifier_Disturber.h"
#include "MDFmodifier_SFX.h"
#include "MDFmodifier_Weather.h"
#include "MDFmodifier_SoftBody.h"
#include "MDFmodifier_SPG2_Holder.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_Wind.h"
#endif

#ifdef _GAMECUBE
#ifndef _FINAL_
#include "GXI_GC/GXI_dbg.h"
#endif
#endif
extern void PROTEX_Modifier_Reinit(MDF_tdst_Modifier *);

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Modifier globals
 ***********************************************************************************************************************
 */

MDF_tdst_ModifierInterface	MDF_gast_ModifierInterface[MDF_C_Modifier_Number];
MDF_tdst_Modifier			*MDF_gpst_GlobalCreate = NULL;
/*$off*/
#ifdef ACTIVE_EDITORS
char *MDF_gasz_ModifierName[MDF_C_Modifier_Number] =
{
	"Snap",
	"WaVe YoUr BoDy",
	"Explode",
	"LegLink",
	"Morphing",
	"Look-At Camera (but keep straight)",
	"Shadow",
	"Special Look-At Camera ",
	"SOUND",
	"X-MEN",
	"X-MEC",
	"Subdivision Sprite Generator",
	"Symetrie",
	"Rotation Retard (BONES ONLY)",
	"Snake (BONES ONLY)",
	"SOUND FX",
	"Earth, Wind & Fire",
	"Saving Add Matrix",
	"Particule Generator",
	"SOUND DURING LOADING",
	"Info Photo",
	"Store Transformed Points",
	"Crush Points",
	"RLI Carte" ,
	"Lazy",
	"Geometry Generator",
	"Fur Setting",
	"Vertex perturb",
	"Sprite mapper 2",
	"ODE Joint",
	"Matrix bore",
	"Copy/Paste Grid",
    "SOUND VOLUME",
	"Water3D",
    "Disturber",
    "Special Effect",
    "Rotation paste",
    "Translation paste",
    "Animated GAO",
    "WeatherFX",
    "Soft Body",
    "Wind",
    "Dynamic Fur",
    "SPG2Holder",
    "FREE", // reserve XENON (do not use)
    "FREE", // reserve XENON (do not use)
    "FREE", // reserve XENON (do not use)
    "FREE", // reserve XENON (do not use)
    "FREE", // reserve XENON (do not use)
    "Vine",
    "Dynamic fog",
    "Dynamic fog spherre emiter",
    "Bone refine",
    "Bone meca",
	"Clone Object (!! Ne pas toucher !!)",
	"Texture wave",
};
#endif /* ACTIVE_EDITORS */
/*$on */

/*$4
 ***********************************************************************************************************************
    General
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Modifier_Init(void)
{
	L_memset(MDF_gast_ModifierInterface, 0, sizeof(MDF_gast_ModifierInterface));

#ifdef USE_DOUBLE_RENDERING
	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].pfnv_Interpolate = GEO_ModifierMorphing_Interpolate;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].pfnv_Interpolate = GAO_ModifierXMEN_Interpolate;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].pfnv_Interpolate = SPG_Modifier_Interpolate;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].pfnv_Interpolate = GAO_ModifierLazy_Interpolate;
#endif

	/*$1- MDF_C_Modifier_Snap ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_Snap].ul_Type = MDF_C_Modifier_Snap;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Snap].pfnv_Create = GEO_ModifierSnap_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Snap].pfnv_Destroy = GEO_ModifierSnap_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Snap].pfnv_Apply = GEO_ModifierSnap_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Snap].pfnv_Unapply = GEO_ModifierSnap_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Snap].pfnul_Load = GEO_ul_ModifierSnap_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Snap].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_Snap].pfnv_Save = GEO_ModifierSnap_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Snap].pfnb_CanBeApply = GEO_Modifier_CanBeApply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Snap].pfnv_ApplyInGeom = GEO_ModifierSnap_Apply;
#endif

	/*$1- MDF_C_Modifier_OnduleTonCorps ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].ul_Type = MDF_C_Modifier_OnduleTonCorps;
	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].pfnv_Create = GEO_ModifierOnduleTonCorps_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].pfnv_Destroy = GEO_ModifierOnduleTonCorps_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].pfnv_Apply = GEO_ModifierOnduleTonCorps_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].pfnv_Unapply = GEO_ModifierOnduleTonCorps_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].pfnul_Load = GEO_ul_ModifierOnduleTonCorps_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].pfnv_Save = GEO_ModifierOnduleTonCorps_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].pfni_Copy = GEO_ModifierOnduleTonCorps_Copy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].pfnb_CanBeApply = GEO_Modifier_CanBeApply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_OnduleTonCorps].pfnv_ApplyInGeom = GEO_ModifierOnduleTonCorps_Apply;
#endif

	/*$1- MDF_C_Modifier_Explode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_Explode].ul_Type = MDF_C_Modifier_Explode;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Explode].pfnv_Create = GAO_ModifierExplode_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Explode].pfnv_Destroy = GAO_ModifierExplode_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Explode].pfnv_Apply = GAO_ModifierExplode_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Explode].pfnv_Unapply = GAO_ModifierExplode_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Explode].pfnul_Load = GAO_ModifierExplode_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Explode].pfnv_Reinit = GAO_ModifierExplode_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_Explode].pfnv_Save = GAO_ModifierExplode_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Explode].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Explode].pfnv_ApplyInGeom = GAO_ModifierExplode_Apply;
#endif

	/*$1- MDF_C_Modifier_LegLink ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_LegLink].ul_Type = MDF_C_Modifier_LegLink;
	MDF_gast_ModifierInterface[MDF_C_Modifier_LegLink].pfnv_Create = GAO_ModifierLegLink_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_LegLink].pfnv_Destroy = GAO_ModifierLegLink_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_LegLink].pfnv_Apply = GAO_ModifierLegLink_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_LegLink].pfnv_Unapply = GAO_ModifierLegLink_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_LegLink].pfnul_Load = GAO_ModifierLegLink_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_LegLink].pfnv_Reinit = GAO_ModifierLegLink_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_LegLink].pfnv_Save = GAO_ModifierLegLink_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_LegLink].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_LegLink].pfnv_ApplyInGeom = GAO_ModifierLegLink_Apply;
#endif

	/*$1- MDF_C_Modifier_Morphing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].ul_Type = MDF_C_Modifier_Morphing;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].pfnv_Create = GEO_ModifierMorphing_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].pfnv_Destroy = GEO_ModifierMorphing_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].pfnv_Apply = GEO_ModifierMorphing_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].pfnv_Unapply = GEO_ModifierMorphing_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].pfnul_Load = GEO_ul_ModifierMorphing_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].pfnv_Save = GEO_ModifierMorphing_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].pfnb_CanBeApply = GEO_Modifier_CanBeApply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Morphing].pfnv_ApplyInGeom = GEO_ModifierMorphing_Apply;
#endif

	/*$1- MDF_C_Modifier_SemiLookAt ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_SemiLookAt].ul_Type = MDF_C_Modifier_SemiLookAt;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SemiLookAt].pfnv_Create = GAO_ModifierSemiLookAt_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SemiLookAt].pfnv_Destroy = GAO_ModifierSemiLookAt_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SemiLookAt].pfnv_Apply = GAO_ModifierSemiLookAt_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SemiLookAt].pfnv_Unapply = GAO_ModifierSemiLookAt_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SemiLookAt].pfnul_Load = GAO_ModifierSemiLookAt_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SemiLookAt].pfnv_Reinit = GAO_ModifierSemiLookAt_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_SemiLookAt].pfnv_Save = GAO_ModifierSemiLookAt_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SemiLookAt].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SemiLookAt].pfnv_ApplyInGeom = GAO_ModifierSemiLookAt_Apply;
#endif
	

	/*$1- MDF_C_Modifier_Shadow ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_Shadow].ul_Type = MDF_C_Modifier_Shadow;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Shadow].pfnv_Create = GAO_ModifierShadow_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Shadow].pfnv_Destroy = GAO_ModifierShadow_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Shadow].pfnv_Apply = GAO_ModifierShadow_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Shadow].pfnv_Unapply = GAO_ModifierShadow_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Shadow].pfnul_Load = GAO_ModifierShadow_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Shadow].pfnv_Reinit = GAO_ModifierShadow_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_Shadow].pfnv_Save = GAO_ModifierShadow_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Shadow].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Shadow].pfnv_ApplyInGeom = GAO_ModifierShadow_Apply;
#endif

	/*$1- MDF_C_Modifier_SpecialLookAt ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_SpecialLookAt].ul_Type = MDF_C_Modifier_SpecialLookAt;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpecialLookAt].pfnv_Create = GAO_ModifierSpecialLookAt_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpecialLookAt].pfnv_Destroy = GAO_ModifierSpecialLookAt_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpecialLookAt].pfnv_Apply = GAO_ModifierSpecialLookAt_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpecialLookAt].pfnv_Unapply = GAO_ModifierSpecialLookAt_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpecialLookAt].pfnul_Load = GAO_ModifierSpecialLookAt_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpecialLookAt].pfnv_Reinit = GAO_ModifierSpecialLookAt_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpecialLookAt].pfnv_Save = GAO_ModifierSpecialLookAt_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpecialLookAt].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpecialLookAt].pfnv_ApplyInGeom = GAO_ModifierSpecialLookAt_Apply;
#endif

	/*$1- MDF_C_Modifier_Sound ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].ul_Type = MDF_C_Modifier_Sound;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].pfnv_Create = GEN_ModifierSound_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].pfnv_Destroy = GEN_ModifierSound_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].pfnv_Apply = GEN_ModifierSound_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].pfnv_Unapply = GEN_ModifierSound_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].pfnul_Load = GEN_ModifierSound_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].pfnv_Reinit = GEN_ModifierSound_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].pfnv_Save = GEN_ModifierSound_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].pfnv_ApplyInGeom = NULL;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Sound].pfni_Copy = GEN_ModifierSound_Copy;
#endif

	/*$1- MDF_C_Modifier_XMEN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].ul_Type = MDF_C_Modifier_XMEN;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].pfnv_Create = GAO_ModifierXMEN_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].pfnv_Destroy = GAO_ModifierXMEN_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].pfnv_Apply = GAO_ModifierXMEN_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].pfnv_Unapply = GAO_ModifierXMEN_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].pfnul_Load = GAO_ModifierXMEN_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].pfnv_Reinit = GAO_ModifierXMEN_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].pfnv_Save = GAO_ModifierXMEN_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEN].pfnv_ApplyInGeom = GAO_ModifierXMEN_Apply;
#endif

	/*$1- MDF_C_Modifier_XMEC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEC].ul_Type = MDF_C_Modifier_XMEC;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEC].pfnv_Create = GAO_ModifierXMEC_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEC].pfnv_Destroy = GAO_ModifierXMEC_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEC].pfnv_Apply = GAO_ModifierXMEC_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEC].pfnv_Unapply = GAO_ModifierXMEC_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEC].pfnul_Load = GAO_ModifierXMEC_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEC].pfnv_Reinit = GAO_ModifierXMEC_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEC].pfnv_Save = GAO_ModifierXMEC_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEC].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_XMEC].pfnv_ApplyInGeom = GAO_ModifierXMEC_Apply;
#endif

	/*$1- MDF_C_Modifier_SPG ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].ul_Type = MDF_C_Modifier_SPG;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].pfnv_Create = SPG_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].pfnv_Destroy = SPG_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].pfnv_Apply = SPG_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].pfnv_Unapply = SPG_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].pfnul_Load = SPG_ul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].pfnv_Save = SPG_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].pfnb_CanBeApply = GEO_Modifier_CanBeApply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SPG].pfnv_ApplyInGeom = SPG_Modifier_Apply;
#endif

	/*$1- MDF_C_Modifier_Symetrie ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_Symetrie].ul_Type = MDF_C_Modifier_Symetrie;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Symetrie].pfnv_Create = GEO_ModifierSymetrie_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Symetrie].pfnv_Destroy = GEO_ModifierSymetrie_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Symetrie].pfnv_Apply = GEO_ModifierSymetrie_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Symetrie].pfnv_Unapply = GEO_ModifierSymetrie_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Symetrie].pfnul_Load = GEO_ul_ModifierSymetrie_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Symetrie].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_Symetrie].pfnv_Save = GEO_ModifierSymetrie_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Symetrie].pfnb_CanBeApply = GEO_Modifier_CanBeApply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Symetrie].pfnv_ApplyInGeom = GEO_ModifierSymetrie_Apply;
#endif

	/*$1- MDF_C_Modifier_ROTR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_ROTR].ul_Type = MDF_C_Modifier_ROTR;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ROTR].pfnv_Create = GAO_ModifierROTR_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ROTR].pfnv_Destroy = GAO_ModifierROTR_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ROTR].pfnv_Apply = GAO_ModifierROTR_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ROTR].pfnv_Unapply = GAO_ModifierROTR_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ROTR].pfnul_Load = GAO_ModifierROTR_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ROTR].pfnv_Reinit = GAO_ModifierROTR_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_ROTR].pfnv_Save = GAO_ModifierROTR_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ROTR].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ROTR].pfnv_ApplyInGeom = GAO_ModifierROTR_Apply;
#endif

	/*$1- MDF_C_Modifier_SNAKE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_SNAKE].ul_Type = MDF_C_Modifier_SNAKE;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SNAKE].pfnv_Create = GAO_ModifierSNAKE_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SNAKE].pfnv_Destroy = GAO_ModifierSNAKE_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SNAKE].pfnv_Apply = GAO_ModifierSNAKE_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SNAKE].pfnv_Unapply = GAO_ModifierSNAKE_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SNAKE].pfnul_Load = GAO_ModifierSNAKE_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SNAKE].pfnv_Reinit = GAO_ModifierSNAKE_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_SNAKE].pfnv_Save = GAO_ModifierSNAKE_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SNAKE].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SNAKE].pfnv_ApplyInGeom = GAO_ModifierSNAKE_Apply;
#endif

	/*$1- MDF_C_Modifier_SoundFx ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].ul_Type = MDF_C_Modifier_SoundFx;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].pfnv_Create = GEN_ModifierSoundFx_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].pfnv_Destroy = GEN_ModifierSoundFx_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].pfnv_Apply = GEN_ModifierSoundFx_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].pfnv_Unapply = GEN_ModifierSoundFx_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].pfnul_Load = GEN_ModifierSoundFx_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].pfnv_Reinit = GEN_ModifierSoundFx_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].pfnv_Save = GEN_ModifierSoundFx_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].pfnv_ApplyInGeom = NULL;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoundFx].pfni_Copy = GEN_ModifierSoundFx_Copy;
#endif

	/*$1- MDF_C_Modifier_PROTEX ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_PROTEX].ul_Type = MDF_C_Modifier_PROTEX;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PROTEX].pfnv_Create = PROTEX_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PROTEX].pfnv_Destroy = PROTEX_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PROTEX].pfnv_Apply = PROTEX_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PROTEX].pfnv_Unapply = PROTEX_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PROTEX].pfnul_Load = PROTEX_ul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PROTEX].pfnv_Reinit = PROTEX_Modifier_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_PROTEX].pfnv_Save = PROTEX_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PROTEX].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PROTEX].pfnv_ApplyInGeom = NULL;
#endif

	/*$1- MDF_C_Modifier_SaveAddMatrix ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].ul_Type = MDF_C_Modifier_SaveAddMatrix;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnv_Create = GAO_ModifierSaveAddMatrix_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnv_Destroy = GAO_ModifierSaveAddMatrix_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnul_Load = GAO_ul_ModifierSaveAddMatrix_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnv_Save = GAO_ModifierSaveAddMatrix_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnv_ApplyInGeom = NULL;
#else
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnv_Create = NULL;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnv_Destroy = NULL;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnul_Load = NULL;
#endif
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnv_Apply = NULL;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnv_Unapply = NULL;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SaveAddMatrix].pfnv_Reinit = NULL;

	/*$1- MDF_C_Modifier_PAG ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_PAG].ul_Type = MDF_C_Modifier_PAG;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PAG].pfnv_Create = MPAG_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PAG].pfnv_Destroy = MPAG_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PAG].pfnv_Apply = MPAG_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PAG].pfnv_Unapply = MPAG_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PAG].pfnul_Load = MPAG_ul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PAG].pfnv_Reinit = MPAG_ul_Modifier_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_PAG].pfnv_Save = MPAG_ul_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PAG].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_PAG].pfnv_ApplyInGeom = NULL;
#endif

	/*$1- MDF_C_Modifier_SoundLoading ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].ul_Type = MDF_C_Modifier_SoundLoading;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].pfnv_Create = MDF_LoadingSound_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].pfnv_Destroy = MDF_LoadingSound_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].pfnv_Apply = MDF_LoadingSound_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].pfnv_Unapply = MDF_LoadingSound_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].pfnul_Load = MDF_LoadingSound_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].pfnv_Reinit = MDF_LoadingSound_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].pfnv_Save = MDF_LoadingSound_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].pfnv_ApplyInGeom = NULL;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoundLoading].pfni_Copy = MDF_LoadingSound_Copy;
#endif

	/*$1- MDF_C_Modifier_InfoPhoto ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_InfoPhoto].ul_Type = MDF_C_Modifier_InfoPhoto;
	MDF_gast_ModifierInterface[MDF_C_Modifier_InfoPhoto].pfnv_Create = GAO_ModifierPhoto_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_InfoPhoto].pfnv_Destroy = GAO_ModifierPhoto_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_InfoPhoto].pfnv_Apply = GAO_ModifierPhoto_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_InfoPhoto].pfnv_Unapply = GAO_ModifierPhoto_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_InfoPhoto].pfnul_Load = GAO_ModifierPhoto_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_InfoPhoto].pfnv_Reinit = GAO_ModifierPhoto_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_InfoPhoto].pfnv_Save = GAO_ModifierPhoto_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_InfoPhoto].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_InfoPhoto].pfnv_ApplyInGeom = NULL;
#endif

	/*$1- MDF_C_Modifier_StoreTransformedPoints ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_StoreTransformedPoints].ul_Type = MDF_C_Modifier_StoreTransformedPoints;
	MDF_gast_ModifierInterface[MDF_C_Modifier_StoreTransformedPoints].pfnv_Create = GEO_ModifierSTP_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_StoreTransformedPoints].pfnv_Destroy = GEO_ModifierSTP_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_StoreTransformedPoints].pfnv_Apply = GEO_ModifierSTP_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_StoreTransformedPoints].pfnv_Unapply = GEO_ModifierSTP_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_StoreTransformedPoints].pfnul_Load = GEO_ul_ModifierSTP_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_StoreTransformedPoints].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_StoreTransformedPoints].pfnv_Save = GEO_ModifierSTP_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_StoreTransformedPoints].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_StoreTransformedPoints].pfnv_ApplyInGeom = NULL;
#endif

	/*$1- Crush points ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_Crush].ul_Type = MDF_C_Modifier_Crush;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Crush].pfnv_Create = GEO_ModifierCrush_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Crush].pfnv_Destroy = GEO_ModifierCrush_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Crush].pfnv_Apply = GEO_ModifierCrush_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Crush].pfnv_Unapply = GEO_ModifierCrush_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Crush].pfnul_Load = GEO_ul_ModifierCrush_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Crush].pfnv_Reinit = GEO_ModifierCrush_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_Crush].pfnv_Save = GEO_ModifierCrush_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Crush].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Crush].pfnv_ApplyInGeom = NULL;
#endif

	/*$1- RLI Carte ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_RLICarte].ul_Type = MDF_C_Modifier_RLICarte;
	MDF_gast_ModifierInterface[MDF_C_Modifier_RLICarte].pfnv_Create = GEO_ModifierRLICarte_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_RLICarte].pfnv_Destroy = GEO_ModifierRLICarte_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_RLICarte].pfnv_Apply = GEO_ModifierRLICarte_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_RLICarte].pfnv_Unapply = GEO_ModifierRLICarte_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_RLICarte].pfnul_Load = GEO_ul_ModifierRLICarte_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_RLICarte].pfnv_Reinit = GEO_ModifierRLICarte_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_RLICarte].pfnv_Save = GEO_ModifierRLICarte_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_RLICarte].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_RLICarte].pfnv_ApplyInGeom = NULL;
#endif

	/*$1- Lazy ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].ul_Type = MDF_C_Modifier_Lazy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].pfnv_Create = GAO_ModifierLazy_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].pfnv_Destroy = GAO_ModifierLazy_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].pfnv_Apply = GAO_ModifierLazy_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].pfnv_Unapply = GAO_ModifierLazy_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].pfnul_Load = GAO_ul_ModifierLazy_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].pfnv_Reinit = GAO_ModifierLazy_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].pfnv_Save = GAO_ModifierLazy_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Lazy].pfnv_ApplyInGeom = NULL;
#endif

	/*$1- MDF_C_Modifier_GPG ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_GPG].ul_Type = MDF_C_Modifier_GPG;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GPG].pfnv_Create = GPG_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GPG].pfnv_Destroy = GPG_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GPG].pfnv_Apply = GPG_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GPG].pfnv_Unapply = GPG_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GPG].pfnul_Load = GPG_ul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GPG].pfnv_Reinit = GPG_Modifier_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_GPG].pfnv_Save = GPG_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GPG].pfnb_CanBeApply = GEO_Modifier_CanBeApply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GPG].pfnv_ApplyInGeom = GPG_Modifier_Apply;
#endif

	/*$1- MDF_C_Modifier_FUR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_FUR].ul_Type = MDF_C_Modifier_FUR;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FUR].pfnv_Create = FUR_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FUR].pfnv_Destroy = FUR_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FUR].pfnv_Apply = FUR_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FUR].pfnv_Unapply = FUR_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FUR].pfnul_Load = FUR_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FUR].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_FUR].pfnv_Save = FUR_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FUR].pfnb_CanBeApply = FUR_Modifier_CanBeApply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FUR].pfnv_ApplyInGeom = NULL;
#endif


	/*$1- MDF_C_Modifier_VertexFieldOfForce ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_VertexPerturb].ul_Type = MDF_C_Modifier_VertexPerturb;
	MDF_gast_ModifierInterface[MDF_C_Modifier_VertexPerturb].pfnv_Create = GEO_ModifierPerturb_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_VertexPerturb].pfnv_Destroy = GEO_ModifierPerturb_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_VertexPerturb].pfnv_Apply = GEO_ModifierPerturb_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_VertexPerturb].pfnv_Unapply = GEO_ModifierPerturb_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_VertexPerturb].pfnul_Load = GEO_ul_ModifierPerturb_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_VertexPerturb].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_VertexPerturb].pfnv_Save = GEO_ModifierPerturb_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_VertexPerturb].pfnb_CanBeApply = GEO_Modifier_CanBeApply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_VertexPerturb].pfnv_ApplyInGeom = NULL;
#endif

	/*$1- MDF_C_Modifier_SpriteMapper2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].ul_Type = MDF_C_Modifier_SpriteMapper2;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].pfnv_Create = SPG2_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].pfnv_Destroy = SPG2_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].pfnv_Apply = SPG2_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].pfnv_Unapply = SPG2_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].pfnul_Load = SPG2_ul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].pfnv_Save = SPG2_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].pfni_Copy = SPG2_Modifier_Copy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SpriteMapper2].pfnv_ApplyInGeom = SPG2_Modifier_Apply;
#endif

	/*$1- MDF_C_Modifier_ODE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_ODE].ul_Type = MDF_C_Modifier_ODE;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ODE].pfnv_Create = GAO_ModifierODE_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ODE].pfnv_Destroy = GAO_ModifierODE_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ODE].pfnv_Apply = GAO_ModifierODE_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ODE].pfnv_Unapply = GAO_ModifierODE_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ODE].pfnul_Load = GAO_ModifierODE_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ODE].pfnv_Reinit = GAO_ModifierODE_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_ODE].pfnv_Save = GAO_ModifierODE_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ODE].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_ODE].pfnv_ApplyInGeom = NULL;
#endif

	/*$1- MDF_C_Modifier_MatrixBore ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_MatrixBore].ul_Type = MDF_C_Modifier_MatrixBore;
	MDF_gast_ModifierInterface[MDF_C_Modifier_MatrixBore].pfnv_Create = MatrixBore_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_MatrixBore].pfnv_Destroy = MatrixBore_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_MatrixBore].pfnv_Apply = MatrixBore_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_MatrixBore].pfnv_Unapply = MatrixBore_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_MatrixBore].pfnul_Load = MatrixBore_ul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_MatrixBore].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_MatrixBore].pfnv_Save = MatrixBore_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_MatrixBore].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_MatrixBore].pfnv_ApplyInGeom = MatrixBore_Modifier_Apply;
#endif

	/*$1- MDF_C_Modifier_GRID ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_GRID].ul_Type = MDF_C_Modifier_GRID;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GRID].pfnv_Create = Grid_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GRID].pfnv_Destroy = Grid_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GRID].pfnv_Apply = Grid_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GRID].pfnv_Unapply = Grid_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GRID].pfnul_Load = Grid_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GRID].pfnv_Reinit = Grid_Modifier_Reinit;

#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_GRID].pfnv_Save = Grid_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GRID].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_GRID].pfnv_ApplyInGeom = NULL;
#endif

	/*$1- MDF_C_Modifier_SoundVolume ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].ul_Type = MDF_C_Modifier_SoundVolume;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfnv_Create = SND_ModifierSoundVol_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfnv_Destroy = SND_ModifierSoundVol_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfnv_Apply = SND_ModifierSoundVol_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfnv_Unapply = SND_ModifierSoundVol_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfnul_Load = SND_ModifierSoundVol_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfnv_Reinit = SND_ModifierSoundVol_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfnv_Save = SND_ModifierSoundVol_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfnv_ApplyInGeom = NULL;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfni_Copy = SND_ModifierSoundVol_Copy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_SoundVolume].pfnv_Desactivate = SND_ModifierSoundVol_Desactivate;
#endif

#ifdef JADEFUSION
	MDF_gast_ModifierInterface[MDF_C_Modifier_WATER3D].ul_Type = MDF_C_Modifier_WATER3D;
	MDF_gast_ModifierInterface[MDF_C_Modifier_WATER3D].pfnv_Create = WATER3D_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_WATER3D].pfnv_Destroy = WATER3D_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_WATER3D].pfnv_Apply = WATER3D_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_WATER3D].pfnv_Unapply = WATER3D_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_WATER3D].pfnul_Load = WATER3D_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_WATER3D].pfnv_Reinit = WATER3D_Modifier_Reinit;

#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_WATER3D].pfnv_Save = WATER3D_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_WATER3D].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_WATER3D].pfnv_ApplyInGeom = NULL;
#endif

    MDF_gast_ModifierInterface[MDF_C_Modifier_Disturber].ul_Type = MDF_C_Modifier_Disturber;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Disturber].pfnv_Create = Disturber_Modifier_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Disturber].pfnv_Destroy = Disturber_Modifier_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Disturber].pfnv_Apply = Disturber_Modifier_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Disturber].pfnv_Unapply = Disturber_Modifier_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Disturber].pfnul_Load = Disturber_Modifier_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Disturber].pfnv_Reinit = Disturber_Modifier_Reinit;

#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_Disturber].pfnv_Save = Disturber_Modifier_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Disturber].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Disturber].pfnv_ApplyInGeom = NULL;
#endif

    MDF_gast_ModifierInterface[MDF_C_Modifier_Sfx].ul_Type = MDF_C_Modifier_Sfx;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Sfx].pfnv_Create = GAO_ModifierSfx_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Sfx].pfnv_Destroy = GAO_ModifierSfx_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Sfx].pfnv_Apply = GAO_ModifierSfx_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Sfx].pfnv_Unapply = GAO_ModifierSfx_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Sfx].pfnul_Load = GAO_ModifierSfx_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Sfx].pfnv_Reinit = GAO_ModifierSfx_Reinit;

#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_Sfx].pfnv_Save = GAO_ModifierSfx_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Sfx].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Sfx].pfnv_ApplyInGeom = NULL;
#endif
#endif

    MDF_gast_ModifierInterface[MDF_C_Modifier_RotationPaste].ul_Type      = MDF_C_Modifier_RotationPaste;
    MDF_gast_ModifierInterface[MDF_C_Modifier_RotationPaste].pfnv_Create  = GAO_ModifierRotationPaste_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_RotationPaste].pfnv_Destroy = GAO_ModifierRotationPaste_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_RotationPaste].pfnv_Apply   = GAO_ModifierRotationPaste_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_RotationPaste].pfnv_Unapply = GAO_ModifierRotationPaste_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_RotationPaste].pfnul_Load   = GAO_ModifierRotationPaste_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_RotationPaste].pfnv_Reinit  = NULL;

#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_RotationPaste].pfnv_Save = GAO_ModifierRotationPaste_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_RotationPaste].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
    MDF_gast_ModifierInterface[MDF_C_Modifier_RotationPaste].pfnv_ApplyInGeom = NULL;
#endif

    MDF_gast_ModifierInterface[MDF_C_Modifier_TranslationPaste].ul_Type      = MDF_C_Modifier_TranslationPaste;
    MDF_gast_ModifierInterface[MDF_C_Modifier_TranslationPaste].pfnv_Create  = GAO_ModifierTranslationPaste_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_TranslationPaste].pfnv_Destroy = GAO_ModifierTranslationPaste_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_TranslationPaste].pfnv_Apply   = GAO_ModifierTranslationPaste_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_TranslationPaste].pfnv_Unapply = GAO_ModifierTranslationPaste_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_TranslationPaste].pfnul_Load   = GAO_ModifierTranslationPaste_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_TranslationPaste].pfnv_Reinit  = NULL;

#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_TranslationPaste].pfnv_Save = GAO_ModifierTranslationPaste_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_TranslationPaste].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
    MDF_gast_ModifierInterface[MDF_C_Modifier_TranslationPaste].pfnv_ApplyInGeom = NULL;
#endif

#ifdef JADEFUSION
    /*$1- MDF_C_Modifier_DYNFUR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MDF_gast_ModifierInterface[MDF_C_Modifier_DYNFUR].ul_Type = MDF_C_Modifier_DYNFUR;
    MDF_gast_ModifierInterface[MDF_C_Modifier_DYNFUR].pfnv_Create = DYNFUR_Modifier_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_DYNFUR].pfnv_Destroy = DYNFUR_Modifier_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_DYNFUR].pfnv_Apply = DYNFUR_Modifier_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_DYNFUR].pfnv_Unapply = DYNFUR_Modifier_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_DYNFUR].pfnul_Load = DYNFUR_Modifier_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_DYNFUR].pfnv_Reinit = DYNFUR_Modifier_Reinit;
#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_DYNFUR].pfnv_Save = DYNFUR_Modifier_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_DYNFUR].pfnb_CanBeApply = DYNFUR_Modifier_CanBeApply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_DYNFUR].pfnv_ApplyInGeom = NULL;
#endif
#endif

    MDF_gast_ModifierInterface[MDF_C_Modifier_AnimatedGAO].ul_Type      = MDF_C_Modifier_AnimatedGAO;
    MDF_gast_ModifierInterface[MDF_C_Modifier_AnimatedGAO].pfnv_Create  = GAO_ModifierAnimatedGAO_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_AnimatedGAO].pfnv_Destroy = GAO_ModifierAnimatedGAO_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_AnimatedGAO].pfnv_Apply   = GAO_ModifierAnimatedGAO_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_AnimatedGAO].pfnv_Unapply = GAO_ModifierAnimatedGAO_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_AnimatedGAO].pfnul_Load   = GAO_ModifierAnimatedGAO_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_AnimatedGAO].pfnv_Reinit  = GAO_ModifierAnimatedGAO_Reinit;

#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_AnimatedGAO].pfnv_Save = GAO_ModifierAnimatedGAO_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_AnimatedGAO].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
    MDF_gast_ModifierInterface[MDF_C_Modifier_AnimatedGAO].pfnv_ApplyInGeom = NULL;
#endif

#ifdef JADEFUSION
    // Weather
    // -------
    MDF_gast_ModifierInterface[MDF_C_Modifier_Weather].ul_Type          = MDF_C_Modifier_Weather;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Weather].pfnv_Create      = Weather_Modifier_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Weather].pfnv_Destroy     = Weather_Modifier_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Weather].pfnv_Apply       = Weather_Modifier_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Weather].pfnv_Unapply     = Weather_Modifier_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Weather].pfnul_Load       = Weather_Modifier_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Weather].pfnv_Reinit      = Weather_Modifier_Reinit;
#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_Weather].pfnv_Save        = Weather_Modifier_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Weather].pfnb_CanBeApply  = GAO_Modifier_ApplyAlways;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Weather].pfnv_ApplyInGeom = NULL;
#endif

    MDF_gast_ModifierInterface[MDF_C_Modifier_SPG2Holder].ul_Type = MDF_C_Modifier_SPG2Holder;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SPG2Holder].pfnv_Create = SPG2Holder_Modifier_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SPG2Holder].pfnv_Destroy = SPG2Holder_Modifier_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SPG2Holder].pfnv_Apply = SPG2Holder_Modifier_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SPG2Holder].pfnv_Unapply = SPG2Holder_Modifier_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SPG2Holder].pfnul_Load = SPG2Holder_Modifier_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SPG2Holder].pfnv_Reinit = SPG2Holder_Modifier_Reinit;

#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_SPG2Holder].pfnv_Save = SPG2Holder_Modifier_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SPG2Holder].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SPG2Holder].pfnv_ApplyInGeom = NULL;
#endif
#endif

	/*$1- MDF_C_Modifier_FogDyn ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].ul_Type = MDF_C_Modifier_FogDyn;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].pfnv_Create = GAO_ModifierFOGDY_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].pfnv_Destroy = GAO_ModifierFOGDY_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].pfnv_Apply = GAO_ModifierFOGDY_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].pfnv_Unapply = GAO_ModifierFOGDY_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].pfnul_Load = FOGDY_ul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].pfnv_Reinit = GAO_ModifierFOGDY_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].pfnv_Save = FOGDY_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].pfnv_ApplyInGeom = NULL;
    MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn].pfni_Copy = FogDy_Modifier_Copy;
#endif

    /*$1- MDF_C_Modifier_Vine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MDF_gast_ModifierInterface[MDF_C_Modifier_Vine].ul_Type = MDF_C_Modifier_Vine;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Vine].pfnv_Create = VINE_Modifier_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Vine].pfnv_Destroy = VINE_Modifier_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Vine].pfnv_Apply = VINE_Modifier_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Vine].pfnv_Unapply = VINE_Modifier_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Vine].pfnul_Load = VINE_Modifier_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Vine].pfnv_Reinit = VINE_Modifier_Reinit;
#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_Vine].pfnv_Save = VINE_Modifier_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Vine].pfnb_CanBeApply = VINE_Modifier_CanBeApply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Vine].pfnv_ApplyInGeom = NULL;
#endif

#ifdef JADEFUSION
	/*$1- MDF_C_Modifier_FogDyn_Emiter ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].ul_Type = MDF_C_Modifier_FogDyn_Emiter;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Create = GAO_ModifierFOGDY_Emtr_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Destroy = GAO_ModifierFOGDY_Emtr_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Apply = GAO_ModifierFOGDY_Emtr_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Unapply = GAO_ModifierFOGDY_Emtr_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnul_Load = FOGDY_Emtr_ul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Reinit = GAO_ModifierFOGDY_Emtr_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Save = FOGDY_Emtr_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_ApplyInGeom = NULL;
    MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfni_Copy = FOGDY_Emtr_Modifier_Copy;
#endif
#endif

	/*$1- MDF_C_Modifier_FogDyn_Emiter ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].ul_Type = MDF_C_Modifier_FogDyn_Emiter;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Create = GAO_ModifierFOGDY_Emtr_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Destroy = GAO_ModifierFOGDY_Emtr_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Apply = GAO_ModifierFOGDY_Emtr_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Unapply = GAO_ModifierFOGDY_Emtr_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnul_Load = FOGDY_Emtr_ul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Reinit = GAO_ModifierFOGDY_Emtr_Reinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_Save = FOGDY_Emtr_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfnv_ApplyInGeom = NULL;
    MDF_gast_ModifierInterface[MDF_C_Modifier_FogDyn_Emiter].pfni_Copy = FOGDY_Emtr_Modifier_Copy;
#endif


	/*$1- MDF_C_Modifier_FogDyn_Emiter ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].ul_Type = MDF_C_Modifier_Half_Angle;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].pfnv_Create = GAO_ModifierBoneRefineCreate;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].pfnv_Destroy = GAO_ModifierBoneRefineDestroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].pfnv_Apply = GAO_ModifierBoneRefineApply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].pfnv_Unapply = GAO_ModifierBoneRefineUnapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].pfnul_Load = BoneRefineul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].pfnv_Reinit = GAO_ModifierBoneRefineReinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].pfnv_Save = BoneRefineModifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].pfnv_ApplyInGeom = NULL;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Half_Angle].pfni_Copy = BoneRefineModifier_Copy;
#endif
	/*$1- GAO_ModifierBoneMeca ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].ul_Type = MDF_C_Modifier_BoneMeca;
	MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].pfnv_Create = GAO_ModifierBoneMecaCreate;
	MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].pfnv_Destroy = GAO_ModifierBoneMecaDestroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].pfnv_Apply = GAO_ModifierBoneMecaApply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].pfnv_Unapply = GAO_ModifierBoneMecaUnapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].pfnul_Load = BoneMecaul_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].pfnv_Reinit = GAO_ModifierBoneMecaReinit;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].pfnv_Save = BoneMecaModifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
	MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].pfnv_ApplyInGeom = NULL;
    MDF_gast_ModifierInterface[MDF_C_Modifier_BoneMeca].pfni_Copy = BoneMecaModifier_Copy;
#endif

#ifdef JADEFUSION
    /*$1- MDF_C_Modifier_SoftBody ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if !defined(XML_CONV_TOOL)
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoftBody].ul_Type = MDF_C_Modifier_SoftBody;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoftBody].pfnv_Create = GAO_ModifierSoftBody_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoftBody].pfnv_Destroy = GAO_ModifierSoftBody_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoftBody].pfnv_Apply = GAO_ModifierSoftBody_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoftBody].pfnv_Unapply = GAO_ModifierSoftBody_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoftBody].pfnul_Load = GAO_ModifierSoftBody_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoftBody].pfnv_Reinit = GAO_ModifierSoftBody_Reinit;








#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoftBody].pfnv_Save = GAO_ModifierSoftBody_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoftBody].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
    MDF_gast_ModifierInterface[MDF_C_Modifier_SoftBody].pfnv_ApplyInGeom = NULL;
#endif

    /*$1- MDF_C_Modifier_Wind ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MDF_gast_ModifierInterface[MDF_C_Modifier_Wind].ul_Type = MDF_C_Modifier_Wind;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Wind].pfnv_Create = GAO_ModifierWind_Create;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Wind].pfnv_Destroy = GAO_ModifierWind_Destroy;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Wind].pfnv_Apply = GAO_ModifierWind_Apply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Wind].pfnv_Unapply = GAO_ModifierWind_Unapply;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Wind].pfnul_Load = GAO_ModifierWind_Load;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Wind].pfnv_Reinit = GAO_ModifierWind_Reinit;

#ifdef ACTIVE_EDITORS
    MDF_gast_ModifierInterface[MDF_C_Modifier_Wind].pfnv_Save = GAO_ModifierWind_Save;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Wind].pfnb_CanBeApply = GAO_Modifier_ApplyAlways;
    MDF_gast_ModifierInterface[MDF_C_Modifier_Wind].pfnv_ApplyInGeom = NULL;
#endif
#endif //TOOL
#endif //JADEFUSION

	/*$1- MDF_C_Modifier_FClone ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef JADEFUSION
	MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].ul_Type = MDF_C_Modifier_FClone;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].pfnv_Create = FCLONE_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].pfnv_Destroy = FCLONE_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].pfnv_Apply = FCLONE_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].pfnv_Unapply = FCLONE_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].pfnul_Load = FCLONE_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].pfnv_Save = FCLONE_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].pfnb_CanBeApply = FCLONE_Modifier_CanBeApply;
	//MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].pfnv_ApplyInGeom = NULL;
#endif
#endif
	/*$1- MDF_C_Modifier_UVTexWave ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gast_ModifierInterface[MDF_C_Modifier_UVTexWave].ul_Type = MDF_C_Modifier_UVTexWave;
	MDF_gast_ModifierInterface[MDF_C_Modifier_UVTexWave].pfnv_Create = UVTexWave_Modifier_Create;
	MDF_gast_ModifierInterface[MDF_C_Modifier_UVTexWave].pfnv_Destroy = UVTexWave_Modifier_Destroy;
	MDF_gast_ModifierInterface[MDF_C_Modifier_UVTexWave].pfnv_Apply = UVTexWave_Modifier_Apply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_UVTexWave].pfnv_Unapply = UVTexWave_Modifier_Unapply;
	MDF_gast_ModifierInterface[MDF_C_Modifier_UVTexWave].pfnul_Load = UVTexWave_Modifier_Load;
	MDF_gast_ModifierInterface[MDF_C_Modifier_UVTexWave].pfnv_Reinit = NULL;
#ifdef ACTIVE_EDITORS
	MDF_gast_ModifierInterface[MDF_C_Modifier_UVTexWave].pfnv_Save = UVTexWave_Modifier_Save;
	MDF_gast_ModifierInterface[MDF_C_Modifier_UVTexWave].pfnb_CanBeApply = UVTexWave_Modifier_CanBeApply;
	//MDF_gast_ModifierInterface[MDF_C_Modifier_FClone].pfnv_ApplyInGeom = NULL;
#endif

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MDF_tdst_Modifier *MDF_pst_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, int i, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = (MDF_tdst_Modifier *) MEM_p_Alloc(sizeof(MDF_tdst_Modifier));
	pst_Mod->i = &MDF_gast_ModifierInterface[i];
	pst_Mod->pst_Next = NULL;
	pst_Mod->pst_Prev = NULL;
	pst_Mod->ul_Flags = 0;
	pst_Mod->pst_Group = NULL;
	pst_Mod->pst_GO = _pst_GO;
	pst_Mod->i->pfnv_Create(_pst_GO, pst_Mod, p_Data);
	return pst_Mod;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	_pst_Mod->i->pfnv_Destroy(_pst_Mod);
	MEM_Free(_pst_Mod);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG MDF_ul_Modifier_Load(MDF_tdst_Modifier **_ppst_Mod, char *_pc_Buffer, int _b_Recurs, void *_p_GO)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	*pc_Cur;
	char	*pc_LookAhead;
	ULONG	ul_EndMark;
	/*~~~~~~~~~~~~~~~~~~*/

	MDF_gpst_GlobalCreate = NULL;
	pc_Cur = _pc_Buffer;
	(*_ppst_Mod) = MDF_pst_Modifier_Create((OBJ_tdst_GameObject *) _p_GO, LOA_ReadInt(&pc_Cur), NULL);
	(*_ppst_Mod)->pst_GO = (OBJ_tdst_GameObject *) _p_GO;
	(*_ppst_Mod)->ul_Flags = LOA_ReadULong(&pc_Cur);

	pc_Cur += (*_ppst_Mod)->i->pfnul_Load((*_ppst_Mod), pc_Cur);

	pc_LookAhead = pc_Cur;
	_LOA_ReadULong(&pc_LookAhead, &ul_EndMark, LOA_eBinLookAheadData);

	if((_b_Recurs) && (ul_EndMark != 0xFFFFFFFF))
	{
		pc_Cur += MDF_ul_Modifier_Load(&(*_ppst_Mod)->pst_Next, pc_Cur, 1, _p_GO);
		if((*_ppst_Mod)->pst_Next) (*_ppst_Mod)->pst_Next->pst_Prev = (*_ppst_Mod);
	}

	return(pc_Cur - _pc_Buffer);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Reinit(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject)) return;
	pst_Mod = _pst_GO->pst_Extended->pst_Modifiers;
	while(pst_Mod)
	{
		if(pst_Mod->i->pfnv_Reinit) pst_Mod->i->pfnv_Reinit(pst_Mod);
		pst_Mod = pst_Mod->pst_Next;
	}
}

#ifdef GSP_PS2_BENCH
extern unsigned int NoMDF;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ApplyAll(struct OBJ_tdst_GameObject_ *_pst_GO, int _i_ExcType)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Modifier;
	void				*pst_Visu;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoMDF) return;
#endif
	pst_Visu = NULL;

	if(_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Visu) pst_Visu = _pst_GO->pst_Base->pst_Visu->pst_Object;

	pst_Modifier = NULL;
	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
		pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
		while(pst_Modifier)
		{
			if
			(
				(
					pst_Modifier->ul_Flags &
						(
#ifdef JADEFUSION
						MDF_C_Modifier_ApplyGao | MDF_C_Modifier_Inactive | MDF_C_Modifier_ApplyGen | MDF_C_Modifier_ApplyRender |
#else
							MDF_C_Modifier_ApplyGao | MDF_C_Modifier_Inactive | MDF_C_Modifier_ApplyGen |
#endif
							MDF_C_Modifier_NoApply
						)
				) == 0
			)
			{
#ifdef ACTIVE_EDITORS
				if((pst_Modifier->ul_Flags & GEO_C_ModifierFlags_ApplyInGeom) && pst_Modifier->i->pfnv_ApplyInGeom)
					pst_Modifier->i->pfnv_ApplyInGeom((MDF_tdst_Modifier*)pst_Modifier, (GEO_tdst_Object*)pst_Visu);
				else
#endif
					pst_Modifier->i->pfnv_Apply(pst_Modifier, (GEO_tdst_Object *) pst_Visu);
			}

			pst_Modifier = pst_Modifier->pst_Next;
		}
	}
}

#ifdef USE_DOUBLE_RENDERING

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_InterpolateAll(struct OBJ_tdst_GameObject_ *_pst_GO, u_int Mode, float fInterpoler)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Modifier;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
		pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
		while(pst_Modifier)
		{
			if(pst_Modifier->i->pfnv_Interpolate)
			{
				pst_Modifier->i->pfnv_Interpolate(pst_Modifier, Mode, fInterpoler);
			}

			pst_Modifier = pst_Modifier->pst_Next;
		}
	}
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_UnApplyAll(struct OBJ_tdst_GameObject_ *_pst_GO, int _i_ExcType)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Modifier;
	void				*pst_Visu;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoMDF) return;
#endif
	pst_Visu = NULL;

	if(_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Visu) pst_Visu = _pst_GO->pst_Base->pst_Visu->pst_Object;

	pst_Modifier = NULL;
	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
		pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
		if(pst_Modifier)
		{
			while(pst_Modifier != NULL)
			{
				if
				(
					pst_Modifier->ul_Flags &
						(
#ifdef JADEFUSION
						MDF_C_Modifier_ApplyGao | MDF_C_Modifier_Inactive | MDF_C_Modifier_ApplyGen | MDF_C_Modifier_ApplyRender |
#else
							MDF_C_Modifier_ApplyGao | MDF_C_Modifier_Inactive | MDF_C_Modifier_ApplyGen |
#endif
								MDF_C_Modifier_NoApply
						)
				)
				{
					pst_Modifier = pst_Modifier->pst_Next;
					continue;
				}

#ifdef ACTIVE_EDITORS
				if(pst_Modifier->ul_Flags & GEO_C_ModifierFlags_ApplyInGeom)
				{
					MDF_Modifier_Destroy(pst_Modifier);
					MDF_Modifier_DelInGameObject(_pst_GO, pst_Modifier);
					LINK_UpdatePointer(_pst_GO);
					LINK_UpdatePointers();
				}
				else
#endif
				{
					pst_Modifier->i->pfnv_Unapply(pst_Modifier, (struct GEO_tdst_Object_ *) pst_Visu);
				}

				pst_Modifier = pst_Modifier->pst_Next;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ApplyAllGao(struct OBJ_tdst_GameObject_ *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Modifier;
	void				*pst_Visu;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoMDF) return;
#endif
	pst_Visu = NULL;
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
	{
		pst_Visu = _pst_GO->pst_Base->pst_Visu->pst_Object;
		GDI_gpst_CurDD->ul_CurrentDrawMask = GDI_gpst_CurDD->ul_DrawMask & _pst_GO->pst_Base->pst_Visu->ul_DrawMask;
	}
	else
		GDI_gpst_CurDD->ul_CurrentDrawMask = GDI_gpst_CurDD->ul_DrawMask;

	pst_Modifier = NULL;
	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
		pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
		while(pst_Modifier)
		{
			if(!(pst_Modifier->ul_Flags & (MDF_C_Modifier_Inactive | MDF_C_Modifier_NoApply)))
			{
				if(pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyGao)
				{
					GDI_gpst_CurDD->pst_CurrentGameObject = _pst_GO;
					pst_Modifier->i->pfnv_Apply(pst_Modifier, (GEO_tdst_Object *) pst_Visu);
				}
			}

			pst_Modifier = pst_Modifier->pst_Next;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_UnApplyAllGao(struct OBJ_tdst_GameObject_ *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Modifier;
	void				*pst_Visu;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(GSP_PS2_BENCH) || (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoMDF) return;
#endif
	pst_Visu = NULL;

	if(_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Visu) pst_Visu = _pst_GO->pst_Base->pst_Visu->pst_Object;

	pst_Modifier = NULL;
	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
		pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
		if(pst_Modifier)
		{
			while(pst_Modifier != NULL)
			{
				if(!(pst_Modifier->ul_Flags & (MDF_C_Modifier_Inactive | MDF_C_Modifier_NoApply)))
				{
					if(pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyGao)
					{
						GDI_gpst_CurDD->pst_CurrentGameObject = _pst_GO;
						pst_Modifier->i->pfnv_Unapply(pst_Modifier, (struct GEO_tdst_Object_ *) pst_Visu);
					}
				}
				else
				{
					if(pst_Modifier->i->ul_Type == MDF_C_Modifier_ODE)
					{
						GDI_gpst_CurDD->pst_CurrentGameObject = _pst_GO;
						pst_Modifier->i->pfnv_Unapply(pst_Modifier, (struct GEO_tdst_Object_ *) pst_Visu);
					}
				}

				pst_Modifier = pst_Modifier->pst_Next;
			}
		}
	}
}

#ifdef JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ApplyAllRender(struct OBJ_tdst_GameObject_ *_pst_GO)
{
    MDF_tdst_Modifier	*pst_Modifier;
    void				*pst_Visu;

    pst_Visu = NULL;
    if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        pst_Visu = _pst_GO->pst_Base->pst_Visu->pst_Object;
    }

    pst_Modifier = NULL;
    if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
    {
        pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
        while(pst_Modifier)
        {
            if(!(pst_Modifier->ul_Flags & (MDF_C_Modifier_Inactive | MDF_C_Modifier_NoApply)))
            {
                if(pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyRender)
                {
                    pst_Modifier->i->pfnv_Apply(pst_Modifier, (GEO_tdst_Object *) pst_Visu);
                }
            }

            pst_Modifier = pst_Modifier->pst_Next;
        }
    }
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void MDF_UnApplyAllRender(struct OBJ_tdst_GameObject_ *_pst_GO)
{
    MDF_tdst_Modifier	*pst_Modifier;
    void				*pst_Visu;

    pst_Visu = NULL;
    if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        pst_Visu = _pst_GO->pst_Base->pst_Visu->pst_Object;
    }

    pst_Modifier = NULL;
    if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
    {
        pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
        if(pst_Modifier)
        {
            while(pst_Modifier != NULL)
            {
                if(!(pst_Modifier->ul_Flags & (MDF_C_Modifier_Inactive | MDF_C_Modifier_NoApply)))
                {
                    if(pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyRender)
                    {
                        pst_Modifier->i->pfnv_Unapply(pst_Modifier, (struct GEO_tdst_Object_ *) pst_Visu);
                    }
                }

                pst_Modifier = pst_Modifier->pst_Next;
            }
        }
    }
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
MDF_tdst_Modifier *MDF_pst_GetByType(OBJ_tdst_GameObject *_pst_GO, int _i_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO || !_pst_GO->pst_Extended) return NULL;

	pst_Mod = _pst_GO->pst_Extended->pst_Modifiers;
	while(pst_Mod)
	{
		if(pst_Mod->i->ul_Type == (ULONG) _i_Type) return pst_Mod;
		pst_Mod = pst_Mod->pst_Next;
	}

	return NULL;
}

#if defined (ACTIVE_EDITORS) || defined (JADEFUSION)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Modifier_DelInGameObject(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(_pst_GO->pst_Extended);

	pst_Mod = _pst_GO->pst_Extended->pst_Modifiers;
	while(pst_Mod)
	{
		if(pst_Mod == _pst_Mod)
		{
			if(pst_Mod == _pst_GO->pst_Extended->pst_Modifiers)
			{
				if(pst_Mod->pst_Next) pst_Mod->pst_Next->pst_Prev = NULL;
				_pst_GO->pst_Extended->pst_Modifiers = pst_Mod->pst_Next;
			}
			else
			{
				if(pst_Mod->pst_Next) pst_Mod->pst_Next->pst_Prev = pst_Mod->pst_Prev;
				pst_Mod->pst_Prev->pst_Next = pst_Mod->pst_Next;
			}

			return;
		}

		pst_Mod = pst_Mod->pst_Next;
	}
}
#endif
#if defined (ACTIVE_EDITORS)
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Modifier_ReplaceInGameObject
(
	struct OBJ_tdst_GameObject_ *_pst_GO,
	MDF_tdst_Modifier			*_pst_OldMod,
	MDF_tdst_Modifier			*_pst_NewMod
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(_pst_GO->pst_Extended);

	pst_Mod = _pst_GO->pst_Extended->pst_Modifiers;
	while(pst_Mod)
	{
		if(pst_Mod == _pst_OldMod)
		{
			_pst_NewMod->pst_Next = _pst_OldMod->pst_Next;
			_pst_NewMod->pst_Prev = _pst_OldMod->pst_Prev;

			if(_pst_NewMod->pst_Next) _pst_NewMod->pst_Next->pst_Prev = _pst_NewMod;
			if(_pst_NewMod->pst_Prev) _pst_NewMod->pst_Prev->pst_Next = _pst_NewMod;

			if(pst_Mod == _pst_GO->pst_Extended->pst_Modifiers) _pst_GO->pst_Extended->pst_Modifiers = _pst_NewMod;
			return;
		}

		pst_Mod = pst_Mod->pst_Next;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Modifier_Save(MDF_tdst_Modifier *_pst_Mod, int _i_Recurs)
{
	SAV_Buffer(&_pst_Mod->i->ul_Type, 4);
	SAV_Buffer(&_pst_Mod->ul_Flags, 4);

	_pst_Mod->i->pfnv_Save(_pst_Mod);

	if(_pst_Mod->pst_Next && _i_Recurs)
	{
		MDF_Modifier_Save(_pst_Mod->pst_Next, _i_Recurs);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int MDF_Modifier_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src)
{
	if(_pst_Src->i->ul_Type != _pst_Dst->i->ul_Type) return 0;
	if (_pst_Src->i->pfni_Copy == NULL) return 0;
	return ( _pst_Src->i->pfni_Copy( _pst_Dst, _pst_Src ) );
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Modifier_AddToGameObject(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(_pst_GO->pst_Extended);
	_pst_Mod->pst_GO = _pst_GO;
	if(_pst_GO->pst_Extended->pst_Modifiers == NULL)
	{
		_pst_GO->pst_Extended->pst_Modifiers = _pst_Mod;
		return;
	}

	pst_Mod = _pst_GO->pst_Extended->pst_Modifiers;
	while(pst_Mod->pst_Next) pst_Mod = pst_Mod->pst_Next;

	_pst_Mod->pst_Prev = pst_Mod;
	pst_Mod->pst_Next = _pst_Mod;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_Modifier_DuplicateList(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_NewMod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MDF_gpst_GlobalCreate = NULL;
	if(!_pst_GO) return;
	if(!_pst_GO->pst_Extended) return;
	if(!pst_Mod)
	{
		_pst_GO->pst_Extended->pst_Modifiers = NULL;
		return;
	}

	while(pst_Mod)
	{
		MDF_gpst_GlobalCreate = pst_Mod;
		pst_NewMod = MDF_pst_Modifier_Create(_pst_GO, pst_Mod->i->ul_Type, pst_Mod->p_Data);
		MDF_Modifier_AddToGameObject(_pst_GO, pst_NewMod);
		pst_Mod = pst_Mod->pst_Next;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_Group *MDF_Modifier_GetCurGroup(MDF_tdst_Modifier *pst_Mod)
{
	if(pst_Mod->pst_Group) return pst_Mod->pst_Group;
	if(!pst_Mod->pst_GO->pst_Extended) return NULL;
	return pst_Mod->pst_GO->pst_Extended->pst_Group;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
