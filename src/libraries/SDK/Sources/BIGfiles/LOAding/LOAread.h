// LOAread.h 
// Common declarations for cross platform loading

#ifndef PSX2_TARGET
#pragma once
#endif // #ifndef PSX2_TARGET

#ifndef __LOAread_h__
#define __LOAread_h__

#include "BASe/BAStypes.h"
#include "BASe/BASsys.h"

#include "MATHs\MATHstruct.h"
#include "AIinterp/Sources/AIstruct.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif // #ifdef __cplusplus

extern BOOL LOA_gb_SpeedMode;
extern int BIG_gi_ReadMode;

#ifdef ACTIVE_EDITORS
	#define LOA_BINARIZATION
#endif // #ifdef ACTIVE_EDITORS

// The LOA_ALIGNED_LOAD define activates aligned loading of values
// mostly used for PSX2, it avoids making unaligned access to 32 bits values.
//#ifdef PSX2_TARGET
#if defined(_XENON) || defined(PSX2_TARGET)
	#define LOA_ALIGNED_LOAD
#endif // #ifdef PSX2_TARGET

// Loading types depending on engine state.
//
// Engine Binarized
// - Normal Load (PC, final GC)
// - Swapped Load (dev GC)
// - Aligned Load (PS2)
//
// Engine Non Binarized
// - Normal Load (PC)
// - Swapped Load (dev GC)
// - Aligned Load (PS2)
// - Editor only data Skipped
//
// Editor Binarizing
// - Normal Load ==> Load & Save
// - Editor Load ==> Load & No Save
//
// Editor Normal
// - Normal Load ==> Load
// - Editor Load ==> Load




// M_OPT_LOAD_MEMCPY is used to load directly from the buffer by a memcpy
// it is only used for versions reading binary data (i.e. not binarizing or swapping)
// it cannot be used it data needs to be aligned
// Call this macro as the first thing in your callback, if possible, it will
// memcpy from the buffer and return, it is not possible, nothing is done and the
// code below the macro is executed.
// DO NOT USE This macro if the code after it must always be executed
//
// Defining FORCE_OPTIMIZED_LOAD forces the loading through memcpy, used only
// for final versions using binary data known not to require swapping or alignment.

// #define FORCE_OPTIMIZED_LOAD

#if defined(LOA_ALIGNED_LOAD) || !defined(LOA_BINARIZATION)

#define M_OPT_LOAD_MEMCPY(SourceBuffer, TargetBuffer, Type)

#else // ! (defined(LOA_ALIGNED_LOAD) || !defined(LOA_BINARIZATION))

#if defined(FORCE_OPTIMIZED_LOAD)
#define M_OPT_LOAD_MEMCPY(SourceBuffer, TargetBuffer, Type)\
	do{\
		if(TargetBuffer != NULL)\
		{\
			L_memcpy((TargetBuffer), (SourceBuffer), sizeof(Type));\
		}\
		(SourceBuffer) += sizeof(Type);\
		return;\
	}\
	while(0)

#else // ! FORCE_OPTIMIZED_LOAD

#define M_OPT_LOAD_MEMCPY(SourceBuffer, TargetBuffer, Type)\
	if(LOA_IsBinaryData() && !LOA_IsBinarizing() && !LOA_IsSwapperActive())\
	do{\
		if(TargetBuffer != NULL)\
		{\
			L_memcpy((TargetBuffer), (SourceBuffer), sizeof(Type));\
		}\
		(SourceBuffer) += sizeof(Type);\
		return;\
	}\
	while(0)

#endif // #if !defined(FORCE_OPTIMIZED_LOAD)

#endif // #if defined(LOA_ALIGNED_LOAD) || !defined(LOA_BINARIZATION)

#define SKIP_IF_NULL_DATA(SourceBuffer, Data, SkipSize)\
	if(Data == NULL)\
	{\
		if(!((_eBinFlags & LOA_eBinEditorData) && LOA_IsBinaryData())) /* don't skip if its editor data with binary data because it is not on disk */\
			((CHAR*)SourceBuffer) += SkipSize;\
		return;\
	}

#define ubiGetFieldOffset(Structure, Field) ((ULONG) (&((Structure *) 0)->Field))

