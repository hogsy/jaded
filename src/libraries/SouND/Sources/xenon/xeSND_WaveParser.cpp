#include "Precomp.h"

#ifdef _XENON

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "xeSND_WaveParser.h"
#include "xeSND_Engine.h"

#include "BIGfiles/LOAding/LOAdefs.h"

// ***********************************************************************************************************************
//    Class Methods : CWaveParser
// ***********************************************************************************************************************

CWaveParser::CWaveParser()
{
	L_memset(&m_stTruncatedWave, 0, sizeof(m_stTruncatedWave));
}

// ***********************************************************************************************************************

CWaveParser::~CWaveParser()
{
}

// ***********************************************************************************************************************

unsigned int CWaveParser::GetMarkerOffset(unsigned int Param_uiFormat, unsigned int Param_uiChannelCount, unsigned int Param_uiSampleCount)
{
	unsigned int uiBlockSize;
	unsigned int uiSamplesPerBlock;

	switch(Param_uiFormat)
	{
		case SND_Cte_WAVE_FORMAT_PCM:			uiBlockSize =  2; uiSamplesPerBlock =  1; break;
		case SND_Cte_WAVE_FORMAT_MS_ADPCM:		uiBlockSize = 36; uiSamplesPerBlock = 60; break;
		case SND_Cte_WAVE_FORMAT_XBOX_ADPCM:	uiBlockSize = 36; uiSamplesPerBlock = 64; break;
		default:								return 0;
	}

	return ((Param_uiSampleCount * uiBlockSize * Param_uiChannelCount) / uiSamplesPerBlock);
}

// ***********************************************************************************************************************

