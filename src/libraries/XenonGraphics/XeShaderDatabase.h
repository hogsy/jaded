// ------------------------------------------------------------------------------------------------
// File   : XeShaderDatabase.h
// Date   : 2005-08-19
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
//
// SC: Note: All the functions named Thread_*() are executed by the shader compilation threads and
//           the others are executed by the main thread
//
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XESHADERDATABASE_H
#define GUARD_XESHADERDATABASE_H

// ------------------------------------------------------------------------------------------------
// TYPES
// ------------------------------------------------------------------------------------------------

struct XeShaderMacro
{
    union
    {
        struct
        {
            ULONG ulVSNoTexCoord   : 1;
            ULONG ulVSNoNormal     : 1;
            ULONG ulVSNoColor      : 1;
            ULONG ulVSBonesConfig  : 3;
            ULONG ulVSLightsConfig : 3;

            ULONG ulUnused         : 23;
        };
        ULONG ulRaw;
    };

    ULONG64 ulVSOutputSemantics;

    void Clear(void) { ulRaw = 0; ulVSOutputSemantics = 0; }
};

// ------------------------------------------------------------------------------------------------
// CLASSES
// ------------------------------------------------------------------------------------------------

class XeShaderInclude : public ID3DXInclude
{
public:

    XeShaderInclude(void);
    ~XeShaderInclude(void);

    HRESULT __stdcall Open(D3DXINCLUDE_TYPE IncludeType, 
                           LPCSTR           pFileName, 
                           LPCVOID          pParentData, 
                           LPCVOID*         ppData, 
                           UINT*            pBytes
#ifndef ACTIVE_EDITORS
						   ,
						   LPSTR pFullPath,
						   DWORD cbFullPath
#endif
						   );
    HRESULT __stdcall Close(LPCVOID pData);
};

class XeShaderDatabase
{
private:

    enum
    {
        DB_MAX_NAME             = 512,                          // Maximum length of the database name
        DB_GRANULARITY          = 64,                           // FAT increment

        SHADER_MAX_NAME         = 64,                           // Maximum name length for a shader
        SHADER_MAX_FILES        = 64,                           // Maximum number of shader files

        SHADER_VERTEX           = 0x00000001,                   // Vertex shader
        SHADER_PIXEL            = 0x00000002,                   // Pixel shader
        SHADER_BOTH             = SHADER_VERTEX | SHADER_PIXEL,

        REQUEST_MAX             = 1024,                         // Maximum number of requests in the compilation thread

#if defined(ACTIVE_EDITORS)
        FRAME_CREATE_MAX        = 512,                          // Maximum number of shaders create each frame -> Tick()
#else
        FRAME_CREATE_MAX        = 8,                            // Maximum number of shaders create each frame -> Tick()
#endif

        THREAD_EVENT_STOP       = 0,                            // Event to stop the compilation thread
        THREAD_EVENT_REQUEST    = 1,                            // Semaphore for compilation requests
        THREAD_EVENT_COUNT      = 2,                            // Number of events

        MASK_ALL                = 0xffffffff,                   // Validation mask

        FAT_MAX_ENTRIES         = 8192,                         // Maximum number of entries in the FAT
    };

#pragma pack(push, 1)
    struct DBHeader
    {
        ULONG ulMagic;
        ULONG ulVersion;
        ULONG ulNbShaders;
        ULONG ulNbFATEntries;
        ULONG ulShadersOffset;
        ULONG ulFATOffset;
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct ShaderEntry
    {
        CHAR  szFileName[SHADER_MAX_NAME];
        ULONG ulFileCRC;
        ULONG ulValidityMask;
        ULONG ulShaderType;
        ULONG ulFileSize;
    };
#pragma pack(pop)

    struct ShaderFile
    {
        CHAR  szFileName[SHADER_MAX_NAME];
        ULONG ulFileCRC;
        ULONG ulValidityMask;
        ULONG ulShaderType;
        void* pFileBuffer;
        ULONG ulFileSize;
    };

#pragma pack(push, 1)
    struct FATEntry
    {
        ULONG    ulItemType;
        ULONG128 ulKey;
        ULONG64  ulUserData;
        ULONG    ulFileOffset;
        ULONG    ulFileSize;
    };
#pragma pack(pop)

    struct CompilationRequest
    {
        ULONG         ulShaderType;
        ULONG128      ulShaderKey;
        CHAR          szEntryString[512];
        CHAR          szEntryPoint[32];
        ULONG         ulCompilationFlags;
        XeShaderMacro oMacros;
    };

    struct CompiledShader
    {
        ULONG               ulShaderType;
        ULONG128            ulShaderKey;
        ID3DXBuffer*        pBuffer;
        ID3DXBuffer*        pBufferPatched;
        ID3DXConstantTable* pConstantTable;
        ULONG64             ulShaderSemantics;
    };

public:

    enum ItemType
    {
        ItemVertexDeclaration = 0,
        ItemVertexShader      = 1,
        ItemPixelShader       = 2,

        ItemCount,

        ItemForceDword        = 0x7fffffff
    };

    XeShaderDatabase(void);
    ~XeShaderDatabase(void);

    void Initialize(void);
    void OnDeviceLost(void);
    void OnDeviceReset(void);
    void Shutdown(void);

    void InitializeCompilationThread(void);
    void ShutdownCompilationThread(void);

    void AddElement(ItemType _eType, const ULONG128& _ulKey, const void* _pBuffer, ULONG _ulSize, ULONG64 _ulUserData = 0 );
    void UnloadShaders(void);

    bool GetShaderFile(const CHAR* _pszName, const void** _ppBuffer, UINT* _puiSize);

