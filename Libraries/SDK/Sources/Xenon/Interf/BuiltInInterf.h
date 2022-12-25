//////////////////////////////////////////////////////////////////////////
// BuiltInInterf
// ------------------------
// Wraps access to built-in Xenon interfaces
//////////////////////////////////////////////////////////////////////////

#ifndef __BUILTININTERF_H__
#define __BUILTININTERF_H__

#if defined(_XENON)

//////////////////////////////////////////////////////////////////////////
// CXeBuiltInInterf

class CXeBuiltInInterf
{
// construction/destruction
public:
	CXeBuiltInInterf();
	~CXeBuiltInInterf();

// interface
public:
	void				ShowDeviceSelector( int nSavegameSize );
	void				SetDeviceSelectorMode( int nMode );
	int					GetDeviceSelectorMode( );
	XCONTENTDEVICEID	GetSelectedDeviceID( );
	void				SetSelectedDeviceID( XCONTENTDEVICEID id );
	void				InvalidateSelectedDevice( );

	void				ShowProfileSelector( );

// fields
private:
	XCONTENTDEVICEID	m_idDevice;
	XOVERLAPPED			m_xovDeviceSelector;
	int					m_nStorageMode;
};

//////////////////////////////////////////////////////////////////////////
// inline functions

inline XCONTENTDEVICEID	CXeBuiltInInterf::GetSelectedDeviceID( )
{
	return m_idDevice;
}

inline void CXeBuiltInInterf::SetSelectedDeviceID( XCONTENTDEVICEID id )
{
	m_idDevice = id;
}

inline void CXeBuiltInInterf::InvalidateSelectedDevice( )
{
	m_idDevice = -1;
}

//////////////////////////////////////////////////////////////////////////
// 

extern CXeBuiltInInterf g_XeBuiltInInterf;

#endif // defined(_XENON)

#endif // __BUILTININTERF_H__
