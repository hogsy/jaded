// Lighting.hlsl

float4 ProcessDirLight(in float3 vModelSpaceNormal, in float3 vModelSpacePosition, in int iCurLight, in bool bSpecular, in int bTwoSided)
{
	float2 vFactors;
	float3 vHalfWay;
	float4 vColor;

	// calculate diffuse contribution (N dot L)
	vFactors.x = dot(vModelSpaceNormal, g_aoLights[iCurLight].Direction.xyz);
	
	if (bTwoSided)
	{
		vFactors.x = abs(vFactors.x);
		vFactors.x += 0.15;
		vFactors.x *= 0.8;
	}
		
	if (bSpecular)
	{
		// calculate halfway vector
		vHalfWay	= normalize(g_vViewerPosition.xyz-vModelSpacePosition.xyz);
		vHalfWay	+= g_aoLights[iCurLight].Direction.xyz; 
		vHalfWay	= normalize(vHalfWay);

		// calculate specular contribution (H dot L)
		vFactors.y = dot(vModelSpaceNormal, vHalfWay);

		// saturate all factors
		vFactors.xy = saturate(vFactors.xy);

		// apply shininess
		vFactors.y = pow(vFactors.y, g_fShininess);

		// Diffuse
		vColor  = g_aoLights[iCurLight].Color * vFactors.x * g_vMatDiffuseColor;		
		
		// Add specular
		vFactors.x = min(vFactors.x*1000000.0f, 1.0f);
		vColor += g_aoLights[iCurLight].Color * g_aoLights[iCurLight].Color.w * vFactors.x * vFactors.y * g_vMatSpecularColor;
	}
	else
	{
		// saturate all factors
		vFactors.x = saturate(vFactors.x);

		vColor  = g_aoLights[iCurLight].Color * vFactors.x * g_vMatDiffuseColor;
	}

	return vColor;
}

float4 ProcessOmniLight(in float3 vModelSpaceNormal, in float3 vModelSpacePosition, in int iCurLight, in bool bSpecular, in int bTwoSided)
{
	float3 vHalfWay;
	float3 vDirection;
	float4 vFactors;
	float4 vColor;
	
	// calculate light direction
	vDirection = g_aoLights[iCurLight].Position.xyz - vModelSpacePosition.xyz;
	
	// normalize direction
	vFactors.z = length(vDirection);
	vDirection = vDirection / vFactors.z;
		
	// calculate contribution factor (N dot L)
	vFactors.x = dot(vModelSpaceNormal, vDirection);
	
	if (bTwoSided)
	{
		vFactors.x = abs(vFactors.x);
		vFactors.x += 0.15;
		vFactors.x *= 0.8;
	}

	// apply attenuation ((far - dist) / (far - near))
	vFactors.y = (g_aoLights[iCurLight].Params.x - vFactors.z) * g_aoLights[iCurLight].Params.y;

	if (bSpecular)
	{
		// calculate halfway vector
		vHalfWay	= normalize(g_vViewerPosition.xyz-vModelSpacePosition.xyz);
		vHalfWay	+= vDirection;
		vHalfWay	= normalize(vHalfWay);

		// calculate specular contribution (H dot L)
		vFactors.w = dot(vModelSpaceNormal, vHalfWay);

		// saturate all factors
		vFactors = saturate(vFactors);

		// shininess
		vFactors.w = pow(vFactors.w, g_fShininess);
		
		// Diffuse
		vColor  = g_aoLights[iCurLight].Color * vFactors.x * g_vMatDiffuseColor;
		
		// Add specular
		vFactors.x = min(vFactors.x*1000000.0f, 1.0f);		
		vColor += g_aoLights[iCurLight].Color * g_aoLights[iCurLight].Color.w * vFactors.w * vFactors.x * g_vMatSpecularColor;
	}
	else
	{
		// saturate all factors
		vFactors.xy = saturate(vFactors.xy);
		
		vColor	= g_aoLights[iCurLight].Color * vFactors.x * g_vMatDiffuseColor;
	}
	
	// use green channel as the light intensity
	vColor *= vFactors.y;
	
	return vColor;	
}

