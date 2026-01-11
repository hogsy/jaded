// Dx9gauges.c


#include "Dx9gauges.h"
#include "Dx9draw.h"
#include "Dx9bench.h"
#include "Dx9matrix.h"
#include "Dx9text.h"
#include "Dx9renderstate.h"
#include "Dx9texturestagestate.h"
#include "BASe\MEMory\MEM.h"
#include <d3dx9math.h>
#include <assert.h>
#include <stdio.h>

/////////////
int	  DX9_GAU_g_iDrawGauges = 0;				// modifiable via console
float DX9_GAU_g_fFrameRateAlarmLimit = 20.0f;	// modifiable via console
float DX9_GAU_g_fFrameRateTarget = 60.0f;		// modifiable via console
int	  DX9_GAU_g_iDrawFrameRate = 0;				// modifiable via console

typedef struct 
{
	char Name[32];
	unsigned FilterType;
}
CounterDescriptor;

#define Counter_FilterEngine 1
#define Counter_FilterDisplay 2
#define Counter_FilterGlobal 4
#define Counter_FilterALL 7

#define TOP_BORDER		50.0f
#define LATERAL_BORDER	20.0f
#define TEXT_COL_WIDTH	50.0f
#define GAUGE_HEIGHT	10.0f

static CounterDescriptor gs_CounterDescription[] =
{
    "MisEng",	Counter_FilterEngine, // tout ce qui est dans engcall et qui n'est pas rasterise
    "Lig",		Counter_FilterDisplay,
    "SKN",		Counter_FilterDisplay,
    "SDW",		Counter_FilterDisplay,
    "DRW",		Counter_FilterDisplay,
//5    
    "ANIr",		Counter_FilterDisplay,
    "SRS",		Counter_FilterDisplay,
    "GFX",		Counter_FilterALL,
    "CVC",		Counter_FilterDisplay,
    "CSpec",	Counter_FilterDisplay,
//10 
    "CDAlp",	Counter_FilterEngine,
    "ANI",		Counter_FilterEngine,	// anim
    "MAT D",	Counter_FilterDisplay,
    "Zlst S",	Counter_FilterDisplay,
    "AI",		Counter_FilterEngine,	// IA
//15
    "INO",		Counter_FilterEngine,	// pad
    "Col",		Counter_FilterEngine,	// colision
    "Rec",		Counter_FilterEngine,	// recalage
    "Eng",		Counter_FilterEngine|Counter_FilterGlobal,	// engine = col + rec + AI + DivEng
    "Dis",		Counter_FilterDisplay|Counter_FilterGlobal,	// display
//20
    "TxProc",	Counter_FilterDisplay, // texture procedural
    "Flip",		Counter_FilterDisplay,// flip (normalement raster jamais active)
    "MRM",		Counter_FilterDisplay,
    "1-DRW",	Counter_FilterDisplay,
    "GO-DRW",	Counter_FilterDisplay,
//25
    "SND",		Counter_FilterEngine, // snd
    "TAM",		Counter_FilterEngine, // tabmanager in engcall
    "EngRei",	Counter_FilterEngine, //eng reinit in engcall
    "WorAct",	Counter_FilterEngine, //woractivate
    "EOT",		Counter_FilterEngine,
//30
    "SnP",		Counter_FilterEngine,
    "MdF",		Counter_FilterEngine, //gen Mdf
    "EVE",		Counter_FilterEngine,
    "HIE",		Counter_FilterEngine,
    "DYN",		Counter_FilterEngine,
//35
    "VIEW",		Counter_FilterALL,
    "Visi",		Counter_FilterALL,
    "Lens F",	Counter_FilterDisplay, // texture procedural
    "FIFO",		Counter_FilterDisplay|Counter_FilterGlobal, // texture procedural
    "AI-SND",	Counter_FilterEngine, // sub part of IA : sound fonctions
//40
    "IOP",		Counter_FilterEngine, //special part for IOP com
    "VIDEO",	Counter_FilterDisplay, //special part for IOP com
    "SPG",		Counter_FilterDisplay, //Sprite generator
	"BigVB",	Counter_FilterDisplay, // Big Vertex Buffer usage
	"GoodVB",	Counter_FilterDisplay, // Good Vertex Buffer usage
//45
	"IndP",		Counter_FilterDisplay, // All element indexed primitive
	"Sprite",	Counter_FilterDisplay,
	"Raster",	Counter_FilterGlobal, // temps mis pour l'affichage de ces rasters !
	"Water",	Counter_FilterDisplay,
	"?????",	Counter_FilterDisplay,

//50
	"?????",	Counter_FilterDisplay,
	"?????",	Counter_FilterDisplay,
	"Morph",	Counter_FilterDisplay,

	"Total",	Counter_FilterALL,// total : toujours en dernier !
};

#define NUM_COUNTERS (sizeof(gs_CounterDescription) / sizeof(CounterDescriptor))

