#include "Shader_Defines.hlsli"
/* VertexShader */
/* 1. 정점의 기본적인 변환(월드, 뷰, 투영)을 수행한다. */


/* 2. 정점의 구성을 변경한다. */
// vector == float4 float3 float2 float
// matrix == float4x4 

/* 상수테이블. == 전역변수 */
float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix, g_ViewInv;
Texture2D       g_Texture, g_Normal_Texture, g_ATOS_Texture;
Texture2D       g_Emission_Texture, g_Noise_Texture, g_PerlinNoise_Texture, g_Mask_Texture;
TextureCube     g_CubeMap;

int g_RandomValues[16];

float g_fMagicNumber, g_Dissolve, g_DissolveRange = 0.1f, g_OutlineWidth;
float g_fTime;
float4 g_vColor;
float4 g_vAmbientColor;
float g_fBeatRate;

float2 g_vUVFlow;
float2 g_vResolution;
float g_fNoiseStrength;

float g_fzoomFactor = 0.f;

float4 g_vCampos;
int2 g_iDivide = { 4, 4 };
uint g_iFrameIndex;

float4 g_vEffectBaseColor;

float4 RayMarch(float3 startPos, float3 dir, float stepSize, int maxSteps)
{
    float3 currPos = startPos;
    currPos += dir * stepSize;
    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
    [unroll]
    for (int i = 0; i < maxSteps; ++i)
    {
        // 클립 공간으로 변환
        float4 clipPos = mul(float4(currPos, 1.0f), g_ProjMatrix);
        float2 texCoord = clipPos.xy / clipPos.w * 0.5f + 0.5f;
        //float2 texCoord = float2(clipPos.x / g_vResolution.x, clipPos.y / g_vResolution.y);;
        
        texCoord.y = 1.f - texCoord.y;
        texCoord.x = 1.f - texCoord.x;
        // 텍스처 좌표가 화면 밖을 벗어나지 않도록 제한
        if (texCoord.x < 0 || texCoord.x > 1 || texCoord.y < 0 || texCoord.y > 1)
            break;

        // G-Buffer에서 깊이 값을 가져옴
        float4 scenePos = g_ATOS_Texture.Sample(LinearSampler, texCoord);
        scenePos = mul(scenePos, matVP);
        
        // 반사 광선이 장면에 맞았는지 확인
        if (length(scenePos.xyz - currPos) < stepSize)
        {
            // 반사된 색상 반환
            return float4(g_Texture.Sample(LinearSampler, texCoord).rgb * .2f, 0.5f);
        }

        // 반사 광선의 현재 위치를 갱신
        currPos += dir * stepSize;
    }

    return float4(0, 0, 0, 0.5); // 실패하면 검정색 반환
}
float4 RayMarch_Screen(float3 startPos, float3 dir, float stepSize, int maxSteps)
{
    float3 currPos = startPos;
    currPos += dir * stepSize;
    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
    [unroll]
    for (int i = 0; i < maxSteps; ++i)
    {
        // 클립 공간으로 변환
        float4 clipPos = mul(float4(currPos, 1.0f), g_ProjMatrix);
        float2 texCoord = clipPos.xy / clipPos.w * 0.5f + 0.5f;
        //float2 texCoord = float2(clipPos.x / g_vResolution.x, clipPos.y / g_vResolution.y);;
        // 텍스처 좌표가 화면 밖을 벗어나지 않도록 제한
        if (texCoord.x < 0 || texCoord.x > 1 || texCoord.y < 0 || texCoord.y > 1)
            break;

        // G-Buffer에서 깊이 값을 가져옴
        float4 scenePos = g_ATOS_Texture.Sample(LinearSampler, texCoord);
        scenePos = mul(scenePos, matVP);
        
        // 반사 광선이 장면에 맞았는지 확인
        if (length(scenePos.xyz - currPos) < stepSize)
        {
            // 반사된 색상 반환
            return float4(g_Texture.Sample(LinearSampler, texCoord).rgb * .2f, 0.5f);
        }

        // 반사 광선의 현재 위치를 갱신
        currPos += dir * stepSize;
    }

    return float4(0.2, 0.2, 0.2, 0.5); // 실패하면 검정색 반환
}

struct VS_IN
{
    float3		vPosition : POSITION;
    float3		vNormal   : NORMAL;
	float2		vTexcoord : TEXCOORD;
    float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
    float4      vWorldPos : TEXCOORD1;
    float4      vNormal : TEXCOORD2;
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
	Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    //Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
    //Out.vBiTangent = vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f);
    
