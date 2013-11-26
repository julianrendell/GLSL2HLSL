#include <string>
#ifndef UNIFORM_H_
#define UNIFORM_H_

#include <vector>

#include "GLES2\gl2.h"

// Copied from Uniform.h
struct Uniform
{
	Uniform(GLenum type, GLenum precision, const std::string &name, unsigned int arraySize)
		: type(type), precision(precision), name(name), arraySize(arraySize)
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

typedef std::vector<Uniform*> UniformArray;


#endif