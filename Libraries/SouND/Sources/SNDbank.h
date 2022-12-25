/*$T SNDbank.h GC 1.138 12/05/03 12:09:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDbank_h__
#define __SNDbank_h__
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

ULONG					SND_ul_CallbackUnknownBank(ULONG);
void					SND_FreeMainStruct(OBJ_tdst_GameObject *_pst_GO);
void					SND_MainFreeUnion(union SND_tdun_Main_ *_pMain);
struct SND_tdst_Bank_	*SND_p_MainGetBank(union SND_tdun_Main_ *_pMain);

ULONG					SND_ul_CallbackLoadBank(ULONG);
void					SND_BankUnload(SND_tdst_Bank *pBank);

ULONG					SND_ul_CallbackLoadMetabank(ULONG);
void					SND_MetabankUnload(SND_tdst_Metabank *pMetabank);

void					SND_MergeBank
						(
							struct OBJ_tdst_GameObject_ *,
							struct OBJ_tdst_GameObject_ *,
							int _i_Begin,
							int _i_End,
							int _i_Overwrite
						);
void					SND_RestoreGaoBank(struct OBJ_tdst_GameObject_ *);
void					SND_BankRestore(struct SND_tdst_Bank_*);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDbank_h__ */
