
#include "stdafx.h"
#include "MSADPCM.h"

typedef struct
{	int				channels, blocksize, samplesperblock, blocks, dataremaining ; 
	int				blockcount ;
	//XTE
	//sf_count_t			samplecount ;
	int				samplecount;
	//XTE
	short			*samples ;
	unsigned char	*block ;
	unsigned char	dummydata [4] ; /* Dummy size */
} MSADPCM_PRIVATE ;

#include	<stdio.h>
#include	<string.h>
#include	<math.h>
//#include	<unistd.h>

//#include	"sndfile.h"
//#include	"config.h"
//#include	"sfendian.h"
//#include	"float_cast.h"
//#include	"common.h"
//#include	"wav_w64.h"

static	void	choose_predictor (unsigned int channels, short *data, int *bpred, int *idelta) ;

int fmsadpcm_encode_block (/*SF_PRIVATE *psf*/MSADPCM_PRIVATE *pms);

/* These required here because we write the header in this file. */

#define RIFF_MARKER	(MAKE_MARKER ('R', 'I', 'F', 'F')) 
#define WAVE_MARKER	(MAKE_MARKER ('W', 'A', 'V', 'E')) 
#define fmt_MARKER	(MAKE_MARKER ('f', 'm', 't', ' ')) 
#define fact_MARKER	(MAKE_MARKER ('f', 'a', 'c', 't')) 
#define data_MARKER	(MAKE_MARKER ('d', 'a', 't', 'a')) 

//#define WAVE_FORMAT_MS_ADPCM	0x0002
#define	MSADPCM_ADAPT_COEFF_COUNT	7

/*============================================================================================
** MS ADPCM static data and functions.
*/

static int AdaptationTable []    = 
{	230, 230, 230, 230, 307, 409, 512, 614,
	768, 614, 512, 409, 307, 230, 230, 230 
} ;

/* TODO : The first 7 coef's are are always hardcode and must
   appear in the actual WAVE file.  They should be read in
   in case a sound program added extras to the list. */

static int AdaptCoeff1 [7] = 
{	256, 512, 0, 192, 240, 460, 392 
} ;

static int AdaptCoeff2 [7] = 
{	0, -256, 0, 64, 0, -208, -232
} ;

static float fAdaptCoeff1 [7] = 
{	256.0f, 512.0f, 0.0f, 192.0f, 240.0f, 460.0f, 392.0f
} ;

static float fAdaptCoeff2 [7] = 
{	0.0f, -256.0f, 0.0f, 64.0f, 0.0f, -208.0f, -232.0f
} ;


/*============================================================================================
**	MS ADPCM Block Layout.
**	======================
**	Block is usually 256, 512 or 1024 bytes depending on sample rate.
**	For a mono file, the block is laid out as follows:
**		byte	purpose
**		0		block predictor [0..6]
**		1,2		initial idelta (positive)
**		3,4		sample 1
**		5,6		sample 0
**		7..n	packed bytecodes
**
**	For a stereo file, the block is laid out as follows:
**		byte	purpose
**		0		block predictor [0..6] for left channel
**		1		block predictor [0..6] for right channel
**		2,3		initial idelta (positive) for left channel
**		4,5		initial idelta (positive) for right channel
**		6,7		sample 1 for left channel
**		8,9		sample 1 for right channel
**		10,11	sample 0 for left channel
**		12,13	sample 0 for right channel
**		14..n	packed bytecodes
*/

/*============================================================================================
** Static functions.
*/

/*==========================================================================================
** MS ADPCM Write Functions.
*/

void
msadpcm_write_adapt_coeffs (/*SF_PRIVATE *psf*/)
{	int k ;

	for (k = 0 ; k < MSADPCM_ADAPT_COEFF_COUNT ; k++);
		/*
		TODO:Write this
		psf_binheader_writef (psf, "e22", AdaptCoeff1 [k], AdaptCoeff2 [k]) ;
		*/
} /* msadpcm_write_adapt_coeffs */

