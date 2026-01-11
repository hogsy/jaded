/*$T MDFmodifier_FOGDY.c GC! */


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
#include "GDInterface/GDIrequest.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_BrumeDynamique.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTlinear.h"
#include "GFX/GFX.h"
#include "GFX/GFXExplode.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif

#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"


#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#define MaxSpheres 8
ULONG				FOGDYN_LODMinIndex;
typedef struct SPheresParrams_
{
	float				FOGDYN_PERT_SphereLod	[MaxSpheres];
	float				FOGDYN_PERT_SphereRadius[MaxSpheres];
	MATH_tdst_Vector	FOGDYN_PERT_SphereCenters[MaxSpheres];
	MATH_tdst_Vector	FOGDYN_PERT_SphereSpeed[MaxSpheres];
	u32					FOGDYN_PERT_SphereChannel[MaxSpheres];
	OBJ_tdst_GameObject*FOGDYN_PERT_Gao[MaxSpheres];
	ULONG				FOGDYN_PERT_ulNumberOfSpheres;
} SPheresParrams;

SPheresParrams	AllSpheres[2];
SPheresParrams *p_CurrentSphere = &AllSpheres[0];
SPheresParrams *p_LastCurrentSphere = &AllSpheres[0];


#define FOGDYN_Cte_ActiveGaoMaxNb		(2*MaxSpheres)
OBJ_tdst_GameObject*					FOGDYN_gap_ActiveGao[FOGDYN_Cte_ActiveGaoMaxNb];
int										FOGDYN_gi_ActiveGaoNb = 0;

void FOGDYN_Reset(void)
{
	L_memset(FOGDYN_gap_ActiveGao, 0, FOGDYN_Cte_ActiveGaoMaxNb*sizeof(OBJ_tdst_GameObject *));
	FOGDYN_gi_ActiveGaoNb = 0;
}
 
void MODIFIER_FOGDYN_OneFrameCall()
{
	p_LastCurrentSphere = p_CurrentSphere;
	if (p_CurrentSphere == &AllSpheres[0])
		p_CurrentSphere = &AllSpheres[1];
	else
		p_CurrentSphere = &AllSpheres[0];
	p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres = 0;
}

void FOGDyn_AddASphere(MATH_tdst_Vector	*Pos , MATH_tdst_Vector	*Speed , float Radius, u32 Mode, OBJ_tdst_GameObject*ptr )
{
	float LOD;
	if (p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres == 0) FOGDYN_LODMinIndex = 0;
	LOD = MATH_f_Distance(Pos,&GDI_gpst_CurDD->st_Camera.st_Matrix.T);
	if (LOD == 0.0f)
		LOD = 1000000.0f;
	else
		LOD = Radius / LOD ;
	if (p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres < MaxSpheres)
	{
		if (p_CurrentSphere -> FOGDYN_PERT_SphereLod[FOGDYN_LODMinIndex] > LOD) FOGDYN_LODMinIndex = p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres;
		p_CurrentSphere -> FOGDYN_PERT_SphereLod[p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres] = LOD;
		p_CurrentSphere -> FOGDYN_PERT_SphereCenters[p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres] = *Pos;
		p_CurrentSphere -> FOGDYN_PERT_Gao[p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres] = ptr;
		p_CurrentSphere -> FOGDYN_PERT_SphereRadius[p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres] = Radius;
		p_CurrentSphere -> FOGDYN_PERT_SphereSpeed[p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres] = *Speed;
		p_CurrentSphere -> FOGDYN_PERT_SphereChannel[p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres] = Mode;
		p_CurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres ++;
	} else
		if (LOD > p_CurrentSphere -> FOGDYN_PERT_SphereLod[FOGDYN_LODMinIndex])
		{
			ULONG Counter;
			/* Take place of LOD Min */
			p_CurrentSphere -> FOGDYN_PERT_SphereLod[FOGDYN_LODMinIndex] = LOD;
			p_CurrentSphere -> FOGDYN_PERT_SphereCenters[FOGDYN_LODMinIndex] = *Pos;
			p_CurrentSphere -> FOGDYN_PERT_Gao[FOGDYN_LODMinIndex] = ptr;
			p_CurrentSphere -> FOGDYN_PERT_SphereRadius[FOGDYN_LODMinIndex] = Radius;
			p_CurrentSphere -> FOGDYN_PERT_SphereSpeed[FOGDYN_LODMinIndex] = *Speed;
			p_CurrentSphere -> FOGDYN_PERT_SphereChannel[FOGDYN_LODMinIndex] = Mode;
			/* Recompute FOGDYN_LODMinIndex */
			Counter = MaxSpheres - 1;
			FOGDYN_LODMinIndex = MaxSpheres - 1;
			while (Counter--)
				if (p_CurrentSphere -> FOGDYN_PERT_SphereLod[FOGDYN_LODMinIndex] > p_CurrentSphere -> FOGDYN_PERT_SphereLod[Counter]) FOGDYN_LODMinIndex = Counter;
		}
}



/*$4
***********************************************************************************************************************
FOGDY modifier :
Modifier apply FOGDY on world
***********************************************************************************************************************
*/
#ifdef USE_DOUBLE_RENDERING	
void 								GAO_ModifierFOGDY_Interpolate
	(
struct MDF_tdst_Modifier_ * p_Mod, 
	u_int mode , 
	float fInterpolatedValue
	)
{
}
#endif

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierFOGDY_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierFOGDY));

	if (p_Data == NULL)
	{
		/* First init */
		L_memset(_pst_Mod->p_Data , 0 , sizeof(GAO_tdst_ModifierFOGDY));
		_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
	}
	else
	{
		GAO_tdst_ModifierFOGDY *p_FOGDY;
		_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
		p_FOGDY = (GAO_tdst_ModifierFOGDY *)_pst_Mod->p_Data;
		L_memcpy( _pst_Mod->p_Data, p_Data, sizeof (GAO_tdst_ModifierFOGDY) );
	}
#ifdef ACTIVE_EDITORS
	((GAO_tdst_ModifierFOGDY *)_pst_Mod->p_Data) ->p_GO = _pst_GO;
	((GAO_tdst_ModifierFOGDY *)_pst_Mod->p_Data) ->ulCodeKey = 0xC0DE2002;
	((GAO_tdst_ModifierFOGDY *)_pst_Mod->p_Data) ->ActiveChannel = -1;
#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierFOGDY_Destroy(MDF_tdst_Modifier *_pst_Mod)
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

