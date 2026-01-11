
struct VSOUT
{
    float4 Position         : POSITION;
    float4 Diffuse			: COLOR;
    float2 TexCoord0        : TEXCOORD0;
};

uniform extern sampler DiffuseMap               : register(s0); // Diffuse texture

float4 Modulate2X( VSOUT Input ) : COLOR
{
    float4 DiffuseColor = tex2D( DiffuseMap, Input.TexCoord0 );
    DiffuseColor = DiffuseColor * Input.Diffuse * 2;
        
    return DiffuseColor;
}