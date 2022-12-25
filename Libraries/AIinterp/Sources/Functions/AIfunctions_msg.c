/*$T AIfunctions_msg.c GC 1.139 03/01/04 11:10:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIdebug.h"
#include "AIinterp/Sources/AImsg.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "BASe/MEMory/MEM.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

AI_tdst_Message gst_Lifo;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MSGGetCount_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_MessageLiFo *pst_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO->pst_Extended) return 0;
	if(!pst_GO->pst_Extended->pst_Msg) return 0;
	pst_Msg = (AI_tdst_MessageLiFo *) pst_GO->pst_Extended->pst_Msg;
	return pst_Msg->i_NumMessages;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGetCount(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_MSGGetCount_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGSetNull_C(AI_tdst_Message *pmsg)
{
	L_memset(pmsg, 0, sizeof(AI_tdst_Message));
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGSetNull(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGSetNull_C((AI_tdst_Message *) st_Var.pv_Addr);
	return ++_pst_Node;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGSend_C(OBJ_tdst_GameObject *pst_GO, AI_tdst_Message *Msg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_MessageLiFo *pst_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO->pst_Extended) return;
	if(!pst_GO->pst_Extended->pst_Msg) return;

	/* Add the message in LIFO array of dest object */
	pst_Msg = (AI_tdst_MessageLiFo *) pst_GO->pst_Extended->pst_Msg;
	if((ULONG) pst_Msg->i_NumMessages < AI_C_MaxMessages)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		AI_tdst_MessageList *pt_Msg = (AI_tdst_MessageList*)MEM_p_Alloc(sizeof(AI_tdst_MessageList));
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		L_memcpy(&pt_Msg->msg, Msg, sizeof(AI_tdst_Message));

		if(!pst_Msg->pst_First) pst_Msg->pst_First = pt_Msg;
		pt_Msg->pst_Prev = pst_Msg->pst_Last;
		pt_Msg->pst_Next = NULL;
		if(pst_Msg->pst_Last) pst_Msg->pst_Last->pst_Next = pt_Msg;
		pst_Msg->pst_Last = pt_Msg;

		pst_Msg->i_NumMessages++;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGSend(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGSend_C(pst_GO, (AI_tdst_Message *) st_Var.pv_Addr);
	return ++_pst_Node;
}