void GAO_ModifierFOGDY_PercuteDetected(MDF_tdst_Modifier *_pst_Mod,MATH_tdst_Vector *pSrc,MATH_tdst_Vector *pSpeed,float SpeedNorm)
{
#if !defined(XML_CONV_TOOL)
	WOR_tdst_World		*pst_World;
	USHORT ti_GFX_Impact;
	int result;
	float Aperture,ZMax ;
	GAO_tdst_ModifierFOGDY *_pst_FOGDY; 
	MATH_tdst_Vector Speed,Grvity ;
	_pst_FOGDY = (GAO_tdst_ModifierFOGDY *)_pst_Mod->p_Data;

	pst_World = WOR_World_GetWorldOfObject(_pst_Mod->pst_GO);


	// ETINCELLES
	result = GFX_i_Add(&pst_World->pst_GFX , 9 , _pst_Mod->pst_GO); // Create the boum;
	ERR_X_Assert( result != -1 ); // the boum has not to boum itself -- Max 6 Feb 2003
	ti_GFX_Impact = (USHORT) result;

	GFX_FlagSet(pst_World->pst_GFX , ti_GFX_Impact, 0 , 1);
	GFX_FlagSet(pst_World->pst_GFX , ti_GFX_Impact, 2 , 1);

	Speed = *pSpeed;

	Aperture = 0.8f;
	ZMax = fSin(Aperture * 0.5f) * SpeedNorm ;
	if (Speed.z < SpeedNorm) Speed.z = SpeedNorm;
	MATH_NormalizeVector(&Speed,&Speed);

	//Speed
	Grvity.x = Grvity.y = 0.0f;
	Grvity.z = _pst_FOGDY->Gravitiy;


	if ((_pst_Mod->pst_GO->pst_Base) &&	(_pst_Mod->pst_GO->pst_Base->pst_Visu) && (_pst_Mod->pst_GO->pst_Base->pst_Visu->pst_Material))
		GFX_SetMaterial(pst_World->pst_GFX, ti_GFX_Impact, (void *)_pst_Mod->pst_GO->pst_Base->pst_Visu->pst_Material);


	GFX_FlagSet(pst_World->pst_GFX , ti_GFX_Impact , 0, 1);
	GFX_FlagSet(pst_World->pst_GFX , ti_GFX_Impact , 2, 1);

	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9000, Aperture)		;												// angle d'ouverture
	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9001, SpeedNorm * _pst_FOGDY->SpeedMin);														// vitesse initiale minimum
	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9002, SpeedNorm * _pst_FOGDY->SpeedMax);														// vitesse initiale maximum
	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9003, 1.0f - _pst_FOGDY->FrictionGrow )		;												// friction
	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9004, _pst_FOGDY->SizeMin)			;											// épaisseur
	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9005, _pst_FOGDY->TimePhase1)			;											// durée de vie minimum
	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9006, _pst_FOGDY->TimePhase1*_pst_FOGDY->TimeVariance)			;											// durée de vie maximum
	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9007, 2.0f )			;											// multiplicateur de longueur
	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9008, 0.000f )		;											// generation périod
	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9009, _pst_FOGDY->TimePhase2)			;											// durée de mort minimun
	GFX_Setf(pst_World->pst_GFX , ti_GFX_Impact, 9010, _pst_FOGDY->TimePhase2*_pst_FOGDY->TimeVariance)			;											// durée de mort maximun
	GFX_Seti(pst_World->pst_GFX , ti_GFX_Impact, 9100, _pst_FOGDY->ulNumberOfActiveSprites)			;											// Nombre
	GFX_Seti(pst_World->pst_GFX , ti_GFX_Impact, 9102, 0x18)		;										// Flags (1 => agrandissement centré)
	GFX_Setv(pst_World->pst_GFX , ti_GFX_Impact, 9200, pSrc)	;									// Origine des étincelles
	GFX_Setv(pst_World->pst_GFX , ti_GFX_Impact, 9201, &Speed)	;							// Direction
	GFX_Setv(pst_World->pst_GFX , ti_GFX_Impact, 9202, &Grvity )	;						// gravity
	GFX_Seti(pst_World->pst_GFX , ti_GFX_Impact, 9101, _pst_FOGDY->Colors1) 					;							// couleur}


	//	_pst_FOGDY -> GFX_SPARK_ID = ti_GFX_Impact;

