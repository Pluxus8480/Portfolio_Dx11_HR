
#include "Shader_Defines.hlsli"

float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_WorldInvMatrix;
textureCUBE		g_Texture;
Texture2D g_Texture2D, g_PositionTexture, g_Noise_Effect_Texture_01;

uint g_ObjectID;

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

float2 g_vResolution;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vTexcoord : TEXCOORD;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vTexcoord : TEXCOORD0;
};

// 리턴값 VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)
/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
/* SV_ : Shader Value : 내가 셰이더에서 처리해야할 모든 연산을 끝냈으니 앞으로는 니가 알아서 해야할 과정을 수행해라. */

/* VertexShader */
/* 1. 정점의 기본적인 변환(월드, 뷰, 투영)을 수행한다.: 로컬스페이스상의 정점정보를 투영행렬까지 곱해서 처리해주는 과정이 필요하다 . */
/* 2. 정점의 구성을 변경한다.  */
VS_OUT VS_MAIN(VS_IN In)
{	
	VS_OUT			Out = (VS_OUT)0;	

	matrix			matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;		
}

VS_OUT VS_DECAL(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vPosition;

    return Out;
}
/* Triangle List로 그리고 있는 경우, 정점 세개가 정점 쉐이더를 통과할 때까지 모으는 과정을 거치낟. */
/* w나누기 연산을 통해서 투영에 해당하는 과정을 거치낟. 정점들은 투영스페이스(-1, 1 ~ 1, -1) 상에 존재하낟. */
/* 뷰포트 스페이스로 변환한다. 정점들은 윈도우 좌표공간(0, 0 ~ winsizex, winsizey)에 존재한다.  */
/* 래스라이즈를 수행하게된다. : 정점 정보를 보간하여 픽셀의 정보를 생성한다. */
/* 생성된 픽셀을 픽셀 쉐이더로 던져서 픽셀의 색을 결정한다. */

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    uint	vPixelColor : SV_TARGET2;
    float4 vSky : SV_TARGET5;
};
struct PS_OUT_DECAL
{
    float4 vColor : SV_TARGET0;
};
struct PS_OUT_STENCIL
{
    uint iId : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vPixelColor = g_ObjectID;
	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}
PS_OUT PS_SKY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a <= 0.1f)
        discard;
    Out.vSky = Out.vColor;
    return Out;
}

PS_OUT_DECAL PS_SSD_RGB(PS_IN In)
{
    PS_OUT_DECAL Out = (PS_OUT_DECAL) 0;
    
    float2 vTexUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);

    vector vWorldPos = g_PositionTexture.Sample(LinearSampler, vTexUV);
    vector vLocalPos = mul(vWorldPos, g_WorldInvMatrix);

    float3 ObjectAbsPos = abs(vLocalPos.xyz);
    clip(0.5f - ObjectAbsPos);

    float2 decalUV = vLocalPos.xz + 0.5f;
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, decalUV);

    float4 mask = g_Texture2D.Sample(LinearSampler, decalUV);
    

    Out.vColor = g_vEffectBaseColorR * mask.r + g_vEffectBaseColorG * mask.g + g_vEffectBaseColorB * mask.b;
    Out.vColor.a *= mask.a * g_fEffectAlpha;

    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
    return Out;
}
PS_OUT_DECAL PS_SSD_MONOCOLOR(PS_IN In)
{
    PS_OUT_DECAL Out = (PS_OUT_DECAL) 0;
    
    float2 vTexUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);
    //vTexUV.x = (In.vPosition.x / In.vPosition.w) * 0.5f + 0.5f;
    //vTexUV.y = (In.vPosition.y / In.vPosition.w) * -0.5f + 0.5f;
    
    vector vWorldPos = g_PositionTexture.Sample(LinearSampler, vTexUV);
    
    vector vLocalPos = mul(vWorldPos, g_WorldInvMatrix);

    float3 ObjectAbsPos = abs(vLocalPos.xyz);
    
    clip(0.5f - ObjectAbsPos);


    float2 decalUV = vLocalPos.xz + 0.5f;
    
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, decalUV);

    float4 mask = g_Texture2D.Sample(LinearSampler, decalUV);
    
    Out.vColor = g_vEffectBaseColor;
    Out.vColor.a = mask.r * g_fEffectAlpha;

    if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    {
        discard;
    }
    return Out;
}