typedef enum LOA_eBinFlags_
{
LOA_eBinEngineData = 1,
LOA_eBinEditorData = 2,
// Look ahead data is not written to the binary file (i.e. like Editor Data Flag) 
// it is used to gather information on the coming data, but the data will still
// be processed afterwards
LOA_eBinLookAheadData = 4,
LOA_eBinFileInfo = 8, // Information about the file size, etc.
LOA_eBinFLAGSSIZE
} LOA_eBinFlags;

typedef struct LOA_tdstBinFileHeader_
{
	ULONG ul_Size;
} LOA_tdstBinFileHeader;


#ifdef LOA_BINARIZATION
extern BOOL LOA_gb_Binarizing;
extern BOOL LOA_gb_SwapOnWrite;

void LOA_InitBinaryBuffer(LONG _lBufferSize, LONG _lGranularity);
void LOA_ClearBinaryBuffer();
CHAR * LOA_GetBinaryBuffer();
CHAR * LOA_GetCurBinaryBuffer();
void LOA_SetSwapOnWrite(BOOL _bSwapOnWrite);

_inline_ BOOL LOA_IsBinarizing()
{
	return LOA_gb_SpeedMode && (BIG_gi_ReadMode == 1);
}

_inline_ BOOL LOA_SwapOnWrite()
{
	return LOA_gb_SwapOnWrite;
}


#endif LOA_BINARIZATION

extern BOOL LOA_gb_SwapperActive;
extern BOOL LOA_gb_BinaryData;

void LOA_SetIsSwapperActive(BOOL _bSwapperActive);

#if defined(_XENON) || defined(_GAMECUBE) || defined(ACTIVE_EDITORS)
_inline_ BOOL LOA_IsSwapperActive(void)
{
	return LOA_gb_SwapperActive;
}
#else
#define LOA_IsSwapperActive() (0)
#endif

_inline_ BOOL LOA_IsBinaryData(void)
{
	return LOA_gb_SpeedMode && (BIG_gi_ReadMode == 2);
}

CHAR * LOA_FetchBuffer(ULONG _ul_Length);


#define LOA_ReadLong(Buffer)		_LOA_ReadLong(Buffer,	NULL, LOA_eBinEngineData)
#define LOA_ReadShort(Buffer)		_LOA_ReadShort(Buffer,	NULL, LOA_eBinEngineData)
#define LOA_ReadChar(Buffer)		_LOA_ReadChar(Buffer,	NULL, LOA_eBinEngineData)
#define LOA_ReadFloat(Buffer)		_LOA_ReadFloat(Buffer,	NULL, LOA_eBinEngineData)
#define LOA_ReadInt(Buffer)			_LOA_ReadInt(Buffer,		NULL, LOA_eBinEngineData)
#define LOA_ReadUInt(Buffer)		_LOA_ReadUInt(Buffer,		NULL, LOA_eBinEngineData)
#define LOA_ReadUChar(Buffer)		_LOA_ReadUChar(Buffer,	NULL, LOA_eBinEngineData)
#define LOA_ReadUShort(Buffer)		_LOA_ReadUShort(Buffer,		NULL, LOA_eBinEngineData)
#define LOA_ReadULong(Buffer)		_LOA_ReadULong(Buffer,	NULL, LOA_eBinEngineData)

#define LOA_ReadCharArray(Buffer, DestArray, ArraySize)	_LOA_ReadCharArray(Buffer, DestArray, ArraySize, LOA_eBinEngineData)
#define LOA_ReadString(Buffer, String, StringLength)	_LOA_ReadString(Buffer, String, StringLength, LOA_eBinEngineData)
#define LOA_ReadVector(Buffer, Data)	_LOA_ReadVector(Buffer, Data, LOA_eBinEngineData)
#define LOA_ReadMatrix(Buffer, Data)	_LOA_ReadMatrix(Buffer, Data, LOA_eBinEngineData)
#define LOA_ReadQuaternion(Buffer, Data)	_LOA_ReadQuaternion(Buffer, Data, LOA_eBinEngineData)
#define LOA_ReadCompressedQuaternion(Buffer, Data)		_LOA_ReadCompressedQuaternion(Buffer, Data, LOA_eBinEngineData)
#define LOA_ReadUltraCompressedQuaternion(Buffer, Data) _LOA_ReadUltraCompressedQuaternion(Buffer, Data, LOA_eBinEngineData)
#define LOA_ReadCompressedVector(Buffer, Data)			_LOA_ReadCompressedVector(Buffer, Data, LOA_eBinEngineData)
#define LOA_ReadUltraCompressedVector(Buffer, Data)		_LOA_ReadUltraCompressedVector(Buffer, Data, LOA_eBinEngineData)