#endif // XML_CONV_TOOL
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierFOGDY_CreateGfx(MDF_tdst_Modifier *_pst_Mod, OBJ_tdst_GameObject*ptr)
{
#if !defined(XML_CONV_TOOL)
	WOR_tdst_World		*pst_World;
	USHORT GFX;
	int result, i,j;
	MATH_tdst_Vector stLocal;
	GAO_tdst_ModifierFOGDY *_pst_FOGDY; 
			

	if( ptr && (FOGDYN_gi_ActiveGaoNb < FOGDYN_Cte_ActiveGaoMaxNb) )
	{
		for(i=0, j=-1; i<FOGDYN_gi_ActiveGaoNb ; i++)
		{
			if(FOGDYN_gap_ActiveGao[i] != ptr) continue;
			j=i;
			break;
		}

		if(j==-1)
		{
			FOGDYN_gap_ActiveGao[FOGDYN_gi_ActiveGaoNb] = ptr;
			FOGDYN_gi_ActiveGaoNb++;
		}
	}


	_pst_FOGDY = (GAO_tdst_ModifierFOGDY *)_pst_Mod->p_Data;

	pst_World = WOR_World_GetWorldOfObject(_pst_Mod->pst_GO);

	result = GFX_i_Add(&pst_World->pst_GFX , 13 , _pst_Mod->pst_GO); // Create the boum;
	ERR_X_Assert( result != -1 ); // the boum has not to boum itself -- Max 6 Feb 2003
	GFX = (USHORT) result;
	//GFX = pst_World->pst_GFX->uw_Id;
	if ((_pst_Mod->pst_GO->pst_Base) &&
		(_pst_Mod->pst_GO->pst_Base->pst_Visu) &&
		(_pst_Mod->pst_GO->pst_Base->pst_Visu->pst_Material))
		GFX_SetMaterial(pst_World->pst_GFX, GFX, (void *)_pst_Mod->pst_GO->pst_Base->pst_Visu->pst_Material);

	GFX_Seti(pst_World->pst_GFX , GFX, 13100, _pst_FOGDY -> ulNumberOfActiveSprites); // // Buffer number of sprite
	GFX_Seti(pst_World->pst_GFX , GFX, 13106, _pst_FOGDY -> ulNumberOfActiveSprites); // // number of sprite to generate

	_pst_FOGDY -> GFX_ID = GFX;
	GFX_FlagSet(pst_World->pst_GFX , GFX, 0 , 1);
	GFX_FlagSet(pst_World->pst_GFX , GFX, 2 , 1);

	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13110, _pst_FOGDY->SubMaterialNum1); // Material sub-mat num
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13111, _pst_FOGDY->SubMaterialNum2); // Material sub-mat num
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13112, _pst_FOGDY->SubMaterialNum3); // Material sub-mat num
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13113, _pst_FOGDY->SubMaterialNum4); // Material sub-mat num
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13103, _pst_FOGDY->Colors1); // // Color fase 0
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13104, _pst_FOGDY->Colors2); // // Color fase 1
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13105, _pst_FOGDY->Colors3); // // Color fase 2
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13107, 1); // // Transparent
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13000, _pst_FOGDY->GrowingMin); // // Grwing speed min
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13001, _pst_FOGDY->GrowingMax); // // Grwing speed max
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13002, _pst_FOGDY->FrictionGrow ); // // Friction Grow
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13003, _pst_FOGDY->TimePhase1); // // Time fase 1
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13004, _pst_FOGDY->TimePhase2); // // Time fase 2
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13005, _pst_FOGDY->SizeMin); // // Creation size min
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13006, _pst_FOGDY->SizeMax); // // Creation size max
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13007, _pst_FOGDY->Gravitiy); // // Gravity
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13008, _pst_FOGDY->GenerationRate); // // generation rate

	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13009, _pst_FOGDY->SpeedMin); // // Norm speed min
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13010, _pst_FOGDY->SpeedMax); // // Norm speed max
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13012, _pst_FOGDY->TimeVariance); // // dephasage
	MATH_InitVector(&stLocal ,1.0f,1.0f,_pst_FOGDY->ExtractionSpeed);
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13201, &stLocal); // // Speed min
	MATH_InitVector(&stLocal ,-1.0f,-1.0f,0.0f);
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13202, &stLocal); // // Speed max
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13203, &_pst_FOGDY->FrictionSpeed); // // friction speed
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13200, &_pst_Mod->pst_GO->pst_GlobalMatrix->T ); // // Creation Pos
#endif
}
/*
=======================================================================================================================
=======================================================================================================================
*/
#define IS_Finished(A) ((*(ULONG *)A) == 1)
void GAO_ModifierFOGDY_Manage(MDF_tdst_Modifier *_pst_Mod)
{
#if !defined(XML_CONV_TOOL)
	GAO_tdst_ModifierFOGDY *_pst_FOGDY; 
	MATH_tdst_Vector stLocal,stZoom;
	MATH_tdst_Vector		*p_SphereCenter,*p_SphereSpeed,*p_stGroundNormale;
	float					f_SphereRadius;
	float					Factor;
	WOR_tdst_World		*pst_World;
	GFX_tdst_List		*pst_GFX;
	GFX_tdst_Explode	*pExplode;
	OBJ_tdst_GameObject	*ptr;
	
	u32 SphereCounter;

	pst_World = WOR_World_GetWorldOfObject(_pst_Mod->pst_GO);
	_pst_FOGDY = (GAO_tdst_ModifierFOGDY *)_pst_Mod->p_Data;
	if ((_pst_FOGDY ->CollisionMode == 3) || (_pst_FOGDY ->CollisionMode == 1))// SparkMode || dust Mode
	{
		MATH_tdst_Matrix    st_Matrix3 ONLY_PSX2_ALIGNED(16);
		MATH_tdst_Matrix    st_Matrix2 ONLY_PSX2_ALIGNED(16);

		MATH_SetIdentityMatrix(&st_Matrix2);
		MATH_SetIdentityMatrix(&st_Matrix3);
		MATH_MakeOGLMatrix(&st_Matrix2, _pst_Mod->pst_GO->pst_GlobalMatrix);
		MATH_Invert33Matrix(&st_Matrix3, &st_Matrix2);
		/* Inverts the 3x3 matrix */
		MATH_NegVector(MATH_pst_GetTranslation(&st_Matrix3), MATH_pst_GetTranslation(_pst_Mod->pst_GO->pst_GlobalMatrix));
		MATH_TransformVector(&st_Matrix3.T, &st_Matrix3, &st_Matrix3.T);

		SphereCounter = p_LastCurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres;

		MATH_GetScale(&stZoom, _pst_Mod->pst_GO->pst_GlobalMatrix);
		Factor = 1.0f / ((stZoom.x + stZoom.y) * 0.5f);

		while (SphereCounter--)
		{
			if ((p_LastCurrentSphere -> FOGDYN_PERT_SphereChannel[SphereCounter] & _pst_FOGDY->ActiveChannel) & 0xffff)
			{
				p_SphereCenter =&p_LastCurrentSphere -> FOGDYN_PERT_SphereCenters[SphereCounter];
				f_SphereRadius = p_LastCurrentSphere -> FOGDYN_PERT_SphereRadius[SphereCounter] * Factor;
				p_SphereSpeed = &p_LastCurrentSphere -> FOGDYN_PERT_SphereSpeed[SphereCounter];
				ptr = p_LastCurrentSphere -> FOGDYN_PERT_Gao[SphereCounter];

				MATH_TransformVertex(&stLocal , &st_Matrix3 , p_SphereCenter );

				if ((fAbs(stLocal.x)<_pst_FOGDY->XSize + f_SphereRadius) &&
					(fAbs(stLocal.y)<_pst_FOGDY->YSize + f_SphereRadius) &&
					(fAbs(stLocal.z)<_pst_FOGDY->ZSize + f_SphereRadius) ) 
				{
					float 	SpeedNorm ;
					SpeedNorm = MATH_f_NormVector(p_SphereSpeed);

					if (SpeedNorm >= _pst_FOGDY->SpeedStart) 
					{
						if (SpeedNorm > _pst_FOGDY->MaxSpeed) SpeedNorm = _pst_FOGDY->MaxSpeed;
						if (_pst_FOGDY ->CollisionMode == 1) 
						{
							// DUST
							u32 ulNumberOfSpritesOriginals;
							/* Setup position & direction */
							ulNumberOfSpritesOriginals = _pst_FOGDY -> ulNumberOfActiveSprites;
							if (_pst_FOGDY->FrictionLenght != 0)
							{
								float SpeedNorm2;
								u32 Infinite;
								MATH_tdst_Vector		SphereCenter2,SphereSpeed2;
								SphereCenter2 = *p_SphereCenter;
								MATH_ScaleVector(&SphereSpeed2, p_SphereSpeed, TIM_gf_dt);
								SpeedNorm2 = SpeedNorm * TIM_gf_dt;
								Infinite = 10;
								while ((SpeedNorm2 > _pst_FOGDY->FrictionLenght) && Infinite) 
								{
									GAO_ModifierFOGDY_CreateGfx(_pst_Mod, ptr);
									GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13200, &SphereCenter2 ); // // Creation Pos
									GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13009, SpeedNorm  * _pst_FOGDY->SpeedMin); // // Norm speed min
									GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13010, SpeedNorm  * _pst_FOGDY->SpeedMax); // // Norm speed max
									_pst_FOGDY->GFX_ID = -1;
									SpeedNorm2 -= _pst_FOGDY->FrictionLenght;
									MATH_AddScaleVector(&SphereCenter2 , &SphereCenter2 , &SphereSpeed2 , -_pst_FOGDY->FrictionLenght);
									Infinite--;
								}
							} else
							{
								GAO_ModifierFOGDY_CreateGfx(_pst_Mod, ptr);
								GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13200, p_SphereCenter ); // // Creation Pos
								GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13009, SpeedNorm  * _pst_FOGDY->SpeedMin); // // Norm speed min
								GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13010, SpeedNorm  * _pst_FOGDY->SpeedMax); // // Norm speed max
							}
							_pst_FOGDY->GFX_ID = -1;
							_pst_FOGDY -> ulNumberOfActiveSprites = ulNumberOfSpritesOriginals ;
						} else
							// SPARK
							GAO_ModifierFOGDY_PercuteDetected(_pst_Mod,p_SphereCenter,p_SphereSpeed,SpeedNorm);
					}
				}
			}
		}
		return;
	}


	if (_pst_FOGDY -> ulNumberOfActiveSprites < 3) return;