LARGE_INTEGER DX9_PerfCounter[NUM_COUNTERS];
LARGE_INTEGER DX9_PerfCounterStart[NUM_COUNTERS];

typedef struct ColoredVertex
{
    D3DXVECTOR3 v;
	DWORD		c;
} ColoredVertex;

static ColoredVertex *s_pVertexBufferBase, *s_pVertexBuffer;

static void DrawQueuedRectangles(void)
{
	assert(s_pVertexBuffer);

	// draw all stored triangles
	Dx9_DrawPrimitiveUP(
		D3DPT_TRIANGLELIST, (s_pVertexBuffer - s_pVertexBufferBase) / 3,
		s_pVertexBufferBase, sizeof(ColoredVertex));

	// reset vertex buffer
	s_pVertexBuffer = s_pVertexBufferBase;
}

static void DrawRectangle(float x, float y, float w, float h, DWORD c, bool immediately)
{
	if(s_pVertexBuffer == 0)
	{
		s_pVertexBufferBase = MEM_p_Alloc(1024 * sizeof(ColoredVertex));
		ZeroMemory(s_pVertexBufferBase, 1024 * sizeof(ColoredVertex));
		s_pVertexBuffer = s_pVertexBufferBase;
	}

	s_pVertexBuffer->v.x = x;
	s_pVertexBuffer->v.y = y;
	s_pVertexBuffer->c   = c;
	++s_pVertexBuffer;
	s_pVertexBuffer->v.x = x + w;
	s_pVertexBuffer->v.y = y;
	s_pVertexBuffer->c   = c;
	++s_pVertexBuffer;
	s_pVertexBuffer->v.x = x;
	s_pVertexBuffer->v.y = y + h;
	s_pVertexBuffer->c   = c;
	++s_pVertexBuffer;
	s_pVertexBuffer->v.x = x;
	s_pVertexBuffer->v.y = y + h;
	s_pVertexBuffer->c   = c;
	++s_pVertexBuffer;
	s_pVertexBuffer->v.x = x + w;
	s_pVertexBuffer->v.y = y;
	s_pVertexBuffer->c   = c;
	++s_pVertexBuffer;
	s_pVertexBuffer->v.x = x + w;
	s_pVertexBuffer->v.y = y + h;
	s_pVertexBuffer->c   = c;
	++s_pVertexBuffer;

	if(immediately)
		DrawQueuedRectangles();
}

static void DrawGauge(const char* descr, float numFrames, float y, unsigned color, float viewportWidth)
{
	float x, frameWidth;
	char value[32];

	sprintf(value, "%5.3f", numFrames);
	Dx9_DrawText(descr, (int)LATERAL_BORDER, (int)y, 0xffffffff);
	Dx9_DrawText(value, (int)(viewportWidth - TEXT_COL_WIDTH), (int)y, 0xffffffff);

	if (numFrames > 4.0f)
		numFrames = 4.0f;

	x = LATERAL_BORDER + TEXT_COL_WIDTH;
	frameWidth = (viewportWidth - (LATERAL_BORDER + TEXT_COL_WIDTH) * 2.0f) * 64.0f / 85.0f;

	while(numFrames > 0.0f)
	{
		float w = (numFrames < 1.0f ? numFrames : 1.0f) * frameWidth;

		DrawRectangle(x, y, w, GAUGE_HEIGHT, color, false);

		x += w;
		numFrames -= 1.0f;
		frameWidth /= 4.0f;
		color ^= 0xffff;
	}
}

static double s_smoothedCounter[NUM_COUNTERS];

