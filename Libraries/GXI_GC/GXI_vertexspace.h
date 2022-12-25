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

#ifndef __GXI_VERTEXSPACE_H__
#define __GXI_VERTEXSPACE_H__

void * GXI_GetVertexSpace(u32 spaceNeeded);
void   GXI_SwitchVertexSpace();

#endif
