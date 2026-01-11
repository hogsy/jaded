// Dx9matrix.c

#include "d3dx9math.h"

#include "Dx9matrix.h"
#include "GDInterface/GDInterface.h"


/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

////////////
void	Dx9_SetProjectionMatrix( CAM_tdst_Camera *pCamera )
{
	float					f_ScreenRatio;
	float					f;
	ULONG					Flags;
	LONG					w, h, W, H;
	LONG					x, y;
	float					l, r, t, b;
	D3DCLIPSTATUS9			stCS;

	// Compute screen ratio
	if ( ( GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst <= 0 ) || 
		( GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst >= GDI_Cul_SRC_Number ) )
	{
		f_ScreenRatio = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
	}
	else
	{
		f_ScreenRatio = GDI_gaf_ScreenRation[ GDI_gpst_CurDD->st_ScreenFormat.l_ScreenRatioConst ];
	}

	f = pCamera->f_YoverX * GDI_gpst_CurDD->st_ScreenFormat.f_PixelYoverX * f_ScreenRatio;
	Flags = GDI_gpst_CurDD->st_ScreenFormat.ul_Flags;

	/* Compute height and width of screen */
	w = GDI_gpst_CurDD->st_Device.l_Width;
	h = GDI_gpst_CurDD->st_Device.l_Height;

	if ( Flags & GDI_Cul_SFF_ReferenceIsY )
	{
		H = h;
		W = (LONG) (h / f);

		if(((Flags & GDI_Cul_SFF_CropToWindow) && (W > w)) || ((Flags & GDI_Cul_SFF_OccupyAll) && (W < w)))
		{
			H = (LONG) (w * f);
			W = w;
		}
	}
	else
	{
		H = (LONG) (w * f);
		W = w;

		if(((Flags & GDI_Cul_SFF_CropToWindow) && (H > h)) || ((Flags & GDI_Cul_SFF_OccupyAll) && (H < h)))
		{
			W = (LONG) (h / f);
			H = h;
		}
	}

	if ( pCamera->f_ViewportWidth == 0 )
	{
		pCamera->f_ViewportWidth = 1.0f;
		pCamera->f_ViewportHeight = 1.0f;
		pCamera->f_ViewportLeft = 0.0f;
		pCamera->f_ViewportTop = 0.0f;
	}

	x = (int) ( pCamera->f_ViewportLeft * W + ((w - W) / 2) );
	y = (int) ( pCamera->f_ViewportTop * H + ((h - H) / 2) );
	h = (int) ( pCamera->f_ViewportHeight * H );
	w = (int) ( pCamera->f_ViewportWidth * W );

	pCamera->l_ViewportRealLeft = x;
	pCamera->l_ViewportRealTop = y;

    if ( pCamera->ul_Flags & CAM_Cul_Flags_Ortho )
    {
		D3DXMATRIX offsetMatrix;
		D3DXMATRIX orthoMatrix;
		float offsetX = 0.5f / w; // to place the position exactly at pixel center 
		float offsetY = 0.5f / w; // to place the position exactly at pixel center  

		D3DXMatrixTranslation( &offsetMatrix, offsetX, offsetY, 0.0f );
		D3DXMatrixOrthoOffCenterRH( &orthoMatrix, 0.0f, (float) w, 0.0f, (float) h, 0.0f, 1.0f );

		D3DXMatrixMultiply( (D3DXMATRIX *) gDx9SpecificData.pProjectionMatrix, &orthoMatrix, &offsetMatrix );
		IDirect3DDevice9_SetTransform( gDx9SpecificData.pD3DDevice,	D3DTS_PROJECTION, (const D3DMATRIX *) gDx9SpecificData.pProjectionMatrix );

        return;
    }

	if( pCamera->ul_Flags & CAM_Cul_Flags_Perspective )
	{
		f = 1.0f / fNormalTan( pCamera->f_FieldOfVision / 2 );
		MATH_SetIdentityMatrix( gDx9SpecificData.pProjectionMatrix );

		if( GDI_gpst_CurDD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_ReferenceIsY )
		{
			gDx9SpecificData.pProjectionMatrix->Ix = f * f_ScreenRatio;
			gDx9SpecificData.pProjectionMatrix->Jy = -f;
		}
		else
		{
			gDx9SpecificData.pProjectionMatrix->Ix = f;
			gDx9SpecificData.pProjectionMatrix->Jy = -f / f_ScreenRatio;
		}

		gDx9SpecificData.pProjectionMatrix->Kz = ( pCamera->f_FarPlane + pCamera->f_NearPlane ) / ( pCamera->f_FarPlane - pCamera->f_NearPlane );
		gDx9SpecificData.pProjectionMatrix->Sz = 1.0f;
		gDx9SpecificData.pProjectionMatrix->T.z = -0.05f;
		gDx9SpecificData.pProjectionMatrix->w = 0.0f;

		IDirect3DDevice9_SetTransform( gDx9SpecificData.pD3DDevice,	D3DTS_PROJECTION, (const struct _D3DMATRIX *) gDx9SpecificData.pProjectionMatrix );

		stCS.ClipUnion = D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP | D3DCS_BOTTOM | D3DCS_FRONT | D3DCS_BACK;
		stCS.ClipIntersection = D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP | D3DCS_BOTTOM | D3DCS_FRONT | D3DCS_BACK;
		IDirect3DDevice9_SetClipStatus( gDx9SpecificData.pD3DDevice, &stCS );
	}
	else
	{
		float	f_IsoFactorZoom, f_Scale;

		MATH_SetIdentityMatrix( gDx9SpecificData.pProjectionMatrix );

		f_IsoFactorZoom = pCamera->f_IsoFactor * pCamera->f_IsoZoom;
		f_Scale = f_IsoFactorZoom;

		if ( GDI_gpst_CurDD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_ReferenceIsY )
		{
			f = 1.0f / GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;

			if( pCamera->f_IsoFactor == 0 )
			{
				Dx9_Ortho( (1.0f - f) / 2.f, (1.f + f) / 2.f, 0.f, 1.f, -pCamera->f_NearPlane, -pCamera->f_FarPlane );
			}
			else
			{
				l = f_Scale * (-f);
				r = f_Scale * (f);
				b = f_Scale * -1.f;
				t = f_Scale * 1.f;

				Dx9_Ortho( l, r, b, t, pCamera->f_FarPlane, -pCamera->f_FarPlane );
			}
		}
		else
		{
			f = GDI_gpst_CurDD->st_ScreenFormat.f_ScreenYoverX;
			if(pCamera->f_IsoFactor == 0.f)
			{
				Dx9_Ortho( 0.f, 1.f, (1.f - f) / 2.f, (1.f + f) / 2.f, -pCamera->f_NearPlane, -pCamera->f_FarPlane);
			}
			else
			{
				t = f_Scale * (-f);
				b = f_Scale * (f);
				l = f_Scale * -1.f;
				r = f_Scale * 1.f;

				Dx9_Ortho( l, r, b, t, pCamera->f_FarPlane, -pCamera->f_FarPlane);
			}
		}
	}

	Dx9_SetViewport( x, y, w, h );
}

