/**********************************************************************
*<
FILE: RTMax.h

DESCRIPTION:	Main Include for DirectX and MS Effect related code
				
CREATED BY:		Neil Hazzard

HISTORY:		February 2004

*>	Copyright (c) 2004, All Rights Reserved.
**********************************************************************/

#ifndef __RTMAX__H
#define __RTMAX__H

#ifndef __D3DX9_H__
	#include <d3dx9.h>
#endif

#ifndef __JAGAPI__
	#include "max.h"
#endif

#ifndef __STDMAT__H
	#include "stdmat.h"
#endif

//!Some useful memory management macros
#define SAFE_DELETE(p)			{ if (p) { delete (p);		(p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p);	(p)=NULL; } }
#define SAFE_RELEASE(p)			{ if (p) { (p)->Release();	(p)=NULL; } }

//! Switch to compile between Dx9.0b and Dx9.0c
#define SUMMER_SDK



class ILightManager;
class IRenderMesh;
class IEffectParser;

typedef enum MaxSemantics{
	kSemanticUnknown,
	kSemanticDirection,
	kSemanticPosition,
	kSemanticWorld,
	kSemanticView,
	kSemanticPerspective,
	kSemanticWorldView,
	kSemanticProjection,
	kSemanticProjectionI,
	kSemanticProjectionIT,
	kSemanticViewI,
	kSemanticWorldI,
	kSemanticWorldViewI,
	kSemanticWorldViewProj,
	kSemanticWorldViewIT,
	kSemanticViewIT,
	kSemanticWorldIT,
	kSemanticWorldViewT,
	kSemanticWorldT,
	kSemanticViewT,
	kSemanticProjectionT,
	kSemanticWorldCamPos,
	kSemanticLightColor,
	kSemanticNormalMap,
	kSemanticBumpMap,
	kSemanticDiffuseMap,
	kSemanticSpecularMap,
	kSemanticOpacityMap,
	kSemanticLightMap,
	kSemanticDisplacementMap,
	kSemanticGlobalTime,
	kSemanticGlobalBkgrd,
	kSemanticLocalBBMin,
	kSemanticLocalBBMax,
	kSemanticLocalBBSize,
	kSemanticRecipLocalBBSize,
	kSemanticBitmapOverride,
	kSemanticColorOverride,
	kSemanticViewportDimensions,
	kSemanticATan,
	kSemanticDiffuse,
	kSemanticAmbient,
	kSemanticSpecular,
	kSemanticSpecularLevel,
	kSemanticGlossLevel,
	kSemanticOpacity,
	kSemanticEmissive,
	kSemanticReflectionMap,
	kSemanticLightFallOff,
	kSemanticLightHotSpot

}MaxSemantics;

//!The Effect Manager is responsible for displaying effect parameters.  In max this is implemented by the DirectX 9 Material.
/*! All parameters except textures are handled by the Effect Manager.  This includes setting the data on the render pass.  Textures
are controlled by the Effect Parser, as the the parser understands the usage, the manager simply provided the resource name from the UI
It is the manager's responsiblity to make sure the data is set each pass.  This may or may not mean providing a UI, but the methods
provide enough hints to be able to implement this.
*/
class IEffectManager{

public:
	/*! Display a floating point value in the Dialog box
	\param handle The handle to the parameter in the effect file
	\param paramName The name of the actual parameter.  This will be used in the paramblock
	\param uiName The name to be displayed in the Dialog box
	\param val The default value
	\param min The minimum value for the UI control
	\param max The maximum value for the UI control
	\param step The increment value for the UI control
	\param semantic The semantic of the parameter.  
	*/
	virtual void SetFloatParam(D3DXHANDLE handle, TCHAR * paramName, TCHAR * uiName, float val,float min, float max, float step,MaxSemantics semantic )=0;

