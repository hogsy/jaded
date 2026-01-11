/*$T MAD_RAD.CPP GC!1.32 08/22/20 18:16:13 */

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define DONOTCOMPRESS
#define MAX_MEM_ALLOWED_4_NON_CMPRSSD_BMP 200000000 /* 200 Mo , after it will be compressed */
unsigned long ulTotalMemoryAllowedForPictureBuffer = 0;

#define MAD_SHOW_ELEMENTS	/* DEBUG */
//#define SHOW_CALCUL			0 /* DEBUG */
#define RENDER_QUALITY		3
#define QUAKE_QUALITY		1
#define MAD_NumberOFSub		20
//#define MAD_DLZBCol			CurrentWireColor
#define MR_Infinite         10.0e37f
#define MAD_Box_Color1		0xAfAf00//FFFF
#define MAD_Box_Color2		0xffff00
#define MAD_BOX_SUBVECTLEN  0.5f
#define MAD_BOX_LINELEN		(SourceSX >> 4)
#define MAD_BoxMask			0x2
#define NumberOFREpeat      5
#define FOCALE              0.5f
#define MAD_RAD_CLIPPING_NEAR 0.01f
#define COMPUTEBCKCOLOR		0x04040404

#define MAXSourceSXShift    11
#define MAXSourceSX         (1 << MAXSourceSXShift)
#define MAXSourceSY         MAXSourceSX

#define SUBDIV_DEFAULT_QUALITY		3 /* good */
static unsigned long    *SaveTableau;
static unsigned long    *TABLEAU;
static unsigned long    *PICKINGBUFF;
static float            *ZBUFFER;
static unsigned long	MAD_BackGroudColor = 0x60606060;

/* Static float *GlobBuffer; */
static unsigned long    LIGHT_DISCRET = 10;
static unsigned long    SourceSXShift = 9;
static long             SourceSX = 1 << SourceSXShift;
#define SourceSY    SourceSX

static unsigned long    TotalNumBerOfPoints = 0;
static unsigned long    TotalNumBerOfPolygons = 0;
static unsigned long	CurrentLightType = 0;
static unsigned long	UseAmbiantLight = 0;
#define					MAD_RAD_QUKE_NUMMAXPONT_DEF 2000 
static unsigned long	MAD_RAD_QUKE_NUMMAXPONT = MAD_RAD_QUKE_NUMMAXPONT_DEF;
static long     ComputeMaps = 1;


static float ChannelSize = 50.0f;

#include <WINDOWS.H>
#include <COMMCTRL.H>
#include <math.H>
#include "resource.H"
#include "MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "MAD_Rasterize/Sources/MAD_Rasterize.h"
#include "MAD_mem/Sources/MAD_mem.h"
#include "MAD_LOD.H"
#define TMT_CLASSNAME   "GRAAL"
#include "viewer.h"
#include "SCD_MTH.cpp"
HCURSOR                 CURS[6];
static unsigned long    QUITAP;
static unsigned long    Force2sided = 0;
static unsigned long    CurrentSurface;
static MAD_Vertex       CurrentFLOATColor;
static unsigned long    CurrentSurfaceSizeY;
static unsigned long    CurrentSurfaceSizeX;
static unsigned long    CurrentSelectedLight = 0;
unsigned long			CurrentEdgeFlag;

static float            LittleCirle = 0.7f;
static float            MAD_SURFACE_THRESHOLD = 6.0f;

static MAD_World        *MW;
static MAD_World        *ORIGINAL_MW;
static MAD_Matrix       CURRENT_VIEW_MATRIX;
static HWND             Mainhwnd;
static HWND             Renderhwnd;
static HWND             RollHwnd;
static unsigned long    ImCompute = 0;
static HWND             hwndComboCam;
static HWND             hwndComboRender;
static HWND             hwndProgress;
static char             FileNAme[260];
static unsigned long    STOP = 0;
static unsigned long    GLOW_ON = 0;
static unsigned long    FIRSTGLOW = 0;
static unsigned long    CastShadow = 0;
static unsigned long    PaintLight = 0;
static unsigned long    OverWalls = 0;
static float            EXTRACTION = 0.0f;
static float            DIRECTIONAL_VIEW_SIZE = 0.0f;
static float            CurrentLightFar = 0.0f;
static float            CurrentLightNear = 0.0f;
static double           MATH_gd_Decal;
static unsigned long    QUAKE_ON = 0;
static unsigned long    ROLL_ON = 0;
static unsigned long	PREVENTWENFINIFHED = 0;
static long    ROLL_DELTA = 0;
static MAD_MAT_MatID *Mat;

static unsigned long    MOUSEBUTTONS = 0;   /* 1 right 2 left */
#define fInterpretLongAsFloat(a)    (*((float *) &(a)))
#define lInterpretFloatAsLong(a)    (*((long *) &(a)))
static unsigned long    CurrentQuality = 1;

static RADCAM               AllRadCam[5000];
static MAD_R_Clipping_Info  CI;
static unsigned long        CurrentTriangleColor = 0;
static unsigned long        CurrentWireColor = 0;

static unsigned long        CurrentPickingColor = 0;
static unsigned long        NumberRadCam = 0;
static unsigned long        NumberLightCam = 0;
static unsigned long        CurrentRadCam = 0;
static unsigned long        CurrentLightCam = 0;
static unsigned long		Displaymode = 2; /* 0 = norender ; 1 = Gourrau ; 2 = ZBuff ; 3 = ColorObject ; 4 = Cut mode */;
static unsigned long		ShowLines = 0; /* 0 = nothing ; 1 = lines */;
static unsigned long		NumBerOfObject = 0;
float						fswap;

#define FSWAP(a, b) \
    fswap = a; \
    a = b; \
b = fswap;

typedef struct MAD_Color4D_
{
	float C[4];
}
MAD_Color4D;

/*
===================================================================================================
===================================================================================================
*/
void CompputingQuality(unsigned long Counter)
{
    switch(Counter)
    {
    case 0:
        LIGHT_DISCRET = 5;
        SourceSXShift = 7;
        SourceSX = 1 << SourceSXShift;
        MAD_SURFACE_THRESHOLD = 5.0f;
        break;
    case 1:
        LIGHT_DISCRET = 5;
        SourceSXShift = 8;
        SourceSX = 1 << SourceSXShift;
        MAD_SURFACE_THRESHOLD = 5.0f;
        break;
    case 2:
        LIGHT_DISCRET = 6;
        SourceSXShift = 9;
        SourceSX = 1 << SourceSXShift;
        MAD_SURFACE_THRESHOLD = 10.0f;
        break;
    case 3:
        LIGHT_DISCRET = 7;
        SourceSXShift = 9;
        SourceSX = 1 << SourceSXShift;
        MAD_SURFACE_THRESHOLD = 10.0f;
        break;
    case 4:
        LIGHT_DISCRET = 9;
        SourceSXShift = 11;
        SourceSX = 1 << SourceSXShift;
        MAD_SURFACE_THRESHOLD = 10.0f;
        break;
    }
	
    /* Recompute GlobBuffer */
	
    /* GlobBuffer */
    CurrentQuality = Counter;
}

void    Compute_MW(unsigned long Illum, unsigned long Subdiv, unsigned long ComputeMap);

/*
===================================================================================================
===================================================================================================
*/
static inline long lFloatToLongOpt(float a)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    double  b;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    b = a + MATH_gd_Decal;
    return lInterpretFloatAsLong(b);
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_EnableWindow(int nIDDlgItem, unsigned long ED)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DWORD   dwStyle;
    HWND    Button;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Button = GetDlgItem(Mainhwnd, nIDDlgItem);
    if(!ED)
    {
        dwStyle = GetWindowLong(Button, GWL_STYLE);
        dwStyle |= WS_DISABLED;
        SetWindowLong(Button, GWL_STYLE, dwStyle);
    }
    else
    {
        dwStyle = GetWindowLong(Button, GWL_STYLE);
        dwStyle &= ~WS_DISABLED;
        SetWindowLong(Button, GWL_STYLE, dwStyle);
    }
	
    ShowWindow(Button, SW_HIDE);
    ShowWindow(Button, SW_SHOW);
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_UpdateInfoStr(char *Str)
{
    SetDlgItemText(Mainhwnd, IDC_infocurrent, Str);
	}

void MAD_UpdateInfoStr2(char *Str)
{
    SetDlgItemText(Mainhwnd, IDC_infocurrent2, Str);
	}

/*
===================================================================================================
===================================================================================================
*/
void MAD_UpdateInfo(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    Info[1024];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    sprintf
		(
        Info,
        "Number of triangles: %d \nNumber of Points : %d ",
		TotalNumBerOfPolygons,
        TotalNumBerOfPoints
		);
    SetDlgItemText(Mainhwnd, IDC_INFO, Info);
}

/*
===================================================================================================
===================================================================================================
*/
static float SeprogressPosBase = 0.0f;
static float SeprogressPosMult = 1.0f;
unsigned long SeprogressPos(float P)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MSG msg;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	P = P * SeprogressPosMult + SeprogressPosBase;
	
    /* MAD_UpdateInfoStr2(StrFast); */
    if(P >= 1.0f) 
		P = 0.99999f;
	
    SendMessage(hwndProgress, PBM_SETPOS, (WPARAM) (P * 1000), 0);
    while(PeekMessage(&msg, NULL/* WinDeauws */, 0, 0, PM_REMOVE))
    {
        if(!IsDialogMessage(Mainhwnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	
    return STOP;
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_GriseAll(unsigned long Enable)
{
    MAD_EnableWindow(IDC_COMBO1, Enable);
    MAD_EnableWindow(IDC_COMBO3, Enable);
    MAD_EnableWindow(IDC_CHECK1, Enable);
    MAD_EnableWindow(IDC_BUTTONSimplify, Enable);
    MAD_EnableWindow(IDSAVE, Enable);
    MAD_EnableWindow(IDC_COMBO2, Enable);
    MAD_EnableWindow(IDC_RADIODIRECTSHADOW, Enable);
    MAD_EnableWindow(IDC_STATICOUSE, Enable);
    MAD_EnableWindow(IDC_BUTTONSimplify2, Enable);
    MAD_EnableWindow(IDC_BUTTON1, Enable);
    MAD_EnableWindow(IDC_EDIT2, Enable);
    MAD_EnableWindow(IDSTOP, Enable ^ 1);
    MAD_EnableWindow(IDC_Refine, Enable);
    MAD_EnableWindow(IDC_CHECK2, Enable);
    MAD_EnableWindow(IDC_CHECK3, Enable);
    MAD_EnableWindow(IDC_CHECK4, Enable);
	
    MAD_EnableWindow(IDC_COMBO4, Enable);
    MAD_EnableWindow(IDC_EDIT3, Enable);
    MAD_EnableWindow(IDC_CHECK5, Enable);
    MAD_EnableWindow(IDC_CHECK6, Enable);
	MAD_EnableWindow(IDC_BUTTONAmbientColor, Enable);
	MAD_EnableWindow(IDC_BUTTONBCKCOL, Enable);

	

	MAD_EnableWindow(IDC_COMBO5, Enable);
	MAD_EnableWindow(IDC_BUTTONColorlight, Enable);
	MAD_EnableWindow(IDC_CHECK10, Enable);
	MAD_EnableWindow(IDC_CHECK9, Enable);
	MAD_EnableWindow(IDC_CHECK7, Enable);
	MAD_EnableWindow(IDC_CHECK8, Enable);
	MAD_EnableWindow(IDC_CHECK13, Enable);
	
	MAD_EnableWindow(IDC_BUTTON3, Enable);

	
    MAD_EnableWindow(IDC_BUTTONFC, Enable);
	
    MAD_EnableWindow(IDC_STATICOUZE, Enable);
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long MAD_Color_2_RadColor(unsigned long MC)
{
    MC |= (MC & 255) << 24;
    MC &= 0xffffff00;
    MC |= (MC & 0xff0000) >> 16;
    MC &= 0xff00ffff;
    MC |= (MC & 0xff000000) >> 8;
    return MC;
}
/*
===================================================================================================
Z Cliping
===================================================================================================
*/
void MAD_DrawLine(MAD_R_Vertex *A, MAD_R_Vertex *B)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_R_Vertex    *Swp;
    unsigned long   *Adress;
    static long     DX, DY, Accum, inc, Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if(A->Y > B->Y)
    {
        Swp = A;
        A = B;
        B = Swp;
    }
	
    DX = lFloatToLongOpt(A->X);
    DY = lFloatToLongOpt(A->Y);
    Adress = &TABLEAU[(DY << SourceSXShift) + DX];
    DY = lFloatToLongOpt(B->Y) - DY;
    DX = lFloatToLongOpt(B->X) - DX;
    Accum = 0;
    inc = 1;
    if(DX < 0)
    {
        DX = -DX;
        inc = -1;
    }
	
    if(DX > DY)
    {
        if(DX > SourceSX) DX = DX;
        for(Counter = 0; Counter < DX; Counter++)
        {
            Accum += DY;
            *Adress = 0xffffffff;
            if(Accum > DX)
            {
                Adress += SourceSX;
                Accum -= DX;
            }
			
            Adress += inc;
        }
    }
    else
    {
        for(Counter = 0; Counter < DY; Counter++)
        {
            Accum += DX;
            *Adress = 0xffffffff;
            if(Accum > DY)
            {
                Adress += inc;
                Accum -= DY;
            }
			
            Adress += SourceSX;
        }
    }
}

/*
===================================================================================================
Draw line special drawbox selected
===================================================================================================
*/

void MAD_DrawLine_Special(MAD_R_Vertex *A, MAD_R_Vertex *B)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_R_Vertex    *Swp;
    unsigned long   *Adress;
    float           *Zbuf;
    unsigned long   *PBuf;
	
    long			DX, DY, Accum, inc, Counter;
	long			LineLen;
	long			LineStart;
	long			LineBegin;

    float           ZAccum, Zinc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LineStart = 0;
    if(A->Y > B->Y)
    {
        Swp = A;
        A = B;
        B = Swp;
		LineStart = 1;
    } 
	
    DX = lFloatToLongOpt(A->X);
    DY = lFloatToLongOpt(A->Y);
    Adress = &TABLEAU[(DY << SourceSXShift) + DX];
    Zbuf = &ZBUFFER[(DY << SourceSXShift) + DX];
    PBuf = &PICKINGBUFF[(DY << SourceSXShift) + DX];
    DY = lFloatToLongOpt(B->Y) - DY;
    DX = lFloatToLongOpt(B->X) - DX;
    Accum = 0;
    inc = 1;
    if(DX < 0)
    {
        DX = -DX;
        inc = -1;
    } 
	
    if(DX > DY)
    {
        Zinc = (B->OoZ - A->OoZ) / (float) DX;
        ZAccum = A->OoZ * 1.02f;
        Zinc *= 1.02f;
        if(DX > SourceSX) DX = DX;
		LineLen = DX;
		LineBegin = 0;
		if (!LineStart)
		{
			if (LineLen > MAD_BOX_LINELEN)
				LineLen = MAD_BOX_LINELEN;
		} else
		{
			if (LineLen > MAD_BOX_LINELEN)
				LineBegin = LineLen - MAD_BOX_LINELEN;
		}
        for(Counter = 0; Counter < LineLen; Counter++)
        {
            Accum += DY;
			if (Counter >= LineBegin)
			{
				if ((Counter & MAD_BoxMask) == 0)
				{
					if((*PBuf != CurrentPickingColor) || (ZAccum >= *Zbuf)) *Adress = MAD_Box_Color1;
				}else
				{
					if((*PBuf != CurrentPickingColor) || (ZAccum >= *Zbuf)) *Adress = MAD_Box_Color2;
				}
			}
			if(Accum > DX)
			{
				Adress += SourceSX;
				Zbuf += SourceSX;
				PBuf += SourceSX;
				Accum -= DX;
			}
			
            ZAccum += Zinc;
            Adress += inc;
            Zbuf += inc;
            PBuf += inc;
        }
    }
    else
    {
        Zinc = (B->OoZ - A->OoZ) / (float) DY;
        ZAccum = A->OoZ * 1.02f;
        Zinc *= 1.02f;
		LineLen = DY;
		LineBegin = 0;
		if (!LineStart)
		{
			if (LineLen > MAD_BOX_LINELEN)
				LineLen = MAD_BOX_LINELEN;
		} else
		{
			if (LineLen > MAD_BOX_LINELEN)
				LineBegin = LineLen - MAD_BOX_LINELEN;
		}
        for(Counter = 0; Counter < LineLen; Counter++)
        {
            Accum += DX;
			if (Counter >= LineBegin)
			{
				if ((Counter & MAD_BoxMask) == 0)
				{
					if((*PBuf != CurrentPickingColor) || (ZAccum >= *Zbuf)) *Adress = MAD_Box_Color1;
				}else
				{
					if((*PBuf != CurrentPickingColor) || (ZAccum >= *Zbuf)) *Adress = MAD_Box_Color2;
				}
			}
			if(Accum > DY)
			{
				Adress += inc;
				Zbuf += inc;
				PBuf += inc;
				Accum -= DY;
			}
			
            ZAccum += Zinc;
            Adress += SourceSX;
            Zbuf += SourceSX;
            PBuf += SourceSX;
        }
    }
}

/*
===================================================================================================
Z Cliping
===================================================================================================
*/
void MAD_DrawLineZBuffer(MAD_R_Vertex *A, MAD_R_Vertex *B,unsigned long SideNumAndIsClipped)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_R_Vertex    *Swp;
    unsigned long   *Adress;
    float           *Zbuf;

    static long     DX, DY, Accum, inc, Counter;
	unsigned long	MAD_DLZBCol;
    float           ZAccum, Zinc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_DLZBCol = CurrentWireColor;
	if (((1L << SideNumAndIsClipped) & CurrentEdgeFlag) == 0)
	{
		if (ShowLines == 1)
			return;
		MAD_DLZBCol = ((MAD_DLZBCol & 0xfcfcfcfc) >> 2) + ((MAD_DLZBCol & 0xf8f8f8f8) >> 3);
	}

    if(A->Y > B->Y)
    {
        Swp = A;
        A = B;
        B = Swp;
    }
	
    DX = lFloatToLongOpt(A->X);
    DY = lFloatToLongOpt(A->Y);
    Adress = &TABLEAU[(DY << SourceSXShift) + DX];
    Zbuf = &ZBUFFER[(DY << SourceSXShift) + DX];
    DY = lFloatToLongOpt(B->Y) - DY;
    DX = lFloatToLongOpt(B->X) - DX;
    Accum = 0;
    inc = 1;
    if(DX < 0)
    {
        DX = -DX;
        inc = -1;
    }
	
    if(DX > DY)
    {
        Zinc = (B->OoZ - A->OoZ) / (float) DX;
        ZAccum = A->OoZ * 1.01f;
        Zinc *= 1.01f;
        if(DX > SourceSX) DX = DX;
        for(Counter = 0; Counter < DX; Counter++)
        {
            Accum += DY;
            if (ZAccum >= *Zbuf)
			{
				*Adress = MAD_DLZBCol;
				*Zbuf = ZAccum;
			}
            if(Accum > DX)
            {
                Adress += SourceSX;
                Zbuf += SourceSX;
                Accum -= DX;
            }
			
            ZAccum += Zinc;
            Adress += inc;
            Zbuf += inc;
        }
    }
    else
    {
        Zinc = (B->OoZ - A->OoZ) / (float) DY;
        ZAccum = A->OoZ * 1.01f;
        Zinc *= 1.01f;
        for(Counter = 0; Counter < DY; Counter++)
        {
            Accum += DX;
            if (ZAccum >= *Zbuf)
			{
				*Adress = MAD_DLZBCol;
				*Zbuf = ZAccum;
			}
            if(Accum > DY)
            {
                Adress += inc;
                Zbuf += inc;
                Accum -= DY;
            }

            ZAccum += Zinc;
            Adress += SourceSX;
            Zbuf += SourceSX;
        }
    }
}
/*
===================================================================================================
Z Cliping
===================================================================================================
*/
void MAD_DrawLineZBufferISO(MAD_R_Vertex *A, MAD_R_Vertex *B,unsigned long SideNumAndIsClipped)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_R_Vertex    *Swp;
    unsigned long   *Adress,MAD_DLZBCol;
    float           *Zbuf;
	
    static long     DX, DY, Accum, inc, Counter;
    float           ZAccum, Zinc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_DLZBCol = CurrentWireColor;
	if (((1L << SideNumAndIsClipped) & CurrentEdgeFlag) == 0)
	{
		MAD_DLZBCol = (MAD_DLZBCol & 0xfcfcfcfc) >> 1;
	}
	
    if(A->Y > B->Y)
    {
        Swp = A;
        A = B;
        B = Swp;
    }
	
    DX = lFloatToLongOpt(A->X);
    DY = lFloatToLongOpt(A->Y);
    Adress = &TABLEAU[(DY << SourceSXShift) + DX];
    Zbuf = &ZBUFFER[(DY << SourceSXShift) + DX];
    DY = lFloatToLongOpt(B->Y) - DY;
    DX = lFloatToLongOpt(B->X) - DX;
    Accum = 0;
    inc = 1;
    if(DX < 0)
    {
        DX = -DX;
        inc = -1;
    }
	
    if(DX > DY)
    {
        Zinc = (B->OoZ - A->OoZ) / (float) DX;
        ZAccum = A->OoZ + 0.5f;
        if(DX > SourceSX) DX = DX;
        for(Counter = 0; Counter < DX; Counter++)
        {
            Accum += DY;
            if (ZAccum >= *Zbuf)
			{
				*Adress = MAD_DLZBCol;
				*Zbuf = ZAccum;
			}
            if(Accum > DX)
            {
                Adress += SourceSX;
                Zbuf += SourceSX;
                Accum -= DX;
            }
			
            ZAccum += Zinc;
            Adress += inc;
            Zbuf += inc;
        }
    }
    else
    {
        Zinc = (B->OoZ - A->OoZ) / (float) DY;
        ZAccum = A->OoZ + 0.5f;
        for(Counter = 0; Counter < DY; Counter++)
        {
            Accum += DX;
            if (ZAccum >= *Zbuf)
			{
				*Adress = MAD_DLZBCol;
				*Zbuf = ZAccum;
			}
            if(Accum > DY)
            {
                Adress += inc;
                Zbuf += inc;
                Accum -= DY;
            }
			
            ZAccum += Zinc;
            Adress += SourceSX;
            Zbuf += SourceSX;
        }
    }
}

#define RAD_DOT_PRODUCT(a, b, _X, _Y, _Z)   (a->_Y * b->_Y) + (a->_Z * b->_Z) + (a->_X * b->_X)

unsigned long   Color;

#define MAX_Compressed_MAPS 4096
unsigned long   *AllMap[MAX_Compressed_MAPS];

/*
===================================================================================================
===================================================================================================
*/
void ResetAllCompressedMap(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   MapCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ulTotalMemoryAllowedForPictureBuffer = 0;
    for(MapCounter = 0; MapCounter < MAX_Compressed_MAPS; MapCounter++)
        if(AllMap[MapCounter])
        {
            free(AllMap[MapCounter]);
            AllMap[MapCounter] = 0;
        }
}

/*
===================================================================================================
===================================================================================================
*/
void initAllCompressedMap(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   MapCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ulTotalMemoryAllowedForPictureBuffer = 0;
    for(MapCounter = 0; MapCounter < MAX_Compressed_MAPS; MapCounter++)
    {
        AllMap[MapCounter] = NULL;
    }
}

/*
===================================================================================================
Rle DECOMPRESSION
===================================================================================================
*/
void MAP_DeCompress(unsigned long *Picture, unsigned long *CompressedPicture)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   NumberOfEntry, NumberOfColor, Color;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TABLEAU = SaveTableau;

#ifdef DONOTCOMPRESS
	if (*CompressedPicture == 0xC0DE0000)
	{
		TABLEAU = CompressedPicture + 1;
		return;
	}
