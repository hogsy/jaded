/*$T CDV_Manager.h GC! 1.097 01/12/01 15:25:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_TARGET
#ifndef __CDV_Manager_h__
#define __CDV_Manager_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

/* standard limits */
#define CDV_Cte_SectorSize		2048
#define CDV_Cte_MaxDirNb		256
#define CDV_Cte_MaxDirLevel		8
#define CDV_Cte_MaxNameLength	15	/* "12345678.123;1" included the '\0' */

/* standard specs */
#define CDV_Cte_SectSystem0			0
#define CDV_Cte_SectSystem1			1
#define CDV_Cte_SectSystem2			2
#define CDV_Cte_SectSystem3			3
#define CDV_Cte_SectSystem4			4
#define CDV_Cte_SectSystem5			5
#define CDV_Cte_SectSystem6			6
#define CDV_Cte_SectSystem7			7
#define CDV_Cte_SectSystem8			8
#define CDV_Cte_SectSystem9			9
#define CDV_Cte_SectSystem10		10
#define CDV_Cte_SectSystem11		11
#define CDV_Cte_SectSystem12		12
#define CDV_Cte_SectSystem13		13
#define CDV_Cte_SectSystem14		14
#define CDV_Cte_SectSystem15		15
#define CDV_Cte_SectPVD				16
#define CDV_Cte_SectPVDT			17
#define CDV_Cte_SectLPathTable		18
#define CDV_Cte_SectOptLPathTable	19
#define CDV_Cte_SectPathTable		20
#define CDV_Cte_SectOptPathTable	21
#define CDV_Cte_SectRootDirectory	22

/* standard values */
#define CDV_Cte_StdVolType	1		/* Primary Volume Descriptor type */
#define CDV_Cte_VolEndType	255		/* Volume Descriptor Set Terminator type */
#define CDV_Cte_StdVolID	"CD001"

/* File Flags for Directory Records */
#define CDV_Cte_existenceBit	0x01
#define CDV_Cte_directoryBit	0x02
#define CDV_Cte_associatedBit	0x04
#define CDV_Cte_recordBit		0x08
#define CDV_Cte_protectionBit	0x10
#define CDV_Cte_multiextentBit	0x80

/* lib Cte */
#define CDV_Cte_SeekSet			0
#define CDV_Cte_SeekCur			1
#define CDV_Cte_SeekEnd			2
#define CDV_Cte_DefaultBuffSize (16 * CDV_Cte_SectorSize)
#define CDV_Cte_MaxFileNb		110
#define CDV_Cte_TryCount		10
#define CDV_Cte_SpinMode		SCECdSpinNom
#define CDV_Cte_DataPattern		SCECdSecS2048
//#define CDV_Cte_MediaType		SCECdCD

/*
 -----------------------------------------------------------------------------------------------------------------------
    #define CDV_Cte_MediaType SCECdDVD
 -----------------------------------------------------------------------------------------------------------------------
 */

/*$4
 ***********************************************************************************************************************
    Types
 ***********************************************************************************************************************
 */

typedef struct	CDV_tdst_Sector_
{
	unsigned int	ui_StartSector;
	unsigned int	ui_SectorNb;
} CDV_tdst_Sector;

typedef struct	CDV_tdst_FileTree_
{
	unsigned int				ui_StartSector;
	unsigned int				ui_SectorNb;
	unsigned int				ui_ByteSize;
	unsigned int				ui_FileNb;
	struct CDV_tdst_FileTree_	**pst_FileLst;
	unsigned int				ui_SubDirNb;
	struct CDV_tdst_FileTree_	**pst_SubDirLst;
	char						asz_Name[CDV_Cte_MaxNameLength];
} CDV_tdst_FileTree;

typedef struct	CDV_tdst_FileHandler_
{
	unsigned int	ui_StartSector;
	unsigned int	ui_SectorNb;
	unsigned int	ui_FileSeek;
	unsigned int	ui_FileSize;
	unsigned char	*puc_Buffer;
	unsigned int	ui_BufferSize;
	unsigned int	ui_BufferSeek;
	unsigned int	ui_Flags;
	unsigned int	ui_StartSectorOct;
	char			asz_Name[CDV_Cte_MaxNameLength];
} CDV_tdst_FileHandler;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ISO 9660 structures
 -----------------------------------------------------------------------------------------------------------------------
 */

#pragma pack(0)
typedef struct	CDV_tdst_DateTime_
{
	char	asz_Year[4];									/* 0-3 */
	char	asz_Month[2];									/* 4-5 */
	char	asz_Day[2];										/* 5-7 */
	char	asz_Hour[2];									/* 8-9 */
	char	asz_Min[2];										/* 10-11 */
	char	asz_Sec[2];										/* 12-13 */
	char	asz_msec[2];									/* 14-15 */
	char	i8_GMT;											/* 16 */
} CDV_tdst_DateTime;

