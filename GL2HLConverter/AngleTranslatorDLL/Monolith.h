#define GL_NONE                           0 // Copied from gl2.h
#define	D3D11_VS_OUTPUT_REGISTER_COUNT	( 32 )
#define	D3D11_SHADER_MAJOR_VERSION	( 5 )
#define	D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT	( 8 )

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
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

//SHADER CLASSES================================================================================================================

class Shader
{
public:
	void parseVaryings();
	void resetVaryingsRegisterAssignment();

	bool mUsesMultipleRenderTargets;
    bool mUsesFragColor;
    bool mUsesFragData;
    bool mUsesFragCoord;
    bool mUsesFrontFacing;
    bool mUsesPointSize;
    bool mUsesPointCoord;
    bool mUsesDepthRange;
    bool mUsesFragDepth;
    bool mUsesDiscardRewriting;

	VaryingList mVaryings;

	char *mSource;
    char *mHlsl;
    char *mInfoLog;

	void *compiler;

	ActiveUniforms mActiveUniforms;
};

class VertexShader : public Shader
{
  public:
    void parseAttributes();

    AttributeArray mAttributes;
};

class FragmentShader : public Shader
{
};

VertexShader *vShader;
FragmentShader *fShader;

class InfoLog
{
  public:
    InfoLog();
    ~InfoLog();

    int getLength() const;
    void getLog(GLsizei bufSize, GLsizei *length, char *infoLog);

    void appendSanitized(const char *message);
    void append(const char *info, ...);
    void reset();

    char *mInfoLog;
};

//FUNCTIONS================================================================================================================

std::string decorateAttribute(const std::string &name);

void linkUniforms();
void linkAttributes();
bool linkVaryings(int registers, const Varying *packing[][4],
                                 std::string& pixelHLSL, std::string& vertexHLSL,
                                 FragmentShader *fragmentShader, VertexShader *vertexShader);
int VariableColumnCount(GLenum type);
int VariableRowCount(GLenum type);
int packVaryings(const Varying *packing[][4], FragmentShader *fragmentShader);
bool link();

void bindAttributeLocation(GLuint index, const GLchar* name);
bool compareVarying(const Varying &x, const Varying &y);
bool isCompiled(char * hlsl);
GLenum parseType(const std::string &type);

void compileToHLSL(Shader *shader);

void compileVertexShader(VertexShader *vShader);
void compileFragmentShader(FragmentShader *fShader);

void constructCompiler(ShBuiltInResources resources);
ShBuiltInResources initBuiltInResources(); 
void humongoid(const char * fragmentShaderSrc, const char * vertexShaderSrc);
	

