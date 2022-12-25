/*$T DIAgrovertexpos_dlg.h GC! 1.081 01/09/02 11:17:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

typedef struct	EDIA_VertexPos_Data_
{
	struct GEO_tdst_Object_ *pst_Obj;
	MATH_tdst_Matrix		*M;
	MATH_tdst_Matrix		IM;
	int						nbV;
	MATH_tdst_Vector		**pp_Vertex;
} EDIA_VertexPos_Data;

class F3D_cl_View;

class EDIA_cl_GroVertexPos : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_GroVertexPos(F3D_cl_View *_pst_View);
	~EDIA_cl_GroVertexPos();

	EDIA_VertexPos_Data *mpst_Data;
	int					mi_NbData;
	int					mi_VertexIndex;
	char				*msz_GroName;
	char				*msz_GaoName;

	BOOL				mb_OneObject;
	BOOL				mb_OneVertex;
	BOOL				mb_Global;
    BOOL                mb_Move;

    MATH_tdst_Vector    mst_Pivot;
	MATH_tdst_Vector	mst_Vertex;
    MATH_tdst_Vector    mst_Move;
	int					i_Coord;

    F3D_cl_View         *mpst_View;
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	AddData(struct OBJ_tdst_GameObject_ *, struct GEO_tdst_Object_ *);
    void    Create();
    void    Destroy();

	void	GetVertex(void);
	void	ShowVertex(MATH_tdst_Vector *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			OnInitDialog(void);
	BOOL			PreTranslateMessage(MSG *);

    afx_msg void    OnButtonPosOrShift( void );
	afx_msg void	OnButtonGlobal(void);
	afx_msg void	OnButtonLocal(void);
	afx_msg void	OnButtonDoOperation( void );
	afx_msg void	OnButtonReorder( void );
	afx_msg void	OnVertexChange(void);

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
