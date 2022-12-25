/*$T DIArlicarte_dlg.h GC! 1.081 05/31/00 10:59:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

#include "BASe/BAStypes.h"

class EDIA_cl_RLICarteDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_RLICarteDialog(struct MDF_tdst_Modifier_ *, struct OBJ_tdst_GameObject_ *, struct GEO_tdst_Object_ * );
	~EDIA_cl_RLICarteDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    struct OBJ_tdst_GameObject_                 *mpst_Gao;
    struct GEO_tdst_Object_                     *mpst_Geo;
	struct MDF_tdst_Modifier_					*mpst_Modifier;
	struct GEO_tdst_ModifierRLICarte_			*mpst_RLICarte;

	int											i_CurGroup;

	CButton										*dpo_OpButton[ 4 ];

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
	int		i_OnMessage(ULONG, ULONG, ULONG);
	void	RefreshFromExt(void);

	void	List_Fill(void);
	void	List_GetCur( void );

	void	Op_Update( void );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnGetMinMaxInfo(MINMAXINFO FAR *);

	afx_msg void	OnButton_Deduct(void);
	afx_msg void	OnButton_Mark( void );
	afx_msg void	OnButton_ColorInit(void);
	afx_msg void	OnButton_AllColorInit(void);
	afx_msg void	OnButton_Color(void);
	afx_msg void	OnButton_AllColor(void);
	afx_msg void	OnButton_Sel(void);
	afx_msg void	OnButton_Update(void);
	afx_msg void	OnButton_Reinit(void);
	afx_msg void	OnButton_OpNone(void);
	afx_msg void	OnButton_OpSet(void);
	afx_msg void	OnButton_OpAdd(void);
	afx_msg void	OnButton_OpSub(void);
	afx_msg void	OnButton_Save( void );
	afx_msg void	OnButton_Load( void );

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
