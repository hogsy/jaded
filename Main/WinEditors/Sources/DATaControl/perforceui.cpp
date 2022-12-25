// (C) Copyright 2004 Ubi Soft Entertainment Inc.
//
// $File: //technology-group/latest_version/tools/sourcecontrol/private/src/perforce/perforceui.cpp $
// $Revision: #8 $
// $Change: 80731 $
// $DateTime: 2004/06/04 11:33:53 $


#include "precomp.h"
#include "perforceui.h"
#include "Greta/syntax2.h"


//
// PerforceUI
//

PerforceUI::PerforceUI()
: m_isError(false)
{
}

PerforceUI::~PerforceUI()
{
}

void PerforceUI::InputData(StrBuf* /*strbuf*/, Error* /*e*/)
{
    assert(false);
}

void PerforceUI::Prompt( const StrPtr &msg, StrBuf &rsp, 
						int noEcho, Error *e )
{
	assert(false);						
}

void PerforceUI::HandleError(Error* pErr )
{
	StrBuf buf;
	pErr->Fmt(buf, EF_PLAIN);
	OutputDebugString(buf.Text());
    assert(false);
}

void PerforceUI::Message(Error* /*err*/)
{
    assert(false);
}

void PerforceUI::OutputError(const_char* /*errBuf*/)
{
    assert(false);
}

void PerforceUI::OutputInfo(char /*level*/, const_char* /*data*/)
{
    assert(false);
}

void PerforceUI::OutputBinary(const_char* /*data*/, int /*length*/)
{
    assert(false);
}

void PerforceUI::OutputText(const_char* /*data*/, int /*length*/)
{
    assert(false);
}

void PerforceUI::OutputStat(StrDict* /*varList*/)
{
    assert(false);
}


PerforceMonitorUI::PerforceMonitorUI() : 
m_iProcessCount(0)
{
}

PerforceMonitorUI::~PerforceMonitorUI()
{
}

void PerforceMonitorUI::Message(Error* pErr)
{
	StrBuf buf;
	pErr->Fmt(buf, EF_PLAIN);

	static const regex::rpattern ProcessPattern(".+[0-9][0-9]:[0-9][0-9]:[0-9][0-9].+");
	regex::match_results results;
	std::string str = buf.Text();
	if (ProcessPattern.match(str, results).matched)
	{
		m_iProcessCount ++;
	}
}

//
// PerforceChangeUI
//

PerforceChangeUI::PerforceChangeUI(
	const char* change,	
    const char* client,
    const char* user,
    const char* description,
	const char* files)
{
    assert(description); // Perforce says this argument is mandatory

    FillForm(change,client, user, description,files);
}

PerforceChangeUI::~PerforceChangeUI()
{
}

STL::string PerforceChangeUI::GetChangeNum() const
{
    assert(!IsError());
    assert(!m_changeNum.empty());

    return m_changeNum;
}

void PerforceChangeUI::FillForm(
	const char* change,							
    const char* client,
    const char* user,
    const char* description,
	const char* files)
{   
    static const char* form = 
        "Change: %change%\n\n"
        "Client: %client%\n\n"
        "User: %user%\n\n"
        "Description: %description%\n\n"
		"Files: %files%\n\n";

    m_filledForm = form;
    regex::subst_results results;

	const regex::rpattern changePattern("%change%", change);
	if (!changePattern.substitute(m_filledForm, results))
		assert(false);

    const regex::rpattern clientPattern("%client%", client);
    if (!clientPattern.substitute(m_filledForm, results))
        assert(false);

    const regex::rpattern userPattern("%user%", user);
    if (!userPattern.substitute(m_filledForm, results))
        assert(false);

    // To make Perforce happy, we should format the description so that each line 
    // begins with a space, otherwise it will think each line is a new field in the form
    static const regex::rpattern descriptionFormatPattern("\n[^ ]", "\n ", regex::GLOBAL);
    STL::string formattedDescription(description);
    descriptionFormatPattern.substitute(formattedDescription, results);

    const regex::rpattern descriptionPattern("%description%", formattedDescription);
    if (!descriptionPattern.substitute(m_filledForm, results))
        assert(false);

	const regex::rpattern filesPattern("%files%", files);
	if (!filesPattern.substitute(m_filledForm, results))
		assert(false);
}

void PerforceChangeUI::InputData(StrBuf* strbuf, Error* /*error*/)
{
    strbuf->Set(m_filledForm.c_str());
}