#ifdef ACTIVE_EDITORS	
	if (_pst_FOGDY->IsInit)
	{
		pst_GFX = GFX_IsExist(pst_World->pst_GFX , _pst_FOGDY ->GFX_ID);
		if (pst_GFX)
			GFX_i_SetDeath(&pst_World->pst_GFX, pst_GFX);
		_pst_FOGDY->IsInit = 0;
		_pst_FOGDY->GFX_ID = -1;
	}
#endif



	if (!GFX_IsExist(pst_World->pst_GFX , _pst_FOGDY ->GFX_ID))
	{
		GAO_ModifierFOGDY_CreateGfx(_pst_Mod, NULL);
	}
	pst_GFX = GFX_IsExist(pst_World->pst_GFX , _pst_FOGDY ->GFX_ID);
	pExplode = (GFX_tdst_Explode	*)pst_GFX->p_Data;

	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13101, 0); // Material sub-mat num
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13103, _pst_FOGDY->Colors1); // // Color fase 0
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13104, _pst_FOGDY->Colors2); // // Color fase 1
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13105, _pst_FOGDY->Colors3); // // Color fase 2
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13107, 1); // // Transparent
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13000, _pst_FOGDY->GrowingMin); // // Grwing speed min
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13001, _pst_FOGDY->GrowingMax); // // Grwing speed max
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13002, _pst_FOGDY->FrictionGrow ); // // Friction Grow
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13003, _pst_FOGDY->TimePhase1); // // Time fase 1
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13004, _pst_FOGDY->TimePhase2); // // Time fase 2
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13005, _pst_FOGDY->SizeMin); // // Creation size min
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13006, _pst_FOGDY->SizeMax); // // Creation size max
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13007, _pst_FOGDY->Gravitiy); // // Gravity
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13008, _pst_FOGDY->GenerationRate); // // generation rate

	MATH_GetScale(&stZoom, _pst_Mod->pst_GO->pst_GlobalMatrix);

	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13009, _pst_FOGDY->SpeedMin); // // Norm speed min
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13010, _pst_FOGDY->SpeedMax); // // Norm speed max
	GFX_Setf(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13012, _pst_FOGDY->TimeVariance); // // dephasage
	MATH_InitVector(&stLocal ,1000.0f,1000.0f,0.0f);
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13201, &stLocal); // // Speed min
	MATH_InitVector(&stLocal ,-1000.0f,-1000.0f,0.0f);
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13202, &stLocal); // // Speed max
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13203, &_pst_FOGDY->FrictionSpeed); // // friction speed



	MATH_AddScaleVector(&stLocal ,&_pst_Mod->pst_GO->pst_GlobalMatrix->T, MATH_pst_GetXAxis(_pst_Mod->pst_GO->pst_GlobalMatrix),-_pst_FOGDY->XSize * stZoom.x);
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13200, &stLocal ); // // Creation Pos
	/*	MATH_ScaleVector(&stLocal ,MATH_pst_GetXAxis(_pst_Mod->pst_GO->pst_GlobalMatrix),_pst_FOGDY->XSize * stZoom.x);
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13207, &stLocal); // // Xaxis*/
	MATH_ScaleVector(&stLocal ,MATH_pst_GetYAxis(_pst_Mod->pst_GO->pst_GlobalMatrix),_pst_FOGDY->YSize * stZoom.y);
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13208, &stLocal); // // Yaxis
	MATH_ScaleVector(&stLocal ,MATH_pst_GetZAxis(_pst_Mod->pst_GO->pst_GlobalMatrix),_pst_FOGDY->ZSize * stZoom.z);
	GFX_Setv(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13209, &stLocal); // // Yaxis
	GFX_Seti(pst_World->pst_GFX , _pst_FOGDY -> GFX_ID , 13106, _pst_FOGDY -> ulNumberOfActiveSprites); // // number of sprite to generate

	/* Apply Mechanics */
	SphereCounter = p_LastCurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres;
	while (SphereCounter--)
	{
		u32 SpriteCounter;
		u32 *pTimeE;
		SOFT_tdst_AVertex		*p_Pos___SZ;
		SOFT_tdst_AVertex		*p_Speed_GS;

		MATH_tdst_Vector		stLocalWIND;
		float					stGroundDistance;

		p_stGroundNormale = MATH_pst_GetZAxis(_pst_Mod->pst_GO->pst_GlobalMatrix);
		stGroundDistance = MATH_f_DotProduct( p_stGroundNormale , &_pst_Mod->pst_GO->pst_GlobalMatrix->T);


		p_SphereCenter =&p_LastCurrentSphere -> FOGDYN_PERT_SphereCenters[SphereCounter];
		f_SphereRadius = p_LastCurrentSphere -> FOGDYN_PERT_SphereRadius[SphereCounter];
		p_SphereSpeed = &p_LastCurrentSphere -> FOGDYN_PERT_SphereSpeed[SphereCounter];

		MATH_SubVector(&stLocal,p_SphereCenter ,&_pst_Mod->pst_GO->pst_GlobalMatrix->T);


		pTimeE = (u32 *)pExplode->p_TimeElapsed;
		p_Pos___SZ = pExplode->p_Pos___SZ;
		p_Speed_GS = pExplode->p_Speed_GS;
		SpriteCounter = pExplode->ulNumberOfSprites;
		stLocalWIND .x = TIM_gf_dt * 50.1f;
		switch (_pst_FOGDY ->CollisionMode)
		{
		case 2:
			while (SpriteCounter--)
			{
				if (*pTimeE != 1)
				{
					float Distance;
					MATH_SubVector(&stLocal,(MATH_tdst_Vector *)p_Pos___SZ,p_SphereCenter);
					Distance = MATH_f_SqrNormVector(&stLocal);
					if (Distance < f_SphereRadius * f_SphereRadius)
					{
						/* Push it along radius */
						MATH_SetNormVector(&stLocal,&stLocal,f_SphereRadius - fSqrt(Distance));
						stLocal.z = 0;
						MATH_AddScaleVector((MATH_tdst_Vector *)p_Pos___SZ,(MATH_tdst_Vector *)p_Pos___SZ,&stLocal,_pst_FOGDY ->ExtractionSpeed);
						MATH_AddScaleVector((MATH_tdst_Vector *)p_Speed_GS,(MATH_tdst_Vector *)p_Speed_GS,&stLocal,_pst_FOGDY ->PushPower );
						//							*(float *)pTimeE += _pst_FOGDY->TimePhase2 * 0.005f;
					}
					MATH_AddVector((MATH_tdst_Vector *)p_Pos___SZ,(MATH_tdst_Vector *)p_Pos___SZ,&stLocalWIND);
					MATH_SubVector(&stLocal,(MATH_tdst_Vector *)p_Pos___SZ,&_pst_Mod->pst_GO->pst_GlobalMatrix->T);
					if ((fAbs(stLocal.x)>_pst_FOGDY->XSize) ||
						(fAbs(stLocal.y)>_pst_FOGDY->YSize) ||
						(fAbs(stLocal.z)>_pst_FOGDY->ZSize) )
						*(float *)pTimeE += _pst_FOGDY->TimePhase2 * 0.01f;
				}
				p_Pos___SZ ++;
				p_Speed_GS ++;
				pTimeE ++ ;
			}
			break;
		case 1://Dust Mode
			while (SpriteCounter--)
			{
				if (*pTimeE != 1)
				{
					float Distance;
					MATH_SubVector(&stLocal,(MATH_tdst_Vector *)p_Pos___SZ,p_SphereCenter);
					Distance = MATH_f_SqrNormVector(&stLocal);
					if (Distance < f_SphereRadius * f_SphereRadius)
					{
						/* AllowItTo growUp */
						p_Speed_GS->w	= fRand(_pst_FOGDY->GrowingMin,_pst_FOGDY->GrowingMax);
						/* Extract of Ground */
					}
				}
				p_Pos___SZ ++;
				p_Speed_GS ++;
				pTimeE ++ ;
			}
			break;
		}
	}

