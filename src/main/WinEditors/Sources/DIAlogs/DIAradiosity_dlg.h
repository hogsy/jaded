/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#ifdef ACTIVE_EDITORS

#include "MATHs/MATH.h"
#include "DIAlogs/DIAbase.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class EDIA_cl_RadiosityDialog : public EDIA_cl_BaseDialog
{
	ULONG *Flags;
	ULONG *Quality;
	ULONG *BreakEdgeThresh;
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_RadiosityDialog(ULONG *Flags , ULONG *TheQulity,ULONG *BET);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
   BOOL    OnInitDialog(void);

   afx_msg void    OnGO(void);
   afx_msg void    OnSO(void);
   afx_msg void    OnOP(void);
   afx_msg void    OnEP(void);
   afx_msg void    OnABE(void);
   afx_msg void    OnQualityCombo   (void);
   afx_msg void    OnBET(void);
   afx_msg void    OnSmoothFront(void);
   afx_msg void    OnComputeRadiosity(void);
   afx_msg void    OnUBCKCLR(void);
   afx_msg void    OnSmoothRes(void);
   afx_msg void    OnFasthRes(void);
   DECLARE_MESSAGE_MAP()


/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    
};
#endif  /* ACTIVE_EDITORS */
