#include "GX8/RASter/Gx8_Ras_Def.h"
#include "GDInterface/GDInterface.h"
#include "BASe/BAStypes.h"
#include "GX8/RASter/Gx8_Bench.h"
#include <d3d8.h>
#include "Gx8\Gx8init.h"
#include "MATerial\MATstruct.h"

#include "Sdk\Sources\TIMer\PROfiler\XBox\PRO_xb.h"
#include "GX8/RASter/Gx8_CheatFlags.h"
#include <d3dx8.h>

#include "Gx8\Gx8renderstate.h"

#ifndef _XBOX
#define XB_fn_vDrawColoredZoomText
#endif

#include "Gx8_MenuSetup.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

float g_fFramePerSecond;

typedef struct 
{
	char Name[32];
	u_int FilterType;
}
RasterDescriptor;

#define Raster_FilterEngine 1
#define Raster_FilterDisplay 2
#define Raster_FilterGlobal 4

#define Raster_FilterALL 7

extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;
extern u_int Status;
extern u_int RasterMode;
extern u_int RasterFilter;
extern u_int g_iDDShadow;
extern int g_iNumShadow;

#define C_HSIZE 6.4f
#define C_VSIZE 4.8f
// left : 10% of screen
#define XLEFT 10.f
// size max : 80% of screen, size/8
#define HSIZEON8 10.f
// height or one line : 1/100 screen height
#define HEIGHTRAS 1.5f

#define FIRST_FRAME_WIDTH				( 65.0f * 70.0f / 85.0f )		// width of first frame raster bar
#define FRAME_WIDTH_DIVISION_COEFF		4.0f							// division coeff for frame width

static float gfTotalFNumber=0.f;

static RasterDescriptor RasterDescritpors[GSP_NRaster] =
{
    "MisEng",Raster_FilterEngine, // tout ce qui est dans engcall et qui n'est pas rasterise
    "Lig",Raster_FilterDisplay,
    "SKN",Raster_FilterDisplay,
    "SDW",Raster_FilterDisplay,
    "DRW",Raster_FilterDisplay,
//5    
    "ANIr",Raster_FilterDisplay,
    "SRS",Raster_FilterDisplay,
    "GFX",Raster_FilterALL,
    "CVC",Raster_FilterDisplay,
    "CSpec",Raster_FilterDisplay,
//10 
    "CDAlp",Raster_FilterEngine,
    "ANI",Raster_FilterEngine,	// anim
    "MAT D",Raster_FilterDisplay,
    "Zlst S",Raster_FilterDisplay,
    "AI",Raster_FilterEngine,	// IA
//15
    "INO",Raster_FilterEngine,	// pad
    "Col",Raster_FilterEngine,	// colision
    "Rec",Raster_FilterEngine,	// recalage
    "Eng",Raster_FilterEngine|Raster_FilterGlobal,	// engine = col + rec + AI + DivEng
    "Dis",Raster_FilterDisplay|Raster_FilterGlobal,	// display
//20
    "TxProc",Raster_FilterDisplay, // texture procedural
    "Flip",Raster_FilterDisplay,// flip (normalement raster jamais active)
    "MRM",Raster_FilterDisplay,
    "1-DRW",Raster_FilterDisplay,
    "GO-DRW",Raster_FilterDisplay,
//25
    "SND",Raster_FilterEngine, // snd
    "TAM",Raster_FilterEngine, // tabmanager in engcall
    "EngRei",Raster_FilterEngine, //eng reinit in engcall
    "WorAct",Raster_FilterEngine, //woractivate
    "EOT",Raster_FilterEngine,
//30
    "SnP",Raster_FilterEngine,
    "MdF",Raster_FilterEngine, //gen Mdf
    "EVE",Raster_FilterEngine,
    "HIE",Raster_FilterEngine,
    "DYN",Raster_FilterEngine,
//35
    "VIEW",Raster_FilterALL,
    "Visi",Raster_FilterALL,
    "Lens F",Raster_FilterDisplay, // texture procedural
    "FIFO",Raster_FilterDisplay|Raster_FilterGlobal, // texture procedural
    "AI-SND",Raster_FilterEngine, // sub part of IA : sound fonctions
//40
    "IOP",Raster_FilterEngine, //special part for IOP com
    "VIDEO",Raster_FilterDisplay, //special part for IOP com
    "SPG",Raster_FilterDisplay, //Sprite generator
	"BigVB",Raster_FilterDisplay, // Big Vertex Buffer usage
	"GoodVB",Raster_FilterDisplay, // Good Vertex Buffer usage
//45
	"IndP",Raster_FilterDisplay, // All element indexed primitive
	"Sprite",Raster_FilterDisplay,
	"Rasters",Raster_FilterGlobal, // temps mis pour l'affichage de ces rasters !
	"Water",Raster_FilterDisplay,
//49 
    "SPG2",Raster_FilterDisplay, //Sprite generator
    "SPG2b",Raster_FilterDisplay, //Sprite generator

	"Total",Raster_FilterALL,// total : toujours en dernier !
};
/*
 * on a donc trame = ino + eng + dis + snd + txp
 * avec 
 *      eng = col + rec + ai + diveng
 */

