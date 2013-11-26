#ifndef SHADER_H_
#define SHADER_H_

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <list>
#include <vector>

#include "GLSLANG\ShaderLang.h"
#include "GLES2\gl2.h"

#include "Utilities.h"
#include "Uniform.h"

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


class Shader
{

public:
	Shader()
	{
	}

    virtual ~Shader()
	{
	}
	
	bool isCompiled();
	const char * getHLSL();

protected:
	ShBuiltInResources initBuiltInResources();
	void compileToHLSL(ShShaderType shaderType);
	void parseVaryings();
	void resetVaryingsRegisterAssignment();
	GLenum parseType(const std::string &type);
	static bool compareVarying(const Varying &x, const Varying &y);
	void setSource(const char *shaderSrc);
	void setHLSL(const char *shaderSrc);

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

private:
	friend class ProgramBinary;

	void constructCompiler(ShShaderType shaderType, ShBuiltInResources resources);

	static void *mCompiler;
	char *mSource;
    char *mHlsl;
    char *mInfoLog;
	ActiveUniforms mActiveUniforms;
};

class VertexShader : public Shader
{
	public:
		VertexShader(const char *shaderSrc);
		~VertexShader();

		void parseAttributes();
		int getSemanticIndex(const std::string &attributeName);
		void compile();

		AttributeArray mAttributes;
};

class FragmentShader : public Shader
{
	public:
		FragmentShader(const char *shaderSrc);
		~FragmentShader();

		void compile();
};

#endif