#ifndef PROGRAM_BINARY_H_
#define PROGRAM_BINARY_H_

#define _CRT_SECURE_NO_WARNINGS

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

#include <stdio.h>
#include <set>
#include <string>
#include <algorithm>

#include "Shader.h"
#include "Angletypes.h"
#include "Program.h"


class AttributeBindings;

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

// ProgramBinary has a few functions from Program.cpp (This was how Angle split it up)
class ProgramBinary
{
	public:
		 ProgramBinary();
		~ProgramBinary();

		void link(AttributeBindings attributeBindings, FragmentShader *fShader, VertexShader *vShader);
		void initAttributesByLayout();
		GLint getUniformLocation(std::string name);
		std::string decorateAttribute(const std::string &name);

		CoordinateSystemType coordSystem;

	private:
		bool defineUniform(GLenum shader, const Uniform &constant);

		bool linkUniforms(ActiveUniforms vertexUniforms, ActiveUniforms fragmentUniforms);
		bool linkAttributes(const AttributeBindings &attributeBindings, FragmentShader *fragmentShader, VertexShader *vertexShader);
		bool linkVaryings(int registers, const Varying *packing[][4],
											std::string& pixelHLSL, std::string& vertexHLSL,
											FragmentShader *fragmentShader, VertexShader *vertexShader);

		int packVaryings(const Varying *packing[][4], FragmentShader *fragmentShader);

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

		UniformArray mUniforms;
		typedef std::vector<UniformLocation> UniformIndex;
		UniformIndex mUniformIndex;
};


#endif