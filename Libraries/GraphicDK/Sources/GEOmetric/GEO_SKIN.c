/*$T GEO_SKIN.c GC!1.55 02/28/00 18:25:15 */

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include "Precomp.h"

//#if !defined(_XENON)
//#if defined(_XBOX) || defined(_XENON)
#define USE_SSE 0

#if USE_SSE
#	include "xmmintrin.h"

#	define OPT_COMPUTENORMALS  1
#	define OPT_COMPUTE4DISPLAY 1

static bool bFirstTime = true;
static __m128 _MINUSONE_;
static __m128 _ONE_;
__m128 _MASK_0FFF_;//only X Y Z
__m128 _MASK_F000_;//Only W
static __m128 _ZEROS_;
static __m64 _ZEROS64_;
const struct
{
	int i[ 4 ];
	float f[ 4 ];
	float z[ 4 ];
	int w[ 4 ];
	float one[ 4 ];
} _Mask = {
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0x00000000,
        -1.0f,
        -1.0f,
        -1.0f,
        -1.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0x00000000,
        0x00000000,
        0x00000000,
        0xffffffff,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
};

#endif//USE_SSE

#ifdef PSX2_TARGET
#	include "PSX2debug.h"
#else// PSX2_TARGET
#	ifdef _GAMECUBE
#	else// _GAMECUBE
#		ifdef ACTIVE_EDITORS
#			include "MAD_loadSave/Sources/MAD_Struct_V0.h"
#		endif// ACTIVE_EDITORS
#	endif    // _GAMECUBE
#endif        // PSX2_TARGET

#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine\Sources\OBJects\OBJgizmo.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORupdate.h"

#include "GDInterface/GDIrasters.h"
#include "SELection/SELection.h"
#include "GRObject/GROedit.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEO_SKIN.h"
#include "GEOmetric/GEO_STRIP.h"
#include "SOFT/SOFTPickingBuffer.h"

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"

#ifdef _GAMECUBE
#	include "GEOmetric/GEO_STRIP.h"
#	ifndef _FINAL_
#		include "GXI_GC/GXI_dbg.h"
#	endif// _FINAL_
#endif    // _GAMECUBE

#ifdef _XENON_RENDER
#	include "XenonGraphics/XeVertexShaderManager.h"
#	include "XenonGraphics/XeContextManager.h"
#endif

