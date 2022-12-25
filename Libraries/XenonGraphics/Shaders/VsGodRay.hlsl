//-----------------------------------------------------------------------------
// GodRay vertex shader
//-----------------------------------------------------------------------------

struct VSIN
{
    float4 Position         : POSITION;
    float4 Color			: COLOR0;
    float2 TexCoord0        : TEXCOORD0;
};

struct VSOUT
{
    float4 Position         : POSITION;
    float4 Color			: COLOR0;
    float2 TexCoord0        : TEXCOORD0;
};

VSOUT ProcessVertex(VSOUT Input,
					int iShaderId, 
					int iUnused1, 
					int iUnused2, 
					int iUnused3, 
					int iUnused4, 
					int iUnused5, 
					int iUnused6, 
					int iUnused7)
{
    VSOUT Output = (VSOUT)0;
    
	Output.Position		= Input.Position;
	Output.Color		= Input.Color;
	Output.TexCoord0	= Input.TexCoord0;

	return Output;
}