	return Out;		
}
VS_OUT VS_REF(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    vector vPosition = mul(float4(In.vPosition + In.vNormal * g_OutlineWidth, 1.f), matWVP);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matWV));
    return Out;
}
VS_OUT VS_OUTLINE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    vector vPosition = mul(float4(In.vPosition + In.vNormal * g_OutlineWidth, 1.f), matWVP);
    

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    return Out;
}
VS_OUT VS_MIRROR_X(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    matrix matW = mul(matrix(-1.f, 0.f, 0.f, 0.f,
                        0.f, 1.f, 0.f, 0.f,
                        0.f, 0.f, 1.f, 0.f,
                        0.f, 0.f, 0.f, 1.f), g_WorldMatrix);
    matWV = mul(matW, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), matW);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matW));
    return Out;
}

VS_OUT VS_WORLD_TEXCOORD(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vPosition = vPosition;
    Out.vTexcoord = mul(In.vTexcoord, g_WorldMatrix);
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    return Out;
}

VS_OUT VS_DISTORT(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    //vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vNormal =   mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vPosition = mul(float4(In.vPosition, 1.f) + vector(In.vNormal, 0.f) * sin(g_fTime + In.vTexcoord.y * PI * 2.f) * 10.f, matWVP);
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
    Out.vPosition = mul(float4(In.vPosition, 1.f) + vector(In.vNormal, 0.f) * sin(g_fTime + In.vTexcoord.x * PI * 2.f) * 10.f, matWVP);
    return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
    float4      vWorldPos : TEXCOORD1;
    float4      vNormal : TEXCOORD2;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
    uint        vID : SV_TARGET1;
    float4      vPosition : SV_TARGET2;
    float4      vNormal : SV_TARGET3;
   // float4      vORM    : SV_TARGET4;
    float4      vEmission    : SV_TARGET4;
};
struct PS_OUT2
{
    float4 vColor : SV_TARGET0;
    //uint vID : SV_TARGET1;
};
//float4 PS_MAIN(PS_IN In) : COLOR0
//{
//
//}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	if(Out.vColor.a < 0.1f)
    {
        discard;
    }
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;

    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    return Out;
}

PS_OUT2 PS_SWEEP(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.r;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
    //Out.vID = g_iObjectID;
  
    return Out;
}

PS_OUT2 PS_DISTORTION(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);
  
    vector vNoise = g_Noise_Texture.Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y +sin(g_fTime)));
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
    Out.vEmission = g_Emission_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    return Out;
}

PS_OUT PS_EMISSION_MOVING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

// 확대 비율을 설정합니다 (예: 2배 확대)
    float zoomLevel = 1.5f;

// 이동 방향과 정도를 설정하는 변수입니다. zoomFactor는 0에서 1 사이 값을 가집니다.
    float2 panOffset = float2(g_fzoomFactor*0.1f, g_fzoomFactor * 0.05f); // 가로와 세로로 비율을 다르게 줄 수도 있음

// 확대 중심점을 기준으로 텍스쳐 좌표를 보정하고 이동합니다.
    float2 zoomCenter = float2(0.5f, 0.5f); // 확대의 중심점
    float2 zoomTexCoord = (In.vTexcoord - zoomCenter) / zoomLevel + zoomCenter + panOffset;

// 조정된 텍스쳐 좌표로 텍스쳐 샘플링
    Out.vColor = g_Texture.Sample(LinearSampler, zoomTexCoord);

    if (Out.vColor.a < 0.1f)
    {
        discard;
    }

    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vEmission = g_Emission_Texture.Sample(LinearSampler, zoomTexCoord);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    return Out;
}


PS_OUT PS_CONVEYORBELT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord + g_vUVFlow);

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
PS_OUT PS_OUTLINE(PS_IN In)
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
float4 PS_SKY(PS_IN In) : SV_TARGET0
{
    float4 Out = (float4) 0;
    
    Out = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if(Out.a <= 0.1f)
        discard;
    return Out;
}

#define THRESHOLD_1 0.7f
#define THRESHOLD_2 0.43f
#define WATERCOLOR_1 float3(0.417f, 1.f, 1.f)
#define WATERCOLOR_2 float3(0.f, 0.68f, 0.74f)
#define WATERCOLOR_3 float3(0.f, 0.38f, 0.58f)

