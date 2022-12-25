/*$T DIAlist_dlg.h GC! 1.100 03/07/01 15:25:21 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define EDIA_List_SpeedSelectGAO		1
#define EDIA_List_SpeedSelectMDL		2
#define EDIA_List_SpeedSelectFCT		3
#define EDIA_List_SpeedSelectHistoryFCT 4
#define EDIA_List_SpeedSelectGlob		5
#define EDIA_List_CallStack				6
#define EDIA_List_SpeedSelectSector		7
#define EDIA_List_SelectMultipleLangage	8
#define EDIA_List_SelectMultipleTxg		9

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_ListDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_ListDialog(int);
	~EDIA_cl_ListDialog();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	int						mi_Mode;
	class F3D_cl_View		*mpo_View;
	struct WOR_tdst_World_	*mpst_World;
	class EAI_cl_Frame		*mpo_AIFrame;
	BIG_INDEX				mul_Model;
	BIG_INDEX				mul_Current;
	
	int						*mpi_Sel, mi_SelNb;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
	void	SpeedSelectGAO(void);
	void	SpeedSelectMDL(BIG_INDEX);
	void	SpeedSelectGlob(BIG_INDEX);
	void	SpeedSelectFCT(void);
	void	SpeedSelectHisFCT(void);
	void	SpeedSelectCallStack(void);
	void	SpeedSelectSector(void);
	void	SelectMultipleLangage( void );
	void	SelectMultipleTxg( void );
	void	OnOK(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	afx_msg void	OnSize(UINT, int, int);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
