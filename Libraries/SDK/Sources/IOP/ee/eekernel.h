/*$T eeKernel.h GC! 1.097 09/27/01 14:19:03 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_TARGET
#ifndef __eeKernel_h__
#define __eeKernel_h__

#ifdef _DEBUG
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    DEBUG
 -----------------------------------------------------------------------------------------------------------------------
 */

#define L_sceSifInitRpc(m)		sceSifInitRpc(m)
#define L_sceSifRebootIop(img)	sceSifRebootIop(img)
#define L_sceSifSyncIop()		sceSifSyncIop()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_sceSifLoadFileReset(void)
{
	/*~~~~~~*/
	int icode;
	/*~~~~~~*/

	icode = sceSifLoadFileReset();
	if(icode != 0) eeDbg_M_MsgX(eeDbg_Err_002A " : sceSifLoadFileReset 0x%x(%d)", icode, icode);
	return icode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_sceSifLoadModule(const char *a, int b, const char *c)
{
	/*~~~~~~*/
	int icode;
	/*~~~~~~*/

	icode = sceSifLoadModule(a, b, c);
	if(icode < 0)
	{
		switch(icode)
		{
		case SCE_EBINDMISS:		eeDbg_M_Msg(eeDbg_Err_002A " : sceSifLoadModule SCE_EBINDMISS"); break;
		case SCE_EVERSIONMISS:	eeDbg_M_Msg(eeDbg_Err_002A " : sceSifLoadModule SCE_EVERSIONMISS"); break;
		case SCE_ECALLMISS:		eeDbg_M_Msg(eeDbg_Err_002A " : sceSifLoadModule SCE_ECALLMISS"); break;
		default:				eeDbg_M_MsgX(eeDbg_Err_002A " : sceSifLoadModule ??? : 0x%x(%d)", icode, icode); break;
		}
	}

	return icode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_sceFsReset(void)
{
	/*~~~~~~*/
	int icode;
	/*~~~~~~*/

	icode = sceFsReset();
	if(icode != 0) eeDbg_M_MsgX(eeDbg_Err_002A " : sceFsReset 0x%x(%d)", icode, icode);
	return icode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_sceCdInit(int mode)
{
	/*~~~~~~*/
	int icode;
	/*~~~~~~*/

	icode = sceCdInit(mode);
	switch(icode)
	{
	case 0:		eeDbg_M_MsgX(eeDbg_Err_002A " : sceCdInit failed", icode, icode); break;
	case 1:		break;
	case 2:		eeDbg_M_Msg(eeDbg_Err_002A " : sceCdInit default module detected"); break;
	default:	eeDbg_M_MsgX(eeDbg_Err_002A " : sceCdInit ??? 0x%x(%d)", icode, icode); break;
	}

	return icode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_sceCdMmode(int m)
{
	/*~~~~~~*/
	int icode;
	/*~~~~~~*/

	icode = sceCdMmode(m);
	switch(icode)
	{
	case 0:		eeDbg_M_Msg(eeDbg_Err_002A " : sceCdMmode failed"); break;
	case 1:		break;
	default:	eeDbg_M_MsgX(eeDbg_Err_002A " : sceCdMmode ??? 0x%x(%d)", icode, icode); break;
	}

	return icode;
}

#else

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    RELEASE
 -----------------------------------------------------------------------------------------------------------------------
 */

#define L_sceSifInitRpc(m)			sceSifInitRpc(m)
#define L_sceSifRebootIop(a)		sceSifRebootIop(a)
#define L_sceSifSyncIop()			sceSifSyncIop()
#define L_sceSifLoadFileReset()		sceSifLoadFileReset()
#define L_sceSifLoadModule(a, b, c) sceSifLoadModule(a, b, c)
#define L_sceFsReset()				sceFsReset()
#define L_sceCdInit(m)				sceCdInit(m)
#define L_sceCdMmode(m)				sceCdMmode(m)
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* __eeKernel_h__ */
#endif /* PSX2_TARGET */
