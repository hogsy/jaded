/*$T gcSND_Profile.h GC! 1.097 06/03/02 18:23:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE
#ifndef __gcSND_Profile__
#define __gcSND_Profile__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

#ifdef GC_ENABLE_SOUND_PROFILER
void	gcSND_InitProfilerModule(void);
void	gcSND_CloseProfilerModule(void);
void	gcSND_ProfilerEnable(void);
void	gcSND_ProfilerDisable(void);
BOOL    gcSND_b_ProfilerIsEnable(void);
void	gcSND_UpdateProfiler(void);
void	gcSND_ProfilerDisplay(f32);
void    gcSND_ProfilerReset(void);
#else
#define gcSND_InitProfilerModule()
#define gcSND_CloseProfilerModule()
#define gcSND_ProfilerEnable()
#define gcSND_ProfilerDisable()
#define gcSND_UpdateProfiler()
#define gcSND_ProfilerDisplay(l)
#define gcSND_b_ProfilerIsEnable()  (FALSE)
#define gcSND_ProfilerReset()
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __gcSND_Profile__ */
#endif /* _GAMECUBE */