#endif
	
    NumberOfEntry = *(CompressedPicture++);
    while(NumberOfEntry--)
    {
        NumberOfColor = *(CompressedPicture++);
        Color = *(CompressedPicture++);
        while(NumberOfColor--) *(Picture++) = Color;
    }
}

/*
===================================================================================================
Rle COMPRESSION true color
===================================================================================================
*/
unsigned long *MAP_Compress(unsigned long *Picture, unsigned long Size)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   NumberOfEntry, Color, SizeSave, EntryAtomCntr;
    unsigned long   *PicturePtr;
    unsigned long   *ReturnedPtr, *WorkingPtr;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef DONOTCOMPRESS
	if (ulTotalMemoryAllowedForPictureBuffer < MAX_MEM_ALLOWED_4_NON_CMPRSSD_BMP)
	{
		ulTotalMemoryAllowedForPictureBuffer += (Size << 2L) + 4L + 4L;
		ReturnedPtr = (unsigned long *) malloc((Size << 2L) + 4L + 4L);
		*(ReturnedPtr)   = 0xC0DE0000; /*non compressed*/
		memcpy(ReturnedPtr+1 , Picture , Size << 2L);
		return ReturnedPtr;
	}
#endif
	
    /* Compute Size */
    PicturePtr = Picture;
    Color = *PicturePtr;
    Color ^= 0xffffffff;
    SizeSave = Size;
    NumberOfEntry = 1;
    while(SizeSave--)
    {
        if(Color != *PicturePtr)
        {
            NumberOfEntry++;
            Color = *PicturePtr;
        }
		
        PicturePtr++;
    }
    ulTotalMemoryAllowedForPictureBuffer += 4L * NumberOfEntry * 2L + 4L + 8L;
	WorkingPtr = ReturnedPtr = (unsigned long *) malloc(4L * NumberOfEntry * 2L + 4L + 8L);
    *(WorkingPtr++) = NumberOfEntry;
    PicturePtr = Picture;
    Color = *PicturePtr;
    Color ^= 0xffffffff;
    SizeSave = Size;
    NumberOfEntry = 0;
    EntryAtomCntr = 0;
    while(SizeSave--)
    {
        if(Color != *PicturePtr)
        {
            *(WorkingPtr++) = EntryAtomCntr;
            *(WorkingPtr++) = Color;
            EntryAtomCntr = 0;
            NumberOfEntry++;
            Color = *PicturePtr;
        }
		
        EntryAtomCntr++;
        PicturePtr++;
    }
	
    *(WorkingPtr++) = EntryAtomCntr;
    *(WorkingPtr++) = Color;
    *(WorkingPtr++) = 0;
    *(WorkingPtr++) = 0;
	
    return ReturnedPtr;
}

/*
===================================================================================================
Rendering unicolor
===================================================================================================
*/
void RasterizeCallbackD34(unsigned long Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Xcounter, End, *Adress, *Last;
    float           Interpol, Interpol0, *Zaddress;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if(!Displaymode) return;
    if((R->XEnd - R->P.X) > 0)
    {
        Xcounter = lFloatToLongOpt(R->P.X);
        End = lFloatToLongOpt(R->XEnd);
        Adress = &TABLEAU[(Y << SourceSXShift) + Xcounter];
        Zaddress = &ZBUFFER[(Y << SourceSXShift) + Xcounter];
        Last = Adress + End - Xcounter;
        Interpol = R->P.OoZ;
        Interpol0 = R->P.SclrToInt[0];
        CurrentSurface += End - Xcounter;
        while(Adress < Last)
        {
            if(*Zaddress < Interpol)
            {
                *Adress = CurrentTriangleColor;
                *Zaddress = Interpol;
            }
			
            Interpol += PXInc->OoZ;
            Zaddress++;
            Adress++;
        }
    }
}

/*
===================================================================================================
GOURAUD SHADING
===================================================================================================
*/
void RasterizeCallbackD1(unsigned long Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Xcounter, End, *Adress, *Last;
    unsigned long   *Picking;
    float           Interpol, *Zaddress;
    long			Red, Green, Blue , Redi, Greeni, Bluei;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if(!Displaymode) return;
    if((R->XEnd - R->P.X) > 0)
    {
        Xcounter = lFloatToLongOpt(R->P.X);
        End = lFloatToLongOpt(R->XEnd);
        Adress = &TABLEAU[(Y << SourceSXShift) + Xcounter];
        Picking = &PICKINGBUFF[(Y << SourceSXShift) + Xcounter];
        Zaddress = &ZBUFFER[(Y << SourceSXShift) + Xcounter];
        Last = Adress + End - Xcounter;
        Interpol = R->P.OoZ;
        Red =   lFloatToLongOpt(R->P.SclrToInt[0] * 65536.0f);
        Green = lFloatToLongOpt(R->P.SclrToInt[1] * 65536.0f);
        Blue =  lFloatToLongOpt(R->P.SclrToInt[2] * 65536.0f);
        Redi=   lFloatToLongOpt(PXInc->SclrToInt[0] * 65536.0f);
        Greeni= lFloatToLongOpt(PXInc->SclrToInt[1] * 65536.0f);
        Bluei=  lFloatToLongOpt(PXInc->SclrToInt[2] * 65536.0f);
        CurrentSurface += End - Xcounter;
        while(Adress < Last)
        {
            if(*Zaddress < Interpol)
            {
                *Adress = ((0xff0000 & Red) >> 16) | ((0xff0000 & Green) >> 8) | (0xff0000 & Blue);
                *Picking = CurrentPickingColor;
                *Zaddress = Interpol;
            }
			
            Interpol += PXInc->OoZ;
            Red += Redi;
            Green += Greeni;
            Blue += Bluei;
            Zaddress++;
            Adress++;
            Picking++;
        }
    }
}

/*
===================================================================================================
FILL ZBUFFER ONLY
===================================================================================================
*/
void RasterizeCallbackD1_FILLZBUFONLY(unsigned long Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Xcounter, End, *Adress, *Last;
    unsigned long   *Picking;
    float           Interpol, *Zaddress;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if(!Displaymode) return;
    if((R->XEnd - R->P.X) > 0)
    {
        Xcounter = lFloatToLongOpt(R->P.X);
        End = lFloatToLongOpt(R->XEnd);
        Adress = &TABLEAU[(Y << SourceSXShift) + Xcounter];
        Picking = &PICKINGBUFF[(Y << SourceSXShift) + Xcounter];
        Zaddress = &ZBUFFER[(Y << SourceSXShift) + Xcounter];
        Last = Adress + End - Xcounter;
        Interpol = R->P.OoZ;
        CurrentSurface += End - Xcounter;
        while(Adress < Last)
        {
            if(*Zaddress < Interpol)
            {
                *Picking = CurrentPickingColor;
                *Zaddress = Interpol;
            }
			
            Interpol += PXInc->OoZ;
            Zaddress++;
            Adress++;
            Picking++;
        }
    }
}

/*
===================================================================================================
Draw ZBuffer
===================================================================================================
*/
void RasterizeCallbackD2(unsigned long Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Xcounter, End, *Adress, *Last;
    float           Interpol, Interpol0, *Zaddress;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if(!Displaymode) return;
    if((R->XEnd - R->P.X) > 0)
    {
        Xcounter = lFloatToLongOpt(R->P.X);
        End = lFloatToLongOpt(R->XEnd);
        Adress = &TABLEAU[(Y << SourceSXShift) + Xcounter];
        Zaddress = &ZBUFFER[(Y << SourceSXShift) + Xcounter];
        Last = Adress + End - Xcounter;
        Interpol = R->P.OoZ * 10000.0f;
        Interpol0 = PXInc->OoZ * 10000.0f;
        CurrentSurface += End - Xcounter;
        while(Adress < Last)
        {
            if(*Zaddress < Interpol)
            {
                *Adress = lFloatToLongOpt(Interpol)/* | (lFloatToLongOpt(Interpol0)<<8) */;
                *Zaddress = Interpol;
            }
			
            Interpol += Interpol0;
            Zaddress++;
            Adress++;
			
            /* Interpol2-= PXInc -> SclrToInt[1]; */
        }
    }
}

#define STATE_UNDER 1
#define STATE_OVER  2
unsigned long   StateOfTriangle;

/*
===================================================================================================
Sign TABLEAU with triangle color
===================================================================================================
*/
void RasterizeMarkCallback(unsigned long Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Xcounter, End, *Adress, *Last;
    float           Interpol, *Zaddress;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if((R->XEnd - R->P.X) > 0)
    {
        Xcounter = lFloatToLongOpt(R->P.X);
        End = lFloatToLongOpt(R->XEnd);
        Adress = &TABLEAU[(Y << SourceSXShift) + Xcounter];
        Zaddress = &ZBUFFER[(Y << SourceSXShift) + Xcounter];
        Last = Adress + End - Xcounter;
        Interpol = R->P.OoZ;
        CurrentSurface += End - Xcounter;
        while(Adress < Last)
        {
            if(*Zaddress < Interpol)
            {
                *Adress = CurrentTriangleColor;
                *Zaddress = Interpol;
            }
			
            Interpol += PXInc->OoZ;
            Zaddress++;
            Adress++;
        }
    }
}


/*
===================================================================================================
Detect REcover traingles
===================================================================================================
*/
void RasterizeDetectCallback(unsigned long Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Xcounter, End, *Adress, *Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!FIRSTGLOW) return;

    if((R->XEnd - R->P.X) > 0)
    {
        Xcounter = lFloatToLongOpt(R->P.X);
        End = lFloatToLongOpt(R->XEnd);
        Adress = &TABLEAU[(Y << SourceSXShift) + Xcounter];
        Last = Adress + End - Xcounter;
        CurrentSurface += End - Xcounter;
        if(StateOfTriangle == (STATE_OVER | STATE_UNDER)) return;
        while(Adress < Last)
        {
            if(*Adress == CurrentTriangleColor)
                StateOfTriangle |= STATE_OVER;
            else
                StateOfTriangle |= STATE_UNDER;
            Adress++;
        }
    }
}

/*
===================================================================================================
===================================================================================================
*/
void RadNodeCallback(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    if(NumberLightCam == 0)
    {
        SendMessage(GetDlgItem(Mainhwnd, IDC_COMBO5), CB_ADDSTRING, 0, (long) MWN->ID.Name);
    }
	
    CurrentSelectedLight = 0;
	
    SendMessage(hwndComboCam, CB_ADDSTRING, 0, (long) MWN->ID.Name);
    AllRadCam[NumberRadCam].NodeNumber = NodeNumber;
    AllRadCam[NumberRadCam].Matrix = MWN->Matrix;
    AllRadCam[NumberRadCam].Object = MW->AllObjects[MWN->Object];
    AllRadCam[NumberRadCam].Focale = 0.45f;
	AllRadCam[NumberRadCam].IsOrtho = 0;
    if(AllRadCam[NumberRadCam].Object->IDType == ID_MAD_Camera)
    {
        if(((MAD_Camera *) AllRadCam[NumberRadCam].Object)->CameraType == MAD_e_PerspCamera)
        {
            AllRadCam[NumberRadCam].Focale = 0.5f /
                (float) tan(((MAD_Camera *) AllRadCam[NumberRadCam].Object)->PerspInf.Fov * 0.5f);
        }
        if(((MAD_Camera *) AllRadCam[NumberRadCam].Object)->CameraType == MAD_e_OrthoCamera)
        {
            AllRadCam[NumberRadCam].Focale = 0.5f / ((MAD_Camera *) AllRadCam[NumberRadCam].Object)->OrthoInf.ChannelSize;
			AllRadCam[NumberRadCam].IsOrtho = 1;
        }
    }
	
    if(AllRadCam[NumberRadCam].Object->IDType == ID_MAD_Light)
    {
        if(((MAD_Light *) AllRadCam[NumberRadCam].Object)->LightType == MAD_LightSpot)
        {
            AllRadCam[NumberRadCam].Focale = 0.5f /
                (float) tan(((MAD_Light *) AllRadCam[NumberRadCam].Object)->Spot.BigAlpha * 0.5f);
        }
        if(((MAD_Light *) AllRadCam[NumberRadCam].Object)->LightType == MAD_LightDirect)
        {
            AllRadCam[NumberRadCam].Focale = 1.0f / 200.0f;
			AllRadCam[NumberRadCam].IsOrtho = 1;
        }
    }
	
    *(unsigned long *) &AllRadCam[NumberRadCam].Focale &= 0x7fffffff;
	
    /* MWN->Matrix.Translation.z = -MWN->Matrix.Translation.z; */
    NumberRadCam++;
}

/*
===================================================================================================
Points & trianglles counter
===================================================================================================
*/
void DrawObjectNodeCallbackCountPoly(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject *GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
    TotalNumBerOfPoints += GO->NumberOfPoints;
    TotalNumBerOfPolygons += MAD_GetNumberOfFace(GO);
}

/*
===================================================================================================
Enum all objects for Combo4
===================================================================================================
*/
void ObjectPickerInitCallback(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    SendMessage(GetDlgItem(Mainhwnd, IDC_COMBO4), CB_ADDSTRING, 0, (long) MWN->ID.Name);
}

static unsigned long        ObjectSelectedCounter;
static unsigned long        EditNumberLock = 0;
static unsigned long        CurrentObjectSelected;
static MAD_GeometricObject  *SelectedObject;
extern unsigned long        LOD_C_IsObjectLOD(MAD_GeometricObject *ObjectToDraw);
extern void			        LOD_C_FreezeLOD(MAD_GeometricObject *ObjectToDraw);
extern void                 OBJ_ComputeObjectLODForDisplay_Number
(
 MAD_GeometricObject *ObjectToDraw,
 unsigned long		NewNum
 );
extern unsigned long        ShowOnlyThisObject = 0;
void                        Render_MW(void);

/*
===================================================================================================
Retreive picke object from picking buffer
===================================================================================================
*/
void ObjectPickFinder(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    if(ObjectSelectedCounter == CurrentObjectSelected)
        SelectedObject = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
    ObjectSelectedCounter++;
}

/*
===================================================================================================
===================================================================================================
*/
void InitCurrentObjectSelectedFromCombo(void)
{
    CurrentObjectSelected = SendDlgItemMessage(Mainhwnd, IDC_COMBO4, CB_GETCURSEL, 0, 0);
    ObjectSelectedCounter = 0;
    SelectedObject = NULL;
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, ObjectPickFinder);
    EditNumberLock = 1;
    if(SelectedObject)
    {
        MAD_EnableWindow(IDC_CHECK6, 1);
        MAD_EnableWindow(IDC_CHECK5, 1);
        if(LOD_C_IsObjectLOD(SelectedObject))
        {
            MAD_EnableWindow(IDC_EDIT3, 1);
            MAD_EnableWindow(IDC_TEXTNumPoints, 1);
			ShowWindow( RollHwnd, 1); 
            SetDlgItemInt(Mainhwnd, IDC_EDIT3, SelectedObject->NumberOfPoints, FALSE);
        }
        else
        {
            MAD_EnableWindow(IDC_EDIT3, 0);
            MAD_EnableWindow(IDC_TEXTNumPoints, 0);
			ShowWindow( RollHwnd, 0); 
            SetDlgItemInt(Mainhwnd, IDC_EDIT3, SelectedObject->NumberOfPoints, FALSE);
        }
		
        if(SelectedObject->ObjectFlags & GO_ShadowCut)
            CheckDlgButton(Mainhwnd, IDC_CHECK6, BST_CHECKED);
        else
            CheckDlgButton(Mainhwnd, IDC_CHECK6, BST_UNCHECKED);

/*		if (SelectedObject -> ObjectFlags & GO_NonMagnifold)
			MAD_EnableWindow(IDC_CHECK6, 0);*/
    }
    else
    {
        MAD_EnableWindow(IDC_CHECK6, 0);
        MAD_EnableWindow(IDC_CHECK5, 0);
        MAD_EnableWindow(IDC_EDIT3, 0);
        MAD_EnableWindow(IDC_TEXTNumPoints, 0);
		ShowWindow( RollHwnd, 0); 
        SetDlgItemText(Mainhwnd, IDC_EDIT3, "");
    }
	
    EditNumberLock = 0;
}

/*
===================================================================================================
===================================================================================================
*/
void ModifyPointNumInObjectSelectedFromCombo(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    long   NewNumber;
    int             Succes;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if	(EditNumberLock) return;
	EditNumberLock = 1;
	if (GetDlgItemInt(Mainhwnd, IDC_EDIT3, &Succes, FALSE) > 10000)
	{
		NewNumber = GetDlgItemInt(Mainhwnd, IDC_EDIT3, &Succes, FALSE) - (ROLL_DELTA * 100);
	} else
		if (GetDlgItemInt(Mainhwnd, IDC_EDIT3, &Succes, FALSE) > 1000)
		{
			NewNumber = GetDlgItemInt(Mainhwnd, IDC_EDIT3, &Succes, FALSE) - (ROLL_DELTA * 20);
		} 	else
			if (GetDlgItemInt(Mainhwnd, IDC_EDIT3, &Succes, FALSE) > 100)
			{
				NewNumber = GetDlgItemInt(Mainhwnd, IDC_EDIT3, &Succes, FALSE) - (ROLL_DELTA * 4);
			} 	else
					NewNumber = GetDlgItemInt(Mainhwnd, IDC_EDIT3, &Succes, FALSE) - (ROLL_DELTA );
	ROLL_DELTA = 0;
    Succes = 0;
    if(NewNumber < 0)
    {
        NewNumber = 0;
        Succes = 1;
    }
	
    if((unsigned long)NewNumber >= SelectedObject->RealNumberOfPoints)
    {
        NewNumber = SelectedObject->RealNumberOfPoints - 1;
        Succes = 1;
    }
	

    OBJ_ComputeObjectLODForDisplay_Number
		(
        SelectedObject,
        NewNumber 
		);

    SetDlgItemInt(Mainhwnd, IDC_DISPLAYNBPOINTSOLUTION , SelectedObject->NumberOfPoints, FALSE);
    SetDlgItemInt(Mainhwnd, IDC_EDIT3 , SelectedObject->NumberOfPoints, FALSE);

    Render_MW();
    TotalNumBerOfPoints = 0;
    TotalNumBerOfPolygons = 0;
    CurrentPickingColor = 0;
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, DrawObjectNodeCallbackCountPoly);
    MAD_UpdateInfo();
	EditNumberLock = 0;
}