	/*! Display an integer value in the Dialog box
	\param handle The handle to the parameter in the effect file
	\param paramName The name of the actual parameter.  This will be used in the paramblock
	\param uiName The name to be displayed in the Dialog box
	\param val The default value
	\param min The minimum value for the UI control
	\param max The maximum value for the UI control
	\param step The increment value for the UI control
	\param semantic The semantic of the parameter.  
	*/
	virtual void SetIntParam(D3DXHANDLE handle, TCHAR * paramName, TCHAR * uiName,int val,int min, int max, int step,MaxSemantics semantic )=0;

	/*! Display a color value in the Dialog box.  This will be a standard max color swatch
	\param handle The handle to the parameter in the effect file
	\param paramName The name of the actual parameter.  This will be used in the paramblock
	\param uiName The name to be displayed in the Dialog box
	\param color The default color to display
	\param semantic The semantic of the parameter.  
	*/
	virtual void SetColorParam(D3DXHANDLE handle, TCHAR * paramName, TCHAR * uiName,D3DXVECTOR4 color,MaxSemantics semantic)=0;

	/*! Display a D3DVECTOR4 value in the Dialog box.  This will be 4 max spinner UI elements
	\param handle The handle to the parameter in the effect file
	\param paramName The name of the actual parameter.  This will be used in the paramblock
	\param uiName The name to be displayed in the Dialog box
	\param val The default value
	\param min The minimum value for the UI control
	\param max The maximum value for the UI control
	\param step The increment value for the UI control
	\param semantic The semantic of the parameter.  
	*/	
	virtual void SetPoint4Param(D3DXHANDLE handle, TCHAR * paramName, TCHAR * uiName,D3DXVECTOR4 val,float min, float max, float step,MaxSemantics semantic) = 0;

	/*! Display a boolean value in the Dialog box.  This will be a standard max check box
	\param handle The handle to the parameter in the effect file
	\param paramName The name of the actual parameter.  This will be used in the paramblock
	\param uiName The name to be displayed in the Dialog box
	\param val The default color to display
	\param semantic The semantic of the parameter.  
	*/
	virtual void SetBooleanParam(D3DXHANDLE handle, TCHAR * paramName, TCHAR * uiName,BOOL val,MaxSemantics semantic) = 0;
	
	/*! Display a bitmap selection button
	\param handle The handle to the parameter in the effect file
	\param paramName The name of the actual parameter.  This will be used in the paramblock
	\param uiName The name to be displayed in the Dialog box
	\param filename The default filename to display
	\param semantic The semantic of the parameter.  This is defined in the effect file and is used for render to texture.
	\param mappingEnabled Specifies whether the mapping channel UI should be shown for the texture
	\param mappingChannel This provides the default mapping channel to use.
	*/
	virtual void SetTextureParam(D3DXHANDLE handle, TCHAR * paramName, TCHAR * uiName,TCHAR * filename, MaxSemantics semantic, bool mappingEnabled = false, int mappingChannel=1)=0;

	/*! Display a Light selection box, or provide access to lights based on the parameter
	\param handle The handle to the parameter in the effect file
	\param paramName The name of the actual parameter.  This will be used in the paramblock
	\param uiName The name to be displayed in the Dialog box
	\target Specifies if the light is a directional light.  This is used to filter the UI light list.
	\param semantic The semantic of the parameter.
	*/
	virtual void SetLightParam(D3DXHANDLE handle, TCHAR * paramName, TCHAR * uiName,bool target,MaxSemantics semantic) = 0;

	/*! Non UI Element used to set various transforms defined in the effect file
	\param handle The handle to the parameter in the effect file
	\param semantic The semantic of the transform to set.
	*/
	virtual void SetTransformParam(D3DXHANDLE handle,MaxSemantics semantic)=0;

	/*! Non UI Element used to set various geometry based data defined in the effect file
	\param handle The handle to the parameter in the effect file
	\param semantic The semantic of the transform to set.
	*/
	virtual void SetGeometryParam(D3DXHANDLE handle,MaxSemantics semantic)=0;

	/*! Non UI Element used to set various environmental based data defined in the effect file
	\param handle The handle to the parameter in the effect file
	\param semantic The semantic of the transform to set.
	*/

