#define GL_NONE                           0 // Copied from gl2.h

#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "GLSLANG\ShaderLang.h"

typedef unsigned int     GLenum; // Copied from gl2.h

// Copied from Shader.h
struct Varying
{
    Varying(GLenum type, const std::string &name, int size, bool array)
        : type(type), name(name), size(size), array(array), reg(-1), col(-1)
    {
    }

    GLenum type;
    std::string name;
    int size;   // Number of 'type' elements
    bool array;

    int reg;    // First varying register, assigned during link
    int col;    // First register element, assigned during link
};

typedef std::list<Varying> VaryingList;


// Copied from Shader.h
struct Attribute
{
	Attribute() : type(GL_NONE), name("")
    {
    }

	Attribute(GLenum type, const std::string &name) : type(type), name(name)
    {
    }

    GLenum type;
    std::string name;
};

typedef std::vector<Attribute> AttributeArray;

// Copied from Uniform.h
struct Uniform
{
	Uniform(GLenum type, GLenum precision, const char *name, int arraySize, int registerIndex)
		: type(type), precision(precision), name(name), arraySize(arraySize), registerIndex(registerIndex)
	{
	}

    GLenum type;
    GLenum precision;
    std::string name;
    unsigned int arraySize;
    
    int registerIndex;
};

typedef std::vector<Uniform> ActiveUniforms;


//================================================================================================================
VaryingList varyings;

bool usesMultipleRenderTargets;
bool usesFragColor;
bool usesFragData;
bool usesFragCoord;
bool usesFrontFacing;
bool usesPointSize;
bool usesPointCoord;
bool usesDepthRange;
bool usesFragDepth;
bool usesDiscardRewriting;

static void *fragmentCompiler;
static void *vertexCompiler;

char *hlsl;
char *infoLog;
ActiveUniforms activeUniforms;
//================================================================================================================



// FUNCTIONS
void CompileShader(void * compiler, const char * shaderSrc);
void ConstructCompiler(ShBuiltInResources resources);
ShBuiltInResources InitBuiltInResources(); 
void Humongoid(const char * fragmentShaderSrc, const char * vertexShaderSrc);