/*
===================================================================================================
===================================================================================================
*/
void ReinitCurrentLightCam(void)
{
	if (!NumberLightCam) return;
    if(((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->IsOn)
        CheckDlgButton(Mainhwnd, IDC_CHECK10, BST_CHECKED);
    else
        CheckDlgButton(Mainhwnd, IDC_CHECK10, BST_UNCHECKED);
	
    if(((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->LightFlags & MAD_LightFlag_OverWalls)
        CheckDlgButton(Mainhwnd, IDC_CHECK9, BST_CHECKED);
    else
        CheckDlgButton(Mainhwnd, IDC_CHECK9, BST_UNCHECKED);
    if
		(
        ((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->LightFlags & MAD_LightFlag_CastShadow
		)
        CheckDlgButton(Mainhwnd, IDC_CHECK8, BST_CHECKED);
    else
        CheckDlgButton(Mainhwnd, IDC_CHECK8, BST_UNCHECKED);
    if
		(
        ((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->LightFlags & MAD_LightFlag_PaintLight
		)
        CheckDlgButton(Mainhwnd, IDC_CHECK7, BST_CHECKED);
    else
        CheckDlgButton(Mainhwnd, IDC_CHECK7, BST_UNCHECKED);
}

/*
===================================================================================================
===================================================================================================
*/
void EnumRadCam(void)
{
    SendMessage(hwndComboCam, CB_RESETCONTENT, 0, 0);
    NumberLightCam = 0;
    SendMessage(GetDlgItem(Mainhwnd, IDC_COMBO5), CB_RESETCONTENT, 0, 0);
	
    MAD_NodeEnum(MW, ID_MAD_Light, RadNodeCallback);
    NumberLightCam = NumberRadCam;
    MAD_NodeEnum(MW, ID_MAD_Camera, RadNodeCallback);
    SendMessage(hwndComboCam, CB_SETCURSEL, 0, 0);
	
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, ObjectPickerInitCallback);
//    SendMessage(GetDlgItem(Mainhwnd, IDC_COMBO4), CB_SETCURSEL, 0, 0);
    SendMessage(GetDlgItem(Mainhwnd, IDC_COMBO5), CB_SETCURSEL, 0, 0);
    InitCurrentObjectSelectedFromCombo();
	
    CurrentRadCam = 0;
    ReinitCurrentLightCam();
}

/*
===================================================================================================
===================================================================================================
*/
void Transp_Matrix(MAD_Matrix *Result)
{
    FSWAP(Result->J.x, Result->I.y);
    FSWAP(Result->K.x, Result->I.z);
    FSWAP(Result->K.y, Result->J.z);
}

/*
===================================================================================================
===================================================================================================
*/
inline void Mul_Matrix_Ras3D(MAD_Matrix *M, MAD_Simple3DVertex *Src, MAD_R_Vertex *Result)
{
    Result->X = M->Translation.x;
    Result->Y = M->Translation.y;
    Result->Z = M->Translation.z;
    Result->X += RAD_DOT_PRODUCT((&M->I), (&Src->Point), x, y, z);
    Result->Y += RAD_DOT_PRODUCT((&M->J), (&Src->Point), x, y, z);
    Result->Z += RAD_DOT_PRODUCT((&M->K), (&Src->Point), x, y, z);
}

#define NegVect(a) \
    a.x = -a.x; \
    a.y = -a.y; \
a.z = -a.z;

/*
===================================================================================================
===================================================================================================
*/
void NegMatrix(MAD_Matrix *Result)
{
    NegVect(Result->Translation);
    NegVect(Result->I);
    NegVect(Result->J);
    NegVect(Result->K);
}

/*
===================================================================================================
===================================================================================================
*/
void InvScaleMatrix(MAD_Matrix *Result)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   Lenght;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Transp_Matrix(Result);
    Lenght = Result->I.x * Result->I.x + Result->I.y * Result->I.y + Result->I.z * Result->I.z;
    Lenght = 1.0f / (float) sqrt(Lenght);
    Lenght *= Lenght;
    Result->I.x *= Lenght;
    Result->I.y *= Lenght;
    Result->I.z *= Lenght;
	
    Lenght = Result->J.x * Result->J.x + Result->J.y * Result->J.y + Result->J.z * Result->J.z;
    Lenght = 1.0f / (float) sqrt(Lenght);
    Lenght *= Lenght;
    Result->J.x *= Lenght;
    Result->J.y *= Lenght;
    Result->J.z *= Lenght;
	
    Lenght = Result->K.x * Result->K.x + Result->K.y * Result->K.y + Result->K.z * Result->K.z;
    Lenght = 1.0f / (float) sqrt(Lenght);
    Lenght *= Lenght;
    Result->K.x *= Lenght;
    Result->K.y *= Lenght;
    Result->K.z *= Lenght;
    Transp_Matrix(Result);
}

/*
===================================================================================================
Matrix * Matrix
===================================================================================================
*/
void Matrix_Matrix(MAD_Matrix *M1, MAD_Matrix *M2, MAD_Matrix *Result)
{
    Transp_Matrix(M2);
    Result->Translation.x = RAD_DOT_PRODUCT((&M2->I), (&M1->Translation), x, y, z) +
        M2->Translation.x;
    Result->Translation.y = RAD_DOT_PRODUCT((&M2->J), (&M1->Translation), x, y, z) +
        M2->Translation.y;
    Result->Translation.z = RAD_DOT_PRODUCT((&M2->K), (&M1->Translation), x, y, z) +
        M2->Translation.z;
    Result->I.x = RAD_DOT_PRODUCT((&M1->I), (&M2->I), x, y, z);
    Result->I.y = RAD_DOT_PRODUCT((&M1->I), (&M2->J), x, y, z);
    Result->I.z = RAD_DOT_PRODUCT((&M1->I), (&M2->K), x, y, z);
    Result->J.x = RAD_DOT_PRODUCT((&M1->J), (&M2->I), x, y, z);
    Result->J.y = RAD_DOT_PRODUCT((&M1->J), (&M2->J), x, y, z);
    Result->J.z = RAD_DOT_PRODUCT((&M1->J), (&M2->K), x, y, z);
    Result->K.x = RAD_DOT_PRODUCT((&M1->K), (&M2->I), x, y, z);
    Result->K.y = RAD_DOT_PRODUCT((&M1->K), (&M2->J), x, y, z);
    Result->K.z = RAD_DOT_PRODUCT((&M1->K), (&M2->K), x, y, z);
    Transp_Matrix(M2);
}

/*
===================================================================================================
===================================================================================================
*/
void MAtrixFromLongitudeLatitude(MAD_Matrix *M, float Lat, float Long )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Matrix  M1;
    MAD_Matrix  M2;
    MAD_Vertex  Trans;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Trans = M->Translation;
    M->Translation.x = 0.0f;
    M->Translation.y = 0.0f;
    M->Translation.z = 0.0f;
    memset(&M1, 0, sizeof(M1));
    M1.K.x = (float) sin(Long);
    M1.K.y = (float) cos(Long);
    M1.K.z = 0.0f;
    M1.I.x = -M1.K.y;
    M1.I.y = M1.K.x;
    M1.I.z = 0.0f;
    M1.J.x = 0.0f;
    M1.J.y = 0.0f;
    M1.J.z = 1.0f;

    memset(&M2, 0, sizeof(M2));
    M2.I.x = 1.0f;
    M2.I.y = 0.0f;
    M2.I.z = 0.0f;
    M2.J.x = 0.0f;
    M2.J.y = (float) cos(Lat);
    M2.J.z = (float) sin(Lat);
    M2.K.x = 0.0f;
    M2.K.y = -(float) sin(Lat);
    M2.K.z = (float) cos(Lat);

    Matrix_Matrix(&M2, &M1, M);
	
    /* *M = M1; */
    M->Translation = Trans;

}

/*
===================================================================================================
===================================================================================================
*/
void LongitudeLatitudeFromMAtrix(MAD_Matrix *M, float *Lat, float *Long)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Vertex  ZDir, ZLoc;
    float       AC;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    ZDir.x = M->K.x;
    ZDir.y = M->K.y;
    ZDir.z = M->K.z;
    ZLoc = ZDir;
    ZDir.z = 0.0f;
    NORMALIZE((&ZDir), x, y, z);
    if(ZDir.x > 1.0f) ZDir.x = 1.0f;
    if(ZDir.x < -1.0f) ZDir.x = -1.0f;
    AC = (float) asin(ZDir.x);
    if(ZDir.y < 0.0f)
    {
        AC = 3.1415927f - AC;
    }
	
    *Long = AC;
    if(ZLoc.z > 1.0f) ZLoc.z = 1.0f;
    if(ZLoc.z < -1.0f) ZLoc.z = -1.0f;
    *Lat = (float) asin(-ZLoc.z);
}

/*
===================================================================================================
===================================================================================================
*/
void NormMatrix(MAD_Matrix *Result)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   Lenght;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Transp_Matrix(Result);
    Lenght = Result->I.x * Result->I.x + Result->I.y * Result->I.y + Result->I.z * Result->I.z;
    Lenght = 1.0f / (float) sqrt(Lenght);
    Result->I.x *= Lenght;
    Result->I.y *= Lenght;
    Result->I.z *= Lenght;
	
    Lenght = Result->J.x * Result->J.x + Result->J.y * Result->J.y + Result->J.z * Result->J.z;
    Lenght = 1.0f / (float) sqrt(Lenght);
    Result->J.x *= Lenght;
    Result->J.y *= Lenght;
    Result->J.z *= Lenght;
	
    Lenght = Result->K.x * Result->K.x + Result->K.y * Result->K.y + Result->K.z * Result->K.z;
    Lenght = 1.0f / (float) sqrt(Lenght);
    Result->K.x *= Lenght;
    Result->K.y *= Lenght;
    Result->K.z *= Lenght;
    Transp_Matrix(Result);
}

/*
===================================================================================================
===================================================================================================
*/
void Inv_Matrix(MAD_Matrix *M1, MAD_Matrix *Result)
{
    if(Result != M1) *Result = *M1;
    Transp_Matrix(Result);
    InvScaleMatrix(Result);
    Transp_Matrix(Result);
    Result->Translation.x = -RAD_DOT_PRODUCT((&Result->I), (&M1->Translation), x, y, z);
    Result->Translation.y = -RAD_DOT_PRODUCT((&Result->J), (&M1->Translation), x, y, z);
    Result->Translation.z = -RAD_DOT_PRODUCT((&Result->K), (&M1->Translation), x, y, z);
    Transp_Matrix(Result);
}

/*
===================================================================================================
Init a matrix for multi - Subview ( omni lights )
===================================================================================================
*/
void MAD_IniTMatrixDir(MAD_Matrix *LightMatrix, unsigned long QuarterCounter)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Vertex  TranslationSave;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* 0 North 1 East 2 South 3 West 4 Up 5 Down */
    TranslationSave = LightMatrix->Translation;
    memset(LightMatrix, 0, sizeof(*LightMatrix));
    LightMatrix->Translation = TranslationSave;
    switch(QuarterCounter)
    {
    case 0:
        LightMatrix->I.x = 1.0f;
        LightMatrix->J.z = 1.0f;
        LightMatrix->K.y = -1.0f;
        break;
    case 1:
        LightMatrix->I.y = 1.0f;
        LightMatrix->J.z = -1.0f;
        LightMatrix->K.x = -1.0f;
        break;
    case 2:
        LightMatrix->I.x = -1.0f;
        LightMatrix->J.z = 1.0f;
        LightMatrix->K.y = 1.0f;
        break;
    case 3:
        LightMatrix->I.y = 1.0f;
        LightMatrix->J.z = 1.0f;
        LightMatrix->K.x = 1.0f;
        break;
    case 4:
        LightMatrix->I.x = 1.0f;
        LightMatrix->J.y = -1.0f;
        LightMatrix->K.z = -1.0f;
        break;
    case 5:
        LightMatrix->I.x = 1.0f;
        LightMatrix->J.y = 1.0f;
        LightMatrix->K.z = 1.0f;
        break;
    }
	
    /* Transp_Matrix(LightMatrix); */
}

/*
===================================================================================================
Test visibility flag
===================================================================================================
*/
inline unsigned long IS_Visible(MAD_Simple3DVertex *S3D)
{
	return (*(unsigned long *) &(S3D->Normale.x) & 1) ;
}


/*
===================================================================================================
Compute Bounding Box of an object
===================================================================================================
*/
void ComputeBDVOLCallback(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject *GO;
    int                 Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MWN -> p_stBDV = (MAD_BoundingVolume *)malloc(sizeof (MAD_BoundingVolume));
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
    MWN->p_stBDV ->XMin = MWN->p_stBDV ->YMin = MWN->p_stBDV ->ZMin = MR_Infinite;
    MWN->p_stBDV ->XMax = MWN->p_stBDV ->YMax = MWN->p_stBDV ->ZMax = -MR_Infinite;
	
    for(Counter = 0; Counter < (int) GO->NumberOfPoints; Counter++)
    {
        if(MWN -> p_stBDV ->XMin > (GO->OBJ_PointList + Counter)->Point.x)
            MWN -> p_stBDV ->XMin = (GO->OBJ_PointList + Counter)->Point.x;
        if(MWN -> p_stBDV ->YMin > (GO->OBJ_PointList + Counter)->Point.y)
            MWN -> p_stBDV ->YMin = (GO->OBJ_PointList + Counter)->Point.y;
        if(MWN -> p_stBDV ->ZMin > (GO->OBJ_PointList + Counter)->Point.z)
            MWN -> p_stBDV ->ZMin = (GO->OBJ_PointList + Counter)->Point.z;
        if(MWN -> p_stBDV ->XMax < (GO->OBJ_PointList + Counter)->Point.x)
            MWN -> p_stBDV ->XMax = (GO->OBJ_PointList + Counter)->Point.x;
        if(MWN -> p_stBDV ->YMax < (GO->OBJ_PointList + Counter)->Point.y)
            MWN -> p_stBDV ->YMax = (GO->OBJ_PointList + Counter)->Point.y;
        if(MWN -> p_stBDV ->ZMax < (GO->OBJ_PointList + Counter)->Point.z)
            MWN -> p_stBDV ->ZMax = (GO->OBJ_PointList + Counter)->Point.z;
    }
}

/*
===================================================================================================
Compute Bounding Box of an object
===================================================================================================
*/

/* DrawBDVOL */
#define EXTRACTVALUE    ExtractV

/*
===================================================================================================
===================================================================================================
*/
void Draw_BOX(MAD_WorldNode *MWN, MAD_Matrix *MM)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_R_Clipping_Info LocalCI;
    unsigned long       Counter1, Counter2, And, Ext;
    float               ExtractV;
    MAD_R_Vertex        P[2];
    MAD_R_Vertex        Send[2];
    MAD_Simple3DVertex  PS3V[2];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    LocalCI = CI;
    for(Ext = 0; Ext < 1; Ext++)
    {
        ExtractV = (float) Ext;
        for(Counter1 = 0; Counter1 < 8; Counter1++)
        {
            if(Counter1 & 1)
                PS3V[0].Point.x = MWN -> p_stBDV->XMin - EXTRACTVALUE;
            else
                PS3V[0].Point.x = MWN -> p_stBDV->XMax + EXTRACTVALUE;
			
            if(Counter1 & 2)
                PS3V[0].Point.y = MWN -> p_stBDV->YMin - EXTRACTVALUE;
            else
                PS3V[0].Point.y = MWN -> p_stBDV->YMax + EXTRACTVALUE;
			
            if(Counter1 & 4)
                PS3V[0].Point.z = MWN -> p_stBDV->ZMin - EXTRACTVALUE;
            else
                PS3V[0].Point.z = MWN -> p_stBDV->ZMax + EXTRACTVALUE;
            Mul_Matrix_Ras3D(MM, &PS3V[0], &P[0]);
            P[0].Z = -P[0].Z;
            for(Counter2 = Counter1; Counter2 < 8; Counter2++)
            {
                And = Counter2 ^ Counter1;
                if((And == 1) || (And == 2) || (And == 4))
                {
                    if(Counter2 & 1)
                        PS3V[1].Point.x = MWN -> p_stBDV->XMin - EXTRACTVALUE;
                    else
                        PS3V[1].Point.x = MWN -> p_stBDV->XMax + EXTRACTVALUE;
					
                    if(Counter2 & 2)
                        PS3V[1].Point.y = MWN -> p_stBDV->YMin - EXTRACTVALUE;
                    else
                        PS3V[1].Point.y = MWN -> p_stBDV->YMax + EXTRACTVALUE;
					
                    if(Counter2 & 4)
                        PS3V[1].Point.z = MWN -> p_stBDV->ZMin - EXTRACTVALUE;
                    else
                        PS3V[1].Point.z = MWN -> p_stBDV->ZMax + EXTRACTVALUE;
                    Mul_Matrix_Ras3D(MM, &PS3V[1], &P[1]);
                    P[1].Z = -P[1].Z;

                    Send[1].X = (P[0].X + P[1].X * MAD_BOX_SUBVECTLEN) / (1.0f + MAD_BOX_SUBVECTLEN);
                    Send[1].Y = (P[0].Y + P[1].Y * MAD_BOX_SUBVECTLEN) / (1.0f + MAD_BOX_SUBVECTLEN);
                    Send[1].Z = (P[0].Z + P[1].Z * MAD_BOX_SUBVECTLEN) / (1.0f + MAD_BOX_SUBVECTLEN);
					Send[0] = P[0];
                    /* ClipDrawLine ( P , &CI , MAD_DrawLineZBuffer); */
                    ClipDrawLine(Send, &CI, MAD_DrawLine_Special);
                    Send[1].X = (P[1].X + P[0].X * MAD_BOX_SUBVECTLEN) / (1.0f + MAD_BOX_SUBVECTLEN);
                    Send[1].Y = (P[1].Y + P[0].Y * MAD_BOX_SUBVECTLEN) / (1.0f + MAD_BOX_SUBVECTLEN);
                    Send[1].Z = (P[1].Z + P[0].Z * MAD_BOX_SUBVECTLEN) / (1.0f + MAD_BOX_SUBVECTLEN);
					Send[0] = P[1];
                    /* ClipDrawLine ( P , &CI , MAD_DrawLineZBuffer); */
                    ClipDrawLine(Send, &CI, MAD_DrawLine_Special);

                }
            }
        }
    }
}

#define MR_XinCull  1
#define MR_XaxCull  2
#define MR_YinCull  4
#define MR_YaxCull  8
#define MR_ZinCull  16
#define MR_ZaxCull  32

/*
===================================================================================================
Point Culling
===================================================================================================
*/
unsigned long ISVisible_Point(MAD_R_Vertex *P, float Z)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   ReturnValue;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    ReturnValue = 0;
    if(P->X < CI.XMin)
        ReturnValue |= MR_XinCull;
    if(P->X > CI.XMax )
        ReturnValue |= MR_XaxCull;
    if(P->Y < CI.YMin)
        ReturnValue |= MR_YinCull;
    if(P->Y > CI.YMax )
        ReturnValue |= MR_YaxCull;
    if(Z < CI.ZMin)
        ReturnValue |= MR_ZinCull;
    if(Z > CI.ZMax)
        ReturnValue |= MR_ZaxCull;

    /* If (P3D -> Point.z > CI . ZMax) ReturnValue |= MR_ZaxCull; */
    return ReturnValue;
}

/*
===================================================================================================
Object culling
===================================================================================================
*/
unsigned long ISVisible
(
 MAD_WorldNode       *MWN,
 MAD_GeometricObject *GO,
 MAD_Matrix          *MM,
 unsigned long       *CullOr
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 Counter;
    unsigned long       Cull;
    MAD_Simple3DVertex  Point;
    MAD_R_Vertex        P, P2D;
    float               Z;
	unsigned long		Zob;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Cull = MR_XinCull | MR_XaxCull | MR_YinCull | MR_YaxCull | MR_ZinCull | MR_ZaxCull;
    *CullOr = 0;

	*(float *)&Zob = 10000000.0f;
	
    for(Counter = 0; Counter < 8; Counter++)
    {
        if(Counter & 1)
            Point.Point.x = MWN -> p_stBDV->XMin;
        else
            Point.Point.x = MWN -> p_stBDV->XMax;
		
        if(Counter & 2)
            Point.Point.y = MWN -> p_stBDV->YMin;
        else
            Point.Point.y = MWN -> p_stBDV->YMax;
		
        if(Counter & 4)
            Point.Point.z = MWN -> p_stBDV->ZMin;
        else
            Point.Point.z = MWN -> p_stBDV->ZMax;
	
        Mul_Matrix_Ras3D(MM, &Point, &P);
        P.Z = -P.Z;
        Z = P.Z;
		if (Z < CI.ZMin)
		{
			*(unsigned long *)&P . X = (*(unsigned long *)&Point.Point.x & 0x80000000) | Zob;
			*(unsigned long *)&P . Y = (*(unsigned long *)&Point.Point.y & 0x80000000) | Zob;
			*(unsigned long *)&P . X ^= 0x80000000;
			*(unsigned long *)&P . Y ^= 0x80000000;
			P.Z = CI.ZMin;
		}
        CI.ProjectVertexCLBK(&P, &P2D, &CI);
        Cull &= ISVisible_Point(&P2D, Z);
        *CullOr |= ISVisible_Point(&P2D, Z);
    }
	
    if(Cull)
        return 0;
    else
        return 1;
}

/*
===================================================================================================
Serial transformation
===================================================================================================
*/
void Serial_MulMatrixPoint
(
 MAD_Matrix          *MM,
 MAD_R_Vertex        *Dest,
 MAD_Simple3DVertex  *Source,
 unsigned long       Number
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_R_Vertex    *LasSrc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    LasSrc = Dest + Number;
    while(LasSrc > Dest)
    {
        Mul_Matrix_Ras3D(MM, Source++, Dest++);
    }
	
    Dest -= Number;
    Source -= Number;
    while(LasSrc > Dest)
    {
        *(unsigned long *) &Dest->Z ^= 0x80000000;
        Dest++;
    }
}
void Serial_Projette
(
 MAD_Matrix          *MM,
 MAD_R_Vertex        *Dest,
 MAD_R_Vertex        *Source,
 unsigned long       Number
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_R_Vertex    *LasSrc;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    LasSrc = Dest + Number;
    while(LasSrc > Dest)
    {
		CI.ProjectVertexCLBK(Source++, Dest++, &CI);
    }
	
}

static unsigned long    NumEntryWB = 0;
MAD_R_Vertex            *WorkingBuffer = NULL;
MAD_R_Vertex            *WorkingBuffer2D = NULL;

/*
===================================================================================================
Rendering object
===================================================================================================
*/

void MAD_ColorUlong2Float(unsigned long ULColor, float *Red, float *Green, float *Blue)
{
	*Red = (float)((ULColor >> 16) & 0xff) * 1.0f / 255.0f;
	*Green= (float)((ULColor >> 8) & 0xff) * 1.0f / 255.0f;
	*Blue= (float)(ULColor & 0xff) * 1.0f / 255.0f;
}

void Render_GO(MAD_World *MW, MAD_GeometricObject *GO, MAD_Matrix *MM, unsigned long Visibility)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long               Counter, Counter2;
    MAD_GeometricObjectElement  *Current;
    MAD_Face                    *CurrentFace;
    MAD_R_Vertex                P[3];
    float                       LenMax;
	float RedMult , GreenMult , BlueMult;
	MAD_MAT_MatID *LocalMat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(GO->NumberOfPoints > NumEntryWB)
    {
        if(WorkingBuffer) free(WorkingBuffer);
        if(WorkingBuffer2D) free(WorkingBuffer2D);
        NumEntryWB = GO->NumberOfPoints + 512;
        WorkingBuffer = (MAD_R_Vertex *) malloc(NumEntryWB * sizeof(MAD_R_Vertex));
        WorkingBuffer2D = (MAD_R_Vertex *) malloc(NumEntryWB * sizeof(MAD_R_Vertex));
    }

    Serial_MulMatrixPoint(MM, WorkingBuffer, GO->OBJ_PointList, GO->NumberOfPoints);

    for(Counter = 0; Counter < GO->NumberOfElements; Counter++)
    {
        Current = GO->Elements[Counter];
        CurrentFace = Current->Faces;
		RedMult  = 1.0f;
		GreenMult= 1.0f; 
		BlueMult = 1.0f;
		
		if (Displaymode == 2)
		{
			if (Mat != NULL)
			{
				LocalMat = NULL;
				if (Mat -> MaterialType == ID_MAT_Standard)
					LocalMat = Mat;
				if (Mat -> MaterialType == ID_MAT_MultiMaterial)
					LocalMat = MW -> AllMaterial[((MAD_MultiMaterial *)Mat )-> SubMats[Current -> MaterialID % ((MAD_MultiMaterial *)Mat )-> NumberOfSubMaterials]];
				if (LocalMat != NULL)
				{
					if (LocalMat -> MaterialType == ID_MAT_Standard)
					{
						MAD_ColorUlong2Float(((MAD_StandarMaterial *)LocalMat)->Diffuse , &RedMult , &GreenMult , &BlueMult);
					}
				}
			}
		}
        for(Counter2 = 0; Counter2 < Current->NumberOfTriangles; Counter2++, CurrentFace++)
        {
			CurrentEdgeFlag = CurrentFace->MAXflags;
            P[0] = WorkingBuffer[CurrentFace->Index[0]];
            P[2] = WorkingBuffer[CurrentFace->Index[1]];
            P[1] = WorkingBuffer[CurrentFace->Index[2]];
            if (CI.RasterizeCallback == RasterizeCallbackD1)
            {
#ifdef MAD_SHOW_ELEMENTS
				if (Displaymode == 3)
				{
					RedMult = 0.0f;
					GreenMult = 0.0f;
					BlueMult = 0.0f;
					if (Counter & 1) RedMult += 127.0f;
					if (Counter & 2) RedMult += 127.0f;
					if (Counter & 4) BlueMult += 127.0f;
					if (Counter & 8) BlueMult += 127.0f;
					if (Counter & 16) GreenMult += 127.0f;
					if (Counter & 32) GreenMult += 127.0f;
					P[0].SclrToInt[0] = RedMult;
					P[1].SclrToInt[0] = RedMult;
					P[2].SclrToInt[0] = RedMult;
					P[0].SclrToInt[1] = GreenMult;
					P[2].SclrToInt[1] = GreenMult;
					P[1].SclrToInt[1] = GreenMult;
					P[0].SclrToInt[2] = BlueMult;
					P[2].SclrToInt[2] = BlueMult;
					P[1].SclrToInt[2] = BlueMult;
				} else
#endif
				{
					P[0].SclrToInt[0] = (float)MAD_GetRed(GO->Colors[CurrentFace->Index[0]]) * RedMult	;
					P[0].SclrToInt[1] = (float)MAD_GetGreen(GO->Colors[CurrentFace->Index[0]]) * GreenMult ;
					P[0].SclrToInt[2] = (float)MAD_GetBlue(GO->Colors[CurrentFace->Index[0]]) * BlueMult	;
					P[2].SclrToInt[0] = (float)MAD_GetRed(GO->Colors[CurrentFace->Index[1]]) * RedMult	;
					P[2].SclrToInt[1] = (float)MAD_GetGreen(GO->Colors[CurrentFace->Index[1]]) * GreenMult ;
					P[2].SclrToInt[2] = (float)MAD_GetBlue(GO->Colors[CurrentFace->Index[1]]) * BlueMult	;
					P[1].SclrToInt[0] = (float)MAD_GetRed(GO->Colors[CurrentFace->Index[2]]) * RedMult	;
					P[1].SclrToInt[1] = (float)MAD_GetGreen(GO->Colors[CurrentFace->Index[2]]) * GreenMult ;
					P[1].SclrToInt[2] = (float)MAD_GetBlue(GO->Colors[CurrentFace->Index[2]]) * BlueMult	;
				}
            }
/*			if (CI.RasterizeCallback == RasterizeMarkCallback)
				CurrentFace->MAXflags &= ~(1<<8);*/
			
            CurrentSurface = 0;
            if(OverWalls)
                CurrentTriangleColor = 0x00FF0000;
            else
                CurrentTriangleColor = CurrentFace->Undefined0 >> 8;
			CI.ClipFlags = 0;
            MAD_Rasterize_TriQuad3D(P, &CI, &LenMax);
        }
    }
}

