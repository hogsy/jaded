/*$T DIAcheckworld_dlg.h GC! 1.081 05/29/01 10:33:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "EDItors/Sources/PERForce/PERCDataCtrl.h"
#ifdef JADEFUSION
#include "LIGHT/LIGhtrejection.h"
#endif
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

#ifdef JADEFUSION
//define for info type
#define	EDIAC_InfoType_Normal		0
#define	EDIAC_InfoType_IsLightedBy	1
#endif

class EDIA_cl_GaoInfo: public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
#ifdef JADEFUSION
	EDIA_cl_GaoInfo(struct OBJ_tdst_GameObject_ *,int _i_InfoType = 0);
#else
	EDIA_cl_GaoInfo(struct OBJ_tdst_GameObject_ *);
#endif
	~EDIA_cl_GaoInfo();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	struct OBJ_tdst_GameObject_	*mpst_Gao;
    void *                      mpo_View;

    int                     mi_TreeContent;
    int                     mi_IconGro;
    int                     mi_IconGrm;
    int                     mi_IconTex;
    int                     mi_IconGao;
    int                     mi_IconGrl;
    int                     mi_IconMap;
    int                     mi_IconCob;
    int                     mi_IconGam;
    int                     mi_IconCin;
    int                     mi_IconCmd;
    int                     mi_IconSkl;
    int                     mi_IconShp;
    int                     mi_IconAck;
    int                     mi_IconTrl;
    int                     mi_IconOin;
	int                     mi_IconOva;
    int                     mi_IconMdl;
    int                     mi_IconSnk;
    int                     mi_IconMsk;
	int						mi_IconGrp;
	int						mi_IconRli;
	int						mi_IconPfb;


    ULONG                   mul_Index;
#ifdef JADEFUSION
	int						mi_InfoType;
#endif
protected:

	// Used to communicate with the Perforce API
	PER_CDataCtrlEmulator	m_DataCtrlEmul;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
	void	OnDestroy(void);
    BOOL    PreTranslateMessage( MSG * );

    void    FillTree( void );
#ifdef JADEFUSION
	void	FillTreeMaterialMulti(GRO_tdst_Struct* _pst_Gro, HTREEITEM hParent);
#endif
    void    ChangeGao( struct OBJ_tdst_GameObject_ * );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	afx_msg void	OnSize(UINT, int, int);
    afx_msg void    OnBtGo( void );
    afx_msg void    OnSelect(NMHDR *, LRESULT *);

#ifdef JADEFUSION
	afx_msg void    OnBt3DVIew( void );
	afx_msg void	OnBnClickedRadio();
#endif
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
