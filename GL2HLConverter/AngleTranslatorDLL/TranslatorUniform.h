#include <string>
#ifndef TRANSLATOR_UNIFORM_H_
#define TRANSLATOR_UNIFORM_H_

#include <vector>

#include "GLES2\gl2.h"

namespace sh
{

struct Uniform
{
    Uniform(GLenum type, GLenum precision, const char *name, int arraySize, int registerIndex);

    GLenum type;
    GLenum precision;
    std::string name;
    unsigned int arraySize;
    
    int registerIndex;
};

typedef std::vector<Uniform> ActiveUniforms;

}


#endif