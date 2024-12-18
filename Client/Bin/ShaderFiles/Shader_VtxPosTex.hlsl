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
float2 g_vResolution;
float g_fNoiseStrength, g_Dissolve, g_DissolveRange = 0.1f;
float g_fMagicNumber;
texture2D g_Noise_Texture;


//jeongrae
bool g_bCommandIconRed = false;
float g_StoreBuyBarPercent = 0;
float g_StoreBarScrollUpPercent = 0;
float g_StoreBarScrollDownPercent = 0;
float g_RekkaEffectStartPos = 0;
texture2D g_BossBlurTexture;
texture2D g_RekkaEffect;
bool g_BossDamaged = false;
float g_BlurStrong = 0;
bool g_RekkaBuff = false;

texture2D g_FaceBackTexture;

float g_fKorsicaHpPercent;
texture2D g_KorsicaHpTexture;
bool g_bKorsicaDamaged = false;
bool g_bKorsica = false;

float g_TreasurePercent;
bool g_TreasureGlow;

//BeomSeok
float g_fPlayerHpPercent;
float g_fPlayerReverbPercent;
float g_fPlayerPartnerPercent;
texture2D g_PlayerTexture;
bool g_bPlayerDecreased = false;



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

	return Out;		
}

VS_OUT VS_FULLSCREEN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    In.vPosition.xy = sign(In.vPosition);
    Out.vPosition = float4(In.vPosition.xy, 1.f, 1.f);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

VS_OUT VS_TRAIL(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    vector vPosition = mul(float4(In.vPosition, 1.f), matVP);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct VS_OUT_NORM
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vNormal : NORMAL;
};

VS_OUT_NORM VS_NORM(VS_IN In)
{
	VS_OUT_NORM Out = (VS_OUT_NORM) 0;

	matrix matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	
	vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(vector(0.f, 0.f, -1.f, 0.f), g_WorldMatrix));

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

struct PS_OUT_EMISSION
{
    float4      vEmission : SV_TARGET5;
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
    
    float fAngle = atan2(vUV.x, -vUV.y);
    if(fAngle > .9f * 3.14f)
    {
       // fAngle = .9f * 3.14f - 1.f * fAngle;
    }
    else if (fAngle > g_fProgress - 3.14f * 1.1f)
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

PS_OUT_EMISSION PS_ADD_EMISSION(PS_IN In)
{
    PS_OUT_EMISSION Out = (PS_OUT_EMISSION) 0;
    Out.vEmission = g_Texture.Sample(PointSampler, In.vTexcoord) * g_Color;
    if (length(Out.vEmission.rgb) <= 0.15f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_DECAL_TO_DIFFUSE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) * 0.33f;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a < 0.05f)
    {
        discard;
    }
    //Out.vID = g_iObjectID;

    return Out;
}

PS_OUT PS_WEIGHT_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    Out.vColor.rgb = Out.vColor.rgb * Out.vColor.a;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 vUV = float2(In.vPosition.x / g_vResolution.x * 0.5f + 0.5f, -In.vPosition.y / g_vResolution.y * 0.5f + 0.5f);

    vector vNoise = g_Noise_Texture.Sample(LinearSampler, float2(vUV.x, vUV.y + sin(g_fMagicNumber)));
    float fNoise = vNoise.x;
    vUV += fNoise * g_fNoiseStrength;
    
    Out.vColor = g_Texture.Sample(LinearSampler, vUV);
    Out.vColor.a = 1.f;
    
    //if (Out.vColor.a < 0.05f)
    //{
    //    discard;
    //}
    return Out;
}


texture2D g_PositionTexture;
float4 SonarOrigin;

float Time;
float SonarSpeed;
float SonarInterval;
float SonarThickness;

PS_OUT PS_SONAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vPos = g_PositionTexture.Sample(LinearSampler, In.vTexcoord);
    //vPos = mul(vPos, g_ViewMatrix);
    float distance = length(vPos.xyz - SonarOrigin.xyz);
    //float ip;
    //float distance = modf(Time * SonarSpeed / SonarInterval, ip);
    float innerRange = Time * SonarSpeed;
    float outterRange = innerRange + SonarThickness;
    
    float intensity = sin(saturate((distance - innerRange) / SonarThickness) * PI);
    
    //float d = -vPos.z / vPos.w;

    Out.vColor = g_Color * saturate(intensity);

    
 //   clamp(
	//	float((distance - SonarThickness < d) && (d < distance)) * ((d - (distance - SonarThickness)) / SonarThickness)
	//, 0.0
	//, 1.0
	//);
    if (Out.vColor.a < 0.05f)
    {
        discard;
    }
    return Out;
}
float4 BlockOrigin;
float BlockSize;

