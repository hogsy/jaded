#include <d3d.h>
static unsigned long		   OK = 0;
static RECT                    g_rcScreenRect;
static LPDIRECTDRAW            pddx;
static DDSURFACEDESC           desc;
static LPDIRECTDRAWCLIPPER     clipper;
static LPDIRECTDRAW2           pdd = NULL;
static LPDIRECTDRAWSURFACE     front = NULL;
static LPDIRECTDRAWSURFACE     back = NULL;
void CleanAll()
{
    if (front) front->Release();
    if (back ) back ->Release();
    if (pdd  ) pdd  ->Release();
    front = NULL;
    back = NULL;
    pdd  = NULL;
}


unsigned long  AdaptToWindow(HWND WindowHwnd , HWND SubW , unsigned long SizeX , unsigned long SizeY)

{
    if (front) front->Release();
    if (back ) back ->Release();
    front = NULL;
    back = NULL;
    GetClientRect( SubW, &g_rcScreenRect );
    ClientToScreen( SubW, (POINT*)&g_rcScreenRect.left );
    ClientToScreen( SubW, (POINT*)&g_rcScreenRect.right );
	if (!pdd)
	{
		DirectDrawCreate( 0, &pddx, NULL ) ;
		pddx -> QueryInterface ( IID_IDirectDraw2 , (LPVOID*)&pdd ) ;
		pddx -> Release();
	}
	if (FAILED(pdd -> SetCooperativeLevel( WindowHwnd, DDSCL_NORMAL ) )) return 1;
	desc.dwSize         = sizeof( DDSURFACEDESC );
	desc.dwFlags        = DDSD_CAPS;
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (FAILED(pdd -> CreateSurface( &desc, &front, NULL ) )) return 1;
	desc.dwSize         = sizeof( DDSURFACEDESC );
	desc.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	desc.dwWidth        = SizeX ;
	desc.dwHeight       = SizeY ;
	desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY /*|DDSCAPS_VIDEOMEMORY*/ | DDSCAPS_3DDEVICE;
	if (FAILED(pdd->CreateSurface(  &desc, &back , NULL ) )) return 1;
	if (FAILED(pdd->CreateClipper(  0, &clipper, 0 ) )) return 1;
	if (FAILED(clipper -> SetHWnd( 0, WindowHwnd) )) return 1;
	if (FAILED(front -> SetClipper( clipper ) )) return 1;
	clipper->Release();
	OK = 1;
	return 0;
}

