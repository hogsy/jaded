/*$T GFXline.c GC! 1.081 10/23/00 16:31:11 */


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
#include "GFX/GFXline.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
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
    Functions
 ***********************************************************************************************************************
 */
#pragma

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Line_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Line	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Line *) MEM_p_Alloc(sizeof(GFX_tdst_Line));
	
	L_memset(pst_Data , 0 , sizeof(GFX_tdst_Line));

	MATH_InitVector(&pst_Data->st_A, -10, 0, 0);
	MATH_InitVector(&pst_Data->st_B, 10, 10, 0);
	MATH_InitVector(&pst_Data->Z, 0, 0, 1);
	pst_Data->f_Size = 0.1f;
    pst_Data->ul_Color = 0xFFFFFFFF;

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Line_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Line		*pst_Data;
	MATH_tdst_Vector	X, Y, P, Q;
    ULONG               DM, *pul_RLI;
    float               zref;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Line *) p_Data;

/*#ifdef JADEFUSION
   	MATH_SubVector(&X, &pst_Data->st_B, &pst_Data->st_A);

    if ( MATH_b_NulVector(&X) )
    {
        return;
    }

    MATH_CrossProduct(&Y, &pst_Data->Z, &X);
    if ( MATH_b_NulVector(&Y) )
    {
        return;
    }

    GFX_NeedGeom(4, 4, 2, 1);
#else*/
	GFX_NeedGeom(4, 4, 2, 1);

	//MATH_SubVector(&X, &pst_Data->st_B, &pst_Data->st_A);
	//MATH_CrossProduct(&Y, &pst_Data->Z, &X);
	//MATH_NormalizeEqualVector(&Y);
	//MATH_ScaleEqualVector(&Y, pst_Data->f_Size);

   	MATH_SubVector(&X, &pst_Data->st_B, &pst_Data->st_A);
	
	MATH_CrossProduct(&Y, &pst_Data->Z, &X);

//#endif
	MATH_NormalizeEqualVector(&Y);

    MATH_SubVector( &P, &GDI_gpst_CurDD->st_Camera.st_Matrix.T, &pst_Data->st_A );
    MATH_SubVector( &Q, &pst_Data->st_B, &pst_Data->st_A );
    zref = MATH_f_DotProduct( &P, &Q ) / MATH_f_DotProduct( &Q, &Q );
    if (zref < 0)
        MATH_CopyVector( &P, &pst_Data->st_A );
    else if (zref > 1)
        MATH_CopyVector( &P, &pst_Data->st_B );
    else
        MATH_AddScaleVector( &P, &pst_Data->st_A, &Q, zref );
    MATH_SubEqualVector( &P, &GDI_gpst_CurDD->st_Camera.st_Matrix.T );
    MATH_CrossProduct( &Y, &X, &P );
    
    if ( MATH_b_NulVector( &Y ) )
    {
        MATH_ScaleVector( &Y, &pst_Data->Z, pst_Data->f_Size );
    }
    else
    {
        MATH_NormalizeEqualVector( &Y);
        MATH_ScaleEqualVector( &Y, pst_Data->f_Size );
    }

	MATH_AddVector(VCast(GFX_gpst_Geo->dst_Point), &pst_Data->st_A, &Y);
	MATH_SubVector(VCast(GFX_gpst_Geo->dst_Point + 1), &pst_Data->st_A, &Y);
	MATH_SubVector(VCast(GFX_gpst_Geo->dst_Point + 2), &pst_Data->st_B, &Y);
	MATH_AddVector(VCast(GFX_gpst_Geo->dst_Point + 3), &pst_Data->st_B, &Y);

    pul_RLI = GFX_gpst_Geo->dul_PointColors + 1;
    pul_RLI[0] = pul_RLI[1] = pul_RLI[2] = pul_RLI[3] =  pst_Data->ul_Color;

    GFX_gpst_Geo->dst_UV[0].fU = 0;
    GFX_gpst_Geo->dst_UV[0].fV = 0;
    GFX_gpst_Geo->dst_UV[1].fU = 0;
    GFX_gpst_Geo->dst_UV[1].fV = 1;
    GFX_gpst_Geo->dst_UV[2].fU = 1;
    GFX_gpst_Geo->dst_UV[2].fV = 1;
    GFX_gpst_Geo->dst_UV[3].fU = 1;
    GFX_gpst_Geo->dst_UV[3].fV = 0;


	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[0] = 0;
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[1] = 1;
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[2] = 2;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[0] = 0;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[1] = 1;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[2] = 2;

	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[0] = 2;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[1] = 3;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[2] = 0;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[0] = 2;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[1] = 3;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[2] = 0;

    M_GFX_CheckGeom();

	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_Lighted | GDI_Cul_DM_UseAmbient);
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
}

