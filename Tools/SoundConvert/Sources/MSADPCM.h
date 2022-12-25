/*$T MSADPCM.h GC 1.138 05/12/04 10:40:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifndef __MSADPCM_h__
#define __MSADPCM_h__

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
#define C_MSADPCM_SAMPLESPERBLOCK		    60
#define C_MSADPCM_MONO_BLOCKSIZE			36
#define C_MSADPCM_HeaderSize				50

extern int	WriteADPCMWave
			(
				char			*filename,
				unsigned int	codedBytes,
				unsigned char	*pADPCMData,
				unsigned short	numChannels,
				unsigned int	numSamples,
				unsigned int	samplingFrequency
			);
            

extern int	CompressADPCM
			(
				unsigned int	codedBytes,
				unsigned int	numSamples,
				short			*pInputSamples,
				unsigned char	*pOutput,
				unsigned int	numBlocks,
				unsigned int	extraSamples,
				unsigned short	numChannels
			);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#endif
