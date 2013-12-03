// Attributes
static float4 _vPosition = {0, 0, 0, 0};

static float4 gl_Position = float4(0, 0, 0, 0);

// Varyings


;
void gl_main()
{
{
(gl_Position = _vPosition);
}
}
;
struct VS_INPUT
{
    float4 _vPosition : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 gl_Position : SV_Position;
};

VS_OUTPUT main(VS_INPUT input)
{
    _vPosition = (input._vPosition);

    gl_main();

    VS_OUTPUT output;
    output.gl_Position.x = gl_Position.x;
    output.gl_Position.y = gl_Position.y;
    output.gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5;
    output.gl_Position.w = gl_Position.w;

    return output;
}
