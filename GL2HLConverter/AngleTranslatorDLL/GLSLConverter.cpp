#include "GLSLConverter.h"

ANGLETRANSLATORDLL_API void humongoid(const char * vertexShaderSrc, const char * fragmentShaderSrc, CoordinateSystemType coordinateSystem /*, char ** vertexHlsl, char ** fragmentHlsl */ )
{
	coordinateSystemType = coordinateSystem;

	gl::Program *program = new gl::Program();

	gl::VertexShader *vShader = new gl::VertexShader((char*) vertexShaderSrc);
	gl::FragmentShader *fShader = new gl::FragmentShader((char*) fragmentShaderSrc);

	vShader->compile();
	fShader->compile();
	
	program->attachShaders(vShader, fShader);
	//program->bindAttributeLocation(0, "vPosition"); // Attribute binding appears to be optional (not sure) - seems to happen in linkAttributes
	program->link();
};