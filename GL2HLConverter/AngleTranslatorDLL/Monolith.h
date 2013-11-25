#define GL_NONE                           0 // Copied from gl2.h
#define	D3D11_VS_OUTPUT_REGISTER_COUNT	( 32 )
#define	D3D11_SHADER_MAJOR_VERSION	( 5 )
#define	D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT	( 8 )

#define MAX_VERTEX_ATTRIBS ( 16 )
#define MAX_TEXTURE_IMAGE_UNITS ( 16 )
#define MAX_TEXTURE_IMAGE_UNITS_VTF_SN4 ( 16 )

// Implementation upper limits, real maximums depend on the hardware
#define IMPLEMENTATION_MAX_VERTEX_TEXTURE_IMAGE_UNITS ( 16 )
#define IMPLEMENTATION_MAX_COMBINED_TEXTURE_IMAGE_UNITS ( MAX_TEXTURE_IMAGE_UNITS + IMPLEMENTATION_MAX_VERTEX_TEXTURE_IMAGE_UNITS )
#define IMPLEMENTATION_MAX_VARYING_VECTORS ( 32 )
#define IMPLEMENTATION_MAX_DRAW_BUFFERS ( 8 )
#define MAX_VERTEX_UNIFORM_VECTORS_D3D11 ( 1024 )
#define MAX_FRAGMENT_UNIFORM_VECTORS_D3D11 ( 1024 )

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string>
#include <set>
#include <stdexcept>
#include <algorithm>

#include "GLSLANG\ShaderLang.h"
#include "GLES2\gl2.h"

#include "AngleTranslatorDLL.h"
#include "Shader.h"
#include "Utilities.h"


std::string pixelHLSL;
std::string vertexHLSL;

VertexShader *vShader;
FragmentShader *fShader;


//ENUMS================================================================================================================

enum CoordinateSystemType
{
	LEFT_CARTESIAN,
	RIGHT_CARTESIAN
};

CoordinateSystemType coordSystem;

// Copied from angletypes.h
enum TextureType
{
	TEXTURE_2D,
	TEXTURE_CUBE,

	TEXTURE_TYPE_COUNT,
	TEXTURE_UNKNOWN
};



Attribute mLinkedAttribute[MAX_VERTEX_ATTRIBS];
int mSemanticIndex[MAX_VERTEX_ATTRIBS];
int mAttributesByLayout[MAX_VERTEX_ATTRIBS];


//STRUCTS================================================================================================================


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


int packVaryings(const Varying *packing[][4], FragmentShader *fragmentShader);
void link();

void constructCompiler(ShBuiltInResources resources);

ANGLETRANSLATORDLL_API void humongoid(const char * fragmentShaderSrc, const char * vertexShaderSrc);