#if defined( PSX2_TARGET ) && defined( __cplusplus )
extern "C"
{
#endif

	extern ULONG LOA_ul_FileTypeSize[ 40 ];

	void GEO_SKN_SymetriseMatrix( MATH_tdst_Matrix *pst_Matrix )
	{
		*( ( ULONG * ) &pst_Matrix->Ix ) ^= 0x80000000;
		*( ( ULONG * ) &pst_Matrix->Jx ) ^= 0x80000000;
		*( ( ULONG * ) &pst_Matrix->Kx ) ^= 0x80000000;
		*( ( ULONG * ) &pst_Matrix->T.x ) ^= 0x80000000;
	}
	void GEO_SKN_180Matrix( MATH_tdst_Matrix *pst_Matrix )
	{
		MATH_tdst_Matrix st_Matrix ONLY_PSX2_ALIGNED( 16 );
		MATH_InvertMatrix( &st_Matrix, pst_Matrix );
		*( ( ULONG * ) &st_Matrix.Ix ) ^= 0x80000000;
		*( ( ULONG * ) &st_Matrix.Iy ) ^= 0x80000000;
		*( ( ULONG * ) &st_Matrix.Iz ) ^= 0x80000000;
		*( ( ULONG * ) &st_Matrix.Jx ) ^= 0x80000000;
		*( ( ULONG * ) &st_Matrix.Jy ) ^= 0x80000000;
		*( ( ULONG * ) &st_Matrix.Jz ) ^= 0x80000000;
		MATH_InvertMatrix( pst_Matrix, &st_Matrix );
	}
	void GEO_SKN_MulMatrixMatrixSym( MATH_tdst_Matrix *pst_Dest, MATH_tdst_Matrix *pst_Flashed, MATH_tdst_Matrix *pst_Matrix )
	{
		GEO_SKN_SymetriseMatrix( pst_Flashed );
		MATH_MulMatrixMatrix( pst_Dest, pst_Flashed, pst_Matrix );
		GEO_SKN_SymetriseMatrix( pst_Flashed );
	}

#ifdef ACTIVE_EDITORS
	GEO_tdst_VertexPonderationList *GEO_SKN_CreateList( ULONG NumbrOfPoints, ULONG MatrixNumber );
	ULONG GEO_SKN_IsExpanded( GEO_tdst_Object *pst_Object );
	ULONG u4_Interpol2Color( ULONG ulP1, ULONG ulP2, float fZClipLocalCoef )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG RetValue, Interpoler;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( fZClipLocalCoef >= 0.98f ) return ulP2;
		if ( fZClipLocalCoef <= 0.02f ) return ulP1;
		*( ( float * ) &Interpoler ) = fZClipLocalCoef + 32768.0f + 16384.0f;
		RetValue                     = ( Interpoler & 128 ) ? ( ulP2 & 0xfefefefe ) >> 1 : ( ulP1 & 0xfefefefe ) >> 1;
		RetValue += ( Interpoler & 64 ) ? ( ulP2 & 0xfcfcfcfc ) >> 2 : ( ulP1 & 0xfcfcfcfc ) >> 2;
		RetValue += ( Interpoler & 32 ) ? ( ulP2 & 0xf8f8f8f8 ) >> 3 : ( ulP1 & 0xf8f8f8f8 ) >> 3;
		RetValue += ( Interpoler & 16 ) ? ( ulP2 & 0xf0f0f0f0 ) >> 4 : ( ulP1 & 0xf0f0f0f0 ) >> 4;
		RetValue += ( Interpoler & 8 ) ? ( ulP2 & 0xe0e0e0e0 ) >> 5 : ( ulP1 & 0xe0e0e0e0 ) >> 5;
		RetValue += ( Interpoler & 4 ) ? ( ulP2 & 0xc0c0c0c0 ) >> 6 : ( ulP1 & 0xc0c0c0c0 ) >> 6;
		RetValue += ( Interpoler & 2 ) ? ( ulP2 & 0x80808080 ) >> 7 : ( ulP1 & 0x80808080 ) >> 7;
		return RetValue;
	}

	/*$4
********************************************************************************************************************************************************************************
Mesh edges enumerator
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_ForEachEdgeDo(
	        GEO_tdst_Object *pst_Object,
	        void ( *GEO_SKN_CLBK )( GEO_tdst_Object *, GEO_tdst_ObjectPonderation *p_SRC, GEO_tdst_ObjectPonderation *p_Dst, ULONG I1, ULONG I2 ),
	        float fBlend,
	        ULONG ulFlag )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_ElementIndexedTriangles *dst_Element, *dst_ElementLast;
		GEO_tdst_IndexedTriangle *pst_Face, *pst_FaceLast;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( !GEO_SKN_IsExpanded( pst_Object ) )
			GEO_SKN_Expand( pst_Object );

		GEO_SKN_Push( pst_Object );
		GEO_SKN_CopySelBend( pst_Object, fBlend, 0 );
		dst_Element     = pst_Object->dst_Element;
		dst_ElementLast = dst_Element + pst_Object->l_NbElements;
		while ( dst_Element < dst_ElementLast )
		{
			pst_Face     = dst_Element->dst_Triangle;
			pst_FaceLast = pst_Face + dst_Element->l_NbTriangles;
			while ( pst_Face < pst_FaceLast )
			{
				GEO_SKN_CLBK( pst_Object, pst_Object->p_SKN_Objectponderation->PushStack, pst_Object->p_SKN_Objectponderation, pst_Face->auw_Index[ 0 ], pst_Face->auw_Index[ 1 ] );
				GEO_SKN_CLBK( pst_Object, pst_Object->p_SKN_Objectponderation->PushStack, pst_Object->p_SKN_Objectponderation, pst_Face->auw_Index[ 1 ], pst_Face->auw_Index[ 2 ] );
				GEO_SKN_CLBK( pst_Object, pst_Object->p_SKN_Objectponderation->PushStack, pst_Object->p_SKN_Objectponderation, pst_Face->auw_Index[ 2 ], pst_Face->auw_Index[ 0 ] );
				pst_Face++;
			}
			dst_Element++;
		}
		GEO_SKN_PopedSelBend( pst_Object, 0.0f, ulFlag );
	}

	/*$4
********************************************************************************************************************************************************************************
Vertex ponderation enumerator
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_ForEachPointDo(
	        GEO_tdst_Object *pst_Object,
	        void ( *GEO_SKN_CLBK )( GEO_tdst_Object *, ULONG Number, ULONG U1, float f2 ),
	        ULONG A,
	        float B,
	        ULONG ulFlag )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG Counter, MatrixCounter;
		float SavedPonderation[ GEO_SKN_MAXNUMBEROFMATRIX ];
		LONG l_Sel, l_NbSel;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( !GEO_SKN_IsExpanded( pst_Object ) )
			GEO_SKN_Expand( pst_Object );

		Counter = pst_Object->l_NbPoints;
		l_NbSel = 0;
		if ( ( ulFlag & SKN_Cul_UseSubSel ) && pst_Object->pst_SubObject && pst_Object->pst_SubObject->dc_VSel )
		{
			while ( Counter-- )
			{
				l_Sel = pst_Object->pst_SubObject->dc_VSel[ Counter ] & 1;
				if ( ulFlag & SKN_Cul_InvertSel ) l_Sel = 1 - l_Sel;
				l_NbSel += l_Sel;
				MatrixCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
				while ( MatrixCounter-- )
				{
					SavedPonderation[ MatrixCounter ] = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ]->p_PdrtVrc_E[ Counter ].f_Ponderation;
				}
				GEO_SKN_CLBK( pst_Object, Counter, A, B );
				MatrixCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
				while ( MatrixCounter-- )
				{
					pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ]->p_PdrtVrc_E[ Counter ].f_Ponderation = SavedPonderation[ MatrixCounter ] * ( 1 - l_Sel ) + pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ]->p_PdrtVrc_E[ Counter ].f_Ponderation * l_Sel;
				}
			}
		}
		if ( !( ( ulFlag & SKN_Cul_UseSubSel ) && ( ulFlag & SKN_Cul_InvertSel ) ) )
		{
			if ( l_NbSel == 0 )
			{
				Counter = pst_Object->l_NbPoints;
				while ( Counter-- )
				{
					GEO_SKN_CLBK( pst_Object, Counter, A, B );
				}
			}
		}
	}

	/*$4
********************************************************************************************************************************************************************************
Reset Ponderation
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_Reset_VP( GEO_tdst_Object *pst_Object, ULONG Index, ULONG r, float Value )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation = Value;
		}
	}

	void GEO_SKN_Reset( GEO_tdst_Object *pst_Object, float Value, ULONG ulFlags )
	{
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_Reset_VP, 0, Value, ulFlags );
	}


	/*$4
********************************************************************************************************************************************************************************
Normalize will make unitary ponderation fotr each vertice
********************************************************************************************************************************************************************************
*/

	/*$0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Per vertex
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

	void GEO_SKN_Normalize_Vertex( GEO_tdst_Object *pst_Object, ULONG Index, ULONG X, float Y )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		float fNorm;
		GEO_tdst_ObjectPonderation *p_SRC;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		p_SRC = pst_Object->p_SKN_Objectponderation;

		fNorm            = 0.0f;
		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;

		/* Compute lenght */
		while ( ulNumberOfMatrix-- )
		{
			if ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation < 0.0 )
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation = 0.0f;
			fNorm += pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation;
		}

		if ( fNorm != 0.0 )
		{
			fNorm = 128.0f / fNorm;
			/* Normalize */
			ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
			while ( ulNumberOfMatrix-- )
			{
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation *= fNorm;
			}
			/* 2nd normalize align on (int 128)*/
			/* re Compute lenght */
			fNorm            = 0.0f;
			ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
			while ( ulNumberOfMatrix-- )
			{
				int Destroy;
				Destroy                                                                                                 = ( int ) ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation + 0.49999999999999f );
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation = ( float ) Destroy;
				fNorm += pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation;
			}
			fNorm = 1.0f / fNorm;
			/* re-Normalize */
			ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
			while ( ulNumberOfMatrix-- )
			{
				float fuck;
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation *= fNorm;
				if ( *( ULONG * ) &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation & 0xffff )
					fuck = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation;
			}
		}
		else
		{
			/* Init to first Matrix */
			ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
			while ( ulNumberOfMatrix-- )
			{
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation = 0.0f;
			}

			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ 0 ]->p_PdrtVrc_E[ Index ].f_Ponderation = 1.0f;
		}
	}

	/*$0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Per object
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

	void GEO_SKN_Normalize( GEO_tdst_Object *pst_Object, ULONG ulFlags )
	{
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_Normalize_Vertex, 0, 0.0f, ulFlags );
	}

	/*$4
********************************************************************************************************************************************************************************
Compress Ponderation information. The Goal is to optimize display of skin Can destroy skin if all is in channel 0
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_Compress_VP( GEO_tdst_VertexPonderationList *p_PdrtLst )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG Counter, D0;
		GEO_tdst_ExpandedVertexPonderation *Expd;
		GEO_tdst_CompressedVertexPonderation *Cmpr;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( !p_PdrtLst->p_PdrtVrc_E ) return;

		Expd = p_PdrtLst->p_PdrtVrc_E;
		Cmpr = p_PdrtLst->p_PdrtVrc_C;
		D0   = 0;
		for ( Counter = 0; Counter < p_PdrtLst->us_NumberOfPonderatedVertices; Counter++ )
		{
			if ( Expd->f_Ponderation > 0.02f )
			{
				*( float * ) Cmpr = Expd->f_Ponderation;
				Cmpr->Index       = ( unsigned short ) Counter;
				D0++;
				Cmpr++;
			}

			Expd++;
		}

		p_PdrtLst->us_NumberOfPonderatedVertices = ( unsigned short ) D0;
		if ( D0 )
			p_PdrtLst->p_PdrtVrc_C = ( GEO_tdst_CompressedVertexPonderation * ) MEM_p_Realloc(
			        p_PdrtLst->p_PdrtVrc_C,
			        D0 * sizeof( GEO_tdst_CompressedVertexPonderation ) );
		else
			MEM_Free( p_PdrtLst->p_PdrtVrc_C );
	}

	/*$4
********************************************************************************************************************************************************************************
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_Compress( GEO_tdst_Object *pst_Object )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG MatrixCounter, MatrixNumber;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( !GEO_SKN_IsExpanded( pst_Object ) ) return;


		GEO_SKN_Normalize( pst_Object, 0 );

		/* Begin Compress vertices */
		MatrixCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( MatrixCounter-- )
		{
			GEO_SKN_Compress_VP( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ] );
			if ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ]->us_NumberOfPonderatedVertices == 0 )
			{
				MEM_Free( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ] );
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ] = NULL;
			}
		}
		/* End Compress vertices */

		/* Begin Compress list */
		MatrixNumber = 0;
		for ( MatrixCounter = 0; MatrixCounter < pst_Object->p_SKN_Objectponderation->NumberPdrtLists; MatrixCounter++ )
		{
			if ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ] )
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixNumber++ ] = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ];
		}
		pst_Object->p_SKN_Objectponderation->NumberPdrtLists = ( unsigned short ) MatrixNumber;
		if ( MatrixNumber )
		{
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst = ( GEO_tdst_VertexPonderationList ** ) MEM_p_Realloc(
			        pst_Object->p_SKN_Objectponderation->pp_PdrtLst,
			        MatrixNumber * sizeof( GEO_tdst_VertexPonderationList * ) );
		}
		else
		{
			MEM_Free( pst_Object->p_SKN_Objectponderation->pp_PdrtLst );
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst = NULL;
		}
		/* End Compress list */

		pst_Object->p_SKN_Objectponderation->flags &= 0xfffe;
	}

	/*$4
********************************************************************************************************************************************************************************
Decompress Ponderation information. The Goal is to simplify computation on skin
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_Expand_VP( GEO_tdst_VertexPonderationList *p_PdrtLst, ULONG ul_NumbrOfPoints )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG Counter, D0;
		GEO_tdst_ExpandedVertexPonderation *Expd;
		GEO_tdst_CompressedVertexPonderation *Cmpr;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		Expd = ( GEO_tdst_ExpandedVertexPonderation * ) MEM_p_Alloc( sizeof( GEO_tdst_ExpandedVertexPonderation ) * ul_NumbrOfPoints );
		L_memset( Expd, 0 /* == 0.0f */, sizeof( GEO_tdst_ExpandedVertexPonderation ) * ul_NumbrOfPoints );
		Cmpr = p_PdrtLst->p_PdrtVrc_C;
		D0   = 0;
		for ( Counter = 0; Counter < p_PdrtLst->us_NumberOfPonderatedVertices; Counter++ )
		{
			Expd[ Cmpr->Index ].f_Ponderation = *( float * ) Cmpr;
			Cmpr++;
		}

		p_PdrtLst->us_NumberOfPonderatedVertices = ( unsigned short ) ul_NumbrOfPoints;
		MEM_Free( p_PdrtLst->p_PdrtVrc_C );
		p_PdrtLst->p_PdrtVrc_E = Expd;
	}

	ULONG GEO_SKN_GetMatrixMax( GEO_tdst_VertexPonderationList **p_PdrtLst, ULONG ul_Num )
	{
		/*~~~~~~~~~~~~~~*/
		ULONG ulMAX;
		/*~~~~~~~~~~~~~~*/

		ulMAX = 0;
		while ( ul_Num-- )
		{
			if ( p_PdrtLst[ ul_Num ] && ( ulMAX < p_PdrtLst[ ul_Num ]->us_IndexOfMatrix ) )
				ulMAX = p_PdrtLst[ ul_Num ]->us_IndexOfMatrix;
		}

		return ulMAX;
	}

	void GEO_SKN_Expand( GEO_tdst_Object *pst_Object )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG MatrixCounter, RealMatrixNum;
		GEO_tdst_VertexPonderationList **pp_NewList;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( GEO_SKN_IsExpanded( pst_Object ) ) return;

		/* Begin Expand list */
		RealMatrixNum = GEO_SKN_GetMatrixMax(
		                        pst_Object->p_SKN_Objectponderation->pp_PdrtLst,
		                        pst_Object->p_SKN_Objectponderation->NumberPdrtLists ) +
		                1;
		pp_NewList = ( GEO_tdst_VertexPonderationList ** ) MEM_p_Alloc( RealMatrixNum * sizeof( GEO_tdst_VertexPonderationList * ) );
		L_memset( pp_NewList, 0, sizeof( GEO_tdst_VertexPonderationList * ) * RealMatrixNum );
		MatrixCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( MatrixCounter-- )
		{
			if ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ] )
			{
				pp_NewList[ pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ]->us_IndexOfMatrix ] = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ];
			}
		}

		MEM_Free( pst_Object->p_SKN_Objectponderation->pp_PdrtLst );
		pst_Object->p_SKN_Objectponderation->pp_PdrtLst      = pp_NewList;
		pst_Object->p_SKN_Objectponderation->NumberPdrtLists = ( unsigned short ) RealMatrixNum;

		/* End Expand list */

		/* Begin Expand vertices */
		MatrixCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( MatrixCounter-- )
		{
			if ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ] == NULL )
			{
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ] = GEO_SKN_CreateList(
				        pst_Object->l_NbPoints,
				        MatrixCounter );
			}
			else
			{
				GEO_SKN_Expand_VP( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ], pst_Object->l_NbPoints );
			}
		}
		/* End Expand vertices */
		if ( pst_Object->p_SKN_Objectponderation->dul_PointColors == NULL )
		{
			pst_Object->p_SKN_Objectponderation->dul_PointColors = ( ULONG * ) MEM_p_Alloc( pst_Object->l_NbPoints * ( LONG ) 4 );
			L_memset( pst_Object->p_SKN_Objectponderation->dul_PointColors, 0, pst_Object->l_NbPoints * ( LONG ) 4 );
		}
		pst_Object->p_SKN_Objectponderation->flags |= 1;
	}

	/*$4
********************************************************************************************************************************************************************************
Poped selection blender. Used for All Edges Operation. Internal used
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_PopedSelBend_Vertex( GEO_tdst_Object *pst_Object, ULONG Index, ULONG r, float Value )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation = pst_Object->p_SKN_Objectponderation->PushStack->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation;
		}
	}
	void GEO_SKN_PopedSelBend( GEO_tdst_Object *pst_Object, float fRadius, ULONG ulFlags )
	{
		if ( !pst_Object->p_SKN_Objectponderation->PushStack ) return;
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_PopedSelBend_Vertex, 0, 0.0f, ulFlags ^ SKN_Cul_InvertSel );
	}

	/*$4
********************************************************************************************************************************************************************************
Poped selection blender. Used for All Edges Operation. Internal used
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_CopySelBend_Vertex( GEO_tdst_Object *pst_Object, ULONG Index, ULONG r, float Value )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation *= Value;
		}
	}
	void GEO_SKN_CopySelBend( GEO_tdst_Object *pst_Object, float fRadius, ULONG ulFlags )
	{
		if ( !pst_Object->p_SKN_Objectponderation->PushStack ) return;
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_PopedSelBend_Vertex, 0, fRadius, ulFlags ^ SKN_Cul_InvertSel );
	}

	/*$4
********************************************************************************************************************************************************************************
Invert 2 ponderation
********************************************************************************************************************************************************************************
*/
	ULONG VALUE1, VALUE2;
	void GEO_SKN_SwitchGizmo_Vertex( GEO_tdst_Object *pst_Object, ULONG Index, ULONG r, float Value )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		float Switch;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		Switch                                                                                        = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ VALUE1 ]->p_PdrtVrc_E[ Index ].f_Ponderation;
		pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ VALUE1 ]->p_PdrtVrc_E[ Index ].f_Ponderation = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ VALUE2 ]->p_PdrtVrc_E[ Index ].f_Ponderation;
		pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ VALUE2 ]->p_PdrtVrc_E[ Index ].f_Ponderation = Switch;
	}
	void GEO_SKN_SwitchGizmo( GEO_tdst_Object *pst_Object, ULONG Gizmo1, ULONG Gizmo2, ULONG ulFlags )
	{
		VALUE1 = Gizmo1;
		VALUE2 = Gizmo2;
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_SwitchGizmo_Vertex, 0, 0.0f, ulFlags );
	}
	/*$4
********************************************************************************************************************************************************************************
Smoothing ponderations
********************************************************************************************************************************************************************************
*/
	float fSmoothStraight;
	void GEO_SKN_Smooth_Edge( GEO_tdst_Object *pst_Object, GEO_tdst_ObjectPonderation *p_Src, GEO_tdst_ObjectPonderation *p_Dst, ULONG I1, ULONG I2 )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulNumberOfMatrix = p_Dst->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			p_Dst->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I1 ].f_Ponderation += p_Src->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I2 ].f_Ponderation * fSmoothStraight;
			p_Dst->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I2 ].f_Ponderation += p_Src->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I1 ].f_Ponderation * fSmoothStraight;//*/
		}
	}
	void GEO_SKN_Smooth( GEO_tdst_Object *pst_Object, float fRadiusMax, ULONG bUseSubSel )
	{
		fSmoothStraight = fRadiusMax;
		GEO_SKN_ForEachEdgeDo( pst_Object, GEO_SKN_Smooth_Edge, 1.0f - fRadiusMax, bUseSubSel );
	}

	/*$4
********************************************************************************************************************************************************************************
Expand ponderations
********************************************************************************************************************************************************************************
*/
	float ExpandFactor;
	ULONG ul_Gizmo2Expand;
	void GEO_SKN_ExpandGizmo_Edge( GEO_tdst_Object *pst_Object, GEO_tdst_ObjectPonderation *p_Src, GEO_tdst_ObjectPonderation *p_Dst, ULONG I1, ULONG I2 )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulNumberOfMatrix = p_Dst->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			/* Blend*/
			if ( ul_Gizmo2Expand != ulNumberOfMatrix )
			{
				p_Dst->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I1 ].f_Ponderation *= ( 1.0f - ExpandFactor * p_Src->pp_PdrtLst[ ul_Gizmo2Expand ]->p_PdrtVrc_E[ I2 ].f_Ponderation );
				p_Dst->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I2 ].f_Ponderation *= ( 1.0f - ExpandFactor * p_Src->pp_PdrtLst[ ul_Gizmo2Expand ]->p_PdrtVrc_E[ I1 ].f_Ponderation );
			}
			else
			{
				p_Dst->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I1 ].f_Ponderation += ExpandFactor * p_Src->pp_PdrtLst[ ul_Gizmo2Expand ]->p_PdrtVrc_E[ I2 ].f_Ponderation;
				p_Dst->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I2 ].f_Ponderation += ExpandFactor * p_Src->pp_PdrtLst[ ul_Gizmo2Expand ]->p_PdrtVrc_E[ I1 ].f_Ponderation;
			}
		}
	}
	void GEO_SKN_ExpandGizmo( GEO_tdst_Object *pst_Object, ULONG Gizmo, float thresh, ULONG ulFlags )
	{
		ExpandFactor    = thresh * thresh * thresh;
		ul_Gizmo2Expand = Gizmo;
		GEO_SKN_ForEachEdgeDo( pst_Object, GEO_SKN_ExpandGizmo_Edge, 1.0f, ulFlags );
	}
	/*$4
********************************************************************************************************************************************************************************
ÌnsertBtween ponderATION
********************************************************************************************************************************************************************************
*/

	float InsertFactor;
	ULONG ul_Gizmo2Insert;
	ULONG ul_Gizmo2A;
	ULONG ul_Gizmo2B;
	void GEO_SKN_InsertGizmo_Edge( GEO_tdst_Object *pst_Object, GEO_tdst_ObjectPonderation *p_Src, GEO_tdst_ObjectPonderation *p_Dst, ULONG I1, ULONG I2 )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		float fDiff;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		fDiff = p_Src->pp_PdrtLst[ ul_Gizmo2A ]->p_PdrtVrc_E[ I1 ].f_Ponderation - p_Src->pp_PdrtLst[ ul_Gizmo2A ]->p_PdrtVrc_E[ I2 ].f_Ponderation;
		fDiff *= p_Src->pp_PdrtLst[ ul_Gizmo2B ]->p_PdrtVrc_E[ I1 ].f_Ponderation - p_Src->pp_PdrtLst[ ul_Gizmo2B ]->p_PdrtVrc_E[ I2 ].f_Ponderation;
		if ( fDiff < 0.0f )
			fDiff = -fDiff;
		fDiff *= InsertFactor;

		ulNumberOfMatrix = p_Dst->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			/* Blend*/
			if ( ul_Gizmo2Insert != ulNumberOfMatrix )
			{
				p_Dst->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I1 ].f_Ponderation *= ( 1.0f - fDiff );
				p_Dst->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I2 ].f_Ponderation *= ( 1.0f - fDiff );
			}
			else
			{
				p_Dst->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I1 ].f_Ponderation += fDiff;
				p_Dst->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ I2 ].f_Ponderation += fDiff;
			}
		}
	}
	void GEO_SKN_InsertGizmo( GEO_tdst_Object *pst_Object, ULONG Gizmo, ULONG A, ULONG B, float thresh, ULONG ulFlags )
	{
		InsertFactor    = thresh;
		ul_Gizmo2Insert = Gizmo;
		ul_Gizmo2A      = A;
		ul_Gizmo2B      = B;
		GEO_SKN_ForEachEdgeDo( pst_Object, GEO_SKN_InsertGizmo_Edge, 1.0f, ulFlags );
	}


	/*$4
********************************************************************************************************************************************************************************
Contrast ponderations
********************************************************************************************************************************************************************************
*/
	/*$0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Per Vertex
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
	ULONG *RGB3;
	float *VALUE3;
	void GEO_SKN_Paint_Vertex( GEO_tdst_Object *pst_Object, ULONG Index, ULONG X, float Y )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		/* Reset if not Add */
		if ( !X )
		{
			while ( ulNumberOfMatrix-- )
			{
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation = 0.0f;
			}
		}
		/* Set */
		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		if ( RGB3[ 0 ] < ulNumberOfMatrix )
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ RGB3[ 0 ] ]->p_PdrtVrc_E[ Index ].f_Ponderation += VALUE3[ 0 ];
		if ( RGB3[ 1 ] < ulNumberOfMatrix )
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ RGB3[ 1 ] ]->p_PdrtVrc_E[ Index ].f_Ponderation += VALUE3[ 1 ];
		if ( RGB3[ 2 ] < ulNumberOfMatrix )
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ RGB3[ 2 ] ]->p_PdrtVrc_E[ Index ].f_Ponderation += VALUE3[ 2 ];
	}
	/*$0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Per Object
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
	void GEO_SKN_Paint_SUB( GEO_tdst_Object *pst_Object, ULONG *p_3ulRGB, float *p_3Values, ULONG ulFlags )
	{
		RGB3        = p_3ulRGB;
		VALUE3      = p_3Values;
		VALUE3[ 0 ] = -VALUE3[ 0 ];
		VALUE3[ 1 ] = -VALUE3[ 1 ];
		VALUE3[ 2 ] = -VALUE3[ 2 ];
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_Paint_Vertex, 2, 0.0f, ulFlags );
	}
	void GEO_SKN_Paint_ADD( GEO_tdst_Object *pst_Object, ULONG *p_3ulRGB, float *p_3Values, ULONG ulFlags )
	{
		RGB3   = p_3ulRGB;
		VALUE3 = p_3Values;
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_Paint_Vertex, 1, 0.0f, ulFlags );
	}
	void GEO_SKN_Paint( GEO_tdst_Object *pst_Object, ULONG *p_3ulRGB, float *p_3Values, ULONG ulFlags )
	{
		RGB3   = p_3ulRGB;
		VALUE3 = p_3Values;
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_Paint_Vertex, 0, 0.0f, ulFlags );
	}
	/*$4
********************************************************************************************************************************************************************************
Contrast ponderations
********************************************************************************************************************************************************************************
*/

	/*$0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Per Vertex
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

	void GEO_SKN_ContrastVertex( GEO_tdst_Object *pst_Object, ULONG Index, ULONG X, float Contrast )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		ULONG ulNumberOfPonderation;
		float fMiddle;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		fMiddle               = 0.0f;
		ulNumberOfPonderation = 0;
		ulNumberOfMatrix      = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;

		/* Compute average */
		while ( ulNumberOfMatrix-- )
		{
			ulNumberOfPonderation++;
			fMiddle += pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation;
		}

		if ( ulNumberOfPonderation != 0 )
			fMiddle /= ( float ) ulNumberOfPonderation;
		else
			return;

		/* Contrast */
		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation +=
			        ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation -
			          fMiddle ) *
			        Contrast;
		}
	}

	/*$0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Per Object
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

	void GEO_SKN_Contrast( GEO_tdst_Object *pst_Object, float f_Contrast, ULONG ulFlags )
	{
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_ContrastVertex, 0, f_Contrast, ulFlags );
	}

	/*$4
********************************************************************************************************************************************************************************
Remove isolated ponderation
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_RemoveIsolated( GEO_tdst_Object *pst_Object, ULONG bUseSubSel )
	{
		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( !GEO_SKN_IsExpanded( pst_Object ) )
			GEO_SKN_Expand( pst_Object );

		/* Smoothing here */
	}

	/*$4
********************************************************************************************************************************************************************************
Force N Matrix Max. (typicaly, 3 should be a good max value). Goal is optimisation
********************************************************************************************************************************************************************************
*/

	/*$0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Per vertex: Choose the N most importants matrix of the point..
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

	void GEO_SKN_ForceNmax_Vertex( GEO_tdst_Object *pst_Object, ULONG Index, ULONG NMax, float notused )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG Order[ GEO_SKN_MAXNUMBEROFMATRIX ];
		float MaxVf[ GEO_SKN_MAXNUMBEROFMATRIX ];
		ULONG Counter1, ulNumberOfMatrix;
		float fLocal;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		Counter1 = NMax;
		while ( Counter1-- )
		{
			Order[ Counter1 ] = 0xffffffff;
		}

		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			fLocal   = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation;
			Counter1 = NMax;
			while ( Counter1-- )
			{
				if ( ( Order[ Counter1 ] == 0xffffffff ) || ( MaxVf[ Counter1 ] < fLocal ) )
				{
					Order[ Counter1 ] = ulNumberOfMatrix;
					MaxVf[ Counter1 ] = fLocal;
					Counter1          = 0;
				}
			}

			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation = 0.0f;
		}

		Counter1 = NMax;
		while ( Counter1-- )
		{
			if ( Order[ Counter1 ] != 0xffffffff )
			{
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ Order[ Counter1 ] ]->p_PdrtVrc_E[ Index ].f_Ponderation = MaxVf[ Counter1 ];
			}
		}
	}

	/*$0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Per Object
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

	void GEO_SKN_ForceNMatrixMax( GEO_tdst_Object *pst_Object, ULONG NumberOfMaxMatrix, ULONG ulFlags )
	{
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_ForceNmax_Vertex, NumberOfMaxMatrix, 0.0f, ulFlags );
	}

	/*$4
********************************************************************************************************************************************************************************
All is in the name
********************************************************************************************************************************************************************************
*/
	/*$0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Per vertex: Choose the N most importants matrix of the point..
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
	void GEO_SKN_ForceLimitMin_VERTEX( GEO_tdst_Object *pst_Object, ULONG Index, ULONG NMax, float Limit )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		float fbiggest;
		ULONG ulbiggestI;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		fbiggest         = 0.0f;
		ulbiggestI       = 0;
		while ( ulNumberOfMatrix-- )
		{
			if ( fbiggest < pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation )
			{
				fbiggest   = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation;
				ulbiggestI = ulNumberOfMatrix;
			}
			if ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation < Limit )
			{
				pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation = 0.0f;
			}
		}
		pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulbiggestI ]->p_PdrtVrc_E[ Index ].f_Ponderation = fbiggest;
	}
	/*$0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Per Object
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
	void GEO_SKN_ForceLimitMin( GEO_tdst_Object *pst_Object, float LimitMin, ULONG ulFlags )
	{
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_ForceLimitMin_VERTEX, 0, LimitMin, ulFlags );
	}
	/*$4
********************************************************************************************************************************************************************************
All is in the name
********************************************************************************************************************************************************************************
*/
	ULONG GEO_SKN_IsExpanded( GEO_tdst_Object *pst_Object )
	{
		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return 0;
		return pst_Object->p_SKN_Objectponderation->flags & 1;
	}

	/*$4
********************************************************************************************************************************************************************************
Pop ponderations (used for UNDO-REDO & editor mode)
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_Push( GEO_tdst_Object *pst_Object )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_ObjectPonderation *p_2Push;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( !GEO_SKN_IsExpanded( pst_Object ) )
			GEO_SKN_Expand( pst_Object );
		if ( pst_Object->p_SKN_Objectponderation->REDOStack )
		{
			p_2Push                  = pst_Object->p_SKN_Objectponderation->REDOStack;
			p_2Push->PushStack       = NULL;
			p_2Push->dul_PointColors = NULL;
			while ( p_2Push->REDOStack )
			{
				p_2Push                  = p_2Push->REDOStack;
				p_2Push->dul_PointColors = NULL;
			}
			GEO_SKN_DestroyObjPonderation( p_2Push );
			pst_Object->p_SKN_Objectponderation->REDOStack = NULL;
		}
		p_2Push                                        = GEO_SKN_Duplicate( pst_Object->p_SKN_Objectponderation );
		p_2Push->PushStack                             = pst_Object->p_SKN_Objectponderation;
		pst_Object->p_SKN_Objectponderation            = p_2Push;
		pst_Object->p_SKN_Objectponderation->REDOStack = NULL;
	}

	/*$4
********************************************************************************************************************************************************************************
Push ponderations (used for UNDO-REDO & editor mode)
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_Pop( GEO_tdst_Object *pst_Object )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_ObjectPonderation *p_2Pop;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( pst_Object->p_SKN_Objectponderation->PushStack == NULL ) return;
		p_2Pop                                         = pst_Object->p_SKN_Objectponderation;
		pst_Object->p_SKN_Objectponderation            = p_2Pop->PushStack;
		pst_Object->p_SKN_Objectponderation->REDOStack = p_2Pop;
	}

	/*$4
********************************************************************************************************************************************************************************
Push ponderations (used for UNDO-REDO & editor mode)
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_REDO( GEO_tdst_Object *pst_Object )
	{
		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( pst_Object->p_SKN_Objectponderation->REDOStack == NULL ) return;
		pst_Object->p_SKN_Objectponderation = pst_Object->p_SKN_Objectponderation->REDOStack;
	}

	ULONG GEO_SKN_Get_UnRe_Sate( GEO_tdst_Object *pst_Object )
	{
		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return 0;
		if ( ( pst_Object->p_SKN_Objectponderation->REDOStack && pst_Object->p_SKN_Objectponderation->PushStack ) ) return 3;
		if ( ( pst_Object->p_SKN_Objectponderation->REDOStack ) ) return 2;
		if ( ( pst_Object->p_SKN_Objectponderation->PushStack ) ) return 1;
		return 0;
	}
	/*$4
********************************************************************************************************************************************************************************
Transform colors into skin ponderation
********************************************************************************************************************************************************************************
*/

	ULONG ulgR, ulgG, ulgB;
	void GEO_SKN_Skin2Colors_VP( GEO_tdst_Object *pst_Object, ULONG Index, ULONG r, float Notused )
	{
		ULONG MaxIndex, Counter, Color;
		float MaxValue;
		MaxValue = 0.0f;
		Counter  = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		MaxIndex = 0;
		ulgR %= pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		ulgG %= pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		ulgB %= pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( Counter-- )
		{
			if ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ Counter ]->p_PdrtVrc_E[ Index ].f_Ponderation >= MaxValue )
			{
				MaxValue = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ Counter ]->p_PdrtVrc_E[ Index ].f_Ponderation;
				MaxIndex = Counter;
			}
		}
		Color = ( ( MaxIndex & 0x0004 ) << 21 );
		Color |= ( ( MaxIndex & 0x0002 ) << 14 );
		Color |= ( ( MaxIndex & 0x0001 ) << 7 );
		if ( MaxIndex & 8 ) Color |= 0x404040;
		if ( MaxIndex & 16 ) Color |= 0x202020;

		Color = u4_Interpol2Color( Color, 0, 0.5f );

		MaxValue = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulgR ]->p_PdrtVrc_E[ Index ].f_Ponderation;
		MaxValue += pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulgG ]->p_PdrtVrc_E[ Index ].f_Ponderation;
		MaxValue += pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulgB ]->p_PdrtVrc_E[ Index ].f_Ponderation;

		pst_Object->p_SKN_Objectponderation->dul_PointColors[ Index ] = 0;
		if ( ulgR < pst_Object->p_SKN_Objectponderation->NumberPdrtLists )
		{
			pst_Object->p_SKN_Objectponderation->dul_PointColors[ Index ] |= ( ( ULONG ) ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulgR ]->p_PdrtVrc_E[ Index ].f_Ponderation * 255.0f ) ) << 0;
		}

		if ( ulgG < pst_Object->p_SKN_Objectponderation->NumberPdrtLists )
		{
			pst_Object->p_SKN_Objectponderation->dul_PointColors[ Index ] |= ( ( ULONG ) ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulgG ]->p_PdrtVrc_E[ Index ].f_Ponderation * 255.0f ) ) << 8;
		}

		if ( ulgB < pst_Object->p_SKN_Objectponderation->NumberPdrtLists )
		{
			pst_Object->p_SKN_Objectponderation->dul_PointColors[ Index ] |= ( ( ULONG ) ( pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulgB ]->p_PdrtVrc_E[ Index ].f_Ponderation * 255.0f ) ) << 16;
		}
		pst_Object->p_SKN_Objectponderation->dul_PointColors[ Index ] = u4_Interpol2Color( Color, pst_Object->p_SKN_Objectponderation->dul_PointColors[ Index ], MaxValue );
	}

	void GEO_SKN_Skin2Colors( GEO_tdst_Object *pst_Object, ULONG ulR, ULONG ulG, ULONG ulB, ULONG ulFlags )
	{
		if ( pst_Object->p_SKN_Objectponderation->dul_PointColors == NULL )
		{
			pst_Object->p_SKN_Objectponderation->dul_PointColors = ( ULONG * ) MEM_p_Alloc( pst_Object->l_NbPoints * ( LONG ) 4 );
			L_memset( pst_Object->p_SKN_Objectponderation->dul_PointColors, 0, pst_Object->l_NbPoints * ( LONG ) 4 );
		}

		GEO_SKN_Normalize( pst_Object, 0 );
		ulgR = ulR;
		ulgG = ulG;
		ulgB = ulB;
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_Skin2Colors_VP, 0, 0.0f, ulFlags );
	}
	/*$4
********************************************************************************************************************************************************************************
Transform skin ponderation into colors
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_Colors2Skin_VP( GEO_tdst_Object *pst_Object, ULONG Index, ULONG RGB, float Notused )
	{
		pst_Object->p_SKN_Objectponderation->dul_PointColors[ Index ] = 0;
		if ( ulgR < pst_Object->p_SKN_Objectponderation->NumberPdrtLists )
		{
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulgR ]->p_PdrtVrc_E[ Index ].f_Ponderation = ( float ) ( ( pst_Object->p_SKN_Objectponderation->dul_PointColors[ Index ] >> 0 ) & 0xff ) / 255.0f;
		}

		if ( ulgG < pst_Object->p_SKN_Objectponderation->NumberPdrtLists )
		{
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulgG ]->p_PdrtVrc_E[ Index ].f_Ponderation = ( float ) ( ( pst_Object->p_SKN_Objectponderation->dul_PointColors[ Index ] >> 8 ) & 0xff ) / 255.0f;
		}

		if ( ulgB < pst_Object->p_SKN_Objectponderation->NumberPdrtLists )
		{
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulgB ]->p_PdrtVrc_E[ Index ].f_Ponderation = ( float ) ( ( pst_Object->p_SKN_Objectponderation->dul_PointColors[ Index ] >> 16 ) & 0xff ) / 255.0f;
		}
	}

	void GEO_SKN_Colors2Skin( GEO_tdst_Object *pst_Object, ULONG ulR, ULONG ulG, ULONG ulB, ULONG ulFlags )
	{
		if ( !pst_Object->p_SKN_Objectponderation->dul_PointColors ) return;
		ulgR = ulR;
		ulgG = ulG;
		ulgB = ulB;
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_Colors2Skin_VP, 0, 0.0f, ulFlags );
		GEO_SKN_Normalize( pst_Object, 0 );
	}

	/*$4
********************************************************************************************************************************************************************************
Create the skin, initialize ponderation to first matrix. Editor mode only.
********************************************************************************************************************************************************************************
*/
	GEO_tdst_VertexPonderationList *GEO_SKN_CreateList( ULONG NumbrOfPoints, ULONG MatrixNumber )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_VertexPonderationList *p_RtrnValye;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		p_RtrnValye                                = ( GEO_tdst_VertexPonderationList                                *) MEM_p_Alloc( sizeof( GEO_tdst_VertexPonderationList ) );
		p_RtrnValye->us_IndexOfMatrix              = ( unsigned short ) MatrixNumber;
		p_RtrnValye->us_NumberOfPonderatedVertices = ( unsigned short ) NumbrOfPoints;
		p_RtrnValye->p_PdrtVrc_E                   = ( GEO_tdst_ExpandedVertexPonderation                   *) MEM_p_Alloc( sizeof( GEO_tdst_ExpandedVertexPonderation ) * NumbrOfPoints );
		L_memset(
		        ( void * ) p_RtrnValye->p_PdrtVrc_E,
		        0 /* == 0.0f */,
		        sizeof( GEO_tdst_ExpandedVertexPonderation ) * NumbrOfPoints );
		MATH_SetIdentityMatrix( &p_RtrnValye->st_FlashedMatrix );
		return p_RtrnValye;
	}

	GEO_tdst_ObjectPonderation *GEO_SKN_CreateObjPonderation(
	        GEO_tdst_Object *pst_Object,
	        ULONG NumberOfMatrix,
	        ULONG NumberOfPoints )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_ObjectPonderation *p_NewOP;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( !NumberOfMatrix ) return NULL;
		if ( !NumberOfPoints ) return NULL;
		p_NewOP        = ( GEO_tdst_ObjectPonderation        *) MEM_p_Alloc( sizeof( GEO_tdst_ObjectPonderation ) );
		p_NewOP->flags = 1; /* Create it Expanded */
