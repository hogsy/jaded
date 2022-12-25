// (C) Copyright 2004 Ubi Soft Entertainment Inc.
//
// $File: //technology-group/latest_version/tools/sourcecontrol/private/src/perforce/perforceui.h $
// $Revision: #8 $
// $Change: 80731 $
// $DateTime: 2004/06/04 11:33:53 $

#ifndef UBI_PERFORCE_PERFORCEUI_H__INCLUDED
#define UBI_PERFORCE_PERFORCEUI_H__INCLUDED

#include "p4/clientapi.h"
#include "Greta/regexpr2.h"

using namespace std;
#include <string>
#define STL std

/// Base class for our user interface to the Perforce commands.
/// Each command should have its own user interface.
///
/// PerforceUI subclasses are meant to be thin layers over the Perforce output; their
/// goal is to make the output more easily accessible in C++, but without doing too much 
/// interpretation of that output.
class PerforceUI : public ClientUser
{
public:

    /// @name Construction/destruction
    //@{

    PerforceUI();
    virtual ~PerforceUI();

    //@}
    /// @name Accessors
    //@{

    /// Indicates if the last operation failed.
    bool IsError() const { return m_isError; }

    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    /// For this class, these methods simply assert. Each command should override
    /// the methods it needs. If some are forgotten, the assertions will remind us!
    //@{

	virtual void InputData(StrBuf* strbuf, Error* e);

	virtual void HandleError(Error* err);

    /// @note When a command outputs multiple lines, Message() is called by Perforce 
    /// once for each line.
    virtual void Message(Error* err);

    virtual void OutputError(const_char* errBuf);
	virtual void OutputInfo(char level, const_char* data);
	virtual void OutputBinary(const_char* data, int length);
	virtual void OutputText(const_char* data, int length);

	virtual void OutputStat(StrDict* varList);
	virtual void	Prompt( const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e );

//dl:todo: other methods may need to be overridden

    //@}

protected:

    void SetError(bool isError) { m_isError = isError; }

private:

    bool m_isError;
};

/// User interface for the "p4 change" command
class PerforceChangeUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    /// Constructs with the input arguments for the command.
    PerforceChangeUI(
		const char* change,
        const char* client,
        const char* user,
        const char* description,
		const char* _Files);

    virtual ~PerforceChangeUI();

    /// Returns the changelist number. This is valid only after running a command.
    /// @warning This should not be called if the command generated an error!
    STL::string GetChangeNum() const;

    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    //@{

	virtual void InputData(StrBuf* strbuf, Error* error);
	virtual void Message(Error* error);

    //@}

private:

    void FillForm(
		const char* change,
        const char* client,
        const char* user,
        const char* description,
		const char* _Files);

private:

    STL::string m_filledForm;
    STL::string m_changeNum;
};


class PerforceMonitorUI : public PerforceUI
{
public:

	/// @name Construction/destruction
	//@{

	/// Constructs with the input arguments for the command.
	PerforceMonitorUI();

	virtual ~PerforceMonitorUI();

	virtual void Message(Error* error);

	int GetProcessCount() { return m_iProcessCount ; }

	//@}

private:
	int m_iProcessCount;

};

/// User interface for the "p4 change" command
class PerforceClientUI : public PerforceUI
{
public:

	/// @name Construction/destruction
	//@{

	/// Constructs with the input arguments for the command.
	PerforceClientUI(	const char* client,
						const char* host,
						const char* owner,
						const char* root,
						const char* description,
						const char* view);

	virtual ~PerforceClientUI();

	//@}
	/// @name Overridden methods
	/// These are called by Perforce only, much like callbacks.
	//@{

	virtual void InputData(StrBuf* strbuf, Error* error);
	virtual void Message(Error* error);
	virtual void OutputInfo(char /*level*/, const_char* data);
	std::string GetDescription(){return m_description;}


private:

	void FillForm(	const char* client,
					const char* host,
					const char* owner,
					const char* root,
					const char* description,
					const char* view);

private:

