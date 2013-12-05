// Varyings

static float4 gl_Color[1] =
{
    float4(0, 0, 0, 0)
};

cbuffer DriverConstants : register(b1)
{
};

Texture2D picture;
SamplerState pictureSampler;
uniform float4 Color : register(c0);

#define GL_USES_FRAG_COLOR
void gl_main(float2 tex)
{
{
(gl_Color[0] = picture.Sample(pictureSampler, tex));
gl_Color[0].w = Color.w;
}
}
;
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 gl_Color0 : SV_Target0;
    float4 gl_Color1 : SV_Target1;
    float4 gl_Color2 : SV_Target2;
    float4 gl_Color3 : SV_Target3;
    float4 gl_Color4 : SV_Target4;
    float4 gl_Color5 : SV_Target5;
    float4 gl_Color6 : SV_Target6;
    float4 gl_Color7 : SV_Target7;
};

PS_OUTPUT main(PS_INPUT input)
{

    gl_main(input.tex);

    PS_OUTPUT output;
    output.gl_Color0 = gl_Color[0];
    output.gl_Color1 = gl_Color[0];
    output.gl_Color2 = gl_Color[0];
    output.gl_Color3 = gl_Color[0];
    output.gl_Color4 = gl_Color[0];
    output.gl_Color5 = gl_Color[0];
    output.gl_Color6 = gl_Color[0];
    output.gl_Color7 = gl_Color[0];

    return output;
}
