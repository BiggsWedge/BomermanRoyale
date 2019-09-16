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

[maxvertexcount(4)]
void main(
	line float4 input[2] : SV_POSITION,
	inout TriangleStream<OutputVertex> output)
{
    OutputVertex element0;
    element0.xyzw = input[0].xyzw + float4(-2, 2, -2, 0);
    element0.xyzw = mul(input[0].xyzw, mWorld);
    element0.xyzw = mul(element0.xyzw, mView);
    element0.xyzw = mul(element0.xyzw, mProjection);
    element0.rgba = input[0].rgba;
    output.Append(element0);

    element0.xyzw = input[0].xyzw + float4(2, 2, -2, 0);
    element0.xyzw = mul(input[0].xyzw, mWorld);
    element0.xyzw = mul(element0.xyzw, mView);
    element0.xyzw = mul(element0.xyzw, mProjection);
    element0.rgba = input[0].rgba;
    output.Append(element0);

    element0.xyzw = input[1].xyzw + float4(-2, -2, -2, 0);
    element0.xyzw = mul(input[1].xyzw, mWorld);
    element0.xyzw = mul(element0.xyzw, mView);
    element0.xyzw = mul(element0.xyzw, mProjection);
    element0.rgba = input[1].rgba;
    output.Append(element0);

    element0.xyzw = input[1].xyzw + float4(2, -2, -2, 0);
    element0.xyzw = mul(input[1].xyzw, mWorld);
    element0.xyzw = mul(element0.xyzw, mView);
    element0.xyzw = mul(element0.xyzw, mProjection);
    element0.rgba = input[1].rgba;
    output.Append(element0);

    //output.RestartStrip();

}