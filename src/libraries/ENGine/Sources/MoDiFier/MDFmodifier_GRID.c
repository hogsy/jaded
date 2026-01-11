#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_Grid.h"
#include "WORld/WORstruct.h"
#include "WORld/WORaccess.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_compute.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Grid_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	GRID_tdst_World			*pst_Grid;
	GRID_tdst_World			*pst_Grid1;
	MATH_tdst_Vector		st_Max, st_Min;
	int						XMax, YMax;
	int						XMin, YMin;
	int						XMax1, YMax1;
	int						XMin1, YMin1;
	int						i, j, k, t;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	if(pst_World)
	{
		pst_Grid = pst_World->pst_Grid;
		pst_Grid1 = pst_World->pst_Grid1;
	}
	else
	{
		pst_Grid = NULL;
		pst_Grid1 = NULL;
	}

	MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(_pst_GO->pst_BV), &_pst_GO->pst_GlobalMatrix->T);
	MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(_pst_GO->pst_BV), &_pst_GO->pst_GlobalMatrix->T);

	if(pst_Grid)
	{
		GRI_3Dto2D(pst_Grid, &st_Min, &XMin, &YMin);
		GRI_3Dto2D(pst_Grid, &st_Max, &XMax, &YMax);
	}

	if(pst_Grid1)
	{
		GRI_3Dto2D(pst_Grid1, &st_Min, &XMin1, &YMin1);
		GRI_3Dto2D(pst_Grid1, &st_Max, &XMax1, &YMax1);
	}

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(Grid_tdst_Modifier));
	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	((Grid_tdst_Modifier *) _pst_Mod->p_Data)->uc_Version = 1;
	((Grid_tdst_Modifier *) _pst_Mod->p_Data)->uc_Dummy = 0;
	((Grid_tdst_Modifier *) _pst_Mod->p_Data)->uw_Dummy = 0;

	((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa = NULL;
	((Grid_tdst_Modifier *) _pst_Mod->p_Data)->ul_Num = 0;

	((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa1 = NULL;
	((Grid_tdst_Modifier *) _pst_Mod->p_Data)->ul_Num1 = 0;

	((Grid_tdst_Modifier *) _pst_Mod->p_Data)->ul_Rotate = 0;

	if(!p_Data)
	{
		if(pst_Grid && 0)
		{
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa = (char *) MEM_p_Alloc((XMax - XMin + 1) * (YMax - YMin + 1));
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->ul_Num = (XMax - XMin + 1) * (YMax - YMin + 1);

			k = 0;
			for(i = XMin; i <= XMax; i++)
			{
				for(j = YMin; j <= YMax; j++)
				{
					t = (int) pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup;
					t = t * j + i;

					((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa[k++] = (UCHAR) pst_Grid->pst_EditArray[t].c_Capacities;
				}
			}
		}
		else
		{
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa = NULL;
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->ul_Num = 0;
		}


		if(pst_Grid1 && 0)
		{
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa1 = (char *) MEM_p_Alloc((XMax1 - XMin1 + 1) * (YMax1 - YMin1 + 1));
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->ul_Num1 = (XMax1 - XMin1 + 1) * (YMax1 - YMin1 + 1);

			k = 0;
			for(i = XMin1; i <= XMax1; i++)
			{
				for(j = YMin1; j <= YMax1; j++)
				{
					t = (int) pst_Grid1->w_NumGroupsX * pst_Grid1->c_SizeGroup;
					t = t * j + i;

					((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa1[k++] = (UCHAR) pst_Grid1->pst_EditArray[t].c_Capacities;
				}
			}

		}
		else
		{
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa1 = NULL;
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->ul_Num1 = 0;
		}
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(Grid_tdst_Modifier));

		if(((Grid_tdst_Modifier *) p_Data)->pc_Capa)
		{
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa = (char *) MEM_p_Alloc(((Grid_tdst_Modifier *) p_Data)->ul_Num);
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->ul_Num = ((Grid_tdst_Modifier *) p_Data)->ul_Num;

			L_memcpy(((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa, ((Grid_tdst_Modifier *) p_Data)->pc_Capa, ((Grid_tdst_Modifier *) p_Data)->ul_Num);
		}

		if(((Grid_tdst_Modifier *) p_Data)->pc_Capa1)
		{
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa1 = (char *) MEM_p_Alloc(((Grid_tdst_Modifier *) p_Data)->ul_Num1);
			((Grid_tdst_Modifier *) _pst_Mod->p_Data)->ul_Num1 = ((Grid_tdst_Modifier *) p_Data)->ul_Num1;
			L_memcpy(((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa1, ((Grid_tdst_Modifier *) p_Data)->pc_Capa1, ((Grid_tdst_Modifier *) p_Data)->ul_Num1);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Grid_Modifier_CopyPaste(MDF_tdst_Modifier *_pst_Mod, BOOL _b_Copy)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	GRID_tdst_World			*pst_Grid;
	GRID_tdst_World			*pst_Grid1;
	Grid_tdst_Modifier		*pst_GridMdF;
	MATH_tdst_Vector		st_Min, st_Max;
	int						XMax, YMax;
	int						XMin, YMin;
	int						XMax1, YMax1;
	int						XMin1, YMin1;
	int						i, j, k, t;
	int						x1, y1;
	float					fXMin, fYMin;
	float					fXMax, fYMax;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Mod || !_pst_Mod->p_Data || !_pst_Mod->pst_GO) return;

	pst_GridMdF = (Grid_tdst_Modifier *) _pst_Mod->p_Data;

	if(!_b_Copy && !pst_GridMdF->pc_Capa &&!pst_GridMdF->pc_Capa1) return;


	pst_World = WOR_World_GetWorldOfObject(_pst_Mod->pst_GO);

	MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(_pst_Mod->pst_GO->pst_BV), &_pst_Mod->pst_GO->pst_GlobalMatrix->T);
	MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(_pst_Mod->pst_GO->pst_BV), &_pst_Mod->pst_GO->pst_GlobalMatrix->T);

	if(!pst_World) return;

	pst_Grid = pst_World->pst_Grid;
	pst_Grid1 = pst_World->pst_Grid1;

	if(_b_Copy && pst_GridMdF->pc_Capa)
	{
		MEM_Free(pst_GridMdF->pc_Capa);
		pst_GridMdF->pc_Capa = NULL;

	}

	if(_b_Copy && pst_GridMdF->pc_Capa1)
	{
		MEM_Free(pst_GridMdF->pc_Capa1);
		pst_GridMdF->pc_Capa1 = NULL;
	}

	if(pst_Grid)
	{
		GRI_3Dto2D_Float(pst_Grid, &st_Min, &fXMin, &fYMin);
		GRI_3Dto2D_Float(pst_Grid, &st_Max, &fXMax, &fYMax);

		XMin = (int) fXMin;
		XMax = (int) fXMax;
		YMin = (int) fYMin;
		YMax = (int) fYMax;

		fXMin -= (float) XMin;
		fXMax -= (float) XMax;
		fYMin -= (float) YMin;
		fYMax -= (float) YMax;

		if(_b_Copy)
		{
			pst_GridMdF->ul_Num = (XMax - XMin + 1) * (YMax - YMin + 1);
			pst_GridMdF->pc_Capa = (char *) MEM_p_Alloc(pst_GridMdF->ul_Num);
		}

		if(((XMax - XMin + 1) * (YMax - YMin + 1)) != pst_GridMdF->ul_Num)
		{
			if((XMax + 1 - XMin + 1) * (YMax - YMin + 1) == pst_GridMdF->ul_Num)
				XMax ++;
			else if((XMax - XMin + 1) * (YMax + 1- YMin + 1) == pst_GridMdF->ul_Num)
				YMax ++;
			else if((XMax + 1 - XMin + 1) * (YMax + 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax ++;
				YMax ++;
			}
			else if((XMax - 1 - XMin + 1) * (YMax - YMin + 1) == pst_GridMdF->ul_Num)
				XMax --;
			else if((XMax - XMin + 1) * (YMax - 1 - YMin + 1) == pst_GridMdF->ul_Num)
				YMax --;
			else if((XMax - 1 - XMin + 1) * (YMax - 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax --;
				YMax --;
			}
			else if((XMax - 1 - XMin + 1) * (YMax + 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax --;
				YMax ++;
			}
			else if((XMax + 1 - XMin + 1) * (YMax - 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax ++;
				YMax --;
			}

		}

		k = 0;
		for(j = YMin; j <= YMax; j++)
		{
			for(i = XMin; i <= XMax; i++)
			{
				if(k >= (int) pst_GridMdF->ul_Num)
				{
					k ++;
					continue;
				}

				/* Paint real time array */
				if(!_b_Copy)
				{
					x1 = i;
					y1 = j;
					if(GRI_ConvertTo2DReal(pst_Grid, &x1, &y1))
					{
						t = (int) pst_Grid->w_NumRealGroupsX * pst_Grid->c_SizeGroup;
						t = t * y1 + x1;
						pst_Grid->pst_RealArray[t].c_Capacities = pst_GridMdF->pc_Capa[k];
					}
				}

				/* Editor Array */
				t = (int) pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup;
				t = t * j + i;

				if(_b_Copy)
					pst_GridMdF->pc_Capa[k++] = pst_Grid->pst_EditArray[t].c_Capacities;
				else
					pst_Grid->pst_EditArray[t].c_Capacities = pst_GridMdF->pc_Capa[k++];

			}
		}
	}

	if(pst_Grid1)
	{
		GRI_3Dto2D_Float(pst_Grid1, &st_Min, &fXMin, &fYMin);
		GRI_3Dto2D_Float(pst_Grid1, &st_Max, &fXMax, &fYMax);

		XMin1 = (int) fXMin;
		XMax1 = (int) fXMax;
		YMin1 = (int) fYMin;
		YMax1 = (int) fYMax;

		fXMin -= (float) XMin1;
		fXMax -= (float) XMax1;
		fYMin -= (float) YMin1;
		fYMax -= (float) YMax1;

		if(_b_Copy)
		{
			pst_GridMdF->ul_Num1 = (XMax1 - XMin1 + 1) * (YMax1 - YMin1 + 1);
			pst_GridMdF->pc_Capa1 = (char *) MEM_p_Alloc(pst_GridMdF->ul_Num1);
		}

		if(((XMax1 - XMin1 + 1) * (YMax1 - YMin1 + 1)) != pst_GridMdF->ul_Num1)
		{
			if((XMax1 + 1 - XMin1 + 1) * (YMax1 - YMin1 + 1) == pst_GridMdF->ul_Num1)
				XMax1 ++;
			else if((XMax1 - XMin1 + 1) * (YMax1 + 1 - YMin1 + 1) == pst_GridMdF->ul_Num1)
				YMax1 ++;
			else if((XMax1 + 1 - XMin1 + 1) * (YMax1 + 1 - YMin1 + 1) == pst_GridMdF->ul_Num1)
			{
				XMax1 ++;
				YMax1 ++;
			}
			else if((XMax1 - 1 - XMin1 + 1) * (YMax1 - YMin1 + 1) == pst_GridMdF->ul_Num1)
				XMax1 --;
			else if((XMax1 - XMin1 + 1) * (YMax1 - 1 - YMin1 + 1) == pst_GridMdF->ul_Num1)
				YMax1 --;
			else if((XMax1 - 1 - XMin1 + 1) * (YMax1 - 1 - YMin1 + 1) == pst_GridMdF->ul_Num1)
			{
				XMax1 --;
				YMax1 --;
			}
			else if((XMax1 - 1 - XMin1 + 1) * (YMax1 + 1 - YMin1 + 1) == pst_GridMdF->ul_Num1)
			{
				XMax1 --;
				YMax1 ++;
			}
			else if((XMax1 + 1 - XMin1 + 1) * (YMax1 - 1 - YMin1 + 1) == pst_GridMdF->ul_Num1)
			{
				XMax1 ++;
				YMax1 --;
			}

		}

		k = 0;
		for(j = YMin1; j <= YMax1; j++)
		{
			for(i = XMin1; i <= XMax1; i++)
			{
				if(k >= (int) pst_GridMdF->ul_Num1)
				{
					k ++;
//					LINK_PrintStatusMsg("Error while copy/paste Grid capas");
					continue;
				}

				/* Paint real time array */
				if(!_b_Copy)
				{
					x1 = i;
					y1 = j;
					if(GRI_ConvertTo2DReal(pst_Grid1, &x1, &y1))
					{
						t = (int) pst_Grid1->w_NumRealGroupsX * pst_Grid1->c_SizeGroup;
						t = t * y1 + x1;
						pst_Grid1->pst_RealArray[t].c_Capacities = pst_GridMdF->pc_Capa1[k];
					}
				}

				t = (int) pst_Grid1->w_NumGroupsX * pst_Grid1->c_SizeGroup;
				t = t * j + i;

				if(_b_Copy)
					pst_GridMdF->pc_Capa1[k++] = pst_Grid1->pst_EditArray[t].c_Capacities;
				else
					pst_Grid1->pst_EditArray[t].c_Capacities = pst_GridMdF->pc_Capa1[k++];
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Grid_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	if(_pst_Mod->p_Data) 
	{
		if(((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa)
			MEM_Free(((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa);

		if(((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa1)
			MEM_Free(((Grid_tdst_Modifier *) _pst_Mod->p_Data)->pc_Capa1);

		MEM_Free(_pst_Mod->p_Data);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Grid_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Grid_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Grid_Modifier_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG Grid_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Grid_tdst_Modifier		*pst_GridMdF;
	char					*pc_Cur;
	ULONG					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	pst_GridMdF = (Grid_tdst_Modifier *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	pst_GridMdF->uc_Version = LOA_ReadUChar(&pc_Cur);
	pst_GridMdF->uc_Dummy = LOA_ReadUChar(&pc_Cur);
	pst_GridMdF->uw_Dummy = LOA_ReadUShort(&pc_Cur);

	pst_GridMdF->ul_Num = LOA_ReadULong(&pc_Cur);
	if(pst_GridMdF->ul_Num)
	{
		pst_GridMdF->pc_Capa = (char *) MEM_p_Alloc(pst_GridMdF->ul_Num);

		for(i = 0; i < pst_GridMdF->ul_Num; i++)
			pst_GridMdF->pc_Capa[i] = LOA_ReadChar(&pc_Cur);
	}

	pst_GridMdF->ul_Num1 = LOA_ReadULong(&pc_Cur);
	if(pst_GridMdF->ul_Num1)
	{
		pst_GridMdF->pc_Capa1= (char *) MEM_p_Alloc(pst_GridMdF->ul_Num1);

		for(i = 0; i < pst_GridMdF->ul_Num1; i++)
			pst_GridMdF->pc_Capa1[i] = LOA_ReadChar(&pc_Cur);
	}

	return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Grid_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Grid_tdst_Modifier		*pst_GridMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Mod || !_pst_Mod->p_Data || !_pst_Mod->pst_GO) return;

	pst_GridMdF = (Grid_tdst_Modifier *) _pst_Mod->p_Data;

	SAV_Buffer(&pst_GridMdF->uc_Version, 1);
	SAV_Buffer(&pst_GridMdF->uc_Dummy, 1);
	SAV_Buffer(&pst_GridMdF->uw_Dummy, 2);
	SAV_Buffer(&pst_GridMdF->ul_Num, 4);
	if(pst_GridMdF->ul_Num)
	{
		SAV_Buffer(pst_GridMdF->pc_Capa, pst_GridMdF->ul_Num);
	}

	SAV_Buffer(&pst_GridMdF->ul_Num1, 4);
	if(pst_GridMdF->ul_Num1)
	{
		SAV_Buffer(pst_GridMdF->pc_Capa1, pst_GridMdF->ul_Num1);
	}
}
#endif