PS_OUT PS_BLOCKLIGHT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vPos = g_PositionTexture.Sample(LinearSampler, In.vTexcoord);


    
    float2 vDist = vPos.xz - float2(-10000.f, -10000.f) * BlockSize - BlockOrigin.xz;
    float ipX, ipZ;
    float fX = modf(vDist.x / BlockSize, ipX);
    float fZ = modf(vDist.y / BlockSize, ipZ);
    
    vector vColor = g_Texture.Sample(LinearSampler, float2(fX, fZ));
    vector vNoise = g_Noise_Texture.Sample(LinearSampler, float2(fX, fZ));
    uint iipX = abs(ipX);
    iipX = iipX % 2;
    uint iipZ = abs(ipZ);
    iipZ = iipZ % 2;
    //float ip;
    //float distance = modf(Time * SonarSpeed / SonarInterval, ip);

    //float d = -vPos.z / vPos.w;

    Out.vColor = g_Color * vColor.x * (sign(iipX) == sign(iipZ) || sign(iipX) == -sign(iipZ));

 //   clamp(
	//	float((distance - SonarThickness < d) && (d < distance)) * ((d - (distance - SonarThickness)) / SonarThickness)
	//, 0.0
	//, 1.0
	//);
    float fNoise = vNoise.x;
    if (fNoise < g_Dissolve)
    {
        float fDist = g_Dissolve - fNoise;
        if (fDist < g_DissolveRange)
        {
            if (fDist < g_DissolveRange)
            {
                if (Out.vColor.w > 0.05f)
                    Out.vColor = lerp(vector(1.f, 1.f, 1.f, 1.0f), Out.vColor, fDist / g_DissolveRange);
                else
                    discard;
            }
            else
            {
                discard;
            }
        }
        else
        {
            discard;
        }
    }
    if (Out.vColor.a < 0.05f)
    {
        discard;
    }
    return Out;
}

PS_OUT PS_BLOCKLIGHT_EVEN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vPos = g_PositionTexture.Sample(LinearSampler, In.vTexcoord);

    
    float2 vDist = vPos.xz - float2(-10000.f, -10000.f) * BlockSize - BlockOrigin.xz;
    float ipX, ipZ;
    float fX = modf(vDist.x / BlockSize, ipX);
    float fZ = modf(vDist.y / BlockSize, ipZ);
    
    vector vColor = g_Texture.Sample(LinearSampler, float2(fX, fZ));
    vector vNoise = g_Noise_Texture.Sample(LinearSampler, float2(fX, fZ));
    
    uint iipX = abs(ipX);
    iipX = iipX % 2;
    uint iipZ = abs(ipZ);
    iipZ = iipZ % 2;
    //float ip;
    //float distance = modf(Time * SonarSpeed / SonarInterval, ip);

    //float d = -vPos.z / vPos.w;

    Out.vColor = g_Color * vColor.x * (sign(iipX) != sign(iipZ) && sign(iipX) != -sign(iipZ));

 //   clamp(
	//	float((distance - SonarThickness < d) && (d < distance)) * ((d - (distance - SonarThickness)) / SonarThickness)
	//, 0.0
	//, 1.0
	//);
    float fNoise = vNoise.x;
    if (fNoise < g_Dissolve)
    {
        float fDist = g_Dissolve - fNoise;
        if (fDist < g_DissolveRange)
        {
            if (Out.vColor.w > 0.05f)
                Out.vColor = lerp(vector(1.f, 1.f, 1.f, 1.0f), Out.vColor, fDist / g_DissolveRange);
            else
                discard;
        }
        else
        {
            discard;
        }
    }
    if (Out.vColor.a < 0.05f)
    {
        discard;
    }
    return Out;
}

PS_OUT PS_STOREBAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x > g_StoreBuyBarPercent)
        discard;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_STORESCROLLBAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    if (g_StoreBarScrollDownPercent < In.vTexcoord.y)
        discard;
    
    if (In.vTexcoord.y < g_StoreBarScrollUpPercent)
        discard;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_CommandIconCircle(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float Distance = sqrt(pow(In.vTexcoord.x - 0.5f, 2) + pow(In.vTexcoord.y - 0.5f, 2));
    if (g_bCommandIconRed)
    {
        if (Distance > 0.4f)
            discard;
    }
    else
    {
        if (Distance > 0.44f)
            discard;
    }
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
	
    return Out;
}

