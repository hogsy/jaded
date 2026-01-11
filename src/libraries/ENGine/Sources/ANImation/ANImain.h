/*$T ANImain.h GC! 1.081 08/25/00 14:13:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    This function is call once every frame and compute the animations of all objects
 -----------------------------------------------------------------------------------------------------------------------
 */

void						ANI_RecomputeFlash(struct OBJ_tdst_GameObject_ *, struct ANI_tdst_Anim_ *);
void						ANI_ApplyHierarchyOnFlash(struct OBJ_tdst_GameObject_ *);
void						ANI_RemoveHierarchyOnFlash(struct OBJ_tdst_GameObject_ *);
int							ANI_i_GetVisualOfAICanal(struct OBJ_tdst_GameObject_ *, UCHAR);
void						ANI_ChangeBoneShape(struct OBJ_tdst_GameObject_ *, UCHAR, UCHAR, BOOL);
void						ANI_ChangeShape(struct OBJ_tdst_GameObject_ *, struct ANI_tdst_Shape_ *);
struct OBJ_tdst_GameObject_ *ANI_pst_GetObjectByAICanal(struct OBJ_tdst_GameObject_ *, UCHAR);
struct OBJ_tdst_GameObject_ *ANI_pst_GetObjectByCanal(struct OBJ_tdst_GameObject_ *, UCHAR);
struct OBJ_tdst_GameObject_ *ANI_pst_GetClosestObjCanal(struct OBJ_tdst_GameObject_ *, MATH_tdst_Vector *);
void						ANI_OneCall(struct OBJ_tdst_GameObject_ *);
void						ANI_MainCall(struct WOR_tdst_World_ *);
void						ANI_UpdateFlash(struct OBJ_tdst_GameObject_ *, UCHAR);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
