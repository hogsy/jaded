/*$T DIArli_dlg.h GC! 1.081 06/05/01 10:50:29 */


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

#define DIARLI_op_none				    0
#define DIARLI_op_compute			    1
#define DIARLI_op_copylg			    2
#define DIARLI_op_copygl			    3
#define	DIARLI_op_reset				    4
#define DIARLI_op_destroy			    5
#define DIARLI_op_destroycompute	    6
#define DIARLI_op_computealphatocolor	7
#define DIARLI_op_lock              	8
#define DIARLI_op_unlock              	9
#define DIARLI_op_invertalpha         	10

#define DIARLI_tgt_GRO      1
#define DIARLI_tgt_GAO      2

class	F3D_cl_View;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_RLIDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_RLIDialog( F3D_cl_View * );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    F3D_cl_View	    *mpo_3DView;
    ULONG           mul_Target;
	ULONG	        mul_Mask;
	ULONG	        mul_Operation;

    BOOL            mb_ObjectSel;
    BOOL            mb_LightSel;

	void	        GetParams(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
	void	OnDestroy(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	
    afx_msg void    OnBt_GaoCompute(void);
    afx_msg void    OnBt_GaoDestroy(void);
    afx_msg void    OnBt_GaoLock(void);
    afx_msg void    OnBt_GaoUnlock(void);
    afx_msg void    OnBt_GaoAdvanced(void);
	
	afx_msg void    OnBt_GroCompute(void);
    afx_msg void    OnBt_GroDestroy(void);
    afx_msg void    OnBt_GroLock(void);
    afx_msg void    OnBt_GroUnlock(void);
    afx_msg void    OnBt_GroAdvanced(void);

    afx_msg void    OnBt_CopyGao2Gro(void);
	afx_msg void    OnBt_CopyGro2Gao(void);
    afx_msg void    OnBt_CopyGao2Lod(void);

	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
