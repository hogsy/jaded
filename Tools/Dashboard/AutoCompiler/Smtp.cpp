/*
Module : SMTP.CPP
Purpose: Implementation for a MFC class encapsulation of the SMTP protocol
Created: PJN / 22-05-1998
History: PJN / 15-06-1998 1) Fixed the case where a single dot occurs on its own
                          in the body of a message
													2) Class now supports Reply-To Header Field
                          3) Class now supports file attachments

				 PJN / 18-06-1998 1) Fixed a memory overwrite problem which was occurring 
				                  with the buffer used for encoding base64 attachments

         PJN / 27-06-1998 1) The case where a line begins with a "." but contains
                          other text is now also catered for. See RFC821, Section 4.5.2
                          for further details.
                          2) m_sBody in CSMTPMessage has now been made protected.
                          Client applications now should call AddBody instead. This
                          ensures that FixSingleDot is only called once even if the 
                          same message is sent a number of times.
                          3) Fixed a number of problems with how the MIME boundaries
                          were defined and sent.
                          4) Got rid of an unreferenced formal parameter 
                          compiler warning when doing a release build

         PJN / 11-09-1998 1) VC 5 project file is now provided
                          2) Attachment array which the message class contains now uses
                          references instead of pointers.
                          3) Now uses Sleep(0) to yield our time slice instead of Sleep(100),
                          this is the preferred way of writting polling style code in Win32
                          without serverly impacting performance.
                          4) All Trace statements now display the value as returned from
                          GetLastError
                          5) A number of extra asserts have been added
                          6) A AddMultipleRecipients function has been added which supports added a 
                          number of recipients at one time from a single string
                          7) Extra trace statements have been added to help in debugging

         PJN / 12-09-98   1) Removed a couple of unreferenced variable compiler warnings when code
                          was compiled with Visual C++ 6.0
                          2) Fixed a major bug which was causing an ASSERT when the CSMTPAttachment
                          destructor was being called in the InitInstance of the sample app. 
                          This was inadvertingly introduced for the 1.2 release. The fix is to revert 
                          fix 2) as done on 11-09-1998. This will also help to reduce the number of 
                          attachment images kept in memory at one time.

         PJN / 18-01-99   1) Full CC & BCC support has been added to the classes

         PJN / 22-02-99   1) Addition of a Get and SetTitle function which allows a files attachment 
                          title to be different that the original filename
                          2) AddMultipleRecipients now ignores addresses if they are empty.
                          3) Improved the reading of responses back from the server by implementing
                          a growable receive buffer
                          4) timeout is now 60 seconds when building for debug

         PJN / 25-03-99   1) Now sleeps for 250 ms instead of yielding the time slice. This helps 
                          reduce CPU usage when waiting for data to arrive in the socket

         PJN / 14-05-99   1) Fixed a bug with the way the code generates time zone fields in the Date headers.

         PJN / 10-09-99   1) Improved CSMTPMessage::GetHeader to include mime field even when no attachments
                          are included.

         PJN / 16-02-00   1) Fixed a problem which was occuring when code was compiled with VC++ 6.0.




Copyright (c) 1998 - 2000 by PJ Naughter.  
All rights reserved.

*/

//////////////// Includes ////////////////////////////////////////////
//#include "PrecompiledHeader.h"
#include "stdafx.h"
#include "smtp.h"


//////////////// Macros / Locals /////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char CSMTPAttachment::m_base64tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz0123456789+/";
#define BASE64_MAXLINE  76
#define EOL  "\r\n"



//////////////// Implementation //////////////////////////////////////
CSMTPSocket::CSMTPSocket()
{
  m_hSocket = INVALID_SOCKET; //default to an invalid scoket descriptor
}

CSMTPSocket::~CSMTPSocket()
{
  Close();
}

BOOL CSMTPSocket::Create()
{
  m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
  return (m_hSocket != INVALID_SOCKET);
}

BOOL CSMTPSocket::Connect(LPCTSTR pszHostAddress, int nPort)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);
  
	LPSTR lpszAscii = T2A((LPTSTR)pszHostAddress);

	//Determine if the address is in dotted notation
	SOCKADDR_IN sockAddr;
	ZeroMemory(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons((u_short)nPort);
	sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);

	//If the address is not dotted notation, then do a DNS 
	//lookup of it.
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(lpszAscii);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
      WSASetLastError(WSAEINVAL); 
			return FALSE;
		}
	}

	//Call the protected version which takes an address 
	//in the form of a standard C style struct.
	return Connect((SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

BOOL CSMTPSocket::Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	return (connect(m_hSocket, lpSockAddr, nSockAddrLen) != SOCKET_ERROR);
}

BOOL CSMTPSocket::Send(LPCSTR pszBuf, int nBuf)
{
  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);

  return (send(m_hSocket, pszBuf, nBuf, 0) != SOCKET_ERROR);
}

