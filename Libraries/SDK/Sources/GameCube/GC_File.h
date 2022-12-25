/*$T GC_File.h GC! 1.097 01/11/02 16:05:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE
#ifndef __GC_File_h__
#define __GC_File_h__
#ifdef __cplusplus
extern "C"
{
#endif
#include <dolphin.h>

#define CDVD_BUFFER_SIZE	(16 * 1024)

typedef struct	GC_file
{
	DVDFileInfo stFileInfo; /* seulement un fichier peut etre ouvert a la fois. (pas suppose d'en avoir plus sur Jade
							 * * GameCube) */
	u32			iCurrentFilePos;
	u32			iFileSize;
	u32			u32ReadBufferStart;
	u32			u32ReadBufferEnd;
	u8			*pReadBuffer;
	s32			iFileEntryNum;
} tdstGC_File;

extern tdstGC_File	*GC_fOpen(u8 *_filename, u8 *_mode);
extern u32			GC_fClose(tdstGC_File *);
extern u32			GC_fRead(tdstGC_File *, u8 *_buffer, u32 _size);
extern u32			GC_fSeek(tdstGC_File *, u32 _pos, u8 _mode);
extern u32			GC_fTell(tdstGC_File *);

#ifdef __cplusplus
}
#endif
#endif /* __GC_File_h__ */
#endif /* _GAMECUBE */