	virtual void SetEnvironmentParam(D3DXHANDLE handle,MaxSemantics semantic)=0;

	/*! Set the techniques used in the effect file.  The default is used as the primary technique especially for multi pass effects
	\param handle The handle to the technqiue
	\param techniqueName The name to display in the UI
	\param bbefault The default technique to used
	*/
	virtual void SetTechnique(D3DXHANDLE handle, TCHAR * techniqueName, bool bDefault) =0;

};
//!Implemented by Effect Parser.
/*! Effect parsers will implement this interface in order to actually render the effect
*/
class IEffectParser
{
public:
	/*! Parse the actual effect file.  All data that is needed to actually render the effect is extracted here.  If a UI is 
	needed, then the IEffectManager interface can be used for this purpose
	*/
	virtual bool ParseEffectFile(LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT effect, TCHAR * filename, IEffectManager * em)=0;

	/*! The UI code will use this to make sure the effect has all the textures loaded and applied to the effect when they change
	from the UI
	*/
	virtual bool LoadTexture(LPDIRECT3DDEVICE9 pDevice,LPD3DXEFFECT effect,D3DXHANDLE texHandle,TCHAR * filename,bool forceReload) =0;

	/*! Remove the parser*/
	virtual void DestroyParser()=0;

	/*! Any initialisation or one off setup, before the render passes happens here.  If new rendertargets are needed or back buffers, this is the 
	place to do it
	*/
	virtual bool PreRender(LPDIRECT3DDEVICE9 pDevice,LPD3DXEFFECT pEffect,IRenderMesh * rmesh, ILightManager * lm, D3DCOLOR bkgColor, int width, int height ) = 0;

	/*! Render the actual object, based on the data supplied in the effect file
	*/
	virtual bool Render(LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT pEffect, IRenderMesh * rmesh, D3DXHANDLE primaryTechnique)=0;

	/*! Alows the user to change the mapping channel used by the texture in the UI
	*/
	virtual void SetMappingChannel(D3DXHANDLE handle, int Channel)=0;

	virtual void SetUseLPRT(bool state){};

	virtual Tab <int> GetMappingChannels()=0;

	//!virtual destructor
	virtual ~IEffectParser() {}

	
//! Allow the EffectManager access to lighting data, so it can expose it through the public interfaces
/*! The EffectManager used to maintain all the lights, now it simply host the UI.  This means the parser needs to keep tabs
on the lights, or at least what parameters of the lights are needed.  This is simple for the parser, as it will need to keep a local
list of the light data so it can request the actual max data from the LightManager.
*/
	//!The number of light based parameters the effect contains
	virtual int GetNumberOfLightParameters() = 0;

	//! The light parameter usage
	virtual MaxSemantics GetLightSemantic(int index)=0;

	//! Get a handle to the light based on the index supplied
	virtual D3DXHANDLE GetLightHandle(int index)=0;


};

//! Access Lighting data from 3ds max lights
/*! This is the main access to the lights.  The user would request the light data from the LightManager based on the D3DHANDLE
*/
class ILightingData{
public:
	/*! Get the Position of the light
	\return A directX vector containing the position
	*/
	virtual D3DXVECTOR4 GetPosition()=0;

	/*! Get the current direction of the light
	\return A directX vector containing the direction
	*/
	virtual D3DXVECTOR4 GetDirection()=0;

	/*! Get the current color
	\return A DirectX color value.
	*/
	virtual D3DXVECTOR4 GetColor()=0;

	virtual float GetFallOff()=0;

	virtual float GetHotSpot()=0;


};

//! A basic light container
/*! The user would use this to access the light data for a specific light.  The system maintains the light list so the user
does not need to worry about the maintainence.
*/
class ILightManager{

public:
	//! Access the Light Data
	/*! Access the light data based on the handle supplied.  The handle is based on the parameter found in the Effect File
	\param handle The handle of the light to access
	\return A pointer to the light data
	*/
	virtual ILightingData * GetLightingData(D3DXHANDLE handle)=0;

};



#include "iFnPub.h"

