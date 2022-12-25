// Side (Simple IDE) - Consol execution process class - LTRowe - 30/11/01 19:40

#ifndef _CONSPROC_H_
#define _CONSPROC_H_

namespace P4
{
//Includes
//--------

//DEFINES
//-------------
#define CONSPROC_ERROR_EXIT  0xFFFFFF00

#define MaxPipeSizeOut 2048
#define MaxPipeSizeErr 512
#define MaxLogBufSizeOut (MaxPipeSizeOut/2)
#define MaxLogBufSizeErr (MaxPipeSizeErr/2)

//Class definition
//----------------
class ConsProc
{
protected:
	bool m_isOnGoing; //Bool indicating if we are running
	void *m_log;       //Pointer to log window where to redirect the output
	HANDLE m_hEnd;    //Execution synchro event (returned by Execute)
	HANDLE m_hStdIn, m_hStdOut, m_hStdErr;              //To be put on the created process
	HANDLE m_hStdInWrite, m_hStdOutRead, m_hStdErrRead; //To be use to get the outputs
	PROCESS_INFORMATION m_processInfo;  //Given by create process - contains the process information
	HANDLE m_hThread; //thread handle

public:
	ConsProc(): m_isOnGoing (false) {};
	~ConsProc();

	//Execute the application a send the ouput to the log window
	//Return a handle to wait on
	HANDLE Execute(char *application, char *commandLine, char *directory, void *log);

	//Execute the application without sending a log
	//Return a handle to wait on
	HANDLE ConsProc::ExecuteSimple(char *application, char *commandLine, char *directory);

	//Wait for the end of the process 
	//return the error code
	unsigned long Terminate();

	//Kill process
	void Kill();

  void InputPipe(char *_s);

	//Thread loop
	void Thread();

private:
	char m_bufOut[MaxLogBufSizeOut];
	char m_bufErr[MaxLogBufSizeErr];
	unsigned long m_offsetOut;
	unsigned long m_offsetErr;


	//Used to read the data on the pipe and output on the log
	void OutputToLog();
};

}

#endif//CONSPROC