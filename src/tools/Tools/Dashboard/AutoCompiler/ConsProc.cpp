// Side (Simple IDE) - Console execution process class - LTRowe - 30/11/01 19:40

//Includes
//------------------------------------------------------------------------------------------
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#include "ConsProc.h"

namespace P4
{

//Defines
//------------------------------------------------------------------------------------------
#define SIDE_ERROR(x) {MessageBox(NULL, x, "Perforce Client Log Error", MB_OK | MB_ICONERROR);}
#define SIDE_CHECK(x) \
{if((BOOL)x == FALSE) \
{\
	int error = GetLastError();\
	char buffer[1024];\
  char errormessage[1024];\
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,error,GetSystemDefaultLangID(),errormessage,1024,NULL);\
	sprintf(buffer, "Executing the line : "#x);\
  sprintf(buffer, "%s : \n\nError (%d) : %s", buffer, error, errormessage);\
	MessageBox(NULL, buffer, "Perforce Error", MB_OK | MB_ICONERROR);\
	return NULL;\
}}


//Forward decleration
//------------------------------------------------------------------------------------------
DWORD WINAPI ConsProcThreadProc(LPVOID lpParameter);
 
//Methods
//------------------------------------------------------------------------------------------

//Execute the application a send the ouput to the log window
//Return a handle to wait on
//----------------------
HANDLE ConsProc::Execute(char *application, char *commandLine, char *directory , void *log)
{
	STARTUPINFO startupInfo;
	HANDLE hCurrentProcess = GetCurrentProcess();
  SECURITY_ATTRIBUTES saAttr;  
 	HANDLE hStdInWriteTmp, hStdOutReadTmp, hStdErrReadTmp;//Temporary handles used for duplication
	unsigned long threadID;

	//Check params
	//------------
	if(/*(application == NULL) ||*/ (commandLine == NULL)) return NULL;
	m_log = log; //log can be null -> no output
	//If we are allready running return null
	if(m_isOnGoing) return NULL;
	//Create the synchro event
	m_hEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	SIDE_CHECK(m_hEnd);


	//Init the startup info struct
	//----------------------------
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
    startupInfo.lpTitle = NULL; 
	startupInfo.dwFlags = 0;
	// Use this if you want to show or hide the child.
	startupInfo.dwFlags    |= STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_HIDE;

	//Create pipes that'll be used to redirect the output
	//---------------------------------------------------
    // Set the bInheritHandle flag so pipe handles are inherited. 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

	// Create the child's process stdout and stderr pipe.
	SIDE_CHECK(CreatePipe(&m_hStdIn,  &hStdInWriteTmp, &saAttr, 0));
	SIDE_CHECK(CreatePipe(&hStdOutReadTmp, &m_hStdOut, &saAttr, MaxPipeSizeOut));
	SIDE_CHECK(CreatePipe(&hStdErrReadTmp, &m_hStdErr, &saAttr, MaxPipeSizeErr));

	// Create new stdout and stderr read handles.
	// Set the properties to FALSE. Otherwise, the child inherits the
	// properties and, as a result, non-closeable handles to the pipes
	// are created.	
	SIDE_CHECK(DuplicateHandle(hCurrentProcess, hStdInWriteTmp, hCurrentProcess, &m_hStdInWrite, 0, FALSE, DUPLICATE_SAME_ACCESS));
	SIDE_CHECK(DuplicateHandle(hCurrentProcess, hStdOutReadTmp, hCurrentProcess, &m_hStdOutRead, 0, FALSE, DUPLICATE_SAME_ACCESS));
	SIDE_CHECK(DuplicateHandle(hCurrentProcess, hStdErrReadTmp, hCurrentProcess, &m_hStdErrRead, 0, FALSE, DUPLICATE_SAME_ACCESS));

	// Close inheritable copies of the handles you do not want to be inherited.
	SIDE_CHECK(CloseHandle(hStdInWriteTmp));
	SIDE_CHECK(CloseHandle(hStdOutReadTmp));
	SIDE_CHECK(CloseHandle(hStdErrReadTmp));

	//Setup teh startup struct
  startupInfo.dwFlags   |= STARTF_USESTDHANDLES; 
	startupInfo.hStdInput  = m_hStdIn; 
  startupInfo.hStdOutput = m_hStdOut; 
  startupInfo.hStdError  = m_hStdErr; 

	// On NT/2000 the handle must have PROCESS_QUERY_INFORMATION access.
	// This is made using an empty security descriptor. It is not the same
	// as using a NULL pointer for the security attribute!
	//----------------------------------------------------
	SECURITY_DESCRIPTOR  sd;
	SECURITY_ATTRIBUTES sa;

	SIDE_CHECK(InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION));
	SIDE_CHECK(SetSecurityDescriptorDacl(&sd, -1, 0, 0));

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = TRUE;