	STL::string m_filledForm;
	STL::string m_description;
};

/// User interface for the "p4 change -d" command
/// @note Because "p4 change -d" does not need any input/output form, this class
///       is more appropriate than PerforceChangeUI.
class PerforceChangeDeleteUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    /// Constructs with the input arguments for the command.
    PerforceChangeDeleteUI();

    virtual ~PerforceChangeDeleteUI();

    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    //@{

	virtual void Message(Error* error);

    //@}
};

class PerforceDepotsUI : public PerforceUI
{
public:

	/// @name Construction/destruction
	//@{

	/// Constructs with the input arguments for the command.
	PerforceDepotsUI(std::vector<std::string>& _vDepot) :
	m_vDepot(_vDepot)
	{}

	virtual ~PerforceDepotsUI(){}

	//@}
	/// @name Overridden methods
	/// These are called by Perforce only, much like callbacks.
	//@{

	virtual void Message(Error* error)
	{

		StrBuf buf;
		error->Fmt(buf, EF_PLAIN);
		
		static const regex::rpattern pattern("Depot (.+) [0-9].+");
		regex::match_results results;
		regex::match_results::backref_type ref;
		std::string str = buf.Text();
		ref = pattern.match(str, results);

		if (ref.matched) 
		{
			m_vDepot.push_back(results.backref(1).str());
		}
	}

private: 
	std::vector<std::string>& m_vDepot;
	//@}
};

/// User interface for the "p4 fstat" command
class PerforceFstatUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceFstatUI();
    virtual ~PerforceFstatUI();

    //@}
    /// @name Accessors
    //@{

    STL::string GetClientFile(int i) const { return m_results[i].m_clientFile; }
    STL::string GetDepotFile(int i) const { return m_results[i].m_depotFile; }
    STL::string GetHeadAction(int i) const { return m_results[i].m_headAction; }
    STL::string GetAction(int i) const { return m_results[i].m_action; }
    STL::string GetResolveBase(int i) const { return m_results[i].m_resolveBase; }
    STL::string GetResolveBaseRev(int i) const { return m_results[i].m_resolveBaseRev; }
    STL::string GetResolveFrom(int i) const { return m_results[i].m_resolveFrom; }
    STL::string GetResolveFromRev(int i) const { return m_results[i].m_resolveFromRev; }
    bool IsAccessDenied() const { return m_isAccessDenied; }
    int GetNumResults() const { return m_results.size(); }

    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    //@{

	virtual void Message(Error* error);
	virtual void OutputStat(StrDict* varList);

    //@}

private:

	struct Result
	{
    STL::string m_depotFile;
    STL::string m_clientFile;
    STL::string m_headAction;
    STL::string m_action;
	STL::string m_attribute;
	STL::string m_resolveBase;
	STL::string m_resolveBaseRev;
	STL::string m_resolveFrom;
	STL::string m_resolveFromRev;
	};

	STL::vector<Result> m_results;
    bool m_isAccessDenied;
};


/// User interface for the "p4 dirs" command
class PerforceDirsUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceDirsUI();
    virtual ~PerforceDirsUI();

    //@}
    /// @name Accessors
    //@{

    const STL::list<STL::string>& GetDirs() const { return m_dirs; }

    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    //@{

	virtual void Message(Error* error);

    //@}

private:

    STL::list<STL::string> m_dirs;
};

/// User interface for the "p4 files" command
class PerforceFilesUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceFilesUI();
    virtual ~PerforceFilesUI();

    //@}
    /// @name Accessors
    //@{

    const STL::list<STL::string>& GetFiles() const { return m_files; }

    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    //@{

	virtual void Message(Error* error);

    //@}

private:

    STL::list<STL::string> m_files;
};


/// User interface for the "p4 users" command
class PerforceUsersUI : public PerforceUI
{
public:

	/// @name Construction/destruction
	//@{

	PerforceUsersUI();
	virtual ~PerforceUsersUI();

