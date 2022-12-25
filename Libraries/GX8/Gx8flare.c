/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include <xtl.h>
#include <D3D8.h>
#include "MATHs/MATHstruct.h"
#include "GDInterface/GDInterface.h"
#include "Gx8/Gx8flare.h"
#include "GFX/GFXflare.h"
#include "Gx8/Gx8.h"
#include "Gx8/Gx8init.h"
#include "Gx8/Gx8renderstate.h"


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_TestSunVisibility(GDI_tdst_DisplayData *_pst_DD, GFX_tdst_Sun *pst_Data)
{
	return;
#if 0
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Gx8_tdst_SpecificData	*pst_SD;
	int					i;
	float				f_Factor[8][2] ={ { 1, 0 }, { -0.3f, 0.7f }, { -0.7f, 0.3f }, { -0.7f, -0.3f }, { -0.3f, -0.7f }, { 0.3f, -0.7f }, { 0.7f, -0.3f }, { 0.7f, 0.3f } };
	DWORD				ColorWrite, ZWrite;
	DWORD				ZCompFunc;
	UINT				result = 0;
	ULONGLONG			timestamp = 0;
	HRESULT				hr;
	MATH_tdst_Matrix	InverseObjectToCamera;
	MATH_tdst_Vector	v, w;
	BOOL				ok = FALSE;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	pst_SD = GX8_M_SD(_pst_DD);

	// reset Hide Flags
    pst_Data->l_HideFlags <<= 16;
	pst_Data->l_Flags &= ~GFX_Flare_Hidden;

	// manage visibility test result (for center)
	hr = IDirect3DDevice8_GetVisibilityTestResult( pst_SD->mp_D3DDevice, pst_Data->FirstAvailableIndex, &result, &timestamp);
	if ( hr == D3D_OK )
	{
		ok = TRUE;
		if (!result)
			pst_Data->l_HideFlags |= 0x100;
	}
	else
	{	// if test result is not ready, consider sun visibility unchanged !
		pst_Data->l_HideFlags &= 0xffff0000;
		pst_Data->l_HideFlags |= (pst_Data->l_HideFlags >> 16);
	}

	// manage visibility test result (for auxiliary points)
	for ( i = 0; i < 8; i++ )
	{
		hr = IDirect3DDevice8_GetVisibilityTestResult( pst_SD->mp_D3DDevice, pst_Data->FirstAvailableIndex + i + 1, &result, &timestamp);
		if ( hr == D3D_OK )
		{
			if (!result)
				pst_Data->l_HideFlags |= 1 << i;
		}
	}

	if (ok)
	{
		//The game is doing 2x multisampling. Down to 1X 
		IDirect3DDevice8_SetRenderState(pst_SD->mp_D3DDevice, D3DRS_MULTISAMPLEMODE, D3DMULTISAMPLEMODE_1X );


		// save current render states
		IDirect3DDevice8_GetRenderState(pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, &ZWrite);
		IDirect3DDevice8_GetRenderState(pst_SD->mp_D3DDevice, D3DRS_COLORWRITEENABLE, &ColorWrite);
		IDirect3DDevice8_GetRenderState(pst_SD->mp_D3DDevice, D3DRS_ZFUNC, &ZCompFunc);

		// disable any write
		IDirect3DDevice8_SetRenderState(pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, FALSE);
		IDirect3DDevice8_SetRenderState(pst_SD->mp_D3DDevice, D3DRS_COLORWRITEENABLE, FALSE);

		// prepare for drawing
		Gx8_vSetVertexShader( D3DFVF_XYZ );

		// draw center point
		IDirect3DDevice8_BeginVisibilityTest( pst_SD->mp_D3DDevice );
			IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_POINTLIST );
				IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, pst_Data->st_Pos.x, pst_Data->st_Pos.y, pst_Data->st_Pos.z, 1.0f );
			IDirect3DDevice8_End( pst_SD->mp_D3DDevice );
		IDirect3DDevice8_EndVisibilityTest( pst_SD->mp_D3DDevice, pst_Data->FirstAvailableIndex + 0 );

		// calculate Inverse of ObjectToCameraMatrix
		MATH_InvertMatrix(&InverseObjectToCamera,_pst_DD->st_Camera.pst_ObjectToCameraMatrix);
		// reset translation (offset do not need translation)
		memset( &InverseObjectToCamera.T, 0, sizeof(MATH_tdst_Vector) );
		
		// compute and draw auxiliary points
		for ( i = 0; i < 8; i++ )
		{
			static float Multiplier = 1.0f;

			hr = IDirect3DDevice8_GetVisibilityTestResult( pst_SD->mp_D3DDevice, pst_Data->FirstAvailableIndex + i + 1, &result, &timestamp);
			if ( hr != D3D_OK )
				continue;

			// compute offset in world coords
			v.x = f_Factor[i][0] * pst_Data->f_Interval / Multiplier;
			v.y = f_Factor[i][1] * pst_Data->f_Interval / Multiplier;
			v.z = 0.0f;
			MATH_TransformVertex(&w, &InverseObjectToCamera, &v);
			// add pos to offset
			w.x += pst_Data->st_Pos.x;
			w.y += pst_Data->st_Pos.y; 
			w.z += pst_Data->st_Pos.z;

			// draw auxiliary point
			IDirect3DDevice8_BeginVisibilityTest( pst_SD->mp_D3DDevice );
				IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_POINTLIST );
					IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, w.x, w.y, w.z, 1.0f );
				IDirect3DDevice8_End( pst_SD->mp_D3DDevice );
			IDirect3DDevice8_EndVisibilityTest( pst_SD->mp_D3DDevice, pst_Data->FirstAvailableIndex + i + 1 );
		}


		// restore saved render states
		IDirect3DDevice8_SetRenderState(pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, ZWrite);
		IDirect3DDevice8_SetRenderState(pst_SD->mp_D3DDevice, D3DRS_COLORWRITEENABLE, ColorWrite);

		//The game is doing 2x multisampling. Down to 1X 
		IDirect3DDevice8_SetRenderState(pst_SD->mp_D3DDevice, D3DRS_MULTISAMPLEMODE, D3DMULTISAMPLEMODE_2X );

	}
/*
	if ( pst_Data->l_HideFlags == 0x01FF01FF )
	{
		pst_Data->l_Flags |= GFX_Flare_Hidden;
	}
*/
#endif

}