ULONG Rasters[GSP_NRaster];
ULONG Rasters_Max[GSP_NRaster];
float RastersSmooth[GSP_NRaster];
int Rasters_MaxEnble = 0;
LPD3DXSPRITE gpSprite;
u_int Sorted[GSP_NRaster];

float RastersSmooth[GSP_NRaster];
ULONG RastersStarts[GSP_NRaster];

void RAS_DrawShadowedRectangle(LPDIRECT3DDEVICE8 Device, float x, float y, float ww, float h, u_int C)
{
	float frameWidth;
	float w;

   	if (ww<=0.f)
	return;
	
	frameWidth = FIRST_FRAME_WIDTH;

	Gx8_DrawRectangle((void *)Device, x*C_HSIZE + 2.0f,y*C_VSIZE + 2.0f,ww*C_HSIZE,h*C_VSIZE,0x80000000);
	while (ww>0.f)
	{
		w = ((ww>frameWidth)?frameWidth:ww);
		Gx8_DrawRectangle((void *)Device, x*C_HSIZE,y*C_VSIZE,w*C_HSIZE,h*C_VSIZE,0xff000000|C);
		ww -= frameWidth;
		frameWidth /= FRAME_WIDTH_DIVISION_COEFF;
		// plus de 1 trame --> changement de couleur
		C ^= 0xffffff;
		x+= w;
	}
}

BOOL RAS_bDrawOneRaster(LPDIRECT3DDEVICE8 Device, float FrameNumb , float YPos , u_int TheColor)
{
#define C_DecalBarre 2.0f/C_VSIZE

	float fThreshold;
	float width;
	float frameWidth;
	int frame;
	int i;

	fThreshold = ((Status&GSP_Status_Raster_Threshold_On)?0.01f:0.00000001f);
	// Text : text to display for the raster line
	// FrameNumb : nb of frame of the raster line
	// YPos ...
	// TheColor ...
	
	if (FrameNumb<fThreshold)
		return FALSE;

	if (FrameNumb > 4.f)
		FrameNumb = 4.f;

	frame = (int) FrameNumb;
	frameWidth = FIRST_FRAME_WIDTH;
	width = 0.0f;
	for ( i = 0; i<frame; i++)
	{
		width += frameWidth;
		frameWidth /= FRAME_WIDTH_DIVISION_COEFF;
	}
	width += (FrameNumb - frame) * frameWidth;

	RAS_DrawShadowedRectangle(Device, XLEFT*1.5, YPos+C_DecalBarre, width, HEIGHTRAS, TheColor);
	// affichage des cadres transparents sous les textes de rasters.
	Gx8_DrawRectangle((void *)Device, 35.f,YPos*C_VSIZE,XLEFT*1.5f*C_HSIZE-35.f,2.0f*HEIGHTRAS*C_VSIZE,0xa0000000);
	Gx8_DrawRectangle((void *)Device, XLEFT*C_HSIZE*8.5f-4.0f,YPos*C_VSIZE,45.f,2.0f*HEIGHTRAS*C_VSIZE,0xa0000000);
	Gx8_DrawRectangle((void *)Device, (XLEFT*1.5f + FIRST_FRAME_WIDTH)*C_HSIZE, (YPos+C_DecalBarre)*C_VSIZE, 0.2f*C_HSIZE, 2.0f*HEIGHTRAS*C_VSIZE, 0xa0000000);
	Gx8_DrawRectangle((void *)Device, (XLEFT*1.5f + ( FIRST_FRAME_WIDTH * 1.25f) )*C_HSIZE, (YPos+C_DecalBarre)*C_VSIZE, 0.2f*C_HSIZE, 2.0f*HEIGHTRAS*C_VSIZE, 0xa0000000);

	return TRUE;
}


