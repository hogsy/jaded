/**********************************************************************
 *<
	FILE: channel.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/



#ifndef __CHANNEL__H
#define __CHANNEL__H


// Channels within the object.
#define NUM_OBJ_CHANS 9

// Indices for object channels
#define TOPO_CHAN_NUM 0
#define GEOM_CHAN_NUM 1
#define TEXMAP_CHAN_NUM 2
#define MTL_CHAN_NUM 3
#define SELECT_CHAN_NUM 4
#define SUBSEL_TYPE_CHAN_NUM 5 
#define DISP_ATTRIB_CHAN_NUM 6 
#define VERT_COLOR_CHAN_NUM 7
#define GFX_DATA_CHAN_NUM 8

// Bit flags for object channels
#define TOPO_CHANNEL    	(1<<0) // topology (faces, polygons etc)
#define GEOM_CHANNEL    	(1<<1) // vertices
#define TEXMAP_CHANNEL  	(1<<2) // texture vertices and mapping
#define MTL_CHANNEL     	(1<<3) // material on per face basis
#define SELECT_CHANNEL  	(1<<4) // selection bits
#define SUBSEL_TYPE_CHANNEL (1<<5) // vertex/face/edge
#define DISP_ATTRIB_CHANNEL (1<<6) // display attributes
#define VERTCOLOR_CHANNEL   (1<<7) // color per vertex
#define GFX_DATA_CHANNEL	(1<<8) // stripping, edge list, etc.

#define TM_CHANNEL		(1<<9)  // Object transform (may be modified by modifiers)
#define GLOBMTL_CHANNEL (1<<10)  // material applied to object as whole

#define OBJ_CHANNELS  (TOPO_CHANNEL|GEOM_CHANNEL|SELECT_CHANNEL|TEXMAP_CHANNEL|MTL_CHANNEL|SUBSEL_TYPE_CHANNEL|DISP_ATTRIB_CHANNEL|VERTCOLOR_CHANNEL|GFX_DATA_CHANNEL)
#define ALL_CHANNELS  (OBJ_CHANNELS|TM_CHANNEL|GLOBMTL_CHANNEL)


#endif // __CHANNEL__H
