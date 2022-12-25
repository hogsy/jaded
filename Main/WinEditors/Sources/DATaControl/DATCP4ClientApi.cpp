//------------------------------------------------------------------------------
//   DATCPerforce.h
/// \author    SLepasteur
/// \date      2006-05-04
/// \par       Description: 
///            Specification of class DAT_CP4ClientApi
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCP4ClientApi.h"

#ifndef SPEED_EDITORS
std::ofstream DAT_CP4ClientApi::m_LogFile("P4Log.txt");
#endif

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   DAT_CP4ClientApi::DAT_CP4ClientApi()
/// \author    SLepasteur
/// \date      2006-05-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
DAT_CP4ClientApi::DAT_CP4ClientApi():
ClientApi()
#ifndef SPEED_EDITORS
,m_nextArgv("")
#endif
{
}

//------------------------------------------------------------
//   DAT_CP4ClientApi::~DAT_CP4ClientApi()
/// \author    SLepasteur
/// \date      2006-05-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
DAT_CP4ClientApi::~DAT_CP4ClientApi()
{
}

//------------------------------------------------------------
//   DAT_CP4ClientApi::Init(Error* e)
/// \author    SLepasteur
/// \date      2006-05-04
/// \par       Description: 
///            No description available ...
/// \param     Error* e : an error object
/// \see 
//------------------------------------------------------------
void DAT_CP4ClientApi::Init(Error* _e)
{
#ifndef SPEED_EDITORS
	m_LogFile << CurrentTime() << ":" << GetTickCount() << " => Init" << std::endl;
#endif

	ClientApi::Init(_e);

#ifndef SPEED_EDITORS
	if(_e->Test())
	{
		StrBuf msg;
		_e->Fmt(&msg, EF_INDENT);
		m_LogFile << "ERROR : " << msg.Text() << std::endl;
	}
#endif
}

//------------------------------------------------------------
//   DAT_CP4ClientApi::Final(Error* e)
/// \author    SLepasteur
/// \date      2006-05-04
/// \par       Description: 
///            No description available ...
/// \param     Error* e : an error object
/// \see 
//------------------------------------------------------------
void DAT_CP4ClientApi::Final(Error* _e)
{
#ifndef SPEED_EDITORS
	m_LogFile << CurrentTime() << ":" << GetTickCount() << " => Final" << std::endl;
#endif

	ClientApi::Final(_e);

#ifndef SPEED_EDITORS
	if(_e->Test())
	{
		StrBuf msg;
		_e->Fmt(&msg, EF_INDENT);
		m_LogFile << "ERROR : " << msg.Text() << std::endl;
	}
#endif
}

//------------------------------------------------------------
//   DAT_CP4ClientApi::SetArgv(int _argc, char* const* _argv)
/// \author    SLepasteur
/// \date      2006-05-04
/// \par       Description: 
///            No description available ...
/// \param     int _argc : the number of arguments
///            char* const* _argv : the arguments themselves
/// \see 
//------------------------------------------------------------
void DAT_CP4ClientApi::SetArgv(int _argc, char* const* _argv)
{
	ClientApi::SetArgv(_argc, _argv);

#ifndef SPEED_EDITORS
	m_nextArgv = " ";
	for(int i = 0; i < _argc; ++i)
	{
		m_nextArgv += _argv[i];
		m_nextArgv += " ";
	}
#endif
}

//------------------------------------------------------------
//   DAT_CP4ClientApi::Run(const char* _cmd, ClientUser* _pClientUser)
/// \author    SLepasteur
/// \date      2006-05-04
/// \par       Description: 
///            No description available ...
/// \param     const char* _cmd : the name of the command to run
/// \see 
//------------------------------------------------------------
void DAT_CP4ClientApi::Run(const char* _cmd, ClientUser* _pClientUser)
{
#ifndef SPEED_EDITORS
	m_LogFile << CurrentTime() << ":" << GetTickCount() << " => p4 " << _cmd;
	m_LogFile << m_nextArgv << std::endl;
#endif

	ClientApi::Run(_cmd, _pClientUser);
}

#ifndef SPEED_EDITORS
//------------------------------------------------------------
//   DAT_CP4ClientApi::DAT_CP4ClientApi()
/// \author    SLepasteur
/// \date      2006-05-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
std::string DAT_CP4ClientApi::CurrentDate() const
{
    char sTime[24];
    time_t CurrentTime = time(NULL);
    strftime(sTime, sizeof(sTime), "%d/%m/%Y", localtime(&CurrentTime));

    return sTime;
}


//------------------------------------------------------------
//   DAT_CP4ClientApi::DAT_CP4ClientApi()
/// \author    SLepasteur
/// \date      2006-05-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
std::string DAT_CP4ClientApi::CurrentTime() const
{
    char sTime[24];
    time_t CurrentTime = time(NULL);
    strftime(sTime, sizeof(sTime), "%H:%M:%S", localtime(&CurrentTime));

    return sTime;
}
#endif //SPEED_EDITORS