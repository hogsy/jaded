/*
Module : SMTP.H
Purpose: Defines the interface for a MFC class encapsulation of the SMTP protocol
Created: PJN / 22-05-1998

Copyright (c) 1998 - 2000 by PJ Naughter.  
All rights reserved.

*/


/////////////////////////////// Defines ///////////////////////////////////////
#ifndef __SMTP_H__
#define __SMTP_H__

#ifndef __AFXTEMPL_H__
#pragma message("SMTP classes require afxtempl.h in your PCH")                                                                                
#endif

#ifndef _WINSOCKAPI_
#pragma message("SMTP classes require afxsock.h or winsock.h in your PCH")
#endif

#ifndef __AFXPRIV_H__
#pragma message("SMTP classes requires afxpriv.h in your PCH")
#endif
  

/////////////////////////////// Classes ///////////////////////////////////////


//Simple Socket wrapper class
class CSMTPSocket
{
public:
//Constructors / Destructors
  CSMTPSocket();
  ~CSMTPSocket();

//methods
  BOOL  Create();
  BOOL  Connect(LPCTSTR pszHostAddress, int nPort = 110);
  BOOL  Send(LPCSTR pszBuf, int nBuf);
  void  Close();
  int   Receive(LPSTR pszBuf, int nBuf);
  BOOL  IsReadible(BOOL& bReadible);

protected:
  BOOL   Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);
  SOCKET m_hSocket;
};

                     
//Encapsulation of an SMTP email address, used for recipients and in the From: field
class CSMTPAddress
{
public: 
//Constructors / Destructors
  CSMTPAddress();
  CSMTPAddress(const CSMTPAddress& address);
	CSMTPAddress(const CString& sAddress);
	CSMTPAddress(const CString& sFriendly, const CString& sAddress);
	CSMTPAddress& operator=(const CSMTPAddress& r);

//Methods
  CString GetRegularFormat() const;

//Data members
	CString        m_sFriendlyName; //Would set it to contain something like "PJ Naughter"
  CString        m_sEmailAddress; //Would set it to contains something like "pjn@indigo.ie"
};


//Encapsulation of an SMTP file attachment
class CSMTPAttachment
{
public:
//Constructors / Destructors
	CSMTPAttachment();
  ~CSMTPAttachment();

//methods
  BOOL Attach(const CString& sFilename);
  CString GetFilename() const { return m_sFilename; };
  const char* GetEncodedBuffer() const { return m_pszEncoded; };
  int GetEncodedSize() const { return m_nEncodedSize; };
  CString GetTitle() const { return m_sTitle; };
  void SetTitle(const CString& sTitle) { m_sTitle = sTitle; };

protected:
  int Base64BufferSize(int nInputSize);
  BOOL EncodeBase64(const char* aIn, int aInLen, char* aOut, int aOutSize, int* aOutLen);
  static char m_base64tab[];

  CString  m_sFilename;    //The filename you want to send
  CString  m_sTitle;       //What it is to be known as when emailed
  char*    m_pszEncoded;   //The encoded representation of the file
  int      m_nEncodedSize; //size of the encoded string
};


////////////////// Forward declaration
class CSMTPConnection;


//Encapsulation of an SMTP message
class CSMTPMessage
{
public:
//Enums
	enum RECIPIENT_TYPE { TO, CC, BCC };

//Constructors / Destructors
  CSMTPMessage();
  ~CSMTPMessage();

//Recipient support
	int              GetNumberOfRecipients(RECIPIENT_TYPE RecipientType = TO) const;
	int              AddRecipient(CSMTPAddress& recipient, RECIPIENT_TYPE RecipientType = TO);
	void             RemoveRecipient(int nIndex, RECIPIENT_TYPE RecipientType = TO);
	CSMTPAddress     GetRecipient(int nIndex, RECIPIENT_TYPE RecipientType = TO) const;

//Attachment support
  int              GetNumberOfAttachments() const;
	int              AddAttachment(CSMTPAttachment* pAttachment);
	void             RemoveAttachment(int nIndex);
	CSMTPAttachment* GetAttachment(int nIndex) const;

//Misc methods
  virtual CString  GetHeader() const;
  void             AddBody(const CString& sBody);
  BOOL             AddMultipleRecipients(const CString& sRecipients, RECIPIENT_TYPE RecipientType);
	                          
//Data Members
	CSMTPAddress m_From;
	CString      m_sSubject;
  CString      m_sXMailer;
	CSMTPAddress m_ReplyTo;

protected:
  void FixSingleDot(CString& sBody);

	CString m_sBody;
	CArray<CSMTPAddress, CSMTPAddress&> m_ToRecipients;
	CArray<CSMTPAddress, CSMTPAddress&> m_CCRecipients;
	CArray<CSMTPAddress, CSMTPAddress&> m_BCCRecipients;
  CArray<CSMTPAttachment*, CSMTPAttachment*&> m_Attachments;

  friend class CSMTPConnection;
};


//The main class which encapsulates the SMTP connection
class CSMTPConnection
{
public:
//Constructors / Destructors
  CSMTPConnection();
  ~CSMTPConnection();

//Methods
  BOOL    Connect(LPCTSTR pszHostName, int nPort=25);
  BOOL    Disconnect();
  CString GetLastCommandResponse() const { return m_sLastCommandResponse; };
  int     GetLastCommandResponseCode() const { return m_nLastCommandResponseCode; };
  DWORD   GetTimeout() const { return m_dwTimeout; };
  void    SetTimeout(DWORD dwTimeout) { m_dwTimeout = dwTimeout; };
	BOOL    SendMessage(CSMTPMessage& Message);

protected:
  BOOL SendRCPTForRecipient(CSMTPAddress& recipient);
	virtual BOOL ReadCommandResponse(int nExpectedCode);
  virtual BOOL ReadResponse(LPSTR pszBuffer, int nInitialBufSize, LPSTR pszTerminator, 
                            int nExpectedCode, LPSTR* ppszOverFlowBuffer, int nGrowBy=4096);

  CSMTPSocket m_SMTP;
  BOOL        m_bConnected;
  CString     m_sLastCommandResponse;
	DWORD       m_dwTimeout;
  int         m_nLastCommandResponseCode;
};


#endif //__SMTP_H__

