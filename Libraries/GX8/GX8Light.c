/* Dx8Light.c */

#include "Gx8light.h"

#include <D3D8.h>
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "GDInterface/GDInterface.h"
#include "Gx8init.h"
#include "Gx8renderstate.h"
#include "ENGine/Sources/COLlision/COLmain.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/INTersection/INTmain.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "Gx8VertexShaders.h"

#include <assert.h>

// define d'accès aux champs couleur
#define MGETR(color) (((float)(((color) & 0x000000ff) >>  0)) / 255.f)
#define MGETG(color) (((float)(((color) & 0x0000ff00) >>  8)) / 255.f)
#define MGETB(color) (((float)(((color) & 0x00ff0000) >> 16)) / 255.f)
#define MGETA(color) (((float)(((color) & 0xff000000) >> 24)) / 255.f)

#define GX8_C_MAX_HARDWARE_LIGHTS_COUNT 8
static unsigned char gs_ucEnabledLightsCount = 0;

static struct {
    MATH_tdst_Vector  stNodePosition;
    unsigned char     ucLightsCount;
    LIGHT_tdst_Light *a_p_st_EnabledLights[GX8_C_MAX_HARDWARE_LIGHTS_COUNT];
    MATH_tdst_Matrix  a_stLightMatrices[GX8_C_MAX_HARDWARE_LIGHTS_COUNT];
} gs_stLightSet;

void Gx8_vInitializeHWLightsData(void)
{
    gs_stLightSet.ucLightsCount = 0;
    gs_ucEnabledLightsCount = 0;
}

/*
 =======================================================================================================================
    Light set buffering (right now there's not much point in using a light set,
    but this could change if we manage to bufferize whole transparent objects
    because then we will have to bufferize which lights lit them as well...
 =======================================================================================================================
 */
void Gx8_vBeginNewLightSet(OBJ_tdst_GameObject *_pst_Node)
{
    gs_stLightSet.stNodePosition = _pst_Node->pst_GlobalMatrix->T; // needed if there are omniconsts
    gs_stLightSet.ucLightsCount = 0;
}

void Gx8_vAddLightToLightSet(LIGHT_tdst_Light *pst_Light, MATH_tdst_Matrix *p_stLightMatrix)
{
    extern MATH_tdst_Matrix LIGHT_gst_LP;
    gs_stLightSet.a_p_st_EnabledLights[gs_stLightSet.ucLightsCount] = pst_Light;
    gs_stLightSet.a_stLightMatrices[gs_stLightSet.ucLightsCount] = *p_stLightMatrix;//LIGHT_gst_LP;
    
    gs_stLightSet.ucLightsCount++;
}

