// ------------------------------------------------------------------------------------------------
// File   : DAT_CP4BFObjectHeader.cpp
// Date   : 2005-02-07
// Author : Simon Tourangeau
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "precomp.h"
#include <string>
#include "assert.h"
#include "DATCP4BFObject.h"
#include "DATCCompression.h"

DAT_CP4BFObject::DAT_CP4BFObject(const std::string& fileName):
	m_FileName(fileName),
	m_Buff(NULL),
	m_BuffSize(0)
{
	FILE*	fp;

	// Get the file size, allocate a big enough buffer and read the file

	fp = fopen(fileName.c_str(), "rb");
	if (fp)	
	{
		fseek(fp, 0, SEEK_END);
		m_BuffSize = ftell(fp);
		m_Buff = malloc(m_BuffSize);
		fseek(fp, 0, SEEK_SET);
		fread(m_Buff, 1, m_BuffSize, fp);
		fclose(fp);

		Init();
	}
}

DAT_CP4BFObject::DAT_CP4BFObject(void* buff, size_t buffSize)
{
	m_Buff	   = buff;
	m_BuffSize = buffSize;

	Init();
}

DAT_CP4BFObject::~DAT_CP4BFObject()
{
	// If we received a filename, we allocated a buffer to hold the data
	if (m_FileName != "")	
		free(m_Buff);
}

bool DAT_CP4BFObject::Write(std::string& fileName)
{
	FILE* fp;
	bool  bOk = true;

	if (fileName == "")
		fileName == m_FileName;

	fp = fopen(fileName.c_str(), "wb");
	if (fp)
	{
		void*  buff;
		size_t len;

		len = m_Header.Size();
		buff = malloc(len);
		m_Header.Write(buff, len);
		fwrite(buff, 1, len, fp);
		free(buff);

		len = m_Data.CompressedSize();
		buff = malloc(len);
		if (!buff)
			bOk = false;
		else
		{		
			m_Data.Write(buff, len);
		
			if (fwrite(buff, 1, len, fp) != len)
				bOk = false;
			free(buff);
		}

		fclose(fp);
	}
	else 
		bOk = false;

	return bOk;
}

