/*$T gcINOsaving.h GC! 1.081 06/18/02 11:09:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */



#ifndef __gcINOsaving_h__
#define __gcINOsaving_h__

#ifdef __cplusplus
extern "C"
{
#endif


BOOL 	gcINO_b_SavIsWorking(void);
int 	gcINO_i_ReadFile(char *pBuff, int iToRead, int *piRead);
int 	gcINO_i_WriteFile(char *pBuff, int iToWrite, int *piWritten);
int 	gcINO_i_GetCurrAction(void);
void 	gcINO_SavUpdate(void);
int 	gcINO_SavSeti(int rq, int val);
void	gcINO_SavModuleInit(void);
void	gcINO_SavModuleClose(void);














#if 0



/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

#define gcINO_Cte_SavMagic				0x1248ACE0
#define gcINO_Cte_SavAllocUnit			8
#define gcINO_Cte_SavRequiredSectSize	(8 * 1024)
#define gcINO_Cte_SavPortNbMax			2


/*#pragma pack(1)
typedef struct	gcINO_tdst_SavFileDescriptor_
{
	unsigned int	ui_Size;
	unsigned int	ui_Offset;
	char			az_Name[INO_Cte_SavFileNameLength];
} gcINO_tdst_SavFileDescriptor;
#pragma pack(0)*/


/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2------------------------------------------------------------------------------------------------------------------*/

void	gcINO_SavModuleInit(void);
void	gcINO_SavModuleClose(void);

/*$2------------------------------------------------------------------------------------------------------------------*/
int		gcINO_i_SavGetMinSpace(int);

int		gcINO_i_SavActivateConsoleBrowser(void);

/*$2------------------------------------------------------------------------------------------------------------------*/

int		gcINO_i_FileRead(int _i_FileHandler, void *, int _i_Size);
int		gcINO_i_FileWrite(int _i_FileHandler, void *, int _i_Size);

/*$2------------------------------------------------------------------------------------------------------------------*/

int 	gcINO_i_FileGetSize(char *);


/*$2------------------------------------------------------------------------------------------------------------------*/
void	 gcINO_v_MemoryCardManager(int);

BOOL	gcINO_b_AsyncInProgress(int);
int 	gcINO_i_TestPresence(int);
int 	gcINO_i_MountCard(int);
int 	gcINO_i_UnmountCard(int);
int 	gcINO_i_CheckCard(int);
int 	gcINO_i_FormatCard(int);
int 	gcINO_i_TestFreeSpace(int);
int 	gcINO_i_Retry(int);
int 	gcINO_i_Continue(int);
int 	gcINO_i_ActiveConsole(int);
int 	gcINO_i_Open(int);
int 	gcINO_i_Close(int);
int 	gcINO_i_ReadIndex_1(int);
int 	gcINO_i_WriteIndex(int);
int 	gcINO_i_ReadSlot_1(int);
int 	gcINO_i_ReadSlot_2(int);
int 	gcINO_i_ReadSlot_3(int);
int 	gcINO_i_WriteSlot_C(int);
int 	gcINO_i_WriteSlot_E(int);
int 	gcINO_i_WriteSlot_F(int);
int 	gcINO_i_CheckFreeSpace(int);
int 	gcINO_i_TestCard(int);
int 	gcINO_i_Delete(int);
BOOL 	gcINO_b_SavIsWorking(void);
int 	gcINO_i_ReadFile(char *pBuff, int iToRead, int *piRead);
int 	gcINO_i_WriteFile(char *pBuff, int iToWrite, int *piWritten);
int 	gcINO_i_GetCurrAction(void);
void 	gcINO_SavUpdate(void);
int gcINO_SavSeti(int rq, int val);

#endif // 0


#ifdef __cplusplus
}
#endif
#endif /* __INOsaving_h__ */