BOOL 
RAS_bDrawOneTextRaster( LPDIRECT3DDEVICE8 Device,
                        const char* Text,
                        float FrameNumb,
                        float YPos,
                        u_int TheColor)
{
#define C_ZOOM 0.7f
	WCHAR   asz_Temp[512];
	float fThreshold;
	// Text : text to display for the raster line
	// FrameNumb : nb of frame of the raster line
	// YPos ...
	// TheColor : ?
	fThreshold = ((Status&GSP_Status_Raster_Threshold_On)?0.01f:0.f);
	if (FrameNumb>fThreshold)
	{
		// titre du raster
		swprintf (asz_Temp, L"%S", Text);
		XB_fn_vDrawColoredZoomText (40, (int)(YPos*C_VSIZE) , 0xff000000|TheColor, C_ZOOM, asz_Temp);
		//XB_fn_vDrawColoredZoomText (41, (int)(YPos*C_VSIZE)+1 , 0xff000000|(0xffffff-TheColor), C_ZOOM, asz_Temp);

		// et puis la valeur...
		if (Status & GSP_Status_Raster_Prop_On)
			swprintf(asz_Temp, L"%.3d%%", (int)(FrameNumb * 100.0f / gfTotalFNumber));
		else
			swprintf(asz_Temp, L"%.3f", FrameNumb);
		XB_fn_vDrawColoredZoomText ((int)(XLEFT*C_HSIZE*8.5f), (int)(YPos*C_VSIZE), 0xff000000|TheColor, C_ZOOM , asz_Temp);
		//XB_fn_vDrawColoredZoomText ((int)(XLEFT*C_HSIZE*8.5f)+1, (int)(YPos*C_VSIZE)+1 , 0xff000000|(0xffffff-TheColor), C_ZOOM, asz_Temp);

		return TRUE;
	}

	return FALSE;

}

