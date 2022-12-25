/*$T MDFmodifier_MatrixBore.c GC! 1.081 06/09/00 09:00:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
 


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/Grid/GRI_Struct.h"
#include "ENGine/Sources/GRId/GRI_display.h"

#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "GDInterface/GDIrequest.h"

#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_MatrixBore.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTstruct.h"
#include "SOFT/SOFTzlist.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"
#include "BASe/BENch/BENch.h"


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */


void MatrixBore_InitModifier(MatrixBore_tdst_Modifier *pst_MB_)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void MatrixBore_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MatrixBore_tdst_Modifier *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	_pst_Mod->p_Data = MEM_p_AllocAlign(sizeof(MatrixBore_tdst_Modifier),64);
	pst_Data = (MatrixBore_tdst_Modifier *) _pst_Mod->p_Data;
	L_memset(pst_Data , 0 , sizeof(MatrixBore_tdst_Modifier));

	if(!p_Data)
	{
		MatrixBore_InitModifier(pst_Data);
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(MatrixBore_tdst_Modifier));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MatrixBore_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MatrixBore_tdst_Modifier *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (MatrixBore_tdst_Modifier *) _pst_Mod->p_Data;
	MEM_FreeAlign(pst_Data);
}
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */

void MatrixBore_Modifier_Apply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	V;
	MATH_tdst_Matrix	*M;
	MatrixBore_tdst_Modifier *p_MB;
	float				Parralaxolomusum;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	p_MB = (MatrixBore_tdst_Modifier *) _pst_Mod->p_Data;

	pst_GO = GDI_gpst_CurDD->pst_CurrentGameObject;
	M = OBJ_pst_GetAbsoluteMatrix(pst_GO);

#ifdef ACTIVE_EDITORS
	MATH_Orthonormalize(M);	
	if (_pst_Obj->pst_SubObject) return;
#endif

	MATH_SubVector(&V, OBJ_pst_GetAbsolutePosition(pst_GO), &GDI_gpst_CurDD->st_Camera.st_Matrix.T);


	MATH_CrossProduct(MATH_pst_GetXAxis(M), &V, MATH_pst_GetZAxis(M));
	MATH_NormalizeEqualVector(MATH_pst_GetXAxis(M));
	MATH_CrossProduct(MATH_pst_GetYAxis(M), MATH_pst_GetZAxis(M), MATH_pst_GetXAxis(M));


	/* then bore it */
	Parralaxolomusum = MATH_f_DotProduct(MATH_pst_GetYAxis(M),MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));

	MATH_CrossProduct(MATH_pst_GetYAxis(M), MATH_pst_GetXAxis(M), MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
	MATH_NormalizeVector(MATH_pst_GetYAxis(M), MATH_pst_GetYAxis(M));

	MATH_ScaleVector(MATH_pst_GetYAxis(M), MATH_pst_GetYAxis(M) , 1.0f + Parralaxolomusum * p_MB->Strenght);

/*	Parralaxolomusum = MATH_f_DotProduct(&VDirNormalized , MATH_pst_GetZAxis(M));
	Parralaxolomusum = fOptSqrt(1.0f - Parralaxolomusum * Parralaxolomusum);
	MATH_AddScaleVector(MATH_pst_GetYAxis(M) , MATH_pst_GetYAxis(M) , MATH_pst_GetZAxis(M) , Parralaxolomusum * 4.0f);
*/

}


void MatrixBore_Modifier_Unapply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
)
{
}


#ifdef ACTIVE_EDITORS
#define MatrixBore_SAV_LOAD_Buffer(ValueA , TypeC)	ulSize += 4; if (Flags & 1) SAV_Buffer(&ValueA , 4); if (Flags & 2) ValueA = LOA_Read##TypeC(_pc_ReadBuffer);
#else
#define MatrixBore_SAV_LOAD_Buffer(ValueA , TypeC)	ulSize += 4; if (Flags & 2) ValueA = LOA_Read##TypeC(_pc_ReadBuffer);
#endif

ULONG MatrixBore_Modifier_SaveLoad_Parrams(MatrixBore_tdst_Modifier *_pst_MB , ULONG Flags, char **_pc_ReadBuffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							ulSize,ulVersion;
	ULONG							Dummy;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Dummy = 0;
	ulSize = 0;
	if (Flags & 1) // Save
	{
		ulVersion = 0; // Increment Last version Here
	}
	if (Flags & 2) // Read
	{
		MatrixBore_InitModifier(_pst_MB);
	}
	/* SaveLoad Version */
	MatrixBore_SAV_LOAD_Buffer(ulVersion, Long);
	/* SaveLoad Strenght */
	MatrixBore_SAV_LOAD_Buffer(_pst_MB ->Strenght, Float);

#ifdef ACTIVE_EDITORS
	_pst_MB->ulCodeKey = 0xC0DE2002;
#endif
	return ulSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
void MatrixBore_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MatrixBore_tdst_Modifier				*_pst_MB_;
	ULONG								ulSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_MB_ = (MatrixBore_tdst_Modifier *) _pst_Mod->p_Data;
	/* Save Size */
	ulSize = MatrixBore_Modifier_SaveLoad_Parrams(_pst_MB_ , 0 , NULL);
	SAV_Buffer(&ulSize, 4);
	MatrixBore_Modifier_SaveLoad_Parrams(_pst_MB_ , 1 , NULL);
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
int MatrixBore_Modifier_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MatrixBore_tdst_Modifier	*Src, *Dst ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Src = (MatrixBore_tdst_Modifier *) _pst_Src->p_Data;
	Dst = (MatrixBore_tdst_Modifier *) _pst_Dst->p_Data;
	
	/* copy */
	memcpy(Dst , Src , sizeof(MatrixBore_tdst_Modifier));
	return 1;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */

ULONG MatrixBore_ul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MatrixBore_tdst_Modifier				*_pst_MB ;
	unsigned char					*pc_Cur;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Cur =  (unsigned char*)_pc_Buffer;
	_pst_MB = (MatrixBore_tdst_Modifier *) _pst_Mod->p_Data;
	/* Skip size */
	LOA_ReadLong((CHAR**)&pc_Cur);
	MatrixBore_Modifier_SaveLoad_Parrams(_pst_MB , 2, (char**)&pc_Cur);

	//return pc_Cur - _pc_Buffer ;
	return pc_Cur - (unsigned char*)_pc_Buffer;
}



#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
