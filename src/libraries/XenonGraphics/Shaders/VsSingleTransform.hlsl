struct VSOUT
{
    float4 Position         : POSITION;
    float2 TexCoord0        : TEXCOORD0;
};


uniform float4x4 g_mWorldViewProj;              // World-view-projection matrix


VSOUT SingleTransform( const float3 Position   : POSITION,
                       const float3 Normal     : NORMAL, 
                       const float2 TexCoord0  : TEXCOORD0)
{
    VSOUT  Output;

    // Transform the vertex
    Output.Position = mul( float4(Position, 1.0f), g_mWorldViewProj );

    // Pass thru base texcoords
    Output.TexCoord0 = TexCoord0;

    return Output;
}