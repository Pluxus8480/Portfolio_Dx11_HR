#include "Shader_Defines.hlsli"
/* VertexShader */
/* 1. 정점의 기본적인 변환(월드, 뷰, 투영)을 수행한다. */


/* 2. 정점의 구성을 변경한다. */
// vector == float4 float3 float2 float
// matrix == float4x4 

/* 상수테이블. == 전역변수 */
float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;
float4			g_Color = float4(1.f,1.f,1.f,1.f);
float           g_fHp = 1.f;
float           g_fProgress = 0.f;
float           g_fMagicNumber;


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD;
    
    float4 vRight       : TEXCOORD1;
    float4 vUp          : TEXCOORD2;
    float4 vLook        : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime    : TEXCOORD5;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
    float2      vLifeTime : TEXCOORD1;
};

// 리턴값 VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)

/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
// SV_ : 사용자 지정 연산을 끝내고 장치에게 넘겨주는 시멘틱
VS_OUT VS_MAIN(VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	
	
    matrix matWV, matWVP, matInstance;
    //{
    //In.vRight,
    //In.vUp,
    //In.vLook,
    //In.vTranslation,
    //};
    matInstance =  matrix(
                    In.vRight,
                    In.vUp,
                    In.vLook,
                    In.vTranslation);
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    vector vPosition = mul(float4(In.vPosition, 1.f), mul(matInstance, matWVP));
    
    Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
	return Out;		
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
    float2      vLifeTime : TEXCOORD1;
	
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
	if(Out.vColor.a <= 0.1f)
    {
        discard;
    }
	
    Out.vColor.a = saturate(In.vLifeTime.x - In.vLifeTime.y);
    
    return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
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


technique11 DefaultTechnique
{
	pass Default
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader  = NULL;
		HullShader		= NULL;
		DomainShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

    pass HpBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HP_BAR();
    }
    pass Blend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLEND();
    }

    pass RoundProgress
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ROUND_PROGRESS();
    }

    
    pass ShakeVertical
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHAKE_VERTICAL();
    }

    pass BlendPoint
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLEND_POINT();
    }
    pass FromRight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FROM_RIGHT();
    }
    pass FromLeft
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FROM_LEFT();
    }
    //pass Default_1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    GeometryShader = NULL;
    //    HullShader = NULL;
    //    DomainShader = NULL;
    //    PixelShader = compile ps_5_0 PS_MAIN();
    //}
}