	//Create the proces
	//-----------------
	SIDE_CHECK(CreateProcess(application, commandLine, &sa, NULL, TRUE, CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS, NULL, directory, &startupInfo, &m_processInfo));

	//Create the reading thread
	//-------------------------
	m_isOnGoing = true;
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConsProcThreadProc, (LPVOID)this, 0, &threadID);
  SetThreadPriority(m_hThread,THREAD_PRIORITY_HIGHEST);

	return m_hEnd;
}

//Execute in a diferent console (No redirection)
//----------------------
HANDLE ConsProc::ExecuteSimple(char *application, char *commandLine, char *directory)
{
	STARTUPINFO startupInfo;
	unsigned long threadID;

	//Check params
	//------------
	if((application == NULL) || (commandLine == NULL)) return NULL;
	//Set the log to NULL -> No output
	m_log = NULL;
	//If we are allready running return null
	if(m_isOnGoing) return NULL;
	//Create the synchro event
	m_hEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	SIDE_CHECK(m_hEnd);

	//Put all unnecessary stuff to NULL (no redirection)
	//----------------------------------
	m_hStdIn  = NULL;
	m_hStdOut = NULL;
	m_hStdErr = NULL;
	m_hStdInWrite = NULL;
	m_hStdOutRead = NULL;
	m_hStdErrRead = NULL;

	//Init the startup info struct
	//----------------------------
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
  startupInfo.lpTitle = NULL; 
	startupInfo.dwFlags = 0;
	// Use this if you want to show or hide the child.
	startupInfo.dwFlags    |= STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_SHOW;

	// On NT/2000 the handle must have PROCESS_QUERY_INFORMATION access.
	// This is made using an empty security descriptor. It is not the same
	// as using a NULL pointer for the security attribute!
	//----------------------------------------------------
	PSECURITY_DESCRIPTOR  sd;
	SECURITY_ATTRIBUTES sa;

	SIDE_CHECK(InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION));
	SIDE_CHECK(SetSecurityDescriptorDacl(&sd, -1, 0, 0));

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = TRUE;

	//Create the proces
	//-----------------
//	SIDE_CHECK(CreateProcess(application, commandLine, &sa, NULL, TRUE, CREATE_NEW_CONSOLE | HIGH_PRIORITY_CLASS, NULL, directory, &startupInfo, &m_processInfo));
	SIDE_CHECK(CreateProcess(application, commandLine, &sa, NULL, TRUE, CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS, NULL, directory, &startupInfo, &m_processInfo));

	m_isOnGoing = true;
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConsProcThreadProc, (LPVOID)this, 0, &threadID);
  SetThreadPriority(m_hThread,THREAD_PRIORITY_HIGHEST);

	return m_hEnd;
}

//Reading Thread Function
//-----------------------
DWORD WINAPI ConsProcThreadProc(LPVOID lpParameter)
{
	((ConsProc *)lpParameter)->Thread();

  ExitThread(0);
	return 0;
}

//Thread loop
//-----------
void ConsProc::Thread()
{
	//Read the process output ...
  m_offsetOut = 0;
  m_offsetErr = 0;
	do
	{
		Sleep(1);
		OutputToLog();
	}
	while(WaitForSingleObject(m_processInfo.hProcess, 1) != WAIT_OBJECT_0);

	//Ouput one last time in case there is data left in pipes
	Sleep(100);
	OutputToLog();

	//Tell people that we are finished
	m_isOnGoing = false;
	SetEvent(m_hEnd);
}