int CSMTPSocket::Receive(LPSTR pszBuf, int nBuf)
{
  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);

  return recv(m_hSocket, pszBuf, nBuf, 0); 
}

void CSMTPSocket::Close()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		VERIFY(SOCKET_ERROR != closesocket(m_hSocket));
		m_hSocket = INVALID_SOCKET;
	}
}

BOOL CSMTPSocket::IsReadible(BOOL& bReadible)
{
  timeval timeout = {0, 0};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(m_hSocket, &fds);
  int nStatus = select(0, &fds, NULL, NULL, &timeout);
  if (nStatus == SOCKET_ERROR)
  {
    return FALSE;
  }
  else
  {
    bReadible = !(nStatus == 0);
    return TRUE;
  }
}


CSMTPAddress::CSMTPAddress() 
{
}

CSMTPAddress::CSMTPAddress(const CSMTPAddress& address)
{
  *this = address;
}

CSMTPAddress::CSMTPAddress(const CString& sAddress) : 
              m_sEmailAddress(sAddress) 
{
  ASSERT(m_sEmailAddress.GetLength()); //An empty address is not allowed
}

CSMTPAddress::CSMTPAddress(const CString& sFriendly, const CString& sAddress) : 
              m_sFriendlyName(sFriendly), m_sEmailAddress(sAddress) 
{
  ASSERT(m_sEmailAddress.GetLength()); //An empty address is not allowed
}

CSMTPAddress& CSMTPAddress::operator=(const CSMTPAddress& r) 
{ 
  m_sFriendlyName = r.m_sFriendlyName; 
	m_sEmailAddress = r.m_sEmailAddress; 
	return *this;
}

CString CSMTPAddress::GetRegularFormat() const
{
  ASSERT(m_sEmailAddress.GetLength()); //Email Address must be valid

  CString sAddress;
  if (m_sFriendlyName.IsEmpty())
    sAddress = m_sEmailAddress;  //Just transfer the address across directly
  else
    sAddress.Format(_T("%s <%s>"), m_sFriendlyName, m_sEmailAddress);

  return sAddress;
}





CSMTPAttachment::CSMTPAttachment()
{
  m_pszEncoded = NULL;
  m_nEncodedSize = 0;
}

CSMTPAttachment::~CSMTPAttachment()
{
  //free up any memory we allocated
  if (m_pszEncoded)
	{
    delete [] m_pszEncoded;
		m_pszEncoded = NULL;
	}
}

BOOL CSMTPAttachment::Attach(const CString& sFilename)
{
  ASSERT(sFilename.GetLength());  //Empty Filename !

  //free up any memory we previously allocated
  if (m_pszEncoded)
  {
    delete [] m_pszEncoded;
    m_pszEncoded = NULL;
  }

  //determine the file size
  CFileStatus fs;
  if (!CFile::GetStatus(sFilename, fs))
  {
    TRACE(_T("Failed to get the status for file %s, probably does not exist\n"), sFilename);
    return FALSE;
  }

  //open up the file for reading in
  CFile infile;
  if (!infile.Open(sFilename, CFile::modeRead | CFile::shareDenyWrite))
  {
    TRACE(_T("Failed to open file to be attached\n"));
    return FALSE;
  }

  //read in the contents of the input file
  char* pszIn = new char[fs.m_size];
  infile.Read(pszIn, fs.m_size);

  //allocate the encoded buffer
  int nOutSize = Base64BufferSize(fs.m_size);
  m_pszEncoded = new char[nOutSize];

  //Do the encoding
  EncodeBase64(pszIn, fs.m_size, m_pszEncoded, nOutSize, &m_nEncodedSize);

  //delete the input buffer
  delete [] pszIn;

  //Close the input file
  infile.Close();

	//Hive away the filename
  TCHAR sPath[_MAX_PATH];
  TCHAR sFname[_MAX_FNAME];
  TCHAR sExt[_MAX_EXT];
  _tsplitpath(sFilename, NULL, NULL, sFname, sExt);
  _tmakepath(sPath, NULL, NULL, sFname, sExt);
	m_sFilename = sPath;
  m_sTitle = sPath;

  return TRUE;
}