void fnvDrawOneShadow(LPDIRECT3DDEVICE8 Device,int textNum)
{
#define XX 480.f
#define YY 20.f
#define WW 128.f
#define HH 128.f
/*#define U 128.f
#define V 128.f*/
#define U 1.f
#define V 1.f
	ULONG ulBlendingMode = 0;

	// en dessous, cadre noir
	ulBlendingMode = 0;

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);
    Gx8_RS_CullFace(p_gGx8SpecificData, D3DCULL_CCW);

	GDI_gpst_CurDD->ul_CurrentDrawMask ^= GDI_Cul_DM_TestBackFace;
	//GDI_gpst_CurDD->ul_CurrentDrawMask ^= GDI_Cul_DM_NotInvertBF;

	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode,0);

    IDirect3DDevice8_SetRenderState( Device, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetTexture(Device,0,NULL);

	Gx8_DrawRectangle((void *)Device,XX-1,YY-1,WW+2.f,HH+2.f,0xffffffff);

	// render the rendered target
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_PSX2ShadowSpecific);
	Gx8_SetTextureBlending(0x80000000+textNum, ulBlendingMode,0);

	{
		struct MonStream_ {
			float x,y,z,rhw;
			DWORD Color;
			float u,v;
		} MonStream[4];
		HRESULT hr;

		UINT uiVStride;
		Gx8_vSetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(2));
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(2));

		MonStream[0].x = XX;
		MonStream[0].y = YY+HH;
		MonStream[0].z = 0.0f;
		MonStream[0].rhw = 0.99999f;
		MonStream[0].Color = 0x80ffffff;
		MonStream[0].u = 0.f;
		MonStream[0].v = 1.0f*V;

		MonStream[1].x = XX;
		MonStream[1].y = YY;
		MonStream[1].z = 0.0f;
		MonStream[1].rhw = 0.99999f;
		MonStream[1].Color = 0x80ffffff;
		MonStream[1].u = 0.f;
		MonStream[1].v = 0.f;

		MonStream[2].x = XX+WW;
		MonStream[2].y = YY+HH;
		MonStream[2].z = 0.0f;
		MonStream[2].rhw = 0.99999f;
		MonStream[2].Color = 0x80ffffff;
		MonStream[2].u = 1.f*U;
		MonStream[2].v = 1.0f*V;

		MonStream[3].x = XX+WW;
		MonStream[3].y = YY;
		MonStream[3].z = 0.0f;
		MonStream[3].rhw = 0.99999f;
		MonStream[3].Color = 0x80ffffff;
		MonStream[3].u = 1.f*U;
		MonStream[3].v = 0.0f;

		hr = IDirect3DDevice8_DrawPrimitiveUP(Device,D3DPT_TRIANGLESTRIP,2,(void*)MonStream,uiVStride);
	}
			
}
void Gx8_MemoryAlarm()
{
#ifndef _FINAL_
{
	if (MEM_gst_MemoryInfo.ul_CurrentFree>>10 < 512)
	{
		static u8 flop;
		char string[1024];
		WCHAR   asz_string[1024];
		sprintf(string,"WARNING:%dKo of free mem", MEM_gst_MemoryInfo.ul_CurrentFree>>10 );
		swprintf (asz_string, L"%S", string);
		if (flop<8)
		XB_fn_vDrawColoredZoomText (80, 20, 0xffff0000, 1.5f , asz_string);
		else
		XB_fn_vDrawColoredZoomText (80, 20, 0xff880000, 1.5f , asz_string);
		flop%=16;
		flop++;
		
	}
}
#endif
}
void Gx8_MemorySpy(void *Dev)
{
#ifndef _FINAL_
LPDIRECT3DDEVICE8 Device = (LPDIRECT3DDEVICE8)Dev;
	//#ifdef MEM_SPY
{
char string[1024];

extern float TIM_gf_MainClockReal;
extern float WOR_TimeSinceLastWorldLoad;

u32 YPos;
YPos = 10;
XB_fn_vBeginDrawText();

	{
		extern u32 MEM_ulSummaryNb;
		extern	MEM_stSummaryElement *MEM_SortSummaryTable[MEM_ulSummaryTableSize];
		extern void MEM_vBuildSummary();
		u32 CurrentNum,Summ;
		CurrentNum = MEM_ulSummaryNb;
		MEM_vBuildSummary();
		if (MEM_ulSummaryNb != -1)
		{
			char string_MO[1024];
			WCHAR   asz_string[1024];
			Summ  = 0;
			for (CurrentNum = 0 ; CurrentNum < MEM_ulSummaryNb ; CurrentNum++) Summ += MEM_SortSummaryTable[CurrentNum]->ulAllocSize;
			sprintf(string_MO,"%2d.%03dMo(%d%%)", Summ >> 20 , (((Summ >> 10) & 1023) * 999) / 1024 , (u32)(100.0f * (float)Summ / (float)MEM_gst_MemoryInfo.ul_RealSize ));
			sprintf(string,"TOTAL:%s", string_MO );
			swprintf (asz_string, L"%S", string);
			
			Gx8_SetRastersRenderState();
			XB_fn_vDrawColoredZoomText (100, 40 + YPos, 0x8f7070ff, C_ZOOM , asz_string);

			YPos += (int)(16.0 * 1.0f);

			for (CurrentNum = 0 ; CurrentNum < MEM_ulSummaryNb ; CurrentNum++)
			{
				if (MEM_SortSummaryTable[CurrentNum]->ulAllocSize > 128 * 1024)
				{
					u32 color;
					sprintf(string_MO,"%2d.%03dMo ", MEM_SortSummaryTable[CurrentNum]->ulAllocSize >> 20 , (((MEM_SortSummaryTable[CurrentNum]->ulAllocSize >> 10) & 1023) * 999) / 1024 );

					color  = (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 4)) << (24 - 2 - 4);
					color |= (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 2)) << (16 - 2 - 2);
					color |= (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 0)) << (8 - 2);
					color |= 0x3f3f3f;
					color |= 0xff000000;

					sprintf(string,"%s:%s", MEM_SortSummaryTable[CurrentNum]->sHeader , string_MO );
					swprintf (asz_string, L"%S", string);
					XB_fn_vDrawColoredZoomText (100, 40 + YPos, color, C_ZOOM , asz_string);
					
					YPos += (int)(16.0 * 1.0f);
				}
			}
		}
	}
XB_fn_vEndDrawText();
}
#endif
}