void AI_EvalFunc_MSG_DataSend_C(OBJ_tdst_GameObject *_pst_DestGO, OBJ_tdst_GameObject *_pst_PosGO, int _i_Id, int _i_Int1, float _f_float1)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    AI_tdst_Message st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/


    L_memset(&st_Msg, 0, sizeof(AI_tdst_Message));
    
    st_Msg.i_Id = _i_Id;
    st_Msg.i_Int1 = _i_Int1;
    st_Msg.st_Vec2.x = _f_float1;

    if(_pst_PosGO && _pst_PosGO->pst_GlobalMatrix) 
        MATH_CopyVector(&st_Msg.st_Vec1, &_pst_PosGO->pst_GlobalMatrix->T);

    AI_EvalFunc_MSGSend_C(_pst_DestGO, &st_Msg);
}
/**/
AI_tdst_Node *AI_EvalFunc_MSG_DataSend(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_DestGO;
    OBJ_tdst_GameObject *pst_PosGO;
    int i_Id;
    int i_Int1;
    float f_float1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_float1 = AI_PopFloat();
	i_Int1 = AI_PopInt();
	i_Id = AI_PopInt();

    pst_PosGO = AI_PopGameObject();
	AI_M_GetCurrentObject(pst_DestGO);

	AI_EvalFunc_MSG_DataSend_C(pst_DestGO, pst_PosGO, i_Id, i_Int1, f_float1);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGet_C(OBJ_tdst_GameObject *pst_GO, AI_tdst_Message *ret)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_MessageLiFo *pst_Msg;
	AI_tdst_MessageList *pt_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO->pst_Extended, "No message struct allocated");
	AI_Check(pst_GO->pst_Extended->pst_Msg, "No message struct allocated");

	pst_Msg = (AI_tdst_MessageLiFo *) pst_GO->pst_Extended->pst_Msg;
	if(pst_Msg->i_NumMessages > 0)
	{
		pst_Msg->i_NumMessages--;
		pt_Msg = pst_Msg->pst_Last;
		L_memcpy(ret, &pt_Msg->msg, sizeof(AI_tdst_Message));

		if(pt_Msg->pst_Prev)
		{
			pt_Msg->pst_Prev->pst_Next = NULL;
			pst_Msg->pst_Last = pt_Msg->pst_Prev;
		}
		else
		{
			pst_Msg->pst_Last = pst_Msg->pst_First = NULL;
		}

		MEM_Free(pt_Msg);
	}
	else
	{
		ret->i_Id = 0;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Message		st_Msg;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_MSGGet_C(pst_GO, &st_Msg);
	AI_PushVal(&st_Msg, AI_gaw_EnumLink[TYPE_MESSAGE]);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGRead_C(OBJ_tdst_GameObject *pst_GO, int i_Num, AI_tdst_Message *ret)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_MessageLiFo *pst_Msg;
	AI_tdst_MessageList *pst_Ret;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO->pst_Extended, "No message struct allocated");
	AI_Check(pst_GO->pst_Extended->pst_Msg, "No message struct allocated");

	ret->i_Id = 0;
	pst_Msg = (AI_tdst_MessageLiFo *) pst_GO->pst_Extended->pst_Msg;
	if(i_Num < pst_Msg->i_NumMessages)
	{
		pst_Ret = pst_Msg->pst_First;
		while(pst_Ret && i_Num) pst_Ret = pst_Ret->pst_Next, i_Num--;
		if(pst_Ret) L_memcpy(ret, &pst_Ret->msg, sizeof(AI_tdst_Message));
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGRead(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Num;
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Num = AI_PopInt();
	AI_EvalFunc_MSGRead_C(pst_GO, i_Num, &st_Msg);
	AI_PushVal(&st_Msg, AI_gaw_EnumLink[TYPE_MESSAGE]);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGetByID_C(OBJ_tdst_GameObject *pst_GO, int i_ID, AI_tdst_Message *ret)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_MessageLiFo *pst_Msg;
	AI_tdst_MessageList *pst_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO->pst_Extended, "No message struct allocated");
	AI_Check(pst_GO->pst_Extended->pst_Msg, "No message struct allocated");

	ret->i_Id = 0;
	pst_Msg = (AI_tdst_MessageLiFo *) pst_GO->pst_Extended->pst_Msg;
	if(pst_Msg->i_NumMessages > 0)
	{
		pst_Res = pst_Msg->pst_First;
		while(pst_Res)
		{
			if(pst_Res->msg.i_Id == i_ID)
			{
				L_memcpy(ret, &pst_Res->msg, sizeof(AI_tdst_Message));

				if(pst_Res->pst_Prev)
					pst_Res->pst_Prev->pst_Next = pst_Res->pst_Next;
				else
					pst_Msg->pst_First = pst_Res->pst_Next;

				if(pst_Res->pst_Next)
					pst_Res->pst_Next->pst_Prev = pst_Res->pst_Prev;
				else
					pst_Msg->pst_Last = pst_Res->pst_Prev;

				MEM_Free(pst_Res);
				pst_Msg->i_NumMessages--;
				return;
			}

			pst_Res = pst_Res->pst_Next;
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGetByID(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_ID;
	AI_tdst_Message		st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_ID = AI_PopInt();
	AI_EvalFunc_MSGGetByID_C(pst_GO, i_ID, &st_Msg);
	AI_PushVal(&st_Msg, AI_gaw_EnumLink[TYPE_MESSAGE]);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGClear_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_MessageLiFo *pst_Msg;
	AI_tdst_MessageList *pt_Msg, *pt_Next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO->pst_Extended, "No message struct allocated");
	AI_Check(pst_GO->pst_Extended->pst_Msg, "No message struct allocated");

	pst_Msg = (AI_tdst_MessageLiFo *) pst_GO->pst_Extended->pst_Msg;
	pt_Msg = pst_Msg->pst_First;
	while(pt_Msg)
	{
		pt_Next = pt_Msg->pst_Next;
		MEM_Free(pt_Msg);
		pt_Msg = pt_Next;
	}

	pst_Msg->pst_First = pst_Msg->pst_Last = NULL;
	pst_Msg->i_NumMessages = 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGClear(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_MSGClear_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: Allocate Msg struct Note: If already allocated, nothing is performed
 =======================================================================================================================
 */
void AI_EvalFunc_MSGOn_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	ULONG	l_IdentityFlags;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Msg))
	{
		l_IdentityFlags = OBJ_ul_FlagsIdentityGet(pst_GO);
		l_IdentityFlags |= OBJ_C_IdentityFlag_Msg;
		OBJ_ChangeIdentityFlags(pst_GO, l_IdentityFlags, OBJ_C_UnvalidFlags);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGOn(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_MSGOn_C(pst_GO);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

AI_tdst_GlobalMessageList	gast_GlobalMessages[C_GM_MaxTypes];
int							gi_GlobalMessageID = 0;

#define MSG_CheckType(a) \
	AI_Check(a < C_GM_MaxTypes, "Bad global message type"); \
	AI_Check(gast_GlobalMessages[a].num != gast_GlobalMessages[a].max, "Stack of global message is filled");

/*
 =======================================================================================================================
 =======================================================================================================================
 */

AI_tdst_GlobalMessage *MSG_IDTOP(unsigned int a)
{
	if((a >> 24) >= C_GM_MaxTypes) return 0;
	if((a & 0x00FFFFFF) >= (unsigned int) gast_GlobalMessages[a >> 24].num) return 0;
	return gast_GlobalMessages[(a) >> 24].msg + ((a) & 0x00FFFFFF);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MSG_GlobalFirstInit(void)
{
	L_memset(gast_GlobalMessages, 0, sizeof(gast_GlobalMessages));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MSG_GlobalDestroy(void)
{
	/*~~*/
	int						i, j;
	AI_tdst_GlobalMessage	*pgm;
	/*~~*/

	for(i = 0; i < C_GM_MaxTypes; i++)
	{
		if(gast_GlobalMessages[i].msg) 
        {
#ifdef ACTIVE_EDITORS
            if(gast_GlobalMessages[i].num) 
            {
                for(j = 0; j < gast_GlobalMessages[i].num; j++)
                {
                    pgm = gast_GlobalMessages[i].msg + j;
                    if (pgm->flags & C_GM_Deleted) continue;

                    AI_RemoveWatchOnAddressRange(&pgm->msg,(char *)&pgm->msg + sizeof(pgm->msg));
                }
            }
#endif //ACTIVE_EDITORS

            MEM_Free(gast_GlobalMessages[i].msg);
        }
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern BOOL ENG_gb_InPause;

void MSG_GlobalReinit(void)
{
	/*~~*/
	int i;
	/*~~*/

	ENG_gb_InPause = 0;
	gi_GlobalMessageID = 0;
	for(i = 0; i < C_GM_MaxTypes; i++)
	{
		gast_GlobalMessages[i].num = 0;
		gast_GlobalMessages[i].firsthole = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_GlobalMessage	*pst_BreakOnMessageDelete = NULL;
void MSGGlobalDelete(AI_tdst_GlobalMessage *pgm, int type)
{
#ifdef ACTIVE_EDITORS
    // Break on delete ?
    if (pst_BreakOnMessageDelete == pgm)
    {
        ENG_gb_ForcePauseEngine = TRUE;
        MessageBox(0, "Break On Message Delete", "AI", MB_OK);
        pst_BreakOnMessageDelete = NULL;
    }

    // Remove breakpoints on message.
    AI_RemoveWatchOnAddressRange(&pgm->msg,(char *)&pgm->msg + sizeof(pgm->msg));
#endif //ACTIVE_EDITORS

    // Remove message from engine
	pgm->flags |= C_GM_Deleted;
	pgm->msg.i_Id = 0;
	pgm->nexthole = gast_GlobalMessages[type].firsthole;
	gast_GlobalMessages[type].firsthole = pgm;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MSG_GlobalProcess(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i, j;
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ENG_gb_InPause) return;
	for(i = 0; i < C_GM_MaxTypes; i++)
	{
		if(!gast_GlobalMessages[i].num) continue;
		for(j = 0; j < gast_GlobalMessages[i].num; j++)
		{
			pgm = gast_GlobalMessages[i].msg + j;
			if(pgm->flags & C_GM_Deleted) continue;

			if((pgm->flags & C_GM_AddEOF) && !(pgm->flags & C_GM_DeleteEOF))
			{
				pgm->flags &= ~C_GM_AddEOF;
			}
			else
			{
				if(!(pgm->flags & C_GM_Infinit))
				{
					pgm->TimeLife -= TIM_gf_dt;
					if(pgm->TimeLife <= 0) 
                        pgm->flags |= C_GM_DeleteEOF;
				}

				if(pgm->flags & C_GM_DeleteEOF)
				{
                    MSGGlobalDelete(pgm,i);
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalAlloc_C(int type, int size)
{
	if(gast_GlobalMessages[type].max >= size) return;

	if(gast_GlobalMessages[type].msg)
	{
		gast_GlobalMessages[type].msg = (AI_tdst_GlobalMessage*)MEM_p_Realloc
			(
				gast_GlobalMessages[type].msg,
				size * sizeof(AI_tdst_GlobalMessage)
			);
	}
	else
	{
		gast_GlobalMessages[type].msg = (AI_tdst_GlobalMessage*)MEM_p_VMAlloc(size * sizeof(AI_tdst_GlobalMessage));
	}

	gast_GlobalMessages[type].max = size;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalAlloc(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~*/
	int type, size;
	/*~~~~~~~~~~~*/

	size = AI_PopInt();
	type = AI_PopInt();
	AI_EvalFunc_MSGGlobalAlloc_C(type, size);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalSend_C
(
	OBJ_tdst_GameObject		*pst_GO,
	int						type,
	float					life,
	AI_tdst_Message			*pmsg,
	int						flags,
	AI_tdst_GlobalMessageId *pmsgid
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*ptg;
	int						index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(gast_GlobalMessages[type].num == gast_GlobalMessages[type].max) return;
	if(gast_GlobalMessages[type].firsthole)
	{
		ptg = gast_GlobalMessages[type].firsthole;
		index = ptg - gast_GlobalMessages[type].msg;
		gast_GlobalMessages[type].firsthole = ptg->nexthole;
	}
	else
	{
		index = gast_GlobalMessages[type].num;
		ptg = &gast_GlobalMessages[type].msg[gast_GlobalMessages[type].num++];
	}

	ptg->flags = 0;
	if(flags & C_GM_AddEOF) ptg->flags = C_GM_AddEOF;
	if(flags & C_GM_Infinit) ptg->flags |= C_GM_Infinit;
	ptg->TimeLife = life;
	L_memcpy(&ptg->msg, pmsg, sizeof(AI_tdst_Message));
	ptg->msg.i_Id = ++gi_GlobalMessageID;
	ptg->msg.pst_Sender = pst_GO;
	pmsgid->msgid = (type << 24) + index;
	pmsgid->id = ptg->msg.i_Id;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalSend(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	int						type, flags;
	float					life;
	OBJ_tdst_GameObject		*pst_GO;
	AI_tdst_GlobalMessageId msgid;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	flags = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	life = AI_PopFloat();
	type = AI_PopInt();

	MSG_CheckType(type);
	AI_EvalFunc_MSGGlobalSend_C(pst_GO, type, life, (AI_tdst_Message *) st_Var.pv_Addr, flags, &msgid);
	AI_PushVal(&msgid, AI_gaw_EnumLink[TYPE_MESSAGEID]);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalScan_C(int type, int *rank, AI_tdst_GlobalMessageId *pmsg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*ptg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	(*rank)++;
	ptg = &gast_GlobalMessages[type].msg[*rank];
	while((ULONG) * rank < (ULONG) gast_GlobalMessages[type].num)
	{
		if((ptg->flags & C_GM_AddEOF) || (ptg->flags & C_GM_Deleted))
		{
			ptg++;
			(*rank)++;
			continue;
		}

		pmsg->id = ptg->msg.i_Id;
		pmsg->msgid = (type << 24) +*rank;
		return;
	}

	pmsg->msgid = -1;
	pmsg->id = -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalScan(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						type;
	AI_tdst_GlobalMessageId id;
	AI_tdst_UnionVar		Val;
	AI_tdst_PushVar			st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	type = AI_PopInt();
	AI_EvalFunc_MSGGlobalScan_C(type, (int *) st_Var.pv_Addr, &id);
	AI_PushVal(&id, AI_gaw_EnumLink[TYPE_MESSAGEID]);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalReplace_C(AI_tdst_GlobalMessageId *pmsgid, AI_tdst_Message *pmsg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return;
	L_memcpy(&pgm->msg, pmsg, sizeof(AI_tdst_Message));
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalReplace(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val, Val1;
	AI_tdst_PushVar		st_Var, st_Var1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGGlobalReplace_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr, (AI_tdst_Message *) st_Var1.pv_Addr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalSetLife_C(AI_tdst_GlobalMessageId *pmsgid, float life)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return;
	pgm->TimeLife = life;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalSetLife(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	float				life;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	life = AI_PopFloat();
	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGGlobalSetLife_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr, life);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_MSGGlobalGetLife_C(AI_tdst_GlobalMessageId *pmsgid)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return 0;
	return pgm->TimeLife;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalGetLife(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_PushFloat(AI_EvalFunc_MSGGlobalGetLife_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MSGGlobalGetType_C(AI_tdst_GlobalMessageId *pmsgid)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return 0;
	return pmsgid->msgid >> 24;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalGetType(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_PushInt(AI_EvalFunc_MSGGlobalGetType_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalDelete_C(AI_tdst_GlobalMessageId *pmsgid, int flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return;
	if(flags & C_GM_DeleteEOF)
	{
		pgm->flags |= C_GM_DeleteEOF;
		return;
	}

    MSGGlobalDelete(pgm,pmsgid->msgid >> 24);
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalDelete(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	int					flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	flags = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGGlobalDelete_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr, flags);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MSGGlobalIsValid_C(AI_tdst_GlobalMessageId *pmsgid)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((pmsgid->msgid >> 24) == 0) return 0;
	if((pmsgid->msgid >> 24) > C_GM_MaxTypes) return 0;
	if((pmsgid->msgid & 0x00FFFFFF) > gast_GlobalMessages[pmsgid->msgid >> 24].num) return 0;
	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return 0;
	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalIsValid(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_PushInt(AI_EvalFunc_MSGGlobalIsValid_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalSetInvalid_C(AI_tdst_GlobalMessageId *pmsgid)
{
	pmsgid->id = -1;
	pmsgid->msgid = -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalSetInvalid(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGGlobalSetInvalid_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalGetMsg_C(AI_tdst_GlobalMessageId *pmsgid, AI_tdst_Message *pmsg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pmsg->i_Id = 0;
	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return;
	L_memcpy(pmsg, &pgm->msg, sizeof(AI_tdst_Message));
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalGetMsg(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	AI_tdst_Message		msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGGlobalGetMsg_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr, &msg);
	AI_PushVal(&msg, AI_gaw_EnumLink[TYPE_MESSAGE]);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalSearchIntGao_C(int type, int *rank, AI_tdst_Message *pscan, AI_tdst_GlobalMessageId *pmsg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*ptg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	(*rank)++;
	ptg = &gast_GlobalMessages[type].msg[*rank];
	while((ULONG) * rank < (ULONG) gast_GlobalMessages[type].num)
	{
		if((ptg->flags & C_GM_AddEOF) || (ptg->flags & C_GM_Deleted))
		{
			ptg++;
			(*rank)++;
			continue;
		}

		if
		(
			(pscan->i_Int1 && pscan->i_Int1 != ptg->msg.i_Int1)
		||	(pscan->i_Int2 && pscan->i_Int2 != ptg->msg.i_Int2)
		||	(pscan->i_Int3 && pscan->i_Int3 != ptg->msg.i_Int3)
		||	(pscan->i_Int4 && pscan->i_Int4 != ptg->msg.i_Int4)
		||	(pscan->i_Int5 && pscan->i_Int5 != ptg->msg.i_Int5)
		||	(pscan->pst_Sender && pscan->pst_Sender != ptg->msg.pst_Sender)
		||	(pscan->pst_GAO1 && pscan->pst_GAO1 != ptg->msg.pst_GAO1)
		||	(pscan->pst_GAO2 && pscan->pst_GAO2 != ptg->msg.pst_GAO2)
		||	(pscan->pst_GAO3 && pscan->pst_GAO3 != ptg->msg.pst_GAO3)
		||	(pscan->pst_GAO4 && pscan->pst_GAO4 != ptg->msg.pst_GAO4)
		||	(pscan->pst_GAO5 && pscan->pst_GAO5 != ptg->msg.pst_GAO5)
		)
		{
			ptg++;
			(*rank)++;
			continue;
		}

		pmsg->id = ptg->msg.i_Id;
		pmsg->msgid = (type << 24) +*rank;
		return;
	}

	pmsg->msgid = -1;
	pmsg->id = -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalSearchIntGao(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						type;
	AI_tdst_GlobalMessageId id;
	AI_tdst_UnionVar		Val, Val1;
	AI_tdst_PushVar			st_Var, st_Var1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVar(&Val, &st_Var);
	type = AI_PopInt();
	AI_EvalFunc_MSGGlobalSearchIntGao_C(type, (int *) st_Var.pv_Addr, (AI_tdst_Message *) st_Var1.pv_Addr, &id);
	AI_PushVal(&id, AI_gaw_EnumLink[TYPE_MESSAGEID]);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalSetGao_C(AI_tdst_GlobalMessageId *pmsgid, OBJ_tdst_GameObject *val, int num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return;
	*(&pgm->msg.pst_GAO1 + num) = val;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalSetGao(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	int					num;
	OBJ_tdst_GameObject *val;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	num = AI_PopInt();
	val = AI_PopGameObject();
	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGGlobalSetGao_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr, val, num);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalSetInt_C(AI_tdst_GlobalMessageId *pmsgid, int val, int num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return;
	*(&pgm->msg.i_Int1 + num) = val;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalSetInt(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	int					num;
	int					val;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	num = AI_PopInt();
	val = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGGlobalSetInt_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr, val, num);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalSetVector_C(AI_tdst_GlobalMessageId *pmsgid, MATH_tdst_Vector *val, int num, int mask)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	MATH_tdst_Vector		*pp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return;
	pp = &pgm->msg.st_Vec1 + num;
	switch(mask)
	{
	case 0: MATH_CopyVector(pp, val); break;
	case 1: pp->x = val->x; break;
	case 2: pp->y = val->y; break;
	case 3: pp->z = val->z; break;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalSetVector(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	int					num;
	int					mask;
	MATH_tdst_Vector	*val;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	mask = AI_PopInt();
	num = AI_PopInt();
	val = AI_PopVectorPtr();
	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGGlobalSetVector_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr, val, num, mask);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_MSGGlobalGetGao_C(AI_tdst_GlobalMessageId *pmsgid, int num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return NULL;
	return *(&pgm->msg.pst_GAO1 + num);
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalGetGao(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	int					num;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	num = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_PushGameObject(AI_EvalFunc_MSGGlobalGetGao_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr, num));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_MSGGlobalGetSender_C(AI_tdst_GlobalMessageId *pmsgid)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return NULL;
	return pgm->msg.pst_Sender;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalGetSender(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_PushGameObject(AI_EvalFunc_MSGGlobalGetSender_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MSGGlobalGetInt_C(AI_tdst_GlobalMessageId *pmsgid, int num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id) return 0;
	return *(&pgm->msg.i_Int1 + num);
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalGetInt(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	int					num;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	num = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_PushInt(AI_EvalFunc_MSGGlobalGetInt_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr, num));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MSGGlobalGetVector_C(AI_tdst_GlobalMessageId *pmsgid, int num, MATH_tdst_Vector *dest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_GlobalMessage	*pgm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pgm = MSG_IDTOP(pmsgid->msgid);
	if(!pgm || pgm->msg.i_Id != pmsgid->id)
	{
		dest->x = dest->y = dest->z = 0;
		return;
	}

	MATH_CopyVector(dest, &pgm->msg.st_Vec1 + num);
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalGetVector(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	int					num;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	num = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_EvalFunc_MSGGlobalGetVector_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr, num, &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MSGGlobalGetId_C(AI_tdst_GlobalMessageId *pmsgid)
{
	if(!AI_EvalFunc_MSGGlobalIsValid_C(pmsgid)) return -1;
	return pmsgid->id;
}
/**/
AI_tdst_Node *AI_EvalFunc_MSGGlobalGetId(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_PushInt(AI_EvalFunc_MSGGlobalGetId_C((AI_tdst_GlobalMessageId *) st_Var.pv_Addr));
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