void ConsProc::InputPipe(char *_s)
{
  DWORD l;
  //printf(_s);
  WriteFile(m_hStdInWrite,_s,strlen(_s),&l,0);
}

//Used to read the data on the pipe and output on the log
//--------------------------------------------------------
void ConsProc::OutputToLog()
{
	unsigned long nbRead;
	unsigned long nbTotalInBuffer;
	unsigned long pos;
	unsigned long lastPos;
	unsigned long totalBytesAvail;
  char output[MaxLogBufSizeOut+100];
  unsigned long nbLogLines;

	//If there is a log check for the output
	//if(m_log != NULL)
	{
		//Init
		pos = 0;
		lastPos = 0;
    nbLogLines = 0;

		//Standard output
		//---------------
		//Check if any data is in the pipe (This is necessary because if we do a read on a empty pipe, it'll block)
		if((PeekNamedPipe(m_hStdOutRead, NULL, NULL, NULL, &totalBytesAvail, NULL) == TRUE) && (totalBytesAvail != 0))
		{
			//Ok read that available data
			if(ReadFile( m_hStdOutRead, m_bufOut + m_offsetOut, MaxLogBufSizeOut-m_offsetOut, &nbRead, NULL) && nbRead != 0)  
			{
        //Read data - analyse
        nbTotalInBuffer = nbRead + m_offsetOut;
				while(pos < nbTotalInBuffer && nbLogLines < 3)
				{
					while((pos < nbTotalInBuffer) && (m_bufOut[pos] != '\n') && (m_bufOut[pos] != '\r')) pos++;
					if((pos < nbTotalInBuffer) && ((m_bufOut[pos] == '\n') || (m_bufOut[pos] == '\r')))
					{
						m_bufOut[pos++] = 0;
						if(m_log != NULL) 
            {
              sprintf(output,"%s",m_bufOut + lastPos);
              //m_log->vAddTextWithAnalyse(output);
              nbLogLines++;
            }
            else
            {
              fprintf(stdout,"%s\n",m_bufOut + lastPos);
            }
						m_offsetOut = pos;
						//Skip all other contiguous \r or \n
						while((pos < nbTotalInBuffer) && ((m_bufOut[pos] == '\n') || (m_bufOut[pos] == '\r'))) pos++;
						m_offsetOut = pos;
            lastPos = pos;
					}
				}

        if (m_offsetOut==nbTotalInBuffer)
        {
          m_offsetOut = 0;
        }
        else
        {
				  memmove(m_bufOut, m_bufOut + m_offsetOut, nbTotalInBuffer - m_offsetOut);
				  m_offsetOut = nbTotalInBuffer - m_offsetOut;
        }
			}

			if(m_offsetOut == MaxLogBufSizeOut)
			{
        memcpy(output,m_bufOut,MaxLogBufSizeOut);
        output[MaxLogBufSizeOut] = 0;
        //m_log->vAddText(output);
        m_offsetOut = 0;
			}
		}

		//Reinit
		pos = 0;
		lastPos = 0;
    nbLogLines = 0;

    //Error ouput
		//-----------
		//Check if any data is in the pipe (This is necessary because if we do a read on a empty pipe, it'll block)
		if((PeekNamedPipe(m_hStdErrRead, NULL, NULL, NULL, &totalBytesAvail, NULL) == TRUE) && (totalBytesAvail != 0))
		{
			//Ok read that available data
			if(ReadFile( m_hStdErrRead, m_bufErr + m_offsetErr, MaxLogBufSizeErr-m_offsetErr, &nbRead, NULL) && nbRead != 0)  
			{
				 //Read data - analyse
        nbTotalInBuffer = nbRead + m_offsetOut;
				while(pos < nbTotalInBuffer && nbLogLines < 100)
				{
					while((pos < nbTotalInBuffer) && (m_bufErr[pos] != '\n') && (m_bufErr[pos] != '\r')) pos++;
					if((pos < nbTotalInBuffer) && ((m_bufErr[pos] == '\n') || (m_bufErr[pos] == '\r')))
					{
						m_bufErr[pos++] = 0;
						if(m_log != NULL)
            {
              sprintf(output,"{c2020a0}%s{p}",m_bufErr + m_offsetErr);
              //m_log->vAddTextWithAnalyse(output);
              nbLogLines++;
            }
            else
            {
              fprintf(stdout,"%s\n",m_bufErr + m_offsetErr);
            }

						m_offsetErr = pos;
						//Skip all other contiguous \r or \n
						while((pos < nbTotalInBuffer) && ((m_bufErr[pos] == '\n') || (m_bufErr[pos] == '\r'))) pos++;
						m_offsetErr = pos;
            lastPos = pos;
					}
				}

        if (m_offsetErr==nbTotalInBuffer)
        {
          m_offsetErr = 0;
        }
        else
        {
          memmove(m_bufErr, m_bufErr + m_offsetErr, nbTotalInBuffer - m_offsetErr);
				  m_offsetErr = nbTotalInBuffer - m_offsetErr;
        }
			}

      //*** The buffer is filled without any cr and cl.
			if(m_offsetErr == MaxLogBufSizeErr)
			{
        strcpy(output,"{c2020a0}");
        memcpy(output+9,m_bufErr,MaxLogBufSizeErr);
        output[MaxLogBufSizeErr+9] = 0;
        strcat(output,"{p}");
        //m_log->vAddText(output);
        m_offsetErr = 0;
			}
		}
	}
}

