#ifndef __MDFMODIFIER_GRID_H__
#define __MDFMODIFIER_GRID_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif



typedef struct Grid_tdst_Modifier_
{
	UCHAR						uc_Version;
	UCHAR						uc_Dummy;
	USHORT						uw_Dummy;

	/* Grid 0 */
	char						*pc_Capa;
	ULONG						ul_Num;

	/* Grid 1 */
	char						*pc_Capa1;
	ULONG						ul_Num1;

	ULONG						ul_Rotate;

} 
	Grid_tdst_Modifier;


extern void Grid_Modifier_Create(OBJ_tdst_GameObject *, MDF_tdst_Modifier *, void *);
extern void Grid_Modifier_Destroy(MDF_tdst_Modifier *);
extern void Grid_Modifier_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void Grid_Modifier_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void Grid_Modifier_Reinit(MDF_tdst_Modifier *);
extern ULONG Grid_Modifier_Load(MDF_tdst_Modifier *, char *);
#ifdef ACTIVE_EDITORS
extern void Grid_Modifier_Save(MDF_tdst_Modifier *);
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
