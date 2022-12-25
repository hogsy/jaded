/*$T SNDwave.h GC 1.138 07/21/05 15:46:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef _SNDWAVE_H_
#define _SNDWAVE_H_

/*$4
 ***********************************************************************************************************************
    Headers
 ***********************************************************************************************************************
 */

#include "SouND/Sources/SNDstruct.h"
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define SND_Cte_WAVE_FORMAT_UNKNOWN		0x0000
#define SND_Cte_WAVE_FORMAT_PCM			0x0001
#define SND_Cte_WAVE_FORMAT_MS_ADPCM	0x0002
#define SND_Cte_WAVE_FORMAT_XBOX_ADPCM	0x0069
#define SND_Cte_WAVE_FORMAT_GAMECUBE	0xFFFE
#define SND_Cte_WAVE_FORMAT_PS2			0xFFFF

#if 1	/* little endian (intel) */
#define SND_M_MARKER(a, b, c, d)	((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))
#else /* big endian (motorola) */
#define SND_M_MARKER(a, b, c, d)	(((a) << 24) | ((b) << 16) | ((c) << 8) | (d))
#endif
/**/
#define SND_Cte_Marker_RIFF (SND_M_MARKER('R', 'I', 'F', 'F'))
#define SND_Cte_Marker_WAVE (SND_M_MARKER('W', 'A', 'V', 'E'))
#define SND_Cte_Marker_fmt	(SND_M_MARKER('f', 'm', 't', ' '))
#define SND_Cte_Marker_fact (SND_M_MARKER('f', 'a', 'c', 't'))
#define SND_Cte_Marker_data (SND_M_MARKER('d', 'a', 't', 'a'))
#define SND_Cte_Marker_cue	(SND_M_MARKER('c', 'u', 'e', ' '))
#define SND_Cte_Marker_plst (SND_M_MARKER('p', 'l', 's', 't'))
#define SND_Cte_Marker_labl (SND_M_MARKER('l', 'a', 'b', 'l'))
#define SND_Cte_Marker_slnt (SND_M_MARKER('s', 'l', 'n', 't'))
#define SND_Cte_Marker_note (SND_M_MARKER('n', 'o', 't', 'e'))
#define SND_Cte_Marker_smpl (SND_M_MARKER('s', 'm', 'p', 'l'))
#define SND_Cte_Marker_bext (SND_M_MARKER('b', 'e', 'x', 't'))
#define SND_Cte_Marker_MEXT (SND_M_MARKER('M', 'E', 'X', 'T'))
#define SND_Cte_Marker_DISP (SND_M_MARKER('D', 'I', 'S', 'P'))
#define SND_Cte_Marker_PAD	(SND_M_MARKER('P', 'A', 'D', ' '))
#define SND_Cte_Marker_JUNK (SND_M_MARKER('J', 'U', 'N', 'K'))
#define SND_Cte_Marker_ltxt (SND_M_MARKER('l', 't', 'x', 't'))
#define SND_Cte_Marker_file (SND_M_MARKER('f', 'i', 'l', 'e'))
/**/
#define SND_Cte_Marker_LIST (SND_M_MARKER('L', 'I', 'S', 'T'))
#define SND_Cte_Marker_adtl (SND_M_MARKER('a', 'd', 't', 'l'))
#define SND_Cte_Marker_INFO (SND_M_MARKER('I', 'N', 'F', 'O'))
#define SND_Cte_Marker_wavl (SND_M_MARKER('w', 'a', 'v', 'l'))

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

#if defined(PSX2_TARGET) || defined(_GAMECUBE)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef unsigned int	UI32, UL32;
typedef unsigned short	UI16, UL16;
typedef unsigned char	UI8, UL8;

