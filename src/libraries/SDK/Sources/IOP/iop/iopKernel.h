/*$T iopKernel.h GC! 1.097 02/04/02 10:58:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP
#ifndef __iopKernel_h__
#define __iopKernel_h__

#ifdef _DEBUG

/*$4
 ***********************************************************************************************************************
    DEBUG
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_CreateSema(struct SemaParam *param)
{
	/*~~~~~~*/
	int rcode;
	/*~~~~~~*/

	rcode = CreateSema(param);
	switch(rcode)
	{
	case KE_NO_MEMORY:			iopDbg_M_Err(iopDbg_Err_0085 " KE_NO_MEMORY"); break;
	case KE_ILLEGAL_ATTR:		iopDbg_M_Err(iopDbg_Err_0085 " KE_ILLEGAL_ATTR"); break;
	case KE_ILLEGAL_CONTEXT:	iopDbg_M_Err(iopDbg_Err_0085 " KE_ILLEGAL_CONTEXT"); break;
	default:					break;
	}

	return rcode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_CreateThread(struct ThreadParam *param)
{
	/*~~~~~~*/
	int rcode;
	/*~~~~~~*/

	rcode = CreateThread(param);
	switch(rcode)
	{
	case KE_NO_MEMORY:			iopDbg_M_Err(iopDbg_Err_0084 " KE_NO_MEMORY"); break;
	case KE_ILLEGAL_ATTR:		iopDbg_M_Err(iopDbg_Err_0084 " KE_ILLEGAL_ATTR"); break;
	case KE_ILLEGAL_STACK_SIZE:  iopDbg_M_Err(iopDbg_Err_0084 " KE_ILLEGAL_STACK_SIZE"); break;
	case KE_ILLEGAL_CONTEXT:	 iopDbg_M_Err(iopDbg_Err_0084 " KE_DORMANT"); break;
	case KE_ILLEGAL_PRIORITY:	 iopDbg_M_Err(iopDbg_Err_0084 " KE_ILLEGAL_PRIORITY"); break;
	case KE_ILLEGAL_ENTRY:		 iopDbg_M_Err(iopDbg_Err_0084 " KE_ILLEGAL_ENTRY"); break;
	}

	return rcode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_ChangeThreadPriority(int thid, int priority)
{
	/*~~~~~~*/
	int rcode;
	/*~~~~~~*/

	rcode = ChangeThreadPriority(thid, priority);
	switch(rcode)
	{
	case KE_OK:					break;
	case KE_UNKNOWN_THID:		 iopDbg_M_Err(iopDbg_Err_0083 " KE_UNKNOWN_THID"); break;
	case KE_ILLEGAL_PRIORITY:	 iopDbg_M_Err(iopDbg_Err_0083 " KE_ILLEGAL_PRIORITY"); break;
	case KE_ILLEGAL_CONTEXT:	 iopDbg_M_Err(iopDbg_Err_0083 " KE_ILLEGAL_CONTEXT"); break;
	case KE_DORMANT:			 iopDbg_M_Err(iopDbg_Err_0083 " KE_DORMANT"); break;
	default:                     iopDbg_M_Err(iopDbg_Err_0083); break;
	}

	return rcode;
}

inline int L_WakeupThread(int thid)
{
    int ret;
    ret = WakeupThread(thid);
	switch(ret)
	{
	case KE_OK:					break;
	case KE_UNKNOWN_THID:		 iopDbg_M_Err(iopDbg_Err_0096 " KE_UNKNOWN_THID"); break;
	case KE_ILLEGAL_PRIORITY:	 iopDbg_M_Err(iopDbg_Err_0096 " KE_ILLEGAL_PRIORITY"); break;
	}
	
	return ret;
}

inline int L_SleepThread(void)
{
    int rcode;
    rcode = SleepThread();
	switch(rcode)
	{
	case KE_OK:					break;
	case KE_CAN_NOT_WAIT:		iopDbg_M_Err(iopDbg_Err_0097 " KE_CAN_NOT_WAIT"); break;
	case KE_RELEASE_WAIT:	    iopDbg_M_Err(iopDbg_Err_0097 " KE_RELEASE_WAIT"); break;
	case KE_ILLEGAL_CONTEXT:    iopDbg_M_Err(iopDbg_Err_0097 " KE_ILLEGAL_CONTEXT"); break;
	default:                    iopDbg_M_Err(iopDbg_Err_0097); break;
	}

	return rcode;
}
#else

/*$4
 ***********************************************************************************************************************
    RELEASE
 ***********************************************************************************************************************
 */

#define L_CreateSema			CreateSema
#define L_CreateThread			CreateThread
#define L_ChangeThreadPriority  ChangeThreadPriority
#define L_WakeupThread          WakeupThread
#define L_SleepThread           SleepThread

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
#endif /* __iopKernel_h__ */
#endif /* PSX2_IOP */