SND_tdst_WaveDesc* CWaveParser::ProcessRIFFFile(unsigned int Param_uiStartPositionInFile, unsigned int Param_uiKey, eSoundBFIndex Param_eSoundBF)
{
	SND_tdst_WaveDesc* pWaveDescriptor = (SND_tdst_WaveDesc*)0xFFFFFFFF;

	// Set Database Building Mode

	BOOL bDatabaseBuilding = (Param_eSoundBF != eSoundBF_None);

	if (!bDatabaseBuilding)
	{
		// Check If Already Loaded

		pWaveDescriptor = (SND_tdst_WaveDesc*)LOA_ul_SearchAddress(Param_uiStartPositionInFile);

		if ((unsigned int)pWaveDescriptor != 0xFFFFFFFF)
		{
			pWaveDescriptor->ul_UserCount++;
			return pWaveDescriptor;
		}

		// Populate From Database (If Found)

		pWaveDescriptor = GetAudioEngine()->GetHeaderList()->RetrieveFromDatabase(Param_uiKey/*, Param_uiStartPositionInFile, Param_eSoundBF*/);

		if ((unsigned int)pWaveDescriptor != 0xFFFFFFFF)
		{
			return pWaveDescriptor;
		}

		// Find Alternate BF To Use (And Replace Position)

		unsigned int uiPosition = GetAudioEngine()->SoundFile_FindKey(Param_uiKey, &Param_eSoundBF);

		if (uiPosition != 0xFFFFFFFF)
		{
			Param_uiStartPositionInFile = uiPosition;
		}
		else
		{
#if _XENON_SOUND_FORCEALTERNATEBF
			m_stTruncatedWave.ul_UserCount++;
			return &m_stTruncatedWave;
#endif
		}
	}

	// Prepare Stream Reader

	CStreamReader* pStreamReader = GetAudioEngine()->GetStreamReader();

	pStreamReader->Open(GetAudioEngine()->SoundFile_Fetch(Param_eSoundBF)->GetHandle_Sync(), Param_uiStartPositionInFile, FALSE);

	// Fetch Size

	unsigned int uiFileSize = pStreamReader->Read32();

	// Handle Truncated File

	if (uiFileSize == 8)
	{
		pStreamReader->Close();
		m_stTruncatedWave.ul_UserCount++;
		return &m_stTruncatedWave;
	}

	// Clear Fields

	stWave_Cue_Point_Descriptor	stCuePoints;

	stCuePoints.dwCuePoints = 0;

	// Scan RIFF Tags

	while (pStreamReader->GetCurrentOffsetInData() < uiFileSize)
	{
		unsigned int uiChunkID		= pStreamReader->Read32();
		unsigned int uiChunkSize	= pStreamReader->Read32();

		switch (uiChunkID)
		{
			// ***********************************************************************************************************

			case kChunkID_RIFF:
			{
				pStreamReader->Skip(4); // 'WAVE'
				break;
			}

			// ***********************************************************************************************************

			case kChunkID_Format:
			{
				stWave_Chunk_Format_Common	stfmtCk;
				stWave_MSADPCM_Coeff*		aCoeff;
				unsigned short				wBitsPerSample;
				unsigned short				cbSize;
				unsigned short				wSamplesPerBlock;
				unsigned short				wNumCoef;

				// Common

				stfmtCk.wFormatTag			= pStreamReader->Read16();
				stfmtCk.wChannels			= pStreamReader->Read16();
				stfmtCk.dwSamplesPerSec		= pStreamReader->Read32();
				stfmtCk.dwAvgBytesPerSec	= pStreamReader->Read32();
				stfmtCk.wBlockAlign			= pStreamReader->Read16();

				uiChunkSize -= sizeof(stWave_Chunk_Format_Common);

				// Format Specific

				switch(stfmtCk.wFormatTag)
				{
					case SND_Cte_WAVE_FORMAT_PCM:
					{
						wBitsPerSample		= (8 * stfmtCk.wBlockAlign) / stfmtCk.wChannels;
						cbSize				= 0;
						wSamplesPerBlock	= stfmtCk.wChannels;

						if(uiChunkSize == sizeof(stWave_Chunk_Format_PCM))
						{
							wBitsPerSample = pStreamReader->Read16();
						}

						if(uiChunkSize == sizeof(stWave_Chunk_Format_Extended))
						{
							wBitsPerSample	= pStreamReader->Read16();
							cbSize			= pStreamReader->Read16();
						}
						break;
					}

					case SND_Cte_WAVE_FORMAT_MS_ADPCM:
					{
						wBitsPerSample		= pStreamReader->Read16();
						cbSize				= pStreamReader->Read16();
						wSamplesPerBlock	= pStreamReader->Read16();
						wNumCoef			= pStreamReader->Read16();

						// Allocate Coefficient Array

						aCoeff = (stWave_MSADPCM_Coeff*)GetAudioEngine()->MemoryAllocate(wNumCoef * sizeof(stWave_MSADPCM_Coeff), eMemoryTracker_General);

						// Populate Coefficients

						for(int iLoop = 0; iLoop < wNumCoef; iLoop++)
						{
							aCoeff[iLoop].iCoef1 = pStreamReader->Read32();
							aCoeff[iLoop].iCoef2 = pStreamReader->Read32();
						}
						break;
					}

					case SND_Cte_WAVE_FORMAT_XBOX_ADPCM:
					{
						wBitsPerSample		= pStreamReader->Read16();
						cbSize				= pStreamReader->Read16();
						wSamplesPerBlock	= 64;

						if (cbSize == sizeof(short))
						{
							cbSize				= 0;
							wSamplesPerBlock	= pStreamReader->Read16();
						}
						break;
					}
				}

				pStreamReader->Skip(cbSize);

				// Allocate Wave Descriptor

				pWaveDescriptor = (SND_tdst_WaveDesc*)GetAudioEngine()->MemoryAllocate(sizeof(SND_tdst_WaveDesc), eMemoryTracker_General);

				// Populate Descriptor

				pWaveDescriptor->ul_FileKey			= Param_uiKey;
				pWaveDescriptor->ul_UserCount		= 1;
				pWaveDescriptor->cSoundBFIndex		= Param_eSoundBF;

				pWaveDescriptor->wFormatTag			= stfmtCk.wFormatTag;
				pWaveDescriptor->wChannels			= stfmtCk.wChannels;
				pWaveDescriptor->dwSamplesPerSec	= stfmtCk.dwSamplesPerSec;
				pWaveDescriptor->dwAvgBytesPerSec	= stfmtCk.dwAvgBytesPerSec;
				pWaveDescriptor->wBlockAlign		= stfmtCk.wBlockAlign;
				pWaveDescriptor->wBitsPerSample		= wBitsPerSample;
				pWaveDescriptor->cbSize				= cbSize;
				pWaveDescriptor->wSamplesPerBlock	= wSamplesPerBlock;
				break;
			}

			// ***********************************************************************************************************

			case kChunkID_Data:
			{
				// Error Checking

				if ((unsigned int)pWaveDescriptor == 0xFFFFFFFF)
				{
					break;
				}

				pWaveDescriptor->ul_DataSize = uiChunkSize;

				//if (LOA_IsBinaryData())
				//{
					//Param_uiStartPositionInFile = BIG_ul_SearchKeyToPos(LOA_ul_GetCurrentKey());
				//}

				// Get File Position Of WAVE Data

				pWaveDescriptor->ul_DataPosition = pStreamReader->GetCurrentOffsetInData() + Param_uiStartPositionInFile;

				// Cache Data In A Prefetch Buffer (If Already Fully Loaded & Not Building Database)

				if (!bDatabaseBuilding && (pStreamReader->GetRemainingUnreadInBuffer() >= pWaveDescriptor->ul_DataSize))
				{
					CPrefetchBuffer* pBuffer = GetAudioEngine()->GetPrefetchList()->Request(pWaveDescriptor->ul_DataPosition, pWaveDescriptor->ul_DataSize, Param_eSoundBF);

					if (pBuffer != NULL)
					{
						pBuffer->StoreDataToPrefetch((char*)pStreamReader->GetCurrentBufferPointer());
					}
				}

				// Skip Data

				pStreamReader->Skip(pWaveDescriptor->ul_DataSize);
				break;
			}

			// ***********************************************************************************************************

			case kChunkID_Cue:
			{
				stCuePoints.dwCuePoints	= pStreamReader->Read32();
				stCuePoints.dst_Table	= (stWave_Cue_Point*)GetAudioEngine()->MemoryAllocate(stCuePoints.dwCuePoints * sizeof(stWave_Cue_Point), eMemoryTracker_General);

				for(unsigned int uiLoop = 0; uiLoop < stCuePoints.dwCuePoints; uiLoop++)
				{
					stCuePoints.dst_Table[uiLoop].dwName		= pStreamReader->Read32();
					stCuePoints.dst_Table[uiLoop].dwPosition	= pStreamReader->Read32();

					pStreamReader->Skip(16);
				}
				break;
			}

			// ***********************************************************************************************************

			case kChunkID_List:
			{
				pStreamReader->Skip(4); // 'adtl'
				break;
			}

			// ***********************************************************************************************************

			case kChunkID_Label:
			{
				// find the cue point 

				unsigned int dwName = pStreamReader->Read32();

				for(unsigned int uiLoop = 0; uiLoop < stCuePoints.dwCuePoints; uiLoop++)
				{
					if (stCuePoints.dst_Table[uiLoop].dwName == dwName)
					{
						// read name

						char acName[1024];

						int iNameLength = pStreamReader->ReadString(acName);

						if ((acName[0] >= '0') && (acName[0] <= '9'))
						{
							// Begins With Number : Exit Point

							stCuePoints.dst_Table[uiLoop].sz_Label = NULL;
						}
						else
						{
							// "Loop", "Start", "End" Or Event Point

							stCuePoints.dst_Table[uiLoop].sz_Label = (char*)GetAudioEngine()->MemoryAllocate(iNameLength + 1, eMemoryTracker_General);

							L_strcpy(stCuePoints.dst_Table[uiLoop].sz_Label, acName);
						}
						break;
					}
				}
				break;
			}

			// ***********************************************************************************************************

			case kChunkID_LabelledText:
			{
				unsigned int uiOffset_ChunkStartInData	= pStreamReader->GetCurrentOffsetInData();
				unsigned int dwName						= pStreamReader->Read32();

				for (unsigned int uiLoop = 0; uiLoop < stCuePoints.dwCuePoints; uiLoop++)
				{
					if (stCuePoints.dst_Table[uiLoop].dwName == dwName)
					{
						stCuePoints.dst_Table[uiLoop].dwSampleLength = pStreamReader->Read32();
						break;
					}
				}

				pStreamReader->SeekInData(uiOffset_ChunkStartInData + uiChunkSize);
				break;
			}

			// ***********************************************************************************************************

			default:
			{
				pStreamReader->Skip(uiChunkSize);
				break;
			}

			// ***********************************************************************************************************
		}

		// Align On 2 Bytes Boundary

		if ((pStreamReader->GetCurrentOffsetInData() & 1) != 0)
		{
			pStreamReader->Skip();
		}
	}

	// Parse Cue Points (If Applicable)

	if ((stCuePoints.dwCuePoints > 0) && ((unsigned int)pWaveDescriptor != 0xFFFFFFFF))
	{
		BAS_tdst_barray	stExitPointCollection;
		BAS_tdst_barray	stSignalPointCollection;
		BAS_tdst_barray	stRegionPointCollection;

		BAS_binit(&stExitPointCollection  , 1);
		BAS_binit(&stSignalPointCollection, 1);
		BAS_binit(&stRegionPointCollection, 1);

		for (unsigned int uiLoop = 0; uiLoop < stCuePoints.dwCuePoints; uiLoop++)
		{
			if (stCuePoints.dst_Table[uiLoop].sz_Label != NULL)
			{
				// if there is a label, it is a signal point or start/loop/end

				if (L_stricmp(stCuePoints.dst_Table[uiLoop].sz_Label, SND_Csz_LabelStart) == 0)
				{
					// Start Marker

					pWaveDescriptor->ul_StartOffset = GetMarkerOffset(pWaveDescriptor->wFormatTag, pWaveDescriptor->wChannels, stCuePoints.dst_Table[uiLoop].dwPosition);

					// Register As Possible Exit Point

					BAS_binsert(stCuePoints.dst_Table[uiLoop].dwPosition, stCuePoints.dst_Table[uiLoop].dwPosition, &stExitPointCollection);
				}
				else if (L_stricmp(stCuePoints.dst_Table[uiLoop].sz_Label, SND_Csz_LabelEnd) == 0)
				{
					// End Marker

					pWaveDescriptor->ul_EndOffset = GetMarkerOffset(pWaveDescriptor->wFormatTag, pWaveDescriptor->wChannels, stCuePoints.dst_Table[uiLoop].dwPosition);

					// Register As Possible Exit Point

					BAS_binsert(stCuePoints.dst_Table[uiLoop].dwPosition, stCuePoints.dst_Table[uiLoop].dwPosition, &stExitPointCollection);
				}
				else if (L_stricmp(stCuePoints.dst_Table[uiLoop].sz_Label, SND_Csz_LabelLoop) == 0)
				{
					// Loop Region

                    pWaveDescriptor->b_HasLoop = 1;

					pWaveDescriptor->ul_LoopBeginOffset	= GetMarkerOffset(pWaveDescriptor->wFormatTag, pWaveDescriptor->wChannels, stCuePoints.dst_Table[uiLoop].dwPosition);
					pWaveDescriptor->ul_LoopEndOffset	= GetMarkerOffset(pWaveDescriptor->wFormatTag, pWaveDescriptor->wChannels, stCuePoints.dst_Table[uiLoop].dwPosition + stCuePoints.dst_Table[uiLoop].dwSampleLength);

					if(pWaveDescriptor->ul_LoopBeginOffset == pWaveDescriptor->ul_LoopEndOffset)
					{
						pWaveDescriptor->ul_LoopBeginOffset	= 0;
						pWaveDescriptor->ul_LoopEndOffset	= 0;
					}

					// Register As Possible Exit Point

					BAS_binsert(stCuePoints.dst_Table[uiLoop].dwPosition, stCuePoints.dst_Table[uiLoop].dwPosition, &stExitPointCollection);
				}
                else
				{
					// Signal Marker

                    if (stCuePoints.dst_Table[uiLoop].dwSampleLength)
					{
                        BAS_binsert(stCuePoints.dst_Table[uiLoop].dwPosition, uiLoop, &stRegionPointCollection);
					}
                    else
					{
					    BAS_binsert(stCuePoints.dst_Table[uiLoop].dwPosition, *((int*)stCuePoints.dst_Table[uiLoop].sz_Label), &stSignalPointCollection);
					}
				}
			}
		}

		// Set Loop Points

		if (pWaveDescriptor->ul_LoopEndOffset == 0)
		{
			pWaveDescriptor->ul_LoopEndOffset = pWaveDescriptor->ul_DataSize;
		}

		// Set Exit Points

		if (stExitPointCollection.num > 0)
		{
			pWaveDescriptor->ul_ExitPointNb	= stExitPointCollection.num;
			pWaveDescriptor->aul_ExitPoint	= (ULONG*)GetAudioEngine()->MemoryAllocate(pWaveDescriptor->ul_ExitPointNb * sizeof(ULONG), eMemoryTracker_General);

			for(int iLoop = 0; iLoop < stExitPointCollection.num; iLoop++)
			{
				pWaveDescriptor->aul_ExitPoint[iLoop] = stExitPointCollection.base[iLoop].ul_Key;
			}
		}

		BAS_bfree(&stExitPointCollection);

		// Set Signal Points

		if (stSignalPointCollection.num > 0)
		{
			pWaveDescriptor->ul_SignalPointNb	= stSignalPointCollection.num;
			pWaveDescriptor->dst_SignalTable	= (SND_tdst_SignalPoint*)GetAudioEngine()->MemoryAllocate(pWaveDescriptor->ul_SignalPointNb * sizeof(SND_tdst_SignalPoint), eMemoryTracker_General);

			for (int iLoop = 0; iLoop < stSignalPointCollection.num; iLoop++)
			{
				pWaveDescriptor->dst_SignalTable[iLoop].ul_Position	= stSignalPointCollection.base[iLoop].ul_Key;
				pWaveDescriptor->dst_SignalTable[iLoop].ul_Label	= stSignalPointCollection.base[iLoop].ul_Val;
			}
		}

		BAS_bfree(&stSignalPointCollection);

		// Set Region

		if (stRegionPointCollection.num > 0)
		{
			pWaveDescriptor->ul_RegionNb		= stRegionPointCollection.num;
			pWaveDescriptor->dst_RegionTable	= (SND_tdst_Region*)GetAudioEngine()->MemoryAllocate(pWaveDescriptor->ul_RegionNb * sizeof(SND_tdst_Region), eMemoryTracker_General);

			for (int iLoop = 0; iLoop < stRegionPointCollection.num; iLoop++)
			{
				pWaveDescriptor->dst_RegionTable[iLoop].ul_Label			= *((int*)stCuePoints.dst_Table[stRegionPointCollection.base[iLoop].ul_Val].sz_Label);
                pWaveDescriptor->dst_RegionTable[iLoop].ul_StartPosition	= stRegionPointCollection.base[iLoop].ul_Key;
                pWaveDescriptor->dst_RegionTable[iLoop].ul_StopPosition		= pWaveDescriptor->dst_RegionTable[iLoop].ul_StartPosition + stCuePoints.dst_Table[stRegionPointCollection.base[iLoop].ul_Val].dwSampleLength;
			}
		}

		BAS_bfree(&stRegionPointCollection);

		// Free Cue Point Descriptor

		if (stCuePoints.dst_Table != NULL)
		{
			for (unsigned int uiLoop = 0; uiLoop < stCuePoints.dwCuePoints; uiLoop++)
			{
				if (stCuePoints.dst_Table[uiLoop].sz_Label != NULL)
				{
					GetAudioEngine()->MemoryFree(stCuePoints.dst_Table[uiLoop].sz_Label, eMemoryTracker_General);
				}
			}

			GetAudioEngine()->MemoryFree(stCuePoints.dst_Table, eMemoryTracker_General);
		}
	}

	// Close Stream Reader

	pStreamReader->Close();

	// Return Descriptor Pointer If Successful (Truncated Wave Otherwise)

	if ((unsigned int)pWaveDescriptor != 0xFFFFFFFF)
	{
		// Add Compact Header To Database

		GetAudioEngine()->GetHeaderList()->AddToDatabase(pWaveDescriptor, Param_uiStartPositionInFile);

		return pWaveDescriptor;
	}
	else
	{
		m_stTruncatedWave.ul_UserCount++;
		return &m_stTruncatedWave;
	}
}