//#define CODE_TO_PLAY_CHESS
#ifdef USE_DOUBLE_RENDERING	
void  GFX_Line_Interpolate( void *p_Data , u_int Mode , float fInterpoler)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Line	*pst_AF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_AF = (GFX_tdst_Line *)p_Data;
    
    if (!((*(ULONG *)&pst_AF->st_A2.z) & 1)) Mode = 101;

	if (Mode < 100)
	// Render I
	{
			if (Mode == 0)
			{
				pst_AF->st_B2 = pst_AF->st_B1;
				pst_AF->st_A2 = pst_AF->st_A1;
				pst_AF->st_A1 = pst_AF->st_A;
				pst_AF->st_B1 = pst_AF->st_B;
			}
			MATH_BlendVector(&pst_AF->st_B , &pst_AF->st_B2 , &pst_AF->st_B1 , fInterpoler);
			MATH_BlendVector(&pst_AF->st_A , &pst_AF->st_A2 , &pst_AF->st_A1 , fInterpoler);
		} else
	// Render K
	if (Mode == 101)
	{
		pst_AF->st_A2  = pst_AF->st_A1  = pst_AF->st_A;
		pst_AF->st_B2  = pst_AF->st_B1  = pst_AF->st_B;
	} else 
	{
		pst_AF->st_A  = pst_AF->st_A1;
		pst_AF->st_B  = pst_AF->st_B1;
	} 
	
	(*(ULONG *)&pst_AF->st_A2.z) |= 1;
    
}
#endif

#ifndef CODE_TO_PLAY_CHESS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Line_Request(int a, int b, int c, int d)
{
	return 0;
}

#else

#include "e:/myprojects/Chess2/Chess/Chess_Interface.h"

