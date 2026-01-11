/*$T F3Dview_undo.h GC!1.71 01/25/00 16:38:03 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "EDIundo.h"
#include "F3Dframe/F3Dview.h"
#include "SELection/SELection.h"
#include "BASe/BAStypes.h"
#include "GEOmetric/GEOsubobject.h"

/*$4
 ***********************************************************************************************************************
    Selection enumeration callback
 ***********************************************************************************************************************
 */

extern BOOL F3D_b_Undo_Select(SEL_tdst_SelectedItem *, ULONG, ULONG);
extern BOOL F3D_b_Undo_Unselect(SEL_tdst_SelectedItem *, ULONG, ULONG);

/*$4
 ***********************************************************************************************************************
    Undo for any helper modification
 ***********************************************************************************************************************
 */
typedef struct F3D_HelperModif_Data_
{
    MATH_tdst_Matrix    st_Matrix;
    GEO_Vertex          *dst_Point;
} F3D_HelperModif_Data;

class F3D_cl_Undo_HelperModif : public EDI_cl_UndoModif
{
    DECLARE_DYNAMIC( F3D_cl_Undo_HelperModif )

public:
    F3D_cl_View             *mpst_View;
    BOOL                    mb_SavePoint;
    int                     mi_Number;
    int                     mi_Count;

    SEL_tdst_Selection      mst_Selection;
    F3D_HelperModif_Data    *mdst_Data;

public:
    F3D_cl_Undo_HelperModif(F3D_cl_View *_pst_View, BOOL, BOOL _b_Locked = FALSE);
    ~F3D_cl_Undo_HelperModif(void);

    BOOL    b_Do(void);
    BOOL    b_Undo(void);
};

/*$4
 ***********************************************************************************************************************
    Undo for selection operation
 ***********************************************************************************************************************
 */

class F3D_cl_Undo_Selection : public EDI_cl_UndoModif
{
    
    DECLARE_DYNAMIC( F3D_cl_Undo_Selection )

public:
    F3D_cl_View         *mpst_View;
    SEL_tdst_Selection  mst_Selection;
public:
    F3D_cl_Undo_Selection (F3D_cl_View *_pst_View, BOOL _b_Locked = FALSE);
    ~F3D_cl_Undo_Selection(void);

    void    SwapSelection(void);

    BOOL    b_Do(void);
    BOOL    b_Undo(void);
};

/*$4
 ***********************************************************************************************************************
    Undo for selection destruction
 ***********************************************************************************************************************
 */

class F3D_cl_Undo_Destruction : public EDI_cl_UndoModif
{
    DECLARE_DYNAMIC( F3D_cl_Undo_Destruction )

public:
    F3D_cl_View         *mpst_View;
    SEL_tdst_Selection  mst_Selection;
    MATH_tdst_Matrix    *mdst_Matrix;
    ULONG               *mdul_LinkDest;
public:
    F3D_cl_Undo_Destruction (F3D_cl_View *, BOOL _b_Locked = FALSE);
    ~F3D_cl_Undo_Destruction(void);

    BOOL    b_Do(void);
    BOOL    b_Undo(void);
};

/*$4
 ***********************************************************************************************************************
    Undo for displacement of UVs
 ***********************************************************************************************************************
 */
class F3D_cl_Undo_UVMoving : public EDI_cl_UndoModif
{
    DECLARE_DYNAMIC( F3D_cl_Undo_UVMoving )

public:
    F3D_cl_View         *mpst_View;
    int                 mi_NbObj;
    ULONG               mul_ObjKey[4];
    GEO_tdst_UV         *mpst_UV[4];
    
public:
    F3D_cl_Undo_UVMoving(F3D_cl_View *, int, GEO_tdst_Object **, GEO_tdst_UV **, BOOL _b_Locked = FALSE);
    ~F3D_cl_Undo_UVMoving(void);

    BOOL    b_Do(void);
    BOOL    b_Undo(void);
};

/*$4
 ***********************************************************************************************************************
    Undo for turning an edge
 ***********************************************************************************************************************
 */
class F3D_cl_Undo_TurnEdge : public EDI_cl_UndoModif
{
    DECLARE_DYNAMIC( F3D_cl_Undo_TurnEdge )

public:
    F3D_cl_View         *mpst_View;
    GEO_tdst_Object     *mpst_Obj;
    int                 mai_Old[2];
    int                 mai_New[2];
    
public:
    F3D_cl_Undo_TurnEdge(F3D_cl_View *, GEO_tdst_Object *, struct GEO_tdst_SubObject_ETurnData_ *);
    ~F3D_cl_Undo_TurnEdge(void);

    BOOL    b_Do(void);
    BOOL    b_Undo(void);
};


/*$4
 ***********************************************************************************************************************
    Undo for face flipping
 ***********************************************************************************************************************
 */
class F3D_cl_Undo_FlipFace: public EDI_cl_UndoModif
{
    DECLARE_DYNAMIC( F3D_cl_Undo_FlipFace)

public:
    F3D_cl_View         *mpst_View;
    
public:
    F3D_cl_Undo_FlipFace(F3D_cl_View *, BOOL _b_Locked = FALSE);
    ~F3D_cl_Undo_FlipFace(void);

