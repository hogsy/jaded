/*
 *  File: xboxStreamingEmul.h
 *
 *  Responsibility: To provide asycronous disk access functions mirrored from PS2 (file eeRPC_Manager)
 *
 */
#ifndef __XBOX_STREAMING_EMUL_H__
#define __XBOX_STREAMING_EMUL_H__

void	xbox_StreamingInit(void);

int		xbox_i_AsyncRead(HANDLE _i_File, unsigned int _ui_Pos, char *, int _i_Size, int prio);
int		xbox_i_OpenBigfile(void);
int		xbox_i_GetAsyncStatus(int);
int		xbox_i_CancelAsyncRq(int rq);
void	xbox_RefreshAsyncStatus(void);
void    xbox_AsyncReadReset( void );

/* Threaded function, called by xbSND_Thread */
void	xbox_ThreadedTask(void);


#endif
