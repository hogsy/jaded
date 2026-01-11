/*$T SPU.h GC! 1.097 01/18/02 15:31:13 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SPU_h__
#define __SPU_h__
#ifdef PSX2_TARGET
#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define SPU_M_GetAddr(__a)	(void *) ((unsigned int) __a & 0x00FFFFFF)

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

int			SPU_i_InitModule(void);
void		*SPU_pv_Alloc(unsigned int _ui_Size);
void		SPU_Free(void *_pv);
void		*SPU_pv_FxAlloc(int core);
void		*SPU_pv_StreamAlloc(unsigned int _ui_Size);
void		SPU_StreamFree(void *_pv);
void		*SPU_pv_LoadingSoundAlloc(unsigned int _ui_Size);
void		SPU_LoadingSoundFree(void *_pv);
void		SPU_FreeAll(void);

#ifndef _FINAL_
int			SPU_i_GetVoiceSize(void);
int			SPU_i_GetFxSize(void);
int			SPU_i_GetSysSize(void);
int			SPU_i_GetFragmentSize(void);
int         SPU_i_GetFreeSize(void);
#endif

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

#ifndef _FINAL_
extern int	SPU_gi_StreamAllocSize;
extern int	SPU_gi_AllocSize;
extern int	SPU_gi_AllocClusterSize;
extern int	SPU_gi_AllocMaxSize;
extern int	SPU_gi_AllocMaxClusterSize;
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
extern "C"
{
#endif
#endif /* PSX2_TARGET */
#endif /* __SPU_h__ */
