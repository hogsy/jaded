#include <eeregs.h>
#include <libgraph.h>
#include "Gsp.h"

extern __declspec(scratchpad) u_int volatile gsulSyncroPath;
extern void GSP_ArtisanalFlushAll();

void Gsp_Flush_FAST(int ulAnd)
{
	u_int MEror;
#ifdef GSP_USE_TRIPLE_BUFFERRING
	if (ulAnd & FLUSH_DMA1)
		GSP_ArtisanalFlushAll();
#endif	
	if (gsulSyncroPath & FLUSH_WORLDLOAD)
		if (!(gsulSyncroPath & GIF_LOCKED))		
			gsulSyncroPath = gsulSyncroPath;
	
	MEror = 30000000;
	ulAnd &= ~GIF_LOCKED;
	if (gsulSyncroPath & FLUSH_DMA1) 
		GspGlobal_ACCESS(DMA_VS_CPU)++;
	else 
		GspGlobal_ACCESS(CPU_VS_DMA)++;

	while ((gsulSyncroPath & ulAnd) && MEror){MEror--;}; 
	ulAnd &= ~(FLUSH_DMA1|FLUSH_DMA2|FLUSH_DMA9|FLUSH_DMA8);
	while (ulAnd && MEror)
	{
		ulAnd &= sceGsSyncPath(1, 0);
		MEror--;
	}

	if (!MEror)
	{
	    extern int InfiniteCrash;
	    InfiniteCrash = 1;
	    GSP_RestoreCorrectPath(1);
	} 
};

void Gsp_Flush_DBG(u_int ulAnd)
{
	u_int MEror;
	
#ifdef GSP_USE_TRIPLE_BUFFERRING
	if (ulAnd & FLUSH_DMA1)
		GSP_ArtisanalFlushAll();
#endif	
	
	if (gsulSyncroPath & FLUSH_WORLDLOAD)
		if (!(gsulSyncroPath & GIF_LOCKED))		
			gsulSyncroPath = gsulSyncroPath;
	
	MEror = 30000000;
	ulAnd &= ~GIF_LOCKED;
	if (gsulSyncroPath & FLUSH_DMA1) 
		GspGlobal_ACCESS(DMA_VS_CPU)++;
	else 
		GspGlobal_ACCESS(CPU_VS_DMA)++;

	while ((gsulSyncroPath & ulAnd) && MEror){MEror--;}; 
	ulAnd &= ~(FLUSH_DMA1|FLUSH_DMA2|FLUSH_DMA9|FLUSH_DMA8);
	while (ulAnd && MEror)
	{
		ulAnd &= sceGsSyncPath(1, 0);
		MEror--;
	}

	if (!MEror)
	{
		extern void GSP_RestoreCorrectPath(u_int Mode);
		ulAnd = sceGsSyncPath(1, 0);
		sceGsSyncPath(0, 0);
		GSP_RestoreCorrectPath(1);
	} 
}

int Gsp_DMA2End(int)
{
	gsulSyncroPath &= ~FLUSH_DMA2;
   	ExitHandler();
   	return 0;
}

/* TO SPR */
int Gsp_DMA9End(int)
{
	gsulSyncroPath &= ~FLUSH_DMA9;
   	ExitHandler();
   	return 0;
}

/* FROM SPR */
int Gsp_DMA8End(int)
{
	gsulSyncroPath &= ~FLUSH_DMA8;
   	ExitHandler();
   	return 0;
}


static Gsp_DmaTag   stDmaHeader1[4] __attribute__((aligned(64)));
void Gsp_SendToVif1(u_int Address , u_int Size)
{
	/* End of VIF packet aligned with 128b MUST BE SET TO 0 ( VIF_NOP ) !!! */
	Gsp_DmaTag   *p_DmaHeader;
	GSP_FlushAllFloatingFlip();
	Gsp_Flush(FLUSH_DMA1);
	p_DmaHeader = stDmaHeader1;
	GSP_UnCachePtr(p_DmaHeader);
	p_DmaHeader[0].ui32[0] = 0x00000000 | (Size >> 0L); 	// DMA REFe
	p_DmaHeader[0].ui32[1] = GSP_DMAFormatAddress(Address);			// Source data
	p_DmaHeader[1].ul64 = 0L;
	p_DmaHeader[2].ul64 = 0L;
	p_DmaHeader[3].ul64 = 0L;
	Gsp_SendToDma1(GSP_DMAFormatAddress((u_int)p_DmaHeader), DX_CHCR);
}

static Gsp_DmaTag   stDmaHeader2[4] __attribute__((aligned(64)));
void Gsp_SendToGif(u_int Address , u_int Size)
{
	Gsp_DmaTag   *p_DmaHeader;
	/* End of VIF packet aligned with 128b MUST BE SET TO 0 ( VIF_NOP ) !!! */
//	GSP_FlushAllFloatingFlip();
	Gsp_Flush(FLUSH_DMA2);
	p_DmaHeader = stDmaHeader2;
	GSP_UnCachePtr(p_DmaHeader);
	p_DmaHeader[0].ui32[0] = 0x00000000 | (Size >> 0L); 	// DMA REFe
	p_DmaHeader[0].ui32[1] = GSP_DMAFormatAddress(Address);	// Source data
	p_DmaHeader[1].ul64 = 0L;
	p_DmaHeader[2].ul64 = 0L;
	p_DmaHeader[3].ul64 = 0L;
	Gsp_SendToDma2(GSP_DMAFormatAddress((u_int)p_DmaHeader), DX_CHCR_NODTG);
}

