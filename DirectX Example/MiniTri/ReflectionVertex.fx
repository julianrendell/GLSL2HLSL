// Attributes
static float4 _Position = {0, 0, 0, 0};
static float2 _TextureCoordinates = {0, 0};

static float4 gl_Position = float4(0, 0, 0, 0);

// Varyings
static float2 _textureCoordinateVarying = {0, 0};


;
;
;
void gl_main()
{
{
(gl_Position = _Position);
(_textureCoordinateVarying = _TextureCoordinates);
}
}
;
struct VS_INPUT
{
    float4 _Position : TEXCOORD0;
    float2 _TextureCoordinates : TEXCOORD1;
};

struct VS_OUTPUT
{
    float2 v0 : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

VS_OUTPUT main(VS_INPUT input)
{
    _Position = (input._Position);
    _TextureCoordinates = (input._TextureCoordinates);

    gl_main();

    VS_OUTPUT output;
    output.gl_Position.x = gl_Position.x;
    output.gl_Position.y = gl_Position.y;
    output.gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5;
    output.gl_Position.w = gl_Position.w;
    output.v0 = _textureCoordinateVarying;

    return output;
}