/*
===================================================================================================
Special rendering object for detect
===================================================================================================
*/
void Detect_GO(MAD_World *MW, MAD_GeometricObject *GO, MAD_Matrix *MM, unsigned long Visibility)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long               Counter, Counter2;
    MAD_GeometricObjectElement  *Current;
    MAD_Face                    *CurrentFace;
    MAD_R_Vertex                P[3];
    float                       LenMax , Limit;
    unsigned long               VertexVisibility, VertexFarNear;
    MAD_Simple3DVertex          *S3V[3];
	unsigned long				SurfaceAnd;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if(GO->NumberOfPoints > NumEntryWB)
    {
        if(WorkingBuffer) free(WorkingBuffer);
        if(WorkingBuffer2D) free(WorkingBuffer2D);
        NumEntryWB = GO->NumberOfPoints + 512;
        WorkingBuffer = (MAD_R_Vertex *) malloc(NumEntryWB * sizeof(MAD_R_Vertex));
        WorkingBuffer2D = (MAD_R_Vertex *) malloc(NumEntryWB * sizeof(MAD_R_Vertex));
    }
	if (Force2sided)
		SurfaceAnd = 0x7fffffff;
	else
		SurfaceAnd = 0xffffffff;
	
    Serial_MulMatrixPoint(MM, WorkingBuffer, GO->OBJ_PointList, GO->NumberOfPoints);

	Limit = (float)SourceSX * (float)SourceSX * 0.5f;
	
    for(Counter = 0; Counter < GO->NumberOfElements; Counter++)
    {
        Current = GO->Elements[Counter];
        CurrentFace = Current->Faces;
        for(Counter2 = 0; Counter2 < Current->NumberOfTriangles; Counter2++, CurrentFace++)
        {
			if (CurrentFace->Undefined0 & 1) continue; /* Already subdivided */
			if (ComputeMaps)
				CurrentFace->MAXflags &= ~(1<<8);
			else
				if (CurrentFace->MAXflags & (1<<8)) 
					continue; /* Stop flag */
            P[0] = WorkingBuffer[CurrentFace->Index[0]];
            P[2] = WorkingBuffer[CurrentFace->Index[1]];
            P[1] = WorkingBuffer[CurrentFace->Index[2]];
            StateOfTriangle = 0;
            CurrentSurface = 0;
            if(OverWalls)
                CurrentTriangleColor = 0x00FF0000;
            else
                CurrentTriangleColor = CurrentFace->Undefined0 >> 8;

			CI.ClipFlags = 0;
			MAD_Rasterize_TriQuad3D(P, &CI, &LenMax);
			*(unsigned long *)&LenMax &= SurfaceAnd;

	        VertexVisibility = 0;
            S3V[0] = GO->OBJ_PointList + CurrentFace->Index[0];
            S3V[1] = GO->OBJ_PointList + CurrentFace->Index[1];
            S3V[2] = GO->OBJ_PointList + CurrentFace->Index[2];
            if(IS_Visible(S3V[0])) VertexVisibility |= 8 + 1;
            if(IS_Visible(S3V[1])) VertexVisibility |= 4;
            if(IS_Visible(S3V[2])) VertexVisibility |= 2;
			
            VertexFarNear = 0;
			
            /* Far */
            if(*(unsigned long *) &S3V[0]->Normale.y & 1) VertexFarNear |= 1;
            if(*(unsigned long *) &S3V[1]->Normale.y & 1) VertexFarNear |= 2;
            if(*(unsigned long *) &S3V[2]->Normale.y & 1) VertexFarNear |= 4;
			
            /* Near */
            if(*(unsigned long *) &S3V[0]->Normale.z & 1) VertexFarNear |= 8;
            if(*(unsigned long *) &S3V[1]->Normale.z & 1) VertexFarNear |= 16;
            if(*(unsigned long *) &S3V[2]->Normale.z & 1) VertexFarNear |= 32;
			
            if(VertexVisibility) StateOfTriangle |= STATE_OVER;
            if(VertexVisibility != (1 + 2 + 4 + 8))
                StateOfTriangle |= STATE_UNDER;
			
            /* Too far ? */
            if((VertexFarNear & (1 + 2 + 4)) == (1 + 2 + 4))
                StateOfTriangle = 0;
            else
/*				if ((!FIRSTGLOW) && ((VertexFarNear & (8 + 16 + 32)) == (8 + 16 + 32)))
					StateOfTriangle = 0;
				else*/
				{
					if((VertexFarNear & (1 + 2 + 4)) != 0)
						StateOfTriangle = (STATE_UNDER | STATE_OVER);
					if
						(
						(VertexFarNear & (8 + 16 + 32)) &&
						((VertexFarNear & (8 + 16 + 32)) != (8 + 16 + 32))
						)
						StateOfTriangle = (STATE_UNDER | STATE_OVER);
				}

			if (LenMax > Limit) StateOfTriangle = (STATE_UNDER | STATE_OVER);
			
            if(StateOfTriangle == (STATE_UNDER | STATE_OVER))
            {
//                if(LenMax >= MAD_SURFACE_THRESHOLD)
                if(LenMax >= 50.0f)
                    CurrentFace->Undefined0 |= 1;
            }
        }
    }
}

/*
===================================================================================================
Detect visibility
===================================================================================================
*/
inline unsigned long Visible_TAGGED(MAD_Simple3DVertex *S3D,MAD_R_Vertex *P3D, MAD_R_Vertex *P2D, unsigned long Tag)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    long            Xecr, Yecr , KKKK;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* Mul_Matrix_Ras3D(MM, S3D , &P3D); */
	KKKK = 0;
    if(P3D->Z > CI.ZMin)
    {
        Xecr = lFloatToLongOpt(P2D->X);
        Yecr = lFloatToLongOpt(P2D->Y);
        if((Xecr >= CI.XMin) && (Xecr < CI.XMax) && (Yecr >= CI.YMin) && (Yecr < CI.YMax))
        {
            if(TABLEAU[(Yecr << SourceSXShift) + Xecr] == Tag)
            {
                *(unsigned long *) &(S3D->Normale.x) |= 1;
            } else
			{
			if ((Xecr < CI.XMax - 1) && (Yecr < CI.YMax - 1) )
			if ((Xecr > CI.XMin) && (Yecr > CI.YMin) )
			{
				if
					(
					(TABLEAU[((Yecr + 1) << SourceSXShift) + Xecr] == Tag) ||
					(TABLEAU[((Yecr - 1) << SourceSXShift) + Xecr] == Tag) ||
					(TABLEAU[(Yecr << SourceSXShift) + Xecr + 1] == Tag) ||
					(TABLEAU[(Yecr << SourceSXShift) + Xecr - 1] == Tag)
					)
				{
					*(unsigned long *) &(S3D->Normale.x) |= 1;
				}
			}//*/
			}
			KKKK = 1;
        } 

    }
	if ((!FIRSTGLOW) && (!KKKK) && (!(CurrentLightType == MAD_LightOmni)))
	{
		*(unsigned long *) &(S3D->Normale.x) &= 0xfffffffe;
		*(unsigned long *) &(S3D->Normale.x) |= (*(unsigned long *) &(S3D->Normale.x) & 0x00000002) >> 1;
	}
	
    return 0;
}
/*
===================================================================================================
===================================================================================================
*/
void Compute_Vertex_Visibility2(MAD_World *MW, MAD_GeometricObject *GO, MAD_Matrix *MM)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long               Counter, Counter3;
    MAD_GeometricObjectElement  *Current;
    MAD_Face                    *CurrentFace,*LastFace;
    MAD_R_Vertex                *P,*P2D;
//    MAD_R_Vertex                P[3];
    float                       SQR_Near, SQR_Far, Dist;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if(GO->NumberOfPoints > NumEntryWB)
    {
        if(WorkingBuffer) free(WorkingBuffer);
        if(WorkingBuffer2D) free(WorkingBuffer2D);
        NumEntryWB = GO->NumberOfPoints + 512;
        WorkingBuffer = (MAD_R_Vertex *) malloc(NumEntryWB * sizeof(MAD_R_Vertex));
        WorkingBuffer2D = (MAD_R_Vertex *) malloc(NumEntryWB * sizeof(MAD_R_Vertex));
    }
    SQR_Far = CurrentLightFar * CurrentLightFar;
    SQR_Near = CurrentLightNear * CurrentLightNear;
	
    Serial_MulMatrixPoint(MM, WorkingBuffer, GO->OBJ_PointList, GO->NumberOfPoints);
    Serial_Projette(MM, WorkingBuffer2D, WorkingBuffer, GO->NumberOfPoints);
	
    for(Counter = 0; Counter < GO->NumberOfElements; Counter++)
    {
        Current = GO->Elements[Counter];
        CurrentFace = Current->Faces;
		LastFace = CurrentFace + Current->NumberOfTriangles;
		while (CurrentFace < LastFace)
		{
			StateOfTriangle = 0;
			if(OverWalls)
				CurrentTriangleColor = 0x00FF0000;
			else
				CurrentTriangleColor = CurrentFace->Undefined0 >> 8;
			for(Counter3 = 0; Counter3 < 3; Counter3++)
			{
				if (*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[Counter3])->Normale.x) & 1) 
					continue;
				P = &WorkingBuffer[CurrentFace->Index[Counter3]];
				P2D = &WorkingBuffer2D[CurrentFace->Index[Counter3]];
				Visible_TAGGED((GO->OBJ_PointList + CurrentFace->Index[Counter3]), P, P2D , CurrentTriangleColor);
				//Distances test 
				if (FIRSTGLOW)
				{
					Dist = P->X * P->X + P->Y * P->Y + P->Z * P->Z;
					if(Dist > SQR_Far)
						*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[Counter3])->Normale.y) |= 1;
					if(Dist > SQR_Near)
						*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[Counter3])->Normale.z) |= 1;

				}
			}
			CurrentFace++;
		}
    }//*/
/*    for(Counter = 0; Counter < GO->NumberOfElements; Counter++)
    {
        Current = GO->Elements[Counter];
        CurrentFace = Current->Faces;
        for(Counter2 = 0; Counter2 < Current->NumberOfTriangles; Counter2++, CurrentFace++)
        {
            StateOfTriangle = 0;
            P[0] = WorkingBuffer[CurrentFace->Index[0]];
            P[1] = WorkingBuffer[CurrentFace->Index[1]];
            P[2] = WorkingBuffer[CurrentFace->Index[2]];
            if(OverWalls)
                CurrentTriangleColor = 0x00FF0000;
            else
                CurrentTriangleColor = CurrentFace->Undefined0 >> 8;
            Visible_TAGGED((GO->OBJ_PointList + CurrentFace->Index[0]), &P[0], CurrentTriangleColor);
            Visible_TAGGED((GO->OBJ_PointList + CurrentFace->Index[1]), &P[1], CurrentTriangleColor);
            Visible_TAGGED((GO->OBJ_PointList + CurrentFace->Index[2]), &P[2], CurrentTriangleColor);

            //Distances test 
			if (FIRSTGLOW)
			{
				Dist = P[0].X * P[0].X + P[0].Y * P[0].Y + P[0].Z * P[0].Z;
				if(Dist > SQR_Far)
					*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[0])->Normale.y) |= 1;
				if(Dist > SQR_Near)
					*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[0])->Normale.z) |= 1;
				Dist = P[1].X * P[1].X + P[1].Y * P[1].Y + P[1].Z * P[1].Z;
				if(Dist > SQR_Far)
					*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[1])->Normale.y) |= 1;
				if(Dist > SQR_Near)
					*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[1])->Normale.z) |= 1;
				Dist = P[2].X * P[2].X + P[2].Y * P[2].Y + P[2].Z * P[2].Z;
				if(Dist > SQR_Far)
					*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[2])->Normale.y) |= 1;
				if(Dist > SQR_Near)
					*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[2])->Normale.z) |= 1;
			}
        }
    }//*/

}

void Compute_Vertex_Visibility(MAD_World *MW, MAD_GeometricObject *GO, MAD_Matrix *MM)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long               Counter, Counter3;
    MAD_GeometricObjectElement  *Current;
    MAD_Face                    *CurrentFace,*LastFace;
    MAD_R_Vertex                *P,*P2D;
//    MAD_R_Vertex                P[3];
    float                       SQR_Near, SQR_Far, Dist;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if(GO->NumberOfPoints > NumEntryWB)
    {
        if(WorkingBuffer) free(WorkingBuffer);
        if(WorkingBuffer2D) free(WorkingBuffer2D);
        NumEntryWB = GO->NumberOfPoints + 512;
        WorkingBuffer = (MAD_R_Vertex *) malloc(NumEntryWB * sizeof(MAD_R_Vertex));
        WorkingBuffer2D = (MAD_R_Vertex *) malloc(NumEntryWB * sizeof(MAD_R_Vertex));
    }
    SQR_Far = CurrentLightFar * CurrentLightFar;
    SQR_Near = CurrentLightNear * CurrentLightNear;
	
    Serial_MulMatrixPoint(MM, WorkingBuffer, GO->OBJ_PointList, GO->NumberOfPoints);
    Serial_Projette(MM, WorkingBuffer2D, WorkingBuffer, GO->NumberOfPoints);
	
    for(Counter = 0; Counter < GO->NumberOfElements; Counter++)
    {
        Current = GO->Elements[Counter];
        CurrentFace = Current->Faces;
		LastFace = CurrentFace + Current->NumberOfTriangles;
		while (CurrentFace < LastFace)
		{
			StateOfTriangle = 0;
			if(OverWalls)
				CurrentTriangleColor = 0x00FF0000;
			else
				CurrentTriangleColor = CurrentFace->Undefined0 >> 8;
			for(Counter3 = 0; Counter3 < 3; Counter3++)
			{
				if (*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[Counter3])->Normale.x) & 1) 
					continue;
				P = &WorkingBuffer[CurrentFace->Index[Counter3]];
				P2D = &WorkingBuffer2D[CurrentFace->Index[Counter3]];
				Visible_TAGGED((GO->OBJ_PointList + CurrentFace->Index[Counter3]), P, P2D , CurrentTriangleColor);
				//Distances test 
				if (FIRSTGLOW)
				{
					Dist = P->X * P->X + P->Y * P->Y + P->Z * P->Z;
					if(Dist > SQR_Far)
						*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[Counter3])->Normale.y) |= 1;
					if(Dist > SQR_Near)
						*(unsigned long *) &((GO->OBJ_PointList + CurrentFace->Index[Counter3])->Normale.z) |= 1;
				}
			}
			CurrentFace++;
		}
    }//*/
}

static MAD_Matrix   CurrentMatrix;
static MAD_Matrix   CurrentMatrixNotGlowed;


/*
===================================================================================================
===================================================================================================
*/
void DrawObjectNodeCallback(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Matrix      MR;
    unsigned long   Visibility;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Matrix_Matrix(&MWN->Matrix, &CurrentMatrix, &MR);
    if(Displaymode == 3)
    {
        CurrentTriangleColor = MAD_Color_2_RadColor(MWN->WiredColor);
    }

    Transp_Matrix(&MR);
	Mat = NULL;
	{
		if(ISVisible(MWN, (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity], &MR, &Visibility))
			Render_GO(MW, (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity], &MR, Visibility);
	}
    Transp_Matrix(&MR);
}

/*
===================================================================================================
===================================================================================================
*/

void DrawObjectNodeCallback_RENDER(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Matrix      MR;
    unsigned long   Visibility;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Matrix_Matrix(&MWN->Matrix, &CurrentMatrix, &MR);
    if(Displaymode == 3)
    {
        CurrentWireColor = CurrentTriangleColor = MAD_Color_2_RadColor(MWN->WiredColor);
    }
	CurrentWireColor = MAD_Color_2_RadColor(MWN->WiredColor);
	if ((MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity] == SelectedObject)
		CurrentWireColor = 0xffffffff;
	
    Transp_Matrix(&MR);
	Mat = NULL;
	if (MWN -> Material != MAD_NULL_INDEX)
	Mat = MW -> AllMaterial[MWN -> Material];
	
    if(ISVisible(MWN, (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity], &MR, &Visibility))
    {
        if 
			((ShowOnlyThisObject) &&
			 ((MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity] != SelectedObject))
            Draw_BOX(MWN, &MR);
        else
		{
			if (QUAKE_ON && (((MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity])->NumberOfPoints > MAD_RAD_QUKE_NUMMAXPONT))
				Render_GO(MW, (MAD_GeometricObject *) MW->AllObjects[MWN->Object], &MR, Visibility);
			else
				Render_GO(MW, (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity], &MR, Visibility);
		}
    }
	
    Transp_Matrix(&MR);
    CurrentPickingColor++;
}

/*
===================================================================================================
===================================================================================================
*/
void DrawObjectNodeCallback_DrawBox(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Matrix      MR;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Matrix_Matrix(&MWN->Matrix, &CurrentMatrix, &MR);
    Transp_Matrix(&MR);
    if((MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity] == SelectedObject) Draw_BOX(MWN, &MR);
    Transp_Matrix(&MR);
    CurrentPickingColor++;
}

/*
===================================================================================================
===================================================================================================
*/
void DetectObjectNodeCallback(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Matrix      MR;
    unsigned long   Visibility;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Matrix_Matrix(&MWN->Matrix, &CurrentMatrix, &MR);
    Transp_Matrix(&MR);
	if(((MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity])->ObjectFlags & GO_ShadowCut)
		if(ISVisible(MWN, (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity], &MR, &Visibility))
	        Detect_GO(MW, (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity], &MR, Visibility);
    Transp_Matrix(&MR);
}

/*
===================================================================================================
===================================================================================================
*/
void FirstInitObjectClbk(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject         *GO;
    unsigned long               Counter;
    MAD_GeometricObjectElement  *Current;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
	
    MAD_RemoveIllgalFaces(GO);
    GO->NumberOfPointsWithoutRadiosity = GO->NumberOfPoints;
	
    GO->Undefined1 = 0;
    GO->Undefined2 = 0;
    GO->Undefined3 = 0;
	
    for(Counter = 0; Counter < GO->NumberOfElements; Counter++)
    {
        Current = GO->Elements[Counter];
        Current->Undefined0 = 0;
        Current->Undefined1 = 0;
        Current->NumberOfUsedIndex = 0;
        Current->UsedIndex = NULL;
        Current->OneUVPerVertexPerElementBase = 0;
        Current->Undefined5 = 0;
        Current->Undefined6 = 0;
    }

    unsigned long       FaceCounter, NumberOfFace, MID, Nothing;
    MAD_Face            *CurrentFace;
    NumberOfFace = MAD_GetNumberOfFace(GO);

    /* Count number of triangles per vertex */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(GO, FaceCounter, &CurrentFace, &MID, &Nothing);
		CurrentFace ->MAXflags |= 7;
		CurrentFace ->MAXflags &= ~(1<<8);
    }
	
	MAD_ComputeOneIndexPerUVPerElementSolution(GO,0);
}

void    Illum_Norme_GO(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber);

/*
===================================================================================================
===================================================================================================
*/
void BeforeSaveClbk(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject         *GO;
    unsigned long               Counter, Counter2;
    MAD_GeometricObjectElement  *Current;
    MAD_Face                    *CurrentFace;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
	
    /* Cleaning exedent of UV */
	MAD_RestoreOneIndexPerUVPerElementSolution(GO);
//    Illum_Norme_GO(MW, MWN, NodeNumber);
    for(Counter = 0; Counter < GO->NumberOfElements; Counter++)
    {
        Current = GO->Elements[Counter];
        Current->Undefined0 = 0;
        Current->Undefined1 = 0;
        for(Counter2 = 0; Counter2 < Current->NumberOfTriangles; Counter2++, CurrentFace++)
        {
            CurrentFace = Current->Faces;
        }
    }
	
}

/*
===================================================================================================
===================================================================================================
*/
void AfterSaveClbk(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject *GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
	
}

/*
===================================================================================================
===================================================================================================
*/
void ComputeVisibility_CLBK(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Matrix      MR;
    unsigned long   Visibility;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* MAD_UpdateInfoStr(MWN-> ID . Name); */
    Matrix_Matrix(&MWN->Matrix, &CurrentMatrix, &MR);
    Transp_Matrix(&MR);
    if(ISVisible(MWN, (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity], &MR, &Visibility))
        Compute_Vertex_Visibility(MW, (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity], &MR);
    Transp_Matrix(&MR);
}


void MAD_RAD_AddColor(MAD_ColorARGB *Color, unsigned long Counter,MAD_Vertex *CurrentFLOATColor,float Strenght)
{
	MAD_Color4D			*P_P4D;

	P_P4D = (MAD_Color4D *)Color;
	P_P4D += Counter;
	P_P4D->C[0] += CurrentFLOATColor -> x * Strenght;
	P_P4D->C[1] += CurrentFLOATColor -> y * Strenght;
	P_P4D->C[2] += CurrentFLOATColor -> z * Strenght;
}

void MAD_COLOR_2_P4D(MAD_Color4D *P_P4D, unsigned long Color)
{
		P_P4D->C[0] = (float)MAD_GetRed(Color);
		P_P4D->C[1] = (float)MAD_GetGreen(Color);
		P_P4D->C[2] = (float)MAD_GetBlue(Color);
		P_P4D->C[3] = (float)MAD_GetAlpha(Color);
}
/* Called for Keeping originals RLI */
void DrawObjectNodeCallbackIlluminate_USERRLI
(
 MAD_World       *MW,
 MAD_WorldNode   *MWN,
 unsigned long   NodeNumber
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       Counter;
	MAD_GeometricObject *GO;
	MAD_Color4D			stP4D,*p_P4d;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//	if (!FIRSTGLOW) return;
	if (!MWN->UsersRLIOfObject) return;
	if (!(MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)) return;

    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
	p_P4d = (MAD_Color4D *)GO->Colors;
    for(Counter = 0; Counter < GO->NumberOfPoints; Counter++)
    {
		MAD_COLOR_2_P4D ( &stP4D , *(MWN->UsersRLIOfObject + Counter) );
		p_P4d ->C[0] += stP4D.C[0];
		p_P4d ->C[1] += stP4D.C[1];
		p_P4d ->C[2] += stP4D.C[2];
		p_P4d ->C[3] = stP4D.C[3]; /* Keep alpha from Users */
		p_P4d ++;
    }
}
/*
===================================================================================================
ZBuufer is ready , just compute the light for each vertex and cull with ZBuff
===================================================================================================
*/
void DrawObjectNodeCallbackIlluminate_SPOT
(
 MAD_World       *MW,
 MAD_WorldNode   *MWN,
 unsigned long   NodeNumber
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       Counter;
    float               Strenght, DotProd;
    MAD_Matrix          MR;
    MAD_Matrix          Notrans;
    MAD_R_Vertex        P3D, P3DNormale, P2D;
    MAD_Simple3DVertex  N3D;
    MAD_GeometricObject *GO;
    float               OoverLightDiscret;
    float               Dist;
    float               SQR_Near;
    float               SQR_Far;
    float               Oover_SQR_Far_Minus_SQR_Near;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (!(MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)) return;
	
    OoverLightDiscret = 1.0f / (float) LIGHT_DISCRET;
    OoverLightDiscret *= OoverLightDiscret;
	
    SQR_Near = CurrentLightNear * CurrentLightNear;
    SQR_Far = CurrentLightFar * CurrentLightFar;
	
    Oover_SQR_Far_Minus_SQR_Near = 1.0f / (SQR_Far - SQR_Near);
	
    /* MAD_UpdateInfoStr(MWN-> ID . Name); MAD_ComputeNormales(MW , MWN,NodeNumber); */
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
    Matrix_Matrix(&MWN->Matrix, &CurrentMatrixNotGlowed, &MR);
    Transp_Matrix(&MR);
	
    Notrans = MR;
    NormMatrix(&Notrans);
    Notrans.Translation.x = 0.0f;
    Notrans.Translation.y = 0.0f;
    Notrans.Translation.z = 0.0f;
    for(Counter = 0; Counter < GO->NumberOfPoints; Counter++)
    {
        if(IS_Visible(GO->OBJ_PointList + Counter))
        {
            Mul_Matrix_Ras3D(&MR, (GO->OBJ_PointList + Counter), &P3D);
            P3D.Z = -P3D.Z;
            CI.ProjectVertexCLBK(&P3D, &P2D, &CI);
            if(PaintLight)
                DotProd = -1.0f;
            else
            {
                N3D.Point = (GO->OBJ_PointList + Counter)->Normale;
                Mul_Matrix_Ras3D(&Notrans, (&N3D), &P3DNormale);
                DotProd = P3D.X * P3D.X + P3D.Y * P3D.Y + P3D.Z * P3D.Z;
                DotProd = (float) sqrt(DotProd);
                DotProd = -1.0f / DotProd;
                DotProd = (P3D.X * P3DNormale.X + P3D.Y * P3DNormale.Y - P3D.Z * P3DNormale.Z) *
                    DotProd;
            }
			
            if(DotProd < 0.f)
            {
                Strenght = (P2D.X - CI.ProjectCam.PX) * (P2D.X - CI.ProjectCam.PX) +
                    (P2D.Y - CI.ProjectCam.PY) * (P2D.Y - CI.ProjectCam.PY);
                Strenght = (float) sqrt(Strenght);
                if(Strenght < (float) SourceSX * 0.5f)
                {
                    if(Strenght < SourceSX * LittleCirle * 0.5f)
                        Strenght = 255.0f;
                    else
                    {
                        Strenght = 255.0f * (Strenght - SourceSX * LittleCirle * 0.5f) /
                            (SourceSX * 0.5f - SourceSX * LittleCirle * 0.5f);
                        Strenght = 255.0f - Strenght;
                    }
					
                    Strenght *= -DotProd;
					
                    /* Distance Factor */
                    Dist = P3D.X * P3D.X + P3D.Y * P3D.Y + P3D.Z * P3D.Z;
                    if(Dist < SQR_Far)
                    {
                        if(Dist > SQR_Near)
                        {
                            Dist = (SQR_Far - Dist) * Oover_SQR_Far_Minus_SQR_Near;
                        }
                        else
                            Dist = 1.0f;
						
                        Strenght *= Dist;
                        Strenght *= OoverLightDiscret;
/*						if (!FIRSTGLOW)
						{
							if((*(unsigned long *)(&(GO->OBJ_PointList + Counter) -> Normale.x) & 2) == 0)
								Strenght *= 2;
						}
*/						
                        /* Strenght *= 255.0f; */
						MAD_RAD_AddColor(GO->Colors,Counter,&CurrentFLOATColor,Strenght);
                    }
                }
            }
        }
    }
	
    Transp_Matrix(&MR);
	
    /*
	* Matrix_Matrix(&MWN->Matrix, &CurrentMatrix, &MR); if(Displaymode == 3) {
	* CurrentTriangleColor = MAD_Color_2_RadColor(MWN->WiredColor); }
	*/
	
    /* Render_GO(MW, (MAD_GeometricObject *) MW->AllObjects[MWN->Object], &MR); */
	
    /* MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, DrawObjectNodeCallbackIlluminate); */
}

