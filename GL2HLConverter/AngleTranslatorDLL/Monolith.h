#define GL_NONE                           0 // Copied from gl2.h

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <list>
#include <vector>
#include <set>
#include <stdexcept>

#include "GLSLANG\ShaderLang.h"
#include "GLES2\gl2.h"

//STRUCTS & ENUMS================================================================================================================

typedef unsigned int     GLenum; // Copied from gl2.h

// Enum taken from constants.h
enum
{
    MAX_VERTEX_ATTRIBS = 16,
    MAX_TEXTURE_IMAGE_UNITS = 16,

    // Implementation upper limits, real maximums depend on the hardware
    IMPLEMENTATION_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 16,
    IMPLEMENTATION_MAX_COMBINED_TEXTURE_IMAGE_UNITS = MAX_TEXTURE_IMAGE_UNITS + IMPLEMENTATION_MAX_VERTEX_TEXTURE_IMAGE_UNITS,    

    IMPLEMENTATION_MAX_VARYING_VECTORS = 32,
    IMPLEMENTATION_MAX_DRAW_BUFFERS = 8
};

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

std::set<std::string> attributeBinding[MAX_VERTEX_ATTRIBS];

//FLAGS & PLACEHOLDERS================================================================================================================

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

char *fragmentHLSL = NULL;
char *vertexHLSL = NULL;

char *fragmentInfoLog = NULL;
char *vertexInfoLog = NULL;

VaryingList varyings;
AttributeArray attributes;
ActiveUniforms activeUniforms;

//FUNCTIONS================================================================================================================

void LinkUniforms();
void LinkAttributes();
void LinkVaryings();
bool Link();

void BindAttributeLocation(GLuint index, const GLchar* name);

bool CompareVarying(const Varying &x, const Varying &y);

bool IsCompiled(char * hlsl);
GLenum ParseType(const std::string &type);
void ParseAttributes(char * hlsl);
void ParseVaryings(char * hlsl);
void CompileToHLSL(void * compiler, const char * shaderSrc, char * hlsl, char * infoLog);

void CompileFragmentShader();
void CompileVertexShader();

void ConstructCompiler(ShBuiltInResources resources);
ShBuiltInResources InitBuiltInResources(); 
void Humongoid(const char * fragmentShaderSrc, const char * vertexShaderSrc);
	