typedef struct	CDV_tdst_iDateTime_
{
	char	i8_Year;										/* 0 */
	char	i8_Month;										/* 1 */
	char	i8_Day;											/* 2 */
	char	i8_Hour;										/* 3 */
	char	i8_Min;											/* 4 */
	char	i8_Sec;											/* 5 */
	char	i8_GMT;											/* 6 */
} CDV_tdst_iDateTime;

typedef struct	CDV_tdst_DIR_
{
	char				i8_Length;							/* 0 */
	char				i8_ExtAttrLength;					/* 1 */
	char				i32_StartSector[4];					/* 2-5 */
	char				pad0[4];							/* 6-9 */
	char				i32_DataLength[4];					/* 10-13 */
	char				pad1[4];							/* 14-17 */
	CDV_tdst_iDateTime	st_RecordingDateTime;				/* 18-24 */
	char				i8_FileFlags;						/* 25 */
	char				i8_FileUnitSize;					/* 26 */
	char				i8_InterleaveGapSize;				/* 27 */
	char				i16_VolumeSequenceNumber[2];		/* 28-29 */
	char				pad2[2];							/* 30-31 */
	char				i8_NameLength;						/* 32 */
	char				asz_Name[1];						/* 33-... */
} CDV_tdst_DIR;

typedef struct	CDV_tdst_PVD_
{
	char				i8_VDType;							/* 0 */
	char				asz_VSStandardID[5];				/* 1-5 */
	char				i8_VSStandardVersion;				/* 6 */
	char				pad1;								/* 7 */
	char				asz_SystemID[32];					/* 8-39 */
	char				asz_VolumeID[32];					/* 40-71 */
	char				pad2[8];							/* 72-79 */
	char				i32lsb_VolumeSpaceSize[4];			/* 80-83 */
	char				i32msb_VolumeSpaceSize[4];			/* 84-87 */
	char				pad3[32];							/* 88-119 */
	char				i16lsb_VolumeSetSize[2];			/* 120-121 */
	char				i16msb_VolumeSetSize[2];			/* 122-123 */
	char				i16lsb_VolumeSequenceNumber[2];		/* 124-125 */
	char				i16msb_VolumeSequenceNumber[2];		/* 126-127 */
	char				i16lsb_LogicalBlockSize[2];			/* 128-129 */
	char				i16msb_LogicalBlockSize[2];			/* 130-131 */
	char				i32lsb_PathTableSize[4];			/* 132-135 */
	char				i32msb_PathTableSize[4];			/* 136-139 */
	char				i32lsb_LPath_TableLoc_Sector[4];	/* 140-143 */
	char				i32lsb_OptLPath_TableLoc_Sector[4]; /* 144-147 */
	char				i32msb_LPath_TableLoc_Sector[4];	/* 148-151 */
	char				i32msb_OptLPath_TableLoc_Sector[4]; /* 152-155 */
	CDV_tdst_DIR		st_RootDirectoryEntry;				/* 156-189 */
	char				asz_VolumeSetID[128];				/* 190-317 */
	char				asz_PublisherID[128];				/* 318-445 */
	char				asz_DataPreparerID[128];			/* 446-573 */
	char				asz_ApplicationID[128];				/* 574-701 */
	char				asz_CopyrightFileID[37];			/* 702-738 */
	char				asz_AbstractFileID[37];				/* 739-775 */
	char				asz_BibliographyFileID[37];			/* 776-812 */
	CDV_tdst_DateTime	st_VolumeCreationDateTime;			/* 813-829 */
	CDV_tdst_DateTime	st_VolumeModificationDateTime;		/* 830-846 */
	CDV_tdst_DateTime	st_VolumeExpirationDateTime;		/* 847-863 */
	CDV_tdst_DateTime	st_VolumeEffectiveDateTime;			/* 864-880 */
	char				i8_FileStructureStandardVersion;	/* 881 */
	char				pad4;								/* 882 */
	char				asz_ApplicationUse[512];			/* 883-1394 */
	char				asz_FutureStandardization[653];		/* 1395-2047 */
} CDV_tdst_PVD;

typedef struct	CDV_tdst_PathTableRecord_
{
	char	i8_IDLength;									/* 0 */
	char	i8_ExtAttrLength;								/* 1 */
	char	i32_StartSector[4];								/* 2-5 */
	char	i16_ParentDir[2];								/* 6-7 */
	char	asz_Name[1];									/* 8-... */
} CDV_tdst_PathTableRecord;

/*$4
 ***********************************************************************************************************************
    ee/iop dependant part
 ***********************************************************************************************************************
 */

#ifdef PSX2_IOP
#define _include_iopCDV_Manager_h_
#include "IOP/iop/iopCDV_Manager.h"
#undef _include_iopCDV_Manager_h_
#else
#define _include_eeCDV_Manager_h_
#include "IOP/ee/eeCDV_Manager.h"
#undef _include_eeCDV_Manager_h_
#endif

/*$4
 ***********************************************************************************************************************
    end of ee/iop dependant part
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __CDV_Manager_h__ */
#endif /* PSX2_TARGET */