float4 ProcessSpotLight(in float3 vModelSpaceNormal, in float3 vModelSpacePosition, in int iCurLight, in bool bSpecular, in int bTwoSided)
{
	float3 vDirection, vPosToLight;
	float4 vFactors;
	float3 vHalfWay;
	float4 vColor;
	float  fLength;
			
	// calculate light direction
	vPosToLight = g_aoLights[iCurLight].Position.xyz - vModelSpacePosition.xyz;
	
	// normalize direction
	fLength = length(vPosToLight);
	vDirection = vPosToLight / fLength;

	// calculate contribution (N dot L)
	vFactors.x = dot(vModelSpaceNormal, vDirection);
	
	if (bTwoSided)
	{
		vFactors.x = abs(vFactors.x);
		vFactors.x += 0.15;
		vFactors.x *= 0.8;
	}

	// apply attenuation ((far - dist) / (far - near))
	vFactors.y = (g_aoLights[iCurLight].Params.x - fLength) * g_aoLights[iCurLight].Params.y;
	
	// apply spotlight cone ((cos(alpha) - outer) / (inner - outer))
	vFactors.z = (dot(g_aoLights[iCurLight].Direction.xyz, vDirection) - g_aoLights[iCurLight].Params.z) * g_aoLights[iCurLight].Params.w;	
	
	if (bSpecular)
	{
		// calculate halfway vector 
		vHalfWay	= normalize(g_vViewerPosition.xyz-vModelSpacePosition.xyz);
		vHalfWay	+= vDirection;
		vHalfWay	= normalize(vHalfWay);

		// calculate specular contribution (H dot L)
		vFactors.w = dot(vModelSpaceNormal, vHalfWay);

		// saturate all factors
		vFactors = saturate(vFactors);

		// shininess
		vFactors.w = pow(vFactors.w, g_fShininess);

		// Diffuse
		vColor  = g_aoLights[iCurLight].Color * vFactors.x * g_vMatDiffuseColor;

		// Add Specular
		vFactors.x = min(vFactors.x*1000000.0f, 1.0f);
		vColor += g_aoLights[iCurLight].Color * g_aoLights[iCurLight].Color.w * vFactors.w * vFactors.x * g_vMatSpecularColor;
	}
	else
	{
		// saturate all factors
		vFactors.xyz = saturate(vFactors.xyz);

		vColor  = g_aoLights[iCurLight].Color * vFactors.x * g_vMatDiffuseColor;
	}

	// use green channel as the light intensity
	vColor *= vFactors.y * vFactors.z;
	
	return vColor;
}

float4 ProcessCylSpotLight(in float3 vModelSpaceNormal, in float3 vModelSpacePosition, in int iCurLight, in bool bSpecular, in int bTwoSided)
{
	float3 vDirection, vPosToLight;
	float4 vFactors;
	float3 vHalfWay;
	float4 vColor;
	float  fLength;
			
	// calculate light direction
	vPosToLight = g_aoLights[iCurLight].Position.xyz - vModelSpacePosition.xyz;
	
	// normalize direction
	fLength = length(vPosToLight);
	vDirection = vPosToLight / fLength;

	// calculate contribution (N dot L)
	vFactors.x = dot(vModelSpaceNormal, vDirection);
	
	if (bTwoSided)
	{
		vFactors.x = abs(vFactors.x);
		vFactors.x += 0.15;
		vFactors.x *= 0.8;
	}

	// apply attenuation ((far - dist) / (far - near))
	vFactors.y = (g_aoLights[iCurLight].Params.x - fLength) * g_aoLights[iCurLight].Params.y;
	
	vDirection = g_aoLights[iCurLight].Direction.xyz;
	
	// apply cylinder (1 - ((d    - inner) / (outer - inner)))
	float fDot = dot(g_aoLights[iCurLight].Direction.xyz, -vPosToLight);
	vFactors.z = 0.0f;
	if (fDot < 0.0f)
	{
		float d = length(-vPosToLight - (fDot * vDirection));
		vFactors.z = 1.0f - saturate((d - g_aoLights[iCurLight].Params.z) * g_aoLights[iCurLight].Params.w);		
	}
	
	if (bSpecular)
	{
		// calculate halfway vector 
		vHalfWay	= normalize(g_vViewerPosition.xyz-vModelSpacePosition.xyz); 
		vHalfWay	+= vDirection;
		vHalfWay	= normalize(vHalfWay);

		// calculate specular contribution (H dot L)
		vFactors.w = dot(vModelSpaceNormal, vHalfWay);
		
		// saturate all factors
		vFactors = saturate(vFactors);

		// shininess
		vFactors.w = pow(vFactors.w, g_fShininess);		
		
		// Diffuse
		vColor  = g_aoLights[iCurLight].Color * vFactors.x * g_vMatDiffuseColor;
		
		// Specular
		vFactors.x = min(vFactors.x*1000000.0f, 1.0f);
		vColor += g_aoLights[iCurLight].Color * g_aoLights[iCurLight].Color.w * vFactors.w * vFactors.x * g_vMatSpecularColor;
	}
	else
	{
		// saturate all factors
		vFactors.xyz = saturate(vFactors.xyz);

		vColor  = g_aoLights[iCurLight].Color * vFactors.x * g_vMatDiffuseColor;
	}

	// use green channel as the light intensity
	vColor *= vFactors.y * vFactors.z;
	
	return vColor;
}

