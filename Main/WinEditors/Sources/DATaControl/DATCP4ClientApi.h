//------------------------------------------------------------------------------
//   DATCPerforce.h
/// \author    SLepasteur
/// \date      2006-05-04
/// \par       Description: 
///            Specification of class DAT_CP4ClientApi
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTAPI_H__
#define __DATCP4CLIENTAPI_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "p4/clientapi.h"
#include <fstream>
#include <string>

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------

class DAT_CP4ClientApi : public ClientApi
{
	//--------------------------------------------------------------------------
	// public definitions
	//--------------------------------------------------------------------------
public:
	DAT_CP4ClientApi();
	~DAT_CP4ClientApi();

	static void Log(const std::string _message) 
	{
#ifndef SPEED_EDITORS
		m_LogFile << "\t" << _message << std::endl;
#endif
	}

	void Init(Error* _e);
	void Final(Error* _e);
	void SetArgv(int _argc, char* const* _argv);
	void Run(const char* _cmd, ClientUser* _pClientUser);

	//--------------------------------------------------------------------------
	// private definitions
	//--------------------------------------------------------------------------
private:
#ifndef SPEED_EDITORS
	std::string CurrentDate() const;
	std::string CurrentTime() const;

	static std::ofstream	m_LogFile;
	std::string				m_nextArgv;
#endif
};

#endif