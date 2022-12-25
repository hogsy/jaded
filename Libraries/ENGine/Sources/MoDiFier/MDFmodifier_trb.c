/*$T MDFmodifier_XSPRING.c GC! */


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
#include "MoDiFier/MDFmodifier_XSPRING.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTlinear.h"
#include "SOFT/SOFTMatrixStack.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"


#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif


void XSPRING_FirstInit()
{
	static ULONG bFirst = 0;
	if (bFirst) return;
	bFirst = 1;
}

/*$4
***********************************************************************************************************************
XSPRING modifier :
Modifier apply XSPRING on world
***********************************************************************************************************************
*/

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierXSPRING_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	GAO_tdst_ModifierXSPRING *p_XSPRING;
	XSPRING_FirstInit();
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierXSPRING));
	p_XSPRING = (GAO_tdst_ModifierXSPRING *)_pst_Mod->p_Data;
   if (p_Data == NULL)
   {
		/* First init */
	    L_memset(_pst_Mod->p_Data , 0 , sizeof(GAO_tdst_ModifierXSPRING));
	    p_XSPRING->bk_GOSPRING = (ULONG)-1;
		_pst_Mod-> ul_Flags = 0;
   }
   else
   {
		L_memcpy( _pst_Mod->p_Data, p_Data, sizeof (GAO_tdst_ModifierXSPRING) );
   }
#ifdef ACTIVE_EDITORS
	p_XSPRING->ulCodeKey = 0xC0DE2001;
#endif
   p_XSPRING->pst_GO = _pst_GO;
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierXSPRING_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	if (_pst_Mod->p_Data)
	{
		MEM_Free(_pst_Mod->p_Data);
	}
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierXSPRING_Normalize(MATH_tdst_Vector *p_Pos0 ,  MATH_tdst_Vector **p_AllPos ,  ULONG Numbers , float BoneLenght)
{ 
	ULONG ITER,GOMECCOUNTER;
	MATH_tdst_Vector	*p_stPreviousPos;
	MATH_tdst_Vector	stForces[GOMEC_NUMBER];
	ITER = 100;
	while (ITER--)
	{
		p_stPreviousPos = p_Pos0;
		for (GOMECCOUNTER = 0 ; GOMECCOUNTER < Numbers ; GOMECCOUNTER ++)
		{
			stForces[GOMECCOUNTER].x = 0.0f;
			stForces[GOMECCOUNTER].y = 0.0f;
			stForces[GOMECCOUNTER].z = 0.0f;
		}
		p_stPreviousPos = p_Pos0;
		for (GOMECCOUNTER = 0 ; GOMECCOUNTER < Numbers ; GOMECCOUNTER ++)
		{
			MATH_tdst_Vector    st_Dist;
			float fBF;
			MATH_SubVector(&st_Dist , p_stPreviousPos,p_AllPos[GOMECCOUNTER]);
			fBF = MATH_f_NormVector(&st_Dist );
			fBF -= BoneLenght;
			if (fBF < 0.0f) fBF = 0.0f;
			MATH_NormalizeAnyVector(&st_Dist,&st_Dist);
			if (GOMECCOUNTER != 0) 
			{
				MATH_AddScaleVector(&stForces[GOMECCOUNTER - 1 ] , &stForces[GOMECCOUNTER - 1] , &st_Dist , -fBF * 0.5f);
				MATH_AddScaleVector(&stForces[GOMECCOUNTER ] , &stForces[GOMECCOUNTER ] , &st_Dist , fBF * 0.5f);
			} else
				MATH_AddScaleVector(&stForces[GOMECCOUNTER ] , &stForces[GOMECCOUNTER ] , &st_Dist , fBF);
			p_stPreviousPos= p_AllPos[GOMECCOUNTER];
		}
		for (GOMECCOUNTER = 1 ; GOMECCOUNTER < Numbers; GOMECCOUNTER ++)
		{
			MATH_AddVector(p_AllPos[GOMECCOUNTER], p_AllPos[GOMECCOUNTER], &stForces[GOMECCOUNTER]);
		}
	}//*/
}
#if 0
#endif
void GAO_ModifierXSPRING_Compute(GAO_tdst_ModifierXSPRING *p_XSPRING , float fDT)
{
	MATH_tdst_Vector stSPRING;
	MATH_tdst_Matrix stMAtrix;
	SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);
	stMAtrix = *p_XSPRING->pst_GO->pst_GlobalMatrix;
	MATH_SubVector(&stSPRING , &stMAtrix.T, &p_XSPRING->stSavedMatrix.T);
	MATH_AddScaleVector(&p_XSPRING->stSpeed , &p_XSPRING->stSpeed , &stSPRING , fDT * p_XSPRING->fTension);
	MATH_AddScaleVector(&p_XSPRING->stSavedMatrix.T , &p_XSPRING->stSavedMatrix.T , &p_XSPRING->stSpeed , fDT);
	MATH_ScaleEqualVector(&p_XSPRING->stSpeed , p_XSPRING->fElasticity);
	stMAtrix.T = p_XSPRING->stSavedMatrix.T;
	SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack , &stMAtrix);
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
}
void GAO_ModifierXSPRING_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	float DT;
	DT = TIM_gf_dt;
	if (DT == 0.0f) DT = 1.0f/50.0f;
	
	GAO_ModifierXSPRING_Compute((GAO_tdst_ModifierXSPRING *)_pst_Mod->p_Data , DT);
	
}
/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierXSPRING_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierXSPRING_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierXSPRING *p_XSPRING;
	p_XSPRING = (GAO_tdst_ModifierXSPRING *)_pst_Mod->p_Data;
	p_XSPRING->stSavedMatrix.T = p_XSPRING->pst_GO->pst_GlobalMatrix->T ;
	MATH_InitVectorToZero(&p_XSPRING->stSpeed);

}

/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG GAO_ModifierXSPRING_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	GAO_tdst_ModifierXSPRING *p_XSPRING;
	char *_pc_BufferSave;
	ULONG ulVersion;
	XSPRING_FirstInit();
	_pc_BufferSave = _pc_Buffer;
	p_XSPRING = (GAO_tdst_ModifierXSPRING *)_pst_Mod->p_Data;
	/* size */
	_pc_Buffer += 4;
	/* version */
	ulVersion = *(ULONG *)_pc_Buffer;
	_pc_Buffer += 4;

	p_XSPRING ->pst_GO = _pst_Mod->pst_GO;

#ifdef ACTIVE_EDITORS
	p_XSPRING->ulCodeKey = 0xC0DE2001;
#endif

	return _pc_Buffer - _pc_BufferSave ;
}

#ifdef ACTIVE_EDITORS
/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierXSPRING_Save(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierXSPRING *p_XSPRING;
	ULONG ulSize;

	p_XSPRING = (GAO_tdst_ModifierXSPRING *)_pst_Mod->p_Data;

   /* Save Size */
	ulSize = 4;
	SAV_Buffer(&ulSize, 4); 
   /* Save version */
	ulSize = 0;
	SAV_Buffer(&ulSize, 4); 
}
	
#endif
	
	
	
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
