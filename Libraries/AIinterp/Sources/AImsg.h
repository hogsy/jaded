/*$T AImsg.h GC!1.41 08/03/99 10:54:25 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "TIMer/PROfiler/PROdefs.h"

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __AIMSG_H__
#define __AIMSG_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define AI_C_MaxMessages	1000
#define C_GM_MaxTypes		128

#define C_GM_AddEOF			0x00000001
#define C_GM_DeleteEOF		0x00000002
#define C_GM_Deleted		0x00000004
#define C_GM_Infinit		0x00000008

/*
 ---------------------------------------------------------------------------------------------------
    Structure that defines a message
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  AI_tdst_Message_
{
    OBJ_tdst_GameObject *pst_Sender;    /* Must be the first of the field */
    OBJ_tdst_GameObject *pst_GAO1;
    OBJ_tdst_GameObject *pst_GAO2;
    OBJ_tdst_GameObject *pst_GAO3;
    OBJ_tdst_GameObject *pst_GAO4;
    OBJ_tdst_GameObject *pst_GAO5;
    MATH_tdst_Vector    st_Vec1;
    MATH_tdst_Vector    st_Vec2;
    MATH_tdst_Vector    st_Vec3;
    MATH_tdst_Vector    st_Vec4;
    MATH_tdst_Vector    st_Vec5;
    int					i_Int1;
    int					i_Int2;
    int					i_Int3;
    int					i_Int4;
    int					i_Int5;
    int					i_Id;
} AI_tdst_Message;

typedef struct  AI_tdst_MessageList_
{
	AI_tdst_Message		msg;
    struct AI_tdst_MessageList_ *pst_Next;
    struct AI_tdst_MessageList_ *pst_Prev;
} AI_tdst_MessageList;

typedef struct AI_tdst_GlobalMessage_
{
	ULONG				flags;
	union
	{
		struct AI_tdst_GlobalMessage_	*nexthole;
		float							TimeLife;
	};

	AI_tdst_Message		msg;
} AI_tdst_GlobalMessage;

typedef struct AI_tdst_GlobalMessageId_
{
	int						msgid;
	int						id;
} AI_tdst_GlobalMessageId;

typedef struct AI_tdst_GlobalMessageList_
{
	AI_tdst_GlobalMessage	*msg;
	AI_tdst_GlobalMessage	*firsthole;
	int						num;
	int						max;
#ifdef ACTIVE_EDITORS
	PRO_tdst_TrameRaster	st_Raster;
	char					az_NameRaster[64];
#endif
} AI_tdst_GlobalMessageList;

/*
 ---------------------------------------------------------------------------------------------------
    Structure for each object
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  AI_tdst_MessageLiFo_
{
    AI_tdst_MessageList *pst_First;
    AI_tdst_MessageList *pst_Last;
    int             i_NumMessages;      /* Number of messages in the array */
} AI_tdst_MessageLiFo;

extern void MSG_GlobalFirstInit(void);
extern void MSG_GlobalDestroy(void);
extern void MSG_GlobalReinit(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __AIMSG_H__ */