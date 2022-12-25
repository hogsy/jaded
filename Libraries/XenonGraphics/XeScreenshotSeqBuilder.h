// ------------------------------------------------------------------------------------------------
// File   : XeScreenshotSeqBuilder.h
// Date   : 2005-07-28
// Author : Etienne Fournier
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef XESCREENSHOTSEQBUILDER_H
#define XESCREENSHOTSEQBUILDER_H

#include <vector>
#include <algorithm>

#define XE_MAXNAMELENGTH 64

// ------------------------------------------------------------------------------------------------
// CLASSES
// ------------------------------------------------------------------------------------------------
class XeScreenshotSeqBuilder
{
	typedef struct XeScreenshotInfo_
	{
		MATH_tdst_Matrix st_Matrix;
		char			 sz_Name[XE_MAXNAMELENGTH];
	} XeScreenshotInfo;

public:

	XeScreenshotSeqBuilder(void);
	~XeScreenshotSeqBuilder(void);

	BOOL Load(char* _sz_path);

#if defined(ACTIVE_EDITORS)
	BOOL Save(char* _sz_path);

	void Add   (MATH_tdst_Matrix* _pst_Matrix, char* _szName = NULL);
	void Delete(int _iPos);

	void Clear (void);

	void Up    (int _iPos);
	void Down  (int _iPos);

	char* GetScreenshotName  (int _iPos);
	void  SetScreenshotName  (int _iPos, char* _szName);
#endif
	
	int  GetNbScreenshot();
	MATH_tdst_Matrix* GetScreenshotMatrix(int _iPos);

private:
	std::vector<XeScreenshotInfo>			m_vScreenshotInfo;
	std::vector<XeScreenshotInfo>::iterator m_It;
};

// ------------------------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------------------------
extern XeScreenshotSeqBuilder g_oXeScreenshotSeqBuilder;

#endif // XESCREENSHOTSEQBUILDER_H