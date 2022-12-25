// Reads the version in the Ressources (version block)

#ifndef VREAD_VersionReader_h
#define VREAD_VersionReader_h 1

class VREAD_VersionReader 
{
  public:
      static BOOL bGetFileVersion (char* _szFileName, DWORD& _ulV1, DWORD& _ulV2, DWORD& _ulV3, DWORD& _ulV4);
      static BOOL bGetFileVersion (char* _szFileName, char* _szVersion);
};

#endif