#ifndef VS_NO_TEX_COORD
#ifndef VS_NO_NORMAL

float ComputePPLFromDirLight(in int iCurLightIndex, in TangentSpaceBasis Basis, in float3 vModelSpacePosition, in float3 vTangentSpaceViewer, in int bTwoSided, inout VSOUT Output )
{	
	if (iCurLightIndex != 3)
	{
		// transform direction to tangent space
		Output.LightDir[iCurLightIndex].x = dot(Basis.Tangent,  g_aoLights[iCurLightIndex].Direction);
		Output.LightDir[iCurLightIndex].y = dot(Basis.BiNormal, g_aoLights[iCurLightIndex].Direction);
		Output.LightDir[iCurLightIndex].z = dot(Basis.Normal,   g_aoLights[iCurLightIndex].Direction);

	    // calculate halfway vector
	    Output.HalfWay[iCurLightIndex].xyz = normalize( Output.LightDir[iCurLightIndex].xyz + vTangentSpaceViewer );
	
		if(bTwoSided)
		{
			Output.LightDir[iCurLightIndex] = abs(Output.LightDir[iCurLightIndex]);
			Output.HalfWay[iCurLightIndex] = abs(Output.HalfWay[iCurLightIndex]);
		}
	}
	else
	{
	    float3 vLightDir;
        vLightDir.x = dot(Basis.Tangent,  g_aoLights[iCurLightIndex].Direction);
		vLightDir.y = dot(Basis.BiNormal, g_aoLights[iCurLightIndex].Direction);
		vLightDir.z = dot(Basis.Normal,   g_aoLights[iCurLightIndex].Direction);
    
	    float3 vHalfWay = normalize( vLightDir + vTangentSpaceViewer );

		if(bTwoSided)
		{
		    vLightDir = abs( vLightDir );
		    vHalfWay = abs( vHalfWay );
		}
		
		Output.LightDir[0].w = vLightDir.x;
		Output.LightDir[1].w = vLightDir.y;
		Output.LightDir[2].w = vLightDir.z;

    	Output.HalfWay[0].w = vHalfWay.x;
		Output.HalfWay[1].w = vHalfWay.y;
		Output.HalfWay[2].w = vHalfWay.z;
	}
	
	return 1.0f;
}

float ComputePPLFromOmniLight(in int iCurLightIndex, in TangentSpaceBasis Basis, in float3 vModelSpacePosition, in float3 vTangentSpaceViewer, in int bTwoSided, inout VSOUT Output)
{
	float2 vFactors;

	// find light direction
	float3 vDirection;
	
	vDirection = g_aoLights[iCurLightIndex].Position.xyz - vModelSpacePosition.xyz;
	
	// apply inversion if necessary
	vDirection *= g_aoLights[iCurLightIndex].Position.w;
	
	// normalize direction
	vFactors.x = length(vDirection);

	vDirection = vDirection / vFactors.x;
		
	// calculate attenuation ((far - dist) / (far - near))
	vFactors.y = (g_aoLights[iCurLightIndex].Params.x - vFactors.x) * g_aoLights[iCurLightIndex].Params.y;
	
	// saturate all factors
	vFactors = saturate(vFactors);
	
	if (iCurLightIndex != 3)
	{
		// transform direction to tangent space
		Output.LightDir[iCurLightIndex].x = dot(Basis.Tangent,  vDirection);
		Output.LightDir[iCurLightIndex].y = dot(Basis.BiNormal, vDirection);
		Output.LightDir[iCurLightIndex].z = dot(Basis.Normal,   vDirection); 
		
		// calculate halfway vector
	    Output.HalfWay[iCurLightIndex].xyz = normalize( Output.LightDir[iCurLightIndex].xyz + vTangentSpaceViewer );

		if(bTwoSided)
		{
			Output.LightDir[iCurLightIndex] = abs(Output.LightDir[iCurLightIndex]);
			Output.HalfWay[iCurLightIndex] = abs(Output.HalfWay[iCurLightIndex]);
		}
	}
	else
	{
	    float3 vLightDir;
        vLightDir.x = dot(Basis.Tangent,  vDirection);
		vLightDir.y = dot(Basis.BiNormal, vDirection);
		vLightDir.z = dot(Basis.Normal,   vDirection);
    
	    float3 vHalfWay = normalize( vLightDir + vTangentSpaceViewer );

		if(bTwoSided)
		{
		    vLightDir = abs( vLightDir );
		    vHalfWay = abs( vHalfWay );
		}
		
		Output.LightDir[0].w = vLightDir.x;
		Output.LightDir[1].w = vLightDir.y;
		Output.LightDir[2].w = vLightDir.z;

    	Output.HalfWay[0].w = vHalfWay.x;
		Output.HalfWay[1].w = vHalfWay.y;
		Output.HalfWay[2].w = vHalfWay.z;
	}
	
	return vFactors.y;
}