void PerforceChangeUI::Message(Error* error)
{
    SetError(error->IsError() != 0);

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    OutputDebugString(buf.Text());
	
	if (IsError())
        return;

    static const regex::rpattern pattern("Change ([0-9]+) (created|deleted)");
    regex::match_results results;
    regex::match_results::backref_type ref;
	std::string str = buf.Text();
    ref = pattern.match(str, results);

    if (!ref.matched) 
    {
        // Got different message than expected...
        SetError(true); 
        return;
    }

    m_changeNum = results.backref(1).str();
}

//
// PerforceClientUI
//

PerforceClientUI::PerforceClientUI(const char* client,
								   const char* host,
								   const char* owner,
								   const char* root,
								   const char* description,
								   const char* view)
{
	FillForm(client,host, owner, root,description,view);
}

PerforceClientUI::~PerforceClientUI()
{
}

void PerforceClientUI::FillForm(const char* client,
								const char* host,
								const char* owner,
								const char* root,
								const char* description,
								const char* view)
{   
	static const char* form = 
		"Client: %client%\n\n"
		"Host: %host%\n\n"
		"Owner: %owner%\n\n"
		"Root: %root%\n\n"
		"Options: noallwrite clobber nocompress unlocked nomodtime normdir\n\n"
		"Description: %description%\n\n"
		"View: %view%\n\n";
		

	m_filledForm = form;
	regex::subst_results results;

	const regex::rpattern clientPattern("%client%", client);
	if (!clientPattern.substitute(m_filledForm, results))
		assert(false);

	const regex::rpattern hostPattern("%host%", host);
	if (!hostPattern.substitute(m_filledForm, results))
		assert(false);

	const regex::rpattern ownerPattern("%owner%", owner);
	if (!ownerPattern.substitute(m_filledForm, results))
		assert(false);

	const regex::rpattern rootPattern("%root%", root);
	if (!rootPattern.substitute(m_filledForm, results))
		assert(false);

	const regex::rpattern DescriptionPattern("%description%", description);
	if (!DescriptionPattern.substitute(m_filledForm, results))
		assert(false);

	// To make Perforce happy, we should format the description so that each line 
	// begins with a space, otherwise it will think each line is a new field in the form
	//static const regex::rpattern viewFormatPattern("\n[^ ]", "\n ", regex::GLOBAL);
	//STL::string formattedView(view);
	//viewFormatPattern.substitute(formattedView, results);

	//const regex::rpattern descriptionPattern("%view%", formattedView);
	const regex::rpattern ViewPattern("%view%", view);
	if (!ViewPattern.substitute(m_filledForm, results))
		assert(false);
}

void PerforceClientUI::InputData(StrBuf* strbuf, Error* /*error*/)
{
	strbuf->Set(m_filledForm.c_str());
}

void PerforceClientUI::Message(Error* error)
{
	SetError(error->IsError() != 0);

	// Process the message returned by the command
	StrBuf buf;
	error->Fmt(buf, EF_PLAIN);

	OutputDebugString(buf.Text());

	if (IsError())
		return;

}

void  PerforceClientUI::OutputInfo(char /*level*/, const_char* data)
{
	regex::subst_results substResults;
	regex::match_results results;
	static const regex::rpattern FormatPattern("(\n|\r|\t)", " ", regex::GLOBAL);
	
	std::string strData(data);
	strData = strstr(data,"# Use");
	strData = strstr(strData.c_str(),"Description:");
	FormatPattern.substitute(strData, substResults);

	static const regex::rpattern pattern("Description:  (.*)  Root.*");

	regex::match_results::backref_type ref;
	ref = pattern.match(strData, results);

	if (ref.matched) 
	{
		m_description = results.backref(1).str();
	}
}

//
// PerforceChangeDeleteUI
//

PerforceChangeDeleteUI::PerforceChangeDeleteUI()
{
}

PerforceChangeDeleteUI::~PerforceChangeDeleteUI()
{
}

void PerforceChangeDeleteUI::Message(Error* error)
{
    SetError(error->IsError() != 0);

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    if (IsError())
        return;

    static const regex::rpattern pattern("Change [0-9]+ deleted");
    regex::match_results results;
    regex::match_results::backref_type ref;
	std::string str = buf.Text();
    ref = pattern.match(str, results);

    if (!ref.matched) 
    {
        // Got different message than expected...
        SetError(true); 
        return;
    }
}

//
// PerforceFstatUI
//

PerforceFstatUI::PerforceFstatUI() :
	m_isAccessDenied(false)
{
}

PerforceFstatUI::~PerforceFstatUI()
{
}