	//@}
	/// @name Accessors
	//@{

	const STL::list<STL::string>& GetUsers() const { return m_lUsers; }

	//@}
	/// @name Overridden methods
	/// These are called by Perforce only, much like callbacks.
	//@{

	virtual void Message(Error* error);

	//@}

private:

	STL::list<STL::string> m_lUsers;
};

/// User interface for the "p4 clients" command
class PerforceClientsUI : public PerforceUI
{
public:

	/// @name Construction/destruction
	//@{

	PerforceClientsUI();
	virtual ~PerforceClientsUI();

	//@}
	/// @name Accessors
	//@{

	const STL::list<STL::string>& GetClients() const { return m_lClient; }

	//@}
	/// @name Overridden methods
	/// These are called by Perforce only, much like callbacks.
	//@{

	virtual void Message(Error* error);

	//@}

private:

	STL::list<STL::string> m_lClient;
};

/// User interface for the "p4 sync" command
class PerforceSyncUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceSyncUI();
    virtual ~PerforceSyncUI();

    //@}
    /// @name Accessors
    //@{

    /// Indicates whether or not the files to be synchronized were already up-to-date.
    bool IsUpToDate() const { return m_isUpToDate; }
	bool IsNotOnServer() const { return m_isNotOnServer; }
	bool IsSync() const { return m_IsSync; }
	bool IsDeleted() const { return m_IsDeleted; }

    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    //@{

	virtual void Message(Error* error);

    //@}

private:

    bool m_isUpToDate;
	bool m_isNotOnServer;
	bool m_IsSync;
	bool m_IsDeleted;
};


/// User interface for the "p4 info" command
class PerforceInfoUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceInfoUI();
    virtual ~PerforceInfoUI();

    //@}
    /// @name Accessors
    //@{

    STL::string GetUserName() const { return m_userName; }
    STL::string GetClientName() const { return m_clientName; }
    STL::string GetClientRoot() const { return m_clientRoot; }

    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    //@{

	virtual void Message(Error* error);

    //@}

private:

    STL::string m_userName;
    STL::string m_clientName;
    STL::string m_clientRoot;
};

/// User interface for the "p4 add" command
class PerforceAddUI : public PerforceUI
{
public:

    /// @name Construction/destruction
	//@{

    PerforceAddUI();
    virtual ~PerforceAddUI();

	//@}
    /// @name Accessors
    //@{

	bool IsOpenedAdd() const { return m_isOpenedAdd; }
    bool IsCurrentlyOpened() const { return m_isCurrentlyOpened; }
	bool IsExistingFile() const { return m_isExistingFile; }

	//@}
    /// @name Overridden methods
	//@{

	virtual void Message(Error* error);
	virtual void OutputInfo(char level, const_char* data);
	
	//@}

private:

	bool m_isOpenedAdd;
    bool m_isCurrentlyOpened;
	bool m_isExistingFile;
};

/// User interface for the "p4 edit" command
class PerforceEditUI : public PerforceUI
{
public:

    /// @name Construction/destruction
	//@{

    PerforceEditUI();
    virtual ~PerforceEditUI();

	//@}
    /// @name Accessors
    //@{

	bool IsOpened() const { return m_isOpened; }
    bool IsCurrentlyOpened() const { return m_isCurrentlyOpened; }
    bool IsOpenedAtOlderRevision() const { return m_isOlderRevision && m_isOpened; }
	bool IsOpenedExclusively() const { return m_isOpenedExclusively; }
	bool IsUnknown() const { return m_isUnknown; }

    //@}
    /// @name Overridden methods
	//@{

	virtual void Message(Error* error);
	virtual void OutputInfo(char level, const_char* data);
	
	//@}

private:

	bool m_isOpened;
    bool m_isOlderRevision;
	bool m_isCurrentlyOpened;
	bool m_isOpenedExclusively;
	bool m_isUnknown;
};

/// User interface for the "p4 delete" command
class PerforceDeleteUI : public PerforceUI
{
public:

