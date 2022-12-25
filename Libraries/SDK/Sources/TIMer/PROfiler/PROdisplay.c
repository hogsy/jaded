/*$T PROdisplay.c GC!1.67 01/03/00 11:54:18 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "TIMer/PROfiler/PROdisplay.h"
#include "ENGine/Sources/ENGvars.h"
#include "TIMer/TIMdefs.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIdebug.h"

#include "AIinterp/Sources/AImsg.h"
extern AI_tdst_GlobalMessageList	gast_GlobalMessages[C_GM_MaxTypes];

#if defined(_XBOX) || defined(_XENON)
#include "Xbox\PRO_xb.h"
#endif // _XBOX

#ifdef RASTERS_ON

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

struct PRO_tdst_TrameRaster_    *mpst_CurrentCategory = NULL;
struct PRO_tdst_TrameRaster_    *mpst_CurrentSubCategory = NULL;
struct PRO_tdst_TrameRaster_    *mpst_CurrentName = NULL;
int	PRO_TrameCounter = 0;
#define RASTER_TIME_WINDOW 60
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef PSX2_TARGET

void ExtTextOut(int x, int y, char * lpString) {
    printf("%s\n", lpString);
}

int PRO_i_DisplayOneRaster(int _i_Y, PRO_tdst_TrameRaster *_pst_Current, BOOL _b_FirstName)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Temp[512];
    float   f_Current;
    int     i_X;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    i_X = 10;


#define ESPACE  13


    if(mpst_CurrentSubCategory == NULL)
    {
        if(_b_FirstName)
        {
            ExtTextOut
            (
                i_X,
                _i_Y,
                _pst_Current->psz_SubCategory
            );

            if(mpst_CurrentName == NULL)
            {
                _i_Y += ESPACE;
            }
        }

        i_X += 10;
        if(mpst_CurrentName)
        {
            i_X += 60;
        }

        ExtTextOut(i_X, _i_Y,_pst_Current->psz_Name);
    }
    else
    {
        ExtTextOut(i_X, _i_Y, _pst_Current->psz_Name);
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        User data raster. Only display value
     -------------------------------------------------------------------------------------------------------------------
     */
	if(_pst_Current->e_Type == PRO_E_Float)
    {
        sprintf(asz_Temp, "%.5f            ", *(float*)&(_pst_Current->ul_UserData));
    }
	else if(_pst_Current->e_Type == PRO_E_Long)
    {
        sprintf(asz_Temp, "%d            ", _pst_Current->ul_UserData);
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Engine table raster
     -------------------------------------------------------------------------------------------------------------------
     */
    else if(_pst_Current->e_Type == PRO_E_Table)
    {
        return _i_Y + ESPACE;
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Global messages
     -------------------------------------------------------------------------------------------------------------------
     */
    else if(_pst_Current->e_Type == PRO_E_MsgGlob)
    {
        return _i_Y + ESPACE;
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        String raster. User data is a pointer on it
     -------------------------------------------------------------------------------------------------------------------
     */
    else if(_pst_Current->e_Type == PRO_E_String)
        L_strcpy(asz_Temp, (char *) _pst_Current->ul_UserData);

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Else display current and max raster value
     -------------------------------------------------------------------------------------------------------------------
     */
    else
    {
        f_Current = (float) _pst_Current->ul_CurrentValue / (float) TIM_gul_TicksPerSecond;
		_pst_Current->ul_CurrentValue = 0;

        /* Monitor Hz */
        f_Current *= TIM_gf_SynchroFrequency;
        sprintf(asz_Temp, "  %.3f                 ", f_Current);
    }

    ExtTextOut(i_X + 150, _i_Y, asz_Temp);

    /* No more need to display that raster */
    _pst_Current->b_NeedDisplay = FALSE;

    return _i_Y + ESPACE;
}

#else /* PSX2_TARGET */

#ifdef _GAMECUBE
#else


#if defined(_XBOX) || defined(_XENON)

void ExtTextOut(int _x, int _y, WCHAR* _lpString)
{
#if !defined(_XENON)
	// print in a 2nd column if necessary
	if (_y > 350)
	{
		_y -= 350;
		_x += 300;
	}
	// draw text
	XB_fn_vDrawText (_x, _y + 80, _lpString);
#endif
}

int PRO_i_DisplayOneRaster (int _i_Y, PRO_tdst_TrameRaster *_pst_Current, BOOL _b_FirstName)
{
	//~~~~~~~~~~~~~~~~~~~~
	WCHAR   asz_Temp[512];
	float   f_Current;
	int     i_X;
	//~~~~~~~~~~~~~~~~~~~~

	i_X = 40;

#define ESPACE 20

	if(mpst_CurrentSubCategory == NULL)
	{
		if(_b_FirstName)
		{
			swprintf (asz_Temp, L"%S", _pst_Current->psz_SubCategory);
			ExtTextOut (i_X, _i_Y, asz_Temp);
			_i_Y += ESPACE;
		}
		
		i_X += 10;
		if(mpst_CurrentName)
			i_X += 60;

		swprintf (asz_Temp, L"%S", _pst_Current->psz_Name);
		ExtTextOut (i_X, _i_Y, asz_Temp);
	}
	else
	{
		swprintf (asz_Temp, L"%S", _pst_Current->psz_Name);
		ExtTextOut (i_X, _i_Y, asz_Temp);
	}
	
	/*$2
	-------------------------------------------------------------------------------------------------------------------
	Engine table raster
	-------------------------------------------------------------------------------------------------------------------
	*/
	if(_pst_Current->e_Type == PRO_E_Table)
		return _i_Y + ESPACE;

	/*$2
	-------------------------------------------------------------------------------------------------------------------
	Global messages
	-------------------------------------------------------------------------------------------------------------------
	*/
	if(_pst_Current->e_Type == PRO_E_MsgGlob)
		return _i_Y + ESPACE;

	/*$2
	-------------------------------------------------------------------------------------------------------------------
	User data raster. Only display value
	-------------------------------------------------------------------------------------------------------------------
	*/
	
	if(_pst_Current->e_Type == PRO_E_Float)
		swprintf(asz_Temp,  L"%.5f            ", *(float*)&(_pst_Current->ul_UserData));
	else if(_pst_Current->e_Type == PRO_E_Long)
		swprintf(asz_Temp, L"%d            ", _pst_Current->ul_UserData);

	/*$2
	-------------------------------------------------------------------------------------------------------------------
	String raster. User data is a pointer on it
	-------------------------------------------------------------------------------------------------------------------
	*/
	else if(_pst_Current->e_Type == PRO_E_String)
		swprintf (asz_Temp, L"%s", (char *) _pst_Current->ul_UserData);
		
	/*$2
	-------------------------------------------------------------------------------------------------------------------
	Else display current and max raster value
	-------------------------------------------------------------------------------------------------------------------
	*/
	else
	{
		f_Current = (float) _pst_Current->ul_CurrentValue / (float) TIM_gul_TicksPerSecond;
		_pst_Current->ul_CurrentValue = 0;

		/* Monitor Hz */
		f_Current *= 60;
		swprintf(asz_Temp, L"  %.3f                 ", f_Current);
	}
	
	ExtTextOut(i_X + 170, _i_Y, asz_Temp);
	
	/* No more need to display that raster */
	_pst_Current->b_NeedDisplay = FALSE;
	
	return _i_Y + ESPACE;
}


#else // _XBOX

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ConvGao(char *az, OBJ_tdst_GameObject *pst_GAO)
{
	if(((int) pst_GAO) == -1)
	{
		sprintf(az, "INVALID REFERENCE");
	}
	else
	{
		if((int) pst_GAO == AI_C_MainActor0) pst_GAO = AI_gpst_MainActors[0];
		if((int) pst_GAO == AI_C_MainActor1) pst_GAO = AI_gpst_MainActors[1];
		if(pst_GAO && !OBJ_IsValidGAO(pst_GAO))
		{
			sprintf(az, "INVALID GAO");
		}
		else if(pst_GAO)
		{
			if(pst_GAO->sz_Name)
				sprintf(az, pst_GAO->sz_Name);
			else
				sprintf(az, "I'me here but without name...");
		}
		else
			sprintf(az, "None");
	}
}

#endif /* ACTIVE_EDITORS */

int PRO_i_DisplayOneRaster(HWND _hwnd, HDC _hdc, int _i_Y, PRO_tdst_TrameRaster *_pst_Current, BOOL _b_FirstName)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Temp[512];
    float   f_Current;
    int     i_X;
    RECT    o_Rect;
#ifdef ACTIVE_EDITORS
	RECT	rect1;
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    i_X = 10;

    GetClientRect(_hwnd, &o_Rect);

#define ESPACE  13

    /* Display sub/name. */
    SetBkColor(_hdc, GetSysColor(COLOR_BTNFACE));

    if(mpst_CurrentSubCategory == NULL)
    {
        if(_b_FirstName)
        {
            ExtTextOut
            (
                _hdc,
                i_X,
                _i_Y,
                0,
                NULL,
                _pst_Current->psz_SubCategory,
                L_strlen(_pst_Current->psz_SubCategory),
                NULL
            );

            if(mpst_CurrentName == NULL)
            {
                _i_Y += ESPACE;
            }
        }

        i_X += 10;
        if(mpst_CurrentName)
        {
            i_X += 60;
        }

        ExtTextOut(_hdc, i_X, _i_Y, 0, NULL, _pst_Current->psz_Name, L_strlen(_pst_Current->psz_Name), NULL);
    }
    else
    {
        ExtTextOut(_hdc, i_X, _i_Y, 0, NULL, _pst_Current->psz_Name, L_strlen(_pst_Current->psz_Name), NULL);
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        User data raster. Only display value
     -------------------------------------------------------------------------------------------------------------------
     */
	if(_pst_Current->e_Type == PRO_E_Float)
    {
        sprintf(asz_Temp, "%.5f            ", *(float*)&(_pst_Current->ul_UserData));
    }
	else if(_pst_Current->e_Type == PRO_E_Long)
    {
        sprintf(asz_Temp, "%d            ", _pst_Current->ul_UserData);
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Global messages
     -------------------------------------------------------------------------------------------------------------------
     */
    else if(_pst_Current->e_Type == PRO_E_MsgGlob)
    {
#ifdef ACTIVE_EDITORS
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int					i;
		AI_tdst_Message		*pst_Msg;
		char				asz_Msg1[1000];
		char				asz_Msg2[256];
		char				asz_Msg3[256];
		char				asz_Msg4[256];
		char				asz_Msg5[256];
		char				asz_Msg6[256];
		char				asz_Msg7[256];
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(i = 0; i < gast_GlobalMessages[_pst_Current->ul_UserData].num; i++, pst_Msg++)
		{
			if(gast_GlobalMessages[_pst_Current->ul_UserData].msg[i].flags & C_GM_Deleted) continue;
			pst_Msg = &gast_GlobalMessages[_pst_Current->ul_UserData].msg[i].msg;
			SetBkColor(_hdc, GetSysColor(COLOR_BTNFACE));
			rect1.left = i_X + 5;
			rect1.top = _i_Y;
			rect1.right = i_X + 500;
			rect1.bottom = rect1.top + ESPACE;

			ConvGao(asz_Msg2, pst_Msg->pst_Sender);
			ConvGao(asz_Msg3, pst_Msg->pst_GAO1);
			ConvGao(asz_Msg4, pst_Msg->pst_GAO2);
			ConvGao(asz_Msg5, pst_Msg->pst_GAO3);
			ConvGao(asz_Msg6, pst_Msg->pst_GAO4);
			ConvGao(asz_Msg7, pst_Msg->pst_GAO5);

			sprintf
			(
				asz_Msg1, 
				"%d | %s %s | %s | %s | %s | %s %.3f,%.3f,%.3f %.3f,%.3f,%.3f %.3f,%.3f,%.3f %.3f,%.3f,%.3f %.3f,%.3f,%.3f %d | %d | %d | %d | %d", 
				pst_Msg->i_Id,
				asz_Msg2, /* sender */
				asz_Msg3,
				asz_Msg4,
				asz_Msg5,
				asz_Msg6,
				asz_Msg7,
				pst_Msg->st_Vec1.x, 
				pst_Msg->st_Vec1.y, 
				pst_Msg->st_Vec1.z,
				pst_Msg->st_Vec2.x, 
				pst_Msg->st_Vec2.y, 
				pst_Msg->st_Vec2.z,
				pst_Msg->st_Vec3.x, 
				pst_Msg->st_Vec3.y, 
				pst_Msg->st_Vec3.z,
				pst_Msg->st_Vec4.x, 
				pst_Msg->st_Vec4.y, 
				pst_Msg->st_Vec4.z,
				pst_Msg->st_Vec5.x, 
				pst_Msg->st_Vec5.y, 
				pst_Msg->st_Vec5.z,
				pst_Msg->i_Int1,
				pst_Msg->i_Int2,
				pst_Msg->i_Int3,
				pst_Msg->i_Int4,
				pst_Msg->i_Int5
			);       

			ExtTextOut(_hdc, i_X + 10, _i_Y, ETO_OPAQUE, &rect1, asz_Msg1, L_strlen(asz_Msg1), NULL);
            _i_Y += ESPACE;
		}
#endif
        return _i_Y + ESPACE;
	}

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Engine table raster
     -------------------------------------------------------------------------------------------------------------------
     */
    else if(_pst_Current->e_Type == PRO_E_Table)
    {
#ifdef ACTIVE_EDITORS
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        TAB_tdst_PFtable    *pst_EOT;
        TAB_tdst_PFelem     *pst_CurrentElem;
        TAB_tdst_PFelem     *pst_EndElem;
        OBJ_tdst_GameObject *pst_GO;
        int                 x;
		char				az_Tmp[200];
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        pst_EOT = (TAB_tdst_PFtable *) _pst_Current->ul_UserData;
        pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_EOT);
        pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_EOT);
        x = 0;
        for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
        {
            pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
            if(TAB_b_IsAHole(pst_GO)) continue;

			SetBkColor(_hdc, GetSysColor(COLOR_BTNFACE));
			rect1.left = i_X + 150 + x;
			rect1.top = _i_Y;
			rect1.right = i_X + x + 290;
			rect1.bottom = rect1.top + ESPACE;
			L_strcpy(az_Tmp, pst_GO->sz_Name);
			if(L_strrchr(az_Tmp, '.')) *L_strrchr(az_Tmp, '.') = 0;
            ExtTextOut(_hdc, i_X + 150 + x, _i_Y, ETO_CLIPPED | ETO_OPAQUE, &rect1, az_Tmp, L_strlen(az_Tmp), NULL);

            x += 150;
            if(pst_CurrentElem != pst_EndElem)
            {
                if(o_Rect.right - (i_X + 150 + x) < 150)
                {
                    x = 0;
                    _i_Y += ESPACE;
					rect1.left = 0;
					rect1.top = _i_Y;
					rect1.right = 150;
					rect1.bottom = rect1.top + ESPACE;
					ExtTextOut(_hdc, 0, _i_Y, ETO_OPAQUE, &rect1, "", 0, NULL);
                }
            }
        }

        _pst_Current->b_NeedDisplay = FALSE;
#endif
        return _i_Y + ESPACE;
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        String raster. User data is a pointer on it
     -------------------------------------------------------------------------------------------------------------------
     */
    else if(_pst_Current->e_Type == PRO_E_String)
        L_strcpy(asz_Temp, (char *) _pst_Current->ul_UserData);

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Else display current and max raster value
     -------------------------------------------------------------------------------------------------------------------
     */
    else
    {
		extern float TIM_gf_SynchroFrequency;

		if(_pst_Current->ul_CurrentValue == 0)
		{
			f_Current = (float) _pst_Current->ul_PrevValue / (float) TIM_gul_TicksPerSecond;
		}
		else
		{
			f_Current = (float) _pst_Current->ul_CurrentValue / (float) TIM_gul_TicksPerSecond;
			_pst_Current->ul_PrevValue = _pst_Current->ul_CurrentValue;

            // Manage min, max and average for RASTER_TIME_WINDOW frames
            _pst_Current->f_AverageValue += (float)_pst_Current->ul_CurrentValue;
            if (_pst_Current->ul_MinValue > _pst_Current->ul_CurrentValue) _pst_Current->ul_MinValue = _pst_Current->ul_CurrentValue;
            if (_pst_Current->ul_MaxValue < _pst_Current->ul_CurrentValue) _pst_Current->ul_MaxValue = _pst_Current->ul_CurrentValue;

            _pst_Current->ul_ValueNb++;
            if (_pst_Current->ul_ValueNb >= RASTER_TIME_WINDOW)
            {
                _pst_Current->ul_ValueNb = 0; 
                _pst_Current->f_PrevMin = (float) _pst_Current->ul_MinValue;
                _pst_Current->f_PrevMax = (float) _pst_Current->ul_MaxValue;
                _pst_Current->f_PrevAverage = _pst_Current->f_AverageValue/(float)RASTER_TIME_WINDOW;
                _pst_Current->ul_MinValue = 0xFFFFFFFF;
                _pst_Current->ul_MaxValue = 0;
                _pst_Current->f_AverageValue = 0.f;
            }

			_pst_Current->ul_CurrentValue = 0;
		}

        /* Monitor Hz */
        f_Current *= TIM_gf_SynchroFrequency;

        // Use history to display more info
        {
        float fAverage,fMin,fMax,fTemp;

        fTemp = TIM_gf_SynchroFrequency / (float) (TIM_gul_TicksPerSecond);
        fAverage = fTemp * _pst_Current->f_PrevAverage;
        fMin = fTemp * _pst_Current->f_PrevMin;
        fMax = fTemp * _pst_Current->f_PrevMax;

        sprintf(asz_Temp, "  %.3f  %.3f  %.3f  %.3f ", f_Current,fMin,fAverage,fMax);
        }
    }

    ExtTextOut(_hdc, i_X + 150, _i_Y, 0, NULL, asz_Temp, L_strlen(asz_Temp), NULL);

    /* No more need to display that raster */
    _pst_Current->b_NeedDisplay = FALSE;

    return _i_Y + ESPACE;
}

#endif /* _GAMECUBE */
#endif /* PSX2_TARGET */




/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if (defined( PSX2_TARGET) || defined (_GAMECUBE))
int PRO_OneTrameEnding(int i_Y)
{

#ifdef _GAMECUBE
#pragma unused(i_Y)
#endif

    return 0;
}
#else

#if defined(_XBOX) || defined(_XENON)
int PRO_OneTrameEnding(int i_Y)
#else
int PRO_OneTrameEnding(HWND hwnd, HDC _hdc, int i_Y)
#endif

{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    PRO_tdst_TrameRaster    *_pst_Current, *pst_Current1, *pst_Current2;
    BOOL                    b_First, b_MustBreak;;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* No raster to display */
	PRO_TrameCounter++;
    if(mpst_CurrentCategory == NULL) return i_Y;

    /* Display all subcategories. */
    _pst_Current = _PRO_gpst_FirstTrameRaster;
    b_MustBreak = FALSE;
    while(_pst_Current)
    {
        /* Get the good category */
        if
        (
            (_pst_Current->b_NeedDisplay) &&
            (!L_strcmpi(_pst_Current->psz_Category, mpst_CurrentCategory->psz_Category))
        )
        {
            pst_Current1 = _pst_Current;
            while(pst_Current1)
            {
                if
                (
                    (pst_Current1->b_NeedDisplay) &&
                    (!L_strcmpi(pst_Current1->psz_Category, mpst_CurrentCategory->psz_Category)) &&
                    (
                        (mpst_CurrentSubCategory == NULL) ||
                        (!L_strcmpi(pst_Current1->psz_SubCategory, mpst_CurrentSubCategory->psz_SubCategory))
                    )
                )
                {
                    pst_Current2 = pst_Current1;
                    while(pst_Current2)
                    {
                        if
                        (
                            (pst_Current2->b_NeedDisplay) &&
                            (!L_strcmpi(pst_Current2->psz_Category, mpst_CurrentCategory->psz_Category)) &&
                            (!L_strcmpi(pst_Current2->psz_SubCategory, pst_Current1->psz_SubCategory)) &&
                            (
                                (mpst_CurrentName == NULL) ||
                                (!L_strcmpi(pst_Current2->psz_Name, mpst_CurrentName->psz_Name))
                            )
                        )
                        {
                            b_First = mpst_CurrentName || (pst_Current2 == pst_Current1);
#if defined(PSX2_TARGET) || defined (_XBOX) || defined(_XENON)
                            i_Y = PRO_i_DisplayOneRaster(i_Y, pst_Current2, b_First);
#else
                            i_Y = PRO_i_DisplayOneRaster(hwnd, _hdc, i_Y, pst_Current2, b_First);
#endif
                            if(mpst_CurrentName)
                            {
                                b_MustBreak = TRUE;
                                break;
                            }
                        }

                        pst_Current2 = pst_Current2->pst_NextRaster;
                    }

                    if(b_MustBreak && mpst_CurrentCategory)
                    {
                        break;
                    }
                }

                pst_Current1 = pst_Current1->pst_NextRaster;
            }

            /* Break if just one sub and one name */
            if(b_MustBreak && mpst_CurrentCategory)
            {
                break;
            }
        }

        _pst_Current->b_NeedDisplay = TRUE;
        _pst_Current = _pst_Current->pst_NextRaster;
    }

    return i_Y;
}
#endif
#endif // _XBOX
#endif /* RASTERS_ON */