//#include "../RenderShaderDefines.hlsli"

struct EyeData
{
	matrix view;
	matrix projection;
};

cbuffer ViewData : register(b0)
{
	EyeData eyes[2];
}

cbuffer EyePosition : register(b1) {
	float4 EyePos;
}

struct GSOutput
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL0;
	float4 texCoord : COLOR;
	float4 wpos : WORLDPOS;
	float4 shadowPos : SHADOW;
	float4 eyePos : HEADPOS;
	uint IID : CL_IID;
	uint viewport : SV_ViewportArrayIndex;
};

struct GSInput
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL0;
	float4 texCoord : COLOR;
	float4 wpos : WORLDPOS;
	float4 shadowPos : SHADOW;
	uint IID : CL_IID;
};

[maxvertexcount(6)]
void main(triangle GSInput input[3], inout TriangleStream<GSOutput> TriStream)
{
	GSOutput output;
	[unroll]
	for (uint i = 0; i < 3; ++i)
	{
		output.position = mul(mul(input[i].position, eyes[0].view), eyes[0].projection);
		output.normal = input[i].normal;
		output.texCoord = input[i].texCoord;
		output.wpos = input[i].wpos;
		output.IID = input[i].IID;
		output.viewport = 0;
		output.shadowPos = input[i].shadowPos;
		output.eyePos = EyePos;
		TriStream.Append(output);
	}

	TriStream.RestartStrip();

	[unroll]
	for (uint j = 0; j < 3; ++j)
	{
		output.position = mul(mul(input[j].position, eyes[1].view), eyes[1].projection);
		output.normal = input[j].normal;
		output.texCoord = input[j].texCoord;
		output.wpos = input[j].wpos;
		output.IID = input[j].IID;
		output.viewport = 1;
		output.shadowPos = input[j].shadowPos;
		output.eyePos = EyePos;
		TriStream.Append(output);
	}
	//TriStream.RestartStrip();
}