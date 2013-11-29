#include "GLESUniform.h"
#include "Utilities.h"

namespace gl
{

Uniform::Uniform(GLenum type, GLenum precision, const std::string &name, unsigned int arraySize)
    : type(type), precision(precision), name(name), arraySize(arraySize)
{
    int bytes = UniformInternalSize(type) * elementCount();
    data = new unsigned char[bytes];
    memset(data, 0, bytes);
    dirty = true;

    psRegisterIndex = -1;
    vsRegisterIndex = -1;
    registerCount = VariableRowCount(type) * elementCount();
}

Uniform::~Uniform()
{
    delete[] data;
}

bool Uniform::isArray() const
{
    return arraySize > 0;
}

unsigned int Uniform::elementCount() const
{
    return arraySize > 0 ? arraySize : 1;
}

}
