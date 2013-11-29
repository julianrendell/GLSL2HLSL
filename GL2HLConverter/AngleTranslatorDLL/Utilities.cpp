#include "Utilities.h"

int VariableColumnCount(GLenum type)
{
    switch (type)
    {
      case GL_NONE:
        return 0;
      case GL_BOOL:
      case GL_FLOAT:
      case GL_INT:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_CUBE:
        return 1;
      case GL_BOOL_VEC2:
      case GL_FLOAT_VEC2:
      case GL_INT_VEC2:
      case GL_FLOAT_MAT2:
        return 2;
      case GL_INT_VEC3:
      case GL_FLOAT_VEC3:
      case GL_BOOL_VEC3:
      case GL_FLOAT_MAT3:
        return 3;
      case GL_BOOL_VEC4:
      case GL_FLOAT_VEC4:
      case GL_INT_VEC4:
      case GL_FLOAT_MAT4:
        return 4;
      default:
        return 0;
    }

    return 0;
}

int VariableRowCount(GLenum type)
{
    switch (type)
    {
      case GL_NONE:
        return 0;
      case GL_BOOL:
      case GL_FLOAT:
      case GL_INT:
      case GL_BOOL_VEC2:
      case GL_FLOAT_VEC2:
      case GL_INT_VEC2:
      case GL_INT_VEC3:
      case GL_FLOAT_VEC3:
      case GL_BOOL_VEC3:
      case GL_BOOL_VEC4:
      case GL_FLOAT_VEC4:
      case GL_INT_VEC4:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_CUBE:
        return 1;
      case GL_FLOAT_MAT2:
        return 2;
      case GL_FLOAT_MAT3:
        return 3;
      case GL_FLOAT_MAT4:
        return 4;
      default:
        return 0;
    }

    return 0;
}

// This method came from utilities.cpp
int AllocateFirstFreeBits(unsigned int *bits, unsigned int allocationSize, unsigned int bitsSize)
{
	if (allocationSize > bitsSize)
	{
		throw new std::runtime_error("Allocation size is greater than bits size");
	}

	unsigned int mask = std::numeric_limits<unsigned int>::max() >> (std::numeric_limits<unsigned int>::digits - allocationSize);

	for (unsigned int i = 0; i < bitsSize - allocationSize + 1; i++)
	{
		if ((*bits & mask) == 0)
		{
			*bits |= mask;
			return i;
		}

		mask <<= 1;
	}

	return -1;
}

std::string str(int i)
{
	int size = _scprintf("%d", i);
	char *buffer = new char[size+1];

	sprintf(buffer, "%d", i);
	buffer[size+1] = '\0';

	return buffer;
}

char* cstr(std::string str)
{
	char * cstr = new char [str.length()+1];
	std::strcpy(cstr, str.c_str());

	return cstr;
}

GLenum UniformComponentType(GLenum type)
{
    switch(type)
    {
      case GL_BOOL:
      case GL_BOOL_VEC2:
      case GL_BOOL_VEC3:
      case GL_BOOL_VEC4:
          return GL_BOOL;
      case GL_FLOAT:
      case GL_FLOAT_VEC2:
      case GL_FLOAT_VEC3:
      case GL_FLOAT_VEC4:
      case GL_FLOAT_MAT2:
      case GL_FLOAT_MAT3:
      case GL_FLOAT_MAT4:
          return GL_FLOAT;
      case GL_INT:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_CUBE:
      case GL_INT_VEC2:
      case GL_INT_VEC3:
      case GL_INT_VEC4:
          return GL_INT;
      default:
          throw new std::runtime_error("Unknown uniform component type");
    }

    return GL_NONE;
}

size_t UniformComponentSize(GLenum type)
{
    switch(type)
    {
      case GL_BOOL:  return sizeof(GLint);
      case GL_FLOAT: return sizeof(GLfloat);
      case GL_INT:   return sizeof(GLint);
	  default:       throw new std::runtime_error("Unknown uniform component size");
    }

    return 0;
}

size_t UniformInternalSize(GLenum type)
{
    // Expanded to 4-element vectors
    return UniformComponentSize(UniformComponentType(type)) * VariableRowCount(type) * 4;
}
