
#include "Client_Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
texture2D		g_DissolveTexture;

float			g_DissolveTimer;
float			g_DissolveLifespan;
float			g_AlphaVolume;
float			g_AlphaMaxVolume;
int				g_ColorType;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

/* DrawIndexed�Լ��� ȣ���ϸ�. */
/* �ε������ۿ� ����ִ� �ε�����°�� ������ VS_MAIN�Լ��� ���ڷ� ������. VS_IN�� ����ȴ�. */
/* �Ϲ������� TriangleList�� �׸����, ���� ������ ���� VS_MAIN�Լ��� ���ڷ� ������. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));

	/* ������ ��ġ�� ���� �� ��������� ���Ѵ�. ���� ������ ViewSpace�� �����ϳ�. */
	/* ������ı��� ���ϸ� ������ġ�� w�� �佺���̽� ���� z�� �����Ѵ�. == Out.vPosition�� �ݵ�� float4�̾���ϴ� ����. */
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = vNormal;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

/* �̷��� ������� �ȼ��� PS_MAIN�Լ��� ���ڷ� ������. */
/* �����ϴ� ���� Target0 == ��ġ�� 0��°�� ���ε��Ǿ��ִ� ����Ÿ��(�Ϲ������� �����)�� �׸���. */
/* �׷��� ����ۿ� ���� �׷�����. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);	
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);

	if (Out.vDiffuse.a <= 0.3f)
		discard;

	return Out;
}


PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);

	/* Dissolve with Highlight */
	float dissolveFactor = lerp(0, 1, g_DissolveTimer / g_DissolveLifespan); /* If dissolveFactor:
																			 == 0:   Should not Dissolve
																			 == 1:   Should Dissolve Everything. */
	float4 dissolveColor = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);
	dissolveColor.a = dissolveColor.y;
	dissolveColor.yz = dissolveColor.x;

	float dissolveValue = dissolveColor.r - dissolveFactor; /* If dissolveValue:
															> .15:      No Dissolve
															0 ~ .15f:   Highlight
															<= 0:      Dissolve. */

	if (dissolveValue <= 0)
		discard;
	else if (dissolveValue < .15f)
	{
		float3 colorYellow = float3(1.f, .95f, .6f);
		float3 colorOrange = float3(.92f, .36f, .2f);

		float3 lerpColor = lerp(colorOrange, colorYellow, dissolveValue / .15f);
		Out.vDiffuse.rgb = lerpColor;
	}

	return Out;
}

PS_OUT PS_MAIN_TEST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	//Out.vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);


	/*Out.vDiffuse.rgb = float3(1.f, .95f, .6f);
	Out.vDiffuse.a = 1.f;*/
	if (Out.vDiffuse.r < 0.5f)
		discard;
	else
	{
		float3 colorYellow = float3(1.f, .95f, .6f);
		float3 colorOrange = float3(.92f, .36f, .2f);

		float3 lerpColor = lerp(colorYellow, colorOrange, Out.vDiffuse.r / 1.f);
		//�ð��� �޾Ƽ� ���� ���İ� 0���� �ǵ��� �����.
		Out.vDiffuse.rgb = lerpColor;
	}

	return Out;
}

PS_OUT PS_MAIN_COLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);

	if (Out.vDiffuse.a <= 0.3f)
		discard;
	else
	{
		if (g_ColorType == 0)
		{//�����������
			float3 colorYellow = float3(1.f, .95f, .6f);
			Out.vDiffuse.rgb = colorYellow;
		}
		else if (g_ColorType == 1)
		{//���� ��ź��
			float3 colorPurple = float3(0.8f, 0.4f, 1.f);
			Out.vDiffuse.rgb = colorPurple;
			Out.vDiffuse.a = 0.5f;
		}
		else if (g_ColorType == 2)
		{// ���� Į��
			float3 colorRed = float3(.8f, .2f, .2f);
			Out.vDiffuse.rgb = colorRed;
			Out.vDiffuse.a = 0.7f;
		}
	}
	return Out;
}

PS_OUT PS_MAIN_HIT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);

	if (Out.vDiffuse.a <= 0.3f)
		discard;
	else
	{

		float3 colorRed = float3(1.f, 0.f, .05f);
		Out.vDiffuse.rgb = colorRed;
	
	}
	return Out;
}

technique11 DefaultTechnique
{
	pass Default // 0
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Dissolve //1
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
	}

	pass ForAttackEffect //2
	{
		SetRasterizerState(RS_CullNone);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_TEST();
	}

	pass ForColorEffect //3
	{
		SetRasterizerState(RS_CullNone);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLOR();
	}

	pass ForHitEffect //4
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HIT();
	}
}