PS_OUT_DECAL PS_SSD_WARNING(PS_IN In)
{
    PS_OUT_DECAL Out = (PS_OUT_DECAL) 0;
    
    float2 vTexUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);
    
    vector vWorldPos = g_PositionTexture.Sample(LinearSampler, vTexUV);
    vector vLocalPos = mul(vWorldPos, g_WorldInvMatrix);

    float3 ObjectAbsPos = abs(vLocalPos.xyz);
    clip(0.5f - ObjectAbsPos);

    float2 decalUV = vLocalPos.xz + 0.5f;
    
    float fX = (decalUV.x - 0.5f);
    fX = 0.25f - fX * fX;
    float fThreshold = fX + 0.75f;

    decalUV = (decalUV * g_vUVFlow - g_vUVFlow + 1.f);
    float fThreshold2 = (g_fDissolveThreshold * g_vUVFlow.y - g_vUVFlow.y + g_fDissolveThreshold);
    fThreshold2 = fX + fThreshold2 * 0.75f;
    if (decalUV.y <= fThreshold)
    {
        ;
        
        Out.vColor = g_vEffectBaseColorR;
        Out.vColor.a *= g_fEffectAlpha + (fThreshold - decalUV.y < g_fEffectColorBlendRate) * 0.5f;
    }
    else
        discard;
    
    if (decalUV.y <= fThreshold2)
    {
        Out.vColor = g_vEffectBaseColorG;
        Out.vColor.a *= g_fEffectAlpha + (fThreshold2 - decalUV.y < g_fEffectColorBlendRate) * 0.5f;
    }

    return Out;
}

PS_OUT_DECAL PS_SSD_WARNING_CIRCLE(PS_IN In)
{
    PS_OUT_DECAL Out = (PS_OUT_DECAL) 0;
    
    float2 vTexUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);
    
    vector vWorldPos = g_PositionTexture.Sample(LinearSampler, vTexUV);
    vector vLocalPos = mul(vWorldPos, g_WorldInvMatrix);

    float3 ObjectAbsPos = abs(vLocalPos.xyz);
    clip(0.5f - ObjectAbsPos);

    float2 decalUV = vLocalPos.xz + 0.5f;
    
    float fX = decalUV.x - 0.5f;
    float fY = decalUV.y - 0.5f;
    float fRadious = sqrt(fX * fX + fY * fY);
    if (fRadious > 0.5f)
    {
        discard;
    }
    if (fRadious > g_fDissolveThreshold * 0.5f)
    {
        Out.vColor = g_vEffectBaseColorR;
        Out.vColor.a *= g_fEffectAlpha + (0.5f - fRadious < g_fEffectColorBlendRate) * 0.5f;
    }
    else
    {
        Out.vColor = g_vEffectBaseColorG;
        Out.vColor.a *= g_fEffectAlpha + (g_fDissolveThreshold - fRadious < g_fEffectColorBlendRate) * 0.5f;
    }

    return Out;
}