#	ifdef ACTIVE_EDITORS
		p_NewOP->SelectionCLBK = NULL;
		p_NewOP->ClassPtr      = NULL;
		p_NewOP->PushStack     = NULL;
		p_NewOP->REDOStack     = NULL;
#	endif// ACTIVE_EDITORS
		p_NewOP->pp_PdrtLst = ( GEO_tdst_VertexPonderationList ** ) MEM_p_Alloc( NumberOfMatrix * sizeof( GEO_tdst_VertexPonderationList * ) );

		p_NewOP->dul_PointColors = ( ULONG * ) MEM_p_Alloc( NumberOfPoints * ( LONG ) 4 );
		L_memset( p_NewOP->dul_PointColors, 0, NumberOfPoints * ( LONG ) 4 );

		p_NewOP->NumberPdrtLists = ( unsigned short ) NumberOfMatrix;
		while ( NumberOfMatrix-- )
		{
			p_NewOP->pp_PdrtLst[ NumberOfMatrix ] = GEO_SKN_CreateList( NumberOfPoints, NumberOfMatrix );
		}

		while ( NumberOfPoints-- )
		{
			p_NewOP->pp_PdrtLst[ 0 ]->p_PdrtVrc_E[ NumberOfPoints ].f_Ponderation = 1.0F;
		}

		return p_NewOP;
	}

	/*$4
********************************************************************************************************************************************************************************
Create a new ponderated point
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_ChangeNumberOfPoints( GEO_tdst_ObjectPonderation *p_OSP, ULONG NewNum )
	{
		ULONG ulMCounter;
		ulMCounter = p_OSP->NumberPdrtLists;
		while ( ulMCounter-- )
		{
			p_OSP->pp_PdrtLst[ ulMCounter ]->us_NumberOfPonderatedVertices = ( unsigned short ) NewNum;
			if ( NewNum )
				p_OSP->pp_PdrtLst[ ulMCounter ]->p_PdrtVrc_E = ( GEO_tdst_ExpandedVertexPonderation * ) MEM_p_Realloc( p_OSP->pp_PdrtLst[ ulMCounter ]->p_PdrtVrc_E, ( LONG ) 4 * NewNum );
			else
			{
				MEM_Free( p_OSP->pp_PdrtLst[ ulMCounter ]->p_PdrtVrc_E );
				p_OSP->pp_PdrtLst[ ulMCounter ]->p_PdrtVrc_E = NULL;
			}
		}

		if ( p_OSP->dul_PointColors )
		{
			if ( NewNum )
				p_OSP->dul_PointColors = ( ULONG * ) MEM_p_Realloc( p_OSP->dul_PointColors, 4 * NewNum );
			else
			{
				MEM_Free( p_OSP->dul_PointColors );
				p_OSP->dul_PointColors = NULL;
			}
		}
	}

	/*$4
********************************************************************************************************************************************************************************
Cut an edge
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_AddPointOnEdge( GEO_tdst_Object *_pst_Obj, ULONG A, ULONG B, float f )
	{
		GEO_tdst_ObjectPonderation *p_OSP;
		ULONG Pt, M;

		p_OSP = _pst_Obj->p_SKN_Objectponderation;
		Pt    = _pst_Obj->l_NbPoints - 1;

		while ( p_OSP )
		{
			GEO_SKN_ChangeNumberOfPoints( p_OSP, _pst_Obj->l_NbPoints );

			M = p_OSP->NumberPdrtLists;
			while ( M-- )
			{
				p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ Pt ].f_Ponderation = f * p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ A ].f_Ponderation;
				p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ Pt ].f_Ponderation += ( 1.0f - f ) * p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ B ].f_Ponderation;
			}

			p_OSP = p_OSP->PushStack;
		}
		GEO_SKN_UpdateSkinSelection( _pst_Obj );
	}

	/*$4
********************************************************************************************************************************************************************************
Cut a face
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_AddPointOnFace( GEO_tdst_Object *_pst_Obj, ULONG P1, float f1, ULONG P2, float f2, ULONG P3, float f3 )
	{
		GEO_tdst_ObjectPonderation *p_OSP;
		ULONG Pt, M;

		p_OSP = _pst_Obj->p_SKN_Objectponderation;
		Pt    = _pst_Obj->l_NbPoints - 1;

		while ( p_OSP )
		{
			GEO_SKN_ChangeNumberOfPoints( p_OSP, _pst_Obj->l_NbPoints );

			M = p_OSP->NumberPdrtLists;
			while ( M-- )
			{
				p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ Pt ].f_Ponderation = f1 * p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ P1 ].f_Ponderation;
				p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ Pt ].f_Ponderation += f2 * p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ P2 ].f_Ponderation;
				p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ Pt ].f_Ponderation += f3 * p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ P3 ].f_Ponderation;
			}

			p_OSP = p_OSP->PushStack;
		}

		GEO_SKN_UpdateSkinSelection( _pst_Obj );
	}

	int GEO_SKN_iGetPonderationIndex( GEO_tdst_ObjectPonderation *_p_OSP, SPG2_tdst_Modifier *pstModifier )
	{
		int i;
		for ( i = 0; i < _p_OSP->NumberPdrtLists; i++ )
		{
			if ( pstModifier->ulAlphaFromPondSelector == _p_OSP->pp_PdrtLst[ i ]->us_IndexOfMatrix )
			{
				return i;
			}
		}
		return -1;
	}

	int GEO_SKN_iGetTextureID( SPG2_tdst_Modifier *_pstMod, OBJ_tdst_GameObject *_pst_GO )
	{
		MAT_tdst_Material *pst_Mat = ( MAT_tdst_Material * ) _pst_GO->pst_Base->pst_Visu->pst_Material;
		if ( pst_Mat && ( pst_Mat->st_Id.i->ul_Type == GRO_MaterialMulti ) )
		{
			pst_Mat = ( ( MAT_tdst_Multi * ) pst_Mat )->dpst_SubMaterial[ lMin( _pstMod->ulSubMaterialNum, ( ( MAT_tdst_Multi * ) pst_Mat )->l_NumberOfSubMaterials - 1 ) ];
		}

		if ( pst_Mat &&
		     ( pst_Mat->st_Id.i->ul_Type == GRO_MaterialMultiTexture ) )
		{
			return ( ( MAT_tdst_MultiTexture * ) pst_Mat )->pst_FirstLevel->s_TextureId;
		}
		else
			return -1;
	}

	int GEO_SKN_iGetMaterialFromTextureID( OBJ_tdst_GameObject *_pst_GO, int _iTextureID )
	{
		int i;
		MAT_tdst_Material *pst_Mat = ( MAT_tdst_Material * ) _pst_GO->pst_Base->pst_Visu->pst_Material;
		if ( pst_Mat && ( pst_Mat->st_Id.i->ul_Type == GRO_MaterialMulti ) )
		{
			MAT_tdst_Material *pst_SubMat;
			for ( i = 0; i < ( ( MAT_tdst_Multi * ) pst_Mat )->l_NumberOfSubMaterials; i++ )
			{
				pst_SubMat = ( ( MAT_tdst_Multi * ) pst_Mat )->dpst_SubMaterial[ i ];
				if ( pst_SubMat &&
				     ( pst_SubMat->st_Id.i->ul_Type == GRO_MaterialMultiTexture ) &&
				     ( ( MAT_tdst_MultiTexture * ) pst_SubMat )->pst_FirstLevel->s_TextureId == _iTextureID )
					return i;
			}
		}
		return 0;
	}


	/*$4
********************************************************************************************************************************************************************************
ajoute plusieurs points
la pondération d'un point est récupérée d'un autre point
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_AddSeveralPoint( GEO_tdst_Object *_pst_Obj, LONG oldnb, LONG *I, GEO_tdst_ObjectPonderation *_p_SrcOSP, OBJ_tdst_GameObject *_pst_GO_Dst, OBJ_tdst_GameObject *_pst_GO_Src )
	{
		GEO_tdst_ObjectPonderation *p_OSP;
		LONG i, M;

		p_OSP = _pst_Obj->p_SKN_Objectponderation;

		while ( p_OSP )
		{
			GEO_SKN_ChangeNumberOfPoints( p_OSP, _pst_Obj->l_NbPoints );

			M = p_OSP->NumberPdrtLists;
			while ( M-- )
			{
				L_memset( p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E + oldnb, 0, ( _pst_Obj->l_NbPoints - oldnb ) * sizeof( float ) );
			}

			if ( I )
			{
				M = p_OSP->NumberPdrtLists;
				while ( M-- )
				{
					for ( i = 0; i < oldnb; i++ )
					{
						if ( I[ i ] == -1 ) break;
						p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ I[ i ] ].f_Ponderation = p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ i ].f_Ponderation;
					}
				}
			}

			// Merge new ponderation with old ponderation.
			if ( _p_SrcOSP )
			{
				// For each modifier in src GAO, search for same modifier in dst GAO.
				// If one is found, we use it, else we create it in the dst GAO.
				struct MDF_tdst_Modifier_ *pstModifierSrc = _pst_GO_Src->pst_Extended->pst_Modifiers;
				while ( pstModifierSrc )
				{
					if ( pstModifierSrc->i->ul_Type == MDF_C_Modifier_SpriteMapper2 )
					{
						SPG2_tdst_Modifier *pstSPG2ModifierDst;
						SPG2_tdst_Modifier *pstSPG2ModifierSrc = ( SPG2_tdst_Modifier * ) pstModifierSrc->p_Data;
						int lPonderationIndexDst;
						int lPonderationIndexSrc = GEO_SKN_iGetPonderationIndex( _p_SrcOSP, pstSPG2ModifierSrc );
						int iTextureID_Src       = GEO_SKN_iGetTextureID( pstSPG2ModifierSrc, _pst_GO_Src );

						// If ponderation index was not found, the modifier is not valid, skip it.
						if ( lPonderationIndexSrc != -1 )
						{
							// Search for same modifier in dst GAO.
							struct MDF_tdst_Modifier_ *pstModifierDst = _pst_GO_Dst->pst_Extended->pst_Modifiers;
							while ( pstModifierDst )
							{
								if ( pstModifierDst->i->ul_Type == MDF_C_Modifier_SpriteMapper2 )
								{
									pstSPG2ModifierDst = ( SPG2_tdst_Modifier * ) pstModifierDst->p_Data;
									if ( SPG2_bIsSameSPG2Modifier( pstSPG2ModifierSrc, pstSPG2ModifierDst ) )
									{
										lPonderationIndexDst = GEO_SKN_iGetPonderationIndex( p_OSP, pstSPG2ModifierDst );
										if ( lPonderationIndexDst != -1 )
										{
											// The dst modifier must have a submaterial with same texture name
											// as the submaterial from src modifier
											int iTextureID_Dst = GEO_SKN_iGetTextureID( pstSPG2ModifierDst, _pst_GO_Dst );

											if ( iTextureID_Dst != -1 )
											{
												if ( iTextureID_Src == iTextureID_Dst )
												{
													// Modifier found.
													break;
												}
											}
										}
									}
								}

								pstModifierDst = pstModifierDst->pst_Next;
							}

							// Modifier not found in dst GAO : copy it from src GAO and set ponderations and submaterial.
							if ( !pstModifierDst )
							{
								int iOldMatrixNb = p_OSP->NumberPdrtLists;
								int iNewMatrixNb = iOldMatrixNb + 1;

								// Create modifier.
								pstModifierDst = MDF_pst_Modifier_Create( _pst_GO_Dst, MDF_C_Modifier_SpriteMapper2, pstSPG2ModifierSrc );
								MDF_Modifier_AddToGameObject( _pst_GO_Dst, pstModifierDst );

								// Add one matrix for new modifier
								OBJ_Gizmo_ChangeNumberOfAdditionalMatrix( _pst_GO_Dst->pst_Base->pst_AddMatrix, iNewMatrixNb );
								GEO_SKN_SetNumberOfMatrix( _pst_Obj, iNewMatrixNb );

								pstSPG2ModifierDst                          = ( SPG2_tdst_Modifier                          *) pstModifierDst->p_Data;
								lPonderationIndexDst                        = iOldMatrixNb;
								pstSPG2ModifierDst->ulAlphaFromPondSelector = p_OSP->pp_PdrtLst[ lPonderationIndexDst ]->us_IndexOfMatrix;

								// The submaterial index should be the one that points to the correct texture
								pstSPG2ModifierDst->ulSubMaterialNum = GEO_SKN_iGetMaterialFromTextureID( _pst_GO_Dst, iTextureID_Src );
							}

							// Set ponderation from src to dst for the modifier.
							for ( i = 0; i < _pst_Obj->l_NbPoints - oldnb; i++ )
							{
								p_OSP->pp_PdrtLst[ lPonderationIndexDst ]->p_PdrtVrc_E[ i + oldnb ].f_Ponderation =
								        _p_SrcOSP->pp_PdrtLst[ lPonderationIndexSrc ]->p_PdrtVrc_E[ i ].f_Ponderation;
							}
						}
					}
					pstModifierSrc = pstModifierSrc->pst_Next;
				}
			}

			p_OSP = p_OSP->PushStack;
		}

		GEO_SKN_UpdateSkinSelection( _pst_Obj );
	}

	/*$4
********************************************************************************************************************************************************************************
ajoutes plusieurs points 
la pondération des points ajoutées est calculée en fonction de la pondération de 2 autres points et d'un ratio
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_AddSeveralPointWithRatio( GEO_tdst_Object *_pst_Obj, LONG first, LONG nb, LONG *I0, LONG *I1, float *F )
	{
		GEO_tdst_ObjectPonderation *p_OSP;
		LONG i, Pt, M;

		p_OSP = _pst_Obj->p_SKN_Objectponderation;

		while ( p_OSP )
		{
			GEO_SKN_ChangeNumberOfPoints( p_OSP, _pst_Obj->l_NbPoints );

			M = p_OSP->NumberPdrtLists;
			while ( M-- )
			{
				for ( Pt = first, i = 0; i < nb; i++, Pt++ )
				{
					p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ Pt ].f_Ponderation = F[ i ] * p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ I0[ i ] ].f_Ponderation;
					p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ Pt ].f_Ponderation += ( 1.0f - F[ i ] ) * p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ I1[ i ] ].f_Ponderation;
				}
			}

			p_OSP = p_OSP->PushStack;
		}

		GEO_SKN_UpdateSkinSelection( _pst_Obj );
	}

	/*$4
********************************************************************************************************************************************************************************
delete a point 
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_DelPoint( GEO_tdst_Object *pst_Object )
	{
		GEO_tdst_ObjectPonderation *p_OSP;

		p_OSP = pst_Object->p_SKN_Objectponderation;
		while ( p_OSP )
		{
			GEO_SKN_ChangeNumberOfPoints( p_OSP, pst_Object->l_NbPoints - 1 );
			p_OSP = p_OSP->PushStack;
		}
	}

	/*$4
********************************************************************************************************************************************************************************
delete some point
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_DelSomePoint( GEO_tdst_Object *_pst_Obj, LONG *_pl_Index, LONG _l_NbPointsLeft )
	{
		GEO_tdst_ObjectPonderation *p_OSP;
		LONG i, M;

		p_OSP = _pst_Obj->p_SKN_Objectponderation;

		while ( p_OSP )
		{
			M = p_OSP->NumberPdrtLists;
			while ( M-- )
			{
				for ( i = 0; i < _pst_Obj->l_NbPoints; i++ )
				{
					if ( ( _pl_Index[ i ] == -1 ) || ( i == _pl_Index[ i ] ) ) continue;
					p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ _pl_Index[ i ] ].f_Ponderation = p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ i ].f_Ponderation;
				}
				for ( i = _l_NbPointsLeft; i < _pst_Obj->l_NbPoints; i++ )
				{
					p_OSP->pp_PdrtLst[ M ]->p_PdrtVrc_E[ i ].f_Ponderation = 0;
				}
			}

			GEO_SKN_ChangeNumberOfPoints( p_OSP, _l_NbPointsLeft );
			p_OSP = p_OSP->PushStack;
		}

		i                    = _pst_Obj->l_NbPoints;
		_pst_Obj->l_NbPoints = _l_NbPointsLeft;
		GEO_SKN_UpdateSkinSelection( _pst_Obj );
		_pst_Obj->l_NbPoints = i;
	}

	/*$4
********************************************************************************************************************************************************************************
modification of vertex ponderation when vertex list is modified
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_UpdateAfterVertexEdition( GEO_tdst_Object *_pst_Obj, WOR_tdst_Update_RLI *_pst_Data )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		LONG l_Op;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( !GEO_SKN_IsSkinned( _pst_Obj ) ) return;
		GEO_SKN_Expand( _pst_Obj );

		l_Op = _pst_Data->l_Op & WOR_Update_RLI_OpMask;
		if ( l_Op == WOR_Update_RLI_Add )
		{
			if ( _pst_Data->l_Op & WOR_Update_RLI_Barycentre )
				GEO_SKN_AddPointOnFace( _pst_Obj, _pst_Data->l_Ind0, _pst_Data->f0, _pst_Data->l_Ind1, _pst_Data->f1, _pst_Data->l_Ind2, _pst_Data->f2 );
			else
				GEO_SKN_AddPointOnEdge( _pst_Obj, _pst_Data->l_Ind0, _pst_Data->l_Ind1, _pst_Data->f_Blend );
		}
		else if ( l_Op == WOR_Update_RLI_AddSome )
		{
			GEO_SKN_AddSeveralPoint( _pst_Obj, _pst_Data->l_OldNbPoints, ( LONG * ) _pst_Data->l_Ind1, _pst_Data->p_AddedSKN_Objectponderation, _pst_Data->pst_GoDst, _pst_Data->pst_GoSrc );
		}
		else if ( l_Op == WOR_Update_RLI_AddSomeCenter )
		{
			GEO_SKN_AddSeveralPointWithRatio( _pst_Obj, _pst_Data->l_OldNbPoints, _pst_Data->l_NbAdded, ( LONG * ) _pst_Data->l_Ind0, ( LONG * ) _pst_Data->l_Ind1, ( float * ) _pst_Data->l_Ind2 );
		}
		else if ( l_Op == WOR_Update_RLI_Del )
		{
			GEO_SKN_DelPoint( _pst_Obj );
		}
		else if ( l_Op == WOR_Update_RLI_DelSome )
		{
			GEO_SKN_DelSomePoint( _pst_Obj, ( LONG * ) _pst_Data->l_Ind1, _pst_Data->l_Ind0 );
		}
	}

	/*$4
********************************************************************************************************************************************************************************
Set Number Of Matrix: if no skin: Create the skin else Change the number of matrix, and ask user for differents
scenerizes
********************************************************************************************************************************************************************************
*/

	void GEO_SKN_SetNumberOfMatrix( GEO_tdst_Object *pst_Object, ULONG ulNubrOfMatrix )
	{
		if ( !ulNubrOfMatrix )
		{
			GEO_SKN_DestroyObjPonderation( pst_Object->p_SKN_Objectponderation );
			return;
		}

		if ( !pst_Object->p_SKN_Objectponderation )
		{
			pst_Object->p_SKN_Objectponderation = GEO_SKN_CreateObjPonderation(
			        pst_Object,
			        ulNubrOfMatrix,
			        pst_Object->l_NbPoints );
			return;
		}

		if ( !GEO_SKN_IsExpanded( pst_Object ) )
			GEO_SKN_Expand( pst_Object );

		if ( pst_Object->p_SKN_Objectponderation->NumberPdrtLists == ulNubrOfMatrix ) return;

		pst_Object->p_SKN_Objectponderation->pp_PdrtLst = ( GEO_tdst_VertexPonderationList ** ) MEM_p_Realloc(
		        pst_Object->p_SKN_Objectponderation->pp_PdrtLst,
		        ulNubrOfMatrix * sizeof( GEO_tdst_VertexPonderationList * ) );
		while ( pst_Object->p_SKN_Objectponderation->NumberPdrtLists < ulNubrOfMatrix )
		{
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ pst_Object->p_SKN_Objectponderation->NumberPdrtLists ]                   = GEO_SKN_CreateList( pst_Object->l_NbPoints, ulNubrOfMatrix );
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ pst_Object->p_SKN_Objectponderation->NumberPdrtLists ]->us_IndexOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists++;
		}

		pst_Object->p_SKN_Objectponderation->NumberPdrtLists = ( unsigned short ) ulNubrOfMatrix;
		GEO_SKN_Normalize( pst_Object, 0 );
	}
	/*$4
********************************************************************************************************************************************************************************
Select a given gizmo 
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_SelectGizmo( GEO_tdst_Object *pst_Object, ULONG Gizmo, float Thresh, ULONG bKeepOldSel )
	{
		GEO_tdst_ExpandedVertexPonderation *p_Exp, *p_ExpLst;
		char *pc_Sel;

		if ( !pst_Object->pst_SubObject ) return;
		if ( !pst_Object->pst_SubObject->dc_VSel ) return;
		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( !GEO_SKN_IsExpanded( pst_Object ) )
			GEO_SKN_Expand( pst_Object );
		pc_Sel = pst_Object->pst_SubObject->dc_VSel;
		Gizmo %= pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		p_Exp    = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ Gizmo ]->p_PdrtVrc_E;
		p_ExpLst = p_Exp + pst_Object->l_NbPoints;
		while ( p_Exp < p_ExpLst )
		{
			if ( p_Exp->f_Ponderation > Thresh )
			{
				*pc_Sel = 1;
			}
			else
			{
				if ( !bKeepOldSel )
				{
					*pc_Sel = 0;
				}
			}
			pc_Sel++;
			p_Exp++;
		}
	}
	/*$4
********************************************************************************************************************************************************************************
Find the 3 most importants gizmo from selection..
********************************************************************************************************************************************************************************
*/
	ULONG GEO_SKN_GetInfoAboutSelected( GEO_tdst_Object *p_stObject, ULONG *p_6Gizmo, float *p_6Values, ULONG ulFlag )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG Counter, MatrixCounter, MatrixCounter2, SelectionNum;
		LONG l_Sel;
		float SavedPonderation[ GEO_SKN_MAXNUMBEROFMATRIX ];
		ULONG SavedPonderationIndexes[ GEO_SKN_MAXNUMBEROFMATRIX ];
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		p_6Gizmo[ 0 ]  = 0;
		p_6Gizmo[ 1 ]  = 1;
		p_6Gizmo[ 2 ]  = 2;
		p_6Values[ 0 ] = 0.5f;
		p_6Values[ 1 ] = 0.5f;
		p_6Values[ 2 ] = 0.5f;
		if ( !GEO_SKN_IsSkinned( p_stObject ) ) return 0;
		if ( !GEO_SKN_IsExpanded( p_stObject ) ) GEO_SKN_Expand( p_stObject );

		SelectionNum = 0;

		Counter       = p_stObject->l_NbPoints;
		MatrixCounter = p_stObject->p_SKN_Objectponderation->NumberPdrtLists;
		while ( MatrixCounter-- )
		{
			SavedPonderation[ MatrixCounter ]        = 0.0f;
			SavedPonderationIndexes[ MatrixCounter ] = MatrixCounter;
		}

		if ( ( ulFlag & SKN_Cul_UseSubSel ) && p_stObject->pst_SubObject && p_stObject->pst_SubObject->dc_VSel )
		{
			while ( Counter-- )
			{
				l_Sel = ( p_stObject->pst_SubObject->dc_VSel[ Counter ] & 1 );
				if ( ulFlag & SKN_Cul_InvertSel ) l_Sel = 1 - l_Sel;
				if ( l_Sel )
				{
					MatrixCounter = p_stObject->p_SKN_Objectponderation->NumberPdrtLists;
					while ( MatrixCounter-- )
						SavedPonderation[ MatrixCounter ] += p_stObject->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ]->p_PdrtVrc_E[ Counter ].f_Ponderation;
					SelectionNum++;
				}
			}
		}
		if ( SelectionNum == 0 )
		{
			Counter      = p_stObject->l_NbPoints;
			SelectionNum = p_stObject->l_NbPoints;
			while ( Counter-- )
			{
				MatrixCounter = p_stObject->p_SKN_Objectponderation->NumberPdrtLists;
				while ( MatrixCounter-- )
					SavedPonderation[ MatrixCounter ] += p_stObject->p_SKN_Objectponderation->pp_PdrtLst[ MatrixCounter ]->p_PdrtVrc_E[ Counter ].f_Ponderation;
			}
		}
		/* Sort result */
		MatrixCounter = p_stObject->p_SKN_Objectponderation->NumberPdrtLists;
		for ( MatrixCounter2 = 3; MatrixCounter2 < 6; MatrixCounter2++ )
		{
			p_6Values[ MatrixCounter2 ] = SavedPonderation[ p_6Gizmo[ MatrixCounter2 ] ] / ( float ) SelectionNum;
		}

		for ( MatrixCounter2 = 0; MatrixCounter2 < 3; MatrixCounter2++ )
		{
			for ( MatrixCounter = MatrixCounter2 + 1; MatrixCounter < p_stObject->p_SKN_Objectponderation->NumberPdrtLists; MatrixCounter++ )
			{
				float fSwap;
				ULONG ulSWp;
				if ( SavedPonderation[ MatrixCounter ] > SavedPonderation[ MatrixCounter2 ] )
				{
					fSwap                                     = SavedPonderation[ MatrixCounter2 ];
					SavedPonderation[ MatrixCounter2 ]        = SavedPonderation[ MatrixCounter ];
					SavedPonderation[ MatrixCounter ]         = fSwap;
					ulSWp                                     = SavedPonderationIndexes[ MatrixCounter2 ];
					SavedPonderationIndexes[ MatrixCounter2 ] = SavedPonderationIndexes[ MatrixCounter ];
					SavedPonderationIndexes[ MatrixCounter ]  = ulSWp;
				}
			}
			p_6Gizmo[ MatrixCounter2 ]  = SavedPonderationIndexes[ MatrixCounter2 ];
			p_6Values[ MatrixCounter2 ] = SavedPonderation[ MatrixCounter2 ] / ( float ) SelectionNum;
		}

		return SelectionNum;
	}
	/*$4
********************************************************************************************************************************************************************************
Update skin selection..
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_UpdateSkinSelection( GEO_tdst_Object *p_stObject )
	{
		if ( !GEO_SKN_IsSkinned( p_stObject ) ) return;
		if ( p_stObject->p_SKN_Objectponderation->SelectionCLBK )
		{
			if ( p_stObject->p_SKN_Objectponderation->ClassPtr )
			{
				p_stObject->p_SKN_Objectponderation->SelectionCLBK( p_stObject, p_stObject->p_SKN_Objectponderation->ClassPtr );
			}
		}
	}
#endif /* ACTIVE_EDITORS */

	/*$4
********************************************************************************************************************************************************************************
Duplicate ponderation - Work on compressed or expanded VP
********************************************************************************************************************************************************************************
*/

	GEO_tdst_VertexPonderationList *GEO_SKN_DuplicateList( GEO_tdst_VertexPonderationList *p_VPL )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_VertexPonderationList *p_RtrnValye;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( p_VPL == NULL ) return NULL;
		p_RtrnValye              = ( GEO_tdst_VertexPonderationList              *) MEM_p_Alloc( sizeof( GEO_tdst_VertexPonderationList ) );
		*p_RtrnValye             = *p_VPL;
		p_RtrnValye->p_PdrtVrc_E = ( GEO_tdst_ExpandedVertexPonderation * ) MEM_p_AllocAlign( sizeof( GEO_tdst_ExpandedVertexPonderation ) * ( p_VPL->us_NumberOfPonderatedVertices + 4 ), 16 );
		L_memcpy(
		        ( void * ) p_RtrnValye->p_PdrtVrc_E,
		        p_VPL->p_PdrtVrc_E,
		        sizeof( GEO_tdst_ExpandedVertexPonderation ) * ( p_VPL->us_NumberOfPonderatedVertices + 4 ) );
		return p_RtrnValye;
	}

	GEO_tdst_ObjectPonderation *GEO_SKN_Duplicate( GEO_tdst_ObjectPonderation *p_OSP )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ul_MatrixCounter;
		GEO_tdst_ObjectPonderation *p_OSPRet;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		p_OSPRet  = ( GEO_tdst_ObjectPonderation  *) MEM_p_Alloc( sizeof( GEO_tdst_ObjectPonderation ) );
		*p_OSPRet = *p_OSP;


