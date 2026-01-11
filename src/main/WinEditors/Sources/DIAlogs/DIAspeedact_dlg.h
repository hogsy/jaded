/*$T DIAspeedact_dlg.h GC! 1.100 08/21/01 15:51:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

typedef struct
{
	int i_Count;
	int i_Size;
	UINT i_MaxSize;
	UINT i_MinSize;
	BIG_INDEX ul_MaxSize;
	BIG_INDEX ul_MinSize;
} DIAstats;

class EDIA_cl_SpeedAct : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_SpeedAct(void);
	void	Stats(BIG_INDEX, char *);
	void	Trace(BIG_INDEX);
	void	Delete(BIG_INDEX);
	BOOL	OnRec(CListBox *, BIG_INDEX, int);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	static char					maz_Path[BIG_C_MaxLenPath];
	static BOOL					mb_NotLoaded;
	static BOOL					mb_Loaded;
	CMapStringToPtr				mo_StatsRefs;
	CList<DIAstats, DIAstats>	mo_Stats;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			OnInitDialog(void);

	afx_msg void	OnDelete(void);
	afx_msg void	OnStats(void);
	afx_msg void	OnLog(void);
	afx_msg void	OnAll(void);
	afx_msg void	OnNone(void);
	afx_msg void	OnInvert(void);
	afx_msg void	OnPath(void);
	afx_msg void	OnClose(void);
	afx_msg void	OnExit(void);
	afx_msg void	OnNotLoaded(void);
	afx_msg void	OnLoaded(void);

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
