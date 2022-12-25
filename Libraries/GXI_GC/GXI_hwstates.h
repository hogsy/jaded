#ifndef __GXI_HWSTATES_H__
#define __GXI_HWSTATES_H__

#define NB_TEXMAPID 9
#define NB_ATTR     256

typedef struct
{
	// GXSetZMode
	GXBool				bCompareEnable;
	GXCompare			funcCompare;
	GXBool				bUpdateEnable;

	// GXLoadTexObj
	GXTexObj*			pTexObj[NB_TEXMAPID];

	// GXSetVtxDesc
	GXAttrType			arAttrType[NB_ATTR];

	// GXSetAlphaCompare
	GXCompare			acCompare1;
	u8					acVal1;
	GXAlphaOp			acOp;
	GXCompare			acCompare2;
	u8					acVal2;
	
	// GXSetZCompLoc
	GXBool				zcompBeforeTex;
	
	
	// GXSetBlendMode
    GXBlendMode			bmType;
    GXBlendFactor		bmSrcFactor;
    GXBlendFactor		bmDstFactor;
    GXLogicOp			bmOp;

	// GXSetNumChans
	u8					u8NumChans;
	
	// GXSetNumTexGens
	u8					u8NumTexGens;

	// GXSetNumTevStages
	u8					u8NumTevStages;
	

	// GXSetTevOrder
    GXTevStageID		toStage;
    GXTexCoordID		toCoord;
    GXTexMapID			toMap;
    GXChannelID			toColor;
    
    // GXSetChanCtrl
    GXChannelID			chanID;
    GXBool				chanEnable;
    GXColorSrc			chanColAmbSrc;
    GXColorSrc			chanColMatSrc;
    GXLightID			chanLightMask;
    GXDiffuseFn			chanDiffFn;
    GXAttnFn			chanAttnFn;
    
    // GXSetTevOp
    GXTevStageID		opStage;
    GXTevMode 			opMode;

	// GXSetTevSwap
	GXTevSwapSel		tsRascSel[GX_MAX_TEVSTAGE];
	GXTevSwapSel		tsTexSel[GX_MAX_TEVSTAGE];
	
	// GXSetChanMatColor	
    GXChannelID   		scChanMat;
    GXColor       		scColorMat;
	
	// GXSetChanAmbColor	
    GXChannelID   		scChanAmb;
    GXColor       		scColorAmb;
	
	// GXSetCullMode
	GXCullMode 			cullMode;
	
	// FB Update
	GXBool				ColorUpdate;
	GXBool				AlphaUpdate;
	
	// GXSetArray
	void*				arBasePtr[NB_ATTR];
	u8					arStride[NB_ATTR];
	
	// GXSetCurrentMtx
	u32					uCurrentMtx;
	
	// mul mode ?
	BOOL 				bColorDiv2X;
	BOOL 				bAlphaDiv2X;

} GXI_HW_States;

extern GXI_HW_States g_GXI_HW_States;

#ifndef _FINAL_
extern BOOL g_bUseHWStates;
#endif

void GXI_HW_States_Init();

inline void GX_GXSetZMode(GXBool bCompareEnable, GXCompare funcCompare, GXBool bUpdateEnable)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if( (bCompareEnable != (GXBool)-1 && (bCompareEnable != g_GXI_HW_States.bCompareEnable)) ||
		(bUpdateEnable != (GXBool)-1 && (bUpdateEnable != g_GXI_HW_States.bUpdateEnable)) ||
		(funcCompare != (GXCompare)-1 && (funcCompare != g_GXI_HW_States.funcCompare)))
	{
		if(bCompareEnable != (GXBool)-1) g_GXI_HW_States.bCompareEnable = bCompareEnable;
		if(bUpdateEnable != (GXBool)-1) g_GXI_HW_States.bUpdateEnable = bUpdateEnable;
		if(funcCompare != (GXCompare)-1) g_GXI_HW_States.funcCompare = funcCompare;
		GXSetZMode(g_GXI_HW_States.bCompareEnable, g_GXI_HW_States.funcCompare, g_GXI_HW_States.bUpdateEnable);
	}
#ifndef	_FINAL_
	}
	else
	{
		if(bCompareEnable != (GXBool)-1) g_GXI_HW_States.bCompareEnable = bCompareEnable;
		if(bUpdateEnable != (GXBool)-1) g_GXI_HW_States.bUpdateEnable = bUpdateEnable;
		if(funcCompare != (GXCompare)-1) g_GXI_HW_States.funcCompare = funcCompare;
		GXSetZMode(g_GXI_HW_States.bCompareEnable, g_GXI_HW_States.funcCompare, g_GXI_HW_States.bUpdateEnable);
	}