_inline void Gx8_vApplyLightFromSetLight(Gx8_tdst_SpecificData *pSD,unsigned char ucLightIndex)
{
    D3DLIGHT8 stD3D8Light;
    D3DCOLORVALUE stLightColor;
    const LIGHT_tdst_Light *pst_Light = gs_stLightSet.a_p_st_EnabledLights[ucLightIndex];
	MATH_tdst_Matrix *pstMat = &gs_stLightSet.a_stLightMatrices[ucLightIndex];

    assert(pst_Light->ul_Flags & LIGHT_Cul_LF_UseColor);

    stLightColor.r = MGETR(pst_Light->ul_Color);
    stLightColor.g = MGETG(pst_Light->ul_Color);
    stLightColor.b = MGETB(pst_Light->ul_Color);
    stLightColor.a = 0.0f;

#pragma message("TODO: dynamic alpha lighting ?")
    // You can't do it with hardware lights because they don't use alpha.
    // Instead you'll have to consider vertex shaders (ouch) or CPU computation + VB locking (ouch again)

	if (pst_Light->ul_Flags & LIGHT_Cul_LF_Absorb)
	{
        if (!(pst_Light->ul_Flags & LIGHT_Cul_LF_OmniConst))
            memset(&stLightColor, 0, sizeof(D3DCOLORVALUE));
		stLightColor.r = -stLightColor.r;
		stLightColor.g = -stLightColor.g;
		stLightColor.b = -stLightColor.b;
		//stLightColor.a = -stLightColor.a;
	}

	switch (pst_Light->ul_Flags & LIGHT_Cul_LF_Type)
	{
	case LIGHT_Cul_LF_Direct :
        {
		    stD3D8Light.Type = D3DLIGHT_DIRECTIONAL;
            stD3D8Light.Direction = *(D3DVECTOR *)MATH_pst_GetYAxis (pstMat);


			stD3D8Light.Direction.x= - stD3D8Light.Direction.x;
			stD3D8Light.Direction.y= - stD3D8Light.Direction.y;
			stD3D8Light.Direction.z= - stD3D8Light.Direction.z;


		    stD3D8Light.Range = 0.0f;       // Max 5 Feb 2003. Likely this is not used.
        } break;
	case LIGHT_Cul_LF_Spot :
        {
            LIGHT_tdst_Spot *pst_Spot = (LIGHT_tdst_Spot *)(&pst_Light->st_Spot);
		    stD3D8Light.Type = D3DLIGHT_SPOT;
            if (MATH_b_TestScaleType(pstMat))
            {
		        stD3D8Light.Range = pst_Spot->f_Far * .333f * (pstMat->Sx + pstMat->Sy + pstMat->Sz);
            }
            else
            {
		        stD3D8Light.Range = pst_Spot->f_Far;
            }
		    stD3D8Light.Theta = pst_Spot->f_LittleAlpha;
		    stD3D8Light.Phi = pst_Spot->f_BigAlpha;

		    // direction
            stD3D8Light.Direction = *(D3DVECTOR *)MATH_pst_GetYAxis (pstMat);

		    // position
		    stD3D8Light.Position = *(D3DVECTOR *)MATH_pst_GetTranslation (pstMat);
        } break;
	case LIGHT_Cul_LF_Omni :
        {
            LIGHT_tdst_Omni *pst_Omni = (LIGHT_tdst_Omni *)(&pst_Light->st_Omni);
		    stD3D8Light.Type = D3DLIGHT_POINT;
            if (MATH_b_TestScaleType(pstMat))
            {
		        stD3D8Light.Range = pst_Omni->f_Far * .333f * (pstMat->Sx + pstMat->Sy + pstMat->Sz);
            }
            else
            {
		        stD3D8Light.Range = pst_Omni->f_Far;
            }

		    // position
		    stD3D8Light.Position = *(D3DVECTOR *)MATH_pst_GetTranslation (pstMat);
        } break;
    default:
        return; // unknown/unsupported light type
        break;
	}

	/*OLD stD3D8Light.Falloff = 1.0f;
	stD3D8Light.Attenuation0 = 1.0f;
	stD3D8Light.Attenuation1 = 0.f;
	if (stD3D8Light.Range<=0.01f)
		stD3D8Light.Range = 0.01f;
	stD3D8Light.Attenuation2 = 9.f/(stD3D8Light.Range*stD3D8Light.Range);

	*/

	stD3D8Light.Falloff = 1.0f;
	stD3D8Light.Attenuation0 = 1.0f;
	stD3D8Light.Attenuation1 = 0.f;
	stD3D8Light.Attenuation2 = 0.0f;
	
	


#pragma message("TODO: hardware specular lights")
    //if (pst_Light->ul_Flags & LIGHT_Cul_LF_Specular)
	stD3D8Light.Specular.r = 
	stD3D8Light.Specular.g = 
	stD3D8Light.Specular.b = 
	stD3D8Light.Specular.a = 0.0f;

	if (pst_Light->ul_Flags & LIGHT_Cul_LF_Paint)
	{
		stD3D8Light.Ambient.r = stLightColor.r;
		stD3D8Light.Ambient.g = stLightColor.g;
		stD3D8Light.Ambient.b = stLightColor.b;
		stD3D8Light.Ambient.a = 0.0f;//stLightColor.a;
		stD3D8Light.Diffuse.r = 
		stD3D8Light.Diffuse.g = 		
		stD3D8Light.Diffuse.b = 		
		stD3D8Light.Diffuse.a = 0.0f;		
	}
    else
    {
        if (((pst_Light->ul_Flags & LIGHT_Cul_LF_Type)==LIGHT_Cul_LF_Omni)&&(pst_Light->ul_Flags & LIGHT_Cul_LF_OmniConst)) // caution ! the paint flag disables the omniconst flag (it has priority)
	    {

		    /* intensité constante sur tout l'objet, mais prise en compte des normales */

		    MATH_tdst_Vector	st_LightToPoint;
            float				f_ConstFactor;
		    float f_Dist;



            MATH_SubVector(&st_LightToPoint, &gs_stLightSet.stNodePosition, &pstMat->T);

			//CArlone..check for null vector
			if(st_LightToPoint.x==0.0f && st_LightToPoint.y==0.0f && st_LightToPoint.z==0.0f)
			{
				//If no light vector, return
				return;
			}


            f_Dist = MATH_f_NormVector( &st_LightToPoint );
            if (MATH_b_TestScaleType(pstMat))
            {
		        f_Dist /= .333f * (pstMat->Sx + pstMat->Sy + pstMat->Sz); // Doing this for the second time. Sigh.
            }

            if (f_Dist >= pst_Light->st_Omni.f_Far)
            {
                f_ConstFactor = 0.0f;
            }
            else if (f_Dist <= pst_Light->st_Omni.f_Near)
            {
                f_ConstFactor = 1.0f;
            }
            else
            {
                f_ConstFactor = ( pst_Light->st_Omni.f_Far - f_Dist ) / (pst_Light->st_Omni.f_Far - pst_Light->st_Omni.f_Near);
            }

            stLightColor.r *= f_ConstFactor;
            stLightColor.g *= f_ConstFactor;
            stLightColor.b *= f_ConstFactor;
            //stLightColor.a *= f_ConstFactor;

            // Turn spheric into a directional light :
		    stD3D8Light.Type = D3DLIGHT_DIRECTIONAL;
            MATH_DivVector((MATH_tdst_Vector *)&stD3D8Light.Direction, &st_LightToPoint, f_Dist);


			


			
	    }
		//NEW CARLONE NO OMNI
		else	
		if((pst_Light->ul_Flags & LIGHT_Cul_LF_Type)==LIGHT_Cul_LF_Omni)
		{
			//return;
		}
		//END NEW CARLONE
			
		
	

        stD3D8Light.Diffuse = stLightColor;
	    stD3D8Light.Ambient.r = 
	    stD3D8Light.Ambient.g = 
	    stD3D8Light.Ambient.b = 
	    stD3D8Light.Ambient.a = 0.0f;
    }

	
	IDirect3DDevice8_SetLight (pSD->mp_D3DDevice, ucLightIndex, &stD3D8Light);
	IDirect3DDevice8_LightEnable (pSD->mp_D3DDevice, ucLightIndex, 1/*TRUE*/);
}

