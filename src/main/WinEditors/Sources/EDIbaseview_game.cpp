/*$T EDIbaseview_game.cpp GC!1.41 07/29/99 15:32:58 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "MATHs/MATH.h"
#include "EDIbaseview.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseView::GameTest(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POINT   pt;
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((GetAsyncKeyState('C') < 0) && (GetAsyncKeyState('B') < 0) && !mb_GameKey)
    {
        GetCursorPos(&pt);
        GetWindowRect(&o_Rect);
        if(o_Rect.PtInRect(pt))
        {
            mi_GameOn = (mi_GameOn + 1) % 2;
            mb_GameKey = TRUE;

            switch(mi_GameOn)
            {
            case 0:
                Invalidate();
                break;
            case 1:
				mi_Score = 0;
                mast_GamePos[0].x = ((o_Rect.right - o_Rect.left) / 2) & 0xFFFFFFF0;
                mast_GamePos[0].y = ((o_Rect.bottom - o_Rect.top) / 2) & 0xFFFFFFF0;
                mst_GameMove.x = 0;
                mst_GameMove.y = 0;
                mst_GameMove.z = 100;
                mst_GamePos.x = 100 & 0xFFFFFFF0;
                mst_GamePos.y = 100 & 0xFFFFFFF0;
                mst_GamePos.z = 1.0f;
                Invalidate();
                break;
            }
        }
    }

    if((GetAsyncKeyState('C') >= 0) && (GetAsyncKeyState('B') >= 0)) mb_GameKey = FALSE;
    if(!GetItemCount())
    {
        switch(mi_GameOn)
        {
        case 1:
            GameTick1();
			break;
        }
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseView::GameTick1(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 x, y, i;
    CDC                 *pDC;
    CRect               o_Rect;
    POINT               st_Mouse;
    MATH_tdst_Vector    st_MouseMove;
    int                 it;
    char                asz[100];
    int                 iSpeed;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define SIZE2   8
#define SPEED   (2 * SIZE2)

    pDC = GetWindowDC();
    GetClientRect(&o_Rect);

/*    iSpeed = 400 - ((int) mst_GamePos.z * 10);
	iSpeed /= 7;
    if(iSpeed < 100) iSpeed = 100;
	if(GetAsyncKeyState(VK_SHIFT) < 0)*/
	{
		iSpeed = 5;
	}

    /* Get new mouse, and move vector */
    GetCursorPos(&st_Mouse);
    ScreenToClient(&st_Mouse);
    MATH_InitVector
    (
        &st_MouseMove,
        (float) st_Mouse.x - mst_GameLMouse.x,
        (float) st_Mouse.y - mst_GameLMouse.y,
        0
    );
    mst_GameLMouse.x = st_Mouse.x;
    mst_GameLMouse.y = st_Mouse.y;

    /* Detect mouse */
	if(GetAsyncKeyState(VK_LEFT) < 0) 
	{
		if(mst_GameMove.x != SPEED)
		{
			mst_GameMove.x = -SPEED;
			mst_GameMove.y = 0;
		}
	}
	else if(GetAsyncKeyState(VK_RIGHT) < 0) 
	{
		if(mst_GameMove.x != -SPEED)
		{
			mst_GameMove.x = SPEED;
			mst_GameMove.y = 0;
		}
	}
	if(GetAsyncKeyState(VK_UP) < 0) 
	{
		if(mst_GameMove.y != SPEED)
		{
			mst_GameMove.x = 0;
			mst_GameMove.y = -SPEED;
		}
	}
	else if(GetAsyncKeyState(VK_DOWN) < 0) 
	{
		if(mst_GameMove.y != -SPEED)
		{
			mst_GameMove.x = 0;
			mst_GameMove.y = SPEED;
		}
	}

    /* Score */
    sprintf(asz, "%d       ", mi_Score);

    pDC->SetTextColor(GetSysColor(COLOR_3DDKSHADOW));
    pDC->DrawText
        (
            asz,
            CRect(o_Rect.left + 3, o_Rect.top + 3, 20, 20),
            DT_NOCLIP | DT_TOP | DT_LEFT
        );

    /* Compute new pos */
    mst_GameMove.z++;
    if(mst_GameMove.z > iSpeed)
    {
        /* Erase last point */
        x = mast_GamePos[(int) mst_GamePos.z - 1].x;
        y = mast_GamePos[(int) mst_GamePos.z - 1].y;
        pDC->FillSolidRect(x, y, (2 * SIZE2), (2 * SIZE2), GetSysColor(COLOR_3DSHADOW));

        /* Move */
        L_memmove(&mast_GamePos[1], mast_GamePos, (int) mst_GamePos.z * sizeof(POINT));
        mast_GamePos[0].x = mast_GamePos[1].x + (int) mst_GameMove.x;
        mast_GamePos[0].y = mast_GamePos[1].y + (int) mst_GameMove.y;
        mst_GameMove.z = 0;

        /* Detect border */
        if(mast_GamePos[0].x < 16)
        {
end:
			pDC->SetTextColor(GetSysColor(COLOR_3DDKSHADOW));
			pDC->DrawText
				(
					"Press Space",
					CRect(o_Rect.left + 3, o_Rect.top + 20, 20, 20),
					DT_NOCLIP | DT_TOP | DT_LEFT
				);
			while(GetAsyncKeyState(VK_SPACE) >= 0);
            Invalidate();
            mast_GamePos[0].x = ((o_Rect.right - o_Rect.left) / 2) & 0xFFFFFFF0;
            mast_GamePos[0].y = ((o_Rect.bottom - o_Rect.top) / 2) & 0xFFFFFFF0;
            mst_GameMove.x = 0;
            mst_GameMove.y = 0;
            mst_GameMove.z = 0;
            mst_GamePos.z = 1;
			mi_Score = 0;
            return;
        }

        if(mast_GamePos[0].y < 16) goto end;
        if((int) mast_GamePos[0].x >= (int) (o_Rect.right & 0xFFFFFFF0)) goto end;
        if((int) mast_GamePos[0].y >= (int) (o_Rect.bottom & 0xFFFFFFF0)) goto end;

        /* Detect collision with queue */
        if(mst_GamePos.z >= 4)
        {
            for(i = 1; i < (int) mst_GamePos.z; i++)
            {
                x = abs(mast_GamePos[i].x - mast_GamePos[0].x);
                y = abs(mast_GamePos[i].y - mast_GamePos[0].y);
                if(!x && !y) goto end;
            }
        }

        /* Detect collision with apple */
        x = abs(mast_GamePos[0].x - (int) mst_GamePos.x);
        y = abs(mast_GamePos[0].y - (int) mst_GamePos.y);
        if(x < SIZE2 && y < SIZE2)
        {
            it = 0;
            while(1)
            {
                mst_GamePos.x = (float) ((rand() % o_Rect.right & 0xFFFFFFF0) & 0xFFFFFFF0);
                if(mst_GamePos.x < 16) mst_GamePos.x = 16;
                if(mst_GamePos.x > (float) o_Rect.right - 32) mst_GamePos.x = (float) (o_Rect.right - 32 & 0xFFFFFFF0);
                mst_GamePos.y = (float) ((rand() % o_Rect.bottom & 0xFFFFFFF0) & 0xFFFFFFF0);
                if(mst_GamePos.y > (float) o_Rect.bottom - 32) mst_GamePos.y = (float) (o_Rect.bottom - 32 & 0xFFFFFFF0);
                if(mst_GamePos.y < 16) mst_GamePos.y = 16;
                for(i = 0; i < mst_GamePos.z; i++)
                {
                    if(abs((int) mst_GamePos.x - mast_GamePos[i].x) < 2 * SIZE2) break;
                    if(abs((int) mst_GamePos.y - mast_GamePos[i].y) < 2 * SIZE2) break;
                    goto ok;
                }

                it++;
                if(it == 100) goto end;
            }

ok:
            (int) mst_GamePos.z++;
			mi_Score++;
            L_memmove(&mast_GamePos[1], mast_GamePos, (int) mst_GamePos.z * sizeof(POINT));
            L_memcpy(mast_GamePos, &mast_GamePos[1], sizeof(POINT));

            /* Erase text */
            pDC->FillSolidRect
                (
                    o_Rect.left + 3,
                    o_Rect.top + 4,
                    100,
                    12,
                    GetSysColor(COLOR_3DSHADOW)
                );
        }
    }

    /* Draw apple */
    pDC->Draw3dRect
        (
            ((int) mst_GamePos.x + SIZE2 / 2),
            ((int) mst_GamePos.y + SIZE2 / 2),
            SIZE2,
            SIZE2,
            GetSysColor(COLOR_BTNHILIGHT),
            GetSysColor(COLOR_3DDKSHADOW)
        );
    pDC->FillSolidRect
        (
            ((int) mst_GamePos.x + SIZE2 / 2) + 1,
            ((int) mst_GamePos.y + SIZE2 / 2) + 1,
            SIZE2 - 2,
            SIZE2 - 2,
            GetSysColor(COLOR_BTNFACE)
        );

    /* Draw new rect */
    pDC->Draw3dRect
        (
            mast_GamePos[0].x,
            mast_GamePos[0].y,
            (2 * SIZE2),
            (2 * SIZE2),
            GetSysColor(COLOR_BTNHILIGHT),
            GetSysColor(COLOR_3DDKSHADOW)
        );
    pDC->FillSolidRect
        (
            mast_GamePos[0].x + 1,
            mast_GamePos[0].y + 1,
            (2 * SIZE2) - 2,
            (2 * SIZE2) - 2,
            GetSysColor(COLOR_BTNFACE)
        );
    pDC->Draw3dRect
        (
            mast_GamePos[0].x + 3,
            mast_GamePos[0].y + 3,
            (2 * SIZE2) - 6,
            (2 * SIZE2) - 6,
            GetSysColor(COLOR_3DDKSHADOW),
            GetSysColor(COLOR_BTNHILIGHT)
        );
    if((int) mst_GamePos.z > 1)
    {
        pDC->FillSolidRect
            (
                mast_GamePos[1].x + 1,
                mast_GamePos[1].y + 1,
                (2 * SIZE2) - 2,
                (2 * SIZE2) - 2,
                GetSysColor(COLOR_BTNFACE)
            );
    }

    ReleaseDC(pDC);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseView::GameTick2(void)
{
}

#endif /* ACTIVE_EDITORS */
