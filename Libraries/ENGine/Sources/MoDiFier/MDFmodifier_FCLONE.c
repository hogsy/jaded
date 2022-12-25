
#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"

#include "GDInterface/GDInterface.h"

#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_FCLONE.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/INTersection/INTmain.h"
#include "ENGine/Sources/OBJects/OBJculling.h"

int	NbrObjectClone = 0;//de base
int renderState_Cloned = 0;

//Calbut a l'air
MATH_tdst_Matrix LightMatrixClone[MAXLIGHTPERCLONE];
LIGHT_tdst_Light *LightClone[MAXLIGHTPERCLONE];
int NbCloneLight;

/*#ifdef ACTIVE_EDITORS
int CLONE_CurrentSelection = 0;
#endif*/

// a voir plus tard pour le metre ailleur !!
/*BOOL b_CloneCulled[MAXCLONE];*/
BOOL b_CloneLighted[MAXCLONE];

extern BOOL			ENG_gb_ActiveSectorization;

float rndf(float low,float high)
{
	return low + ( high - low ) * ( (float)rand() )/RAND_MAX;
}
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

	void FCLONE_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
	{
		FCLONE_tdst_Modifier * p_FCLONE;
		_pst_Mod->p_Data = MEM_p_Alloc(sizeof(FCLONE_tdst_Modifier));
		p_FCLONE = (FCLONE_tdst_Modifier *) _pst_Mod->p_Data;

		//	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao  ;

		if(p_Data == NULL)
		{
			NbrObjectClone = 0;
			p_FCLONE->NBR_Instances = 0;//avant 1
		}
		else
		{
			L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(FCLONE_tdst_Modifier));
		}

		//L_memset(&p_FCLONE->Cl_ElementDescriptor , 0 , sizeof(CLONE_ElementDescriptor));
		//p_FCLONE->cl_eData = MEM_p_AllocAlign(sizeof(CLONE_Data),32);
		p_FCLONE->cl_eData = (CLONE_Data*)MEM_p_Alloc(sizeof(CLONE_Data));
	}

	void FCLONE_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
	{
		if(_pst_Mod->p_Data)
		{
			FCLONE_tdst_Modifier   *p_FCLONE;
			p_FCLONE = (FCLONE_tdst_Modifier *) _pst_Mod->p_Data;

			if (p_FCLONE->cl_eData)
				//MEM_FreeAlign(p_FCLONE->cl_eData);	
				MEM_Free(p_FCLONE->cl_eData);	
			MEM_Free(_pst_Mod->p_Data);
		}
		NbrObjectClone = 0;
	}

	void FCLONE_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
	{

/*old		OBJ_tdst_GameObject *pst_TempGO,*Ref_GO;
		TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;

		FCLONE_tdst_Modifier * p_FCLONE;
		static BOOL Update=1;

		if ( !(GDI_gpst_CurDD->ul_DrawMask & GDI_Cul_DM_AutoClone) )
		{
			NbrObjectClone = 0;
			return;
		}

		p_FCLONE = (FCLONE_tdst_Modifier *) _pst_Mod->p_Data;
		if (p_FCLONE)
		{

			WOR_tdst_World *_pst_World;
			Ref_GO = GDI_gpst_CurDD->pst_CurrentGameObject;	

			//if (Update)
			{
				_pst_World = GDI_gpst_CurDD->pst_World;


				pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
				pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);
				p_FCLONE->NBR_Instances =0;
				for(; pst_Elem <= pst_LastElem; pst_Elem++)
				{
					// get and test game object
					pst_TempGO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
					if(TAB_b_IsAHole(pst_TempGO)) continue;

					if ( pst_TempGO->pst_Base )
						if ( pst_TempGO->pst_Base->pst_Visu )
							//if ( pst_TempGO->pst_Base->pst_Visu->pst_Object>0x500 )//?? ?? ?? ?? ?? ??
								if ( !(pst_TempGO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_AutoClone) ) 
									if (pst_TempGO->pst_Base->pst_Visu->pst_Object == Ref_GO->pst_Base->pst_Visu->pst_Object)
										p_FCLONE->NBR_Instances++;
				}
				Update = 0;
			}
			NbrObjectClone = p_FCLONE->NBR_Instances;
			if ( NbrObjectClone ) renderState_Cloned = 1;

#ifdef ACTIVE_EDITORS
			if (p_FCLONE->NBR_SavInstances != NbrObjectClone)
			{
				CLONE_Data *v;
				p_FCLONE->NBR_SavInstances = NbrObjectClone;
				FCLONE_Modifier_Update(_pst_Mod );

				pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
				pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);
				v = p_FCLONE->cl_eData;
				for(; pst_Elem <= pst_LastElem; pst_Elem++)
				{
					// get and test game object
					pst_TempGO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
					if(TAB_b_IsAHole(pst_TempGO)) continue;

					if ( pst_TempGO->pst_Base )
						if ( pst_TempGO->pst_Base->pst_Visu )
							//if ( pst_TempGO->pst_Base->pst_Visu->pst_Object>0x500 )//?? ?? ?? ?? ?? ?? ??
								if ( !(pst_TempGO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_AutoClone) ) 
									if (pst_TempGO->pst_Base->pst_Visu->pst_Object == Ref_GO->pst_Base->pst_Visu->pst_Object)
									{
										//v->vPos = pst_TempGO->pst_GlobalMatrix->T;
										v->Gao = pst_TempGO;
										v++;
									}
				}



			}
			{
				// LiGhTs
				LIGHT_tdst_List *_pst_LightList;
				register GDI_tdst_DisplayData 		*pst_DD;

				NbCloneLight=0;
				pst_DD = &GDI_gpst_CurDD_SPR;
				_pst_LightList = &pst_DD->st_LightList;

				if ( _pst_LightList->ul_Current )
				{
					OBJ_tdst_GameObject			**ppst_LightNode, **ppst_Last;

					ppst_LightNode = _pst_LightList->dpst_Light;
					ppst_Last = ppst_LightNode + _pst_LightList->ul_Current;
					while(ppst_LightNode < ppst_Last)
					{
						LIGHT_gpst_Cur = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;

						if ( LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_CloneLight )
						{
							LightMatrixClone[NbCloneLight] = *(*ppst_LightNode)->pst_GlobalMatrix;//<<<<<<<<<<<<< TEST CLONAGE
							LightClone[NbCloneLight] = LIGHT_gpst_Cur;
							NbCloneLight++;
							if (NbCloneLight > 32) ERR_X_Assert(NbCloneLight > 32);
							LightClone[NbCloneLight] = NULL;
						}			

						ppst_LightNode++;
					}
				}
			}
#endif
			{
				int i;
				//MATH_tdst_Vector vPosCam;
				float f_dist;
				MATH_tdst_Vector	st_dist;
				CLONE_Data *v;

				MATH_tdst_Matrix m_Temp;//,m_tempQ;

				m_Temp = *Ref_GO->pst_GlobalMatrix;

				//vPosCam.x = GDI_gpst_CurDD->st_Camera.st_Matrix.T.x;
				//vPosCam.y = GDI_gpst_CurDD->st_Camera.st_Matrix.T.y;
				//vPosCam.z = GDI_gpst_CurDD->st_Camera.st_Matrix.T.z;

				v = p_FCLONE->cl_eData;
				for (i=0;i<NbrObjectClone;i++)
				{
					int number;
					OBJ_tdst_GameObject* temp;
					temp = (OBJ_tdst_GameObject*)v->Gao;

					// a tester pour secto !!!
					if(ENG_gb_ActiveSectorization)
					{
						if (!(temp->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active))
						{
							b_CloneCulled[i] = FALSE;
							continue;
						}
					}
					if ( !(p_FCLONE->ulFlags & CLONE_Cull) )
					{
						MATH_SubVector(&st_dist, &temp->pst_GlobalMatrix->T, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
						f_dist = (float)(fabs(MATH_f_NormVector(&st_dist)));

						//if ( f_dist>60 ) 
						//{
						//b_CloneCulled[i] = TRUE;
						//v++;
						//continue;
						//}

						//MATH_ConvertQuaternionToMatrix(&m_tempQ, &v->qQ);
						//m_tempQ.T = v->vPos;
						*Ref_GO->pst_GlobalMatrix = *temp->pst_GlobalMatrix;//m_tempQ;

						b_CloneCulled[i] = OBJ_CullingObject(Ref_GO, &GDI_gpst_CurDD->st_Camera);
					}
					else b_CloneCulled[i] = FALSE;

					// Eclairage ??
					number = NbCloneLight;
					while (number--)
					{
						//if (!LightClone[number]) continue;
						if ( temp->pst_BV )
						{
							ULONG ulType;
							MATH_tdst_Vector    v2, *pv;
							ulType  =	LightClone[number]->ul_Flags & LIGHT_Cul_LF_Type;
							if (ulType == LIGHT_Cul_LF_Omni) //|| (ulType == LIGHT_Cul_LF_Spot))
							{

								pv = &temp->pst_GlobalMatrix->T;//OBJ_pst_GetAbsolutePosition( &v->Gao );
								MATH_SubVector( &v2, &LightMatrixClone[number].T, pv );

								if ( p_FCLONE->ulFlags & CLONE_BVReject )
								{
									if (!INT_SphereAABBox(&v2, LightClone[number]->st_Omni.f_Far, OBJ_pst_BV_GetGMin( temp->pst_BV ), OBJ_pst_BV_GetGMax( temp->pst_BV ) ))
										b_CloneLighted[i] = FALSE;
									else 
									{
										b_CloneLighted[i]= TRUE;
										break;
									}
								}
								else b_CloneLighted[i]= TRUE;
							}
							else 
								b_CloneLighted[i] = FALSE;
						}
					}

					v++;
				}
				*Ref_GO->pst_GlobalMatrix = m_Temp;
			}
		}
		//NbrObjectClone = 0;
*/
	}

	void FCLONE_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
	{
		NbrObjectClone =0;
		//old renderState_Cloned =0;
	}

