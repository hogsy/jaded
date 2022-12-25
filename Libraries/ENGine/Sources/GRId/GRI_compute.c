/*$T GRI_compute.c GC! 1.100 08/22/01 09:32:03 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIstack.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_vars.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGkey.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIpaths.h"
#include "LINks/LINKstruct.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/GRId/GRI_vars.h"

#ifdef PSX2_TARGET
#include "PSX2debug.h"
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define M_RealArr(g, x, y)		g->pst_RealArray[x + ((y) * (g)->w_NumRealGroupsX * (g)->c_SizeGroup)]
#define M_EvalArray(g, x, y)	g->pst_EvalArray[x + ((y) * (g)->w_NumRealGroupsX * (g)->c_SizeGroup)]
#define M_Static(g, x, y)		(M_RealArr(g, x, y).c_Capacities & MASK_STATIC)
#define M_Dyn(g, x, y)			((M_RealArr(g, x, y).c_Capacities & 0x30) >> 4)
#define M_Pound(g, x, y)		(GRI_gaf_Ponderations[M_Static(g, x, y)])
#define M_PoundDyn(g, x, y)		(GRI_gaf_PonderationsDyn[M_Dyn(g, x, y)])
#define M_Wall(g, x, y)			((M_RealArr(g, x, y).c_Capacities & OCCUP) || !(M_Pound(g, x, y)) || !(M_PoundDyn(g, x, y)))
#define M_WallNoDyn(g, x, y)	(!(M_Pound(g, x, y)) || !(M_PoundDyn(g, x, y)))

static int				ai_Scan[] = { 0, -1, 1, 0, 0, 1, -1, 0, -1, -1, 1, -1, 1, 1, -1, 1 };
static int				ai_Scan2[] = { -1, 0, 0, -1, 1, 0, 0, 1, -1, 0 };
extern ULONG			LOA_ul_FileTypeSize[40];
extern float			GRI_gaf_Ponderations[16];
extern float			GRI_gaf_PonderationsDyn[4];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL GRI_3Dto2D_Float(GRID_tdst_World *_pst_Grid, MATH_tdst_Vector *_pst_Vec, float *_pf_X, float *_pf_Y)
{
	/*~~~~~~~~~~*/
	BOOL	b_Res;
	/*~~~~~~~~~~*/

	b_Res = TRUE;

	/* X */
	if(_pst_Vec->x < _pst_Grid->f_MinXTotal)
	{
		*_pf_X = 0.0f;
		b_Res = FALSE;
	}
	else if(_pst_Vec->x > _pst_Grid->f_MinXTotal + (_pst_Grid->w_NumGroupsX * _pst_Grid->c_SizeGroup))
	{
		*_pf_X = (float) ((_pst_Grid->w_NumGroupsX * _pst_Grid->c_SizeGroup) - 1);
		b_Res = FALSE;
	}
	else
	{
		*_pf_X =(_pst_Vec->x - _pst_Grid->f_MinXTotal);
	}

	/* Y */
	if(_pst_Vec->y < _pst_Grid->f_MinYTotal)
	{
		*_pf_Y = 0.0f;
		b_Res = FALSE;
	}
	else if(_pst_Vec->y > _pst_Grid->f_MinYTotal + (_pst_Grid->w_NumGroupsY * _pst_Grid->c_SizeGroup))
	{
		*_pf_Y = (float) ((_pst_Grid->w_NumGroupsY * _pst_Grid->c_SizeGroup) - 1);
		b_Res = FALSE;
	}
	else
	{
		*_pf_Y = _pst_Vec->y - _pst_Grid->f_MinYTotal;
	}

	return b_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL GRI_3Dto2D(GRID_tdst_World *_pst_Grid, MATH_tdst_Vector *_pst_Vec, int *_pi_X, int *_pi_Y)
{
	/*~~~~~~~~~~*/
	BOOL	b_Res;
	/*~~~~~~~~~~*/

	b_Res = TRUE;

	/* X */
	if(_pst_Vec->x < _pst_Grid->f_MinXTotal)
	{
		*_pi_X = 0;
		b_Res = FALSE;
	}
	else if(_pst_Vec->x > _pst_Grid->f_MinXTotal + (_pst_Grid->w_NumGroupsX * _pst_Grid->c_SizeGroup))
	{
		*_pi_X = (_pst_Grid->w_NumGroupsX * _pst_Grid->c_SizeGroup) - 1;
		b_Res = FALSE;
	}
	else
	{
		*_pi_X = (int) (_pst_Vec->x - _pst_Grid->f_MinXTotal);
	}

	/* Y */
	if(_pst_Vec->y < _pst_Grid->f_MinYTotal)
	{
		*_pi_Y = 0;
		b_Res = FALSE;
	}
	else if(_pst_Vec->y > _pst_Grid->f_MinYTotal + (_pst_Grid->w_NumGroupsY * _pst_Grid->c_SizeGroup))
	{
		*_pi_Y = (_pst_Grid->w_NumGroupsY * _pst_Grid->c_SizeGroup) - 1;
		b_Res = FALSE;
	}
	else
	{
		*_pi_Y = (int) (_pst_Vec->y - _pst_Grid->f_MinYTotal);
	}

	return b_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL GRI_ConvertTo2DReal(GRID_tdst_World *_pst_Grid, int *_pi_X, int *_pi_Y)
{
	/*~~~~~~~~~~*/
	BOOL	b_Res;
	/*~~~~~~~~~~*/

	b_Res = TRUE;
	*_pi_X -= (_pst_Grid->w_XRealGroup * _pst_Grid->c_SizeGroup);
	if(*_pi_X < 0)
	{
		b_Res = FALSE;
		*_pi_X = 0;
	}

	*_pi_Y -= (_pst_Grid->w_YRealGroup * _pst_Grid->c_SizeGroup);
	if(*_pi_Y < 0)
	{
		b_Res = FALSE;
		*_pi_Y = 0;
	}

	if(*_pi_X >= (_pst_Grid->w_NumRealGroupsX * _pst_Grid->c_SizeGroup))
	{
		*_pi_X = (_pst_Grid->w_NumRealGroupsX * _pst_Grid->c_SizeGroup) - 1;
		b_Res = FALSE;
	}

	if(*_pi_Y >= (_pst_Grid->w_NumRealGroupsY * _pst_Grid->c_SizeGroup))
	{
		*_pi_Y = (_pst_Grid->w_NumRealGroupsY * _pst_Grid->c_SizeGroup) - 1;
		b_Res = FALSE;
	}

	return b_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_SetCapa(GRID_tdst_World *_pst_Grid, MATH_tdst_Vector *_pst_Pos, char _c_Capa)
{
	/*~~~~~~~*/
	int x, y;
	int x1, y1;
	/*~~~~~~~*/

	if(!GRI_3Dto2D(_pst_Grid, _pst_Pos, &x, &y)) return;
	x1 = x;
	y1 = y;
	if(!GRI_ConvertTo2DReal(_pst_Grid, &x1, &y1)) return;

#ifdef ACTIVE_EDITORS
	_pst_Grid->pst_EditArray[x + (y * _pst_Grid->w_NumGroupsX * _pst_Grid->c_SizeGroup)].c_Capacities &= MASK_STATIC;
	_pst_Grid->pst_EditArray[x + (y * _pst_Grid->w_NumGroupsX * _pst_Grid->c_SizeGroup)].c_Capacities |= (_c_Capa & MASK_DYN);
#endif
	_pst_Grid->pst_RealArray[x1 + (y1 * _pst_Grid->w_NumRealGroupsX * _pst_Grid->c_SizeGroup)].c_Capacities &= MASK_STATIC;
	_pst_Grid->pst_RealArray[x1 + (y1 * _pst_Grid->w_NumRealGroupsX * _pst_Grid->c_SizeGroup)].c_Capacities |= (_c_Capa & MASK_DYN);
}
/**/
int GRI_AddCapa(GRID_tdst_World *_pst_Grid, MATH_tdst_Vector *_pst_Pos, char _c_Capa, int *reset)
{
	/*~~~~~~~*/
	int		x, y;
	int		x1, y1;
	char	*pc;
	/*~~~~~~~*/

	if(reset)
	{
		x = (*reset) & 0xFFFF;
		y = (*reset) >> 16;
	}
	else
	{
		if(!GRI_3Dto2D(_pst_Grid, _pst_Pos, &x, &y)) return 1;
	}

	x1 = x;
	y1 = y;
	if(!GRI_ConvertTo2DReal(_pst_Grid, &x1, &y1)) return 1;

	pc = &_pst_Grid->pst_RealArray[x1 + (y1 * _pst_Grid->w_NumRealGroupsX * _pst_Grid->c_SizeGroup)].c_Capacities;

	/* Terrains dynamiques : ce n'est pas additif */
	if((UCHAR) abs(_c_Capa) & 0x30) 
	{
		*pc &= 0xCF;
#ifdef ACTIVE_EDITORS
		_pst_Grid->pst_EditArray[x + (y * _pst_Grid->w_NumGroupsX * _pst_Grid->c_SizeGroup)].c_Capacities &= 0xCF;
#endif
		if(_c_Capa < 0) return 1;
	}

	if(((_c_Capa > 0) && ((UCHAR) _c_Capa & OCCUP)) && ((((UCHAR) *pc) & OCCUP) == OCCUP)) return 0;

#ifdef ACTIVE_EDITORS
	_pst_Grid->pst_EditArray[x + (y * _pst_Grid->w_NumGroupsX * _pst_Grid->c_SizeGroup)].c_Capacities += _c_Capa;
#endif
	*pc += _c_Capa;

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GRI_i_GetCapa(GRID_tdst_World *_pst_Grid, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~*/
	int x, y;
	/*~~~~~*/

	if(!GRI_3Dto2D(_pst_Grid, _pst_Pos, &x, &y)) return 0;
	if(!GRI_ConvertTo2DReal(_pst_Grid, &x, &y)) return 0;
	return _pst_Grid->pst_RealArray[x + (y * _pst_Grid->w_NumRealGroupsX * _pst_Grid->c_SizeGroup)].c_Capacities;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GRI_i_ComputeDest(GRID_tdst_World *_pst_Grid, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int x, y, x1, y1;
	int ai[16] = { -1, 0, -1, -1, 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1 };
	int i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!GRI_3Dto2D(_pst_Grid, _pst_Pos, &x, &y)) return 0;
	if(!GRI_ConvertTo2DReal(_pst_Grid, &x, &y)) return 0;

	if(M_Wall(_pst_Grid, x, y))
	{
		x1 = x;
		y1 = y;
		for(i = 0; i < 16; i += 2)
		{
			x = x1 + ai[i];
			y = y1 + ai[i + 1];
			if(!(M_Wall(_pst_Grid, x, y)))
			{
				_pst_Pos->x = (float) x + (GRID_CASE / 2.0f) + _pst_Grid->f_MinXTotal;
				_pst_Pos->y = (float) y + (GRID_CASE / 2.0f) + _pst_Grid->f_MinYTotal;
				_pst_Pos->x += (_pst_Grid->w_XRealGroup * _pst_Grid->c_SizeGroup);
				_pst_Pos->y += (_pst_Grid->w_YRealGroup * _pst_Grid->c_SizeGroup);
				return 1;
			}
		}
	}
	else
	{
		_pst_Pos->x = (float) x + (GRID_CASE / 2.0f) + _pst_Grid->f_MinXTotal;
		_pst_Pos->y = (float) y + (GRID_CASE / 2.0f) + _pst_Grid->f_MinYTotal;
		_pst_Pos->x += (_pst_Grid->w_XRealGroup * _pst_Grid->c_SizeGroup);
		_pst_Pos->y += (_pst_Grid->w_YRealGroup * _pst_Grid->c_SizeGroup);
		return 1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL GRID_3Dto2D(GRID_tdst_World *_pst_Grid, MATH_tdst_Vector *_pst_Vec, int *_pi_X, int *_pi_Y)
{
	if(!GRI_3Dto2D(_pst_Grid, _pst_Vec, _pi_X, _pi_Y)) return FALSE;
	if(!GRI_ConvertTo2DReal(_pst_Grid, _pi_X, _pi_Y)) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ int GRID_WallBlocked(OBJ_tdst_GameObject *_pst_Obj, GRID_tdst_World *_pst_Grid, int xsrc, int ysrc, int x1, int y1, int max)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_Norm;
	WOR_tdst_World	*pst_World;
	GRID_tdst_World *pst_Grid1;
	float			pound;
	int				x2, y2;
	float			fx2, fy2;
	MATH_tdst_Vector v1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pound = 0;
	pound = M_Pound(_pst_Grid, x1, y1);
	if(pound == 0) return 1;

	if(pound == 666)	// HACK ignoble kong : si c'est brulé, on passe
	{
		pst_World = WOR_World_GetWorldOfObject(_pst_Obj);
		pst_Grid1 = pst_World->pst_Grid1;
		if(!pst_Grid1) return 1;
		fx2 = x1 + (_pst_Grid->w_XRealGroup * _pst_Grid->c_SizeGroup) + _pst_Grid->f_MinXTotal;
		fy2 = y1 + (_pst_Grid->w_YRealGroup * _pst_Grid->c_SizeGroup) + _pst_Grid->f_MinYTotal;
//		x2 = (int) fx2;
//		y2 = (int) fy2;
		v1.x = fx2;
		v1.y = fy2;
		v1.z = 0;
		GRID_3Dto2D(pst_Grid1, &v1, &x2, &y2);
//		GRI_ConvertTo2DReal(pst_Grid1, &x2, &y2);
//		x2 = (int) (fx2 - (pst_Grid1->w_XRealGroup * pst_Grid1->c_SizeGroup) - pst_Grid1->f_MinXTotal);
//		y2 = (int) (fy2 - (pst_Grid1->w_YRealGroup * pst_Grid1->c_SizeGroup) - pst_Grid1->f_MinYTotal);
		if(pst_Grid1->pst_RealArray[x2 + (y2 * pst_Grid1->w_NumRealGroupsX * pst_Grid1->c_SizeGroup)].c_Capacities & 128)
			x1 = x1;
		else
			return 1;
	}

	if(!M_PoundDyn(_pst_Grid, x1, y1)) return 1;
	i_Norm = abs(x1 - xsrc) + abs(y1 - ysrc);
	if(((M_RealArr(_pst_Grid, x1, y1).c_Capacities & OCCUP)) && (i_Norm <= max)) return 1;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GRID_ShortestPath
(
	OBJ_tdst_GameObject *_pst_Obj,
	GRID_tdst_World		*_pst_Grid,
	MATH_tdst_Vector	*_pst_Src,
	MATH_tdst_Vector	*_pst_Dest,
	MATH_tdst_Vector	*_ppst_Array,
	int					_i_Born,		// Au dela de cette distance, les OCCUP sont ignorées
	float				_f_Max,			// On borne le vecteur déplacement à cette norm là
	int					_i_Square,		// On ne recherche jamais de chemin au dela du carré défini par ce rayon
	int					_i_MaxArray		// Nbre max de cases renvoyées dans le tableau
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_NbElemInExplored;
	int					i_NbElemInReached;
	int					i_BestIndex, i_Index1, i_Index2;
	int					i_Index, i_Index3;
	float				f_Eval, f_Dist;
	BOOL				b_Done;
	int					x, y, x1, y1, x2, y2;
	int					xdest, ydest;
	int					xrealdest, yrealdest;
	int					xsrc, ysrc;
	int					i_BestX, i_BestY;
	MATH_tdst_Vector	st_Vec, st_Vec1;
	int					ii;
	int					count, best;
	float				maxdist;
	int					ccc;
	MATH_tdst_Vector	st_Tmp, st_Tmp1, st_Tmp2;
	float				f_Dot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_BestX = 0;
	i_BestY = 0;
	b_Done = FALSE;

	/* 3D to gruid coords */
	if(!GRID_3Dto2D(_pst_Grid, _pst_Src, &xsrc, &ysrc)) return 0;
	if(!GRID_3Dto2D(_pst_Grid, _pst_Dest, &xdest, &ydest)) return 0;
	xrealdest = xdest;
	yrealdest = ydest;

	/* Born destination to be sure it's not too far */
	maxdist = _i_Square < _f_Max ? _i_Square : _f_Max;
	MATH_SubVector(&st_Vec, _pst_Dest, _pst_Src);
	MATH_CopyVector(&st_Vec1, _pst_Dest);
	if(MATH_f_NormVector(&st_Vec) > maxdist)
	{
		MATH_NormalizeVector(&st_Vec, &st_Vec);
		MATH_ScaleVector(&st_Vec, &st_Vec, maxdist);
		MATH_AddVector(&st_Vec1, &st_Vec, _pst_Src);
		GRID_3Dto2D(_pst_Grid, &st_Vec1, &xdest, &ydest);
	}

	/* Src == Dest */
	if(xdest == xsrc && ydest == ysrc)
	{
		_ppst_Array[0].x = (float) xsrc + (_pst_Grid->w_XRealGroup * _pst_Grid->c_SizeGroup) + (GRID_CASE / 2.0f);
		_ppst_Array[0].x += _pst_Grid->f_MinXTotal;
		_ppst_Array[0].y = (float) ysrc + (_pst_Grid->w_YRealGroup * _pst_Grid->c_SizeGroup) + (GRID_CASE / 2.0f);
		_ppst_Array[0].y += _pst_Grid->f_MinYTotal;
		_ppst_Array[0].z = 0.0f;
		return 1;
	}

	/* First reached is first node */
	i_NbElemInExplored = 0;
	i_NbElemInReached = 1;
	M_EvalArray(_pst_Grid, xsrc, ysrc).c_Flag = REACHED;
	M_EvalArray(_pst_Grid, xsrc, ysrc).f_Eval = 0;
	M_EvalArray(_pst_Grid, xsrc, ysrc).w_WayX = -1;
	M_EvalArray(_pst_Grid, xsrc, ysrc).w_WayY = -1;
	GRID_gast_Best[0].pf_Eval = &(M_EvalArray(_pst_Grid, xsrc, ysrc).f_Eval);
	GRID_gast_Best[0].x = xsrc;
	GRID_gast_Best[0].y = ysrc;

	x = xsrc;
	y = ysrc;
	ccc = 0;
	while(i_NbElemInReached && !b_Done)
	{
		/* Retreive the best reached node */
		f_Eval = Cf_Infinit;
		i_BestIndex = (unsigned int) - 1;
		for(i_Index3 = 0; i_Index3 < i_NbElemInReached; i_Index3++)
		{
			ccc++;
			if(*GRID_gast_Best[i_Index3].pf_Eval < f_Eval)
			{
				f_Eval = *GRID_gast_Best[i_Index3].pf_Eval;
				i_BestX = GRID_gast_Best[i_Index3].x;
				i_BestY = GRID_gast_Best[i_Index3].y;
				i_BestIndex = i_Index3;
			}
		}

		/* Add node to explored and remove from reached */
		x = i_BestX;
		y = i_BestY;
		M_EvalArray(_pst_Grid, i_BestX, i_BestY).c_Flag = EXPLORED;
		GRID_gast_Explored[i_NbElemInExplored].x = i_BestX;
		GRID_gast_Explored[i_NbElemInExplored].y = i_BestY;
		if(i_NbElemInExplored >= GRID_MAX_EXPLORED) goto l_Err;

		i_NbElemInExplored++;
		i_NbElemInReached--;
		L_memcpy(&GRID_gast_Best[i_BestIndex], &GRID_gast_Best[i_NbElemInReached], sizeof(GRID_tdst_Best));

		/* Does the destination is reached ? */
		if((x == xdest) && (y == ydest)) b_Done = TRUE;

		/* Choose first scan rank depending on Y axis of perso */
		MATH_CopyVector(&st_Vec, MATH_pst_GetYAxis(_pst_Obj->pst_GlobalMatrix));
		if(st_Vec.x < -0.5f)
			best = 6 * 2;
		else if(st_Vec.x > 0.5f)
			best = 2 * 2;
		else if(st_Vec.y < -0.5f)
			best = 0;
		else
			best = 4 * 2;

		count = 0;
		for(i_Index = best; count < 8; i_Index = (i_Index + 2) % 16)
		{
			/* Distance to dest point */
			x1 = x + ai_Scan[i_Index];
			if(x1 < 0) goto next;
			if(x1 >= _pst_Grid->w_NumRealGroupsX * _pst_Grid->c_SizeGroup) goto l_Err;
			if(abs(x1 - xsrc) > _i_Square) goto next;

			y1 = y + ai_Scan[i_Index + 1];
			if(y1 < 0) goto next;
			if(y1 >= _pst_Grid->w_NumRealGroupsY * _pst_Grid->c_SizeGroup) goto l_Err;
			if(abs(y1 - ysrc) > _i_Square) goto next;

			/* Blocked */
			if(GRID_WallBlocked(_pst_Obj, _pst_Grid, xsrc, ysrc, x1, y1, _i_Born)) goto next;

			/* Diag */
			if(i_Index >= 8)
			{
				ii = i_Index - 8;
				x2 = x + ai_Scan2[ii];
				y2 = y + ai_Scan2[ii + 1];
				if(GRID_WallBlocked(_pst_Obj, _pst_Grid, xsrc, ysrc, x2, y2, _i_Born)) goto next;
				x2 = x + ai_Scan2[ii + 2];
				y2 = y + ai_Scan2[ii + 3];
				if(GRID_WallBlocked(_pst_Obj, _pst_Grid, xsrc, ysrc, x2, y2, _i_Born)) goto next;
			}

			/* Cost of terrain */
			f_Dist = M_Pound(_pst_Grid, x1, y1);
			if(f_Dist >= 665.0f && f_Dist <= 667.0f) 
				f_Dist = 1.0f;

			/* Heuristic */
			f_Dist += (float) (abs(xdest - x1) + abs(ydest - y1));

			/* Prend en compte le changement de direction */
			if(((x1 != xsrc) || (y1 != ysrc)) && ((xdest != xsrc) || (ydest != ysrc)))
			{
/*				OBJ_SightGet(_pst_Obj, &st_Tmp);
				st_Tmp1.x = (float) x1;
				st_Tmp1.y = (float) y1;
				st_Tmp1.z = 0.0f;
				st_Tmp2.x = (float) xsrc;
				st_Tmp2.y = (float) ysrc;
				st_Tmp2.z = 0.0f;
				MATH_SubVector(&st_Tmp1, &st_Tmp1, &st_Tmp2);
				MATH_NormalizeVector(&st_Tmp1, &st_Tmp1);
				f_Dot = MATH_f_DotProduct(&st_Tmp, &st_Tmp1);
				f_Dot = 1.0f - f_Dot;
				f_Dist += (float) fAbs(f_Dot) * 20.0f;*/

				/* Distance to the line source/dest */
				st_Tmp.x = (float) x1;
				st_Tmp.y = (float) y1;
				st_Tmp.z = 0.0f;
				st_Tmp1.x = (float) xdest;
				st_Tmp1.y = (float) ydest;
				st_Tmp1.z = 0.0f;
				st_Tmp2.x = (float) xsrc;
				st_Tmp2.y = (float) ysrc;
				st_Tmp2.z = 0.0f;
				MATH_SubVector(&st_Tmp, &st_Tmp, &st_Tmp2);
				MATH_NormalizeVector(&st_Tmp, &st_Tmp);
				MATH_SubVector(&st_Tmp1, &st_Tmp1, &st_Tmp2);
				MATH_NormalizeVector(&st_Tmp1, &st_Tmp1);
				f_Dot = MATH_f_DotProduct(&st_Tmp, &st_Tmp1);

				f_Dot = 1.0f - f_Dot;
				f_Dist += (float) fAbs(f_Dot) * 30.0f;
			}

			/* Is the node already in explored ? */
			if(M_EvalArray(_pst_Grid, x1, y1).c_Flag == EXPLORED) goto next;

			/* Is the node already in reached ? */
			if(M_EvalArray(_pst_Grid, x1, y1).c_Flag == REACHED)
			{
				/* Update eval if necessary */
				if(M_EvalArray(_pst_Grid, x1, y1).f_Eval > M_EvalArray(_pst_Grid, x, y).f_Eval + f_Dist)
				{
					M_EvalArray(_pst_Grid, x1, y1).f_Eval = M_EvalArray(_pst_Grid, x, y).f_Eval + f_Dist;
					M_EvalArray(_pst_Grid, x1, y1).w_WayX = x;
					M_EvalArray(_pst_Grid, x1, y1).w_WayY = y;
				}

				goto next;
			}

			/* Else add the node in reached */
			M_EvalArray(_pst_Grid, x1, y1).c_Flag = REACHED;
			M_EvalArray(_pst_Grid, x1, y1).f_Eval = M_EvalArray(_pst_Grid, x, y).f_Eval + f_Dist;
			M_EvalArray(_pst_Grid, x1, y1).w_WayX = x;
			M_EvalArray(_pst_Grid, x1, y1).w_WayY = y;

			GRID_gast_Best[i_NbElemInReached].pf_Eval = &(M_EvalArray(_pst_Grid, x1, y1).f_Eval);
			GRID_gast_Best[i_NbElemInReached].x = x1;
			GRID_gast_Best[i_NbElemInReached].y = y1;
			if(i_NbElemInReached >= GRID_MAX_BEST) goto l_Err;
			i_NbElemInReached++;
next:
			count++;
		}
	}

l_Err:
	for(i_Index1 = 0; i_Index1 < i_NbElemInExplored; i_Index1++)
		M_EvalArray(_pst_Grid, GRID_gast_Explored[i_Index1].x, GRID_gast_Explored[i_Index1].y).c_Flag = 0;
	for(i_Index1 = 0; i_Index1 < i_NbElemInReached; i_Index1++)
		M_EvalArray(_pst_Grid, GRID_gast_Best[i_Index1].x, GRID_gast_Best[i_Index1].y).c_Flag = 0;

	if(b_Done)
	{
		/* Size of way */
		i_Index2 = 0;
		x = xdest;
		y = ydest;
		while(x != -1)
		{
			x1 = M_EvalArray(_pst_Grid, x, y).w_WayX;
			y1 = M_EvalArray(_pst_Grid, x, y).w_WayY;
			x = x1;
			y = y1;
			i_Index2++;
		}

//		if(i_Index2 > _i_MaxArray) return 0;

		/* Construct way */
		i_Index1 = 0;
		x = xdest;
		y = ydest;
		while(x != -1)
		{
			if((int) i_Index2 <= _i_MaxArray)
			{
				_ppst_Array[i_Index1].x = (float) x + (_pst_Grid->w_XRealGroup * _pst_Grid->c_SizeGroup) + (GRID_CASE / 2.0f);
				_ppst_Array[i_Index1].x += _pst_Grid->f_MinXTotal;
				_ppst_Array[i_Index1].y = (float) y + (_pst_Grid->w_YRealGroup * _pst_Grid->c_SizeGroup) + (GRID_CASE / 2.0f);
				_ppst_Array[i_Index1].y += _pst_Grid->f_MinYTotal;
				_ppst_Array[i_Index1].z = 0.0f;
				i_Index1++;
			}
			else
				i_Index2--;
			x1 = M_EvalArray(_pst_Grid, x, y).w_WayX;
			y1 = M_EvalArray(_pst_Grid, x, y).w_WayY;
			x = x1;
			y = y1;
		}

		i_NbElemInExplored = i_Index1;
	}
	else
		i_NbElemInExplored = 0;

	return i_NbElemInExplored;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GRI_SmoothWay
(
	GRID_tdst_World		*_pst_Grid,
	MATH_tdst_Vector	*_pst_ArrayPos,
	int					_i_NumPos,
	float				maxpound
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i, j, x, y, x1, y1;
	static int	ai_Scan[] = { -1, -1, 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0 };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = _i_NumPos - 1; i >= 0; i--)
	{
		if(!GRI_3Dto2D(_pst_Grid, &_pst_ArrayPos[i], &x, &y)) return i + 1;

		for(j = 0; j < (int) (sizeof(ai_Scan) / sizeof(int)); j += 2)
		{
			x1 = x + ai_Scan[j];
			y1 = y + ai_Scan[j + 1];
			if(!GRI_ConvertTo2DReal(_pst_Grid, &x1, &y1)) return i + 1;
			if(M_Wall(_pst_Grid, x1, y1)) return i + 1;
			if(M_Pound(_pst_Grid, x1, y1) > maxpound) return i + 1;
		}
	}

	return i + 1;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GRI_Compress(GRID_tdst_World *_pst_Grid, GRID_tdst_Elem *_pst_Array, int x, int y, char *_pc_Comp, int _i_Mask)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRID_tdst_Elem	*pst_RealArray;
	int				i_Num;
	int				i_Val;
	ULONG			ul_Size;
	int				xt, woff;
	int				xend, yend;
	char			*pc_Comp, c_End;
	int				i_NumDif;
	short			*sh_pc_Comp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*~~~~~~~~~~*/
#ifdef PSX2_TARGET

	/* mamagouille */
	char	*toto;
#endif
	/*~~~~~~~~~~*/

	xend = x + _pst_Grid->c_SizeGroup;
	yend = y + _pst_Grid->c_SizeGroup;

	/* Compute beginning address */
	if(_pst_Array == _pst_Grid->pst_RealArray)
	{
		pst_RealArray = _pst_Grid->pst_RealArray + x + (y * _pst_Grid->w_NumRealGroupsX * _pst_Grid->c_SizeGroup);
		woff = _pst_Grid->c_SizeGroup * (_pst_Grid->w_NumRealGroupsX - 1);
	}
	else
	{
		pst_RealArray = _pst_Grid->pst_EditArray + x + (y * _pst_Grid->w_NumGroupsX * _pst_Grid->c_SizeGroup);
		woff = _pst_Grid->c_SizeGroup * (_pst_Grid->w_NumGroupsX - 1);
	}

	i_Num = 1;
	i_NumDif = 0;
	i_Val = pst_RealArray->c_Capacities & _i_Mask;
	pst_RealArray++;
	xt = x + 1;
	pc_Comp = _pc_Comp + 4; /* To store the size at the beginning */
	c_End = 0;

	while(y != yend)
	{
		while(xt != xend)
		{
			if((pst_RealArray->c_Capacities & _i_Mask) == i_Val)
			{
				i_Num++;
			}
			else
			{
				i_NumDif++;
Flush:
				if(i_Num == 1)
				{
					if(i_Val & 0xC0)
					{
						*(pc_Comp++) = (char) 0x80;
						*(pc_Comp++) = 1;
						*(pc_Comp++) = (char) i_Val;
					}
					else
					{
						*(pc_Comp++) = (char) i_Val;
					}
				}
				else if((i_Num <= 0x3F) && (!(i_Val & (char) 0xC0)))
				{
					*(pc_Comp++) = (char) 0xC0 | (char) i_Num;
					*(pc_Comp++) = (char) i_Val;
				}
				else if(i_Num <= 0xFF)
				{
					*(pc_Comp++) = (char) 0x80;
					*(pc_Comp++) = (char) i_Num;
					*(pc_Comp++) = (char) i_Val;
				}
				else if(i_Num <= 0xFFFF)
				{
					*(pc_Comp++) = (char) 0x40;
					sh_pc_Comp = (short *) pc_Comp;
#ifdef PSX2_TARGET
					/* mamagouille */
					toto = (char *) sh_pc_Comp;
					toto[0] = ((char *) &i_Num)[0];
					toto[1] = ((char *) &i_Num)[1];
					sh_pc_Comp++;
#else
					*(sh_pc_Comp++) = (short) i_Num;
#endif
					pc_Comp = (char *) sh_pc_Comp;
					*(pc_Comp++) = (char) i_Val;
				}
				else
				{
					ERR_X_Assert(0);
				}

				if(c_End) goto l_End;

				i_Val = pst_RealArray->c_Capacities & _i_Mask;
				i_Num = 1;
			}

			pst_RealArray++;
			xt++;
		}

		xt = x;
		y++;
		pst_RealArray += woff;
	}

	c_End = 1;
	goto Flush;

l_End:
	ul_Size = pc_Comp - _pc_Comp;
	*(int *) _pc_Comp = ul_Size;

	/* Special case : All is 0 */
	if((i_Val == 0) && (i_NumDif == 0)) return 0;
	return ul_Size;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_UnCompress(GRID_tdst_World *_pst_Grid, GRID_tdst_Elem *_pst_Array, int x, int y)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			*pc_Buf;
	short			*sh_pc_Buf;
	int				xend, yend;
	int				xt;
	int				of, woff;
	char			c_Val, c_Mask;
	int				i_Num;
	GRID_tdst_Elem	*pst_RealArray;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	c_Val = 0;

	/* Choose the right group to decompress */
	if(_pst_Array == _pst_Grid->pst_RealArray)
	{
		if(_pst_Grid->w_XRealGroup + x >= _pst_Grid->w_NumGroupsX) return;
		if(_pst_Grid->w_YRealGroup + y >= _pst_Grid->w_NumGroupsY) return;

		of = (_pst_Grid->w_XRealGroup + x) + ((_pst_Grid->w_YRealGroup + y) * _pst_Grid->w_NumGroupsX);
		x *= _pst_Grid->c_SizeGroup;
		y *= _pst_Grid->c_SizeGroup;
		pst_RealArray = _pst_Grid->pst_RealArray + x + (y * _pst_Grid->c_SizeGroup * _pst_Grid->w_NumRealGroupsX);
		woff = _pst_Grid->c_SizeGroup * (_pst_Grid->w_NumRealGroupsX - 1);
	}
	else
	{
		of = x + (y * _pst_Grid->w_NumGroupsX);
		x *= _pst_Grid->c_SizeGroup;
		y *= _pst_Grid->c_SizeGroup;
		pst_RealArray = _pst_Grid->pst_EditArray + x + (y * _pst_Grid->c_SizeGroup * _pst_Grid->w_NumGroupsX);
		woff = _pst_Grid->c_SizeGroup * (_pst_Grid->w_NumGroupsX - 1);
	}

	if(_pst_Grid->p_RealGroups[of])
	{
		pc_Buf = (char *) _pst_Grid->p_RealGroups[of];
	}
	else
	{
		if((int) _pst_Grid->p_Groups[of] == -1)
			pc_Buf = NULL;
		else
			pc_Buf = ((char *) _pst_Grid->p_CompressBuf) + (int) _pst_Grid->p_Groups[of];
	}

	x *= _pst_Grid->c_SizeGroup;
	y *= _pst_Grid->c_SizeGroup;
	xend = x + _pst_Grid->c_SizeGroup;
	yend = y + _pst_Grid->c_SizeGroup;
	if(pc_Buf) pc_Buf += 4; /* Zap size */
	i_Num = 0;
	xt = x;

	while(y != yend)
	{
		if(!i_Num)
		{
uncomp:
			if(!pc_Buf)
			{
				i_Num = _pst_Grid->c_SizeGroup * _pst_Grid->c_SizeGroup;
				c_Val = 0;
			}
			else
			{
				if((c_Mask = (*pc_Buf &~(COMPRESS_MASK))))
				{
					if(c_Mask == (char) 0xC0)
					{
						i_Num = *pc_Buf & COMPRESS_MASK;
						pc_Buf++;
						c_Val = *(pc_Buf++);
					}
					else if(c_Mask == (char) 0x80)
					{
						pc_Buf++;
						i_Num = *(pc_Buf++);
						i_Num = i_Num & 0xFF;
						c_Val = *(pc_Buf++);
					}
					else if(c_Mask == (char) 0x40)
					{
						pc_Buf++;
						sh_pc_Buf = (short *) pc_Buf;
#ifdef PSX2_TARGET
						/* mamagouille */
						i_Num = ReadLong((char *) sh_pc_Buf);
#else
						i_Num = *sh_pc_Buf;
#endif
						sh_pc_Buf++;
						pc_Buf = (char *) sh_pc_Buf;
						i_Num = i_Num & 0xFFFF;
						c_Val = *(pc_Buf++);
					}
				}
				else
				{
					i_Num = 1;
					c_Val = *(pc_Buf++);
				}
			}
		}

		while(xt != xend)
		{
			pst_RealArray->c_Capacities = c_Val;
			i_Num--;
			xt++;
			pst_RealArray++;
			if(!i_Num) goto uncomp;
		}

		xt = x;
		pst_RealArray += woff;
		y++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_CompressReal(GRID_tdst_World *_pst_Grid, int xnew, int ynew)
{
	/*~~~~~~~~~~~~~*/
	int		i, j, of;
	char	*pc_Buf;
	ULONG	ul_Size;
	int		i_Mask;
	/*~~~~~~~~~~~~~*/

	/* Temp buffer to receive compression result */
	pc_Buf = (char *) MEM_p_Alloc(4 + (_pst_Grid->c_SizeGroup * _pst_Grid->c_SizeGroup * sizeof(GRID_tdst_Elem)));

	for(i = 0; i < _pst_Grid->w_NumRealGroupsX; i++)
	{
		if(_pst_Grid->w_XRealGroup + i >= _pst_Grid->w_NumGroupsX) continue;
		for(j = 0; j < _pst_Grid->w_NumRealGroupsY; j++)
		{
			if(_pst_Grid->w_YRealGroup + j >= _pst_Grid->w_NumGroupsY)
				break;

			/*
			 * Compute the mask. If the group will be present after, we compress static and
			 * dynamic datas. Else we only compress static datas
			 */
			if
			(
				(i + _pst_Grid->w_XRealGroup < xnew)
			||	(j + _pst_Grid->w_YRealGroup < ynew)
			||	(i + _pst_Grid->w_XRealGroup >= xnew + _pst_Grid->w_NumRealGroupsX)
			||	(j + _pst_Grid->w_YRealGroup >= ynew + _pst_Grid->w_NumRealGroupsY)
			)
			{
				/* No need to compress, cause the static flags are already present in p_Groups */
				ul_Size = 0;
				i_Mask = MASK_STATIC;

#ifdef ACTIVE_EDITORS
				GRI_UnCompress
				(
					_pst_Grid,
					_pst_Grid->pst_EditArray,
					i + _pst_Grid->w_XRealGroup,
					j + _pst_Grid->w_YRealGroup
				);
#endif
			}
			else
			{
				/* Compress */
				i_Mask = MASK_STATDYN;
				ul_Size = GRI_Compress
					(
						_pst_Grid,
						_pst_Grid->pst_RealArray,
						i * _pst_Grid->c_SizeGroup,
						j * _pst_Grid->c_SizeGroup,
						pc_Buf,
						i_Mask
					);
			}

			/* Copy in the result in a new compressed buffer if there's something */
			of = (_pst_Grid->w_XRealGroup + i) + ((j + _pst_Grid->w_YRealGroup) * _pst_Grid->w_NumGroupsX);
			if(ul_Size)
			{
				if((!_pst_Grid->p_RealGroups[of]) || (ul_Size != *((ULONG *) _pst_Grid->p_RealGroups[of])))
				{
					if(_pst_Grid->p_RealGroups[of]) MEM_Free(_pst_Grid->p_RealGroups[of]);
					_pst_Grid->p_RealGroups[of] = MEM_p_Alloc(ul_Size);
				}

				L_memcpy(_pst_Grid->p_RealGroups[of], pc_Buf, ul_Size);
			}

			/* Else no real group */
			else
			{
				if(_pst_Grid->p_RealGroups[of])
				{
					MEM_Free(_pst_Grid->p_RealGroups[of]);
					_pst_Grid->p_RealGroups[of] = NULL;
				}
			}
		}
	}

	MEM_Free(pc_Buf);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_UnCompressReal(GRID_tdst_World *_pst_Grid)
{
	/*~~~~~*/
	int i, j;
	/*~~~~~*/

	for(i = 0; i < _pst_Grid->w_NumRealGroupsX; i++)
	{
		for(j = 0; j < _pst_Grid->w_NumRealGroupsY; j++)
		{
			GRI_UnCompress(_pst_Grid, _pst_Grid->pst_RealArray, i, j);
		}
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_Destroy(GRID_tdst_World *_pst_Grid)
{
	/*~~*/
	int i;
	/*~~*/

	if(!_pst_Grid) return;

	for(i = 0; i < _pst_Grid->w_NumGroupsX * _pst_Grid->w_NumGroupsY; i++)
	{
		if(_pst_Grid->p_RealGroups[i])
		{
			MEM_Free(_pst_Grid->p_RealGroups[i]);
		}
	}

	if(_pst_Grid->p_CompressBuf)
	{
		LOA_DeleteAddress(_pst_Grid->p_CompressBuf);
		MEM_Free(_pst_Grid->p_CompressBuf);
	}

	if(_pst_Grid->p_Groups)
	{
		MEM_Free(_pst_Grid->p_Groups);
	}

	if(_pst_Grid->p_RealGroups)
	{
		MEM_Free(_pst_Grid->p_RealGroups);
	}

	if(_pst_Grid->pst_RealArray)
	{
		MEM_Free(_pst_Grid->pst_RealArray);
	}

	if(_pst_Grid->pst_EvalArray)
	{
		MEM_Free(_pst_Grid->pst_EvalArray);
	}

#ifdef ACTIVE_EDITORS
	if(_pst_Grid->pst_EditArray)
	{
		MEM_Free(_pst_Grid->pst_EditArray);
	}

	if(_pst_Grid->p_GameObject)
	{
		OBJ_GameObject_Remove((OBJ_tdst_GameObject *) _pst_Grid->p_GameObject, 1);
		LINK_DelRegisterPointer(_pst_Grid->p_GameObject);
	}

	LINK_DelRegisterPointer(_pst_Grid);
#endif
	LOA_DeleteAddress(_pst_Grid);
	MEM_Free(_pst_Grid);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRID_SetRealCenter(GRID_tdst_World *_pst_Grid, int x, int y)
{
	/*~~~~~~~~~~~*/
	int xnew, ynew;
	int f1;
	/*~~~~~~~~~~~*/

	if(_pst_Grid->maxi - _pst_Grid->mini + 1 <= GRID_REAL_X)
		xnew = _pst_Grid->mini;
	else
	{
		f1 = (int) ((_pst_Grid->w_NumRealGroupsX / 2.0f) + 0.5f);
		xnew = (x / _pst_Grid->c_SizeGroup) - f1;
		if(xnew < _pst_Grid->mini)
			xnew = _pst_Grid->mini;
		else if(xnew + f1 >= _pst_Grid->maxi)
			xnew = _pst_Grid->maxi - f1;
	}

	if(_pst_Grid->maxj - _pst_Grid->minj + 1 <= GRID_REAL_Y)
		ynew = _pst_Grid->minj;
	else
	{
		f1 = (int) ((_pst_Grid->w_NumRealGroupsY / 2.0f) + 0.5f);
		ynew = (y / _pst_Grid->c_SizeGroup) - f1;
		if(ynew < _pst_Grid->minj)
			ynew = _pst_Grid->minj;
		else if(ynew + f1 >= _pst_Grid->maxj)
			ynew = _pst_Grid->maxj - f1;
	}

	if((xnew == _pst_Grid->w_XRealGroup) && (ynew == _pst_Grid->w_YRealGroup)) return;

	/* Compress current array */
	GRI_CompressReal(_pst_Grid, xnew, ynew);

	/* Uncompress at the new position */
	_pst_Grid->w_XRealGroup = xnew;
	_pst_Grid->w_YRealGroup = ynew;
	GRI_UnCompressReal(_pst_Grid);

	/* Update coords min */
	_pst_Grid->f_MinXReal = (float) (_pst_Grid->w_XRealGroup * _pst_Grid->c_SizeGroup);
	_pst_Grid->f_MinYReal = (float) (_pst_Grid->w_YRealGroup * _pst_Grid->c_SizeGroup);
	_pst_Grid->f_MinXReal += _pst_Grid->f_MinXTotal;
	_pst_Grid->f_MinYReal += _pst_Grid->f_MinYTotal;
	/* Philippe -> Update SPG2 */
	{
		extern void SPG2_SpecialGridHasChanged();
		SPG2_SpecialGridHasChanged();
	}

}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_UncompressEdit(GRID_tdst_World *_pst_Grid)
{
	/*~~~~~*/
	int i, j;
	/*~~~~~*/

	for(i = 0; i < _pst_Grid->w_NumGroupsX; i++)
	{
		for(j = 0; j < _pst_Grid->w_NumGroupsY; j++)
		{
			GRI_UnCompress(_pst_Grid, _pst_Grid->pst_EditArray, i, j);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GRI_UpdateCompress(GRID_tdst_World *_pst_Grid)
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG		ul_Total;
	char		*pc_Buf;
	int			i, j;
	ULONG		ul_Size;
	ULONG		ul_Num;
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(_pst_Grid->p_CompressBuf);
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Grid->p_CompressBuf);
	LOA_DeleteAddress(_pst_Grid->p_CompressBuf);
	MEM_Free(_pst_Grid->p_CompressBuf);

	/* Buffer to receive compression result */
	ul_Size = _pst_Grid->c_SizeGroup * _pst_Grid->w_NumGroupsX;
	ul_Size *= _pst_Grid->c_SizeGroup * _pst_Grid->w_NumGroupsY;
	ul_Size *= sizeof(GRID_tdst_Elem);
#ifdef JADEFUSION
	pc_Buf = (char*)MEM_p_Alloc(2 * ul_Size);
#else
	pc_Buf = MEM_p_Alloc(2 * ul_Size);
#endif
	_pst_Grid->p_CompressBuf = pc_Buf;
	LOA_AddAddress(ul_Index, pc_Buf);

	/* Compress actual array */
	ul_Total = 0;
	for(i = 0; i < _pst_Grid->w_NumGroupsX; i++)
	{
		for(j = 0; j < _pst_Grid->w_NumGroupsY; j++)
		{
			ul_Size = GRI_Compress
				(
					_pst_Grid,
					_pst_Grid->pst_EditArray,
					i * _pst_Grid->c_SizeGroup,
					j * _pst_Grid->c_SizeGroup,
					pc_Buf + ul_Total,
					MASK_STATIC
				);
			ul_Num = i + (j * _pst_Grid->w_NumGroupsX);
			if(ul_Size)
			{
				_pst_Grid->p_Groups[ul_Num] = (void *) ul_Total;
			}
			else
			{
				_pst_Grid->p_Groups[ul_Num] = (void *) - 1;
			}

			ul_Total += ul_Size;
		}
	}

	return ul_Total;
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_Reinit(GRID_tdst_World *_pst_Grid)
{
	/*~~*/
	int i;
	int j, of;
	ULONG ul_Size;
	/*~~*/

#ifdef ACTIVE_EDITORS
	if(_pst_Grid->pst_EvalArray)
	{
		MEM_Free(_pst_Grid->pst_EvalArray);
		_pst_Grid->pst_EvalArray = NULL;
	}
#endif

	if(!_pst_Grid->pst_EvalArray)
	{
		_pst_Grid->mini = _pst_Grid->minj = 0;
		_pst_Grid->maxi = GRID_REAL_X;
		_pst_Grid->maxj = GRID_REAL_Y;
		_pst_Grid->mini = _pst_Grid->minj = 1000;
		_pst_Grid->maxi = _pst_Grid->maxj = -1;

		/* On determine les groupes extremes qui contiennent reellement qq chose */
		for(i = 0; i < _pst_Grid->w_NumGroupsX; i++)
		{
			for(j = 0; j < _pst_Grid->w_NumGroupsY; j++)
			{
				of = i + (j * _pst_Grid->w_NumGroupsX);
				if((int) _pst_Grid->p_Groups[of] != -1)
				{
					if(i < _pst_Grid->mini) _pst_Grid->mini = i;
					if(j < _pst_Grid->minj) _pst_Grid->minj = j;
					if(i > _pst_Grid->maxi) _pst_Grid->maxi = i;
					if(j > _pst_Grid->maxj) _pst_Grid->maxj = j;
				}
			}
		}

		if((_pst_Grid->maxi == -1)  || (_pst_Grid->maxj == -1))
		{
			_pst_Grid->w_NumRealGroupsX = 0;
			_pst_Grid->w_NumRealGroupsY = 0;
		}
		else
		{
			_pst_Grid->w_NumRealGroupsX = _pst_Grid->maxi - _pst_Grid->mini + 1;
			_pst_Grid->w_NumRealGroupsY = _pst_Grid->maxj - _pst_Grid->minj + 1;

			if(_pst_Grid->w_NumRealGroupsX > GRID_REAL_X)
			{
				_pst_Grid->w_NumRealGroupsX = GRID_REAL_X;
				_pst_Grid->mini = 0;
				_pst_Grid->maxi = _pst_Grid->w_NumGroupsX;
			}

			if(_pst_Grid->w_NumRealGroupsY > GRID_REAL_Y)
			{
				_pst_Grid->w_NumRealGroupsY = GRID_REAL_Y;
				_pst_Grid->minj = 0;
				_pst_Grid->maxj = _pst_Grid->w_NumGroupsY;
			}
		}

		ul_Size = _pst_Grid->w_NumRealGroupsX * _pst_Grid->c_SizeGroup;
		ul_Size *= _pst_Grid->w_NumRealGroupsY * _pst_Grid->c_SizeGroup;
		ul_Size *= sizeof(GRID_tdst_Eval);
		if(ul_Size) _pst_Grid->pst_EvalArray = (GRID_tdst_Eval *) MEM_p_VMAlloc(ul_Size);
		LOA_ul_FileTypeSize[39] += ul_Size;
		L_memset(_pst_Grid->pst_EvalArray, 0, ul_Size);
	}
	else
	{
	}

	for(i = 0; i < _pst_Grid->w_NumGroupsX * _pst_Grid->w_NumGroupsY; i++)
	{
		if(_pst_Grid->p_RealGroups[i])
		{
			MEM_Free(_pst_Grid->p_RealGroups[i]);
			_pst_Grid->p_RealGroups[i] = NULL;
		}
	}

	/* Set real array with correct values */
	_pst_Grid->w_XRealGroup = _pst_Grid->mini;
	_pst_Grid->w_YRealGroup = _pst_Grid->minj;
	GRI_UnCompressReal(_pst_Grid);

	/* Philippe -> Update SPG2 */
	{
		extern void SPG2_SpecialGridHasChanged();
		SPG2_SpecialGridHasChanged();
	}


#ifdef ACTIVE_EDITORS
	GRI_UncompressEdit(_pst_Grid);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GRI_TagBox(struct GRID_tdst_World_ *_pst_Grid, MATH_tdst_Vector *pos, MATH_tdst_Vector *sight, MATH_tdst_Vector *_pst_1, MATH_tdst_Vector *_pst_2, int _i_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				x1, y1;
	MATH_tdst_Vector	st_TmpA, st_TmpB, st_TmpC, st_TmpD;
	MATH_tdst_Vector	st_AddX, st_AddY, st_AddXE, st_AddYE;
	MATH_tdst_Vector	st_X, st_Y;
	float				normx, normy;
	int					x2d, y2d;
	int					ai_Already[512];
	int					i_NumAlready;
	int					i;
	MATH_tdst_Matrix	M;
	float				stepx, stepy;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SetIdentityMatrix(&M);
	MATH_CopyVector((MATH_tdst_Vector *) &M.Jx, sight);
	st_X.x = st_X.y = 0;
	st_X.z = 1.0f;
	MATH_CrossProduct((MATH_tdst_Vector *) &M.Ix, sight, &st_X);
	MATH_CopyVector((MATH_tdst_Vector *) &M.Kx, &st_X);
	MATH_CopyVector(&M.T, pos);

	MATH_CopyVector(&st_TmpA, _pst_1);
	MATH_CopyVector(&st_TmpB, _pst_1);
	MATH_CopyVector(&st_TmpC, _pst_2);
	MATH_CopyVector(&st_TmpD, _pst_2);
	st_TmpB.y = st_TmpC.y;
	st_TmpD.y = st_TmpA.y;

	MATH_TransformVertex(&st_TmpA, &M, &st_TmpA);
	MATH_TransformVertex(&st_TmpB, &M, &st_TmpB);
	MATH_TransformVertex(&st_TmpC, &M, &st_TmpC);
	MATH_TransformVertex(&st_TmpD, &M, &st_TmpD);

	MATH_SubVector(&st_AddX, &st_TmpD, &st_TmpA);
	MATH_SubVector(&st_AddY, &st_TmpB, &st_TmpA);
	normx = MATH_f_NormVector(&st_AddX);
	normy = MATH_f_NormVector(&st_AddY);

	stepx = fMin(normx * 0.5f, 0.5f);
	stepy = fMin(normy * 0.5f, 0.5f);

	i_NumAlready = 0;
	for(y1 = 0; y1 < normy + stepy; y1 += stepy)
	{
		if(y1 > normy) 
			MATH_SetNormVector(&st_AddYE, &st_AddY, normy);
		else
			MATH_SetNormVector(&st_AddYE, &st_AddY, y1);
		MATH_AddVector(&st_Y, &st_AddYE, &st_TmpA);
		for(x1 = 0; x1 < normx + stepx; x1 += stepx)
		{
			if(x1 > normx) 
				MATH_SetNormVector(&st_AddXE, &st_AddX, normx);
			else
				MATH_SetNormVector(&st_AddXE, &st_AddX, x1);
			MATH_AddVector(&st_X, &st_AddXE, &st_Y);

			/* Pas deux fois */
			if(!GRI_3Dto2D(_pst_Grid, &st_X, &x2d, &y2d)) continue;
			x2d += (y2d << 16);
			for(i = 0; i < i_NumAlready; i++)
			{
				if(ai_Already[i] == x2d) goto end;
			}
			ai_Already[i_NumAlready++] = x2d;

			/* Si une case est pleine, on vire tout ce qu'on a mis, et erreur */
			if(!GRI_AddCapa(_pst_Grid, &st_X, (char) _i_Val, NULL))
			{
				for(i = 0; i < i_NumAlready - 1; i++)
				{
					GRI_AddCapa(_pst_Grid, &st_X, (char) -_i_Val, &ai_Already[i]);
				}

				return 0;
			}
end:;
		}
	}

	return 1;
}

/*
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_SetWidth(GRID_tdst_World *_pst_Grid, int cx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		ul_Size;
	int			i, j, max;
	BIG_INDEX	ul_Index;
	void		**p_NewGrp;
	void		**psrc, **pdest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(cx < _pst_Grid->w_NumRealGroupsX) cx = _pst_Grid->w_NumRealGroupsX;
	max = cx < _pst_Grid->w_NumGroupsX ? cx : _pst_Grid->w_NumGroupsX;

	/* Groups */
	p_NewGrp = (void **) MEM_p_Alloc(cx * _pst_Grid->w_NumGroupsY * sizeof(void *));
	L_memset(p_NewGrp, -1, cx * _pst_Grid->w_NumGroupsY * sizeof(void *));
	psrc = _pst_Grid->p_Groups;
	pdest = p_NewGrp;
	for(j = 0; j < _pst_Grid->w_NumGroupsY; j++)
	{
		for(i = 0; i < max; i++)
		{
			*(pdest++) = *(psrc++);
		}

		if(cx < _pst_Grid->w_NumGroupsX)
			psrc += (_pst_Grid->w_NumGroupsX - cx);
		else
			pdest += (cx - _pst_Grid->w_NumGroupsX);
	}

	MEM_Free(_pst_Grid->p_Groups);
	_pst_Grid->p_Groups = p_NewGrp;

	/* Real groups */
	for(i = 0; i < _pst_Grid->w_NumGroupsX * _pst_Grid->w_NumGroupsY; i++)
	{
		if(_pst_Grid->p_RealGroups[i])
		{
			MEM_Free(_pst_Grid->p_RealGroups[i]);
			_pst_Grid->p_RealGroups[i] = NULL;
		}
	}

	_pst_Grid->p_RealGroups = (void **) MEM_p_Realloc
		(
			_pst_Grid->p_RealGroups,
			cx * _pst_Grid->w_NumGroupsY * sizeof(void *)
		);
	L_memset(_pst_Grid->p_RealGroups, 0, cx * _pst_Grid->w_NumGroupsY * sizeof(void *));

	/* Compress buf */
	ul_Size = cx * _pst_Grid->c_SizeGroup;
	ul_Size *= _pst_Grid->w_NumGroupsY * _pst_Grid->c_SizeGroup;
	ul_Size *= sizeof(GRID_tdst_Elem);
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Grid->p_CompressBuf);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	LOA_DeleteAddress(_pst_Grid->p_CompressBuf);
	_pst_Grid->p_CompressBuf = (void *) MEM_p_Realloc(_pst_Grid->p_CompressBuf, ul_Size);
	LOA_AddAddress(ul_Index, _pst_Grid->p_CompressBuf);

	/* Editor array */
	_pst_Grid->pst_EditArray = (GRID_tdst_Elem *) MEM_p_Realloc(_pst_Grid->pst_EditArray, ul_Size);
	L_memset(_pst_Grid->pst_EditArray, 0, ul_Size);

	/* Reinit grid */
	_pst_Grid->w_NumGroupsX = cx;
	GRI_Reinit(_pst_Grid);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_SetHeight(GRID_tdst_World *_pst_Grid, int cy)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		ul_Size;
	int			i, j, max;
	BIG_INDEX	ul_Index;
	void		**p_NewGrp;
	void		**psrc, **pdest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(cy < _pst_Grid->w_NumRealGroupsY) cy = _pst_Grid->w_NumRealGroupsY;
	max = cy < _pst_Grid->w_NumGroupsY ? cy : _pst_Grid->w_NumGroupsY;

	/* Groups */
	p_NewGrp = (void **) MEM_p_Alloc(cy * _pst_Grid->w_NumGroupsX * sizeof(void *));
	L_memset(p_NewGrp, -1, cy * _pst_Grid->w_NumGroupsX * sizeof(void *));
	psrc = _pst_Grid->p_Groups;
	pdest = p_NewGrp;
	for(j = 0; j < max; j++)
	{
		for(i = 0; i < _pst_Grid->w_NumGroupsX; i++)
		{
			*(pdest++) = *(psrc++);
		}
	}

	MEM_Free(_pst_Grid->p_Groups);
	_pst_Grid->p_Groups = p_NewGrp;

	/* Real groups */
	for(i = 0; i < _pst_Grid->w_NumGroupsX * _pst_Grid->w_NumGroupsY; i++)
	{
		if(_pst_Grid->p_RealGroups[i])
		{
			MEM_Free(_pst_Grid->p_RealGroups[i]);
			_pst_Grid->p_RealGroups[i] = NULL;
		}
	}

	_pst_Grid->p_RealGroups = (void **) MEM_p_Realloc
		(
			_pst_Grid->p_RealGroups,
			cy * _pst_Grid->w_NumGroupsX * sizeof(void *)
		);
	L_memset(_pst_Grid->p_RealGroups, 0, cy * _pst_Grid->w_NumGroupsX * sizeof(void *));

	/* Compress buf */
	ul_Size = cy * _pst_Grid->c_SizeGroup;
	ul_Size *= _pst_Grid->w_NumGroupsX * _pst_Grid->c_SizeGroup;
	ul_Size *= sizeof(GRID_tdst_Elem);
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Grid->p_CompressBuf);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	LOA_DeleteAddress(_pst_Grid->p_CompressBuf);
	_pst_Grid->p_CompressBuf = (void *) MEM_p_Realloc(_pst_Grid->p_CompressBuf, ul_Size);
	LOA_AddAddress(ul_Index, _pst_Grid->p_CompressBuf);

	/* Editor array */
	_pst_Grid->pst_EditArray = (GRID_tdst_Elem *) MEM_p_Realloc(_pst_Grid->pst_EditArray, ul_Size);
	L_memset(_pst_Grid->pst_EditArray, 0, ul_Size);

	/* Reinit grid */
	_pst_Grid->w_NumGroupsY = cy;
	GRI_Reinit(_pst_Grid);
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL TestRay(GRID_tdst_World *_pst_Grid, MATH_tdst_Vector *dst, MATH_tdst_Vector *src)
{
	MATH_tdst_Vector	tv_temp, src1;
	float				tf_norm, tf_norm1, tf_tot;
	int					x, y, x1, y1;

	if(!GRI_3Dto2D(_pst_Grid, src, &x, &y)) return TRUE;
	if(!GRI_ConvertTo2DReal(_pst_Grid, &x, &y)) return TRUE;
	if(!GRI_3Dto2D(_pst_Grid, src, &x1, &y1)) return TRUE;
	if(!GRI_ConvertTo2DReal(_pst_Grid, &x1, &y1)) return TRUE;

	if(M_WallNoDyn(_pst_Grid, x, y)) return TRUE;
	if(M_WallNoDyn(_pst_Grid, x1, y1)) return TRUE;

	MATH_SubVector(&tv_temp, dst, src);
	tv_temp.z = 0;
	tf_norm = MATH_f_NormVector(&tv_temp);
	if(tf_norm < 0.1f) return FALSE;

	tf_norm1 = fMax(fAbs(tv_temp.x), fAbs(tv_temp.y));
	tv_temp.x /= tf_norm1;
	tv_temp.y /= tf_norm1;
	tf_norm1 = MATH_f_NormVector(&tv_temp);

	tf_tot = 0;
	MATH_CopyVector(&src1, src);
	while(tf_tot < tf_norm)
	{
		MATH_AddEqualVector(&src1, &tv_temp);
		tf_tot += tf_norm1;
		if(tf_tot > tf_norm) 
		{
			MATH_CopyVector(&src1, dst);
			tf_tot = tf_norm;
		}

		if(!GRI_3Dto2D(_pst_Grid, &src1, &x, &y)) return TRUE;
		if(!GRI_ConvertTo2DReal(_pst_Grid, &x, &y)) return TRUE;
		if(M_WallNoDyn(_pst_Grid, x, y)) return TRUE;
	}

	return FALSE;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