/*$4
 ***********************************************************************************************************************
    chess
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Line_Request(int a, int b, int c, int d)
{
    char type, color;

	switch(a)
	{
	case 1000:
        Chess_p_Log = LINK_PrintStatusMsg;
		Chess_NewGame();
        if (GetAsyncKeyState( VK_SHIFT ) < 0)
            Chess_Bench();
		break; 

	case 1003:
        if ( (c >= 0) && ( c < 8) )
            b = c * 8 + b;

        type = Chess_GetInfos( (char) b, &color );
        return (d == 0) ? color : type;

	case 1004:
		//Chess_BuildAllMoves();
		break;

	case 1005:
		if((c >= 0) && (c < 8)) b = c * 8 + b;
		return Chess_CanBeMoved((char) b);

	case 1006:
		return Chess_IsAMove((char) b, (char) c);

	case 1007:
		return Chess_MakeMove((char) b, (char) c, (char) d);

	case 1010:
		return Chess_CurrentSide();;

    case 1011:
        Chess_TakeBack();
        break;

    case 1100:
        Chess_ComputerPlay();
        break;
	}

	return 0;
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Tetris_Create(void)
{
#if 0
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Tetris	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Tetris *) MEM_p_Alloc(sizeof(GFX_tdst_Tetris));
    L_memset( pst_Data, 0, sizeof( GFX_tdst_Tetris ) );
    L_memset( pst_Data->ac_Grid, -1, TETRIS_LINE * TETRIS_COL );

    MATH_InitVector( &pst_Data->st_Pos, 0, 0, 0 );
	MATH_InitVector( &pst_Data->X, 1, 0, 0);
	MATH_InitVector( &pst_Data->Z, 0, 0, 1);
	pst_Data->f_Size = 0.1f;
    pst_Data->f_Speed = 1;
    pst_Data->c_CurPiece = -1;

    pst_Data->ai_Stat[0] = 1;
    pst_Data->ai_Stat[1] = 1;
    pst_Data->ai_Stat[2] = 1;
    pst_Data->ai_Stat[3] = 1;
    pst_Data->ai_Stat[4] = 1;
    pst_Data->ai_Stat[5] = 1;
   
    pst_Data->ul_Line[0] = 0;
    pst_Data->ul_Line[1] = 0;
    pst_Data->ul_Line[2] = 0;
    pst_Data->ul_Line[3] = 0;
    pst_Data->ul_Line[4] = 0;

	return (void *) pst_Data;
#endif
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define M_Tetris_1to4Points( c )\
    MATH_AddVector( V+1, V, &X );\
    MATH_AddVector( V+2, V+1, &Z );\
    MATH_AddVector( V+3, V, &Z );\
    *pul_RLI++ = c;\
    *pul_RLI++ = c;\
    *pul_RLI++ = c;\
    *pul_RLI++ = c;\
    V+=4;


void GFX_Tetris_Render(void *p_Data)
{
#if 0
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Tetris		        *pst_Data;
	MATH_tdst_Vector	        *V, N, X, Z;
    float                       x, y, f_LineRatio;
    ULONG                       DM, *pul_RLI;
    ULONG                       ul_Color[6] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00, 0xFFFF00FF, 0xFF00FFFF };
    GEO_tdst_IndexedTriangle    *T, *TLast;
    int                         i_NbPoints, i, j, block, newcol, newturn;
    unsigned short              aauw_Data[ 6 ][ 4 ] = { {0x4444,0x00F0,0x4444, 0x00F0}, {0x4460,0x0740,0x0622,0x02E}, {0x2260,0x0470,0x0644,0x0E2}, {0x0360,0x4620,0x0360,0x4620}, {0x0630,0x2640,0x0630,0x2640}, {0x0660,0x0660,0x0660,0x0660} };
    unsigned short              uw_Data, uw_Test, uw_Test2;
    ULONG                       ul_OldLine;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Tetris *) p_Data;
    MATH_ScaleVector( &X, &pst_Data->X, pst_Data->f_Size );
    MATH_ScaleVector( &Z, &pst_Data->Z, pst_Data->f_Size );

    if (pst_Data->c_CurPiece == -1)
    {
        i = pst_Data->ai_Stat[0] + pst_Data->ai_Stat[1] + pst_Data->ai_Stat[2] + pst_Data->ai_Stat[3] + pst_Data->ai_Stat[4] + pst_Data->ai_Stat[5];
        j = i / 6;
        pst_Data->ac_Prob[0] = (unsigned char) (256 * (((float) (i - pst_Data->ai_Stat[0]) / (5 * i) ) ));
        pst_Data->ac_Prob[1] = pst_Data->ac_Prob[0] + (unsigned char) (256 * (((float) (i - pst_Data->ai_Stat[1]) / (5 * i) ) ));
        pst_Data->ac_Prob[2] = pst_Data->ac_Prob[1] + (unsigned char) (256 * (((float) (i - pst_Data->ai_Stat[2]) / (5 * i) ) ));
        pst_Data->ac_Prob[3] = pst_Data->ac_Prob[2] + (unsigned char) (256 * (((float) (i - pst_Data->ai_Stat[3]) / (5 * i) ) ));
        pst_Data->ac_Prob[4] = pst_Data->ac_Prob[3] + (unsigned char) (256 * (((float) (i - pst_Data->ai_Stat[4]) / (5 * i) ) ));
        pst_Data->ac_Prob[5] = pst_Data->ac_Prob[4] + (unsigned char) (256 * (((float) (i - pst_Data->ai_Stat[5]) / (5 * i)) ));
        i = rand() & 0xFF;
        j = 0;
        while ( (j != 5) && (pst_Data->ac_Prob[j] < i) ) j++;
        pst_Data->ai_Stat[ j ]++;
        pst_Data->c_CurPiece = j;
        pst_Data->i_NbSquare += 4;
        pst_Data->c_CurLine = TETRIS_LINE - 1;
        pst_Data->c_CurCol = (TETRIS_COL - 4) / 2;
        pst_Data->f_Time = 0;
        pst_Data->c_CurTurn = 0;
    }
    else
    {
        if ( !(pst_Data->ul_Request & 0x8000000) && (pst_Data->ul_Request & 0x10) )
            pst_Data->f_Time += (pst_Data->f_Speed / 0.05f) * TIM_gf_dt;
        else
            pst_Data->f_Time += TIM_gf_dt;
        if (pst_Data->f_Time > pst_Data->f_Speed)
        {
            pst_Data->c_CurLine--;
            pst_Data->f_Time -= pst_Data->f_Speed;
        }
    }
    f_LineRatio = pst_Data->f_Time / pst_Data->f_Speed;

    if (pst_Data->ul_Request)
    {
        if (pst_Data->ul_Request & 0x8000000)
        {
        }
        else
        {
            newturn = pst_Data->c_CurTurn;
            newcol = pst_Data->c_CurCol;
            if (pst_Data->ul_Request & 1)
                newturn = (pst_Data->c_CurTurn + 1) & 0x3;
            if (pst_Data->ul_Request & 2)
                newturn = (pst_Data->c_CurTurn + 3) & 0x3;
            if (pst_Data->ul_Request & 4)
                newcol++;
            if (pst_Data->ul_Request & 8)
                newcol--;

            /* blocage */
            uw_Data = aauw_Data[ pst_Data->c_CurPiece ][ newturn ];
            i = (uw_Data & 0x000F) ? 4 : ( uw_Data & 0x00F0) ? 3 : 2;
            block = 0;
            if (pst_Data->c_CurLine <= i)
                block = 1;
            else
            {
                uw_Test = 0x8888;
                for (i = 0; i < 4; i++)
                {
                    uw_Test2 = (uw_Data & uw_Test);
                    if (uw_Test2)
                    {
                        j = (uw_Test2 & 0x000F) ? 5 : (uw_Test2 & 0x00F0) ? 4 : 3;
                        if (pst_Data->ac_Grid[ pst_Data->c_CurLine - j ][ newcol + i ] != -1)
                        {
                            block = 1;
                            break;
                        }
                    }
                    uw_Test >>= 1;
                }
            }

            if (!block)
            {
                pst_Data->c_CurTurn = newturn;
                pst_Data->c_CurCol = newcol;
                uw_Data = aauw_Data[ pst_Data->c_CurPiece ][ pst_Data->c_CurTurn ];
                i = (!(uw_Data & 0x1111)) ? ( (!(uw_Data & 0x2222)) ? 2 : 3 ) : 4;
                if (pst_Data->c_CurCol > TETRIS_COL - i) pst_Data->c_CurCol = TETRIS_COL - i;
                i = (!(uw_Data & 0x8888)) ? ( (!(uw_Data & 0x4444)) ? -2: -1 ) : 0;
                if (pst_Data->c_CurCol < i) pst_Data->c_CurCol = i;
            }
        }
        pst_Data->ul_Request = 0;
    }



    /* blocage */
    uw_Data = aauw_Data[ pst_Data->c_CurPiece ][ pst_Data->c_CurTurn ];
    i = (uw_Data & 0x000F) ? 4 : ( uw_Data & 0x00F0) ? 3 : 2;
    block = 0;
    if (pst_Data->c_CurLine <= i)
    {
        block = 1;
    }
    else
    {
        uw_Test = 0x8888;
        for (i = 0; i < 4; i++)
        {
            uw_Test2 = (uw_Data & uw_Test);
            if (uw_Test2)
            {
                j = (uw_Test2 & 0x000F) ? 5 : (uw_Test2 & 0x00F0) ? 4 : 3;
                if (pst_Data->ac_Grid[ pst_Data->c_CurLine - j ][ pst_Data->c_CurCol + i ] != -1)
                {
                    block = 1;
                    break;

                }
            }
            uw_Test >>= 1;
        }
    }

    if (block)
    {
        if (f_LineRatio < 0.4f)
            f_LineRatio = 0.0f;
        else
        {
            for ( i = 1; i < 5; i++ )
            { 
                for (j = 0; j < 4; j++)
                {
                    if (uw_Data & 0x8000)
                        pst_Data->ac_Grid[ pst_Data->c_CurLine - i][ pst_Data->c_CurCol + j] = pst_Data->c_CurPiece;
                    uw_Data <<= 1;
                }
            }
            
            ul_OldLine = pst_Data->ul_Line[ 0 ];
            //uw_Data = aauw_Data[ pst_Data->c_CurPiece ][ pst_Data->c_CurTurn ];
            for (i = pst_Data->c_CurLine - 1; (i >= 0) && (i >= pst_Data->c_CurLine - 5); i--)
            {
                //if (uw_Data & 0xF)
                {
                    for (j = 0; j < TETRIS_COL; j++)
                        if (pst_Data->ac_Grid[ i ][ j ] == -1) break;
                    if (j == TETRIS_COL)
                    {
                        L_memmove( pst_Data->ac_Grid[ i ], pst_Data->ac_Grid[i + 1], TETRIS_COL * (TETRIS_LINE - i - 1) );
                        L_memset( pst_Data->ac_Grid[ TETRIS_LINE - 1 ], 0xFF, TETRIS_COL );
                        pst_Data->i_NbSquare -= TETRIS_COL;
                        pst_Data->ul_Line[ 0 ]++;
                    }
                }
                //uw_Data >>= 4;
            }
            if (pst_Data->ul_Line[ 0 ] != ul_OldLine )
                pst_Data->ul_Line[ pst_Data->ul_Line[0] - ul_OldLine ]++;

            pst_Data->c_CurPiece = -1;
        }
    }


    i_NbPoints = pst_Data->i_NbSquare + 1;

	GFX_NeedGeom(4 * i_NbPoints, 4, 2 * i_NbPoints, 1);
    V = GFX_gpst_Geo->dst_Point;
    pul_RLI = GFX_gpst_Geo->dul_PointColors + 1;

    MATH_CopyVector( V, &pst_Data->st_Pos );
    MATH_CrossProduct( &N, &pst_Data->X, &pst_Data->Z);
    MATH_AddScaleVector( V, V ,&N, -0.1f);
    MATH_AddScaleVector( V+1, V, &X, TETRIS_COL );
    MATH_AddScaleVector( V+2, V + 1, &Z, TETRIS_LINE );
    MATH_AddScaleVector( V+3, V, &Z, TETRIS_LINE );
    pul_RLI[0] = pul_RLI[1] = pul_RLI[2] = pul_RLI[3] = 0xFF4F4F4F;
    V += 4;
    pul_RLI += 4;

    if (pst_Data->c_CurPiece != -1)
    {
        uw_Data = aauw_Data[ pst_Data->c_CurPiece ][ pst_Data->c_CurTurn ];
        y = (float) pst_Data->c_CurLine - f_LineRatio;
        x = pst_Data->c_CurCol;

        for ( i = 0; i < 4; i++ )
        { 
            y -= 1;
            for (j = 0; j < 4; j++)
            {
                if (uw_Data & 0x8000)
                {
                    MATH_ScaleVector( V, &Z, y );
                    MATH_AddScaleVector( V, V, &X, x );
                    MATH_AddEqualVector( V, &pst_Data->st_Pos );
                    M_Tetris_1to4Points( ul_Color[ pst_Data->c_CurPiece ] );    
                }
                uw_Data <<= 1;
                x += 1;
            }
            x -= 4;
        }
    }

    for (i = 0; i < TETRIS_LINE; i++)
    {
        for (j = 0; j < TETRIS_COL; j++)
        {
            if (pst_Data->ac_Grid[i][j] != -1 )
            {
                MATH_ScaleVector( V, &Z, (float) i );
                MATH_AddScaleVector( V, V, &X, (float )j );
                MATH_AddEqualVector( V, &pst_Data->st_Pos );
                M_Tetris_1to4Points( ul_Color[ pst_Data->ac_Grid[i][j] ] );
            }
        }
    }

    T = GFX_gpst_Geo->dst_Element[0].dst_Triangle;
    TLast = T + (2 * i_NbPoints);
    i = 0;
    while ( T < TLast )
    {
        T->auw_Index[0] = i + 0;
	    T->auw_Index[1] = i + 1;
	    T->auw_Index[2] = i + 2;
        T->auw_UV[0] = 0;
        T->auw_UV[1] = 1;
        T->auw_UV[2] = 2;
        T++;

	    T->auw_Index[0] = i + 2;
	    T->auw_Index[1] = i + 3;
	    T->auw_Index[2] = i + 0;
        T->auw_UV[0] = 2;
        T->auw_UV[1] = 3;
        T->auw_UV[2] = 0;
        T++;

        i += 4;
    }

    GFX_gpst_Geo->dst_UV[0].fU = 0;
    GFX_gpst_Geo->dst_UV[0].fV = 0;
    GFX_gpst_Geo->dst_UV[1].fU = 0;
    GFX_gpst_Geo->dst_UV[1].fV = 1;
    GFX_gpst_Geo->dst_UV[2].fU = 1;
    GFX_gpst_Geo->dst_UV[2].fV = 1;
    GFX_gpst_Geo->dst_UV[3].fU = 1;
    GFX_gpst_Geo->dst_UV[3].fV = 0;


    M_GFX_CheckGeom();

	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_Lighted | GDI_Cul_DM_UseAmbient);
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
#endif
}


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
