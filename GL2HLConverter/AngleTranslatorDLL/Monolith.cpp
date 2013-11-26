#include "Monolith.h"

ANGLETRANSLATORDLL_API void humongoid(const char * vertexShaderSrc, const char * fragmentShaderSrc /*, char ** vertexHlsl, char ** fragmentHlsl */ )
{
	//coordSystem = LEFT_CARTESIAN;

	Program *program = new Program();

	VertexShader *vShader = new VertexShader((char*) vertexShaderSrc);
	FragmentShader *fShader = new FragmentShader((char*) fragmentShaderSrc);

	vShader->compile();
	fShader->compile();
	
	program->attachShaders(vShader, fShader);
	program->bindAttributeLocation(0, "vPosition"); // Attribute binding appears to be optional (not sure) - seems to happen in linkAttributes
	program->link();
};