/*==========================================================================================
*/

static int
msadpcm_encode_block (/*SF_PRIVATE *psf*/MSADPCM_PRIVATE *pms)
{
	unsigned int	blockindx ;
	unsigned char	byte ;
	int				chan, k, predict, bpred [2], idelta [2], errordelta, newsamp ;
	
	choose_predictor (pms->channels, pms->samples, bpred, idelta) ;

	/* Write the block header. */

	if (pms->channels == 1)
	{	pms->block [0]	= bpred [0] ;	
		pms->block [1]	= idelta [0] & 0xFF ;
		pms->block [2]	= idelta [0] >> 8 ;
		pms->block [3]	= pms->samples [1] & 0xFF ;
		pms->block [4]	= pms->samples [1] >> 8 ;
		pms->block [5]	= pms->samples [0] & 0xFF ;
		pms->block [6]	= pms->samples [0] >> 8 ;

		blockindx = 7 ;
		byte = 0 ;

		/* Encode the samples as 4 bit. */
		
		for (k = 2 ; k < pms->samplesperblock ; k++)
		{	predict = (pms->samples [k-1] * AdaptCoeff1 [bpred [0]] + pms->samples [k-2] * AdaptCoeff2 [bpred [0]]) >> 8 ;
			errordelta = (pms->samples [k] - predict) / idelta [0] ;
			if (errordelta < -8)
				errordelta = -8 ;
			else if (errordelta > 7)
				errordelta = 7 ;
			newsamp = predict + (idelta [0] * errordelta) ;
			if (newsamp > 32767)
				newsamp = 32767 ;
			else if (newsamp < -32768)
				newsamp = -32768 ;
			if (errordelta < 0)
				errordelta += 0x10 ;
				
			byte = (byte << 4) | (errordelta & 0xF) ;
			if (k % 2)
			{	pms->block [blockindx++] = byte ;
				byte = 0 ;
				} ;

			idelta [0] = (idelta [0] * AdaptationTable [errordelta]) >> 8 ;
			if (idelta [0] < 16)
				idelta [0] = 16 ;
			pms->samples [k] = newsamp ;
			} ;
		}
	else
	{	/* Stereo file. */
		pms->block [0]	= bpred [0] ;	
		pms->block [1]	= bpred [1] ;	

		pms->block [2]	= idelta [0] & 0xFF ;
		pms->block [3]	= idelta [0] >> 8 ;
		pms->block [4]	= idelta [1] & 0xFF ;
		pms->block [5]	= idelta [1] >> 8 ;
		
		pms->block [6]	= pms->samples [2] & 0xFF ;
		pms->block [7]	= pms->samples [2] >> 8 ;
		pms->block [8]	= pms->samples [3] & 0xFF ;
		pms->block [9]	= pms->samples [3] >> 8 ;

		pms->block [10]	= pms->samples [0] & 0xFF ;
		pms->block [11]	= pms->samples [0] >> 8 ;
		pms->block [12]	= pms->samples [1] & 0xFF ;
		pms->block [13]	= pms->samples [1] >> 8 ;
	
		blockindx = 14 ;
		byte = 0 ;
		chan = 1 ;
		
		for (k = 4 ; k < 2 * pms->samplesperblock ; k++)
		{	chan = k & 1 ;

			predict = (pms->samples [k-2] * AdaptCoeff1 [bpred [chan]] + pms->samples [k-4] * AdaptCoeff2 [bpred [chan]]) >> 8 ;
			errordelta = (pms->samples [k] - predict) / idelta [chan] ;


			if (errordelta < -8)
				errordelta = -8 ;
			else if (errordelta > 7)
				errordelta = 7 ;
			newsamp = predict + (idelta [chan] * errordelta) ;
			if (newsamp > 32767)
				newsamp = 32767 ;
			else if (newsamp < -32768)
				newsamp = -32768 ;
			if (errordelta < 0)
				errordelta += 0x10 ;
				
			byte = (byte << 4) | (errordelta & 0xF) ;

			if (chan)
			{	pms->block [blockindx++] = byte ;
				byte = 0 ;
				} ;

			idelta [chan] = (idelta [chan] * AdaptationTable [errordelta]) >> 8 ;
			if (idelta [chan] < 16)
				idelta [chan] = 16 ;
			pms->samples [k] = newsamp ;
			} ;
		} ;

	/* Write the block to disk. */
/*
	//TODO: Write to disk
	if ((k = psf_fwrite (pms->block, 1, pms->blocksize, psf)) != pms->blocksize)
		psf_log_printf (psf, "*** Warning : short write (%d != %d).\n", k, pms->blocksize) ;
*/		
	memset (pms->samples, 0, pms->samplesperblock * sizeof (short)) ;

	pms->blockcount ++ ;
	pms->samplecount = 0 ;


	return 1 ;
} /* msadpcm_encode_block */