#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/


void GAO_ModifierFOGDY_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	GAO_tdst_ModifierFOGDY *p_FOGDY;
	p_FOGDY = (GAO_tdst_ModifierFOGDY *)_pst_Mod->p_Data;
}
/*
=======================================================================================================================
=======================================================================================================================
*/
#ifdef ACTIVE_EDITORS
extern void OGL_DrawLineEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawLineEx *_pst_Data);
void GAO_ModifierFOGDY_DrawDebugSphere(MATH_tdst_Matrix		*p_stSphereMatrix, MATH_tdst_Vector *pCenter , float Radius , u32 Color)
{
#if !defined(XML_CONV_TOOL)
	MATH_tdst_Matrix				stCurrentInverted,st_Matrix2;
	MATH_tdst_Vector					V1,stZoom;
	MATH_tdst_Vector					stX[2],stZ[2];
	MATH_tdst_Vector					stC[2];
	GDI_tdst_Request_DrawLineEx stDLE;

	u32								Counter2;
	float Factor,DCos,DSin;

	MATH_SetIdentityMatrix(&st_Matrix2);
	MATH_SetIdentityMatrix(&stCurrentInverted);
	MATH_MakeOGLMatrix(&st_Matrix2, p_stSphereMatrix);
	MATH_Invert33Matrix(&stCurrentInverted, &st_Matrix2);
	/* Inverts the 3x3 matrix */
	MATH_NegVector(&stCurrentInverted.T, MATH_pst_GetTranslation(p_stSphereMatrix));
	MATH_TransformVector(&stCurrentInverted.T, &stCurrentInverted, &stCurrentInverted.T);
	MATH_GetScale(&stZoom, p_stSphereMatrix);
	Factor = 1.0f / ((stZoom.x + stZoom.y) * 0.5f);
#define SphereDiscreet 40
	DCos = fCos(2.0f * 3.1415927f / SphereDiscreet);
	DSin = fSin(2.0f * 3.1415927f / SphereDiscreet);
	{
		MATH_TransformVertex(&V1 , &stCurrentInverted , pCenter);
		MATH_TransformVector(&stZ[0] , &stCurrentInverted , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
		MATH_TransformVector(&stZ[1] , &stCurrentInverted , MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
		MATH_NormalizeEqualVector(&stZ[0]);
		MATH_NormalizeEqualVector(&stZ[1]);
		stDLE.A = &stC[0];
		stDLE.B = &stC[1];
		stDLE.f_Width = 2.0f;
		stDLE.ul_Color = Color;

		Counter2 = SphereDiscreet;
		MATH_InitVector(&stX[1] , Factor * Radius, 0.0f , 0.0f);
		MATH_AddScaleVector(&stC[1],&V1,&stZ[0],stX[1].x);
		MATH_AddScaleVector(&stC[1],&stC[1],&stZ[1],stX[1].y);
		while (Counter2--)
		{
			stC[0] = stC[1];
			stX[0] = stX[1];
			stX[1].x = DCos * stX[0].x + DSin * stX[0].y;
			stX[1].y = -DSin * stX[0].x + DCos * stX[0].y;
			MATH_AddScaleVector(&stC[1],&V1,&stZ[0],stX[1].x);
			MATH_AddScaleVector(&stC[1],&stC[1],&stZ[1],stX[1].y);
#ifdef JADEFUSION
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&stDLE);
#else
			OGL_DrawLineEx(GDI_gpst_CurDD, &stDLE);
#endif
		}

	}
#endif // XML_CONV_TOOL
}
#endif
void GAO_ModifierFOGDY_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
#if !defined(XML_CONV_TOOL)
	GAO_tdst_ModifierFOGDY *p_FOGDY; 
	GDI_tdst_Request_DrawLineEx stDLE;
	ULONG Counter;
	GEO_Vertex *pParser;
	MATH_tdst_Vector A[8];

	if (_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;

	p_FOGDY = (GAO_tdst_ModifierFOGDY *)_pst_Mod->p_Data;

	if (p_FOGDY->XSize == -1000000000.0f)
	{
		Counter = _pst_Obj->l_NbPoints;
		pParser = _pst_Obj->dst_Point;

		while (Counter--)
		{
			if (fAbs(pParser->x) > p_FOGDY->XSize) p_FOGDY->XSize = fAbs(pParser->x);
			if (fAbs(pParser->y) > p_FOGDY->YSize) p_FOGDY->YSize = fAbs(pParser->y);
			pParser++;
		}
	}

#ifdef ACTIVE_EDITORS

	if (_pst_Mod->pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
	{
		A[4].x = A[0].x = p_FOGDY->XSize;
		A[5].x = A[1].x = -p_FOGDY->XSize;
		A[6].x = A[2].x = -p_FOGDY->XSize;
		A[7].x = A[3].x = p_FOGDY->XSize;
		A[4].y = A[0].y = p_FOGDY->YSize;
		A[5].y = A[1].y = p_FOGDY->YSize;
		A[6].y = A[2].y = -p_FOGDY->YSize;
		A[7].y = A[3].y = -p_FOGDY->YSize;
		A[0].z = A[1].z = A[2].z = A[3].z = -p_FOGDY->ZSize;
		A[4].z = A[5].z = A[6].z = A[7].z = p_FOGDY->ZSize;

		stDLE.f_Width = 5.0f;
		stDLE.ul_Color = 0x804040;

		for (Counter = 0 ; Counter < 4; Counter ++)
		{
			stDLE.A = &A[Counter];
			stDLE.B = &A[(Counter + 1) & 3];
#ifdef JADEFUSION
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&stDLE);
#else
			OGL_DrawLineEx(GDI_gpst_CurDD, &stDLE);
#endif
			stDLE.A = &A[Counter + 4];
			stDLE.B = &A[((Counter + 1) & 3) + 4];
#ifdef JADEFUSION
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&stDLE);
#else
			OGL_DrawLineEx(GDI_gpst_CurDD, &stDLE);
#endif
			stDLE.A = &A[Counter];
			stDLE.B = &A[Counter + 4];
#ifdef JADEFUSION
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG)&stDLE);
#else
			OGL_DrawLineEx(GDI_gpst_CurDD, &stDLE);
#endif
		}
		/* For each Sphere thats Colide */
		{
			Counter = p_LastCurrentSphere -> FOGDYN_PERT_ulNumberOfSpheres;
			while (Counter--)
			{
				GAO_ModifierFOGDY_DrawDebugSphere(_pst_Mod->pst_GO->pst_GlobalMatrix, &p_LastCurrentSphere -> FOGDYN_PERT_SphereCenters[Counter], p_LastCurrentSphere -> FOGDYN_PERT_SphereRadius[Counter], 0x404080);
			}

		}
	}
