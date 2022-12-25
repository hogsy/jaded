#include <libgraph.h>
#include "Gsp.h"
#include "BASe/MEMory/MEM.h"

#define GSP_REALTIMELOADER
#define GSP_RAM_MEMORY			1024*1024*4



ULONG GSP_GetTextureMemoryAvailable()
{
	ULONG returnValue;
	ULONG Rest;
#ifdef GSP_USE_TEXTURE_CACHE
	if (GspGlobal_ACCESS(ulAvailableTextureCacheSize))
		return GspGlobal_ACCESS(ulAvailableTextureCacheSize);
#endif
	Rest = 640 * 480 * 4 * 3;
	Rest = 1024 * 1024 * 4 - Rest;
	return (1024 * 1024 * 4 - (390/*GspGlobal_ACCESS(TBP) FORCE PAL SIZE*/ << 13)- 1024 * 32 * 6);
}


void GSP_SelectAndLoadTexture(ULONG TextureToSelect)
{
}
