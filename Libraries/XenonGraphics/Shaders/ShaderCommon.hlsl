#ifndef XE_VS_VSOUT
#define XE_VS_VSOUT

struct VSOUT
{
    float4 Position         : POSITION;
    float4 Color0           : COLOR0;
    float4 Color1           : COLOR1;
    float4 TexCoord0        : TEXCOORD0;
    float4 TexCoord1        : TEXCOORD1;
    float4 LightDir[3]      : TEXCOORD2;
    float4 HalfWay[3]       : TEXCOORD5;
};

#endif