/*$F
-----------------------------------------------------------------------------------------------------------------------
<WAVE-form> -> RIFF( 'WAVE'
 							<fmt-ck>			// Format
 							[<fact-ck>]			// Fact chunk
 							[<cue-ck>]			// Cue points
 							[<playlist-ck>]		// Playlist
 							[<assoc-data-list>] // Associated data list
 							<wave-data> )		// Wave data
	--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

<fact-ck>	-> fact( <dwFileSize:DWORD> )   // Number of samples, mandatory when using 'wavl' LIST or compressed data
											// this ck can be expanded in the future

    --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

<wave data> -> { <data-ck> | <data list> } 
    
<data-ck>	-> data( <samples> )
 
<data list> -> LIST( 'wavl' { <data-ck> | <silence-ck> } ... )
 
<silence-ck>-> slnt( dwSamples )

	--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

<cue-ck>	-> cue( <dwCuePoints:DWORD>  // nb of cue pt
						<cue-point>... ) // table of cue pt
<cue-point> -> struct 
					{
						DWORD dwName;			// unique cue name
						DWORD dwPosition;		// play order of the cue
						FOURCC fccChunk;		// ck ID of the chunk that contains the cue pt
						DWORD dwChunkStart;		// ck byte offset relative to the start of the data section of the ‘wavl’ LIST chunk.
						DWORD dwBlockStart;		// block byte offset relative to the start of the data section of the ‘wavl’ LIST chunk.
						DWORD dwSampleOffset;	// sample offset of the cue point relative to the start of the block.
					}
 
	--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

<playlist-ck> -> plst(
					<dwSegments:DWORD>	// Count of play segments
					<play-segment>...	// Play-segment table
					) 

<play-segment> -> struct 
					{
						DWORD dwName;	// name of cue pt
						DWORD dwLength; // section size in samples unit
						DWORD dwLoops;	// nb of times we play the section
					}

	--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

<assoc-data-list> -> LIST( 'adtl'
							<labl-ck> // Label
							<note-ck> // Note
							<ltxt-ck> // Text with data length
							<file-ck> ) // Media file

<labl-ck> -> labl( <dwName:DWORD>   // unique cue name
				   <data:ZSTR> )	// cue label/title

<note-ck> -> note( <dwName:DWORD>	// unique cue name
				   <data:ZSTR> )	// cue comment

<ltxt-ck> -> ltxt(	<dwName:DWORD>			// unique cue name
					<dwSampleLength:DWORD>	// size of the segment in sample unit
					<dwPurpose:DWORD>		// purpose of the text
					<wCountry:WORD>			//
					<wLanguage:WORD>		//
					<wDialect:WORD>			//
					<wCodePage:WORD>		//
					<data:BYTE>... )		// data...

<file-ck> -> file(	<dwName:DWORD>			// unique cue name
					<dwMedType:DWORD>		// file type
					<fileData:BYTE>...)		// file

	--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

<pad-ck> -> PAD( <filler> ) // padding

	--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

<junk-ck> -> JUNK( <filler> ) // padding

	--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

<instrument-ck> -> inst(<bUnshiftedNote:BYTE> // the MIDI note. Valid values range from 0 to 12
						<chFineTune:CHAR>	// the pitch shift adjustment in cents. -50 to +50
						<chGain:CHAR>		// the suggested volume setting
						<bLowNote:BYTE>		// the suggested usable MIDI note number range 0 to 127
						<bHighNote:BYTE>	// 
						<bLowVelocity:BYTE>	// the suggested usable MIDI velocity range 0 to 127
						<bHighVelocity:BYTE> )

	--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

<sample-ck> -> smpl(	<dwManufacturer:DWORD>		// MMA manufacturer ID
						<dwProduct:DWORD>			// 
						<dwSamplePeriod:DWORD>		// # 10E9 / (sample per second)
						<dwMIDIUnityNote:DWORD>		// 
						<dwMIDIPitchFraction:DWORD> // 
						<dwSMPTEFormat:DWORD>		// 
						<dwSMPTEOffset:DWORD>		// 
						<cSampleLoops:DWORD>		// 
						<cbSamplerData:DWORD>		// 
						<sample-loop(s)>			// 
						<sampler-specific-data> )	// 

<sample-loop> struct
				{
					DWORD dwIdentifier;
					DWORD dwType;
					DWORD dwStart;
					DWORD dwEnd;
					DWORD dwFraction;
					DWORD dwPlayCount;
				}
-----------------------------------------------------------------------------------------------------------------------
 */

typedef struct			SND_tdst_RIFFForm_
{
	UI32	dwFormType;
	UI32	dwCkId;
	UI32	dwCkSize;
} SND_tdst_RIFFForm;

typedef struct	SND_tdst_RIFFChunk_
{
	UI32	dwCkId;
	UI32	dwCkSize;
	UI8		dstCkData[1];
} SND_tdst_RIFFChunk;