int CSMTPAttachment::Base64BufferSize(int nInputSize)
{
  int nOutSize = (nInputSize+2)/3*4;                    // 3:4 conversion ratio
  nOutSize += strlen(EOL)*nOutSize/BASE64_MAXLINE + 3;  // Space for newlines and NUL
  return nOutSize;
}

BOOL CSMTPAttachment::EncodeBase64(const char* pszIn, int nInLen, char* pszOut, int nOutSize, int* nOutLen)
{
  //Input Parameter validation
  ASSERT(pszIn);
  ASSERT(pszOut);
  ASSERT(nOutSize);
  ASSERT(nOutSize >= Base64BufferSize(nInLen));

#ifndef _DEBUG
  //justs get rid of "unreferenced formal parameter"
  //compiler warning when doing a release build
  nOutSize;
#endif

  //Set up the parameters prior to the main encoding loop
  int nInPos  = 0;
  int nOutPos = 0;
  int nLineLen = 0;

  // Get three characters at a time from the input buffer and encode them
  for (int i=0; i<nInLen/3; ++i) 
  {
    //Get the next 2 characters
    int c1 = pszIn[nInPos++] & 0xFF;
    int c2 = pszIn[nInPos++] & 0xFF;
    int c3 = pszIn[nInPos++] & 0xFF;

    //Encode into the 4 6 bit characters
    pszOut[nOutPos++] = m_base64tab[(c1 & 0xFC) >> 2];
    pszOut[nOutPos++] = m_base64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
    pszOut[nOutPos++] = m_base64tab[((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6)];
    pszOut[nOutPos++] = m_base64tab[c3 & 0x3F];
    nLineLen += 4;

    //Handle the case where we have gone over the max line boundary
    if (nLineLen >= BASE64_MAXLINE-3) 
    {
      char* cp = EOL;
      pszOut[nOutPos++] = *cp++;
      if (*cp) 
        pszOut[nOutPos++] = *cp;
      nLineLen = 0;
    }
  }

  // Encode the remaining one or two characters in the input buffer
  char* cp;
  switch (nInLen % 3) 
  {
    case 0:
    {
      cp = EOL;
      pszOut[nOutPos++] = *cp++;
      if (*cp) 
        pszOut[nOutPos++] = *cp;
      break;
    }
    case 1:
    {
      int c1 = pszIn[nInPos] & 0xFF;
      pszOut[nOutPos++] = m_base64tab[(c1 & 0xFC) >> 2];
      pszOut[nOutPos++] = m_base64tab[((c1 & 0x03) << 4)];
      pszOut[nOutPos++] = '=';
      pszOut[nOutPos++] = '=';
      cp = EOL;
      pszOut[nOutPos++] = *cp++;
      if (*cp) 
        pszOut[nOutPos++] = *cp;
      break;
    }
    case 2:
    {
      int c1 = pszIn[nInPos++] & 0xFF;
      int c2 = pszIn[nInPos] & 0xFF;
      pszOut[nOutPos++] = m_base64tab[(c1 & 0xFC) >> 2];
      pszOut[nOutPos++] = m_base64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
      pszOut[nOutPos++] = m_base64tab[((c2 & 0x0F) << 2)];
      pszOut[nOutPos++] = '=';
      cp = EOL;
      pszOut[nOutPos++] = *cp++;
      if (*cp) 
        pszOut[nOutPos++] = *cp;
      break;
    }
    default: 
    {
      ASSERT(FALSE); 
      break;
    }
  }
  pszOut[nOutPos] = 0;
  *nOutLen = nOutPos;
  return TRUE;
}





CSMTPMessage::CSMTPMessage() : m_sXMailer(_T("CSMTPConnection v1.3"))
{
}

CSMTPMessage::~CSMTPMessage()
{
}

int CSMTPMessage::GetNumberOfRecipients(RECIPIENT_TYPE RecipientType) const
{
  int nSize = 0;
  switch (RecipientType)
  {
    case TO:  nSize = m_ToRecipients.GetSize();  break;
    case CC:  nSize = m_CCRecipients.GetSize();  break;
    case BCC: nSize = m_BCCRecipients.GetSize(); break;
    default: ASSERT(FALSE);                      break;
  }

	return nSize;
}

int CSMTPMessage::AddRecipient(CSMTPAddress& recipient, RECIPIENT_TYPE RecipientType)
{
  int nIndex = -1;
  switch (RecipientType)
  {
    case TO:  nIndex = m_ToRecipients.Add(recipient);  break;
    case CC:  nIndex = m_CCRecipients.Add(recipient);  break;
    case BCC: nIndex = m_BCCRecipients.Add(recipient); break;
    default: ASSERT(FALSE);                            break;
  }

  return nIndex;
}

void CSMTPMessage::RemoveRecipient(int nIndex, RECIPIENT_TYPE RecipientType)
{
  switch (RecipientType)
  {
    case TO:  m_ToRecipients.RemoveAt(nIndex);  break;
    case CC:  m_CCRecipients.RemoveAt(nIndex);  break;
    case BCC: m_BCCRecipients.RemoveAt(nIndex); break;
    default:  ASSERT(FALSE);                    break;
  }
}

CSMTPAddress CSMTPMessage::GetRecipient(int nIndex, RECIPIENT_TYPE RecipientType) const
{
  CSMTPAddress address;

  switch (RecipientType)
  {
    case TO:  address = m_ToRecipients.GetAt(nIndex);  break;
    case CC:  address = m_CCRecipients.GetAt(nIndex);  break;
    case BCC: address = m_BCCRecipients.GetAt(nIndex); break;
    default: ASSERT(FALSE);                            break;
  }

  return address;
}

int CSMTPMessage::AddAttachment(CSMTPAttachment* pAttachment)
{
  ASSERT(pAttachment->GetFilename().GetLength()); //an empty filename !
	return m_Attachments.Add(pAttachment);
}

void CSMTPMessage::RemoveAttachment(int nIndex)
{
	m_Attachments.RemoveAt(nIndex);
}

CSMTPAttachment* CSMTPMessage::GetAttachment(int nIndex) const
{
	return m_Attachments.GetAt(nIndex);
}

int CSMTPMessage::GetNumberOfAttachments() const
{
	return m_Attachments.GetSize();
}

CString CSMTPMessage::GetHeader() const
{
  //Form the Timezone info which will form part of the Date header
  TIME_ZONE_INFORMATION tzi;
  int nTZBias;
  if (GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_DAYLIGHT)
    nTZBias = tzi.Bias + tzi.DaylightBias;
  else
    nTZBias = tzi.Bias;
  CString sTZBias;
  sTZBias.Format(_T("%+.2d%.2d"), -nTZBias/60, nTZBias%60);

  //Create the "Date:" part of the header
  CTime now(CTime::GetCurrentTime());
  CString sDate(now.Format(_T("%a, %d %b %Y %H:%M:%S ")));
  sDate += sTZBias;

  //Create the "To:" part of the header
  CString sTo;
  for (int i=0; i<GetNumberOfRecipients(TO); i++)
  {
    CSMTPAddress recipient = GetRecipient(i, TO);
    if (i)
 		  sTo += _T(",");
    sTo += recipient.GetRegularFormat();
  }

  //Create the "Cc:" part of the header
  CString sCc;
  for (i=0; i<GetNumberOfRecipients(CC); i++)
  {
    CSMTPAddress recipient = GetRecipient(i, CC);
    if (i)
 		  sCc += _T(",");
    sCc += recipient.GetRegularFormat();
  }

  //No Bcc info added in header

	//Stick everything together
	CString sBuf;
  if (sCc.GetLength())
	  sBuf.Format(_T("From: %s\r\n")\
                _T("To: %s\r\n")\
                _T("Cc: %s\r\n")\
							  _T("Subject: %s\r\n")\
							  _T("Date: %s\r\n")\
							  _T("X-Mailer: %s\r\n"), 
							  m_From.GetRegularFormat(),
                sTo, 
                sCc,
							  m_sSubject,
							  sDate,
							  m_sXMailer);
  else
	  sBuf.Format(_T("From: %s\r\n")\
                _T("To: %s\r\n")\
							  _T("Subject: %s\r\n")\
							  _T("Date: %s\r\n")\
							  _T("X-Mailer: %s\r\n"), 
							  m_From.GetRegularFormat(),
                sTo, 
							  m_sSubject,
							  sDate,
							  m_sXMailer);


	//Add the optional Reply-To Field
	if (m_ReplyTo.m_sEmailAddress.GetLength())
	{
		CString sReply;
		sReply.Format(_T("Reply-To: %s\r\n"), m_ReplyTo.GetRegularFormat());
		sBuf += sReply;
	}

	//Add the optional fields if attachments are included
	if (m_Attachments.GetSize())
		sBuf += _T("MIME-Version: 1.0\r\nContent-type: multipart/mixed; boundary=\"#BOUNDARY#\"\r\n");
  else
		// avoid long textual message being automatically converted by the server:
	  sBuf += _T("MIME-Version: 1.0\r\nContent-type: text/plain; charset=US-ASCII\r\n");    

	sBuf += _T("\r\n");

	//Return the result
	return sBuf;
}

void CSMTPMessage::FixSingleDot(CString& sBody)
{
  int nFind = sBody.Find(_T("\n."));
  if (nFind != -1)
  {
	  CString sLeft(sBody.Left(nFind+1));
	  CString sRight(sBody.Right(sBody.GetLength()-(nFind+1)));
	  FixSingleDot(sRight);
	  sBody = sLeft + _T(".") + sRight;
  }
}

void CSMTPMessage::AddBody(const CString& sBody)
{
  m_sBody = sBody;

  //Fix the case of a single dot on a line in the message body
  FixSingleDot(m_sBody);
}

BOOL CSMTPMessage::AddMultipleRecipients(const CString& sRecipients, RECIPIENT_TYPE RecipientType)
{
	ASSERT(sRecipients.GetLength()); //An empty string is now allowed
	
	//Loop through the whole string, adding recipients as they are encountered
	int length = sRecipients.GetLength();
	TCHAR* buf = new TCHAR[length + 1];	// Allocate a work area (don't touch parameter itself)
	_tcscpy(buf, sRecipients);
	for (int pos=0, start=0; pos<=length; pos++)
	{
		//Valid separators between addresses are ',' or ';'
		if ((buf[pos] == _T(',')) || (buf[pos] == _T(';')) || (buf[pos] == 0))
		{
			buf[pos] = 0;	//Redundant when at the end of string, but who cares.
      CString sTemp(&buf[start]);

			// Now divide the substring into friendly names and e-mail addresses
      CSMTPAddress To;
			int nMark = sTemp.Find(_T('<'));
			if (nMark >= 0)
			{
				To.m_sFriendlyName = sTemp.Left(nMark);
				int nMark2 = sTemp.Find(_T('>'));
				if (nMark2 < nMark)
				{
          //An invalid string was sent in, fail the call
					delete[] buf;
          SetLastError(ERROR_INVALID_DATA);
          TRACE(_T("An error occurred while parsing the recipients string\n"));
					return FALSE;
				}
				// End of mark at closing bracket or end of string
				nMark2 > -1 ? nMark2 = nMark2 : nMark2 = sTemp.GetLength() - 1;
				To.m_sEmailAddress = sTemp.Mid(nMark + 1, nMark2 - (nMark + 1));
			}
			else
			{
				To.m_sEmailAddress = sTemp;
				To.m_sFriendlyName = _T(_T(""));
			}

      //Finally add the new recipient to the array of recipients
			To.m_sEmailAddress.TrimRight();
			To.m_sEmailAddress.TrimLeft();
			To.m_sFriendlyName.TrimRight();
			To.m_sFriendlyName.TrimLeft();
      if (To.m_sEmailAddress.GetLength())
        AddRecipient(To, RecipientType);

      //Move on to the next position
			start = pos + 1;
		}
	}
	delete[] buf;
	return TRUE;
}







CSMTPConnection::CSMTPConnection()
{
  m_bConnected = FALSE;
#ifdef _DEBUG  
  m_dwTimeout = 60000; //default timeout of 60 seconds when debugging
#else  
  m_dwTimeout = 2000;  //default timeout of 2 seconds for normal release code
#endif
}

CSMTPConnection::~CSMTPConnection()
{
  if (m_bConnected)
    Disconnect();
}

BOOL CSMTPConnection::Connect(LPCTSTR pszHostName, int nPort)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

	//paramater validity checking
  ASSERT(pszHostName);

  //Create the socket
  if (!m_SMTP.Create())
  {
    TRACE(_T("Failed to create client socket, GetLastError returns: %d\n"), GetLastError());
    return FALSE;
  }

  //Connect to the SMTP Host
  if (!m_SMTP.Connect(pszHostName, nPort))
  {
    TRACE(_T("Could not connect to the SMTP server %s on port %d, GetLastError returns: %d\n"), pszHostName, nPort, GetLastError());
    return FALSE;
  }
  else
  {
    //We're now connected !!
    m_bConnected = TRUE;

    //check the response to the login
    if (!ReadCommandResponse(220))
    {
      TRACE(_T("An unexpected SMTP login response was received\n"));
      Disconnect();
      return FALSE;
    }

		//retreive the localhost name
    char sHostName[100];
		gethostname(sHostName, sizeof(sHostName));
    TCHAR* pszHostName = A2T(sHostName);

    //Send the HELO command
		CString sBuf;
		sBuf.Format(_T("HELO %s\r\n"), pszHostName);
    LPCSTR pszData = T2A((LPTSTR) (LPCTSTR) sBuf);
    int nCmdLength = strlen(pszData);
    if (!m_SMTP.Send(pszData, nCmdLength))
    {
      Disconnect();
      TRACE(_T("An unexpected error occurred while sending the HELO command\n"));
      return FALSE;
    }
		//check the response to the HELO command
    if (!ReadCommandResponse(250))
    {
      Disconnect();
      TRACE(_T("An unexpected HELO response was received\n"));
      return FALSE;
    } 

    return TRUE;
  }
}

