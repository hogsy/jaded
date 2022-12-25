//-----------------------------------------------------------------------------
// Rain Pixel Shader
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
uniform sampler2D S_TextureSamplers[3] : register(s0);

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------
uniform float  g_fRainAlphaBoost;
uniform float  g_fRainAlphaIntensity;
uniform float4 g_vRainColor;

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define RAIN_PSMODE_NORMAL 0
#define RAIN_PSMODE_BLUR   1
#define RAIN_PSMODE_APPLY  2

//-----------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------
struct PSIN
{
    float4 TexCoord0 : TEXCOORD0_centroid;
    float2 TexCoord1 : TEXCOORD1_centroid;
    float2 TexCoord2 : TEXCOORD2_centroid;
    float2 TexCoord3 : TEXCOORD3_centroid;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

float4 ProcessNormalPixel(PSIN _input)
{
    float4 vColor = tex2D( S_TextureSamplers[0], _input.TexCoord0 ) * g_vRainColor * float4( 1.0f, 1.0f, 1.0f, _input.TexCoord0.a );

    return vColor;
}

float4 ProcessBlurPixel(PSIN _input)
{
    float4 vColor;

    vColor  = tex2D( S_TextureSamplers[0], _input.TexCoord0 );
    vColor += tex2D( S_TextureSamplers[0], _input.TexCoord1 );
    vColor += tex2D( S_TextureSamplers[0], _input.TexCoord2 );
    vColor += tex2D( S_TextureSamplers[0], _input.TexCoord3 );

    return vColor * 0.25f;
}

float4 ProcessApplyPixel(PSIN _input)
{
    float4 vBlur1Color = tex2D( S_TextureSamplers[0], _input.TexCoord0 );
    float4 vBlur2Color = tex2D( S_TextureSamplers[1], _input.TexCoord0 );
    float4 vBackColor  = tex2D( S_TextureSamplers[2], _input.TexCoord0 );

    float fFactor1 = saturate( vBackColor.a * g_fRainAlphaBoost );
    float fFactor2 = saturate( ( vBackColor.a * g_fRainAlphaBoost ) - 1.0f );

    return lerp( vBackColor, lerp( vBlur1Color, vBlur2Color, fFactor2 ), fFactor1 ) + vBackColor.a * g_fRainAlphaIntensity;
}

float4 ProcessPixel (PSIN _input, 
                     int  _mode, 
                     int  iUnused2, 
                     int  iUnused3, 
                     int  iUnused4, 
                     int  iUnused5, 
                     int  iUnused6, 
                     int  iUnused7,
                     int  iUnused8,
                     int  iUnused9,
                     int  iUnused10,
                     int  iUnused11,
                     int  iUnused12,
                     int  iUnused13,
                     int  iUnused14,
                     int  iUnused15) : COLOR0
{
    if ( _mode == RAIN_PSMODE_APPLY )
    {
        return ProcessApplyPixel(_input);
    }
    else if ( _mode == RAIN_PSMODE_BLUR )
    {
        return ProcessBlurPixel(_input);
    }
    else
    {
        return ProcessNormalPixel(_input);
    }
}
