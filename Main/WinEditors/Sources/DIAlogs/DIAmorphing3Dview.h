/*$T DIAmorphing3Dview_dlg.h GC! 1.081 05/31/00 10:59:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"

#define DP()	(mpo_EngineFrame->mpo_DisplayView)
#define DW()	(DP()->mst_WinHandles.pst_World)
#define DDD()	(DP()->mst_WinHandles.pst_DisplayData)

class EDI_cl_BaseFrame;
class F3D_cl_Frame;

class EDIA_cl_Morphing3DViewDialog : public EDIA_cl_BaseDialog
{

///*$2
// -----------------------------------------------------------------------------------------------------------------------
//    CONSTRUCT.
// -----------------------------------------------------------------------------------------------------------------------
// */
//
//public:
//	EDIA_cl_Morphing3DViewDialog( EDI_cl_BaseFrame * );
//	~EDIA_cl_Morphing3DViewDialog( void );
//
///*$2
// -----------------------------------------------------------------------------------------------------------------------
//    ATTRIBUTES.
// -----------------------------------------------------------------------------------------------------------------------
// */
//
//public:
//	EDI_cl_BaseFrame				*mpo_Owner;
//    OBJ_tdst_GameObject				*mpst_Gao;
//
//	F3D_cl_Frame					*mpo_EngineFrame;
//	int								mi_ViewIdx;
//    
///*$2
// -----------------------------------------------------------------------------------------------------------------------
//    OVERWRITE.
// -----------------------------------------------------------------------------------------------------------------------
// */
//
//public:
//	BOOL	OnInitDialog(void);
//	int		i_OnMessage(ULONG, ULONG, ULONG);
//	void	Refresh(void);
//
//
///*$2
// -----------------------------------------------------------------------------------------------------------------------
//    MESSAGE MAP.
// -----------------------------------------------------------------------------------------------------------------------
// */
//
//public:
//    afx_msg void	OnPaint(void);
//	afx_msg void	OnSize(UINT n, int x, int y);
//
//	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