PS_OUT PS_BossHpBar(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x < g_fProgress)
        discard;
    
    if (g_BossDamaged)
    {
        Out.vColor = (g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color);
        Out.vColor += g_BossBlurTexture.Sample(LinearSampler, In.vTexcoord) * g_BlurStrong;
    }
    else
    {
        Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    }
   
    
    if (Out.vColor.a <= 0.2f)
    {
        discard;
    }
    
    return Out;
}

PS_OUT PS_RekkaDeco(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (g_RekkaBuff)
    {
        Out.vColor = g_BossBlurTexture.Sample(LinearSampler, In.vTexcoord);
        float brightness = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.0;
        if (brightness < 0.5)
        {
            discard;
        }
        else
        {
            Out.vColor *= 1.15f;
        }
    }
    else
    {
        Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    }

   
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
    
    return Out;
}

PS_OUT PS_RekkaDecoEffect(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if ((g_RekkaEffectStartPos < In.vTexcoord.x && In.vTexcoord.x < g_RekkaEffectStartPos + 0.15f))
    {
        if (In.vTexcoord.x > 0.5f)
        {
            discard;
        }
        float brightness = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.0;
        if (brightness < 0.5f)
        {
            discard;
        }
        else
        {
            Out.vColor *= 1.25f;
        }
    }
    else if (In.vTexcoord.x < 1 - g_RekkaEffectStartPos && 1 - g_RekkaEffectStartPos - 0.15f < In.vTexcoord.x)
    {
        if (In.vTexcoord.x < 0.5f)
        {
            discard;
        }
        float brightness = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.0;
        if (brightness < 0.5f)
        {
            discard;
        }
        else
        {
            Out.vColor *= 1.25f;
        }
       
    }
    else
    {
        discard;
    }
   
    if (Out.vColor.a <= 0.1f)
    {
        discard;
    }
    
    return Out;
}

PS_OUT PS_KorsicaFace(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    float4 Color = g_FaceBackTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (Color.a < 0.9f)
        discard;
    
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
    
    return Out;
}
PS_OUT PS_KorsicaHpBar(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    if(!g_bKorsica)
    {
        if (g_fKorsicaHpPercent * 0.83f + 0.2f < In.vTexcoord.x + In.vTexcoord.y * 0.2f)
        {
            discard;
        }
        else
        {
            
        }
        
    }
    else
    {
        if (In.vTexcoord.x - In.vTexcoord.y * 0.2f < 0.82f - g_fKorsicaHpPercent * 0.85f)
        {
            discard;
        }
        else
        {
            if (g_bKorsicaDamaged)
            {
                Out.vColor = (g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color) * 0.2f;
                Out.vColor += g_KorsicaHpTexture.Sample(LinearSampler, In.vTexcoord) * 0.8f;           
            }
        }
    }

    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
    
    return Out;
}
PS_OUT PS_TreasureBox(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    if (g_TreasurePercent < In.vTexcoord.x)
    {       
        discard;
    }
    
    if (g_TreasureGlow)
    {
        Out.vColor *= 1.2f;
    }
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }
    
    return Out;
}
PS_OUT PS_PlayerBarHp(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    
    if (g_fPlayerHpPercent <= 0.0f)
    {
        discard; 
    }

    if (In.vTexcoord.y - 0.5f > -1 * (In.vTexcoord.x - g_fPlayerHpPercent))
    {
        discard;
    }
    else
    {
        if (g_bPlayerDecreased)
        {
            Out.vColor = (g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color) * 0.2f;
            Out.vColor += g_PlayerTexture.Sample(LinearSampler, In.vTexcoord) * 0.8f;
        }
    }

    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }

    return Out;
}
PS_OUT PS_PlayerBarReverb(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;

    if (g_fPlayerReverbPercent <= 0.0f)
    {
        discard;
    }

    float reverbPercentAdjusted = 0.2f + g_fPlayerReverbPercent * 0.7f;

    if (In.vTexcoord.y - 0.5f > -1 * (In.vTexcoord.x - reverbPercentAdjusted))
    {
        discard;
    }

    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }

    return Out;
}
PS_OUT PS_PlayerBarPartner(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord) * g_Color;

    float fClippedTexcoordY = (In.vTexcoord.y - 0.3f) / 0.37f;

    if (In.vTexcoord.y < 0.28f || In.vTexcoord.y > 0.65f)
    {
        discard;
    }

    float fInverseTexcoordX = 1.0f - In.vTexcoord.x;
    float fInverseTexcoordY = 1.0f - fClippedTexcoordY;

    float fOffsetY = - 0.4f;
    fInverseTexcoordY += fOffsetY;

    if (fInverseTexcoordY > (g_fPlayerPartnerPercent - (fInverseTexcoordX * 0.47)))
    {
        discard;
    }

    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }

    return Out;
}

