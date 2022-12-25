/*$T DIAreplace_dlg.h GC! 1.078 03/16/00 10:29:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIACUBEMAPGEN_DLG_H__INCLUDED
#define UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIACUBEMAPGEN_DLG_H__INCLUDED

#define CUBEMAP_START_RES	256
#define CUBEMAP_MIN_RES		32
#define CUBEMAP_MAX_RES		512

#define CUBEMAP_SNAPSHOT_PATH	EDI_Csz_Path_Textures "/_Xenon"

#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "DIAlogs/NumEdit.h"

// -- Forward declaration --
class F3D_cl_View;
typedef struct	WOR_tdst_World_ WOR_tdst_World;
typedef struct	CAM_tdst_Camera_ CAM_tdst_Camera;
//// -- Forward declaration --


class EDIA_cl_CubeMapGenDialog : public EDIA_cl_BaseDialog
{

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	CONSTRUCT.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	EDIA_cl_CubeMapGenDialog (struct OBJ_tdst_GameObject_ *);
	~EDIA_cl_CubeMapGenDialog();

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	ATTRIBUTES.
	-----------------------------------------------------------------------------------------------------------------------
	*/
public:
	struct WOR_tdst_World_		*mpst_World;
	F3D_cl_View					*mpo_View;
	struct OBJ_tdst_GameObject_	*mpst_Gao;


private:
	int			mi_Size;
	CString		mstrFace[6];

	CNumEdit	GaoPosX;
	CNumEdit	GaoPosY;
	CNumEdit	GaoPosZ;

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	OVERWRITE.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	BOOL		OnInitDialog(void);
	BOOL		PreTranslateMessage( MSG * );
	void		ChangeGao(struct OBJ_tdst_GameObject_ *);

private:
	void	FillGaoInfo(void);
	void	FillTexNameInfo(void);
	void	FillTexResInfo(void);
	MATH_tdst_Vector GetGenerationPos(void);
	void	SetCameraMatrixForFace(CAM_tdst_Camera *, DWORD);
	BOOL	b_CreateCubeMap(CString, CString, BOOL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

/*$2
-----------------------------------------------------------------------------------------------------------------------
MESSAGE MAP.
-----------------------------------------------------------------------------------------------------------------------
*/

protected:
	afx_msg void OnSpinTexRes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBtOK(void);
	afx_msg void OnCheckGenMipMap(void);
	afx_msg void OnValidateGaoPos(void);
	DECLARE_MESSAGE_MAP()
};


#endif /* ACTIVE_EDITORS */

#endif //UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIACUBEMAPGEN_DLG_H__INCLUDED	