void PerforceFstatUI::Message(Error* error)
{
    // The message "no such file" is considered as a warning by Perforce -- but 
    // regarding fstat, I consider it an error!
    SetError(error->GetSeverity() != E_EMPTY);

    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    if (IsError())
    {
        // Check whether or not access was denied
        static const regex::rpattern pattern("(?:Access for user '.+' has not been enabled|Perforce password \\(P4PASSWD\\) invalid or unset)");
        regex::match_results results;
		std::string str = buf.Text();
        m_isAccessDenied = pattern.match(str, results).matched;
    }
}

void PerforceFstatUI::OutputStat(StrDict* varList)
{
    if (IsError())
        return;

	Result result;

    StrPtr* value;

    value = varList->GetVar("depotFile");
    if (value)
        result.m_depotFile = value->Text();

    value = varList->GetVar("clientFile");
    if (value)
        result.m_clientFile = value->Text();

    value = varList->GetVar("headAction");
    if (value)
        result.m_headAction = value->Text();

    value = varList->GetVar("action");
    if (value)
        result.m_action = value->Text();

    value = varList->GetVar("resolveBaseFile0");
    if (value)
        result.m_resolveBase = value->Text();

    value = varList->GetVar("resolveBaseRev0");
    if (value)
        result.m_resolveBaseRev = value->Text();

    value = varList->GetVar("resolveFromFile0");
    if (value)
        result.m_resolveFrom = value->Text();

    value = varList->GetVar("resolveEndFromRev0");
    if (value)
        result.m_resolveFromRev = value->Text();

	m_results.push_back(result);
}

//
// PerforceDirsUI
//

PerforceDirsUI::PerforceDirsUI()
{
}

PerforceDirsUI::~PerforceDirsUI()
{
}

void PerforceDirsUI::Message(Error* error)
{
    SetError(error->IsError() != 0);

    if (IsError())
        return;

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    static const regex::rpattern errorPattern(" no such file\\(s\\)");
    regex::match_results results;
	std::string str = buf.Text();
    if (errorPattern.match(str, results).matched)
    {
        SetError(true);
        return;
    }

    m_dirs.push_back(buf.Text());
}

//
// PerforceFilesUI
//

PerforceFilesUI::PerforceFilesUI()
{
}

PerforceFilesUI::~PerforceFilesUI()
{
}

void PerforceFilesUI::Message(Error* error)
{
    SetError(error->IsError() != 0);

    if (IsError())
        return;

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    static const regex::rpattern filePattern("(.+)#[0-9]+ - .+");
    regex::match_results results;
    regex::match_results::backref_type ref;
	std::string str = buf.Text();
    ref = filePattern.match(str, results); // Keep only the file name
    if (ref.matched)
        m_files.push_back(results.backref(1).str());
}


//
// PerforceUsersUI
//

PerforceUsersUI::PerforceUsersUI()
{
}

PerforceUsersUI::~PerforceUsersUI()
{
}

void PerforceUsersUI::Message(Error* error)
{
	SetError(error->IsError() != 0);

	if (IsError())
		return;

	// Process the message returned by the command
	StrBuf buf;
	error->Fmt(buf, EF_PLAIN);

	static const regex::rpattern userPattern("(.+) <.+");
	regex::match_results results;
	regex::match_results::backref_type ref;
	std::string str = buf.Text();
	ref = userPattern.match(str, results); // Keep only the file name
	if (ref.matched)
		m_lUsers.push_back(results.backref(1).str());
}

//
// PerforceUsersUI
//

PerforceClientsUI::PerforceClientsUI()
{
}

PerforceClientsUI::~PerforceClientsUI()
{
}

void PerforceClientsUI::Message(Error* error)
{
	SetError(error->IsError() != 0);

	if (IsError())
		return;

	// Process the message returned by the command
	StrBuf buf;
	error->Fmt(buf, EF_PLAIN);

	static const regex::rpattern clientPattern("Client (.+) [0-9].+");
	regex::match_results results;
	regex::match_results::backref_type ref;
	std::string str = buf.Text();
	ref = clientPattern.match(str, results); // Keep only the file name
	if (ref.matched)
		m_lClient.push_back(results.backref(1).str());
}

//
// PerforceSyncUI
//

PerforceSyncUI::PerforceSyncUI()
: m_isUpToDate(false),
m_isNotOnServer(false),
m_IsSync(false),
m_IsDeleted(false)
{
}

PerforceSyncUI::~PerforceSyncUI()
{
}

