/*$T iopDebug.h GC! 1.097 04/25/02 14:23:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP
#ifndef __iopDebug_h__
#define __iopDebug_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    switches
 ***********************************************************************************************************************
 */

#ifdef _DEBUG

/* #define __iopDbg_Msg__ */
#define __iopDbg_Err__
#endif
#ifdef RELEASE

/* #define __iopDbg_Msg__ */
#define __iopDbg_Err__
#endif
#if defined(_FINAL_)// || defined(PSX2_USE_iopCDV)
#undef __iopDbg_Msg__
#undef __iopDbg_Err__

/* #undef __iopDbg_Err__ */
#endif

/*$4
 ***********************************************************************************************************************
    CDV trace
 ***********************************************************************************************************************
 */

/* #define __CDV_Trace__ */
#ifdef __CDV_Trace__
extern char CDV_dbg_Proc[4];

#define CDV_Dbg_Display(...)	printf(__VA_ARGS__)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline void CDV_Dbg_Trace(char *_proc)
{
	CDV_dbg_Proc[0] = _proc[0];
	CDV_dbg_Proc[1] = _proc[1];
	CDV_dbg_Proc[2] = _proc[2];
	CDV_dbg_Proc[3] = (_proc[2] ? _proc[3] : 0);
}

#else
#define CDV_Dbg_Display(...)
#define CDV_Dbg_Trace(_proc)

#endif

/*$4
 ***********************************************************************************************************************
    messages & errors
 ***********************************************************************************************************************
 */

/*$2- messages -------------------------------------------------------------------------------------------------------*/

#ifdef __iopDbg_Msg__
#define iopDbg_Str_MsgHdr	"--IOP["
#define iopDbg_Msg_0000		"0000]: KER Begin of supervisor thread"
#define iopDbg_Msg_0001		"0001]: RPC Command received"
#define iopDbg_Msg_0002		"0002]: KER Supervisor thread go to sleep"
#define iopDbg_Msg_0003		"0003]: KER Supervisor thread awakes"
#define iopDbg_Msg_0004		"0004]: SND Init of Snd..."
#define iopDbg_Msg_0005		"0005]: SND Loading a sound file..."
#define iopDbg_Msg_0006		"0006]: SND Begins transfert to SPU..."
#define iopDbg_Msg_0007		"0007]: SND Ends transfert"
#define iopDbg_Msg_0008		"0008]: KER Starting RPC thread..."
#define iopDbg_Msg_0009		"0009]: KER Starting sound thread..."
#define iopDbg_Msg_000A		"000A]: SND Init Voice Module"
#define iopDbg_Msg_000B		"000B]: SND DestroyVoiceBuffer..."
#define iopDbg_Msg_000C		"000C]: SND CreateVoiceBuffer..."
#define iopDbg_Msg_000E		"000E]: SND StopVoice, ok"
#define iopDbg_Msg_000F		"000F]: SND PlayVoice, ok"
#define iopDbg_Msg_0010		"0010]: SND Stream file detected"
#define iopDbg_Msg_0011		"0011]: SND Destroy Snd Buffer"
#define iopDbg_Msg_0012		"0012]: SND CreatePCMBuffer..."
#define iopDbg_Msg_0013		"0013]: SND PlayPCM, ok"
#define iopDbg_Msg_0014		"0014]: SND StopPCM, ok"
#define iopDbg_Msg_0015		"0015]: SND BigFile opened (only one)"
#define iopDbg_Msg_0016		"0016]: SND BigFile opened (multiple)"
#define iopDbg_Msg_0017		"0017]: SND Close Snd module"
#define iopDbg_Msg_0018		"0018]: SND Close Voice module"
#define iopDbg_Msg_0019		"0019]: SND Close PCM module"
#define iopDbg_Msg_001A		"001A]: SND Init PCM module"
#define iopDbg_Msg_001B		"001B]: SND Duplicate Voice Buff"
#define iopDbg_Msg_001D		"001D]: SND Pause Voice"
#define iopDbg_Msg_001E		"001E]: SND Set VoiceVol"
#define iopDbg_Msg_001F		"001F]: SND PlayLoop"
#define iopDbg_Msg_0020		"0020]: CDV Checking CD/DVD format"
#define iopDbg_Msg_0021		"0021]: CDV CD/DVD format ok"
#define iopDbg_Msg_0022		"0022]: CDV Starting CDV module"
#define iopDbg_Msg_0023		"0023]: CDV Module ready"
#define iopDbg_Msg_0024		"0024]: CDV DVD Mode selected"
#define iopDbg_Msg_0025		"0025]: CDV CD-ROM mode selected"
#define iopDbg_Msg_0026		"0026]: SND play Rq delayed"
#define iopDbg_Msg_0027		"0027]: SND directstop canceled"
#define iopDbg_Msg_0028		"0028]: SND duplicate file"
#define iopDbg_Msg_0029		"0029]: SND PlayNoLoop"
#define iopDbg_Msg_002A		"002A]: SND Reinit-play stream"
#define iopDbg_Msg_002B		"002B]: SND get ressources"
#define iopDbg_Msg_002C		"002C]: SND [CAUTION] voice state has changed"
#define iopDbg_Msg_002D		"002D]: SND unload file"

