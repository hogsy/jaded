/*$T DIAmtx_dlg.h GC! 1.097 03/29/02 11:08:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "Sound/Sources/MTX.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_MtxDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_MtxDialog(ULONG _ul_VideoFatIndex = -1, ULONG _ul_SoundFatIndex = -1);
    ~EDIA_cl_MtxDialog(void) ;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:

	/* video */
	ULONG						mul_VideoFatIndex;
	CString						mo_VideoFile;
	struct MTX_tdst_VideoInfo_	mst_VideoInfo;
    BOOL                        mb_VideoIsNTSC; 
    unsigned int                mui_VideoBufferSize;
	float						mf_VideoDuration;

	/* sound */
	ULONG						mul_SoundFatIndex;
	CString						mo_SoundFile;
	int							mi_SoundPlayerFrequency;
	float						mf_SoundPlayerVolume;
	struct MTX_tdst_SoundInfo_	mst_SoundInfo;    
    unsigned int                mui_SoundBufferSize;
    unsigned int                mui_SoundTotalSize;    

	/* output */
	ULONG						mul_OutputFatIndex;
	CString						mo_OutputPath;
	CString						mo_OutputFile;
	BOOL						mb_OutputFileExist;

	BOOL						mb_EnableOKButton;

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	void						SetPath(ULONG, int);
	void						CheckCreationCondition(void);
	void						SetOutputFileFromVideoFile(BOOL b_Refresh = TRUE);
    void                        SetVideoDuration(void);
    void                        SetSoundDuration(void);
	void						EvalVideoBufferSize(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	DoDataExchange(CDataExchange *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnSelectSoundFile(void);
	afx_msg void	OnSelectNtsc(void);
	afx_msg void	OnSelectPal(void);
	afx_msg void	OnSelectVideoFile(void);
	afx_msg void	OnSelectOutputFile(void);
	afx_msg void	OnEditChangeSndBuff(void);
	afx_msg void	OnEditChangeVideoBuff(void);
	afx_msg void	OnEditChangeSndFreq(void);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
