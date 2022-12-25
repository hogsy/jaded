/*$T DIAcompletion_dlg.h GC!1.41 09/09/99 15:27:30 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

#define AI_C_Completion_Variable	1
#define AI_C_Completion_ProcList	2
#define AI_C_Completion_Macro		3
#define AI_C_Completion_All			4


/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EAI_cl_Frame;
class CCompListBox : public CListBox
{
public:
    EAI_cl_Frame    *mpo_Frame;
	void MeasureItem(MEASUREITEMSTRUCT *lp)
	{
		lp->itemHeight = 18;
	}
	void DrawItem(DRAWITEMSTRUCT *);
};

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

//////////////////////////////////////////////////////////////////////////
_inline_ int CompareAscending(const void *a, const void *b)
{
  CString *pA = (CString*)a;
  CString *pB = (CString*)b;
  return (pA->CompareNoCase(*pB));
}

//////////////////////////////////////////////////////////////////////////
_inline_ int CompareDescending(const void *a, const void *b)
{
  CString *pA = (CString*)a;
  CString *pB = (CString*)b;
  return (-1 * (pA->CompareNoCase(*pB)));
}

//////////////////////////////////////////////////////////////////////////
_inline_ void SortStringArray (CStringArray& csa, BOOL bDescending)
{
  int iArraySize = csa.GetSize();
  if (iArraySize <= 0)
     return;

  int iCSSize = sizeof (CString*);
  void* pArrayStart = (void *)&csa[0];

  if (bDescending)
     qsort (pArrayStart, iArraySize, iCSSize, CompareDescending);
  else
     qsort (pArrayStart, iArraySize, iCSSize, CompareAscending);
}

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class EDIA_cl_Completion : public EDIA_cl_BaseDialog
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_Completion (int what);
    ~EDIA_cl_Completion (void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EAI_cl_Frame    *mpo_Frame;
    CString         mo_Word;
	CCompListBox	*mpo_List;
	int				mi_What;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    int		AddList(CMapStringToPtr *);

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    DoDataExchange(CDataExchange *);
    void    OnOK(void);
    void    OnCancel(void);
	BOOL	OnInitDialog(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

protected:
    afx_msg void    OnSize(UINT, int, int);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
