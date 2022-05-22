/*
	Basic shader including ambient lighting
*/
struct VOut 
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 light_dir : TEXCOORD1;
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
	output.normal = normalize(norm); //normalize(mul(float4(norm, 0), worldm).xyz);
	output.light_dir = float3(-0.1, 0.9, -0.1);
	//output.pos = position;

	output.tex = tex;
	
	return output;
}

Texture2D shaderTexture;
SamplerState sampleType;

float4 PShader(float4 position : SV_POSITION, float2 tex : TEXCOORD0, float3 normal : NORMAL, float3 light_dir : TEXCOORD1) : SV_TARGET
{
	float3 norm = normalize(normal);
	float3 ldir = normalize(light_dir);
	float n_dot_l = dot(light_dir, normal);

	float4 color = shaderTexture.Sample(sampleType, tex);
	float3 ambient = ambientcol.rgb * ambientcol.a * color.rgb;
	float3 diffuse = (float3) 0;
	float4 light_col = float4(0.2, 0.5, 0.9, 1.0);
	if (n_dot_l > 0) {
		diffuse = light_col.rgb * light_col.a * n_dot_l * color.rgb;
	}

	float4 outcol = (float4)0;
	outcol.rgb = ambient + diffuse;
	//outcol.rgb = float3(norm.x, norm.y, norm.z);
	outcol.a = color.a;
	return outcol;


}