/*$1- format chunck ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	SND_tdst_WAVEfmtCk_Partcommon_
{
	UI16	wFormatTag;
	UI16	wChannels;
	UI32	dwSamplesPerSec;
	UI32	dwAvgBytesPerSec;
	UI16	wBlockAlign;
} SND_tdst_WAVEfmtCk_Partcommon;

typedef struct	SND_tdst_WAVEfmtCk_PartPCMspecific_
{
	UI16	wBitsPerSample;
} SND_tdst_WAVEfmtCk_PartPCMspecific;

typedef struct	SND_tdst_WAVEfmtCk_PartExtended_
{
	UI16	wBitsPerSample;
	UI16	cbSize;
} SND_tdst_WAVEfmtCk_PartExtended;

typedef struct	SND_tdst_WAVEfmtCk_PartXbox_
{
	UI16	wBitsPerSample;
	UI16	cbSize;
	UI16	wSamplesPerBlock;
} SND_tdst_WAVEfmtCk_PartXbox;

typedef struct	SND_tdst_MS_ADPCM_COEFF_
{
	int iCoef1;
	int iCoef2;
} SND_tdst_MS_ADPCM_COEFF;

typedef struct	SND_tdst_WAVEfmtCk_PartMS_ADPCM_
{
	UI16					wBitsPerSample;
	UI16					cbSize;
	UI16					wSamplesPerBlock;
	UI16					wNumCoef;
	SND_tdst_MS_ADPCM_COEFF aCoeff[1];
} SND_tdst_WAVEfmtCk_PartMS_ADPCM;

/*$1- fact chunck ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	SND_tdst_WAVEfactCk_
{
	UI32	dwFileSize;
} SND_tdst_WAVEfactCk;

/*$1- cue point ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	SND_tdst_WAVEcue_point_
{
	UI32	dwName;
	UI32	dwPosition;
	char	fccChunk[4];
	UI32	dwChunkStart;
	UI32	dwBlockStart;
	UI32	dwSampleOffset;
} SND_tdst_WAVEcue_point;

typedef struct	SND_tdst_WAVEcueCk_
{
	UI32					dwCuePoints;
	SND_tdst_WAVEcue_point	*dstCuePointTable;
} SND_tdst_WAVEcueCk;

/*$1- play list ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	SND_tdst_WAVEplaysegment_
{
	UI32	dwName;
	UI32	dwLength;
	UI32	dwLoops;
} SND_tdst_WAVEplay_segment;

typedef struct	SND_tdst_WAVEplaylistCk_
{
	UI32						dwSegments;
	SND_tdst_WAVEplay_segment	dstSegment[1];
} SND_tdst_WAVEplaylistCk;

/*$1- assoc data list ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	SND_tdst_WAVElablCk_
{
	UI32	dwName;
	UI8		data[1];
} SND_tdst_WAVElablCk;

typedef struct	SND_tdst_WAVEnoteCk_
{
	UI32	dwName;
	UI8		data[1];
} SND_tdst_WAVEnoteCk;

typedef struct	SND_tdst_WAVEltxtCk_
{
	UI32	dwName;
	UI32	dwSampleLength;
	char	dwPurpose[4];
	UI16	wCountry;
	UI16	wLanguage;
	UI16	wDialect;
	UI16	wCodePage;

	/* UI8 data[1]; if any */
} SND_tdst_WAVEltxtCk;

typedef struct	SND_tdst_WAVEfileCk_
{
	UI32	dwName;
	UI32	dwMedType;
	UI8		fileData[1];
} SND_tdst_WAVEfileCk;

/*$1- instrument chunk ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	SND_tdst_WAVEinstCk_
{
	UI8 bUnshiftedNote;
	UI8 chFineTune;
	UI8 chGain;
	UI8 bLowNote;
	UI8 bHighNote;
	UI8 bLowVelocity;
	UI8 bHighVelocity;
} SND_tdst_WAVEinstCk;

/*$1- sample chunk ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	SND_tdst_WAVEsample_loop_
{
	UI32	dwIdentifier;
	UI32	dwType;
	UI32	dwStart;
	UI32	dwEnd;
	UI32	dwFraction;
	UI32	dwPlayCount;
} SND_tdst_WAVEsample_loop;

typedef struct	SND_tdst_WAVEsmplCk_
{
	UI32						dwManufacturer;
	UI32						dwProduct;
	UI32						dwSamplePeriod;
	UI32						dwMIDIUnityNote;
	UI32						dwMIDIPitchFraction;
	UI32						dwSMPTEFormat;
	UI32						dwSMPTEOffset;
	UI32						cSampleLoops;
	UI32						cbSamplerData;
	SND_tdst_WAVEsample_loop	dstSampleLoop[1];
	UI8							dstSamplerSpecificData[1];
} SND_tdst_WAVEsmplCk;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#if defined(PSX2_TARGET) || defined(_GAMECUBE)
#pragma pack(1)
#else
#pragma pack(pop, 1)
#endif
typedef struct	SND_tdst_PlaySegment_
{
	UI32	dwLength;	/* play segment */
	UI32	dwLoops;
} SND_tdst_PlaySegment;

typedef struct	SND_tdst_CuePoint_
{
	UI32					dwName;			/* cue */
	UI32					dwPosition;
	char					fccChunk[4];
	UI32					dwChunkStart;
	UI32					dwBlockStart;
	UI32					dwSampleOffset;

	UI32					ui_SegmentNb;
	SND_tdst_PlaySegment	*dst_Segment;

	UI32					dwSampleLength; /* ltxt */
	char					dwPurpose[4];
	UI16					wCountry;
	UI16					wLanguage;
	UI16					wDialect;
	UI16					wCodePage;
	char					*sz_Txt;
	char					*sz_Label;		/* label */
	char					*sz_Note;		/* note */
} SND_tdst_CuePoint;

