//  =============================================================================
//  (C) Copyright 2001 Ubi Soft
//  =============================================================================
//
//  Description   : Manage the arrays for dolphin shared resources
//					by locking a range in the array then unlock it when no longer needed
//
//  Author        : Dany Joannette
//  Date          : 07 June 2001
//
//  =============================================================================


const u32 VERTEX_SPACE_SIZE= 512 * 1024; // must be enough to fit an entire frame !
static u8 vertexSpace[1][VERTEX_SPACE_SIZE] ATTRIBUTE_ALIGN(32);

static u32 currentIndex[2]={0,0};

u8  g_iCurrentVertexSpace=0;

#ifndef _FINAL_
u32 g_maxvertexspacesize = 0; 
#endif

void GXI_SwitchVertexSpace()
{
}

void * GXI_GetVertexSpace(u32 spaceNeeded)
{	
	void * retPos;
#ifdef GC_BENCHGRAPH
	GXI_Global_ACCESS(ulVertxArraySize) += spaceNeeded;
#endif
	
	if (currentIndex[g_iCurrentVertexSpace]+spaceNeeded > VERTEX_SPACE_SIZE)
	{
		//OSReport("!!!!!!! vertex space too small to fit the frame !!!!!!\n");
		currentIndex[g_iCurrentVertexSpace] = 0;
	}

	retPos=&(vertexSpace[g_iCurrentVertexSpace][currentIndex[g_iCurrentVertexSpace]]);
	
	currentIndex[g_iCurrentVertexSpace]=OSRoundUp32B(currentIndex[g_iCurrentVertexSpace]+spaceNeeded);

#ifndef _FINAL_
	g_maxvertexspacesize = max(max(currentIndex[0], currentIndex[1]), g_maxvertexspacesize);
#endif	
	
	return retPos;
}
