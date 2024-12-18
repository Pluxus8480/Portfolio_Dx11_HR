#include "Shader_Defines.hlsli"

float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D		g_Texture;
float4			g_Color = float4(1.f,1.f,1.f,1.f);
float           g_fProgress = 0.f;
float2 g_vResolution;
float g_fNoiseStrength, g_Dissolve, g_DissolveRange = 0.1f;
float g_fMagicNumber;

Texture2D g_Noise_Effect_Texture_01;
Texture2D g_Emissive_Texture;
Texture2D g_Alpha_Texture;
float4 g_vEffectBaseColor;

float4 g_vEffectBaseColorR;
float4 g_vEffectBaseColorG;
float4 g_vEffectBaseColorB;


float g_fEffectTimeAcc;
float2 g_vUVFlowSpeed;
float2 g_vUVFlow;
float2 g_vUVMultiplier;
float g_fEffectAlpha;

float g_fEffectColorBlendRate;
float g_fDissolveThreshold;

float2 g_vConvexCenter;
float g_fConvexRadius;
float g_fConvexPower;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	
	
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;

	return Out;		
}

VS_OUT VS_FULLSCREEN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    In.vPosition.xy = sign(In.vPosition).xy;
    Out.vPosition = float4(In.vPosition.xy, 1.f, 1.f);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord);

    float4 mask = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float4 g_vEffectBaseColor0 = float4(0.87f, 0.18f, 0.19f, 1.f);
    float4 g_vEffectBaseColor1 = float4(0.96f, 0.8f, 0.22f, 1.f);
    float4 g_vEffectBaseColor2 = float4(0.f, 0.f, 0.f, 0.f);
    
    Out.vColor = g_vEffectBaseColor0 * mask.r + g_vEffectBaseColor1 * mask.g + g_vEffectBaseColor2 * mask.b;
    Out.vColor.a = mask.a;
    
    

    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_UV_FLOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, UV);

    float4 mask = g_Texture.Sample(LinearSampler, UV);
    

    Out.vColor = g_vEffectBaseColorR * mask.r + g_vEffectBaseColorG * mask.g + g_vEffectBaseColorB * mask.b;
    Out.vColor.a = mask.a * g_fEffectAlpha;
    
    

    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_WEIGHTBLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, UV);
    
    float4 mask = g_Texture.Sample(LinearSampler, UV);
    
    
    Out.vColor = g_vEffectBaseColorR * mask.r + g_vEffectBaseColorG * mask.g + g_vEffectBaseColorB * mask.b;
    Out.vColor.a *= mask.a;
    Out.vColor.rgb *= Out.vColor.a;
    
    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_WEIGHTBLEND_MONOCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, UV);
    
    float4 mask = g_Texture.Sample(LinearSampler, UV);
    
    
    Out.vColor = g_vEffectBaseColor;
    Out.vColor.a *= mask.r;
    Out.vColor.rgb *= Out.vColor.a;
    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_NONBLEND_MONOCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, UV);
    
    float4 mask = g_Texture.Sample(LinearSampler, UV);
    
    
    Out.vColor = g_vEffectBaseColor;
    Out.vColor.a = mask.r;
    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_BLEND_MONOCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord);
    
    float4 mask = g_Texture.Sample(LinearSampler, In.vTexcoord + g_vUVFlowSpeed * g_fEffectTimeAcc);
    
    
    Out.vColor = g_vEffectBaseColor * mask.r;
    
    if (Out.vColor.r == 0.f && Out.vColor.g == 0.f && Out.vColor.b == 0.f)
    {
        discard;
    }
    
    Out.vColor.a *= g_fEffectAlpha;
    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_BLEND_ORIGINCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;

    Out.vColor = g_Texture.Sample(LinearSampler, UV);

    Out.vColor.a =  Out.vColor.a * g_fEffectAlpha;
    
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_BLEND_ORIGINCOLOR_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = g_Texture.Sample(LinearSampler, UV);

    Out.vColor.a = Out.vColor.a * g_fEffectAlpha;
    
    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_FLARE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord);
    
    vNoise.r -= g_fDissolveThreshold;
    
    Out.vColor = g_vEffectBaseColor;
    Out.vColor.a = vNoise.r * 2.f;
    Out.vColor.rgb *= Out.vColor.a;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_FLAREALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = g_vEffectBaseColor;
    Out.vColor.a = vNoise.r * 2.f;
    
    Out.vColor.rgb *= Out.vColor.a;
    if (Out.vColor.a <= g_fEffectColorBlendRate)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_MASKFLARE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord);
    
    vNoise.r -= g_fDissolveThreshold;
    
    //Out.vColor = g_vEffectBaseColor;
    
    float4 mask = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = g_vEffectBaseColorR * mask.r + g_vEffectBaseColorG * mask.g + g_vEffectBaseColorB * mask.b;
    
    
    Out.vColor.a = vNoise.r * 2.f;
    Out.vColor.rgb *= Out.vColor.a;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_CONVEX_LENS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 vDist = g_vConvexCenter - In.vTexcoord;
    vDist.y *= g_vResolution.y / g_vResolution.x;
    float fDist = length(vDist);
    if (fDist > g_fConvexRadius)
        discard;
    
    vDist = normalize(vDist);
    float fDiff = g_fConvexRadius - fDist;
    float2 vUV = In.vTexcoord + vDist * min(fDiff, fDist * 2.f) * g_fConvexPower;
    
    Out.vColor = g_Texture.Sample(LinearSampler, vUV);
    Out.vColor.a = 1.f;
    return Out;
}

