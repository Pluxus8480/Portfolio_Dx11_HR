#include "Shader_Defines.hlsli"
/* VertexShader */
/* 1. 정점의 기본적인 변환(월드, 뷰, 투영)을 수행한다. */


/* 2. 정점의 구성을 변경한다. */
// vector == float4 float3 float2 float
// matrix == float4x4 

/* 상수테이블. == 전역변수 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture, g_Normal_Texture, g_ATOS_Texture;

Texture2D g_Noise_Effect_Texture_01;

Texture2D g_Emission, g_Noise_Texture, g_PerlinNoise_Texture, g_Mask_Texture;

float g_fMagicNumber, g_Dissolve, g_DissolveRange = 0.1f;
float4 g_vColor;
float4 g_vAmbientColor;
float4 g_vCamPos;

float2 g_vResolution;
float g_fNoiseStrength;

float4 g_vEffectBaseColorR;
float4 g_vEffectBaseColorG;
float4 g_vEffectBaseColorB;


float g_fEffectTimeAcc;
float2 g_vUVFlowSpeed;
float2 g_vUVFlow;
float2 g_vUVMultiplier;
float g_fEffectAlpha;

float4 g_vEffectBaseColor;
float g_fDissolveThreshold;
float g_fEffectColorBlendRate;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vNormal : TEXCOORD2;
};

// 리턴값 VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)

/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
// SV_ : 사용자 지정 연산을 끝내고 장치에게 넘겨주는 시멘틱
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    return Out;
}

VS_OUT VS_DISTORT_X(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    //vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vPosition = mul(float4(In.vPosition, 1.f) + vector(In.vNormal, 0.f) * sin(0.033f + In.vTexcoord.x * PI * 2.f) * 10.f, matWVP);
    return Out;
}

VS_OUT VS_LASER_TEX(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP, matVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float fY = length(g_WorldMatrix._21_22_23);
    Out.vTexcoord = float2(In.vTexcoord.x , In.vTexcoord.y * fY / 200.f);
    
    float fNoise = g_PerlinNoise_Texture.SampleLevel(LinearSampler, float2(In.vTexcoord.x + g_vUVFlow.x, In.vTexcoord.y + g_vUVFlow.y), 0).r * g_fEffectColorBlendRate;
    
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));

    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix) + Out.vNormal * fNoise;

    Out.vPosition = mul(Out.vWorldPos, matVP);
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vNormal : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    uint vID : SV_TARGET1;
    float4 vPosition : SV_TARGET2;
    float4 vNormal : SV_TARGET3;
    //float4 vORM : SV_TARGET4;
    float4 vEmission : SV_TARGET4;
};
struct PS_OUT2
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float2 vMoveTexcoord = float2(In.vTexcoord.x * 0.5f, In.vTexcoord.y);
    
    vMoveTexcoord.x = vMoveTexcoord.x + g_fEffectTimeAcc;
    
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, vMoveTexcoord);
 
    if (vMoveTexcoord.x < 0.0 || vMoveTexcoord.x > 1.0)
    {
        discard;
    }
    
    float4 specificColor = g_vEffectBaseColor;
    //float4(0.07f, 0.17f, 0.29f, 1.f);
    
    // 노이즈 텍스처와 특정 색상 혼합
    float blendFactor = g_fEffectColorBlendRate; // 혼합 비율 (0.0 - 1.0)
    
    float4 blendedColor = lerp(vNoise, specificColor, blendFactor);
    
    //g_fTime
    
    blendedColor.a = vNoise.r * 1.f;
    
    Out.vColor += blendedColor;
    
    float fAlpha = g_fEffectTimeAcc + 0.5f;
    
    if(fAlpha > 0.5f)
        Out.vColor.a = Out.vColor.a * (1.f - fAlpha) * 2.f;
    
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    
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


float2 speed = float2(1.f, 1.f);
float barrier_force = 1.0f;
float barrier_noise_force = 0.6;
float barrier_fog_noise_force = 0.3;

PS_OUT PS_BARRIER(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    
    float3 viewDir = normalize(g_vCamPos.xyz - In.vWorldPos.xyz / In.vWorldPos.w);
    float rim = pow(1.0 - dot(In.vNormal.xyz, viewDir), 3.0f) * barrier_force;
   
    float2 p = g_Noise_Effect_Texture_01.Sample(LinearSampler, -In.vTexcoord).xy + g_fEffectTimeAcc * speed;
    float line_noise = clamp((sin(g_Noise_Effect_Texture_01.Sample(LinearSampler, p).r * 3.1415926535) - 0.995) * 90.0, 0.0, 1.0) * barrier_noise_force;

    float fog_noise = g_Noise_Effect_Texture_01.Sample(LinearSampler, float2(In.vTexcoord.x * 8.0 - g_fEffectTimeAcc * speed.x, In.vTexcoord.y * 8.0)).r * barrier_fog_noise_force;
    float hexGrid = g_Texture.Sample(LinearSampler, float2(In.vTexcoord.x * 12.f, In.vTexcoord.y * 7.f)).r * saturate(0.125f - abs(g_fEffectTimeAcc - In.vTexcoord.y)) * 4.f;

    Out.vColor.a = clamp(rim + line_noise + fog_noise + hexGrid, 0.0, 1.0);
    Out.vColor.rgb = (g_vEffectBaseColor.rgb + float3(0.1f, 0.1f, 0.1f) * (1.f - rim)) * Out.vColor.a;
    return Out;
}

float fire_aperture = 0.22f;

PS_OUT PS_FIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 shifted_uv = In.vTexcoord + g_vUVFlow;
    
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
#define THRESHOLD_1 0.7f
#define THRESHOLD_2 0.43f

PS_OUT PS_DRAKEN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 UV = In.vTexcoord + float2(0.1f, 1.3f) * g_fEffectTimeAcc; 
    float2 UV2 = In.vTexcoord + float2(0.3f,0.7f) * g_fEffectTimeAcc;
    

    UV *= float2(2.f, 1.f);
    UV2 *= float2(1.f, 0.2f);
    float fNoise = g_Texture.Sample(LinearSampler, UV).r * 0.7f + g_Texture.Sample(LinearSampler, UV2).r * 0.3f;

    if (fNoise < THRESHOLD_2)
    {
        Out.vColor = lerp(g_vEffectBaseColorR, g_vEffectBaseColorG, fNoise / THRESHOLD_2);
    }
    else if (fNoise < THRESHOLD_1)
    {
        Out.vColor = lerp(g_vEffectBaseColorG, g_vEffectBaseColorB, (fNoise - THRESHOLD_1) / (THRESHOLD_2 - THRESHOLD_1));
    }
    else
    {
        Out.vColor = g_vEffectBaseColorB;
    }

    Out.vColor.a *= g_fEffectAlpha;
    Out.vColor.rgb *= Out.vColor.a;
    return Out;
}



PS_OUT PS_MONOCOLOR_MESH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 shifted_uv = In.vTexcoord + g_vUVFlow;
   ;
    float vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, shifted_uv).r;

    Out.vColor.a = vNoise.r * g_fEffectAlpha;
    Out.vColor.rgb = g_vEffectBaseColor.rgb;
    
    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
    return Out;
}

PS_OUT PS_MONOCOLOR_MESH_ACCURATE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 shifted_uv = In.vTexcoord + g_vUVFlow;
   ;
    float vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, shifted_uv).r;

    Out.vColor.a = g_fEffectAlpha;
    Out.vColor.rgb = g_vEffectBaseColor.rgb * Out.vColor.a;
    
    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
    return Out;
}

PS_OUT2 PS_DISTORTION(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);

    vector vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y + sin(g_fEffectTimeAcc)));
    float fNoise = vNoise.x * 2.f - 1.f;
    vUV += fNoise * g_fNoiseStrength;

    Out.vColor = g_Texture.Sample(LinearSampler, vUV);
    Out.vColor.a = 1.f;
    //if (Out.vColor.a < 0.05f)
    //{
    //    discard;
    //}
    return Out;
}

PS_OUT2 PS_DISTORT_RIM(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);

    vector vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y + sin(g_fEffectTimeAcc * 1.52)));
    float fNoise = vNoise.x * 2.f - 1.f;
    vUV += fNoise * g_fNoiseStrength;
    
    float3 viewDir = normalize(g_vCamPos.xyz - In.vWorldPos.xyz / In.vWorldPos.w);
    float rim = pow(max(0.0f, 1.0f - abs(dot(normalize(In.vNormal.xyz), viewDir))), 10.0f);
    Out.vColor = g_Texture.Sample(LinearSampler, vUV);
    Out.vColor += g_vEffectBaseColor * rim * g_fDissolveThreshold;
    Out.vColor.a = 1.f;
    //if (Out.vColor.a < 0.05f)
    //{
    //    discard;
    //}
    return Out;
}

PS_OUT2 PS_DISTORT_JUDGEMENT(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);

    vector vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y));
    float fNoise = vNoise.x * 2.f - 1.f;
    vUV += fNoise * g_fNoiseStrength;
    
    float3 viewDir = normalize(g_vCamPos.xyz - In.vWorldPos.xyz / In.vWorldPos.w);
    float rim = pow(max(0.0f, 1.0f - abs(dot(normalize(In.vNormal.xyz), viewDir))), 10.0f);
    Out.vColor = g_Texture.Sample(LinearSampler, vUV);
    Out.vColor += g_vEffectBaseColor * rim * g_fDissolveThreshold;
    Out.vColor.a = g_fEffectAlpha;
    //if (Out.vColor.a < 0.05f)
    //{
    //    discard;
    //}
    return Out;
}


PS_OUT2 PS_DISTORT_MONOCOLOR(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);

    vector vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y + sin(g_fEffectTimeAcc * 1.52)));
    float fNoise = vNoise.x * 2.f - 1.f;
    vUV += fNoise * g_fNoiseStrength;

    Out.vColor = g_Texture.Sample(LinearSampler, vUV);
    Out.vColor += g_vEffectBaseColor * g_fDissolveThreshold;
    Out.vColor.a = g_fEffectAlpha;
    //if (Out.vColor.a < 0.05f)
    //{
    //    discard;
    //}
    return Out;
}

PS_OUT2 PS_REVERSECOLOR(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);

    //vector vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y + sin(g_fEffectTimeAcc * 1.52)));
    //float fNoise = vNoise.x * 2.f - 1.f;
    //vUV += fNoise * g_fNoiseStrength;
    
    float3 viewDir = normalize(g_vCamPos.xyz - In.vWorldPos.xyz / In.vWorldPos.w);
    float rim = pow(max(0.0f, 1.0f - abs(dot(normalize(In.vNormal.xyz), viewDir))), 10.0f);
    Out.vColor = g_Texture.Sample(LinearSampler, vUV);
    float gray = dot(Out.vColor.rgb, float3(0.299f, 0.587f, 0.114f));

    // 범위 조정: 0에서 0.05, 0.95에서 1 사이로 제한
    if (gray < 0.5)
    {
        gray = lerp(0.0, 0.05, gray / 0.5); // 어두운 부분을 0 ~ 0.05 사이로
    }
    else
    {
        gray = lerp(0.95, 1.0, (gray - 0.5) / 0.5); // 밝은 부분을 0.95 ~ 1.0 사이로
    }
    
    Out.vColor.x = gray; 
    Out.vColor.y = gray;
    Out.vColor.z = gray;
    Out.vColor.a = 1.f;
    //if (Out.vColor.a < 0.05f)
    //{
    //    discard;
    //}
    return Out;
}

PS_OUT2 PS_REVERSECOLOREX(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);

// Calculate the view direction and rim light effect
    float3 viewDir = normalize(g_vCamPos.xyz - In.vWorldPos.xyz / In.vWorldPos.w);
    float rim = pow(max(0.0f, 1.0f - abs(dot(normalize(In.vNormal.xyz), viewDir))), 10.0f);

// Sample the texture
    float4 originalColor = g_Texture.Sample(LinearSampler, vUV);

// Invert the color
    float4 invertedColor;
    invertedColor.rgb = 1.0f - originalColor.rgb;
    invertedColor.a = originalColor.a;

// Blend between the original and inverted colors based on dissolveValue
    float dissolveValue = saturate(g_fDissolveThreshold); // Ensure dissolveValue is clamped between 0 and 1
    Out.vColor = lerp(invertedColor, originalColor, dissolveValue);

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
    if (x < 0.25f)
        fTheshhold -= g_vThreshold.x;
    else if (x < .5f)
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

technique11 DefaultTechnique
{
    pass Default
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass AlphaBlend
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass EffectCCW
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass UVFlow
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_UV_FLOW();
    }
    pass Barrier
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BARRIER();
    }
    pass Fire
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FIRE();
    }
    pass Distortion
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }
    pass DistortRim
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORT_RIM();
    }
    pass MonoColorMesh
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MONOCOLOR_MESH();
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
    pass ColorReverse
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REVERSECOLOR();
    }
    pass DistortMonocolor
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORT_MONOCOLOR();
    }
    pass DrakkenLaser
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_LASER_TEX();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DRAKEN();
    }
    pass MonoColorMeshAccurate
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MONOCOLOR_MESH_ACCURATE();
    }
    pass ColorReverseEx
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REVERSECOLOREX();
    }
    pass JudgementCut
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORT_JUDGEMENT();
    }
//PS_INVISIBLE_WALL
}