    void RequestVertexShader(const ULONG128& _ulKey, 
                             const CHAR*     _pszEntryString, 
                             const CHAR*     _pszEntryPoint,
                             ULONG           _ulFlags,
                             XeShaderMacro   _oMacros);

    void RequestPixelShader(ULONG64     _ulKey,
                            const CHAR* _pszEntryString,
                            const CHAR* _pszEntryPoint,
                            ULONG       _ulFlags);

#ifdef _XENON
    // Vertex Shader patching on Xenon
    ULONG64  ComputePSInputSemantics( DWORD * _pShaderStream );
    void     PatchVertexShader( DWORD * _pShaderStream, ID3DXBuffer** _ppPatchedShader, ULONG64 _ulVSOutputSemantics );
    void     PatchVertexShaderSemanticDeclaration( CHAR* _strOutput, LONG _lUsage, LONG _lOutputRegister, ULONG _ulMask );
    void     ParseVertexShaderDeclarationLine( const char * _pLineStart, LONG _lLineSize, LONG & _lOuputRegister, LONG & _lUsage );
#endif

    XeVertexShader* GetDefaultVertexShader() { return m_pDefaultVertexShader; }

    inline void Tick(void)
    {
#if !defined(_FINAL_)
        DisplayStatus();
#endif

        // SC: Note: Function is inlined since it is called every frame.
        //           Not using the critical section to read the value because it is faster and 
        //           I don't care if I miss one, since the processing will be done on next frame.
        if (m_ulNbCompiledShaders == 0)
            return;

        CreateSomeShaders();
    }

private:

    void OpenDatabase(void);

#if !defined(_FINAL_)
    void CreateDatabase(void);
#endif

    void ReadDatabase(void);

#if !defined(_FINAL_)
    void MergeDatabase(ULONG _ulVSValidityMask, ULONG _ulPSValidityMask, BOOL _bPreLoadResources = TRUE);
#endif

    void LoadAllShaderFiles(void);
    void UnloadAllShaderFiles(void);

    void LoadShaderFile(const CHAR* _pszShaderName, ULONG _ulShaderFlags, ULONG _ulValidityMask);

    ShaderFile* AddShaderFile(const CHAR* _pszShaderName, void* _pFileBuffer, ULONG _ulFileSize, ULONG _ulShaderFlags, ULONG _ulValidityMask);

    const CHAR* GetShadersPath(void);

    BOOL ReadData(void* _pBuffer, DWORD _dwSize);

    void ReadShaderEntries(ULONG* _pulVSValidity, ULONG* _pulPSValidity);

    ShaderFile* FindShader(const CHAR* _pszName);

#if !defined(_FINAL_)

    void WriteData(void* _pBuffer, DWORD _dwSize);

    void WriteHeader(void);
    void WriteShaderEntries(void);
    void WriteLastFATEntry(void);
    void WriteFAT(void);

#endif

    ULONG ComputeShaderEntriesSize(void) const;

    const CHAR* GetVSCompilationTarget(void) const;
    const CHAR* GetPSCompilationTarget(void) const;

    void CreateDefaultShaders(void);
    void CreateSomeShaders(void);

#if !defined(_FINAL_)
    void InitializeIcon(void);
    void ShutdownIcon(void);
    void DisplayStatus(void);
#endif

    static DWORD WINAPI Thread_Entry(LPVOID _pUserData);
    void Thread_Run(void);
    void Thread_ProcessRequest(void);
    void Thread_CompileVertexShader(CompilationRequest*  _pRequest, 
                                    ID3DXBuffer**        _ppBuffer, 
                                    ID3DXConstantTable** _ppConstantTable);
    void Thread_CompilePixelShader(CompilationRequest*  _pRequest, 
                                   ID3DXBuffer**        _ppBuffer, 
                                   ID3DXConstantTable** _ppConstantTable,
                                   ULONG64 *            _pulSemantics = NULL );

private:

    CHAR   m_szFileName[DB_MAX_NAME];
    CHAR   m_szTempFileName[DB_MAX_NAME];
    HANDLE m_hFileHandle;

    DBHeader m_oHeader;
    FATEntry m_aoFAT[FAT_MAX_ENTRIES];
    ULONG    m_ulFATOffset;
    ULONG    m_ulFATCapacity;
    ULONG    m_ulCurrentWriteOffset;

    ShaderFile m_apoShaderFiles[SHADER_MAX_FILES];
    ULONG      m_ulNbShaderFiles;

    CHAR m_szShaderPath[DB_MAX_NAME];

    // SC: Always trigger the proper access lock when accessing the following resources
    CompilationRequest m_aoRequests[REQUEST_MAX];
    ULONG              m_ulNbRequests;
    CompiledShader     m_aoCompiledShaders[REQUEST_MAX];
    ULONG              m_ulNbCompiledShaders;

    HANDLE           m_hCompilationThread;
    DWORD            m_dwCompilationThreadId;
    CRITICAL_SECTION m_oCompilationRequestLock;
    CRITICAL_SECTION m_oCompiledShadersLock;
    HANDLE           m_hThreadStartedEvent;
    HANDLE           m_hThreadEvents[THREAD_EVENT_COUNT];

    // Temporary shaders while we are loading the real ones
    XeVertexShader* m_pDefaultVertexShader;
    XePixelShader*  m_pDefaultPixelShader;

#if !defined(_FINAL_)
    // Shader compilation icon
    IDirect3DTexture9* m_pIconTexture;
    UINT               m_uiIconId;
    ULONG              m_ulSpriteIndex;
#endif
};

// ------------------------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------------------------
extern XeShaderDatabase g_oXeShaderDatabase;

#endif // #ifdef GUARD_XESHADERDATABASE_H