/*$2------------------------------------------------------------------------------------------------------------------*/

#define SND_Csz_LabelLoop	"loop"
#define SND_Csz_LabelStart	"start"
#define SND_Csz_LabelEnd	"end"

typedef struct	SND_tdst_MyCuePoint_
{
	UI32	dwName;			/* cue */
	UI32	dwPosition;
	UI32	dwSampleLength; /* ltxt */
	char	*sz_Label;		/* label */
} SND_tdst_MyCuePoint;

typedef struct	SND_tdst_CuePointDesc_
{
	unsigned int		dwCuePoints;
	SND_tdst_MyCuePoint *dst_Table;
} SND_tdst_CuePointDesc;

typedef struct	SND_tdst_SignalPoint_
{
	ULONG	ul_Label;
	ULONG	ul_Position;
} SND_tdst_SignalPoint;

typedef struct	SND_tdst_Region_
{
	ULONG	ul_Label;
	ULONG	ul_StartPosition;
	ULONG	ul_StopPosition;
} SND_tdst_Region;

typedef struct	SND_tdst_WaveDesc_
{
	/* bigfile */
	ULONG					ul_Flags;
	ULONG					ul_UserCount;
	ULONG					ul_FileKey;
	ULONG					ul_DataSize;
	ULONG					ul_DataPosition;

	/* format */
	UI16					wFormatTag;
	UI16					wChannels;
	UI32					dwSamplesPerSec;
	UI32					dwAvgBytesPerSec;
	UI16					wBlockAlign;
	UI16					wBitsPerSample;
	UI16					cbSize;
	UI16					wSamplesPerBlock;

	/* regions for jade */
	char					b_HasLoop;
#if defined(_XENON)
    char                    cSoundBFIndex;
#else
    char                    b_Dummy1;
#endif
	char					b_Dummy2;
	char					b_Dummy3;

	ULONG					ul_StartOffset;
	ULONG					ul_EndOffset;
	/**/
	ULONG					ul_LoopBeginOffset;
	ULONG					ul_LoopEndOffset;
	/**/
	ULONG					ul_ExitPointNb;
	ULONG					*aul_ExitPoint;
	/**/
	ULONG					ul_SignalPointNb;
	SND_tdst_SignalPoint	*dst_SignalTable;
	/**/
	ULONG					ul_RegionNb;
	SND_tdst_Region			*dst_RegionTable;
} SND_tdst_WaveDesc;

/*$2------------------------------------------------------------------------------------------------------------------*/

#if defined(_GAMECUBE)
typedef struct	tWAVEFORMAT
{
	WORD	wFormatTag;
	WORD	nChannels;
	DWORD	nSamplesPerSec;
	DWORD	nAvgBytesPerSec;
	WORD	nBlockAlign;
} WAVEFORMAT;
#endif
#if defined(PSX2_TARGET) || defined(_GAMECUBE)
typedef struct	tWAVEFORMATEX
{
	WORD	wFormatTag;
	WORD	nChannels;
	DWORD	nSamplesPerSec;
	DWORD	nAvgBytesPerSec;
	WORD	nBlockAlign;
	WORD	wBitsPerSample;
	WORD	cbSize;
} WAVEFORMATEX;
#endif
typedef struct	SND_tdst_WaveData_
{
	int				i_Size;			/* Size of data (PCM or ADPCM) */
	BYTE			*pbData;		/* Wave Bits */
	WAVEFORMATEX	st_WaveFmtx;	/* Wave Header */
} SND_tdst_WaveData;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void			SND_WaveInitModule(void);
ULONG			SND_ul_WaveCallback(ULONG _ulFilePos);
void			SND_WaveUnload(SND_tdst_WaveDesc *pWave);
unsigned int	SND_ui_SizeToSample(unsigned short usFormat, unsigned short usChannel, unsigned int uiSize);
unsigned int	SND_ui_SampleToSize(unsigned short usFormat, unsigned short usChannel, unsigned int uiSample);
BOOL			SND_b_SizeIsEqual
				(
					unsigned short	usFormat,
					unsigned short	usChannel,
					unsigned int	ui_Size1,
					unsigned int	ui_Size2
				);
BOOL			SND_b_SampleIsEqual
				(
					unsigned short	usFormat,
					unsigned short	usChannel,
					unsigned int	ui_Size1,
					unsigned int	ui_Size2
				);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* _SNDWAVE_H_ */
