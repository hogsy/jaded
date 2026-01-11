// Dx9init.h

#ifndef __DX9INIT_H__
#define __DX9INIT_H__

#include "Dx9struct.h"
#include "GDInterface/GDInterface.h"


#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
    Function
 ****************************************************************************************************/

/*===================================================================================================
    Close / init / reinit
 ===================================================================================================*/

Dx9_tdst_SpecificData*	Dx9_pst_CreateDevice( void );
void					Dx9_DestroyDevice( void *p_SD );
LONG					Dx9_l_Close( GDI_tdst_DisplayData *pDD );
LONG					Dx9_l_Init( HWND hWnd, GDI_tdst_DisplayData *pDD );
LONG					Dx9_l_ReadaptDisplay( HWND hWnd, GDI_tdst_DisplayData *pDD );

/*===================================================================================================
    Query for texture formats etc.
 ===================================================================================================*/
bool					Dx9_CheckDeviceFormat(DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat);
bool					Dx9_CheckDepthStencilMatch(D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat);

/*===================================================================================================
    Flip / clear / surface status
 ===================================================================================================*/
void					Dx9_Flip(void);
void					Dx9_Clear(LONG, ULONG);



#ifdef __cplusplus
}
#endif

#endif /* __DX9INIT_H__ */