float ComputePPLFromSpotLight(in int iCurLightIndex, in TangentSpaceBasis Basis, in float3 vModelSpacePosition, in float3 vTangentSpaceViewer, in int bTwoSided, inout VSOUT Output, in bool bCylindricalAttenuation)
{
	float3 vFactors;
	float3 vDirection, vPosToLight;
	
	// find light direction 
	vPosToLight = g_aoLights[iCurLightIndex].Position.xyz - vModelSpacePosition.xyz;
	
	// normalize direction
	vFactors.x = length(vPosToLight);
		
	// calculate attenuation ((far - dist) / (far - near))
	vFactors.y = (g_aoLights[iCurLightIndex].Params.x - vFactors.x) * g_aoLights[iCurLightIndex].Params.y;
	
	if (!bCylindricalAttenuation)
	{
		vDirection = vPosToLight / vFactors.x;
	
		// apply spotlight cone ((cos(alpha) - outer) / (inner - outer))
		vFactors.z = (dot(g_aoLights[iCurLightIndex].Direction, vDirection) - g_aoLights[iCurLightIndex].Params.z) * g_aoLights[iCurLightIndex].Params.w;	
	}
	else
	{
		vDirection = g_aoLights[iCurLightIndex].Direction;
		
		// apply cylinder (1 - ((d - inner) / (outer - inner)))
		float fDot = dot(g_aoLights[iCurLightIndex].Direction, -vPosToLight);
		vFactors.z = 0.0f;
		if (fDot < 0.0f)
		{
			float d = length(-vPosToLight - (fDot * vDirection));
			vFactors.z = 1.0f - saturate((d - g_aoLights[iCurLightIndex].Params.z) * g_aoLights[iCurLightIndex].Params.w);		
		}
	}

	// saturate all factors
	vFactors = saturate(vFactors);
	
	if (iCurLightIndex != 3)
	{
		// transform direction to tangent space
		Output.LightDir[iCurLightIndex].x = dot(Basis.Tangent,  vDirection);
		Output.LightDir[iCurLightIndex].y = dot(Basis.BiNormal, vDirection);
		Output.LightDir[iCurLightIndex].z = dot(Basis.Normal,   vDirection);
			
		// transform halfway vector to tangent space
        Output.HalfWay[iCurLightIndex].xyz = normalize( Output.LightDir[iCurLightIndex].xyz + vTangentSpaceViewer );

		if(bTwoSided)
		{
			Output.LightDir[iCurLightIndex] = abs(Output.LightDir[iCurLightIndex]);
			Output.HalfWay[iCurLightIndex] = abs(Output.HalfWay[iCurLightIndex]);
		}
	}
	else
	{
	    float3 vLightDir;
        vLightDir.x = dot(Basis.Tangent,  vDirection);
		vLightDir.y = dot(Basis.BiNormal, vDirection);
		vLightDir.z = dot(Basis.Normal,   vDirection);
    
	    float3 vHalfWay = normalize( vLightDir + vTangentSpaceViewer );

		if(bTwoSided)
		{
		    vLightDir = abs( vLightDir );
		    vHalfWay = abs( vHalfWay );
		}
		
		Output.LightDir[0].w = vLightDir.x;
		Output.LightDir[1].w = vLightDir.y;
		Output.LightDir[2].w = vLightDir.z;

    	Output.HalfWay[0].w = vHalfWay.x;
		Output.HalfWay[1].w = vHalfWay.y;
		Output.HalfWay[2].w = vHalfWay.z;
	}
	
	return vFactors.y * vFactors.z;
}

#endif // VS_NO_NORMAL
#endif // VS_NO_TEX_COORD