PS_OUT PS_WATER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
   // float fNormStrength = clamp(length(normalize(In.vNormal).xz), 0.1f, 1.f);
    float2 UV = In.vTexcoord + float2(0.1f, -1.3f) * g_fTime;// * fNormStrength;
    float2 UV2 = In.vTexcoord + float2(0.3f, -0.7f) * g_fTime;// * fNormStrength;
    

    UV *= float2(2.f, 1.f);
    UV2 *= float2(1.f, 0.2f);
    float fNoise = g_Texture.Sample(LinearSampler, UV).r * 0.7f + g_Texture.Sample(LinearSampler, UV2).r * 0.3f;

    if (fNoise < THRESHOLD_2)
    {
        Out.vColor.rgb = lerp(WATERCOLOR_1, WATERCOLOR_2, fNoise / THRESHOLD_2);
    }
    else if (fNoise < THRESHOLD_1)
    {
        Out.vColor.rgb = lerp(WATERCOLOR_2, WATERCOLOR_3, (fNoise - THRESHOLD_1) / (THRESHOLD_2 - THRESHOLD_1));
    }
    else
    {
        Out.vColor.rgb = WATERCOLOR_3;
    }

    Out.vColor.a = 1.f;
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;

    return Out;
}

PS_OUT PS_Frame(PS_IN In)
{
	PS_OUT Out = (PS_OUT) 0;
    
	int iX = g_iFrameIndex % g_iDivide.x;
	int iY = g_iFrameIndex / g_iDivide.x;
    
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
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
	return Out;
}

PS_OUT PS_BELTEMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord + g_vUVFlow);

    if (Out.vColor.a < 0.1f)
    {
        discard;
    }
	Out.vEmission = g_Emission_Texture.Sample(LinearSampler, In.vTexcoord + g_vUVFlow);
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    return Out;
}

PS_OUT PS_DIFFUSECOLOR(PS_IN In)
{
	PS_OUT Out = (PS_OUT) 0;
	float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
	if (vColor.a < 0.1f)
	{
		discard;
	}
    
	Out.vColor = vColor * g_vColor;
	Out.vID = g_iObjectID;
	Out.vPosition = In.vWorldPos;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	return Out;
}

PS_OUT PS_VISUALIZER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 텍스처 샘플링
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    // 텍스처 좌표를 7x16 그리드로 분할하여 셀 인덱스 계산
    float cellWidth = 1.0 / 16.0;
    float cellHeight = (1.0 - 0.86) / 7.0; // 7단계로 나누기 위한 간격
    
    int x = (int) (In.vTexcoord.x / cellWidth);
    int y = (int) (In.vTexcoord.y / cellHeight);

    // g_RandomValues에서 현재 x 위치에 대한 최대 y 값을 가져오기
    int maxLevel = g_RandomValues[x];
    
    // g_fBeatRate를 0.02 단위로 분할하여 임계값을 캡슐화
    float thresholdLevel = 1.0 - (maxLevel - 1) * 0.02;
    float adjustedBeatRate = thresholdLevel * g_fBeatRate + (1.0 - g_fBeatRate);

    // 조건에 맞는 셀 활성화 여부 결정
    float brightness = (In.vTexcoord.y > adjustedBeatRate) ? 1.0f : 0.0f;
    
    // 알파가 낮으면 픽셀 버림
    if (vColor.a < 0.1f)
    {
        discard;
    }


    if (vColor.x > 0.2f)
    {
    // 색상 조정 - 활성화된 셀은 강한 색상으로 표시
        if (brightness > 0.0)
        {
            Out.vColor = float4(1.0f, 0.8f, 0.2f, 1.0f) * brightness; // 활성화된 셀의 색상 (노란빛 강조)
        }
        else
        {
            Out.vColor = float4(1.0f, 0.8f, 0.2f, 1.0f) * brightness * 0.f; // 비활성화된 셀은 원래 색상의 반투명
        }
    }
    else
    {
        Out.vColor = vColor;
    }
    
    // 출력 값 설정
    Out.vID = g_iObjectID;
    Out.vPosition = In.vWorldPos;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
    
    return Out;
}

float4 PS_MAIN_LIGHTDEPTH(PS_IN In) : SV_TARGET0
{
    float4 Out = (float4) 0;

    Out = vector(In.vPosition.z / In.vPosition.w, In.vPosition.w / 3000.f, 0.f, 0.f);

    return Out;
}

PS_OUT2 PS_REFLECTION(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float3 normal = normalize(In.vNormal.xyz);
    float4 pos =  In.vWorldPos;
    float3 viewPos = mul(pos, g_ViewMatrix);

    float3 viewDir = normalize(viewPos);
    float3 reflectDir = reflect(viewDir, normal);

    float4 reflectionColor = RayMarch(viewPos, reflectDir, 1.f, 50);
    Out.vColor = reflectionColor;
    return Out;
}