void PerforceSyncUI::Message(Error* error)
{
	m_isNotOnServer = false;
	m_isUpToDate	= false;
	m_IsSync		= false;
	m_IsDeleted	    = false;

	SetError(error->IsError() != 0);

    if (IsError())
    {
        m_isUpToDate = false;
        return;
    }

    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    // As soon as a message says something else than "file(s) up-to-date", 
    // we consider the file as not up-to-date
    static const regex::rpattern upToDatePattern(".+ - file\\(s\\) up-to-date");
    regex::match_results results;
    regex::match_results::backref_type ref;

	if (upToDatePattern.match(std::string(buf.Text()), results).matched)
		m_isUpToDate = true;

	static const regex::rpattern noSuchFilePattern(".+ - no such file\\(s\\).");

	if (noSuchFilePattern.match(std::string(buf.Text()), results).matched)
        m_isNotOnServer = true;

	static const regex::rpattern syncPattern(".+ - refreshing|updating .+");

	if (syncPattern.match(std::string(buf.Text()), results).matched)
		m_IsSync = true;

	static const regex::rpattern syncPatternDeleted(".+ - deleted .+");

	if (syncPatternDeleted.match(std::string(buf.Text()), results).matched)
		m_IsDeleted = true;

}

//
// PerforceInfoUI
//

PerforceInfoUI::PerforceInfoUI()
{
}

PerforceInfoUI::~PerforceInfoUI()
{
}

void PerforceInfoUI::Message(Error* error)
{
    SetError(error->IsError() != 0);

    if (IsError())
        return;

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    regex::match_results results;
    regex::match_results::backref_type ref;
    
    //TODO: It would be more efficient to concatenate all the messages into a buffer 
    //      and then having a single regex search through the whole buffer.

    if (m_userName.empty()) // If not found yet, try to match it with the current message
    {
        static const regex::rpattern pattern("^User name: (.+)");
        ref = pattern.match(std::string(buf.Text()), results);
        if (ref.matched)
            m_userName = results.backref(1).str();
    }
    else if (m_clientName.empty()) 
    {
        static const regex::rpattern pattern("^Client name: (.+)");
        ref = pattern.match(std::string(buf.Text()), results);
        if (ref.matched)
            m_clientName = results.backref(1).str();
    }
    else if (m_clientRoot.empty()) 
    {
        static const regex::rpattern pattern("^Client root: (.+)");
        ref = pattern.match(std::string(buf.Text()), results);
        if (ref.matched)
            m_clientRoot = results.backref(1).str();
    }
}

//
// PerforceAddUI
//

PerforceAddUI::PerforceAddUI()
: m_isOpenedAdd(false),
  m_isCurrentlyOpened(false),
  m_isExistingFile(false)
{
}

PerforceAddUI::~PerforceAddUI()
{
}

void PerforceAddUI::Message(Error* error)
{
	m_isOpenedAdd = false;
	m_isCurrentlyOpened = false;
	m_isExistingFile = false;

    SetError(error->IsError() != 0);

	if (IsError())
        return;

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

	if (buf.Length() == 0)
    {
        // An empty message means nothing was added
        SetError(true);
        return;
    }

	static const regex::rpattern openedPattern(".+ - opened for add");
	static const regex::rpattern currentlyOpenedPattern(".+ - (currently opened for add|can't change from|can't add \\(already opened*)");
	static const regex::rpattern existingFilePattern(".+ - (can't add existing file|warning: add of existing file)");
	regex::match_results results;

    if (openedPattern.match(std::string(buf.Text()), results).matched)
        m_isOpenedAdd = true;

	else if (currentlyOpenedPattern.match(std::string(buf.Text()), results).matched)
	{
        SetError(true);
		m_isCurrentlyOpened = true;
	}

	else if (existingFilePattern.match(std::string(buf.Text()), results).matched) 
    {
        SetError(true);
		m_isExistingFile = true;
    }
}

void PerforceAddUI::OutputInfo(char /*level*/, const_char* /*data*/)
{
}

//
// PerforceEditUI
//

PerforceEditUI::PerforceEditUI()
: m_isOpened(false),
  m_isOlderRevision(false),
  m_isCurrentlyOpened(false),
  m_isOpenedExclusively(false),
  m_isUnknown(false)
{
}

PerforceEditUI::~PerforceEditUI()
{
}

