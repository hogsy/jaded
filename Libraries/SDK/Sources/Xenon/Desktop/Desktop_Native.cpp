// ****************************************
// Visual Desktop Windowing Manager
//
// Used for King Kong Xenon Debugging
//
// By Alexandre David (January 2005)
// ****************************************

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "Precomp.h"

#include "Desktop.h"

// ***********************************************************************************************************************

#if defined(DESKTOP_ENABLE)

#include "INOut/INOjoystick.h"
#include "TEXture/TEXstruct.h"
#include "STRing/STRstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "GraphicDK/Sources/TEXture/TEXstruct.h"

#if defined(_XENON)
#include "XenonGraphics/XeUtils.h"
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeTextureManager.h"
#include "SOUnd/Sources/Xenon/xeSND_Engine.h"
#endif

// ***********************************************************************************************************************
//    Externs
// ***********************************************************************************************************************

extern WOR_tdst_World* WOR_gpst_CurrentWorld;

// ***********************************************************************************************************************

namespace Desktop
{

// ***********************************************************************************************************************
//    Functions - Internal
// ***********************************************************************************************************************

TInt Button_ConvertToNative(eButton Param_eButton)
{
	switch (Param_eButton)
	{
		case eButton_A:				return eXeButton_A;
		case eButton_B:				return eXeButton_B;
		case eButton_X:				return eXeButton_X;
		case eButton_Y:				return eXeButton_Y;
		case eButton_Trigger_Left:	return eXeButton_Trigger_Left;
		case eButton_Trigger_Right:	return eXeButton_Trigger_Right;
		case eButton_Back:			return eXeButton_Back;
		case eButton_Start:			return eXeButton_Start;
		case eButton_Thumb_Right:	return eXeButton_Thumb_Right;
		case eButton_Thumb_Left:	return eXeButton_Thumb_Left;
		case eButton_DPad_Up:		return eXeButton_DPad_Up;
		case eButton_DPad_Right:	return eXeButton_DPad_Right;
		case eButton_DPad_Down:		return eXeButton_DPad_Down;
		case eButton_DPad_Left:		return eXeButton_DPad_Left;
		case eButton_None:			return -1;
		default:					return -1;
	}
}

// ***********************************************************************************************************************

TInt Blending_ConvertToNative(eBlending Param_eBlending)
{
	switch (Param_eBlending)
	{
		case eBlending_Copy:		return MAT_Cc_Op_Copy;
		case eBlending_Alpha:		return MAT_Cc_Op_Alpha;
		case eBlending_Add:			return MAT_Cc_Op_Add;
		case eBlending_AlphaAdd:	return MAT_Cc_Op_XeAlphaAdd;
		default:					return MAT_Cc_Op_Alpha;
	}
}

// ***********************************************************************************************************************
//    Functions - Exposed
// ***********************************************************************************************************************

void* Memory_Allocate(TInt Param_iSize)
{
	if (Param_iSize <= 0)
	{
		return NULL;
	}

	return MEM_p_Alloc(Param_iSize);
}

// ***********************************************************************************************************************

void Memory_Free(void* Param_pHeapBlock)
{
	if (Param_pHeapBlock != NULL)
	{
		MEM_Free(Param_pHeapBlock);
	}
}

// ***********************************************************************************************************************

void Memory_Fill(void* Param_pMemory, TInt Param_iSize, TByte Param_byValue)
{
	if ((Param_iSize <= 0) || (Param_pMemory == NULL))
	{
		return;
	}

	L_memset(Param_pMemory, Param_byValue, Param_iSize);
}

// ***********************************************************************************************************************

TBool Input_IsLocked()
{
	return xeINO_bDesktop_InputLock;
}

// ***********************************************************************************************************************

void Input_SetLock(TBool Param_bIsInputLocked)
{
	xeINO_bDesktop_InputLock = Param_bIsInputLocked;
}

// ***********************************************************************************************************************

TFloat Input_GetMotion_X()
{
	MATH_tdst_Vector Vector;
	INO_Joystick_Move(&Vector, INO_i_GetCurrentPad());

	return Vector.x;
}

// ***********************************************************************************************************************

TFloat Input_GetMotion_Y()
{
	MATH_tdst_Vector Vector;
	INO_Joystick_Move(&Vector, INO_i_GetCurrentPad());

	return Vector.y;
}

// ***********************************************************************************************************************

TBool Input_IsButtonCurrentlyPressed(eButton Param_eButton)
{
	return INO_b_Joystick_IsButtonDown(Button_ConvertToNative(Param_eButton));
}

// ***********************************************************************************************************************

TBool Input_IsButtonRecentlyPressed(eButton Param_eButton)
{
	return INO_b_Joystick_IsButtonJustDown(Button_ConvertToNative(Param_eButton));
}

// ***********************************************************************************************************************

STR_tdst_FontDesc* Rendering_GetFontDescriptor(TEngineResID Param_iTextureID)
{
    STR_tdst_FontDesc* pst_FontDesc = STR_pst_GetFont(Param_iTextureID);
    if (pst_FontDesc != NULL)
    {
        return pst_FontDesc;
    }

    TEX_tdst_Data* pst_Tex = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, Param_iTextureID);

