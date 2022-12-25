#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>
#include "Gsp.h"
#include "GSPinit.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "MATerial/MATstruct.h"
#include "Gsp_Bench.h"
#include "ENGvars.h"
#include "PSX2debug.h"
#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif

#define MAX_NumSPR 120
static u_int p_CurrentBufferPointerSaved;

void GSP_DrawSpriteBegin()
{
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	if (GspGlobal_ACCESS(bSRSMustBeUodate)) 
	{
		if(GspGlobal_ACCESS(ulNewBMD) & MAT_Cul_Flag_UseLocalAlpha)
			GspGlobal_ACCESS(ulColorOr) = 0xff000000;	
		else
			GspGlobal_ACCESS(ulColorOr) = 0;//*/
	}//*/
	p_CurrentBufferPointerSaved = gs_st_Globals.p_CurrentBufferPointer;
	Gsp_SetDrawBuffer_XYZ_COLORS(&gs_st_Globals , MAX_NumSPR , (u_int *)1);
	GspGlobal_ACCESS(p_Last_COLOR_pointer)[0] = 0;
}
void GSP_DrawSprites()
{
	GspGifTag *p_DmaTG;
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;
	
	

	/* Save GIF Tag in point 0*/
	p_DmaTG = (GspGifTag*)p_BIG->p_Last_XYZW_pointer;
	Gsp_M_SetGifTag(p_DmaTG, 0 , 1 , 1 , 6  , GSP_GIF_FLG_PACKED , 6 , 0x5252A1); /* set VU1 GIF TAG for sprites */
	/* NLOOP is not set here because we don't kow how many sprite will be Z culled*/
	
#ifdef GSP_PS2_BENCH
	p_BIG->NSPRITES += p_BIG->p_Last_COLOR_pointer[0];
#endif	
	
	p_BIG->ulNextVU1Jump= 0x1500000C; /* Call render sprites */
	Gsp_SetDrawBuffer_CMD1(p_BIG , p_BIG->p_Last_COLOR_pointer[0]);
	p_BIG->ulNextVU1Jump = 0x15000000; /* Call 0 */
}

void GSP_DrawSpriteEnd()
{
	if (GspGlobal_ACCESS(p_Last_COLOR_pointer)[0]) /* Their is sprites */
	{
		GspGlobal_ACCESS(p_Last_XYZW_pointer)[1].x = GIGSCALE0;
		GspGlobal_ACCESS(p_Last_XYZW_pointer)[1].y = GspGlobal_ACCESS(YCorrectionCoef) * GIGSCALE0;
		GSP_DrawSprites();
	} else
	{
		//Must Restore Buffer'
		gs_st_Globals.p_CurrentBufferPointer = p_CurrentBufferPointerSaved;
	}
}