#endif
	g_GXI_HW_States.uCurrentMtx = 0xFFFFFFFF;
}

inline void GX_GXSetBlendMode(GXBlendMode type, GXBlendFactor src_factor, GXBlendFactor dst_factor, GXLogicOp op )
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if(g_GXI_HW_States.bmType == type)
	{
		if(type == GX_BM_BLEND &&
		   g_GXI_HW_States.bmSrcFactor == src_factor &&
		   g_GXI_HW_States.bmDstFactor == dst_factor)
		   return;
		if(type == GX_BM_LOGIC &&
			g_GXI_HW_States.bmOp == op)
			return;
	}

	g_GXI_HW_States.bmType = type;
	g_GXI_HW_States.bmSrcFactor = src_factor;
	g_GXI_HW_States.bmDstFactor = dst_factor;
	g_GXI_HW_States.bmOp = op;
	GXSetBlendMode(type, src_factor, dst_factor, op);
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.bmType = type;
		g_GXI_HW_States.bmSrcFactor = src_factor;
		g_GXI_HW_States.bmDstFactor = dst_factor;
		g_GXI_HW_States.bmOp = op;
		GXSetBlendMode(type, src_factor, dst_factor, op);
	}
#endif
}

inline void GX_GXLoadTexObj(GXTexObj *obj, GXTexMapID id)
{
/*#ifndef _FINAL_
	if(0)
	{
#endif	
	if(g_GXI_HW_States.pTexObj[id] != obj)
	{
		g_GXI_HW_States.pTexObj[id] = obj;
		GXLoadTexObj(obj, id);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.pTexObj[id] = obj;
		GXLoadTexObj(obj, id);
	}
#endif*/
	GXLoadTexObj(obj, id);
}

inline void GX_GXSetVtxDesc(GXAttr attr, GXAttrType type)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if(g_GXI_HW_States.arAttrType[attr] != type)
	{
		g_GXI_HW_States.arAttrType[attr] = type;
		GXSetVtxDesc(attr, type);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.arAttrType[attr] = type;
		GXSetVtxDesc(attr, type);
	}
#endif
}


inline void GX_GXSetAlphaCompare(GXCompare _acCompare1, u8 _acVal1, GXAlphaOp _acOp, GXCompare _acCompare2, u8 _acVal2)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if( g_GXI_HW_States.acCompare1 != _acCompare1 ||
		g_GXI_HW_States.acVal1 != _acVal1 ||
		g_GXI_HW_States.acOp != _acOp ||
		g_GXI_HW_States.acCompare2 != _acCompare2 ||
		g_GXI_HW_States.acVal2 != _acVal2 )
	{
		g_GXI_HW_States.acCompare1 = _acCompare1;
		g_GXI_HW_States.acVal1 = _acVal1;
		g_GXI_HW_States.acOp = _acOp;
		g_GXI_HW_States.acCompare2 = _acCompare2;
		g_GXI_HW_States.acVal2 = _acVal2;
		
		GXSetAlphaCompare(_acCompare1, _acVal1, _acOp, _acCompare2, _acVal2);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.acCompare1 = _acCompare1;
		g_GXI_HW_States.acVal1 = _acVal1;
		g_GXI_HW_States.acOp = _acOp;
		g_GXI_HW_States.acCompare2 = _acCompare2;
		g_GXI_HW_States.acVal2 = _acVal2;
		
		GXSetAlphaCompare(_acCompare1, _acVal1, _acOp, _acCompare2, _acVal2);
	}
#endif
}


inline void GX_GXSetZCompLoc(GXBool _zcompBeforeTex)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if(g_GXI_HW_States.zcompBeforeTex != _zcompBeforeTex)
	{
		g_GXI_HW_States.zcompBeforeTex = _zcompBeforeTex;
		GXSetZCompLoc(_zcompBeforeTex);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.zcompBeforeTex = _zcompBeforeTex;
		GXSetZCompLoc(_zcompBeforeTex);
	}
#endif
}

	
inline void GX_GXSetNumChans(u8 nChans)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if(g_GXI_HW_States.u8NumChans != nChans)
	{
		g_GXI_HW_States.u8NumChans = nChans;
		GXSetNumChans(nChans);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.u8NumChans = nChans;
		GXSetNumChans(nChans);
	}