BOOL CSMTPConnection::Disconnect()
{
  BOOL bSuccess = FALSE;      

  //disconnect from the SMTP server if connected 
  if (m_bConnected)
  {
    char sBuf[10];
    strcpy(sBuf, "QUIT\r\n");
    int nCmdLength = strlen(sBuf);
    if (!m_SMTP.Send(sBuf, nCmdLength))
      TRACE(_T("Failed in call to send QUIT command, GetLastError returns: %d\n"), GetLastError());

    //Check the reponse
    bSuccess = ReadCommandResponse(221);
    if (!bSuccess)
    {
      SetLastError(ERROR_BAD_COMMAND);
      TRACE(_T("An unexpected QUIT response was received\n"));
    }

    //Reset all the state variables
    m_bConnected = FALSE;
  }
  else
    TRACE(_T("Already disconnected from SMTP server, doing nothing\n"));
 
  //free up our socket
  m_SMTP.Close();
 
  return bSuccess;
}

BOOL CSMTPConnection::SendMessage(CSMTPMessage& Message)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

	//paramater validity checking
  ASSERT(m_bConnected); //Must be connected to send a message

  //Send the MAIL command
	ASSERT(Message.m_From.m_sEmailAddress.GetLength());
  CString sBuf;
  sBuf.Format(_T("MAIL FROM:<%s>\r\n"), Message.m_From.m_sEmailAddress);
  LPCSTR pszMailFrom = T2A((LPTSTR) (LPCTSTR) sBuf);
  int nCmdLength = strlen(pszMailFrom);
  if (!m_SMTP.Send(pszMailFrom, nCmdLength))
  {
    TRACE(_T("Failed in call to send MAIL command, GetLastError returns: %d\n"), GetLastError());
    return FALSE;
  }

  //check the response to the MAIL command
  if (!ReadCommandResponse(250))
  {
    SetLastError(ERROR_BAD_COMMAND);
    TRACE(_T("An unexpected MAIL response was received\n"));
    return FALSE;
  } 

  //Send the RCPT command, one for each recipient (includes the TO, CC & BCC recipients)

  //Must be sending to someone
  ASSERT(Message.GetNumberOfRecipients(CSMTPMessage::TO) + 
         Message.GetNumberOfRecipients(CSMTPMessage::CC) + 
         Message.GetNumberOfRecipients(CSMTPMessage::BCC));

  //First the "To" recipients
  for (int i=0; i<Message.GetNumberOfRecipients(CSMTPMessage::TO); i++)
  {
    CSMTPAddress recipient = Message.GetRecipient(i, CSMTPMessage::TO);
    if (!SendRCPTForRecipient(recipient))
      return FALSE;
  }

  //Then the "CC" recipients
  for (i=0; i<Message.GetNumberOfRecipients(CSMTPMessage::CC); i++)
  {
    CSMTPAddress recipient = Message.GetRecipient(i, CSMTPMessage::CC);
    if (!SendRCPTForRecipient(recipient))
      return FALSE;
  }

  //Then the "BCC" recipients
  for (i=0; i<Message.GetNumberOfRecipients(CSMTPMessage::BCC); i++)
  {
    CSMTPAddress recipient = Message.GetRecipient(i, CSMTPMessage::BCC);
    if (!SendRCPTForRecipient(recipient))
      return FALSE;
  }

  //Send the DATA command
  char* pszDataCommand = "DATA\r\n";
  nCmdLength = strlen(pszDataCommand);
  if (!m_SMTP.Send(pszDataCommand, nCmdLength))
  {
    TRACE(_T("Failed in call to send MAIL command, GetLastError returns: %d\n"), GetLastError());
    return FALSE;
  }

  //check the response to the DATA command
  if (!ReadCommandResponse(354))
  {
    SetLastError(ERROR_BAD_COMMAND);
    TRACE(_T("An unexpected DATA response was received\n"));
    return FALSE;
  } 

  //Send the Header
  CString sHeader = Message.GetHeader();
  char* pszHeader = T2A((LPTSTR) (LPCTSTR) sHeader);
  nCmdLength = strlen(pszHeader);
  if (!m_SMTP.Send(pszHeader, nCmdLength))
  {
    TRACE(_T("Failed in call to send the header, GetLastError returns: %d\n"), GetLastError());
    return FALSE;
  }

	//Send the Mime Header for the body
  if (Message.m_Attachments.GetSize())
  {
	  char* psBodyHeader = (_T("\r\n--#BOUNDARY#\r\n")\
		                      _T("Content-Type: text/plain; charset=us-ascii\r\n")\
									        _T("Content-Transfer-Encoding: quoted-printable\r\n\r\n"));
    nCmdLength = strlen(psBodyHeader);
    if (!m_SMTP.Send(psBodyHeader, nCmdLength))
    {
      TRACE(_T("Failed in call to send the body header, GetLastError returns: %d\n"), GetLastError());
      return FALSE;
    }
  }

  //Send the body
  char* pszBody = T2A((LPTSTR) (LPCTSTR) Message.m_sBody);
  nCmdLength = strlen(pszBody);
  if (!m_SMTP.Send(pszBody, nCmdLength))
  {
    TRACE(_T("Failed in call to send the header, GetLastError returns: %d\n"), GetLastError());
    return FALSE;
  }

  //Send all the attachments
  for (i=0; i<Message.m_Attachments.GetSize(); i++)
  {
    CSMTPAttachment* pAttachment = Message.m_Attachments.GetAt(i);

		//First send the Mime header for each attachment
		CString sContent;
		sContent.Format(_T("\r\n\r\n--#BOUNDARY#\r\n")\
		                _T("Content-Type: application/octet-stream; name=%s\r\n")\
										_T("Content-Transfer-Encoding: base64\r\n")\
										_T("Content-Disposition: attachment; filename=%s\r\n\r\n"), 
                    pAttachment->GetFilename(), pAttachment->GetTitle());

		char* pszContent = T2A((LPTSTR) (LPCTSTR) sContent);
		nCmdLength = strlen(pszContent);
		if (!m_SMTP.Send(pszContent, nCmdLength))
		{
			TRACE(_T("Failed in call to send Mime attachment header, GetLastError returns: %d\n"), GetLastError());
			return FALSE;
		}

    //Then send the encoded attachment
    if (!m_SMTP.Send(pAttachment->GetEncodedBuffer(), pAttachment->GetEncodedSize()))
    {
      TRACE(_T("Failed in call to send the attachment, GetLastError returns: %d\n"), GetLastError());
      return FALSE;
    }
  }

	//Send the final mime boundary
  if (Message.m_Attachments.GetSize())
  {
	  char* pszFinalBoundary = "\r\n--#BOUNDARY#--";
	  nCmdLength = strlen(pszFinalBoundary);
	  if (!m_SMTP.Send(pszFinalBoundary, nCmdLength))
	  {
		  TRACE(_T("Failed in call to send Mime attachment header, GetLastError returns: %d\n"), GetLastError());
		  return FALSE;
	  }
  }

  //Send the end of message indicator
  char* pszEOM = "\r\n.\r\n";
	nCmdLength = strlen(pszEOM);
  if (!m_SMTP.Send(pszEOM, nCmdLength))
  {
    TRACE(_T("Failed in call to send end of message indicator, GetLastError returns: %d\n"), GetLastError());
    return FALSE;
  }

  //check the response to the End of Message command
  if (!ReadCommandResponse(250))
  {
    SetLastError(ERROR_BAD_COMMAND);
    TRACE(_T("An unexpected end of message response was received\n"));
    return FALSE;
  } 

	return TRUE;
}