void PerforceEditUI::Message(Error* error)
{
	m_isOpened = false;
	m_isOlderRevision = false;
	m_isCurrentlyOpened = false;
	m_isOpenedExclusively = false;
	m_isUnknown = false;

	SetError(error->IsError() != 0);

	if (IsError())
        return;

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

	if (buf.Length() == 0)
    {
        // An empty message means nothing was edited
		m_isOpened = false;
        SetError(true);
        return;
    }

	static const regex::rpattern openedPattern(" - opened for edit");
	static const regex::rpattern currentlyOpenedPattern(" - currently opened for edit");
	static const regex::rpattern currentlyOpenedPattern2(" - use 'reopen'");
    static const regex::rpattern openedExclusivelyPattern(" - can't edit exclusive file");
	static const regex::rpattern unknownPattern(" - file\\(s\\) not on client");
    
	regex::match_results results;

    m_isOlderRevision = STL::string(buf.Text()).find(" - must sync/resolve") != STL::string::npos;

    if (openedPattern.match(std::string(buf.Text()), results).matched)
        m_isOpened = true;

	else if (currentlyOpenedPattern.match(std::string(buf.Text()), results).matched) 
		m_isCurrentlyOpened = true;
	else if (currentlyOpenedPattern2.match(std::string(buf.Text()), results).matched) 
		m_isCurrentlyOpened = true;
	else if (openedExclusivelyPattern.match(std::string(buf.Text()), results).matched || m_isOpenedExclusively)
	{
		SetError(true);	
		m_isOpenedExclusively = true;
	}
	else if (unknownPattern.match(std::string(buf.Text()), results).matched) 
    {
        SetError(true);
		m_isUnknown = true;
		m_isOpened = false;
    }
}

void PerforceEditUI::OutputInfo(char /*level*/, const_char* /*data*/)
{
}

//
// PerforceDeleteUI
//

PerforceDeleteUI::PerforceDeleteUI()
: m_isOpenedDelete(false),
  m_isAlreadyDeleted(false),
  m_isAlreadyOpened(false),
  m_isUnknown(false)
{
}

PerforceDeleteUI::~PerforceDeleteUI()
{
}

void PerforceDeleteUI::Message(Error* error)
{
	m_isOpenedDelete = false;
	m_isAlreadyDeleted = false;
	m_isAlreadyOpened = false;
	m_isUnknown = false;

	SetError(error->IsError() != 0);

	if (IsError())
        return;

 // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN); 

	if (buf.Length() == 0)
    {
        // An empty message means nothing was added
        SetError(true);
        return;
    }

	static const regex::rpattern openedDeletePattern(".+ - opened for delete");
	static const regex::rpattern alreadyDeletedPattern(".+ - (currently opened for delete|can't change from)");
    static const regex::rpattern unknownPattern(".+ file\\(s\\) not on client");
	static const regex::rpattern alreadyOpenedPattern(".+ - (can't delete|warning:)");

	regex::match_results results;

	std::string str = buf.Text();

    if (openedDeletePattern.match(str, results).matched)
        m_isOpenedDelete = true;

	else if (alreadyDeletedPattern.match(str, results).matched) 
	{
        SetError(true);	
		m_isAlreadyDeleted = true;
	}
	
	else if (alreadyOpenedPattern.match(str, results).matched) 
	{
        SetError(true);
		m_isAlreadyOpened = true;
	}
	
	else if (unknownPattern.match(str, results).matched) 
    {
        SetError(true);
		m_isUnknown = true;
    }
}

void PerforceDeleteUI::HandleError(Error* /*err*/)
{
}

//
// PerforceRevertUI
//

PerforceRevertUI::PerforceRevertUI()
: m_isReverted(false),
  m_isNotOpened(false)
{
}

PerforceRevertUI::~PerforceRevertUI()
{
}

void PerforceRevertUI::Message(Error* error)
{
	m_isReverted = false;
	m_isNotOpened = false;

	SetError(error->IsError() != 0);

    if (IsError())
        return;

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    if (buf.Length() == 0)
    {
        // An empty message means nothing was reverted
        SetError(true);
        return;
    }

    static const regex::rpattern revertedPattern(" - was .+,.+reverted|abandoned");
    //static const regex::rpattern notOpenedPattern(" - file\\(s\\) not opened on this client");

    bool notOpened = STL::string(buf.Text()).find(" - file(s) not opened on this client") != STL::string::npos;

    regex::match_results results;

    if (notOpened) 
	{
        // The files were not opened, thus could not be reverted
        SetError(true);
        m_isNotOpened = true; 
    }
    else if (revertedPattern.match(std::string(buf.Text()), results).matched)
    {
        m_isReverted = true;
    }
}