#endif
}

inline void GX_GXSetNumTexGens(u8 nGens)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if(g_GXI_HW_States.u8NumTexGens != nGens)
	{
		g_GXI_HW_States.u8NumTexGens = nGens;
		GXSetNumTexGens(nGens);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.u8NumTexGens = nGens;
		GXSetNumTexGens(nGens);
	}
#endif
}

inline void GX_GXSetNumTevStages(u8 nStages)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if(g_GXI_HW_States.u8NumTevStages != nStages)
	{
		g_GXI_HW_States.u8NumTevStages = nStages;
		GXSetNumTevStages(nStages);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.u8NumTevStages = nStages;
		GXSetNumTevStages(nStages);
	}
#endif
}

inline void GX_GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID color)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if( g_GXI_HW_States.toStage != stage ||
		g_GXI_HW_States.toCoord != coord ||
		g_GXI_HW_States.toMap   != map   ||
		g_GXI_HW_States.toColor != color )
	{
		g_GXI_HW_States.toStage = stage;
		g_GXI_HW_States.toCoord = coord;
		g_GXI_HW_States.toMap   = map;  
		g_GXI_HW_States.toColor = color;
		GXSetTevOrder(stage, coord, map, color);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.toStage = stage;
		g_GXI_HW_States.toCoord = coord;
		g_GXI_HW_States.toMap   = map;  
		g_GXI_HW_States.toColor = color;
		GXSetTevOrder(stage, coord, map, color);
	}
#endif
}

inline void GX_GXSetChanCtrl(GXChannelID chanID, GXBool chanEnable, GXColorSrc chanColAmbSrc, GXColorSrc chanColMatSrc, GXLightID chanLightMask, GXDiffuseFn chanDiffFn, GXAttnFn chanAttnFn)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if( g_GXI_HW_States.chanID != chanID ||
		g_GXI_HW_States.chanEnable != chanEnable ||
		g_GXI_HW_States.chanColAmbSrc != chanColAmbSrc ||
		g_GXI_HW_States.chanColMatSrc != chanColMatSrc ||
		g_GXI_HW_States.chanLightMask != chanLightMask ||
		g_GXI_HW_States.chanDiffFn != chanDiffFn ||
		g_GXI_HW_States.chanAttnFn != chanAttnFn )
	{
		
		g_GXI_HW_States.chanID = chanID;
		g_GXI_HW_States.chanEnable = chanEnable;
		g_GXI_HW_States.chanColAmbSrc = chanColAmbSrc;
		g_GXI_HW_States.chanColMatSrc = chanColMatSrc;
		g_GXI_HW_States.chanLightMask = chanLightMask;
		g_GXI_HW_States.chanDiffFn = chanDiffFn;
		g_GXI_HW_States.chanAttnFn = chanAttnFn;
		
		GXSetChanCtrl(chanID, chanEnable, chanColAmbSrc, chanColMatSrc, chanLightMask, chanDiffFn, chanAttnFn);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.chanID = chanID;
		g_GXI_HW_States.chanEnable = chanEnable;
		g_GXI_HW_States.chanColAmbSrc = chanColAmbSrc;
		g_GXI_HW_States.chanColMatSrc = chanColMatSrc;
		g_GXI_HW_States.chanLightMask = chanLightMask;
		g_GXI_HW_States.chanDiffFn = chanDiffFn;
		g_GXI_HW_States.chanAttnFn = chanAttnFn;
		GXSetChanCtrl(chanID, chanEnable, chanColAmbSrc, chanColMatSrc, chanLightMask, chanDiffFn, chanAttnFn);
	}
#endif
}

extern BOOL g_bTevDirty;
extern BOOL g_bColorDiv2X;
extern BOOL g_bAlphaDiv2X;

inline void GX_GXSetTevOpDirty() {  g_bTevDirty = TRUE; }

