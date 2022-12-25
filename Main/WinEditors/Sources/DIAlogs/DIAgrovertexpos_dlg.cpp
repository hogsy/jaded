/*$T DIAgrovertexpos_dlg.cpp GC! 1.081 08/23/01 15:35:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAgrovertexpos_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDIPaths.h"
#include "LINks/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "F3Dframe/F3Dview.h"

#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOsubobject.h" 

/* Message map */
BEGIN_MESSAGE_MAP(EDIA_cl_GroVertexPos, EDIA_cl_BaseDialog)
    ON_BN_CLICKED( IDC_CHECK_GLOBAL, OnButtonGlobal )
    ON_BN_CLICKED( IDC_CHECK_LOCAL, OnButtonLocal )
    ON_BN_CLICKED( IDC_CHECK_POSSHIFT, OnButtonPosOrShift )
    ON_BN_CLICKED( IDC_BUTTON_DOOP, OnButtonDoOperation )
	ON_BN_CLICKED( IDC_BUTTON_REORDER, OnButtonReorder )
	ON_EN_KILLFOCUS(IDC_EDITX, OnVertexChange)
    ON_EN_KILLFOCUS(IDC_EDITY, OnVertexChange)
    ON_EN_KILLFOCUS(IDC_EDITZ, OnVertexChange)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GroVertexPos::EDIA_cl_GroVertexPos(F3D_cl_View *_pst_View) : EDIA_cl_BaseDialog(DIALOGS_IDD_GROVERTEXPOS), mpst_View(_pst_View)
{
    Create();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GroVertexPos::~EDIA_cl_GroVertexPos()
{
    Destroy();

	if (mpst_View)
        mpst_View->mpo_VertexPos = NULL;

    mpst_View = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::Create()
{
    mpst_Data = NULL;
    mb_Global = 0;
    mb_Move = 0;
    mi_NbData = 0;
    i_Coord = 7;
    MATH_InitVectorToZero( &mst_Pivot );
    MATH_InitVectorToZero( &mst_Move );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::Destroy()
{
    int i;

    if (mpst_Data)
    {
        for (i = 0; i < mi_NbData; i++)
        {
            if (mpst_Data[i].pp_Vertex)
                L_free( mpst_Data[i].pp_Vertex );
        }
        L_free( mpst_Data );
        mpst_Data = NULL;
        mi_NbData = 0;
    }

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_GroVertexPos::OnInitDialog(void)
{
    char sz_Text[ 256 ];

    ShowVertex( NULL );

	mb_OneObject = FALSE;
	mb_OneVertex = FALSE;

    if (mi_NbData == 0)
    {
        GetDlgItem( IDC_EDIT_GAO )->SetWindowText( "select vertex first" );
    }
    else if (mi_NbData == 1)
    {
        ((CButton *) GetDlgItem( IDC_CHECK_GLOBAL ))->SetCheck( mb_Global ? TRUE : FALSE);
        ((CButton *) GetDlgItem( IDC_CHECK_LOCAL  ))->SetCheck( mb_Global ? FALSE : TRUE );
     
        mb_OneObject = TRUE;
        GetDlgItem( IDC_EDIT_GAO )->SetWindowText( msz_GaoName );
        sprintf( sz_Text, "%s (%d)", msz_GroName, mi_VertexIndex );
        GetDlgItem( IDC_EDIT_GRO )->SetWindowText( sz_Text );

        if (mpst_Data->nbV == 1)
        {
            mb_OneVertex = TRUE;
            if (mb_Global)
                MATH_TransformVertex( &mst_Vertex, mpst_Data->M, mpst_Data->pp_Vertex[0]);
            else
                MATH_CopyVector( &mst_Vertex, mpst_Data->pp_Vertex[0] );
            ShowVertex( &mst_Vertex );
        }
    }
    else
    {
        ((CButton *) GetDlgItem( IDC_CHECK_GLOBAL ))->SetCheck( TRUE );
        GetDlgItem( IDC_CHECK_GLOBAL )->EnableWindow( FALSE );
        GetDlgItem( IDC_CHECK_LOCAL )->ShowWindow( SW_HIDE );
        mb_Global = 1;
        GetDlgItem( IDC_EDIT_GAO )->SetWindowText( "Multi selection" );
    }

    if ( mb_Move )
    {
        GetDlgItem( IDC_CHECK_POSSHIFT )->SetWindowText( "move" );
        ((CButton * )GetDlgItem( IDC_CHECK_POSSHIFT ))->SetCheck( TRUE ); 
        ShowVertex( &mst_Move );
    }

	GetDlgItem( IDC_BUTTON_REORDER )->EnableWindow( mb_OneVertex );
	GetDlgItem( IDC_EDIT_ORDER )->EnableWindow( mb_OneVertex );

    GetDlgItem( IDC_EDITX )->SetFocus();
    ((CEdit *) GetDlgItem( IDC_EDITX ))->SetSel( 0, -1);

    sprintf( sz_Text, "%.4f", mst_Pivot.x );
    GetDlgItem( IDC_EDIT_PIVOTX )->SetWindowText( sz_Text );
    sprintf( sz_Text, "%.4f", mst_Pivot.y );
    GetDlgItem( IDC_EDIT_PIVOTY )->SetWindowText( sz_Text );
    sprintf( sz_Text, "%.4f", mst_Pivot.z );
    GetDlgItem( IDC_EDIT_PIVOTZ )->SetWindowText( sz_Text );

    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GroVertexPos::PreTranslateMessage( MSG *msg )
{
    if (msg->message == WM_KEYDOWN)
    {
        if ( ( msg->wParam == VK_RETURN ) || (msg->wParam == VK_TAB ) )
        {
            if (msg->hwnd == GetDlgItem( IDC_EDITX )->GetSafeHwnd() )
            {
                GetDlgItem( IDC_EDITY )->SetFocus();
                ((CEdit *) GetDlgItem( IDC_EDITY ))->SetSel( 0, -1);
                return 1;
            }
            else if (msg->hwnd == GetDlgItem( IDC_EDITY )->GetSafeHwnd() )
            {
                GetDlgItem( IDC_EDITZ )->SetFocus();
                ((CEdit *) GetDlgItem( IDC_EDITZ ))->SetSel( 0, -1);
                return 1;
            }
            else if (msg->hwnd == GetDlgItem( IDC_EDITZ )->GetSafeHwnd() )
            {
                GetDlgItem( IDOK )->SetFocus();
                return 1;
            }   
        }
        if ( msg->wParam == VK_ESCAPE )
        {
            OnCancel();
            return 1;
        }
    }
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::AddData( OBJ_tdst_GameObject *_pst_Gao, GEO_tdst_Object *_pst_Gro )
{
    EDIA_VertexPos_Data *pst_Data;
    int                 i, j;

    if( (!_pst_Gro) || (!_pst_Gao) ) return;
	if(_pst_Gro->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Gro->pst_SubObject) return;

    i = GEO_l_SubObject_GetNbPickedVertices( _pst_Gro );
    if ( i == 0 ) return;

    j = (mi_NbData + 1) * sizeof( EDIA_VertexPos_Data );
    if (mpst_Data)
        mpst_Data = (EDIA_VertexPos_Data *) L_realloc( mpst_Data, j );
    else
        mpst_Data = (EDIA_VertexPos_Data *) L_malloc( j );

    pst_Data = mpst_Data + mi_NbData++;

    pst_Data->pst_Obj = _pst_Gro;
    pst_Data->M = OBJ_pst_GetAbsoluteMatrix( _pst_Gao );
    MATH_SetIdentityMatrix( &pst_Data->IM );
    MATH_InvertMatrix( &pst_Data->IM, pst_Data->M );

    pst_Data->nbV = i;
    pst_Data->pp_Vertex = (GEO_Vertex **) L_malloc( i * sizeof( GEO_Vertex *) );

    for ( i = 0, j = 0; i< _pst_Gro->l_NbPoints; i++)
    {
        if (_pst_Gro->pst_SubObject->dc_VSel[ i ] & 1)
        {
            pst_Data->pp_Vertex[ j++ ] = _pst_Gro->dst_Point + i;
            mi_VertexIndex = i;
        }
    }

    if (mi_NbData == 1)
    {
        msz_GroName = _pst_Gro->st_Id.sz_Name;
        msz_GaoName = _pst_Gao->sz_Name;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::GetVertex( )
{
    char sz_Value[ 16 ];
    char *sz_Cur;

    GetDlgItem( IDC_EDITX )->GetWindowText( sz_Value, 16 );
    mst_Vertex.x = (float ) atof( sz_Value );
    sz_Cur = sz_Value;
    while (*sz_Cur)
    {
        if (isdigit( *sz_Cur)) break;
        sz_Cur++;
    }
    i_Coord = *sz_Cur? 1 : 0;

    GetDlgItem( IDC_EDITY )->GetWindowText( sz_Value, 16 );
    mst_Vertex.y = (float ) atof( sz_Value );
    sz_Cur = sz_Value;
    while (*sz_Cur)
    {
        if (isdigit( *sz_Cur)) break;
        sz_Cur++;
    }
    i_Coord |= *sz_Cur? 2 : 0;

    GetDlgItem( IDC_EDITZ )->GetWindowText( sz_Value, 16 );
    mst_Vertex.z = (float ) atof( sz_Value );
    sz_Cur = sz_Value;
    while (*sz_Cur)
    {
        if (isdigit( *sz_Cur)) break;
        sz_Cur++;
    }
    i_Coord |= *sz_Value ? 4 : 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::ShowVertex( MATH_tdst_Vector *V )
{
    char sz_Value[ 16 ];

    GetDlgItem( IDC_EDITX )->SetWindowText( "" );
    GetDlgItem( IDC_EDITY )->SetWindowText( "" );
    GetDlgItem( IDC_EDITZ )->SetWindowText( "" );
    
    if (V)
    {
        if (i_Coord & 1)
        {
            sprintf( sz_Value, "%.4f", V->x );
            GetDlgItem( IDC_EDITX )->SetWindowText( sz_Value );
        }
        if (i_Coord & 2)
        {
            sprintf( sz_Value, "%.4f", V->y );
            GetDlgItem( IDC_EDITY )->SetWindowText( sz_Value );
        }
        if (i_Coord & 3)
        {
            sprintf( sz_Value, "%.4f", V->z );
            GetDlgItem( IDC_EDITZ )->SetWindowText( sz_Value );
        }
    }
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::OnButtonGlobal( void )
{
    if (mb_Global) return;

    mb_Global = TRUE;
    ((CButton *) GetDlgItem( IDC_CHECK_GLOBAL ))->SetCheck( 1 );
    ((CButton *) GetDlgItem( IDC_CHECK_LOCAL  ))->SetCheck( 0 );

    if (mb_OneVertex)
    {
        GetVertex();
        MATH_TransformVertex( &mst_Vertex, mpst_Data->M, &mst_Vertex);
    }

    MATH_TransformVector( &mst_Move, mpst_Data->M, &mst_Move );

    if (mb_Move)
        ShowVertex( &mst_Move );
    else
    {
        if (mb_OneVertex)
            ShowVertex( &mst_Vertex );
        else
            ShowVertex( NULL );
    }

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::OnButtonLocal( void )
{
    if (!mb_Global) return;

    mb_Global = FALSE;
    ((CButton *) GetDlgItem( IDC_CHECK_GLOBAL ))->SetCheck( 0 );
    ((CButton *) GetDlgItem( IDC_CHECK_LOCAL  ))->SetCheck( 1 );

    if (mb_OneVertex)
    {
        GetVertex();
        MATH_TransformVertex( &mst_Vertex, &mpst_Data->IM, &mst_Vertex);
    }
    MATH_TransformVector( &mst_Move, &mpst_Data->IM, &mst_Move );

    if (mb_Move)
        ShowVertex( &mst_Move );
    else
    {
        if (mb_OneVertex)
            ShowVertex( &mst_Vertex );
        else
            ShowVertex( NULL );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::OnButtonPosOrShift( void )
{
    if ( ((CButton *) GetDlgItem( IDC_CHECK_POSSHIFT ))->GetCheck() )
    {
        mb_Move = TRUE;
        GetDlgItem( IDC_CHECK_POSSHIFT )->SetWindowText( "move" );
        ShowVertex( &mst_Move );
    }
    else
    {
        mb_Move = FALSE;
        GetDlgItem( IDC_CHECK_POSSHIFT )->SetWindowText( "position" );
        ShowVertex( &mst_Vertex );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::OnButtonDoOperation(void)
{
	char	*sz_OpStr[5] = { "+", "-", "*", "/", "=" };
	int		i_State = 0;			/* State = 0 : lecture left operand waiting operator, State = 1 : reading a number after operator  */
	int		i_NumberStart = 0;
	char	sz_Op[ 100 ], *sz_Cur;
	int		i_Operator = -1;		/* -1 : invalide,  0 +, 1 - , 2 *, 3 /, 4 = */
	float	f_OperandR = 0;			/* right operand : a number */
	float	f_Decimal;
	int		i_OperandL = 8;			/* flags x = 1, y = 2, z = 4, 8 : all (special)*/

	MATH_tdst_Vector Temp, **V, **LV;
    int i;

	
	GetDlgItem( IDC_EDIT_OP )->GetWindowText( sz_Op, 99 );

	sz_Cur = sz_Op;
	while ( *sz_Cur != 0 )
	{
		if (i_State == 0)
		{
			switch (*sz_Cur)
			{
			case 'x': (i_OperandL == 8) ? (i_OperandL = 1 ) : i_OperandL |= 1; break;
			case 'y': (i_OperandL == 8) ? (i_OperandL = 2 ) : i_OperandL |= 2; break;
			case 'z': (i_OperandL == 8) ? (i_OperandL = 4 ) : i_OperandL |= 4; break;
			case '+': i_Operator = 0; break;
			case '-': i_Operator = 1; break;
			case '*': i_Operator = 2; break;
			case '/': i_Operator = 3; break;
			case '=': i_Operator = 4; break;
			}
			if (i_Operator != -1) i_State = 1;
		}
		else if ( i_State == 1 )
		{
			if (isdigit( *sz_Cur ) )
			{
				i_NumberStart = 1;
				f_OperandR = (f_OperandR * 10.0f) + ( (float) (*sz_Cur - '0') );
			}
			else if (*sz_Cur == '.') 
			{
				f_Decimal = 0.1f;
				i_State = 2;
			}
			else
			{
				if (i_NumberStart) break;
			}
		}
		else /* state = 2 */
		{
			if (isdigit( *sz_Cur ))
			{
				f_OperandR += ( (float) (*sz_Cur - '0') ) * f_Decimal;
				f_Decimal *= 0.1f;
			}
			else break;
		}
		*sz_Cur++;
	}

	if (i_Operator == -1) return;
	if (i_OperandL == 8) i_OperandL = 7;

	sprintf( sz_Op, "Do Operation : %s%s%s %s %f", 
		( i_OperandL & 1 ) ? "x" : "", 
		( i_OperandL & 2 ) ? "y" : "",
		( i_OperandL & 4 ) ? "z" : "",
		sz_OpStr[ i_Operator ], 
		f_OperandR
	);


	for (i = 0; i < mi_NbData; i++)
    {
        V = mpst_Data[i].pp_Vertex;
        LV = V + mpst_Data[i].nbV;

		if ( (i_Operator == 0) || (i_Operator == 1) )
		{
			Temp.x = ( i_OperandL & 1 ) ? f_OperandR : 0.0f;
			Temp.y = ( i_OperandL & 2 ) ? f_OperandR : 0.0f;
			Temp.z = ( i_OperandL & 4 ) ? f_OperandR : 0.0f;
			if (i_Operator == 1) MATH_NegEqualVector( &Temp );
			for (; V < LV; V++)
				MATH_AddEqualVector( (*V), &Temp );
		}
		else if ((i_Operator == 2) || (i_Operator == 3))
		{
			Temp.x = ( i_OperandL & 1 ) ? f_OperandR : 1.0f;
			Temp.y = ( i_OperandL & 2 ) ? f_OperandR : 1.0f;
			Temp.z = ( i_OperandL & 4 ) ? f_OperandR : 1.0f;
			if (i_Operator == 3) MATH_InvEqualVector( &Temp );
			for (; V < LV; V++)
				MATH_MulEqualTwoVectors( (*V), &Temp );
		}
		else if (i_Operator == 4)
		{
			for (; V < LV; V++)
			{
				if ( i_OperandL & 1 ) (*V)->x = f_OperandR;
				if ( i_OperandL & 2 ) (*V)->y = f_OperandR;
				if ( i_OperandL & 4 ) (*V)->z = f_OperandR;
			}
		}

        GEO_SubObject_UseVMoves( mpst_Data[i].pst_Obj );
        mpst_Data[i].pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
        GEO_SubObject_BuildFaceData(mpst_Data[i].pst_Obj);
	    GEO_SubObject_BuildEdgeData(mpst_Data[i].pst_Obj);
		LINK_Refresh();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::OnButtonReorder(void)
{
	int									i, j, elem, newvertex, oldvertex;
	char								sz_Text[16];
	GEO_Vertex							st_Point;
	MATH_tdst_Vector					st_Normal;
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*pst_Tri;


	if ( !mb_OneObject || !mb_OneVertex) return;

	oldvertex = mi_VertexIndex;
	GetDlgItem( IDC_EDIT_ORDER )->GetWindowText( sz_Text, 15 );
	newvertex = atoi( sz_Text );

	if (newvertex < 0) return;
	if (newvertex >= mpst_Data[0].pst_Obj->l_NbPoints ) return;
	if (newvertex == oldvertex) return;

	/* swap points */
	L_memcpy( &st_Point, mpst_Data[ 0 ].pst_Obj->dst_Point + oldvertex, sizeof (GEO_Vertex ) );
	L_memcpy( mpst_Data[ 0 ].pst_Obj->dst_Point + oldvertex, mpst_Data[ 0 ].pst_Obj->dst_Point + newvertex, sizeof (GEO_Vertex ) );
	L_memcpy( mpst_Data[ 0 ].pst_Obj->dst_Point + newvertex, &st_Point, sizeof (GEO_Vertex ) );

	/* swap normals */
	L_memcpy( &st_Normal, mpst_Data[ 0 ].pst_Obj->dst_PointNormal + oldvertex, sizeof (MATH_tdst_Vector ) );
	L_memcpy( mpst_Data[ 0 ].pst_Obj->dst_PointNormal + oldvertex, mpst_Data[ 0 ].pst_Obj->dst_PointNormal + newvertex, sizeof (MATH_tdst_Vector ) );
	L_memcpy( mpst_Data[ 0 ].pst_Obj->dst_PointNormal + newvertex, &st_Normal, sizeof (MATH_tdst_Vector ) );

	/* update triangles */
	pst_Elem = mpst_Data[ 0 ].pst_Obj->dst_Element;
	for (elem = 0; elem < mpst_Data[ 0 ].pst_Obj->l_NbElements; elem++, pst_Elem++ )
	{
		pst_Tri = pst_Elem->dst_Triangle;
		for (i = 0; i < pst_Elem->l_NbTriangles; i++, pst_Tri++ )
		{
			for (j = 0; j < 3; j++ )
			{
				if (pst_Tri->auw_Index[ j ] == oldvertex ) 
					pst_Tri->auw_Index[ j ] = newvertex;
				else if (pst_Tri->auw_Index[ j ] == newvertex ) 
					pst_Tri->auw_Index[ j ] = oldvertex;
			}
		}
	}
	LINK_Refresh();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroVertexPos::OnVertexChange(void)
{
    MATH_tdst_Vector Temp, **V, **LV;
    int i;

    GetVertex();

    if (!i_Coord) return;

    if ( mb_Move )
    {
        MATH_SubVector( &Temp, &mst_Vertex, &mst_Move );
        MATH_CopyVector( &mst_Move, &mst_Vertex );
        MATH_CopyVector( &mst_Vertex, &Temp );
        
        if (mb_Global)
        {
            for (i = 0; i < mi_NbData; i++)
            {
                V = mpst_Data[i].pp_Vertex;
                LV = V + mpst_Data[i].nbV;
                for (; V < LV; V++)
                {
                    MATH_TransformVertex( &Temp, mpst_Data[i].M, (*V ) );
                    MATH_AddEqualVector( &Temp, &mst_Vertex );
                    MATH_TransformVertex( (*V ), &mpst_Data[i].IM, &Temp);
                }
                GEO_SubObject_UseVMoves( mpst_Data[i].pst_Obj );
                mpst_Data[i].pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
                GEO_SubObject_BuildFaceData(mpst_Data[i].pst_Obj);
	            GEO_SubObject_BuildEdgeData(mpst_Data[i].pst_Obj);
            }
        }
        else
        {
            for (i = 0; i < mi_NbData; i++)
            {
                V = mpst_Data[i].pp_Vertex;
                LV = V + mpst_Data[i].nbV;
                for (; V < LV; V++)
                    MATH_AddEqualVector( (*V), &mst_Vertex );
                GEO_SubObject_UseVMoves( mpst_Data[i].pst_Obj );
                mpst_Data[i].pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
                GEO_SubObject_BuildFaceData(mpst_Data[i].pst_Obj);
	            GEO_SubObject_BuildEdgeData(mpst_Data[i].pst_Obj);
            }
        }
    }
    else
    {
        if (mb_Global)
        {
            for (i = 0; i < mi_NbData; i++)
            {
                V = mpst_Data[i].pp_Vertex;
                LV = V + mpst_Data[i].nbV;
                for (; V < LV; V++)
                {
                    MATH_TransformVertex( &Temp, mpst_Data[i].M, (*V ) );
                    if (i_Coord & 1)    Temp.x = mst_Vertex.x;
                    if (i_Coord & 2)    Temp.y = mst_Vertex.y;
                    if (i_Coord & 4)    Temp.z = mst_Vertex.z;
                    MATH_TransformVertex( (*V ), &mpst_Data[i].IM, &Temp);
                }
                GEO_SubObject_UseVMoves( mpst_Data[i].pst_Obj );
                mpst_Data[i].pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
                GEO_SubObject_BuildFaceData(mpst_Data[i].pst_Obj);
	            GEO_SubObject_BuildEdgeData(mpst_Data[i].pst_Obj);
            }
        }
        else
        {
            for (i = 0; i < mi_NbData; i++)
            {
                V = mpst_Data[i].pp_Vertex;
                LV = V + mpst_Data[i].nbV;
                for (; V < LV; V++)
                {
                    if (i_Coord & 1)    (*V)->x = mst_Vertex.x;
                    if (i_Coord & 2)    (*V)->y = mst_Vertex.y;
                    if (i_Coord & 4)    (*V)->z = mst_Vertex.z;
                }
                GEO_SubObject_UseVMoves( mpst_Data[i].pst_Obj );
                mpst_Data[i].pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
                GEO_SubObject_BuildFaceData(mpst_Data[i].pst_Obj);
	            GEO_SubObject_BuildEdgeData(mpst_Data[i].pst_Obj);
            }
        }
    }

    LINK_Refresh();
}



#endif /* ACTIVE_EDITORS */