//Wait for the end of the process 
//return the error code
//---------------------
unsigned long ConsProc::Terminate()
{
	unsigned long exitCode;
	
	//If ongoing wait
	//---------------
	if(m_isOnGoing)
	{
		while(WaitForSingleObject(m_hEnd, 100) != WAIT_OBJECT_0) 
    {
      Sleep(1);
    }
	}

	//Get the return code
	//-------------------
	SIDE_CHECK(GetExitCodeProcess(m_processInfo.hProcess, &exitCode));
	if(exitCode == STILL_ACTIVE)
	{
		SIDE_CHECK("exitCode == STILL_ACTIVE");
	}

	// Close the thread and process handle
	if(m_hThread != NULL) SIDE_CHECK(CloseHandle(m_hThread));
	SIDE_CHECK(CloseHandle(m_processInfo.hThread));
	SIDE_CHECK(CloseHandle(m_processInfo.hProcess));

	//Desinit all
	//-----------
	SIDE_CHECK(CloseHandle(m_hEnd));
	if(m_hStdIn  != NULL) SIDE_CHECK(CloseHandle(m_hStdIn));
	if(m_hStdOut != NULL) SIDE_CHECK(CloseHandle(m_hStdOut));
	if(m_hStdErr != NULL) SIDE_CHECK(CloseHandle(m_hStdErr));
	if(m_hStdInWrite != NULL) SIDE_CHECK(CloseHandle(m_hStdInWrite));
	if(m_hStdOutRead != NULL) SIDE_CHECK(CloseHandle(m_hStdOutRead));
	if(m_hStdErrRead != NULL) SIDE_CHECK(CloseHandle(m_hStdErrRead));
	Sleep(100);

	return exitCode;
}

//Destrcutor
//----------
ConsProc::~ConsProc()
{
	//If OnGoing
	if(m_isOnGoing)
	{
		Kill();
		Terminate();
	}
}

//Kill process
//------------
void ConsProc::Kill()
{
	if(m_isOnGoing)
	{
		if(!TerminateProcess(m_processInfo.hProcess, CONSPROC_ERROR_EXIT))
		{
			SIDE_ERROR("Can't terminate process");
			return;
		}
		while(WaitForSingleObject(m_processInfo.hProcess, 100) != WAIT_OBJECT_0) 
    {
      Sleep(1);
    }
	}
}

}