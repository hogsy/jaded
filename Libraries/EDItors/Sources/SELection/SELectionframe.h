/*$T SELectionframe.h GC 1.138 10/14/04 14:40:27 */


/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


#pragma once
#ifdef ACTIVE_EDITORS
#include "EDIbaseframe.h"

#include "SELectionframe_act.h"
#include "SELectionGridCtrl.h"

class ESELection_cl_Frame : public EDI_cl_BaseFrame
{
	DECLARE_DYNCREATE(ESELection_cl_Frame)

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	CONSTRUCT.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	ESELection_cl_Frame (void);
	~ESELection_cl_Frame (void);

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	ATTRIBUTES.
	-----------------------------------------------------------------------------------------------------------------------
	*/

protected:
	SELectionGridCtrl*  mp_GridCtrl;

	BOOL mb_ShowStandard;
	BOOL mb_ShowShading;
	BOOL mb_ShowNormalMap;
	BOOL mb_ShowDetailNormalMap;
	BOOL mb_ShowSpecular;
	BOOL mb_ShowEnvironmentMap;
	BOOL mb_ShowMossMap;
    BOOL mb_ShowRimLight;
	BOOL mb_ShowMeshProcessing;

public:
	struct
	{
	} mst_Ini;

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	FUNCTIONS.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	void	OnAction(ULONG);
	BOOL	b_KnowsKey(USHORT);
	BOOL	b_CanActivate(void);
	BOOL	b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
	char	*psz_OnActionGetBase(void)	{ return ESELection_asz_ActionBase; };
	int		i_OnMessage(ULONG, ULONG, ULONG);
	UINT	ui_OnActionState(ULONG);
	void	OnCtrlPopup(CPoint pt);

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	INTERFACE.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	void	OpenProject(void);
	void	CloseProject(void);

	// Save all materials contained in GRID
	void	SaveMaterials(void);

	void	ReinitIni(void);
	void	LoadIni(void);
	void	TreatIni(void);
	void	SaveIni(void);

	void InsertRowsGrid();
	void RemoveRowsGrid();

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	MESSAGE MAP.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnClose(void);

	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
