cbuffer MVP_t : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer joints : register(b1)
{
    int numJoints;
    matrix joints[30];
}

struct VS_IN
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;
    int4 joints : JOINTS;
    float4 weights : WEIGHTS;
};


struct VS_OUT
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD1;
    float4 wPos : WORLD;
    float4 eyePos : EYE;

};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 skinnedPos = float4(0, 0, 0, 0);
    float4 skinnedNorm = float4(0, 0, 0, 0);

    for (int i = 0; i < 4; ++i)
    {
        if (input.joints[i] >= 0)
        {
			skinnedPos += mul(float4(input.pos, 1.0f), joints[input.joints[i]]) *input.weights[i];
            skinnedNorm += mul(float4(input.norm, 0.0f), joints[input.joints[i]]) * input.weights[i];
        }
    }

    output.pos = mul(float4(skinnedPos.xyz, 1), world);
	output.pos = mul(float4(input.pos.xyz, 1), world);
    output.wPos = output.pos;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    //output.pos = float4(output.pos.xyz, 1);

    output.norm = mul(float4(skinnedNorm.xyz, 0), world);
	output.norm = mul(float4(0.0f, 0.0f, 1.0f, 0.0f), world);

    output.tex = float2(input.tex.x, 1.0f - input.tex.y);
    output.eyePos.x = -dot(view[3].xyz, view[0].xyz);
    output.eyePos.y = -dot(view[3].xyz, view[1].xyz);
    output.eyePos.z = -dot(view[3].xyz, view[2].xyz);
    output.eyePos.w = 1.0f;

    return output;
}