void RAS_fnvDrawRasters(void *Dev)
{
int i;
float YPos,fFact,frameRateLimit;
LPDIRECT3DDEVICE8 Device = (LPDIRECT3DDEVICE8)Dev;
ULONG					ulBlendingMode;
u_int Filter;
DWORD Color;
#define AR_SMOOTH 0.95f

YPos = 100.0f-XLEFT;

	Gx8_MemoryAlarm();
	if (Device==NULL)
		return;

	// draw menus
	Gsp_Setup(Dev);

	// draw debug shadow
	if (g_iDDShadow) 
		fnvDrawOneShadow(Device,g_iNumShadow);

	// doit on afficher les rasters ?
	if ((Status&GSP_Status_Raster_On) == 0)
	{
		return;
	}

	Filter = RasterFilter;
	if (RasterFilter == 0) Filter = 0xffffffff;
	if (RasterFilter == 3) Filter = Raster_FilterGlobal;

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode,0);

	Gx8_RS_DepthFunc(p_gGx8SpecificData, D3DCMP_ALWAYS);
    Gx8_RS_CullFace(p_gGx8SpecificData, D3DCULL_CCW);

    IDirect3DDevice8_SetRenderState( Device, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetTexture(Device,0,NULL);

	// get the total framenumb
	fFact = g_fFramePerSecond/(float)TIM_ul_PreciseGetTicksPerSecond();
	gfTotalFNumber = Rasters[GSP_NRaster-1]*fFact;

	frameRateLimit = g_fFramePerSecond / g_FrameRateAlarmLimit;
	if ( gfTotalFNumber > frameRateLimit )
	{		// draw red bar alarm
		Gx8_DrawRectangle(Device, 0.0f, 0.0f, 1000.0f, 40.0f, 0xffff0000);
	}

	// calcul des smooths de rasters, et tri
	for (i=0; i<GSP_NRaster; i++)
	{
		RastersSmooth[i] = RastersSmooth[i] * AR_SMOOTH + (float)Rasters[i] * (1.0f - AR_SMOOTH);
	}

	{ //tri	
		u_int RC,RCS;
		for (RC = 0 ; RC < GSP_NRaster ; RC++) 
			Sorted[RC] = RC;
		// si tri...
		if ((RasterMode&=0x10000)!=0)
		{
			for (RC = 0 ; RC < GSP_NRaster - 1 ; RC++) 
			for (RCS = RC + 1 ; RCS < GSP_NRaster; RCS++) 
			{
				if (RastersSmooth[Sorted[RC]] < RastersSmooth[Sorted[RCS]])
				{
					u_int SWAP;
					SWAP = Sorted[RC];
					Sorted[RC] = Sorted[RCS];
					Sorted[RCS] =  SWAP;
				}
			}
		}
	}

	for (i=0; i<GSP_NRaster; i++)
	{
		if (YPos>10.f)
		{
			if ((RasterDescritpors[Sorted[i]].FilterType&Filter)!=0)
			{
				Color = 0xf0808020;
				if ((Sorted[i] + 1) & 1) Color |= 0x40;
				if ((Sorted[i] + 1) & 2) Color |= 0x4000;
				if ((Sorted[i] + 1) & 4) Color |= 0x400000;
				if (RAS_bDrawOneRaster(Device,(float)Rasters[Sorted[i]]*fFact,YPos,Color))
					YPos -= 2*HEIGHTRAS;
			}
		}
	}

	// textes
	YPos = 100.0f-XLEFT;
    Gx8_SetRastersRenderState();
	for (i=0; i<GSP_NRaster; i++)
	{
		if (YPos>10.f)
		{
			if ((RasterDescritpors[Sorted[i]].FilterType&Filter)!=0)
			{
				Color = 0xf0808020;
				if ((Sorted[i] + 1) & 1) Color |= 0x40;
				if ((Sorted[i] + 1) & 2) Color |= 0x4000;
				if ((Sorted[i] + 1) & 4) Color |= 0x400000;

				if (RAS_bDrawOneTextRaster(Device,RasterDescritpors[Sorted[i]].Name,(float)Rasters[Sorted[i]]*fFact,YPos,Color))
					YPos -= 2*HEIGHTRAS;
			}
		}
	}

	// end : RAZ counters
	for (i=0; i<GSP_NRaster; i++)
	{
		//RastersStarts[i] = 0; 
		Rasters[i] = 0;
	}

	// draw frame rate number
	if ( g_FrameRateNumber )
		RAS_bDrawOneTextRaster( Device, "Frame Rate", (float) g_fFramePerSecond / gfTotalFNumber, 5.0f, 0x00ffffff);

}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