#ifdef ACTIVE_EDITORS
		p_OSPRet->PushStack       = NULL;
		p_OSPRet->REDOStack       = NULL;
		p_OSPRet->dul_PointColors = NULL;
#endif// ACTIVE_EDITORS
		ul_MatrixCounter = p_OSPRet->NumberPdrtLists;
		if ( ul_MatrixCounter )
		{
			p_OSPRet->pp_PdrtLst = ( GEO_tdst_VertexPonderationList ** ) MEM_p_Alloc( p_OSP->NumberPdrtLists * sizeof( GEO_tdst_VertexPonderationList * ) );
			L_memcpy(
			        p_OSPRet->pp_PdrtLst,
			        p_OSP->pp_PdrtLst,
			        sizeof( GEO_tdst_VertexPonderationList * ) * p_OSP->NumberPdrtLists );
			while ( ul_MatrixCounter-- )
			{
				p_OSPRet->pp_PdrtLst[ ul_MatrixCounter ] = GEO_SKN_DuplicateList( p_OSP->pp_PdrtLst[ ul_MatrixCounter ] );
			}
		}

		return ( p_OSPRet );
	}

	/*$4
********************************************************************************************************************************************************************************
Destroy ponderation
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_DestroyObjPonderation( GEO_tdst_ObjectPonderation *p_ObjPond )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ul_MatrixCounter;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		if ( !p_ObjPond ) return;

#ifdef ACTIVE_EDITORS

		if ( p_ObjPond->PushStack )
			GEO_SKN_DestroyObjPonderation( p_ObjPond->PushStack );

		if ( p_ObjPond->REDOStack )
			GEO_SKN_DestroyObjPonderation( p_ObjPond->REDOStack );

		if ( p_ObjPond->dul_PointColors )
			MEM_Free( p_ObjPond->dul_PointColors );

#endif// ACTIVE_EDITORS

		ul_MatrixCounter = p_ObjPond->NumberPdrtLists;
		if ( ul_MatrixCounter )
		{
			while ( ul_MatrixCounter-- )
			{
				if ( p_ObjPond->pp_PdrtLst[ ul_MatrixCounter ] )
				{
					MEM_FreeAlign( p_ObjPond->pp_PdrtLst[ ul_MatrixCounter ]->p_PdrtVrc_C );
					MEM_Free( p_ObjPond->pp_PdrtLst[ ul_MatrixCounter ] );
				}
			}
			MEM_Free( p_ObjPond->pp_PdrtLst );
		}

		MEM_Free( p_ObjPond );
	}
#ifdef ACTIVE_EDITORS
	/*$4
********************************************************************************************************************************************************************************
Flash or restore the specifief matrix - WORK ON COMPRESSED OR EXPANDED SKIN
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_RestoreGizmo( OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *pst_Object, ULONG ulNum2Flhs )
	{
		ULONG ul_MatrixCounter;
		MATH_tdst_Matrix st_Matrix;
		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( !( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix ) ) return;
		if ( _pst_GO->pst_Base->pst_AddMatrix->l_Number <= ( LONG ) ulNum2Flhs ) return;

		ul_MatrixCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ul_MatrixCounter-- )
		{
			if ( ulNum2Flhs == pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->us_IndexOfMatrix )
			{
				if ( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer )
				{
					if ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ulNum2Flhs ].pst_GO )
					{
						OBJ_ComputeGlobalWhenHie( _pst_GO );
						OBJ_ComputeGlobalWhenHie( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ulNum2Flhs ].pst_GO );
						MATH_InvertMatrix( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ulNum2Flhs ].pst_GO->pst_GlobalMatrix, &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->st_FlashedMatrix );
						MATH_MulMatrixMatrix( &st_Matrix, _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ulNum2Flhs ].pst_GO->pst_GlobalMatrix, _pst_GO->pst_GlobalMatrix );
						if ( !( _pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Symetric ) )
							GEO_SKN_SymetriseMatrix( &st_Matrix );
						MATH_CopyMatrix( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ulNum2Flhs ].pst_GO->pst_GlobalMatrix, &st_Matrix );
					}
				}
				else
				{
					MATH_InvertMatrix( &_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ ulNum2Flhs ].st_Matrix, &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->st_FlashedMatrix );
					if ( !( _pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Symetric ) )
						GEO_SKN_SymetriseMatrix( &_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ ulNum2Flhs ].st_Matrix );
				}
			}
		}
		/* Restore hierachical matrix if needed */
		ul_MatrixCounter = _pst_GO->pst_Base->pst_AddMatrix->l_Number;
		if ( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer )
			while ( ul_MatrixCounter-- )
			{
				if ( ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixCounter ].l_MatrixId == 0xffffffff ) || ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixCounter ].pst_GO && ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixCounter ].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer ) ) )
					OBJ_ComputeLocalWhenHie( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixCounter ].pst_GO );
			}
	}
	void GEO_SKN_FlashGizmo( OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *pst_Object, ULONG ulNum2Flhs )
	{
		ULONG ul_MatrixCounter;
		MATH_tdst_Matrix st_Matrix2, st_Matrix3;
		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( !( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix ) ) return;
		if ( _pst_GO->pst_Base->pst_AddMatrix->l_Number <= ( LONG ) ulNum2Flhs ) return;

		ul_MatrixCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ul_MatrixCounter-- )
		{
			if ( ulNum2Flhs == pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->us_IndexOfMatrix )
			{
				if ( ( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer ) && ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ulNum2Flhs ].pst_GO ) )
				{
					OBJ_tdst_GameObject *_pst_GizmoGO;
					OBJ_ComputeGlobalWhenHie( _pst_GO );
					OBJ_ComputeGlobalWhenHie( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ulNum2Flhs ].pst_GO );
					_pst_GizmoGO = _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ulNum2Flhs ].pst_GO;
					if ( ( _pst_GizmoGO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject ) && ( _pst_GizmoGO->pst_Extended ) )
					{
						MDF_tdst_Modifier *pst_Modifier;
						extern void GAO_ModifierBoneRefineApply( MDF_tdst_Modifier * _pst_Mod, GEO_tdst_Object * _pst_Obj );
						pst_Modifier = _pst_GizmoGO->pst_Extended->pst_Modifiers;
						while ( pst_Modifier )
						{
							if ( pst_Modifier->i->ul_Type == MDF_C_Modifier_Half_Angle )
							{
								if ( ( !( pst_Modifier->ul_Flags & ( MDF_C_Modifier_Inactive | MDF_C_Modifier_NoApply ) ) ) &&
								     ( pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyGao ) )
								{
									GAO_ModifierBoneRefineApply( pst_Modifier, NULL );
								}
							}
							pst_Modifier = pst_Modifier->pst_Next;
						}
					}
					MATH_InvertMatrix( &st_Matrix2, _pst_GO->pst_GlobalMatrix );
					MATH_MulMatrixMatrix( &st_Matrix3, _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ulNum2Flhs ].pst_GO->pst_GlobalMatrix, &st_Matrix2 );
					MATH_InvertMatrix( &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->st_FlashedMatrix, &st_Matrix3 );
				}
				else
					MATH_InvertMatrix( &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->st_FlashedMatrix, &_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ ulNum2Flhs ].st_Matrix );

				if ( !( _pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Symetric ) )
					GEO_SKN_SymetriseMatrix( &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->st_FlashedMatrix );
			}
		}
	}
	void GEO_SKN_R_180_Gizmo( OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *pst_Object, ULONG ulNum2Flhs )
	{
		ULONG ul_MatrixCounter;
		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;

		ul_MatrixCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ul_MatrixCounter-- )
		{
			if ( ulNum2Flhs == pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->us_IndexOfMatrix )
			{
				GEO_SKN_180Matrix( &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->st_FlashedMatrix );
			}
		}
	}

	/*$4
********************************************************************************************************************************************************************************
Symetrise X
PondList must be expand, All matrixes must be flashed
********************************************************************************************************************************************************************************
*/
	ULONG *p_VertexSymetrics;
	ULONG *p_MatrixSymetrics;
	u32 GEO_SKN_GetnumberOfFathers( OBJ_tdst_GameObject *_pst_GO, u32 ul_MatrixNum )
	{
		u32 RetVal;
		RetVal = 0;
		if ( ( u32 ) _pst_GO->pst_Base->pst_AddMatrix->l_Number > ul_MatrixNum )
		{
			if ( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer )
			{
				if ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixNum ].pst_GO && ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixNum ].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer ) )
				{
					_pst_GO = _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixNum ].pst_GO;
					while ( _pst_GO && ( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy ) && ( _pst_GO->pst_Base->pst_Hierarchy ) )
					{
						_pst_GO = _pst_GO->pst_Base->pst_Hierarchy->pst_Father;
						RetVal++;
					}
				}
			}
		}
		return RetVal;
	}

	void GEO_SKN_Symetrise_X_Vertex( GEO_tdst_Object *pst_Object, ULONG Index, ULONG X, float Y )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		ULONG IndexSource;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		IndexSource = p_VertexSymetrics[ Index ];
		if ( IndexSource == Index ) return;

		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			ULONG MatrixSym;
			MatrixSym = p_MatrixSymetrics[ ulNumberOfMatrix ];
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation =
			        pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ MatrixSym ]->p_PdrtVrc_E[ IndexSource ].f_Ponderation;
		}
	}
	void GEO_SKN_Symetrise_X( OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *pst_Object, ULONG ulFlag )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		float SqrDist;
		LONG C1, C2, ulNumberOfMatrix;
		MATH_tdst_Vector Local1, Local2;
		MATH_tdst_Matrix InvertM ONLY_PSX2_ALIGNED( 16 );
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( !GEO_SKN_IsExpanded( pst_Object ) ) return;
		p_VertexSymetrics = ( unsigned long * ) MEM_p_Alloc( pst_Object->l_NbPoints * 4 );
		p_MatrixSymetrics = ( unsigned long * ) MEM_p_Alloc( pst_Object->p_SKN_Objectponderation->NumberPdrtLists * 4 );

		/* Compute Symteric matrix */
		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			MATH_InvertMatrix( &InvertM, &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->st_FlashedMatrix );
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->st_FlashedMatrix = InvertM;
		}
		for ( C1 = 0; C1 < pst_Object->p_SKN_Objectponderation->NumberPdrtLists; C1++ )
		{
			u32 NumberOfFathersA, NumberOfFathersB;
			NumberOfFathersA = GEO_SKN_GetnumberOfFathers( _pst_GO, pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ C1 ]->us_IndexOfMatrix );
			SqrDist          = 1000000000000000000000000000000000000.0f;
			MATH_AddVector( &Local1, &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ C1 ]->st_FlashedMatrix.T, MATH_pst_GetZAxis( &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ C1 ]->st_FlashedMatrix ) );
			Local1.x                = -Local1.x;
			p_MatrixSymetrics[ C1 ] = 0;
			for ( C2 = 0; C2 < pst_Object->p_SKN_Objectponderation->NumberPdrtLists; C2++ )
			{
				float LDist;
				NumberOfFathersB = GEO_SKN_GetnumberOfFathers( _pst_GO, pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ C2 ]->us_IndexOfMatrix );
				MATH_AddVector( &Local2, &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ C2 ]->st_FlashedMatrix.T, MATH_pst_GetZAxis( &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ C2 ]->st_FlashedMatrix ) );
				MATH_SubVector( &Local2, &Local1, &Local2 );
				LDist = MATH_f_SqrVector( &Local2 );
				if ( ( LDist < SqrDist ) && ( NumberOfFathersA == NumberOfFathersB ) )
				{
					SqrDist                 = LDist;
					p_MatrixSymetrics[ C1 ] = C2;
				}
			}
		}
		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			MATH_InvertMatrix( &InvertM, &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->st_FlashedMatrix );
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->st_FlashedMatrix = InvertM;
		}

		/* Compute Symterix Vertex ! time is N^2 */
		for ( C1 = 0; C1 < pst_Object->l_NbPoints; C1++ )
		{
			SqrDist                 = 1000000000000000000000000000000000000.0f;
			Local1                  = pst_Object->dst_Point[ C1 ];
			Local1.x                = -Local1.x;
			p_VertexSymetrics[ C1 ] = 0;
			for ( C2 = 0; C2 < pst_Object->l_NbPoints; C2++ )
			{
				float LDist;
				MATH_SubVector( &Local2, &Local1, &pst_Object->dst_Point[ C2 ] );
				LDist = MATH_f_SqrVector( &Local2 );
				if ( LDist < SqrDist )
				{
					SqrDist                 = LDist;
					p_VertexSymetrics[ C1 ] = C2;
				}
			}
		}

		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_Symetrise_X_Vertex, 0, 0.0f, ulFlag );
		MEM_Free( p_VertexSymetrics );
		MEM_Free( p_MatrixSymetrics );
	}
	/*$4
********************************************************************************************************************************************************************************
Deduct proxy
PondList must be expand, All matrixes must be flashed
********************************************************************************************************************************************************************************
*/
	void GEO_SKN_DeductProxy_Vertex( GEO_tdst_Object *pst_Object, ULONG Index, ULONG X, float Y )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		MATH_tdst_Vector Local;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		/* Reset if not Add */
		while ( ulNumberOfMatrix-- )
		{
			float Dist;
			MATH_SubVector( &Local, VCast( &pst_Object->dst_Point[ Index ] ), &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->st_FlashedMatrix.T );
			if ( X )
			{
				Local.x = ( -pst_Object->dst_Point[ Index ].x ) - pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->st_FlashedMatrix.T.x;
			}//*/
			Dist = MATH_f_SqrVector( &Local );
			if ( Dist != 0.0f ) Dist = 1.0f / Dist;
			else
				Dist = 10000000000000000000000000000.0f;
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->p_PdrtVrc_E[ Index ].f_Ponderation = Dist;
		}
	}
	void GEO_SKN_Deduct_Proxy( OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *pst_Object, ULONG ulFlag )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ulNumberOfMatrix;
		ULONG InvertX;
		MATH_tdst_Matrix InvertM ONLY_PSX2_ALIGNED( 16 );
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;
		if ( !GEO_SKN_IsExpanded( pst_Object ) ) return;


		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			MATH_InvertMatrix( &InvertM, &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->st_FlashedMatrix );
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->st_FlashedMatrix = InvertM;
		}
		InvertX = 0;
		if ( !( _pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Symetric ) )
			InvertX = 1;
		GEO_SKN_ForEachPointDo( pst_Object, GEO_SKN_DeductProxy_Vertex, InvertX, 0.0f, ulFlag );
		ulNumberOfMatrix = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ulNumberOfMatrix-- )
		{
			MATH_InvertMatrix( &InvertM, &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->st_FlashedMatrix );
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ulNumberOfMatrix ]->st_FlashedMatrix = InvertM;
		}
		GEO_SKN_ForceNMatrixMax( pst_Object, 1, ulFlag );
		GEO_SKN_Normalize( pst_Object, ulFlag );
	}