#endif
	GAO_ModifierFOGDY_Manage(_pst_Mod);
#endif // XML_CONV_TOOL
}

void GAO_ModifierFOGDY_Init(GAO_tdst_ModifierFOGDY *p_FOGDY)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierFOGDY_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierFOGDY *p_FOGDY;
	p_FOGDY = (GAO_tdst_ModifierFOGDY *)_pst_Mod->p_Data;
	/*
		
	*/

	_pst_Mod ->ul_Flags |= MDF_C_Modifier_ApplyGao;
}


#ifdef ACTIVE_EDITORS
#define FOGDY_SAV_LOAD_Buffer(ValueA , TypeC)	ulSize += sizeof(ValueA); if (Flags & 1) SAV_Buffer(&ValueA , sizeof(ValueA)); if (Flags & 2) ValueA = LOA_Read##TypeC(_pc_ReadBuffer);
#else
#define FOGDY_SAV_LOAD_Buffer(ValueA , TypeC)	ulSize += sizeof(ValueA); if (Flags & 2) ValueA = LOA_Read##TypeC(_pc_ReadBuffer);
#endif

#if defined(XML_CONV_TOOL)
ULONG gGaoFogDynEmtrVer;
#endif

ULONG FOGDY_Modifier_SaveLoad_Parrams(GAO_tdst_ModifierFOGDY *_pst_FOGDY, ULONG Flags, char **_pc_ReadBuffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							ulSize,ulVersion;
	ULONG							Dummy;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Dummy = 0;
	ulSize = 0;
	if (Flags & 1) // Save
	{
		ulVersion = 11; // Increment Last version Here
	}
	if (Flags & 2) // Read
	{
		GAO_ModifierFOGDY_Init(_pst_FOGDY);
	}
#if defined(XML_CONV_TOOL)
	else
		ulVersion = gGaoFogDynEmtrVer;
#endif

	/* SaveLoad Version */
	FOGDY_SAV_LOAD_Buffer(ulVersion, Long);

#if defined(XML_CONV_TOOL)
	if (Flags & 2) // Read
		gGaoFogDynEmtrVer = ulVersion;
#endif

	//FOGDYN_PERT_SAV_LOAD_Buffer(_pst_SPG2->ulFlags, Long);
	if (ulVersion < 3)
	{
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->XSize, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->YSize, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->ZSize, Float);
	}
#if !defined(XML_CONV_TOOL)
	_pst_FOGDY->XSize = -1000000000.0f;
	_pst_FOGDY->YSize = -1000000000.0f;
#endif
	if (ulVersion >= 1)
	{
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->Colors1, Long);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->Colors2, Long);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->Colors3, Long);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->TimePhase1, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->TimePhase2, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->SpeedMin, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->SpeedMax, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->SizeMin, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->SizeMax, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->GrowingMin, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->GrowingMax, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->FrictionGrow, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->FrictionSpeed.x, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->FrictionSpeed.y, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->FrictionSpeed.z, Float);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->Gravitiy, Float);
		if (ulVersion >= 2)
		{
			FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->TimeVariance, Float);
			FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->ulNumberOfActiveSprites, Long);
			FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->GenerationRate, Float);
			if (ulVersion >= 4)
			{
				FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->ZSize, Float);
				if (ulVersion >= 5)
				{
					FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->CollisionMode, Long);
					if (ulVersion >= 6)
					{
						FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->PushPower, Float);
						if (ulVersion >= 7)
						{
							FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->ExtractionSpeed, Float);
							if (ulVersion >= 8)
							{
								FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->FrictionLenght, Float);
								if (ulVersion >= 9)
								{
									FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->ActiveChannel, Long);
									FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->SubMaterialNum1, Long);
									if (ulVersion >= 10)
									{
										FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->SubMaterialNum2, Long);
										FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->SubMaterialNum3, Long);
										FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->SubMaterialNum4, Long);
										if (ulVersion >= 11)
										{
											FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->SpeedStart, Float);
											FOGDY_SAV_LOAD_Buffer(_pst_FOGDY->MaxSpeed, Float);
										}
									}
								} else
									_pst_FOGDY->ActiveChannel = -1;
							}
						}
					}
				}
			}
		}


	}

#ifdef ACTIVE_EDITORS
	_pst_FOGDY->ulCodeKey = 0xC0DE2002;
#endif
	_pst_FOGDY ->GFX_ID = -1;
	//	_pst_FOGDY ->GFX_SPARK_ID = -1;
	return ulSize;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
#ifdef ACTIVE_EDITORS
#if defined(XML_CONV_TOOL)
static ULONG FOGDY_Size = 0;
#endif
void FOGDY_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierFOGDY				*_pst_FOGDY;
	ULONG								ulSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_FOGDY = (GAO_tdst_ModifierFOGDY *) _pst_Mod->p_Data;
	/* Save Size */
#if !defined(XML_CONV_TOOL)
	ulSize = FOGDY_Modifier_SaveLoad_Parrams(_pst_FOGDY , 0 , NULL);
#else
	ulSize = FOGDY_Size;
#endif
	_pst_FOGDY->p_GO = _pst_Mod->pst_GO;
	SAV_Buffer(&ulSize, 4);
	FOGDY_Modifier_SaveLoad_Parrams(_pst_FOGDY , 1 , NULL);
}
#endif

/*
=======================================================================================================================
=======================================================================================================================
*/

