/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#if defined( _WIN32 )
#	include "direct.h"
#	include "io.h"
#	include "sys/locking.h"
#else
#	include <limits.h>
#endif
#include "stdio.h"
#include "stdlib.h"
#include "sys/stat.h"

typedef FILE *L_FILE;
#define CLI_FileOpen( __h_FILE ) ( __h_FILE != NULL )
#if defined( _WIN32 )
#	define L_MAX_PATH _MAX_PATH
#else
#	define L_MAX_PATH PATH_MAX
#endif
#define L_fileno                _fileno
#define L_errno                 errno
#define L_freadA( a, b, c, d )  ( _read( _fileno( d ), a, b * c ) / b * c )
#define L_fread                 fread
#define L_fwriteA( a, b, c, d ) ( _write( _fileno( d ), a, b * c ) / b * c )
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
