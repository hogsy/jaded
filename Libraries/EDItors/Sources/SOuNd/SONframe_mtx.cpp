/*$T SONframe_mtx.cpp GC! 1.081 07/29/02 12:15:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "Sound/Sources/SNDwave.h"
#include "Sound/Sources/SNDconv.h"
#include "Sound/Sources/SNDconst.h"
#include "Sound/Sources/MTX.h"
#include "SONframe.h"

#include "EDImainframe.h"
#include "DIAlogs/DIAmtx_dlg.h"
#include "BIGfiles/BIGread.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKmsg.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"

#include "Res/Res.h"


#include "IOP/RPC_Manager.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    private types
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

char			gsz_WorkingPath[BIG_C_MaxLenPath] = ".";
unsigned int	gui_MaxBufferSize = 300 * 1024;

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

int				MTX_MergeVideoAndSound
				(
					BOOL	_bNTSC,
					MTX_tdst_VideoInfo *,
					MTX_tdst_SoundInfo *,
					unsigned int *,
					char **,
					unsigned int _ui_VideoBufferSize,
					unsigned int _ui_SoundBufferSize
				);
void			MTX_InsertDataBlock
				(
					FILE *,
					unsigned int *,
					char *,
					unsigned int _ui_DataSize,
					unsigned int *,
					unsigned int _ui_WriteSize
				);

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnCreateMTX(ULONG _ul_Fat)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_MtxDialog	*po_Dialog;
	MTX_tdst_VideoInfo	*pst_Video;
	MTX_tdst_SoundInfo	*pst_Sound;
	unsigned int		ui_MtxSize;
	char				*pc_MtxBuffer;
	int					result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Dialog = new EDIA_cl_MtxDialog(_ul_Fat);

	if(po_Dialog->DoModal() == IDOK)
	{
		if(po_Dialog->mb_OutputFileExist)
		{
			M_MF()->MessageBox("File already exists, creation canceled !", "Error", MB_OK | MB_ICONSTOP);
		}
		else
		{
			pst_Video = &po_Dialog->mst_VideoInfo;
			pst_Sound = &po_Dialog->mst_SoundInfo;
			pst_Sound->f_PlayerVol = po_Dialog->mf_SoundPlayerVolume;
			pst_Sound->i_PlayerFreq = po_Dialog->mi_SoundPlayerFrequency;

			ui_MtxSize = 0;
			pc_MtxBuffer = NULL;

			LINK_PrintStatusMsg("Create MTX file...");
			result = MTX_MergeVideoAndSound
				(
					po_Dialog->mb_VideoIsNTSC,
					pst_Video,
					pst_Sound,
					&ui_MtxSize,
					&pc_MtxBuffer,
					po_Dialog->mui_VideoBufferSize,
					po_Dialog->mui_SoundBufferSize					
				);

			if(!result)
			{
				SAV_Begin((char *) (LPCSTR) po_Dialog->mo_OutputPath, (char *) (LPCSTR) po_Dialog->mo_OutputFile);

				LINK_PrintStatusMsg("Saving file (super long!)...");
				SAV_Buffer(pc_MtxBuffer, ui_MtxSize);
				SAV_ul_End();

				LINK_FatHasChanged();
				LINK_PrintStatusMsg("File created !");
			}
			else
			{
				LINK_PrintStatusMsg("Process aborted !");
			}

			L_free(pc_MtxBuffer);
		}
	}

	delete po_Dialog;
}

/*
 =======================================================================================================================
    file header block header block block header block block header block block header block block header block block
    header block
 =======================================================================================================================
 */