inline void GX_GXSetTevOp(GXTevStageID stage, GXTevMode mode)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if( g_bTevDirty 
		|| g_GXI_HW_States.opStage != stage
		|| g_GXI_HW_States.opMode  != mode 
		|| g_GXI_HW_States.bColorDiv2X  != g_bColorDiv2X 
		|| g_GXI_HW_States.bAlphaDiv2X  != g_bAlphaDiv2X 
		)
	{
		GXTevColorArg carg;
		GXTevAlphaArg aarg;
		
		g_bTevDirty = FALSE;
		g_GXI_HW_States.opStage = stage;
		g_GXI_HW_States.opMode = mode;
		g_GXI_HW_States.bColorDiv2X = g_bColorDiv2X;
		g_GXI_HW_States.bAlphaDiv2X = g_bAlphaDiv2X;
				
		carg = GX_CC_RASC;
	    aarg = GX_CA_RASA;

	    if (stage != GX_TEVSTAGE0) 
	    {
	        carg = GX_CC_CPREV;
	        aarg = GX_CA_APREV;
	    }
	        
    	// ColorRas or AlphaRas = Color or alpha rasterized from lighting and vertex color, with lighting equations.
		switch(mode)
		{
			// Used when texture and color are present.
			// Color = ColorTexture * ColorRas * scale
			// Alpha = AlphaTexture * AlphaRas
			case GX_MODULATE: 	GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, carg, GX_CC_ZERO);
								GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_TEXA, aarg, GX_CA_ZERO);
			break;
			
			// Used when no texture.
			// Color = ColorRas * scale
			// Alpha = AlphaRas
			case GX_PASSCLR:  	GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, carg);
								GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, aarg);
			break;
			
			// Used when no light and vertex color.
			// Color = ColorTexture * scale
			// Alpha = AlphaTexture
			case GX_REPLACE:	GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
						        GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
			break;
			
	        // Used in "sand storm"
			// Color = ((1.0 - AlphaTexture)*ColorRas + AlphaTexture*ColorTexture) ) * scale
			// Alpha = AlphaRas
			case GX_DECAL:      GXSetTevColorIn(stage, carg, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
        						GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, aarg);
	        break;	
	        
	        // Never used
			// Color = ((1.0 - ColorTexture)*ColorRas + ColorTexture) * scale
			// Alpha = AlphaTexture * AlphaRas
	        case GX_BLEND:      GXSetTevColorIn(stage, carg, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
					    	    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_TEXA, aarg, GX_CA_ZERO);
        	break;					        
		}
		
		GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, g_GXI_HW_States.bColorDiv2X ? GX_CS_DIVIDE_2 : GX_CS_SCALE_1, 1, GX_TEVPREV);
		GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, g_GXI_HW_States.bAlphaDiv2X ? GX_CS_DIVIDE_2 : GX_CS_SCALE_1, 1, GX_TEVPREV);
	}
#ifndef	_FINAL_
	}
	else
	{
		GXTevColorArg carg;
		GXTevAlphaArg aarg;
		
		g_GXI_HW_States.opStage = stage;
		g_GXI_HW_States.opMode = mode;
		g_GXI_HW_States.bColorDiv2X = g_bColorDiv2X;
		g_GXI_HW_States.bAlphaDiv2X = g_bAlphaDiv2X;
						
	    if (stage != GX_TEVSTAGE0) 
	    {
	        carg = GX_CC_CPREV;
	        aarg = GX_CA_APREV;
	    }
	    else
    	{
			carg = GX_CC_RASC;
		    aarg = GX_CA_RASA;
    	}
    
		switch(mode)
		{
			case GX_MODULATE: 	GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, carg, GX_CC_ZERO);
								GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_TEXA, aarg, GX_CA_ZERO);
			break;
			
			case GX_PASSCLR:  	GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, carg);
								GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, aarg);
			break;
			
			case GX_REPLACE:	GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
						        GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
			break;
									        
			case GX_DECAL:      GXSetTevColorIn(stage, carg, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
        						GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, aarg);
	        break;	
	        
	        case GX_BLEND:      GXSetTevColorIn(stage, carg, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
					    	    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_TEXA, aarg, GX_CA_ZERO);
        	break;					        
		}
		
		GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, g_GXI_HW_States.bColorDiv2X ? GX_CS_DIVIDE_2 : GX_CS_SCALE_1, 1, GX_TEVPREV);
		GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, g_GXI_HW_States.bAlphaDiv2X ? GX_CS_DIVIDE_2 : GX_CS_SCALE_1, 1, GX_TEVPREV);
	}
#endif
}

inline void GX_GXSetChanMatColor(GXChannelID scChan, GXColor scColor)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if( g_GXI_HW_States.scChanMat != scChan ||
	    *((u32*)&g_GXI_HW_States.scColorMat) != *((u32*)&scColor) )
	{
		g_GXI_HW_States.scChanMat = scChan;
		g_GXI_HW_States.scColorMat = scColor;
		GXSetChanMatColor(scChan,scColor);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.scChanMat = scChan;
		g_GXI_HW_States.scColorMat = scColor;
		GXSetChanMatColor(scChan,scColor);
	}
