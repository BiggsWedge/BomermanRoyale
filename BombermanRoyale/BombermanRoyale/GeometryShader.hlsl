cbuffer ConstantBuffer : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProjection;
};

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
    float4 rgba : COLOR;
};

[maxvertexcount(2)]
void main(
	line float4 input[2] : SV_POSITION, 
	inout LineStream<OutputVertex> output) {
    for (uint i = 0; i < 2; i++)
	{
        OutputVertex element;
        element.xyzw = mul(input[i].xyzw, mWorld);
        element.xyzw = mul(element.xyzw, mView);
        element.xyzw = mul(element.xyzw, mProjection);
        element.rgba = input[i].rgba;
		output.Append(element);
	}
}