/*$2- macros ---------------------------------------------------------------------------------------------------------*/

#define iopDbg_M_Msg(__a)			printf(iopDbg_Str_MsgHdr __a "\n")
#define iopDbg_M_MsgX(__a, ...)		printf(iopDbg_Str_MsgHdr __a "\n", __VA_ARGS__)
#define iopDbg_M_MsgNX(__a, ...)	printf(iopDbg_Str_MsgHdr __a "\n")

#else	/* __iopDbg_Msg__ */
#define iopDbg_M_Msg(__a)
#define iopDbg_M_MsgX(__a, ...)
#define iopDbg_M_MsgNX(__a, ...)

#endif /* __iopDbg_Msg__ */

/*$2- error ----------------------------------------------------------------------------------------------------------*/

#ifdef __iopDbg_Err__
#define iopDbg_Str_ErrHdr	"ERR**IOP["
#define iopDbg_Err_1000		"1000]: KER Can't create the supervisor thread"
#define iopDbg_Err_0001		"0001]: KER Can't create the RPC server thread"
#define iopDbg_Err_0002		"0002]: SND Can't open a sound file"
#define iopDbg_Err_0003		"0003]: SND Can't alloc memory"
#define iopDbg_Err_0004		"0004]: SND Read failed"
#define iopDbg_Err_0005		"0005]: MEM No more spu memory free"
#define iopDbg_Err_0006		"0006]: MEM No more new hole !!!"
#define iopDbg_Err_0007		"0007]: MEM Try to free a bad area !!!"
#define iopDbg_Err_0008		"0008]: SND No any more sound file handler"
#define iopDbg_Err_0009		"0009]: SND No any more sound PCM Buffer"
#define iopDbg_Err_000A		"000A]: SND Pb in 1st wave read"
#define iopDbg_Err_000B		"000B]: KER Can't create sound thread"
#define iopDbg_Err_000C		"000C]: SND Bad PCM Id"
#define iopDbg_Err_000D		"000D]: SND Bad PCM Scheduler activation"
#define iopDbg_Err_000E		"000E]: SND Refresh buffer failed"
#define iopDbg_Err_000F		"000F]: SND Try to create too much DMA stream"
#define iopDbg_Err_0010		"0010]: SND Error in DMA scheduler"
#define iopDbg_Err_0011		"0011]: SND No more DMA request free"
#define iopDbg_Err_0012		"0012]: SND Voice transfert sequence error"
#define iopDbg_Err_0013		"0013]: SND Pb in 1st ADPCM read"
#define iopDbg_Err_0014		"0014]: SND Refresh buffer failed"
#define iopDbg_Err_0015		"0015]: SND Buffer size isn't modulo 16"
#define iopDbg_Err_0016		"0016]: SND Can't start the alarm"
#define iopDbg_Err_0017		"0017]: SND Can't stop the alarm"
#define iopDbg_Err_0018		"0018]: SND No enought memory for init"
#define iopDbg_Err_0019		"0019]: SND bad value for dma transert"
#define iopDbg_Err_001A		"001A]: SND Voice error"
#define iopDbg_Err_001B		"001B]: SND Bad Id"
#define iopDbg_Err_001C		"001C]: SND Can't duplicate Streamed file"
#define iopDbg_Err_001D		"001D]: SND Can't open bigfile"
#define iopDbg_Err_001E		"001E]: RPC Bad RPC command"
#define iopDbg_Err_001F		"001F]: RPC Can't destroy a duplicated Buff"
#define iopDbg_Err_0020		"0020]: SND read error"
#define iopDbg_Err_0021		"0021]: SND seek error"
#define iopDbg_Err_0022		"0022]: SND can't alloc for play looping"
#define iopDbg_Err_0023		"0023]: SND DirectSetSize failed"
#define iopDbg_Err_0024		"0024]: SND overwriting snd queue"
#define iopDbg_Err_0025		"0025]: SND will overflow snd queue"
#define iopDbg_Err_0026		"0026]: SND no more hard buffer"
#define iopDbg_Err_0027		"0027]: SND Very bad : stereo dialog !!"
#define iopDbg_Err_0028		"0028]: SND big problem : no more memory!!"
#define iopDbg_Err_0029		"0029]: SND duplicate failed"
#define iopDbg_Err_002A		"002A]: SND bad RIFF"
#define iopDbg_Err_002B		"002B]: SND parse failed"
#define iopDbg_Err_002C		"002C]: SND no more soft buffer"
#define iopDbg_Err_002D		"002D]: SND create stream failed"
#define iopDbg_Err_002E		"002E]: SND create sound buffer failed"
#define iopDbg_Err_002F		"002F]: SND duplicate failed"
#define iopDbg_Err_0030		"0030]: CDV CD/DVD format ko"
#define iopDbg_Err_0031		"0031]: CDV Root directory"
#define iopDbg_Err_0032		"0032]: CDV parent directory"
#define iopDbg_Err_0033		"0033]: CDV sceCdRead failed"
#define iopDbg_Err_0034		"0034]: CDV sceCdInit failed"
#define iopDbg_Err_0035		"0035]: CDV sceCdMmode failed"
#define iopDbg_Err_0036		"0036]: CDV sceCdDiskReady failed"
#define iopDbg_Err_0037		"0037]: CDV sceCdStandby failed"
#define iopDbg_Err_0038		"0038]: CDV CD/DVD mode unknown"
#define iopDbg_Err_0039		"0039]: CDV No more space in the file array"
#define iopDbg_Err_003A		"003A]: CDV sceCdGetError failed"
#define iopDbg_Err_003B		"003B]: SND last extend overflow"
#define iopDbg_Err_003C		"003C]: SND big problem during realloc !!!"
#define iopDbg_Err_003D		"003D]: SND no more file handler"
#define iopDbg_Err_003E		"003E]: FS  no more free request space"
#define iopDbg_Err_003F		"003F]: FS  overflow of Rq list"
#define iopDbg_Err_0040		"0040]: CDV No error !"
#define iopDbg_Err_0041		"0041]: CDV Outermost track reached during playback"
#define iopDbg_Err_0042		"0042]: CDV Cover opened during playback"
#define iopDbg_Err_0043		"0043]: CDV Problem occurred during read"
#define iopDbg_Err_0044		"0044]: CDV Error code SCECdErPRM"
#define iopDbg_Err_0045		"0045]: CDV Error code SCECdErILI"
#define iopDbg_Err_0046		"0046]: CDV Error code SCECdErIPI"
#define iopDbg_Err_0047		"0047]: CDV Not appropriate for disc in drive"
#define iopDbg_Err_0048		"0048]: CDV Processing command"
#define iopDbg_Err_0049		"0049]: CDV No Disc in tray"
#define iopDbg_Err_004A		"004A]: CDV Error code SCECdErOPENS"
#define iopDbg_Err_004B		"004B]: CDV Error code SCECdErCMD"
#define iopDbg_Err_004C		"004C]: CDV Abort command received"
#define iopDbg_Err_004D		"004D]: CDV Unknown error"
#define iopDbg_Err_004E		"004E]: CDV No enougth memory"
#define iopDbg_Err_004F		"004F]: MEM No enougth memory"
#define iopDbg_Err_0050		"0050]: RPC No enougth memory to set buffer size"
#define iopDbg_Err_0051		"0051]: RPC fatal error when setting buffer size"
#define iopDbg_Err_0052		"0052]: CDV can't create CDV semaphore"
#define iopDbg_Err_0053		"0053]: SND bad SndFile Id"
#define iopDbg_Err_0054		"0054]: SND bad SndBuffer Id"
#define iopDbg_Err_0055		"0055]: SND bad parse mode"
#define iopDbg_Err_0056		"0056]: SND buffer yet stopped"
#define iopDbg_Err_0057		"0057]: SND bad buffer address"
#define iopDbg_Err_0058		"0058]: SND bad hardbuffer"
#define iopDbg_Err_0059		"0059]: SND get ressources failed"
#define iopDbg_Err_005A		"005A]: SND Play during stop!!"
#define iopDbg_Err_005B		"005B]: KER Can't create one semaphore"
#define iopDbg_Err_005C		"005C]: SND bad dma Id for registering rq"
#define iopDbg_Err_005D		"005D]: SND try to register rq for free SB"
#define iopDbg_Err_005E		"005E]: SND try to play free SB"
#define iopDbg_Err_005F		"005F]: KER bad semaphore id"
#define iopDbg_Err_0060		"0060]: KER bad WaitSema return code"
#define iopDbg_Err_0061		"0061]: KER bad SignalSema return code"
#define iopDbg_Err_0062		"0062]: SND libsd error"
#define iopDbg_Err_0063		"0063]: KER bad iSignalSema return code"
#define iopDbg_Err_0064		"0064]: KER bad CpuSuspendIntr return code"
#define iopDbg_Err_0065		"0065]: KER bad CpuResumeIntr return code"
#define iopDbg_Err_0066		"0066]: KER bad DisableIntr return code"
#define iopDbg_Err_0067		"0067]: KER bad EnableIntr return code"
#define iopDbg_Err_0068		"0068]: SND bad SPU transfert status"
#define iopDbg_Err_0069		"0069]: SND too big size"
#define iopDbg_Err_0070		"0070]: KER bad fopen code"
#define iopDbg_Err_0071		"0071]: KER bad fread code"
#define iopDbg_Err_0072		"0072]: KER bad fseek code"
#define iopDbg_Err_0073		"0073]: KER bad fclose code"
#define iopDbg_Err_0074		"0074]: SND bad Fx Mode"
#define iopDbg_Err_0075		"0075]: SND bad Fx delay"
#define iopDbg_Err_0076		"0076]: SND bad Fx feedback"
#define iopDbg_Err_0077		"0077]: SND bad Rq Id for RPC"
#define iopDbg_Err_0078		"0078]: SND null addr for spu"
#define iopDbg_Err_0079		"0079]: SND No more RAM"
#define iopDbg_Err_007A		"007A]: SND create SB failed"
#define iopDbg_Err_007B		"007B]: SND bad stream ID"
#define iopDbg_Err_007D		"007D]: SND play canceled!!"
#define iopDbg_Err_007E		"007E]: SND loading canceled"
#define iopDbg_Err_007F		"007F]: SND dma rq error"
#define iopDbg_Err_0080		"0080]: SND dma signal whithout IT"
#define iopDbg_Err_0081		"0081]: SND bad command for stream"
#define iopDbg_Err_0082		"0082]: CLI no more ram"
#define iopDbg_Err_0083		"0083]: KER can not change thread priority"
#define iopDbg_Err_0084		"0084]: KER can not create thread"
#define iopDbg_Err_0085		"0085]: KER can not create sema"
#define iopDbg_Err_0086		"0086]: SND hard buffer is not free"
#define iopDbg_Err_0087		"0087]: CLI no more RAM for stream"
#define iopDbg_Err_0088		"0088]: CLI synchro error"
#define iopDbg_Err_0089		"0089]: SND no more IOP RAM"
#define iopDbg_Err_008A		"008A]: SND too big prefetch size"
#define iopDbg_Err_008B		"008B]: SND stereo desync"
#define iopDbg_Err_008C		"008C]: SND stream buffer over-read"
#define iopDbg_Err_008D		"008D]: SND bad stream state"
#define iopDbg_Err_008E		"008E]: SND PlayRq on stereo"
#define iopDbg_Err_008F		"008F]: SND Try to refresh bad stream"
#define iopDbg_Err_0090		"0090]: MTX no more memory"
#define iopDbg_Err_0091		"0091]: SND bad FX parama"
#define iopDbg_Err_0092		"0092]: CLI no more RAM"
#define iopDbg_Err_0093		"0093]: CLI str file error"
#define iopDbg_Err_0094		"0094]: SND stream error"
#define iopDbg_Err_0095		"0095]: SND dma queue overflow"
#define iopDbg_Err_0096		"0096]: KER WakeupThread error"
#define iopDbg_Err_0097		"0097]: KER SleepThread error"
#define iopDbg_Err_0098		"0098]: CDV Disc error"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    macros
 -----------------------------------------------------------------------------------------------------------------------
 */

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void	iopDBG_MapResetError(int);
void	iopDBG_Err(const char *);

