/*$T OGLrequest.c GC! 1.086 06/28/00 17:18:55 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#include "ddraw.h"
#endif

#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>
#include <devvif1.h>
#include <libvifpk.h>
#include <libipu.h>

#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"

#include "Gsp.h"
#include "GSPinit.h"
#include "GSPtex.h"
#include "GDInterface/GDInterface.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "MATerial/MATstruct.h"
#include "GSPdebugfct.h"
#include "Gsp_Bench.h"
#include "BASe/BENch/Bench.h"
#include "GSP_Video.h"
#include "GSPtex.h"
#include "SDK/Sources/IOP/RPC_Manager.h"
#include "SDK/Sources/IOP/iop/iopCLI.h"

extern u_int NoVideo;
#if 0
 // Bon j'ai modifé les fonctions utilisées pour le preload pour pouvoir être appelées pour la video :
 
//1/ déclaration et init
#include "SDK/Sources/IOP/RPC_Manager.h"
int global_i_fd;
int global_i_requestID;
char* global_pc_Buffer;
int global_i_BufferSize
int global_i_CurrentSeek;
 
void Init(void)
{
	global_i_fd = eeRPC_i_OpenBigfile(); // si fd < 0 -> erreur
	global_i_requestID = -1;
	global_pc_Buffer = MEM_p_Alloc(max size of buffer);  // attention pour l'instant les refresh de stream ne peuvent pas dépasser les 40 Ko
	global_i_BufferSize = 0;
	global_i_CurrentSeek = 0;
}
 
//2/ faire une requète de lecture asynchrone
global_i_BufferSize = taille à lire < 40Ko
global_i_CurrentSeek = postion dans le BF
global_i_requestID = eeRPC_i_AsyncRead(global_i_fd, global_i_CurrentSeek, global_pc_Buffer, global_i_BufferSize,1);
if(global_i_requestID<0) erreur;
 
//3/ checker la fin de la lecture avant utilisation du buffer
int i;
i = eeRPC_i_GetAsyncStatus(global_i_requestID);
switch(i)
{
    case 1:
        fin ok, on peut utiliser le buffer
    case -1:
        erreur, requète annulée
    case 0:
        requète en cours
}
#endif

#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif

#define GSP_VIDEOSTRBUFFERSIZE 256*1024*1
#define GSP_VIDEO_GUARDBAND 1024
#define GSP_VIDEO_CLUSTEROPT 2048
GSP_VideoStream stVideoFullScreen;
u_int 			ulVideoScreentValid = 0;
u_int           GSP_ui_Video=0;
extern u_int volatile VBlankOdd;

/********************************************************************************************************************************/
/* 													Stream's used by video START												*/
/********************************************************************************************************************************/
int 		lBigFileID = -1;