/*
===================================================================================================
ZBuufer is ready , just compute the light for each vertex and cull with ZBuff
===================================================================================================
*/
void DrawObjectNodeCallbackIlluminate_OMNI
(
 MAD_World       *MW,
 MAD_WorldNode   *MWN,
 unsigned long   NodeNumber
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       Counter;
    float               Strenght, DotProd;
    MAD_Matrix          MR;
    MAD_Matrix          Notrans;
    MAD_R_Vertex        P3D, P3DNormale/*, P2D*/;
    MAD_Simple3DVertex  N3D;
    MAD_GeometricObject *GO;
    float               OoverLightDiscret;
    float               SQR_Near;
    float               Dist;
    float               SQR_Far;
    float               Oover_SQR_Far_Minus_SQR_Near;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!(MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)) return;
	
    OoverLightDiscret = 1.0f / (float) LIGHT_DISCRET;
    OoverLightDiscret *= OoverLightDiscret;
	
    SQR_Near = CurrentLightNear * CurrentLightNear;
    SQR_Far = CurrentLightFar * CurrentLightFar;
	
    Oover_SQR_Far_Minus_SQR_Near = 1.0f / (SQR_Far - SQR_Near);
	
    /* MAD_UpdateInfoStr(MWN-> ID . Name); MAD_ComputeNormales(MW , MWN,NodeNumber); */
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
    Matrix_Matrix(&MWN->Matrix, &CurrentMatrixNotGlowed, &MR);
    Transp_Matrix(&MR);
	
    Notrans = MR;
    NormMatrix(&Notrans);
    Notrans.Translation.x = 0.0f;
    Notrans.Translation.y = 0.0f;
    Notrans.Translation.z = 0.0f;
    for(Counter = 0; Counter < GO->NumberOfPoints; Counter++)
    {
        if(IS_Visible(GO->OBJ_PointList + Counter))
        {
            Mul_Matrix_Ras3D(&MR, (GO->OBJ_PointList + Counter), &P3D);
            P3D.Z = -P3D.Z;
//            CI.ProjectVertexCLBK(&P3D, &P2D, &CI);
            if(PaintLight)
                DotProd = -1.0f;
            else
            {
                N3D.Point = (GO->OBJ_PointList + Counter)->Normale;
                Mul_Matrix_Ras3D(&Notrans, (&N3D), &P3DNormale);
                DotProd = P3D.X * P3D.X + P3D.Y * P3D.Y + P3D.Z * P3D.Z;
                DotProd = (float) sqrt(DotProd);
                DotProd = -1.0f / DotProd;
                DotProd = (P3D.X * P3DNormale.X + P3D.Y * P3DNormale.Y - P3D.Z * P3DNormale.Z) * DotProd;
            }
			
            if(DotProd < 0.f)
            {
                Strenght = DotProd * -255.0f;
				
                Dist = P3D.X * P3D.X + P3D.Y * P3D.Y + P3D.Z * P3D.Z;
                if(Dist < SQR_Far)
                {
                    if(Dist > SQR_Near)
                    {
                        Dist = (SQR_Far - Dist) * Oover_SQR_Far_Minus_SQR_Near;
                    }
                    else
                        Dist = 1.0f;
					
                    Strenght *= Dist;
                    Strenght *= OoverLightDiscret;
/*					if (!FIRSTGLOW)
					{
						if((*(unsigned long *)(&(GO->OBJ_PointList + Counter) -> Normale.x) & 2) == 0)
							Strenght *= 2;
					}*/
					
                    /* Strenght *= 255.0f; */
					MAD_RAD_AddColor(GO->Colors , Counter,&CurrentFLOATColor,Strenght);
                }
            }
        }
    }
	
    Transp_Matrix(&MR);
}

/*
===================================================================================================
ZBuufer is ready , just compute the light for each vertex and cull with ZBuff
===================================================================================================
*/
void DrawObjectNodeCallbackIlluminate_DIRECT
(
 MAD_World       *MW,
 MAD_WorldNode   *MWN,
 unsigned long   NodeNumber
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       Counter;
    float               Strenght, DotProd;
    MAD_Matrix          MR;
    MAD_Matrix          Notrans;
    MAD_R_Vertex        P3DNormale/*, P2D*/;
    MAD_Simple3DVertex  N3D;
    MAD_GeometricObject *GO;
    float               OoverLightDiscret;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!(MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)) return;
	
    OoverLightDiscret = 1.0f / (float) LIGHT_DISCRET;
    OoverLightDiscret *= OoverLightDiscret;
	
    /* MAD_UpdateInfoStr(MWN-> ID . Name); MAD_ComputeNormales(MW , MWN,NodeNumber); */
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
    Matrix_Matrix(&MWN->Matrix, &CurrentMatrixNotGlowed, &MR);
    Transp_Matrix(&MR);
	
    Notrans = MR;
    NormMatrix(&Notrans);
    Notrans.Translation.x = 0.0f;
    Notrans.Translation.y = 0.0f;
    Notrans.Translation.z = 0.0f;
    for(Counter = 0; Counter < GO->NumberOfPoints; Counter++)
    {
        if(IS_Visible(GO->OBJ_PointList + Counter))
        {
            if(PaintLight)
                DotProd = -1.0f;
            else
            {
                N3D.Point = (GO->OBJ_PointList + Counter)->Normale;
                Mul_Matrix_Ras3D(&Notrans, (&N3D), &P3DNormale);
                DotProd = P3DNormale.Z ;
            }
			if (DotProd < 0.0f)
			{
				Strenght = DotProd * -255.0f;
				Strenght *= OoverLightDiscret;
				MAD_RAD_AddColor(GO->Colors , Counter,&CurrentFLOATColor,Strenght);
			}
        }
    }
	
    Transp_Matrix(&MR);
}

/*
===================================================================================================
===================================================================================================
*/
void Numerotize(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject *GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];

	GO -> ObjectFlags &= ~GO_NonMagnifold;
	if (!MAD_IsMagnifold( GO ))
	{
		char Str[1024];
		sprintf(Str , "The object %s is non-Magnifold. \n That mean that some edges touch more than two triangles. \n For that reason, it will not be subdivised and optimised... \n Use STL-Check MAX Modifier (Multiples Edges, Spikes & \n Doubles Faces) to find the problem. " , MWN -> ID . Name);
        MessageBox ( Mainhwnd, Str, TEXT("Topologic Error"), MB_OK | MB_ICONINFORMATION   | MB_TASKMODAL | MB_SETFOREGROUND );
		GO -> ObjectFlags &= ~GO_ShadowCut;
		GO -> ObjectFlags |= GO_NonMagnifold;
	}


    MAD_Numerotize(MW, GO);
}

/*
===================================================================================================
===================================================================================================
*/
void Illum_Norme_GO(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject *GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
	MAD_TurnEdges(MW, GO);
}

/*
===================================================================================================
===================================================================================================
*/
void Store_Tags_CLBK(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       Counter;
    MAD_GeometricObject *GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
    for(Counter = 0; Counter < GO->NumberOfPoints; Counter++)
    {
        *(unsigned long *) &((GO->OBJ_PointList + Counter)->Normale.x) |= (*(unsigned long *) &((GO->OBJ_PointList + Counter)->Normale.x) & 0x00000003) << 1;
    }
}

/*
===================================================================================================
===================================================================================================
*/
void Reset_Tags_CLBK(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       Counter;
    MAD_GeometricObject *GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
	if (FIRSTGLOW)
	{
		for(Counter = 0; Counter < GO->NumberOfPoints; Counter++)
		{
			*(unsigned long *) &((GO->OBJ_PointList + Counter)->Normale.x) &= 0xFFFFFFF8;
			*(unsigned long *) &((GO->OBJ_PointList + Counter)->Normale.y) &= 0xFFFFFFFC;
			*(unsigned long *) &((GO->OBJ_PointList + Counter)->Normale.z) &= 0xFFFFFFFC;
		}
	} else
	{
		for(Counter = 0; Counter < GO->NumberOfPoints; Counter++)
		{
			*(unsigned long *) &((GO->OBJ_PointList + Counter)->Normale.x) &= 0xFFFFFFFE;
		}
	} //*/
}

/*
===================================================================================================
===================================================================================================
*/
void DrawObjectNodeCallbackIlluminateReset
(
 MAD_World       *MW,
 MAD_WorldNode   *MWN,
 unsigned long   NodeNumber
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       Counter;
    MAD_GeometricObject *GO;
	float				Ra,Ga,Ba;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
	if (MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)	
	{
		if (UseAmbiantLight)
		{
			MAD_ColorUlong2Float(MW->AmbientColor , &Ra , &Ga , &Ba);
			Ra *= 255.0f;
			Ga *= 255.0f;
			Ba *= 255.0f;
			for(Counter = 0; Counter < GO->NumberOfPoints; Counter++)
			{
				MAD_SetRed(GO->Colors[Counter],(long)Ra);
				MAD_SetGreen(GO->Colors[Counter],(long)Ga);
				MAD_SetBlue(GO->Colors[Counter],(long)Ba);
			}
		}
		else
		for(Counter = 0; Counter < GO->NumberOfPoints; Counter++)
		{
			GO->Colors[Counter] = 0;
		}
	}
    TotalNumBerOfPoints += GO->NumberOfPoints;
    TotalNumBerOfPolygons += MAD_GetNumberOfFace(GO);
}

/*
===================================================================================================
===================================================================================================
*/
void ColorizeObjectNodeCallback(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject *GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!(MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)) return;

    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
    if(GO->Colors == NULL)
    {
        memset(GO->Colors, 0, GO->NumberOfPoints * sizeof(MAD_ColorARGB));
    }
}

/*
===================================================================================================
===================================================================================================
*/
void ResetColors(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject *GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!(MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)) return;

    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
    GO->Colors = NULL;
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_COLORS_2_P4D(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject *GO;
	MAD_Color4D			*P_P4D;
	MAD_ColorARGB		*Originals;
	unsigned long Counter;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!(MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)) return;


    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
	if (!GO->NumberOfPoints) return;
	MAD_REALLOC(MAD_ColorARGB , GO->Colors , 5L * GO->NumberOfPoints);
	P_P4D = (MAD_Color4D *)(GO->Colors + GO->NumberOfPoints);
	Originals = (unsigned long *)GO->Colors;
	Counter = GO->NumberOfPoints;
	while (Counter--)
	{
		MAD_COLOR_2_P4D(P_P4D, *Originals);
		P_P4D++;
		Originals++;
	}
	GO->Colors += GO->NumberOfPoints;
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_P4D_2_COLORS(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject *GO;
	MAD_Color4D			*P_P4D;
	MAD_ColorARGB		*Originals;
	unsigned long Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!(MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)) return;


    GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
	if (!GO->NumberOfPoints) return;
	GO->Colors -= GO->NumberOfPoints;
	P_P4D = (MAD_Color4D *)(GO->Colors + GO->NumberOfPoints);
	Originals = (unsigned long *)GO->Colors;
	Counter = GO->NumberOfPoints;
	while (Counter--)
	{
		if (P_P4D->C[0] > 255.0f) P_P4D->C[0] = 255.0f;
		if (P_P4D->C[1] > 255.0f) P_P4D->C[1] = 255.0f;
		if (P_P4D->C[2] > 255.0f) P_P4D->C[2] = 255.0f;
		if (P_P4D->C[3] > 255.0f) P_P4D->C[3] = 255.0f;
		MAD_SetRed(*Originals,(unsigned long)P_P4D->C[0]);
		MAD_SetGreen(*Originals,(unsigned long)P_P4D->C[1]);
		MAD_SetBlue(*Originals,(unsigned long)P_P4D->C[2]);
		MAD_SetAlpha(*Originals,(unsigned long)P_P4D->C[3]);
		P_P4D++;
		Originals++;
	}
}

/*
===================================================================================================
===================================================================================================
*/
void Subdiv_All_GO(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
	unsigned long NumberOfPoints;
//    MAD_UpdateInfoStr(MWN->ID.Name);
	if (!(MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)) return;
	if(((MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity])->ObjectFlags & GO_ShadowCut)
	{
		if (MWN->ObjectWithRadiosity != MAD_NULL_INDEX)
		{
			NumberOfPoints = ((MAD_GeometricObject *)MW->AllObjects[MWN->Object])->NumberOfPoints;
			if (NumberOfPoints == ((MAD_GeometricObject *)MW->AllObjects[MWN->ObjectWithRadiosity])->NumberOfPoints)
			{
				MAD_MALLOC(MAD_ColorARGB, ((MAD_GeometricObject *)MW->AllObjects[MWN->ObjectWithRadiosity])->SecondRLIField , NumberOfPoints);
				memcpy(((MAD_GeometricObject *)MW->AllObjects[MWN->ObjectWithRadiosity])->SecondRLIField , ((MAD_GeometricObject *)MW->AllObjects[MWN->ObjectWithRadiosity])->Colors, sizeof (MAD_ColorARGB) * NumberOfPoints);
			}
			
			if (LOD_C_IsObjectLOD((MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity]))
				LOD_C_FreezeLOD((MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity]);
			MAD_SubdiviseGO(MW, MWN , (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity]);
		}
	}
}

/*
===================================================================================================
===================================================================================================
*/
void Simplify_GO(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
	unsigned long NumberOfPoints;
    MAD_GeometricObject *GO;
    MAD_UpdateInfoStr(MWN->ID.Name);
	
	if (!(MW->Hierarchie[NodeNumber].NODE_Flags & MNF_MustBeMadRaded)) return;
	GO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
	if (!LOD_C_IsObjectLOD((MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity]))
	{
		NumberOfPoints = ((MAD_GeometricObject *)MW->AllObjects[MWN->Object])->NumberOfPoints;
		MAD_MALLOC(MAD_ColorARGB, MWN->RLIOfObject , NumberOfPoints);
		memcpy(MWN->RLIOfObject , ((MAD_GeometricObject *)MW->AllObjects[MWN->ObjectWithRadiosity])->Colors, sizeof (MAD_ColorARGB) * NumberOfPoints);
	}
		
	if(GO->NumberOfPointsWithoutRadiosity != GO->NumberOfPoints)
	{
		LOD_C_ComputeObjectLOD(GO, GO->NumberOfPointsWithoutRadiosity , SeprogressPos);
	}
    SeprogressPos(0.0f);
}

/*
===================================================================================================
===================================================================================================
*/
void CmpNumBerOfObject(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    NumBerOfObject++;
}

#define Circle_Discret  100

/*
===================================================================================================
===================================================================================================
*/
void Draw_Circles(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   CircleCounter;
    long            C2;
    float           Kx, Ky, Kx2, Ky2;
    MAD_R_Vertex    P1, P2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Kx2 = (float) sin(3.1415927f * 2.0f) * (SourceSX - 4.0f) * 0.5f + SourceSX * 0.5f;
    Ky2 = (float) cos(3.1415927f * 2.0f) * (SourceSY - 4.0f) * 0.5f + SourceSY * 0.5f;
    for(CircleCounter = 0; CircleCounter < Circle_Discret; CircleCounter++)
    {
        Kx = (float) sin(3.1415927f * 2.0f * (float) CircleCounter / (float) Circle_Discret) *
            (SourceSX - 4.0f) * 0.5f + SourceSX * 0.5f;
        Ky = (float) cos(3.1415927f * 2.0f * (float) CircleCounter / (float) Circle_Discret) *
            (SourceSY - 4.0f) * 0.5f + SourceSY * 0.5f;
        P1.X = Kx;
        P1.Y = Ky;
        P2.X = Kx2;
        P2.Y = Ky2;
        MAD_DrawLine(&P1, &P2);
		
        Kx2 = Kx;
        Ky2 = Ky;
    }
	
    Kx2 = (float) sin(3.1415927f * 2.0f) * (SourceSX - 4.0f) * LittleCirle * 0.5f + SourceSX * 0.5f;
    Ky2 = (float) cos(3.1415927f * 2.0f) * (SourceSY - 4.0f) * LittleCirle * 0.5f + SourceSY * 0.5f;
    for(CircleCounter = 0; CircleCounter < Circle_Discret; CircleCounter++)
    {
        Kx = (float) sin(3.1415927f * 2.0f * (float) CircleCounter / (float) Circle_Discret) *
            (SourceSX - 4.0f) * LittleCirle * 0.5f + SourceSX * 0.5f;
        Ky = (float) cos(3.1415927f * 2.0f * (float) CircleCounter / (float) Circle_Discret) *
            (SourceSY - 4.0f) * LittleCirle * 0.5f + SourceSY * 0.5f;
        P1.X = Kx;
        P1.Y = Ky;
        P2.X = Kx2;
        P2.Y = Ky2;
        MAD_DrawLine(&P1, &P2);
		
        Kx2 = Kx;
        Ky2 = Ky;
    }
	
    for(CircleCounter = 0; CircleCounter < (unsigned long) SourceSY; CircleCounter++)
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        unsigned long   *PtrCol;
        float           *PtrCol2;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		
        PtrCol = &TABLEAU[CircleCounter * SourceSX];
        PtrCol2 = &ZBUFFER[CircleCounter * SourceSX];
        for(C2 = 0; C2 < SourceSX; C2++)
        {
            if(*PtrCol == 0xffffffff)
            {
                C2 = SourceSX;
                continue;
            }
			
            *PtrCol = 0xffffffff;
            *PtrCol2 = 10.0e37f;
            PtrCol++;
            PtrCol2++;
        }
		
        PtrCol = &TABLEAU[CircleCounter * SourceSX] + SourceSX - 1;
        PtrCol2 = &ZBUFFER[CircleCounter * SourceSX] + SourceSX - 1;
        C2 = SourceSX;
        while(C2--)
        {
            if(*PtrCol == 0xffffffff)
            {
                C2 = 0;
                continue;
            }
			
            *PtrCol = 0xffffffff;
            *PtrCol2 = 10.0e37f;
            PtrCol--;
            PtrCol2--;
        }
    }
}

#define SUBDIV  9

/*
===================================================================================================
===================================================================================================
*/
void Draw_Cross(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   XCounter, YCounter;
    unsigned long   XPOS, YPOS;
    float           OoSudv;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    OoSudv = 1.0f / ((float) SUBDIV * 2.0f);
    for(XCounter = 0; XCounter < SUBDIV; XCounter++)
    {
        for(YCounter = 0; YCounter < SUBDIV; YCounter++)
        {
            XPOS = lFloatToLongOpt(SourceSX * ((float) XCounter * 2.0f + 1.0f) * OoSudv);
            YPOS = lFloatToLongOpt(SourceSY * ((float) YCounter * 2.0f + 1.0f) * OoSudv);
            TABLEAU[(XPOS << SourceSXShift) + YPOS] = 0xFFFFFFFF;
        }
    }
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_InitZGlobe(float ZMax, float ZMin)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float/* *Src, */    *Dst, Coef;
    unsigned long       Number;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* Src = GlobBuffer; */
    Dst = ZBUFFER;
    Coef = ZMax ;
    Number = SourceSX * SourceSY;
    while(Number--) *(Dst++) = Coef;
}

/*
===================================================================================================
===================================================================================================
*/
void Render_MW(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Matrix      M;
    unsigned long   SaveCurrentQuality;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TABLEAU = SaveTableau;
    if(!QUAKE_ON)
    {
        SaveCurrentQuality = CurrentQuality;
        CompputingQuality(RENDER_QUALITY);
        AdaptToWindow(Mainhwnd, GetDlgItem(Mainhwnd, IDC_CUSTOMRENDER), SourceSX, SourceSY);
    }
	
    CI.XMin = 0.0f;
    CI.XMax = (float) SourceSX;
    CI.YMin = 0.0f;
    CI.YMax = (float) SourceSY;
    CI.ZMin = MAD_RAD_CLIPPING_NEAR;
    CI.Force2sidedFLAG = Force2sided;
    CI.InvertBCKFCFlag = 0;
    if(Displaymode == 1)
        CI.NmbrOfSclrToInt = 3;
    else
	{
		if(Displaymode == 2)
			CI.NmbrOfSclrToInt = 3;
		else
		if(Displaymode == 3)
			CI.NmbrOfSclrToInt = 3;
		else
			CI.NmbrOfSclrToInt = 0;
	}
	if (AllRadCam[CurrentRadCam].IsOrtho)
	{
		AllRadCam[CurrentRadCam].Focale = 1.0f / ChannelSize;
	}
    CI.ProjectCam.CX = AllRadCam[CurrentRadCam].Focale * (SourceSX);
    CI.ProjectCam.CY = -AllRadCam[CurrentRadCam].Focale * (SourceSY);
	CI.ProjectCam.IsOrtho = AllRadCam[CurrentRadCam].IsOrtho;
    CI.ProjectCam.PX = SourceSX * 0.5f;
    CI.ProjectCam.PY = SourceSY * 0.5f;
    CI.MAD_DrawLineCLBK = NULL;
    CI.MAD_PlotCLBK = NULL;
	if (AllRadCam[CurrentRadCam].IsOrtho)
	{
	    CI.ZMin = -MR_Infinite;
		
		CI.ProjectVertexCLBK = ProjectVertexISO;
	}
	else
		CI.ProjectVertexCLBK = ProjectVertex;
    if(Displaymode == 1)
    {
        CI.RasterizeCallback = RasterizeCallbackD1;
    }
    else
    {
        if(Displaymode == 2)
            CI.RasterizeCallback = RasterizeCallbackD1;
        else
        if(Displaymode == 3)
            CI.RasterizeCallback = RasterizeCallbackD1;
        else
            CI.RasterizeCallback = RasterizeCallbackD34;
        if(Displaymode == 0)
            CI.RasterizeCallback = RasterizeCallbackD1_FILLZBUFONLY;
    }
	{
		unsigned long ResetCounter;
		ResetCounter = SourceSX * SourceSY;
		while (ResetCounter--)
		{
			TABLEAU[ResetCounter] = MAD_BackGroudColor;
		}
	}
	
	if (AllRadCam[CurrentRadCam].IsOrtho)
	{
		unsigned long Cout , V , *ZBUF;
		float Trans;
		Trans = -MR_Infinite;
		V = *(unsigned long *)&Trans;
		ZBUF = (unsigned long *)ZBUFFER;
		Cout = SourceSX * SourceSY;
		while (Cout--)
		{
			*(ZBUF++) = V;
		}
	} else
	    memset(ZBUFFER, 0x00, SourceSX * SourceSY * 4);
    memset(PICKINGBUFF, 0xff, SourceSX * SourceSY * 4);
    M = CURRENT_VIEW_MATRIX;
    M.Translation.z = -M.Translation.z;
    M.Translation.x = -M.Translation.x;
    Inv_Matrix(&M, &CurrentMatrix);
    CI.ZMax = MR_Infinite;
	
    /* MAD_InitZGlobe(CI.ZMax , CI.ZMin); */
    if(ShowLines || (Displaymode == 0))
    {
        CI.XMin += 1.f;
        CI.XMax -= 1.f;
        CI.YMin += 1.f;
        CI.YMax -= 1.f;
		if (AllRadCam[CurrentRadCam].IsOrtho)
        CI.MAD_DrawLineCLBK = MAD_DrawLineZBufferISO;
		else
        CI.MAD_DrawLineCLBK = MAD_DrawLineZBuffer;
    }
	
    CurrentPickingColor = 0;
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, DrawObjectNodeCallback_RENDER);
    CurrentPickingColor = 0;
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, DrawObjectNodeCallback_DrawBox);
    MAD_UpdateInfoStr("");
    Blitte_32(TABLEAU);
    if(!QUAKE_ON)
    {
        CompputingQuality(SaveCurrentQuality);
        AdaptToWindow(Mainhwnd, GetDlgItem(Mainhwnd, IDC_CUSTOMRENDER), SourceSX, SourceSY);
    }
}