//XTE
//static sf_count_t 
static int
//XTE
msadpcm_write_block (/*SF_PRIVATE *psf,*/ MSADPCM_PRIVATE *pms, short *ptr, int len)
{	
	int		count, total = 0, indx = 0 ;
	
	while (indx < len)
	{	
		count = (pms->samplesperblock - pms->samplecount) * pms->channels ;

		if (count > len - indx)
			count = len - indx ;

		memcpy (&(pms->samples [pms->samplecount * pms->channels]), &(ptr [total]), count * sizeof (short)) ;
		indx += count ;
		pms->samplecount += count / pms->channels ;
		total = indx ;

		if (pms->samplecount >= pms->samplesperblock)
			/*
			TODO: Pass correct parameter
			msadpcm_encode_block (psf, pms) ;
			*/
			//Test with floats
			//msadpcm_encode_block(pms);
			fmsadpcm_encode_block(pms);

		//XTE
		pms->block += pms->blocksize;
		//XTE


	} ;

	return total ;		
} /* msadpcm_write_block */



int CompressADPCM(unsigned codedBytes, unsigned numSamples,short* pInputSamples,unsigned char *pOutput,unsigned numBlocks,unsigned extraSamples,unsigned short numChannels)
{
	unsigned convertedSamples = 0,convertedBlocks = 0,i,toBeConvertedBlocks;	

	MSADPCM_PRIVATE MyMSADPCM;

	MyMSADPCM.channels = numChannels;
    MyMSADPCM.samplesperblock = C_MSADPCM_SAMPLESPERBLOCK;
    MyMSADPCM.blocksize = numChannels*C_MSADPCM_MONO_BLOCKSIZE;
	MyMSADPCM.blocks = 1;
	MyMSADPCM.dataremaining = 0;
	MyMSADPCM.samplecount = 0;
	MyMSADPCM.blockcount = 0;

	MyMSADPCM.samples = pInputSamples;
	MyMSADPCM.block = pOutput;

	if( extraSamples != 0 )
	{
		toBeConvertedBlocks = numBlocks - 1;
	}
	else
	{
		toBeConvertedBlocks = numBlocks;
	}
	//Convert n-1 blocks
	for(i=0;i<toBeConvertedBlocks;++i)
	{
		//Prepare all the pointers for the block conversion
		MyMSADPCM.block = pOutput + (i * MyMSADPCM.blocksize);
		MyMSADPCM.samples = pInputSamples + (i * MyMSADPCM.samplesperblock * MyMSADPCM.channels);

		msadpcm_encode_block(&MyMSADPCM);
		//fmsadpcm_encode_block(&MyMSADPCM);
	}
	if( extraSamples != 0 )
	{
		//Encode the last block
		MyMSADPCM.block = pOutput + (i * MyMSADPCM.blocksize);
		MyMSADPCM.samples = pInputSamples + (i * MyMSADPCM.samplesperblock * MyMSADPCM.channels);
		MyMSADPCM.samplesperblock = extraSamples;
		msadpcm_encode_block(&MyMSADPCM);
		//fmsadpcm_encode_block(&MyMSADPCM);
	}

	return i;
}

