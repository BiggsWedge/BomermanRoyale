cbuffer MVP_t : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer JOINTS : register(b1)
{
    int numJoints;
    matrix joints[50];
}

struct VS_IN
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;
    float4 tang : TANGENT;
    uint4 joints : JOINTS;
    float4 weights : WEIGHTS;
};


struct VS_OUT
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD1;
    float4 wPos : WORLD;
    float4 eyePos : EYE;
    float3 tangent : TANGENT;
    float3 bitang : BITANGENT;

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
            skinnedPos += mul(float4(input.pos, 1.0f), joints[input.joints[i]]) * input.weights[i];
            skinnedNorm += mul(float4(input.norm, 1.0f), joints[input.joints[i]]) * input.weights[i];
        }
    }
	//skinnedPos = float4(input.pos, 1.0f);
	//skinnedNorm = float4(input.norm, 0.0f);

    output.pos = mul(float4(skinnedPos.xyz, 1), world);
    output.wPos = output.pos;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
	//output.pos = float4(output.pos.xyz, 1);

    output.norm = normalize(input.norm); //mul(float4(skinnedNorm.xyz, 1), world).xyz);
    output.tangent = normalize(mul(float4(input.tang.xyz * input.tang.w, 0.0f), world));
    output.bitang = normalize(mul(float4(cross(input.norm.xyz, input.tang.xyz), 0.0f), world));

    output.norm = normalize(mul(normalize(float4(input.norm.xyx, 1)), world).xyz);

    output.tex = float2(input.tex.x, input.tex.y);
    output.eyePos.x = -dot(view[3].xyz, view[0].xyz);
    output.eyePos.y = -dot(view[3].xyz, view[1].xyz);
    output.eyePos.z = -dot(view[3].xyz, view[2].xyz);
    output.eyePos.z = 1.0f;

    return output;
}