BOOL CSMTPConnection::SendRCPTForRecipient(CSMTPAddress& recipient)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

	ASSERT(recipient.m_sEmailAddress.GetLength()); //must have an email address for this recipient

  CString sBuf;
  sBuf.Format(_T("RCPT TO:<%s>\r\n"), recipient.m_sEmailAddress);
  LPTSTR pszRCPT = T2A((LPTSTR) (LPCTSTR) sBuf);

  int nCmdLength = strlen(pszRCPT);
  if (!m_SMTP.Send(pszRCPT, nCmdLength))
  {
    TRACE(_T("Failed in call to send RCPT command, GetLastError returns: %d\n"), GetLastError());
    return FALSE;
  }

  //check the response to the RCPT command
  if (!ReadCommandResponse(250))
  {
    SetLastError(ERROR_BAD_COMMAND);
    TRACE(_T("An unexpected RCPT response was received\n"));
    return FALSE;
  } 
  
  return TRUE;
}

BOOL CSMTPConnection::ReadCommandResponse(int nExpectedCode)
{
  LPSTR pszOverFlowBuffer = NULL;
  char sBuf[256];
  BOOL bSuccess = ReadResponse(sBuf, 256, "\r\n", nExpectedCode, &pszOverFlowBuffer);
  if (pszOverFlowBuffer)
    delete [] pszOverFlowBuffer;

  return bSuccess;
}