unsigned long Blitte(unsigned long *p_sourceptr)
{
	DDSURFACEDESC ddsd;
	if (!OK) return 1;
	ddsd.dwSize  = sizeof( DDSURFACEDESC );
	if (!(FAILED(back->Lock(NULL, &ddsd,DDLOCK_WAIT,NULL))))
	{
		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
		{
			unsigned long *p_sourceptrLast , *p_destptr ,lYCounterLocal, lSizeX,lSizeY,lPitch;
			
			lSizeX = ddsd.dwWidth;
			lSizeY = ddsd.dwHeight;
			lPitch = ddsd.lPitch;
			
			//                if ((ddsd.ddpfPixelFormat.dwRBitMask & 0xFFFF )== 0x00007c00)
			{
				for (lYCounterLocal = 0; lYCounterLocal < lSizeY ; lYCounterLocal++)
				{
					p_destptr = ((unsigned long *)ddsd.lpSurface) + (lPitch >> 2) * lYCounterLocal;
					p_sourceptrLast = p_sourceptr  + (lSizeX >> 1);
					for (;p_sourceptr < p_sourceptrLast ;p_sourceptr ++,p_destptr ++)
					{
						*p_destptr = (*p_sourceptr & 0x001F001F) | ((*p_sourceptr & 0xFFC0FFC0)>>1);
					}
				}
			}
		}
		else
            if (ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
            {
                unsigned long *p_sourceptrLast , *p_destptr ,lYCounterLocal, lSizeX,lSizeY,lPitch;
                
                lSizeX = ddsd.dwWidth;
                lSizeY = ddsd.dwHeight;
                lPitch = ddsd.lPitch;
                for (lYCounterLocal = 0; lYCounterLocal < lSizeY ; lYCounterLocal++)
                {
                    p_destptr = ((unsigned long *)ddsd.lpSurface) + (lPitch >> 2) * lYCounterLocal;
                    p_sourceptrLast = p_sourceptr  + (lSizeX >> 1);
                    for (;p_sourceptr < p_sourceptrLast ;p_sourceptr ++,p_destptr += 2)
                    {
                        *p_destptr = ((*p_sourceptr & 0x001F0000)>>13) | ((*p_sourceptr & 0xF8000000)>>8) | ((*p_sourceptr & 0x07C00000)>>11);
                        *(p_destptr +1)= *p_destptr ;
                    }
                }
            }
            back->Unlock(ddsd.lpSurface);
	}
	front->Blt( &g_rcScreenRect,back,NULL, DDBLT_ASYNC, NULL);
    return 0;
}

unsigned long Blitte_32(unsigned long *p_sourceptr)
{
	DDSURFACEDESC ddsd;
	if (!OK) return 1;
	ddsd.dwSize  = sizeof( DDSURFACEDESC );
	if (!(FAILED(back->Lock(NULL, &ddsd,DDLOCK_WAIT,NULL))))
	{
		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
		{
			unsigned long *p_sourceptrLast , *p_destptr ,lYCounterLocal, lSizeX,lSizeY,lPitch;
			
			lSizeX = ddsd.dwWidth;
			lSizeY = ddsd.dwHeight;
			lPitch = ddsd.lPitch;
			
			if ((ddsd.ddpfPixelFormat.dwRBitMask & 0xFFFF )== 0x00007c00) /* 0x555*/ 
			{
				for (lYCounterLocal = 0; lYCounterLocal < lSizeY ; lYCounterLocal++)
				{
					p_destptr = ((unsigned long *)ddsd.lpSurface) + (lPitch >> 2) * lYCounterLocal;
					p_sourceptrLast = p_sourceptr  + lSizeX;
					for (;p_sourceptr < p_sourceptrLast ;p_sourceptr += 2 ,p_destptr ++)
					{
						*p_destptr = ((*(p_sourceptr+1) & 0xF80000)<<7) |
							((*(p_sourceptr+1) & 0x00F800)<<10) |
							((*(p_sourceptr+1) & 0x0000F8)<<13) |
							((*(p_sourceptr+0) & 0xF80000)>>9) |
							((*(p_sourceptr+0) & 0x00F800)>>6) |
							((*(p_sourceptr+0) & 0x0000F8)>>3);
					}
				}
			}
			else /* 0x565*/ 
			{
				for (lYCounterLocal = 0; lYCounterLocal < lSizeY ; lYCounterLocal++)
				{
					p_destptr = ((unsigned long *)ddsd.lpSurface) + (lPitch >> 2) * lYCounterLocal;
					p_sourceptrLast = p_sourceptr  + lSizeX;
					for (;p_sourceptr < p_sourceptrLast ;p_sourceptr += 2 ,p_destptr ++)
					{
						*p_destptr = ((*(p_sourceptr+1) & 0xF80000)<<8) |
							((*(p_sourceptr+1) & 0x00F800)<<11) |
							((*(p_sourceptr+1) & 0x0000F8)<<13) |
							((*(p_sourceptr+0) & 0xF80000)>>8) |
							((*(p_sourceptr+0) & 0x00F800)>>5) |
							((*(p_sourceptr+0) & 0x0000F8)>>3);
					}
				}
			}
		}
		else
            if (ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
            {
                unsigned long /**p_sourceptrLast ,*/ *p_destptr ,lYCounterLocal, lSizeX,lSizeY,lPitch;
                
                lSizeX = ddsd.dwWidth;
                lSizeY = ddsd.dwHeight;
                lPitch = ddsd.lPitch;
                for (lYCounterLocal = 0; lYCounterLocal < lSizeY ; lYCounterLocal++)
                {
                    p_destptr = ((unsigned long *)ddsd.lpSurface) + (lPitch >> 2) * lYCounterLocal;
//                    p_sourceptrLast = p_sourceptr  + lSizeX;
					memcpy(p_destptr , p_sourceptr ,  lSizeX << 2);
					p_sourceptr += lSizeX;
/*                    for (;p_sourceptr < p_sourceptrLast ;p_sourceptr ++,p_destptr ++)
                    {
                        *p_destptr = *p_sourceptr;
                    }*/
                }
            }
		else
            if (ddsd.ddpfPixelFormat.dwRGBBitCount == 24)
            {
                unsigned long *p_sourceptrLast , lYCounterLocal, lSizeX,lSizeY,lPitch;
				unsigned char *p_destptr ;
                
                lSizeX = ddsd.dwWidth;
                lSizeY = ddsd.dwHeight;
                lPitch = ddsd.lPitch;
                for (lYCounterLocal = 0; lYCounterLocal < lSizeY ; lYCounterLocal++)
                {
                    p_destptr = ((unsigned char *)ddsd.lpSurface) + lPitch * lYCounterLocal;
                    p_sourceptrLast = p_sourceptr  + lSizeX;
                    for (;p_sourceptr < p_sourceptrLast ;p_sourceptr ++,p_destptr += 3)
                    {
                        *(p_destptr+2) = (unsigned char)((*p_sourceptr) >> 16);
                        *(p_destptr+1) = (unsigned char)((*p_sourceptr) >> 8);
                        *(p_destptr+0) = (unsigned char)((*p_sourceptr));
                    }
                }
            }
            back->Unlock(ddsd.lpSurface);
	}
	front->Blt( &g_rcScreenRect,back,NULL, DDBLT_ASYNC, NULL);
    return 0;
}

