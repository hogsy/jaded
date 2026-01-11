/*$T DIAmixer_dlg.h GC! 1.081 11/04/02 15:05:54 */


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

class EDIA_cl_MixerDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_MixerDialog(void);
	~EDIA_cl_MixerDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	HMIXER			mh_Mixer;
	MIXERCAPS		mst_MixerCaps;
	MIXERLINE		mst_MixerLine;
	MIXERLINE		mst_MixerConnection;
	MIXERCONTROL	mst_MixerControl;

	int				mi_CurrMixerLineIndex;
	int				mi_CurrMixerConnection;
	int				mi_CurrMixerControl;

	HMIXER			MixAppNewDevice(HMIXER hmxCur, UINT uMxId);
	BOOL			MixAppRefreshLineList(void);
	BOOL			MixAppGetComponentName(LPMIXERLINE pmxl, char *);
	BOOL			MixAppGetControlTypeName(LPMIXERCONTROL pmxctrl, char *);
	LRESULT			MixAppLineChange(HMIXER hmx, DWORD dwLineID);
	LRESULT			MixAppControlChange(HMIXER hmx, DWORD dwControlID);
	void			MixHideAllButtons(void);
	void			DrawCustomControl(void *);
	void			DrawMeterControl(void *);
	void			DrawSwitchControl(void *);
	void			DrawNumberControl(void *);
	void			DrawSliderControl(void *);
	void			DrawFaderControl(void *);
	void			DrawListControl(void *);
	void			DrawUnknownControl(void *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	PreTranslateMessage(MSG *);
	BOOL	OnInitDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	afx_msg void	OnSelMixerLine(void);
	afx_msg void	OnSelMixerConnection(void);
	afx_msg void	OnSelMixerControl(void);
	afx_msg void	OnPos1Change(void);
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
