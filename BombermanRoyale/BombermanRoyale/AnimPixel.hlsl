Texture2D txDiffuse : register(t0);
Texture2D txEmissive : register(t1);
Texture2D txSpecular : register(t2);
SamplerState txSampler : register(s0);

struct Light
{
    float3 dir;
    float intensity;
    float3 position;
    float pad1;
    float3 color;
    float pad2;
    int type;
};

struct Material
{
    float3 surfaceDiffuse;
    float diffuseFactor;
    float3 surfaceEmissive;
    float emissiveFactor;
    float3 surfaceSpecular;
    float specularFactor;
    float3 surfaceShiny;
    float shininess;
};

cbuffer LightBuffer : register(b0)
{
    Light light;
}
cbuffer MatBuffer : register(b1)
{
    Material _mat;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD1;
    float4 wPos : WORLD;
    float4 eye : EYE;
};

static float4 ambient_light = { 0.9f, 0.75f, 0.5f, 1.0f };
static float4 light_dir = { 0.0f, -1.0f, 1.0f, 1.0f };

float4 main(PS_IN input) : SV_TARGET
{


    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 norm = float4(normalize(input.norm), 1.0f);

    //finalColor += ambient_light;



    float lightRatio = saturate(dot(-normalize(light_dir.xyz), norm.xyz));
    
    finalColor += lightRatio * (float4(float3(0.9f, 0.75f, 0.5f) * 1.0f, 1.0f));




    /*

	float3 light_dir = light.position - input.wPos.xyz;
	float sq_distance = dot(light_dir, light_dir);
	light_dir = light_dir / sqrt(sq_distance);

	float3 eyeDir = input.eye.xyz - input.wPos.xyz;
	float sqDistEye = dot(eyeDir, eyeDir);
	float distanceEye = sqrt(sqDistEye);
	eyeDir = eyeDir / distanceEye;

	float3 norm = normalize(input.norm.xyz);
	float nl = dot(norm, light_dir);
	float diffuse_intensity = saturate(nl);
	float3 half_vector = normalize(light_dir + eyeDir);
	float nh = dot(norm, half_vector);
	float specIntensity = pow(saturate(nh), 1 + _mat.surfaceShiny);
	//float4 lightIntensity = float4(light.color.xyz, 0.0) * light.intensity * sq_distance;
	float4 mat_Diffuse = txDiffuse.Sample(txSampler, input.tex) * float4(_mat.surfaceDiffuse, 0.0) * _mat.diffuseFactor;
	float4 mat_specular = txSpecular.Sample(txSampler, input.tex) * float4(_mat.surfaceSpecular, 0.0) * _mat.specularFactor;
	float4 mat_emissive = txEmissive.Sample(txSampler, input.tex) * float4(_mat.surfaceEmissive, 0.0) * _mat.emissiveFactor;
	float4 emissive = mat_emissive;
	float4 ambient = mat_Diffuse * ambient_light;
	float4 specular = mat_specular * specIntensity /** lightIntensity;
	float4 diffuse = mat_Diffuse * diffuse_intensity /** lightIntensity;
	diffuse.xyz -= specular.xyz;
	diffuse.xyz = saturate(diffuse.xyz);
	finalColor = ambient + specular + diffuse + emissive;
    */
    //finalColor = txDiffuse.Sample(txSampler, input.tex);
    return saturate(finalColor * txDiffuse.Sample(txSampler, input.tex));
}