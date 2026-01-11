//////////////////////////////////////////////////////////////////////////
// XeSimpleRenderer
// ------------------------
// Simple renderer to take over standard renderer in situations where
// it can not be active (ex: when loading a map)
//////////////////////////////////////////////////////////////////////////

#ifndef __SIMPLERENDERER_H__
#define __SIMPLERENDERER_H__

#if defined(_XENON)

//////////////////////////////////////////////////////////////////////////
// CXeSimpleRenderer

class CXeSimpleRenderer
{
// construction/destruction
public:
	CXeSimpleRenderer();
	~CXeSimpleRenderer();

// interface
public:
	void				Activate( );
	void				Deactivate( );

	bool				IsActive( ) const;
	bool				IsReady( ) const;
	bool				IsDeactivating( ) const;

	bool				OwnsDevice( ) const;

	void				RequestReleaseDeviceOwnership( );
	void				SignalDeviceOwnershipAvailable( );

	void				SignalMainRenderingOff( );
	void				SignalDeactivated( );

	int					GetLoadingWorldID( );
	void				SetLoadingWorldID( int nWorldID );

	static DWORD		ThreadEntryPoint(void* pData);

// fields
private:
	HANDLE				m_hThread;
	DWORD				m_dwThreadId;

	int volatile		m_nLoadingWorldID;

	LONG volatile		m_nState;
	LONG volatile		m_nDeviceState;
	LONG volatile		m_nDeviceCommand;
};

//////////////////////////////////////////////////////////////////////////
// 

extern __declspec(align(32)) CXeSimpleRenderer g_oXeSimpleRenderer;

#endif // defined(_XENON)

#endif // __SIMPLERENDERER_H__
