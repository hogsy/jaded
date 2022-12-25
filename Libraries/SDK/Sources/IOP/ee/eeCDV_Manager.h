/*$T eeCDV_Manager.h GC! 1.097 03/20/02 11:51:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    this file can't be included, use CDV_Manager.h
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef _include_eeCDV_Manager_h_
#error this file can not be included in other file than CDV_Manager.h
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    default config when using CDV
 -----------------------------------------------------------------------------------------------------------------------
 */
char *JADEPS2_GETNAME(u32 What);
#define CDV_Cte_IcoFile		JADEPS2_GETNAME(0)
#define CDV_Cte_BigFile		JADEPS2_GETNAME(1)
#define CDV_Cte_SpeFile		JADEPS2_GETNAME(2)
#define CDV_Cte_DefaultBin	TRUE
#define CDV_Cte_DefaultSnd	TRUE

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

int				eeCDV_i_InitModule(void);
int				eeCDV_i_CloseModule(void);
int				eeCDV_i_SeekFile(int _i_FileHandler, int origin, unsigned int pos);
unsigned int	eeCDV_ui_TellFile(int _i_FileHandler);
int				eeCDV_i_CloseFile(int _i_FileHandler);
int				eeCDV_i_OpenFile(char *asz_FileName);
int				eeCDV_i_ReadFile(int _i_FileHandler, char *_pc_Buff, int _i_Size);
int				eeCDV_i_GetFileSize(int _i_FileHandler);
int				eeCDV_i_GeteeBufferSize(int _i_FileHandler);
int				eeCDV_i_SeteeBufferSize(int _i_FileHandler, int _i_NewSize);
int				eeCDV_i_DirectReadFile(int _i_FileHandler, char **_pc_Buff, int _i_Size);
int				eeCDV_i_DirectSeekFile(unsigned int pos);

#ifdef PSX2_USE_iopCDV
int				eeCDV_i_SetiopBufferSize(int _i_FileHandler, int _i_NewSize);
int				eeCDV_i_GetiopBufferSize(int _i_FileHandler);
#endif