#endif// ACTIVE_EDITORS
/*$4
********************************************************************************************************************************************************************************
Compute Mesh deformation for display
********************************************************************************************************************************************************************************
*/
#include "BASe/BENch/BENch.h"

	unsigned int GEO_SKN_OneOnTwoOptimization = 0;

#if defined( OPT_COMPUTENORMALS )

	extern void GEO_SKN_ComputeNormals_OPT( GEO_tdst_Object *_pst_Object, GEO_Vertex *pst_Point );
	void GEO_SKN_ComputeNormals( GEO_tdst_Object *_pst_Object, GEO_Vertex *pst_Point )
	{
		static bool UseOptNormal = true;

		if ( UseOptNormal )
		{
			GEO_SKN_ComputeNormals_OPT( _pst_Object, pst_Point );
		}
	}

	void GEO_SKN_ComputeNormals_OPT( GEO_tdst_Object *_pst_Object, GEO_Vertex *pst_Point )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		//    GEO_Vertex                          *pst_Pt[3];
		//    MATH_tdst_Vector                  *pst_Normal, *pst_LastNormal;
		_Custom_Normal_ *pst_Normal, *pst_LastNormal;
		//	__declspec(align(16)) MATH_tdst_Vector st_TriangleNormal, st_Vect1, st_Vect2;
		GEO_tdst_ElementIndexedTriangles *pst_Element, *pst_LastElement;
		GEO_tdst_IndexedTriangle *pst_Triangle, *pst_LastTriangle, *pst_NextTriangle;
		int NormalsArraySize = 0;
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#	ifdef OPT_COMPUTENORMALS
		static bool bNormalize   = true;
		static bool bZeroNormals = true;
		static bool bCalcNormal  = true;

		__m128 MinusOne;
		__m128 One_Or_MinusOne;
		if ( bFirstTime )
		{
			memcpy( ( void * ) &_MASK_0FFF_, &_Mask.i[ 0 ], sizeof( __m128 ) );
			memcpy( ( void * ) &_MASK_F000_, &_Mask.w[ 0 ], sizeof( __m128 ) );
			memcpy( ( void * ) &_MINUSONE_, &_Mask.f[ 0 ], sizeof( __m128 ) );
			memcpy( ( void * ) &_ONE_, &_Mask.one[ 0 ], sizeof( __m128 ) );
			memcpy( ( void * ) &_ZEROS_, &_Mask.z[ 0 ], sizeof( __m128 ) );
			memcpy( ( void * ) &_ZEROS64_, &_Mask.z[ 0 ], sizeof( __m64 ) );
			bFirstTime = false;

			//change rounding mode
			if ( _MM_GET_ROUNDING_MODE() == _MM_ROUND_NEAREST )
			{
				/* Rounding mode is round toward zero */

				//set to TOWARD_ZERO for precision problems on XBOX!
				_MM_SET_ROUNDING_MODE( _MM_ROUND_TOWARD_ZERO );
			}
		}
#	endif// OPT_COMPUTENORMALS

		if ( !_pst_Object->l_NbPoints ) return;


		_GSP_BeginRaster( 2 );

		GEO_UseNormals( _pst_Object );

		if ( _pst_Object->dst_OriginalPointNormal ) return;

		pst_Normal     = ( _Custom_Normal_     *) _pst_Object->dst_PointNormal;//Cast necessary for the trick
		pst_LastNormal = pst_Normal + _pst_Object->l_NbPoints;

		//it's the first time in the normal initialization

		//Alloc the array of normals
		NormalsArraySize                     = ( ( int ) ( pst_LastNormal ) - ( int ) pst_Normal );
		_pst_Object->dst_OriginalPointNormal = MEM_p_AllocAlign( NormalsArraySize, 16 );

		if ( bZeroNormals )
		{
			//	L_memset(pst_Normal , 0 , sizeof(MATH_tdst_Vector) * _pst_Object->l_NbPoints);

			__m64 *pst_TmpNormal;
			__m64 *pst_TmpNormalLast;
			__m64 ZeroVect;


			int iSize     = ( int ) pst_LastNormal - ( int ) pst_Normal;
			int iPad      = ( int ) pst_Normal & 0x7;
			int iHalfQuad = iSize >> 3;
			int iRest     = iSize - iHalfQuad * 8 + iPad;
			pst_TmpNormal = ( __m64 * ) pst_Normal;

			pst_TmpNormalLast = pst_TmpNormal + ( iHalfQuad & 0xfffffff8 );
			ZeroVect          = _mm_xor_si64( ZeroVect, ZeroVect );
			for ( ; ( pst_TmpNormal ) != pst_TmpNormalLast; pst_TmpNormal += 8 )
			{
				_mm_prefetch( ( const char * ) pst_TmpNormal + 4, _MM_HINT_NTA );
				_mm_stream_pi( pst_TmpNormal + 0, ZeroVect );
				_mm_stream_pi( pst_TmpNormal + 1, ZeroVect );
				_mm_stream_pi( pst_TmpNormal + 2, ZeroVect );
				_mm_stream_pi( pst_TmpNormal + 3, ZeroVect );
				_mm_prefetch( ( const char * ) pst_TmpNormal + 8, _MM_HINT_NTA );
				_mm_stream_pi( pst_TmpNormal + 4, ZeroVect );
				_mm_stream_pi( pst_TmpNormal + 5, ZeroVect );
				_mm_stream_pi( pst_TmpNormal + 6, ZeroVect );
				_mm_stream_pi( pst_TmpNormal + 7, ZeroVect );
			}


			_mm_empty();

			L_memset( pst_TmpNormal, 0, ( ( int ) pst_LastNormal - ( int ) pst_TmpNormal ) );

			//Precache loop
			{
				/*			typedef struct _4FLOAT
		{
		float x,y,z,w;
		}__declspec(align(16))_4FloatAlign;

		_4FloatAlign	*SVect = pst_Point;
		__m128	TmpVect;
		int NumPoints = _pst_Object->l_NbPoints >> 2;
		while(NumPoints--)
		{
		_mm_prefetch(SVect+1 , _MM_HINT_NTA );
		TmpVect = _mm_load_ps(SVect);
		_mm_stream_ps(SVect++,TmpVect);
		}*/
			}
		}

		//-------------------------------------------------------------------------------------------
		//Now compute the normals
		//-------------------------------------------------------------------------------------------
		if ( !( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric ) )
		{
			One_Or_MinusOne = _mm_load_ps( ( float * ) &_MINUSONE_ );
		}
		else
		{
			One_Or_MinusOne = _mm_load_ps( ( float * ) &_ONE_ );
		}

		MinusOne = _mm_load_ps( ( float * ) &_MINUSONE_ );

		pst_Element     = _pst_Object->dst_Element;
		pst_LastElement = pst_Element + _pst_Object->l_NbElements;
		if ( bCalcNormal )
		{
			__m128 Pt0;
			__m128 Pt1;
			__m128 Pt2;
			__m128 Vect2;
			__m128 Vect1;
			__m128 m1;
			__m128 m2;
			__m128 Normal;
			__m128 Normal0;
			__m128 Normal1;
			__m128 Normal2;
			__m128 Shuffle1;
			__m128 Shuffle2;

			for ( ; pst_Element < pst_LastElement; pst_Element++ )
			{
				bool bFlip;
				_Custom_Normal_ *pst_NormalPt0;
				_Custom_Normal_ *pst_NormalPt1;
				_Custom_Normal_ *pst_NormalPt2;

				_mm_prefetch( ( const char * ) ( pst_Element + 1 ), _MM_HINT_NTA );


				if ( pst_Element->pst_StripData )
				{
					ULONG stripCounter;

					for ( stripCounter = 0; stripCounter < pst_Element->pst_StripData->ulStripNumber; stripCounter++ )
					{
						unsigned short idx0, idx1, idx2;
						GEO_tdst_OneStrip *pOneStrip            = &pst_Element->pst_StripData->pStripList[ stripCounter ];
						GEO_tdst_MinVertexData *pVertexData     = pOneStrip->pMinVertexDataList;
						GEO_tdst_MinVertexData *pLastVertexData = pVertexData + ( pOneStrip->ulVertexNumber );

						bFlip = false;
						//Precalc the first triangle
						//					while(pVertexData != pLastVertexData)
						{
							idx0 = pVertexData->auw_Index;
							pVertexData++;
							idx1 = pVertexData->auw_Index;
							pVertexData++;
							idx2 = pVertexData->auw_Index;
							pVertexData++;

							//						_mm_prefetch((const char *)(pVertexData)+0x30 , _MM_HINT_NTA );

							Pt0 = _mm_loadu_ps( ( float * ) ( pst_Point + idx0 ) );
							Pt1 = _mm_loadu_ps( ( float * ) ( pst_Point + idx1 ) );
							Pt2 = _mm_loadu_ps( ( float * ) ( pst_Point + idx2 ) );

							Vect1 = _mm_sub_ps( Pt1, Pt0 );
							Vect2 = _mm_sub_ps( Pt1, Pt2 );


							Vect1 = _mm_mul_ss( Vect1, One_Or_MinusOne );
							Vect2 = _mm_mul_ss( Vect2, One_Or_MinusOne );


							//Cross prod
							m2     = _mm_mul_ps( _mm_shuffle_ps( Vect1, Vect1, _MM_SHUFFLE( 3, 0, 2, 1 ) ), _mm_shuffle_ps( Vect2, Vect2, _MM_SHUFFLE( 3, 1, 0, 2 ) ) );
							m1     = _mm_mul_ps( _mm_shuffle_ps( Vect1, Vect1, _MM_SHUFFLE( 3, 1, 0, 2 ) ), _mm_shuffle_ps( Vect2, Vect2, _MM_SHUFFLE( 3, 0, 2, 1 ) ) );
							Normal = _mm_sub_ps( m1, m2 );
							//Cross prod end

							pst_NormalPt0 = pst_Normal + idx0;
							pst_NormalPt1 = pst_Normal + idx1;
							pst_NormalPt2 = pst_Normal + idx2;

							_mm_stream_ps( ( float * ) pst_NormalPt0, _mm_add_ps( _mm_load_ps( ( float * ) pst_NormalPt0 ), Normal ) );
							Normal0 = _mm_add_ps( _mm_load_ps( ( float * ) pst_NormalPt1 ), Normal );
							Normal1 = _mm_add_ps( _mm_load_ps( ( float * ) pst_NormalPt2 ), Normal );

							//						Vect1 = _mm_sub_ps( Pt2 , Pt1 );
							Vect1 = _mm_mul_ps( Vect2, MinusOne );
							Pt1   = _mm_shuffle_ps( Pt2, Pt2, _MM_SHUFFLE( 3, 2, 1, 0 ) );

							Shuffle1 = _mm_shuffle_ps( Vect1, Vect1, _MM_SHUFFLE( 3, 0, 2, 1 ) );
							Shuffle2 = _mm_shuffle_ps( Vect1, Vect1, _MM_SHUFFLE( 3, 1, 0, 2 ) );

							pst_NormalPt0 = pst_NormalPt1;
							pst_NormalPt1 = pst_NormalPt2;
						}


						while ( pVertexData < pLastVertexData )
						{
							idx2 = pVertexData->auw_Index;
							Pt2  = _mm_loadu_ps( ( float  *) ( pst_Point + idx2 ) );
							_mm_prefetch( ( ( const char * ) ( pVertexData ) ) + 32, _MM_HINT_NTA );
							pVertexData++;

							pst_NormalPt2 = pst_Normal + idx2;
							_mm_prefetch( ( ( const char * ) pst_NormalPt2 ) + 32, _MM_HINT_NTA );


							//Sub to find triangle side vector
							Vect2 = _mm_sub_ps( Pt1, Pt2 );
							//Mul for simmetry
							Vect2 = _mm_mul_ss( Vect2, One_Or_MinusOne );

							//Cross prod. Use previous shuffled values
							m2 = _mm_mul_ps( Shuffle1, _mm_shuffle_ps( Vect2, Vect2, _MM_SHUFFLE( 3, 1, 0, 2 ) ) );
							m1 = _mm_mul_ps( Shuffle2, _mm_shuffle_ps( Vect2, Vect2, _MM_SHUFFLE( 3, 0, 2, 1 ) ) );
							//Cross prod end

							if ( bFlip )
							{
								Normal = _mm_sub_ps( m1, m2 );
							}
							else
								Normal = _mm_sub_ps( m2, m1 );


							_mm_stream_ps( ( float * ) pst_NormalPt0, _mm_add_ps( Normal0, Normal ) );
							Normal0 = _mm_add_ps( Normal1, Normal );
							Normal1 = _mm_add_ps( _mm_load_ps( ( float * ) pst_NormalPt2 ), Normal );


							//						Vect1 = _mm_sub_ps( Pt2 , Pt1 );
							Vect1    = _mm_mul_ps( Vect2, MinusOne );
							Shuffle1 = _mm_shuffle_ps( Vect1, Vect1, _MM_SHUFFLE( 3, 0, 2, 1 ) );
							Shuffle2 = _mm_shuffle_ps( Vect1, Vect1, _MM_SHUFFLE( 3, 1, 0, 2 ) );
							Pt1      = Pt2;


							pst_NormalPt0 = pst_NormalPt1;
							pst_NormalPt1 = pst_NormalPt2;

							bFlip = !bFlip;
						}

						_mm_stream_ps( ( float * ) pst_NormalPt0, Normal0 );
						_mm_stream_ps( ( float * ) pst_NormalPt1, Normal1 );
					}
				}
				else
				{
					//Process triangles
					pst_NextTriangle = pst_Triangle = pst_Element->dst_Triangle;
					pst_LastTriangle                = pst_Triangle + pst_Element->l_NbTriangles;

					//The next triangle
					pst_NextTriangle += 1;
					for ( ; pst_Triangle < pst_LastTriangle; pst_Triangle++, pst_NextTriangle++ )
					{
						__m128 Pt0;
						__m128 Pt1;
						__m128 Pt2;
						__m128 Vect1;
						__m128 Vect2;
						__m128 m1;
						__m128 m2;
						__m128 Normal;

						unsigned short idx0 = pst_Triangle->auw_Index[ 0 ];
						unsigned short idx1 = pst_Triangle->auw_Index[ 1 ];
						unsigned short idx2 = pst_Triangle->auw_Index[ 2 ];
						//				__declspec(align(16)) MATH_tdst_Vector *pst_NormalPt[4];
						_Custom_Normal_ *pst_NormalPt[ 4 ];

						_mm_prefetch( ( const char * ) pst_NextTriangle, _MM_HINT_NTA );

						Pt0 = _mm_loadu_ps( ( float * ) ( pst_Point + idx0 ) );
						//				Pt0 = _mm_and_ps(_mm_loadu_ps((pst_Point + idx0)),_MASK_0FFF_);
						Pt1 = _mm_loadu_ps( ( float * ) ( pst_Point + idx1 ) );
						//				Pt1 = _mm_and_ps(_mm_loadu_ps((pst_Point + idx1)),_MASK_0FFF_);
						Pt2 = _mm_loadu_ps( ( float * ) ( pst_Point + idx2 ) );
						//				Pt2 = _mm_and_ps(_mm_loadu_ps((pst_Point + idx2)),_MASK_0FFF_);

						Vect1 = _mm_sub_ps( Pt1, Pt0 );
						Vect2 = _mm_sub_ps( Pt1, Pt2 );


						/*			if (!(GDI_gpst_CurDD ->ul_CurrentDrawMask & GDI_Cul_DM_Symetric))
				{
				Vect1 = _mm_mul_ss(Vect1,_MINUSONE_);
				Vect2 = _mm_mul_ss(Vect2,_MINUSONE_);
				}*/
						Vect1 = _mm_mul_ss( Vect1, One_Or_MinusOne );
						Vect2 = _mm_mul_ss( Vect2, One_Or_MinusOne );


						/*Dest->x = fMul(A->y, B->z) - fMul(A->z, B->y);
				Dest->y = fMul(A->z, B->x) - fMul(A->x, B->z);
				Dest->z = fMul(A->x, B->y) - fMul(A->y, B->x);*/

						//Cross prod
						m2     = _mm_mul_ps( _mm_shuffle_ps( Vect1, Vect1, _MM_SHUFFLE( 3, 0, 2, 1 ) ), _mm_shuffle_ps( Vect2, Vect2, _MM_SHUFFLE( 3, 1, 0, 2 ) ) );
						m1     = _mm_mul_ps( _mm_shuffle_ps( Vect1, Vect1, _MM_SHUFFLE( 3, 1, 0, 2 ) ), _mm_shuffle_ps( Vect2, Vect2, _MM_SHUFFLE( 3, 0, 2, 1 ) ) );
						Normal = _mm_sub_ps( m1, m2 );
						//Cross prod end


						pst_NormalPt[ 0 ] = pst_Normal + idx0;
						pst_NormalPt[ 1 ] = pst_Normal + idx1;
						pst_NormalPt[ 2 ] = pst_Normal + idx2;


						//Use aligned normals
						//			_mm_store_ps(&pst_NormalPt[0],_mm_add_ps(_mm_load_ps(pst_NormalPt[0]) , Normal ));
						//			_mm_store_ps(&pst_NormalPt[1],_mm_add_ps(_mm_load_ps(pst_NormalPt[1]) , Normal ));
						//			_mm_store_ps(&pst_NormalPt[2],_mm_add_ps(_mm_load_ps(pst_NormalPt[2]) , Normal ));

						//Use NOT aligned normals

						//					Result =  _mm_add_ps(_mm_load_ps(pst_NormalPt[0]) , Normal );
						_mm_stream_ps( ( float * ) &pst_NormalPt[ 0 ]->x, _mm_add_ps( _mm_load_ps( ( float * ) &pst_NormalPt[ 0 ]->x ), Normal ) );

						//					Result =  _mm_add_ps(_mm_load_ps(pst_NormalPt[1]) , Normal );
						_mm_stream_ps( ( float * ) &pst_NormalPt[ 1 ]->x, _mm_add_ps( _mm_load_ps( ( float * ) &pst_NormalPt[ 1 ]->x ), Normal ) );

						//					Result =  _mm_add_ps(_mm_load_ps(pst_NormalPt[2]) , Normal );
						_mm_stream_ps( ( float * ) &pst_NormalPt[ 2 ]->x, _mm_add_ps( _mm_load_ps( ( float * ) &pst_NormalPt[ 2 ]->x ), Normal ) );
					}
				}
			}
		}//	(bCalcNormal)

		//	for(; pst_Normal < pst_LastNormal; pst_Normal++)
		//		MATH_NormalizeVector(pst_Normal, pst_Normal);
		if ( bNormalize )
		{
			__m128 vec;
			__m128 r;
			__m128 t;
			//		MATH_tdst_Vector	*DstN = (MATH_tdst_Vector *)pst_Normal;	//here's the trick. I cast the outout vector to the original...
			//		_Custom_Normal_	*DstN = (_Custom_Normal_ *)pst_Normal;
			//copy to the original buffer
			GEO_Vertex *dstPoints;
			_Custom_Normal_ *DstN         = _pst_Object->dst_OriginalPointNormal;
			MATH_tdst_Vector *DstN_NoSkin = ( MATH_tdst_Vector * ) pst_Normal;//here's the trick. I cast the outout vector to the original...
			dstPoints                     = _pst_Object->dst_Point;
			for ( ; pst_Normal < pst_LastNormal; pst_Normal++, DstN++, DstN_NoSkin++, dstPoints++ )
			{
				if ( dstPoints->x * dstPoints->x < 0.00000625f )
					pst_Normal->x = 0.0f;
				/*
		fInvNorm = MATH_f_InvNormVector(A);
		Dest->x = fMul(A->x, fInvNorm);
		Dest->y = fMul(A->y, fInvNorm);
		Dest->z = fMul(A->z, fInvNorm);
		*/
				//			vec = _mm_and_ps(_mm_load_ps(pst_Normal),_MASK_0FFF_);
				pst_Normal->w = 0.0f;
				vec           = _mm_load_ps( ( float           *) pst_Normal );
				_mm_prefetch( ( ( const char * ) DstN ) + 0x30, _MM_HINT_NTA );

				r   = _mm_mul_ps( vec, vec );
				r   = _mm_add_ps( _mm_movehl_ps( r, r ), r );
				t   = _mm_add_ss( _mm_shuffle_ps( r, r, 1 ), r );
				t   = _mm_rsqrt_ss( t );
				vec = _mm_mul_ps( vec, _mm_shuffle_ps( t, t, 0 ) );

				//non-aligned 4 float
				_mm_storeu_ps( &DstN_NoSkin->x, vec );
				_mm_prefetch( ( ( const char * ) pst_Normal ) + 0x30, _MM_HINT_NTA );
				_mm_stream_ps( &DstN->x, vec );
			}
		}

		_GSP_EndRaster( 2 );
	}
