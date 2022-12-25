/*$T GFXtable.c GC! 1.081 10/10/01 12:45:45 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "GDInterface/GDInterface.h"
#include "GFX/GFX.h"
#include "GFX/GFXtable.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTstruct.h"
#include "STRing/STRstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#ifdef PSX2_TARGET

/* mamagouille */
#include "PSX2debug.h"
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Table Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Table_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Table	*pst_Data;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Table *) MEM_p_Alloc(sizeof(GFX_tdst_Table));
	L_memset(pst_Data, 0, sizeof(GFX_tdst_Table));

	pst_Data->c_NbCol = 10;
	pst_Data->c_NbRow = 10;

	for(i = 0; i < 10; i++)
	{
		pst_Data->ul_Color[i] = 0xFFFFFFFF;
		pst_Data->f_RowSize[i] = 0.1f;
		pst_Data->f_ColSize[i] = 0.1f;
	}

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Table_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector            st_Temp;
	GFX_tdst_Table				*pst_Data;
	GEO_Vertex					*V;
	ULONG						*pul_RLI, ul_Color, DM;
	GEO_tdst_IndexedTriangle	*T;
	float						x1, y1, x2, y2, x3, y3, x4, y4, X1, X2, Y1, Y2;
	int							i, row, col, cases;
    static int test = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Table *) p_Data;

	cases = pst_Data->c_NbCol * pst_Data->c_NbRow;
	if(!cases) return 1;

	GFX_NeedGeom(4 * cases, 4, 2 * cases, 1);

	/* set UV */
	GFX_gpst_Geo->dst_UV[0].fU = 0;
	GFX_gpst_Geo->dst_UV[0].fV = 0;
	GFX_gpst_Geo->dst_UV[1].fU = 1;
	GFX_gpst_Geo->dst_UV[1].fV = 0;
	GFX_gpst_Geo->dst_UV[2].fU = 1;
	GFX_gpst_Geo->dst_UV[2].fV = 1;
	GFX_gpst_Geo->dst_UV[3].fU = 0;
	GFX_gpst_Geo->dst_UV[3].fV = 1;

	/* set vertex */
	
    x1 = 0;
    x2 = GDI_gpst_CurDD->st_Camera.f_Width;
    y1 = GDI_gpst_CurDD->st_Camera.f_Height;
    y2 = 0;
    
    /*
    x1 = (float) GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft;
	x2 = x1 + GDI_gpst_CurDD->st_Camera.f_Width;
	y1 = (float) GDI_gpst_CurDD->st_Camera.l_ViewportRealTop;
	y2 = y1 + GDI_gpst_CurDD->st_Camera.f_Height;
    */

	pul_RLI = GFX_gpst_Geo->dul_PointColors + 1;
	V = GFX_gpst_Geo->dst_Point;
	T = GFX_gpst_Geo->dst_Element->dst_Triangle;

#if defined(_XBOX) || defined(_XENON)
	y4 = y1 + ( pst_Data->st_Pos.y + pst_Data->f_RowSize[0] ) * (y2 - y1);
#else
	y4 = y1 + ( pst_Data->st_Pos.y ) * (y2 - y1);
#endif


	for(row = 0; row < pst_Data->c_NbRow; row++)
	{
		y3 = y4;
        y4 += pst_Data->f_RowSize[row] * (y2 - y1);

		x4 = x1 + pst_Data->st_Pos.x * (x2 - x1);
		for(col = 0; col < pst_Data->c_NbCol; col++)
		{
			x3 = x4;
			x4 += pst_Data->f_ColSize[col] * (x2 - x1);

			ul_Color = pst_Data->ul_Color[pst_Data->ac_Content[row][col]];

			i = V - GFX_gpst_Geo->dst_Point;
			T->auw_Index[0] = i;
			T->auw_Index[1] = i + 1;
			T->auw_Index[2] = i + 2;
			T->auw_UV[0] = 0;
			T->auw_UV[1] = 1;
			T->auw_UV[2] = 2;
			T++;
			T->auw_Index[0] = i + 2;
			T->auw_Index[1] = i + 3;
			T->auw_Index[2] = i;
			T->auw_UV[0] = 2;
			T->auw_UV[1] = 3;
			T->auw_UV[2] = 0;
			T++;

			X1 = x3;
			X2 = x4;
			Y1 = y3;
			Y2 = y4;
            //STR_M_RecalageSprite( x3, y3, x4, y4 );
			STR_M_RecalageSprite( X1, Y1, X2, Y2 );

			//MATH_InitVector(V++, x3, y3, pst_Data->st_Pos.z);
			//MATH_InitVector(V++, x4, y3, pst_Data->st_Pos.z);
			//MATH_InitVector(V++, x4, y4, pst_Data->st_Pos.z);
			//MATH_InitVector(V++, x3, y4, pst_Data->st_Pos.z);
			
			MATH_InitVector(V++, X1, Y1, pst_Data->st_Pos.z);
			MATH_InitVector(V++, X2, Y1, pst_Data->st_Pos.z);
			MATH_InitVector(V++, X2, Y2, pst_Data->st_Pos.z);
			MATH_InitVector(V++, X1, Y2, pst_Data->st_Pos.z);

			*pul_RLI++ = ul_Color;
			*pul_RLI++ = ul_Color;
			*pul_RLI++ = ul_Color;
            *pul_RLI++ = ul_Color;
		}
	}

	V = GFX_gpst_Geo->dst_Point;
	for(i = 0; i < 4 * cases; i++)
	{

#if defined(_XBOX) || defined(_XENON)
		V[i].y*=1.1f;
#endif

        CAM_2Dto3DCamera2(&GDI_gpst_CurDD->st_Camera, &st_Temp, VCast(&V[i]));
        MATH_TransformVertex(VCast(&V[i]), &GDI_gpst_CurDD->st_Camera.st_Matrix, &st_Temp);
        //CAM_2Dto3D(&GDI_gpst_CurDD->st_Camera, VCast(&V[i]), VCast(&V[i]));
	}

    M_GFX_CheckGeom();

	DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
	GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_Lighted);
	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
	GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Table_Setv(void *p_Data, int i_Param, MATH_tdst_Vector *V)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Table	*pst_Data;
	int				row, col;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Table *) p_Data;

	switch(i_Param)
	{
	case GFX_TableRowSize:
		row = (int) V->x;
        if (row == -1)
        {
            for (row = 0; row < 10; row++)
                pst_Data->f_RowSize[row] = V->y;
        }
        else
        {
		    if ((row < 0) || (row > 10)) return;
		    pst_Data->f_RowSize[row] = V->y;
        }
		break;
	case GFX_TableColSize:
		col = (int) V->x;
        if (col == -1)
        {
            for (col = 0; col < 10; col++)
                pst_Data->f_ColSize[col] = V->y;
        }
		if ((col < 0) || (col > 10))return;
		pst_Data->f_ColSize[col] = V->y;
		break;
	case GFX_TableCaseContent:
		row = (int) V->x;
		if ((row < 0) || (row > 10)) return;
		col = (int) V->y;
		if ((col < 0) || (col > 10))return;
		pst_Data->ac_Content[row][col] = (char) V->z;
        if ( pst_Data->ac_Content[row][col] < 0)
            pst_Data->ac_Content[row][col] = 0;
        if ( pst_Data->ac_Content[row][col] >= 10)
            pst_Data->ac_Content[row][col] = 9;
		break;
	}
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
