// ------------------------------------------------------------------------------------------------
// File   : DAT_CP4BFObjectHeader.h
// Date   : 2005-02-07
// Author : Simon Tourangeau
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------
#pragma once

class DAT_CP4BFObjectHeader
{
public:
	// CTor
	DAT_CP4BFObjectHeader();
	// DTor
	~DAT_CP4BFObjectHeader();

	// Get the total size of the BFObjectHeader TLV
	size_t Size();

	// Extract the TLV data from the buffer to the BFObjectHeader
	// It returns a pointer pointing past the last byte read from buffer
	void* Read(void* buff, size_t bufSize);
	// Send the data from the BFObjectHeader to the buffer in TLV format
	// It returns a pointer pointing past the last byte writen to buffer
	void* Write(void* buff, size_t bufSize);

	// Get the BFObjectHeader attributes
	bool Path(std::string& strPath);
	bool Version(UINT& version);
	bool JadeKey(ULONG& key);
	bool IsUniverseKey(UINT& isUniverseKey);

	// Set the BFObjectHeader attributes
	void SetPath(std::string& strPath);
	void SetVersion(UINT version);
	void SetJadeKey(ULONG key);
	void SetIsUniverseKey(UINT isUniverseKey);

	// Get the file extension 
	bool PathExt(std::string& strExt);

private:
	// This enum contains the attributes that can
	// be found in the TLV
	enum eBFObjectHeader
	{
		header = 0x12340000,	// why this value ? less likely to be found that 0 or 1
		path,
		version,
		jadeKey,
		universeKey
	};

	std::string	m_Path;		// Object's path in BF structure
	UINT		m_Version;	// BF version in which this object was saved
	ULONG		m_JadeKey;	// BF Key for that object
	UINT		m_IsUniverseKey; // if the file is the universe key

	bool		m_PathIsSet;
	bool		m_VersionIsSet;
	bool		m_JadeKeyIsSet;
	bool		m_IsUniverseKeySet;
};

class DAT_CP4BFObjectData
{
public:
	// CTor
	DAT_CP4BFObjectData();
	// DTor
	~DAT_CP4BFObjectData();

	// Get the total size of the BFObjectData TLV
	size_t Size();
	// Get the actual number of byte contained in data (excluding TLV header)
	size_t DataLen();
	// Get the total size of the BFObjectData TLV once compressed (0 if data is not compressed)
	size_t CompressedSize();
	// Get the actual number of byte contained in data (excluding TLV header) once compressed (0 if data is not compressed)
	size_t CompressedDataLen();

	// Free any buffers that were allocated by us
	void Free();

	// Extract the TLV data from the buffer
	// It returns a pointer pointing past the last byte read from buffer
	void* Read(void* buff, size_t bufSize);
	// Send the data to the buffer in TLV format
	// It returns a pointer pointing past the last byte writen to buffer
	void* Write(void* buff, size_t& bufSize);

	// Set a pointer to the data
	// Data can be compressed before being stored 
	void SetData(void* buff, size_t bufSize, bool compressIt);
	// Get a pointer to the data and the size of the buffer
	// If the data was stored compressed, it was decompressed automatically 
	// upon reading
	void Data(void*& buff, size_t& bufSize);

private:
	enum eBFObjectData
	{
		compressedData = 0x23450000,	// why this value ? less likely to be found that 0 or 1
		uncompressedData,
	};

	void*	m_Data;
	size_t	m_DataLen;

	bool    m_Compressed;
	void*	m_CompressedData;
	size_t	m_CompressedDataLen;
};

class DAT_CP4BFObject
{
public:
	DAT_CP4BFObject(const std::string& fileName);
	DAT_CP4BFObject(void* buff, size_t buffSize);
	virtual ~DAT_CP4BFObject();

	bool DAT_CP4BFObject::Write(std::string& fileName);

	inline DAT_CP4BFObjectHeader&	Header()	{ return m_Header; }
	inline DAT_CP4BFObjectData&		Data()		{ return m_Data; }
	inline void*					Buff()		{ return m_Buff; }
	inline size_t					BuffSize()	{ return m_BuffSize; }

private:
	DAT_CP4BFObject();		// block default ctor
	void					Init();
	std::string					m_FileName;
	void*					m_Buff;
	size_t					m_BuffSize;
	DAT_CP4BFObjectHeader	m_Header;
	DAT_CP4BFObjectData		m_Data;
};

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
typedef UINT tTlvType;
typedef UINT tTlvLen;

class CTlv
{
public:
	~CTlv();

	// TLVs are mapped over buffers, they are not created
	// Call this to get a pointer to a TLV from a buffer
	static CTlv* Instance(void* buff);

	// Get the TLV Type
	inline tTlvType Type() { return m_type; } 
	// Get the TLV lenght
	inline tTlvLen  Len() { return m_len; }

	// Skip this TLV, and get a pointer to the next one
	CTlv* Skip();
	// Get a pointer to the data section of the TLV
	void* Data();
	// Get the value in the TLV. It also returns a pointer to the next TLV
	CTlv* Value(UINT& val);
	// Get the value in the TLV. It also returns a pointer to the next TLV
	CTlv* Value(ULONG& val);
	// Get the value in the TLV. It also returns a pointer to the next TLV
	CTlv* Value(std::string& val);

	// Get the size of the overhead created by a TLV (just the type and len)
	static tTlvLen OverheadSize();

	// Set the type and len of the TLV, and get a pointer to the value area
	void* OverheadSet(tTlvType type, tTlvLen len);
	// Set the type, len and value of the TLV. It also returns a pointer to 
	// the next TLV
	CTlv* ValueSet(tTlvType type, tTlvLen len, void* val);

private:
	// Blocked methods
	CTlv();						// Do not instantiate CTlv.. map it over a buffer
	CTlv(CTlv& rhs);			// copy constructor
	CTlv& operator=(CTlv& rhs);	// Do not copy TLVs

	// Get a pointer to the next TLV (skip header + len() bytes of data)
	CTlv*		NextTlv();

	// Type, lenght and data are mapped over a buffer
	tTlvType	m_type;
	tTlvLen		m_len;
	UINT		m_value;
};