void PerforceRevertUI::OutputInfo(char /*level*/, const_char* /*data*/)
{
}

//
// PerforceSubmitUI
//

PerforceSubmitUI::PerforceSubmitUI()
:m_isNotFoundLocal(false),
 m_isEmptyChange(false),
 m_isMustResolve(false)
{
}

PerforceSubmitUI::~PerforceSubmitUI()
{
}

void PerforceSubmitUI::Message(Error* error)
{
	m_isNotFoundLocal	= false;
	m_isEmptyChange		= false;
	m_isMustResolve		= false;

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    if (buf.Length() == 0)
    {
        // An empty message means nothing was submitted
        SetError(true);
        return;
    }
	static const regex::rpattern submittedPattern("Change ([0-9]+) submitted.");
	static const regex::rpattern submittedRenamePattern(".+ renamed change ([0-9]+) and submitted.");
	static const regex::rpattern resolvePattern1(".+ - must resolve (//.+)#[0-9]*");
	static const regex::rpattern resolvePattern2("(//.+) - must resolve #[0-9]*");
	static const regex::rpattern emptyChangePattern("No files to submit.");
	regex::match_results results;

	std::string str(buf.Text());

	if (submittedPattern.match(str, results).matched || 
		submittedRenamePattern.match(str, results).matched)
    {
		m_ulChangelistNumber = atol(results.backref(1).str().c_str());
        SetError(false);
		return;
    }
	else if(resolvePattern1.match(str, results).matched
			|| resolvePattern2.match(str, results).matched)
	{
		m_isMustResolve = true;
		m_mustResolveFiles.push_back(results.backref(1).str());
		SetError(true);
        return;
	}
	else if(emptyChangePattern.match(str, results).matched)
	{
		m_isEmptyChange = true;
		SetError(true);
        return;
	}
	else
	{
		SetError(true);
        return;
	}
}
void PerforceSubmitUI::HandleError(Error* err)
{
    // Process the message returned by the command
    StrBuf buf;
    err->Fmt(buf, EF_PLAIN);
	
	static const regex::rpattern notLocalPattern(".+: (.+): The system cannot find the file specified.");
	regex::match_results results;

	std::string str(buf.Text());
	
	if (notLocalPattern.match(str, results).matched)
	{
		m_isNotFoundLocal = true;
		m_notFoundLocalFiles.push_back(results.backref(1).str());
		SetError(true);
        return;
	}
	else
	{
		SetError(true);
        return;
	}
}

//
// PerforceChangesUI
//
PerforceChangesUI::PerforceChangesUI()
{
	m_changeNums.clear();
}

PerforceChangesUI::~PerforceChangesUI()
{
}

void PerforceChangesUI::Message(Error* error)
{
    SetError(error->IsError() != 0);

    if (IsError())
        return;

 // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

       if (buf.Length() == 0)
    {
        // An empty message means nothing was printed
        SetError(true);
        return;
    }
    static const regex::rpattern pattern("Change ([0-9]+) .+ '(.+) '$");
	regex::match_results results;
	
	if (pattern.match(std::string(buf.Text()), results).matched)
		m_changeNums.push_back(results.backref(1).str());
}

//
// PerforceReopenUI
//

PerforceReopenUI::PerforceReopenUI()
{
}

PerforceReopenUI::~PerforceReopenUI()
{
}

void PerforceReopenUI::Message(Error* error)
{
    SetError(error->IsError() != 0);

    if (IsError())
        return;

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    if (buf.Length() == 0)
    {
        // An empty message means nothing was printed
        SetError(true);
        return;
    }
    static const regex::rpattern reopenedPattern(".+ - reopened; change [0-9]+");
	static const regex::rpattern nochangePattern(".+ - nothing changed");
	static const regex::rpattern unknownPattern("Change [0-9]+ unknown.");
	regex::match_results results;
	std::string str(buf.Text());
	if (reopenedPattern.match(str, results).matched)
		return;

	else if (nochangePattern.match(str, results).matched)
		return;

	else if (unknownPattern.match(str, results).matched)
	{
		SetError(true);
		return;
	}
}

//
// PerforceWhereUI
//

PerforceWhereUI::PerforceWhereUI()
{
}

PerforceWhereUI::~PerforceWhereUI()
{
}