    /// @name Construction/destruction
	//@{

    PerforceDeleteUI();
    virtual ~PerforceDeleteUI();

	//@}
    /// @name Accessors
    //@{

	bool IsOpenedDelete() const { return m_isOpenedDelete; }
    bool IsAlreadyDeleted() const { return m_isAlreadyDeleted; }
	bool IsAlreadyOpened() const { return m_isAlreadyOpened; }
	bool IsUnknown() const { return m_isUnknown; }

	//@}
    /// @name Overridden methods
	//@{

	virtual void Message(Error* error);
	void HandleError(Error* err);

	//@}

private:

	bool m_isOpenedDelete;
	bool m_isAlreadyDeleted;
	bool m_isAlreadyOpened;
	bool m_isUnknown;
};

/// User interface for the "p4 revert" command
class PerforceRevertUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceRevertUI();
    virtual ~PerforceRevertUI();

    //@}
    /// @name Accessors
    //@{

    bool IsReverted() const { return m_isReverted; }
    bool IsNotOpened() const { return m_isNotOpened; }

    //@}
    /// @name Overridden methods
    //@{

	virtual void Message(Error* error);
	virtual void OutputInfo(char level, const_char* data);

    //@}

private:

	bool m_isReverted;
    bool m_isNotOpened; // Note: This kind of negative bool is ugly, but it reflects Perforce's corresponding flag
};

/// User interface for the "p4 submit" command
class PerforceSubmitUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceSubmitUI();
    virtual ~PerforceSubmitUI();

    //@}
    /// @name Accessors
    //@{

    bool IsNotFoundLocal() const { return m_isNotFoundLocal; }
    bool IsEmptyChange() const { return m_isEmptyChange; }
	bool IsMustResolve() const { return m_isMustResolve; }
	ULONG GetChangelistNumber() const { return m_ulChangelistNumber ; }
	const STL::list<STL::string>& GetNotFoundLocalFiles() const { return m_notFoundLocalFiles; }
	const STL::list<STL::string>& GetMustResolveFiles() const { return m_mustResolveFiles; }

    //@}
    /// @name Overridden methods
    //@{

	virtual void Message(Error* error);
	virtual void HandleError(Error* error);
	    
	//@}
private:

	STL::list<STL::string> m_notFoundLocalFiles;
	STL::list<STL::string> m_mustResolveFiles;
	ULONG m_ulChangelistNumber;
	bool m_isMustResolve;
	bool m_isNotFoundLocal;
	bool m_isEmptyChange;
};

/// User interface for the "p4 changes" command
class PerforceChangesUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

	PerforceChangesUI();
    virtual ~PerforceChangesUI();

	//@}
    /// @name Accessors
    //@{

	const STL::list<STL::string>& GetChangeNums() const { return m_changeNums; }

    //@}
    /// @name Overridden methods
    //@{

	virtual void Message(Error* error);

	//@}

private:

	STL::list<STL::string> m_changeNums;
};

/// User interface for the "p4 reopen" command
class PerforceReopenUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceReopenUI();
    virtual ~PerforceReopenUI();
	
    //@}
    /// @name Overridden methods
    //@{

	virtual void Message(Error* error);

	//@}
};

/// User interface for the "p4 where" command
/// @note This class is not meant to process multiple-line output from the p4 where command,
///       thus the caller must avoid wildcards such as '...' and '*'. When multiple lines are
///       output, only the last line is taken into account.
class PerforceWhereUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceWhereUI();
    virtual ~PerforceWhereUI();
	
    //@}
    /// @name Accessors
    //@{

    STL::string GetDepotFile() const { return m_depotFile; }
    STL::string GetLocalPath() const { return m_localPath; }

    //@}
    /// @name Overridden methods
    //@{

	virtual void Message(Error* error);

	//@}

private:

    STL::string m_depotFile;
    STL::string m_localPath;
};


class PerforceFileInfo
{
public:
	std::string strFilename;
	std::string strFileRevision;
	std::string strCommand;
};