u_int GSP_StreamCreate(GSP_Stream *p_Str , u_int ulBigPos , u_int ulBufferSize)
{

	return 1;
}
u_int GSP_StreamUpdate(GSP_Stream *p_Str )
{
	return 0;
}
u_char *GSP_StreamGetNextBuffer(GSP_Stream *p_Str )
{
/*	if (p_Str->iSourceFile < 0) return NULL;
	if (p_Str->pCurrentBufferSOFT->ulState == GSP_StrBuf_Ready_For_SOFT)
	{
		p_Str->pCurrentBufferSOFT->ulState = GSP_StrBuf_Used_By_SOFT;
		if (p_Str->pCurrentBufferSOFT->p_PreviousBuffer->ulState == GSP_StrBuf_Used_By_SOFT)
		{
			p_Str->pCurrentBufferSOFT->p_PreviousBuffer->ulState = GSP_StrBuf_Ready_For_IO;
			L_memcpy(p_Str->pCurrentBufferSOFT->pBuffer - GSP_VIDEO_GUARDBAND, p_Str->pCurrentBufferSOFT->p_PreviousBuffer->pBuffer + p_Str->ulBufferSize - GSP_VIDEO_GUARDBAND, GSP_VIDEO_GUARDBAND);
		}
		p_Str->pCurrentBufferSOFT = p_Str->pCurrentBufferSOFT->p_NextBuffer;
		p_Str->ulValidBufferNum--;
		return p_Str->pCurrentBufferSOFT->p_PreviousBuffer->pBuffer;
	} else */return NULL;
}
void GSP_StreamRestart(GSP_Stream *p_Str)
{
/*	u_int BufferCounter;
	if (p_Str->iSourceFile < 0) return;
	if(ulVideoScreentValid) 
	{
		ulVideoScreentValid |= 2;
		eeRPC_ReinitFileStreaming(0);
	}
	
	
	BufferCounter = M_GSP_Str_RotativBufferNum();
	while (BufferCounter--)
	{	
		if (p_Str->stBuffers[BufferCounter].ulState == GSP_StrBuf_Used_By_IO)
		{
			if (eeRPC_i_CancelAsyncRq(p_Str->stBuffers[BufferCounter].lRequestNumber) == 0)
				p_Str->stBuffers[BufferCounter].ulState = GSP_StrBuf_Ready_For_IO;
			else
				p_Str->stBuffers[BufferCounter].ulState = GSP_StrBuf_Used_By_Trash;
			p_Str->stBuffers[BufferCounter].lRequestNumber = -1;
		}
		else
			p_Str->stBuffers[BufferCounter].ulState = GSP_StrBuf_Ready_For_IO;
	}
	p_Str->ulBigPosCurrent = p_Str->ulBigPosStart;
	p_Str->pCurrentBufferSOFT = p_Str->pCurrentBufferIO;
	p_Str->ulValidBufferNum = 0;
}
void GSP_StreamClose(GSP_Stream *p_Str)
{
	u_int GetNe , BufferCounter;
	if (p_Str->iSourceFile < 0) return;
	if (ulVideoScreentValid) eeRPC_DestroyFileStreaming(TRUE);
	GetNe  = 1;
	while (GetNe)
	{
		GetNe = 0;
		BufferCounter = M_GSP_Str_RotativBufferNum();
		while (BufferCounter--)
		{	
			if (p_Str->stBuffers[BufferCounter].ulState == GSP_StrBuf_Used_By_IO)
			{
				if (eeRPC_i_CancelAsyncRq(p_Str->stBuffers[BufferCounter].lRequestNumber) == 0)
					p_Str->stBuffers[BufferCounter].ulState = GSP_StrBuf_Ready_For_IO;
				else 
					GetNe = 1;
				p_Str->stBuffers[BufferCounter].lRequestNumber =-1;
			}
		}
	}
	
	for (GetNe = 0 ; GetNe < M_GSP_Str_RotativBufferNum() ; GetNe ++)
	{
		*((u_int *)&(p_Str->stBuffers[GetNe].pBuffer)) -= GSP_VIDEO_GUARDBAND;
	    MEM_FreeAlign((void *)((u_int)p_Str->stBuffers[GetNe].pBuffer & 0x0fffffff));
	}
    p_Str->iSourceFile = -1;
    GSP_ui_Video = 0;*/
}
/********************************************************************************************************************************/
/* 													Stream's used by video END													*/
/********************************************************************************************************************************/

/********************************************************************************************************************************/
/* 														video 																	*/
/********************************************************************************************************************************/

void GSP_VideoStreamInit( GSP_VideoStream *p_VideoStr )
{
}

u_int GSP_VideoStreamCreate( GSP_VideoStream *p_VideoStr , u_int ulBigPos ,  u_int MaxImageSize , u_int *p_OutBuffer)
{

}
void GSP_VideoStreamClose(GSP_VideoStream *p_VideoStr )
{

}
void GSP_VideoStreamRestart(GSP_VideoStream *p_VideoStr )
{

}
void GSP_VideoStreamSync_Pulse(GSP_VideoStream *p_VideoStr)
{

}
_inline_ u_int VD16(u_int *pDEST,u_int *pSRC , u_int Pitch)
{
return 0;
}


void GSP_VideoStreamSync(GSP_VideoStream *p_VideoStr)
{
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */

void GSP_VideoCopyImage(GSP_VideoStream *p_VideoStr , u_int *pDEST)
{
}
extern u_int GSP_VRamLoadText_FROM_RAM(u_int VramPtr , u_int DBW , u_int *p_FirstPixel , u_int W , u_int H ,u_int DX,u_int DY,  u_int BPP , u_int TagAddress);

void GSP_VideoScreenSet(GSP_VideoStream *p_VideoStr , signed int X, signed int Y)
{

}
extern void Gsp_InitFB_VIDEO(u_int VOn);
void GSP_VideoFullScreenCreate(ULONG ulBigKey )
{

}

void GSP_VideoFullScreenUpdate()
{

}

void GSP_VideoFullScreenRestart()
{

}

void GSP_VideoFullScreenKill()
{

}



#if defined PSX2_TARGET && defined __cplusplus
}
#endif

