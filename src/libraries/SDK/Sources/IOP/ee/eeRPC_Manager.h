/*$T eeRPC_Manager.h GC! 1.081 10/01/02 11:51:38 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    this file can't be included, use RPC_Manager.h
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef _include_eeRPC_Manager_h_
#error this file can not be included in other file than RPC_Manager.h
#endif

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- module ---------------------------------------------------------------------------------------------------------*/

int		eeRPC_i_InitModule(void);
int		eeRPC_i_LoadModule(int i_flag);

/*$2- async read interface -------------------------------------------------------------------------------------------*/

int		eeRPC_i_AsyncRead(int _i_File, unsigned int _ui_Pos, char *, int _i_Size, int prio);
int		eeRPC_i_OpenBigfile(void);
int		eeRPC_i_GetAsyncStatus(int);
int		eeRPC_i_CancelAsyncRq(int rq);
void	eeRPC_RefreshAsyncStatus(void);
int		eeRPC_i_ExecAsyncCommand
		(
			unsigned int	_ui_Cmd,
			char *,
			unsigned int _ui_InBufferSize,
			char *,
			unsigned int _ui_OutBufferSize,
			void * _pfv_callback,
			void *
		);

/*$2- file streaming -------------------------------------------------------------------------------------------------*/

int		eeRPC_i_CreateFileStreaming(unsigned int _ui_FilePosition, unsigned int _ui_FileSize);
void	eeRPC_DestroyFileStreaming(BOOL _b_CloseFile);
void	eeRPC_ReinitFileStreaming(unsigned int _ui_ReinitFilePosition);

/*$2- IOP command ----------------------------------------------------------------------------------------------------*/

int		eeRPC_i_PushCommand(int _i_Command, void *, int _i_InSize, void *, int _i_OutSize);
void	eeRPC_FlushCommandBeforeEngine(void);
void	eeRPC_FlushCommandAfterEngine(void);

/*$2------------------------------------------------------------------------------------------------------------------*/

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

