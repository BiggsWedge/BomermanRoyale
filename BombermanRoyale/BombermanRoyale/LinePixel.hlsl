

struct OutputVertex {
	float4 xyzw : SV_POSITION; //system value sv
	float4 rgba : COLOR;
};

float4 main(OutputVertex inputPixel) : SV_TARGET
{
	return inputPixel.rgba;
}