    if ((pst_Tex == NULL) || !(pst_Tex->uw_Flags & TEX_uw_RawPal))
    {
        return NULL;
    }

    Param_iTextureID = TEX_gst_GlobalList.dst_Texture[pst_Tex->w_Height].ul_Key;

    return STR_pst_GetFont(Param_iTextureID);
}

// ***********************************************************************************************************************

void Rendering_PopulateFontDescriptor(stFontDescriptor* Param_pstDescriptor, TEngineResID Param_iEngineResourceID)
{
	// Error Checking

	if (Param_pstDescriptor->bIsPopulated)
	{
		return;
	}

	// Fetch Engine Font Descriptor

	STR_tdst_FontDesc* pstNative_Descriptor = Rendering_GetFontDescriptor(Param_iEngineResourceID);

	if (pstNative_Descriptor == NULL)
	{
		return;
	}

	// Populate Desktop Descriptor From Engine Data

	for (TInt Loop = 32; Loop < (TInt)pstNative_Descriptor->ul_NbChar; Loop++)
	{
		Param_pstDescriptor->astLetter[Loop].fU0 = pstNative_Descriptor->pst_Letter[Loop - 32].f_U[0];
		Param_pstDescriptor->astLetter[Loop].fV0 = pstNative_Descriptor->pst_Letter[Loop - 32].f_V[0];
		Param_pstDescriptor->astLetter[Loop].fU1 = pstNative_Descriptor->pst_Letter[Loop - 32].f_U[1];
		Param_pstDescriptor->astLetter[Loop].fV1 = pstNative_Descriptor->pst_Letter[Loop - 32].f_V[1];
	}

	Param_pstDescriptor->bIsPopulated = TRUE;
}

// ***********************************************************************************************************************

TInt Texture_GetCount()
{
	return TEX_gst_GlobalList.l_NumberOfTextures;
}

// ***********************************************************************************************************************

TInt Texture_GetIndexFromID(TEngineResID Param_iEngineResourceID)
{
	// Fetch Texture

	TEX_tdst_Data* pstTexture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, Param_iEngineResourceID);

	// Error Checking

	if (pstTexture == NULL)
	{
		return kiTextureID_None;
	}

	return pstTexture->w_Index;
}

// ***********************************************************************************************************************

TInt Texture_GetIDFromIndex(TInt Param_iTextureIndex)
{
	if ((Param_iTextureIndex < 0) || (Param_iTextureIndex >= Texture_GetCount()))
	{
		return kiTextureID_None;
	}

	return TEX_gst_GlobalList.dst_Texture[Param_iTextureIndex].ul_Key;
}