#else// OPT_COMPUTENORMALS
void GEO_SKN_ComputeNormals( GEO_tdst_Object *_pst_Object, GEO_Vertex *pst_Point )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex *pst_Pt[ 3 ], *dstPoints;
	MATH_tdst_Vector *pst_Normal, *pst_LastNormal;
	//u16									*pst_NormalLoc;
	//u32									NbPoints;
	MATH_tdst_Vector st_TriangleNormal, st_Vect1, st_Vect2;
	GEO_tdst_ElementIndexedTriangles *pst_Element, *pst_LastElement;
	GEO_tdst_IndexedTriangle *pst_Triangle, *pst_LastTriangle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !_pst_Object->l_NbPoints ) return;

	_GSP_BeginRaster( 2 );

	GEO_UseNormals( _pst_Object );// OK
	pst_Normal     = _pst_Object->dst_PointNormal;
	pst_LastNormal = pst_Normal + _pst_Object->l_NbPoints;
	L_memset( pst_Normal, 0, sizeof( MATH_tdst_Vector ) * _pst_Object->l_NbPoints );
#	ifdef PSX2_TARGET
	if ( _pst_Object->p_CompressedNormals )
	{
		pst_Element     = _pst_Object->dst_Element;
		pst_LastElement = pst_Element + _pst_Object->l_NbElements;
		for ( ; pst_Element < pst_LastElement; pst_Element++ )
		{
			if ( pst_Element->pst_StripDataPS2 )
			{
				GEO_SKN_ASMComputeNormals_STRIP( pst_Point,
				                                 ( u_long * ) pst_Normal,
				                                 pst_Element->pst_StripDataPS2->pVertexIndexes,
				                                 pst_Element->pst_StripDataPS2->ulNumberOfAtoms, GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric );
			}
			else
			{
				pst_Triangle     = pst_Element->dst_Triangle;
				pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;
				for ( ; pst_Triangle < pst_LastTriangle; pst_Triangle++ )
				{
					pst_Pt[ 0 ] = pst_Point + pst_Triangle->auw_Index[ 0 ];
					pst_Pt[ 1 ] = pst_Point + pst_Triangle->auw_Index[ 1 ];
					pst_Pt[ 2 ] = pst_Point + pst_Triangle->auw_Index[ 2 ];
					MATH_SubVector( &st_Vect1, VCast( pst_Pt[ 0 ] ), VCast( pst_Pt[ 1 ] ) );
					MATH_SubVector( &st_Vect2, VCast( pst_Pt[ 0 ] ), VCast( pst_Pt[ 2 ] ) );
					/*				if (!(GDI_gpst_CurDD ->ul_CurrentDrawMask & GDI_Cul_DM_Symetric))
					{
						st_Vect1.x = -st_Vect1.x;
						st_Vect2.x = -st_Vect2.x;
					}//*/
					MATH_CrossProduct( &st_TriangleNormal, &st_Vect1, &st_Vect2 );
					MATH_AddEqualVector( pst_Normal + pst_Triangle->auw_Index[ 0 ], &st_TriangleNormal );
					MATH_AddEqualVector( pst_Normal + pst_Triangle->auw_Index[ 1 ], &st_TriangleNormal );
					MATH_AddEqualVector( pst_Normal + pst_Triangle->auw_Index[ 2 ], &st_TriangleNormal );
				}
			}
		}
		NbPoints      = _pst_Object->l_NbPoints;
		dstPoints     = _pst_Object->dst_Point;
		pst_NormalLoc = ( u16 * ) pst_Normal;
		while ( NbPoints-- )
		{
			if ( dstPoints->x * dstPoints->x < 0.00000625f )
				*pst_NormalLoc = 0;
			dstPoints++;
			pst_NormalLoc += 4;
		}
		/* Transmutation X0.KZX then integrate it */
		if ( _pst_Object->dst_Element && ( _pst_Object->dst_Element->pst_StripDataPS2 == NULL ) )
		{
			MATH_tdst_Vector *pProut, *pLastProut;
			u_char *plom;
			pProut = pst_Normal;
			plom   = ( u_char   *) pst_Normal;
			while ( pProut < pst_LastNormal )
			{
				*( plom++ ) = *( ( u_char * ) ( &pProut->x ) + 2 );
				*( plom++ ) = *( ( u_char * ) ( &pProut->x ) + 3 );
				*( plom++ ) = *( ( u_char * ) ( &pProut->y ) + 2 );
				*( plom++ ) = *( ( u_char * ) ( &pProut->y ) + 3 );
				*( plom++ ) = *( ( u_char * ) ( &pProut->z ) + 2 );
				*( plom++ ) = *( ( u_char * ) ( &pProut->z ) + 3 );
				*( plom++ ) = 0;
				*( plom++ ) = 0;
				pProut++;
			}
		}
		// Decompress
		L_memset( _pst_Object->p_CompressedNormals, 0, sizeof( ULONG ) * _pst_Object->l_NbPoints );
		*( float * ) _pst_Object->p_CompressedNormals = 120.0f / 128.0f;
		GEO_SKN_SerialNormalize_And_Compress( ( u_long * ) _pst_Object->dst_PointNormal, _pst_Object->p_CompressedNormals, _pst_Object->l_NbPoints );
		{
			unsigned int *p_LastF_D;
			u_short *p_LastF_C;
			unsigned int Counter;
			p_LastF_C = ( u_short * ) pst_Normal;
			p_LastF_D = ( unsigned int * ) pst_Normal;
			Counter   = _pst_Object->l_NbPoints;
			p_LastF_C += Counter * 4;
			p_LastF_D += Counter * 3;
			while ( Counter-- )
			{
				p_LastF_C -= 4;
				p_LastF_D -= 3;
				p_LastF_D[ 2 ] = p_LastF_C[ 2 ] << 16;
				p_LastF_D[ 1 ] = p_LastF_C[ 1 ] << 16;
				p_LastF_D[ 0 ] = p_LastF_C[ 0 ] << 16;
			}
		}//*/
		{
			u32 NormalCounter, *pColor, *pNormal;
			pColor = _pst_Object->dul_PointColors;
			if ( pColor )
			{
				pColor++;
				pNormal       = _pst_Object->p_CompressedNormals;
				NormalCounter = _pst_Object->l_NbPoints;
				while ( NormalCounter-- )
				{
					*pNormal &= 0xffffff;
					*pNormal |= *pColor & 0xff000000;
					pNormal++;
					pColor++;
				}
			}
		}
	}
#	else// PSX2_TARGET

	pst_Element     = _pst_Object->dst_Element;
	pst_LastElement = pst_Element + _pst_Object->l_NbElements;
	for ( ; pst_Element < pst_LastElement; pst_Element++ )
	{
#		ifdef _GAMECUBE
		if ( pst_Element->dl )
		{
			u16 p1, p2, p3;
			BOOL bflip;
			u8 *Stream, *StreamLast;
			u32 Pitch, Mode8;

			Stream     = pst_Element->dl;
			StreamLast = Stream + ( pst_Element->dl_size & 0x0fffffff );
			Pitch      = 8;
			Mode8      = 0;
			if ( pst_Element->dl_size & 0x80000000 )
			{
				Pitch -= 2;
				Mode8 = 1;
			};
			if ( pst_Element->dl_size & 0x40000000 ) Pitch -= 1;
			if ( pst_Element->dl_size & 0x20000000 ) Pitch -= 1;
			while ( Stream < StreamLast )
			{
				{
					u16 lNbVertex;
					u8 *StreamStripLast;

					Stream++;                     // Flags
					lNbVertex = *( u16 * ) Stream;// Number Of Vertexes
					Stream += 2;                  // Number Of Vertexes
					bflip           = FALSE;
					StreamStripLast = Stream + lNbVertex * Pitch;
					if ( Mode8 )
					{
						p1 = ( u32 ) * ( u8 * ) Stream;
						Stream += Pitch;
						p2 = ( u32 ) * ( u8 * ) Stream;
						Stream += Pitch;
					}
					else
					{
						p1 = ( u32 ) * ( u16 * ) Stream;
						Stream += Pitch;
						p2 = ( u32 ) * ( u16 * ) Stream;
						Stream += Pitch;
					}
					if ( p1 > _pst_Object->l_NbPoints )
					{
						u32 BUG;
						BUG = 0;
					}
					if ( p2 > _pst_Object->l_NbPoints )
					{
						u32 BUG;
						BUG = 0;
					}

					while ( Stream < StreamStripLast )// Parse VertexIndexes
					{
						if ( Mode8 ) p3 = ( u32 ) * ( u8 * ) Stream;
						else
							p3 = ( u32 ) * ( u16 * ) Stream;

						if ( p3 > _pst_Object->l_NbPoints )
						{
							u32 BUG;
							BUG = 0;
						}

						pst_Pt[ 0 ] = pst_Point + p1;
						pst_Pt[ 1 ] = pst_Point + p2;
						pst_Pt[ 2 ] = pst_Point + p3;

						MATH_SubVector( &st_Vect1, VCast( pst_Pt[ 0 ] ), VCast( pst_Pt[ 1 ] ) );
						MATH_SubVector( &st_Vect2, VCast( pst_Pt[ 0 ] ), VCast( pst_Pt[ 2 ] ) );

						if ( !( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric ) )
						{
							st_Vect1.x = -st_Vect1.x;
							st_Vect2.x = -st_Vect2.x;
						}

						MATH_CrossProduct( &st_TriangleNormal, &st_Vect1, &st_Vect2 );

						if ( bflip ) MATH_NegVector( &st_TriangleNormal, &st_TriangleNormal );

						MATH_AddEqualVector( pst_Normal + p1, &st_TriangleNormal );
						MATH_AddEqualVector( pst_Normal + p2, &st_TriangleNormal );
						MATH_AddEqualVector( pst_Normal + p3, &st_TriangleNormal );

						p1    = p2;
						p2    = p3;
						bflip = !bflip;

						Stream += Pitch;
					}
					while ( ( *Stream == 0 ) && ( Stream < StreamLast ) ) Stream++;// Skip DX_NOP
				}
			}
		}
		else
#		endif// _GAMECUBE
		{
			pst_Triangle     = pst_Element->dst_Triangle;
			pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

			for ( ; pst_Triangle < pst_LastTriangle; pst_Triangle++ )
			{
				pst_Pt[ 0 ] = pst_Point + pst_Triangle->auw_Index[ 0 ];
				pst_Pt[ 1 ] = pst_Point + pst_Triangle->auw_Index[ 1 ];
				pst_Pt[ 2 ] = pst_Point + pst_Triangle->auw_Index[ 2 ];


				MATH_SubVector( &st_Vect1, VCast( pst_Pt[ 0 ] ), VCast( pst_Pt[ 1 ] ) );
				MATH_SubVector( &st_Vect2, VCast( pst_Pt[ 0 ] ), VCast( pst_Pt[ 2 ] ) );

				if ( !( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric ) )
				{
					st_Vect1.x = -st_Vect1.x;
					st_Vect2.x = -st_Vect2.x;
				}

				MATH_CrossProduct( &st_TriangleNormal, &st_Vect1, &st_Vect2 );

				MATH_AddEqualVector( pst_Normal + pst_Triangle->auw_Index[ 0 ], &st_TriangleNormal );
				MATH_AddEqualVector( pst_Normal + pst_Triangle->auw_Index[ 1 ], &st_TriangleNormal );
				MATH_AddEqualVector( pst_Normal + pst_Triangle->auw_Index[ 2 ], &st_TriangleNormal );
			}
		}
	}

	// Normals for objects that are displayed once with GDI_Cul_DM_Symetric,
	// and once without  GDI_Cul_DM_Symetric,
	// are corrected on seam (small dstPoints->x).
	if ( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric )
		_pst_Object->ulStripFlag |= GEO_C_Strip_UseSymetry;
	else
		_pst_Object->ulStripFlag |= GEO_C_Strip_UseDirect;

	if ( ( _pst_Object->ulStripFlag & GEO_C_Strip_UseSymetry ) &&
	     ( _pst_Object->ulStripFlag & GEO_C_Strip_UseDirect ) )
	{
		dstPoints = _pst_Object->dst_Point;
		for ( ; pst_Normal < pst_LastNormal; pst_Normal++, dstPoints++ )
		{
			// Correction of normal on seam.
			if ( dstPoints->x * dstPoints->x < 0.00000625f )
				pst_Normal->x = 0.0f;
			MATH_NormalizeAnyVector( pst_Normal, pst_Normal );
		}
	}
	else
	{
		dstPoints = _pst_Object->dst_Point;
		for ( ; pst_Normal < pst_LastNormal; pst_Normal++, dstPoints++ )
			MATH_NormalizeAnyVector( pst_Normal, pst_Normal );
	}
#	endif    // #else PSX2_TARGET
	_GSP_EndRaster( 2 );
}
#endif        //OPT_COMPUTENORMALS

#if defined( ACTIVE_EDITORS ) && defined( _XENON_RENDER )

	        void
	        GEO_SKN_ComputeTangents( GEO_tdst_Object *_pst_Geo, GEO_Vertex *_pst_Points, GRO_tdst_Visu *_pst_Visu )
	{
		if ( !_pst_Geo || !_pst_Points || !_pst_Geo->l_NbPoints )
			return;

		ERR_X_Assert( _pst_Visu->l_NbXeElements == _pst_Geo->l_NbElements );

		for ( int iElement = 0; iElement < _pst_Geo->l_NbElements; ++iElement )
		{
			if ( _pst_Visu->p_XeElements[ iElement ].dst_TangentSpace != NULL )
			{
				GEO_CalculateTangentSpaceBasis( _pst_Geo,
				                                iElement,
				                                _pst_Visu->p_XeElements[ iElement ].dst_TangentSpace,
				                                _pst_Points );
			}
		}
	}

#endif


	void GEO_SKN_Compute4Display( OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *pst_Object )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_Vertex *p_Src, *p_Dst;
		ULONG ul_MatrixCounter;
		GEO_tdst_CompressedVertexPonderation *p_stCP;
#if defined( OPT_COMPUTENORMALS )
		__declspec( align( 16 ) ) MATH_tdst_Matrix st_Matrix, *p_UsedMatrix, st_Matrix2;
		__declspec( align( 16 ) ) MATH_tdst_Matrix st_Matrix3;
#	if defined( OPT_COMPUTE4DISPLAY )
		_Custom_Normal_ *pSourceNormals;
		MATH_tdst_Vector *pDestNormals;
		MATH_tdst_Vector *pDstVertex;
		GEO_tdst_CompressedVertexPonderation *p_stCP_Saved;
#	endif
		int gNumZeroWeight = 0;
#else // _XBOX
	MATH_tdst_Matrix st_Matrix, *p_UsedMatrix, st_Matrix2;
	MATH_tdst_Matrix st_Matrix3 ONLY_PSX2_ALIGNED( 16 );
#endif// _XBOX
		LONG ul_MatrixNum;
#if defined( PSX2_TARGET ) || defined( _GAMECUBE )
		MATH_tdst_Matrix st_OGLMatrix ONLY_PSX2_ALIGNED( 16 );
#endif// #if defined(PSX2_TARGET) || defined(_GAMECUBE)

#ifndef _GAMECUBE
		MATH_tdst_Vector stLocal;
		GEO_tdst_CompressedVertexPonderation *p_stCPLst;
#endif  // #ifndef _GAMECUBE
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef OPT_COMPUTE4DISPLAY
		if ( bFirstTime )
		{
			memcpy( ( void * ) &_MASK_0FFF_, &_Mask.i[ 0 ], sizeof( __m128 ) );
			memcpy( ( void * ) &_MINUSONE_, &_Mask.f[ 0 ], sizeof( __m128 ) );
			memcpy( ( void * ) &_ZEROS_, &_Mask.z[ 0 ], sizeof( __m128 ) );
			memcpy( ( void * ) &_ZEROS64_, &_Mask.z[ 0 ], sizeof( __m64 ) );
			bFirstTime = false;
		}
#endif// OPT_COMPUTE4DISPLAY

		if ( !GEO_SKN_IsSkinned( pst_Object ) ) return;

#ifdef ACTIVE_EDITORS
		GEO_SKN_Compress( pst_Object );
#endif// ACTIVE_EDITORS
        //	GDI_gpst_CurDD ->ul_CurrentDrawMask |= GDI_Cul_DM_DontRecomputeNormales;

		if ( !( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ActiveSkin ) ) return;
		/*POPOWARINIG a mettre ??
    if (!(GDI_gpst_CurDD->ul_CurrentDrawMask &  GDI_Cul_DM_ActiveSkin)
#if defined(_XENON_RENDER)
        && !GDI_b_IsXenonGraphics()
#endif
       )
        return;
*/
		if ( !( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix ) ) return;

		_GSP_BeginRaster( 2 );

		ul_MatrixCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		if ( GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer )
		{
			p_Dst = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB2;
			p_Src = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
		}
		else
		{
			p_Dst = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
			p_Src = pst_Object->dst_Point;
		}
		L_memset( p_Dst, 0 /* == 0.0f */, sizeof( GEO_Vertex ) * pst_Object->l_NbPoints );

#ifdef PSX2_TARGET
		if ( ( !( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_DontRecomputeNormales ) ) && pst_Object->p_CompressedNormals )
		{
			GEO_UseNormals( pst_Object );// OK

			if ( pst_Object->p_SKN_Objectponderation->FUCKING_ANTIBUG_OF_THE_NORMALS == 1 )
			{
				ULONG SaveDM;
				SaveDM = GDI_gpst_CurDD->ul_CurrentDrawMask;
				GDI_gpst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_Symetric;
				GEO_SKN_ComputeNormals( pst_Object, pst_Object->dst_Point );
				GDI_gpst_CurDD->ul_CurrentDrawMask                                  = SaveDM;
				pst_Object->p_SKN_Objectponderation->FUCKING_ANTIBUG_OF_THE_NORMALS = 0;
			}
			else
				pst_Object->p_SKN_Objectponderation->FUCKING_ANTIBUG_OF_THE_NORMALS--;

			GEO_SKN_SerialConvert_Int2Flt( ( u_long32 * ) pst_Object->p_CompressedNormals, ( u_long64 * ) GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecularField, pst_Object->l_NbPoints );
			L_memset( ( GEO_Vertex * ) GDI_gpst_CurDD->pst_ComputingBuffers->ast_3D, 0, 8 * pst_Object->l_NbPoints );
		}//*/
#endif   // PSX2_TARGET

