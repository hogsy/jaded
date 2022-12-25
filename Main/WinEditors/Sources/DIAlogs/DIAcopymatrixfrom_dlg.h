/*$T DIAcopymatrixfrom_dlg.h GC! 1.081 08/23/01 15:05:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

#define EDIA_CopyMatrixFlag_Trans       0x01
#define EDIA_CopyMatrixFlag_Rotation    0x02
#define EDIA_CopyMatrixFlag_Scale       0x04
#define EDIA_CopyMatrixFlag_TransX      0x08
#define EDIA_CopyMatrixFlag_TransY      0x10
#define EDIA_CopyMatrixFlag_TransZ      0x20

#define EDIA_CopyMatrixFlag_All         0x3F
#define EDIA_CopyMatrixFlag_AllAxes     0x38

class EDIA_cl_CopyMatrixFrom : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_CopyMatrixFrom( struct OBJ_tdst_GameObject_ *, ULONG );
    ~EDIA_cl_CopyMatrixFrom();

    struct OBJ_tdst_GameObject_ *mpst_GO;

    ULONG   mul_Flags;
    

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
    void	OnOK(void);

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