#define LOA_ReadAINode(Buffer, Data)		_LOA_ReadAINode(Buffer, Data, LOA_eBinEngineData)
#define LOA_ReadAILocal(Buffer, Data)		_LOA_ReadAILocal(Buffer, Data, LOA_eBinEngineData)

#define LOA_ReadSpriteMapper(Buffer, Data)		_LOA_ReadSpriteMapper(Buffer, Data, LOA_eBinEngineData)

#ifdef FORCE_OPTIMIZED_LOAD

// FORCE_OPTIMIZED_LOAD is defined for final builds with 
// binary data that does not contain Editor data, thus 
// All load request for editor data are ignored in this mode.

#define LOA_ReadLong_Ed(Buffer, Data)	0
#define LOA_ReadShort_Ed(Buffer, Data)	0
#define LOA_ReadChar_Ed(Buffer, Data)	0
#define LOA_ReadFloat_Ed(Buffer, Data)	0
#define LOA_ReadInt_Ed(Buffer, Data)	0
#define LOA_ReadUInt_Ed(Buffer, Data)	0
#define LOA_ReadUChar_Ed(Buffer, Data)	0
#define LOA_ReadUShort_Ed(Buffer, Data) 0
#define LOA_ReadULong_Ed(Buffer, Data)	0
#define LOA_ReadCharArray_Ed(Buffer, DestArray, ArraySize)
#define LOA_ReadString_Ed(Buffer, String, StringLength)

#define LOA_ReadVector_Ed(Buffer, Data)
#define LOA_ReadMatrix_Ed(Buffer, Data)	
#define LOA_ReadQuaternion_Ed(Buffer, Data)
#define LOA_ReadCompressedQuaternion_Ed(Buffer, Data)
#define LOA_ReadUltraCompressedQuaternion_Ed(Buffer, Data)
#define LOA_ReadCompressedVector_Ed(Buffer, Data)
#define LOA_ReadUltraCompressedVector_Ed(Buffer, Data)

#define LOA_ReadAINode_Ed(Buffer, Data)
#define LOA_ReadAILocal_Ed(Buffer, Data)

#define LOA_ReadSpriteMapper_Ed(Buffer, Data)


#else // ! FORCE_OPTIMIZED_LOAD

#ifdef ACTIVE_EDITORS

#define LOA_ReadLong_Ed(Buffer, Data)	_LOA_ReadLong(Buffer,	Data, LOA_eBinEditorData)
#define LOA_ReadShort_Ed(Buffer, Data)	_LOA_ReadShort(Buffer,	Data, LOA_eBinEditorData)
#define LOA_ReadChar_Ed(Buffer, Data)	_LOA_ReadChar(Buffer,	Data, LOA_eBinEditorData)
#define LOA_ReadFloat_Ed(Buffer, Data)	_LOA_ReadFloat(Buffer,	Data, LOA_eBinEditorData)
#define LOA_ReadInt_Ed(Buffer, Data)	_LOA_ReadInt(Buffer,	Data, LOA_eBinEditorData)
#define LOA_ReadUInt_Ed(Buffer, Data)	_LOA_ReadUInt(Buffer,	Data, LOA_eBinEditorData)
#define LOA_ReadUChar_Ed(Buffer, Data)	_LOA_ReadUChar(Buffer,	Data, LOA_eBinEditorData)
#define LOA_ReadUShort_Ed(Buffer, Data)	_LOA_ReadUShort(Buffer,	Data, LOA_eBinEditorData)
#define LOA_ReadULong_Ed(Buffer, Data)	_LOA_ReadULong(Buffer,	Data, LOA_eBinEditorData)

