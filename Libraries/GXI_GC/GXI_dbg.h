
#ifndef __GXI_DBG_H__
#define __GXI_DBG_H__

extern BOOL g_bShowNormals;
extern BOOL g_bShowSkinElements;
extern BOOL g_bShowVertexColors;
extern BOOL g_bShowLights;
extern BOOL g_bUseHardwareTextureMatrices;
extern BOOL g_bUseHardwareLights;
extern BOOL g_bUseHWStates;
extern BOOL g_bUseHWMul2x;

extern BOOL NoMulti;

extern BOOL NoMATDRAW;
extern BOOL NoSDW;
extern BOOL NoSPR;
extern BOOL NoGEODRAW;
extern BOOL NoGODRAW;
extern BOOL NoLIGH;
extern BOOL NoGFX;
extern BOOL NoZLST;
extern BOOL NoSKN;
extern BOOL NoADM;
extern BOOL NoPAG;
extern BOOL NoSTR;
extern BOOL NoMDF;
extern BOOL NoFUR;
extern BOOL NoSPG2;

/*
 ===================================================================================================
    Draw primitives
 ===================================================================================================
 */

void GXI_DBG_ShowVertexColors(GEO_tdst_ElementIndexedTriangles	*_pst_Element,GEO_Vertex *_pst_Point,ULONG ulnumberOfPoints);
void GXI_DBG_ShowSkinElements(GEO_tdst_ElementIndexedTriangles	*_pst_Element,GEO_Vertex *_pst_Point,ULONG ulnumberOfPoints);
void GXI_DBG_DrawObjectNormals(GEO_tdst_ElementIndexedTriangles	*_pst_Element,GEO_Vertex *_pst_Point,ULONG ulnumberOfPoints);
void GXI_DBG_DrawLight(Vec *pos);

#endif /* __GXI_INIT_H__ */