int MTX_MergeVideoAndSound
(
	BOOL				_bNTSC,
	MTX_tdst_VideoInfo	*pst_VideoInfo,
	MTX_tdst_SoundInfo	*pst_SoundInfo,
	unsigned int		*_pui_MtxSize,
	char				**_pc_MtxBuffer,
	unsigned int		_ui_VideoBufferSize,
	unsigned int		_ui_SoundBufferSize
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	L_FILE					h_File;
	char					*pc_Parser;
	unsigned int			ui_VideoWrittenSize;
	unsigned int			ui_SoundWrittenSize;
	unsigned int			ui_SoundWrittenSize2;
	MTX_tdst_MtxFileHeader	st_FileHeader;
	char					asz_TempPath[MAX_PATH];
	char					asz_TempFile[MAX_PATH];
	DWORD					length;
	float					f_Frame;
	EDIA_cl_UPDATEDialog	*po_Dial;
	int						N, R;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    open temp file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	length = GetTempPath(MAX_PATH, asz_TempPath);
	if(!length)
	{
		ERR_X_Warning(0, "Can not find temp path", NULL);
		return -1;
	}

	length = GetTempFileName(asz_TempPath, "mtx", 0, asz_TempFile);
	if(!length)
	{
		ERR_X_Warning(0, "Temp file can not be created, operation failed.", NULL);
		return -1;
	}

	h_File = L_fopen(asz_TempFile, "w+b");
	if(!h_File)
	{
		ERR_X_Warning(0, "Temp file can not be opened, operation failed.", NULL);
		return -1;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    temp buffer
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pc_Parser = (char *) L_malloc(gui_MaxBufferSize);
	if(!pc_Parser)
	{
		ERR_X_Warning(0, "temp buffer can not be allocated, operation failed.", NULL);
		L_fclose(h_File);
		return -1;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    progress bar
	 -------------------------------------------------------------------------------------------------------------------
	 */

	po_Dial = new EDIA_cl_UPDATEDialog((char *) "");
	po_Dial->DoModeless();

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    reset var
	 -------------------------------------------------------------------------------------------------------------------
	 */

	*_pc_MtxBuffer = NULL;
	*_pui_MtxSize = 0;
	L_memset(&st_FileHeader, 0, sizeof(MTX_tdst_MtxFileHeader));
	f_Frame = _bNTSC ? 1.0f / 60.0f : 1.0f / 50.0f;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    write video+sound headers
	 -------------------------------------------------------------------------------------------------------------------
	 */

	st_FileHeader.ui_FileID = MTX_Cte_FileId;
	st_FileHeader.ui_FileVersion = MTX_Cte_FileVersion;
	st_FileHeader.ui_FileSize = 0;

	st_FileHeader.st_Video.ui_FileSize = pst_VideoInfo->ui_FileSize;
	st_FileHeader.st_Video.ui_VideoBufferSize = _ui_VideoBufferSize;

	/* check parser size */
	if(gui_MaxBufferSize < st_FileHeader.st_Video.ui_VideoBufferSize)
	{
		pc_Parser = (char *) L_realloc(pc_Parser, st_FileHeader.st_Video.ui_VideoBufferSize);
		gui_MaxBufferSize = st_FileHeader.st_Video.ui_VideoBufferSize;
	}

	N = st_FileHeader.st_Video.ui_FileSize / st_FileHeader.st_Video.ui_VideoBufferSize;
	R = st_FileHeader.st_Video.ui_FileSize - (N * st_FileHeader.st_Video.ui_VideoBufferSize);
	if(R) N++;

	if(pst_SoundInfo->ui_DataPos && (pst_SoundInfo->ui_DataPos != -1))
	{
		st_FileHeader.st_Sound.f_Vol = pst_SoundInfo->f_PlayerVol;
		st_FileHeader.st_Sound.ui_Channel = pst_SoundInfo->ui_Channel;
		st_FileHeader.st_Sound.ui_Freq = pst_SoundInfo->i_Freq;
		st_FileHeader.st_Sound.ui_SoundBufferSize = _ui_SoundBufferSize;
		st_FileHeader.st_Sound.ui_DataSize = N * st_FileHeader.st_Sound.ui_SoundBufferSize;
	}
	else
	{
		st_FileHeader.st_Sound.f_Vol = 0.0f;
		st_FileHeader.st_Sound.ui_Channel = 0;
		st_FileHeader.st_Sound.ui_Freq = 0;
		st_FileHeader.st_Sound.ui_SoundBufferSize = 0;
		st_FileHeader.st_Sound.ui_DataSize = 0;
	}

	L_fwrite(&st_FileHeader, sizeof(MTX_tdst_MtxFileHeader), 1, h_File);
	(*_pui_MtxSize) += sizeof(MTX_tdst_MtxFileHeader);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    then go ahead
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ui_VideoWrittenSize = 0;
	ui_SoundWrittenSize2 = ui_SoundWrittenSize = 0;

	if(st_FileHeader.st_Sound.ui_SoundBufferSize)
	{
		MTX_InsertDataBlock
		(
			h_File,
			_pui_MtxSize,
			pst_SoundInfo->pc_Data,
			pst_SoundInfo->ui_DataSize,
			&ui_SoundWrittenSize,
			st_FileHeader.st_Sound.ui_SoundBufferSize
		);

		if(st_FileHeader.st_Sound.ui_Channel == 2)
		{
			MTX_InsertDataBlock
			(
				h_File,
				_pui_MtxSize,
				pst_SoundInfo->pc_Data + pst_SoundInfo->ui_DataSize,
				pst_SoundInfo->ui_DataSize,
				&ui_SoundWrittenSize2,
				st_FileHeader.st_Sound.ui_SoundBufferSize
			);
		}
	}
	
	/* **** make an empty video data *** */
	{
		L_memset(pc_Parser, 0, st_FileHeader.st_Video.ui_VideoBufferSize);
		L_fwrite(pc_Parser, st_FileHeader.st_Video.ui_VideoBufferSize, 1, h_File);
		(*_pui_MtxSize) += st_FileHeader.st_Video.ui_VideoBufferSize;
	}
	/* *** *** *** *** *** *** *** *** *** *** */

	while((st_FileHeader.st_Video.ui_VideoBufferSize + ui_VideoWrittenSize) < pst_VideoInfo->ui_FileSize)
	{
		po_Dial->OnRefreshBarText
			(
				(float) (st_FileHeader.st_Video.ui_VideoBufferSize + ui_VideoWrittenSize) /
					(float) pst_VideoInfo->ui_FileSize,
					"..."
			);
		if(po_Dial->bIsCanceled) break;

		/*$1- write sound block ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(st_FileHeader.st_Sound.ui_SoundBufferSize)
		{
			MTX_InsertDataBlock
			(
				h_File,
				_pui_MtxSize,
				pst_SoundInfo->pc_Data,
				pst_SoundInfo->ui_DataSize,
				&ui_SoundWrittenSize,
				st_FileHeader.st_Sound.ui_SoundBufferSize
			);

			if(st_FileHeader.st_Sound.ui_Channel == 2)
			{
				MTX_InsertDataBlock
				(
					h_File,
					_pui_MtxSize,
					pst_SoundInfo->pc_Data + pst_SoundInfo->ui_DataSize,
					pst_SoundInfo->ui_DataSize,
					&ui_SoundWrittenSize2,
					st_FileHeader.st_Sound.ui_SoundBufferSize
				);
			}
		}

		/* read */
		BIG_Read(pst_VideoInfo->ui_FilePos + ui_VideoWrittenSize, pc_Parser, st_FileHeader.st_Video.ui_VideoBufferSize);
		ui_VideoWrittenSize += st_FileHeader.st_Video.ui_VideoBufferSize;

		/* write */
		L_fwrite(pc_Parser, st_FileHeader.st_Video.ui_VideoBufferSize, 1, h_File);
		(*_pui_MtxSize) += st_FileHeader.st_Video.ui_VideoBufferSize;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    check cancelation
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(po_Dial->bIsCanceled)
	{
		L_free(pc_Parser);
		L_fclose(h_File);
		DeleteFile(asz_TempFile);
		delete(po_Dial);
		return -1;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    end of file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(ui_VideoWrittenSize < pst_VideoInfo->ui_FileSize)
	{
		/*~~~~~~~~~~~~~~~~~~~~*/
		unsigned int	ui_last;
		/*~~~~~~~~~~~~~~~~~~~~*/

		if(st_FileHeader.st_Sound.ui_SoundBufferSize)
		{
			MTX_InsertDataBlock
			(
				h_File,
				_pui_MtxSize,
				pst_SoundInfo->pc_Data,
				pst_SoundInfo->ui_DataSize,
				&ui_SoundWrittenSize,
				st_FileHeader.st_Sound.ui_SoundBufferSize
			);
			if(st_FileHeader.st_Sound.ui_Channel == 2)
			{
				MTX_InsertDataBlock
				(
					h_File,
					_pui_MtxSize,
					pst_SoundInfo->pc_Data + pst_SoundInfo->ui_DataSize,
					pst_SoundInfo->ui_DataSize,
					&ui_SoundWrittenSize2,
					st_FileHeader.st_Sound.ui_SoundBufferSize
				);
			}
		}

		ui_last = pst_VideoInfo->ui_FileSize - ui_VideoWrittenSize;
		BIG_Read(pst_VideoInfo->ui_FilePos + ui_VideoWrittenSize, pc_Parser, ui_last);
		ui_VideoWrittenSize += ui_last;

		/* complete this block */
		L_memset(pc_Parser + ui_last, 0, st_FileHeader.st_Video.ui_VideoBufferSize - ui_last);
		ui_VideoWrittenSize += (st_FileHeader.st_Video.ui_VideoBufferSize - ui_last);

		L_fwrite(pc_Parser, st_FileHeader.st_Video.ui_VideoBufferSize, 1, h_File);
		(*_pui_MtxSize) += st_FileHeader.st_Video.ui_VideoBufferSize;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    free locals
	 -------------------------------------------------------------------------------------------------------------------
	 */

	L_free(pc_Parser);
	delete(po_Dial);
	po_Dial = NULL;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    re-load the file in RAM
	 -------------------------------------------------------------------------------------------------------------------
	 */

	*_pc_MtxBuffer = (char *) L_malloc((*_pui_MtxSize));
	if(!*_pc_MtxBuffer)
	{
		ERR_X_Warning(0, "[MTX] can not allocate the last temp buffer", NULL);
		return -1;
	}

	L_fseek(h_File, 0, SEEK_SET);
	L_fread((*_pc_MtxBuffer), (*_pui_MtxSize), 1, h_File);
	L_fclose(h_File);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    update file size
	 -------------------------------------------------------------------------------------------------------------------
	 */

	st_FileHeader.ui_FileSize = (*_pui_MtxSize);
	L_memcpy((*_pc_MtxBuffer), &st_FileHeader, sizeof(MTX_tdst_MtxFileHeader));

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    delete temp file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	DeleteFile(asz_TempFile);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MTX_InsertDataBlock
(
	FILE			*_h_File,
	unsigned int	*_pui_MtxSize,
	char			*_pc_Data,
	unsigned int	_ui_DataSize,
	unsigned int	*_pui_SoundWrittenSize,
	unsigned int	_ui_WriteSize
)
{
	/*~~~~~~~~~~~~*/
	char	*pc_Src;
	/*~~~~~~~~~~~~*/

	if(!_h_File) return;

	pc_Src = _pc_Data + *_pui_SoundWrittenSize;
	if(_ui_DataSize >= _ui_WriteSize + *_pui_SoundWrittenSize)
	{
		L_fwrite(pc_Src, _ui_WriteSize, 1, _h_File);
		(*_pui_MtxSize) += _ui_WriteSize;
		(*_pui_SoundWrittenSize) += _ui_WriteSize;
	}
	else
	{
		L_fwrite(pc_Src, _ui_DataSize - *_pui_SoundWrittenSize, 1, _h_File);
		(*_pui_MtxSize) += (_ui_DataSize - *_pui_SoundWrittenSize);

		_ui_WriteSize = _ui_WriteSize - (_ui_DataSize - *_pui_SoundWrittenSize);
		*_pui_SoundWrittenSize = 0;

		if(_ui_WriteSize)
		{
			L_fwrite(_pc_Data, _ui_WriteSize, 1, _h_File);
			(*_pui_MtxSize) += _ui_WriteSize;
			(*_pui_SoundWrittenSize) += _ui_WriteSize;
		}
	}
}

#endif /* ACTIVE_EDITORS */
