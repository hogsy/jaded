#ifndef _GX8ADDINFO_H_
#define _GX8ADDINFO_H_

#define GX8_SPG_MAX_SPRITE 256										// it must be equal to SPG_Nb_Max_SpritePerPass 
#define GX8_SPG_MAX_SPRITE_PER_LEVEL	(GX8_SPG_MAX_SPRITE/4)		// 
#define GX8_SPG_LEVEL_NUMBER			(4)							// 

typedef struct
{
	unsigned short	auw_VertexIndex;
	unsigned short	auw_UVIndex;
} Gx8_tdstVBConstructionList;


#define MAX_DUPLICATED_BUFFER 100

typedef struct
{
	LPDIRECT3DVERTEXBUFFER8		pVB;
	unsigned int                iFrameNumber;
	void						*pToGameObject;
} Gx8_tdstVBData;

typedef struct
{
	Gx8_tdstVBData				* pVBData;
	unsigned int				iVBNumber;
	int							iVBIndex;
    int							iVertexShader;
    int                         iVertexStride;
} Gx8_tdstMultipleVB;

typedef struct
{
	Gx8_tdstMultipleVB			* pMultipleVB;
	unsigned int				iMVBNumber;
	unsigned int				iMVBIndex;
} Gx8_tdstMultipleVBList;


	// Sprite Vertex Buffer data
typedef struct
{
	LPDIRECT3DVERTEXBUFFER8		pSPG_VB;				// pointer to Sprite (SPG) Vertex Buffer
	int							iSpriteVertexNumber;	// Number of vertices in the Sprite VB
	int							iMaterialID;			// 
	float						fGlobalSize;			// 
	float						fGlobalRatio;			// 
} Gx8_tdstSpriteVBData;

typedef struct Gx8_stAddInfo
{
		// VB data
	Gx8_tdstMultipleVBList		* pMultipleVBList;		//
	UINT                        uiVertexOffset;			// Offset in the VB (specific for each element of the same object)
    UINT                        uiVertexCount;			// Number of vertices used by the element in the VB. Also the size of d_stVBConstructionList.
	int						    iIsDynamic;				//
		// IB data
    LPDIRECT3DINDEXBUFFER8      lpd3dibIndexBuffer;		// Index Buffer (specific for each element of the same object)
    UINT                        uiIndexOffset;			// Offset in the IB (specific for each element of the same object)
		// Aux data
    Gx8_tdstVBConstructionList  * d_stVBConstructionList; // An array specifying how each vertex in the VB is constructed.
		// SpriteVBData
	Gx8_tdstSpriteVBData		* pSpriteVBData;		// 
	UINT						uiUpdateHistory;		// a bit field that stores the update history of the latest frame 
} Gx8_tdstAddInfo;

#endif