#define PARSER_LOADER_INTERFACE Interface_ID(0x7fbe42d6, 0x3e7f412b)

//! A mini Max class descriptor.  This is purely to instantiate multiple objects, and query the Parser ID for the DLL
class EffectDescriptor
{
public:
	virtual IEffectParser * CreateParser() = 0;
	virtual TCHAR * GetParserDescription () {return NULL;}
	virtual ULONG GetParserID()=0;

};


class IParserLoader : public FPStaticInterface
{
public:

	virtual int	GetNumberOfParsers()=0;
	virtual IEffectParser * GetParser(ULONG parserID) =0;
	virtual void LoadDLLs()=0;
	virtual void UnloadDLLs()=0;
	virtual TCHAR * GetParserDesc(ULONG parserID)=0;

	// virtual EffectDescriptor* GetEffectDescritor(int index) = 0;
	// virtual EffectParser * GetParser(int index) =0;

};

inline IParserLoader* GetParserLoader() { return (IParserLoader*)GetCOREInterface(PARSER_LOADER_INTERFACE); }

//! Maintains a Dx version of the current mesh in the cache.  It will render either the max mesh or a screen aligned quad
class IRenderMesh
{

public:

	enum MeshType{
		kMesh,
		kScreenQuad,
	};

	virtual void		SetMeshType(MeshType type)=0;

	//! Used to update the Quad version of the mesh
	/*!
	\param *Device A Pointer to the main DX interface
	\param width The width of the render target
	\param height The height of the render target
	\param texWidth The width of the texture being used
	\param texheight The height of the texture being used
	\return TRUE if successful
	*/
	virtual bool		Evaluate(IDirect3DDevice9 *Device, int width, int height, int texWidth, int texHeight)=0;

	//! Setting the node parameter to non NULL, will make sure the Normals are transformed correctly
	/*!
	\param *Device A Pointer to the main DX interface
	\param *aMesh The mesh whose data is to be extracted
	\param MatIndex The material index for the faces to be rendered.  This provides support for multi subobject materials
	\param NegScale Used to determine if negative scale is being used on the object.
	\return TRUE if successful
	*/
	virtual bool		Evaluate(IDirect3DDevice9 *Device, Mesh *aMesh, int MatIndex, bool NegScale)=0;

	//! Store relevant mapping data
	/*! Allows the app to provide upto 8 mapping channels, this is used to extract the texcoord data from the mesh
	\param  map A tab of active mapping channels
	*/
	virtual void		SetMappingData(Tab <int> map)=0;

	//! Renders the current object
	/*!
	\param *Device A Pointer to the main DX interface
	\return TRUE if successful
	*/
	virtual bool		Render(IDirect3DDevice9 *Device)=0;

	/*! force an invalidation of the internal data structures
	*/
	virtual void		Invalidate()=0;

};


//! This class maintains a list of the meshes and RenderMeshes used per instance.  It acts as a factory class
/*! This is useful for Materials where a single material could be applied to multiple nodes, so a cache is needed so the material
can pull the correct mesh data
*/
class IRenderMeshCache{

public:
	DllExport static  IRenderMeshCache *  GetRenderMeshCache();

	//! Set the size of the cache to maintain;
	/*!
	\param cacheSize The size of the cache
	\param meshType The type of object to hold.  This can be either a mesh or a quad.
	*/
	virtual void SetSizeOfCache(int cacheSize, IRenderMesh::MeshType meshType=IRenderMesh::kMesh)=0;

	//! Fill up the cache with data from max.
	/*!
	\param *mesh The mesh to store in the cache
	\param *node The node to store in the cache
	\param time The time to use in any evaluations
	\param & meshChanged If the mesh is different to that in the cache, this specifies the case.
	\return The position in the cache of the mesh/node combo
	*/
	virtual int SetCachedMesh (Mesh *mesh, INode *node, TimeValue time, bool & meshChanged)=0;

	//! Get the active render mesh
	/*!
	\param index The index of the mesh to retrieve
	\return The IRenderMesh based on the index provided
	*/
	virtual IRenderMesh * GetActiveRenderMesh(int index)=0;

