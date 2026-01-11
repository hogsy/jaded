/**********************************************************************
 *<
	FILE: assert1.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifdef assert
#undef assert
#endif

// Commenting out line below for Luna Release - IB 6/18/02
//#define _SHOW_DEBUG_ASSERTIONS_IN_RELEASE

#define assert( expr ) ( expr || assert1( /*#expr,*/ __LINE__, __FILE__ ) )

#define MaxAssert( expr ) ( (expr) || assert1( __LINE__, __FILE__ ) )

extern int UtilExport assert1( /*char *expr,*/ int line, char *file );

#if defined(_DEBUG)
#define DbgAssert( expr ) ( (expr) || assert1( __LINE__, __FILE__ ) )
#define DbgVerify( expr ) ( (expr) || assert1( __LINE__, __FILE__ ) )
#elif defined(_SHOW_DEBUG_ASSERTIONS_IN_RELEASE)
#define DbgAssert( expr ) ( (expr) || assert1( __LINE__, __FILE__ ) )
#define DbgVerify( expr ) ( (expr) || assert1( __LINE__, __FILE__ ) )
#else	// _SHOW_DEBUG_ASSERTIONS_IN_RELEASE
#define DbgAssert( expr )
#define DbgVerify( expr ) ( expr )
#endif	// _DEBUG


//-----------------------------------------------------------------------------
//-- Assert callback mechanism. (GG:06/27/02)

#ifndef CASSERTCB_CLASS
#define CASSERTCB_CLASS

enum {
	ASSERTCB_IGNORE,		//-- Ignore the assert and continue
	ASSERTCB_CANCEL,		//-- Cancel the whole thing. Max will be brought down.
	ASSERTCB_CONTINUE		//-- Proceed with the normal assert code (dialogue and whatnot)
};

class CAssertCB {
	public:
		virtual	int Assert(int line, char* file)=0;
};

UtilExport CAssertCB* SetAssertCB(CAssertCB* cb);
#endif