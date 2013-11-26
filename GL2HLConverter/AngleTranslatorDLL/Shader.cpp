#include "Shader.h"


VertexShader::VertexShader(const char *shaderSrc)
{
	setSource(shaderSrc);
}

VertexShader::~VertexShader()
{
}

FragmentShader::FragmentShader(const char *shaderSrc)
{
	setSource(shaderSrc);
}

FragmentShader::~FragmentShader()
{
}

void *Shader::mCompiler = NULL;

GLenum Shader::parseType(const std::string &type)
{
	if (type == "float")
    {
        return GL_FLOAT;
    }
    else if (type == "float2")
    {
        return GL_FLOAT_VEC2;
    }
    else if (type == "float3")
    {
        return GL_FLOAT_VEC3;
    }
    else if (type == "float4")
    {
        return GL_FLOAT_VEC4;
    }
    else if (type == "float2x2")
    {
        return GL_FLOAT_MAT2;
    }
    else if (type == "float3x3")
    {
        return GL_FLOAT_MAT3;
    }
    else if (type == "float4x4")
    {
        return GL_FLOAT_MAT4;
    }
    else
	{
		return GL_NONE;
	}
}

// This is taken from Shader.cpp
// true if varying x has a higher priority in packing than y
bool Shader::compareVarying(const Varying &x, const Varying &y)
{
    if(x.type == y.type)
    {
        return x.size > y.size;
    }

    switch (x.type)
    {
      case GL_FLOAT_MAT4: return true;
      case GL_FLOAT_MAT2:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return true;
          case GL_FLOAT_VEC4: return true;
          case GL_FLOAT_MAT3: return true;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_VEC4:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return true;
          case GL_FLOAT_MAT3: return true;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_MAT3:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return false;
          case GL_FLOAT_MAT3: return true;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_VEC3:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return false;
          case GL_FLOAT_MAT3: return false;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_VEC2:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return false;
          case GL_FLOAT_MAT3: return false;
          case GL_FLOAT_VEC3: return false;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT: return false;
      default: return false;
    }

    return false;
}

void Shader::parseVaryings()
{
	if (mHlsl)
    {
		const char *input = strstr(mHlsl, "// Varyings") + 12;

        while(true)
        {
            char varyingType[256];
            char varyingName[256];

            int matches = sscanf(input, "static %255s %255s", varyingType, varyingName);

            if (matches != 2)
            {
                break;
            }

            char *array = strstr(varyingName, "[");
            int size = 1;

            if (array)
            {
                size = atoi(array + 1);
                *array = '\0';
            }

			GLenum type = parseType(varyingType);
			if (type == GL_NONE)
			{
				throw new std::runtime_error("Type of varying is unknown");
			}

			mVaryings.push_back(Varying(type, varyingName, size, array != NULL));

            input = strstr(input, ";") + 2;
        }

		mUsesMultipleRenderTargets = strstr(mHlsl, "GL_USES_MRT") != NULL;
        mUsesFragColor = strstr(mHlsl, "GL_USES_FRAG_COLOR") != NULL;
        mUsesFragData = strstr(mHlsl, "GL_USES_FRAG_DATA") != NULL;
        mUsesFragCoord = strstr(mHlsl, "GL_USES_FRAG_COORD") != NULL;
        mUsesFrontFacing = strstr(mHlsl, "GL_USES_FRONT_FACING") != NULL;
        mUsesPointSize = strstr(mHlsl, "GL_USES_POINT_SIZE") != NULL;
        mUsesPointCoord = strstr(mHlsl, "GL_USES_POINT_COORD") != NULL;
		mUsesDepthRange = strstr(mHlsl, "GL_USES_DEPTH_RANGE") != NULL;
        mUsesFragDepth = strstr(mHlsl, "GL_USES_FRAG_DEPTH") != NULL;
        mUsesDiscardRewriting = strstr(mHlsl, "ANGLE_USES_DISCARD_REWRITING") != NULL;
    }
}

void Shader::resetVaryingsRegisterAssignment()
{
    for (VaryingList::iterator var = mVaryings.begin(); var != mVaryings.end(); var++)
    {
        var->reg = -1;
        var->col = -1;
    }
}

void VertexShader::parseAttributes()
{
	const char *hlsl = getHLSL();

	if (hlsl)
    {
		const char *input = strstr(hlsl, "// Attributes") + 14;

        while(true)
        {
            char attributeType[256];
            char attributeName[256];

            int matches = sscanf(input, "static %255s _%255s", attributeType, attributeName);

            if (matches != 2)
            {
                break;
            }

			GLenum type = parseType(attributeType);
			if (type == GL_NONE)
			{
				throw new std::runtime_error("Type of attribute is unknown");
			}

			mAttributes.push_back(Attribute(type, attributeName));

            input = strstr(input, ";") + 2;
        }
    }
}