void Dx9_DrawPerformanceGauges(void)
{
	int i, numCounters;
	float y, frameScale;
	unsigned Filter = 0xffffffff;
	DWORD Color;
	int sortedIdx[NUM_COUNTERS];
	D3DMATRIX transf;
	float viewportWidth  = (float)gDx9SpecificData.Viewport.Width;
	float viewportHeight = (float)gDx9SpecificData.Viewport.Height;
	LARGE_INTEGER freq;

	if(!DX9_GAU_g_iDrawGauges)
		return;

	Dx9_EndPerfCounter(NUM_COUNTERS-1);	// total
    Dx9_EndPerfCounter(19);				// display
    Dx9_BeginPerfCounter(47);			// rasters

	Dx9_SaveState(D3DSBT_ALL);
	D3DXMatrixIdentity(&transf);
	Dx9_SetViewMatrixX(&transf);
	D3DXMatrixOrthoOffCenterRH(&transf, 0, viewportWidth, viewportHeight, 0, -1, 1);
	Dx9_SetProjectionMatrixX(&transf);

	// draw debug shadow
	//if (g_iDDShadow) 
	//	DrawOneShadow(Device,g_iNumShadow);

//	Filter = CounterFilter;
//	if (CounterFilter == 0) Filter = 0xffffffff;
//	if (CounterFilter == 3) Filter = Counter_FilterGlobal;

	// get the total number of frames
	QueryPerformanceFrequency(&freq);

	if (DX9_PerfCounter[NUM_COUNTERS-1].QuadPart * DX9_GAU_g_fFrameRateAlarmLimit > freq.QuadPart)
	{
		// draw red bar alarm
		DrawRectangle(0.0f, 0.0f, viewportWidth, 40.0f, 0xffff0000, false);
	}

	// compute smoothed values for sorting
	for (i=0; i<NUM_COUNTERS; i++)
		s_smoothedCounter[i] = s_smoothedCounter[i] * 0.95 + DX9_PerfCounter[i].QuadPart * 0.05;

	// perform a bubble-sort cycle using the smoothed values
	for (i = 0 ; i < NUM_COUNTERS ; i++) 
		sortedIdx[i] = i;

	for (i = 0 ; i < NUM_COUNTERS - 1 ; i++) 
	{
		int j = i + 1;
		for (; j < NUM_COUNTERS; j++) 
		{
			if (s_smoothedCounter[sortedIdx[i]] < s_smoothedCounter[sortedIdx[j]])
			{
				unsigned tmp = sortedIdx[i];
				sortedIdx[i] = sortedIdx[j];
				sortedIdx[j] = tmp;
			}
		}
	}

	// begin scene
    Dx9_RS_SetVertexShader(0);
	Dx9_RS_SetVertexFormat(D3DFVF_XYZ | D3DFVF_DIFFUSE, false);
	Dx9_RS_ZEnable(D3DZB_FALSE);
	Dx9_RS_CullMode(D3DCULL_NONE);
	Dx9_RS_Lighting(FALSE);
	Dx9_RS_ColorWriteEnable(0xf);
	Dx9_RS_AlphaBlendEnable(TRUE);
	Dx9_RS_SrcBlend(D3DBLEND_SRCALPHA);
	Dx9_RS_DestBlend(D3DBLEND_INVSRCALPHA);
	Dx9_TSS_UseTexture(0, -1);
	Dx9_TSS_UseTexture(1, -1);
	Dx9_TSS_UseTexture(2, -1);
	Dx9_TSS_UseTexture(3, -1);
	Dx9_BeginScene();

	y = TOP_BORDER + 2 * GAUGE_HEIGHT;
	frameScale = DX9_GAU_g_fFrameRateTarget / freq.QuadPart;
	for (i = 0; i < NUM_COUNTERS; i++)
	{
		float gaugeWidth = DX9_PerfCounter[sortedIdx[i]].QuadPart * frameScale;
		if (gs_CounterDescription[sortedIdx[i]].FilterType & Filter && gaugeWidth > 0.01)
		{
			y += 1.5f * GAUGE_HEIGHT;
			if (y > viewportHeight)
				break;
		}
	}

	if(DX9_GAU_g_iDrawGauges == 1)
		numCounters = i;
	else
		numCounters = 1;

	// the shadow must be drawn before all the text
	DrawRectangle(0, TOP_BORDER, viewportWidth, numCounters * (1.5f * GAUGE_HEIGHT), 0x80000000, true);

	Dx9_DrawTextBegin(); // optimization only

	y = TOP_BORDER + 2 * GAUGE_HEIGHT;
	for (i = 0; i < numCounters; i++)
	{
		float gaugeWidth = DX9_PerfCounter[sortedIdx[i]].QuadPart * frameScale;
		if (gs_CounterDescription[sortedIdx[i]].FilterType & Filter && gaugeWidth > 0.01)
		{
			Color = 0xf0808020;
			if ((sortedIdx[i] + 1) & 1) Color |= 0x40;
			if ((sortedIdx[i] + 1) & 2) Color |= 0x4000;
			if ((sortedIdx[i] + 1) & 4) Color |= 0x400000;

			DrawGauge(gs_CounterDescription[sortedIdx[i]].Name, gaugeWidth, y, Color, viewportWidth);

			y += 1.5f * GAUGE_HEIGHT;
			if (y > viewportHeight)
				break;
		}
	}
	// draw frame rate
	DrawGauge("Frame Rate",
		(float)freq.QuadPart / (float)DX9_PerfCounter[NUM_COUNTERS-1].QuadPart,
		TOP_BORDER, 0x00ffffff, viewportWidth);

	Dx9_DrawTextEnd(); // optimization only

	DrawQueuedRectangles();

	// end scene
	Dx9_EndScene();
	Dx9_RestoreState();

	// reset counters
	memset(DX9_PerfCounter, 0, sizeof(DX9_PerfCounter));

    Dx9_EndPerfCounter(47);					// rasters
    Dx9_BeginPerfCounter(19);				// display
    Dx9_BeginPerfCounter(NUM_COUNTERS-1);	// total
}