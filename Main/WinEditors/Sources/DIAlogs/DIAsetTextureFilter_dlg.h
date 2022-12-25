
#ifndef UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIASETTEXTUREFILTER_DLG_H__INCLUDED
#define UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIASETTEXTUREFILTER_DLG_H__INCLUDED

#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
//#include "DIAlogs/DIAComboBoxCompletion.h"
#include "GEOmetric/GEOobject.h"

//#include "BASe/BAStypes.h"

/*class EDIA_cl_SetTextureFilterDialog : public EDIA_cl_BaseDialog
{

//
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
//

public:
	EDIA_cl_SetTextureFilterDialog (char *, int _i_MaxLen = 100, BOOL _b_Pass = FALSE);

//
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
//

public:
	CString mo_Name;
	CString mo_Title;
	int		mi_MaxLen;
	BOOL	mb_Pass;

//
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
//

public:
	void	DoDataExchange(CDataExchange *);
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
static char     TEX_FilterID[8][32] =
{
    "Marmotte",
    "Poularde",
    "Cochon de sable",
    "Truite de mer",
	"Baleine asmathique",
	"Rat de désert",
	"Papi fait de la résistance",
	""
};
#define TEX_NbFilterID 9

class EDIA_cl_SetTextureFilterDialogCombo : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_SetTextureFilterDialogCombo (char *, int _i_MaxLen = 100);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	CString					mo_Name;
	CString					mo_Title;
	int						mi_MaxLen;
	//CList<char *, char *>	mo_List;
	//CList<int, int>			mo_ListDatas;
	int						mi_CurSelData;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	//void	AddItem(char *, int = 0);
	void	SetDefault(char *);
	void	DoDataExchange(CDataExchange *);
	BOOL	PreTranslateMessage(MSG *);
	void	FillListFilter(void);
	BOOL	OnInitDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */
	//afx_msg void	OnRadio_NewFilter( void );
	afx_msg void	OnBnClickedOk();
	afx_msg void	OnMenuFilter(void);

	DECLARE_MESSAGE_MAP()

};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*class EDIA_cl_NameRLIDialogCombo : public EDIA_cl_BaseDialog
{

//
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 //

public:
	EDIA_cl_NameRLIDialogCombo (char *,
	                            OBJ_tdst_GameObject	**_mo_ListGAO,
	                            GEO_tdst_Object	**mo_ListGRO,
								int _i_MaxLen = 100);

//
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------


public:
	CString					mo_Name;
	CString					mo_Title;
	int						mi_MaxLen;
	CList<char *, char *>	mo_List;
	CList<int, int>			mo_ListDatas;
	int						mi_CurSelData;
	
	BOOL					WorkWithGRORLI;
	OBJ_tdst_GameObject		**mo_ListGAO;
	GEO_tdst_Object			**mo_ListGRO;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------


public:
	void	AddItem(int);
	void	SetDefault(char *);
	void	DoDataExchange(CDataExchange *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 
	afx_msg void	OnSelChange( void );
	afx_msg void	OnRadio_GAO( void );
	afx_msg void	OnRadio_GRO( void );

	DECLARE_MESSAGE_MAP()
};
*/
/*$4
***********************************************************************************************************************
***********************************************************************************************************************
*/

// -- Forward declaration --
//class F3D_cl_View;
//typedef struct OBJ_tdst_GameObject_ OBJ_tdst_GameObject;
// -- Forward declaration --

/*class EDIA_cl_SetTextureFilterDialogDuplicate : public EDIA_cl_BaseDialog
{

	//
	-----------------------------------------------------------------------------------------------------------------------
	CONSTRUCT.
	-----------------------------------------------------------------------------------------------------------------------
	//

public:
	EDIA_cl_SetTextureFilterDialogDuplicate (F3D_cl_View *po_View, OBJ_tdst_GameObject *po_Gao);

	//
	-----------------------------------------------------------------------------------------------------------------------
	ATTRIBUTES.
	-----------------------------------------------------------------------------------------------------------------------
	//
public:
	int			m_iTypeOfDuplication;
	CString		m_strGaoName;

private:
	CTreeCtrl	*m_pTreeCtl;
	CButton		*m_OKButton ;
	CComboBoxCompletion m_cbCompletion ;
	BOOL		m_bDlgIsInitialized;
	CString		m_strInitialGaoName;

	OBJ_tdst_GameObject *mpo_Gao ;
	F3D_cl_View			*mpo_View ;

	int         mi_TreeContent;
	int         mi_IconGro;
	int         mi_IconGrm;
	int         mi_IconTex;
	int         mi_IconGao;
	int         mi_IconGrl;
	int         mi_IconMap;
	int         mi_IconCob;
	int         mi_IconGam;
	int         mi_IconCin;
	int         mi_IconCmd;
	int         mi_IconSkl;
	int         mi_IconShp;
	int         mi_IconAck;
	int         mi_IconTrl;
	int         mi_IconOin;
	int         mi_IconOva;
	int         mi_IconMdl;
	int         mi_IconSnk;
	int         mi_IconMsk;
	int			mi_IconGrp;

	//
	-----------------------------------------------------------------------------------------------------------------------
	OVERWRITE.
	-----------------------------------------------------------------------------------------------------------------------
	//

public:
	BOOL	OnInitDialog(void);
		
private:
	void	UpdateTree(int iTypeOfDuplication) ;
	void	VerifyName(void);

	//
	-----------------------------------------------------------------------------------------------------------------------
	MESSAGE MAP.
	-----------------------------------------------------------------------------------------------------------------------
	//

protected:
	afx_msg void OnBnClickedRadio();
	afx_msg void OnEnChangeCombo();
	afx_msg void OnBnClickedOk();
		
	DECLARE_MESSAGE_MAP()
};

/*$4
***********************************************************************************************************************
***********************************************************************************************************************
*/

// -- Forward declaration --
//typedef struct	WOR_tdst_World_ WOR_tdst_World;
// -- Forward declaration --

// -- Type of dialog --
//#define DLG_LIGHT 0
//#define DLG_WAYPOINT 1 

/*class EDIA_cl_NameDialogCompletion : public EDIA_cl_BaseDialog
{

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	CONSTRUCT.
	-----------------------------------------------------------------------------------------------------------------------
	*/

/*public:
	EDIA_cl_NameDialogCompletion (WOR_tdst_World *pst_World, int iType = DLG_WAYPOINT, OBJ_tdst_GameObject *pst_GO = NULL);

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	ATTRIBUTES.
	-----------------------------------------------------------------------------------------------------------------------
	*/

/*private:
	
	CComboBoxCompletion m_cbCompletion ;
	CButton		*m_OKButton ;

	WOR_tdst_World *m_pWorld;
	OBJ_tdst_GameObject	*m_pst_GO;
	int m_iType ;

public:
	CString m_strName ;

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	OVERWRITE.
	-----------------------------------------------------------------------------------------------------------------------
	*/

/*public:
	BOOL	OnInitDialog(void);


private:

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	MESSAGE MAP.
	-----------------------------------------------------------------------------------------------------------------------
	*/

/*protected:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeCombo();
	DECLARE_MESSAGE_MAP()
};



/*$4
***********************************************************************************************************************
***********************************************************************************************************************
*/

////M chbani 
//class EDIA_cl_CopyGAO : public EDIA_cl_BaseDialog
//{
//
//	/*$2
//	-----------------------------------------------------------------------------------------------------------------------
//	CONSTRUCT.
//	-----------------------------------------------------------------------------------------------------------------------
//	*/
//
//public:
//	EDIA_cl_CopyGAO (char * );
//
//	/*$2
//	-----------------------------------------------------------------------------------------------------------------------
//	ATTRIBUTES.
//	-----------------------------------------------------------------------------------------------------------------------
//	*/
//
//public:
//	CString mo_Name;
//	CString mo_Title;
//	int		mi_CurSelData;
//	
//
//	/*$2
//	-----------------------------------------------------------------------------------------------------------------------
//	OVERWRITE.
//	-----------------------------------------------------------------------------------------------------------------------
//	*/
//
//public:
//	BOOL	OnInitDialog(void);
//	
//
//	/*$2
//	-----------------------------------------------------------------------------------------------------------------------
//	MESSAGE MAP.
//	-----------------------------------------------------------------------------------------------------------------------
//	*/
//
//protected:
//	afx_msg void OnBnClickedRadio();
//	afx_msg void OnBnClickedCheck();
//	afx_msg void OnEnChangeCombo();
//	afx_msg void OnBnClickedOk();
//
//	DECLARE_MESSAGE_MAP()
//
//};
//


#endif /* ACTIVE_EDITORS */

#endif //UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIANAME_DLG_H__INCLUDED
