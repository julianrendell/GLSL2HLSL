// AngleTranslatorDLL.cpp : Defines the exported functions for the DLL application.
//

//#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "stdio.h"
#include "AngleTranslatorDLL.h"
//#include "compiler\debug.h"


ANGLETRANSLATORDLL_API int ShaderInitialize()
{
	return ShInitialize();
}

ANGLETRANSLATORDLL_API ShHandle ShaderConstructCompiler(ShShaderType type,
    ShShaderSpec spec,
    ShShaderOutput output,
    const ShBuiltInResources* resources)
{
	return ShConstructCompiler(type, spec, output, resources);
}

ANGLETRANSLATORDLL_API int ShaderCompile(const ShHandle handle,
    const char* const shaderStrings[],
    size_t numStrings,
    int compileOptions
    )
{
	return ShCompile(handle, shaderStrings, numStrings, compileOptions);
}

ANGLETRANSLATORDLL_API void ShaderDestruct(ShHandle handle)
{
	ShDestruct(handle);
}

ANGLETRANSLATORDLL_API int ShaderFinalize()
{
	return ShFinalize();
}

ANGLETRANSLATORDLL_API void ShaderInitBuiltInResources(ShBuiltInResources* resources)
{
	ShInitBuiltInResources(resources);
}

ANGLETRANSLATORDLL_API void ShaderGetInfo(const ShHandle handle, ShShaderInfo pname, size_t* params)
{
	ShGetInfo(handle, pname, params);
}

ANGLETRANSLATORDLL_API void ShaderGetObjectCode(const ShHandle handle, char* objCode)
{
	ShGetObjectCode(handle, objCode);
}


///
// Create a shader object, load the shader source, and
// compile the shader.
//
ANGLETRANSLATORDLL_API GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;
   
   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
   	return 0;

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );
   
   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled ) 
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

      glDeleteShader ( shader );
      return 0;
   }

   return 0;

}

ANGLETRANSLATORDLL_API GLuint GetHLSL(const char *shaderSrc)
{
	GLuint vertexShader;
    GLuint fragmentShader;
	GLuint programObject;
	GLint linked;

	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, shaderSrc);

	// Create the program object
    programObject = glCreateProgram ( );
   
    if ( programObject == 0 )
       return 0;

    glAttachShader ( programObject, fragmentShader );

    // Bind vPosition to attribute 0   
    glBindAttribLocation ( programObject, 0, "vPosition" );

    // Link the program
    glLinkProgram ( programObject );

    // Check the link status
    glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

	GLsizei maxCount = 1;
	GLsizei count = 1;
	glGetAttachedShaders(programObject, maxCount, &count, &fragmentShader);

	gl::Context *context = gl::getNonLostContext();
	gl::Shader *shader = context->getShader(fragmentShader);
	gl::FragmentShader* mFragmentShader = (gl::FragmentShader*) shader;
	std::string fShader = shader->getHLSL();

	return 0;
}