#define LOA_ReadCharArray_Ed(Buffer, DestArray, ArraySize)	_LOA_ReadCharArray(Buffer, DestArray, ArraySize, LOA_eBinEditorData)
#define LOA_ReadString_Ed(Buffer, String, StringLength)	_LOA_ReadString(Buffer, String, StringLength, LOA_eBinEditorData)
#define LOA_ReadVector_Ed(Buffer, Data)	_LOA_ReadVector(Buffer, Data, LOA_eBinEditorData)
#define LOA_ReadMatrix_Ed(Buffer, Data)	_LOA_ReadMatrix(Buffer, Data, LOA_eBinEditorData)
#define LOA_ReadQuaternion_Ed(Buffer, Data)	_LOA_ReadQuaternion(Buffer, Data, LOA_eBinEditorData)
#define LOA_ReadCompressedQuaternion_Ed(Buffer, Data)		_LOA_ReadCompressedQuaternion(Buffer, Data, LOA_eBinEditorData)
#define LOA_ReadUltraCompressedQuaternion_Ed(Buffer, Data)	_LOA_ReadUltraCompressedQuaternion(Buffer, Data, LOA_eBinEditorData)
#define LOA_ReadCompressedVector_Ed(Buffer, Data)			_LOA_ReadCompressedVector(Buffer, Data, LOA_eBinEditorData)
#define LOA_ReadUltraCompressedVector_Ed(Buffer, Data)		_LOA_ReadUltraCompressedVector(Buffer, Data, LOA_eBinEditorData)

#define LOA_ReadAINode_Ed(Buffer, Data)		_LOA_ReadAINode(Buffer, Data, LOA_eBinEditorData)
#define LOA_ReadAILocal_Ed(Buffer, Data)	_LOA_ReadAILocal(Buffer, Data, LOA_eBinEditorData)

#define LOA_ReadSpriteMapper_Ed(Buffer, Data)	_LOA_ReadSpriteMapper(Buffer, Data, LOA_eBinEditorData)

#else

// When not in editor, the variable is not used so it does not need to exist in a non editor build

#define LOA_ReadLong_Ed(Buffer, Data)	_LOA_ReadLong(Buffer,	NULL, LOA_eBinEditorData)
#define LOA_ReadShort_Ed(Buffer, Data)	_LOA_ReadShort(Buffer,	NULL, LOA_eBinEditorData)
#define LOA_ReadChar_Ed(Buffer, Data)	_LOA_ReadChar(Buffer,	NULL, LOA_eBinEditorData)
#define LOA_ReadFloat_Ed(Buffer, Data)	_LOA_ReadFloat(Buffer,	NULL, LOA_eBinEditorData)
#define LOA_ReadInt_Ed(Buffer, Data)	_LOA_ReadInt(Buffer,	NULL, LOA_eBinEditorData)
#define LOA_ReadUInt_Ed(Buffer, Data)	_LOA_ReadUInt(Buffer,	NULL, LOA_eBinEditorData)
#define LOA_ReadUChar_Ed(Buffer, Data)	_LOA_ReadUChar(Buffer,	NULL, LOA_eBinEditorData)
#define LOA_ReadUShort_Ed(Buffer, Data)	_LOA_ReadUShort(Buffer,	NULL, LOA_eBinEditorData)
#define LOA_ReadULong_Ed(Buffer, Data)	_LOA_ReadULong(Buffer,	NULL, LOA_eBinEditorData)

