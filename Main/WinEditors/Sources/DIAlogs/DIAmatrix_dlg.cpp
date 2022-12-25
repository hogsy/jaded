/*$T DIAmatrix_dlg.cpp GC! 1.098 12/05/00 16:26:03 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAmatrix_dlg.h"
#include "LINKs/LINKtoed.h"
#include "Res/Res.h"

BEGIN_MESSAGE_MAP(EDIA_cl_MatrixDialog, EDIA_cl_BaseDialog)
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_ANGLEH, OnAngleH)
	ON_EN_KILLFOCUS(IDC_ANGLEV, OnAngleV)
    ON_EN_KILLFOCUS(IDC_ANGLEX, OnAngleX)
    ON_EN_KILLFOCUS(IDC_ANGLEY, OnAngleY)
    ON_EN_KILLFOCUS(IDC_ANGLEZ, OnAngleZ)
END_MESSAGE_MAP()
/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MatrixDialog::EDIA_cl_MatrixDialog(void) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_EVAV_MATRIX)
{
        mb_First = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MatrixDialog::OnInitDialog(void)
{
	// Save
	L_memcpy( &mst_SavedMatrix, mpst_Matrix, sizeof(MATH_tdst_Matrix) );

	return EDIA_cl_BaseDialog::OnInitDialog();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MatrixDialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				asz_Temp[100];
	CString				o_String;
	MATH_tdst_Vector	v1, v2;
	float				v, h, temp;
    MATH_tdst_Matrix    Rot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_gb_CanRefresh = TRUE;
	CDialog::DoDataExchange(pDX);
	if(!pDX->m_bSaveAndValidate)
	{
		sprintf(asz_Temp, "%.4f", mpst_Matrix->Ix);
		DDX_Text(pDX, IDC_EDITIX, CString(asz_Temp));
		sprintf(asz_Temp, "%.4f", mpst_Matrix->Iy);
		DDX_Text(pDX, IDC_EDITIY, CString(asz_Temp));
		sprintf(asz_Temp, "%.4f", mpst_Matrix->Iz);
		DDX_Text(pDX, IDC_EDITIZ, CString(asz_Temp));

		sprintf(asz_Temp, "%.4f", mpst_Matrix->Jx);
		DDX_Text(pDX, IDC_EDITJX, CString(asz_Temp));
		sprintf(asz_Temp, "%.4f", mpst_Matrix->Jy);
		DDX_Text(pDX, IDC_EDITJY, CString(asz_Temp));
		sprintf(asz_Temp, "%.4f", mpst_Matrix->Jz);
		DDX_Text(pDX, IDC_EDITJZ, CString(asz_Temp));

		sprintf(asz_Temp, "%.4f", mpst_Matrix->Kx);
		DDX_Text(pDX, IDC_EDITKX, CString(asz_Temp));
		sprintf(asz_Temp, "%.4f", mpst_Matrix->Ky);
		DDX_Text(pDX, IDC_EDITKY, CString(asz_Temp));
		sprintf(asz_Temp, "%.4f", mpst_Matrix->Kz);
		DDX_Text(pDX, IDC_EDITKZ, CString(asz_Temp));

        if (mb_First)
        {
            mb_First = 0;

            MATH_GetRotationMatrix( &Rot, mpst_Matrix );
            temp = -Rot.Iz;
            if ( (temp <= -1) || (temp >= 1) )
            {
                mst_Rotation.y = (temp <= -1) ? -Cf_PiBy2 : Cf_PiBy2;
                mst_Rotation.x = 0;
                
                temp = Rot.Jy;
                if (temp > 1) temp = 1;
                else if (temp < -1) temp = -1;
                mst_Rotation.x = fAcos( temp );
                if ( Rot.Ky < 0 )
                    mst_Rotation.x = -mst_Rotation.x;
            }
            else
            {
                mst_Rotation.y = fAsin( temp );

                temp = Rot.Ix / fCos( mst_Rotation.y);
                if (temp > 1) temp = 1;
                else if (temp < -1) temp = -1;
                mst_Rotation.x = fAcos( temp );
                if ( ( Rot.Jx / fCos( mst_Rotation.y)) < 0 )
                    mst_Rotation.x = -mst_Rotation.x;

                temp = Rot.Kz / fCos( mst_Rotation.y);
                if (temp > 1) temp = 1;
                else if (temp < -1) temp = -1;
                mst_Rotation.z = fAcos( temp );
                if ( ( Rot.Ky / fCos( mst_Rotation.y)) < 0)
                    mst_Rotation.z = -mst_Rotation.z;
            }
        }

        MATH_ScaleEqualVector( &mst_Rotation, (180.0f / Cf_Pi) );
        sprintf(asz_Temp, "%.4f", mst_Rotation.x);
        DDX_Text(pDX, IDC_ANGLEX, CString(asz_Temp));
        sprintf(asz_Temp, "%.4f", mst_Rotation.y);
        DDX_Text(pDX, IDC_ANGLEY, CString(asz_Temp));
        sprintf(asz_Temp, "%.4f", mst_Rotation.z);
        DDX_Text(pDX, IDC_ANGLEZ, CString(asz_Temp));


		MATH_CopyVector(&v1, MATH_pst_GetYAxis(mpst_Matrix));
		MATH_CopyVector(&v2, &v1);
		v2.z = 0;
		MATH_NormalizeVector(&v2, &v2);
		v = MATH_f_VecAngle(&v1, &v2, &MATH_gst_BaseVectorI);
//		if(v1.z > 0) v = -v;

		h = MATH_f_VecAngle(&v2, &MATH_gst_BaseVectorJ, &MATH_gst_BaseVectorK);

		v = (v * 180) / Cf_Pi;
		h = (h * 180) / Cf_Pi;
		sprintf(asz_Temp, "%.4f", v);
		DDX_Text(pDX, IDC_ANGLEV, CString(asz_Temp));
		sprintf(asz_Temp, "%.4f", h);
		DDX_Text(pDX, IDC_ANGLEH, CString(asz_Temp));
	}
	else
	{
		DDX_Text(pDX, IDC_EDITIX, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mpst_Matrix->Ix);
		DDX_Text(pDX, IDC_EDITIY, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mpst_Matrix->Iy);
		DDX_Text(pDX, IDC_EDITIZ, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mpst_Matrix->Iz);

		DDX_Text(pDX, IDC_EDITJX, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mpst_Matrix->Jx);
		DDX_Text(pDX, IDC_EDITJY, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mpst_Matrix->Jy);
		DDX_Text(pDX, IDC_EDITJZ, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mpst_Matrix->Jz);

		DDX_Text(pDX, IDC_EDITKX, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mpst_Matrix->Kx);
		DDX_Text(pDX, IDC_EDITKY, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mpst_Matrix->Ky);
		DDX_Text(pDX, IDC_EDITKZ, o_String);
		sscanf((char *) (LPCSTR) o_String, "%f", &mpst_Matrix->Kz);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Rotate(MATH_tdst_Vector *p, MATH_tdst_Vector *v, MATH_tdst_Vector *axe, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	W, st_Vec2;
	float				fd, f_Norm2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	fd = MATH_f_DotProduct(axe, v);
	f_Norm2 = MATH_f_DotProduct(axe, axe);

	fd /= f_Norm2;
	MATH_ScaleVector(&W, axe, fd);
	MATH_SubEqualVector(v, &W);
	MATH_ScaleVector(p, v, fCos(f));
	MATH_NormalizeEqualVector(axe);
	MATH_CrossProduct(&st_Vec2, v, axe);
	MATH_AddScaleVector(p, p, &st_Vec2, fSin(f));
	MATH_AddEqualVector(p, &W);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MatrixDialog::MakeRotation(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CEdit				*pedit;
	MATH_tdst_Vector	vec, vy, vyy;
	CString				o;
	float				h, v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pedit = (CEdit *) GetDlgItem(IDC_ANGLEH);
	pedit->GetWindowText(o);
	sscanf((char *) (LPCSTR) o, "%f", &h);
	pedit = (CEdit *) GetDlgItem(IDC_ANGLEV);
	pedit->GetWindowText(o);
	sscanf((char *) (LPCSTR) o, "%f", &v);

	h = (h * Cf_Pi) / 180;
	v = (v * Cf_Pi) / 180;

	Rotate(&vy, &MATH_gst_BaseVectorJ, &MATH_gst_BaseVectorK, h);
	MATH_CrossProduct(&vec, &vy, &MATH_gst_BaseVectorK);
	MATH_CopyVector(&vyy, &vy);
	Rotate(&vy, &vyy, &vec, v);

	MATH_CopyVector(MATH_pst_GetYAxis(mpst_Matrix), &vy);
	MATH_CopyVector(MATH_pst_GetXAxis(mpst_Matrix), &vec);
	MATH_CrossProduct(MATH_pst_GetZAxis(mpst_Matrix), &vec, &vy);

    mb_First = 1;
	UpdateData(FALSE);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MatrixDialog::MakeRotationWithXYZ( void )
{
    char    sz_Value[16];
    float   cosx, sinx, cosy, siny, cosz, sinz;

    GetDlgItem( IDC_ANGLEX)->GetWindowText( sz_Value, 15 );
    mst_Rotation.x = (float) atof( sz_Value );
    GetDlgItem( IDC_ANGLEY)->GetWindowText( sz_Value, 15 );
    mst_Rotation.y = (float) atof( sz_Value );
    GetDlgItem( IDC_ANGLEZ)->GetWindowText( sz_Value, 15 );
    mst_Rotation.z = (float) atof( sz_Value );

    MATH_ScaleEqualVector( &mst_Rotation, (Cf_Pi / 180) );

    cosx = fCos( mst_Rotation.x );
    sinx = fSin( mst_Rotation.x );
    cosy = fCos( mst_Rotation.y );
    siny = fSin( mst_Rotation.y );
    cosz = fCos( mst_Rotation.z );
    sinz = fSin( mst_Rotation.z );

    mpst_Matrix->Ix = cosy * cosz;
    mpst_Matrix->Iy = cosy * sinz;
    mpst_Matrix->Iz = -siny;
    mpst_Matrix->Jx = sinx * siny * cosz - cosx * sinz;
    mpst_Matrix->Jy = sinx * siny * sinz + cosx * cosz;
    mpst_Matrix->Jz = sinx * cosy;
    mpst_Matrix->Kx = cosx * siny * cosz + sinx * sinz;
    mpst_Matrix->Ky = cosx * siny * sinz - sinx * cosz;
    mpst_Matrix->Kz = cosx * cosy;

    UpdateData(FALSE); 
    LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MatrixDialog::OnAngleH(void)
{
	MakeRotation();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MatrixDialog::OnAngleV(void)
{
	MakeRotation();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MatrixDialog::OnAngleX(void)
{
	MakeRotationWithXYZ();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MatrixDialog::OnAngleY(void)
{
	MakeRotationWithXYZ();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MatrixDialog::OnAngleZ(void)
{
	MakeRotationWithXYZ();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MatrixDialog::OnDestroy(void)
{
	L_memcpy( mpst_Matrix, &mst_SavedMatrix, sizeof(MATH_tdst_Matrix) );
}


#endif /* ACTIVE_EDITORS */
