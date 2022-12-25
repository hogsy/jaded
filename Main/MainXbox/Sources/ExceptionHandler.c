/* 
	Exception Handler
	Handle the exception when the debugger is not active.

	08/09/03	Tiziano Sardone, Ubisoft Italy

*/

#include "Precomp.h"
#include <stdio.h>
//#include <wchar.h>
#include "Xtl.h"
#include "XbDm.h"
//#undef UNICODE
/*
#if defined( _FINAL_ )
#undef _FINAL_
#endif
*/
#define DEMO
#define EXCEPTION_STRING_BUFFER_SIZE		1024
#define EXCEPTION_STRING_BUFFER_TMP_SIZE	256

extern void WriteStringEx(WCHAR *pStr, float posx, float posy, DWORD color);
extern LPDIRECT3DDEVICE8 Gx8_GetDevice(void);
//extern WINBASEAPI LONG WINAPI MyExceptionHandlerRoutines(struct _EXCEPTION_POINTERS *ExceptionInfo);

LPDIRECT3DDEVICE8 DX_Device = NULL;

WCHAR	gStringBuffer[EXCEPTION_STRING_BUFFER_SIZE];
WCHAR	gExcTmp[EXCEPTION_STRING_BUFFER_TMP_SIZE];

#define Ex8_ForceCopy()	{IDirect3DDevice8_Swap(DX_Device, D3DSWAP_COPY);}
#define Ex8_ForceFinish() {IDirect3DDevice8_Swap(DX_Device, D3DSWAP_FINISH);}
#define Ex8_ForceClear() {IDirect3DDevice8_Clear( DX_Device, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0xFF00FF00, 1.0f, 0 );}