#endif
}

inline void GX_GXSetChanAmbColor(GXChannelID scChan, GXColor scColor)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if( g_GXI_HW_States.scChanAmb != scChan ||
	    *((u32*)&g_GXI_HW_States.scColorAmb) != *((u32*)&scColor) )
	{
		g_GXI_HW_States.scChanAmb = scChan;
		g_GXI_HW_States.scColorAmb = scColor;
		GXSetChanAmbColor(scChan,scColor);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.scChanAmb = scChan;
		g_GXI_HW_States.scColorAmb = scColor;
		GXSetChanAmbColor(scChan,scColor);
	}
#endif
}

inline void GX_GXSetCullMode( GXCullMode cullMode )
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if( g_GXI_HW_States.cullMode != cullMode)
	{
		g_GXI_HW_States.cullMode = cullMode;
		GXSetCullMode(cullMode);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.cullMode = cullMode;
		GXSetCullMode(cullMode);
	}
#endif
}
	
inline void GX_GXSetArray( GXAttr attr, void *base_ptr, u8 stride )
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if(g_GXI_HW_States.arBasePtr[attr] != base_ptr ||
	   g_GXI_HW_States.arStride[attr] != stride)
	{
		g_GXI_HW_States.arBasePtr[attr]=base_ptr;
		g_GXI_HW_States.arStride[attr]=stride;
		GXSetArray(attr, base_ptr, stride);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.arBasePtr[attr]=base_ptr;
		g_GXI_HW_States.arStride[attr]=stride;
		GXSetArray(attr, base_ptr, stride);
	}
#endif
}

inline void GX_GXSetColorUpdate(GXBool _bOn)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if(g_GXI_HW_States.ColorUpdate != _bOn)
	{
		g_GXI_HW_States.ColorUpdate=_bOn;
		GXSetColorUpdate(g_GXI_HW_States.ColorUpdate);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.ColorUpdate=_bOn;
		GXSetColorUpdate(g_GXI_HW_States.ColorUpdate);
	}
#endif	
}

inline void GX_GXSetAlphaUpdate(GXBool _bOn)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if(g_GXI_HW_States.AlphaUpdate != _bOn)
	{
		g_GXI_HW_States.AlphaUpdate=_bOn;
		GXSetAlphaUpdate(g_GXI_HW_States.AlphaUpdate);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.AlphaUpdate=_bOn;
		GXSetAlphaUpdate(g_GXI_HW_States.AlphaUpdate);
	}
#endif	
}

inline void GX_GXSetTevSwapMode(GXTevStageID _stage, GXTevSwapSel _rasc_sel, GXTevSwapSel _tex_sel)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if(   g_GXI_HW_States.tsRascSel[_stage] != _rasc_sel
	   || g_GXI_HW_States.tsTexSel[_stage] != _tex_sel
	  )
	{
	    g_GXI_HW_States.tsRascSel[_stage] = _rasc_sel;
	    g_GXI_HW_States.tsTexSel[_stage] = _tex_sel;	  
	  
		GXSetTevSwapMode(_stage, g_GXI_HW_States.tsRascSel[_stage], g_GXI_HW_States.tsTexSel[_stage]);
	}
#ifndef	_FINAL_
	}
	else
	{
	    g_GXI_HW_States.tsRascSel[_stage] = _rasc_sel;
	    g_GXI_HW_States.tsTexSel[_stage] = _tex_sel;	  
	  
		GXSetTevSwapMode(_stage, g_GXI_HW_States.tsRascSel[_stage], g_GXI_HW_States.tsTexSel[_stage]);
	}
#endif	
}

inline void GX_GXSetCurrentMtx(u32 _uIndex)
{
#ifndef _FINAL_
	if(g_bUseHWStates)
	{
#endif	
	if (g_GXI_HW_States.uCurrentMtx != _uIndex)
	{
		g_GXI_HW_States.uCurrentMtx = _uIndex;
		GXSetCurrentMtx(_uIndex);
	}
#ifndef	_FINAL_
	}
	else
	{
		g_GXI_HW_States.uCurrentMtx = _uIndex;	  
		GXSetCurrentMtx(_uIndex);
	}
#endif	

}

inline u32 GX_uGetCurrentMtx()
{
	return g_GXI_HW_States.uCurrentMtx;
}

#endif
