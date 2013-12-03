// Varyings
static float2 _textureCoordinateVarying = {0, 0};

static float4 gl_Color[1] =
{
    float4(0, 0, 0, 0)
};

cbuffer DriverConstants : register(b1)
{
};

uniform float _Base : register(c0);
uniform float _Bias : register(c1);
uniform float _Percentage : register(c2);
uniform SamplerState sampler__Texture0 : register(s0);
uniform Texture2D texture__Texture0 : register(t0);

float4 gl_texture2D(Texture2D t, SamplerState s, float2 uv)
{
    return t.Sample(s, uv);
}

#define GL_USES_FRAG_COLOR
;
;
;
;
;
void gl_main()
{
{
float2 _inverseCoordinates = _textureCoordinateVarying;
(_inverseCoordinates.y = (1.0 - _inverseCoordinates.y));
(gl_Color[0] = gl_texture2D(texture__Texture0, sampler__Texture0, _inverseCoordinates));
float _alpha = ((1.0 - min(pow((_textureCoordinateVarying.y / _Percentage), _Bias), 1.0)) * _Base);
(gl_Color[0].w *= _alpha);
(gl_Color[0].xyz *= _alpha);
}
}
;
struct PS_INPUT
{
    float2 v0 : TEXCOORD0;
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
    _textureCoordinateVarying = input.v0.xy;

    gl_main();

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