BOOL CSMTPConnection::ReadResponse(LPSTR pszBuffer, int nInitialBufSize, LPSTR pszTerminator, int nExpectedCode, LPSTR* ppszOverFlowBuffer, int nGrowBy)
{
  ASSERT(ppszOverFlowBuffer);          //Must have a valid string pointer
  ASSERT(*ppszOverFlowBuffer == NULL); //Initially it must point to a NULL string

  //must have been created first
  ASSERT(m_bConnected);

  //The local variables which will receive the data
  LPSTR pszRecvBuffer = pszBuffer;
  int nBufSize = nInitialBufSize;
  
  //retrieve the reponse using until we
	//get the terminator or a timeout occurs
	BOOL bFoundTerminator = FALSE;
	int nReceived = 0;
	DWORD dwStartTicks = ::GetTickCount();
	while (!bFoundTerminator)
	{
		//Has the timeout occured
		if ((::GetTickCount() - dwStartTicks) >	m_dwTimeout)
		{
		  pszRecvBuffer[nReceived] = '\0';
      SetLastError(WSAETIMEDOUT);
      m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
			return FALSE;
		}

    //check the socket for readability
    BOOL bReadible;
    if (!m_SMTP.IsReadible(bReadible))
    {
	    pszRecvBuffer[nReceived] = '\0';
			m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
			return FALSE;
    }
    else if (!bReadible) //no data to receive, just loop around
    {
      Sleep(250); //Sleep for a while before we loop around again
      continue;
    }

		//receive the data from the socket
    int nBufRemaining = nBufSize-nReceived-1; //Allows allow one space for the NULL terminator
    if (nBufRemaining<0)
      nBufRemaining = 0;
	  int nData = m_SMTP.Receive(pszRecvBuffer+nReceived, nBufRemaining);

    //Reset the idle timeout if data was received
    if (nData)
    {
			dwStartTicks = ::GetTickCount();

      //Increment the count of data received
		  nReceived += nData;							   
    }

    //If an error occurred receiving the data
		if (nData == SOCKET_ERROR)
		{
      //NULL terminate the data received
      if (pszRecvBuffer)
		    pszBuffer[nReceived] = '\0';

      m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
		  return FALSE; 
		}
		else
		{
      //NULL terminate the data received
      if (pszRecvBuffer)
		    pszRecvBuffer[nReceived] = '\0';

      if (nBufRemaining-nData == 0) //No space left in the current buffer
      {
        //Allocate the new receive buffer
        nBufSize += nGrowBy; //Grow the buffer by the specified amount
        LPSTR pszNewBuf = new char[nBufSize];

        //copy the old contents over to the new buffer and assign 
        //the new buffer to the local variable used for retreiving 
        //from the socket
        if (pszRecvBuffer)
          strcpy(pszNewBuf, pszRecvBuffer);
        pszRecvBuffer = pszNewBuf;

        //delete the old buffer if it was allocated
        if (*ppszOverFlowBuffer)
          delete [] *ppszOverFlowBuffer;
        
        //Remember the overflow buffer for the next time around
        *ppszOverFlowBuffer = pszNewBuf;        
      }
		}

    //Check to see if the terminator character(s) have been found
		bFoundTerminator = (strstr(pszRecvBuffer, pszTerminator) != NULL);
	}

	//Remove the terminator from the response data
  pszRecvBuffer[nReceived - strlen(pszTerminator)] = '\0';

  //determine if the response is an error
	char sCode[4];
	strncpy(sCode, pszRecvBuffer, 3);
	sCode[3] = '\0';
	sscanf(sCode, "%d", &m_nLastCommandResponseCode);
	BOOL bSuccess = (m_nLastCommandResponseCode == nExpectedCode);

  if (!bSuccess)
  {
    SetLastError(WSAEPROTONOSUPPORT);
    m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
  }

  return bSuccess;
}
