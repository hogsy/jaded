#ifndef XBPROFILING
#define XBPROFILING
#include <stdio.h>

#define	XBPROFILING_DEFAULT_LOGFILENAME "d:\\results.cap"

//-----------------------------------------------------------------------------
// Name: XBProfiling_IsFastCAPRequested()
// Desc: Returns TRUE if a FastCAP profile has been requested with the
// TriggerProfile tool on the PC. Typical usage would be to call this once
// frame and trigger a profile when it returns TRUE. This function will return
// TRUE once for each use of TriggerProfile
// Typical usage is like this:
//
//        CXBSimpleFastCapProfile profile;
//        if ( XBProfiling_IsFastCAPRequested() )
//            profile.BeginProfiling();
//
//       In release builds the code compiles away to nothing.
//-----------------------------------------------------------------------------
#ifdef  PROFILE_ENABLE
BOOL XBProfiling_IsFastCAPRequested();
#else
inline BOOL XBProfiling_IsFastCAPRequested() { return FALSE; }
#endif




//-----------------------------------------------------------------------------
// Name: XBProfiling_GetLogFileName()
// Desc: Get the log file name and path for the CAP profiling.
//-----------------------------------------------------------------------------
#ifdef  PROFILE_ENABLE
void XBProfiling_GetLogFileName(char *_sz_LogFileName);
#else
inline void XBProfiling_GetLogFileName(char *_sz_LogFileName) {}
#endif




//-----------------------------------------------------------------------------
// Name: class SimpleFastCapProfile
// Desc: Simple class to manage programmatic /fastcap captures.
//
//       The entire class is inline, so you can include it anywhere in your
// code.
//       To use it, declare an object of SimpleFastCapProfile type in your
// main loop. When you want to create a /fastcap capture, call
// BeginProfiling(). When the object leaves scope it's destructor will
// automatically call EndProfiling(), or you can call it manually.
//
// /fastcap captures only work if your code was built with the /fastcap
// compiler option.
//-----------------------------------------------------------------------------
class CXBSimpleFastCapProfile
{
public:
    inline CXBSimpleFastCapProfile()
    {
#ifdef  PROFILE_ENABLE
        m_amountOfMemory = 0;
		m_freq = 10000;
		mb_DumpProfilingData = FALSE;
		mb_ContinuousCapture = FALSE;
#endif
    }

    inline ~CXBSimpleFastCapProfile()
    {
        EndProfiling();
    }
	
	//-----------------------------------------------------------------------------
	// Name: IsContinuousCapture()
	// Desc: Continuous capture or not
	//-----------------------------------------------------------------------------
#ifdef  PROFILE_ENABLE
	BOOL IsContinuousCapture( )		   { return mb_ContinuousCapture; };
#else
	// Compile to nothing in release builds.
	inline BOOL IsContinuousCapture( ) { return FALSE; }
#endif

	//-----------------------------------------------------------------------------
	// Name: SetContinuousCapture()
	// Desc: Set Continuous capture modeS
	//-----------------------------------------------------------------------------
#ifdef  PROFILE_ENABLE
	void SetContinuousCapture( )		   { mb_ContinuousCapture = TRUE; };
#else
	// Compile to nothing in release builds.
	inline void SetContinuousCapture( ) {}
#endif

    //-----------------------------------------------------------------------------
    // Name: BeginProfiling()
    // Desc: Start /fastcap profiling. You can specify how much memory to use, or
    // you can accept the default and let the function adjust as needed if less
    // memory is available. 
    //     Call EndProfiling() or let the destructor call it at the end of the
    // frame.
    //-----------------------------------------------------------------------------
#ifdef  PROFILE_ENABLE
    void BeginProfiling( DWORD amountOfMemory = 32 * 1024 * 1024 );
#else
    // Compile to nothing in release builds.
	inline void BeginProfiling( DWORD amountOfMemory = 32 * 1024 * 1024 ) { }
#endif

    //-----------------------------------------------------------------------------
    // Name: EndProfiling()
    // Desc: End /fastcap profiling. Then you can manually copy the resulting
    // .cap file to your PC and use xbperfview, together with the .exe and .pdb
    // files, to analyze your performance.
    //-----------------------------------------------------------------------------
#ifdef  PROFILE_ENABLE
    void EndProfiling();
#else
    // Compile to nothing in release builds
    inline void EndProfiling() {}
#endif

	//-----------------------------------------------------------------------------
	// Name: SetProfilingOptions()
	// Desc: Establish the operating options for collection of profiling data.
	//-----------------------------------------------------------------------------
#ifdef  PROFILE_ENABLE
	void SetProfilingOptions( DWORD flags );
#else
	// Compile to nothing in release builds
	inline void SetProfilingOptions( DWORD flags ) {}
#endif

#ifdef PROFILE_ENABLE
	inline void DumpProfilingData()	{ mb_DumpProfilingData = TRUE; }
#else
	inline void DumpProfilingData()	{}
#endif

private:
#ifdef  PROFILE_ENABLE
    DWORD   m_amountOfMemory;

	// profiler interrupt handler
	HANDLE	m_handle;
	//frequency of the interrupt
	ULONG	m_freq;

	//default log filename
	char msz_FileName[256];

	BOOL mb_DumpProfilingData;
	BOOL mb_ContinuousCapture;
#endif

    // The copy constructor and assignment operator are private and
    // unimplemented to disallow object copying.
    CXBSimpleFastCapProfile& operator=(const CXBSimpleFastCapProfile& rhs);
    CXBSimpleFastCapProfile(const CXBSimpleFastCapProfile& rhs);
};

