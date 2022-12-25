//////////////////////////////////////////////////////////////////////////
// BuiltInInterf
// ------------------------
// Wraps access to built-in Xenon interfaces
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"
#include "BuiltInInterf.h"

#include "..\Profile\Profile.h"
#include "..\MenuManager\MenuManager.h"

#if defined(_XENON)

extern bool g_bFoundNewDefaultDevice;

//////////////////////////////////////////////////////////////////////////
// CXeBuiltInInterf

CXeBuiltInInterf g_XeBuiltInInterf;

CXeBuiltInInterf::CXeBuiltInInterf( )
	: m_idDevice( -1 ),
	  m_nStorageMode( 0 )
{
	ZeroMemory( &m_xovDeviceSelector, sizeof( m_xovDeviceSelector ) );
}

CXeBuiltInInterf::~CXeBuiltInInterf( )
{
}

void CXeBuiltInInterf::ShowDeviceSelector( int nSavegameSize )
{
	// don't do anything if in demo mode
	if( g_MenuManager.IsInDemoMode_NoStorageAccess( ) )
		return;

	// display device selector
	ULARGE_INTEGER nSize = {0};
	nSize.QuadPart = nSavegameSize;

	DWORD dwFlags = 0;
	dwFlags = XCONTENTFLAG_FORCE_SHOW_UI | XCONTENTFLAG_MANAGESTORAGE;

	const DWORD dwResult = XShowDeviceSelectorUI
		(
		g_XeProfile.GetUserIndex( ),
		XCONTENTTYPE_SAVEDGAME,
		dwFlags,
		nSize,
		&m_idDevice,
		&m_xovDeviceSelector
		);

	ERR_X_Assert( dwResult == ERROR_IO_PENDING );
}

void CXeBuiltInInterf::SetDeviceSelectorMode( int nMode )
{
	m_nStorageMode = nMode;

	if( m_nStorageMode == 1 )
		g_bFoundNewDefaultDevice = true;
}

int CXeBuiltInInterf::GetDeviceSelectorMode( )
{
	return m_nStorageMode;
}

void CXeBuiltInInterf::ShowProfileSelector( )
{
	XShowSigninUI( 1, 0 );
}

#endif
