#include "Shader_Defines.hlsli"
/* VertexShader */
/* 1. 정점의 기본적인 변환(월드, 뷰, 투영)을 수행한다. */


/* 2. 정점의 구성을 변경한다. */
// vector == float4 float3 float2 float
// matrix == float4x4 

/* 상수테이블. == 전역변수 */
float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;
texture2D		g_MaskTexture;
texture2D		g_DiffuseTexture[3];
float           g_fMagicNumber;
float4          g_vAmbientColor;

struct VS_IN
{
    float3		vPosition : POSITION;
    float3		vNormal   : NORMAL;
	float2		vTexcoord : TEXCOORD;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
    float4      vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
    float4      vWorldPos : TEXCOORD1;
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
	
    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	return Out;		
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
	
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
    float4      vNormal : SV_TARGET3;

};
struct PS_OUT_EMISSION
{
    float4 vEmission : SV_TARGET5;
};
struct PS_OUT_NON_LIT
{
    float4 vColor : SV_TARGET0;
};

//float4 PS_MAIN(PS_IN In) : COLOR0
//{
//
//}

PS_OUT_NON_LIT PS_MAIN(PS_IN In)
{
    PS_OUT_NON_LIT Out = (PS_OUT_NON_LIT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor *= g_vAmbientColor;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
	if(Out.vColor.a < 0.1f)
    {
        discard;
    }

    return Out;
}

PS_OUT_NON_LIT PS_MONOCROME_MASK(PS_IN In)
{
    PS_OUT_NON_LIT Out = (PS_OUT_NON_LIT) 0;
    vector vMaskColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vMaskColor * g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord) + (1.f - vMaskColor) * g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord);
    Out.vColor *= g_vAmbientColor;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a < 0.05f)
    {
        discard;
    }

    return Out;
}
PS_OUT_NON_LIT PS_MUZZLE_FLASH(PS_IN In)
{
    PS_OUT_NON_LIT Out = (PS_OUT_NON_LIT) 0;
    vector vMaskColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vMaskColor * g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = vMaskColor;
    Out.vColor *= g_vAmbientColor;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a < 0.05f)
    {
        discard;
    }

    return Out;
}

PS_OUT_NON_LIT PS_SMOKE_TRAIL(PS_IN In)
{
    PS_OUT_NON_LIT Out = (PS_OUT_NON_LIT) 0;
    In.vTexcoord.y += cos(g_fMagicNumber * In.vTexcoord.x * 10.f) * 0.25f;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.b;
    Out.vColor *= g_vAmbientColor;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a < 0.05f)
    {
        discard;
    }

    return Out;
}

PS_OUT_NON_LIT PS_WALL(PS_IN In)
{
    PS_OUT_NON_LIT Out = (PS_OUT_NON_LIT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor *= 4.f * g_fMagicNumber;
    Out.vColor.a = 0.5f * g_fMagicNumber;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    
    return Out;
}
PS_OUT_NON_LIT PS_CHARGE(PS_IN In)
{
    PS_OUT_NON_LIT Out = (PS_OUT_NON_LIT) 0;
    vector vMaskColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vMaskColor * g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord) * 2.f;
    Out.vColor.a = vMaskColor * g_fMagicNumber;
    Out.vColor *= g_vAmbientColor;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a < 0.05f)
    {
        discard;
    }

    return Out;
}

PS_OUT_NON_LIT PS_RUNE(PS_IN In)
{
    PS_OUT_NON_LIT Out = (PS_OUT_NON_LIT) 0;
 
    Out.vColor = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord);
    Out.vColor += g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) * g_fMagicNumber;
    Out.vColor *= g_vAmbientColor;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a < 0.05f)
    {
        discard;
    }

    return Out;
}

PS_OUT_NON_LIT PS_MASKTODIFFUSE(PS_IN In)
{
    PS_OUT_NON_LIT Out = (PS_OUT_NON_LIT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) * g_fMagicNumber * 0.33f;
    Out.vColor *= g_vAmbientColor;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }

    return Out;
}

PS_OUT_NON_LIT PS_COLORBOOST(PS_IN In)
{
    PS_OUT_NON_LIT Out = (PS_OUT_NON_LIT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_fMagicNumber;
    Out.vColor *= g_vAmbientColor;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a < 0.05f)
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

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader  = NULL;
		HullShader		= NULL;
		DomainShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
    pass MuzzleFlash
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MUZZLE_FLASH();
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
    pass SmokeTrail
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SMOKE_TRAIL();
    }
    pass WallProjectile
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WALL();
    }
    pass Charge
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_CHARGE();
    }
    pass Rune
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RUNE();
    }
    pass MaskToDiffuse
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MASKTODIFFUSE();
    }
    pass ColorBoost
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLORBOOST();
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