/*
===================================================================================================
===================================================================================================
*/
float GetRadiuOfLight(MAD_Light *ML)
{
    switch(ML->LightType)
    {
    case MAD_LightOmni:
        return(ML->Omni.RadiusOfLight);
        break;
    case MAD_LightSpot:
        return(ML->Spot.RadiusOfLight);
        break;
    case MAD_LightDirect:
		return 0.0f;
        break;
    }
	
    return(0.0f);
}

/*
===================================================================================================
===================================================================================================
*/
void GetColorOfLight(MAD_Light *ML, MAD_Vertex *MV)
{
    MV->z = (ML->LightColor & 0xff) * 1.0f / 256.0f;
    MV->y = ((ML->LightColor & 0xff00) >> 8) * 1.0f / 256.0f;
    MV->x = ((ML->LightColor & 0xff0000) >> 16) * 1.0f / 256.0f;
	
    MV->z *= ML->Multiplier; 
	MV->y *= ML->Multiplier; 
	MV->x *= ML->Multiplier;
}

#define NOGLOW_CROSS_DEF    EXTRACTION * 0.5f

/*
===================================================================================================
===================================================================================================
*/
void Init_NoGlow_Cross(unsigned long GlowCounter, float *Kx, float *Ky)
{
    switch(GlowCounter)
    {
    case 0:
        *Kx = 0.0f;
        *Ky = 0.0f;
        break;
    case 1:
        *Kx = 0.0f;
        *Ky = -NOGLOW_CROSS_DEF;
        break;
    case 2:
        *Kx = NOGLOW_CROSS_DEF;
        *Ky = 0.0f;
        break;
    case 3:
        *Kx = 0.0f;
        *Ky = NOGLOW_CROSS_DEF;
        break;
    case 4:
        *Kx = -NOGLOW_CROSS_DEF;
        *Ky = 0.0f;
        break;
    }
}

static float    GlobalRandom = 0.0f;
/*
===================================================================================================
===================================================================================================
*/
void MoveMatrixForGlow
(
 unsigned long   GlowCounter,
 unsigned long   GlowCounter2,
 unsigned long   LightCounter,
 MAD_Matrix      *MR
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float           Lenght, Kx, Ky, Kz, Spirale;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    GlobalRandom += 3.1415927f * 6.9123f / (float) (LIGHT_DISCRET);
    Spirale = ((float) (GlowCounter * 2.0f + 1.0f) / (float) (LIGHT_DISCRET * 2.0f)) - 0.5f;
    Kx = GetRadiuOfLight((MAD_Light *) AllRadCam[LightCounter].Object) * Spirale;
    Spirale = ((float) (GlowCounter2 * 2.0f + 1.0f) / (float) (LIGHT_DISCRET * 2.0f)) - 0.5f;
    Ky = GetRadiuOfLight((MAD_Light *) AllRadCam[LightCounter].Object) * Spirale;
    Spirale = (float) GlowCounter / (float) (LIGHT_DISCRET);
    Kz = GetRadiuOfLight((MAD_Light *) AllRadCam[LightCounter].Object) *
        (float) sin(GlobalRandom * 0.7956f) *
        0.5f *
        Spirale;
    Lenght = (MR->I.x * MR->I.x + MR->I.y * MR->I.y + MR->I.z * MR->I.z);
    Lenght = Kx / (float) sqrt(Lenght);
    MR->Translation.x += MR->I.x * Lenght;
    MR->Translation.y += MR->I.y * Lenght;
    MR->Translation.z += MR->I.z * Lenght;
    Lenght = (MR->J.x * MR->J.x + MR->J.y * MR->J.y + MR->J.z * MR->J.z);
    Lenght = Ky / (float) sqrt(Lenght);
    MR->Translation.x += MR->J.x * Lenght;
    MR->Translation.y += MR->J.y * Lenght;
    MR->Translation.z += MR->J.z * Lenght;
	if (CurrentLightType == MAD_LightOmni)
	{
		Lenght = (MR->K.x * MR->K.x + MR->K.y * MR->K.y + MR->K.z * MR->K.z);
		Lenght = Kz / (float) sqrt(Lenght);
		MR->Translation.x += MR->K.x * Lenght;
		MR->Translation.y += MR->K.y * Lenght;
		MR->Translation.z += MR->K.z * Lenght;
	}
}

static float GlobaXMA = 500.0f;
static float GlobaYMA = 500.0f;
static float GlobaXMI = -500.0f;
static float GlobaYMI = -500.0f;
static unsigned long GLOBALXCount , GLOBALYCount;
void  MAD_IniTDirectMatrixDir(MAD_Matrix *MR, unsigned long QuarterCounter)
{
	unsigned long KX,KY;
	MAD_Matrix MM;
	/* Compute World Size XMA - XMI */
	KX = QuarterCounter % GLOBALXCount;
	KY = QuarterCounter / GLOBALXCount;
	MM = *MR;
    Transp_Matrix(&MM);
	MM.Translation . x = -(GlobaXMI * MM.I . x  + ((float)KX) * MM.I . x * ChannelSize);
	MM.Translation . y =  (GlobaXMI * MM.J . x  + ((float)KX) * MM.J . x * ChannelSize);
	MM.Translation . z = -(GlobaXMI * MM.K . x  + ((float)KX) * MM.K . x * ChannelSize);
	MM.Translation . x += GlobaYMI * MM.I . y + ((float)KY) * MM.I . y * ChannelSize;
	MM.Translation . y -= GlobaYMI * MM.J . y + ((float)KY) * MM.J . y * ChannelSize;
	MM.Translation . z += GlobaYMI * MM.K . y + ((float)KY) * MM.K . y * ChannelSize;
    Transp_Matrix(&MM);
	*MR = MM;
}
static MAD_Matrix MAMIMATRIX;
//static int FirsttimeZob = 0;
void ComputeMAMICallback(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 Counter;
	MAD_Matrix			MR;
    MAD_Simple3DVertex  Point;
    MAD_R_Vertex        P;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//	if (FirsttimeZob) return;
//	FirsttimeZob = 1;
	if (MR_Infinite == MWN -> p_stBDV->XMin) 
		return;
	
	MAMIMATRIX.Translation . x = 0.0f;
	MAMIMATRIX.Translation . y = 0.0f;
	MAMIMATRIX.Translation . z = 0.0f;
    Matrix_Matrix(&MWN->Matrix, &MAMIMATRIX, &MR);
    Transp_Matrix(&MR);
    for(Counter = 0; Counter < 8; Counter++)
    {
        if(Counter & 1)
            Point.Point.x = MWN -> p_stBDV->XMin;
        else
            Point.Point.x = MWN -> p_stBDV->XMax;
		
        if(Counter & 2)
            Point.Point.y = MWN -> p_stBDV->YMin;
        else
            Point.Point.y = MWN -> p_stBDV->YMax;
		
        if(Counter & 4)
            Point.Point.z = MWN -> p_stBDV->ZMin;
        else
            Point.Point.z = MWN -> p_stBDV->ZMax;
	
        Mul_Matrix_Ras3D(&MR, &Point, &P);
		P.Y = -P.Y;
		if (P.X > GlobaXMA) GlobaXMA = P.X;
		if (P.Y > GlobaYMA) GlobaYMA = P.Y;
		if (P.X < GlobaXMI) GlobaXMI = P.X;
		if (P.Y < GlobaYMI) GlobaYMI = P.Y;
    }
}
unsigned long GetNumBerOfViewsFromCurrentDirectLight(RADCAM *CRC)
{
	MAD_Matrix Mtrx;
	NormMatrix(&CRC->Matrix);
	MAMIMATRIX = CRC->Matrix;
	MAMIMATRIX . Translation.x = 0.0f;
	MAMIMATRIX . Translation.y = 0.0f;
	MAMIMATRIX . Translation.z = 0.0f;
	Inv_Matrix(&MAMIMATRIX, &Mtrx);
	
	MAMIMATRIX = Mtrx;
	GlobaXMA = GlobaYMA = -MR_Infinite;
	GlobaXMI = GlobaYMI = MR_Infinite;
//	FirsttimeZob = 0;
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, ComputeMAMICallback);
	/* Compute World Size XMA - XMI */
	GLOBALXCount = (unsigned long)(((GlobaXMA - GlobaXMI) / ChannelSize) + 2.0f);
	GLOBALYCount = (unsigned long)(((GlobaYMA - GlobaYMI) / ChannelSize) + 2.0f);
	return GLOBALXCount * GLOBALYCount;
}
/*
===================================================================================================
===================================================================================================
*/
void Compute_MW(unsigned long Illum, unsigned long Subdiv, unsigned long CM)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Matrix      MR, CurrentLightMatrix;
    unsigned long   LightCounter, MemSet1, *MemSet2;
    unsigned long   GlowCounter, GlowCounter2;
    unsigned long   QuarterCounter, QuarterNumber, LIGHT_DISCRET_LOCAL;
    static long     FirstTime = 1;
    unsigned long   CurrentCompressedMap;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GlobalRandom = 0.0f;
    if(CM) ComputeMaps = 1;
    if((Illum == 0) && (Subdiv == 0)) 
		return;
	TABLEAU = SaveTableau;
    CurrentCompressedMap = 0;
/*	if (Illum)
	ResetAllCompressedMap();*/
    CI.XMin = 0.0f;
    CI.XMax = (float) SourceSX;
    CI.YMin = 0.0f;
    CI.YMax = (float) SourceSY;
    CI.ZMin = MAD_RAD_CLIPPING_NEAR;
    CI.NmbrOfSclrToInt = 0;
    CI.ProjectCam.PX = SourceSX * 0.5f;
    CI.ProjectCam.PY = SourceSY * 0.5f;
    CI.MAD_DrawLineCLBK = NULL;
    CI.MAD_PlotCLBK = NULL;

    CI.ProjectVertexCLBK = ProjectVertex;
    CI.Force2sidedFLAG = Force2sided;
    CI.InvertBCKFCFlag = 0;
	
    LIGHT_DISCRET_LOCAL = LIGHT_DISCRET;
    if(!GLOW_ON)
    {
        LIGHT_DISCRET = 1;
    }
	if (Illum)
	{
		MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, MAD_COLORS_2_P4D);
	}
	
    for(LightCounter = 0; LightCounter < NumberLightCam; LightCounter++)
    {
		CI.ProjectCam.CX = AllRadCam[LightCounter].Focale * (SourceSX);
		CI.ProjectCam.CY = -AllRadCam[LightCounter].Focale * (SourceSY);
		/* set globals parrameters */
        if(!((MAD_Light *) AllRadCam[LightCounter].Object)->IsOn) continue;

        CastShadow = PaintLight = OverWalls = 0;
        if(((MAD_Light *) AllRadCam[LightCounter].Object)->LightFlags & MAD_LightFlag_CastShadow) CastShadow = 1;
        if(((MAD_Light *) AllRadCam[LightCounter].Object)->LightFlags & MAD_LightFlag_PaintLight) PaintLight = 1;
        if(((MAD_Light *) AllRadCam[LightCounter].Object)->LightFlags & MAD_LightFlag_OverWalls)  OverWalls = 1;

        CI.ZMax = MR_Infinite;
		CI.ZMin = MAD_RAD_CLIPPING_NEAR;

		CurrentLightType = ((MAD_Light *) AllRadCam[LightCounter].Object)->LightType;

		switch (CurrentLightType)
		{
		case MAD_LightOmni:
            CurrentLightNear = ((MAD_Light *) AllRadCam[LightCounter].Object)->Omni.Near;
            CI.ZMax = ((MAD_Light *) AllRadCam[LightCounter].Object)->Omni.Far;
	        CurrentLightFar = CI.ZMax;
			CI.ProjectVertexCLBK = ProjectVertex;
            QuarterNumber = 6;
			break;
        case MAD_LightDirect:
			CurrentLightNear = CI.ZMin = -MR_Infinite;
			CI.ZMax = MR_Infinite;
	        CurrentLightFar = CI.ZMax;
            CI.ProjectVertexCLBK = ProjectVertexISO;
			AllRadCam[LightCounter].Focale = 1.0f / (ChannelSize * 1.10f);
			CI.ProjectCam.CX = AllRadCam[LightCounter].Focale * (SourceSX);
			CI.ProjectCam.CY = -AllRadCam[LightCounter].Focale * (SourceSY);
			QuarterNumber = GetNumBerOfViewsFromCurrentDirectLight(&AllRadCam[LightCounter]);
			break;
        case MAD_LightSpot:
            LittleCirle = (float) tan(0.5f * ((MAD_Light *) AllRadCam[LightCounter].Object)->Spot.LittleAlpha) / (float) tan(0.5f * ((MAD_Light *) AllRadCam[LightCounter].Object)->Spot.BigAlpha);
            CurrentLightNear = ((MAD_Light *) AllRadCam[LightCounter].Object)->Spot.Near;
            CI.ZMax = ((MAD_Light *) AllRadCam[LightCounter].Object)->Spot.Far;
	        CurrentLightFar = CI.ZMax;
			CI.ProjectVertexCLBK = ProjectVertex;
			QuarterNumber = 1;
			break;
        }

        MAD_UpdateInfoStr((*((AllRadCam[LightCounter]).Object)).Name);

        GetColorOfLight((MAD_Light *) AllRadCam[LightCounter].Object, &CurrentFLOATColor);

        if((GetRadiuOfLight((MAD_Light *) AllRadCam[LightCounter].Object) == 0.0f))
        {
            CurrentFLOATColor.x *= LIGHT_DISCRET * LIGHT_DISCRET;
            CurrentFLOATColor.y *= LIGHT_DISCRET * LIGHT_DISCRET;
            CurrentFLOATColor.z *= LIGHT_DISCRET * LIGHT_DISCRET;
        }

		FIRSTGLOW = 1;

        for(GlowCounter = 0; GlowCounter < LIGHT_DISCRET; GlowCounter++)
        {
            for(GlowCounter2 = 0; GlowCounter2 < LIGHT_DISCRET; GlowCounter2++)
	            {
				if(!Illum)
				{
					if	(SeprogressPos(((float) LightCounter * (float) LIGHT_DISCRET  * (float) LIGHT_DISCRET +(float) GlowCounter  * (float) LIGHT_DISCRET  + (float) GlowCounter2 ) / ((float) (LIGHT_DISCRET) * (float) (LIGHT_DISCRET) * (float) (NumberLightCam)))) return;
				}
                if  ((GetRadiuOfLight((MAD_Light *) AllRadCam[LightCounter].Object) == 0.0f) && (GlowCounter | GlowCounter2)) continue;
				
                CurrentLightMatrix = AllRadCam[LightCounter].Matrix;
                MR = CurrentLightMatrix;
				
                /* The glow is a simple move of the light */
                MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, Reset_Tags_CLBK);

                if ((GLOW_ON) && (!FIRSTGLOW)) MoveMatrixForGlow(GlowCounter, GlowCounter2, LightCounter, &MR);

                MR.Translation.z = -MR.Translation.z;
                MR.Translation.x = -MR.Translation.x;
                /* For each view subdivision, precompute global visibility */
                if(QuarterNumber != 1)
                {
                    for(QuarterCounter = 0; QuarterCounter < QuarterNumber; QuarterCounter++)
                    {
						if	(SeprogressPos
							(((float) LightCounter * (float) LIGHT_DISCRET  * (float) LIGHT_DISCRET +(float) GlowCounter  * (float) LIGHT_DISCRET  + (float) GlowCounter2 ) / 
							((float) (LIGHT_DISCRET) * (float) (LIGHT_DISCRET) * (float) (NumberLightCam)))) return;

                        if (CurrentLightType == MAD_LightOmni) 
							MAD_IniTMatrixDir(&MR, QuarterCounter);
                        if (CurrentLightType == MAD_LightDirect) 
							MAD_IniTDirectMatrixDir(&MR, QuarterCounter);
						
						if (FIRSTGLOW) Inv_Matrix(&MR, &CurrentMatrixNotGlowed);
                        Inv_Matrix(&MR, &CurrentMatrix);
						
                        if ((AllMap[CurrentCompressedMap] != NULL) && (CurrentCompressedMap < MAX_Compressed_MAPS))
						{
                            MAP_DeCompress(TABLEAU, AllMap[CurrentCompressedMap]);
						}
                        else
                        {
							if (AllRadCam[LightCounter].IsOrtho)
								MAD_InitZGlobe(-CI.ZMax, CI.ZMin);
							else
								MAD_InitZGlobe(RAS_ZBUFFER_MULT / CI.ZMax, CI.ZMin);
                            if ( (ComputeMaps) && (CurrentCompressedMap < MAX_Compressed_MAPS))
                            {
                                MemSet2 = (unsigned long *) TABLEAU;
                                MemSet1 = SourceSX * SourceSY;
                                while(MemSet1--)
                                    *(MemSet2++) = COMPUTEBCKCOLOR;
                            }
							
                            CI.RasterizeCallback = RasterizeMarkCallback;
							CI.MAD_PlotCLBK = NULL;
                            CurrentPickingColor = 0;
                            MAD_NodeEnum(ORIGINAL_MW, ID_MAD_GeometricObject_V0, DrawObjectNodeCallback);

                            if(ComputeMaps) AllMap[CurrentCompressedMap] = MAP_Compress(TABLEAU,1L * SourceSX * SourceSY);
                        }

                        CurrentCompressedMap++;
#ifdef SHOW_CALCUL
						Blitte_32(TABLEAU);
						Sleep(SHOW_CALCUL);
#endif
                        /* Compute vertex visibility here */
                        MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, ComputeVisibility_CLBK);
						if (FIRSTGLOW) MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, Store_Tags_CLBK);
                    }
					
                    /* Restore Value */
                    CurrentCompressedMap -= QuarterNumber;
                }
				
                /* For each view subdivision */
                for(QuarterCounter = 0; QuarterCounter < QuarterNumber; QuarterCounter++)
                {
					if	(SeprogressPos
						(((float) LightCounter * (float) LIGHT_DISCRET  * (float) LIGHT_DISCRET +(float) GlowCounter  * (float) LIGHT_DISCRET  + (float) GlowCounter2 ) / 
						((float) (LIGHT_DISCRET) * (float) (LIGHT_DISCRET) * (float) (NumberLightCam)))) return;

                    if(CurrentLightType == MAD_LightOmni)
                    {
                        MAD_IniTMatrixDir(&MR, QuarterCounter);
                    }
                    if(CurrentLightType == MAD_LightDirect)
                    {
                        MAD_IniTDirectMatrixDir(&MR, QuarterCounter);
                    }
					if (FIRSTGLOW) Inv_Matrix(&MR, &CurrentMatrixNotGlowed);
                    Inv_Matrix(&MR, &CurrentMatrix);

                    /* Decompress pre-computed map or compute it */
                    if ((AllMap[CurrentCompressedMap] != NULL) && (CurrentCompressedMap < MAX_Compressed_MAPS))
                        MAP_DeCompress(TABLEAU, AllMap[CurrentCompressedMap]);
                    else
                    {
						if (AllRadCam[LightCounter].IsOrtho)
							MAD_InitZGlobe(-CI.ZMax, CI.ZMin);
						else
							MAD_InitZGlobe(RAS_ZBUFFER_MULT / CI.ZMax, CI.ZMin);
                        if ((ComputeMaps) && (CurrentCompressedMap < MAX_Compressed_MAPS))
                        {
                            MemSet2 = (unsigned long *) TABLEAU;
                            MemSet1 = SourceSX * SourceSY;
                            while(MemSet1--)
                                *(MemSet2++) = COMPUTEBCKCOLOR;
                        }

                        CI.RasterizeCallback = RasterizeMarkCallback;
						CI.MAD_PlotCLBK = NULL;
                        CurrentPickingColor = 0;
                        MAD_NodeEnum(ORIGINAL_MW, ID_MAD_GeometricObject_V0, DrawObjectNodeCallback);

                        if(ComputeMaps) 
							AllMap[CurrentCompressedMap] = MAP_Compress(TABLEAU,1L * SourceSX * SourceSY);
						CI.MAD_PlotCLBK = NULL;

                    }
					
                    CurrentCompressedMap++;

					
                    /* If spot, compute visibility here (optimization) */
                    if(CurrentLightType == MAD_LightSpot)
                    {
                        if(Subdiv && FIRSTGLOW)
                        {
                            Draw_Circles();
                        }
						
                        MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, ComputeVisibility_CLBK);
						if (FIRSTGLOW)
							MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, Store_Tags_CLBK);
                    }

					if (QuarterNumber == 1)
						if(CurrentLightType == MAD_LightDirect)
						{
							MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, ComputeVisibility_CLBK);
							if (FIRSTGLOW) MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, Store_Tags_CLBK);
						}

                    if ((CurrentLightType == MAD_LightOmni) && Subdiv) 
						Draw_Cross();
					
#ifdef SHOW_CALCUL 
					Blitte_32(TABLEAU);
					Sleep(SHOW_CALCUL);
#endif
                    /* Detect subdivisions */
                    CI.RasterizeCallback = RasterizeDetectCallback;
                    if(Subdiv && CastShadow)
                        MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, DetectObjectNodeCallback);
                }
				
                if(Illum)
                {
                    CurrentPickingColor = 0;
					switch (CurrentLightType)
					{
                    case MAD_LightSpot:		MAD_NodeEnum(MW,ID_MAD_GeometricObject_V0,DrawObjectNodeCallbackIlluminate_SPOT);	break;
                    case MAD_LightOmni:		MAD_NodeEnum(MW,ID_MAD_GeometricObject_V0,DrawObjectNodeCallbackIlluminate_OMNI);	break;
                    case MAD_LightDirect:	MAD_NodeEnum(MW,ID_MAD_GeometricObject_V0,DrawObjectNodeCallbackIlluminate_DIRECT);	break;
					}
					
                }

				FIRSTGLOW = 0;
            }
        }

    }
	
	if (Illum)
	{
		MAD_NodeEnum(MW,ID_MAD_GeometricObject_V0,DrawObjectNodeCallbackIlluminate_USERRLI);
		MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, MAD_P4D_2_COLORS);
	}

    LIGHT_DISCRET = LIGHT_DISCRET_LOCAL;
    MAD_UpdateInfoStr("");
	SeprogressPos(((float) LightCounter * (float) LIGHT_DISCRET  * (float) LIGHT_DISCRET +(float) GlowCounter  * (float) LIGHT_DISCRET  + (float) GlowCounter2 ) / ((float) (LIGHT_DISCRET) * (float) (LIGHT_DISCRET) * (float) (NumberLightCam)));
    if(Subdiv)
    {
		MAD_UpdateInfoStr("Subdivide...");
        MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, Subdiv_All_GO);
        MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, ColorizeObjectNodeCallback);
    }
	
    if(Illum)
    {
		MAD_UpdateInfoStr2("Turn edges...");
        MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, Illum_Norme_GO);
    }
	SeprogressPos(1.0f);
	TABLEAU = SaveTableau;
    ComputeMaps = 0;
}