//-----------------------------------------------------------------------------
// This object encapsulates the PIXBeginNamedEvent/EndEvent commands, to ensure
// that they are always properly nested, while requiring a minimum of code
// modifications to add these profiling calls - just one line per event.
// This class completely disappears in release builds.
// Example usage:
// BeginEventObject eventObject( "Object %s", object->GetName() );
// BeginEventObject eventObject( D3DCOLOR_XRGB( 0xff,0xff,0x7f ), "FrameMove" );
//-----------------------------------------------------------------------------
class CXBBeginEventObject
{
public:
    //-----------------------------------------------------------------------------
    // Name: CXBBeginEventObject()
    // Desc: The constructors call PIXBeginNamedEvent and the destructor will
    // call PIXEndNamedEvent. This constructor takes just a color and a name.
    // The color is optional, and you can specify up to two additional parameters
    // for sprintf style formatting of the event label - the appropriate template
    // constructor will be automatically called.
    //-----------------------------------------------------------------------------
    inline CXBBeginEventObject(D3DCOLOR color, const char* name)
    {
#if defined(PIX_ENABLE)
        PIXBeginNamedEvent(color, name);
#endif
    }
    // This constructor is the same as the one above except that it
    // doesn't require the color parameter. A color will be chosen from a table.
    inline CXBBeginEventObject(const char* name)
    {
#if defined(PIX_ENABLE)
		PIXBeginNamedEvent(GetNextColor(), name);
#endif
    }



    // Template constructor to support the sprintf style interface to PIXBeginNamedEvent
    // This constructor is used for one sprintf parameter of any type.
    template <typename T>
    inline CXBBeginEventObject(D3DCOLOR color, const char* name, T data1)
    {
#if defined(PIX_ENABLE)
		PIXBeginNamedEvent(color, name, data1);
#endif
    }
    // This constructor is the same as the one above except that it
    // doesn't require the color parameter. A color will be chosen from a table.
    template <typename T>
    inline CXBBeginEventObject(const char* name, T data1)
    {
#if defined(PIX_ENABLE)
        PIXBeginNamedEvent(GetNextColor(), name, data1);
#endif
    }



    // Template constructor to support the sprintf style interface to PIXBeginNamedEvent
    // This constructor is used for two sprintf parameters of any types.
    template <typename T, typename T2>
    inline CXBBeginEventObject(D3DCOLOR color, const char* name, T data1, T2 data2)
    {
#if defined(PIX_ENABLE)
        PIXBeginNamedEvent(color, name, data1, data2);
#endif
    }
    // This constructor is the same as the one above except that it
    // doesn't require the color parameter. A color will be chosen from a table.
    template <typename T, typename T2>
    inline CXBBeginEventObject(const char* name, T data1, T2 data2)
    {
#if defined(PIX_ENABLE)
        PIXBeginNamedEvent(GetNextColor, name, data1, data2);
#endif
    }



    //-----------------------------------------------------------------------------
    // Name: ~CXBBeginEventObject()
    // Desc: The destructor for CXBBeginEventObject calls PIXEndNamedEvent to
    // ensure that Begin/End calls are always matched.
    //-----------------------------------------------------------------------------
    inline ~CXBBeginEventObject()
    {
#if defined(PIX_ENABLE)
        PIXEndNamedEvent();
#endif
    }

private:
#if defined(PIX_ENABLE)

    //-----------------------------------------------------------------------------
    // GetNextColor() simplifies the effective usage of BeginEventObject by
    // returning a sequence of colors, so that adjacent events in the timeline
    // are different colors. When the hierarchy is collapsed it may end up that
    // adjacent events are the same color, but with enough colors in the
    // s_colors array this should be rare. Some extra code could avoid this
    // possibility, but it's not really important.
    // The colors will be different on different captures.
    //-----------------------------------------------------------------------------
    inline D3DCOLOR GetNextColor() const
    {
        static int s_currentColor = 0;
        // Add all your favorite colors to this array.
        static D3DCOLOR s_colors[] =
        {
            D3DCOLOR_XRGB(0xFF, 0x00, 0x00),    // Red
            D3DCOLOR_XRGB(0x00, 0xFF, 0x00),    // Green
            D3DCOLOR_XRGB(0x00, 0x00, 0xFF),    // Blue
            D3DCOLOR_XRGB(0xFF, 0xFF, 0x00), // Red+Green makes ???
            D3DCOLOR_XRGB(0xFF, 0x00, 0xFF),    // Purple
            D3DCOLOR_XRGB(0x00, 0xFF, 0xFF),    // Green+Blue makes ???
        };
        ++s_currentColor;
        if ( s_currentColor >= ( sizeof(s_colors) / sizeof( s_colors[0] ) ) )
            s_currentColor = 0;
        return s_colors[s_currentColor];
    }
#endif

    // The copy constructor and assignment operator are private and
    // unimplemented to disallow object copying.
    CXBBeginEventObject(const CXBBeginEventObject& rhs);
    CXBBeginEventObject& operator=(const CXBBeginEventObject& rhs);
};

#ifdef USE_PIX_CPU
    #define CXBBeginEventObjectCPU      CXBBeginEventObject
#else
class CXBBeginEventObjectCPU
{
public:
    CXBBeginEventObjectCPU(D3DCOLOR color, const char* name) {};

    template <typename T>
    CXBBeginEventObjectCPU(D3DCOLOR color, const char* name, T data1) {};

    template <typename T>
    CXBBeginEventObjectCPU(const char* name, T data1) {};

    template <typename T, typename T2>
    CXBBeginEventObjectCPU(D3DCOLOR color, const char* name, T data1, T2 data2) {};

    template <typename T, typename T2>
    CXBBeginEventObjectCPU(const char* name, T data1, T2 data2) {};
};
#endif


extern __declspec(align(32)) CXBSimpleFastCapProfile g_oXeProfileManager;

#endif  // XBPROFILING
