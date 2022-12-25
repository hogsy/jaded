/*$T F3DView_grid.cpp GC! 1.092 09/04/00 14:22:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/GRId/GRI_save.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/GRId/GRI_display.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "DIAlogs/DIAgrid_dlg.h"

#ifdef JADEFUSION
extern BOOL GRI_gb_Edit;
extern BOOL GRI_gb_Modified;
#else
extern "C" BOOL GRI_gb_Edit;
extern "C" BOOL GRI_gb_Modified;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Grid_SetWidth(int i, int cx)
{
	if(i == 1)
		GRI_SetWidth(mst_WinHandles.pst_World->pst_Grid, cx);
	else
		GRI_SetWidth(mst_WinHandles.pst_World->pst_Grid1, cx);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Grid_SetHeight(int i, int cy)
{
	if(i == 1)
		GRI_SetHeight(mst_WinHandles.pst_World->pst_Grid, cy);
	else
		GRI_SetHeight(mst_WinHandles.pst_World->pst_Grid1, cy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Grid_Create(int i)
{
	GRI_CreateGrid(mst_WinHandles.pst_World, i);

	if(mst_WinHandles.pst_World->pst_Grid && (i == 2))
	{
		mst_WinHandles.pst_World->pst_Grid1->f_MinXTotal = mst_WinHandles.pst_World->pst_Grid->f_MinXTotal;
		mst_WinHandles.pst_World->pst_Grid1->f_MinYTotal = mst_WinHandles.pst_World->pst_Grid->f_MinYTotal;
	}
	else if(mst_WinHandles.pst_World->pst_Grid1 && (i == 1))
	{
		mst_WinHandles.pst_World->pst_Grid->f_MinXTotal = mst_WinHandles.pst_World->pst_Grid1->f_MinXTotal;
		mst_WinHandles.pst_World->pst_Grid->f_MinYTotal = mst_WinHandles.pst_World->pst_Grid1->f_MinYTotal;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Grid_Destroy(int i)
{
	if(i == 1)
	{
		GRI_Destroy(mst_WinHandles.pst_World->pst_Grid);
		mst_WinHandles.pst_World->pst_Grid = NULL;
	}
	else
	{
		GRI_Destroy(mst_WinHandles.pst_World->pst_Grid1);
		mst_WinHandles.pst_World->pst_Grid1 = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Grid_EditSwap(void)
{
	static EDIA_cl_GridDialog *po_Dlg = NULL;

	mb_GridEdit = !mb_GridEdit;
	GRI_gb_Edit = mb_GridEdit;
    if(mb_GridEdit) 
	{
		if(!po_Dlg)
		{
			po_Dlg = new EDIA_cl_GridDialog;
			po_Dlg->DoModeless();
		}
		else
		{
			po_Dlg->ShowWindow(SW_SHOW);
		}

		Selection_b_Treat(NULL, 0, 0, 0);
	}
	else
	{
		if(po_Dlg) po_Dlg->ShowWindow(SW_HIDE);
		if(GRI_gb_Modified)
		{
			if(mst_WinHandles.pst_DisplayData->pst_World->pst_Grid) GRI_UpdateCompress(mst_WinHandles.pst_DisplayData->pst_World->pst_Grid);
			if(mst_WinHandles.pst_DisplayData->pst_World->pst_Grid1) GRI_UpdateCompress(mst_WinHandles.pst_DisplayData->pst_World->pst_Grid1);
			GRI_gb_Modified = FALSE;
		}
	}

	/* Force display first grid */
	if(mb_GridEdit && !(mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1))
		mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags |= GRID_Cul_DO_Show;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::Grid_b_InEdit(void)
{
	return mb_GridEdit;
}