struct PS_IN_NORM
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vNormal : NORMAL;
};

struct PS_OUT_NORM
{
	float4 vColor : SV_TARGET0;
	uint vID : SV_TARGET1;
	float4 vPosition : SV_TARGET2;
	float4 vNormal : SV_TARGET3;
};

PS_OUT_NORM PS_NORM(PS_IN_NORM In)
{
	PS_OUT_NORM Out = (PS_OUT_NORM) 0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
	if (Out.vColor.a <= 0.1f)
	{
		discard;
	}

	Out.vID = g_iObjectID;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vPosition = In.vWorldPos;
    
	return Out;
}
PS_OUT PS_SuccessLine(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.3f)
        discard;
    
    Out.vColor *= 1.2f;
    //float brightness = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.0;
    //if (brightness < 0.5f)
    //{
    //    discard;
    //}
    //else if (Out.vColor.b < 0.8f)
    //{
    //    discard;
    //}
    
    //Out.vColor *= 0.9f;
    
    return Out;
}
PS_OUT PS_TramHpBar(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (g_fProgress <= 0.0f)
    {
        discard;
    }
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    
    if (g_fProgress + 0.1f < In.vTexcoord.x + In.vTexcoord.y * 0.25f)
    {
        discard;
    }
    
    if (Out.vColor.a <= 0.1f)
    {
        discard;
    }

    return Out;
}

PS_OUT PS_StunGaugeBar(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 center = float2(0.25, 0.9);
    float2 dir = In.vTexcoord - center;
    float angle = atan2(dir.y, dir.x);
    float normalizedAngle = (angle + 3.1415) / (2.0 * 3.1415);

    if (g_fProgress < normalizedAngle)
    {
        discard;
    }
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    
    if (Out.vColor.a <= 0.05f)
    {
        discard;
    }

    return Out;
}

PS_OUT PS_KaleHpBar(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x < g_fProgress)
    {
        discard;
    }
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * g_Color;
    
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
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

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
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

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
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);


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
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);


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
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FROM_LEFT();
    }

    pass Emission
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ADD_EMISSION();
    }

    pass Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_TRAIL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WEIGHT_BLEND();
    }
    pass TrailDecal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_TRAIL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DECAL_TO_DIFFUSE();
    }
    pass TrailDistortion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NoWriteZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_TRAIL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }
    pass Sonar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NonZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SONAR();
    }
    pass BlockLight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NonZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOCKLIGHT();
    }
    pass BlockLightEven
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_NonZ, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_FULLSCREEN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOCKLIGHT_EVEN();
    }
    pass StoreBuyBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_STOREBAR();
    }
    pass StoreScrollBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_STORESCROLLBAR();
    }
    pass CommandIconCircle
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_CommandIconCircle();
    }
    pass BossHpBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BossHpBar();
    }
    pass RekkaDeco
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RekkaDeco();
    }
    pass RekkaDecoEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RekkaDecoEffect();
    }
    pass KorsicaFace
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_KorsicaFace();
    }
    pass KorsicaHpBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_KorsicaHpBar();
    }
    pass TreasureBox
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TreasureBox();
    }
    pass PlayerBarHp
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PlayerBarHp();
    }
    pass PlayerBarReverb
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PlayerBarReverb();
    }
    pass PlayerBarPartner
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PlayerBarPartner();
    }
	pass PosTexNorm
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_NORM();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_NORM();
	}

    pass SuccessLine
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SuccessLine();
    }
    pass TramHpBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_TramHpBar();
    }
    pass StunGaugeBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_StunGaugeBar();
    }
    pass KaleHpBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_KaleHpBar();
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