void Gx8_vApplyLightSet(void)
{
    Gx8_tdst_SpecificData *pSD = GX8_M_SD(GDI_gpst_CurDD);
    char i;

    Gx8_RS_Lighting( pSD, TRUE );

    for (i = 0; i < gs_stLightSet.ucLightsCount; i++)
    {
        // set and enable hardware lights
        Gx8_vApplyLightFromSetLight(pSD, i);
    }
    // turn off remaining lights
    while (i < gs_ucEnabledLightsCount)
    {
		IDirect3DDevice8_LightEnable (pSD->mp_D3DDevice, i, FALSE);
        i++;
    }
    gs_ucEnabledLightsCount = gs_stLightSet.ucLightsCount;

    /*
    // test
    //if (FALSE)
    {
        D3DLIGHT8 stD3D8Light;

        // common stuff
        stD3D8Light.Type = D3DLIGHT_DIRECTIONAL;
        memset(&stD3D8Light.Ambient, 0, sizeof(D3DCOLORVALUE));
        memset(&stD3D8Light.Specular, 0, sizeof(D3DCOLORVALUE));

        stD3D8Light.Diffuse.r = 1.0f;
        stD3D8Light.Diffuse.g = 0.0f;
        stD3D8Light.Diffuse.b = 0.0f;
        stD3D8Light.Diffuse.a = 0.0f;
        stD3D8Light.Direction.x = 1.0f;
        stD3D8Light.Direction.y = 0.0f;
        stD3D8Light.Direction.z = 0.0f;
        IDirect3DDevice8_SetLight(pSD->mp_D3DDevice, 0, &stD3D8Light);
        IDirect3DDevice8_LightEnable (pSD->mp_D3DDevice, 0, 1);

        stD3D8Light.Diffuse.r = 0.0f;
        stD3D8Light.Diffuse.g = 1.0f;
        stD3D8Light.Diffuse.b = 0.0f;
        stD3D8Light.Diffuse.a = 0.0f;
        stD3D8Light.Direction.x = 0.0f;
        stD3D8Light.Direction.y = 1.0f;
        stD3D8Light.Direction.z = 0.0f;
        IDirect3DDevice8_SetLight(pSD->mp_D3DDevice, 1, &stD3D8Light);
        IDirect3DDevice8_LightEnable (pSD->mp_D3DDevice, 1, 1);

        stD3D8Light.Diffuse.r = 0.0f;
        stD3D8Light.Diffuse.g = 0.0f;
        stD3D8Light.Diffuse.b = 1.0f;
        stD3D8Light.Diffuse.a = 0.0f;
        stD3D8Light.Direction.x = 0.0f;
        stD3D8Light.Direction.y = 0.0f;
        stD3D8Light.Direction.z = 1.0f;
        IDirect3DDevice8_SetLight(pSD->mp_D3DDevice, 2, &stD3D8Light);
        IDirect3DDevice8_LightEnable (pSD->mp_D3DDevice, 2, 1);
    }/**/
}