#define LOA_ReadCharArray_Ed(Buffer, DestArray, ArraySize)	_LOA_ReadCharArray(Buffer, NULL, ArraySize, LOA_eBinEditorData)
#define LOA_ReadString_Ed(Buffer, String, StringLength)	_LOA_ReadString(Buffer, NULL, StringLength, LOA_eBinEditorData)
#define LOA_ReadVector_Ed(Buffer, Data)	_LOA_ReadVector(Buffer, NULL, LOA_eBinEditorData)
#define LOA_ReadMatrix_Ed(Buffer, Data)	_LOA_ReadMatrix(Buffer, NULL, LOA_eBinEditorData)
#define LOA_ReadQuaternion_Ed(Buffer, Data)	_LOA_ReadQuaternion(Buffer, NULL, LOA_eBinEditorData)
#define LOA_ReadCompressedQuaternion_Ed(Buffer, Data)		_LOA_ReadCompressedQuaternion(Buffer, NULL, LOA_eBinEditorData)
#define LOA_ReadUltraCompressedQuaternion_Ed(Buffer, Data)	_LOA_ReadUltraCompressedQuaternion(Buffer, NULL, LOA_eBinEditorData)
#define LOA_ReadCompressedVector_Ed(Buffer, Data)			_LOA_ReadCompressedVector(Buffer, NULL, LOA_eBinEditorData)
#define LOA_ReadUltraCompressedVector_Ed(Buffer, Data)		_LOA_ReadUltraCompressedVector(Buffer, NULL, LOA_eBinEditorData)

#define LOA_ReadAINode_Ed(Buffer, Data)		_LOA_ReadAINode(Buffer, NULL, LOA_eBinEditorData)
#define LOA_ReadAILocal_Ed(Buffer, Data)	_LOA_ReadAILocal(Buffer, NULL, LOA_eBinEditorData)

#define LOA_ReadSpriteMapper_Ed(Buffer, Data)	_LOA_ReadSpriteMapper(Buffer, NULL, LOA_eBinEditorData)

#endif // #ifdef ACTIVE_EDITORS

#endif // #ifdef FORCE_OPTIMIZED_LOAD





LONG	_LOA_ReadLong(CHAR ** _ppcBuffer, LONG * _Data, LOA_eBinFlags _eBinFlags);
SHORT	_LOA_ReadShort(CHAR ** _ppcBuffer, SHORT * _Data, LOA_eBinFlags _eBinFlags);
CHAR	_LOA_ReadChar(CHAR ** _ppcBuffer, CHAR * _Data, LOA_eBinFlags _eBinFlags);
float	_LOA_ReadFloat(CHAR ** _ppcBuffer, float * _Data, LOA_eBinFlags _eBinFlags);


_inline_ INT _LOA_ReadInt(CHAR ** _ppcBuffer, INT * _Data, LOA_eBinFlags _eBinFlags)
{
	return (INT) _LOA_ReadLong(_ppcBuffer, (LONG*) _Data, _eBinFlags);
}

_inline_ UINT _LOA_ReadUInt(CHAR ** _ppcBuffer, UINT * _Data, LOA_eBinFlags _eBinFlags)
{
	return (UINT) _LOA_ReadLong(_ppcBuffer, (LONG*) _Data, _eBinFlags);
}

_inline_ UCHAR _LOA_ReadUChar(CHAR ** _ppcBuffer, UCHAR * _Data, LOA_eBinFlags _eBinFlags)
{
	return (UCHAR) _LOA_ReadChar(_ppcBuffer, (CHAR*) _Data, _eBinFlags);
}

_inline_ USHORT _LOA_ReadUShort(CHAR ** _ppcBuffer, USHORT * _Data, LOA_eBinFlags _eBinFlags)
{
	return (USHORT) _LOA_ReadShort(_ppcBuffer, (SHORT*) _Data, _eBinFlags);
}

_inline_ ULONG _LOA_ReadULong(CHAR ** _ppcBuffer, ULONG * _Data, LOA_eBinFlags _eBinFlags)
{
	return (ULONG) _LOA_ReadLong(_ppcBuffer, (LONG*) _Data, _eBinFlags);
}

