// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

static const float GRASS_TRESHOLD = 0.7f;
static const float MUD_TRESHOLD = 0.7f;
static const float ICE_TRESHOLD = 0.4f;


Texture2D shaderTexture1 : register(t0);
Texture2D shaderTexture2 : register(t1);
Texture2D shaderTexture3 : register(t2);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
	float4	textureColor;
    float3	lightDir;
    float	lightIntensity;
    float4	color;
	
	float4 grassColor = (shaderTexture1.Sample(SampleType, input.tex));
	float4 mudColor = (shaderTexture2.Sample(SampleType, input.tex));
	float4 iceColor = (shaderTexture3.Sample(SampleType, input.tex));
	float4 blendColor = ((shaderTexture1).Sample(SampleType, input.tex) / 2.0f) + (shaderTexture2.Sample(SampleType, input.tex) / 2.0f);
	float slope = 1.0f - input.normal.y;
	
	float blendValue;
	float interpolationValue = (1 - input.position3D.y);
	
	//if (slope < ICE_TRESHOLD)
	//{
	//	textureColor = iceColor;
	//}
	//else if (slope < GRASS_TRESHOLD)
	//{
	//	blendValue = slope / GRASS_TRESHOLD;
	//	textureColor = lerp(iceColor, grassColor, blendValue);
	//}
	//else if (slope < MUD_TRESHOLD)
	//{
	//	textureColor = mudColor;
	//}
	
	
	//if (input.position3D.y < 0.85f)
	//{
	if (slope < 0.2f)
	{
		blendValue = slope / 0.2f;
		textureColor = lerp(mudColor, grassColor, blendValue);
	}
	if (slope >= 0.2f && slope < 0.55f)
	{
		blendValue = (slope - 0.2f) / (0.55f - 0.2f);
		textureColor = lerp(grassColor, mudColor, blendValue);
	}
	if (slope > 0.55f )
	{
		textureColor = iceColor;
	}
	//}
	if (input.position3D.y > 0.95f)// || slope < 0.55f)
	{
		textureColor = iceColor;
	}
	
	//if (input.position3D.y > 0.7f && input.position3D.y < 1.2f)
	//{
	//	textureColor = lerp(iceColor, blendColor, 0.5f);
	//}
	
	
	

	// Invert the light direction for calculations.
		lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
		lightIntensity = saturate(dot(input.normal, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
		color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
		color = saturate(color);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
		//textureColor = shaderTexture1.Sample(SampleType, input.tex);
		color = color * textureColor;

		return color;
	}