#ifdef ACTIVE_EDITORS
	/*int FCLONE_Modifier_CurrentSelection()
	{
	return CLONE_CurrentSelection;
	}*/
	BOOL FCLONE_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg)
	{
		return TRUE;
	}
	void FCLONE_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		FCLONE_tdst_Modifier	*pst_Data;
		ULONG				ul_Size;
		//	int i;
		//	CLONE_Data *v;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Data = (FCLONE_tdst_Modifier *) _pst_Mod->p_Data;

		ul_Size = 4*2;
		SAV_Buffer(&ul_Size, 4);
		SAV_Buffer(&pst_Data->ulFlags, 4);
		return;

		/*	SAV_Buffer(&pst_Data->NBR_Instances, 4);
		//SAV_Buffer(&pst_Data->cl_eData,32);

		v = pst_Data->cl_eData;
		for (i=0;i<pst_Data->NBR_Instances;i++)
		{
		SAV_Buffer(&v->vPos,12);
		SAV_Buffer(&v->qQ,16);
		SAV_Buffer(&v->zoom,4);
		v++;
		}*/
	}
#endif

	void FCLONE_Modifier_Update(MDF_tdst_Modifier *_pst_Mod)
	{
		FCLONE_tdst_Modifier   *p_FCLONE;

		//if (p_FCLONE->NBR_Instances<1) return;
		p_FCLONE = (FCLONE_tdst_Modifier *) _pst_Mod->p_Data;
		if (p_FCLONE->NBR_Instances<1) return;

		//	p_FCLONE->cl_eData = MEM_p_ReallocAlign(p_FCLONE->cl_eData,sizeof(CLONE_Data) * p_FCLONE->NBR_Instances ,32);
		p_FCLONE->cl_eData = (CLONE_Data*)MEM_p_Realloc(p_FCLONE->cl_eData,sizeof(CLONE_Data) * p_FCLONE->NBR_Instances);
	}

	ULONG FCLONE_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		char                        *pc_Cur;
		FCLONE_tdst_Modifier   *pst_Data;
		ULONG i;
		CLONE_Data *v;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


		//float c;
		pst_Data = (FCLONE_tdst_Modifier *) _pst_Mod->p_Data;
		pc_Cur = _pc_Buffer;

		//	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao  ;

		// Size
		LOA_ReadLong_Ed(&pc_Cur, NULL); // skip size
		
		//return (pc_Cur - _pc_Buffer);
		pst_Data->ulFlags = LOA_ReadULong(&pc_Cur);

		return (pc_Cur - _pc_Buffer);

		pst_Data->NBR_Instances = LOA_ReadULong(&pc_Cur);
		//return (pc_Cur - _pc_Buffer);

#ifdef ACTIVE_EDITORS
		pst_Data->NBR_SavInstances = pst_Data->NBR_Instances;
#endif
		FCLONE_Modifier_Update(_pst_Mod );

		v = pst_Data->cl_eData;
		for (i=0;i<pst_Data->NBR_Instances;i++)
		{
			/*	LOA_ReadVector(&pc_Cur,&v->vPos);
			LOA_ReadQuaternion(&pc_Cur,&v->qQ);
			v->zoom = LOA_ReadFloat(&pc_Cur);*/
			v++;
		}

		//c = LOA_ReadFloat(&pc_Cur);
		return (pc_Cur - _pc_Buffer);

#if defined (__cplusplus) && !defined(JADEFUSION)
	}
#endif
}