//////////////////////////////////
void	Dx9_SetViewport( LONG x, LONG y, LONG w, LONG h )
{
    if( !( gDx9SpecificData.pD3DDevice ) ) 
		return;

    if(x < 0) 
		x = 0;
    if(y < 0) 
		y = 0;
    if( w + x > (LONG) gDx9SpecificData.d3dPresentParameters.BackBufferWidth ) 
		w = gDx9SpecificData.d3dPresentParameters.BackBufferWidth - x;
    if( h + y > (LONG) gDx9SpecificData.d3dPresentParameters.BackBufferHeight ) 
		h = gDx9SpecificData.d3dPresentParameters.BackBufferHeight - y;

	gDx9SpecificData.Viewport.X			= x;
    gDx9SpecificData.Viewport.Y			= y;
	gDx9SpecificData.Viewport.Width		= w;
    gDx9SpecificData.Viewport.Height	= h;
    gDx9SpecificData.Viewport.MinZ		= 0.0f;
    gDx9SpecificData.Viewport.MaxZ		= 1.0f;

    IDirect3DDevice9_SetViewport( gDx9SpecificData.pD3DDevice, &gDx9SpecificData.Viewport );
}


void	Dx9_SetViewMatrix( MATH_tdst_Matrix *pMatrix )
{
	MATH_tdst_Matrix st_OGLMatrix;
	MATH_MakeOGLMatrix(&st_OGLMatrix, pMatrix);
	IDirect3DDevice9_SetTransform(gDx9SpecificData.pD3DDevice, D3DTS_VIEW, (const D3DMATRIX*)&st_OGLMatrix);
}

void	Dx9_SetWorldMatrix( MATH_tdst_Matrix *pMatrix )
{
	MATH_tdst_Matrix st_OGLMatrix;
	MATH_MakeOGLMatrix(&st_OGLMatrix, pMatrix);
	IDirect3DDevice9_SetTransform(gDx9SpecificData.pD3DDevice, D3DTS_WORLD, (const D3DMATRIX*)&st_OGLMatrix);
}

void	Dx9_SetViewMatrix_SDW( MATH_tdst_Matrix *pMatrix, float *limits )
{
	(void) pMatrix;
	(void) limits;
}

void	Dx9_SetTextureTarget( ULONG textureNumber, ULONG clear )
{
	(void) textureNumber;
	(void) clear;
}


/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

void Dx9_Ortho( float Left, float Right, float Bottom, float Top, float Near, float Far)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	D3DMATRIX	OrthoMatrix;
	/*~~~~~~~~~~~~~~~~~~~~*/

	memset( &OrthoMatrix, 0, sizeof(OrthoMatrix) );

	OrthoMatrix._11 = 2.0f / (Right - Left);
	OrthoMatrix._22 = 2.0f / (Top - Bottom);
	OrthoMatrix._33 = -4.0f / (Far - Near);

	OrthoMatrix._14 = -(Right + Left) / (Right - Left);
	OrthoMatrix._24 = -(Top + Bottom) / (Top - Bottom);
	OrthoMatrix._34 = -(Far + Near) / (Far - Near);

	OrthoMatrix._44 = 1.0f;

	{
		D3DCLIPSTATUS9	stCS;

		stCS.ClipUnion = D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP | D3DCS_BOTTOM;
		stCS.ClipIntersection = D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP | D3DCS_BOTTOM;
		IDirect3DDevice9_SetClipStatus(gDx9SpecificData.pD3DDevice, &stCS);
	}

	IDirect3DDevice9_SetTransform( gDx9SpecificData.pD3DDevice, D3DTS_PROJECTION, &OrthoMatrix );
}

