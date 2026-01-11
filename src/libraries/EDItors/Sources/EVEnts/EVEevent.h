/*$T EVEevent.h GC! 1.077 03/21/00 11:22:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EDIbaseframe.h"

class	EEVE_cl_Track;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	EEVE_cl_Event
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EEVE_cl_Event (void);
	~EEVE_cl_Event(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Attributes
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	CString					mo_Name;
	EEVE_cl_Track			*mpo_Track;
	int						mi_DrawMode;
	CRect					mo_Rect;
	float					mf_Width;
	float					mf_CurWidth;
	int						mi_NumEvent;
	BOOL					mb_Empty;
	int						mi_IndexAI;
	BOOL					mb_Expand;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void					Init(void);

	void					Draw(CDC *, CRect *, BOOL = FALSE, BOOL = FALSE);
	void					RecomputeDelay(void);
    void                    RecomputeTimeKey( unsigned short );
	void					RecomputeWidth(void);
	void					RoundTo(float);
	struct EVE_tdst_Event_	*pst_FindEngineEvent(void);
    EEVE_cl_Event           *po_FindEditorEvent(struct EVE_tdst_Event_ *);

	/*$1- AI function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	void					SetAIFunctionName(void);
	void					DrawAIFunction(CDC *, CRect *);
	void					FillAIFunction(struct EVE_tdst_Event_ *);
	void					ComputeAIFunction(struct EVE_tdst_Event_ *, int);
	void					SetAIFunction(struct EVE_tdst_Event_ *);
	bool					b_TestAIFunctionType(short);
	void					SetSoundPlayerAIFunction(struct EVE_tdst_Event_ *);
	void					SetIntParameter(int, int);						
	int						i_GetIntParameter(int);

	/*$1- Interpolation key ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	void					SetInterpolationKeyName(void);
	void					DrawInterpolationKey(struct EVE_tdst_Event_	*, CDC *, CRect *);
	void					FillInterpolationKey(struct EVE_tdst_Event_ *);
	void					SetInterpolationKey(struct EVE_tdst_Event_ *, BOOL _b_New = TRUE);
    void					ComputeInterpolationKey( struct EVE_tdst_Event_ *, int, BOOL  );
    int                     InterpolationKey_i_HitTest( struct EVE_tdst_Event_ *, CPoint );
    void                    InterpolationKey_OnMouseMove( struct EVE_tdst_Event_ *, int, float );

    /*$1 - Time key ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	void					SetTimeKeyName(void);
	void					DrawTimeKey(struct EVE_tdst_Event_	*, CDC *, CRect *);
	void					FillTimeKey(struct EVE_tdst_Event_ *);
	void					SetTimeKey(struct EVE_tdst_Event_ *);
    void					ComputeTimeKey( struct EVE_tdst_Event_ *, int );

	void					OnLButtonDown(void);
	void					OnLButtonDblClk(void);
    void                    OnMButtonDown(void);
	/*$1 - Morph key ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	void					FillMorphKey(struct EVE_tdst_Event_ *);
	void					DrawMorphKey(struct EVE_tdst_Event_	*, CDC *, CRect *);
};
#endif /* ACTIVE_EDITORS */