/*
===================================================================================================
===================================================================================================
*/
static BOOL CALLBACK HelpDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    static unsigned long    ACTIVE;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    switch(msg)
    {
    case WM_INITDIALOG:
        ACTIVE = 1;
        return TRUE;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        DestroyWindow(hWnd);
        break;
    case WM_NCACTIVATE:
        if(ACTIVE)
        {
            if(wParam == TRUE) return TRUE;
            ACTIVE = 0;
            DestroyWindow(hWnd);
        }
		
        break;
    default:
        return FALSE;
    }
	
    return TRUE;
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long CDLG_USR_GetColor(unsigned long Color, HWND MUM)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CHOOSECOLOR     CCLR;
    COLORREF        C[16];
    unsigned long   i;
    unsigned long   AlphaSave;
    BYTE            R, G, B;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    AlphaSave = Color & 0xff000000;
    i = 8;
    while(i--)
    {
        if(i & 1)
            R = 0xff;
        else
            R = 0;
        if(i & 2)
            G = 0xff;
        else
            G = 0;
        if(i & 4)
            B = 0xff;
        else
            B = 0;
        C[i] = RGB(R, G, B);
    }
	
    i = 8;
    while(i--)
        C[i + 8] = RGB(((i << 5) - 1), ((i << 5) - 1), ((i << 5) - 1));
    C[8] = 0;
    ZeroMemory(&CCLR, sizeof(CCLR));
    CCLR.lStructSize = sizeof(CCLR);
    CCLR.Flags = CC_FULLOPEN | CC_RGBINIT;
    CCLR.lpCustColors = C;
    CCLR.rgbResult = Color;
    CCLR.hwndOwner = MUM;
    ChooseColor(&CCLR);
    return((CCLR.rgbResult & 0x00ffffff) | AlphaSave);
}

#define DISPLAY_HELP(HelpButton, HelpWindow) \
{ \
	HWND    HelpHWND; \
	RECT    LOCALRECT; \
	GetWindowRect(GetDlgItem(Mainhwnd, HelpButton), &LOCALRECT); \
	HelpHWND = CreateDialog \
	( \
	(HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), \
	MAKEINTRESOURCE(HelpWindow), \
	hWnd, \
	(int(__stdcall *) (struct HWND__ *,unsigned int,unsigned int,long)) & HelpDlgProc \
	); \
	SetWindowPos \
	( \
	HelpHWND, \
	NULL, \
	LOCALRECT.left, \
	LOCALRECT.top, \
	100, \
	100, \
	SWP_SHOWWINDOW | SWP_NOSIZE \
	); \
	ShowWindow(HelpHWND, SW_SHOW); \
} \
break;
static float    CurLat = 0.0f;
static float    CurLong = 0.0f;

/*
===================================================================================================
===================================================================================================
*/
static unsigned long ulOldMouseButton;
void QUAKE_MOVE_CAM(float DX, float DY , POINT *MCScreen)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static unsigned long IsIN = 0;
	static unsigned long ZISCOMPTE = 0;
    float   Dxf, Dyf  ;
	static float  ZScreen  = 0.0f;
	static float  ZScreen2 = 0.0f;
	static MAD_Vertex stTarg;
	static MAD_Vertex stTargNormed;
	static POINT MCScreenOriginal;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (ulOldMouseButton != MOUSEBUTTONS)
	{
		ZScreen2 = ZScreen = ZBUFFER[(MCScreen->y << SourceSXShift) + MCScreen->x];
		if (ZScreen != 0.0f)
		{
			MAD_Simple3DVertex Targ;
			MCScreenOriginal = *MCScreen;
			if (AllRadCam[CurrentRadCam].IsOrtho)
			{
				ZScreen2 = ZScreen = -ZScreen;
				Targ.Point.x =  ((float)MCScreenOriginal.x - CI . ProjectCam.PX) / CI . ProjectCam.CX;
				Targ.Point.y =  ((float)MCScreenOriginal.y - CI . ProjectCam.PY) / CI . ProjectCam.CY;
				Targ.Point.z =  ZScreen ;
			}
			else
			{
				ZScreen2 = ZScreen = RAS_ZBUFFER_MULT / ZScreen;
				Targ.Point.x =  ZScreen * ((float)MCScreenOriginal.x - CI . ProjectCam.PX) / CI . ProjectCam.CX;
				Targ.Point.y =  ZScreen * ((float)MCScreenOriginal.y - CI . ProjectCam.PY) / CI . ProjectCam.CY;
				Targ.Point.z =  ZScreen ;
			}
			stTarg = CURRENT_VIEW_MATRIX.Translation;
			stTarg.x +=-Targ.Point.z	* (CURRENT_VIEW_MATRIX.K.x);
			stTarg.y +=-Targ.Point.z	* (CURRENT_VIEW_MATRIX.K.y);
			stTarg.z +=-Targ.Point.z	* (CURRENT_VIEW_MATRIX.K.z);
			stTarg.x += Targ.Point.x	* (CURRENT_VIEW_MATRIX.I.x);
			stTarg.y += Targ.Point.x	* (CURRENT_VIEW_MATRIX.I.y);
			stTarg.z += Targ.Point.x	* (CURRENT_VIEW_MATRIX.I.z);
			stTarg.x += Targ.Point.y	* (CURRENT_VIEW_MATRIX.J.x);
			stTarg.y += Targ.Point.y	* (CURRENT_VIEW_MATRIX.J.y);
			stTarg.z += Targ.Point.y	* (CURRENT_VIEW_MATRIX.J.z);
			stTargNormed = stTarg;
			stTargNormed.x -= CURRENT_VIEW_MATRIX.Translation.x;
			stTargNormed.y -= CURRENT_VIEW_MATRIX.Translation.y;
			stTargNormed.z -= CURRENT_VIEW_MATRIX.Translation.z;
			NORMALIZE((&stTargNormed),x,y,z);
		} 			
	} 
	ulOldMouseButton = MOUSEBUTTONS;
	if (!IsIN)
	{
		IsIN = 1;
		Dxf = (float) DX;
		Dyf = (float) DY;
		switch(MOUSEBUTTONS)
		{
		case 0: /* Nothing */
			MAD_RAD_QUKE_NUMMAXPONT += MAD_RAD_QUKE_NUMMAXPONT_DEF;
			break;
		case 1: /* Rotate */
			{
				MAD_Vertex stTargDiff;
				if (ZScreen != 0.0f)
				{
					CurLong -= Dxf * 0.02f;
					CurLat += Dyf * 0.02f;
				}
				else
				{
					CurLong -= Dxf * 0.01f;
					CurLat += Dyf * 0.01f;
				}
				if(CurLat > 3.1415927f / 2.0f) CurLat = 3.1415927f / 2.0f;
				if(CurLat < -3.1415927f / 2.0f) CurLat = -3.1415927f / 2.0f;
				MAtrixFromLongitudeLatitude(&CURRENT_VIEW_MATRIX, CurLat, CurLong );
				if (ZScreen != 0.0f)
				{
					MAD_Simple3DVertex Targ;
					if (AllRadCam[CurrentRadCam].IsOrtho)
					{
						ZScreen2 = ZScreen = -ZScreen;
						Targ.Point.x =  ((float)MCScreenOriginal.x - CI . ProjectCam.PX) / CI . ProjectCam.CX;
						Targ.Point.y =  ((float)MCScreenOriginal.y - CI . ProjectCam.PY) / CI . ProjectCam.CY;
						Targ.Point.z =  ZScreen ;
					}
					else
					{
						Targ.Point.x =  ZScreen * ((float)MCScreenOriginal.x - CI . ProjectCam.PX) / CI . ProjectCam.CX;
						Targ.Point.y =  ZScreen * ((float)MCScreenOriginal.y - CI . ProjectCam.PY) / CI . ProjectCam.CY;
						Targ.Point.z =  ZScreen ;
					}
					stTargDiff.x =-Targ.Point.z	* (CURRENT_VIEW_MATRIX.K.x);
					stTargDiff.y =-Targ.Point.z	* (CURRENT_VIEW_MATRIX.K.y);
					stTargDiff.z =-Targ.Point.z	* (CURRENT_VIEW_MATRIX.K.z);
					stTargDiff.x += Targ.Point.x	* (CURRENT_VIEW_MATRIX.I.x);
					stTargDiff.y += Targ.Point.x	* (CURRENT_VIEW_MATRIX.I.y);
					stTargDiff.z += Targ.Point.x	* (CURRENT_VIEW_MATRIX.I.z);
					stTargDiff.x += Targ.Point.y	* (CURRENT_VIEW_MATRIX.J.x);
					stTargDiff.y += Targ.Point.y	* (CURRENT_VIEW_MATRIX.J.y);
					stTargDiff.z += Targ.Point.y	* (CURRENT_VIEW_MATRIX.J.z);
					CURRENT_VIEW_MATRIX . Translation .x = stTarg.x - stTargDiff.x;
					CURRENT_VIEW_MATRIX . Translation .y = stTarg.y - stTargDiff.y;
					CURRENT_VIEW_MATRIX . Translation .z = stTarg.z - stTargDiff.z;
				} 
			}
			MAD_RAD_QUKE_NUMMAXPONT = MAD_RAD_QUKE_NUMMAXPONT_DEF;
			break;
		case 2: /* ZOOM */
			if (AllRadCam[CurrentRadCam].IsOrtho)
			{
				float SaveChannelSize;
				Dyf = (float) DY * 0.005f;
				ChannelSize -= ChannelSize * Dyf;
				if (ChannelSize < 1.0f)
					ChannelSize = 1.0f;
				SaveChannelSize = ChannelSize;
				SetDlgItemInt( Mainhwnd , IDC_EDIT2 , (unsigned long)ChannelSize , FALSE);
				ChannelSize = SaveChannelSize;
				ResetAllCompressedMap();
				Compute_MW(0, 0, 1);
			} else
			{
				if (ZScreen != 0.0f)
				{
					Dyf = ZScreen2 * (float) DY * 0.01f;
					ZScreen2 -= Dyf;
					CURRENT_VIEW_MATRIX.Translation.x += Dyf * stTargNormed .x;
					CURRENT_VIEW_MATRIX.Translation.y += Dyf * stTargNormed .y;
					CURRENT_VIEW_MATRIX.Translation.z += Dyf * stTargNormed .z;
				} else
				{
					Dxf *= 0.003f;
					Dyf *= 0.005f;
					CURRENT_VIEW_MATRIX.Translation.x += Dxf * CURRENT_VIEW_MATRIX.I.x;
					CURRENT_VIEW_MATRIX.Translation.y += Dxf * CURRENT_VIEW_MATRIX.I.y;
					CURRENT_VIEW_MATRIX.Translation.z += Dxf * CURRENT_VIEW_MATRIX.I.z;
					CURRENT_VIEW_MATRIX.Translation.x -= Dyf * CURRENT_VIEW_MATRIX.K.x;
					CURRENT_VIEW_MATRIX.Translation.y -= Dyf * CURRENT_VIEW_MATRIX.K.y;
					CURRENT_VIEW_MATRIX.Translation.z -= Dyf * CURRENT_VIEW_MATRIX.K.z;
				}
			}
			MAD_RAD_QUKE_NUMMAXPONT = MAD_RAD_QUKE_NUMMAXPONT_DEF;
			break;
		case 3: /* PAN */
		case 4: /* PAN */

			if (ZScreen != 0.0f)
			{
				if (CI.ProjectCam.IsOrtho)
				{
					Dxf = (float) DX / (float)CI.ProjectCam.CX;
					Dyf = (float) DY / (float)CI.ProjectCam.CY;
				} else
				{
					Dxf = ZScreen * (float) DX / (float)CI.ProjectCam.CX;
					Dyf = ZScreen * (float) DY / (float)CI.ProjectCam.CY;
				}
			} else
			{
				Dxf *= 0.003f;
				Dyf *= -0.003f;
			}

			CURRENT_VIEW_MATRIX.Translation.x += Dxf * CURRENT_VIEW_MATRIX.I.x;
			CURRENT_VIEW_MATRIX.Translation.y += Dxf * CURRENT_VIEW_MATRIX.I.y;
			CURRENT_VIEW_MATRIX.Translation.z += Dxf * CURRENT_VIEW_MATRIX.I.z;
			CURRENT_VIEW_MATRIX.Translation.x += Dyf * CURRENT_VIEW_MATRIX.J.x;
			CURRENT_VIEW_MATRIX.Translation.y += Dyf * CURRENT_VIEW_MATRIX.J.y;
			CURRENT_VIEW_MATRIX.Translation.z += Dyf * CURRENT_VIEW_MATRIX.J.z;
			MAD_RAD_QUKE_NUMMAXPONT = MAD_RAD_QUKE_NUMMAXPONT_DEF;
			break;
		}
		IsIN = 0;
	}
}

/*
===================================================================================================
===================================================================================================
*/
void UPDATEQUAKE(void)
{
    switch(MOUSEBUTTONS)
    {
    case 0: /* Nothing */
        SetCursor(CURS[0]);
        break;
    case 1: /* Rotate */
        SetCursor(CURS[1]);
        break;
    case 2: /* ZOOM */
        SetCursor(CURS[2]);
        break;
    case 3: /* PAN */
    case 4: /* PAN */
        SetCursor(CURS[3]);
        break;
    case 5: /* PERSPECTIV */
        SetCursor(CURS[5]);
        break;
    }
}

/*
===================================================================================================
===================================================================================================
*/
void QUAKE_ON_OFF(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    RECT            L;
    static ULONG    SaveCurrentQuality;
    static RECT     SavedRect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    QUAKE_ON ^= 1;
    if(QUAKE_ON)
    {
		ulOldMouseButton = 0xfffffff;
        MOUSEBUTTONS = 0;
        LongitudeLatitudeFromMAtrix(&CURRENT_VIEW_MATRIX, &CurLat, &CurLong);
        GetClipCursor(&SavedRect);
        SetCapture(Mainhwnd);
        GetWindowRect(Renderhwnd, &L);
		
        /* ClipCursor( &L ); */
        SaveCurrentQuality = CurrentQuality;
        CompputingQuality(QUAKE_QUALITY);
        AdaptToWindow(Mainhwnd, GetDlgItem(Mainhwnd, IDC_CUSTOMRENDER), SourceSX, SourceSY);
        UPDATEQUAKE();
    }
    else
    {
        ClipCursor(&SavedRect);
        ReleaseCapture();
        SetCursor(CURS[0]);
        CompputingQuality(SaveCurrentQuality);
        AdaptToWindow(Mainhwnd, GetDlgItem(Mainhwnd, IDC_CUSTOMRENDER), SourceSX, SourceSY);
        Render_MW();
    }
}

void vQUAKE_OFF(void) {	if (QUAKE_ON) QUAKE_ON_OFF();}
void vQUAKE_ON(void) {if (!QUAKE_ON) QUAKE_ON_OFF();}

/*
===================================================================================================
===================================================================================================
*/
void ROLL_ON_OFF(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    static RECT     SavedRect;
    RECT    LOCALRECT;
    static ULONG    SaveCurrentQuality;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    ROLL_ON ^= 1;
    if(ROLL_ON)
    {
        GetClipCursor(&SavedRect);
        SetCapture(GetDesktopWindow());
		
		ShowCursor( 0 ); 
		ROLL_DELTA = 0;
  
	    ShowWindow(GetDlgItem(Mainhwnd, IDC_DISPLAYNBPOINTSOLUTION), SW_SHOW);
		ShowWindow(GetDlgItem(Mainhwnd, IDC_EDIT3), SW_HIDE);


		GetWindowRect(RollHwnd, &LOCALRECT);
		ClipCursor(&LOCALRECT);
		SetCursorPos(( LOCALRECT . left + LOCALRECT . right ) >> 1, ( LOCALRECT . top + LOCALRECT . bottom) >> 1);
        SaveCurrentQuality = CurrentQuality;
        CompputingQuality(QUAKE_QUALITY);
        AdaptToWindow(Mainhwnd, GetDlgItem(Mainhwnd, IDC_CUSTOMRENDER), SourceSX, SourceSY);
    }
    else
    {
		ShowCursor( 1 ); 
		ClipCursor(&SavedRect);
        ReleaseCapture();
	    ShowWindow(GetDlgItem(Mainhwnd, IDC_DISPLAYNBPOINTSOLUTION), SW_HIDE);
		ShowWindow(GetDlgItem(Mainhwnd, IDC_EDIT3), SW_SHOW);
		ROLL_DELTA = 0;
        CompputingQuality(SaveCurrentQuality);
        AdaptToWindow(Mainhwnd, GetDlgItem(Mainhwnd, IDC_CUSTOMRENDER), SourceSX, SourceSY);
    }
}

/*
===================================================================================================
===================================================================================================
*/
static BOOL CALLBACK RadDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    FILE            *F;
    unsigned long   Counter;
    static long     LocalLock = 0;
    static long     FirstSubdivision = 1;
    char            ConvertBuf[64];
	unsigned long   SUBILLOPTIM = 0;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    switch(msg)
    {
    case WM_INITDIALOG:
        Mainhwnd = hWnd;
        hwndComboCam = GetDlgItem(hWnd, IDC_COMBO1);
        hwndComboRender = GetDlgItem(hWnd, IDC_COMBO3);
        SendMessage(hwndComboRender, CB_RESETCONTENT, 0, 0);
        SendMessage(hwndComboRender, CB_ADDSTRING, 0, (long) "Wired Only");
        SendMessage(hwndComboRender, CB_ADDSTRING, 0, (long) "RLI");
        SendMessage(hwndComboRender, CB_ADDSTRING, 0, (long) "RLI * Mat. Diffuse");
#ifdef MAD_SHOW_ELEMENTS
        SendMessage(hwndComboRender, CB_ADDSTRING, 0, (long) "Show Elements");
#endif
        SendMessage(hwndComboRender, CB_SETCURSEL, Displaymode, 0);
		
        SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_RESETCONTENT, 0, 0);
        SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_ADDSTRING, 0, (long) "Super Fast");
        SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_ADDSTRING, 0, (long) "Fast");
        SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_ADDSTRING, 0, (long) "Normal");
        SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_ADDSTRING, 0, (long) "Good");
        SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_ADDSTRING, 0, (long) "Very Good");
        SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_SETCURSEL, SUBDIV_DEFAULT_QUALITY, 0);
        CompputingQuality(SUBDIV_DEFAULT_QUALITY);
        ResetAllCompressedMap();
        Compute_MW(0, 0, 1);
        CheckRadioButton
			(
            hWnd,
            IDC_RADIODIRECTSHADOW,
            IDC_RADIODIRECTSHADOWP2,
            IDC_RADIODIRECTSHADOW
			);
		
        sprintf(ConvertBuf, "%.2f", EXTRACTION);
		
        /* SetDlgItemText(hWnd, IDC_EDIT1 , ConvertBuf); */
        sprintf(ConvertBuf, "%.2f", DIRECTIONAL_VIEW_SIZE);
		
        hwndProgress = GetDlgItem(hWnd, IDC_PROGRESS1);
        SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 1000));
        SendMessage(hwndProgress, PBM_SETSTEP, (WPARAM) 1, 0);
        if(GLOW_ON)
            CheckDlgButton(hWnd, IDC_CHECK2, BST_CHECKED);
        else
            CheckDlgButton(hWnd, IDC_CHECK2, BST_UNCHECKED);	
        if(Force2sided)
            CheckDlgButton(hWnd, IDC_CHECK4, BST_CHECKED);
        else
            CheckDlgButton(hWnd, IDC_CHECK4, BST_UNCHECKED);
		if (PREVENTWENFINIFHED)
			CheckDlgButton(hWnd, IDC_CHECK11, BST_CHECKED);
        else
            CheckDlgButton(hWnd, IDC_CHECK11, BST_UNCHECKED);

		if (UseAmbiantLight)
			CheckDlgButton(hWnd, IDC_CHECK13, BST_CHECKED);
        else
            CheckDlgButton(hWnd, IDC_CHECK13, BST_UNCHECKED);
		
        EnumRadCam();
        Renderhwnd = GetDlgItem(hWnd, IDC_CUSTOMRENDER);
		RollHwnd = GetDlgItem(hWnd, IDC_RollRECT);
		ShowWindow( RollHwnd, 0); 
        MAD_EnableWindow(IDSTOP, 0);
    case WM_DISPLAYCHANGE:
    case WM_SIZE:   /* Check to see if we are losing or gaining our window. */
    case WM_MOVE:
        AdaptToWindow(hWnd, GetDlgItem(hWnd, IDC_CUSTOMRENDER), SourceSX, SourceSY);
		if(!ImCompute) Blitte_32(TABLEAU);
        return TRUE;
        break;
    case WM_ACTIVATE:
		if (LOWORD(wParam) != WA_INACTIVE)
		{
			AdaptToWindow(hWnd, GetDlgItem(hWnd, IDC_CUSTOMRENDER), SourceSX, SourceSY);
			if(!ImCompute)
			{
				Render_MW();
			}
		}

        return TRUE;
        break;
    case WM_CLOSE:
    case WM_DESTROY:
        EndDialog(hWnd, 0);
        STOP = 1;
        QUITAP = 0;
		break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_CHAR:
        break;
