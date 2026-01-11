//////////////////////////////////////////////////////////////////////////
// Xenon Gamer Profile
// ------------------------
// Manages Gamer Profile related stuff
//////////////////////////////////////////////////////////////////////////

#ifndef __PROFILE_H__
#define __PROFILE_H__

#if defined(_XENON)

#include <map>
#include <string>

using namespace std;

//////////////////////////////////////////////////////////////////////////
// CXeProfile

class CXeProfile
{
// type definitions
public:
	enum XeProfile_Difficulty
	{
		DIFFICULTY_EASY,
		DIFFICULTY_NORMAL,
		DIFFICULTY_HARD
	};

	enum XeProfile_Language
	{
		LANGUAGE_ENGLISH,
		LANGUAGE_FRENCH,
		LANGUAGE_SPANISH,
		LANGUAGE_ITALIAN,
		LANGUAGE_GERMAN,
		LANGUAGE_DUTCH,
		LANGUAGE_PORTUGUESE,
		LANGUAGE_CHINESE,
		LANGUAGE_KOREAN,
		LANGUAGE_JAPANESE
	};

// construction
public:
	CXeProfile( );
	~CXeProfile( );

// interface
public:
	bool					IsReady( ) const;

	int						GetID( ) const;

	int						GetUserIndex( ) const;
	void					SetUserIndex( int nIndex );

	void					SetGameOwner( bool bIsOwner );
	bool					GetGameOwner( ) const;

	void					UpdateProfileSettings( );
	void					Pump( );

	XeProfile_Difficulty	GetDifficulty( ) const;
	XeProfile_Language		GetLanguage( ) const;
	bool					IsYAxisInverted( ) const;
	bool					IsControllerVibrationEnabled( ) const;

// implementation
private:
	void	Cleanup( );
	void	InternalPump( );

// fields
private:
	typedef map< string, int > 	ProfileList;

	bool								m_bReady;

	XOVERLAPPED							m_xov;
	XUSER_READ_PROFILE_SETTING_RESULT*	m_pSettingResults;

	ProfileList							m_lstProfiles;

	XeProfile_Difficulty				m_eDifficulty;
	XeProfile_Language					m_eLanguage;

	bool								m_bYAxisInverted;
	bool								m_bControllerVibrationEnabled;

	bool								m_bGameOwner;

	bool								m_bUserSet;
	int									m_nUserIndex;
};

//////////////////////////////////////////////////////////////////////////
// inline

inline void	CXeProfile::Pump( )
{
	if( m_xov.hEvent == NULL )
		return;
	else
		InternalPump( );
}

inline CXeProfile::XeProfile_Difficulty CXeProfile::GetDifficulty( ) const
{
	return m_eDifficulty;
}

inline CXeProfile::XeProfile_Language CXeProfile::GetLanguage( ) const
{
	return m_eLanguage;
}

inline bool CXeProfile::IsYAxisInverted( ) const
{
	return m_bYAxisInverted;
}

inline bool CXeProfile::IsControllerVibrationEnabled( ) const
{
	return m_bControllerVibrationEnabled;
}

inline void CXeProfile::SetGameOwner( bool bIsOwner )
{
	m_bGameOwner = bIsOwner;
}

inline bool CXeProfile::GetGameOwner( ) const
{
	return m_bGameOwner;
}

inline int CXeProfile::GetUserIndex( ) const
{
	return m_nUserIndex;
}

inline void CXeProfile::SetUserIndex( int nIndex )
{
	m_nUserIndex = nIndex;
	m_bUserSet = true;

	UpdateProfileSettings( );
}

//////////////////////////////////////////////////////////////////////////
// extern definition

extern CXeProfile g_XeProfile;

#endif // defined(_XENON)

#endif // __PROFILE_H__