PS_OUT2 PS_REFLECTION_SCREEN(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x, In.vPosition.y / g_vResolution.y);

    float3 normal = normalize(mul(float4(g_Normal_Texture.Sample(LinearSampler, vUV).xyz, 0.f), g_ViewMatrix));
    float4 pos = g_ATOS_Texture.Sample(LinearSampler, vUV);
    float3 viewPos = mul(pos, g_ViewMatrix);

    float3 viewDir = normalize(viewPos);
    float3 reflectDir = reflect(viewDir, normal);

    float4 reflectionColor = RayMarch_Screen(viewPos, reflectDir, 1.f, 50);
    Out.vColor = reflectionColor;
    return Out;
}

PS_OUT2 PS_REFLECTION_CUBE(PS_IN In)
{
    PS_OUT2 Out = (PS_OUT2) 0;
    
    float3 N = normalize(In.vNormal.xyz);
    float3 I = normalize((g_vCampos - In.vPosition).xyz);
   
    float3 R = 2 * dot(I, N) * N - I;

    Out.vColor = g_CubeMap.Sample(LinearSampler, R);
    Out.vColor.a = 0.5f;
    return Out;
}

PS_OUT PS_MASKFLARE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 mask = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = g_vEffectBaseColor * mask.r + g_vEffectBaseColor * mask.g + g_vEffectBaseColor * mask.b;

    Out.vColor.a = mask.a * g_vEffectBaseColor.a;
    Out.vColor.rgb *= Out.vColor.a;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_MASKSHAFT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 mask = g_Texture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = g_vEffectBaseColor * mask.r + g_vEffectBaseColor * mask.g + g_vEffectBaseColor * mask.b;

// Apply alpha adjustment based on the y-coordinate of the UV
    float alphaModifier = 1.0f;
    if (In.vTexcoord.y < 0.5f)
    {
        alphaModifier = smoothstep(0.0f, 0.5f, In.vTexcoord.y);
    }

    Out.vColor.a = mask.a * g_vEffectBaseColor.a * alphaModifier;
    
    Out.vColor.a = Out.vColor.a * 0.5f;
    
    Out.vColor.rgb *= Out.vColor.a;

    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }

    return Out;
}

PS_OUT PS_EmissionFrame(PS_IN In)
{
	PS_OUT Out = (PS_OUT) 0;
    
	int iX = g_iFrameIndex % g_iDivide.x;
	int iY = g_iFrameIndex / g_iDivide.x;
    
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
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vEmission = g_Emission_Texture.Sample(LinearSampler, vLeftTop + vCurCoord);
    
	return Out;
}

PS_OUT PS_EXECUTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    discard;
    
    return Out;
}

PS_OUT PS_GRASS(PS_IN In)
{
	PS_OUT Out = (PS_OUT) 0;
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a < 0.1f)
	{
		discard;
	}
	Out.vID = g_iObjectID;
	Out.vPosition = In.vWorldPos;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.4f);
	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_CULL_None);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader  = NULL;
		HullShader		= NULL;
		DomainShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

    pass MIRROR_X
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MIRROR_X();
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
    pass Belt
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_CONVEYORBELT();
    }
    pass CCW
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass CW
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_CULL_CW);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
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
    pass SKY
    {
        SetRasterizerState(RS_CULL_CW);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SKY();
    }
    pass Water
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WATER();
    }
	pass Frame
	{
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_CULL_CCW);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_Frame();
	}
	pass AlphaEmission
	{
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EMISSION();
	}
	pass BeltEmission
	{
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BELTEMISSIVE();
	}
	pass NoneCulling
	{
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_CULL_None);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass DiffuseColor
	{
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_CULL_None);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DIFFUSECOLOR();
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
    pass Reflection
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_REF();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REFLECTION();
    }
    pass ReflectionScreen
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_REF();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REFLECTION_SCREEN();
    }
    pass ReflectionCube
    {
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_OUTLINE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REFLECTION_CUBE();
    }
    pass DCBlend
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DIFFUSECOLOR();
    }
    pass EmissionMoving
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_EMISSION_MOVING();
    }

    pass ShaftLight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_EffectBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MASKSHAFT();
    }

    pass Visualizer
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_CULL_CCW);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_VISUALIZER();
    }

	pass EmissionFrame
	{
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_CULL_CCW);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_EmissionFrame();
	}

    pass Execution
    {
        SetDepthStencilState(DS_Default, 0);
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_EXECUTION();
    }

	pass Grass
	{
		SetDepthStencilState(DS_Default, 0);
		SetRasterizerState(RS_CULL_None);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_GRASS();
	}

}
//