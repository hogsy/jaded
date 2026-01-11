//-----------------------------------------------------------------------------
// Light Shaft Pixel Shader
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler2D S_AllSamplers[4] : register(s0);

// Default
#define S_CookieTexture 0
#define S_NoiseTexture  1
#define S_ShadowTexture 2
#define S_DepthTexture  3

// Blur
#define S_BlurTexture 0

// Apply
#define S_LightShaftSampler 0
#define S_FrustumSampler    1
#define S_DepthSampler      2

// Resample
#define S_ResampleSampler 0

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------
float4 g_vLightShaftColor;
float4 g_vLightShaftAttenuation;

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define g_fLightShaftIntensity g_vLightShaftColor.a
#define g_fLightShaftAttStart  g_vLightShaftAttenuation.r
#define g_fLightShaftAttFactor g_vLightShaftAttenuation.g

#define LightShaftMode_Render   0
#define LightShaftMode_Apply    1
#define LightShaftMode_Blur     2
#define LightShaftMode_Resample 3

//-----------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------
struct PSIN
{
    float4 Diffuse        : COLOR0_centroid;
    float4 CookieTexCoord : TEXCOORD0_centroid;
    float4 NoiseTexCoord1 : TEXCOORD1_centroid;
    float4 NoiseTexCoord2 : TEXCOORD2_centroid;
    float4 ShadowTexCoord : TEXCOORD3_centroid;
    float4 DepthTexCoord  : TEXCOORD4_centroid;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

float4 ProcessRenderPixel(PSIN _input, int _nbNoiseTex, int _enableShadow, int _enableColor, int _enableFog)
{
    float4 vColor;
    float  fCookie;
    float  fAttenuation;
    float  fNoise = 0.0f;

    // Cookie
    fCookie = tex2Dproj( S_AllSamplers[S_CookieTexture], _input.CookieTexCoord ).a;

    // Noise
    if ( _nbNoiseTex >= 1 )
    {
        fNoise += 2.0f * fCookie * (tex2Dproj( S_AllSamplers[S_NoiseTexture], _input.NoiseTexCoord1 ).a - 0.5f);
    }
    if ( _nbNoiseTex >= 2 )
    {
        fNoise += 2.0f * fCookie * (tex2Dproj( S_AllSamplers[S_NoiseTexture], _input.NoiseTexCoord2 ).a - 0.5f);
    }

    // Attenuation
    fAttenuation = 1.0f - (g_fLightShaftAttFactor * saturate( _input.CookieTexCoord.z - g_fLightShaftAttStart ) );

    if ( _enableShadow != 0 )
    {
        float fDistance = ( _input.ShadowTexCoord.z / _input.ShadowTexCoord.w );

        if ( tex2Dproj( S_AllSamplers[S_ShadowTexture], _input.ShadowTexCoord ).r < fDistance )
           fAttenuation = 0.0f;
    }

    vColor = g_vLightShaftColor * fAttenuation * ( fNoise + fCookie );

    if ( _enableColor != 0 )
    {
        vColor *= _input.Diffuse;

        float4 vDepth = _input.DepthTexCoord / _input.DepthTexCoord.w;
        vDepth.x = 0.5f * ( vDepth.x + 1.0f );
        vDepth.y = 0.5f * ( 1.0f - vDepth.y );
        float  fDepth = tex2D( S_AllSamplers[S_DepthTexture], vDepth.xy ).r;
        if ( fDepth < vDepth.z )
            vColor = 0.0f;

        if (_enableFog)
        {
            vColor.rgba = lerp( vColor.rgba, float4( 0.0f, 0.0f, 0.0f, 0.0f ), _input.NoiseTexCoord1.z );
        }
    }
    else
    {
        vColor *= g_fLightShaftIntensity;

        if (_enableFog)
        {
            vColor.rgb = lerp( vColor.rgb, float3( 0.0f, 0.0f, 0.0f ), _input.NoiseTexCoord1.z );
        }
    }

    return vColor;
}

float4 ProcessApplyPixel(PSIN _input, int _saturateColors)
{
    float4 vColor = tex2D( S_AllSamplers[S_LightShaftSampler], _input.CookieTexCoord );

    if ( _saturateColors != 0 )
    {
        return 0.1f * vColor;
    }
    else
    {
        return vColor;
    }
}

float4 ProcessBlurPixel(PSIN _input, int _combineColors)
{
    float  fIntensity;
    float4 vColor;

    vColor  = tex2D( S_AllSamplers[S_BlurTexture], _input.CookieTexCoord );
    vColor += tex2D( S_AllSamplers[S_BlurTexture], _input.NoiseTexCoord1 );
    vColor += tex2D( S_AllSamplers[S_BlurTexture], _input.NoiseTexCoord2 );
    vColor += tex2D( S_AllSamplers[S_BlurTexture], _input.ShadowTexCoord );

    if ( _combineColors != 0 )
    {
        fIntensity = dot( vColor, float4( 0.25f, 0.25f, 0.25f, 0.25f ) );

        return g_vLightShaftColor * float4( fIntensity, fIntensity, fIntensity, 0.0f );
    }
    else
    {
        return vColor * 0.25f;
    }
}

float4 ProcessResamplePixel(PSIN _input)
{
#if defined(_XENON)

    return tex2D( S_AllSamplers[S_ResampleSampler], _input.CookieTexCoord );

#else

    return tex2D( S_AllSamplers[S_ResampleSampler], _input.CookieTexCoord ).r;
    
#endif
}

float4 ProcessPixel (PSIN _input, 
                     int  _mode, 
                     int  _nbNoiseTex, 
                     int  _enableShadow, 
                     int  _enableColor, 
                     int  _combineColors, 
                     int  _saturateColors, 
                     int  _enableFog, 
                     int  iUnused8,
                     int  iUnused9,
                     int  iUnused10,
                     int  iUnused11,
                     int  iUnused12,
                     int  iUnused13,
                     int  iUnused14,
                     int  iUnused15) : COLOR0
{
    if (_mode == LightShaftMode_Apply)
    {
        return ProcessApplyPixel(_input, _saturateColors);
    }
    else if (_mode == LightShaftMode_Blur)
    {
        return ProcessBlurPixel(_input, _combineColors);
    }
    else if (_mode == LightShaftMode_Resample)
    {
        return ProcessResamplePixel(_input);
    }
    else
    {
        return ProcessRenderPixel(_input, _nbNoiseTex, _enableShadow, _enableColor, _enableFog);
    }
}