int VertexShader::getSemanticIndex(const std::string &attributeName)
{
    if (!attributeName.empty())
    {
        int semanticIndex = 0;
        for (AttributeArray::iterator attribute = mAttributes.begin(); attribute != mAttributes.end(); attribute++)
        {
            if (attribute->name == attributeName)
            {
                return semanticIndex;
            }

            semanticIndex += VariableRowCount(attribute->type);
        }
    }

    return -1;
}

ShBuiltInResources Shader::initBuiltInResources()
{
	ShBuiltInResources resources;

	ShInitBuiltInResources(&resources);

	// These are values specific to DX11, from Renderer11.cpp
	resources.MaxVertexAttribs = 16;
	resources.MaxVertexUniformVectors = 1024;
	resources.MaxVaryingVectors = 32;
	resources.MaxVertexTextureImageUnits = 16;
	resources.MaxCombinedTextureImageUnits = 16 + 16;
	resources.MaxTextureImageUnits = 16;
	resources.MaxFragmentUniformVectors = 1024;
	resources.MaxDrawBuffers = 8;

	resources.OES_standard_derivatives = true;
	resources.EXT_draw_buffers = 8 > 1;
	resources.FragmentPrecisionHigh = 1;
	resources.EXT_frag_depth = 1;

	return resources;
}

void Shader::constructCompiler(ShShaderType shaderType, ShBuiltInResources resources)
{
	if (shaderType == SH_FRAGMENT_SHADER)
	{
		mCompiler = ShConstructCompiler(shaderType, SH_GLES2_SPEC, SH_HLSL11_OUTPUT, &resources);
		if (NULL == mCompiler)
		{
			throw new std::runtime_error("Fragment compiler construction failed!");
		}
	}
	else if (shaderType == SH_VERTEX_SHADER)
	{
		mCompiler = ShConstructCompiler(SH_VERTEX_SHADER, SH_GLES2_SPEC, SH_HLSL11_OUTPUT, &resources);
		if (NULL == mCompiler)
		{
			throw new std::runtime_error("Vertex compiler construction failed!");
		}
	}
	else
	{
		throw new std::runtime_error("Unrecognized shader type");
	}
}

void Shader::compileToHLSL(ShShaderType shaderType)
{
	ShBuiltInResources resources = initBuiltInResources();
	constructCompiler(shaderType, resources);

	int result = ShCompile(mCompiler, &mSource, 1, SH_OBJECT_CODE);

	if (result)
	{
		size_t objCodeLen = 0;
		ShGetInfo(mCompiler, SH_OBJECT_CODE_LENGTH, &objCodeLen);
		mHlsl = new char[objCodeLen];
		ShGetObjectCode(mCompiler, mHlsl);

		void *activeUniforms;
		ShGetInfoPointer(mCompiler, SH_ACTIVE_UNIFORMS_ARRAY, &activeUniforms);
		activeUniforms = (ActiveUniforms*)activeUniforms;
	}
	else
	{
		size_t infoLogLen = 0;
		ShGetInfo(mCompiler, SH_INFO_LOG_LENGTH, &infoLogLen);
		mInfoLog = new char[infoLogLen];
		ShGetInfoLog(mCompiler, mInfoLog);

		throw new std::runtime_error(mInfoLog);
	}
}

bool Shader::isCompiled()
{
	return mHlsl != NULL;
}

void VertexShader::compile()
{
	compileToHLSL(SH_VERTEX_SHADER);

	if (!isCompiled())
	{
		throw new std::runtime_error("Vertex shader compilation has failed! (HLSL string is empty)");
	}

	parseAttributes();
	parseVaryings();
}

void FragmentShader::compile()
{
	compileToHLSL(SH_FRAGMENT_SHADER);

	if (!isCompiled())
	{
		throw new std::runtime_error("Fragment shader compilation has failed! (HLSL string is empty)");
	}

	parseVaryings();
	mVaryings.sort(compareVarying);
}

const char *Shader::getHLSL()
{
    return mHlsl;
}

void Shader::setSource(const char *shaderSrc)
{
	mSource = (char*) shaderSrc;
}

void Shader::setHLSL(const char *hlsl)
{
	mHlsl = (char*) hlsl;
}