PS_OUT_DECAL PS_SSD_WARNING_CIRCLE_CLIP(PS_IN In)
{
    PS_OUT_DECAL Out = (PS_OUT_DECAL) 0;

    float2 vTexUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);

    vector vWorldPos = g_PositionTexture.Sample(LinearSampler, vTexUV);
    vector vLocalPos = mul(vWorldPos, g_WorldInvMatrix);

    float3 ObjectAbsPos = abs(vLocalPos.xyz);
    clip(0.5f - ObjectAbsPos);

    float2 decalUV = vLocalPos.xz + 0.5f;

    float fX = decalUV.x - 0.5f;
    float fY = decalUV.y - 0.5f;
    float fRadious = sqrt(fX * fX + fY * fY);
    if (fRadious > 0.5f)
    {
        discard;
    }

    // 각도를 계산하여 부채꼴 모양을 만듭니다. atan2 함수로 각도를 계산합니다.
    float angle = atan2(fY, fX); // 라디안 값으로 계산
    if (angle < 0.0f) // atan2는 -PI에서 PI 범위의 값을 반환하므로 0~2PI로 변환
        angle += 2.0f * 3.14159265359f;

    // 60도를 제외한 부분 (예: 60도 ~ 120도 제외)
    float excludeStartAngle = radians(30.0f); // 시작 각도 60도
    float excludeEndAngle = radians(150.0f); // 끝 각도 120도
    if (angle >= excludeStartAngle && angle <= excludeEndAngle)
    {
        discard; // 이 범위의 각도를 제외
    }

    // 디졸브 효과 및 색상 처리
    if (fRadious > g_fDissolveThreshold * 0.5f)
    {
        Out.vColor = g_vEffectBaseColorR;
        Out.vColor.a *= g_fEffectAlpha + (0.5f - fRadious < g_fEffectColorBlendRate) * 0.5f;
    }
    else
    {
        Out.vColor = g_vEffectBaseColorG;
        Out.vColor.a *= g_fEffectAlpha + (g_fDissolveThreshold - fRadious < g_fEffectColorBlendRate) * 0.5f;
    }

    return Out;
}

PS_OUT_STENCIL PS_SSD_SPOTLIGHT(PS_IN In)
{
    PS_OUT_STENCIL Out = (PS_OUT_STENCIL) 0;
    
    float2 vTexUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);
    //vTexUV.x = (In.vPosition.x / In.vPosition.w) * 0.5f + 0.5f;
    //vTexUV.y = (In.vPosition.y / In.vPosition.w) * -0.5f + 0.5f;
    
    vector vWorldPos = g_PositionTexture.Sample(LinearSampler, vTexUV);
    
    vector vLocalPos = mul(vWorldPos, g_WorldInvMatrix);

    float3 ObjectAbsPos = abs(vLocalPos.xyz);
    
    float fDist = sqrt(vLocalPos.x * vLocalPos.x + vLocalPos.z * vLocalPos.z);
    if (fDist > 0.5f)
        discard;
    
    clip(0.5f - ObjectAbsPos);


    float2 decalUV = vLocalPos.xz + 0.5f;
    
    float4 vNoise = g_Noise_Effect_Texture_01.Sample(LinearSampler, decalUV);
    Out.iId = 255 * g_fEffectAlpha;
    
    //float4 mask = g_Texture2D.Sample(LinearSampler, decalUV);
    
    //Out.vColor = g_vEffectBaseColor;
    //Out.vColor.a = mask.r * g_fEffectAlpha;

    //if (vNoise.r < g_fDissolveThreshold || Out.vColor.a <= 0.05f)
    //{
    //    discard;
    //}
    
    return Out;
}

technique11			DefaultTechnique
{

	pass Default
	{	
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
    pass SKY_BOX_1
    {
        SetRasterizerState(RS_CULL_CW);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SKY();
    }
    pass GRID_BOX_2
    {
        SetRasterizerState(RS_CULL_CW);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SKY();
    }
    pass SSD
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_DECAL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SSD_RGB();
    }
    pass SSD_MONOCOLOR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_DECAL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SSD_MONOCOLOR();
    }
    pass SSD_WARNING
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_DECAL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SSD_WARNING();
    }

    pass SSD_WARNING_CIRCLE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_DECAL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SSD_WARNING_CIRCLE();
    }

    pass SSD_WARNING_CIRCLE_CLIP
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_DECAL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SSD_WARNING_CIRCLE_CLIP();
    }
    pass SSD_SPOTLIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NonZ, 0);//카메라와 데칼 박스가 충돌시 그려지지 않는 문제 해결(z 테스트를 안하면 됨)
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_DECAL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SSD_SPOTLIGHT();
    }
//
}

