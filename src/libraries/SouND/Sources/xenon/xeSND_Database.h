#ifndef __xeSND_Database_h__
#define __xeSND_Database_h__

// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************

typedef struct
{
	unsigned int	uiPosition;
	eSoundBFIndex	eSoundBF;
	unsigned int	uiSize;
	char			acData;
}
stMapFileEntry;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CCompactHeader : public CListItem
{
private:
	unsigned int	m_uiKey;

	int				m_iSoundBF_Index;
	//unsigned int	m_uiSoundBF_FilePosition;

	unsigned int	m_uiBigFile_DataPosition;
	unsigned int	m_uiBigFile_DataSize;
	unsigned int	m_uiFormat_Tag;
	unsigned int	m_uiFormat_ChannelCount;
	unsigned int	m_uiFormat_SamplesPerSecond;
	unsigned int	m_uiFormat_AverageBytesPerSecond;
	unsigned int	m_uiFormat_BlockAlign;
	unsigned int	m_uiFormat_BitsPerSample;
	unsigned int	m_uiFormat_ExtraInfoSize;
	unsigned int	m_uiFormat_SamplesPerBlock;
	BOOL			m_bHasLoop;
	unsigned int	m_uiLoop_BeginOffset;
	unsigned int	m_uiLoop_EndOffset;
	unsigned int	m_uiOffset_Start;
	unsigned int	m_uiOffset_End;
	unsigned int	m_uiExitPoint_Count;
	unsigned int*	m_uiExitPoints;
	unsigned int	m_uiSignalPoint_Count;
	unsigned int*	m_uiSignalPoint_Positions;
	unsigned int*	m_uiSignalPoint_Labels;
	unsigned int	m_uiRegion_Count;
	unsigned int*	m_uiRegion_Labels;
	unsigned int*	m_uiRegion_StartPositions;
	unsigned int*	m_uiRegion_StopPositions;

public:
    M_DeclareOperatorNewAndDelete();

	CCompactHeader(unsigned int Param_uiKey);
	virtual ~CCompactHeader();

	unsigned int	Get_Key();
	int				Get_SoundBF_Index();
	void			Set_SoundBF_Index(int Param_iSoundBF_Index);
	//unsigned int	Get_SoundBF_FilePosition();
	//void			Set_SoundBF_FilePosition(unsigned int Param_uiSoundBF_FilePosition);
	unsigned int	Get_BigFile_DataPosition();
	void			Set_BigFile_DataPosition(unsigned int Param_uiDataPosition);
	unsigned int	Get_BigFile_DataSize();
	void			Set_BigFile_DataSize(unsigned int Param_uiDataSize);
	unsigned int	Get_Format_Tag();
	void			Set_Format_Tag(unsigned int Param_uiTag);
	unsigned int	Get_Format_ChannelCount();
	void			Set_Format_ChannelCount(unsigned int Param_uiChannelCount);
	unsigned int	Get_Format_SamplesPerSecond();
	void			Set_Format_SamplesPerSecond(unsigned int Param_uiSamplesPerSecond);
	unsigned int	Get_Format_AverageBytesPerSecond();
	void			Set_Format_AverageBytesPerSecond(unsigned int Param_uiAverageBytesPerSecond);
	unsigned int	Get_Format_BlockAlign();
	void			Set_Format_BlockAlign(unsigned int Param_uiBlockAlign);
	unsigned int	Get_Format_BitsPerSample();
	void			Set_Format_BitsPerSample(unsigned int Param_uiBitsPerSample);
	unsigned int	Get_Format_ExtraInfoSize();
	void			Set_Format_ExtraInfoSize(unsigned int Param_uiExtraInfoSize);
	unsigned int	Get_Format_SamplesPerBlock();
	void			Set_Format_SamplesPerBlock(unsigned int Param_uiSamplesPerBlock);
	BOOL			Get_Loop();
	void			Set_Loop(BOOL Param_bHasLoop);
	unsigned int	Get_Loop_BeginOffset();
	void			Set_Loop_BeginOffset(unsigned int Param_uiLoop_BeginOffset);
	unsigned int	Get_Loop_EndOffset();
	void			Set_Loop_EndOffset(unsigned int Param_uiLoop_EndOffset);
	unsigned int	Get_Offset_Start();
	void			Set_Offset_Start(unsigned int Param_uiOffset_Start);
	unsigned int	Get_Offset_End();
	void			Set_Offset_End(unsigned int Param_uiOffset_End);
	unsigned int	Get_ExitPoint_Count();
	void			Set_ExitPoint_Count(unsigned int Param_uiExitPoint_Count);
	unsigned int	Get_ExitPoint_Value(unsigned int Param_uiIndex);
	void			Set_ExitPoint_Value(unsigned int Param_uiIndex, unsigned int Param_uiExitPoint_Value);
	unsigned int	Get_SignalPoint_Count();
	void			Set_SignalPoint_Count(unsigned int Param_uiSignalPoint_Count);
	unsigned int	Get_SignalPoint_Position(unsigned int Param_uiIndex);
	void			Set_SignalPoint_Position(unsigned int Param_uiIndex, unsigned int Param_uiSignalPoint_Position);
	unsigned int	Get_SignalPoint_Label(unsigned int Param_uiIndex);
	void			Set_SignalPoint_Label(unsigned int Param_uiIndex, unsigned int Param_uiSignalPoint_Label);
	unsigned int	Get_Region_Count();
	void			Set_Region_Count(unsigned int Param_uiRegion_Count);
	unsigned int	Get_Region_Label(unsigned int Param_uiIndex);
	void			Set_Region_Label(unsigned int Param_uiIndex, unsigned int Param_uiRegion_Label);
	unsigned int	Get_Region_StartPosition(unsigned int Param_uiIndex);
	void			Set_Region_StartPosition(unsigned int Param_uiIndex, unsigned int Param_uiRegion_StartPosition);
	unsigned int	Get_Region_StopPosition(unsigned int Param_uiIndex);
	void			Set_Region_StopPosition(unsigned int Param_uiIndex, unsigned int Param_uiRegion_StopPosition);
};

// ***********************************************************************************************************************

class CHeaderList : public CItemList
{
private:
	CFile*			m_pDatabaseFile;
	unsigned int	m_uiDatabaseEntryCount;
	CBuffer*		m_pMapFile_Data;
	CFile*			m_pMapFile_Handle;
	char			m_acFilename[64];

public:
    M_DeclareOperatorNewAndDelete();

	CHeaderList();
	virtual ~CHeaderList();

	BOOL				IsDatabaseOpen();
	CCompactHeader*		Find(unsigned int Param_uiKey);
	CCompactHeader*		Request(unsigned int Param_uiKey, BOOL Param_bNoSearch = FALSE);
	void				BuildEntireDatabase();
	void				OpenDatabase();
	void				UpdateDatabase();
	void				CloseDatabase();
	void				AddToDatabase(SND_tdst_WaveDesc* Param_pWaveDescriptor, unsigned int Param_uiSoundBF_FilePosition);
	SND_tdst_WaveDesc*	RetrieveFromDatabase(unsigned int Param_uiKey/*, unsigned int Param_uiSoundBF_FilePosition, int Param_iSoundBF_Index*/);
	void				OpenMapFile(unsigned int Param_uiMapKey);
	void				AddToMapFile(unsigned int Param_uiPosition, eSoundBFIndex Param_eSoundBF, unsigned int Param_uiSize, char* Param_pData);
	void				CloseMapFile();
};

// ***********************************************************************************************************************

#endif // __xeSND_Database_h__