void PerforceWhereUI::Message(Error* error)
{
    SetError(error->IsError() != 0);
   
    if (IsError())
        return;

  // Process the message returned by the command
	StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    static const regex::rpattern pattern("(//.+) //.+ ([a-zA-Z]:.+)");
	regex::match_results results;
	
	if (pattern.match(std::string(buf.Text()), results).matched)
    {
        m_depotFile = results.backref(1).str();
        m_localPath = results.backref(2).str();
    }
    else
    {
        SetError(true);
    }
}

//
// PerforceDescribeUI
//

PerforceDescribeUI::PerforceDescribeUI(PerforceChangelistInfo* _pChangelistInfo/*= NULL*/):
m_pChangelistInfo(_pChangelistInfo)
{
}

PerforceDescribeUI::~PerforceDescribeUI()
{
}

void PerforceDescribeUI::OutputText(const_char* data, int /*length*/)
{
	m_strWholeText += data;
	
	// this is the output format we are getting from perforce
	// "Change 10249 by ycharbonneau@PC-YCHARBONNEAU-user007 on 2005/02/08 10:33:38"
	// "Change 10263 by ycharbonneau@PC-YCHARBONNEAU-user007 on 2005/02/09 16:49:02 *pending*"

	static const regex::rpattern patternPending("Change (.+) by (.+)@(.+) on ([0-9]*/[0-9]*/[0-9]*) .+ \\*(pending)\\*(.*)");
	static const regex::rpattern patternSubmitted("Change (.+) by (.+)@(.+) on ([0-9]*/[0-9]*/[0-9]* [0-9]*:[0-9]*:[0-9]*)(.*)");
	static const regex::rpattern descriptionFormatPattern("(\r|\n)\n(\t|\n)", " ", regex::GLOBAL);
	static const regex::rpattern descriptionFormatPattern2("\n\n", " ", regex::GLOBAL);

	regex::match_results results;
	regex::subst_results substResults;

	STL::string strData = data;

	descriptionFormatPattern.substitute(strData, substResults);
	descriptionFormatPattern2.substitute(strData, substResults);


	if (m_pChangelistInfo && patternPending.match(strData, results).matched)
	{
		m_pChangelistInfo->strChangelist = results.backref(1).str();
		m_pChangelistInfo->strUser = results.backref(2).str();
		m_pChangelistInfo->strClient = results.backref(3).str();
		m_pChangelistInfo->strDate = results.backref(4).str();
		m_pChangelistInfo->strStatus = results.backref(5).str();
		m_pChangelistInfo->strDescription = results.backref(6).str();
	}
	else if (m_pChangelistInfo && patternSubmitted.match(strData, results).matched)
	{
		m_pChangelistInfo->strChangelist = results.backref(1).str();
		m_pChangelistInfo->strUser = results.backref(2).str();
		m_pChangelistInfo->strClient = results.backref(3).str();
		m_pChangelistInfo->strDate = results.backref(4).str();
		m_pChangelistInfo->strStatus = "submitted";
		m_pChangelistInfo->strDescription = results.backref(5).str();
	}


	// STL::string strData = data;
	descriptionFormatPattern.substitute(strData, substResults);
	descriptionFormatPattern2.substitute(strData, substResults);
  
	static const regex::rpattern pattern(".+ \\*pending\\* (.+)($|Affected)",regex::MULTILINE);
	static const regex::rpattern pattern2(".+ [0-9]*:[0-9]*:[0-9]* (.+)($|Affected)",regex::MULTILINE);

	if (pattern.match(strData, results).matched || 
		pattern2.match(strData, results).matched)
    {
		m_description = &(*results.backref(1).reserved1);
		if ( m_pChangelistInfo ) 
		{
			m_pChangelistInfo->strDescription = m_description;
		}
	}
}

void PerforceDescribeUI::Message(Error* e)
{
	StrBuf buf;
	e->Fmt(buf, EF_PLAIN);	

	m_strWholeText += buf.Text( );
	m_strWholeText += "\n";

	// Do we want to get files contained in ChangeList ?
	if ( m_pChangelistInfo )
	{
		static const regex::rpattern patternfiles("(.+)#([0-9]*) (add|edit|delete|integrate)");
		regex::match_results results;

		if (patternfiles.match(std::string(buf.Value()), results).matched)
		{
			PerforceFileInfo* pFileInfo = new PerforceFileInfo; 
			pFileInfo->strFilename = results.backref(1).str();
			pFileInfo->strFileRevision = results.backref(2).str();
			pFileInfo->strCommand = results.backref(3).str();
			m_pChangelistInfo->vFileInfo.push_back(pFileInfo);
		}
	}
}

//
// PerforceOpenedUI
//

PerforceOpenedUI::PerforceOpenedUI()
{
}

