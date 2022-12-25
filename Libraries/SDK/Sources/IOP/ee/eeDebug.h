/*$T eeDebug.h GC! 1.097 12/03/01 10:30:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_TARGET
#ifndef __eeDebug_h__
#define __eeDebug_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    switch
 ***********************************************************************************************************************
 */

#ifdef _DEBUG
//#define __eeDbg_Msg__
#define __eeDbg_Err__
#endif

#ifdef RELEASE
//#define __eeDbg_Msg__
//#define __eeDbg_Err__
#endif

#if defined(_FINAL_) || defined(PSX2_USE_iopCDV)
#undef __eeDbg_Msg__
#undef __eeDbg_Err__ 
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    debug messages
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef __eeDbg_Msg__
#define eeDbg_Str_MsgHdr		"-- EE["
#define eeDbg_Msg_0000			"0000]: CDV Disc checking"
#define eeDbg_Msg_0001			"0001]: CDV CD/DVD format ok"
#define eeDbg_Msg_0002			"0002]: CDV Module ee cfg"
#define eeDbg_Msg_0003			"0003]: CDV Module iop cfg"
#define eeDbg_Msg_0004			"0004]: CDV DVD mode selected"
#define eeDbg_Msg_0005			"0005]: CDV CD-ROM mode selected"
#define eeDbg_Msg_0006			"0006]: KER Begin of eeMain"
#define eeDbg_Msg_0007			"0007]: KER End of eeMain"
#define eeDbg_Msg_0008			"0008]: RPC Loading IRX modules..."
#define eeDbg_Msg_0009			"0009]: MC  Start init..."
#define eeDbg_Msg_000A			"000A]: RPC buffer size move to"
#define eeDbg_Msg_000B			"000B]: MC  End of init"
#define eeDbg_Msg_000C			"000C]: INI New arg bigfile : "
#define eeDbg_Msg_000D			"000D]: INI New arg bin : "
#define eeDbg_Msg_000E			"000E]: INI New arg X : "
#define eeDbg_Msg_000F			"000F]: INI New arg sound : "
#define eeDbg_Msg_0010			"0010]: SND FX depth phase inverted"
#define eeDbg_Msg_0011			"0011]: SND FX feedback don't affect this mode"
#define eeDbg_Msg_0012			"0012]: SND FX delay don't affect this mode"
#define eeDbg_Msg_0013			"0013]: SND FX volume phase inverted"
#define eeDbg_M_Msg(__a)		scePrintf(eeDbg_Str_MsgHdr __a "\n")
#define eeDbg_M_MsgX(__a, ...)	scePrintf(eeDbg_Str_MsgHdr __a "\n", __VA_ARGS__)
#else
#define eeDbg_M_Msg(__a)
#define eeDbg_M_MsgX(__a, ...)
#endif /* __eeDbg_Msg__ */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    errors
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef __eeDbg_Err__
#define eeDbg_Str_ErrHdr		"** EE["
#define eeDbg_Err_0000			"0000]: CDV CD/DVD format ko"
#define eeDbg_Err_0001			"0001]: CDV Root directory"
#define eeDbg_Err_0002			"0002]: CDV parent directory"
#define eeDbg_Err_0003			"0003]: CDV sceCdRead failed"
#define eeDbg_Err_0004			"0004]: SND command failed"
#define eeDbg_Err_0005			"0005]: CDV sceCdMmode failed"
#define eeDbg_Err_0006			"0006]: CDV sceCdDiskReady failed"
#define eeDbg_Err_0007			"0007]: CDV sceCdStandby failed"
#define eeDbg_Err_0008			"0008]: CD/CDV DVD mode unknown"
#define eeDbg_Err_0009			"0009]: CDV No more space in the file array"
#define eeDbg_Err_000A			"000A]: CDV sceCdGetError failed"
#define eeDbg_Err_0010			"0010]: CDV No error !"
#define eeDbg_Err_0011			"0011]: CDV Outermost track reached during playback"
#define eeDbg_Err_0012			"0012]: CDV Cover opened during playback"
#define eeDbg_Err_0013			"0013]: CDV Problem occurred during read"
#define eeDbg_Err_0014			"0014]: CDV error code SCECdErPRM"
#define eeDbg_Err_0015			"0015]: CDV error code SCECdErILI"
#define eeDbg_Err_0016			"0016]: CDV error code SCECdErIPI"
#define eeDbg_Err_0017			"0017]: CDV Not appropriate for disc in drive"
#define eeDbg_Err_0018			"0018]: CDV Processing command"
#define eeDbg_Err_0019			"0019]: CDV No Disc in tray"
#define eeDbg_Err_001A			"001A]: CDV error code SCECdErOPENS"
#define eeDbg_Err_001B			"001B]: CDV error code SCECdErCMD"
#define eeDbg_Err_001C			"001C]: CDV Abort command received"
#define eeDbg_Err_001D			"001D]: CDV Unknown error"
#define eeDbg_Err_001E			"001E]: CDV No enougth memory"
#define eeDbg_Err_001F			"001F]: IRX Module can't be loaded !!"
#define eeDbg_Err_0020			"0020]: IRX Module loading failed !!"
#define eeDbg_Err_0021			"0021]: RPC client data can't be created !!"
#define eeDbg_Err_0022			"0022]: RPC command failed !!"
#define eeDbg_Err_0023			"0023]: SND command failed !!"
#define eeDbg_Err_0024			"0024]: RPC no enought space for RPC command"
#define eeDbg_Err_0025			"0025]: CDV read failed"
#define eeDbg_Err_0026			"0026]: CDV/RPC read limit reached!!"
#define eeDbg_Err_0027			"0027]: MC  sce lib can't be initialized"
#define eeDbg_Err_0028			"0028]: SND get ressources failed"
#define eeDbg_Err_0029			"0029]: SND bad file"
#define eeDbg_Err_002A			"002A]: KER bad return code"
#define eeDbg_Err_002B			"002B]: RPC bad in pointer"
#define eeDbg_Err_002C			"002C]: RPC bad out pointer"
#define eeDbg_Err_002D			"002C]: SND try to destroy one shared SB"
#define eeDbg_Err_002E			"002E]: SND no more free SPU hole"
#define eeDbg_Err_002F			"002F]: SND try to alloc SPU with null size"
#define eeDbg_Err_0030			"0030]: SND bad command for stream"
#define eeDbg_Err_0031			"0031]: RPC no more IOP RAM"
#define eeDbg_Err_0032			"0032]: SND wav can not be stereo"
#define eeDbg_Err_0033			"0033]: SND position is not aligned"

#define eeDbg_M_Err(__a)		scePrintf(eeDbg_Str_ErrHdr __a "\n")
#define eeDbg_M_ErrX(__a, ...)	scePrintf(eeDbg_Str_ErrHdr __a "\n", __VA_ARGS__)
#define eeDbg_M_Assert(__a, __b) \
	if(!(__a)) eeDbg_M_Err(__b)
#else
#define eeDbg_M_Err(__a)
#define eeDbg_M_ErrX(__a, ...)
#define eeDbg_M_Assert(__a, __b)
#endif /* __eeDbg_Err__ */


/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */


#ifdef __cplusplus
}
#endif

#endif /* __eeDebug_h__ */
#endif /* PSX2_TARGET */
