/*$T SOFTlinear.c GC!1.71 01/26/00 16:49:48 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "GEOmetric/GEOobject.h"
#include "SOFT/SOFTlinear.h"
#include "BASe/BAStypes.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"

#include "BASe/BENch/BENch.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" {
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Project(MATH_tdst_Vector *_pst_Dest, MATH_tdst_Vector *_pst_Src, LONG _l_Number, CAM_tdst_Camera *_pst_Cam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    *pst_Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Last = _pst_Src + _l_Number;

    if (_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
    {
        for(; _pst_Src < pst_Last; _pst_Src++, _pst_Dest++)
        {
            _pst_Dest->z = fOptInv(_pst_Src->z);
            _pst_Dest->x = _pst_Src->x * _pst_Dest->z * _pst_Cam->f_FactorX + _pst_Cam->f_CenterX;
            _pst_Dest->y = _pst_Cam->f_CenterY - _pst_Src->y * _pst_Dest->z * _pst_Cam->f_FactorY;
        }
    }
    else
    {
        for(; _pst_Src < pst_Last; _pst_Src++, _pst_Dest++)
        {
            _pst_Dest->z = fOptInv(_pst_Src->z);
            _pst_Dest->x = _pst_Src->x * _pst_Cam->f_FactorX + _pst_Cam->f_CenterX;
            _pst_Dest->y = _pst_Cam->f_CenterY - _pst_Src->y * _pst_Cam->f_FactorY;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_TransformAndProject
(
    MATH_tdst_Vector    *_pst_Dest,
    MATH_tdst_Vector    *_pst_Src,
    LONG                _l_Number,
    CAM_tdst_Camera     *_pst_Cam
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    *pst_Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Last = _pst_Src + _l_Number;

    if (_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
    {
        for(; _pst_Src < pst_Last; _pst_Src++, _pst_Dest++)
        {
            MATH_TransformVertex(_pst_Dest, _pst_Cam->pst_ObjectToCameraMatrix, _pst_Src);

            if (_pst_Dest->z <= 0)
                _pst_Dest->x = _pst_Dest->y = 0x80000000;
            else
            {
                _pst_Dest->z = fOptInv(_pst_Dest->z);
                _pst_Dest->x = _pst_Dest->x * _pst_Dest->z * _pst_Cam->f_FactorX + _pst_Cam->f_CenterX;
                _pst_Dest->y = _pst_Cam->f_CenterY - _pst_Dest->y * _pst_Dest->z * _pst_Cam->f_FactorY;
            }
        }
    }
    else
    {
        for(; _pst_Src < pst_Last; _pst_Src++, _pst_Dest++)
        {
            MATH_TransformVertex(_pst_Dest, _pst_Cam->pst_ObjectToCameraMatrix, _pst_Src);

            _pst_Dest->z = fOptInv(_pst_Dest->z);
            _pst_Dest->x = _pst_Dest->x * _pst_Cam->f_FactorX + _pst_Cam->f_CenterX;
            _pst_Dest->y = _pst_Cam->f_CenterY - _pst_Dest->y * _pst_Cam->f_FactorY;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_TransformAndProjectIn2Buffers
(
    MATH_tdst_Vector    *_pst_3D,
    MATH_tdst_Vector    *_pst_2D,
    GEO_Vertex          *_pst_Src,
    LONG                _l_Number,
    CAM_tdst_Camera     *_pst_Cam
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_Vertex    *pst_Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Last = _pst_Src + _l_Number;

    if (_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
    {
        for(; _pst_Src < pst_Last; _pst_Src++, _pst_3D++, _pst_2D++)
        {
            MATH_TransformVertex(_pst_3D, _pst_Cam->pst_ObjectToCameraMatrix, VCast(_pst_Src));

            if (_pst_3D->z <= 0)
                _pst_2D->x = _pst_2D->y = 0x80000000;
            else
            {
                _pst_2D->z = fOptInv(_pst_3D->z);
                _pst_2D->x = _pst_3D->x * _pst_2D->z * _pst_Cam->f_FactorX + _pst_Cam->f_CenterX;
                _pst_2D->y = _pst_Cam->f_CenterY - _pst_3D->y * _pst_2D->z * _pst_Cam->f_FactorY;
            }
        }
    }
    else
    {
        for(; _pst_Src < pst_Last; _pst_Src++, _pst_3D++, _pst_2D++)
        {
            MATH_TransformVertex(_pst_3D, _pst_Cam->pst_ObjectToCameraMatrix, VCast(_pst_Src));

            _pst_2D->z = fOptInv(_pst_3D->z);
            _pst_2D->x = _pst_3D->x * _pst_Cam->f_FactorX + _pst_Cam->f_CenterX;
            _pst_2D->y = _pst_Cam->f_CenterY - _pst_3D->y * _pst_Cam->f_FactorY;
        }
    }
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_TransformAndProjectInComputingBuffer
(
    SOFT_tdst_ComputingBuffers  *_pst_CB,
    GEO_Vertex                  *_pst_Src,
    LONG                        _l_Number,
    CAM_tdst_Camera             *_pst_Cam
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_Vertex          *pst_3D;
    SOFT_tdst_Vertex    *pst_2D;
    GEO_Vertex          *pst_Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Last = _pst_Src + _l_Number;
    pst_3D = _pst_CB->ast_3D;
    pst_2D = _pst_CB->ast_2D;

    if (_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
    {
        for(; _pst_Src < pst_Last; _pst_Src++, pst_3D++, pst_2D++)
        {
            MATH_TransformVertex( VCast(pst_3D), _pst_Cam->pst_ObjectToCameraMatrix, VCast(_pst_Src));

            pst_2D->ooz = fOptInv(pst_3D->z);
            pst_2D->x = pst_3D->x * pst_2D->ooz * _pst_Cam->f_FactorX + _pst_Cam->f_CenterX;
            pst_2D->y = _pst_Cam->f_CenterY - pst_3D->y * pst_2D->ooz * _pst_Cam->f_FactorY;
        }
    }
    else
    {
        for(; _pst_Src < pst_Last; _pst_Src++, pst_3D++, pst_2D++)
        {
            MATH_TransformVertex(VCast(pst_3D), _pst_Cam->pst_ObjectToCameraMatrix, VCast(_pst_Src));

            pst_2D->ooz = fOptInv(pst_3D->z);
            pst_2D->x = pst_3D->x * _pst_Cam->f_FactorX + _pst_Cam->f_CenterX;
            pst_2D->y = _pst_Cam->f_CenterY - pst_3D->y * _pst_Cam->f_FactorY;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_Compute_Z
                    (
                        float *DstZ,
                        GEO_Vertex *Src,
                        LONG Number,
                        MATH_tdst_Matrix *p_Matrix)
{
}
#ifdef PSX2_TARGET
/*
-------------------------------
	Src and Dst are aligned
-------------------------------
*/
void SOFT_SerialTranformVertices(GEO_Vertex  *Src , GEO_Vertex  *Dst , ULONG Number , MATH_tdst_Matrix *p_Matrix)
{
	asm ("
.set noreorder
			beq 		a2,zero,NOTHING
			mtsab 		a3,0
			mtsah 		zero,0
			lq			t0,0(a3)
			lq			t1,16(a3)
			lq			t2,32(a3)
			lq			t3,48(a3)
			lq			t4,64(a3)
			lw  		t5,64(a3) /* Load Flags */
			qfsrv 		t0 , t1 , t0
			qfsrv 		t1 , t2 , t1
			qfsrv 		t2 , t3 , t2
			qfsrv 		t3 , t4 , t3
			andi		t5 , t5 , 0x8
			qmtc2		t0 , $vf10
			qmtc2		t1 , $vf11
			qmtc2		t2 , $vf12
			beq			t5 , zero , LOOP
			qmtc2		t3 , $vf13
			vmulw.xyz 	$vf10 , $vf10 , $vf10w
			vmulw.xyz 	$vf11 , $vf11 , $vf11w
			vmulw.xyz 	$vf12 , $vf12 , $vf12w
LOOP:		lqc2		$vf04,0(a0)
			addi		a2,a2,-1
		    vmulax.xyz  ACC, $vf10, $vf04x
		    vmadday.xyz ACC, $vf11, $vf04y
    		vmaddaz.xyz ACC, $vf12, $vf04z
    		vmaddw      $vf05, $vf13, vf00w
			addi		a1,a1,16
			addi		a0,a0,16
    		bne			a2,zero,LOOP
    		sqc2		$vf05,-16(a1)
NOTHING:			
.set reorder
	");
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_TransformInComputingBuffer
(
    SOFT_tdst_ComputingBuffers  *_pst_CB,
    GEO_Vertex                  *Src,
    LONG                        Number,
    CAM_tdst_Camera             *_pst_Cam
)
{
#ifdef PSX2_TARGET
	SOFT_SerialTranformVertices(Src , _pst_CB->ast_3D ,  Number , _pst_Cam->pst_ObjectToCameraMatrix);
#else

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_Vertex          *pst_Last, *pst_3D;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Last = Src + Number;
    pst_3D = _pst_CB->ast_3D;

    for(; Src < pst_Last; Src++, pst_3D++)
    {
        MATH_TransformVertex(VCast(pst_3D), _pst_Cam->pst_ObjectToCameraMatrix, VCast(Src));
    }
#endif	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_ProjectInComputingBuffer
(
    SOFT_tdst_ComputingBuffers  *_pst_CB,
    MATH_tdst_Vector            *_pst_Src,
    LONG                        _l_Number,
    CAM_tdst_Camera             *_pst_Cam
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    SOFT_tdst_Vertex    *pst_2D;
    MATH_tdst_Vector    *pst_Last;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Last = _pst_Src + _l_Number;
    pst_2D = _pst_CB->ast_2D;

    if (_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
    {
        for(; _pst_Src < pst_Last; _pst_Src++, pst_2D++)
        {
            pst_2D->ooz = fOptInv(_pst_Src->z);
            pst_2D->x = _pst_Src->x * pst_2D->ooz * _pst_Cam->f_FactorX + _pst_Cam->f_CenterX;
            pst_2D->y = _pst_Cam->f_CenterY - _pst_Src->y * pst_2D->ooz * _pst_Cam->f_FactorY;
        }
    }
    else
    {
        for(; _pst_Src < pst_Last; _pst_Src++, pst_2D++)
        {
            pst_2D->ooz = fOptInv(_pst_Src->z);
            pst_2D->x = _pst_Src->x * _pst_Cam->f_FactorX + _pst_Cam->f_CenterX;
            pst_2D->y = _pst_Cam->f_CenterY - _pst_Src->y * _pst_Cam->f_FactorY;
        }
    }
}

/*$4
 ***********************************************************************************************************************
    Specular
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    This function will compute the symetric vectors camera - vertex_normale. It will be used by
    each light and some UV calculation.
 =======================================================================================================================
 */
 
#ifdef PSX2_TARGET
extern void asm_SOFT_ComputeSpecularVectors(ULONG Params);
#endif 

void SOFT_ComputeSpecularVectors(GEO_tdst_Object *pst_Obj)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    *VSrc, *VSrcLast, *VSpec;
    MATH_tdst_Vector    CamPos, Local;
    GEO_Vertex          *Point3D;
    MATH_tdst_Matrix    Matrix ONLY_PSX2_ALIGNED(16);
    float               Norm, Norm2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	JADED_PROFILER_START();

    CamPos.x = CamPos.y = CamPos.z = 0.0f;
    MATH_InvertMatrix(&Matrix, GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);
    MATH_TransformVertex(&CamPos, &Matrix, &CamPos);
	GEO_UseNormals(pst_Obj);
    VSrc = pst_Obj->dst_PointNormal;
    VSrcLast = VSrc + pst_Obj->l_NbPoints;
	Point3D = GDI_gpst_CurDD->p_Current_Vertex_List;
    VSpec = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecularField;

    if ( !( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric ) )
    {
        while(VSrc < VSrcLast)
        {
            Local.x = -Point3D->x - CamPos.x;
            Local.y = Point3D->y - CamPos.y;
            Local.z = Point3D->z - CamPos.z;
            //MATH_SubVector(&Local, VCast(Point3D), &CamPos);
            Norm = MATH_f_SqrNormVector(&Local);
            Norm = fOptInvSqrt(Norm);
            Norm2 = -Local.x * VSrc->x + Local.y * VSrc->y + Local.z * VSrc->z;
            Norm2 *= -1.90f;
            VSpec->x = -Norm * (Local.x - VSrc->x * Norm2);
            VSpec->y = Norm * (Local.y + VSrc->y * Norm2);
            VSpec->z = Norm * (Local.z + VSrc->z * Norm2);
            Point3D++;
            VSrc++;
            VSpec++;
        }
    }
    else//*/
    {
#ifdef PSX2_TARGET
	ULONG ASMPARAMS[10];
	ASMPARAMS[0] = (ULONG)Point3D;
	ASMPARAMS[1] = (ULONG)VSrc;
	ASMPARAMS[2] = (ULONG)VSpec;
	ASMPARAMS[3] = (ULONG)&CamPos;
	ASMPARAMS[4] = (ULONG)pst_Obj->l_NbPoints;
	asm_SOFT_ComputeSpecularVectors((ULONG)ASMPARAMS);
#else
        while(VSrc < VSrcLast)
        {
            MATH_SubVector(&Local, VCast(Point3D), &CamPos);
            Norm = MATH_f_SqrNormVector(&Local);
            Norm = fOptInvSqrt(Norm);
            Norm2 = Local.x * VSrc->x + Local.y * VSrc->y + Local.z * VSrc->z;
            Norm2 *= -1.90f;
            VSpec->x = Norm * (Local.x + VSrc->x * Norm2);
            VSpec->y = Norm * (Local.y + VSrc->y * Norm2);
            VSpec->z = Norm * (Local.z + VSrc->z * Norm2);
            Point3D++;
            VSrc++;
            VSpec++;
        }
#endif
    }

    GDI_gpst_CurDD->pst_ComputingBuffers->ul_Flags |= SOFT_Cul_CB_SpecularField;

	JADED_PROFILER_END();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_pst_InitSpecularColors(ULONG NumPoints)
{
	L_memset(GDI_gpst_CurDD_SPR.pst_ComputingBuffers->aul_Specular , 0 , NumPoints << 2);
    GDI_gpst_CurDD->pst_ComputingBuffers->ul_Flags |= SOFT_Cul_CB_SpecularColorField;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Vector *SOFT_pst_GetSpecularVectors()
{
    if(!(GDI_gpst_CurDD->pst_ComputingBuffers->ul_Flags & SOFT_Cul_CB_SpecularField))
    {
        if(!GDI_gpst_CurDD->pst_CurrentGeo) return NULL;
        SOFT_ComputeSpecularVectors(GDI_gpst_CurDD->pst_CurrentGeo);
    }
    return(GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecularField);
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
