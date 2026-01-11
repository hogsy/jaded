/*$T iopCLI.h GC 1.138 06/01/04 09:48:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __iopCLI_h__
#define __iopCLI_h__

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef PSX2_TARGET

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define iopCLI_Cte_PreLoadSize	(40 * 1024)
/**/
#ifdef PSX2_IOP

/*$4-******************************************************************************************************************/

#define L_min(a, b) ((a) < (b) ? (a) : (b))
#define L_max(a, b) ((a) < (b) ? (b) : (a))

#ifdef TRACE_OVERWRITE
#define L_memset(a,b,c)	iopMemSet(a,b,c,__FILE__, __LINE__)
#define L_memcpy(a,b,c)	iopMemCpy(a,b,c,__FILE__, __LINE__)
#else
#define L_memset	iopMemSet_asm
#define L_memcpy	memcpy
#endif


/*$2------------------------------------------------------------------------------------------------------------------*/

#ifdef PSX2_USE_iopCDV
#define L_fopen(_name)							iopCDV_i_OpenFile((_name))
#define L_fclose(_fd)							iopCDV_i_CloseFile((_fd))
#define L_fseeknosema(_fd, _origin, _offset)	iopCDV_i_SeekFile((_fd), (_origin), (_offset))
#else
#define L_fopen(_name)							open((_name), O_RDONLY)
#define L_fclose(_fd)							close((_fd))
#define L_fseeknosema(_fd, _origin, _offset)	lseek((_fd), (_offset), (_origin))
#endif /* PSX2_USE_iopCDV */

#define L_freadnosema(a, b, c)		__L_freadnosema((a), (b), (c))
#define L_seekandread(a, b, c, d)	__L_seekandread((a), (b), (c), (d))

/*$2- semaphores -----------------------------------------------------------------------------------------------------*/

#ifdef _CLI_DBGMESSAGE
#define CLI_WaitSema(_id)		__CLI_WaitSema((_id), __FILE__, __LINE__)
#define CLI_SignalSema(_id)		__CLI_SignalSema((_id), __FILE__, __LINE__)
#define CLI_iSignalSema(_id)	__CLI_iSignalSema((_id), __FILE__, __LINE__)
#else
#define CLI_WaitSema(_id)		WaitSema((_id))
#define CLI_SignalSema(_id)		SignalSema((_id))
#define CLI_iSignalSema(_id)	iSignalSema((_id))
#endif /* _CLI_DBGMESSAGE */

/*$2------------------------------------------------------------------------------------------------------------------*/

#define iopCLI_Cte_RqFree		1
#define iopCLI_Cte_RqReading	2
#define iopCLI_Cte_RqSucceed	3
#define iopCLI_Cte_RqFailed		4

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	iopCLI_tdst_ReadRequest_
{
	int				i_FileHandler;
	unsigned int	ui_FileSeek;
	char			*pc_FileBuffer;
	char			*pc_WriteBuffer;
	int				i_BufferSize;
	int				i_Status;
	int				i_RdId;
} iopCLI_tdst_ReadRequest;

typedef struct	IOP_tdst_StrFile_
{
	int				i_FileHandler;
	unsigned int	ui_FilePosition;
	unsigned int	ui_FileSize;
	unsigned int	ui_FileCurrSeek;

	char			*pc_StrBuffer;
	unsigned int	ui_BufferSize;

	unsigned int	ui_MuxFrameSize;
	unsigned short	us_Channel;
	volatile unsigned short	us_Allocated;
} IOP_tdst_StrFile;

#define IOP_Cte_StrFileNb	2




int		IOP_i_StrFileInit(IOP_tdst_StrFile *_pStr);
int		IOP_StrFileRead(int id, unsigned int _ui_Seek, char *_pc_Buffer, unsigned int _ui_Size);
void	IOP_StrFileFree(int i);
int     IOP_i_StrFileReinit(int i, unsigned int ui_FilePosition, unsigned int ui_FileSize);

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- mem ------------------------------------------------------------------------------------------------------------*/

asm void	*iopMemSet_asm(char *, unsigned char c, int size);

#ifdef TRACE_OVERWRITE
void	    *iopMemSet(char *, unsigned char c, int size, char*, int);
void	    *iopMemCpy(char *, char *, int size, char*, int);
#endif

/*$2- file access ----------------------------------------------------------------------------------------------------*/

int		iopCLI_OpenFile(char *);
void	iopCLI_CloseFile(int _i_Id);
int		iopCLI_i_AddRequestReadFile(int, int, unsigned int, int);
int		iopCLI_i_GetFileRequest(int, char **);
void	iopCLI_FileSystemExec(int);
char	*iopCLI_pc_AllocBuffer(void);
void	iopCLI_FreeBuffer(char *);
int		iopCLI_i_FileExecRead(void *, char *);
void	iopCLI_ResetAllRequestReadFile(void);

/*$2- module ---------------------------------------------------------------------------------------------------------*/

void	iopCLI_InitModule(void);

/*$4
 ***********************************************************************************************************************
    inline
 ***********************************************************************************************************************
 */

