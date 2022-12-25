/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __CLIFILE_H__
#define __CLIFILE_H__



#if (!defined PSX2_TARGET && !defined _GAMECUBE && !defined _XBOX && !defined _XENON)

#include "direct.h"
#include "io.h"
#include "sys/locking.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/stat.h"

typedef FILE *                  L_FILE;
#define CLI_FileOpen(__h_FILE)	(__h_FILE != NULL)
#define L_MAX_PATH              _MAX_PATH
#define L_fileno                _fileno
#define L_errno                 errno
#define L_freadA(a, b, c, d)    (_read(_fileno(d), a, b * c) / b * c)
#define L_fread                 fread
#define L_fwriteA(a, b, c, d)   (_write(_fileno(d), a, b * c) / b * c)
#define L_fwrite                fwrite
#define L_fputs                 fputs
#define L_fputc                 fputc
#define L_fgetc                 fgetc
#define L_fgets                 fgets
#define L_setvbuf               setvbuf
#define L_IONBF                 _IONBF
#define L_feof                  feof
#define L_ftell                 BIG_ftell
#define L_fseek                 BIG_fseek
#define L_SEEK_SET              SEEK_SET
#define L_SEEK_CUR              SEEK_CUR
#define L_SEEK_END              SEEK_END
#define L_rewind                rewind
#define L_fopen                 fopen
#define L_fopen_AB              "ab"
#define L_fopen_WB              "wb"
#define L_fopen_RB              "rb"
#define L_fopen_RPB             "r+b"
#define L_fclose                fclose
#define L_unlink                _unlink
#define L_rename                rename
#define L_access                _access
#define L_chmod                 _chmod
#define L_S_IREAD               _S_IREAD
#define L_S_IWRITE              _S_IWRITE
#define L_finddata_t            _finddata_t
#define L_A_SUBDIR              _A_SUBDIR
#define L_findfirst             _findfirst
#define L_findnext              _findnext
#define L_findclose             _findclose
#define L_mkdir                 _mkdir
#define L_fullpath              _fullpath
#define L_LK_NBLCK              LK_NBLCK
#define L_LK_UNLCK              LK_UNLCK
#define L_locking               _locking



#else 
/*$F---------------------------------------------------------------------

								PSX2
								
-----------------------------------------------------------------------*/

#ifdef PSX2_TARGET
#include "stdio.h"
#include "stdlib.h"
#include "sys/stat.h"
#include "sifdev.h"


#define L_MAX_PATH              FILENAME_MAX

/* file handler */
typedef int                      L_FILE;		

/* fclose and fopen */
#define L_fopen                 sceOpen
#define L_fclose                sceClose
#define L_fopen_AB              SCE_WRONLY
#define L_fopen_WB              SCE_WRONLY
#define L_fopen_RB              SCE_RDONLY
#define L_fopen_RPB             SCE_RDONLY

/* validity of file handler */
#define CLI_FileOpen(__h_FILE)	(__h_FILE >= 0)		

/* fseek */
#define L_fseek                 BIG_fseek
#define L_SEEK_SET              SCE_SEEK_SET
#define L_SEEK_CUR              SCE_SEEK_CUR
#define L_SEEK_END              SCE_SEEK_END

/* fread */
/* !! the return value is 1 if all data are read else 0 */
/* it is different from C library fread */
#define L_fread(a, b, c, d)     (sceRead( d, a, b * c) / b * c)
#define L_freadA(a, b, c, d)    (sceRead( d, a, b * c) / b * c)

/* fwrite */
#define L_fwriteA(a, b, c, d)   (sceWrite( d, a, b * c) / b * c)
#define L_fwrite(a, b, c, d)    (sceWrite( d, a, b * c) / b * c)


/* other functions, not implemented */
#define L_setvbuf(a, b, c, d)   /* there is no equivalent */
#define L_IONBF                 /* there is no equivalent */
#define L_access(a,b)           (1)  /* not yet implemented by SCE */
#define L_unlink(a)             /* there is no equivalent */
#define L_chmod(a, b)           /* there is no equivalent */