void Gx8_vApplyLightSetForLitAlphaShaders(void)
{
    unsigned char i;
    DWORD dwParallelIndex = 0xFFFFFFFF;
    DWORD dwSphericalIndex = 0xFFFFFFFF;
//    LPDIRECT3DDEVICE8 mp_D3DDevice = GX8_M_SD(GDI_gpst_CurDD)->mp_D3DDevice;
    Gx8_tdstLightForShaders stLightForShaders;
    
    // find out the light indices
    for (i = 0; i < gs_stLightSet.ucLightsCount; i++)
    {




        const LIGHT_tdst_Light *pst_Light = gs_stLightSet.a_p_st_EnabledLights[i];
        
		
		
		// set and enable hardware lights
    	if (pst_Light->ul_Flags & LIGHT_Cul_LF_Paint)
            continue;

        switch (pst_Light->ul_Flags & LIGHT_Cul_LF_Type)
        {
        case LIGHT_Cul_LF_Direct:
            {
                dwParallelIndex = i;
                if (dwSphericalIndex != 0xFFFFFFFF)
                    break;
            } break;
	    case LIGHT_Cul_LF_Omni :
            {
                if (pst_Light->ul_Flags & LIGHT_Cul_LF_OmniConst)
                {
                    dwParallelIndex = i;
                    if (dwSphericalIndex != -1)
                        break;
                }
                else
                {
                    dwSphericalIndex = i;
                    if (dwParallelIndex != -1)
                        break;
                }
            } break;
        default:
            break;
        }
    }

    if (((dwParallelIndex != -1) || (dwParallelIndex != -1)) && !(GX8_M_SD(GDI_gpst_CurDD)->eStateFlags & Gx8_C_InvertLocalMatrixSet))
    {
        MATH_tdst_Matrix stJadeMatrix;
        // Compute invert world matrix, in order to put the lights in the object's system.

        MATH_MakeOGLMatrix(&stJadeMatrix, GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);
        D3DXMatrixInverse(&GX8_M_SD(GDI_gpst_CurDD)->stInvertWorldMatrix, NULL, (D3DXMATRIX *)&stJadeMatrix);
    }

    if (dwParallelIndex != -1)
    {
        MATH_tdst_Matrix *pstMat = &gs_stLightSet.a_stLightMatrices[dwParallelIndex];
        const LIGHT_tdst_Light *pst_Light = gs_stLightSet.a_p_st_EnabledLights[dwParallelIndex];

        if ((pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct)
        {
            stLightForShaders.stParallelLightColor.r = MGETR(pst_Light->ul_Color);
            stLightForShaders.stParallelLightColor.g = MGETG(pst_Light->ul_Color);
            stLightForShaders.stParallelLightColor.b = MGETB(pst_Light->ul_Color);

            *(D3DVECTOR *)&stLightForShaders.stParallelLightDirection = *(D3DVECTOR *)MATH_pst_GetYAxis (pstMat);

			//Carlone...transform direction.
			stLightForShaders.stParallelLightDirection.x= -stLightForShaders.stParallelLightDirection.x;
			stLightForShaders.stParallelLightDirection.y= -stLightForShaders.stParallelLightDirection.y;
			stLightForShaders.stParallelLightDirection.z= -stLightForShaders.stParallelLightDirection.z;

        }
        else // omni const
        {
		    MATH_tdst_Vector	st_LightToPoint;
            float				f_ConstFactor;
		    float f_Dist;

            MATH_SubVector(&st_LightToPoint, &gs_stLightSet.stNodePosition, &pstMat->T);
            f_Dist = MATH_f_NormVector( &st_LightToPoint );
            if (MATH_b_TestScaleType(pstMat))
            {
		        f_Dist /= .333f * (pstMat->Sx + pstMat->Sy + pstMat->Sz); // Doing this for the second time. Sigh.
            }

            if (f_Dist >= pst_Light->st_Omni.f_Far)
            {
                f_ConstFactor = 0.0f;
            }
            else if (f_Dist <= pst_Light->st_Omni.f_Near)
            {
                f_ConstFactor = 1.0f;
            }
            else
            {
                f_ConstFactor = ( pst_Light->st_Omni.f_Far - f_Dist ) / (pst_Light->st_Omni.f_Far - pst_Light->st_Omni.f_Near);
            }

            stLightForShaders.stParallelLightColor.r = MGETR(pst_Light->ul_Color) * f_ConstFactor;
            stLightForShaders.stParallelLightColor.g = MGETG(pst_Light->ul_Color) * f_ConstFactor;
            stLightForShaders.stParallelLightColor.b = MGETB(pst_Light->ul_Color) * f_ConstFactor;

	        if (pst_Light->ul_Flags & LIGHT_Cul_LF_Absorb)
            {
                stLightForShaders.stParallelLightColor.r = -stLightForShaders.stParallelLightColor.r;
                stLightForShaders.stParallelLightColor.g = -stLightForShaders.stParallelLightColor.g;
                stLightForShaders.stParallelLightColor.b = -stLightForShaders.stParallelLightColor.b;
            }

            MATH_DivVector((MATH_tdst_Vector *)&stLightForShaders.stParallelLightDirection, &st_LightToPoint, f_Dist);


			//Carlone...transform direction.
			stLightForShaders.stParallelLightDirection.x= -stLightForShaders.stParallelLightDirection.x;
			stLightForShaders.stParallelLightDirection.y= -stLightForShaders.stParallelLightDirection.y;
			stLightForShaders.stParallelLightDirection.z= -stLightForShaders.stParallelLightDirection.z;


        }

        // Set light in the object local system
        D3DXVec3TransformNormal((D3DVECTOR *)&stLightForShaders.stParallelLightDirection,
                                (D3DVECTOR *)&stLightForShaders.stParallelLightDirection,
                                &GX8_M_SD(GDI_gpst_CurDD)->stInvertWorldMatrix); // supposes that no scaling occurs
    }
    else
    {
        memset(&stLightForShaders.stParallelLightColor, 0, sizeof(D3DCOLORVALUE));
    }
    if (dwSphericalIndex != -1)
    {
        const LIGHT_tdst_Light *pst_Light = gs_stLightSet.a_p_st_EnabledLights[dwSphericalIndex];
        const LIGHT_tdst_Omni *pst_Omni = (LIGHT_tdst_Omni *)(&pst_Light->st_Omni);
        MATH_tdst_Matrix *pstMat = &gs_stLightSet.a_stLightMatrices[dwSphericalIndex];
        float fRange;

        if (MATH_b_TestScaleType(pstMat))
        {
		    fRange = pst_Omni->f_Far * .333f * (pstMat->Sx + pstMat->Sy + pstMat->Sz);
        }
        else
        {
		    fRange = pst_Omni->f_Far;
        }
	    if (fRange<=0.01f)
		    fRange = 0.01f;

		// position
        D3DXVec3Transform((D3DXVECTOR4 *)&stLightForShaders.stSphericalLightPosition,
                          (D3DVECTOR *)MATH_pst_GetTranslation (pstMat),
                          &GX8_M_SD(GDI_gpst_CurDD)->stInvertWorldMatrix); // supposes that no scaling occurs

	    stLightForShaders.stSphericalLightAttenuation.x = 1.0f;
	    stLightForShaders.stSphericalLightAttenuation.y = 0.f;
	    stLightForShaders.stSphericalLightAttenuation.z = 9.f/(fRange*fRange);

	    if (pst_Light->ul_Flags & LIGHT_Cul_LF_Absorb)
        {
            stLightForShaders.stSphericalLightColor.r = -MGETR(pst_Light->ul_Color);
            stLightForShaders.stSphericalLightColor.g = -MGETG(pst_Light->ul_Color);
            stLightForShaders.stSphericalLightColor.b = -MGETB(pst_Light->ul_Color);
        }
        else
        {
            stLightForShaders.stSphericalLightColor.r = MGETR(pst_Light->ul_Color);
            stLightForShaders.stSphericalLightColor.g = MGETG(pst_Light->ul_Color);
            stLightForShaders.stSphericalLightColor.b = MGETB(pst_Light->ul_Color);
        }
    }
    else
    {
        memset(&stLightForShaders.stSphericalLightColor, 0, sizeof(D3DCOLORVALUE));
    }

    Gx8_vSetLightsForShaderConstants(&stLightForShaders);
}

void LIGHT_TurnOffObjectLighting(void)
{
    Gx8_tdst_SpecificData *pSD = GX8_M_SD(GDI_gpst_CurDD);

    while (gs_ucEnabledLightsCount > 0)
    {
        gs_ucEnabledLightsCount--;
		IDirect3DDevice8_LightEnable (pSD->mp_D3DDevice, gs_ucEnabledLightsCount, FALSE);
    }

    Gx8_RS_Lighting(pSD, FALSE);
    GDI_gpst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseLitAlphaShader;
}

void LIGHT_TurnOffAllLights(void)
{
    Gx8_tdst_SpecificData *pSD = GX8_M_SD(GDI_gpst_CurDD);

    while (gs_ucEnabledLightsCount > 0)
    {
        gs_ucEnabledLightsCount--;
		IDirect3DDevice8_LightEnable (pSD->mp_D3DDevice, gs_ucEnabledLightsCount, FALSE);
    }

    Gx8_RS_Lighting(pSD, 1);
	Gx8_SetGlobalAmbient(pSD,0);
    GDI_gpst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseLitAlphaShader;
}

void LIGHT_SetLighting(BOOL bSet)
{
    Gx8_tdst_SpecificData *pSD = GX8_M_SD(GDI_gpst_CurDD);
    Gx8_RS_Lighting(pSD, (bSet==TRUE));
}

#define LIGHT_ObjSkinFlag   (OBJ_C_IdentityFlag_Bone | OBJ_C_IdentityFlag_Hierarchy | OBJ_C_IdentityFlag_AdditionalMatrix)
int LIGHT_SendObjectToLight_HW( LIGHT_tdst_List *_pst_LightList, OBJ_tdst_GameObject *_pst_Node, ULONG _ulValidityMask)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DWORD                   dwAmbient;
    BOOL                    bObjectUsesRLIColors;
    BOOL                    bObjectUsesRLIAlphas;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    extern OBJ_tdst_GameObject    *LIGHT_gpst_Bump;
	Gx8_tdst_SpecificData *pSD;


    LIGHT_gpst_Bump = NULL;





	//THIS TEST HAS TO BE CHECKED!!!!!!///////////////////////////////////////////////////////////////////////////////

    if ( _pst_Node->pst_Base->pst_Visu->IndexInMultipleVBList != 0 )
    {
#pragma message("TODO: find out materials that don't actually use diffuse") // cf rouille is a black texture so diffuse is actually not used...
        bObjectUsesRLIColors = (_ulValidityMask & (MAT_ValidateMask_RLI+MAT_ValidateMask_Diffuse)) != 0;
        bObjectUsesRLIAlphas = (_ulValidityMask & MAT_ValidateMask_RLIAlpha) != 0;
       
		
		// In fact we want to know if the object RLIs and uses them.
        // If it doesn't use them we don't bother, it saves time...
    }
    else
    {
        bObjectUsesRLIColors = FALSE;
        bObjectUsesRLIAlphas = (_ulValidityMask & MAT_ValidateMask_RLIAlpha) != 0;
	//	bObjectUsesRLIAlphas = FALSE;
    }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////






	//Carlone...create object's normals////////////////////////////////////
	//GEO_UseNormals((GEO_tdst_Object *) _pst_Node->pst_Base->pst_Visu->pst_Object);
	///////////////////////////////////////////////////////////////////////



    Gx8_vBeginNewLightSet(_pst_Node);



   if ((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted) &&
   (_ulValidityMask & (MAT_ValidateMask_RLI+MAT_ValidateMask_Diffuse)) && // no point in lighting if per-vertex color isn't used...
   (_pst_LightList->ul_Current != 0))
    {
		D3DMATERIAL8			tempMat;
	    OBJ_tdst_GameObject   **ppst_LightNode, **ppst_Last;
   	    BOOL                    b_BVRejection;
	    LONG                    l_FirstComputing = 1;

	    ppst_LightNode = _pst_LightList->dpst_Light;
	    ppst_Last = ppst_LightNode + _pst_LightList->ul_Current;

        b_BVRejection = GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_UseBVForLightRejection;




		//Lighting ALPHA value from diffuse////////////////////////////////////////////////////////////////////////////////////

		pSD = GX8_M_SD(GDI_gpst_CurDD);

		//IDirect3DDevice8_SetRenderState(pSD->mp_D3DDevice,D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL );
		//IDirect3DDevice8_SetRenderState(pSD->mp_D3DDevice,D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_MATERIAL );
		//IDirect3DDevice8_SetRenderState(pSD->mp_D3DDevice,D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1 );
		//IDirect3DDevice8_SetRenderState(pSD->mp_D3DDevice,D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1 );
		
		
		//Set color for temporary material///////////////////////////////////////
		tempMat.Ambient.a=0.0f;
		tempMat.Ambient.r=0.0f;
		tempMat.Ambient.g=0.0f;
		tempMat.Ambient.b=0.0f;

		tempMat.Diffuse.a=0.0f;
		tempMat.Diffuse.r=1.0f;
		tempMat.Diffuse.g=1.0f;
		tempMat.Diffuse.b=1.0f;

		tempMat.Power=0.0f;
	
		tempMat.Specular.a=0.0f;
		tempMat.Specular.r=0.0f;
		tempMat.Specular.g=0.0f;
		tempMat.Specular.b=0.0f;


		tempMat.Emissive.a=0.0f;
		tempMat.Emissive.r=0.0f;
		tempMat.Emissive.g=0.0f;
		tempMat.Emissive.b=0.0f;
		//////////////////////////////////////////////////////////////////////////

		//OK HW LIGHTING
		IDirect3DDevice8_SetRenderState(pSD->mp_D3DDevice,D3DRS_LIGHTING,TRUE);

		//OK PER COLOR VERTEX
		IDirect3DDevice8_SetRenderState(pSD->mp_D3DDevice,D3DRS_COLORVERTEX,TRUE);

		//Set the material
		IDirect3DDevice8_SetMaterial(pSD->mp_D3DDevice,&tempMat);


		//Device light's settings (it's ok only if diffuse doesn't contains alpha)//////////////////////////////
		IDirect3DDevice8_SetRenderState(pSD->mp_D3DDevice,D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL );
		IDirect3DDevice8_SetRenderState(pSD->mp_D3DDevice,D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_MATERIAL );
		IDirect3DDevice8_SetRenderState(pSD->mp_D3DDevice,D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL );
		IDirect3DDevice8_SetRenderState(pSD->mp_D3DDevice,D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1 );
		////////////////////////////////////////////////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		

	    while(ppst_LightNode < ppst_Last)
	    {

            OBJ_tdst_GameObject *_pst_LightNode = *ppst_LightNode;
	        LIGHT_gpst_Cur = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;


			//CARLONE...NEW FOR HW LIGHTING ///////////////////////////////////////////////////////////////////////////////////////////
			switch(LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_Type | LIGHT_Cul_LF_Specular))
			{
				 case LIGHT_Cul_LF_Omni | LIGHT_Cul_LF_Specular:
					 {
						++ppst_LightNode;
						continue;
					 }
				 case LIGHT_Cul_LF_Omni:
					 {
						 //++ppst_LightNode;
						//continue;	
					 }
					 break;

				 case LIGHT_Cul_LF_Direct | LIGHT_Cul_LF_Specular:
					 {
						
					 ++ppst_LightNode;
					 continue;

					 }

				 case LIGHT_Cul_LF_Direct:
					  {
						 // ++ppst_LightNode;
						 // continue;	
					 }


					 break;

				 case LIGHT_Cul_LF_Spot | LIGHT_Cul_LF_Specular:
					 {
					 ++ppst_LightNode;
					 continue;

					 }
				 case LIGHT_Cul_LF_Spot:

					 //++ppst_LightNode;
					 //continue;

					 break;
				
			}
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	        if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_UseColor)
	        {
                BOOL bRejectLight = FALSE;

                // Find out if object is into the influence volume of the light
                if (_pst_Node->ul_StatusAndControlFlags & OBJ_C_StatusFlag_RTL)
                {
                    if ( !(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_RealTimeOnDynam) )
                        bRejectLight = TRUE;
                }
                else
                {
                    if ( !(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_RealTimeOnNonDynam) )
                    {
                        bRejectLight = TRUE;
                    }
                    else if ((LIGHT_gpst_Cur->ul_Flags & (LIGHT_Cul_LF_Type+LIGHT_Cul_LF_OmniConst)) == (LIGHT_Cul_LF_Omni+LIGHT_Cul_LF_OmniConst))
                    {
        		        MATH_tdst_Vector	st_LightToPoint;
		                float f_SqDist;

                        MATH_SubVector(&st_LightToPoint, &_pst_Node->pst_GlobalMatrix->T, &_pst_LightNode->pst_GlobalMatrix->T);
                        f_SqDist = MATH_f_SqrNormVector( &st_LightToPoint );
                        if (f_SqDist >= fSqr(LIGHT_gpst_Cur->st_Omni.f_Far))
                        {
                            bRejectLight = TRUE;
                        }

                    }
                }

                if (LIGHT_gpst_Cur->pst_GO)
                {
                    if ( LIGHT_gpst_Cur->pst_GO != ANI_pst_GetReferenceInit(_pst_Node) )
                        bRejectLight = TRUE;   
                }


                if (bRejectLight)
                {
                    ppst_LightNode++;
	                continue;
                }
                else
                {
                    if (b_BVRejection && (_pst_Node->pst_BV) )
                    {
		                ULONG ulType;
		                ulType  =	LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Type;
                        if ((ulType == LIGHT_Cul_LF_Omni) || (ulType == LIGHT_Cul_LF_Spot))
		                {
                            OBJ_tdst_GameObject *pst_Father;
                            MATH_tdst_Vector    v, *pv;
			                if (   
	                                ((_pst_Node->ul_IdentityFlags & (LIGHT_ObjSkinFlag|OBJ_C_IdentityFlag_Hierarchy) ) == (LIGHT_ObjSkinFlag|OBJ_C_IdentityFlag_Hierarchy)) && 
	                                (_pst_Node->pst_Base->pst_AddMatrix->l_Number > 1) && 
	                                (pst_Father = _pst_Node->pst_Base->pst_Hierarchy->pst_Father) != NULL
	                           )
			                {
	                            pv = &v;
	                            MATH_TransformVertex( pv, pst_Father->pst_GlobalMatrix, &_pst_Node->pst_Base->pst_Hierarchy->st_LocalMatrix.T );
			                }
	                        else
	                            pv = &_pst_Node->pst_GlobalMatrix->T;
		                    MATH_SubVector( &v, &_pst_LightNode->pst_GlobalMatrix->T, pv );
                            if (!INT_SphereAABBox(&v, LIGHT_gpst_Cur->st_Omni.f_Far, OBJ_pst_BV_GetGMin(_pst_Node->pst_BV ), OBJ_pst_BV_GetGMax(_pst_Node->pst_BV ) ))
                            {
                                ppst_LightNode++;
	                            continue;
                            }
                        }
                    }

		            if(l_FirstComputing)
		            {
			            l_FirstComputing = 0;
			            MATH_InvertMatrix(&LIGHT_gst_NIM, _pst_Node->pst_GlobalMatrix);
		            }
    		        MATH_MulMatrixMatrix(&LIGHT_gst_LP, (*ppst_LightNode)->pst_GlobalMatrix, &LIGHT_gst_NIM);
	                Gx8_vAddLightToLightSet(LIGHT_gpst_Cur, (*ppst_LightNode)->pst_GlobalMatrix);
                }
            }
#pragma message("TODO: hardware bump lights") // ...and look more closely at that 'additional material' stuff...
            if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Bump)
                LIGHT_gpst_Bump = (OBJ_tdst_GameObject *)*ppst_LightNode;

            if ( (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_AddMaterial )
            {
	            if (GDI_gpst_CurDD->pst_AdditionalMaterial == NULL)
	            {
	                if(((*ppst_LightNode)->pst_Base) && ((*ppst_LightNode)->pst_Base->pst_Visu) && ((*ppst_LightNode)->pst_Base->pst_Visu->pst_Material) )
	                {
                   	    int i_Mat;

                        if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_AddMaterialBVMustBeInside)
	                        i_Mat = COL_b_BVIn( (*ppst_LightNode), _pst_Node );
	                    else
	                        i_Mat = COL_b_BVOverlap( _pst_Node, (*ppst_LightNode), TRUE, TRUE, FALSE);
	                    
	                    if ( i_Mat )
	                    {
	                        GDI_gpst_CurDD->pst_AdditionalMaterial = (*ppst_LightNode)->pst_Base->pst_Visu->pst_Material;
	                        if (LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_AddMaterialDontUseNormalMaterial)
                                GDI_gpst_CurDD->ul_CurrentDrawMask &= ~GDI_Cul_DM_UseNormalMaterial;
	                        if (GDI_gpst_CurDD->pst_AdditionalMaterial->i->ul_Type == GRO_MaterialMulti)
	                        {
	                            i_Mat = (int) LIGHT_gpst_Cur->st_AddMaterial.w_Id;
	                            i_Mat %= ((MAT_tdst_Multi *) GDI_gpst_CurDD->pst_AdditionalMaterial)->l_NumberOfSubMaterials;
	                            GDI_gpst_CurDD->pst_AdditionalMaterial = (GRO_tdst_Struct *) ((MAT_tdst_Multi *) GDI_gpst_CurDD->pst_AdditionalMaterial)->dpst_SubMaterial[ i_Mat ];
                            }
                        }
                    }
                }
            }

		    ppst_LightNode++;
        }
	}

    if ((bObjectUsesRLIColors) && (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_UseRLI))
    {
        dwAmbient = 0; // Ambient is already in the vertex buffer
    }
    else if (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_UseAmbient)
    {
        if (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_DontUseAmbient2)
        {
            dwAmbient = GDI_gpst_CurDD->pst_World->ul_AmbientColor;
        }
	    else
        {
            dwAmbient = GDI_gpst_CurDD->pst_World->ul_AmbientColor2;
        }
    }
    else
    {
        dwAmbient = 0;
    }

    GDI_gpst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseLitAlphaShader;
    if ((dwAmbient != 0) || (gs_stLightSet.ucLightsCount != 0))
    {
        // Object apparently needs lighting
        if (bObjectUsesRLIAlphas)
        {
            GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseLitAlphaShader;
            Gx8_vApplyLightSetForLitAlphaShaders();
            Gx8_vSetTransformMatrixForShaders();
        }
        else
        {
  //CARLONE..SOFTWARE AMBIENT          Gx8_SetGlobalAmbient(GX8_M_SD(GDI_gpst_CurDD), dwAmbient);
            Gx8_vApplyLightSet();
        }
    }
    else if (bObjectUsesRLIColors)
    {
        Gx8_RS_Lighting( GX8_M_SD(GDI_gpst_CurDD), FALSE );
    }
    else
    {
        // Got to have a black per-vertex color.
        // Turning lighting off wouldn't do because then D3D would use a white per-vertex color...
         //CARLONE..SOFTWARE AMBIENT Gx8_SetGlobalAmbient(GX8_M_SD(GDI_gpst_CurDD), 0);
        Gx8_vApplyLightSet();
    }


	//Return number of applied lights.
	return gs_stLightSet.ucLightsCount;
}



void Gx8_SetAmbient(DWORD color)
{

}