void GSP_DrawSpriteX(SOFT_tdst_AVertex	*_pst_Point , u_int Color)
{
	register ULONG *Index;
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;
	Index = p_BIG->p_Last_COLOR_pointer;
	p_BIG->p_Last_COLOR_pointer[(*Index)+2] = Color;
	((u_long128*)p_BIG->p_Last_XYZW_pointer)[(*Index)+2] = *((u_long128 *)_pst_Point);
	
	(*Index)++;
	/* Test if buffer is full */
	if (*Index == MAX_NumSPR - 2)
	{
		p_BIG->p_Last_XYZW_pointer[1].x = GIGSCALE0;
		p_BIG->p_Last_XYZW_pointer[1].y = p_BIG->YCorrectionCoef * GIGSCALE0;
		GSP_DrawSprites();
		p_CurrentBufferPointerSaved = gs_st_Globals.p_CurrentBufferPointer;
		Gsp_SetDrawBuffer_XYZ_COLORS(p_BIG , MAX_NumSPR , (u_int *)1);
		p_BIG->p_Last_COLOR_pointer[0] = 0;
	}
}
asm void GSP_CopySprites4(GEO_tdst_IndexedSprite *p_CurrentSprite,u_long128 *p_PointDest ,u_int	*p_ColorDest , u_long128 *p_PointSource ,u_int	color , u_int Number)
{
	/* PARAMS : 
		a0 = p_CurrentSprite 
		a1 = p_PointDest ;
		A2 = p_ColorDest
		a3 = p_PointSource p_PointSource p_PointSource p_PointSource
		t0 = color
		t1 = Number
		t2 = 4 sprites
		t3 = *P1
		t4 = *P2
		t5 = *P3
		t6 = *P4
		t7 = 
		t8 = pand 0x0000ffff0000ffff0000ffff0000ffff
		t9 = 
	*/
		.set noreorder
		addiu	t8,$0,-1
		srl		t8,t8,16
		ppacw 	t8,t8,t8
		ppacw 	t8,t8,t8
		ppacw 	a3,a3,a3
		ppacw 	a3,a3,a3
		ppacw 	t0,t0,t0
		b		LOOP_E
		ppacw 	t0,t0,t0
LOOP:	lq		t2,0(a0)
		sd		t0,0(a2)
		pand	t3,t2,t8
		qmtc2	t2,$vf15
		psllw	t3,t3,4
		sd		t0,8(a2)
		paddw	t3,t3,a3
		addiu	a0,a0,0x10		 
		dsrl32	t4,t3,0
		pcpyud  t5,t3,t3
		dsrl32	t6,t5,0
		lqc2	$vf10,0(t3)
		lqc2	$vf12,0(t5)
		lqc2	$vf11,0(t4)
		vmulx.w	$vf10,$vf00,$vf15x
		lqc2	$vf13,0(t6)
		vmuly.w	$vf11,$vf00,$vf15y
		sqc2	$vf10,0x00(a1)
		vmulz.w	$vf12,$vf00,$vf15z
		sqc2	$vf11,0x10(a1)
		vmulw.w	$vf13,$vf00,$vf15w
		sqc2	$vf12,0x20(a1)
		sqc2	$vf13,0x30(a1)
		addiu	a1,a1,0x40
		addiu	a2,a2,0x10
LOOP_E:	bne		t1,$0,LOOP
		addiu 	t1,t1,-1
		jr ra
		nop
		.set reorder
}

asm void GSP_CopySpritesColors4(GEO_tdst_IndexedSprite *p_CurrentSprite,u_long128 *p_PointDest ,u_int	*p_ColorDest , u_long128 *p_PointSource ,u_int	*pcolor , u_int Number , u_int	OR , u_int XOR)
{
	/* PARAMS : 
		a0 = p_CurrentSprite 
		a1 = p_PointDest ;
		A2 = p_ColorDest
		a3 = p_PointSource 	p_PointSource 	p_PointSource 	p_PointSource
		t0 = p_color		p_color			p_color			p_color
		t1 = Number
		t2 = 4 sprites
		t3 = *P1
		t4 = *P2
		t5 = *P3
		t6 = *C1 	*C2		*C3		*C4
		t7 = pand 0x0000ffff0000ffff0000ffff0000ffff
		t8 = XOR	XOR		XOR		XOR
		t9 = OR		OR		OR		OR
	*/
		.set noreorder
		addiu	t7,$0,-1
		srl		t7,t7,16
		ppacw 	t7,t7,t7
		ppacw 	t7,t7,t7
		ppacw 	t9,t2,t2
		ppacw 	t9,t9,t9
		ppacw 	t8,t3,t3
		ppacw 	t8,t8,t8
		ppacw 	a3,a3,a3
		ppacw 	a3,a3,a3
		ppacw 	t0,t0,t0
		b		LOOP_E
		ppacw 	t0,t0,t0
LOOP:	lq		t2,0(a0)
		pand	t3,t2,t7
		qmtc2	t2,$vf15
		psllw	t3,t3,4
		paddw	t3,t3,a3
		addiu	a0,a0,0x10		 
		dsrl32	t4,t3,0
		pcpyud  t5,t3,t3
		dsrl32	t6,t5,0
		lq		t3,0(t3)
		lq		t4,0(t4)
		lq		t5,0(t5)
		lq		t6,0(t6)
		qmtc2	t3,$vf10
		qmtc2	t4,$vf11
		pextuw 	t3,t5,t3
		qmtc2	t5,$vf12
		vmulx.w	$vf10,$vf00,$vf15x
		pextuw	t5,t6,t4
		pextuw	t3,t5,t3
		qmtc2	t6,$vf13
		vmuly.w	$vf11,$vf00,$vf15y
		sqc2	$vf10,0x00(a1)
		vmulz.w	$vf12,$vf00,$vf15z
		por		t3,t3,t9
		pxor	t3,t3,t8
		sqc2	$vf11,0x10(a1)
		vmulw.w	$vf13,$vf00,$vf15w
		psrlw	t3,t3,1
		sd		t3,0(a2)
		sqc2	$vf12,0x20(a1)
		sqc2	$vf13,0x30(a1)
		pcpyud  t3,t3,t3
		sd		t3,0x8(a2)
		addiu	a1,a1,0x40
		addiu	a2,a2,0x10
LOOP_E:	bne		t1,$0,LOOP
		addiu 	t1,t1,-1
		jr ra
		nop
		.set reorder
}
	