PerforceOpenedUI::~PerforceOpenedUI()
{
}

void PerforceOpenedUI::Message(Error* err)
{
	SetError(err->IsError() != 0);

    if (IsError())
        return;

	StrBuf buf;
    err->Fmt(buf, EF_PLAIN);
	
	static const regex::rpattern emptyPattern("File\\(s\\) not opened on this client");
	static const regex::rpattern openedfilesPattern("(.+)#[0-9]+ - (.+) (default|change) ");
    regex::match_results results;
	std::string str(buf.Text());
    if (emptyPattern.match(str, results).matched)
    {
        return;
    }
	else if (openedfilesPattern.match(str, results).matched)
    {
		OpenedFiles openedFiles;

		openedFiles.m_FilePath = results.backref(1).str();
		openedFiles.m_FileAction = results.backref(2).str();

		m_openedFiles.push_back(openedFiles);
	}
}

//
// PerforceFilelogUI
//

PerforceFilelogUI::PerforceFilelogUI()
	: m_bError( false ),
	  m_bIsLine ( false )
{
}

PerforceFilelogUI::~PerforceFilelogUI()
{
}

void PerforceFilelogUI::Message(Error* err)
{
	m_bError = false;
	m_bIsLine = false;

	SetError(err->IsError() != 0);

	if (IsError())
		return;

	std::string strLine;

	StrBuf buf;
	err->Fmt(buf, EF_PLAIN);

	strLine = buf.Text( );

	// substitute special characters
	regex::subst_results results;

	const regex::rpattern changePatternN("\n", "<!-N-!>");
	const regex::rpattern changePatternR("\r", "<!-R-!>");
	const regex::rpattern changePatternT("\t", "<!-T-!>");

	while( changePatternN.substitute(strLine, results) );
	while( changePatternR.substitute(strLine, results) );
	while( changePatternT.substitute(strLine, results) );

	// match
	static const regex::rpattern nosuchfilePattern( ".*no such file\\(s\\).*$" );
	static const regex::rpattern filenamePattern( "^\\/\\/.*" );
	static const regex::rpattern revisionPattern( "^#([0-9]*) change ([0-9]*) (.*) on (.+) by (.*) \\((.*)\\)(<!-N-!><!-N-!><!-T-!>|.*')(.*)($|'$)" );

	if( nosuchfilePattern.match( strLine, results ).matched )
	{
		m_bError = true;
	}
	else if( filenamePattern.match( strLine, results ).matched )
	{
		m_strFilename = strLine.c_str( );
	}
	else if( revisionPattern.match( strLine, results ).matched )
	{
		m_bIsLine = true;

		m_strRevision    = results.backref(1).str( );
		m_strChangelist  = results.backref(2).str( );
		m_strAction      = results.backref(3).str( );
		m_strDate        = results.backref(4).str( );
		m_strUser        = results.backref(5).str( );
		m_strFileType    = results.backref(6).str( );
		m_strDescription = results.backref(8).str( );

		const regex::rpattern revertPatternN("<!-N-!>", "\n");
		const regex::rpattern revertPatternR("<!-R-!>", "\r");
		const regex::rpattern revertPatternT("<!-T-!>", "\t");

		while( revertPatternN.substitute(m_strDescription, results) );
		while( revertPatternR.substitute(m_strDescription, results) );
		while( revertPatternT.substitute(m_strDescription, results) );
	}
}

//
// PerforcePrintUI
//

PerforcePrintUI::PerforcePrintUI()
{
}

PerforcePrintUI::~PerforcePrintUI()
{
}

void PerforcePrintUI::Message(Error* err)
{
	SetError(err->IsError() != 0);
}

//
// PerforceDiffUI
//

PerforceDiffUI::PerforceDiffUI()
: m_isSame(true),
  m_isNotOpened(false)
{
}

PerforceDiffUI::~PerforceDiffUI()
{
}

void PerforceDiffUI::Message(Error* error)
{
    SetError(error->IsError() != 0);

    if (IsError())
        return;

    // Process the message returned by the command
    StrBuf buf;
    error->Fmt(buf, EF_PLAIN);

    if (buf.Length() == 0)
    {
		m_isSame = true;
        return;
    }

    bool notOpened = STL::string(buf.Text()).find(" - file(s) not opened on this client") != STL::string::npos;

    regex::match_results results;

    if (notOpened) 
	{
        // The files were not opened, thus could not be reverted
        SetError(true);
        m_isNotOpened = true;
    }
	else
		m_isSame = false;
}
