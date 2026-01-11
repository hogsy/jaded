/*$T DIAmorphingsliders_dlg.h GC! 1.081 05/31/00 10:59:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"

#define		COLOR_BCKGRND		0
#define		COLOR_PROG0			1
#define		COLOR_PROG1			2
#define		COLOR_FACTOR0		3
#define		COLOR_FACTOR1		4
#define		COLOR_CURSOR		5

#define		COLOR_NUMBER		6

class EDIA_cl_MorphingDialog;

typedef struct EDIAMorph_tdst_Line_
{
	float	f_FactorOffset;
	float	f_FactorEnd;
	float	f_ProgOffset;
	float	f_ProgEnd;
	int		i_Visible;
	int		i_Index;
	CRect	o_Factor;
	CRect	o_Prog;
	CRect	o_Rect;
} EDIAMorph_tdst_Line;

class EDIA_cl_MorphingSlidersDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_MorphingSlidersDialog( EDIA_cl_MorphingDialog *, int );
	~EDIA_cl_MorphingSlidersDialog( void );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_MorphingDialog						*mpo_Owner;
	
    struct OBJ_tdst_GameObject_                 *mpst_Gao;
    struct GEO_tdst_Object_                     *mpst_Geo;
	struct MDF_tdst_Modifier_					*mpst_Modifier;
	struct GEO_tdst_ModifierMorphing_			*mpst_Morph;
	struct GEO_tdst_ModifierMorphing_Channel_	*mpst_CurChannel;
    struct GRO_tdst_EditOptions_	            *mpst_EditOptions;

	BOOL										mb_LockRefresh;
    int                                         mi_Capture;
	int											mi_CaptureLine;
	CPoint										mo_CapturePt;

    CRect                                       mo_FactorRect;
    CRect                                       mo_ProgRect;
    float                                       mf_FactorOffset;
    float                                       mf_ProgOffset;
	int											mi_WindowHeight;
    
    int											mi_ShowFactor;
    int											mi_ShowProg;
    int											mi_ShowSlider;
    
    EDIAMorph_tdst_Line							mast_Line[ 8 ];
	int											mi_LineNb;
	int											mi_Synchronise;
    
    int											mi_UnitWidth;
    //float										mf_UnitSegs;
    
    CFont										mo_SmallFont;
	ULONG										maaul_Color[ 2 ][ COLOR_NUMBER ];
    
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
	int		i_OnMessage(ULONG, ULONG, ULONG);
	void	RefreshFromExt(void);

    int     LB_i_ItemFromPoint( CListBox *, CPoint *);

    void    Factor_Set( int, int );
    
	int		Line_GetHeight( int );
	void	Line_Display( CDC *, int , CRect *, int );

	void	Line_SetProgFromPoint( int, CPoint );
	void	Line_SetFactorFromPoint( int, CPoint );

	void	Window_SetHeight( void );
	BOOL	PlugTo3DView( int );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			PreTranslateMessage(MSG *);
	afx_msg void	OnGetMinMaxInfo(MINMAXINFO FAR *);
    afx_msg void	OnPaint(void);

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