ULONG FOGDY_ul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierFOGDY				*_pst_FOGDY;
	char								*pc_Cur;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Cur = _pc_Buffer;
	_pst_FOGDY = (GAO_tdst_ModifierFOGDY *) _pst_Mod->p_Data;
	/* Skip size */
#if !defined(XML_CONV_TOOL)
	LOA_ReadLong(&pc_Cur);
#else
	FOGDY_Size = LOA_ReadLong(&pc_Cur);
#endif 
	FOGDY_Modifier_SaveLoad_Parrams(_pst_FOGDY , 2, &pc_Cur);
#ifdef ACTIVE_EDITORS
	_pst_FOGDY->p_GO = _pst_Mod->pst_GO;
#endif

	return pc_Cur - _pc_Buffer ;
}

#ifdef ACTIVE_EDITORS
int FogDy_Modifier_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierFOGDY	*Src, *Dst ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Src = (GAO_tdst_ModifierFOGDY *) _pst_Src->p_Data;
	Dst = (GAO_tdst_ModifierFOGDY *) _pst_Dst->p_Data;

	/* copy */
	memcpy(Dst , Src , sizeof(GAO_tdst_ModifierFOGDY));
	return 1;
}
#endif

/**********************************************************************************************/
/**********************************************************************************************/
/***********   Dynamic fog spheres emiter   ***************************************************/
/**********************************************************************************************/
/**********************************************************************************************/
#ifdef USE_DOUBLE_RENDERING	
void GAO_ModifierFOGDY_Emtr_Interpolate(struct MDF_tdst_Modifier_ * p_Mod, u32 mode , float fInterpolatedValue) {};
#endif
void GAO_ModifierFOGDY_Emtr_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierFOGDY_Emiter));

	if (p_Data == NULL)
	{
		/* First init */
		L_memset(_pst_Mod->p_Data , 0 , sizeof(GAO_tdst_ModifierFOGDY_Emiter));
		_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
	}
	else
	{
		GAO_tdst_ModifierFOGDY_Emiter *p_FOGDY;
		_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
		p_FOGDY = (GAO_tdst_ModifierFOGDY_Emiter *)_pst_Mod->p_Data;
		L_memcpy( _pst_Mod->p_Data, p_Data, sizeof (GAO_tdst_ModifierFOGDY_Emiter) );
	}
#ifdef ACTIVE_EDITORS
	((GAO_tdst_ModifierFOGDY_Emiter *)_pst_Mod->p_Data) ->ulCodeKey = 0xC0DE2002;
	((GAO_tdst_ModifierFOGDY_Emiter *)_pst_Mod->p_Data) ->ActiveChannel = -1;
#endif
}