struct PerforceChangelistInfo
{
	std::string strChangelist;
	std::string strDate;
	std::string strUser;
	std::string strClient;
	std::string strStatus;
	std::string strDescription;
	std::vector<PerforceFileInfo*> vFileInfo;
};

/// User interface for the "p4 describe" command
class PerforceDescribeUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

	

	PerforceDescribeUI(PerforceChangelistInfo* _pChangelistInfo = NULL);
    virtual ~PerforceDescribeUI();
	
    //@}
    /// @name Accessors
    //@{

    STL::string GetDescription() const {return m_description; }
    STL::string GetIntegralText() const {return m_strWholeText; }

    //@}
    /// @name Overridden methods
    //@{

	virtual void OutputText(const_char* data, int length);
	virtual void Message(Error* error);
	//@}

private:

    STL::string m_description;
    STL::string m_strWholeText;
	PerforceChangelistInfo* m_pChangelistInfo;
};

/// User interface for the "p4 opened" command
class PerforceOpenedUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceOpenedUI();
    virtual ~PerforceOpenedUI();

	//@}
    /// @name Accessors
    //@{

    STL::string GetFilePath(int i) const { return m_openedFiles[i].m_FilePath; }
    STL::string GetFileAction(int i) const { return m_openedFiles[i].m_FileAction; }
	int GetNumFiles() const { return m_openedFiles.size(); }

    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    //@{

	virtual void Message(Error* error);

    //@}

private:

	struct OpenedFiles
	{
    STL::string m_FilePath;
    STL::string m_FileAction;
	};

	STL::vector<OpenedFiles> m_openedFiles;
};

/// User interface for the "p4 filelog" command
class PerforceFilelogUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceFilelogUI();
    virtual ~PerforceFilelogUI();

	//@}
    /// @name Accessors
    //@{


	bool				IsError( ) const		{ return m_bError; }
	bool				IsLine( ) const         { return m_bIsLine; }
	const STL::string&	GetFilename( ) const    { return m_strFilename; }
	const STL::string&	GetChangelist( ) const  { return m_strChangelist; }
	const STL::string&	GetRevision( ) const    { return m_strRevision; }
	const STL::string&	GetAction( ) const      { return m_strAction; }
	const STL::string&	GetDate( ) const        { return m_strDate; }
	const STL::string&	GetDescription( ) const { return m_strDescription; }
	const STL::string&	GetFileType( ) const    { return m_strFileType; }
	const STL::string&	GetUser( ) const        { return m_strUser; }

    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    //@{

	virtual void Message(Error* error);

    //@}

private:
	bool						m_bError;

	STL::string					m_strFilename;

	bool						m_bIsLine;
	STL::string					m_strChangelist;
	STL::string					m_strRevision;
	STL::string					m_strAction;
	STL::string					m_strDate;
	STL::string					m_strDescription;
	STL::string					m_strUser;
	STL::string					m_strFileType;
};

/// User interface for the "p4 print" command
class PerforcePrintUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforcePrintUI();
    virtual ~PerforcePrintUI();

	//@}
    /// @name Accessors
    //@{


    //@}
    /// @name Overridden methods
    /// These are called by Perforce only, much like callbacks.
    //@{

	virtual void Message(Error* error);

    //@}
};

/// User interface for the "p4 diff" command
class PerforceDiffUI : public PerforceUI
{
public:

    /// @name Construction/destruction
    //@{

    PerforceDiffUI();
    virtual ~PerforceDiffUI();

    //@}
    /// @name Accessors
    //@{

    bool IsSame() const { return m_isSame; }
    bool IsNotOpened() const { return m_isNotOpened; }

    //@}
    /// @name Overridden methods
    //@{

	virtual void Message(Error* error);

	//@}

protected:

	bool m_isSame;
    bool m_isNotOpened; // Note: This kind of negative bool is ugly, but it reflects Perforce's corresponding flag
};

#endif
