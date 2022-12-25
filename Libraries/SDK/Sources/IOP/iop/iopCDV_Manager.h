/*$T iopCDV_Manager.h GC! 1.097 03/20/02 11:02:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    this file can't be included, use CDV_Manager.h
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef _include_iopCDV_Manager_h_
#error this file can not be included in other file than CDV_Manager.h
#endif
#ifdef PSX2_USE_iopCDV
int				iopCDV_i_InitModule(void);
int				iopCDV_i_CloseModule(void);
int				iopCDV_i_SeekFile(int _i_FileHandler, int origin, unsigned int pos);
unsigned int	iopCDV_ui_TellFile(int _i_FileHandler);
int				iopCDV_i_CloseFile(int _i_FileHandler);
int				iopCDV_i_OpenFile(char *asz_FileName);
int				iopCDV_i_ReadFile(int _i_FileHandler, char *_pc_Buff, int _i_Size);
int				iopCDV_i_RemoteOpenFile(char *asz_FileName, CDV_tdst_FileHandler *_pst_Handler);
int				iopCDV_i_SyncRead(unsigned int _ui_SectorsNb, unsigned int _ui_1stSector, char *_pc_Buffer);
int				iopCDV_i_GetiopBufferSize(int _i_FileHandler);
int				iopCDV_i_RemoteOpenFile(char *asz_FileName, CDV_tdst_FileHandler *_pst_Handler);
int				iopCDV_i_SetBuffer(int _i_FileHandler, int _i_Size, char *_pc_Buff);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    only for Snd
 -----------------------------------------------------------------------------------------------------------------------
 */

int				iopCDV_i_DirectSetSize(int _i_FileHandler, int _i_NewSize);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    only for CDV optim
 -----------------------------------------------------------------------------------------------------------------------
 */

extern int		iopCDV_gb_CachedAccess;

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ! PSX2_USE_iopCDV */
extern int		iopCDV_gi_DiscType;