void GAO_ModifierFOGDY_Emtr_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	int i, j;
	SPheresParrams *pSphere;

	pSphere = AllSpheres ;
	for(i=0; i<sizeof(AllSpheres) / sizeof(SPheresParrams); i++, pSphere++ )
	{
		for(j=0; j<MaxSpheres; j++)
		{
			if(_pst_Mod->pst_GO == pSphere->FOGDYN_PERT_Gao[j])
				pSphere->FOGDYN_PERT_Gao[j] = NULL;
		}
	}
	
	if (_pst_Mod->p_Data)
	{
		MEM_Free(_pst_Mod->p_Data);
	}
}
void GAO_ModifierFOGDY_Emtr_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj) {};
void FOGDY_GetBonePos(OBJ_tdst_GameObject *_pst_GO , GAO_tdst_ModifierFOGDY_Emiter *p_Emiter , u32 ul_MatrixNum, MATH_tdst_Vector *p_Center, u32 ul_MatrixI)
{
	if (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)
	{
		if ((u32)_pst_GO->pst_Base->pst_AddMatrix->l_Number > ul_MatrixNum) 
		{
			if (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
			{
				if (_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ul_MatrixNum].pst_GO)
				{
					*p_Center = _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ul_MatrixNum].pst_GO->pst_GlobalMatrix->T;
					MATH_AddScaleVector(p_Center , p_Center , MATH_pst_GetXAxis(&_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ul_MatrixNum].st_Matrix) , p_Emiter->BoneDelta[ul_MatrixI].x);
					MATH_AddScaleVector(p_Center , p_Center , MATH_pst_GetYAxis(&_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ul_MatrixNum].st_Matrix) , p_Emiter->BoneDelta[ul_MatrixI].y);
					MATH_AddScaleVector(p_Center , p_Center , MATH_pst_GetZAxis(&_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ul_MatrixNum].st_Matrix) , p_Emiter->BoneDelta[ul_MatrixI].z);
				}
			}
			else
			{
				*p_Center = _pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ul_MatrixNum].st_Matrix.T;
				MATH_AddScaleVector(p_Center , p_Center , MATH_pst_GetXAxis(&_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ul_MatrixNum].st_Matrix) , p_Emiter->BoneDelta[ul_MatrixI].x);
				MATH_AddScaleVector(p_Center , p_Center , MATH_pst_GetYAxis(&_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ul_MatrixNum].st_Matrix) , p_Emiter->BoneDelta[ul_MatrixI].y);
				MATH_AddScaleVector(p_Center , p_Center , MATH_pst_GetZAxis(&_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ul_MatrixNum].st_Matrix) , p_Emiter->BoneDelta[ul_MatrixI].z);
				MATH_TransformVertex(p_Center ,_pst_GO->pst_GlobalMatrix, p_Center );
			}
		}
		else
		{
			*p_Center = _pst_GO->pst_GlobalMatrix->T;
			MATH_AddScaleVector(p_Center , p_Center , MATH_pst_GetXAxis(_pst_GO->pst_GlobalMatrix) , p_Emiter->BoneDelta[ul_MatrixI].x);
			MATH_AddScaleVector(p_Center , p_Center , MATH_pst_GetYAxis(_pst_GO->pst_GlobalMatrix) , p_Emiter->BoneDelta[ul_MatrixI].y);
			MATH_AddScaleVector(p_Center , p_Center , MATH_pst_GetZAxis(_pst_GO->pst_GlobalMatrix) , p_Emiter->BoneDelta[ul_MatrixI].z);
		}
	}
}
void GAO_ModifierFOGDY_Emtr_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj) 
{
	u32 Counter;
	GAO_tdst_ModifierFOGDY_Emiter *p_Emitr;
	float SpeedScale;
	OBJ_tdst_GameObject*pst_GO;



	p_Emitr = (GAO_tdst_ModifierFOGDY_Emiter *)_pst_Mod->p_Data;
	if (((int)p_Emitr ->ActiveBoneNumber) < 0) p_Emitr ->ActiveBoneNumber = 0;

#ifdef ACTIVE_EDITORS
	if ((_pst_Mod->pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) &&  (p_Emitr->ulShowInfo))
	{
		Counter = p_Emitr ->ActiveBoneNumber ;
		while (Counter--)
		{
			if (p_Emitr ->BoneRadius[Counter] == 0.0f) p_Emitr ->BoneRadius[Counter] = 1.0f;
			GAO_ModifierFOGDY_DrawDebugSphere(_pst_Mod->pst_GO->pst_GlobalMatrix, &p_Emitr -> BoneCenters[Counter], p_Emitr ->BoneRadius[Counter], 0xff0000);
		}
	}
	if (p_Emitr ->ActiveBoneNumber > ModifierFOGDY_Emiter_BoneNUmber)
		p_Emitr ->ActiveBoneNumber = ModifierFOGDY_Emiter_BoneNUmber;
#endif
	SpeedScale = 1.0f / TIM_gf_dt;
	for (Counter = 0 ;Counter < p_Emitr ->ActiveBoneNumber ; Counter++)
	{
		p_Emitr -> BoneSpeeds[Counter] = p_Emitr -> BoneCenters[Counter];
		FOGDY_GetBonePos(_pst_Mod->pst_GO, p_Emitr , p_Emitr ->BoneID[Counter] , &p_Emitr -> BoneCenters[Counter],Counter);
		
		pst_GO = NULL;
		if(_pst_Mod->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
		{
			OBJ_tdst_Group			*pst_Skeleton;
			TAB_tdst_PFelem			*pst_CurrentBone;

			if(_pst_Mod->pst_GO->pst_Base && _pst_Mod->pst_GO->pst_Base->pst_GameObjectAnim && _pst_Mod->pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
			{
				pst_Skeleton = _pst_Mod->pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
				if(p_Emitr->BoneID[Counter] < pst_Skeleton->pst_AllObjects->ul_NbElems)
				{
					pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
					pst_CurrentBone +=  p_Emitr ->BoneID[Counter];
					if(!TAB_b_IsAHole(pst_CurrentBone))
						pst_GO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;	
				}
			}
		}

		MATH_SubVector(&p_Emitr -> BoneSpeeds[Counter] , &p_Emitr -> BoneCenters[Counter] , &p_Emitr -> BoneSpeeds[Counter]);
		MATH_ScaleEqualVector(&p_Emitr -> BoneSpeeds[Counter] , SpeedScale);
		FOGDyn_AddASphere(&p_Emitr ->BoneCenters [Counter], &p_Emitr -> BoneSpeeds[Counter], p_Emitr ->BoneRadius [Counter], p_Emitr ->ActiveChannel, pst_GO );//*/
	}

};		
void GAO_ModifierFOGDY_Emtr_Init(GAO_tdst_ModifierFOGDY_Emiter *p_FOGDY) {};
void GAO_ModifierFOGDY_Emtr_Reinit(MDF_tdst_Modifier *_pst_Mod) {};
ULONG FOGDY_Emtr_Modifier_SaveLoad_Parrams(GAO_tdst_ModifierFOGDY_Emiter *_pst_FOGDY_Emtr, ULONG Flags, char **_pc_ReadBuffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							ulSize,ulVersion;
	ULONG							Dummy,Counter	;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Dummy = 0;
	ulSize = 0;
	if (Flags & 1) // Save
	{
#if defined(XML_CONV_TOOL)
		ulVersion = gGaoFogDynEmtrVer;
#else
		ulVersion = 2; // Increment Last version Here
#endif
	}
	if (Flags & 2) // Read
	{
		GAO_ModifierFOGDY_Emtr_Init(_pst_FOGDY_Emtr);
	}
	/* SaveLoad Version */
	FOGDY_SAV_LOAD_Buffer(ulVersion, Long);

#if defined(XML_CONV_TOOL)
	if (Flags & 2) // Read
		gGaoFogDynEmtrVer = ulVersion;
#endif

	FOGDY_SAV_LOAD_Buffer(_pst_FOGDY_Emtr->ActiveBoneNumber, Long);
	Counter = ModifierFOGDY_Emiter_BoneNUmber;
	while (Counter--)
	{
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY_Emtr->BoneID[Counter], Long);
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY_Emtr->BoneRadius[Counter], Float);
		if (ulVersion >= 2)
		{
			FOGDY_SAV_LOAD_Buffer(_pst_FOGDY_Emtr->BoneDelta[Counter].x, Float);
			FOGDY_SAV_LOAD_Buffer(_pst_FOGDY_Emtr->BoneDelta[Counter].y, Float);
			FOGDY_SAV_LOAD_Buffer(_pst_FOGDY_Emtr->BoneDelta[Counter].z, Float);
		} else
			MATH_InitVector(&_pst_FOGDY_Emtr->BoneDelta[Counter] , 0.0f, 0.0f, 0.0f);
	}
	if (ulVersion >= 1)
	{
		FOGDY_SAV_LOAD_Buffer(_pst_FOGDY_Emtr->ActiveChannel, Long);
	} else
		_pst_FOGDY_Emtr->ActiveChannel = -1;
	
#ifdef ACTIVE_EDITORS
	_pst_FOGDY_Emtr->ulCodeKey = 0xC0DE2002;
#endif
	return ulSize;
}

#ifdef ACTIVE_EDITORS
int FOGDY_Emtr_Modifier_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierFOGDY_Emiter	*Src, *Dst ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Src = (GAO_tdst_ModifierFOGDY_Emiter *) _pst_Src->p_Data;
	Dst = (GAO_tdst_ModifierFOGDY_Emiter *) _pst_Dst->p_Data;
	memcpy(Dst , Src , sizeof(GAO_tdst_ModifierFOGDY_Emiter));
	return 1;
}
#if defined(XML_CONV_TOOL)
static ULONG FOGDY_Emtr_Size = 0;
#endif
void FOGDY_Emtr_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	ULONG								ulSize;
#if !defined(XML_CONV_TOOL)
	ulSize = FOGDY_Emtr_Modifier_SaveLoad_Parrams((GAO_tdst_ModifierFOGDY_Emiter *) _pst_Mod->p_Data, 0 , NULL);	/* Save Size */
#else
	ulSize = FOGDY_Emtr_Size;
#endif
	SAV_Buffer(&ulSize, 4);
	FOGDY_Emtr_Modifier_SaveLoad_Parrams((GAO_tdst_ModifierFOGDY_Emiter *) _pst_Mod->p_Data, 1 , NULL);
}
#endif
ULONG FOGDY_Emtr_ul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	char *pc_Cur;
	pc_Cur = _pc_Buffer;
#if !defined(XML_CONV_TOOL)
	LOA_ReadLong(&pc_Cur); /* Skip size */	
#else
	FOGDY_Emtr_Size = LOA_ReadLong(&pc_Cur);
#endif 
	FOGDY_Emtr_Modifier_SaveLoad_Parrams((GAO_tdst_ModifierFOGDY_Emiter *) _pst_Mod->p_Data , 2, &pc_Cur);
	_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
	AllSpheres[0].FOGDYN_PERT_ulNumberOfSpheres = 0;
	AllSpheres[1].FOGDYN_PERT_ulNumberOfSpheres = 0;
	return pc_Cur - _pc_Buffer ;
}



#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
