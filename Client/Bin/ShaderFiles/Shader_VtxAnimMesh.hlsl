#include "Shader_Defines.hlsli"
/* VertexShader */
/* 1. 정점의 기본적인 변환(월드, 뷰, 투영)을 수행한다. */


/* 2. 정점의 구성을 변경한다. */
// vector == float4 float3 float2 float
// matrix == float4x4 

/* 상수테이블. == 전역변수 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_ViewInvMatrix, g_BoneMatrix[768];
Texture2D g_Texture, g_Normal_Texture, g_ATOS_Texture, g_Emission_Texture, g_Noise_Texture, g_Flow_Texture;
float g_fMagicNumber, g_Dissolve, g_DissolveRange = 0.1f, g_RimWidth = 0.1f, g_OutlineWidth;
float4 g_vColor, g_vCamPos;
float g_fTime;
float2 g_vUVMult;

int2 g_iDivide = { 4, 4 };
uint g_iFrameIndex;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD;
    float3 vTangent : TANGENT;
    uint4 vBlendIndex0 : BLENDINDICES;
    float4 vBlendWeight0 : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vNormal : TEXCOORD2;
    float4 vTangent : TEXCOORD3;
    float4 vBiTangent : TEXCOORD4;
};

// 리턴값 VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)

/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
// SV_ : 사용자 지정 연산을 끝내고 장치에게 넘겨주는 시멘틱
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP, matBW, BoneMatrix;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    //float fW = 1.f - In.vBlendWeight.x - In.vBlendWeight.y - In.vBlendWeight.z;
    BoneMatrix = g_BoneMatrix[In.vBlendIndex0.x] * In.vBlendWeight0.x
                + g_BoneMatrix[In.vBlendIndex0.y] * In.vBlendWeight0.y
                + g_BoneMatrix[In.vBlendIndex0.z] * In.vBlendWeight0.z
                + g_BoneMatrix[In.vBlendIndex0.w] * In.vBlendWeight0.w;
    
    matBW = mul(BoneMatrix, g_WorldMatrix);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), matBW));
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), matBW));
    Out.vBiTangent = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);
    return Out;
}

VS_OUT VS_OUTLINE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matVP, matBW, BoneMatrix;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    //float fW = 1.f - In.vBlendWeight.x - In.vBlendWeight.y - In.vBlendWeight.z;
    BoneMatrix = g_BoneMatrix[In.vBlendIndex0.x] * In.vBlendWeight0.x
                + g_BoneMatrix[In.vBlendIndex0.y] * In.vBlendWeight0.y
                + g_BoneMatrix[In.vBlendIndex0.z] * In.vBlendWeight0.z
                + g_BoneMatrix[In.vBlendIndex0.w] * In.vBlendWeight0.w;
    //matrix OutlineOffset = (g_OutlineWidth, 0.f,0.f,0.f,
    //                        0.f, g_OutlineWidth, 0.f, 0.f,
    //                        0.f, 0.f, g_OutlineWidth, 0.f,
    //                        0.f, 0.f, 0.f, 1.f);
    //matrix OutlineOffset = (matrix)0;
    //OutlineOffset._11 = g_OutlineWidth;
    //OutlineOffset._22 = g_OutlineWidth;
    //OutlineOffset._33 = g_OutlineWidth;
    //OutlineOffset._44 = 1.f;
    
    matBW = mul(BoneMatrix, g_WorldMatrix);
    Out.vWorldPos = mul(float4(In.vPosition + In.vNormal * g_OutlineWidth, 1.f), matBW);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matBW));
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), matBW));
    Out.vBiTangent = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);
    
    Out.vPosition = mul(Out.vWorldPos, matVP);
    Out.vTexcoord = In.vTexcoord;

    
    return Out;
}


float FlowSpeed, Time, DisintegrationAmount;

[maxvertexcount(3)]
void GS_DISINTEGRATE(triangle VS_OUT input[3], inout TriangleStream<VS_OUT> TriStream)
{
    float2 vAbUV = input[0].vTexcoord + input[1].vTexcoord + input[2].vTexcoord;
    vAbUV *= 0.33f;
    
    //float4 vAbWorldPos = input[0].vWorldPos + input[1].vWorldPos + input[2].vWorldPos;
    //vAbWorldPos *= 0.33f;
    
    
    //float2 flowUV = vAbUV + FlowSpeed * Time;
    float4 vNoise = g_Noise_Texture.SampleLevel(LinearSampler, vAbUV, 0);
    float dis = clamp(DisintegrationAmount - vNoise.x, 0.f, DisintegrationAmount);
    float4 flowValue = g_Flow_Texture.SampleLevel(LinearSampler, vAbUV, 0) * dis * 200.f;
    matrix world = g_WorldMatrix;
    world[3] = float4(0.f, 0.f, 0.f, 1.f);
    flowValue = mul(flowValue, world);
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    
    for (int i = 0; i < 3; i++)
    {
        VS_OUT output = input[i];
        
        output.vWorldPos.xyz += flowValue.xyz;
        output.vPosition = mul(output.vWorldPos, matVP);
        
        TriStream.Append(output);
    }
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vNormal : TEXCOORD2;
    float4 vTangent : TEXCOORD3;
    float4 vBiTangent : TEXCOORD4;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    uint vID : SV_Target1;
    float4 vPosition : SV_TARGET2;
    float4 vNormal : SV_TARGET3;
    //float4    vORM    : SV_TARGET4;
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
    
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.w = 0.4f;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
    return Out;
}

PS_OUT PS_COLORBOOST(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor.rgb *= g_fMagicNumber;
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.w = 0.4f;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
    return Out;
}

PS_OUT PS_EMISSION(PS_IN In)
{

    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.w = 0.4f;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
    Out.vEmission = g_Emission_Texture.Sample(LinearSampler, In.vTexcoord);
    return Out;
}

PS_OUT PS_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vNoise = g_Noise_Texture.Sample(LinearSampler, In.vTexcoord);
    float fNoise = vNoise.x;
    if (fNoise < g_Dissolve)
    {
        float fDist = g_Dissolve - fNoise;
        if (fDist < g_DissolveRange)
        {
            Out.vColor = lerp(vector(1.f, 1.f, 1.f, 1.0f), Out.vColor, fDist / g_DissolveRange);
        }
        else
        {
            discard;
        }
    }
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.w = 0.4f;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
    return Out;
}

float4 PS_MONOCOLOR(PS_IN In) : SV_TARGET0
{
    float4 Out = (float4) 0;
    Out = g_vColor;
    Out.rgb = Out.rgb * Out.a;
    
    
    if (Out.a < 0.03f)
    {
        discard;
    }
    //Out.vID = g_iObjectID;
    //Out.vPosition = In.vWorldPos;
    //Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    //Out.vORM = g_ORM_Texture.Sample(LinearSampler, In.vTexcoord);
    return Out;
}
float4 PS_COLORONLY(PS_IN In) : SV_TARGET0
{
    float4 Out = (float4) 0;
    Out = g_Texture.Sample(LinearSampler, In.vTexcoord);
    //vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
    float3 viewDir = normalize(g_vCamPos.xyz - In.vWorldPos.xyz / In.vWorldPos.w);
    float3 vNormal = normalize(In.vNormal.xyz);
    //float fRim = pow(1.f - saturate(dot(vNormal, viewDir)), 2.f);
    float fRim = saturate(dot(vNormal, viewDir));
    Out *= g_vColor;
    Out.a *= (1.f + fRim);
    Out.rgb = Out.rgb * Out.a;
    if (Out.a < 0.1f)
    {
        discard;
    }
    //Out.vID = g_iObjectID;
    //Out.vPosition = In.vWorldPos;
    //Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    //Out.vORM = g_ORM_Texture.Sample(LinearSampler, In.vTexcoord);
    return Out;
}
PS_OUT PS_RIM_EMISSION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.w = 0.4f;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);

    
    vector vNormal = normalize(In.vNormal * 2.f - 1.f);
    float fRim = pow(1.f - saturate(dot(vNormal.xyz, normalize(g_vCamPos.xyz - In.vWorldPos.xyz / In.vWorldPos.w))), 3.f);
    Out.vEmission += g_vColor * fRim;

    
    return Out;
}
PS_OUT PS_RIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.w = 0.4f;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
    float3 viewDir = normalize(g_vCamPos.xyz - In.vWorldPos.xyz / In.vWorldPos.w);
    float3 vNormal = normalize(Out.vNormal * 2.f - 1.f).xyz;
    float fRim = pow(1.f - saturate(dot(vNormal, viewDir)), 3.f);
    Out.vEmission = g_vColor * fRim;

    
    return Out;
}
PS_OUT PS_OUTLINE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = vector(0.f, 0.f, 0.f, 1.f);
    
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    return Out;
}
vector g_vOutlineColor;
PS_OUT PS_OUTLINE_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_vOutlineColor;
    
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    return Out;
}


float2 g_vResolution;
float g_fNoiseStrength;
float4 g_vDistortionColor;

PS_OUT2 PS_DISTORTION(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);
  
    vector vNoise = g_Noise_Texture.Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y + sin(g_fTime)));
    float fNoise = vNoise.x * 2.f - 1.f;
    vUV += fNoise * g_fNoiseStrength;
    
    Out.vColor = g_Texture.Sample(LinearSampler, vUV);
    Out.vColor.rgb += g_vDistortionColor.rgb;
    Out.vColor.a = 1.f;
    //if (Out.vColor.a < 0.05f)
    //{
    //    discard;
    //}
    return Out;
}

PS_OUT2 PS_DISTORTION_BOOST(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);
  
    vector vNoise = g_Noise_Texture.Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y + sin(g_fTime)));
    float fNoise = vNoise.x * 2.f - 1.f;
    vUV += fNoise * g_fNoiseStrength;
    
    Out.vColor = g_Texture.Sample(LinearSampler, vUV);
    Out.vColor.rgb += g_vDistortionColor.rgb + vNoise.x;
    Out.vColor.a = 1.f;
    //if (Out.vColor.a < 0.05f)
    //{
    //    discard;
    //}
    return Out;
}

float4 PS_MAIN_LIGHTDEPTH(PS_IN In) : SV_TARGET0
{
    float4 Out = (float4) 0;

    Out = vector(In.vPosition.z / In.vPosition.w, In.vPosition.w / 3000.f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_NORMAL_MAP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.a = 1.f;
    return Out;
}

PS_OUT PS_EMISSION_COLOR(PS_IN In)
{

    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.a = .4f;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
    Out.vEmission = g_vColor;
    return Out;
}

PS_OUT PS_FRAME(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    int iX = g_iFrameIndex % g_iDivide.y;
    int iY = g_iFrameIndex / g_iDivide.y;
    
    float fSizeX = (1.f / (float) g_iDivide.x);
    float fSizeY = (1.f / (float) g_iDivide.y);
    
    float2 vLeftTop = { fSizeX * iX, fSizeY * iY };
    float2 vCurCoord = float2(In.vTexcoord.x * fSizeX, In.vTexcoord.y * fSizeY);
    
    Out.vColor = g_Texture.Sample(LinearSampler, vLeftTop + vCurCoord);
    
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.a = .4f;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
    Out.vEmission = g_Emission_Texture.Sample(LinearSampler, vLeftTop + vCurCoord);
    return Out;
}

PS_OUT PS_MIM_WING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    //g_vUVMult
    float2 UV = float2(In.vTexcoord.x * 8.f, In.vTexcoord.y * 16.f);
    
    Out.vColor = g_Texture.Sample(LinearSampler, UV);

    if (Out.vColor.r < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.a = .4f;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
    Out.vEmission = g_vColor * Out.vColor.r;
    return Out;
}

PS_OUT PS_DEFFEREDOUTLINE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = normalMapSample(g_Normal_Texture, In.vTexcoord, In.vTangent, In.vBiTangent, In.vNormal);
    Out.vNormal.w = 0.f;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
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

    pass Emission
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_EMISSION();
    }
    pass Dissolve
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE();
    }
    pass MonoColor
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MONOCOLOR();
    }
    pass RimEmission
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RIM_EMISSION();
    }
    pass Rim
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RIM();
    }
    pass ColorOnly
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLORONLY();
    }
    pass Disintegration
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DISINTEGRATE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLORONLY();
    }
    pass Outline
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_CULL_CW);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_OUTLINE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE();
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
    pass Outlined
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
    pass OutlineColor
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_CULL_CW);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_OUTLINE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE_COLOR();
    }
    pass Shadow
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }
    pass NormalMap
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL_MAP();
    }
    pass DistortionBoost
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION_BOOST();
    }
    pass OutlineCCW
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_OUTLINE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE();
    }
    pass EmissionColor
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_EMISSION_COLOR();
    }

    //pass Default_1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    GeometryShader = NULL;
    //    HullShader = NULL;
    //    DomainShader = NULL;
    //    PixelShader = compile ps_5_0 PS_MAIN();
    //}
    pass Frame
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FRAME();
    }
    pass EmissionCW
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_EMISSION();
    }
    pass MimosaWing
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MIM_WING();
    }

    pass DEFFEREDOUTLINE
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DEFFEREDOUTLINE();
    }
    pass MonoColorRim
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_OUTLINE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MONOCOLOR();
    }
}