/*----------------------------------------------------------------------------------------
**	Choosing the block predictor.
**	Each block requires a predictor and an idelta for each channel. 
**	The predictor is in the range [0..6] which is an indx into the	two AdaptCoeff tables. 
**	The predictor is chosen by trying all of the possible predictors on a small set of
**	samples at the beginning of the block. The predictor with the smallest average
**	abs (idelta) is chosen as the best predictor for this block. 
**	The value of idelta is chosen to to give a 4 bit code value of +/- 4 (approx. half the 
**	max. code value). If the average abs (idelta) is zero, the sixth predictor is chosen.
**	If the value of idelta is less then 16 it is set to 16.
**
**	Microsoft uses an IDELTA_COUNT (number of sample pairs used to choose best predictor)
**	value of 3. The best possible results would be obtained by using all the samples to
**	choose the predictor.
*/

//#define		IDELTA_COUNT	3
//#define		IDELTA_COUNT	6
//Set to 60 in order to select the best predictor over all the samples of the block (assume 60 samples per block)
#define		IDELTA_COUNT	60

static	void	
choose_predictor (unsigned int channels, short *data, int *block_pred, int *idelta)
{	unsigned int	chan, k, bpred, idelta_sum, best_bpred, best_idelta ;
	
	for (chan = 0 ; chan < channels; chan++)
	{	best_bpred = best_idelta = 0 ;

		for (bpred = 0 ; bpred < 7 ; bpred++)
		{	idelta_sum = 0 ;
			for (k = 2 ; k < 2 + IDELTA_COUNT ; k++)
				idelta_sum += abs (data [k*channels] - ((data [(k-1)*channels] * AdaptCoeff1 [bpred] + data [(k-2)*channels] * AdaptCoeff2 [bpred]) >> 8)) ;
			idelta_sum /= (4 * IDELTA_COUNT) ;
			
			if (bpred == 0 || idelta_sum < best_idelta)
			{	best_bpred = bpred ;
				best_idelta = idelta_sum ;
				} ;
				
			if (! idelta_sum)
			{	best_bpred = bpred ;
				best_idelta = 16 ;
				break ;
				} ;
		
			} ; /* for bpred ... */
		if (best_idelta < 16)
			best_idelta = 16 ;
	
		block_pred [chan] = best_bpred ;
		idelta [chan]     = best_idelta ;
		} ;

	return ;
} /* choose_predictor */