void DAT_CP4BFObject::Init()
{
	void* pBuff = NULL;
	// Read the file header
	pBuff = m_Header.Read(m_Buff, m_BuffSize);

	// If there was a header, and there is still data left, read it
	if (m_Header.Size() > 0 && m_Header.Size() < m_BuffSize)
		m_Data.Read(pBuff, m_BuffSize - m_Header.Size());
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
DAT_CP4BFObjectHeader::DAT_CP4BFObjectHeader():
	m_Version(0),
	m_JadeKey(0),
	m_IsUniverseKey(0),
  	m_PathIsSet(false),
	m_VersionIsSet(false),
	m_JadeKeyIsSet(false),
	m_IsUniverseKeySet(false)
{
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
DAT_CP4BFObjectHeader::~DAT_CP4BFObjectHeader()
{
	// Nothing to do
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void* DAT_CP4BFObjectHeader::Read(void* buff, size_t bufSize)
{
	void* retBuff = buff;
	CTlv* pTlv = CTlv::Instance(buff);		// Get a TLV from this buffer
	CTlv* pStartOfTlv	= pTlv;				// Remember the start of this TLV
	CTlv* pNextTlv		= pTlv->Skip();		// Get the start of the NEXT TLV
	
	// Is the TLV of the expected type ?
	if (pTlv->Type() == eBFObjectHeader::header)
	{
		// This TLV contains a bunch of TLV...
		// Get the first TLV
		pTlv = CTlv::Instance(pTlv->Data());

		// Loop thru all TLVs
		while(pTlv &&											// valid TLV
			  pTlv < pNextTlv &&								// not passed end of top TLV
			  ((size_t)pTlv + pTlv->Len() - (size_t)pStartOfTlv) < bufSize)		// not passed end of buff
		{
			switch(pTlv->Type())
			{
			// Read the path string
			case eBFObjectHeader::path:
				pTlv = pTlv->Value(m_Path);
				m_PathIsSet = true;
				break;

			// Read the version
			case eBFObjectHeader::version:
				pTlv = pTlv->Value(m_Version);
				m_VersionIsSet = true;
				break;

			// Read the jadeKey 
			case eBFObjectHeader::jadeKey:
				pTlv = pTlv->Value(m_JadeKey);
				m_JadeKeyIsSet = true;
				break;

			case eBFObjectHeader::universeKey:
				pTlv = pTlv->Value(m_IsUniverseKey);
				m_IsUniverseKeySet = true;
				break;

			// Unknown TLV, skip it
			default:
				pTlv = pTlv->Skip();
			}
		}
		
		// The returned buffer will point to the next TLV
		retBuff = pNextTlv;
	}

	return retBuff;
}


// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void* DAT_CP4BFObjectHeader::Write(void* buff, size_t bufSize)
{
	CTlv* pTlv = (CTlv* )buff;
	CTlv* pStartOfTlv = pTlv;
	size_t curSize;

	if (bufSize > CTlv::OverheadSize())
		pTlv = (CTlv* )pTlv->OverheadSet(eBFObjectHeader::header, Size() - CTlv::OverheadSize());
	curSize = (size_t)pTlv - (size_t)pStartOfTlv;
	if (m_PathIsSet && (curSize + m_Path.length()) < bufSize)
		pTlv = pTlv->ValueSet(eBFObjectHeader::path,	m_Path.length(),	(void* )m_Path.c_str());
	curSize = (size_t)pTlv - (size_t)pStartOfTlv;
	if (m_VersionIsSet && (curSize + sizeof(m_Version)) < bufSize)
		pTlv = pTlv->ValueSet(eBFObjectHeader::version, sizeof(m_Version),	(void* )&m_Version);
	curSize = (size_t)pTlv - (size_t)pStartOfTlv;
	if (m_JadeKeyIsSet  && (curSize + sizeof(m_JadeKey)) < bufSize)
		pTlv = pTlv->ValueSet(eBFObjectHeader::jadeKey, sizeof(m_JadeKey),	(void* )&m_JadeKey);
	curSize = (size_t)pTlv - (size_t)pStartOfTlv;
	if (m_IsUniverseKeySet  && (curSize + sizeof(m_IsUniverseKey)) < bufSize)
		pTlv = pTlv->ValueSet(eBFObjectHeader::universeKey, sizeof(m_IsUniverseKey),	(void* )&m_IsUniverseKey);

	return pTlv;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
// Get the size of the object header
size_t DAT_CP4BFObjectHeader::Size()
{
	size_t size;
	
	size =  CTlv::OverheadSize() 			// overhead for header TLV  
			+ (m_PathIsSet		 ? m_Path.length()*sizeof(char)	+ CTlv::OverheadSize(): 0)
			+ (m_VersionIsSet	 ? sizeof(m_Version)			+ CTlv::OverheadSize(): 0) 
			+ (m_JadeKeyIsSet	 ? sizeof(m_JadeKey)			+ CTlv::OverheadSize(): 0)
			+ (m_IsUniverseKeySet ? sizeof(m_IsUniverseKey)		+ CTlv::OverheadSize(): 0);
	return size;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
bool DAT_CP4BFObjectHeader::Path(std::string& strPath)
{ 
	strPath = m_Path; 

	return m_PathIsSet;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
bool DAT_CP4BFObjectHeader::PathExt(std::string& strExt)
{
	std::string::size_type pos = m_Path.find_last_of('.');
	if (pos != std::string::npos)
		strExt = m_Path.substr(pos);
	else
		strExt = "";

	return m_PathIsSet;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
bool DAT_CP4BFObjectHeader::Version(UINT& version)
{ 
	version = m_Version; 

	return m_VersionIsSet;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
bool DAT_CP4BFObjectHeader::JadeKey(ULONG& key)
{ 
	key     = m_JadeKey; 

	return m_JadeKeyIsSet;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
bool DAT_CP4BFObjectHeader::IsUniverseKey(UINT& IsUniverseKey)	
{ 
	IsUniverseKey    = m_IsUniverseKey; 

	return m_IsUniverseKeySet;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void DAT_CP4BFObjectHeader::SetPath(std::string& strPath)	
{ 
	m_PathIsSet = true; 
	m_Path    = strPath; 
}



// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void DAT_CP4BFObjectHeader::SetVersion(UINT version)	
{ 
	m_VersionIsSet = true; 
	m_Version = version; 
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void DAT_CP4BFObjectHeader::SetJadeKey(ULONG key)
{ 
	m_JadeKeyIsSet = true;
	m_JadeKey = key; 
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void DAT_CP4BFObjectHeader::SetIsUniverseKey(UINT IsUniverseKey)
{ 
	m_IsUniverseKeySet = true;
	m_IsUniverseKey = IsUniverseKey; 
}

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
DAT_CP4BFObjectData::DAT_CP4BFObjectData():
	m_Data(NULL),
	m_DataLen(0),
	m_Compressed(false),
	m_CompressedData(NULL),
	m_CompressedDataLen(0)
{
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
DAT_CP4BFObjectData::~DAT_CP4BFObjectData()
{
	delete[] m_CompressedData;
	m_CompressedData = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
size_t DAT_CP4BFObjectData::Size()
{
	size_t size;
	
	size =  CTlv::OverheadSize() 			// overhead for header TLV  
			+ DataLen();
	return size;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
size_t DAT_CP4BFObjectData::CompressedSize()
{
	size_t size;

	size =  CTlv::OverheadSize() 			// overhead for header TLV  
		+ CompressedDataLen();
	return size;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
size_t DAT_CP4BFObjectData::DataLen()
{
	return m_DataLen;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
size_t DAT_CP4BFObjectData::CompressedDataLen()
{
	if( !m_Compressed )
		return this->m_DataLen;

	return m_CompressedDataLen;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void* DAT_CP4BFObjectData::Read(void* buff, size_t size)
{
	if ( size == 0 ) 
	{
		m_Data = NULL;
		m_DataLen = 0;
		return NULL;
	}

	void* retBuff = buff;
	CTlv* pTlv = CTlv::Instance(buff);		// Get a TLV from this buffer
	
	// Is the TLV of the expected type ?
	switch (pTlv->Type())
	{
	case eBFObjectData::uncompressedData:
		m_DataLen = pTlv->Len();
		m_Data = pTlv->Data();
		break;

	case eBFObjectData::compressedData:
		// JFP: Set the m_Compressed flag since we know we are compressed. This fixes an "out of memory" problem
		//      that made it so that we were busting the 2 gigs limit in a Windows processes when doing a "get latest".
		m_Compressed = true;

		// Inflate the file (it will allocate a buffer and point pBuff to it)
		DAT_CCompression::Uncompress((char* )pTlv->Data(), (UCHAR**)&m_Data, pTlv->Len(), (ULONG* )&m_DataLen);
		break;

	default:
		break;
	}

	retBuff = pTlv->Skip();
	return retBuff;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void DAT_CP4BFObjectData::Free()
{
	if (m_Compressed)
	{
		delete[] m_Data;
	}
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void* DAT_CP4BFObjectData::Write(void* buff, size_t& bufSize)
{
	CTlv* pTlv = CTlv::Instance(buff);

	if (!m_Data)
		return pTlv;

	if (m_Compressed)
	{
#ifdef JADEFUSION
		if (bufSize >= CompressedSize())
#else
		if (m_CompressedDataLen < pTlv->Len())
#endif
		{
			pTlv = pTlv->ValueSet(eBFObjectData::compressedData,
								  m_CompressedDataLen,
								  m_CompressedData);
		}
#if !defined(XML_CONV_TOOL) && defined(JADEFUSION)
		else
		{
			ERR_X_ForceErrorThrow("Mismatch of compressed buffer sizes in DAT_CP4BFObjectData::Write.\nPlease contact your Data Manager.", NULL);
		}
#endif
		bufSize = CTlv::Instance(buff)->Len() + CTlv::OverheadSize(); // if data was compressed, size has changed
	}
	else // not compressed
	{
#ifdef JADEFUSION
		if (bufSize >= Size()) 
#else
		if (bufSize < pTlv->Len()) 
#endif
		{
			pTlv = pTlv->ValueSet(eBFObjectData::uncompressedData,
								  m_DataLen,
								  m_Data);
		}
#if !defined(XML_CONV_TOOL) && defined(JADEFUSION)
		else
		{
			ERR_X_ForceErrorThrow("Mismatch of buffer sizes in DAT_CP4BFObjectData::Write.\nPlease contact your Data Manager.", NULL);
		}
#endif

	}

	return pTlv;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void DAT_CP4BFObjectData::SetData(void* buff, size_t bufSize, bool compressIt)
{
	m_Compressed = compressIt;

	m_Data = buff;
	m_DataLen = bufSize;

	if( m_Compressed )
	{
		if( m_CompressedData != NULL )
		{
			delete[] m_CompressedData;
			m_CompressedData = NULL;
		}

		DAT_CCompression::Compress( (const Byte* )m_Data, (Byte**)&m_CompressedData, bufSize, (ULONG* )&m_CompressedDataLen );
	}
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void DAT_CP4BFObjectData::Data(void*& buff, size_t& bufSize)
{
	buff	= m_Data;
	bufSize = m_DataLen;
}

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
CTlv* CTlv::Instance(void* buff)
{
	return (CTlv* )buff;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
CTlv::~CTlv()
{
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void* CTlv::Data()
{
	return (void* )&m_value;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
CTlv* CTlv::Skip()
{
	return NextTlv();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
CTlv* CTlv::Value(UINT& val)
{
	val = m_value;
	return NextTlv();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
CTlv* CTlv::Value(ULONG& val)
{
	val = m_value;
	return NextTlv();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
CTlv* CTlv::Value(std::string& val)
{
	val = "";
	val.append((char* )&m_value, m_len);
	return NextTlv();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void* CTlv::OverheadSet(tTlvType type, tTlvLen len)
{
	m_type = type;
	m_len  = len;

	return (void* )&m_value;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
size_t CTlv::OverheadSize()
{ 
	return (sizeof(tTlvType) + sizeof(tTlvLen)); 
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
CTlv* CTlv::ValueSet(tTlvType type, tTlvLen len, void* val)
{
	memcpy(OverheadSet(type, len), val, len);

	return NextTlv();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
CTlv* CTlv::NextTlv()
{ 
	return (CTlv* )((UINT )this + 
					sizeof(tTlvType) +
					sizeof(tTlvLen)  + 
					m_len); 
}

