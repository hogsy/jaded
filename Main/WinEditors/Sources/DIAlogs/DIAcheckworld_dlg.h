/*$T DIAcheckworld_dlg.h GC! 1.081 05/29/01 10:33:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "F3Dframe/F3Dview.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
#define EDIACW_Textures     1
#define EDIACW_Materiaux    2
#define EDIACW_Graphics     3

#define EDIAC_FILTERRAW     1
#define EDIAC_FILTERTEX     2
#define EDIAC_FILTERTGA     4
#define EDIAC_FILTERALL     7

#define EDIAC_FLAGSTEXREALLYUSED            1
#define EDIAC_FLAGSTEXSHOWINEDITOR          2
#define EDIAC_FLAGSTEXSHOWUNUSEDINEDITOR    4
#define EDIAC_FLAGSTEXSHOW                  6

class EOUT_cl_Frame;


class EDIA_cl_CheckWorld: public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_CheckWorld(struct WOR_tdst_World_ *);
#ifdef JADEFUSION
	~EDIA_cl_CheckWorld(void);
#endif
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	struct WOR_tdst_World_	*mpst_World;
#ifdef JADEFUSION
	F3D_cl_View				*mpo_View;
#endif
    int                     mi_TreeContent;
    int                     mi_IconGro;
    int                     mi_IconGrm;
    int                     mi_IconTex;
    int                     mi_IconGao;
    ULONG                   mul_Index;
    int                     mi_TexFilter;
    int                     mi_TexFlags;
    

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
	void	OnDestroy(void);
    BOOL    PreTranslateMessage( MSG * );

    void    FillTree_Graphics( void );
    void    FillTree_Materiaux( void );
    void    FillTree_Textures( void );
    void    Expand_Graphics( HTREEITEM );
    void    Expand_MultiMat( HTREEITEM );
    void    Expand_Textures( HTREEITEM );
    void    ExpandRecurse( CTreeCtrl *, HTREEITEM );
    void	SelectGaoRecurse( CTreeCtrl *, HTREEITEM, EOUT_cl_Frame *);
    void    Menu_Texture( void );
#ifdef JADEFUSION
	BOOL	b_Change_Selection(	ULONG );
	BOOL	FindItem(DWORD, HTREEITEM &, HTREEITEM = NULL);

private:
	BOOL	b_IsLoaded(BIG_KEY);
#endif
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	afx_msg void    OnGetMinMaxInfo(MINMAXINFO FAR *);
	
	afx_msg void	OnSize(UINT, int, int);
    afx_msg void    OnBtWorld( void );
    afx_msg void    OnBtMem( void );
    afx_msg void    OnBtList( void );
    afx_msg void    OnBtGo( void );
#ifdef JADEFUSION
	afx_msg void	OnBt3DView( void );
#endif
	afx_msg void    OnRadio( void );

    afx_msg void    OnExpanded(NMHDR *, LRESULT *);
    afx_msg void    OnSelect(NMHDR *, LRESULT *);
#ifdef JADEFUSION
	afx_msg void	OnDblClk(NMHDR *, LRESULT *);
#endif
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