PS_OUT PS_EXPLOSION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, UV);

    float4 mask = g_Texture.Sample(LinearSampler, UV);
    float4 mask2 = g_Emissive_Texture.Sample(LinearSampler, UV);
    float4 alpha = g_Alpha_Texture.Sample(LinearSampler, UV);
    

    Out.vColor = g_vEffectBaseColorR * mask.r + g_vEffectBaseColorG * mask2.r;
    Out.vColor.a = alpha.r * g_fEffectAlpha;
    
    

    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_TEXTURE_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, UV);

    Out.vColor = g_Texture.Sample(LinearSampler, UV);
    

    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

float fire_aperture = 0.22f;

PS_OUT PS_FIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 shifted_uv = In.vTexcoord + g_vUVFlow;
   ;
    float fire_noise = g_Texture.Sample(LinearSampler, shifted_uv).r;

    float noise = In.vTexcoord.y * (((In.vTexcoord.y + fire_aperture) * fire_noise - fire_aperture) * 75.0f);
    float4 fire_color = lerp(g_vEffectBaseColorR, g_vEffectBaseColorG, noise * 0.05);

    Out.vColor.a = clamp(noise, 0.0, 1.0) * g_fEffectAlpha;
    Out.vColor.rgb = fire_color.rgb * Out.vColor.a;
    
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord);
    
    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
    return Out;
}