#if defined( OPT_COMPUTE4DISPLAY )
		pSourceNormals = pst_Object->dst_OriginalPointNormal;
		if ( !pSourceNormals )//next time...
			return;

		pDestNormals = pst_Object->dst_PointNormal;
		L_memset( pDestNormals, 0, ( sizeof( MATH_tdst_Vector ) + 4 ) * pst_Object->l_NbPoints );
#endif// OPT_COMPUTE4DISPLAY
		MATH_InvertMatrix( &st_Matrix3, _pst_GO->pst_GlobalMatrix );

#ifdef JADEFUSION
#	if defined( _XENON_RENDER )

		BOOL bSoftSkinning = pst_Object->b_ForceSoftSkinning;

		if ( !bSoftSkinning
#		if defined( ACTIVE_EDITORS )
		     && ( ( GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer ) == 0 )
#		endif
		)
		{
			g_pXeContextManagerEngine->BeginSkinning();
		}
#	endif

		for ( ul_MatrixCounter = 0; ul_MatrixCounter < pst_Object->p_SKN_Objectponderation->NumberPdrtLists; ul_MatrixCounter++ )
#else
	while ( ul_MatrixCounter-- )
#endif
		{
			/* P_Matrix; */
			{
				ULONG NumberOFPonds;
				NumberOFPonds = ( ULONG ) pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->us_NumberOfPonderatedVertices;
				p_stCP        = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->p_PdrtVrc_C;

#if defined( OPT_COMPUTE4DISPLAY )
				p_stCP_Saved = p_stCP;
#endif// OPT_COMPUTE4DISPLAY

				if ( GEO_MRM_ul_IsMrmObject( pst_Object ) )
				{
					while ( NumberOFPonds && ( p_stCP[ NumberOFPonds - 1 ].Index > pst_Object->l_NbPoints ) ) NumberOFPonds--;
				};// Resolve MRM Exccedent of points;
				if ( NumberOFPonds )
				{
					ul_MatrixNum = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->us_IndexOfMatrix;
					if ( _pst_GO->pst_Base->pst_AddMatrix->l_Number > ul_MatrixNum )
					{
						if ( _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer )
						{
							p_UsedMatrix = _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixNum ].pst_Matrix;
							if ( ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixNum ].l_MatrixId == ( LONG ) 0xffffffff ) || ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixNum ].pst_GO && ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixNum ].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer ) ) )
							{
								if ( _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixNum ].pst_GO )
								{
									OBJ_tdst_GameObject *_pst_GizmoGO;
									_pst_GizmoGO = _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ ul_MatrixNum ].pst_GO;
									if ( ( _pst_GizmoGO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject ) && ( _pst_GizmoGO->pst_Extended ) )
									{
										MDF_tdst_Modifier *pst_Modifier;
										extern void GAO_ModifierLazy_Apply( MDF_tdst_Modifier * _pst_Mod, GEO_tdst_Object * _pst_Obj );
										extern void GAO_ModifierBoneRefineApply( MDF_tdst_Modifier * _pst_Mod, GEO_tdst_Object * _pst_Obj );
										pst_Modifier = _pst_GizmoGO->pst_Extended->pst_Modifiers;
										while ( pst_Modifier )
										{
											if ( pst_Modifier->i->ul_Type == MDF_C_Modifier_Lazy )
											{
												if ( ( !( pst_Modifier->ul_Flags & ( MDF_C_Modifier_Inactive | MDF_C_Modifier_NoApply ) ) ) &&
												     ( pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyGao ) )
												{
													GAO_ModifierLazy_Apply( pst_Modifier, NULL );
												}
											}
											else if ( pst_Modifier->i->ul_Type == MDF_C_Modifier_Half_Angle )
											{
												if ( ( !( pst_Modifier->ul_Flags & ( MDF_C_Modifier_Inactive | MDF_C_Modifier_NoApply ) ) ) &&
												     ( pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyGao ) )
												{
													GAO_ModifierBoneRefineApply( pst_Modifier, NULL );
												}
											}
											pst_Modifier = pst_Modifier->pst_Next;
										}
									}
									p_UsedMatrix = _pst_GizmoGO->pst_GlobalMatrix;
									MATH_MulMatrixMatrix( &st_Matrix2, p_UsedMatrix, &st_Matrix3 );
								}
								else
								{
									MATH_SetIdentityMatrix( &st_Matrix2 );
								}
								p_UsedMatrix = &st_Matrix2;
							}
						}
						else
							p_UsedMatrix = &_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ ul_MatrixNum ].st_Matrix;
						if ( p_UsedMatrix )
						{
							if ( _pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Symetric )
								MATH_MulMatrixMatrix( &st_Matrix, &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->st_FlashedMatrix, p_UsedMatrix );
							else
								GEO_SKN_MulMatrixMatrixSym( &st_Matrix, &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_MatrixCounter ]->st_FlashedMatrix, p_UsedMatrix );
						}
						else
							MATH_SetIdentityMatrix( &st_Matrix ); /*Why not ?*/
					}
					else
						MATH_SetIdentityMatrix( &st_Matrix ); /*Why not ?*/
#ifdef _GAMECUBE
					MATH_MakeOGLMatrix( &st_OGLMatrix, &st_Matrix );
					GC_MATH_SerialSkin( p_Dst, p_Src, &st_OGLMatrix, NumberOFPonds, p_stCP );
#else// _GAMECUBE
#	ifdef PSX2_SKN_ASSEMBLY
				if ( NumberOFPonds >= 4 )
				{
					MATH_MakeOGLMatrix( &st_OGLMatrix, &st_Matrix );
					asm_MATH_SerialSkin( p_Dst, p_Src, &st_OGLMatrix, NumberOFPonds, p_stCP );
					/* Normals transformation */
#		ifdef PSX2_TARGET
					if ( !( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_DontRecomputeNormales ) )
						asm_MATH_SerialSkin_NRM( ( GEO_Vertex * ) GDI_gpst_CurDD->pst_ComputingBuffers->ast_3D, ( GEO_Vertex * ) GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecularField, &st_OGLMatrix, NumberOFPonds, p_stCP );//*/
#		endif// PSX2_TARGET
				}
				else