    BOOL    b_Do(void);
    BOOL    b_Undo(void);
};

/*$4
 ***********************************************************************************************************************
    Undo for cutting an edge
 ***********************************************************************************************************************
 */
class F3D_cl_Undo_CutEdge : public EDI_cl_UndoModif
{
    DECLARE_DYNAMIC( F3D_cl_Undo_CutEdge )
public:
    F3D_cl_View                 *mpst_View;
    GEO_tdst_Object             *mpst_Obj;
    GEO_tdst_SubObject_ECutData mst_CutData;
    
public:
    F3D_cl_Undo_CutEdge(F3D_cl_View *, GEO_tdst_Object *, struct GEO_tdst_SubObject_ECutData_ *);
    ~F3D_cl_Undo_CutEdge(void);

    BOOL    b_Do(void);
    BOOL    b_Undo(void);
};

/*$4
 ***********************************************************************************************************************
    Undo for selection of sub object
 ***********************************************************************************************************************
 */
typedef struct F3D_tdst_SubObjectSel_
{
    GEO_tdst_Object                 *pst_Obj;
    LONG                            l_OldNumber;
    USHORT                          *duw_OldSel;
    LONG                            l_NewNumber;
    USHORT                          *duw_NewSel;
    struct F3D_tdst_SubObjectSel_   *pst_Next;
} F3D_tdst_SubObjectSel;

class F3D_cl_Undo_SelSubObject : public EDI_cl_UndoModif
{
    DECLARE_DYNAMIC( F3D_cl_Undo_SelSubObject )

public:
    F3D_cl_View                 *mpst_View;
    F3D_tdst_SubObjectSel       *mpst_Data;
    int                         mi_Type;

public:
    F3D_cl_Undo_SelSubObject(F3D_cl_View *, int, BOOL _b_Locked = FALSE);
    ~F3D_cl_Undo_SelSubObject(void);

    void    BeginAddObj( GEO_tdst_Object * );
    void    EndAddObj( GEO_tdst_Object * );

    F3D_tdst_SubObjectSel *pst_GetStruct( GEO_tdst_Object * );

    BOOL    b_Do(void);
    BOOL    b_Undo(void);
};

/*$4
 ***********************************************************************************************************************
    Undo for geometry modification (save all geometry : high memory cost)
    can be used temporary to make a fast undo for modification on one geometry
 ***********************************************************************************************************************
 */
class F3D_cl_Undo_GeoModif : public EDI_cl_UndoModif
{
    DECLARE_DYNAMIC( F3D_cl_Undo_GeoModif )

public:
    F3D_cl_View         *mpst_View;
    ULONG               mul_ObjKey;
    GEO_tdst_Object     *mpst_Obj;
    char                *mpc_Sel;
    ULONG               mul_GaoKey;
    ULONG               *mpul_GaoRLI;
    void                *mpst_GaoMorphData;
    LONG                ml_GaoNbMorphData;

#ifdef JADEFUSION
    OBJ_tdst_GameObject* mpst_GO;
#endif
public:
    F3D_cl_Undo_GeoModif(F3D_cl_View *, GEO_tdst_Object *, OBJ_tdst_GameObject *_pst_Gao = NULL, BOOL _b_Locked = FALSE);
    ~F3D_cl_Undo_GeoModif(void);

    BOOL    b_Do(void);
    BOOL    b_Undo(void);
};

/*$4
 ***********************************************************************************************************************
    Undo for geometry modification on multiple object (save all geometry : high memory cost)
    can be used temporary to make a fast undo for modification on severall geometry
 ***********************************************************************************************************************
 */
class F3D_cl_Undo_MultipleGeoModif : public EDI_cl_UndoModif
{
    DECLARE_DYNAMIC( F3D_cl_Undo_MultipleGeoModif )

public:
    F3D_cl_View             *mpst_View;
    LONG                    ml_Number;
    LONG                    ml_Count;
    F3D_cl_Undo_GeoModif    **mdpcl_GeoModif;
    
public:
    F3D_cl_Undo_MultipleGeoModif(F3D_cl_View *, BOOL _b_Locked = FALSE);
    ~F3D_cl_Undo_MultipleGeoModif(void);

    BOOL    b_Do(void);
    BOOL    b_Undo(void);

    GEO_tdst_Object *GetOldGeo( GEO_tdst_Object *, void ** );
};


/*$4
 ***********************************************************************************************************************
    Undo for grid édition
 ***********************************************************************************************************************
 */
class F3D_cl_Undo_EditGrid : public EDI_cl_UndoModif
{
    DECLARE_DYNAMIC( F3D_cl_Undo_EditGrid )
public:
    F3D_cl_View                 *mpst_View;
	struct GRID_tdst_World_		*mpst_Grid;
	int							mi_XMin;
	int							mi_XMax;
	int							mi_YMin;
	int							mi_YMax;
	char						*mc_PreviousCapa;
	char						*mc_NewCapa;
    
public:
    F3D_cl_Undo_EditGrid(F3D_cl_View *, struct GRID_tdst_World_ *, int, int, int, int, char *);
    ~F3D_cl_Undo_EditGrid(void);

    BOOL    b_Do(void);
    BOOL    b_Undo(void);
};


#endif
