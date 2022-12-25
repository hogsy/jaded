//////////////////////////////////////////////////////////////////////////
// Xenon Notification Manager
// --------------------------
// Checks for system notifications and dispatches them appropriately
//////////////////////////////////////////////////////////////////////////

#ifndef __NOTIFICATION_MANAGER_H__
#define __NOTIFICATION_MANAGER_H__

//////////////////////////////////////////////////////////////////////////
// CXeNotificationManager

class CXeNotificationManager
{
// construction / destruction
public:
	CXeNotificationManager( );
	~CXeNotificationManager( );

// interface
public:
	bool	Initialize( );
	bool	Shutdown( );

	void	CheckForNotifications( );

// fields
private:
	HANDLE			m_hNotification;

    BOOL            m_bForceUnPause;
};

extern CXeNotificationManager g_XeNotificationManager;

#endif // __NOTIFICATION_MANAGER_H__