#	endif    // PSX2_SKN_ASSEMBLY
				{
#	if defined( OPT_COMPUTE4DISPLAY )
					static bool bUpdateSKN = true;
					__m128 Ixyz;
					__m128 Jxyz;
					__m128 Kxyz;
					__m128 Vx;
					__m128 Vy;
					__m128 Vz;
					__m128 VSrcxyz;
					__m128 VSrcxxxx;
					__m128 VSrcyyyy;
					__m128 VSrczzzz;
					__m128 Wxyzw;
					__m128 Txyzw;
					__m128 Dxyz;
					__m128 Vxyz;
					__m128 VDst;

					__m128 Ixyz_NS;
					__m128 Jxyz_NS;
					__m128 Kxyz_NS;

					//Prepare aligned matrix for SSE.
					//Note the new matrix will be:
					//	Ix, Jx, Kx, 1
					//	Iy, Jy, Ky, 1
					//	Iz, Jz, Kz, 1
					//	tx	ty	tz	1
					if ( MATH_b_TestScaleType( &st_Matrix ) )
					{
						/*			MD.M00 = st_Matrix.Ix*st_Matrix.Sx;	MD.M01 = st_Matrix.Iy*st_Matrix.Sx;	MD.M02 = st_Matrix.Iz*st_Matrix.Sx;	MD.M03 = 0.0f;
								MD.M10 = st_Matrix.Jx*st_Matrix.Sy;	MD.M11 = st_Matrix.Jy*st_Matrix.Sy;	MD.M12 = st_Matrix.Jz*st_Matrix.Sy;	MD.M13 = 0.0f;
								MD.M20 = st_Matrix.Kx*st_Matrix.Sz;	MD.M21 = st_Matrix.Ky*st_Matrix.Sz;	MD.M22 = st_Matrix.Kz*st_Matrix.Sz;	MD.M23 = 0.0f;
								MD.M30 = st_Matrix.T.x;				MD.M31 = st_Matrix.T.y;				MD.M32 = st_Matrix.T.z;				MD.M33 = 0.0f;*/
						__m128 Sxxxx;
						__m128 Syyyy;
						__m128 Szzzz;

						Sxxxx = _mm_load_ps1( &st_Matrix.Sx );
						//#ifdef _DEBUG
						//			Ixyz  = _mm_and_ps(_mm_loadu_ps(&st_Matrix.Ix),_MASK_0FFF_);	//load and clear w component
						//#else
						Ixyz = _mm_and_ps( _mm_load_ps( &st_Matrix.Ix ), _MASK_0FFF_ );//load and clear w component
						//#endif
						Ixyz = _mm_mul_ps( Ixyz, Sxxxx );

						Syyyy = _mm_load_ps1( &st_Matrix.Sy );
						//#ifdef _DEBUG
						//			Jxyz  = _mm_and_ps(_mm_loadu_ps(&st_Matrix.Jx),_MASK_0FFF_);
						//#else
						Jxyz = _mm_and_ps( _mm_load_ps( &st_Matrix.Jx ), _MASK_0FFF_ );
						//#endif
						Jxyz = _mm_mul_ps( Jxyz, Syyyy );

						Szzzz = _mm_load_ps1( &st_Matrix.Sz );
						//#ifdef _DEBUG
						//			Kxyz = _mm_and_ps(_mm_loadu_ps(&st_Matrix.Kx),_MASK_0FFF_);
						//#else
						Kxyz = _mm_and_ps( _mm_load_ps( &st_Matrix.Kx ), _MASK_0FFF_ );
						//#endif
						Kxyz = _mm_mul_ps( Kxyz, Szzzz );

						//#ifdef _DEBUG
						//			Txyzw = _mm_and_ps(_mm_loadu_ps(&st_Matrix.T.x),_MASK_0FFF_);
						//#else
						Txyzw = _mm_and_ps( _mm_load_ps( &st_Matrix.T.x ), _MASK_0FFF_ );
						//#endif
					}
					else
					{
						/*MD.M00 = st_Matrix.Ix;	MD.M01 = st_Matrix.Iy;	MD.M02 = st_Matrix.Iz;	MD.M03 = 0.0f;
								MD.M10 = st_Matrix.Jx;	MD.M11 = st_Matrix.Jy;	MD.M12 = st_Matrix.Jz;	MD.M13 = 0.0f;
								MD.M20 = st_Matrix.Kx;	MD.M21 = st_Matrix.Ky;	MD.M22 = st_Matrix.Kz;	MD.M23 = 0.0f;
								MD.M30 = st_Matrix.T.x;	MD.M31 = st_Matrix.T.y;				MD.M32 = st_Matrix.T.z;				MD.M33 = 1.0f;*/
						//#if defined(_DEBUG)
						//			Ixyz  = _mm_and_ps(_mm_loadu_ps(&st_Matrix.Ix),_MASK_0FFF_);	//load and clear w component
						//			Jxyz  = _mm_and_ps(_mm_loadu_ps(&st_Matrix.Jx),_MASK_0FFF_);
						//			Kxyz = _mm_and_ps(_mm_loadu_ps(&st_Matrix.Kx),_MASK_0FFF_);
						//			Txyzw = _mm_and_ps(_mm_loadu_ps(&st_Matrix.T.x),_MASK_0FFF_);
						//#else
						Ixyz  = _mm_and_ps( _mm_load_ps( &st_Matrix.Ix ), _MASK_0FFF_ );//load and clear w component
						Jxyz  = _mm_and_ps( _mm_load_ps( &st_Matrix.Jx ), _MASK_0FFF_ );
						Kxyz  = _mm_and_ps( _mm_load_ps( &st_Matrix.Kx ), _MASK_0FFF_ );
						Txyzw = _mm_and_ps( _mm_load_ps( &st_Matrix.T.x ), _MASK_0FFF_ );
						//#endif
					}

					Ixyz_NS = _mm_and_ps( _mm_load_ps( &st_Matrix.Ix ), _MASK_0FFF_ );//load and clear w component
					Jxyz_NS = _mm_and_ps( _mm_load_ps( &st_Matrix.Jx ), _MASK_0FFF_ );
					Kxyz_NS = _mm_and_ps( _mm_load_ps( &st_Matrix.Kx ), _MASK_0FFF_ );

					//		Ixyz =  _mm_load_ps(&MD.M00);
					//		Jxyz =  _mm_load_ps(&MD.M10);
					//		Kxyz =  _mm_load_ps(&MD.M20);
					//		Txyzw = _mm_load_ps(&MD.M30);


					if ( bUpdateSKN )
					{
						p_stCPLst = p_stCP + NumberOFPonds;
						while ( p_stCP < p_stCPLst )
						{
							//				int Mask = (*(int *)p_stCP) & 0xffff0000;
							//				int *pMask = &Mask;
							//				float fff = *(float *)pMask;
							//				if(fff>0.05f)
							{
								pDstVertex = ( p_Dst + p_stCP->Index );
								_mm_prefetch( ( const char * ) ( p_stCP + 1 ), _MM_HINT_NTA );

								//Fast "vector x matrix" transformation, plus vertex ponderation
								//Thanks to SSE ;)
								//
								//	Original math function
								//	VDst->x = fAdd3(fMul(M->Ix, VSrc->x), fMul(M->Jx, VSrc->y), fMul(M->Kx, VSrc->z));
								//	VDst->y = fAdd3(fMul(M->Iy, VSrc->x), fMul(M->Jy, VSrc->y), fMul(M->Ky, VSrc->z));
								//	VDst->z = fAdd3(fMul(M->Iz, VSrc->x), fMul(M->Jz, VSrc->y), fMul(M->Kz, VSrc->z));


								//Load the source vector
								VSrcxyz = _mm_loadu_ps( &( p_Src + p_stCP->Index )->x );

								//Create the VSrc.xxxx
								VSrcxxxx = _mm_shuffle_ps( VSrcxyz, VSrcxyz, _MM_SHUFFLE( 0, 0, 0, 0 ) );
								Vx       = _mm_mul_ps( Ixyz, VSrcxxxx );

								//Create the VSrc.yyyy
								VSrcyyyy = _mm_shuffle_ps( VSrcxyz, VSrcxyz, _MM_SHUFFLE( 1, 1, 1, 1 ) );
								Vy       = _mm_mul_ps( Jxyz, VSrcyyyy );

								//Create the VSrc.zzzz
								VSrczzzz = _mm_shuffle_ps( VSrcxyz, VSrcxyz, _MM_SHUFFLE( 2, 2, 2, 2 ) );
								Vz       = _mm_mul_ps( Kxyz, VSrczzzz );

								//load the weight
								Wxyzw = _mm_load_ps1( &( *( float * ) p_stCP ) );

								//load the destination vector
								Dxyz = _mm_loadu_ps( ( float * ) pDstVertex );

								//Sum the 3 columns
								Vxyz = _mm_add_ps( Vx, Vy );
								Vxyz = _mm_add_ps( Vxyz, Vz );
								//Sum the translation
								VDst = _mm_add_ps( Vxyz, Txyzw );

								//Do vertex ponderation
								VDst = _mm_mul_ps( VDst, Wxyzw );

								//Add to the final vertex
								Dxyz = _mm_add_ps( Dxyz, VDst );

								//Store to the final location
								_mm_store_ss( &pDstVertex->x, Dxyz );
								_mm_store_ss( &pDstVertex->y, _mm_shuffle_ps( Dxyz, Dxyz, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
								_mm_store_ss( &pDstVertex->z, _mm_shuffle_ps( Dxyz, Dxyz, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );
							}
							p_stCP++;
						}//*/


						////////////////////////////////////////////////////////////////////////////////
						//Now skin the normals
						////////////////////////////////////////////////////////////////////////////////
						p_stCP    = p_stCP_Saved;
						p_stCPLst = p_stCP + NumberOFPonds;
						while ( p_stCP < p_stCPLst )
						{
							//				int Mask = (*(int *)p_stCP) & 0xffff0000;
							//				int *pMask = &Mask;
							//				float fff = *(float *)pMask;
							//				if(fff>0.05f)
							{
								pDstVertex = ( pDestNormals + p_stCP->Index );
								_mm_prefetch( ( const char * ) ( p_stCP + 1 ), _MM_HINT_NTA );

								//Load the source vector
								VSrcxyz = _mm_load_ps( &( pSourceNormals + p_stCP->Index )->x );

								//Create the VSrc.xxxx
								VSrcxxxx = _mm_shuffle_ps( VSrcxyz, VSrcxyz, _MM_SHUFFLE( 0, 0, 0, 0 ) );
								Vx       = _mm_mul_ps( Ixyz_NS, VSrcxxxx );

								//Create the VSrc.yyyy
								VSrcyyyy = _mm_shuffle_ps( VSrcxyz, VSrcxyz, _MM_SHUFFLE( 1, 1, 1, 1 ) );
								Vy       = _mm_mul_ps( Jxyz_NS, VSrcyyyy );

								//Create the VSrc.zzzz
								VSrczzzz = _mm_shuffle_ps( VSrcxyz, VSrcxyz, _MM_SHUFFLE( 2, 2, 2, 2 ) );
								Vz       = _mm_mul_ps( Kxyz_NS, VSrczzzz );

								//load the weight
								Wxyzw = _mm_load_ps1( &( *( float * ) p_stCP ) );

								//load the destination vector
								Dxyz = _mm_loadu_ps( ( float * ) pDstVertex );

								//Sum the 3 columns
								Vxyz = _mm_add_ps( Vx, Vy );
								VDst = _mm_add_ps( Vxyz, Vz );

								//Do vertex ponderation
								VDst = _mm_mul_ps( VDst, Wxyzw );

								//Add to the final vertex
								Dxyz = _mm_add_ps( Dxyz, VDst );

								//Store to the final location
								_mm_store_ss( &pDstVertex->x, Dxyz );
								_mm_store_ss( &pDstVertex->y, _mm_shuffle_ps( Dxyz, Dxyz, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
								_mm_store_ss( &pDstVertex->z, _mm_shuffle_ps( Dxyz, Dxyz, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );
							}

							p_stCP++;
						}
					}
#	elif defined( _XENON_RENDER )//popowarning
					if (
					        !bSoftSkinning && GDI_b_IsXenonGraphics() && !Xe_IsForcingSWSkinning()
#		if defined( ACTIVE_EDITORS )
					        && ( ( GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer ) == 0 )
#		endif
					)
					{
						MATH_tdst_Matrix mtx;
						MATH_MakeOGLMatrix( &mtx, &st_Matrix );
						g_pXeContextManagerEngine->PushSkinningMatrix( ( D3DXMATRIX * ) &mtx );
					}
					else
					{
						p_stCPLst = p_stCP + NumberOFPonds;
						while ( p_stCP < p_stCPLst )
						{

							float Ponderation;
							Ponderation = *( float * ) p_stCP;
							*( ULONG * ) &Ponderation &= 0xffff0000;
							MATH_TransformVertex( &stLocal, &st_Matrix, VCast( p_Src + p_stCP->Index ) );
							( p_Dst + p_stCP->Index )->x += stLocal.x * Ponderation;
							( p_Dst + p_stCP->Index )->y += stLocal.y * Ponderation;
							( p_Dst + p_stCP->Index )->z += stLocal.z * Ponderation;
							p_stCP++;
						}
					}
#	else
					{
						p_stCPLst = p_stCP + NumberOFPonds;
						while ( p_stCP < p_stCPLst )
						{

							float Ponderation;
							Ponderation = *( float * ) p_stCP;
							*( ULONG * ) &Ponderation &= 0xffff0000;
							MATH_TransformVertex( &stLocal, &st_Matrix, VCast( p_Src + p_stCP->Index ) );
							( p_Dst + p_stCP->Index )->x += stLocal.x * Ponderation;
							( p_Dst + p_stCP->Index )->y += stLocal.y * Ponderation;
							( p_Dst + p_stCP->Index )->z += stLocal.z * Ponderation;
							p_stCP++;
						}
					}
#	endif//OPT_COMPUTE4DISPLAY
				}
#endif    // #else _GAMECUBE
				}
			}
		}
#ifdef JADEFUSION
#	if defined( _XENON_RENDER )
		if (
		        bSoftSkinning || !GDI_b_IsXenonGraphics() || Xe_IsForcingSWSkinning()
#		if defined( ACTIVE_EDITORS )
		        || ( ( GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer ) != 0 )
#		endif
		)
		{

			if ( GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer )
				L_memcpy( GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB, GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB2, sizeof( GEO_Vertex ) * pst_Object->l_NbPoints );
			GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseSpecialVertexBuffer;
			GDI_gpst_CurDD->p_Current_Vertex_List = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
		}
		else
		{
			g_pXeContextManagerEngine->EndSkinning();
		}
#	endif
#else
	if ( GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer )
		L_memcpy( GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB, GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB2, sizeof( GEO_Vertex ) * pst_Object->l_NbPoints );
	GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseSpecialVertexBuffer;
	GDI_gpst_CurDD->p_Current_Vertex_List = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
#endif

#ifdef PSX2_TARGET
		if ( !( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_DontRecomputeNormales ) )
		{
			extern void *p_OriginalPtr_CN;
			p_OriginalPtr_CN                = pst_Object->p_CompressedNormals;
			pst_Object->p_CompressedNormals = ( ULONG * ) GDI_gpst_CurDD->pst_ComputingBuffers->ast_3D;
			GEO_SKN_SerialConvert_Flt2Int( ( u_long64 * ) GDI_gpst_CurDD->pst_ComputingBuffers->ast_3D, pst_Object->l_NbPoints );
		}
#endif// PSX2_TARGET
		_GSP_EndRaster( 2 );
	}


#define Cache_Granularity_Po2 2// 4 pts
#define Cache_Granularity     ( 1 << ( Cache_Granularity_Po2 ) )
#define Cache_Size            128

#define SKN_CO_Get( pCE, Pos )   ( pCE[ Pos >> ( 3 + Cache_Granularity_Po2 ) ] & ( 1 << ( ( Pos >> Cache_Granularity_Po2 ) & 7 ) ) )
#define SKN_CO_Set( pCE, Pos )   pCE[ Pos >> ( 3 + Cache_Granularity_Po2 ) ] |= ( 1 << ( ( Pos >> Cache_Granularity_Po2 ) & 7 ) )
#define SKN_CO_ReSet( pCE, Pos ) pCE[ Pos >> ( 3 + Cache_Granularity_Po2 ) ] &= ~( 1 << ( ( Pos >> Cache_Granularity_Po2 ) & 7 ) )
	unsigned int SKN_CO_GetDiff( unsigned char *p1T, unsigned char *p2, unsigned int Number )
	{
		unsigned int ReturnValue;
		ReturnValue = 0;
		while ( Number-- )
		{
			if ( *p1T )
			{
				unsigned char R;
				R = ( *p1T | *p2 ) ^ *p2;// Only p1 not in P2
				while ( R )
				{
					ReturnValue += R & 1;
					R >>= 1;
				}
			}
			p1T++;
			p2++;
		}
		return ReturnValue;
	}

	void GEO_SKN_Optimize4Cache( GEO_tdst_Object *pst_Object )
	{
		/*#if 0
			#define FAsT_t 500
			unsigned char t_CacheEmulator_T[FAsT_t];
			unsigned char t_CacheEmulator_R[FAsT_t];
			unsigned char *pul_CacheEmulator_T;
			unsigned char *pul_CacheEmulator_R;
			u_int	FIFO[Cache_Size];
			u_int	CachePos;
			u_int 	lSize;

			if(!GEO_SKN_IsSkinned(pst_Object)) return;	
			if (!pst_Object->l_NbPoints) return;

			lSize = (pst_Object->l_NbPoints + Cache_Granularity) >> Cache_Granularity_Po2;
			lSize += 8;
			lSize >>= 3; // == Number of char
			if (lSize > FAsT_t)
			{
			pul_CacheEmulator_T = MEM_p_Alloc(lSize);
			pul_CacheEmulator_R = MEM_p_Alloc(lSize);
			} else
			{
			pul_CacheEmulator_T = t_CacheEmulator_T;
			pul_CacheEmulator_R = t_CacheEmulator_R;
			}
			L_memset(pul_CacheEmulator_R , 0 , lSize);
			L_memset(FIFO , 0xff , sizeof(FIFO));

			CachePos = 0;

			{
			u_int ul_MatrixCounter1,ul_MatrixCounter2;
			u_int LowerFounded,LowerFoundedValue;
			GEO_tdst_CompressedVertexPonderation *p_stCP,*p_stCPLst;

			for (ul_MatrixCounter1 = 0 ; ul_MatrixCounter1 < pst_Object->p_SKN_Objectponderation->NumberPdrtLists - 1 ; ul_MatrixCounter1++)
			{
			L_memset(pul_CacheEmulator_T , 0 , lSize);
			LowerFounded = ul_MatrixCounter1;
			LowerFoundedValue = 1000000;
			for (ul_MatrixCounter2 = ul_MatrixCounter1 ; ul_MatrixCounter2 < pst_Object->p_SKN_Objectponderation->NumberPdrtLists ; ul_MatrixCounter2++)
			{
			u_int Diff;
			p_stCP = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ul_MatrixCounter2]->p_PdrtVrc_C;
			p_stCPLst = p_stCP + pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ul_MatrixCounter2]->us_NumberOfPonderatedVertices;
			while(p_stCP < p_stCPLst)
			{
			SKN_CO_Set(pul_CacheEmulator_T,p_stCP->Index);
			p_stCP++;
			}
			Diff = SKN_CO_GetDiff(pul_CacheEmulator_T,pul_CacheEmulator_R,lSize);
			if (Diff < LowerFoundedValue)
			{
			LowerFounded = ul_MatrixCounter2;
			LowerFoundedValue = Diff;
			}
			}
			// Swap the 2 lists ul_MatrixCounter1 <=> ul_MatrixCounter2 
			{
			GEO_tdst_VertexPonderationList *p_Swap;
			p_Swap = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ul_MatrixCounter1];
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ul_MatrixCounter1] = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[LowerFounded];
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[LowerFounded] = p_Swap;
			}
			p_stCP = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ul_MatrixCounter1]->p_PdrtVrc_C;
			p_stCPLst = p_stCP + pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ul_MatrixCounter1]->us_NumberOfPonderatedVertices;

			// Add New vertex in FIFO 
			while(p_stCP < p_stCPLst)
			{
			if (!(SKN_CO_Get(pul_CacheEmulator_R,p_stCP->Index)))
			{
			if (FIFO[CachePos] != -1) 
			{
			SKN_CO_ReSet(pul_CacheEmulator_R,FIFO[CachePos]);
			}
			SKN_CO_Set(pul_CacheEmulator_R,p_stCP->Index);
			FIFO[CachePos] = p_stCP->Index;
			CachePos++;
			if (CachePos == Cache_Size) CachePos = 0;
			}
			p_stCP++;
			}
			}

			}

			if (lSize > FAsT_t)
			{
			MEM_Free(pul_CacheEmulator_R);
			MEM_Free(pul_CacheEmulator_T);
			}
			#endif	*/
	}

	/*$4
		********************************************************************************************************************************************************************************
		Load from buffer. Saved skin is alway's compressed.
		********************************************************************************************************************************************************************************
		*/
	ULONG GEO_SKN_Load( char *OrigBuf, GEO_tdst_Object *pst_Object )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_ObjectPonderation *p_OP;
		char *Buffer;
		ULONG lstCounter, VrtxCounter, *pul_DPtr;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		Buffer = OrigBuf;
		p_OP   = ( GEO_tdst_ObjectPonderation   *) MEM_p_Alloc( sizeof( GEO_tdst_ObjectPonderation ) );
		LOA_ul_FileTypeSize[ 26 ] += sizeof( GEO_tdst_ObjectPonderation );

		p_OP->flags = LOA_ReadUShort( &Buffer );

#ifdef ACTIVE_EDITORS
		p_OP->PushStack       = NULL;
		p_OP->REDOStack       = NULL;
		p_OP->SelectionCLBK   = NULL;
		p_OP->ClassPtr        = NULL;
		p_OP->dul_PointColors = NULL;
#endif// ACTIVE_EDITORS

		lstCounter = p_OP->NumberPdrtLists = LOA_ReadUShort( &Buffer );
		if ( !p_OP->NumberPdrtLists )
		{
#if defined( XML_CONV_TOOL )
			pst_Object->p_SKN_Objectponderation = p_OP;
#else
		MEM_Free( p_OP );
#endif
			return 0;
		}

		p_OP->pp_PdrtLst = ( GEO_tdst_VertexPonderationList ** ) MEM_p_Alloc( sizeof( GEO_tdst_VertexPonderationList * ) * p_OP->NumberPdrtLists );
		LOA_ul_FileTypeSize[ 26 ] += sizeof( GEO_tdst_VertexPonderationList * ) * p_OP->NumberPdrtLists;
		while ( lstCounter-- )
		{
#if defined( _XBOX ) || defined( _XENON )
			p_OP->pp_PdrtLst[ lstCounter ] = ( GEO_tdst_VertexPonderationList * ) MEM_p_AllocAlign( sizeof( GEO_tdst_VertexPonderationList ), 16 );
#else // _XBOX
		p_OP->pp_PdrtLst[ lstCounter ] = ( GEO_tdst_VertexPonderationList * ) MEM_p_Alloc( sizeof( GEO_tdst_VertexPonderationList ) );
#endif// _XBOX
			LOA_ul_FileTypeSize[ 26 ] += sizeof( GEO_tdst_VertexPonderationList );
			p_OP->pp_PdrtLst[ lstCounter ]->us_IndexOfMatrix = LOA_ReadUShort( &Buffer );
			VrtxCounter = p_OP->pp_PdrtLst[ lstCounter ]->us_NumberOfPonderatedVertices = LOA_ReadUShort( &Buffer );
			LOA_ReadMatrix( &Buffer, &p_OP->pp_PdrtLst[ lstCounter ]->st_FlashedMatrix );
			if ( p_OP->pp_PdrtLst[ lstCounter ]->us_NumberOfPonderatedVertices )
			{
				p_OP->pp_PdrtLst[ lstCounter ]->p_PdrtVrc_C = ( GEO_tdst_CompressedVertexPonderation * ) MEM_p_VMAllocAlign( sizeof( GEO_tdst_CompressedVertexPonderation ) * ( VrtxCounter + 4 ), 16 );
				LOA_ul_FileTypeSize[ 26 ] += sizeof( GEO_tdst_CompressedVertexPonderation ) * ( VrtxCounter + 4 );
				pul_DPtr = ( ULONG * ) p_OP->pp_PdrtLst[ lstCounter ]->p_PdrtVrc_C;
				while ( VrtxCounter-- )
				{
					*pul_DPtr = LOA_ReadULong( &Buffer );
					pul_DPtr++;
				}
				VrtxCounter = 4;
				/* Clear the last 4 points */
				while ( VrtxCounter-- ) *( pul_DPtr++ ) = 0;
			}
		}
		pst_Object->p_SKN_Objectponderation = p_OP;
		GEO_SKN_Optimize4Cache( pst_Object );
		return Buffer - OrigBuf;
	}
#ifdef ACTIVE_EDITORS
	/*$4
		********************************************************************************************************************************************************************************
		Save in buffer. Save skin compressed. Editor only
		********************************************************************************************************************************************************************************
		*/
	void GEO_SKN_Save( GEO_tdst_Object *pst_Object )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG ul_lstCounter, ul_VrtxCntr, *pul_DPtr;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#	if !defined( XML_CONV_TOOL )
		GEO_SKN_Normalize( pst_Object, 0 );
		GEO_SKN_Compress( pst_Object );
#	endif// !defined(XML_CONV_TOOL)
		SAV_Buffer( &pst_Object->p_SKN_Objectponderation->flags, 2 );
		SAV_Buffer( &pst_Object->p_SKN_Objectponderation->NumberPdrtLists, 2 );
		ul_lstCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
		while ( ul_lstCounter-- )
		{
			SAV_Buffer( &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_lstCounter ]->us_IndexOfMatrix, 2 );
			SAV_Buffer( &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_lstCounter ]->us_NumberOfPonderatedVertices, 2 );
			SAV_Buffer( &pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_lstCounter ]->st_FlashedMatrix, sizeof( MATH_tdst_Matrix ) );
			ul_VrtxCntr = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_lstCounter ]->us_NumberOfPonderatedVertices;
			pul_DPtr    = ( ULONG    *) pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ ul_lstCounter ]->p_PdrtVrc_C;
			while ( ul_VrtxCntr-- )
			{
				SAV_Buffer( pul_DPtr, 4 );
				pul_DPtr++;
			}
		}
	}

	/*$4
		********************************************************************************************************************************************************************************
		adaps one skin to another
		********************************************************************************************************************************************************************************
		*/
	void GEO_SKN_AdaptToAnotherSkin( OBJ_tdst_GameObject *_pst_GoDst, OBJ_tdst_GameObject *_pst_GoSrc, GEO_tdst_Object *_pst_Dst, GEO_tdst_Object *_pst_Src, char _c_Sel, MATH_tdst_Matrix *_pst_MDst, MATH_tdst_Matrix *_pst_MSrc )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_ObjectPonderation *SKN_Src, *SKN_Dst;
		ULONG ul_Nearest;
		float delta, f_Cur, f_Dist;
		int i, j, k, l;
		GEO_Vertex *dst_SrcPt, *dst_DstPt;
		GEO_Vertex *pst_Temp, *pst_SrcPt, *pst_DstPt;
		GEO_tdst_VertexPonderationList **pp_SrcPL, **pp_DstPL;
		char ac_Src2Dst[ 256 ];
		unsigned short auw_List[ 256 ];
		char *pc_Sel;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if ( ( SKN_Src = _pst_Src->p_SKN_Objectponderation ) == NULL ) return;
		if ( ( SKN_Dst = _pst_Dst->p_SKN_Objectponderation ) == NULL ) return;

		if ( _pst_MDst && _pst_MSrc )
		{
			pst_Temp = _pst_Src->dst_Point;
			if ( _pst_GoSrc )
			{
				GEO_SKN_Compute4Display( _pst_GoSrc, _pst_Src );
				if ( GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer )
					pst_Temp = GDI_gpst_CurDD->p_Current_Vertex_List;
			}

			dst_SrcPt = ( GEO_Vertex * ) L_malloc( sizeof( GEO_Vertex ) * _pst_Src->l_NbPoints );
			for ( i = 0; i < _pst_Src->l_NbPoints; i++ )
				MATH_TransformVertex( VCast( dst_SrcPt + i ), _pst_MSrc, VCast( pst_Temp + i ) );

			dst_DstPt = ( GEO_Vertex * ) L_malloc( sizeof( GEO_Vertex ) * _pst_Dst->l_NbPoints );
			for ( i = 0; i < _pst_Dst->l_NbPoints; i++ )
				MATH_TransformVertex( VCast( dst_DstPt + i ), _pst_MDst, VCast( _pst_Dst->dst_Point + i ) );
		}
		else
		{
			dst_SrcPt = _pst_Src->dst_Point;
			dst_DstPt = _pst_Dst->dst_Point;
		}

		GEO_SKN_Expand( _pst_Dst );
		GEO_SKN_Expand( _pst_Src );

		for ( i = 0; i < SKN_Src->NumberPdrtLists; i++ )
			auw_List[ i ] = SKN_Src->pp_PdrtLst[ i ]->us_IndexOfMatrix;

		if ( ( _pst_GoDst && _pst_GoDst->pst_Base && _pst_GoDst->pst_Base->pst_AddMatrix ) && ( _pst_GoSrc && _pst_GoSrc->pst_Base && _pst_GoSrc->pst_Base->pst_AddMatrix ) )
		{
			for ( i = 0; i < SKN_Src->NumberPdrtLists; i++ )
			{
				k = SKN_Src->pp_PdrtLst[ i ]->us_IndexOfMatrix;
				k = _pst_GoSrc->pst_Base->pst_AddMatrix->dst_GizmoPtr[ k ].l_MatrixId;

				for ( j = 0; j < _pst_GoDst->pst_Base->pst_AddMatrix->l_Number; j++ )
				{
					l = _pst_GoDst->pst_Base->pst_AddMatrix->dst_GizmoPtr[ j ].l_MatrixId;
					if ( l == k )
					{
						auw_List[ i ] = j;
						break;
					}
				}
			}
		}
		/*#if 0
			if (!_c_Sel)
			{
			GEO_SKN_SetNumberOfMatrix( _pst_Dst, SKN_Src->NumberPdrtLists );

			pp_SrcPL = SKN_Src->pp_PdrtLst;
			pp_DstPL = SKN_Dst->pp_PdrtLst;
			for (j = 0; j < SKN_Src->NumberPdrtLists; j++, pp_SrcPL++, pp_DstPL++)
			{

			(*pp_DstPL)->us_IndexOfMatrix = auw_List[j];
			MATH_CopyMatrix( &(*pp_DstPL)->st_FlashedMatrix, &(*pp_SrcPL)->st_FlashedMatrix);
			}

			pst_DstPt = dst_DstPt;
			for ( i = 0; i < _pst_Dst->l_NbPoints; i++, pst_DstPt++)
			{
			f_Dist = Cf_Infinit;

			pst_SrcPt = dst_SrcPt;
			for ( j = 0; j < _pst_Src->l_NbPoints; j++, pst_SrcPt++)
			{
			delta = pst_SrcPt->x - pst_DstPt->x;
			delta *= delta;
			if (delta < f_Dist)
			{
			f_Cur = pst_SrcPt->y - pst_DstPt->y;
			f_Cur *= f_Cur;
			f_Cur += delta;

			if (delta < f_Dist)
			{
			delta = pst_SrcPt->z - pst_DstPt->z;
			delta *= delta;
			f_Cur += delta;
			if (f_Cur < f_Dist)
			{
			f_Dist = f_Cur;
			ul_Nearest = j;
			}
			}
			}
			}

			pp_SrcPL = SKN_Src->pp_PdrtLst;
			pp_DstPL = SKN_Dst->pp_PdrtLst;
			for (j = 0; j < SKN_Src->NumberPdrtLists; j++, pp_SrcPL++, pp_DstPL++)
			{
			(*pp_DstPL)->p_PdrtVrc_E[i].f_Ponderation = (*pp_SrcPL)->p_PdrtVrc_E[ul_Nearest].f_Ponderation;
			}
			}
			}
			else
			#endif*/
		{
			if ( _pst_Dst->pst_SubObject == NULL )
			{
				if ( _pst_MDst && _pst_MSrc )
				{
					L_free( dst_SrcPt );
					L_free( dst_DstPt );
				}
				return;
			}

			pst_DstPt = dst_DstPt;

			GEO_SKN_SetNumberOfMatrix( _pst_Dst, SKN_Src->NumberPdrtLists );

			pc_Sel = _pst_Dst->pst_SubObject->dc_VSel;
			L_memset( ac_Src2Dst, 0xFF, 256 );
			for ( i = 0; i < _pst_Dst->l_NbPoints; i++, pst_DstPt++, pc_Sel++ )
			{
				if ( _c_Sel && !( *pc_Sel & 1 ) ) continue;

				f_Dist    = Cf_Infinit;
				pst_SrcPt = dst_SrcPt;
				for ( j = 0; j < _pst_Src->l_NbPoints; j++, pst_SrcPt++ )
				{
					delta = pst_SrcPt->x - pst_DstPt->x;
					delta *= delta;
					if ( delta < f_Dist )
					{
						f_Cur = pst_SrcPt->y - pst_DstPt->y;
						f_Cur *= f_Cur;
						f_Cur += delta;

						if ( delta < f_Dist )
						{
							delta = pst_SrcPt->z - pst_DstPt->z;
							delta *= delta;
							f_Cur += delta;
							if ( f_Cur < f_Dist )
							{
								f_Dist     = f_Cur;
								ul_Nearest = j;
							}
						}
					}
				}

				pp_SrcPL = SKN_Src->pp_PdrtLst;
				for ( j = 0; j < SKN_Src->NumberPdrtLists; j++, pp_SrcPL++ )
				{
					if ( ( *pp_SrcPL )->p_PdrtVrc_E[ ul_Nearest ].f_Ponderation == 0 )
						continue;

					if ( ac_Src2Dst[ j ] == -1 )
					{
						pp_DstPL = SKN_Dst->pp_PdrtLst;
						for ( k = 0; k < SKN_Dst->NumberPdrtLists; k++, pp_DstPL++ )
						{
							if ( ( *pp_DstPL )->us_IndexOfMatrix == auw_List[ j ] )
								break;
						}
						ac_Src2Dst[ j ] = k;
						if ( k == SKN_Dst->NumberPdrtLists )
						{
							GEO_SKN_SetNumberOfMatrix( _pst_Dst, SKN_Dst->NumberPdrtLists + 1 );
							pp_DstPL                        = SKN_Dst->pp_PdrtLst + k;
							( *pp_DstPL )->us_IndexOfMatrix = auw_List[ j ];
							MATH_CopyMatrix( &( *pp_DstPL )->st_FlashedMatrix, &( *pp_SrcPL )->st_FlashedMatrix );
						}
					}
				}

				pp_DstPL = SKN_Dst->pp_PdrtLst;
				for ( j = 0; j < SKN_Dst->NumberPdrtLists; j++, pp_DstPL++ )
					( *pp_DstPL )->p_PdrtVrc_E[ i ].f_Ponderation = 0;

				pp_SrcPL = SKN_Src->pp_PdrtLst;
				for ( j = 0; j < SKN_Src->NumberPdrtLists; j++, pp_SrcPL++ )
				{
					if ( ( *pp_SrcPL )->p_PdrtVrc_E[ ul_Nearest ].f_Ponderation == 0 )
						continue;

					pp_DstPL                                      = SKN_Dst->pp_PdrtLst + ac_Src2Dst[ j ];
					( *pp_DstPL )->p_PdrtVrc_E[ i ].f_Ponderation = ( *pp_SrcPL )->p_PdrtVrc_E[ ul_Nearest ].f_Ponderation;
				}
			}
		}

		GEO_SKN_Compress( _pst_Dst );
		GEO_SKN_Compress( _pst_Src );

		if ( _pst_MDst && _pst_MSrc )
		{
			L_free( dst_SrcPt );
			L_free( dst_DstPt );
		}
	}

#endif /* ACTIVE_EDITORS */
#if defined( PSX2_TARGET ) && defined( __cplusplus )
}
#endif