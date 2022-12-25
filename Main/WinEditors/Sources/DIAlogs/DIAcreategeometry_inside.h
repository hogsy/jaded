/*$T DIAcreategeometry_inside.h GC! 1.081 03/23/01 15:57:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __DIACREATEGEOMETRYINSIDE_H__
#define __DIACREATEGEOMETRYINSIDE_H__

#pragma once
#ifdef ACTIVE_EDITORS

#include "BASe/BAStypes.h"
#include "GEOmetric/GEOcreateobject.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_CreateGeometry;

class EDIA_cl_CreateGeometryView : public CFormView
{
	DECLARE_DYNCREATE(EDIA_cl_CreateGeometryView)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_CreateGeometryView(void);
	~EDIA_cl_CreateGeometryView();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_CreateGeometry      *mpo_Dialog;
    char                        msz_Path[ BIG_C_MaxLenPath ];
    char                        msz_Name[ BIG_C_MaxLenName ];

    GEO_tdst_CO_Text            mst_TextData;
    GEO_tdst_CO_Plane           mst_PlaneData;
    GEO_tdst_CO_Box             mst_BoxData;
    GEO_tdst_CO_Cylindre        mst_CylindreData;
    GEO_tdst_CO_Sphere          mst_SphereData;
    GEO_tdst_CO_Geosphere       mst_GeosphereData;

    int                         mi_Create;
    int                         mi_Object;
    struct GEO_tdst_Object_     *mpst_Object;
	struct COL_tdst_Cob_		*mpst_Cob;

    BOOL                        mb_UserName;


/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	Create(LPCTSTR, LPCTSTR, DWORD, const RECT &, CWnd *);

    void    Init( void );
    void    Update_Name( void );
    void    Generate_Name( int );
    void    Update_Create( int  );
    void    Update_GameObject( void );
    void    Update_ItemCallback( void );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Message headers
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    void afx_msg    OnButtonText( void );
    void afx_msg    OnButtonPlane( void );
    void afx_msg    OnButtonBox( void );
    void afx_msg    OnButtonCylindre( void );
    void afx_msg    OnButtonSphere( void );
    void afx_msg    OnButtonGeosphere( void );
    void afx_msg    OnButtonCreate( void );
    void afx_msg    OnButtonBrowsePath( void );
    void afx_msg    OnEditName( void );
	void afx_msg	OnCbnSelchangeCombo( void );

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */

#endif /* __DIACREATEGEOMETRYINSIDE_H__ */
