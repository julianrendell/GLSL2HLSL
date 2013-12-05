// Attributes
static float4 _vPosition = {0, 0, 0, 0};
static float2 _vTxPosition = {0, 0};

static float4 gl_Position = float4(0, 0, 0, 0);
static float2 gl_TxPosition = float2(0, 0);


// Varyings


;
void gl_main()
{
{
(gl_Position = _vPosition);
(gl_TxPosition = _vTxPosition);
}
}
;
struct VS_INPUT
{
    float4 _vPosition : TEXCOORD0;
	float2 _vTxPosition : TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 gl_Position : SV_Position;
	float2 gl_TxPosition : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    _vPosition = (input._vPosition);
	_vTxPosition = (input._vTxPosition);

    gl_main();

    VS_OUTPUT output;
    output.gl_Position.x = gl_Position.x;
    output.gl_Position.y = gl_Position.y;
    output.gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5;
    output.gl_Position.w = gl_Position.w;

	output.gl_TxPosition = gl_TxPosition;

    return output;
}
