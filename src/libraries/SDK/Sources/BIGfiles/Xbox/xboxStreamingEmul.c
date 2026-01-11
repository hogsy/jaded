/*
 *  File: xboxStreamingEmul.c
 *
 *  Responsibility: To provide asycronous disk access function mirrored from PS2 (file eeRPC_Manager)
 *
 */

#include "Precomp.h"
#include <xtl.h>
#include <stdbool.h>
#include "BIGfiles/XBox/xboxStreamingEmul.h"
#include "SouND/Sources/xbox/xbSND_Thread.h"
#include "BASe/ERRors/ERRasser.h"

typedef enum
{
	xbox_strm_Free,
	xbox_strm_Busy,
	xbox_strm_Finish,
	xbox_strm_Error,
	xbox_strm_Waiting,
    xbox_strm_Resetting
} xbox_strm_Status;

typedef struct
{
	HANDLE			 i_FileHandler;
	unsigned int	 ui_Position;
	char			 *pc_FileBuffer;
	int				 i_BufferSize;
	xbox_strm_Status i_Status;
} xbox_ReadRequest;

#define xbox_strm_MaxRequestSupported	10

typedef struct
{
/*	int 			b_Valid;
	int				i_FileHandler;
	unsigned int	ui_FilePosition;
	unsigned int	ui_FileSize;*/
	xbox_ReadRequest	ReadRequest[xbox_strm_MaxRequestSupported];
} xbox_Streaming;

volatile xbox_Streaming		xbox_g_Streaming;

void xbox_StreamingInit(void)
{
	int i;

//	xbox_g_Streaming.b_Valid = false;
	
	for( i=0; i<xbox_strm_MaxRequestSupported; i++ )
	{
		xbox_g_Streaming.ReadRequest[i].i_Status = xbox_strm_Free;
	}
}

int	xbox_i_AsyncRead(HANDLE _i_File, unsigned int _ui_Pos, char *_pc_Buff, int _i_Size, int prio)
{
	int i;

	// Search a free ReadRequest space
	for( i=0; xbox_g_Streaming.ReadRequest[i].i_Status != xbox_strm_Free ; ++i )
	{
		if( i == xbox_strm_MaxRequestSupported-1 )
			return -1;
	}
	
	// Store the incoming ReadRequest
	xbox_g_Streaming.ReadRequest[i].i_FileHandler = _i_File;
	xbox_g_Streaming.ReadRequest[i].ui_Position = _ui_Pos;
	xbox_g_Streaming.ReadRequest[i].pc_FileBuffer = _pc_Buff;
	xbox_g_Streaming.ReadRequest[i].i_BufferSize = _i_Size;
	xbox_g_Streaming.ReadRequest[i].i_Status = xbox_strm_Waiting;
	
	// WakeUp the thread to serve the request
	SetEvent(xbSND_gah_Event[xbSND_e_EventAsyncLoading]);
	
	return i;
}

int	xbox_i_OpenBigfile(void)
{
	return 0;
}

int	xbox_i_GetAsyncStatus(int rq)
{
	if((rq < 0) || (rq >= xbox_strm_MaxRequestSupported))
		return -1;
	
	switch(xbox_g_Streaming.ReadRequest[rq].i_Status)
	{
		case xbox_strm_Finish:
		case xbox_strm_Free:
			xbox_g_Streaming.ReadRequest[rq].i_Status = xbox_strm_Free;
			return 1;
		
		case xbox_strm_Busy:
		case xbox_strm_Waiting:
			return 0;

		default:
		case xbox_strm_Error:
			xbox_g_Streaming.ReadRequest[rq].i_Status = xbox_strm_Free;
			return -1;
	}
}

int	xbox_i_CancelAsyncRq(int rq)
{
	if((rq < 0) || (rq >= xbox_strm_MaxRequestSupported))
		return -1;
	
	switch(xbox_g_Streaming.ReadRequest[rq].i_Status)
	{
		case xbox_strm_Busy:
			// Nothing to do, the loading is already started
			break;
		
		case xbox_strm_Waiting:
			/// For Now, do nothing. Think about avoid race condition
			break;
		
		case xbox_strm_Error:
		case xbox_strm_Finish:
		default:
			xbox_g_Streaming.ReadRequest[rq].i_Status = xbox_strm_Free;
	}
	
	return 0;
}

void xbox_RefreshAsyncStatus(void)
{
}

void xbox_AsyncReadReset(void)
{
    int i;

	for( i=0; i<xbox_strm_MaxRequestSupported; i++ )
	{
        if( xbox_g_Streaming.ReadRequest[i].i_Status == xbox_strm_Busy )
        {
            xbox_g_Streaming.ReadRequest[i].i_Status = xbox_strm_Resetting;
        }
        else
        {
            xbox_g_Streaming.ReadRequest[i].i_Status = xbox_strm_Free;
        }
    }
}

void xbox_ThreadedTask(void)
{
	int i;

	for( i=0; i<xbox_strm_MaxRequestSupported; i++ )
	{
        ERR_X_Assert( xbox_g_Streaming.ReadRequest[i].i_Status != xbox_strm_Resetting );
		switch(xbox_g_Streaming.ReadRequest[i].i_Status)
		{
			case xbox_strm_Waiting:
			{
				bool bSuccess;
				DWORD numBytes;

				// Say I'm busy loading something
				xbox_g_Streaming.ReadRequest[i].i_Status = xbox_strm_Busy;
				
				// Serve the request (LOAD)
				SetFilePointer( xbox_g_Streaming.ReadRequest[i].i_FileHandler,
							    xbox_g_Streaming.ReadRequest[i].ui_Position, NULL, FILE_BEGIN );
				bSuccess = ReadFile( xbox_g_Streaming.ReadRequest[i].i_FileHandler,
									 xbox_g_Streaming.ReadRequest[i].pc_FileBuffer, 
									 xbox_g_Streaming.ReadRequest[i].i_BufferSize, &numBytes, NULL );
				
                ERR_X_Assert( xbox_g_Streaming.ReadRequest[i].i_Status == xbox_strm_Busy ||
                              xbox_g_Streaming.ReadRequest[i].i_Status == xbox_strm_Resetting );

                if( xbox_g_Streaming.ReadRequest[i].i_Status == xbox_strm_Busy )
                {
				    xbox_g_Streaming.ReadRequest[i].i_Status = ( bSuccess && (numBytes == xbox_g_Streaming.ReadRequest[i].i_BufferSize) ?
					                                             xbox_strm_Finish : xbox_strm_Error );
                }
                else    // xbox_g_Streaming.ReadRequest[i].i_Status == box_strm_Resetting
                {
                    xbox_g_Streaming.ReadRequest[i].i_Status = xbox_strm_Free;
                }
				break;
			}
			case xbox_strm_Busy:
				xbox_g_Streaming.ReadRequest[i].i_Status = xbox_strm_Error;	// IMPOSSIBLE!!!!
				xbox_g_Streaming.ReadRequest[i].pc_FileBuffer = NULL;
				break;
			
			case xbox_strm_Error:
			case xbox_strm_Finish:
			default:
				break;
		}
	}
}



