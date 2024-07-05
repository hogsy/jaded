/*$T ANIrender.h GC! 1.081 04/05/00 14:13:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "GDInterface/GDInterface.h"

void	ANI_Render(OBJ_tdst_GameObject *);
void	ANI_ComputeBoneMatrix(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *, MATH_tdst_Matrix *, BOOL);
