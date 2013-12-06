// World Space

//uniform mat4 ModelViewProjection;

attribute vec4 Position;
attribute vec2 TextureCoordinates;

varying vec2 textureCoordinateVarying;

void main()
{
	gl_Position = Position; //* ModelViewProjection;

	textureCoordinateVarying = TextureCoordinates;
}