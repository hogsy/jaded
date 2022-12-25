#include <libgraph.h>
#include "Gsp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"

/*$4 prototypes */
GspGifTag PictureGifTag __attribute__((aligned(64)));

void Gsp_ShowVram(u_int Y , u_int W , u_int ColorMode)
{
	sceGsDispEnv gs_DDBG __attribute__((aligned(16)));
	ColorMode = 4 - (ColorMode >> 3);
	sceGsSetDefDispEnv(&gs_DDBG , (short)GspGlobal_ACCESS(BppMode) , (short)GspGlobal_ACCESS(Xsize) , (short)GspGlobal_ACCESS(Ysize) , (short)0 , (short)0 );
	gs_DDBG.dispfb.DBX = 0;
	gs_DDBG.dispfb.DBY = Y;
	sceGsPutDispEnv(&gs_DDBG);
}

void GSP_ClearVRAM()
{
	GspGifTag *p_PictureGifTag ;
	u_int *p_ClearBuf, *p_Save;
	unsigned long MipmapCounter;
	u_int XC,YC;
	
	MEMpro_StartMemRaster();

	p_Save = p_ClearBuf = (u_int *)MEM_p_Alloc(512L * 64L * 4L + 128L);
	L_memset(p_Save , 0 , 512L * 64L * 4L + 128L);
	p_ClearBuf = (u_int *)(((u_int)p_ClearBuf & (~63)) + 64);
	GSP_UnCachePtr(p_ClearBuf);
	Gsp_Flush(FLUSH_ALL);
	
	for (MipmapCounter = 0 ; MipmapCounter < 2048L ; MipmapCounter += 64L)
	{
		p_PictureGifTag = &PictureGifTag;
//		Gsp_Flush(FLUSH_ALL);
		GSP_UnCachePtr(p_PictureGifTag);
		GSP_SetRegister(GSP_BITBLTBUF 	, 8L << 48L);
		GSP_SetRegister(GSP_TRXPOS 	, MipmapCounter << 48L);//*/
		GSP_SetRegister(GSP_TRXREG 	, (/*RRW*/512L << 0L)|(/*RRH*/64L << 32L));
		GSP_SetRegister(GSP_TRXDIR 	, (0L << 0L));
		Gsp_SetGSRegisters();
		Gsp_M_SetGifTag(p_PictureGifTag, 512L * 64L >> 2L , 1 , 0 , 0 , GSP_GIF_FLG_IMAGE , 0 , 0);
		Gsp_SendToGif((u_int)p_PictureGifTag, 1L);
		Gsp_SendToGif((u_int)p_ClearBuf, 512L * 64L >> 2L);
		Gsp_SetGSRegisters();//*/
	}
	MEM_Free(p_Save);
	GSP_FLushAllTextureCache();
	MEMpro_StopMemRaster(MEMpro_Id_GSP);
}
