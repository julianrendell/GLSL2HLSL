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
#include <algorithm>

#include "GLSLANG\ShaderLang.h"
#include "GLES2\gl2.h"

#include "AngleTranslatorDLL.h"

//STRUCTS & ENUMS================================================================================================================

std::string pixelHLSL;
std::string vertexHLSL;

typedef unsigned int     GLenum; // Copied from gl2.h

// Enum taken from constants.h
enum
{
    MAX_VERTEX_ATTRIBS = 16,
    MAX_TEXTURE_IMAGE_UNITS = 16,
	MAX_TEXTURE_IMAGE_UNITS_VTF_SN4 = 16,

    // Implementation upper limits, real maximums depend on the hardware
    IMPLEMENTATION_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 16,
    IMPLEMENTATION_MAX_COMBINED_TEXTURE_IMAGE_UNITS = MAX_TEXTURE_IMAGE_UNITS + IMPLEMENTATION_MAX_VERTEX_TEXTURE_IMAGE_UNITS,    

    IMPLEMENTATION_MAX_VARYING_VECTORS = 32,
    IMPLEMENTATION_MAX_DRAW_BUFFERS = 8,

	MAX_VERTEX_UNIFORM_VECTORS_D3D11 = 1024,
    MAX_FRAGMENT_UNIFORM_VECTORS_D3D11 = 1024
};

// Copied from angletypes.h
enum TextureType
{
    TEXTURE_2D,
    TEXTURE_CUBE,

    TEXTURE_TYPE_COUNT,
    TEXTURE_UNKNOWN
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

Attribute mLinkedAttribute[MAX_VERTEX_ATTRIBS];
int mSemanticIndex[MAX_VERTEX_ATTRIBS];
int mAttributesByLayout[MAX_VERTEX_ATTRIBS];

struct Sampler
{
    bool active;
    GLint logicalTextureUnit;
    TextureType textureType;
};

Sampler mSamplersPS[MAX_TEXTURE_IMAGE_UNITS];
Sampler mSamplersVS[IMPLEMENTATION_MAX_VERTEX_TEXTURE_IMAGE_UNITS];
GLuint mUsedVertexSamplerRange;
GLuint mUsedPixelSamplerRange;

// Copied from Uniform.h
struct Uniform
{
	Uniform(GLenum type, GLenum precision, const std::string &name, unsigned int arraySize)
	{
	}

	unsigned int elementCount() const;

    GLenum type;
    GLenum precision;
    std::string name;
    unsigned int arraySize;
    
    int psRegisterIndex;
    int vsRegisterIndex;

	int registerIndex;

	unsigned int registerCount;
};

typedef std::vector<Uniform> ActiveUniforms;

// Struct used for correlating uniforms/elements of uniform arrays to handles
struct UniformLocation
{
    UniformLocation(const std::string &name, unsigned int element, unsigned int index)
	{
	}

    std::string name;
    unsigned int element;
    unsigned int index;
};

typedef std::vector<Uniform*> UniformArray;

UniformArray mUniforms;
typedef std::vector<UniformLocation> UniformIndex;
UniformIndex mUniformIndex;

struct AttributeSorter
{
    AttributeSorter(const int (&semanticIndices)[MAX_VERTEX_ATTRIBS])
        : originalIndices(semanticIndices)
    {
    }

    bool operator()(int a, int b)
    {
        return originalIndices[a] == -1 ? false : originalIndices[a] < originalIndices[b];
    }

    const int (&originalIndices)[MAX_VERTEX_ATTRIBS];
};

//CLASSES================================================================================================================

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
	int getSemanticIndex(const std::string &attributeName);

    AttributeArray mAttributes;
};

class FragmentShader : public Shader
{
};

VertexShader *vShader;
FragmentShader *fShader;


class AttributeBindings
{
  public:
    void bindAttributeLocation(GLuint index, const char *name);
    int getAttributeBinding(const std::string &name) const;

    std::set<std::string> mAttributeBinding[MAX_VERTEX_ATTRIBS];
};

AttributeBindings attributeBinding;

//FUNCTIONS================================================================================================================

void initAttributesByLayout();
int AllocateFirstFreeBits(unsigned int *bits, unsigned int allocationSize, unsigned int bitsSize);
std::string decorateAttribute(const std::string &name);
bool defineUniform(GLenum shader, const Uniform &constant);
int getUniformLocation(std::string name);

bool linkUniforms(ActiveUniforms vertexUniforms, ActiveUniforms fragmentUniforms);
bool linkAttributes(const AttributeBindings &attributeBindings, FragmentShader *fragmentShader, VertexShader *vertexShader);
bool linkVaryings(int registers, const Varying *packing[][4],
                                 std::string& pixelHLSL, std::string& vertexHLSL,
                                 FragmentShader *fragmentShader, VertexShader *vertexShader);
int VariableColumnCount(GLenum type);
int VariableRowCount(GLenum type);
int packVaryings(const Varying *packing[][4], FragmentShader *fragmentShader);
void link();

bool compareVarying(const Varying &x, const Varying &y);
bool isCompiled(char * hlsl);
GLenum parseType(const std::string &type);

void compileToHLSL(Shader *shader);

void compileVertexShader(VertexShader *vShader);
void compileFragmentShader(FragmentShader *fShader);

void constructCompiler(ShBuiltInResources resources);
ShBuiltInResources initBuiltInResources(); 
ANGLETRANSLATORDLL_API void humongoid(const char * fragmentShaderSrc, const char * vertexShaderSrc);
	

