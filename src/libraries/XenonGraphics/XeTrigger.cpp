//-----------------------------------------------------------------------------
// File: XeTrigger.cpp
//
// Desc: Trigger to command dump of profiling data.
//
//
//-----------------------------------------------------------------------------

#include "Precomp.h"
#include "XeTrigger.h"
#include "XenonGraphics/XeRenderer.h"

//////////////////////////////////////////////////////////////////////////
// global data

TriggerManager g_XeTriggerManager;

#if defined(TRIGGER_ENABLE)

//**************************
//TriggerManager methods
//**************************

// ------------------------------------------------------------------------------------------------
// Name   : TriggerManager::TriggerManager
// Params : 
// RetVal : void
// Descr. : Constructor; initialize Trigger Array and Enable Test on TriggerList
// ------------------------------------------------------------------------------------------------
TriggerManager::TriggerManager():
mh_LogFile(INVALID_HANDLE_VALUE)
{
	EnableTest();

	//**************************
	// add every Trigger here
	// there are two types of Callback
	//
	// AddTrigger(&VAR_ADDRESS, [&]MIN_VAL (or (var_type)NULL), [&]MAX_VAL (or (var_type)NULL),[CALLBACK_FUNC], ["TRIGGER NAME"], [FLAGS]);
	// for a Callback to a non-member function with a prototype like "void MyFunc(void)"
	//
	// AddTrigger(&VAR_ADDRESS, [&]MIN_VAL (or (var_type)NULL), [&]MAX_VAL (or (var_type)NULL), &CLASS_INSTANCE, &CLASS::CLASS_METHOD, ["TRIGGER NAME"], [FLAGS]);
	// for a Callback to a non-static member function of an arbitrary class
	//
	// When a NULL value is passed in MIN_VAL or MAX_VAL parameter, you must specify the variable type - [(var_type)NULL]
	//**************************
}

// ------------------------------------------------------------------------------------------------
// Name   : TriggerManager::~TriggerManager
// Params : 
// RetVal : void
// Descr. : Destructor; Delete all triggers in TriggerList
// ------------------------------------------------------------------------------------------------
TriggerManager::~TriggerManager()
{
	DelAllTriggers();
	b_CloseLogFile();
}

// ------------------------------------------------------------------------------------------------
// Name   : TriggerManager::AddTrigger
// Params : _Trigger : Trigger to add in TriggerList
// RetVal : void
// Descr. : Add trigger in TriggerList if space is available
// ------------------------------------------------------------------------------------------------
void TriggerManager::AddTrigger(TriggerDataIF* _Trigger)
{
	if(!_Trigger)
		return;

	m_TriggerList.push_back(_Trigger);
}

// ------------------------------------------------------------------------------------------------
// Name   : TriggerManager::b_Test
// Params : 
// RetVal : BOOL
// Descr. : Test all triggers in TriggerList
// ------------------------------------------------------------------------------------------------
BOOL TriggerManager::b_Test()
{
	if(!mb_IsEnabled || (m_TriggerList.size() == 0))
		return FALSE;

	//wait before opening logfile until the mapkey is available
	if(g_oXeRenderer.GetFrameCounter() == 5)
		b_OpenLogFile();
	
	BOOL b_Res = FALSE;

	//test all triggers
	TriggerListIt it = m_TriggerList.begin();
	for(; it != m_TriggerList.end(); ++it)
	{
		b_Res = (*it)->b_Test();

		//construct info string and output it to log file
		if(b_Res)
		{
			OutputInfoToLogFile(it);
		}	
	}
	
	return b_Res;
}

// ------------------------------------------------------------------------------------------------
// Name   : TriggerManager:OutputInfoToLog
// Params : _it	: iterator on fired trigger in TriggerList 
// RetVal : 
// Descr. : Output information about the fired trigger in a log file
// ------------------------------------------------------------------------------------------------
void TriggerManager::OutputInfoToLogFile(TriggerListIt _it)
{
	std::strstream oStream;
	char sz_Fcount[32];

	//output Frame counter
	sprintf(sz_Fcount, "F%06d : ", (int)g_oXeRenderer.GetFrameCounter());
	oStream << sz_Fcount;

	switch((*_it)->GetInfo())
	{
	case TRIGGER_ISUNDERMINVALUE:
		oStream << (*_it)->GetName() << "[";
		(*_it)->OutputVarValue(oStream);
		oStream << "] pass under Min Value [";
		(*_it)->OutputMinValue(oStream);
		oStream << "]\n";
		break;
	case TRIGGER_ISOVERMAXVALUE:
		oStream << (*_it)->GetName() << "[";
		(*_it)->OutputVarValue(oStream);
		oStream << "] pass over Max Value [";
		(*_it)->OutputMaxValue(oStream);
		oStream << "]\n";
		break;
	default:
		break;
	}

	b_WriteToLogFile(oStream.str(), (DWORD)oStream.pcount());
}


// ------------------------------------------------------------------------------------------------
// Name   :  TriggerManager::b_OpenLogFile
// Params : 
// RetVal : BOOL : succeed or not
// Descr. : Open Log file
// ------------------------------------------------------------------------------------------------
BOOL TriggerManager::b_OpenLogFile()
{
	char sz_LogFileName[260];

	if(WOR_gpst_CurrentWorld)
	{
		sprintf(sz_LogFileName, "d:\\[%08x]_%s", WOR_gpst_CurrentWorld->h_WorldKey, TRIGGER_DEFAULT_LOGFILENAME);
	}
	else
	{
		sprintf(sz_LogFileName, "d:\\_%s", TRIGGER_DEFAULT_LOGFILENAME);
	}

	mh_LogFile = CreateFile(sz_LogFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (mh_LogFile != INVALID_HANDLE_VALUE)
		return TRUE;
	else
		return FALSE;
}

// ------------------------------------------------------------------------------------------------
// Name   :  TriggerManager::b_CloseLogFile
// Params : 
// RetVal : BOOL : succeed or not
// Descr. : Close Log file
// ------------------------------------------------------------------------------------------------
BOOL TriggerManager::b_CloseLogFile()
{
	if (mh_LogFile != INVALID_HANDLE_VALUE)
	{
		BOOL b_Res = CloseHandle(mh_LogFile);
		mh_LogFile = INVALID_HANDLE_VALUE;
		return b_Res;
	}
	else
		return FALSE;
}

// ------------------------------------------------------------------------------------------------
// Name   :  TriggerManager::b_WriteToLogFile
// Params : _sz_String
// RetVal : BOOL : succeed or not
// Descr. : Write to Log file
// ------------------------------------------------------------------------------------------------
BOOL TriggerManager::b_WriteToLogFile(char* _sz_String, DWORD _dw_StringSize)
{
	if(mh_LogFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dw_SizeWritten;
	SetEndOfFile(mh_LogFile);

	WriteFile(mh_LogFile, _sz_String, _dw_StringSize, &dw_SizeWritten, NULL);
	XeValidate(dw_SizeWritten == _dw_StringSize, "Error writing the triggerManager log file");

	return TRUE;
}
#endif	//TRIGGER_ENABLE