// ***********************************************************************************************************************

TInt Texture_GetWidth(TInt Param_iTextureIndex)
{
	if ((Param_iTextureIndex < 0) || (Param_iTextureIndex >= Texture_GetCount()))
	{
		return -1;
	}

	IDirect3DTexture9* pstTexture = (IDirect3DTexture9*)g_oXeTextureMgr.GetTextureFromID(Param_iTextureIndex);

	if (pstTexture != NULL)
	{
		D3DSURFACE_DESC stDescriptor;

		pstTexture->GetLevelDesc(0, &stDescriptor);

		return stDescriptor.Width;
	}
	else
	{
		return -1;
	}
}

// ***********************************************************************************************************************

TInt Texture_GetHeight(TInt Param_iTextureIndex)
{
	if ((Param_iTextureIndex < 0) || (Param_iTextureIndex >= Texture_GetCount()))
	{
		return -1;
	}

	IDirect3DTexture9* pstTexture = (IDirect3DTexture9*)g_oXeTextureMgr.GetTextureFromID(Param_iTextureIndex);

	if (pstTexture != NULL)
	{
		D3DSURFACE_DESC stDescriptor;

		pstTexture->GetLevelDesc(0, &stDescriptor);

		return stDescriptor.Height;
	}
	else
	{
		return -1;
	}
}

// ***********************************************************************************************************************

TInt Texture_GetSize(TInt Param_iTextureIndex)
{
	if ((Param_iTextureIndex < 0) || (Param_iTextureIndex >= Texture_GetCount()))
	{
		return -1;
	}

	return ComputeAnyTextureSize(g_oXeTextureMgr.GetTextureFromID(Param_iTextureIndex));
}

// ***********************************************************************************************************************

TPWString Texture_GetFormat(TInt Param_iTextureIndex)
{
	// Error Checking

	if ((Param_iTextureIndex < 0) || (Param_iTextureIndex >= Texture_GetCount()))
	{
		return L"Invalid";
	}

	// Convert Format To String

	switch (g_oXeTextureMgr.GetTextureFormat(Param_iTextureIndex))
	{
		default:
		case D3DFMT_UNKNOWN:		return L"Unknown";
		case D3DFMT_DXT1:			return L"DXT1";
		case D3DFMT_DXT3:			return L"DXT3";
		case D3DFMT_DXT5:			return L"DXT5";
		case D3DFMT_A8:				return L"A8";
		case D3DFMT_R5G6B5:			return L"R5G6B5";
		case D3DFMT_X1R5G5B5:		return L"X1R5G5B5";
		case D3DFMT_A1R5G5B5:		return L"A1R5G5B5";
		case D3DFMT_A4R4G4B4:		return L"A4R4G4B4";
		case D3DFMT_X4R4G4B4:		return L"X4R4G4B4";
		case D3DFMT_D16:			return L"D16";
		case D3DFMT_R16F:			return L"R16F";
		case D3DFMT_A8R8G8B8:		return L"A8R8G8B8";
		case D3DFMT_X8R8G8B8:		return L"X8R8G8B8";
		case D3DFMT_A2R10G10B10:	return L"A2R10G10R10";
		case D3DFMT_R32F:			return L"R32F";
		case D3DFMT_A16B16G16R16F:	return L"A16B16G16R16F";
		case D3DFMT_A32B32G32R32F:	return L"A32B32G32R32F";
		case D3DFMT_LIN_A8:			return L"LINEAR A8";
		case D3DFMT_LIN_A4R4G4B4:	return L"LINEAR A4R4G4B4";
		case D3DFMT_LIN_R5G6B5:		return L"LINEAR R5G6B5";
		case D3DFMT_LIN_A8R8G8B8:	return L"LINEAR A8R8G8B8";
		case D3DFMT_LIN_X8R8G8B8:	return L"LINEAR X8R8G8B8";
		case D3DFMT_LIN_DXT1:		return L"LINEAR DXT1";
		case D3DFMT_LIN_DXT3:		return L"LINEAR DXT3";
		case D3DFMT_LIN_DXT5:		return L"LINEAR DXT5";
#if defined(_XENON)
        case D3DFMT_DXN:            return L"DXN";
        case D3DFMT_CTX1:           return L"CTX1";
        case D3DFMT_LIN_DXN:        return L"LINEAR DXN";
        case D3DFMT_LIN_CTX1:       return L"LINEAR CTX1";
#endif
	}
}