#define L_S_IREAD               SCE_RDONLY
#define L_S_IWRITE              SCE_WRONLY

#endif /* ifndef PSX2 */

/*$F---------------------------------------------------------------------

								GAMECUBE
								
-----------------------------------------------------------------------*/
#ifdef _GAMECUBE

#include <stdio.h>
#include <stdlib.h>
#include "GameCube/GC_File.h"


#define L_MAX_PATH              FILENAME_MAX

typedef DVDFileInfo *           L_FILE;

/* fclose and fopen */
#define L_fopen                 GC_fOpen
#define L_fclose                GC_fClose

#define L_feof                  (0)/* not needed */
#define L_ftell                 GC_fTell/* not needed */
#define L_unlink(a)             (0)/* no equivalent */

/* validity of file handler */
#define CLI_FileOpen(__h_FILE)	(__h_FILE != NULL)

/* fseek */
#define L_fseek                 GC_fSeek
#define L_SEEK_SET              SEEK_SET
#define L_SEEK_CUR              SEEK_CUR
#define L_SEEK_END              SEEK_END

#define L_fread(a,b,c,d)		(GC_fRead(d, a, b*c) / b*c)
#define L_fwrite(a,b,c,d)		(0)/* pas de write sur gamecube */
#define L_freadA(a,b,c,d)		(GC_fRead(d, a, b*c) / b*c)
#define L_fwriteA(a,b,c,d)		(0)/* pas de write sur gamecube */
#define L_fopen_AB              "ab"
#define L_fopen_WB              "wb"
#define L_fopen_RB              "rb"
#define L_fopen_RPB             "r+b"


#define L_access(a, b)          (1) /* there is no equivalent */

#define L_chmod(a, b)           (1)/* there is no equivalent */


#define L_S_IREAD               _S_IREAD
#define L_S_IWRITE              _S_IWRITE

#endif


/*$F---------------------------------------------------------------------

								XBOX / XENON
								
-----------------------------------------------------------------------*/
#if defined(_XBOX) || defined(_XENON)

#include "direct.h"
#include "io.h"
#include "sys/locking.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/stat.h"
#if !defined(_XENON)
#include <xtl.h>
#endif

#define L_MAX_PATH              MAX_PATH

typedef HANDLE					L_FILE;

extern int XB_fWrite(void *_p_Buffer, int _i_Size, L_FILE _h_Handle);
extern int XB_fRead(void *_p_Buffer, int _i_Size, L_FILE _h_Handle);

/* fclose and fopen */
#define L_fopen(f,m)			CreateFile(f, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
#define L_fclose                CloseHandle

#define L_feof                  (0)/* not needed */
#define L_unlink(a)             (0)/* no equivalent */

#define L_ftell					(0)

/* validity of file handler */
#define CLI_FileOpen(__h_FILE)	(__h_FILE != INVALID_HANDLE_VALUE)

/* fseek */
#define L_SEEK_SET              FILE_BEGIN
#define L_SEEK_CUR              FILE_CURRENT
#define L_SEEK_END              FILE_END
#define L_SEEK_ERROR			INVALID_SET_FILE_POINTER

#define L_fseek					BIG_fseek

#define L_fread(buffer, n, size, file)		XB_fRead(buffer, size*n, file)
#define L_freadA(buffer, size, n, file)		XB_fRead(buffer, size*n, file)
#define L_fwrite(buffer, size, n, file)		XB_fWrite(buffer, size*n, file)
#define L_fwriteA				L_fwrite

#define L_fopen_AB              "ab"
#define L_fopen_WB              "wb"
#define L_fopen_RB              "rb"
#define L_fopen_RPB             "r+b"


#define L_access(a, b)          (1) /* there is no equivalent */

#define L_chmod(a, b)           (1) /* there is no equivalent */


#define L_S_IREAD               _S_IREAD
#define L_S_IWRITE              _S_IWRITE

#endif // _XBOX


#endif /* if (!defined PSX2 && !defined _GAMECUBE && !defined _XBOX) */
#endif /* __CLIFILE_H__ */