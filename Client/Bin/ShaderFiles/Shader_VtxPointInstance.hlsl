#include "Shader_Defines.hlsli"
/* VertexShader */
/* 1. 정점의 기본적인 변환(월드, 뷰, 투영)을 수행한다. */


/* 2. 정점의 구성을 변경한다. */
// vector == float4 float3 float2 float
// matrix == float4x4 

/* 상수테이블. == 전역변수 */
float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D       g_Texture, g_Noise_Effect_Texture_01;
float4			g_Color = float4(1.f,1.f,1.f,1.f);
float           g_fHp = 1.f;
float           g_fProgress = 0.f;

float4          g_vCamPosition;
float           g_fMagicNumber;

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

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vSize : PSIZE;
    
    row_major  matrix Transform   : WORLD;
    float2 vLifeTime    : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vSize     : PSIZE;
    row_major matrix Transform : WORLD;
    float2      vLifeTime : TEXCOORD0;
};

// 리턴값 VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)

/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
// SV_ : 사용자 지정 연산을 끝내고 장치에게 넘겨주는 시멘틱
VS_OUT VS_MAIN(VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    vector vPosition = mul(float4(In.vPosition, 1.f), mul(In.Transform, g_WorldMatrix));
    
    Out.vPosition = vPosition;
    Out.vSize = In.vSize;
    //float2(In.vSize.x * length(In.Transform._11_12_13), In.vSize.y * length(In.Transform._21_22_23));
    Out.Transform = In.Transform;
    Out.vLifeTime = In.vLifeTime;
	return Out;		
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vSize : PSIZE;
    row_major matrix Transform : WORLD;
    float2 vLifeTime : TEXCOORD0;
};
struct GS_OUT
{
    float4 vPosition : SV_POSITION;

    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> VertexStream)
{
    GS_OUT Out[4];
    
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[3].vLifeTime = In[0].vLifeTime;
    
    //float4 vLook  = g_vCamPosition - In[0].vPosition;
    //float4 vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vSize.x * .5f, 0.f);
    //float4 vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)) * In[0].vSize.y * .5f, 0.f);
    
    float4 vRight = In[0].Transform._11_12_13_14 * In[0].vSize.x * 0.5f;
    //float4 vLook;
    float4 vUp = In[0].Transform._21_22_23_24 * In[0].vSize.y * 0.5f;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    
    
    VertexStream.Append(Out[0]);
    VertexStream.Append(Out[1]);
    VertexStream.Append(Out[2]);
    VertexStream.RestartStrip();
    
    VertexStream.Append(Out[0]);
    VertexStream.Append(Out[2]);
    VertexStream.Append(Out[3]);
    VertexStream.RestartStrip();
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;

    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

//float4 PS_MAIN(PS_IN In) : COLOR0
//{
//
//}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    Out.vColor.a *= saturate((In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.x * 2.f);
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
	if(Out.vColor.a <= 0.1f)
    {
        discard;
    }
	

    return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    Out.vColor.a *= saturate((In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.x * 2.f);
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }

    
    return Out;
}

PS_OUT PS_MAIN_BLEND_POINT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord) * g_Color;
    Out.vColor.a *= saturate((In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.x * 2.f);
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_MAIN_HP_BAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    Out.vColor.a *= saturate((In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.x * 2.f);
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (In.vTexcoord.x > g_fHp)
    {
        Out.vColor.a *= 0.6f;
    }
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
    return Out;
}

PS_OUT PS_MAIN_ROUND_PROGRESS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    float2 vUV;
    vUV = In.vTexcoord * 2.f - 1.f;
    
    float fAngle = atan2(vUV.x, vUV.y);
    
    if (fAngle <= g_fProgress - 3.14f)
    {
        Out.vColor.rgb *= 0.2f;
    }
    Out.vColor.a *= saturate((In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.x * 2.f);
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
    
	
    return Out;
}

PS_OUT PS_MAIN_SHAKE_VERTICAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    In.vTexcoord.y += sin(In.vTexcoord.x * g_fProgress) * 0.1f;
    
    
    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord) * g_Color;
    Out.vColor.a *= saturate((In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.x * 2.f);
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (In.vTexcoord.y < g_fHp)
    {
        Out.vColor.gb = 0.f;
    }
    
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
  	
    return Out;
}

