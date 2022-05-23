/*
	Basic shader including ambient lighting
*/
struct VOut 
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

cbuffer MatrixBuffer {
	matrix worldm;
	matrix viewm;
	matrix projm;
};

cbuffer LightBuffer {
	float4 ambientcol;
};

VOut VShader(float4 position : POSITION, float2 tex : TEXCOORD0, float3 norm : NORMAL) {

	VOut output;
	output.pos = mul(position, worldm);
	output.pos = mul(output.pos, viewm);
	output.pos = mul(output.pos, projm);
	output.tex = tex;
	
	return output;
}

Texture2D shaderTexture;
SamplerState sampleType;

float4 PShader(float4 position : SV_POSITION, float2 tex : TEXCOORD0 ) : SV_TARGET
{

	float4 color = shaderTexture.Sample(sampleType, tex);
	float3 ambient = ambientcol.rgb * ambientcol.a * color.rgb;
	float4 outcol = (float4)0;
	outcol.rgb = ambient;
  	outcol.a = color.a;
   	return outcol;

}