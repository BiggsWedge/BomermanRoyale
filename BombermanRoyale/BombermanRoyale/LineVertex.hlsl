cbuffer ConstantBuffer : register(b0)
{
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProjection;
};

struct ShaderVertex {
	float3 xyzw : POSITION;
	float4 rgba : COLOR;
};

struct OutputVertex {
	float4 xyzw : SV_POSITION; //system value sv
	float4 rgba : COLOR;
};

OutputVertex main(ShaderVertex input) {
	OutputVertex output = (OutputVertex)0;
	output.xyzw = mul(float4(input.xyzw, 1), mWorld);
	output.xyzw = mul(output.xyzw, mView);
	output.xyzw = mul(output.xyzw, mProjection);
	output.rgba = input.rgba;

	return output;
}