PS_OUT PS_FROM_RIGHT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord) * g_Color;
    Out.vColor.a *= saturate((In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.x * 2.f);
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (In.vTexcoord.x > g_fProgress)
    {
        discard;
    }
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_FROM_LEFT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord) * g_Color;
    Out.vColor.a *= saturate((In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.x * 2.f);
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (In.vTexcoord.x < g_fProgress)
    {
        discard;
    }
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_DECALTODIFFUSE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    Out.vColor.a = Out.vColor.r * 0.33f + Out.vColor.g * 0.33f + Out.vColor.b * 0.33f;
    Out.vColor.a *= saturate((In.vLifeTime.x - In.vLifeTime.y) / In.vLifeTime.x * 2.f);
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a <= 0.1f)
    {
        discard;
    }
	
    
    return Out;
}

PS_OUT PS_PARTICLE_DIFFUSE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor.a = Out.vColor.a * g_fEffectAlpha;
    
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
    
    return Out;
}

PS_OUT PS_PARTICLE_DIFFUSE_MASK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor.a = Out.vColor.a * g_fEffectAlpha;
    
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
    
    Out.vColor.r = g_vEffectBaseColor.r;
    Out.vColor.g = g_vEffectBaseColor.g;
    Out.vColor.b = g_vEffectBaseColor.b;
    
    return Out;
}

PS_OUT PS_PARTICLE_SPARK_DIFFUSE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = (In.vTexcoord + g_vUVFlow) * g_vUVMultiplier;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor.a = Out.vColor.a * g_fEffectAlpha;
    
    if (Out.vColor.r <= 0.05f)
    {
        discard;
    }
	    
    Out.vColor.r = g_vEffectBaseColor.r;
    Out.vColor.g = g_vEffectBaseColor.g;
    Out.vColor.b = g_vEffectBaseColor.b;
    
    return Out;
}

float fire_aperture = 0.22f;

PS_OUT PS_FIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 shifted_uv = In.vTexcoord + g_vUVFlow;
    //shifted_uv.y *= In.vLifeTime.y;
    
    float fire_noise = g_Texture.Sample(LinearSampler, shifted_uv).r;

    float noise = In.vTexcoord.y * (((In.vTexcoord.y + fire_aperture) * fire_noise - fire_aperture) * 75.0f);
    float4 fire_color = lerp(g_vEffectBaseColorR, g_vEffectBaseColorG, noise * 0.05);

    Out.vColor.a = clamp(noise, 0.0, 1.0) * g_fEffectAlpha;
    Out.vColor.rgb = fire_color.rgb * Out.vColor.a;
   
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, In.vTexcoord);
    
    if (vNoise.r < In.vLifeTime.y / In.vLifeTime.x || Out.vColor.a <= 0.05f)
    {
        discard;
    }
    return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
		VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader  = compile gs_5_0 GS_MAIN();
		HullShader		= NULL;
		DomainShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

    pass HpBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HP_BAR();
    }
    pass Blend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLEND();
    }

    pass RoundProgress
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ROUND_PROGRESS();
    }

    
    pass ShakeVertical
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHAKE_VERTICAL();
    }

    pass BlendPoint
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLEND_POINT();
    }
    pass FromRight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FROM_RIGHT();
    }
    pass FromLeft
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FROM_LEFT();
    }

    pass DecalToDiffuse
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DECALTODIFFUSE();
    }

    pass ParticlePtInstance
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PARTICLE_DIFFUSE();
    }

    pass ParticleSparkPtInstance
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PARTICLE_SPARK_DIFFUSE();
    }

    pass ParticleFirePtInstance
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FIRE();
    }

    pass ParticlePtInstanceMask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PARTICLE_DIFFUSE_MASK();
    }
}