/*
 =======================================================================================================================
    #define iopDbg_M_ErrX(__a, ...) printf(iopDbg_Str_ErrHdr __a "\n", __VA_ARGS__);
 =======================================================================================================================
 */
#define iopDbg_M_Err(__a)	iopDBG_Err(iopDbg_Str_ErrHdr __a "\n")
#define iopDbg_M_ErrX(__a, ...) \
	do \
	{ \
		sprintf(dbg_buffer, iopDbg_Str_ErrHdr __a "\n", __VA_ARGS__); \
		iopDBG_Err(dbg_buffer); \
	} while(0);
#define iopDbg_M_Assert(__a, __b) \
	do \
	{ \
		if(!(__a)) \
		{ \
			iopDbg_M_Err(__b); \
		} \
	} while(0);
#define iopDbg_M_AssertX(__a, __b, ...) \
	do \
	{ \
		if(!(__a)) \
		{ \
			sprintf(dbg_buffer, iopDbg_Str_ErrHdr __b "\n", __VA_ARGS__); \
			iopDBG_Err(dbg_buffer); \
		} \
	} while(0);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern char dbg_buffer[4096];
extern int	iopDBG_gi_LastErrorCode;
extern int	iopDBG_gi_ErrorNumber;
extern int	iopDBG_gi_LastMapErrorCode;
extern int	iopDBG_gi_MapErrorNumber;

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#else	/* __iopDbg_Err__ */
#define iopDbg_M_Err(__a)
#define iopDbg_M_ErrX(__a, ...)
#define iopDbg_M_Assert(__a, __b)
#define iopDbg_M_AssertX(__a, __b, ...)
#define iopDBG_MapResetError(__a)
#endif /* __iopDbg_Err__ */

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __iopDebug_h__ */
#endif /* PSX2_IOP */