// ***********************************************************************************************************************

TBool Texture_IsValid(TInt Param_iTextureIndex)
{
	if ((Param_iTextureIndex < 0) || (Param_iTextureIndex >= Texture_GetCount()))
	{
		return FALSE;
	}

	IDirect3DTexture9* pstTexture = (IDirect3DTexture9*)g_oXeTextureMgr.GetTextureFromID(Param_iTextureIndex);

	if ((pstTexture == NULL) || ((TEX_gst_GlobalList.dst_Texture[Param_iTextureIndex].uw_Flags & TEX_uw_Shared) != 0))
	{
		return FALSE;
	}

	return TRUE;
}

// ***********************************************************************************************************************

TBool Texture_GetTotalMemoryUsed()
{
	return g_oXeTextureMgr.GetTotalUsedMemory();
}

// ***********************************************************************************************************************

TInt AudioBuffers_GetCount()
{
#if _XENON_SOUND_ENGINE
	return GetAudioEngine()->GetVoiceCount();
#else
	return 0;
#endif
}

// ***********************************************************************************************************************

TInt AudioBuffers_GetUsedMemory()
{
#if _XENON_SOUND_ENGINE
	return GetAudioEngine()->GetMemoryTracker(eMemoryTracker_Buffer_Static)->iAllocatedSize + GetAudioEngine()->GetMemoryTracker(eMemoryTracker_Buffer_Stream)->iAllocatedSize;
#else
	return 0;
#endif
}

// ***********************************************************************************************************************

void Rendering_DrawRectangle(TFloat Param_fX, TFloat Param_fY, TFloat Param_fWidth, TFloat Param_fHeight, TFloat Param_fU0, TFloat Param_fV0, TFloat Param_fU1, TFloat Param_fV1, TARGBColor Param_iColor, eBlending Param_eBlending, TInt Param_iTextureIndex)
{
	DrawRectangleEx(Param_fX, Param_fY, Param_fX + Param_fWidth, Param_fY + Param_fHeight, Param_fU0, Param_fV1, Param_fU1, Param_fV0, Param_iColor, Param_iColor, Param_iColor, Param_iColor, 0.0f, Blending_ConvertToNative(Param_eBlending), Param_iTextureIndex);
}

// ***********************************************************************************************************************

TEngineResID World_GetID()
{
	// Error Checking

	if (WOR_gpst_CurrentWorld == NULL)
	{
		return kiEngineResID_None;
	}

	// Return Current World Key

	return WOR_gpst_CurrentWorld->h_WorldKey;
}

// ***********************************************************************************************************************

void Profile_Begin(const TPString Param_pString)
{
#if defined(PROFILE_ENABLE) || defined(_DEBUG)
	PIXBeginNamedEvent(0, Param_pString);
#endif
}

// ***********************************************************************************************************************

void Profile_Begin(const TPString Param_pFormatString, int Param_iArgument)
{
#if defined(PROFILE_ENABLE) || defined(_DEBUG)
	PIXBeginNamedEvent(0, Param_pFormatString, Param_iArgument);
#endif
}

// ***********************************************************************************************************************

void Profile_End()
{
#if defined(PROFILE_ENABLE) || defined(_DEBUG)
	PIXEndNamedEvent();
#endif
}

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************

#endif // DESKTOP_ENABLE