int WriteADPCMWave(char* filename,unsigned codedBytes,unsigned char *pADPCMData,unsigned short numChannels,unsigned numSamples,unsigned samplingFrequency)
{
	char RIFFstr[] = "RIFF";
	char WAVEstr[] = "WAVE";
	char fmt_str[] = "fmt ";
	char datastr[] = "data";
	char factstr[] = "fact";

	unsigned char copiedasis [] = { 0x20,0x00,0xF4,0x03,0x07,0x00,0x00,0x01,
									0x00,0x00,0x00,0x02,0x00,0xFF,0x00,0x00,
									0x00,0x00,0xC0,0x00,0x40,0x00,0xF0,0x00,
									0x00,0x00,0xCC,0x01,0x30,0xFF,0x88,0x01,
									0x18,0xFF };
	unsigned char copiedasis_Stereo [] = {
									0x20, 0x00, 0xF4, 0x07, 0x07, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02
										, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x40, 0x00, 0xF0, 0x00, 0x00, 0x00, 0xCC, 0x01
										, 0x30, 0xFF, 0x88, 0x01, 0x18, 0xFF

	};


	unsigned char factarray [] = { 0x04,0x00,0x00,0x00,0x56,0x37,0x00,0x00 };
	unsigned size,u32,i;
	unsigned short u16;
	FILE *fp = fopen(filename,"wb");
	if(fp!=NULL)
	{
		fwrite(RIFFstr,1,4,fp);
		size = codedBytes + 4 + 4 + 4 + 50 + 4 + 4 + 4 + 4 + 4;
		fwrite(&size,1,sizeof(unsigned),fp);
		fwrite(WAVEstr,1,4,fp);
		fwrite(fmt_str,1,4,fp);
		size = 50;
		fwrite(&size,1,sizeof(unsigned),fp);
		//Compression format
		u16 = 2;
		fwrite(&u16,1,sizeof(unsigned short),fp);
		//Number of channels
		fwrite(&numChannels,1,sizeof(unsigned short),fp);
		//Sampling frequency
		fwrite(&samplingFrequency,1,sizeof(unsigned),fp);
		//Byterate
		u32 = samplingFrequency*C_MSADPCM_MONO_BLOCKSIZE/C_MSADPCM_SAMPLESPERBLOCK;
		//Testsmall buffer
		//u32 = samplingFrequency*38/64;
		fwrite(&u32,1,sizeof(unsigned),fp);
		//Block align
		//Testsmall buffer
		u16 = numChannels*C_MSADPCM_MONO_BLOCKSIZE ;
		//u16 = (numChannels==1) ? 38 : 64;
		fwrite(&u16,1,sizeof(unsigned short),fp);
		//Bits per sample
		u16 = 4;
		fwrite(&u16,1,sizeof(unsigned short),fp);
		//Copied as is:
		for(i=0;i<34;++i)
			if(numChannels == 1)
				fwrite(copiedasis+i,1,1,fp);
			else
				fwrite(copiedasis_Stereo+i,1,1,fp);
		//Copy fact field
		fwrite(factstr,1,4,fp);
		for(i=0;i<4;++i)
			fwrite(factarray+i,1,1,fp);
		u32 = numSamples;
		fwrite(&numSamples,1,sizeof(unsigned),fp);
		fwrite(datastr,1,4,fp);
		//Write data chunk size
		fwrite(&codedBytes,1,4,fp);
		//Write effective data
		unsigned short u = 0;
		//To test the header
		//for(i=0;i<codedBytes;++i)
		//	fwrite(&u,1,1,fp);
		fwrite(pADPCMData,codedBytes,1,fp);
		fclose(fp);
		return 0;
	}
	else
		return -1;
}

int FloatToInt(float f)
{
	if( f > 0.0f )
		f += .5f;
	else
		f -= .5f;
	return (int) f;
}