#ifdef _CLI_DBGMESSAGE

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void __CLI_WaitSema(int _id, char *file, int line)
{
	/*~~~~~*/
	int code;
	/*~~~~~*/

	iopDbg_M_AssertX((_id > 0), iopDbg_Err_005F ": %d %s(%d)", _id, file, line);
	if(_id <= 0) return;
	code = WaitSema(_id);
	switch(code)
	{
	case KE_OK:					break;
	case KE_ILLEGAL_CONTEXT:	iopDbg_M_ErrX(iopDbg_Err_0060 " : KE_ILLEGAL_CONTEXT : %s(%d)", file, line); break;
	case KE_UNKNOWN_SEMID:		iopDbg_M_ErrX(iopDbg_Err_0060 " : KE_UNKNOWN_SEMID : %s(%d)", file, line); break;
	case KE_SEMA_ZERO:			iopDbg_M_ErrX(iopDbg_Err_0060 " : KE_SEMA_ZERO : %s(%d)", file, line); break;
	case KE_RELEASE_WAIT:		iopDbg_M_ErrX(iopDbg_Err_0060 " : KE_RELEASE_WAIT : %s(%d)", file, line); break;
	case KE_CAN_NOT_WAIT:		iopDbg_M_ErrX(iopDbg_Err_0060 " : KE_CAN_NOT_WAIT : %s(%d)", file, line); break;
	case KE_WAIT_DELETE:		iopDbg_M_ErrX(iopDbg_Err_0060 " : KE_WAIT_DELETE : %s(%d)", file, line); break;
	default:					iopDbg_M_ErrX(iopDbg_Err_0060 " : ??? : %s(%d)", file, line); break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void __CLI_SignalSema(int _id, char *file, int line)
{
	/*~~~~~*/
	int code;
	/*~~~~~*/

	iopDbg_M_AssertX(_id > 0, iopDbg_Err_005F ": %d %s(%d)", _id, file, line);
	if(_id <= 0) return;
	code = SignalSema(_id);
	switch(code)
	{
	case KE_OK:					break;
	case KE_ILLEGAL_CONTEXT:	iopDbg_M_ErrX(iopDbg_Err_0061 " : KE_ILLEGAL_CONTEXT : %s(%d)", file, line); break;
	case KE_UNKNOWN_SEMID:		iopDbg_M_ErrX(iopDbg_Err_0061 " : KE_UNKNOWN_SEMID : %s(%d)", file, line); break;
	case KE_SEMA_OVF:			iopDbg_M_ErrX(iopDbg_Err_0061 " : KE_SEMA_OVF : %s(%d)", file, line); break;
	default:					iopDbg_M_ErrX(iopDbg_Err_0061 " : ??? : %s(%d)", file, line); break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void __CLI_iSignalSema(int _id, char *file, int line)
{
	/*~~~~~*/
	int code;
	/*~~~~~*/

	iopDbg_M_AssertX(_id > 0, iopDbg_Err_005F ": %d %s(%d)", _id, file, line);
	if(_id <= 0) return;
	code = iSignalSema(_id);
	switch(code)
	{
	case KE_OK:					break;
	case KE_ILLEGAL_CONTEXT:	iopDbg_M_ErrX(iopDbg_Err_0062 " : KE_ILLEGAL_CONTEXT : %s(%d)", file, line); break;
	case KE_UNKNOWN_SEMID:		iopDbg_M_ErrX(iopDbg_Err_0062 " : KE_UNKNOWN_SEMID : %s(%d)", file, line); break;
	case KE_SEMA_OVF:			iopDbg_M_ErrX(iopDbg_Err_0062 " : KE_SEMA_OVF : %s(%d)", file, line); break;
	default:					iopDbg_M_ErrX(iopDbg_Err_0062 " : ??? : %s(%d)", file, line); break;
	}
}
#endif /* _CLI_DBGMESSAGE */

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern int										iopCLI_FileStreamingSema;
extern volatile struct iopCLI_tdst_ReadRequest_ iopCLI_gast_Buff[];
extern int										iopCLI_Sema;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CD/DVD access
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef PSX2_USE_iopCDV
#include "CDV_Manager.h"
#include "iop/iopMain.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */

inline int __L_freadnosema(int _fd, char *_buff, unsigned int _size)
{
	if(iopCDV_i_ReadFile(_fd, _buff, _size) != _size)
	{
		iopDbg_M_Err(iopDbg_Err_0014);
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int __L_seekandread(int _fd, unsigned int _position, char *_buff, unsigned int _size)
{
	/*~~~~~~~~~~~~~~~~*/
	unsigned int	res;
	/*~~~~~~~~~~~~~~~~*/

	CLI_WaitSema(iopCLI_Sema);
	iopCDV_i_SeekFile(_fd, CDV_Cte_SeekSet, _position);
	res = iopCDV_i_ReadFile(_fd, _buff, _size);
	CLI_SignalSema(iopCLI_Sema);

	if(res != _size) return -1;
	return 0;
}

#else /* PSX2_USE_iopCDV */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern int start(void);


inline int __L_freadnosema(int _fd, char *_buff, unsigned int _size)
{

#ifdef TRACE_OVERWRITE
    if((int)_buff<(int)start)
    {
        printf("[IOP][__L_freadnosema] %x tentative ecrasement ptr %x size %d from %s(%d)\n", start, _buff, _size, __FILE__, __LINE__);
    }
#endif

	read(_fd, _buff, _size);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int __L_seekandread(int _fd, unsigned int _position, char *_buff, unsigned int _size)
{

#ifdef TRACE_OVERWRITE
    if((int)_buff<(int)start)
    {
        printf("[IOP][__L_seekandread ff] %x tentative ecrasement ptr %x size %d from %s(%d)\n",start, _buff, _size, __FILE__, __LINE__);
    }
#endif

	CLI_WaitSema(iopCLI_Sema);
	lseek(_fd, _position, SEEK_SET);
	read(_fd, _buff, _size);
	CLI_SignalSema(iopCLI_Sema);
	return 0;
}

#endif /* PSX2_USE_iopCDV */

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* PSX2_IOP */
#endif /* PSX2_TARGET */

#ifdef __cplusplus
}
#endif
#endif /* __iopCLI_h__ */
