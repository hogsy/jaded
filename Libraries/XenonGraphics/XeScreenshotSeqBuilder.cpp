// ------------------------------------------------------------------------------------------------
// File   : XeScreenshotSeqBuilder.cpp
// Date   : 2005-07-28
// Author : Etienne Fournier
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"
#include "XeScreenshotSeqBuilder.h"

// ------------------------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------------------------
XeScreenshotSeqBuilder g_oXeScreenshotSeqBuilder;

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

XeScreenshotSeqBuilder::XeScreenshotSeqBuilder(void)
{

}

/*
=======================================================================================================================
=======================================================================================================================
*/
XeScreenshotSeqBuilder::~XeScreenshotSeqBuilder(void)
{

}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL XeScreenshotSeqBuilder::Load(char* _asz_Path)
{
	L_FILE hFile;
	ULONG ulNbStruct;
	XeScreenshotInfo st_ScrenshotInfo;

	hFile = L_fopen(_asz_Path, L_fopen_RB);
	if(!hFile) return FALSE;

	m_vScreenshotInfo.clear();

	//read structure number
	L_freadA(&ulNbStruct, sizeof(ULONG), 1, hFile);

	//read all screenshot info
	for(int i = 0; i < (int)ulNbStruct; i++)
	{
		L_freadA(&st_ScrenshotInfo, sizeof(XeScreenshotInfo), 1, hFile);
		m_vScreenshotInfo.push_back(st_ScrenshotInfo);
	}

	return TRUE;
}

#if defined(ACTIVE_EDITORS)
/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL XeScreenshotSeqBuilder::Save(char* _asz_Path)
{
	XeScreenshotInfo st_Temp;
	L_FILE hFile;

	ULONG ulNbStruct;

	hFile = L_fopen(_asz_Path, L_fopen_WB);
	if(!hFile) return FALSE;

	//structure number
	ulNbStruct = (ULONG)m_vScreenshotInfo.size();
	L_fwriteA((void *) &ulNbStruct, sizeof(ULONG), 1, hFile);

	// save all screenshot info
	for(int i = 0; i < (int)m_vScreenshotInfo.size(); i++)
	{
		st_Temp = m_vScreenshotInfo[i];
		L_fwriteA((void *) &st_Temp, sizeof(XeScreenshotInfo), 1, hFile);
	}

	L_fclose(hFile);

	return TRUE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void XeScreenshotSeqBuilder::Add(MATH_tdst_Matrix* _pst_Matrix, char* _szName)
{
	XeScreenshotInfo st_Temp;

	if(_pst_Matrix)
	{
		st_Temp.st_Matrix = *_pst_Matrix;

		if(_szName && (strlen(_szName) != -1))
			strcpy(st_Temp.sz_Name, _szName);
		else
			sprintf(st_Temp.sz_Name, "Screenshot #%d", (GetNbScreenshot() + 1));

		m_vScreenshotInfo.push_back(st_Temp);
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void XeScreenshotSeqBuilder::Delete(int _iPos)
{
	if(!m_vScreenshotInfo.empty() && _iPos < (int)m_vScreenshotInfo.size())
	{
		m_It = m_vScreenshotInfo.begin() +_iPos;

		//erase matrix
		m_vScreenshotInfo.erase(m_It);
	}
}

/*
=======================================================================================================================
=======================================================================================================================

*/void XeScreenshotSeqBuilder::Clear()
{
	m_vScreenshotInfo.clear();
}

/*
=======================================================================================================================
=======================================================================================================================

*/void XeScreenshotSeqBuilder::Up(int _iPos)
{
	if(_iPos > 0 && _iPos < (int)m_vScreenshotInfo.size())
	{
		m_It = m_vScreenshotInfo.begin() + _iPos;
		iter_swap(m_It, m_It - 1);
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void XeScreenshotSeqBuilder::Down(int _iPos)
{
	if(_iPos < (int)m_vScreenshotInfo.size() - 1)
	{
		m_It = m_vScreenshotInfo.begin() + _iPos;
		iter_swap(m_It, m_It + 1);
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
char* XeScreenshotSeqBuilder::GetScreenshotName(int _iPos)
{
	if(_iPos < (int)m_vScreenshotInfo.size())
	{
		return m_vScreenshotInfo[_iPos].sz_Name;
	}
	else
		return NULL;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void XeScreenshotSeqBuilder::SetScreenshotName(int _iPos, char* _szName)
{
	if(_iPos < (int)m_vScreenshotInfo.size())
	{
		strcpy(m_vScreenshotInfo[_iPos].sz_Name, _szName);
	}
}
#endif

/*
=======================================================================================================================
=======================================================================================================================
*/
int XeScreenshotSeqBuilder::GetNbScreenshot()
{
	return (int)m_vScreenshotInfo.size();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
MATH_tdst_Matrix* XeScreenshotSeqBuilder::GetScreenshotMatrix(int _iPos)
{
	if(_iPos < (int)m_vScreenshotInfo.size())
	{
		return &m_vScreenshotInfo[_iPos].st_Matrix;
	}
	else
		return NULL;
}