WINBASEAPI
long 
WINAPI
MyExceptionHandlerRoutines(EXCEPTION_POINTERS *ExceptionInfo)
{
#if !defined(_FINAL_) && !defined(DEMO)
	DWORD	LocalColor = 0xFF00FF00;
	EXCEPTION_RECORD *pExceptionRecord = ExceptionInfo->ExceptionRecord;

	//Setup the header:
	wcscpy(gStringBuffer,L"- XBOX - SLIP MEDITATION -\n");

/*	while(1)
	{
		Ex8_ForceCopy();
		Ex8_ForceFinish();
		Ex8_ForceClear();
		Ex8_ForceClear();
	}
	*/

	//Now start with ExceptionCode handling
	switch(ExceptionInfo->ExceptionRecord->ExceptionCode)
	{
		case EXCEPTION_ACCESS_VIOLATION:	
			wcscat(gStringBuffer,L"The thread attempted to read from or write to \na virtual address for which it does not have \nthe appropriate access.\n\0");
			LocalColor = 0xffff0000;
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			wcscat(gStringBuffer,L"The thread attempted to access an array element that is out of bounds,\nand the underlying hardware supports bounds checking.\n");
			LocalColor = 0xffaaaa00;
			break;
		case EXCEPTION_BREAKPOINT:
			wcscat(gStringBuffer,L"A breakpoint was encountered.\n");
			LocalColor = 0xffaaaaaa;
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			wcscat(gStringBuffer,L"One of the operands in a floating-point operation is denormal.\nA denormal value is one that is too small to represent as a\nstandard floating-point value.\n");
			LocalColor = 0xffffaa00;
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			wcscat(gStringBuffer,L"The thread attempted to divide a floating-point value by\na floating-point divisor of zero.\n");
			LocalColor = 0xff00ff00;
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			wcscat(gStringBuffer,L"The result of a floating-point operation cannot be represented\nexactly as a decimal fraction.\n");
			LocalColor = 0xff88ffaa;
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			wcscat(gStringBuffer,L"This exception represents any floating-point exception\nnot included in this list.\n");
			LocalColor = 0xff0088ff;
			break;
		case EXCEPTION_FLT_OVERFLOW:
			wcscat(gStringBuffer,L"The exponent of a floating-point operation is greater than\nthe magnitude allowed by the corresponding type.\n");
			LocalColor = 0xff88ff00;
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			wcscat(gStringBuffer,L"The stack overflowed or underflowed as the result of a floating-point operation.\n");
			LocalColor = 0xff888888;
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			wcscat(gStringBuffer,L"The exponent of a floating-point operation is less than\nthe magnitude allowed by the corresponding type.\n");
			LocalColor = 0xff8a8a8a;
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			wcscat(gStringBuffer,L"The thread tried to execute an invalid instruction.\n");
			LocalColor = 0xff0000ff;
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			wcscat(gStringBuffer,L"The thread tried to divide an integer value by an integer divisor of zero.\n");
			LocalColor = 0xff00ffff;
			break;
		case EXCEPTION_INT_OVERFLOW:
			wcscat(gStringBuffer,L"The result of an integer operation caused a carry out\nof the most significant bit of the result.\n");
			LocalColor = 0xffabcdef;
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			wcscat(gStringBuffer,L"An exception handler returned an invalid disposition\nto the exception dispatcher.\nProgrammers using a high-level language such as C should never encounter this exception.\n");
			LocalColor = 0xfffedcba;
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			wcscat(gStringBuffer,L"The thread tried to continue execution\nafter a noncontinuable exception occurred.\n");
			LocalColor = 0xff012345;
			break;
		case EXCEPTION_SINGLE_STEP:
			wcscat(gStringBuffer,L"A trace trap or other single-instruction mechanism\nsignaled that one instruction has been executed.\n");
			LocalColor = 0xff56789a;
			break;
		default:
			wcscat(gStringBuffer,L"Unhandled exception");
			LocalColor = 0xffaa0000;
			break;
	}


	//separator
	wcscat(gStringBuffer,L"-----------------------------------------------------------------------------------------------------\n");

	//Record dump: may be usefull if nasted exception are called
	wsprintfW(gExcTmp,L"Exception record: 0x%p\n",pExceptionRecord->ExceptionRecord);
	wcscat(gStringBuffer,gExcTmp);

	//Address of the exception: very usefull
	wsprintfW(gExcTmp,L"Exception at: 0x%p\n",(pExceptionRecord->ExceptionAddress));
	wcscat(gStringBuffer,gExcTmp);

	//Type of exception: during read or write
	if(pExceptionRecord->ExceptionInformation[0]==0)
	{
		wsprintfW(gExcTmp,L"%p: Exception raised trying to READ address: 0x%p\n",pExceptionRecord->ExceptionCode,(pExceptionRecord->ExceptionInformation[1]));
		wcscat(gStringBuffer,gExcTmp);
	}
	else
	{
		wsprintfW(gExcTmp,L"%p: Exception raised trying to WRITE address: 0x%p\n",pExceptionRecord->ExceptionCode,(pExceptionRecord->ExceptionInformation[1]));
		wcscat(gStringBuffer,gExcTmp);
	}

	//This will dump the rest of the parameters. Actually not needed
/*	for(i=0; i<EXCEPTION_MAXIMUM_PARAMETERS; i++)
	{
		wsprintfW(gExcTmp,L"virtual address: %p\n",pExceptionRecord->ExceptionInformation[i]);
		wcscat(gStringBuffer,gExcTmp);
	}
*/


	{
		//Dump the exception
		Ex8_ForceCopy();
		WriteStringEx(gStringBuffer,10+2, 20+1, 0xff000000);
		WriteStringEx(gStringBuffer,10, 20, LocalColor);
		Ex8_ForceFinish();
//		Gx8_ForceSwap();

	}

//	return EXCEPTION_CONTINUE_SEARCH;
//	return EXCEPTION_CONTINUE_EXECUTION;
//#else
//	DmReboot(DMBOOT_WARM|DMBOOT_NODEBUG);
#endif
	return EXCEPTION_EXECUTE_HANDLER;
}

void InitExceptionHandler(void)
{
#if !defined( _FINAL_ ) && !defined(DEMO)
//#if	!defined(DEMO)
	LPTOP_LEVEL_EXCEPTION_FILTER PrevHandler;
	//Set up the new exception handler
	PrevHandler = SetUnhandledExceptionFilter(&MyExceptionHandlerRoutines);

	if(!PrevHandler)
	{
		Ex8_ForceCopy();
		WriteStringEx(L"No Previous Handler installed", 10.0f, 10.0f, 0xffff00ff);
		Ex8_ForceFinish();
	}

	{
		//Clean the exception buffers
		memset(gStringBuffer,0,sizeof(WCHAR)*EXCEPTION_STRING_BUFFER_SIZE);
		memset(gExcTmp,0,sizeof(WCHAR)*EXCEPTION_STRING_BUFFER_TMP_SIZE);
#if !defined(_FINAL_) && !defined(DEMO)
		DX_Device = Gx8_GetDevice();

		//show that the exception is installed
		Ex8_ForceCopy();
		WriteStringEx(L"Exception Handler installed", 10.0f, 10.0f, 0xffffffff);
		Ex8_ForceFinish();
#endif
	}

#endif

}