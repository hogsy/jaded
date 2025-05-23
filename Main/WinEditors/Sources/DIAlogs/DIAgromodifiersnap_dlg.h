/*$T DIAgromodifiersnap_dlg.h GC!1.55 01/17/00 17:26:24 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

class EDIA_cl_GroModifierSnapDialog : public EDIA_cl_BaseDialog
{
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_GroModifierSnapDialog (void);
    ~EDIA_cl_GroModifierSnapDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    float                       mf_Treshold;
    struct OBJ_tdst_GameObject_ *mpst_TgtGao;
    struct OBJ_tdst_GameObject_ *mpst_SrcGao;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    BOOL    b_InitList(void);
    BOOL    b_InitSrc(void);
    void    InitTresh(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    BOOL    OnInitDialog(void);
    void    OnOK(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected: DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
