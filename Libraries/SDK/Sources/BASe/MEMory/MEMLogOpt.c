/*$T MEMLog.c GC 1.129 09/05/01 11:05:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "BASe/BAStypes.h"

#ifdef PSX2_TARGET
#include "BASe/MEMory/MEMLog.h" /* Avoid C++ errors */
#endif

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIwin.h"

#ifdef MEM_OPT

#if defined(_DEBUG) || defined(ACTIVE_EDITORS)
#ifdef ACTIVE_EDITORS
int MEM_gi_AllocatedMemory = 0; /* Real allocated memory */
int MEM_gi_AddAllocatedMemory = 0;
#endif


#if (defined(PSX2_TARGET) || defined(_GAMECUBE))
#define M_OutputDebugString	printf
#else
#define M_OutputDebugString	OutputDebugString
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_Log
(
	char	*psz_Msg,
	void	*_pv_BlockAddress,
	ULONG	_ul_BlockSize,
	ULONG	_ul_first4bytes,
	char	*_sz_File,
	int		_i_Line
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	aszTmp[512], *psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	psz_Temp = aszTmp + sprintf(aszTmp, "%s(%d) : ", _sz_File, _i_Line);
	psz_Temp += sprintf
		(
			psz_Temp,
			"%s : address = %08X ; size = %6d bytes",
			psz_Msg,
			(LONG) _pv_BlockAddress,
			_ul_BlockSize
		);
	psz_Temp += sprintf(psz_Temp, " first 4 bytes = %08X (", _ul_first4bytes);
	*psz_Temp++ = (char) (((_ul_first4bytes) & 0xFF) < 32 ? '.' : (_ul_first4bytes & 0xFF));
	*psz_Temp++ = (char) (((_ul_first4bytes >> 8) & 0xFF) < 32 ? '.' : ((_ul_first4bytes >> 8) & 0xFF));
	*psz_Temp++ = (char) (((_ul_first4bytes >> 16) & 0xFF) < 32 ? '.' : ((_ul_first4bytes >> 16) & 0xFF));
	*psz_Temp++ = (char) (((_ul_first4bytes >> 24) & 0xFF) < 32 ? '.' : ((_ul_first4bytes >> 24) & 0xFF));
	psz_Temp += sprintf(psz_Temp, ")\n");

	M_OutputDebugString(aszTmp);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_LogString(char *psz_Msg)
{
	M_OutputDebugString(psz_Msg);
}
#endif


/*
 =======================================================================================================================
 =======================================================================================================================
 */

void MEM_ComputeMemoryHoleInfo(ULONG * _pul_NumHoles, ULONG * _pul_SizeOfHoles, BOOL _b_Log)
{
	int uBasicSizeOfHoles;
	* _pul_NumHoles = MEM_gst_MemoryInfo.ul_HolesStatic;

	uBasicSizeOfHoles = MEM_gst_MemoryInfo.ul_RealSize 
		- MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated
		- MEM_uGetLastBlockSize(&MEM_gst_MemoryInfo);

    * _pul_SizeOfHoles = uBasicSizeOfHoles ;

#ifdef _GAMECUBE
    {
        int uVirtualSizeOfHoles = MEM_gst_VirtualMemoryInfo.ul_RealSize 
    		- MEM_gst_VirtualMemoryInfo.ul_DynamicCurrentAllocated
	    	- MEM_uGetLastBlockSize(&MEM_gst_VirtualMemoryInfo);
        * _pul_SizeOfHoles += uVirtualSizeOfHoles;

        * _pul_NumHoles += MEM_gst_VirtualMemoryInfo.ul_HolesStatic;
    }
#endif //_GAMECUBE

}

/*void MEM_ComputeMemoryHoleInfo(ULONG * _pul_NumHoles, ULONG * _pul_SizeOfHoles, BOOL _b_Log)
{
	int  i, j;
	MEM_tdst_ZlistEntry *pst_Zelem;
	void				*pv_Hole;
	ULONG				test;

	if(_pul_SizeOfHoles) (*_pul_SizeOfHoles) = 0;
	if(_pul_NumHoles) (*_pul_NumHoles) = 0;

#ifdef USE_HOLE_OPTIM
{
	ULONG				*pul_Hole;
	int					cpt;
	 
	pul_Hole = MEM_gp_OldHolesList;		
	cpt = 0;
	while(pul_Hole)
	{
		cpt ++;
		if(_pul_SizeOfHoles) (*_pul_SizeOfHoles) += *(pul_Hole + 1);
		if(_pul_NumHoles) (*_pul_NumHoles) ++;
		pul_Hole = (ULONG *) *pul_Hole; 
	}
	
	if(cpt != (int) MEM_ul_NumOldHoles)
	{
		OSReport("Compute Memory PB\n");
	}
}
#endif

	for(i = 0; i < MEM_C_ZlistSize; i++)
	{
		pst_Zelem = &MEM_gst_MemoryInfo.MEM_ast_Zlist[i];
		pv_Hole = pst_Zelem->pv_Hole;
		j = 0;
		if(pv_Hole)
		{
			test = *(LONG *) pv_Hole;
			if(_pul_SizeOfHoles) (*_pul_SizeOfHoles) += (pst_Zelem->ul_Size + 8);
			if(_pul_NumHoles) (*_pul_NumHoles) ++;
			while((test != 0) && (test != 0xFFFFFFFF))
			{
				j ++;
				test = *(LONG *) test;
				if(_pul_SizeOfHoles) (*_pul_SizeOfHoles) += (pst_Zelem->ul_Size + 8);
				if(_pul_NumHoles) (*_pul_NumHoles) ++;
			}
		}

#ifdef _DEBUG
		if(j && _b_Log)
		{
			char	asz_Log[200];


			sprintf(asz_Log,"Index of %u bytes has %u holes \n", MEM_gst_MemoryInfo.MEM_ast_Zlist[i].ul_Size, j);
			M_OutputDebugString(asz_Log);
		}
#endif

	}
#ifdef _DEBUG

	if(_b_Log)
	{
		M_OutputDebugString("------------------------------\n");
	}
#endif
}*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_MakeAllocatedMemoryString(char *asz_Buf)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_NumHoles, ul_HoleSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_ComputeMemoryHoleInfo(&ul_NumHoles, &ul_HoleSize, FALSE);
	sprintf
	(
		asz_Buf,
		"Engine %u ko -- Textures %u ko -- WH %u ko -- Gran %u ko -- Holes % u -- Size Holes %u ko",
		MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated / 1024,
		MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated / 1024,
		(MEM_uGetAllocatedBlockSize(&MEM_gst_MemoryInfo)) / 1024,
		MEM_gst_MemoryInfo.ul_HolesStatic / 1024,
		ul_NumHoles,
		ul_HoleSize / 1024
	);

}


#endif