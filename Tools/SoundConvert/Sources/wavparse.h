//-----------------------------------------------------------------------------
// File: wvaparse.h
//
// Desc: Helper class for reading and writing a .wav file 
//
// Hist: 6.12.01 - adapated from xbsound.h
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBSOUND_H
#define XBSOUND_H

#include "windows.h"
#include <mmsystem.h>

//-----------------------------------------------------------------------------
// FourCC definitions
//-----------------------------------------------------------------------------
const DWORD FOURCC_WAVE   = 'EVAW';
const DWORD FOURCC_FORMAT = ' tmf';
const DWORD FOURCC_DATA   = 'atad';
const DWORD FOURCC_CUE   = ' euc';
const DWORD FOURCC_PLST   = 'tslp';
const DWORD FOURCC_LABL   = 'lbal';
const DWORD FOURCC_NOTE   = 'eton';
const DWORD FOURCC_LTXT   = 'txtl';
const DWORD FOURCC_INFO   = 'OFNI';





//-----------------------------------------------------------------------------
// Name: RIFFHEADER
// Desc: For parsing WAV files
//-----------------------------------------------------------------------------
struct RIFFHEADER
{
    FOURCC  fccChunkId;
    DWORD   dwDataSize;
};

#define RIFFCHUNK_FLAGS_VALID   0x00000001


//-----------------------------------------------------------------------------
// Name: class CRiffChunk
// Desc: RIFF chunk utility class
//-----------------------------------------------------------------------------
class CRiffChunk
{
    FOURCC            m_fccChunkId;       // Chunk identifier
    const CRiffChunk* m_pParentChunk;     // Parent chunk
    HANDLE            m_hFile;
    DWORD             m_dwDataOffset;     // Chunk data offset
    DWORD             m_dwDataSize;       // Chunk data size
    DWORD             m_dwFlags;          // Chunk flags

public:
    CRiffChunk();

    // Initialization
    VOID    Initialize( FOURCC fccChunkId, const CRiffChunk* pParentChunk,
                        HANDLE hFile );
    HRESULT Open();
    BOOL    IsValid()     { return !!(m_dwFlags & RIFFCHUNK_FLAGS_VALID); }

    // Data
    HRESULT ReadData( LONG lOffset, VOID* pData, DWORD dwDataSize );

    // Chunk information
    FOURCC  GetChunkId()  { return m_fccChunkId; }
    DWORD   GetDataSize() { return m_dwDataSize; }
};




//-----------------------------------------------------------------------------
// Name: class CWaveParser
// Desc: Wave file utility class
//-----------------------------------------------------------------------------
class CWaveParser
{
    HANDLE      m_hFile;            // File handle
    CRiffChunk  m_RiffChunk;        // RIFF chunk
    CRiffChunk  m_FormatChunk;      // Format chunk
    CRiffChunk  m_DataChunk;        // Data chunk

public:
    CWaveParser();
    ~CWaveParser();

    // Initialization
    HRESULT Open( const CHAR* strFileName );
    VOID    Close();

    // File format
    HRESULT GetFormat( WAVEFORMATEX* pwfxFormat, DWORD dwFormatSize );

    // File data
    HRESULT ReadSample( DWORD dwPosition, VOID* pBuffer, DWORD dwBufferSize, 
                        DWORD* pdwRead );

    // File properties
    VOID    GetDuration( DWORD* pdwDuration ) { *pdwDuration = m_DataChunk.GetDataSize(); }
};




#endif // XBSOUND_H