int fmsadpcm_encode_block (/*SF_PRIVATE *psf*/MSADPCM_PRIVATE *pms)
{
	unsigned int	blockindx ;
	unsigned char	byte ;
	int				chan, k, /*predict,*/ bpred [2], idelta [2], errordelta, newsamp ;
	float			fpredict,fnewsamp,fidelta[2],ferrordelta;
	
	choose_predictor (pms->channels, pms->samples, bpred, idelta) ;

	/* Write the block header. */

	if (pms->channels == 1)
	{	
//		pms->block [0]	= bpred [0] ;	
//		pms->block [1]	= idelta [0] & 0xFF ;
//		pms->block [2]	= idelta [0] >> 8 ;
//		pms->block [3]	= pms->samples [1] & 0xFF ;
//		pms->block [4]	= pms->samples [1] >> 8 ;
//		pms->block [5]	= pms->samples [0] & 0xFF ;
//		pms->block [6]	= pms->samples [0] >> 8 ;
//
//		blockindx = 7 ;
//		byte = 0 ;
//
//		/* Encode the samples as 4 bit. */
//		
//		for (k = 2 ; k < pms->samplesperblock ; k++)
//		{	predict = (pms->samples [k-1] * AdaptCoeff1 [bpred [0]] + pms->samples [k-2] * AdaptCoeff2 [bpred [0]]) >> 8 ;
//			errordelta = (pms->samples [k] - predict) / idelta [0] ;
//			if (errordelta < -8)
//				errordelta = -8 ;
//			else if (errordelta > 7)
//				errordelta = 7 ;
//			newsamp = predict + (idelta [0] * errordelta) ;
//			if (newsamp > 32767)
//				newsamp = 32767 ;
//			else if (newsamp < -32768)
//				newsamp = -32768 ;
//			if (errordelta < 0)
//				errordelta += 0x10 ;
//				
//			byte = (byte << 4) | (errordelta & 0xF) ;
//			if (k % 2)
//			{	
//				pms->block [blockindx++] = byte ;
//				byte = 0 ;
//			} ;
//
//			idelta [0] = (idelta [0] * AdaptationTable [errordelta]) >> 8 ;
//			if (idelta [0] < 16)
//				idelta [0] = 16 ;
//			pms->samples [k] = newsamp ;
//		} ;
		
	}
	else
	{	/* Stereo file. */
		pms->block [0]	= bpred [0] ;	
		pms->block [1]	= bpred [1] ;	

		pms->block [2]	= idelta [0] & 0xFF ;
		pms->block [3]	= idelta [0] >> 8 ;
		pms->block [4]	= idelta [1] & 0xFF ;
		pms->block [5]	= idelta [1] >> 8 ;
		
		pms->block [6]	= pms->samples [2] & 0xFF ;
		pms->block [7]	= pms->samples [2] >> 8 ;
		pms->block [8]	= pms->samples [3] & 0xFF ;
		pms->block [9]	= pms->samples [3] >> 8 ;

		pms->block [10]	= pms->samples [0] & 0xFF ;
		pms->block [11]	= pms->samples [0] >> 8 ;
		pms->block [12]	= pms->samples [1] & 0xFF ;
		pms->block [13]	= pms->samples [1] >> 8 ;
	
		blockindx = 14 ;
		byte = 0 ;
		chan = 1 ;

		fidelta[0] = (float) idelta[0];
		fidelta[1] = (float) idelta[1];
		
		for (k = 4 ; k < 2 * pms->samplesperblock ; k++)
		{	
			chan = k & 1 ;

			fpredict = (pms->samples [k-2] * fAdaptCoeff1 [bpred [chan]] + pms->samples [k-4] * fAdaptCoeff2 [bpred [chan]]) / 256.0f ;
			ferrordelta = (pms->samples [k] - fpredict) / fidelta [chan] ;


			if (ferrordelta < -8.0f)
				ferrordelta = -8.0f;
			else if (ferrordelta > 7.0f)
				ferrordelta = 7.0f;
			fnewsamp = fpredict + (fidelta [chan] * ferrordelta) ;
			if (fnewsamp > 32767.0f)
				fnewsamp = 32767.0f ;
			else if (fnewsamp < -32768.0f)
				fnewsamp = -32768.0f ;
			if (ferrordelta < 0.0f)
				ferrordelta += 0x10 ;

			errordelta = FloatToInt(ferrordelta);
				
			byte = (byte << 4) | (errordelta & 0xF) ;

			if (chan)
			{	
				pms->block [blockindx++] = byte ;
				byte = 0 ;
			} ;

			fidelta [chan] = (fidelta [chan] * AdaptationTable [errordelta]) / 256;
			if (fidelta [chan] < 16.0f)
				fidelta [chan] = 16.0f ;
			idelta[chan] = FloatToInt(fidelta[chan]);
			newsamp = FloatToInt(fnewsamp);
			pms->samples [k] = newsamp ;
			} ;
		} ;

	/* Write the block to disk. */
/*
	//TODO: Write to disk
	if ((k = psf_fwrite (pms->block, 1, pms->blocksize, psf)) != pms->blocksize)
		psf_log_printf (psf, "*** Warning : short write (%d != %d).\n", k, pms->blocksize) ;
*/		
	memset (pms->samples, 0, pms->samplesperblock * sizeof (short)) ;

	pms->blockcount ++ ;
	pms->samplecount = 0 ;


	return 1 ;
} /* msadpcm_encode_block */


