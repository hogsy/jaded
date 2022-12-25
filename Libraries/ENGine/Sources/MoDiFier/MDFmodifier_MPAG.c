/*$T MDFmodifier_ROTR.c GC 1.129 10/09/01 16:44:58 */


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
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_ROTR.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTlinear.h"
#include "MDFmodifier_MPAG.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void MPAG_Modifier_Create(OBJ_tdst_GameObject * _pst_GO, MDF_tdst_Modifier * _pst_Mod, void *p_Data)
{
	MPAG_tdst_Modifier *p_ROTR;

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(MPAG_tdst_Modifier));
	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
	p_ROTR = (MPAG_tdst_Modifier *) _pst_Mod->p_Data;
	if(p_Data == NULL)
	{
		p_ROTR->pst_P = NULL;//PAG_pst_Create();
	}
	else
	{
		//p_ROTR->pst_P = PAG_p_Duplicate(((MPAG_tdst_Modifier *) p_Data)->pst_P, NULL, NULL, 0);
		p_ROTR->pst_P = (PAG_tdst_Struct*)PAG_p_Duplicate(((MPAG_tdst_Modifier *) p_Data)->pst_P, NULL, NULL, 0);
	}
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void MPAG_Modifier_Destroy(MDF_tdst_Modifier * _pst_Mod)
{
	MPAG_tdst_Modifier *p_ROTR;

	p_ROTR = (MPAG_tdst_Modifier *) _pst_Mod->p_Data;
	if(p_ROTR) 
	{
		PAG_Free(p_ROTR->pst_P);
		MEM_Free(p_ROTR);
	}
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void MPAG_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object * _pst_Obj)
{
	MPAG_tdst_Modifier *p_ROTR;
	p_ROTR = (MPAG_tdst_Modifier *) _pst_Mod->p_Data;

	SOFT_MatrixStack_Reset(&GDI_gpst_CurDD->st_MatrixStack, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);		
	SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO));
	GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);

	PAG_RenderCommon(_pst_Mod->pst_GO, p_ROTR->pst_P, 1);

	SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void MPAG_Modifier_Unapply(MDF_tdst_Modifier * _pst_Mod, GEO_tdst_Object * _pst_Obj)
{
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void MPAG_ul_Modifier_Reinit(MDF_tdst_Modifier * _pst_Mod)
{
	MPAG_tdst_Modifier *p_ROTR;
	p_ROTR = (MPAG_tdst_Modifier *) _pst_Mod->p_Data;
	if(!p_ROTR->pst_P) p_ROTR->pst_P = PAG_pst_Create();
	PAG_Reinit((GRO_tdst_Struct *) p_ROTR->pst_P);
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
ULONG MPAG_ul_Modifier_Load(MDF_tdst_Modifier * _pst_Mod, char *_pc_Buffer)
{
	MPAG_tdst_Modifier *p_ROTR;
	char *_pc_BufferSave;

	_pc_BufferSave = _pc_Buffer;
	p_ROTR = (MPAG_tdst_Modifier *) _pst_Mod->p_Data;
	//p_ROTR->pst_P = PAG_p_CreateFromBuffer(NULL, &_pc_Buffer, NULL);
	p_ROTR->pst_P = (PAG_tdst_Struct*)PAG_p_CreateFromBuffer(NULL, &_pc_Buffer, NULL);
	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;

	return _pc_Buffer - _pc_BufferSave;
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#ifdef ACTIVE_EDITORS
void MPAG_ul_Modifier_Save(MDF_tdst_Modifier * _pst_Mod)
{
	MPAG_tdst_Modifier *p_ROTR;
	p_ROTR = (MPAG_tdst_Modifier *) _pst_Mod->p_Data;
	PAG_l_SaveInBufferCommon(p_ROTR->pst_P, NULL);
}
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
