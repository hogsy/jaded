//------------------------------------------------------------------------------
//   DAT_CP4ClientUserHeader.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DAT_CP4CLIENTUSERHEADER_H__
#define __DAT_CP4CLIENTUSERHEADER_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "PerforceUI.h"
#include "DATCP4FileSysHeader.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserHeader : public PerforceUI
{
public:
	DAT_CP4ClientUserHeader(std::vector<DAT_CP4ClientInfoHeader*>* _pvBFObjectHeader /* delete pushed back buffers after usage */) :
	m_pvObjectHeader(_pvBFObjectHeader)
	{
		m_uiReadSize = 0;
		m_bHeaderRead = FALSE;
		ms_Buffer.Clear();
	}

	virtual void Message(Error* pError)
	{
		m_bHeaderRead = FALSE;
		StrBuf buffer;
		pError->Fmt(&buffer);
	}

	virtual void OutputBinary( const_char *buf, int len )
	{
		if ( !m_bHeaderRead ) 
		{
			ms_Buffer.Append(buf,len);
			m_uiReadSize += len;
			DAT_CP4BFObjectHeader ObjectHeader;
			ObjectHeader.Read(ms_Buffer.Value(),ms_Buffer.Length());
			if ( m_uiReadSize >= ObjectHeader.Size() ) 
			{
				m_bHeaderRead = TRUE;
				// we do not want to get all the file, we just need the header to retreive 
				// file info. But their is no way to stop perforce from sending this file, 
				// since we do not want to abord the whole command.
				ProcessHeader();
			}
		}
	}

	virtual void ProcessHeader()
	{
		if ( m_pvObjectHeader && ms_Buffer.Length() > 0 ) 
		{
			DAT_CP4ClientInfoHeader* pInfoHeader = new DAT_CP4ClientInfoHeader;

			// checking for header info
			DAT_CP4BFObjectHeader ObjectHeader;
			ObjectHeader.Read( ms_Buffer.Value(), ms_Buffer.Length());

			// transfering header info to FStatInfo 
			DAT_CUtils::GetHeader(ObjectHeader,*pInfoHeader);
			m_pvObjectHeader->push_back(pInfoHeader);
			ms_Buffer.Clear();
		}
	}

private:
	std::vector<DAT_CP4ClientInfoHeader*>* m_pvObjectHeader;
	static StrBuf	ms_Buffer;
	BOOL			m_bHeaderRead;
	UINT			m_uiReadSize;

};

StrBuf DAT_CP4ClientUserHeader::ms_Buffer;
//------------------------------------------------------------------------------

#endif //#ifndef __DAT_CP4CLIENTUSERHEADER_H__