#define signedLOWORD(l) ((short) (l))
#define signedHIWORD(l) ((short) (((DWORD) (l) >> 16) & 0xFFFF))
    case WM_MOUSEMOVE:
        if(!ImCompute)
        {
            if(QUAKE_ON)
            {
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                static ULONG    IM_DRAW = 0;
                RECT            LOCALRECT;
                long            NwX, NwY, Bouge;
                static long     DX = 0;
                static long     DY = 0;
                static long     OLDDX = 0;
                static long     OLDDY = 0;
				POINT			MC;
				POINT			ScreenPoint;
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				
                GetWindowRect(Mainhwnd, &LOCALRECT);
				GetCursorPos(&MC);
				NwX = MC.x;
				NwY = MC.y;
/*                NwX = signedLOWORD(lParam);
                NwY = signedHIWORD(lParam);
                NwX += LOCALRECT.left;
                NwY += LOCALRECT.top;*/
                GetWindowRect(GetDlgItem(hWnd, IDC_CUSTOMRENDER), &LOCALRECT);
				ScreenPoint . x = (UINT)((float)SourceSX * (float)(MC . x - LOCALRECT . left)  / (float)(LOCALRECT.right - LOCALRECT.left));
				ScreenPoint . y = (UINT)((float)SourceSX * (float)(MC . y - LOCALRECT . top)  / (float)(LOCALRECT.bottom - LOCALRECT.top));
                Bouge = 0;
                DX += OLDDX - NwX;
                DY += OLDDY - NwY;
                if(NwY <= LOCALRECT.top)
                {
                    Bouge = 1;
                    NwY = LOCALRECT.bottom - 2;
                }
				
                if(NwY >= LOCALRECT.bottom - 1)
                {
                    Bouge = 1;
                    NwY = LOCALRECT.top + 2;
                }
				
                if(NwX <= LOCALRECT.left)
                {
                    Bouge = 1;
                    NwX = LOCALRECT.right - 2;
                }
				
                if(NwX >= LOCALRECT.right - 1)
                {
                    Bouge = 1;
                    NwX = LOCALRECT.left + 2;
                }
				
                OLDDX = NwX;
                OLDDY = NwY;
                if(Bouge)
                {
                    SetCursorPos(NwX, NwY);
                }
				
                if(!IM_DRAW)
                {
                    IM_DRAW = 1;
                    if(!Bouge) QUAKE_MOVE_CAM((float)DX * (float)SourceSX / (float)(LOCALRECT.right - LOCALRECT.left), DY * (float)SourceSX / (float)(LOCALRECT.bottom - LOCALRECT.top), &ScreenPoint);
                    DX = 0;
                    DY = 0;
                    Render_MW();
//                    Blitte_32(TABLEAU);
                    IM_DRAW = 0;
                }
            } else
				if (ROLL_ON)
				{
	                RECT    LOCALRECT;
					POINT	Point;
	                GetWindowRect(RollHwnd, &LOCALRECT);
					GetCursorPos( &Point ); 
 					ROLL_DELTA += Point.y - (( LOCALRECT . top + LOCALRECT . bottom) >> 1);
					SetCursorPos(( LOCALRECT . left + LOCALRECT . right ) >> 1, ( LOCALRECT . top + LOCALRECT . bottom) >> 1);
					ModifyPointNumInObjectSelectedFromCombo();

				}
		}
		
        break;
    case WM_LBUTTONDBLCLK:
        if(!ImCompute)
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            RECT    RenderRect;
            POINTS  MousePos;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			
            MousePos.x = LOWORD(lParam);    /* Horizontal position of cursor */
            MousePos.y = HIWORD(lParam);    /* Vertical position of cursor */
            GetClientRect(hWnd, &RenderRect);
            ClientToScreen(hWnd, (POINT *) &RenderRect.left);
            ClientToScreen(hWnd, (POINT *) &RenderRect.right);
            MousePos.x += (short) RenderRect.left;
            MousePos.y += (short) RenderRect.top;
            GetClientRect(GetDlgItem(hWnd, IDC_CUSTOMRENDER), &RenderRect);
            ClientToScreen(GetDlgItem(hWnd, IDC_CUSTOMRENDER), (POINT *) &RenderRect.left);
            ClientToScreen(GetDlgItem(hWnd, IDC_CUSTOMRENDER), (POINT *) &RenderRect.right);
            if(wParam & MK_LBUTTON)
            {
                if
					(
                    (MousePos.x < RenderRect.right) &&
                    (MousePos.x > RenderRect.left) &&
                    (MousePos.y > RenderRect.top) &&
                    (MousePos.y < RenderRect.bottom)
					)
                {
                    QUAKE_ON_OFF();
                }
			}
        }
		
        break;
    case WM_LBUTTONDOWN:
        if(!ImCompute)
        {
            if(!QUAKE_ON)
            {
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                RECT        RenderRect;
                RECT        RollRect;
                POINTS      MousePos;
                static int  OldPicked = 0xffffffff;
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				
                MousePos.x = LOWORD(lParam);    /* Horizontal position of cursor */
                MousePos.y = HIWORD(lParam);    /* Vertical position of cursor */
                GetClientRect(hWnd, &RenderRect);
                ClientToScreen(hWnd, (POINT *) &RenderRect.left);
                ClientToScreen(hWnd, (POINT *) &RenderRect.right);
                MousePos.x += (short) RenderRect.left;
                MousePos.y += (short) RenderRect.top;
                GetClientRect(Renderhwnd, &RenderRect);
                ClientToScreen(Renderhwnd, (POINT *) &RenderRect.left);
                ClientToScreen(Renderhwnd, (POINT *) &RenderRect.right);
                GetClientRect(RollHwnd, &RollRect);
                ClientToScreen(RollHwnd, (POINT *) &RollRect.left);
                ClientToScreen(RollHwnd, (POINT *) &RollRect.right);
				if
                    (
					(MousePos.x < RenderRect.right) &&
					(MousePos.x > RenderRect.left) &&
					(MousePos.y > RenderRect.top) &&
					(MousePos.y < RenderRect.bottom) &&
					(!ShowOnlyThisObject)
                    )
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					float   mx, my;
					int     picked;
					int     SaveCurrentQuality;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					
					SaveCurrentQuality = CurrentQuality;
					CompputingQuality(RENDER_QUALITY);
					
					/* Render window */
					mx = (float) (MousePos.x - RenderRect.left) / (float)
						(RenderRect.right - RenderRect.left);
					my = (float) (MousePos.y - RenderRect.top) / (float)
						(RenderRect.bottom - RenderRect.top);
					mx *= (float) SourceSX;
					my *= (float) SourceSY;
					picked = PICKINGBUFF[(((ULONG) my) << SourceSXShift) + (ULONG) mx];
					
					CompputingQuality(SaveCurrentQuality);
					
					SendMessage(GetDlgItem(Mainhwnd, IDC_COMBO4), CB_SETCURSEL, picked, 0);
					InitCurrentObjectSelectedFromCombo();
					if(OldPicked != picked) Render_MW();
					OldPicked = picked;
				}
				else 
                    if
						(
                        (MousePos.x < RollRect.right) &&
                        (MousePos.x > RollRect.left) &&
                        (MousePos.y > RollRect.top) &&
                        (MousePos.y < RollRect.bottom)
						)
                    {
						if ((SelectedObject != NULL) && (LOD_C_IsObjectLOD(SelectedObject)))
							ROLL_ON_OFF();
                    }
            }
            else
            {
                MOUSEBUTTONS |= 2;
                UPDATEQUAKE();
            }
        }
		
        break;
    case WM_LBUTTONUP:
        if(QUAKE_ON)
        {
            MOUSEBUTTONS &= ~2;
            UPDATEQUAKE();
        }
		else if (ROLL_ON)	ROLL_ON_OFF();
			
		
        break;
    case WM_RBUTTONDOWN:
        if(QUAKE_ON)
        {
            MOUSEBUTTONS |= 1;
            UPDATEQUAKE();
        }
		
        break;
    case WM_RBUTTONUP:
        if(QUAKE_ON)
        {
            MOUSEBUTTONS &= ~1;
            UPDATEQUAKE();
        }
		
        break;
    case WM_MBUTTONDOWN:
        if(QUAKE_ON)
        {
            MOUSEBUTTONS |= 4;
            UPDATEQUAKE();
        }
		
        break;
    case WM_MBUTTONUP:
        if(QUAKE_ON)
        {
            MOUSEBUTTONS &= ~4;
            UPDATEQUAKE();
        }
		
        break;
		
    case WM_COMMAND:
        if(HIWORD(wParam) == EN_CHANGE)
        {
            switch(LOWORD(wParam))
            {
            case IDC_EDIT3:
                ModifyPointNumInObjectSelectedFromCombo();
                break;
            case IDC_EDIT2:
				{
					int AAA;
					ChannelSize = (float)GetDlgItemInt( Mainhwnd , IDC_EDIT2 , &AAA , FALSE);
					
					if (AllRadCam[CurrentRadCam].IsOrtho)
					{
						if (!QUAKE_ON) Render_MW();
					}
				}
                break;
            }
        }
		
        if(HIWORD(wParam) == CBN_SELCHANGE)
        {
            switch(LOWORD(wParam))
            {
            case IDC_COMBO1:    /* Camera mode */
                CurrentRadCam = SendDlgItemMessage(hWnd, IDC_COMBO1, CB_GETCURSEL, 0, 0);
                CURRENT_VIEW_MATRIX = AllRadCam[CurrentRadCam].Matrix;
                Render_MW();
                break;
            case IDC_COMBO3:    /* Shading mode */
                Displaymode = SendDlgItemMessage(hWnd, IDC_COMBO3, CB_GETCURSEL, 0, 0);
                Render_MW();
                break;
            case IDC_COMBO2:
                Counter = SendDlgItemMessage(hWnd, IDC_COMBO2, CB_GETCURSEL, 0, 0);
                CompputingQuality(Counter);
                ResetAllCompressedMap();
                Compute_MW(0, 0, 1);
                AdaptToWindow(hWnd, GetDlgItem(hWnd, IDC_CUSTOMRENDER), SourceSX, SourceSY);
                Render_MW();
                break;
            case IDC_COMBO4:
                InitCurrentObjectSelectedFromCombo();
                Render_MW();
                break;
            case IDC_COMBO5:
                CurrentSelectedLight = SendDlgItemMessage(hWnd, IDC_COMBO5, CB_GETCURSEL, 0, 0);
                ReinitCurrentLightCam();
                break;
            }
        }
        else
        {
            switch(LOWORD(wParam))
            {
            case IDSAVE:
                MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, BeforeSaveClbk);
                F = fopen(FileNAme, "wb");
                if(F)
                {
                    MAD_Save(MW, F, 0);
                    fclose(F);
                }

                MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, AfterSaveClbk);
            case IDCANCEL:
                EndDialog(hWnd, 0);
                STOP = 1;
                QUITAP = 0;
                break;
            case IDSTOP:
                STOP = 1;
                break;
            case IDC_CHECK1:
                ShowLines = (ShowLines + 1) % 3;
                Render_MW();
                break;
            case IDC_CHECK2:
                GLOW_ON ^= 1;
                ResetAllCompressedMap();
                Compute_MW(0, 0, 1);
                break;
            case IDC_CHECK4:
                Force2sided ^= 1;
                ResetAllCompressedMap();
                Compute_MW(0, 0, 1);
                break;
            case IDC_CHECK5:
                ShowOnlyThisObject ^= 1;
                Render_MW();
                break;
            case IDC_CHECK6:
                if(SelectedObject != NULL)
                {
                    SelectedObject->ObjectFlags ^= GO_ShadowCut;
					if ((SelectedObject -> ObjectFlags & GO_NonMagnifold) && (SelectedObject -> ObjectFlags & GO_ShadowCut))
				        MessageBox ( NULL, TEXT("This Object is non-magnfold.. \n Sometimes it's works, sometimes \n it's crash if you optimize or subdivide... "), TEXT("Topologic Error"), MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL | MB_TOPMOST | MB_SETFOREGROUND );
                }
				
                break;
            case IDC_CHECK7:
                ((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->LightFlags ^= MAD_LightFlag_PaintLight;
                ResetAllCompressedMap();
                Compute_MW(0, 0, 1);
                break;
            case IDC_CHECK8:
                ((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->LightFlags ^= MAD_LightFlag_CastShadow;
                ResetAllCompressedMap();
                Compute_MW(0, 0, 1);
                break;
            case IDC_CHECK9:
                ((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->LightFlags ^= MAD_LightFlag_OverWalls;
                ResetAllCompressedMap();
                Compute_MW(0, 0, 1);
                break;
            case IDC_CHECK10:
                if(((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->IsOn)
                    ((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->IsOn = 0;
                else
                    ((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->IsOn = 1;
                ResetAllCompressedMap();
                Compute_MW(0, 0, 1);
                break;
			case IDC_CHECK11:
				PREVENTWENFINIFHED ^= 1;
				break;
			case IDC_CHECK13:
				UseAmbiantLight ^= 1;
				break;
            case IDC_BUTTONColorlight:
                {
                    ((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->LightColor = CDLG_USR_GetColor
                        (
						((MAD_Light *) AllRadCam[CurrentSelectedLight].Object)->LightColor,
						hWnd
                        );
                }
				
                break;

            case IDC_BUTTONSimplify2:
                if(!ImCompute)
                {
					MAD_UpdateInfoStr2("Illumine...");
					SeprogressPosMult = 1.0f; 
					SeprogressPosBase = 0.0f;
	                ResetAllCompressedMap();
		            Compute_MW(0, 0, 1);
                    ImCompute = 1;
                    MAD_GriseAll(0);
                    STOP = 0;
                    TotalNumBerOfPoints = 0;
                    TotalNumBerOfPolygons = 0;
                    CurrentPickingColor = 0;
                    MAD_NodeEnum
						(
                        MW,
                        ID_MAD_GeometricObject_V0,
                        DrawObjectNodeCallbackIlluminateReset
						);
                    MAD_UpdateInfo();
					MAD_ComputeNeightbouring(MW);
                    Compute_MW(1, 0, 0);
                    MAD_GriseAll(1);
                    Render_MW();
                    ImCompute = 0;
                    InitCurrentObjectSelectedFromCombo();
					SeprogressPos(0.0f);
					MAD_UpdateInfoStr2("Illumine");
					MAD_UpdateInfoStr("Finished");
					if (PREVENTWENFINIFHED)
						MessageBox( hWnd , " Illumine is finished ", TEXT("MAD RAD") , MB_OK | MB_ICONINFORMATION | MB_TASKMODAL | MB_TOPMOST | MB_SETFOREGROUND ); 
                }
                break;
			case IDC_BUTTON3:
				SUBILLOPTIM = 1;
            case IDC_Refine:
                if(!ImCompute)
                {
					unsigned long OldNumberOfPoints;
					unsigned long Counter;
                    ImCompute = 1;
                    MAD_GriseAll(0);
	                ResetAllCompressedMap();
		            Compute_MW(0, 0, 1);
                    STOP = 0;
                    TotalNumBerOfPoints = 0;
                    TotalNumBerOfPolygons = 0;
                    CurrentPickingColor = 0;
                    MAD_NodeEnum(MW,ID_MAD_GeometricObject_V0,DrawObjectNodeCallbackCountPoly);
					OldNumberOfPoints = TotalNumBerOfPoints+1;
					Counter = 0;
					SeprogressPosMult = 0.5f / (float)MAD_NumberOFSub; 
					SeprogressPosBase = 0.0f;
                    while ((OldNumberOfPoints != TotalNumBerOfPoints) && (!STOP) && (Counter < MAD_NumberOFSub))
                    {
						MAD_UpdateInfoStr2("Subdivide...");
						OldNumberOfPoints = TotalNumBerOfPoints;
				        MAD_ComputeNeightbouring(MW);
                        Compute_MW(0, 1, 0);
						Counter++;
						TotalNumBerOfPoints = 0;
						TotalNumBerOfPolygons = 0;
						CurrentPickingColor = 0;
						MAD_NodeEnum(MW,ID_MAD_GeometricObject_V0,DrawObjectNodeCallbackCountPoly);
						MAD_UpdateInfo();
						SeprogressPosBase += 0.5f / (float)MAD_NumberOFSub;
						SeprogressPos(0.0f);
						if (TotalNumBerOfPoints >= 1000000) Counter = 0xfffffff;
						Render_MW();
                    }
					if (STOP)
						SUBILLOPTIM = 0;
					STOP = 0;
                    TotalNumBerOfPoints = 0;
                    TotalNumBerOfPolygons = 0;
                    CurrentPickingColor = 0;
                    MAD_NodeEnum(MW,ID_MAD_GeometricObject_V0,DrawObjectNodeCallbackIlluminateReset);
                    MAD_UpdateInfo();
					SeprogressPosMult = 0.5f; 
					SeprogressPosBase = 0.5f;
					MAD_UpdateInfoStr2("Illumine...");
					MAD_ComputeNeightbouring(MW);
                    Compute_MW(1, 0, 0);
                    Render_MW();
                    MAD_GriseAll(1);
                    ImCompute = 0;
                    InitCurrentObjectSelectedFromCombo();
					SeprogressPosMult = 1.0f; 
					SeprogressPosBase = 0.0f;
					SeprogressPos(0.0f);
					MAD_UpdateInfoStr2("Subdivide");
					MAD_UpdateInfoStr("Finished");
					if (PREVENTWENFINIFHED && (!SUBILLOPTIM))
						MessageBox( hWnd , " Subdivide is finished ", TEXT("MAD RAD") , MB_OK | MB_ICONINFORMATION | MB_TASKMODAL | MB_TOPMOST |  MB_SETFOREGROUND ); 
                }
                if (!SUBILLOPTIM) break;
                if (STOP) break;
            case IDC_BUTTONSimplify:
                if(!ImCompute)
                {
					MAD_UpdateInfoStr2("Optimize...");
                    ImCompute = 1;
                    STOP = 0;
                    MAD_GriseAll(0);
                    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, Simplify_GO);
                    MAD_ComputeNeightbouring(MW);
                    ResetAllCompressedMap();
                    Compute_MW(0, 0, 1);
                    TotalNumBerOfPoints = 0;
                    TotalNumBerOfPolygons = 0;
                    CurrentPickingColor = 0;
                    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, DrawObjectNodeCallbackCountPoly);
                    MAD_UpdateInfo();
                    MAD_GriseAll(1);
                    Render_MW();
                    ImCompute = 0;
                    InitCurrentObjectSelectedFromCombo();
					MAD_UpdateInfoStr2("Optimize");
					MAD_UpdateInfoStr("Finished");
					if (PREVENTWENFINIFHED && (!SUBILLOPTIM))
						MessageBox( hWnd , " Optimize is finished ", TEXT("MAD RAD") , MB_OK | MB_ICONINFORMATION | MB_TASKMODAL | MB_TOPMOST | MB_SETFOREGROUND ); 
                }
				if (PREVENTWENFINIFHED && (SUBILLOPTIM))
					MessageBox( hWnd , "Subdivide -> Illumine -> Optimize is finished ", TEXT("MAD RAD") , MB_OK | MB_ICONINFORMATION | MB_TASKMODAL | MB_TOPMOST | MB_SETFOREGROUND ); 
				SUBILLOPTIM = 0;
				break;
			case IDC_BUTTONAmbientColor:
                MW->AmbientColor = CDLG_USR_GetColor(MW->AmbientColor,hWnd);
				break;
			case IDC_BUTTONBCKCOL:
				MAD_BackGroudColor = (MAD_BackGroudColor & 0x00ff00) | ((MAD_BackGroudColor & 0xff) << 16) | ((MAD_BackGroudColor & 0xff0000) >> 16);
				MAD_BackGroudColor = CDLG_USR_GetColor(MAD_BackGroudColor,hWnd);
				MAD_BackGroudColor = (MAD_BackGroudColor & 0x00ff00) | ((MAD_BackGroudColor & 0xff) << 16) | ((MAD_BackGroudColor & 0xff0000) >> 16);
				Render_MW();
				break;
            case IDC_BUTTONFC:
                QUAKE_ON_OFF();
                break;
				/* Case IDC_BUTTONHELP1: DISPLAY_HELP(IDC_BUTTONHELP1, IDD_HELP1 ); break; */
            case IDC_BUTTONHELP2:
                DISPLAY_HELP(IDC_BUTTONHELP2, IDD_HELP2);
                break;
            case IDC_BUTTONHELP3:
                DISPLAY_HELP(IDC_BUTTONHELP3, IDD_HELP3);
                break;
            case IDC_BUTTONHELP4:
                DISPLAY_HELP(IDC_BUTTONHELP4, IDD_HELP4);
                break;
            }
        }
		
        break;
    default:
        return FALSE;
    }
	
    return TRUE;
}

/*
===================================================================================================
===================================================================================================
*/
extern "C" {
extern unsigned long Base;
}
INT WINAPI WinMain(HINSTANCE hInstTLIB, HINSTANCE, LPSTR strCmdLine, INT)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MSG             msg;
    unsigned long   Count;
    HWND            WinDeauws;
    FILE            *F;
    char            MyString[1024];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Count = 0;
	
    QUITAP = 1;
	
    CURS[0] = LoadCursor(hInstTLIB, MAKEINTRESOURCE(Normal));
    CURS[1] = LoadCursor(hInstTLIB, MAKEINTRESOURCE(Rotation));
    CURS[2] = LoadCursor(hInstTLIB, MAKEINTRESOURCE(TMT_Zoom));
    CURS[3] = LoadCursor(hInstTLIB, MAKEINTRESOURCE(Hand));
    CURS[4] = LoadCursor(hInstTLIB, MAKEINTRESOURCE(ROLL));
    CURS[5] = LoadCursor(hInstTLIB, MAKEINTRESOURCE(PERSPECTIV));
	
    /*
	* If (!DialogBox ( hInstTLIB , MAKEINTRESOURCE( IDD_DIALOG1 ), NULL , (int (__stdcall
	* )(void)) &RadDlgProc)) return 0;
	*/
    MW = NULL;
    strcpy(FileNAme, strCmdLine);
	Mad_meminit();
    MTH_fn_vInit();
    if(*strCmdLine != 0)
    {
        F = fopen(strCmdLine, "rb");
        if(F)
        {
            MW = MAD_Load(F, NULL, NULL);
            fclose(F);
	        F = fopen(strCmdLine, "rb");
			ORIGINAL_MW = MAD_Load(F, NULL, NULL);
            fclose(F);
	    }
    } else
	{

	}
	
    if(!MW)
    {
        MessageBox
			(
            NULL,
            TEXT("Can't open source file \n\n->Exit"),
            TEXT("Read error"),
            MB_OK | MB_ICONSTOP | MB_TASKMODAL
			);
        return 1;
    }

	MAD_MakeNoHinstancesGO(MW);
	MAD_MakeNoHinstancesGO(ORIGINAL_MW);
	
    SaveTableau = TABLEAU = (unsigned long *) malloc(MAXSourceSX * MAXSourceSY * 4L + MAXSourceSX * 4L);
    PICKINGBUFF = (unsigned long *) malloc(MAXSourceSX * MAXSourceSY * 4L + MAXSourceSX * 4L);
    ZBUFFER = (float *) malloc(MAXSourceSX * MAXSourceSY * 4L + MAXSourceSX * 4L);
	
    /* GlobBuffer = (float *)malloc(MAXSourceSX * MAXSourceSY * 4L); */
    initAllCompressedMap();
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, FirstInitObjectClbk);
    MAD_NodeEnum(ORIGINAL_MW, ID_MAD_GeometricObject_V0, FirstInitObjectClbk);
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, CmpNumBerOfObject);
    MAD_NodeEnum(ORIGINAL_MW, ID_MAD_GeometricObject_V0, CmpNumBerOfObject);
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, ComputeBDVOLCallback);
    MAD_NodeEnum(ORIGINAL_MW, ID_MAD_GeometricObject_V0, ComputeBDVOLCallback);
    MAD_ComputeAllNormales(MW);
    MAD_ComputeAllNormales(ORIGINAL_MW);
    MAD_ComputeNeightbouring(MW);
    MAD_ComputeNeightbouring(ORIGINAL_MW);
	
    /* IDI_ICON1 */
    MAD_Rasterize_Init();
    InitCommonControls();
    MATH_gd_Decal = 3.0F * pow(2, 51);
    WinDeauws = CreateDialog
        (
		hInstTLIB,
		MAKEINTRESOURCE(IDD_DIALOG1),
		NULL,
		(int(__stdcall *) (struct HWND__ *,unsigned int,unsigned int,long)) & RadDlgProc
        );
    if (!NumberLightCam) 
    {
        MessageBox
			(
            NULL,
            TEXT("There is no light in that scene.\n\n->Exit"),
            TEXT("No lights"),
            MB_OK | MB_ICONSTOP | MB_TASKMODAL
			);
        return 1;
    }
    ShowWindow(WinDeauws, SW_SHOW);

	SendMessage(WinDeauws, WM_SETICON , ICON_BIG , (LPARAM)LoadIcon(hInstTLIB , MAKEINTRESOURCE(IDI_ICON2)));


    sprintf(MyString, "Easy Rader (File %s)", strCmdLine);
    SetWindowText(WinDeauws, MyString);
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, ColorizeObjectNodeCallback);
	Base = 0;
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, Numerotize);
	Base = 0;
    MAD_NodeEnum(ORIGINAL_MW, ID_MAD_GeometricObject_V0, Numerotize);
	InitCurrentObjectSelectedFromCombo();
    TotalNumBerOfPoints = 0;
    TotalNumBerOfPolygons = 0;
    CurrentPickingColor = 0;
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, DrawObjectNodeCallbackCountPoly);
    MAD_UpdateInfo();
	SetDlgItemInt( Mainhwnd , IDC_EDIT2 , (unsigned long)ChannelSize , FALSE);
	while(QUITAP)
    {
        Count++;
        while(PeekMessage(&msg, NULL/*WinDeauws */, 0, 0, PM_REMOVE))
        {
            if(!IsDialogMessage(WinDeauws, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
		
        if(Count < 5)
        {
            CURRENT_VIEW_MATRIX = AllRadCam[CurrentRadCam].Matrix;
			if (Count == 4)
			SendMessage(WinDeauws, WM_COMMAND, (BN_CLICKED << 16) | IDC_BUTTONSimplify2, 0);
            Render_MW();
        }
		
        /* Main loop */
		
        /* Memset sqrt memcmp malloc fseek */
    }
	
    free(TABLEAU);
    free(PICKINGBUFF);
    free(ZBUFFER);
	
    /* Free(GlobBuffer); */
    CleanAll();
	
    MAD_FREE(); 
    UnregisterClass(TMT_CLASSNAME, hInstTLIB);
    return 0;
}

