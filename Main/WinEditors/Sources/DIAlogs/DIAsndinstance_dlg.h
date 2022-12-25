/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#ifdef ACTIVE_EDITORS


#include "DIAlogs/DIAbase.h"
#include "SouND/Sources/SNDstruct.h"
//#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDconst.h"
//#include "SouND/Sources/SNDwave.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class EDIA_cl_SndInstanceDialog : public EDIA_cl_BaseDialog
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_SndInstanceDialog(int index = -1);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    SND_tdst_SoundInstance mst_SoundInstance;    
    int mi_Index;

    void CheckDynVol(void);
    
/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    DoDataExchange(CDataExchange *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnUseCheckDynPan(void);
    afx_msg void    OnUseCheckX(void);
    afx_msg void    OnUseCheckY(void);
    afx_msg void    OnUseCheckZ(void);
    afx_msg void    OnUseCheckDoppler(void);
    afx_msg void    OnUseCheckFx(void);
    afx_msg void    OnUseSphericVol(void);
    afx_msg void    OnUseAxisVol(void);
    afx_msg void    OnUseGroup1(void);
    afx_msg void    OnUseGroup2(void);
    afx_msg void    OnUseGroup3(void);
    afx_msg void    OnUseGroup4(void);
    afx_msg void    OnUseGroup5(void);
    afx_msg void    OnApply(void);
    afx_msg void    OnOk(void);
    afx_msg void    OnRefresh(void);
    DECLARE_MESSAGE_MAP()

};
#endif  /* ACTIVE_EDITORS */
