/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#ifdef ACTIVE_EDITORS


#include "DIAlogs/DIAbase.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class EDIA_cl_BroFilterDialog : public EDIA_cl_BaseDialog
{
public:
    CString mo_FileName;
    ULONG mul_Index;
    BOOL    mb_Modified;

public:
    EDIA_cl_BroFilterDialog(char*);
    ~EDIA_cl_BroFilterDialog(void);



public:
    void RedrawIdList(unsigned int uiID, BAS_tdst_barray*pst_Array);
    void RedrawFileList(unsigned int uiID, BAS_tdst_barray*pst_Array);
    void RedrawDirList(unsigned int uiID, CList<CString, CString>*pst);
    void SaveFilter(void);
    
/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    BOOL	OnInitDialog(void);
    void    DoDataExchange(CDataExchange *);
	BOOL	PreTranslateMessage(MSG *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnOk(void);

    afx_msg void    OnAddExId(void);
    afx_msg void    OnDelExId(void);
    afx_msg void    OnAddExFile(void);
    afx_msg void    OnDelExFile(void);
    afx_msg void    OnAddExDir(void);
    afx_msg void    OnDelExDir(void);

    afx_msg void    OnMirror(void);
    afx_msg void    OnTestDate(void);

    afx_msg void    OnAddFoId(void);
    afx_msg void    OnDelFoId(void);
    afx_msg void    OnAddFoFile(void);
    afx_msg void    OnDelFoFile(void);
    afx_msg void    OnAddFoDir(void);
    afx_msg void    OnDelFoDir(void);
    DECLARE_MESSAGE_MAP()
};
#endif  /* ACTIVE_EDITORS */