	/*!
	\param index Get the node for the current cached object
	\return The INode for the current cache
	*/
	virtual INode * GetActiveNode(int index)=0;

	/*!
	\param index Get the mesh for the current cached object
	\return The INode for the current cache
	*/
	virtual Mesh * GetActiveMesh(int index)=0;

	virtual ~IRenderMeshCache() {}
	virtual void Release()=0;

};

//! A factory class, used to create HLSL code based on the material passed in.
class IHLSLCodeGenerator{

public:
	enum CodeVersion {
		PS_1_X,
		PS_2_0,
		PS_3_0,
	};

	//! Access to the factory class
	DllExport static IHLSLCodeGenerator * GetHLSLCodeGenerator();

	//!Generate HLSL Code based on the material
	/*! This will create HLSL code based on the material and lights in the scene.  It will generate code for Blinn and ONB style
	specular component.  It will support Ambient,Diffuse, specular, Self Illumination, opacity, bump, relfection maps.
	\param *mtl The material source
	\param lights A table of lights to used in the HLSL generation
	\param &transp True if there is a transparent component
	\returns A string containing the code.
	*/
	virtual TCHAR * GenerateFragmentCode(StdMat2 * mtl, Tab <INode *> lights, bool & transp) = 0;

	//! Generates a basic Vertex shader to support the pixel shader
	/*\
	\return A string containing the code
	*/
	virtual TCHAR * GenerateVertexCode()=0;

	//! Generates an Effect File based on the material supplied.
	/*! This is similar to GenerateFragmentCode, but will optimize the HLSL code to assembler, based on a pass compiler.
	\returns A string containing the effect file.  This can be loaded in the DirectX 9 Shader material
	*/
	virtual TCHAR * GenerateEffectFile(StdMat2 * mtl,Tab <INode *> lights,CodeVersion code, bool & transp, int instSize, bool userLPRT = false) =0;

	//! Get the texture map used by the specified mapName
	/*! When the material tree is processed, a texture map can provide its only HLSL code and texture samplers.  This function
	will return the texmap of these maps and the default diffuse, bump etc..
	\param * mat The standard material whose textures are to be queried
	\param mapName This is the name parsed out in the effect file
	\param & bump Allows special handling of the bump map
	\returns A pointer to the specified texture map
	*/
	virtual Texmap* GetShaderDefinedTexmap(StdMat2 * mat, TCHAR * mapName, bool & bump)=0;

	virtual ~IHLSLCodeGenerator() {}
	virtual void Release() = 0;
};


#define HLSL_TEXTURE_INTERFACE Interface_ID(0xc2b2185, 0x635556bf)

class TextureData{
public:
	//! UI Name to appear in the Effect file
	TCHAR UIName[25];

	//! Sampler name is used as the filename for the texture, in the effect file.  It is also used as a lookup for the texmap
	TCHAR SamplerName[25];

	//! The sub map number the texture refers to - the calling code will use texmap->GetSubTexmap(SubMapNum) to access it.
	int SubMapNum;
};

typedef Tab < TextureData> TexDataList;

class IHLSLTexmap : public BaseInterface
{
public:
	// Interface Lifetime
	virtual LifetimeType	LifetimeControl() { return noRelease; }
	virtual Interface_ID	GetID() { return HLSL_TEXTURE_INTERFACE; }

	//! Specifies that the Texmap will provide HLSL code for the Dx Standard Material
	/*! 
	\return True if the Texmap supports HLSL code generation.
	*/
	virtual bool DoesSupportHLSL(){return false;}
	
	//! Allow texture to provide an hlsl representation of the texture.
	/*! 
	*/
	virtual void GetTexmapHLSLFunction(TCHAR * code, TCHAR * entryPoint){};

	//! Provide access to any textures used in the HLSL code
	/*!
	*/
	virtual void GetTextureData(TexDataList * list ){};

};

struct DxGeneralFunctions{
	DllExport  static TCHAR * GetDXResourceFilename(TCHAR * fileName);
};


#endif