/* */
void GSP_CopySprites(GEO_tdst_IndexedSprite *p_CurrentSprite,u_long128 *p_PointDest ,u_int	*p_ColorDest , u_long128 *p_PointSource ,u_int	Color , u_int Number)
{
	ULONG ulNumberFast;
	ulNumberFast = Number >> 2;
	GSP_CopySprites4(p_CurrentSprite,p_PointDest ,p_ColorDest , p_PointSource ,Color , ulNumberFast);
	ulNumberFast <<= 2;
	p_CurrentSprite += ulNumberFast;
	p_PointDest += ulNumberFast;
	p_ColorDest += ulNumberFast;
	Number &= 3;
	while (Number--)
	{
		*p_PointDest = ((u_long128 *)p_PointSource)[p_CurrentSprite->auw_Index];
		((u_int *)p_PointDest)[3] = *(u_int *)p_CurrentSprite;
		*(p_ColorDest++)		= Color;		
		p_PointDest++;
		p_CurrentSprite++;
	}
}

void GSP_CopySpritesColors(GEO_tdst_IndexedSprite *p_CurrentSprite,u_long128 *p_PointDest ,u_int	*p_ColorDest , u_long128 *p_PointSource ,u_int	*p_ColorSource , u_int Number)
{
	register ULONG ulNumberFast;
	register ULONG ulColorOr,ulColorXOr;
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;
	ulColorOr = p_BIG->ulColorOr | 0x01010101;
	ulColorXOr = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ulColorXor | 0x01010101;
	ulNumberFast = Number >> 2;
	GSP_CopySpritesColors4(p_CurrentSprite,p_PointDest ,p_ColorDest , p_PointSource ,p_ColorSource, ulNumberFast , ulColorOr , ulColorXOr);
	ulNumberFast <<= 2;
	p_CurrentSprite += ulNumberFast;
	p_PointDest += ulNumberFast;
	p_ColorDest += ulNumberFast;
	Number &= 3;//*/
	while (Number--)
	{
		*p_PointDest = ((u_long128 *)p_PointSource)[p_CurrentSprite->auw_Index];
		*(p_ColorDest++)		= ((((SOFT_tdst_AVertex *)p_PointDest)->c | ulColorOr) ^ ulColorXOr) >> 1;
		((u_int *)p_PointDest)[3] = *(u_int *)p_CurrentSprite;
		p_PointDest++;
		p_CurrentSprite++;
	}
}
void L_memcpy16(u_long128 *p_dest , u_long128 *p_src , ULONG ulNUmber)
{
	u_long128 tmp0,tmp1,tmp2,tmp3,tmpX;
	asm __volatile__ {"
	.set noreorder
		andi    tmpX,ulNUmber,3
		srl		ulNUmber,ulNUmber,2
		beq ulNUmber,zero , end
		nop
loop:
		addiu	ulNUmber,ulNUmber,-1
		lq		tmp0,0x00(p_src)
		lq		tmp1,0x10(p_src)
		lq		tmp2,0x20(p_src)
		lq		tmp3,0x30(p_src)
		sq		tmp0,0x00(p_dest)
		sq		tmp1,0x10(p_dest)
		sq		tmp2,0x20(p_dest)
		sq		tmp3,0x30(p_dest)
		addiu	p_src,p_src,0x40
		bne 	ulNUmber,zero , loop
		addiu	p_dest,p_dest,0x40
end:		
		beq tmpX,zero , end2
loop2:
		addiu	tmpX,tmpX,-1
		lq		tmp0,0x00(p_src)
		sq		tmp0,0x00(p_dest)
		addiu	p_src,p_src,0x10
		bne 	tmpX,zero , loop2
		addiu	p_dest,p_dest,0x10
end2:		
	.set reorder
	"} 
}

void L_memcpy4(u_long128 *p_dest , u_long128 *p_src , ULONG ulNUmber)
{
	u_long128 tmp0,tmp1,tmp2,tmp3,tmpX;
	asm __volatile__ {"
	.set noreorder
		andi    tmpX,ulNUmber,3
		srl		ulNUmber,ulNUmber,2
		beq ulNUmber,zero , end
		nop
loop:
		addiu	ulNUmber,ulNUmber,-1
		ld		tmp0,0x00(p_src)
		ld		tmp2,0x08(p_src)
		sd		tmp0,0x00(p_dest)
		sd		tmp2,0x08(p_dest)
		addiu	p_src,p_src,0x10
		bne 	ulNUmber,zero , loop
		addiu	p_dest,p_dest,0x10
end:		
		beq tmpX,zero , end2
loop2:
		addiu	tmpX,tmpX,-1
		lw		tmp0,0x00(p_src)
		sw		tmp0,0x00(p_dest)
		addiu	p_src,p_src,0x04
		bne 	tmpX,zero , loop2
		addiu	p_dest,p_dest,0x04
end2:		
	.set reorder
	"} 
}

void L_memcpy4_shift(u_long128 *p_dest , u_long128 *p_src , ULONG ulNUmber)
{
	u_long128 tmp0,tmp1,tmp2,tmp3,tmpX,tmpAnd,tmpAnd2;
	((u32 *)&tmpAnd2)[0] = ((u32 *)&tmpAnd2)[1] = ((u32 *)&tmpAnd2)[2] = ((u32 *)&tmpAnd2)[3] = 0xfefefefe;
	tmpAnd = tmpAnd2;
	
	asm __volatile__ {"
	.set noreorder
		andi    tmpX,ulNUmber,3
		srl		ulNUmber,ulNUmber,2
		beq ulNUmber,zero , end
		nop
loop:
		addiu	ulNUmber,ulNUmber,-1
		ld		tmp0,0x00(p_src)
		ld		tmp2,0x08(p_src)
		pand	tmp0,tmp0,tmpAnd
		pand	tmp2,tmp2,tmpAnd
		psrlw	tmp0,tmp0,1
		psrlw	tmp2,tmp2,1
		sd		tmp0,0x00(p_dest)
		sd		tmp2,0x08(p_dest)
		addiu	p_src,p_src,0x10
		bne 	ulNUmber,zero , loop
		addiu	p_dest,p_dest,0x10
end:		
		beq tmpX,zero , end2
loop2:
		addiu	tmpX,tmpX,-1
		lw		tmp0,0x00(p_src)
		pand	tmp0,tmp0,tmpAnd
		psrlw	tmp0,tmp0,1
		sw		tmp0,0x00(p_dest)
		addiu	p_src,p_src,0x04
		bne 	tmpX,zero , loop2
		addiu	p_dest,p_dest,0x04
end2:		
	.set reorder
	"} 
}
void L_memset4(u_long128 *p_dest , ULONG ulColor  , ULONG ulNUmber)
{
	u_long128 tmp0,tmp1,tmp2,tmp3,tmpX;
	asm __volatile__ {"
	.set noreorder
		andi    tmpX,ulNUmber,3
		ppacw 	ulColor,ulColor,ulColor
		ppacw 	ulColor,ulColor,ulColor
		srl		ulNUmber,ulNUmber,2
		beq ulNUmber,zero , end
		nop
loop: 	nop
		addiu	ulNUmber,ulNUmber,-1
		sd		ulColor,0x00(p_dest)
		sd		ulColor,0x08(p_dest)
		bne 	ulNUmber,zero , loop
		addiu	p_dest,p_dest,0x10
end:		
		beq tmpX,zero , end2
loop2:	nop
		nop
		addiu	tmpX,tmpX,-1
		sw		ulColor,0x00(p_dest)
		bne 	tmpX,zero , loop2
		addiu	p_dest,p_dest,0x04
end2:		
	.set reorder
	"} 
}


LONG GSP_l_DrawElementIndexedSprite(GEO_tdst_ElementIndexedSprite	*_pst_Element,GEO_Vertex*_pst_Point,ULONG NumberOfPoints)
{
	register ULONG SrpiteCounter , SrpiteNumber;
	GEO_tdst_IndexedSprite *p_CurrentSprite;
	ULONG *p_Colors;
	float fFactorX,fFactorY ; 
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);

	if (p_BIG->bSRSMustBeUodate) 
	{
		if(p_BIG->ulNewBMD & MAT_Cul_Flag_UseLocalAlpha)
			p_BIG->ulColorOr = 0xff000000;	
		else
			p_BIG->ulColorOr = 0;//*/
	}//*/
	
/*	GspGlobal_ACCESS(NSPRITES) += _pst_Element->l_NbSprites;
	return 0;//*/
	p_CurrentSprite = _pst_Element->dst_Sprite;
	SrpiteCounter = _pst_Element->l_NbSprites;
//	if (p_BIG->bSRSMustBeUodate) GSP_SetTextureBlending12(&gs_st_Globals);//*/
	fFactorX = GIGSCALE0 * _pst_Element->fGlobalSize * _pst_Element->fGlobalRatio;
	fFactorY = p_BIG->YCorrectionCoef * GIGSCALE0 * _pst_Element->fGlobalSize;
	p_Colors = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	if (p_Colors)
	{
		if (p_CurrentSprite->auw_Index == 0xC0DE) // GFX Signal of mega-flux
		{
			_pst_Point += p_CurrentSprite[1].auw_Index;
			p_Colors +=  p_CurrentSprite[1].auw_Index;
			while (SrpiteCounter)
			{
				SrpiteNumber = lMin(MAX_NumSPR - 2 , SrpiteCounter);
				Gsp_SetDrawBuffer_XYZ_COLORS(p_BIG , SrpiteNumber + 2 , (u_int *)1);
				L_memcpy16((u_long128 *)p_BIG->p_Last_XYZW_pointer + 2 , _pst_Point , SrpiteNumber);
				L_memcpy4_shift(p_BIG->p_Last_COLOR_pointer + 2 , p_Colors ,SrpiteNumber);
				//GSP_CopySpritesColors(p_CurrentSprite,(u_long128 *)p_BIG->MDB)-> p_Last_XYZW_pointer + 2 ,p_BIG->MDB)-> p_Last_COLOR_pointer + 2 , (u_long128 *)_pst_Point ,p_Colors , SrpiteNumber);
				p_BIG->p_Last_COLOR_pointer[0] = SrpiteNumber;
				p_BIG->p_Last_XYZW_pointer[1].x = fFactorX;
				p_BIG->p_Last_XYZW_pointer[1].y = fFactorY;
				GSP_DrawSprites();
				p_CurrentSprite += SrpiteNumber;
				_pst_Point += SrpiteNumber;
				p_Colors += SrpiteNumber;
				SrpiteCounter -= SrpiteNumber;
			}
		}
		else//*/
		while (SrpiteCounter)
		{
			SrpiteNumber = lMin(MAX_NumSPR - 2 , SrpiteCounter);
			Gsp_SetDrawBuffer_XYZ_COLORS(p_BIG , SrpiteNumber + 2 , (u_int *)1);
			GSP_CopySpritesColors(p_CurrentSprite,(u_long128 *)p_BIG->p_Last_XYZW_pointer + 2 ,p_BIG->p_Last_COLOR_pointer + 2 , (u_long128 *)_pst_Point ,p_Colors , SrpiteNumber);
			p_BIG->p_Last_COLOR_pointer[0] = SrpiteNumber;
			p_BIG->p_Last_XYZW_pointer[1].x = fFactorX;
			p_BIG->p_Last_XYZW_pointer[1].y = fFactorY;
			GSP_DrawSprites();
			p_CurrentSprite += SrpiteNumber;
			SrpiteCounter -= SrpiteNumber;
		}
	} else
	{
		if (p_CurrentSprite->auw_Index == 0xC0DE) // GFX Signal of mega-flux
		{
			ULONG ulColorOr,ulColorXOr , ulColor;
			ulColorOr = p_BIG->ulColorOr | 0x01010101;
			ulColorXOr = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ulColorXor | 0x01010101;
			ulColor = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ul_Ambient;
			ulColor = ((ulColor | ulColorOr) ^ ulColorXOr) >> 1;
			_pst_Point += p_CurrentSprite[1].auw_Index;
			while (SrpiteCounter)
			{
				SrpiteNumber = lMin(MAX_NumSPR - 2 , SrpiteCounter);
				Gsp_SetDrawBuffer_XYZ_COLORS(p_BIG , SrpiteNumber + 2 , (u_int *)1);
				L_memcpy16((u_long128 *)p_BIG->p_Last_XYZW_pointer + 2 , _pst_Point , SrpiteNumber);
				L_memset4(p_BIG->p_Last_COLOR_pointer + 2 , ulColor ,SrpiteNumber);
				p_BIG->p_Last_COLOR_pointer[0] = SrpiteNumber;
				p_BIG->p_Last_XYZW_pointer[1].x = fFactorX;
				p_BIG->p_Last_XYZW_pointer[1].y = fFactorY;
				GSP_DrawSprites();
				p_CurrentSprite += SrpiteNumber;
				_pst_Point += SrpiteNumber;
				p_Colors += SrpiteNumber;
				SrpiteCounter -= SrpiteNumber;
			}
		}
		else//*/
		{
			ULONG ulColorOr,ulColorXOr , ulColor;
			ulColorOr = p_BIG->ulColorOr | 0x01010101;
			ulColorXOr = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ulColorXor | 0x01010101;
			ulColor = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ul_Ambient;
			ulColor = ((ulColor | ulColorOr) ^ ulColorXOr) >> 1;
			while (SrpiteCounter)
			{
				SrpiteNumber = lMin(MAX_NumSPR - 2 , SrpiteCounter);
				Gsp_SetDrawBuffer_XYZ_COLORS(p_BIG , SrpiteNumber + 2 , (u_int *)1);
				GSP_CopySprites(p_CurrentSprite,(u_long128 *)p_BIG->p_Last_XYZW_pointer + 2 ,p_BIG->p_Last_COLOR_pointer + 2 , (u_long128 *)_pst_Point ,ulColor , SrpiteNumber);
				p_BIG->p_Last_COLOR_pointer[0] = SrpiteNumber;
				p_BIG->p_Last_XYZW_pointer[1].x = fFactorX;
				p_BIG->p_Last_XYZW_pointer[1].y = fFactorY;
				GSP_DrawSprites();
				p_CurrentSprite += SrpiteNumber;
				SrpiteCounter -= SrpiteNumber;
			}
		}
	}//*/
	return 0;
}


#if defined PSX2_TARGET && defined __cplusplus
}
#endif