// ***********************************************************************************************************************

void CWaveParser::DestroyDescriptor(SND_tdst_WaveDesc* __restrict Param_pWaveDescriptor)
{
	// Error Checking

	if (Param_pWaveDescriptor == NULL)
	{
		return;
	}

	// Decrement Reference Counter

	Param_pWaveDescriptor->ul_UserCount--;

	if (Param_pWaveDescriptor->ul_UserCount > 0)
	{
		return;
	}

	// Delete Address

	LOA_DeleteAddress(Param_pWaveDescriptor);

	// Free Exit Points

	if (Param_pWaveDescriptor->aul_ExitPoint)
	{
		GetAudioEngine()->MemoryFree(Param_pWaveDescriptor->aul_ExitPoint, eMemoryTracker_General);
	}

	// Free Signal Table

	if (Param_pWaveDescriptor->dst_SignalTable)
	{
		GetAudioEngine()->MemoryFree(Param_pWaveDescriptor->dst_SignalTable, eMemoryTracker_General);
	}

	// Free Region Table

	if (Param_pWaveDescriptor->dst_RegionTable)
	{
		GetAudioEngine()->MemoryFree(Param_pWaveDescriptor->dst_RegionTable, eMemoryTracker_General);
	}

	// Free Descriptor (If Applicable)

	if (Param_pWaveDescriptor != &m_stTruncatedWave)
	{
		GetAudioEngine()->MemoryFree(Param_pWaveDescriptor, eMemoryTracker_General);
	}
}

// ***********************************************************************************************************************

#endif // _XENON