#ifndef JADEFUSION
extern "C" 
{
#endif

/* Current Sel values */
int		gi_XSel = -1;
int		gi_YSel = -1;

int		gi_XCursor = -1;
int		gi_YCursor = -1;

int		gi_MinXSel = -1;
int		gi_MinYSel = -1;
int		gi_MaxXSel = -1;
int		gi_MaxYSel = -1;

/* Saved Sel values */
int		gi_XSave = -1;
int		gi_YSave = -1;

int		gi_MinXSave = -1;
int		gi_MinYSave = -1;
int		gi_MaxXSave = -1;
int		gi_MaxYSave = -1;


UCHAR	*guc_CapaSel = NULL;
UCHAR	*guc_CapaSel1 = NULL;
#ifndef JADEFUSION
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void Grid_PaintBase(GRID_tdst_World *pst_Grid, int a, int b, UCHAR uc_Capa)
{
	int		i;
	int		x1, y1;


	if(a < 0) return;
	if(b < 0) return;
	if(a >= pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup) return;
	if(b >= pst_Grid->w_NumGroupsY * pst_Grid->c_SizeGroup) return;

	uc_Capa &= MASK_STATIC;

	/* Paint real time array */
	x1 = a;
	y1 = b;
	if(GRI_ConvertTo2DReal(pst_Grid, &x1, &y1))
	{
		i = (int) pst_Grid->w_NumRealGroupsX * pst_Grid->c_SizeGroup;
		i = i * y1 + x1;
		pst_Grid->pst_RealArray[i].c_Capacities = uc_Capa;
	}

	/* Paint edit array */
	i = (int) pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup;
	i = i * b + a;

	if(pst_Grid->pst_EditArray[i].c_Capacities == uc_Capa) return;

	pst_Grid->pst_EditArray[i].c_Capacities = uc_Capa;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void Grid_PaintFill(GRID_tdst_World *pst_Grid, int a, int b, UCHAR uc_Ref, UCHAR uc_Capa, int reclevel)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	static UCHAR	uc_Tst;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(a < 0) return;
	if(b < 0) return;
	if(a >= pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup) return;
	if(b >= pst_Grid->w_NumGroupsY * pst_Grid->c_SizeGroup) return;
	uc_Capa &= MASK_STATIC;
	uc_Ref &= MASK_STATIC;

	uc_Tst = pst_Grid->pst_EditArray[a + (b * pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup)].c_Capacities & MASK_STATIC;
	if(uc_Tst != uc_Ref) return;
	if(uc_Tst == uc_Capa) return;
	Grid_PaintBase(pst_Grid, a, b, uc_Capa); 
	if(reclevel > 10000) return; /* Blindage trop de recurs */

	Grid_PaintFill(pst_Grid, a - 1, b, uc_Ref, uc_Capa, reclevel + 1); 
	Grid_PaintFill(pst_Grid, a + 1, b, uc_Ref, uc_Capa, reclevel + 1); 
	Grid_PaintFill(pst_Grid, a, b - 1, uc_Ref, uc_Capa, reclevel + 1); 
	Grid_PaintFill(pst_Grid, a, b + 1, uc_Ref, uc_Capa, reclevel + 1); 
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Grid_Paint(GRID_tdst_World *pst_Grid, int x, int y, BOOL _b_Erase)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int				a, b, i;
	unsigned char	uc_Capa, uc_Ref;
	static int		i9[] = {-1,-1,0,-1,1,-1,1,0,1,1,0,1,-1,1,-1,0,0,0};
	BOOL			b_Sel;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!GRID_2DtoGridCoord
		(
			mst_WinHandles.pst_DisplayData->pst_World,
			pst_Grid,
			mst_WinHandles.pst_DisplayData,
			x,
			y,
			&a,
			&b,
			1
		)
	) return;

	b_Sel = GetAsyncKeyState(VK_CONTROL) < 0;

	/* Pick value */
	if(GetAsyncKeyState('P') < 0)
	{
		i = (int) pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup;
		i = i * b + a;
		((EOUT_cl_Frame *) mpo_AssociatedEditor)->mst_Ini.uc_GridPaintValue = pst_Grid->pst_EditArray[i].c_Capacities;
		return;
	}

	if(b_Sel)
	{
		if(gi_XSel == -1)
		{
			gi_XSel = a;
			gi_YSel = b;
		}

		gi_MinXSel = (a < gi_XSel) ? a :  gi_XSel;
		gi_MinYSel = (b < gi_YSel) ? b :  gi_YSel;
		gi_MaxXSel = (a > gi_XSel) ? a :  gi_XSel;
		gi_MaxYSel = (b > gi_YSel) ? b :  gi_YSel;
	}
	else
	{
		if(_b_Erase)
			uc_Capa = 0;
		else
			uc_Capa = (((EOUT_cl_Frame *) mpo_AssociatedEditor)->mst_Ini.uc_GridPaintValue) & MASK_STATIC;

		/* Paint real time array */
		switch(((EOUT_cl_Frame *) mpo_AssociatedEditor)->mst_Ini.ul_GridPaintMode)
		{
		case 0:	
			{
				if(pst_Grid->pst_EditArray[((int) (pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup) * b) + a].c_Capacities != uc_Capa)
				{
					mpo_CurrentModif = new F3D_cl_Undo_EditGrid(this, pst_Grid, a, b, a, b, (char *) &uc_Capa);
					mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
					mpo_CurrentModif = NULL;
				}

				Grid_PaintBase(pst_Grid, a, b, uc_Capa); 
				break;
			}
		case 1: 
			{
				for(i = 0; i < sizeof(i9) / 4; i+=2)
				{
					x = a + i9[i];
					y = b + i9[i+1];

					if(pst_Grid->pst_EditArray[((int) (pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup) * y) + x].c_Capacities != uc_Capa)
					{
						mpo_CurrentModif = new F3D_cl_Undo_EditGrid(this, pst_Grid, x, y, x, y, (char *) &uc_Capa);
						mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
						mpo_CurrentModif = NULL;
					}

					Grid_PaintBase(pst_Grid, x, y, uc_Capa); 
				}
			}
			break;
		case 2:
			uc_Ref = pst_Grid->pst_EditArray[a + (b * pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup)].c_Capacities & MASK_STATIC;
			Grid_PaintFill(pst_Grid, a, b, uc_Ref, uc_Capa, 0);
			break;
		}
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Grid_ResetSel()
{
	gi_XSel = -1;
	gi_YSel = -1;

	gi_MinXSel = -1;
	gi_MinYSel = -1;
	gi_MaxXSel = -1;
	gi_MaxYSel = -1;

	gi_XCursor = -1;
	gi_YCursor = -1;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Grid_CopySel(WOR_tdst_World *pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int	i, j, k, t;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	if((gi_XSel == -1) || !pst_World) 
	{
		return;
	}

	if(pst_World->pst_Grid && (mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show))
	{
		if(guc_CapaSel)
			MEM_Free(guc_CapaSel);

		guc_CapaSel = (UCHAR *) MEM_p_Alloc((gi_MaxXSel - gi_MinXSel + 1) * (gi_MaxYSel - gi_MinYSel + 1));

		k = 0;
		for(i = gi_MinXSel; i <= gi_MaxXSel; i++)
		{
			for(j = gi_MinYSel; j <= gi_MaxYSel; j++)
			{
				t = (int) pst_World->pst_Grid->w_NumGroupsX * pst_World->pst_Grid->c_SizeGroup;
				t = t * j + i;

				guc_CapaSel[k++] = (UCHAR) pst_World->pst_Grid->pst_EditArray[t].c_Capacities;
			}
		}
	}

	if(pst_World->pst_Grid1 && (mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1) )
	{
		if(guc_CapaSel1)
			MEM_Free(guc_CapaSel1);

		guc_CapaSel1 = (UCHAR *) MEM_p_Alloc((gi_MaxXSel - gi_MinXSel + 1) * (gi_MaxYSel - gi_MinYSel + 1));

		k = 0;
		for(i = gi_MinXSel; i <= gi_MaxXSel; i++)
		{
			for(j = gi_MinYSel; j <= gi_MaxYSel; j++)
			{
				t = (int) pst_World->pst_Grid1->w_NumGroupsX * pst_World->pst_Grid1->c_SizeGroup;
				t = t * j + i;

				guc_CapaSel1[k++] = (UCHAR) pst_World->pst_Grid1->pst_EditArray[t].c_Capacities;
			}
		}
	}


	gi_MinXSave = gi_MinXSel;
	gi_MinYSave = gi_MinYSel;
	gi_MaxXSave = gi_MaxXSel;
	gi_MaxYSave = gi_MaxYSel;

	gi_XSave = gi_XSel;
	gi_YSave = gi_YSel;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Grid_PasteSel(WOR_tdst_World *pst_World, int x, int y)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	GRID_tdst_World *pst_Grid;
	int		a, b, i, j, k, t;
	int		x1, y1;
	int		starta, startb;
	BOOL	b_CasBatard;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_World) 
	{
		return;
	}

	if(!pst_World->pst_Grid && !pst_World->pst_Grid1) return;

	pst_Grid = pst_World->pst_Grid ? pst_World->pst_Grid : pst_World->pst_Grid1;

	b_CasBatard = FALSE;

	if
	(
		((mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show) && !(mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1) && guc_CapaSel1)
	||  (!(mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show) && (mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1) && guc_CapaSel)
	)
		b_CasBatard = TRUE;

	if
	(
		!GRID_2DtoGridCoord
		(
			mst_WinHandles.pst_DisplayData->pst_World,
			pst_World->pst_Grid ? pst_World->pst_Grid : pst_World->pst_Grid1,
			mst_WinHandles.pst_DisplayData,
			x,
			y,
			&a,
			&b,
			1
		)
	) 
	{
		return;
	}

	starta = (gi_XSave == gi_MinXSave) ? a : a - (gi_MaxXSave - gi_MinXSave);
	startb = (gi_YSave == gi_MinYSave) ? b : b - (gi_MaxYSave - gi_MinYSave);

	if(pst_World->pst_Grid && (mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show))
	{
		mpo_CurrentModif = new F3D_cl_Undo_EditGrid(this, pst_World->pst_Grid, starta, startb, starta + gi_MaxXSave - gi_MinXSave, startb + gi_MaxYSave - gi_MinYSave, (char *) guc_CapaSel);
		mpo_CurrentModif->SetDesc("Edit Grid");
		mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
		mpo_CurrentModif = NULL;
	}

	if(pst_World->pst_Grid1 && (mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1))
	{
		mpo_CurrentModif = new F3D_cl_Undo_EditGrid(this, pst_World->pst_Grid1, starta, startb, starta + gi_MaxXSave - gi_MinXSave, startb + gi_MaxYSave - gi_MinYSave, (char *) guc_CapaSel);
		mpo_CurrentModif->SetDesc("Edit Grid 1");
		mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
		mpo_CurrentModif = NULL;
	}


	k = 0;
	for(i = starta; i <= starta + (gi_MaxXSave  - gi_MinXSave) ; i++)
	{
		if(i >= pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup)
		{
			k += gi_MaxYSave - gi_MinYSave;
			continue;
		}

		for(j = startb; j <= startb + (gi_MaxYSave - gi_MinYSave); j++)
		{
			if(j >= pst_Grid->w_NumGroupsY * pst_Grid->c_SizeGroup)
			{
				k ++;
			}

			/* Paint real time array */
			x1 = i;
			y1 = j;
			if(pst_World->pst_Grid && (b_CasBatard ? guc_CapaSel1 :  guc_CapaSel) && (mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show))
			{
				if(GRI_ConvertTo2DReal(pst_World->pst_Grid, &x1, &y1))
				{
					t = (int) pst_World->pst_Grid->w_NumRealGroupsX * pst_World->pst_Grid->c_SizeGroup;
					t = t * y1 + x1;
					pst_World->pst_Grid->pst_RealArray[t].c_Capacities = b_CasBatard ? (char) guc_CapaSel1[k] : (char) guc_CapaSel[k];
				}

				/* Editor array */
				t = (int) pst_World->pst_Grid->w_NumGroupsX * pst_World->pst_Grid->c_SizeGroup;
				t = t * j + i;

				pst_World->pst_Grid->pst_EditArray[t].c_Capacities = b_CasBatard ? (char) guc_CapaSel1[k] : (char) guc_CapaSel[k];
			}


			/* Paint real time array */
			x1 = i;
			y1 = j;
			if(pst_World->pst_Grid1 && (b_CasBatard ? guc_CapaSel :  guc_CapaSel1) && (mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1))
			{
				if(GRI_ConvertTo2DReal(pst_World->pst_Grid1, &x1, &y1))
				{
					t = (int) pst_World->pst_Grid1->w_NumRealGroupsX * pst_World->pst_Grid1->c_SizeGroup;
					t = t * y1 + x1;
					pst_World->pst_Grid1->pst_RealArray[t].c_Capacities = b_CasBatard ? (char) guc_CapaSel[k] : (char) guc_CapaSel1[k];
				}

				/* Editor array */
				t = (int) pst_World->pst_Grid1->w_NumGroupsX * pst_World->pst_Grid1->c_SizeGroup;
				t = t * j + i;

				pst_World->pst_Grid1->pst_EditArray[t].c_Capacities = b_CasBatard ? (char) guc_CapaSel[k] : (char) guc_CapaSel1[k];
			}

			k ++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::Grid_b_Paint(int x, int y, BOOL _b_Erase)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(IsActionValidate(F3D_Action_CameraOn)) return FALSE;

	pst_World = mst_WinHandles.pst_DisplayData->pst_World;
	if(!pst_World) return FALSE;

	GRI_gb_Modified = TRUE;
	if(mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show)
	{
		Grid_Paint(pst_World->pst_Grid, x, y, _b_Erase);
	}

	if(mst_WinHandles.pst_DisplayData->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1)
	{
		Grid_Paint(pst_World->pst_Grid1, x, y, _b_Erase);
	}

	return TRUE;
}

#endif