void _LOA_ReadBinFileHeader(CHAR ** _ppcBuffer, LOA_tdstBinFileHeader * _Data, LOA_eBinFlags _eBinFlags);
void _LOA_ReadCharArray(CHAR ** _ppcBuffer, CHAR * _pcDestArray, LONG _lArraySize, LOA_eBinFlags _eBinFlags);
void _LOA_ReadString(CHAR ** _ppcBuffer, CHAR * _szString, LONG _lStringLength, LOA_eBinFlags _eBinFlags);
void _LOA_ReadVector(CHAR ** _ppcBuffer, MATH_tdst_Vector * _Data, LOA_eBinFlags _eBinFlags);
void _LOA_ReadMatrix(CHAR ** _ppcBuffer, MATH_tdst_Matrix * _Data, LOA_eBinFlags _eBinFlags);
void _LOA_ReadQuaternion(CHAR ** _ppcBuffer, MATH_tdst_Quaternion * _Data, LOA_eBinFlags _eBinFlags);
void _LOA_ReadCompressedQuaternion(CHAR ** _ppcBuffer, MATH_tdst_CompressedQuaternion * _Data, LOA_eBinFlags _eBinFlags);
void _LOA_ReadUltraCompressedQuaternion(CHAR ** _ppcBuffer, MATH_tdst_UltraCompressedQuaternion * _Data, LOA_eBinFlags _eBinFlags);
void _LOA_ReadCompressedVector(CHAR ** _ppcBuffer, MATH_tdst_CompressedVector * _Data, LOA_eBinFlags _eBinFlags);
void _LOA_ReadUltraCompressedVector(CHAR ** _ppcBuffer, MATH_tdst_UltraCompressedVector * _Data, LOA_eBinFlags _eBinFlags);
void _LOA_ReadAINode(CHAR ** _ppcBuffer, AI_tdst_Node * _Data, LOA_eBinFlags _eBinFlags);
void _LOA_ReadAILocal(CHAR ** _ppcBuffer, AI_tdst_Local * _Data, LOA_eBinFlags _eBinFlags);

#ifdef LOA_ALIGNED_LOAD

#ifdef PSX2_TARGET
_inline_ LONG ReadAlignedLong(register CHAR *addr)
{
	register LONG	tmp;

	asm	__volatile__ ("
		lwr	tmp, 0(addr)
		lwl tmp, 3(addr)
		");

	return tmp;
}
#else

_inline_ LONG ReadAlignedLong(CHAR *_from)
{
	//~~~~~~~~~~~
	LONG	lLong;
	//~~~~~~~~~~~
	((CHAR *) &lLong)[0] = _from[0];
	((CHAR *) &lLong)[1] = _from[1];
	((CHAR *) &lLong)[2] = _from[2];
	((CHAR *) &lLong)[3] = _from[3];
	return lLong;
}

#endif // #ifdef PSX2_TARGET

_inline_ float ReadAlignedFloat(CHAR *_from)
{
	//~~~~~~~~~~~
	float	ffloat;
	//~~~~~~~~~~~
	((CHAR *) &ffloat)[0] = _from[0];
	((CHAR *) &ffloat)[1] = _from[1];
	((CHAR *) &ffloat)[2] = _from[2];
	((CHAR *) &ffloat)[3] = _from[3];
	return ffloat;
}

_inline_ SHORT ReadAlignedShort(CHAR *_from)
{
	//~~~~~~~~~~~
	SHORT lshort;
	//~~~~~~~~~~~
	((CHAR *) &lshort)[0] = _from[0];
	((CHAR *) &lshort)[1] = _from[1];
	return lshort;
}

#endif // #ifdef LOA_ALIGNED_LOAD


_inline_ void SwapByte(CHAR * x, CHAR * y)
{
    CHAR a = *x;
    *x = *y;
    *y = a;
}

/*_inline_ void SwapWord(SHORT * x)
{
    //CHAR * data = (CHAR *) x;
    //SwapByte(data, data+1);
    *x = ((*x&0x00ff)<<8) | ((*x&0xff00)>>8);
}*/

#define SwapWord(x)  (*x = ((*x&0x00ff)<<8) | ((*x&0xff00)>>8))
#define SwapDWord(x) (*x = ((*x&0x000000ff)<<24) | ((*x&0x0000ff00)<<8) | ((*x&0x00ff0000)>>8) | ((*x&0xff000000)>>24))

/*_inline_ void SwapDWord(LONG * x)
{
   /* CHAR * data = (CHAR *) x;
    SwapByte(data, data+3);
    SwapByte(data+1, data+2);
    */
  // *x = ((*x&0x000000ff)<<24) | ((*x&0x0000ff00)<<8) | ((*x&0x00ff0000)>>8) | ((*x&0xff000000)>>24);
//
//}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif // #ifdef __cplusplus

#endif // #ifndef __LOAread_h__