PS_OUT PS_FIREWALL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow);
    
    float4 mask = g_Texture.Sample(LinearSampler, UV);
    
    Out.vColor = g_vEffectBaseColor;
    Out.vColor.a = mask.r;
    if (Out.vColor.a < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_WARNING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, UV);

    float4 mask = g_Texture.Sample(LinearSampler, UV);
    

    Out.vColor = g_vEffectBaseColorR * mask.r + g_vEffectBaseColorG * mask.g + g_vEffectBaseColorB * mask.b;
    Out.vColor.a = mask.a * g_fEffectAlpha * 2.f;
    Out.vColor += vNoise * saturate(vNoise.a - g_fDissolveThreshold);
   
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}
float4 g_vThreshold;
PS_OUT PS_INVISIBLE_WALL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord) * g_vUVMultiplier;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord * g_vUVMultiplier);

    float4 mask = g_Texture.Sample(LinearSampler, In.vTexcoord * g_vUVMultiplier);
    

    Out.vColor = g_vEffectBaseColorR * mask.r + g_vEffectBaseColorG * mask.g + g_vEffectBaseColorB * mask.b;
    Out.vColor.a = mask.a * g_fEffectAlpha;
    Out.vColor.rgb *= Out.vColor.a;
    float x = UV.x - floor(UV.x);
    float fTheshhold = g_fDissolveThreshold;
    if(x < 0.25f)
        fTheshhold -= g_vThreshold.x;
    else if(x < .5f)
        fTheshhold -= g_vThreshold.y;
    else if (x < .75f)
        fTheshhold -= g_vThreshold.z;
    else
        fTheshhold -= g_vThreshold.w;
    
    if (vNoise.r < fTheshhold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_FLARE_BACK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord);
    
    vNoise.r -= g_fDissolveThreshold;
    
    Out.vColor = g_vEffectBaseColor;
    Out.vColor.a = max(vNoise.r, 0.3f) * 2.f;
    Out.vColor.rgb *= Out.vColor.a;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_WHITEOUT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord);
    
    vNoise.r -= g_fDissolveThreshold;
    
    Out.vColor = g_vEffectBaseColor;
    Out.vColor.a *= vNoise.r * 2.f;
	
    return Out;
}

PS_OUT PS_TOWER_ELEC(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, UV);

    float4 mask = g_Texture.Sample(LinearSampler, UV);
    

    Out.vColor = g_vEffectBaseColorR * mask.r + g_vEffectBaseColorG * mask.g + g_vEffectBaseColorB * mask.b + g_vEffectBaseColor * max(mask.a - mask.r - mask.g - mask.b, 0.f);
    Out.vColor.a = mask.a * g_fEffectAlpha;
    
    

    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}
PS_OUT PS_BG_OVERRIDER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 UV = (In.vTexcoord) + g_vUVFlow;


    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, UV);
    if (vNoise.r + 0.2f < g_fDissolveThreshold)
    {
        discard;
    }
    float4 mask = g_Texture.Sample(LinearSampler, UV);
    
    Out.vColor = g_vEffectBaseColor;

    if (mask.r <= 0.5f)
    {
        float fLerp = mask.r * 2.f;
        Out.vColor = g_vEffectBaseColorR * (1.f - fLerp) + g_vEffectBaseColor * fLerp;
    }
	
    return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader  = NULL;
		HullShader		= NULL;
		DomainShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
    pass WeightBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WEIGHTBLEND();
    }
    pass WeightBlendMonoColor
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WEIGHTBLEND_MONOCOLOR();
    }
    pass NonBlendMonoColor
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_NONBLEND_MONOCOLOR();
    }

    pass UVFlow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_UV_FLOW();
    }
    pass Flare
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FLARE();
    }
    pass BlendOriginColor
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_ORIGINCOLOR();
    }
    pass ConvexLens
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NonZ, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_CONVEX_LENS();
    }

    pass Explosion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_EXPLOSION();
    }
    pass TextureColor
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TEXTURE_COLOR();
    }
    pass Fire
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FIRE();
    }
    pass Warning
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WARNING();
    }
    pass MaskFlare
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MASKFLARE();
    }
    pass BlendMonoColor
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_MONOCOLOR();
    }
    pass InvisibleWall
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_INVISIBLE_WALL();
    }
    pass BlendOriginColorDissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_ORIGINCOLOR_DISSOLVE();
    }
    pass FireWall
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FIREWALL();
    }
    pass FlareAlpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FLAREALPHA();
    }
    pass BACKEFFECT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FLARE_BACK();
    }
    pass LENS_WHITEOUT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NonZ, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WHITEOUT();
    }
    pass TOWER_ELEC
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TOWER_ELEC();
    }
    pass BG_OVERRIDE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NonZ, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BG_OVERRIDER();
    }
    pass BG_OVERRIDE_TEXT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NonZ, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TEXTURE_COLOR();
    }
//
}
