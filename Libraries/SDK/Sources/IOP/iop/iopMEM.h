/*$T iopMEM.h GC! 1.097 02/26/02 11:12:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP

/*$T iopMEM.h GC! 1.097 02/26/02 11:12:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __iopMEM_h__
#define __iopMEM_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define iopMEM_Cte_RAMSize		(2 * 1024 * 1024)
#define iopMEM_Cte_KitRAMSize	(8 * 1024 * 1024)

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

int iopMEM_i_InitModule(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    IOP memory
 -----------------------------------------------------------------------------------------------------------------------
 */
//#define iopMEM_HISTORY

#ifdef iopMEM_HISTORY
#define iopMEM_pv_iopAlloc(_s)			_iopMEM_pv_iopAlloc(_s, __FILE__, __LINE__)
#define iopMEM_pv_iopAllocAlign(_s, _a) _iopMEM_pv_iopAllocAlign(_s, _a, __FILE__, __LINE__)
void	*_iopMEM_pv_iopAlloc(unsigned int _ui_Size, char *file, int line);
void	*_iopMEM_pv_iopAllocAlign(unsigned int _ui_BlockSize, unsigned int _ui_Alignment, char *file, int line);
void    iopMEM_Histo(void);
#else
#define iopMEM_pv_iopAlloc(_s)			_iopMEM_pv_iopAlloc(_s)
#define iopMEM_pv_iopAllocAlign(_s, _a) _iopMEM_pv_iopAllocAlign(_s, _a)
void	*_iopMEM_pv_iopAlloc(unsigned int _ui_Size);
void	*_iopMEM_pv_iopAllocAlign(unsigned int _ui_BlockSize, unsigned int _ui_Alignment);
#define iopMEM_Histo(s)
#endif


void	iopMEM_iopFree(void *);
void	iopMEM_iopFreeAlign(void *);


#define iopMEM_i_iopFreeSize()		QueryMemSize()
#define iopMEM_i_iopMaxFreeSize()	QueryMaxFreeMemSize()

#ifdef _FINAL_
#define iopMEM_Stat()
#else
void    iopMEM_Stat(void);
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    SPU
 -----------------------------------------------------------------------------------------------------------------------
 */

int		iopMEM_i_spuResetMemory(void);
void	*iopMEM_pv_spuFxAlloc(int core);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef __cplusplus
}
#endif
#endif /* __iopMEM_h__ */
#endif
